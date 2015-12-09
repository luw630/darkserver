#pragma once

namespace DreamNet_Win32
{
    const static int MAX_ACCEPT = 10;

    class cDirectNetServer : 
        public iDirectNetServer 
    {
    public:
        DNRESULT Initialize( iDirectNetCryption *cryption,
            iDirectNetServerEventHandler *handler, DWORD maxNumberClients )
        {
            if ( handler == NULL )
            {
                ErrorHandle( "������¼���Ӧ�����" );
                return DNR_E_INVALID_PARAMETER;
            }

            if ( ( maxNumberClients == 0 ) || ( maxNumberClients > 30000 ) )
            {
                ErrorHandle( "����������������" );
                return DNR_E_INVALID_PARAMETER;
            }

            if ( ( iocp != NULL ) ||
                ( this->handler != NULL ) ||
                ( !client_slots.empty() ) ||
                ( !useing_slots.empty() ) || 
                ( !lookup_slots.empty() ) )
            {
                ErrorHandle( "�������Ѿ���ʼ�����ˣ�" );
                return DNR_E_FAIL;
            }

            iocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
            if ( iocp == NULL ) 
            {
                ErrorHandle("������ɶ˿�ʧ�ܣ�", TRUE);
                return DNR_E_FAIL;
            }

            this->cryption = cryption;
            this->handler = handler;

            assert( lookup_slots.empty() );
            client_slots.resize( maxNumberClients );
            for ( DWORD i = 0; i < maxNumberClients; i ++ )
                lookup_slots.push( i );

            return DNR_S_OK;
        }

        DNRESULT Host( DNPORT dnPort )
        {
            if ( iocp == NULL || handler == NULL || lookup_slots.empty() )
                return DNR_E_NOT_INITIALIZED;

            assert( listener == INVALID_SOCKET );

            HANDLE event_notify = CreateEvent( 0, 0, 0, 0 );
            HANDLE event_thread = threadWaiter.BeginThread( 
                boost::bind( &cDirectNetServer::IocpAcceptThreadProcedure, this, event_notify, dnPort ) );

            HANDLE hostResultEvents[ 2 ] = { event_thread, event_notify };

            // ������߳��˳�����hostʧ�ܣ�
            DWORD hostResult = WaitForMultipleObjects( 2, hostResultEvents, FALSE, INFINITE );
            CloseHandle( event_notify );
            if ( hostResult == WAIT_OBJECT_0 )
            {
                threadWaiter.Quit();
                return DNR_E_FAIL;
            }

            // ���host�ɹ������������������̣߳�
            // cpu * 2 �������߳�
            SYSTEM_INFO systemInfo;
            GetSystemInfo( &systemInfo );
            for ( int i = ( systemInfo.dwNumberOfProcessors * 2 ); i > 0; i -- )
            {
                threadWaiter.BeginThread( 
                    boost::bind( &cDirectNetServer::IocpWorkerThreadProcedure, this ) );
            }

            // ���������ʱ���巢���̣߳���Ϊ�����ط���ʹ�õ�����̵߳ľ����
            threadWaiter.BeginThread( 
                boost::bind( &cDirectNetServer::IocpSendThreadProcedure, this ) );

            return DNR_S_OK;
        }

        DNRESULT SendTo( DNID dnidPlayer, PVOID pvBuffer, DWORD dwLength )
        {
            if ( ( iocp == NULL ) || ( listener == INVALID_SOCKET ) )
                return DNR_E_NOT_INITIALIZED;

            if ( pvBuffer == NULL || dwLength == 0 || dwLength > 0xf000 )
                return DNR_E_INVALID_PARAMETER;

            boost::shared_ptr< Connection > conn = GetUsedSlot( dnidPlayer );
            if ( !conn )
                return DNR_E_INVALID_PLAYER;

            if ( !conn->Push( pvBuffer, dwLength ) )
            {
                // ���PUSHʧ�ܣ���������Ϣ�������ˣ�������������������Ϣ
                if ( !conn->Send() )
                    return DNR_E_FAIL;

                // ���ͳɹ����ٴγ��Է��͵�ǰ���ݣ�ͬʱ��Ϊ�Ѿ������˷��Ͳ���������Ҫ���¼�� immediately
                return conn->Push( pvBuffer, dwLength ) ? DNR_S_OK : DNR_E_FAIL;
            }

            // �������û�еȴ���������ʾ��Ҫ�ڷ����߳��а������ʱ���ͣ������ﲻ�������������ݣ�
            if ( sendMargin != 0 )
                return DNR_S_OK;

            // PUSH�ɹ�����Ϊ����Ҫ����Ҫ�����������ݣ�
            return conn->Send() ? DNR_S_OK : DNR_E_FAIL;
        }

        DNRESULT DeletePlayer( DNID dnidPlayer )
        {
            if ( ( iocp == NULL ) || ( listener == INVALID_SOCKET ) )
                return DNR_E_NOT_INITIALIZED;

            boost::shared_ptr< Connection > conn = GetUsedSlot( dnidPlayer );
            if ( !conn )
                return DNR_E_INVALID_PLAYER;

            LOG( "�����Ͽ�һ������\r\n" );

            // ����ֻ��Ҫ��������ӹرգ������ӹرպ�Overlap�������ʧ�ܣ��ڽ�����������ȫ���������ӣ�
            conn->CloseConnection();

            return TRUE;
        }

        DNRESULT Close()
        {
            if ( ( iocp == NULL ) || ( listener == INVALID_SOCKET ) )
                return DNR_E_NOT_INITIALIZED;

            // �رռ�����
            {
                SOCKET s = listener;
                listener = INVALID_SOCKET;
                shutdown( s, SD_BOTH );
                closesocket( s );
            }

            // �ر����еĵ�ǰ����
            {
                // ����ط���Ҫ�ö�д������Ϊ�ڲ�����GetUsedSlot���ã���GetUsedSlot��ʹ����slots_lock�Ķ�������
                UGE::ReadWriteLock::Barrier4ReadWrite barrier( slots_lock );

                std::for_each( useing_slots.begin(), useing_slots.end(), 
                    boost::bind( &Connection::CloseConnection, 
                        boost::bind( &cDirectNetServer::GetUsedSlot, 
                            this, 
                            boost::bind( 
                                &std::map< DWORD, DNID >::value_type::second, _1 
                            ) 
                        ), 
                        TRUE 
                    ) 
                );
            }

            // �����˳���ǣ�����ֻ��Ҫ����һ���˳���ǣ���ĳ�������߳��յ������������Ǻ�
            // �ٴη���һ���˳���ǣ�ֱ�����й����߳�ȫ���˳���
            // ��󻹻�ʣ��һ���˳���ǣ����ɺ�ߵĴ��봦��ֱ��������
            PostQueuedCompletionStatus( iocp, NULL, 0, 0 );

            // �ر����й����̣߳�send -> work -> accept
            threadWaiter.Quit();

            ULONG_PTR key = 0;
            DWORD cbTransfered = 0;
            OVERLAPPED* olap = NULL;

            // ��������ʣ����¼���
            while ( GetQueuedCompletionStatus( iocp, &cbTransfered, &key, &olap, 0 ) )
            {
                if ( olap == NULL )
                    continue;

                ProcessOverlapedResult( key, cbTransfered, olap );
            }

            // ������Ϊֹ��Ӧ�����еĲ�������ȫ�������������ݹ�Ϊ��ʼ��״̬��
            assert( lookup_slots.size() == client_slots.size() );
            assert( useing_slots.empty() );

            while ( !lookup_slots.empty() )
                lookup_slots.pop();

            useing_slots.clear();
            client_slots.clear();

            handler = NULL;
            cryption = NULL;

            CloseHandle( iocp );
            iocp = NULL;

            return DNR_S_OK;
        }

        DNRESULT GetPlayerConnectionInfo( DNID dnidPlayer, CONNECTIONINFO* pConnectionInfo )
        {
            if ( ( iocp == NULL ) || ( listener == INVALID_SOCKET ) )
                return DNR_E_NOT_INITIALIZED;

            if ( pConnectionInfo == NULL )
                return DNR_E_INVALID_PARAMETER;

            if ( dnidPlayer == 0xffffffffffffffff )
            {
                pConnectionInfo->addr = static_cast< DWORD >( useing_slots.size() );
                pConnectionInfo->port = static_cast< WORD >( lookup_slots.size() );
                return DNR_S_OK;
            }
            else if ( dnidPlayer == 0xff00ff00ff00ff00 )
            {
                LPDWORD pParameter = ( LPDWORD )pConnectionInfo;
                switch( pParameter[0] )
                {
                case 1: // �޸ķ��������ʱ���
                    {
                        if ( pParameter[1] > 3000 )
                            MessageBox( 0,"�����õķ��������ʱ�������3��!",0,0 );

                        // ע�⣡�������⴦������Ϊ����Ͼɵ��ⲿ����[1��1]�����Խ���*100
                        DWORD margin = sendMargin;
                        sendMargin = pParameter[1] * 100;

                        // ����Ƿ���Ҫ�������緢���߳�
                        if ( ( margin == 0 ) && ( sendMargin != 0 ) )
                            ResumeThread( threadWaiter.LatestThread() );
                    }
                    break;

                case 2:
                    // �����Ƿ��ڷ�������ǰ�����ݰ�ѹ��
                    compress = ( pParameter[1] != 0 );
                    break;

                default:
                    return DNR_E_FAIL;
                }
                return DNR_S_OK;
            }

            // make critical section
            boost::shared_ptr< Connection > conn = GetUsedSlot( dnidPlayer );
            if ( !conn )
                return DNR_E_INVALID_PLAYER;

            conn->GetConnectionInfo( *pConnectionInfo );
            return DNR_S_OK;
        }

    private:
        BOOL QueryNewAccept()
        {
            if ( listener == INVALID_SOCKET )
                return FALSE;

            DWORD slot = -1;
            if ( !LookupEmptySlot( slot ) )
                return FALSE;

            boost::shared_ptr< Connection > conn;

            {
                UGE::ReadWriteLock::Barrier4ReadWrite barrier( slots_lock );

                assert( slot < client_slots.size() );
                assert( !client_slots[ slot ] );

                conn.reset( new Connection( slot ) );
                conn->safeHolder = conn;

                assert( conn ); // new operation failure

                client_slots[ slot ] = conn;
            }

            if ( !conn->QueryAccept( listener ) )
            {
                ErrorHandle( "ִ��AcceptExʧ�ܣ�", TRUE );
                FreeSlot( static_cast< DWORD >( conn->connectionData->dnid ) );
                return FALSE;
            }

            return TRUE;
        }

    private:
        // ��ɶ˿������߳�
        int IocpAcceptThreadProcedure( HANDLE resultEvent, DNPORT dnPort );

        // ��ɶ˿ڹ����߳�
        int IocpWorkerThreadProcedure();

        // ��Ϣ�����߳�
        int IocpSendThreadProcedure();

        inline void ProcessOverlapedResult( ULONG_PTR key, DWORD cbTransfered, OVERLAPPED *olap );

    public:
        cDirectNetServer()
        {
            iocp = NULL;
            handler = NULL;
            cryption = NULL;
            listener = INVALID_SOCKET;
            sendMargin = 0;
            compress = FALSE;

			if ( HMODULE hdll = LoadLibraryA( "shlwapi.dll" ) )
            {
                if ( BOOL ( WINAPI *PathFileExistsA )( LPCSTR ) = 
                    ( BOOL ( WINAPI* )( LPCSTR ) )GetProcAddress( hdll, "PathFileExistsA" ) )
                {
                    if ( compress = PathFileExistsA( ".\\compress.flg" ) )
                        OutputDebugString( "�Ѿ�ͨ�������ļ������˷���˵�����ѹ�����ܣ�\r\n" );
                }
            }

            WSADATA wsad;
            if ( WSAStartup( MAKEWORD( 2, 0 ), &wsad ) != 0 )
                ErrorHandle( "��ǰϵͳ����֧��WinSock2.0����Ӧ������������޷���ɣ�", TRUE );
        }

        ~cDirectNetServer()
        {
            // �����û�������CLOSEֱ���˳���
            Close();

            assert( listener == INVALID_SOCKET );

            WSACleanup();
        }

    public:
        boost::shared_ptr< Connection > GetUsedSlot( DNID dnid )
        {
            DWORD slot = static_cast< DWORD >( dnid & 0x0000ffff );

            UGE::ReadWriteLock::Barrier4Read barrier( slots_lock );

            if ( slot >= client_slots.size() )
                return boost::shared_ptr< Connection >();

            boost::shared_ptr< Connection > &result = client_slots[ slot ]; 

            if ( ( !result ) || ( !result->VerifyDnid( dnid ) ) )
                return boost::shared_ptr< Connection >();

            assert( useing_slots.find( slot ) != useing_slots.end() );

            return result;
        }

        boost::shared_ptr< Connection > GetUsedSlot_HalfDnid( DWORD dnid )
        {
            DWORD slot = static_cast< DWORD >( dnid & 0x0000ffff );

            UGE::ReadWriteLock::Barrier4Read barrier( slots_lock );

            if ( slot >= client_slots.size() )
                return boost::shared_ptr< Connection >();

            boost::shared_ptr< Connection > &result = client_slots[ slot ]; 

            if ( ( !result ) || ( !result->CheckHalfDnid( dnid ) ) )
                return boost::shared_ptr< Connection >();

            assert( useing_slots.find( slot ) != useing_slots.end() );

            return result;
        }

        BOOL CheckKey( ULONG_PTR key )
        {
            DWORD slot = static_cast< WORD >( key );

            UGE::ReadWriteLock::Barrier4Read barrier( slots_lock );

            if ( slot >= client_slots.size() )
                return FALSE;

            boost::shared_ptr< Connection > &result = client_slots[ slot ]; 

            if ( ( !result ) || ( !result->CheckHalfDnid( key ) ) )
                return FALSE;

            return TRUE;
        }

        BOOL LookupEmptySlot( DWORD &slot )
        {
            UGE::ReadWriteLock::Barrier4ReadWrite barrier( slots_lock );

            if ( lookup_slots.empty() )
                return FALSE;

            slot = lookup_slots.top();
            lookup_slots.pop();

            assert( !client_slots[ slot ] );

            return TRUE;
        }

        BOOL FreeSlot( DWORD key )
        {
            DWORD slot = static_cast< WORD >( key );
            boost::shared_ptr< Connection > conn;

            // �ٽ�����ֻ�������������ٽ������ɾ�����ݣ�һ���̶���������ܣ�
            {
                UGE::ReadWriteLock::Barrier4ReadWrite barrier( slots_lock );

                assert( client_slots[ slot ] );

                conn = client_slots[ slot ];
                if ( ( !conn ) || ( !conn->CheckHalfDnid( key ) ) )
                {
                    ErrorHandle( ( !conn ) ? "FreeSlot : �յ�ɾ������" : "FreeSlot : ��ƥ���ɾ������" );
                    return FALSE;
                }

                client_slots[ slot ].reset();

                lookup_slots.push( slot );
                useing_slots.erase( slot );
            }

            // same as delete
            // conn.reset(); use conn's destructor
            return TRUE;
        }

    public:
        HANDLE iocp;
        SOCKET listener;
        iDirectNetServerEventHandler *handler;
        iDirectNetCryption *cryption;

        DWORD sendMargin;   // �����߳��е������ʱ������Ժ����
        BOOL  compress;     // �Ƿ��ڷ���ʱ��������ݽ���ѹ������

        UGE::ThreadWaiter threadWaiter;
        UGE::ReadWriteLock slots_lock;

        std::vector< boost::shared_ptr< Connection > >		client_slots;	// ���е������б�
        std::stack< DWORD >                                 lookup_slots;   // ��ǰ��Ч�������б�
        std::map< DWORD, DNID >                             useing_slots;   // ��ǰ����ʹ�õ������б�
    };

