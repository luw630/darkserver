////////////////////////////////////////////////////////////////////////
//阻塞方式的版本

#include "stdafx.h"

#include <mmsystem.h>
#include <map>

#include "../../pub/thread.h"
#include "boost/bind.hpp"
#include "boost/shared_ptr.hpp"
#include "../lzw.hpp"

#pragma comment ( lib, "winmm.lib" )

#ifdef _DEBUG
void trace( LPCSTR szFormat, ... );
#define LOG OutputDebugString
#else
#define LOG( x )
#endif

// the DirectNetProtocol header
struct DNPHDR {
    UINT16 seqnum;
    UINT16 paclen;
};

// the header that is passed one-way from client to server
struct CRYPTION {
    UINT32 crc32;
    DNPHDR dnphdr;
    UINT64 signature;
};

#define OP_READ_HEAD        0
#define OP_READ_BODY        1

static const int MAX_BUFFER_SIZE = 0x100000; // 为发送端数据准备1MB的发送缓存！

void trace( LPCSTR szFormat, ... )
{
    char timebuf[128];
    char buffer[1025];

    SYSTEMTIME s;
    GetLocalTime(&s);
    sprintf(timebuf, "[%d-%d-%d %d:%d:%d]", s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond);

    try
    {
        va_list header;
        va_start(header, szFormat);
        _vsnprintf(buffer, 1024, szFormat, header);
        va_end(header);

        buffer[1024] = 0;

        std::ofstream stream("client_dinfo.txt", std::ios_base::app);
        stream << timebuf << buffer;
    }
    catch (...)
    {
        std::ofstream stream("client_dinfo.txt", std::ios_base::app);
        stream << "err in try \r\n";
    }
}

void ErrorHandle( LPCSTR info, BOOL le = FALSE, int eid = -1 )
{
    if (le)
    {
        trace("%s - [%d]\r\n", info, WSAGetLastError());
    }
    else if (eid != -1)
    {
        trace("%s - [%d]\r\n", info, eid);
    }
    else
    {
        trace("%s\r\n", info);
    }
}

struct Encryptor
{
    Encryptor( iDirectNetCryption *encryptor ) : encryptor( encryptor ) {}
    operator bool () { return encryptor != NULL; }
    UINT32 crc32( void *data, DWORD size ) { return encryptor->CRC32_compute( data, size ); }
    void encrypt( void *data, DWORD size ) { encryptor->DES_encrypt( data, size ); }
    iDirectNetCryption *encryptor;
};

// 可动态进行伸缩适应的缓存对象类型
struct Buffer : public WSABUF
{
    Buffer( size_t init_size ) 
    { 
        capacity = ( init_size >> 12 ) + 1;
        buf = raw_buffer = new char[ capacity << 12 ];
        last_resize_time = timeGetTime();

        size = 0;
        len = 0; 
    }

    Buffer() 
    { 
        capacity = 0;
        size = 0;
        raw_buffer = NULL;
        last_resize_time = 0;

        buf = 0; 
        len = 0; 
    }

    Buffer( const Buffer &other ) 
    { 
        memcpy( this, &other, sizeof( Buffer ) );
        memset( const_cast< Buffer* >( &other ), 0, sizeof( Buffer ) );
    }

    ~Buffer() 
    { 
        if ( raw_buffer != NULL ) 
            delete raw_buffer; 

        capacity = size = 0;
        raw_buffer = NULL;
        last_resize_time = 0;

        buf = 0, len = 0; 
    }

    Buffer& operator = ( const Buffer &other ) 
    { 
        memcpy( this, &other, sizeof( Buffer ) );
        memset( const_cast< Buffer* >( &other ), 0, sizeof( Buffer ) );
        return *this;
    }

