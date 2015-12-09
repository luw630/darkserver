#pragma once

namespace DreamNet_Win32
{
    // 数据接受器，只存在单独的消息处理线，只有接受器会引发OnDisconnect
    // 重要说明：Receiver相关的线程！
    // A）[事件工作线程] -> [OnEstablished] -> [ActivateReceive]
    // B）[事件工作线程] -> [OnReceiveComplete] -> [ActivateReceive]
    // C）[事件工作线程] -> [OnReceiveComplete] -> [OnDisconnect]
    // D）[模块外部线程] -> [OnClose] -> [OnReceiveComplete] -> [OnDisconnect]
    // 虽然存在多个线程都可能使用到Receiver，但Receiver只有一条单独的消息处理线，所有没有多线程资源抢占威胁！
    struct Receiver
    {
        Receiver( DWORD slot ) : 
            recving( 0 ),
            connectionData( new ConnectionData( slot ) )
        {
            memset( &olap, 0, sizeof( olap ) );
            type = OLS_ACCEPT;

            current_transferred = 
            current_flags = 
            total_remaining = 
            sequence = 
            isRecvingHeader = 0;
        }

        ~Receiver()
        {
        }

        // 连接建立后的回调，这里边会启动读取操作
        template < typename ReceiveCompleteHandler >
        BOOL OnEstablished( SOCKET listener, HANDLE iocp, ReceiveCompleteHandler &handler )
        {
            if ( listener == INVALID_SOCKET )
                // 这种状况是当前模块结束造成的，属于正常状况
                return FALSE;

            if ( setsockopt( connectionData->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
                ( char* )&listener, sizeof( listener ) ) == SOCKET_ERROR )
            {
                ErrorHandle( "执行setsockopt失败！ - SO_UPDATE_ACCEPT_CONTEXT", TRUE );
                return FALSE;
            }

            sockaddr *p1 = NULL, *p2 = NULL;
            int i1 = 0, i2 = 0;

            GetAcceptExSockaddrs( recving.buf, 0, 
                sizeof( sockaddr_in ) + 16, sizeof( sockaddr_in ) + 16, 
                &p1, &i1, &p2, &i2 );

            if ( ( p1 == NULL ) || ( p2 == NULL ) )
            {
                ErrorHandle( "执行GetAcceptExSockaddrs失败！", TRUE );
                return FALSE;
            }

            // 设置打开连接状态探测功能
            DWORD outSize = 0;
            struct { u_long onoff, keepalivetime, keepaliveinterval; } kaParam;
            kaParam.onoff = 1;  
            kaParam.keepaliveinterval = 60000;   // 两次KeepAlive探测间的时间间隔 
            kaParam.keepalivetime = 60000;       // 开始首次KeepAlive探测前的TCP空闭时间 
            static const DWORD SIO_KEEPALIVE_VALS = _WSAIOW( IOC_VENDOR, 4 );
            if ( WSAIoctl( connectionData->sock, SIO_KEEPALIVE_VALS, &kaParam, 
                sizeof( kaParam ), NULL, 0, &outSize, NULL, NULL ) == SOCKET_ERROR )  
            {          
                ErrorHandle( "执行setsockopt失败！ - SIO_KEEPALIVE_VALS", TRUE );
                return FALSE;
            } 

            connectionData->addr = * reinterpret_cast< sockaddr_in* >( p2 );

            // 改变重叠结构标志
            type = OLS_RECEIVE;

            // 指定当前需要获取的数据是数据包首部！
            isRecvingHeader = TRUE;

            // 注册到完成端口
            // 注意：这里注册的
            if ( !CreateIoCompletionPort( ( HANDLE )connectionData->sock, iocp, 
                ( ULONG_PTR )static_cast< DWORD >( connectionData->dnid ), 0 ) )
            {
                ErrorHandle( "注册到完成端口失败！", TRUE );
                return FALSE;
            }

            // 发送连接建立通知
            // 之前考虑过是否将这段代码放置到 ActivateReceive 之后，后来发现不行！
            // 如果连接在启动接收操作后立即断开，则会收到一个没有连接通知为前提的断开消息！
            int connState = 0;
            handler( connectionData, connState );

            // 最后再启动数据接收操作
            if ( !ActivateReceive() )
            {
                // 如果启动失败，则需要搭配刚才的连接建立通知再发送一个连接断开通知！
                ErrorHandle( "启动数据接收操作！", TRUE );
                handler( connectionData, connState = -1 );
                return FALSE;
            }

            return TRUE;
        }

