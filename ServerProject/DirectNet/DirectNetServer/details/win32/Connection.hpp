#pragma once

#if _DEBUG
#undef assert
#define assert( exp ) ( void )( ( exp ) || ( _my_assert( #exp, __FILE__, __LINE__ ), 0 ) )
#endif

namespace DreamNet_Win32
{
    static const unsigned long MTU = ( 1518 - 18 );         // 18 = EthernetII 的帧头尾大小
    static const unsigned long MSS = ( MTU - 40 - 8 );      // 40 = IP数据包头+TCP数据包头 8 = TCPPPPoE的包头尾大小
    static const unsigned long MAX_SBSIZE = ( MSS*4 );      // 以4个TCP最大段来确定当前一个网络数据包的最大长度！超过这个长度的数据被手动切片
    static const unsigned long MAX_BUFFER_SIZE = 0xf000;    // 单个数据的大小包
    static const unsigned long MAX_BUFF_LIMITED = MAX_BUFFER_SIZE * 32; // 最大缓存数据包的大小！设置得足够大来避免在极端环境下出现数据缓存空间耗尽！

    // Define overlapped state
    enum { OLS_ACCEPT = -1, OLS_RECEIVE = -2, OLS_SEND = -3, OLS_CLOSE = -4 };

    // 这个单纯的结构只会被其他对象所引用！
    // 同时在操作时，这个对象也不被放入临界区，因为它的数据并不完全归结于关键数据
    struct ConnectionData
    {
        typedef boost::shared_ptr< ConnectionData > reference_ptr;

        ConnectionData( DWORD slot )
        {
            memset( this, 0, sizeof( ConnectionData ) );

            sock = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
            validate = ( sock != INVALID_SOCKET );

            if ( !validate )
            {
                DWORD le = WSAGetLastError();
                throw exception( "创建套接字失败！" );
            }

            assert( !( slot & 0xffff0000 ) );

            // 计算dnid
            dnid = ( dnid_gen() << 16 ) | slot;
        }

        ~ConnectionData() 
        { 
            assert( !validate ); 

            // 重要：只有这一个地方会 closesocket，也就是说，当该对象被销毁时，连接资源必定会被释放
            closesocket( sock );

            LOG( "client released\r\n" );
        }

        //***************************
        SOCKET          sock;
        void            *userData;          // 该数据用于和外界产生绑定关系
        volatile LONG   validate;           // 标识符，用于确定该连接是否需要关闭（注意，该标志并不表示连接已经失效！）
        BOOL            compress;           // 是否在发送时将打包数据进行压缩处理
        DNID            dnid;               // 连接ID，设计规则为（tickVerifier | slot）
        sockaddr_in     addr;               // 该连接的ip信息
        DWORD           pushFailureTimes;   // 打包数据失败的次数，用于控制一些无效的，带宽不足的连接！
        //---------------------------
    };
}

#include "Buffer.hpp"
#include "Sender.hpp"
#include "Receiver.hpp"

namespace DreamNet_Win32
{
    // 用于给连接管理器进行管理的对象类型
    // 接收器依附于Connection之上，他的生存周期由连接管理器确定
    struct Connection : Receiver
    {
    public:
        //***************************
        BOOL Push( const void *data, size_t size )
        {
            // 这里注意，如果PushPackage返回-1只表示打包失败，
            // 而返回-2则表示打包失败且超过一定次数，需要将该连接直接断开！
            int result = sender->PushPackage( data, size );

            if ( result == 0 )
                return TRUE;

            if ( result == -2 )
                CloseConnection( TRUE );

            return FALSE;
        }

        BOOL Send()
        {
            assert( connectionData );
            if ( !connectionData->validate )
                return FALSE;

            // 带sender引用对象为参数，因为sender在正常情况下是无法自己对自己做出引用的
            return sender->ActivateSend< UGE::CriticalLock::Barrier4ReadWrite >( sender );
        }

        void* SetUserData( void *ptr ) 
        { 
            assert( connectionData );
            std::swap( connectionData->userData, ptr ); 
            return ptr; 
        }

        void* GetUserData() 
        { 
            assert( connectionData );
            return connectionData->userData; 
        }

        BOOL VerifyDnid( DNID &id64 )
        {
            if ( !connectionData->validate )
                return FALSE;

            return ( id64 == connectionData->dnid );
        }