    // 这个函数在发送和接收中都会使用
    void inflate( size_t new_size, bool discard_raw_data ) 
    {
        LOG( "capacity inflated\r\n" );

        if ( discard_raw_data )
        {
            // 每次按需求容量的 1.5 倍进行增长！
            capacity = ( new_size >> 12 ) + 1;
            capacity += ( capacity >> 1 );

            delete raw_buffer;
            raw_buffer = new char[ capacity << 12 ];
        }
        else
        {
            size_t new_capacity = ( new_size >> 12 ) + 1;
            new_capacity += ( new_capacity >> 1 );
            char *new_buffer = new char[ new_capacity << 12 ];

            assert( size <= ( capacity << 12 ) );

            // 只拷贝有效的部分
            memcpy( new_buffer, raw_buffer, size );
            delete raw_buffer;

            raw_buffer = new_buffer;
            capacity = new_capacity;
        }

        last_resize_time = timeGetTime();
        buf = raw_buffer;
    }

    void reset( size_t new_size ) 
    { 
        assert( ( size >> 12 ) < capacity );

        // 容量扩张！
        if ( ( new_size >> 12 ) >= capacity )
        {
            inflate( new_size, true );
        }
        // 跟据需求缩减！
        else if ( ( capacity > 1 ) && ( ( new_size >> 12 ) < ( capacity >> 1 ) ) )
        {
            // 如果十分钟内，buffer的有效数据使用量不能达到当前capacity的1/2，则直接将当前buffer重置！
            if ( abs( ( int )timeGetTime() - ( int )last_resize_time ) > 600000 )
            {
                LOG( "capacity deflate reset\r\n" );

                // 缩减为有效数据的4k取整
                capacity = ( new_size >> 12 ) + 1;
                last_resize_time = timeGetTime();

                delete raw_buffer;
                raw_buffer = new char[ capacity << 12 ];
            }
        }

        buf = raw_buffer;
        len = static_cast< u_long >( size = new_size );
    }

    template < typename Encryptor >
        int push( const void *data, size_t data_size, DWORD sequence, Encryptor encryptor )
    {
        // 单个数据包大于 MAX_BUFFER_SIZE
        if ( data_size > MAX_BUFFER_SIZE )
        {
            // 这个属于错误，不应该进来这么大的数据包
            ErrorHandle( "请求发送的单个数据包过大！" );
            LOG( "请求发送的单个数据包过大！\r\n" );
            return -1;
        }

        // 设定，一个打包数据的最大长度为 MAX_BUFFER_SIZE * 4
        size_t new_size = ( size + data_size + sizeof( CRYPTION ) );
        if ( new_size > ( MAX_BUFFER_SIZE * 8 ) )
        {
            // ErrorHandle( "打包空间超过最大值！" );
            LOG( "打包空间超过最大值！\r\n" );
            // 这个就说明是异常，需要外部进行特殊处理
            // SpecialLog( raw_buffer, size, data, data_size, sequence );
            return -2;
        }

        if ( capacity <= ( new_size >> 12 ) )
            inflate( new_size, false );

        reinterpret_cast< CRYPTION* >( &raw_buffer[ size ] )->dnphdr.seqnum = ( WORD )sequence;
        reinterpret_cast< CRYPTION* >( &raw_buffer[ size ] )->dnphdr.paclen = ( UINT16 )data_size;

        UINT32 u32 = GetTickCount();
        UINT64 u64; QueryPerformanceCounter( ( LARGE_INTEGER* )&u64 );
        reinterpret_cast< CRYPTION* >( &raw_buffer[ size ] )->signature = ( ( u64 << 32 ) | u32 );

        if ( encryptor )
        {
            reinterpret_cast< CRYPTION* >( &raw_buffer[ size ] )->crc32 = 
                encryptor.crc32( ( char* )&raw_buffer[ size ] + sizeof( ((CRYPTION*)0)->crc32 ), 
                sizeof( CRYPTION ) - sizeof( ((CRYPTION*)0)->crc32 ) );

            encryptor.encrypt( &raw_buffer[ size ], sizeof( CRYPTION ) );
        }

        size += sizeof( CRYPTION );

        memcpy( &raw_buffer[ size ], data, data_size );
        size += static_cast< u_long >( data_size );

        len = static_cast< u_long >( size );
        assert( raw_buffer == buf );

        return 0;
    }