    int cDirectNetServer::IocpAcceptThreadProcedure( HANDLE resultEvent, DNPORT dnPort )
    {
        listener = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
        if ( listener == INVALID_SOCKET )
            return 0;

        sockaddr_in addr;
        addr.sin_family = PF_INET;
        addr.sin_port = htons( dnPort );
        addr.sin_addr.s_addr = INADDR_ANY;

        if ( bind( listener, ( sockaddr* )&addr, sizeof( addr ) ) == SOCKET_ERROR )
            return 0;

        if ( listen( listener, MAX_ACCEPT ) == SOCKET_ERROR )
            return 0;

        if ( !CreateIoCompletionPort( ( HANDLE )listener, iocp, ( ULONG_PTR )listener, 0 ) )
            return 0;

        for ( int i = 0; i < MAX_ACCEPT; i ++ )
            QueryNewAccept();

        // ֪ͨ�ⲿ�����������Ѿ�׼����ϣ�
        SetEvent( resultEvent );

        return IocpWorkerThreadProcedure();
    }

    static inline void temp_op1_IocpSendThreadProcedure( DNID *&iter, DNID value )
    {
        *iter = value; iter ++;
    }

    static inline void temp_op2_IocpSendThreadProcedure( cDirectNetServer *_this, DNID value )
    {
        boost::shared_ptr< Connection > conn = _this->GetUsedSlot( value );
        if ( conn ) conn->Send();
    }

