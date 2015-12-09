#pragma once

// #define USE_QUEUE
#include "../lzw.hpp"

namespace DreamNet_Win32
{
    // 重要说明：Sender相关的线程！
    // A）[消息发送线程] -> [InternalSend]
    // B）[事件工作线程] -> [OnSendComplete] -> [InternalSend]
    // C）[事件工作线程] -> [OnReceiveComplete] -> [handler] -> [PushPackage / InternalSend]
    // D）[模块外部线程] -> [PushPackage / InternalSend]
    // E）[模块外部线程] -> [OnClose] -> [OnSendComplete] -> [InternalSend]
    // F）[模块外部线程] -> [OnClose] -> [OnReceiveComplete] -> [handler] -> [PushPackage / InternalSend]
    // 所以，Sender内部的3个主要函数的临界区都加得比较满！！！
    // E、F 没有资源抢夺威胁，因为OnClose中在处理网络事件时，所有的工作线程都已经停止了！！！
    struct Sender
    {
        typedef boost::shared_ptr< Sender > reference_ptr;

        enum SEND_RESULT { SEND_OK, SEND_FAIL, SEND_NODATA, SEND_PENDING };

#ifndef USE_QUEUE
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
#endif

        struct DummyBarrier
        {
            template < typename _lock > DummyBarrier( _lock& ) {}
            ~DummyBarrier() {}
        };

        Sender( const ConnectionData::reference_ptr &connectionData ) : connectionData( connectionData )
        {
            // connectionData 一定是有效的对象！
            assert( connectionData );

            memset( &olap, 0, sizeof( olap ) );

            type = OLS_SEND;
            current_transferred = 0;
            sequence = 0;

#ifdef USE_QUEUE
            sendQ.push( Buffer( 0 ) );
            sendQ.push( Buffer( 0 ) );
#endif
        }

        ~Sender()
        {
            UGE::CriticalLock::Barrier4ReadWrite barrier( lock4Q );

            // 应该不存在处于发送中的数据包
            assert( sending.len == NULL );
        }

        int PushPackage( const void *data, size_t size )
        {
            if ( !connectionData->validate )
                return -1;

            int result = 0;

            {
                UGE::CriticalLock::Barrier4ReadWrite barrier( lock4Q );

                assert( ( sendQ.size() != 0 ) && ( sendQ.size() <= 2 ) );

                if ( sendQ.empty() )
                    return -1;

                // 始终是直接往第一个buffer里边写数据！
                result = ( sendQ.front().push( data, size, sequence ++ ) ? 0 : -1 );

                if ( result )
                {
                    if ( ++ connectionData->pushFailureTimes > 10 )
                    {
                        result = -2;

                        // 打包失败到达一定次数时，直接将连接断开，失败返回
                        // 通过 SD_BOTH 导致连接断开，但不释放资源
                        shutdown( connectionData->sock, SD_BOTH );
                    }
                }
            }

            if ( result == -2 )
            {
                LPBYTE ip4 = &connectionData->addr.sin_addr.S_un.S_un_b.s_b1;
                char buffer[128];
                _snprintf( buffer, 127, "failure times limit break[%d.%d.%d.%d]", ip4[0], ip4[1], ip4[2], ip4[3] );
                ErrorHandle( buffer );
            }

            return result;
        }

        // 发送数据完毕后的回调
        void OnSendComplete( unsigned long nbytes_transferred )
        {
            // 用于保证临界区能够在this有效的范围内进行操作！
            reference_ptr this_ptr;

            {
                UGE::CriticalLock::Barrier4ReadWrite barrier( lock4Q );

                assert( reference );                            // 既然是回调，则reference必定是有效的！
                assert( nbytes_transferred <= sending.len );    // 发送完毕的数据必定要小于等于请求发送的大小
                assert( sendQ.size() == 1 );

                // 备份reference，保持引用，否则可能因为ActivateSend而导致this被释放！
                this_ptr = reference;

                // 重要判断，如果错了的话，后面的操作会导致当机！但实际上应该不会出现！！！
                if ( nbytes_transferred > sending.len )
                    throw exception( "nbytes_transferred > sending.len" );

                // 2010-1-18 逻辑修改
                // 因为现在改成了分段发送，所以可能有发送剩下的数据，再次尝试发送！
                if ( sending.send_incompleted( nbytes_transferred ) )
                    sendQ.insert_front( sending );
                else {
                    // 如果确实没有需要发送的数据了，才转回最早以前的逻辑
                    sending.reset( 0 );
                    sendQ.push( sending );
                }

                // 因为上边已经加锁了，所以使用DummyBarrier屏蔽掉ActivateSend内部的锁
                ActivateSend< DummyBarrier >( this_ptr );
            }
        }

