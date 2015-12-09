#pragma once

// 2010-1-18 ： 逻辑修改
// 为了支持分段数据发送，现将WSABUF::buf用于发送时分段动态递增的发送地址
// 外部的size仍然是总需要发送的数据长度，但WSABUF::len则是当前段需要发送的数据长度
// 剩余的有效发送数据量为 Buffer::size - ( WSABUF::buf - Buffer::raw_buffer )

namespace DreamNet_Win32
{
    // 可动态进行伸缩适应的缓存对象类型
    struct Buffer : public WSABUF
    {
        Buffer( size_t init_size ) 
        { 
            capacity = ( init_size >> 12 ) + 1;
            buf = send_ptr = raw_buffer = new char[ capacity << 12 ];
            last_resize_time = timeGetTime();

            size = 0;
            len = 0; 
            remain_size = 0;
        }

        Buffer() 
        { 
            capacity = 0;
            size = 0;
            raw_buffer = NULL;
            send_ptr = NULL;
            last_resize_time = 0;

            buf = 0; 
            len = 0; 
            remain_size = 0;
        }

        Buffer( const Buffer &other ) 
        { 
            memcpy( this, &other, sizeof( Buffer ) );
            memset( const_cast< Buffer* >( &other ), 0, sizeof( Buffer ) );
        }

        ~Buffer() 
        { 
            if ( raw_buffer != NULL ) 
                delete [] raw_buffer; 

            capacity = size = 0;
            raw_buffer = NULL;
            send_ptr = NULL;
            last_resize_time = 0;

            buf = 0, len = 0; 
            remain_size = 0;
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

                delete [] raw_buffer;
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
                delete [] raw_buffer;

                raw_buffer = new_buffer;
                capacity = new_capacity;
            }

            last_resize_time = timeGetTime();
            send_ptr = buf = raw_buffer;
            remain_size = size;
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
                // 如果一分钟内，buffer的有效数据使用量不能达到当前capacity的1/2，则直接将当前buffer重置！
                if ( abs( ( int )timeGetTime() - ( int )last_resize_time ) > 60000 )
                {
                    LOG( "capacity deflate reset\r\n" );

                    // 缩减为有效数据的4k取整
                    capacity = ( new_size >> 12 ) + 1;
                    last_resize_time = timeGetTime();

                    delete [] raw_buffer;
                    raw_buffer = new char[ capacity << 12 ];
                }
            }

            send_ptr = buf = raw_buffer;
            remain_size = len = static_cast< u_long >( size = new_size );
        }

        BOOL push( const void *data, size_t data_size, DWORD sequence )
        {
            // 单个数据包大于 MAX_BUFFER_SIZE
            if ( data_size > MAX_BUFFER_SIZE )
            {
                ErrorHandle( "请求发送的单个数据包过大！" );
                LOG( "请求发送的单个数据包过大！\r\n" );
                return FALSE;
            }

            // 一个打包数据的最大长度为 MAX_BUFF_LIMITED
            // 在不超过这个值之前，所有待发送数据都会被缓存下来用于后面的数据压缩！
            size_t new_size = ( size + data_size + sizeof( DNPHDR ) );
            if ( new_size > MAX_BUFF_LIMITED )
            {
                // ErrorHandle( "打包空间超过最大值！" );
                LOG( "打包空间超过最大值！\r\n" );
                SpecialLog( raw_buffer, size, data, data_size, sequence );
                return FALSE;
            }

            if ( capacity <= ( new_size >> 12 ) )
                inflate( new_size, false );

            reinterpret_cast< DNPHDR* >( &raw_buffer[ size ] )->seqnum = ( WORD )sequence;
            reinterpret_cast< DNPHDR* >( &raw_buffer[ size ] )->paclen = ( UINT16 )data_size;
            size += sizeof( DNPHDR );

            memcpy( &raw_buffer[ size ], data, data_size );
            size += static_cast< u_long >( data_size );

            remain_size = len = static_cast< u_long >( size );
            assert( raw_buffer == buf );

            return TRUE;
        }

        // 准备下一次发送的数据，主要是填充发送字段大小！
        BOOL send_prepare() 
        {
            if ( send_interrupted() )
                return false;

            assert( remain_size != 0 );
            if ( remain_size == 0 )
                return false; // 如果已经到了发送数据的末尾，则不需要再次发送了

            u_long sendSize = 0;
            char *segPtr = send_ptr;
            char *endPtr = raw_buffer + size;
            while ( send_ptr < endPtr ) {
                if ( ( sendSize + reinterpret_cast< DNPHDR* >( send_ptr )->paclen + sizeof( DNPHDR ) ) > MAX_BUFFER_SIZE )
                    break;
                sendSize += reinterpret_cast< DNPHDR* >( send_ptr )->paclen + sizeof( DNPHDR );
                send_ptr += reinterpret_cast< DNPHDR* >( send_ptr )->paclen + sizeof( DNPHDR );
            }

            assert( send_ptr <= endPtr );
            assert( ( segPtr + sendSize ) == send_ptr );
            remain_size -= sendSize;
            len = sendSize;
            buf = segPtr;
            return true;
        }

        // 当前块发送成功后调用（注意）
        BOOL send_incompleted( DWORD nbytes_transferred )
        {
            buf += nbytes_transferred;
            len -= nbytes_transferred;

            // 如果只发送成功一半，则需要继续重发
            if ( send_interrupted() )
                return true;

            // 如果还有剩余的数据未发送，也需要继续重发
            if ( remain_size ) {
                len = ( u_long )remain_size;
                buf = send_ptr;
                return true;
            }

            return false;
        }

        // 是否发送过程未完成，还有剩余需要发送的数据
        BOOL send_interrupted() {
            assert( remain_size <= size );
            assert( ( ( _W64 DWORD )send_ptr - ( _W64 DWORD )raw_buffer ) == ( size - remain_size ) );
            return ( len != 0 ) && ( send_ptr != buf );
        }

        // 是否有数据需要发送（包括了只发送成功一半的情况）
        BOOL send_valid() {
            return send_interrupted() || ( remain_size != 0 );
        }

        // 增加一个逻辑，如果一定时间后，buffer的有效数据使用量一直不能达到当前capacity的1/2，
        // 则这个buffer在发送完成后做reset的同时，删除所有数据量，重新分配一个不低于4k的临时缓存！
        char *raw_buffer;
        size_t size;            // 有效数据量（对于接收来说，size是总需要接收的数据量大小，对于发送来说，size是总需要发送的数据量大小）
        size_t capacity;        // 有效空间！
        char *send_ptr;         // 还未发送的数据地址！（仅用于发送，send_prepare的关键数据！）
        size_t remain_size;     // 剩余未发送的有效数据量（仅用于发送，是总剩余需要发送的数据量大小，非关键，但用于校验！）
        DWORD last_resize_time;
    };
}