    int cDirectNetServer::IocpSendThreadProcedure()
    {
        DNID *usingSlots = new DNID[ client_slots.size() ];

        while ( !threadWaiter.QuitFlag() )
        {
            if ( sendMargin == 0 )
            {
                // �������������Ϊ0�����ʾ��SEND������ʱ���ͣ�������ֹͣ���ͣ�
                // ���������ｫ��ǰ�����̹߳���ֱ���˳����µ������趨�ٴμ��ǰ�̣߳�
                SuspendThread( threadWaiter.LatestThread() );
                continue;
            }

            DNID *end = usingSlots;

            // critical section for copy all used item;
            {
                UGE::ReadWriteLock::Barrier4Read barrier( slots_lock );

                std::for_each( useing_slots.begin(), useing_slots.end(), 
                    boost::bind( &temp_op1_IocpSendThreadProcedure, 
                        boost::ref( end ), 
                        boost::bind( 
                            &std::map< DWORD, DNID >::value_type::second, _1 
                        ) 
                    ) 
                );
            }

            std::for_each( usingSlots, end, 
                boost::bind( &temp_op2_IocpSendThreadProcedure, this, _1 ) );

            // ��Ϊ�������ϱ߷��͵Ĺ����У��ⲿ�޸����µ�sendMargin���ݣ�����������Ҫ���ݴ�
            if ( sendMargin != 0 )
            {
                Sleep( sendMargin );
            }
        }

        delete [] usingSlots;

        return 1;
    }

