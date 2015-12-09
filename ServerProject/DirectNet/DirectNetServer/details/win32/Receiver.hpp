#pragma once

namespace DreamNet_Win32
{
    // ���ݽ�������ֻ���ڵ�������Ϣ�����ߣ�ֻ�н�����������OnDisconnect
    // ��Ҫ˵����Receiver��ص��̣߳�
    // A��[�¼������߳�] -> [OnEstablished] -> [ActivateReceive]
    // B��[�¼������߳�] -> [OnReceiveComplete] -> [ActivateReceive]
    // C��[�¼������߳�] -> [OnReceiveComplete] -> [OnDisconnect]
    // D��[ģ���ⲿ�߳�] -> [OnClose] -> [OnReceiveComplete] -> [OnDisconnect]
    // ��Ȼ���ڶ���̶߳�����ʹ�õ�Receiver����Receiverֻ��һ����������Ϣ�����ߣ�����û�ж��߳���Դ��ռ��в��
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

        // ���ӽ�����Ļص�������߻�������ȡ����
        template < typename ReceiveCompleteHandler >
        BOOL OnEstablished( SOCKET listener, HANDLE iocp, ReceiveCompleteHandler &handler )
        {
            if ( listener == INVALID_SOCKET )
                // ����״���ǵ�ǰģ�������ɵģ���������״��
                return FALSE;

            if ( setsockopt( connectionData->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
                ( char* )&listener, sizeof( listener ) ) == SOCKET_ERROR )
            {
                ErrorHandle( "ִ��setsockoptʧ�ܣ� - SO_UPDATE_ACCEPT_CONTEXT", TRUE );
                return FALSE;
            }

            sockaddr *p1 = NULL, *p2 = NULL;
            int i1 = 0, i2 = 0;

            GetAcceptExSockaddrs( recving.buf, 0, 
                sizeof( sockaddr_in ) + 16, sizeof( sockaddr_in ) + 16, 
                &p1, &i1, &p2, &i2 );

            if ( ( p1 == NULL ) || ( p2 == NULL ) )
            {
                ErrorHandle( "ִ��GetAcceptExSockaddrsʧ�ܣ�", TRUE );
                return FALSE;
            }

            // ���ô�����״̬̽�⹦��
            DWORD outSize = 0;
            struct { u_long onoff, keepalivetime, keepaliveinterval; } kaParam;
            kaParam.onoff = 1;  
            kaParam.keepaliveinterval = 60000;   // ����KeepAlive̽����ʱ���� 
            kaParam.keepalivetime = 60000;       // ��ʼ�״�KeepAlive̽��ǰ��TCP�ձ�ʱ�� 
            static const DWORD SIO_KEEPALIVE_VALS = _WSAIOW( IOC_VENDOR, 4 );
            if ( WSAIoctl( connectionData->sock, SIO_KEEPALIVE_VALS, &kaParam, 
                sizeof( kaParam ), NULL, 0, &outSize, NULL, NULL ) == SOCKET_ERROR )  
            {          
                ErrorHandle( "ִ��setsockoptʧ�ܣ� - SIO_KEEPALIVE_VALS", TRUE );
                return FALSE;
            } 

            connectionData->addr = * reinterpret_cast< sockaddr_in* >( p2 );

            // �ı��ص��ṹ��־
            type = OLS_RECEIVE;

            // ָ����ǰ��Ҫ��ȡ�����������ݰ��ײ���
            isRecvingHeader = TRUE;

            // ע�ᵽ��ɶ˿�
            // ע�⣺����ע���
            if ( !CreateIoCompletionPort( ( HANDLE )connectionData->sock, iocp, 
                ( ULONG_PTR )static_cast< DWORD >( connectionData->dnid ), 0 ) )
            {
                ErrorHandle( "ע�ᵽ��ɶ˿�ʧ�ܣ�", TRUE );
                return FALSE;
            }

            // �������ӽ���֪ͨ
            // ֮ǰ���ǹ��Ƿ���δ�����õ� ActivateReceive ֮�󣬺������ֲ��У�
            // ����������������ղ����������Ͽ�������յ�һ��û������֪ͨΪǰ��ĶϿ���Ϣ��
            int connState = 0;
            handler( connectionData, connState );

            // ������������ݽ��ղ���
            if ( !ActivateReceive() )
            {
                // �������ʧ�ܣ�����Ҫ����ղŵ����ӽ���֪ͨ�ٷ���һ�����ӶϿ�֪ͨ��
                ErrorHandle( "�������ݽ��ղ�����", TRUE );
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

            // �������չ��̣���������һ��ѭ���Ľ��ղ���������
            if ( SOCKET_ERROR == WSARecv( connectionData->sock, &recving, 1, 
                &current_transferred, &current_flags, &olap, NULL ) )
            {
                if ( WSA_IO_PENDING != WSAGetLastError() )
                    return FALSE;
            }

            return TRUE;
        }

        // ����������Ϻ�Ļص�
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
                LOG( "���ӶϿ�\r\n" );
                return FALSE;
            }

            recving.buf += nbytes_transferred;
            recving.len -= nbytes_transferred;

            if ( recving.len > 0 )
            {
                LOG( "recv non-complete size = %d/%d��\r\n", ( recving.size - recving.len ), total_remaining );

                current_transferred = current_flags = 0;

                // �����������ղ�������ȡʣ������ݣ�����
                if ( SOCKET_ERROR == WSARecv( connectionData->sock, &recving, 1, 
                    &current_transferred, &current_flags, &olap, NULL ) )
                {
                    if ( WSA_IO_PENDING != WSAGetLastError() )
                        return FALSE;
                }

                return TRUE;
            }

            assert( recving.len == 0 );

            // �жϵ�ǰ�Ǵ����ͷ���ǰ����ݣ�
            if ( isRecvingHeader )
            {
                // �����ڽ��ͷ�׶�
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
                        ErrorHandle( "OP_READ_HEAD::crc32 ���ܺ��CRC32����" );
                        LOG( "���ܺ��CRC32����\r\n" );
                        return FALSE;
                    }
                }

                if ( header.dnphdr.seqnum != ( WORD )sequence )
                {
                    ErrorHandle( "detected package lost !!!" );
                    LOG( "�������ݰ���ʧ��\r\n" );
                    return FALSE;
                }

                total_remaining = header.dnphdr.paclen;
                isRecvingHeader = FALSE;
            }
            else
            {
                assert( recving.size == total_remaining  );

                // ��ȡ�����ݽ׶Σ���Ϊ���ݻ�ȡ��ϣ�ֱ�������ص���
                handler( recving.raw_buffer, total_remaining, sequence, connectionData );

                sequence ++;
                isRecvingHeader = TRUE;
            }

            // ������һ�ֵ����ݻ�ȡ��
            return ActivateReceive();
        }

        // ���ӶϿ���Ļص��������ǰ������ص��������Ѿ���ɣ���������ָ�
        // ֻ���ǽ��յ��Ͽ��¼�ʱ�������ã��������룡����
        template < typename ReceiveCompleteHandler >
        void OnDisconnect( ReceiveCompleteHandler &handler )
        {
            // ���������˵�������ղ������Ѿ������������������������Ƿ�����������
            // ���ͽ�����Ϣ
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
        // ��Ϊ����ͬʱ�ύ2��Receive���������Բ�����ڶ��߳����⣡����
        // ---------------------------

        // �����ӵĻ������ݣ�
        const ConnectionData::reference_ptr connectionData;

        boost::weak_ptr< struct Connection > safeHolder;
    };
}