    // 增加一个逻辑，如果一定时间后，buffer的有效数据使用量一直不能达到当前capacity的1/2，
    // 则这个buffer在发送完成后做reset的同时，删除所有数据量，重新分配一个不低于4k的临时缓存！
    char *raw_buffer;
    size_t size;            // 有效数据量（对于接收来说，size是总需要接收的数据量大小，对于发送来说，size是总需要发送的数据量大小）
    size_t capacity;        // 有效空间！
    DWORD last_resize_time;
};

// 本对象在任何时候都至少有一个buffer有效存在
struct TwinsBuffer
{
    TwinsBuffer() : headerIndex( false ), secondValidate( true ) {}

    void push( Buffer &buf ) 
    { 
        assert( !secondValidate ); 

        twins[ !headerIndex ] = buf; 

        secondValidate = true; 
    } 

    void insert_front( Buffer &buf )
    {
        assert( !secondValidate ); 

        twins[ !headerIndex ] = buf; 

        secondValidate = true; 
        headerIndex = !headerIndex; 
    }

    void pop() 
    { 
        assert( secondValidate ); 

        twins[ headerIndex ].~Buffer(); 

        secondValidate = false; 
        headerIndex = !headerIndex; 
    } 

    size_t size() 
    { 
        return secondValidate ? 2 : 1;
    } 

    Buffer& front() 
    { 
        return twins[ headerIndex ]; 
    } 

    bool empty() { return false; }

    Buffer twins[2];
    bool  headerIndex;      // 当前处于第一位的buffer位置！
    bool  secondValidate;   // 当前处于第二位的buffer是否有效！
};