    static inline void temp_op_ProcessOverlapedResult( 
        cDirectNetServer *_this, void *data, size_t size, DWORD sequence, const ConnectionData::reference_ptr &conn )
    {
        assert( _this->handler );

        if ( data != 0 )
        {
            _this->handler->OnReceivePacket( conn->dnid, conn->userData, data, ( DWORD )size, sequence );
        }
        else if ( size == 0 )
        {
            {
                UGE::ReadWriteLock::Barrier4ReadWrite barrier( _this->slots_lock );

                _this->useing_slots[ static_cast< DWORD >( conn->dnid & 0xffff ) ] = conn->dnid;

                // �����ӳɹ���Ϊconn�����趨�Ƿ�ѹ���������ݵı��
                conn->compress = _this->compress;
            }

            // ʹ�ö��������д������Ϊ���ڼ���дudataʱ����������µĴ����ܹ�������ң�ֱ�Ӵ���ջ�ϣ������Ƕ��У���
            PVOID udata = NULL;
            _this->handler->OnCreatePlayer( conn->dnid, &udata );
            conn->userData = udata;
        }
        else if ( size == -1 )
        {
            _this->handler->OnDeletePlayer( conn->dnid, conn->userData );
        }
        else
        {
            assert( 0 );
        }
    }

    void cDirectNetServer::ProcessOverlapedResult( ULONG_PTR key, DWORD cbTransfered, OVERLAPPED *olap )
    {
        // ����֮����ֱ��תΪReceiver������ΪReceiver��Sender�Ľṹͷ������OVERLAP+DWORD����ʽ��
        switch ( reinterpret_cast< Receiver* >( olap )->type )
        {
        case OLS_ACCEPT:
            {
                Receiver *receiver = reinterpret_cast< Receiver* >( olap );
                assert( CheckKey( receiver->key ) );
                boost::shared_ptr< Connection > conn = receiver->safeHolder.lock();

                if ( !conn->OnEstablished( listener, iocp, 
                    boost::bind( &temp_op_ProcessOverlapedResult, this, ( void * )0, _2, _2, _1 ) ) )
                    FreeSlot( conn->key );

                QueryNewAccept();
            }
            break;

        case OLS_RECEIVE:
            {
                Receiver *receiver = reinterpret_cast< Receiver* >( olap );
                assert( CheckKey( key ) );
                assert( static_cast< ULONG_PTR >( receiver->key ) == key );
                assert( static_cast< ULONG_PTR >( receiver->connectionData->dnid ) == key );
                boost::shared_ptr< Connection > conn = receiver->safeHolder.lock();

                // ���ڽ���ʱ����ֵĶ���ڵ���OnReceiveComplete�󣬼���ִ��OnDisconnect���������
                // �ֳ���ʹ�� conn �����ָ����ӵ���Ч�ԣ����Ƕ����ܻ���ɲ�С��Ӱ�죡����
                // �ַ�������������һЩ�ص������У��ϲ���������һЩ����ɾ���Ĳ�����
                // �����´������µ��¼��������߳̽�������ɾ���ˡ�����
                // ��������ڻص������б������ӵ���Ч�ԣ�
                // OnCreatePlayer | OnReceivePacket | OnDeletePlayer
                // ����ͨ�� temp_op_ProcessOverlapedResult ���лص��ģ���ô��ͨ��������֤��Ч��

                if ( !conn->OnReceiveComplete( cbTransfered, cryption,
                    boost::bind( &temp_op_ProcessOverlapedResult, this, _1, _2, _3, _4 ) ) )
                {
                    if ( FreeSlot( static_cast< DWORD >( key ) ) )
                    {
                        // ֻ����һ���ط������ OnDisconnect ������
                        conn->OnDisconnect( 
                            boost::bind( &temp_op_ProcessOverlapedResult, this, ( void * )0, -1, -1, _1 ) );
                    }
                }
            }
            break;

        case OLS_SEND:
            {
                // ��������������ϵ�ͬʱ��������������Ѿ���������״̬�ˣ��Ѿ��������������������assert������
                // assert( CheckKey( key ) );
                Sender *sender = reinterpret_cast< Sender* >( olap );
                assert( static_cast< ULONG_PTR >( sender->connectionData->dnid ) == key );

                sender->OnSendComplete( cbTransfered );
            }
            break;
        }
    }