        BOOL CheckHalfDnid( ULONG_PTR id32 )
        {
            return ( id32 == static_cast< DWORD >( connectionData->dnid ) );
        }

        void GetConnectionInfo( iDirectNetServer::CONNECTIONINFO &cinfo )
        {
            assert( connectionData );
            cinfo.addr = connectionData->addr.sin_addr.s_addr;
	        cinfo.port = connectionData->addr.sin_port;
        }
        //---------------------------

    public:
        // 释放连接！
        // withoutLinger 选项支持——在强制关闭（非法）连接时，不需要考虑TCP协议栈中的TIME_WAIT！
        // TIME_WAIT 指连接关闭后——关闭方会主动等待长度为（*MSL*2）的时间，让远端正确获取到连接关闭消息！
        void CloseConnection( BOOL withoutLinger = FALSE )
        {
            // 这段代码很大可能会存在重入
            // 但是大部分操作是非内存操作，所以不太担心资源无效的问题

            // 容错！
            if ( this == NULL )
                return;

            if ( !connectionData )
                return;

            // 基于原子操作的判断，所以CloseSocket以下的代码只可能被执行一次
            if ( !InterlockedExchange( &connectionData->validate, FALSE ) ) 
                return;

            SOCKET sock = connectionData->sock;

            // withoutLinger，不属于 graceful shutdown
            if ( withoutLinger || !sender )
            {
                // 直接将该连接强制关闭！注意：只是关闭(shutdown)，但没有释放资源！
                BOOL linger = FALSE; 
                setsockopt( sock, SOL_SOCKET, SO_DONTLINGER, ( LPCSTR )&linger, sizeof( linger ) );
                shutdown( sock, SD_BOTH );
            }
            else
            {
                // graceful shutdown
                // 优雅断开的状况下，需要尽可能的将后续的数据完全发送，同时不需要继续接受客户端过来的数据
                // 禁止接收数据！允许发送数据！
                // 如果这个时候接收到客户端数据，onrecv会返回0，则slot_conn会被释放！
                shutdown( sock, SD_RECEIVE );

                // 需要尝试启动发送过程，让后续的数据发送到客户端
                // sender在启动发送后会自己保持一个自己的引用计数，而不会被直接销毁
                // 如果启动发送失败（WsaSend失败或没有数据需要发送），
                // 则内部会根据validate进行判断，进而调用shutdown with SD_BOTH
                sender->ActivateSend< UGE::CriticalLock::Barrier4ReadWrite >( sender );
            }
        }

        BOOL QueryAccept( SOCKET listener )
        {
            if ( listener == INVALID_SOCKET )
                return FALSE;

            assert( recving.capacity >= 1 );

            key = static_cast< DWORD >( connectionData->dnid );

            if ( !AcceptEx( listener, connectionData->sock, recving.buf, 0,
                sizeof( sockaddr_in ) + 16, sizeof( sockaddr_in ) + 16, 
                &current_transferred, &olap ) )
            {
                if ( WSA_IO_PENDING != WSAGetLastError() )
                {
                    CloseConnection( TRUE );
                    return FALSE;
                }
            }

            return TRUE;
        }

    private:
        //***************************
        // 重叠操作失败时的回调！如果当前对象的重叠操作都已经完成，则该对象可以被销毁了！
        // 可能重入
        void OnOverlapFailure( OVERLAPPED *overlap, int lastError ) {}
        //---------------------------

    public:
        //***************************
        Connection( DWORD slot ) :
            Receiver( slot ),
            sender( new Sender( connectionData ) )
        {
            // *************************************************************
            // （重要说明）
            // 当该连接对象有效时，sender::connectionData必定有效
            // 当该连接对象无效时时，sender::connectionData也可能有效
            // connectionData无效时sender&connection必定无效！
            // *************************************************************
        }

        ~Connection()
        {
            CloseConnection( TRUE );
        }
        //---------------------------

    private:
        // 发送器是一个独立的对象，他的生存周期和receiver没有关系
        // 也就是说，当连接被关闭（销毁）时，sender仍然可能正常进行其他操作，并控制自己的存活时间
        // 重要：这里，包括Receiver&Sender中，都通过const限定了引用对象都是在对象构造的同时就有效，对象的生命期间保持有效！
        const Sender::reference_ptr sender;
    };
}