class cDirectNetClient : 
    public iDirectNetClient 
{
    class _SendThread
    {
    public:
        _SendThread( cDirectNetClient *handler, UGE::ThreadWaiter &threadWaiter ) : handler( handler ),threadWaiter(threadWaiter){}

        int operator () (void)
        {
            if( handler->m_Socket == INVALID_SOCKET )
            {
                ErrorHandle("SendThread中套接字为 INVALID_SOCKET");
                return -1;
            }

            // 正在发送中的数据，当这个数据被发送完毕后，就直接push到队列中，作为后续的后备缓存！
            Buffer sending;

            std::pair< LPVOID, size_t > temp;

            while( !threadWaiter.QuitFlag() )
            {
                BOOL haveData = false;

                {
                    UGE::CriticalLock::Barrier4ReadWrite barrier( handler->qLock );

                    assert ( handler->sendQ.size() == 2 );

                    if ( ( handler->sendQ.front().len != 0 ) && ( handler->sendQ.front().buf != NULL ) )
                    {
                        haveData = true;
                        sending = handler->sendQ.front();
                        handler->sendQ.pop();
                    }
                }

                if ( haveData )
                {
                    int len = send( handler->m_Socket, ( char* )sending.buf, ( int )sending.len, 0 );

                    if ( len == SOCKET_ERROR || len == 0 )
                        break;

                    assert( len == sending.len );

                    {
                        UGE::CriticalLock::Barrier4ReadWrite barrier( handler->qLock );

                        assert ( handler->sendQ.size() == 1 );

                        sending.reset( 0 );
                        handler->sendQ.push( sending );
                    }

                    handler->sendSize += ( DWORD )len; //sending.len;
                }
                //if ( handler->m_SendQueue.size() > 0 )
                //{
                //    handler->m_SendQueue.mtsafe_get_and_pop( temp );

                //    int len = send( handler->m_Socket, ( char* )temp.first, ( int )temp.second, 0 );

                //    delete temp.first;

                //    if ( len == SOCKET_ERROR || len == 0 )
                //        break;

                //    assert( len == temp.second );

                //    handler->sendSize += ( DWORD )temp.second;
                //}
                else
                {
                    Sleep( 1 );
                }
            }

            //while(  handler->m_SendQueue.size() > 0 )
            //{
            //    handler->m_SendQueue.mtsafe_get_and_pop( temp );
            //    delete temp.first;
            //}

            if ( handler->m_Socket != INVALID_SOCKET )
            {
                shutdown( handler->m_Socket, SD_BOTH );
                closesocket( handler->m_Socket );
                handler->m_Socket = INVALID_SOCKET;
            }

            return 0;
        }

    public:
        cDirectNetClient *handler;   
        UGE::ThreadWaiter &threadWaiter;   
    };

    class _MainThread
    {
    public:
        _MainThread( cDirectNetClient *handler ) : handler( handler ) {}

        int operator () ( void )
        {
            handler->m_Socket = socket( AF_INET, SOCK_STREAM, 0 );

            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = handler->m_dwIP;
            addr.sin_port = htons( handler->m_wPort );

            bool success = ( connect( handler->m_Socket, ( sockaddr* )&addr, sizeof( addr ) ) != SOCKET_ERROR );

            if ( handler->m_pDirectNetClientEventHandler )
                handler->m_pDirectNetClientEventHandler->OnConnectComplete( success ? DNR_S_OK : DNR_E_FAIL );

            if ( !success )
            {
                closesocket( handler->m_Socket );
                handler->m_Socket = INVALID_SOCKET;
                ErrorHandle( "与服务器连接失败!" );
                return -1;
            }

            UGE::ThreadWaiter threadWaiter;
            threadWaiter.BeginThread( _SendThread( handler, threadWaiter ) );

            // 压缩后的数据包不会超过 0xffff 不过为了容错，这里还是准备 0x20000 这么大的空间吧
            // 注意，服务器上最大能缓存的编码包是 0xffff * 8 所以，这里的最大解码空间为 0x80000
            static const size_t RECV_BUFLEN = 0x20000;
            static const size_t DECODE_BUFLEN = 0x80000;

            int totalBytes      = sizeof( DNPHDR );
            int cbTransferred   = 0;
            int opCode          = OP_READ_HEAD;
            DWORD dwInSequence  = 0;
            LPSTR buffer        = new char[ RECV_BUFLEN ];
            LPSTR decoderBuffer	= new char[ DECODE_BUFLEN ];

            BOOL            isEncoded = false;
            lzw::lzwDecoder decoder;

            while ( !handler->threadWaiter.QuitFlag() )
            {
                int result = recv( handler->m_Socket, &buffer[ cbTransferred ], totalBytes - cbTransferred, 0 );

                if ( ( result == SOCKET_ERROR ) || ( result == 0 ) )
                {
                    if ( result == SOCKET_ERROR )
                        ErrorHandle( "receiving package failure !!!" );

                    break;
                }

                cbTransferred += result;

                if ( cbTransferred < totalBytes )
                    continue;

                if ( opCode == OP_READ_HEAD )
                {
                    #define GetDH ( ( DNPHDR* )( char* )buffer )

                    // 如果是压缩包:seqnum存大小,paclen表示是否为压缩以及是否需要重设字典（0压缩，-1压缩并重设字典，-2存在扩展数据）
                    if ( ( GetDH->paclen == 0 ) || ( GetDH->paclen >= 0xfff0 ) )
                    {
                        if ( GetDH->paclen == 0xffff )
                            decoder.reset();

                        totalBytes = GetDH->seqnum;
                        isEncoded  = true;
                    }
                    else
                    {
                        if ( GetDH->seqnum != ( WORD )dwInSequence )
                        {
                            dwInSequence = GetDH->seqnum;
                            ErrorHandle( "detected package lost !!!" );
                        }

                        totalBytes = GetDH->paclen;
                    }

                    cbTransferred   = 0;
                    opCode          = OP_READ_BODY;

                    //sprintf( temp, "%04x[%3d] = ", GetDH->seqnum, GetDH->paclen );
                    //OutputDebugString( temp );

                    #undef GetDH
                }
                else
                {
                    //sprintf( temp, "%02x%02x%02x%02x%02x%02x%02x%02x[%d]\r\n", 
                    //    (BYTE)buffer[0], (BYTE)buffer[1], (BYTE)buffer[2], (BYTE)buffer[3], 
                    //    (BYTE)buffer[4], (BYTE)buffer[5], (BYTE)buffer[6], (BYTE)buffer[7], totalBytes );
                    //OutputDebugString( temp );

                    if ( isEncoded )
                    {
                        isEncoded = false;
#ifndef _FBYTE_SIZE_
                        // 这个版本就是为了和现有服务器进行兼容，但是需要对解码数据大小进行特殊处理。。
                        UINT16 *encoderBeforeSize = ( UINT16 * )buffer; 
                        size_t decoderSize        = *encoderBeforeSize;
                        decoder.SetEncoderData( buffer + 2, totalBytes - 2 );
                        size_t size = decoder.lzw_decode( decoderBuffer, DECODE_BUFLEN );

                        if ( size != decoderSize )
                        {
                            // 这里主要是因为服务器上允许的压缩数据大小是 0x80000，但是传送的数据只有2个字节长，导致原始数据大小被截断。。。
                            if ( ( size & 0xffff ) != decoderSize )
                            {
                                ErrorHandle( "package decode failure!!!" );
                                break;
                            }
                            decoderSize = size;
                        }
#else
                        // 这个版本是为了扩展压缩前的数据大小，无法和现有的服务器进行兼容。
                        UINT32 *encoderBeforeSize = ( UINT32 * )buffer; 
                        size_t decoderSize        = *encoderBeforeSize;
                        decoder.SetEncoderData( buffer + 4, totalBytes - 4 );
                        size_t size = decoder.lzw_decode( decoderBuffer, DECODE_BUFLEN );

                        if ( size != decoderSize )
                        {
                            ErrorHandle( "package decode failure!!!" );
                            break;
                        }
#endif			

                        for ( size = 0; size < decoderSize; )
                        {
                            #define GetDH ( ( DNPHDR* )( decoderBuffer + size ) )

                            if ( GetDH->seqnum != ( WORD )dwInSequence )
                            {
                                dwInSequence = GetDH->seqnum;
                                ErrorHandle( "detected package lost !!!" );
                            }

                            if ( handler->m_pDirectNetClientEventHandler )
                                handler->m_pDirectNetClientEventHandler->OnReceivePacket( 
                                    decoderBuffer + size + sizeof( DNPHDR ), GetDH->paclen, ++dwInSequence );

                            size += ( sizeof( DNPHDR ) + GetDH->paclen );

                            #undef GetDH
                        }
                    }
                    else
                    {
                        if ( handler->m_pDirectNetClientEventHandler )
                            handler->m_pDirectNetClientEventHandler->OnReceivePacket( buffer, totalBytes, ++dwInSequence );
                    }

                    handler->recvSize += ( totalBytes + sizeof( DNPHDR ) );

                    cbTransferred   = 0;
                    totalBytes      = sizeof( DNPHDR );
                    opCode          = OP_READ_HEAD;
                }
            }

            if ( handler->m_Socket != INVALID_SOCKET )
            {
                shutdown( handler->m_Socket, SD_BOTH );
                closesocket( handler->m_Socket );
                handler->m_Socket = INVALID_SOCKET;
            }

            if ( handler->m_pDirectNetClientEventHandler )
                handler->m_pDirectNetClientEventHandler->OnTerminateSession();

            delete buffer;
			delete decoderBuffer;
	
            return 1;
        }

        cDirectNetClient *handler;
    };

public:
    DNRESULT Initialize( iDirectNetCryption *pDirectCryption, iDirectNetClientEventHandler *pDirectEventHandler )
    {
        if (m_pDirectNetClientEventHandler != NULL)
        {
            ErrorHandle("本对象已经初始化过了！");
            return DNR_E_FAIL;
        }

        if (pDirectEventHandler == NULL)
        {
            ErrorHandle("错误的事件响应句柄！");
            return DNR_E_FAIL;
        }

        m_pDirectNetCryption = pDirectCryption;
        m_pDirectNetClientEventHandler = pDirectEventHandler;

        return DNR_S_OK;
    }

    DNRESULT Connect( LPCSTR szServerIpAddress, DNPORT dnPort )
    {
        if ( m_pDirectNetClientEventHandler == NULL )
        {
            ErrorHandle( "本对象还没有初始化！" );
            return DNR_E_NOT_INITIALIZED;
        }

        if ( ( m_Socket != INVALID_SOCKET ) || ( threadWaiter.LatestThread() != INVALID_HANDLE_VALUE ) )
        {
            ErrorHandle("本对象已经处于连接状态中！");
            return DNR_E_CONNECT_AGAIN;
        }

        m_dwIP = inet_addr( szServerIpAddress );
        m_wPort = dnPort;

        threadWaiter.BeginThread( _MainThread( this ) );

        return DNR_S_PENDING;
    }

    DNRESULT Send( PVOID pvBuffer, DWORD dwSize )
    {
        if ( dwSize == 0xf0f0f0f0 )
        {
            LPDWORD pParameter = ( LPDWORD )pvBuffer;
            switch ( pParameter[0] )
            {
            case 1:
                // m_bUseAutoPackage = (pParameter[1] == 1);
                break;

            case 2:
                pParameter[0] = recvSize;
                pParameter[1] = sendSize;
                break;

            default:
                return DNR_E_FAIL;
            }
            return DNR_S_OK;
        }

        if ( (pvBuffer == NULL) || (dwSize + sizeof(CRYPTION))  > 0xf000 || (dwSize == 0) )
        {
            ErrorHandle("无效的得发送信息!");
            return DNR_E_FAIL;
        }

        int result = 0;
__retry:
        {
            UGE::CriticalLock::Barrier4ReadWrite barrier( qLock );

            // 始终是直接往第一个buffer里边写数据！
            assert( ( sendQ.size() != 0 ) && ( sendQ.size() <= 2 ) );
            result = sendQ.front().push( pvBuffer, dwSize, m_dwOutSequence, Encryptor( m_pDirectNetCryption ) );

            // 只有在成功时，才将包序号增加！
            if ( result == 0 ) m_dwOutSequence ++;
        }

        // 如果发送缓存已满，并且有延迟打包标记，则休眠！
        if ( ( result == -2 ) && pushDelay )
        {
            Sleep( 1 );

            // 如果网络都挂了，则剩余的数据也发不出去，要是想死循环的话，就retry吧。。。
            if ( m_Socket != INVALID_SOCKET ) 
                goto __retry;
        }

        return ( ( result == 0 ) ? DNR_S_PENDING : DNR_E_FAIL );
    }

    DNRESULT Disconnect()
    {
        if ( m_Socket != INVALID_SOCKET )
        {
            shutdown( m_Socket, SD_BOTH );
            closesocket( m_Socket );
            m_Socket = INVALID_SOCKET;
        }

        threadWaiter.Quit();

        return DNR_S_OK;
    }

    DNRESULT Close()
    {
        m_wPort = 0;
        m_dwIP = 0;

        recvSize = sendSize = 0;

        m_dwOutSequence = 0;

        m_pDirectNetCryption = NULL;
        m_pDirectNetClientEventHandler = NULL;

        Disconnect();

        return DNR_S_OK;
    }

    cDirectNetClient()
    {
        m_Socket = INVALID_SOCKET;
        m_wPort = 0;
        m_dwIP = 0;
        m_pDirectNetClientEventHandler = NULL;
        m_pDirectNetCryption = NULL;
        m_dwOutSequence = 0;

        pushDelay = 1;
        recvSize = sendSize = 0;

        WSADATA wsad;
        if (WSAStartup( MAKEWORD(2, 2), &wsad ) != 0)
        {
            ErrorHandle("当前系统并不支持WinSock2.2，相应的网络操作都无法完成！", TRUE);
        }
    }

    ~cDirectNetClient()
    {
        Close();

        WSACleanup();
    }

public:
    UGE::ThreadWaiter threadWaiter;

    SOCKET  m_Socket;
    WORD    m_wPort;
    DWORD   m_dwIP;

    DWORD   recvSize, sendSize;
    DWORD   m_dwOutSequence;

    BOOL    pushDelay;   // 当因发送缓存已满打包失败时，等待后再次尝试，直到发送成功或连接断开！

    iDirectNetClientEventHandler *m_pDirectNetClientEventHandler;
    iDirectNetCryption *m_pDirectNetCryption;

    // UGE::mtQueue< std::pair< LPVOID, size_t > > m_SendQueue;
private:
    TwinsBuffer         sendQ;
    UGE::CriticalLock   qLock;
};

void iDirectNetClient::Release()
{
    delete this;
}

iDirectNetClient* iDirectNetClient::CreateInstance()
{
    return new cDirectNetClient;
}