        // 外部都直接调用该函数，而不是调用InternalSend，因为需要考虑到调用失败后的reference释放
        // 这里之所以是使用this_ptr对象传递而不是使用引用传递，是因为下边的reset可能导致this被直接释放！！！
        // 因为改成引用的问题，已经出了一次错了，记住！！！this_ptr 就是 this->reference！！！所以不能使用引用！
        template < typename _barrier >
        BOOL ActivateSend( const reference_ptr &this_ptr )
        {
            // 既然是请求启动发送，所以this_ptr必定是有效的对象
            assert( this_ptr );
            assert( this_ptr.get() == this );

            // 根据模版的配置来决定 lock4Q 的锁定方式！
            _barrier barrier( lock4Q );

            // 如果发送失败，则需要将发送对象的reference取消
            switch ( InternalSend( this_ptr ) )
            {
            case SEND_OK:
                return TRUE;

            case SEND_FAIL: 
                // 发送失败时，断开连接，释放引用，失败返回
                // 通过 SD_BOTH 导致连接断开，但不释放资源
                shutdown( connectionData->sock, SD_BOTH );
                // without break

            case SEND_NODATA:
                // 没有数据需要发送时，释放引用，失败返回
                // 将自己的引用计数释放，避免内存泄漏，同时因为上边的this_ptr，所以不用担心this因reset被立即释放！
                // shared_ptr不支持多线程同时写，so。。。如果不加临界区就存在安全问题！
                reference.reset();
                // without break

            // case SEND_PENDING:
                // 如果有数据正在发送，直接失败返回

            default: 
                return FALSE;
            }
        }

    private:
        // 2010-1-18 ： 逻辑修改
        // 为了解决超过0xf000大小的数据包在发送过程中压缩数据大小可能会出现异常，导致DNPHDR无法正确存取数据的问题
        // 现在修改发送逻辑为分段发送 —— 及每次发送的数据上限不超过 MAX_BUFFER_SIZE
        void TryEncoding()
        {
            // 数据包过小的情况下，不做数据压缩
            assert( sending.len <= MAX_BUFFER_SIZE );
            if ( sending.len < 32 )
                return;

            // 该处使用了 TLS ( thread local storage )，避免大量消耗栈空间，同时又准备了足够大的缓存预防溢出
            __declspec( thread ) static BYTE encoderBuffer[ 0x11000 ];
            static const int HEADSIZE = sizeof( DNPHDR ) + 2; // 4字节的包头，2字节的长度

            LPBYTE bufferReference = encoderBuffer;

            // 在内存上做8字节对齐，以提高编码时内存读写效率
            bufferReference = ( LPBYTE )( ( _W64 DWORD )( bufferReference + 16 ) & 0xfffffff8 );
            DNPHDR &newHead	= reinterpret_cast< DNPHDR& >( bufferReference[ -HEADSIZE ] ); 
            newHead.paclen	= lencoder.empty() ? 0xffff : 0;

            lencoder.SetEncoderData( bufferReference, MAX_BUFFER_SIZE );
            if ( lencoder.lzw_encode( sending.buf, sending.len ) && 
                ( lencoder.size() > 0 ) &&
                ( sending.len >= ( lencoder.size() + HEADSIZE ) ) && 
                ( lencoder.size() <= ( MAX_BUFFER_SIZE - 0x100 ) ) )
            {
                assert( lencoder.size() <= ( MAX_BUFFER_SIZE - 0x100 ) );

                // BODY前加2个字节表示，压缩前的大小
                reinterpret_cast< WORD& >( bufferReference[ -2 ] ) = ( WORD )sending.len;
                newHead.seqnum = ( WORD )( lencoder.size() + 2 );

                // 由于是分段发送，所以这个reset的逻辑不能再用了！
                // sending.reset( lencoder.size() + HEADSIZE ); 
                // 现在就直接覆盖sending里边的原始数据
                sending.len = lencoder.size() + HEADSIZE;
                memcpy( sending.buf, &newHead, sending.len ); 
            }
            else
                lencoder.reset();
        }

