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
                ErrorHandle( "错误的事件响应句柄！" );
                return DNR_E_INVALID_PARAMETER;
            }

            if ( ( maxNumberClients == 0 ) || ( maxNumberClients > 30000 ) )
            {
                ErrorHandle( "错误的最大连接数！" );
                return DNR_E_INVALID_PARAMETER;
            }

            if ( ( iocp != NULL ) ||
                ( this->handler != NULL ) ||
                ( !client_slots.empty() ) ||
                ( !useing_slots.empty() ) || 
                ( !lookup_slots.empty() ) )
            {
                ErrorHandle( "本对象已经初始化过了！" );
                return DNR_E_FAIL;
            }

            iocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
            if ( iocp == NULL ) 
            {
                ErrorHandle("创建完成端口失败！", TRUE);
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

            // 如果是线程退出，则host失败！
            DWORD hostResult = WaitForMultipleObjects( 2, hostResultEvents, FALSE, INFINITE );
            CloseHandle( event_notify );
            if ( hostResult == WAIT_OBJECT_0 )
            {
                threadWaiter.Quit();
                return DNR_E_FAIL;
            }

            // 如果host成功，则继续启动其余的线程！
            // cpu * 2 个工作线程
            SYSTEM_INFO systemInfo;
            GetSystemInfo( &systemInfo );
            for ( int i = ( systemInfo.dwNumberOfProcessors * 2 ); i > 0; i -- )
            {
                threadWaiter.BeginThread( 
                    boost::bind( &cDirectNetServer::IocpWorkerThreadProcedure, this ) );
            }

            // 最后启动定时脉冲发送线程，因为其他地方会使用到这个线程的句柄！
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
                // 如果PUSH失败，可能是消息队列满了，则尝试主动启动发送消息
                if ( !conn->Send() )
                    return DNR_E_FAIL;

                // 发送成功后，再次尝试发送当前数据！同时因为已经启动了发送操作，则不需要往下检测 immediately
                return conn->Push( pvBuffer, dwLength ) ? DNR_S_OK : DNR_E_FAIL;
            }

            // 如果发送没有等待间隔，则表示需要在发送线程中按脉冲计时发送，即这里不再主动发送数据！
            if ( sendMargin != 0 )
                return DNR_S_OK;

            // PUSH成功后，因为性能要求，需要立即发送数据！
            return conn->Send() ? DNR_S_OK : DNR_E_FAIL;
        }

        DNRESULT DeletePlayer( DNID dnidPlayer )
        {
            if ( ( iocp == NULL ) || ( listener == INVALID_SOCKET ) )
                return DNR_E_NOT_INITIALIZED;

            boost::shared_ptr< Connection > conn = GetUsedSlot( dnidPlayer );
            if ( !conn )
                return DNR_E_INVALID_PLAYER;

            LOG( "主动断开一个连接\r\n" );

            // 这里只需要将这个连接关闭，当连接关闭后，Overlap操作则会失败，在结束处理中完全清除这个连接！
            conn->CloseConnection();

            return TRUE;
        }

        DNRESULT Close()
        {
            if ( ( iocp == NULL ) || ( listener == INVALID_SOCKET ) )
                return DNR_E_NOT_INITIALIZED;

            // 关闭监听器
            {
                SOCKET s = listener;
                listener = INVALID_SOCKET;
                shutdown( s, SD_BOTH );
                closesocket( s );
            }

            // 关闭所有的当前连接
            {
                // 这个地方需要用读写锁！因为内部存在GetUsedSlot调用，而GetUsedSlot中使用了slots_lock的读者锁！
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

            // 发出退出标记，这里只需要发送一个退出标记，当某个工作线程收到并处理这个标记后，
            // 再次发送一个退出标记，直到所有工作线程全部退出，
            // 最后还会剩余一个退出标记，则由后边的代码处理直到结束！
            PostQueuedCompletionStatus( iocp, NULL, 0, 0 );

            // 关闭所有工作线程！send -> work -> accept
            threadWaiter.Quit();

            ULONG_PTR key = 0;
            DWORD cbTransfered = 0;
            OVERLAPPED* olap = NULL;

            // 处理所有剩余的事件！
            while ( GetQueuedCompletionStatus( iocp, &cbTransfered, &key, &olap, 0 ) )
            {
                if ( olap == NULL )
                    continue;

                ProcessOverlapedResult( key, cbTransfered, olap );
            }

            // 到这里为止，应该所有的操作都完全结束！所有数据归为初始化状态！
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
                case 1: // 修改发送脉冲计时间隔
                    {
                        if ( pParameter[1] > 3000 )
                            MessageBox( 0,"新设置的发送脉冲计时间隔大于3秒!",0,0 );

                        // 注意！！！特殊处理！！！为了配合旧的外部设置[1，1]，所以将其*100
                        DWORD margin = sendMargin;
                        sendMargin = pParameter[1] * 100;

                        // 检测是否需要激活网络发送线程
                        if ( ( margin == 0 ) && ( sendMargin != 0 ) )
                            ResumeThread( threadWaiter.LatestThread() );
                    }
                    break;

                case 2:
                    // 设置是否在发送数据前做数据包压缩
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
                ErrorHandle( "执行AcceptEx失败！", TRUE );
                FreeSlot( static_cast< DWORD >( conn->connectionData->dnid ) );
                return FALSE;
            }

            return TRUE;
        }

    private:
        // 完成端口连接线程
        int IocpAcceptThreadProcedure( HANDLE resultEvent, DNPORT dnPort );

        // 完成端口工作线程
        int IocpWorkerThreadProcedure();

        // 消息发送线程
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
                        OutputDebugString( "已经通过配置文件开启了服务端的数据压缩功能！\r\n" );
                }
            }

            WSADATA wsad;
            if ( WSAStartup( MAKEWORD( 2, 0 ), &wsad ) != 0 )
                ErrorHandle( "当前系统并不支持WinSock2.0，相应的网络操作都无法完成！", TRUE );
        }

        ~cDirectNetServer()
        {
            // 防治用户不掉用CLOSE直接退出！
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

            // 临界区内只操作管理器，临界区外才删除数据，一定程度上提高性能！
            {
                UGE::ReadWriteLock::Barrier4ReadWrite barrier( slots_lock );

                assert( client_slots[ slot ] );

                conn = client_slots[ slot ];
                if ( ( !conn ) || ( !conn->CheckHalfDnid( key ) ) )
                {
                    ErrorHandle( ( !conn ) ? "FreeSlot : 空的删除对象" : "FreeSlot : 不匹配的删除对象" );
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

        DWORD sendMargin;   // 发送线程中的脉冲计时间隔，以毫秒计
        BOOL  compress;     // 是否在发送时将打包数据进行压缩处理

        UGE::ThreadWaiter threadWaiter;
        UGE::ReadWriteLock slots_lock;

        std::vector< boost::shared_ptr< Connection > >		client_slots;	// 所有的连接列表
        std::stack< DWORD >                                 lookup_slots;   // 当前无效的连接列表
        std::map< DWORD, DNID >                             useing_slots;   // 当前正在使用的连接列表
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

        // 通知外部，监听操作已经准备完毕！
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
                // 如果发送脉冲间隔为0，则表示由SEND操作及时发送，而不是停止发送！
                // 所以在这里将当前发送线程挂起！直到退出或新的脉冲设定再次激活当前线程！
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

            // 因为可能在上边发送的过程中，外部修改了新的sendMargin数据，所以这里需要做容错！
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

                // 在连接成功后，为conn对象设定是否压缩发送数据的标记
                conn->compress = _this->compress;
            }

            // 使用额外变量的写法，是为了在假设写udata时溢出，而导致的错误能够方便查找（直接错在栈上，而不是堆中！）
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
        // 这里之所以直接转为Receiver，是因为Receiver和Sender的结构头部都是OVERLAP+DWORD的样式！
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

                // 由于近段时间出现的多次在调用OnReceiveComplete后，继续执行OnDisconnect出错的问题
                // 现尝试使用 conn 来保持该连接的有效性！但是对性能会造成不小的影响！！！
                // 现分析，是由于在一些回调操作中，上层代码进行了一些对象删除的操作，
                // 而导致触发了新的事件，其他线程将该连接删除了。。。
                // 处理：如何在回调过程中保持连接的有效性！
                // OnCreatePlayer | OnReceivePacket | OnDeletePlayer
                // 都是通过 temp_op_ProcessOverlapedResult 进行回调的，那么可通过它来保证有效性

                if ( !conn->OnReceiveComplete( cbTransfered, cryption,
                    boost::bind( &temp_op_ProcessOverlapedResult, this, _1, _2, _3, _4 ) ) )
                {
                    if ( FreeSlot( static_cast< DWORD >( key ) ) )
                    {
                        // 只有这一个地方会调用 OnDisconnect ！！！
                        conn->OnDisconnect( 
                            boost::bind( &temp_op_ProcessOverlapedResult, this, ( void * )0, -1, -1, _1 ) );
                    }
                }
            }
            break;

        case OLS_SEND:
            {
                // 当对象发送数据完毕的同时，可能这个对象已经处于游离状态了（已经脱离管理器管理），所以assert无意义
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
                ErrorHandle( "GetQueuedCompletionStatus 操作失败", TRUE );
                break; // or continue ???
            }

            if ( ( key == 0 ) && ( cbTransfered == 0 ) && ( olap == 0 ) )
            {
                // ErrorHandle( "工作线程正常退出！\r\n" );
                Trace( "工作线程[%d][%d]正常退出！\r\n", GetCurrentThreadId(), ( int )( _W64 int )iocp );
                LOG( "工作线程正常退出！\r\n" );
                break;
            }

            assert( ( key != 0 ) && ( olap != 0 ) );

            if ( ( key == 0 ) || ( olap == 0 ) )
            {
                ErrorHandle( "key 或者 olap 无效，工作线程异常退出" );
                LOG( "key 或者 olap 无效，工作线程异常退出！\r\n" );
                break;
            }

            ProcessOverlapedResult( key, cbTransfered, olap );
        }

        if ( !PostQueuedCompletionStatus( iocp, NULL, 0, 0 ) )
            ErrorHandle( "传递退出消息时出现异常！" );

        return 1;
    }
}
