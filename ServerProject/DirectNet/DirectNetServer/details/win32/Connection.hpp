#pragma once

#if _DEBUG
#undef assert
#define assert( exp ) ( void )( ( exp ) || ( _my_assert( #exp, __FILE__, __LINE__ ), 0 ) )
#endif

namespace DreamNet_Win32
{
    static const unsigned long MTU = ( 1518 - 18 );         // 18 = EthernetII ��֡ͷβ��С
    static const unsigned long MSS = ( MTU - 40 - 8 );      // 40 = IP���ݰ�ͷ+TCP���ݰ�ͷ 8 = TCPPPPoE�İ�ͷβ��С
    static const unsigned long MAX_SBSIZE = ( MSS*4 );      // ��4��TCP������ȷ����ǰһ���������ݰ�����󳤶ȣ�����������ȵ����ݱ��ֶ���Ƭ
    static const unsigned long MAX_BUFFER_SIZE = 0xf000;    // �������ݵĴ�С��
    static const unsigned long MAX_BUFF_LIMITED = MAX_BUFFER_SIZE * 32; // ��󻺴����ݰ��Ĵ�С�����õ��㹻���������ڼ��˻����³������ݻ���ռ�ľ���

    // Define overlapped state
    enum { OLS_ACCEPT = -1, OLS_RECEIVE = -2, OLS_SEND = -3, OLS_CLOSE = -4 };

    // ��������Ľṹֻ�ᱻ�������������ã�
    // ͬʱ�ڲ���ʱ���������Ҳ���������ٽ�������Ϊ�������ݲ�����ȫ����ڹؼ�����
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
                throw exception( "�����׽���ʧ�ܣ�" );
            }

            assert( !( slot & 0xffff0000 ) );

            // ����dnid
            dnid = ( dnid_gen() << 16 ) | slot;
        }

        ~ConnectionData() 
        { 
            assert( !validate ); 

            // ��Ҫ��ֻ����һ���ط��� closesocket��Ҳ����˵�����ö�������ʱ��������Դ�ض��ᱻ�ͷ�
            closesocket( sock );

            LOG( "client released\r\n" );
        }

        //***************************
        SOCKET          sock;
        void            *userData;          // ���������ں��������󶨹�ϵ
        volatile LONG   validate;           // ��ʶ��������ȷ���������Ƿ���Ҫ�رգ�ע�⣬�ñ�־������ʾ�����Ѿ�ʧЧ����
        BOOL            compress;           // �Ƿ��ڷ���ʱ��������ݽ���ѹ������
        DNID            dnid;               // ����ID����ƹ���Ϊ��tickVerifier | slot��
        sockaddr_in     addr;               // �����ӵ�ip��Ϣ
        DWORD           pushFailureTimes;   // �������ʧ�ܵĴ��������ڿ���һЩ��Ч�ģ�����������ӣ�
        //---------------------------
    };
}

#include "Buffer.hpp"
#include "Sender.hpp"
#include "Receiver.hpp"

namespace DreamNet_Win32
{
    // ���ڸ����ӹ��������й���Ķ�������
    // ������������Connection֮�ϣ������������������ӹ�����ȷ��
    struct Connection : Receiver
    {
    public:
        //***************************
        BOOL Push( const void *data, size_t size )
        {
            // ����ע�⣬���PushPackage����-1ֻ��ʾ���ʧ�ܣ�
            // ������-2���ʾ���ʧ���ҳ���һ����������Ҫ��������ֱ�ӶϿ���
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

            // ��sender���ö���Ϊ��������Ϊsender��������������޷��Լ����Լ��������õ�
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
        // �ͷ����ӣ�
        // withoutLinger ѡ��֧�֡�����ǿ�ƹرգ��Ƿ�������ʱ������Ҫ����TCPЭ��ջ�е�TIME_WAIT��
        // TIME_WAIT ָ���ӹرպ󡪡��رշ��������ȴ�����Ϊ��*MSL*2����ʱ�䣬��Զ����ȷ��ȡ�����ӹر���Ϣ��
        void CloseConnection( BOOL withoutLinger = FALSE )
        {
            // ��δ���ܴ���ܻ��������
            // ���Ǵ󲿷ֲ����Ƿ��ڴ���������Բ�̫������Դ��Ч������

            // �ݴ�
            if ( this == NULL )
                return;

            if ( !connectionData )
                return;

            // ����ԭ�Ӳ������жϣ�����CloseSocket���µĴ���ֻ���ܱ�ִ��һ��
            if ( !InterlockedExchange( &connectionData->validate, FALSE ) ) 
                return;

            SOCKET sock = connectionData->sock;

            // withoutLinger�������� graceful shutdown
            if ( withoutLinger || !sender )
            {
                // ֱ�ӽ�������ǿ�ƹرգ�ע�⣺ֻ�ǹر�(shutdown)����û���ͷ���Դ��
                BOOL linger = FALSE; 
                setsockopt( sock, SOL_SOCKET, SO_DONTLINGER, ( LPCSTR )&linger, sizeof( linger ) );
                shutdown( sock, SD_BOTH );
            }
            else
            {
                // graceful shutdown
                // ���ŶϿ���״���£���Ҫ�����ܵĽ�������������ȫ���ͣ�ͬʱ����Ҫ�������ܿͻ��˹���������
                // ��ֹ�������ݣ����������ݣ�
                // ������ʱ����յ��ͻ������ݣ�onrecv�᷵��0����slot_conn�ᱻ�ͷţ�
                shutdown( sock, SD_RECEIVE );

                // ��Ҫ�����������͹��̣��ú��������ݷ��͵��ͻ���
                // sender���������ͺ���Լ�����һ���Լ������ü����������ᱻֱ������
                // �����������ʧ�ܣ�WsaSendʧ�ܻ�û��������Ҫ���ͣ���
                // ���ڲ������validate�����жϣ���������shutdown with SD_BOTH
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
        // �ص�����ʧ��ʱ�Ļص��������ǰ������ص��������Ѿ���ɣ���ö�����Ա������ˣ�
        // ��������
        void OnOverlapFailure( OVERLAPPED *overlap, int lastError ) {}
        //---------------------------

    public:
        //***************************
        Connection( DWORD slot ) :
            Receiver( slot ),
            sender( new Sender( connectionData ) )
        {
            // *************************************************************
            // ����Ҫ˵����
            // �������Ӷ�����Чʱ��sender::connectionData�ض���Ч
            // �������Ӷ�����Чʱʱ��sender::connectionDataҲ������Ч
            // connectionData��Чʱsender&connection�ض���Ч��
            // *************************************************************
        }

        ~Connection()
        {
            CloseConnection( TRUE );
        }
        //---------------------------

    private:
        // ��������һ�������Ķ��������������ں�receiverû�й�ϵ
        // Ҳ����˵�������ӱ��رգ����٣�ʱ��sender��Ȼ�����������������������������Լ��Ĵ��ʱ��
        // ��Ҫ���������Receiver&Sender�У���ͨ��const�޶������ö������ڶ������ͬʱ����Ч������������ڼ䱣����Ч��
        const Sender::reference_ptr sender;
    };
}