    int cDirectNetServer::IocpWorkerThreadProcedure()
    {
        ULONG_PTR key = 0;
        DWORD cbTransfered = 0;
        OVERLAPPED *olap = NULL;

        while ( true ) 
        {
            BOOL ret = GetQueuedCompletionStatus( iocp, &cbTransfered, &key, &olap, INFINITE );
            if ( ( ret == 0 ) && ( olap == NULL ) )
            {
                ErrorHandle( "GetQueuedCompletionStatus ����ʧ��", TRUE );
                break; // or continue ???
            }

            if ( ( key == 0 ) && ( cbTransfered == 0 ) && ( olap == 0 ) )
            {
                // ErrorHandle( "�����߳������˳���\r\n" );
                Trace( "�����߳�[%d][%d]�����˳���\r\n", GetCurrentThreadId(), ( int )( _W64 int )iocp );
                LOG( "�����߳������˳���\r\n" );
                break;
            }

            assert( ( key != 0 ) && ( olap != 0 ) );

            if ( ( key == 0 ) || ( olap == 0 ) )
            {
                ErrorHandle( "key ���� olap ��Ч�������߳��쳣�˳�" );
                LOG( "key ���� olap ��Ч�������߳��쳣�˳���\r\n" );
                break;
            }

            ProcessOverlapedResult( key, cbTransfered, olap );
        }

        if ( !PostQueuedCompletionStatus( iocp, NULL, 0, 0 ) )
            ErrorHandle( "�����˳���Ϣʱ�����쳣��" );

        return 1;
    }
}