        BOOL ActivateReceive()
        {
            if ( !connectionData->validate )
                return FALSE;

            if ( isRecvingHeader )
            {
                total_remaining = sizeof( CRYPTION );
            }
            else
            {
                assert( total_remaining != 0 );
                assert( total_remaining <= 0xf000 );
            }

            recving.reset( total_remaining );

            current_transferred = 
            current_flags = 0;

            // 启动接收过程！！！这是一个循环的接收操作！！！
            if ( SOCKET_ERROR == WSARecv( connectionData->sock, &recving, 1, 
                &current_transferred, &current_flags, &olap, NULL ) )
            {
                if ( WSA_IO_PENDING != WSAGetLastError() )
                    return FALSE;
            }

            return TRUE;
        }

        // 接受数据完毕后的回调
        template < typename ReceiveCompleteHandler >
        BOOL OnReceiveComplete( unsigned long nbytes_transferred, 
            iDirectNetCryption *cryption, 
            ReceiveCompleteHandler &handler ) 
        {
            if ( !connectionData->validate )
                return FALSE;

            assert( recving.size == total_remaining );
            assert( ( nbytes_transferred + ( recving.size - recving.len ) ) <= total_remaining );

            if ( nbytes_transferred == 0 )
            {
                LOG( "连接断开\r\n" );
                return FALSE;
            }

            recving.buf += nbytes_transferred;
            recving.len -= nbytes_transferred;

            if ( recving.len > 0 )
            {
                LOG( "recv non-complete size = %d/%d！\r\n", ( recving.size - recving.len ), total_remaining );

                current_transferred = current_flags = 0;

                // 继续启动接收操作，获取剩余的数据！！！
                if ( SOCKET_ERROR == WSARecv( connectionData->sock, &recving, 1, 
                    &current_transferred, &current_flags, &olap, NULL ) )
                {
                    if ( WSA_IO_PENDING != WSAGetLastError() )
                        return FALSE;
                }

                return TRUE;
            }

            assert( recving.len == 0 );

            // 判断当前是处理包头还是包数据！
            if ( isRecvingHeader )
            {
                // 还处于解包头阶段
                assert( recving.size == sizeof( CRYPTION )  );

                CRYPTION &header = * reinterpret_cast< CRYPTION* >( recving.raw_buffer );

                if ( cryption )
                {
                    cryption->DES_decrypt( &header, sizeof( CRYPTION ) );

                    UINT32 crc32 = cryption->CRC32_compute( 
                        ( char* )&header + sizeof( UINT32 ), 
                        sizeof( CRYPTION ) - sizeof( UINT32 ) );

                    if ( crc32 != header.crc32 ) 
                    {
                        ErrorHandle( "OP_READ_HEAD::crc32 解密后的CRC32错误" );
                        LOG( "解密后的CRC32错误\r\n" );
                        return FALSE;
                    }
                }

                if ( header.dnphdr.seqnum != ( WORD )sequence )
                {
                    ErrorHandle( "detected package lost !!!" );
                    LOG( "发现数据包丢失！\r\n" );
                    return FALSE;
                }

                total_remaining = header.dnphdr.paclen;
                isRecvingHeader = FALSE;
            }
            else
            {
                assert( recving.size == total_remaining  );

                // 获取包数据阶段，因为数据获取完毕，直接启动回调！
                handler( recving.raw_buffer, total_remaining, sequence, connectionData );

                sequence ++;
                isRecvingHeader = TRUE;
            }

            // 启动下一轮的数据获取！
            return ActivateReceive();
        }

        // 连接断开后的回调，如果当前对象的重叠操作都已经完成，则发送销毁指令！
        // 只会是接收到断开事件时被动调用，不会重入！！！
        template < typename ReceiveCompleteHandler >
        void OnDisconnect( ReceiveCompleteHandler &handler )
        {
            // 进入这里就说明，接收操作线已经结束（无论正常结束或者是非正常结束）
            // 发送结束消息
            handler( connectionData );
        }

        // ###########################
        OVERLAPPED              olap;
        DWORD                   type;
        DWORD                   key;
        // ***************************
        DWORD                   current_transferred;
        DWORD                   current_flags;
        DWORD                   sequence;
        BOOL                    isRecvingHeader;
        // ---------------------------
        DWORD                   total_remaining;
        Buffer                  recving;
        // 因为不会同时提交2个Receive操作，所以不会存在多线程问题！！！
        // ---------------------------

        // 该连接的基础数据！
        const ConnectionData::reference_ptr connectionData;

        boost::weak_ptr< struct Connection > safeHolder;
    };
}