        SEND_RESULT InternalSend( const reference_ptr &this_ptr )
        {
            assert( ( sendQ.size() >= 1 ) && ( sendQ.size() <= 2 ) );

            // 已经有数据处于发送状态中，不能主动释放引用，而是由发送的回调来启动
            if ( sendQ.size() == 1 )
                return SEND_PENDING;

            // 是否有有效的数据等待发送
            if ( !sendQ.front().send_valid() )
            {
                // 如果没有数据需要发送，则失败返回
                // 现在不需要，也不能在这里启动reset，因为上边的barrier可能因为reset而失效，导致overrun
                // 同时，如果reference有效加上通过了上边的sendQ.size() == 1，
                // 则说明该操作必定是OnSendComplete发起的，所以直接返回FALSE，reset就由外部来做吧

                return connectionData->validate ? SEND_NODATA : SEND_FAIL;
            }

            sending = sendQ.front();
            sendQ.pop();

            // 保证在发送前，reference一定是有效的！
            // 必须在Send之前就保存reference，否则OnSendComplete很可能会先被调用
            if ( !reference )
                reference = this_ptr;

            // 当发送的数据包是第一次发送（排除数据包只发送了一半的状况）时，尝试压缩数据
            //if ( ( sending.raw_buffer == sending.buf ) && connectionData->compress )
            //    TryEncoding();
            // 2010-1-18 逻辑修改
            // 发送前检测，本次发送是为了补齐上次发送未完成的数据，还是重新发送剩余的数据段
            if ( sending.send_prepare() && connectionData->compress )
                TryEncoding();

            // 发送数据，虽然因为有sendQ.size() == 1的保护，这个时候 sending 已经不会被多个线程重用了
            // 但是sendQ却是被临界区保护的，所以还是只能在临界区内使用WSASend！

            if ( SOCKET_ERROR == WSASend( connectionData->sock, &sending, 1, 
                &current_transferred, 0, &olap, NULL ) )
            {
                int eid = WSAGetLastError();
                if ( WSA_IO_PENDING != eid )
                {
                    // ErrorHandle( "send failure", FALSE, eid );
                    // 因为在发送失败后，外部会调用shutdown，所以这里需要将sending清空
                    sending.~Buffer();
                    return SEND_FAIL;
                }
            }

            return SEND_OK;
        }

    public:
        // ###########################
        OVERLAPPED              olap;
        DWORD                   type;
        // ***************************
        DWORD                   current_transferred;
        DWORD                   sequence;
        // ---------------------------
#ifdef USE_QUEUE
        std::queue< Buffer >    sendQ;              // 等待发送的双缓冲[这个队列里就直接保存1个后备缓存数据，所有的缓存数据就直接写入sendQ.front]
#else
        TwinsBuffer             sendQ;              // 等待发送的双缓冲[这个队列里就直接保存1个后备缓存数据，所有的缓存数据就直接写入sendQ.front]
#endif
        Buffer                  sending;            // 正在发送中的数据，当这个数据被发送完毕后，就直接push到队列中，作为后续的后备缓存！
        UGE::CriticalLock       lock4Q;             // sendQ的临界锁！
        // ---------------------------

        // 因为SEND是独立于RECV操作的，RECV负责通知自己在连接管理器中的引用及释放！
        // 但有可能在RECV释放的同时SEND还处于执行状态中！
        // 所以，通过reference来保证SEND的操作对象不会失效！
        // newly ： shared_ptr并不是绝对多先程安全的，需要注意！参见boost document
        reference_ptr                           reference;
        // ---------------------------

        // 该连接的基础数据！const类型，自构造开始，不会被修改，只会在析构时被自动释放！
        // 所以这个数据一定是有效的
        const ConnectionData::reference_ptr     connectionData;

        // 使用LZW在服务端压缩发送数据，客户端只做解压
        lzw::lite_lzwEncoder                    lencoder;
    };
}
