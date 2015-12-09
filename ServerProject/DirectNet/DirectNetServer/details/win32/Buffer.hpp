#pragma once

// 2010-1-18 �� �߼��޸�
// Ϊ��֧�ֶַ����ݷ��ͣ��ֽ�WSABUF::buf���ڷ���ʱ�ֶζ�̬�����ķ��͵�ַ
// �ⲿ��size��Ȼ������Ҫ���͵����ݳ��ȣ���WSABUF::len���ǵ�ǰ����Ҫ���͵����ݳ���
// ʣ�����Ч����������Ϊ Buffer::size - ( WSABUF::buf - Buffer::raw_buffer )

namespace DreamNet_Win32
{
    // �ɶ�̬����������Ӧ�Ļ����������
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

        // ��������ڷ��ͺͽ����ж���ʹ��
        void inflate( size_t new_size, bool discard_raw_data ) 
        {
            LOG( "capacity inflated\r\n" );

            if ( discard_raw_data )
            {
                // ÿ�ΰ����������� 1.5 ������������
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

                // ֻ������Ч�Ĳ���
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

            // �������ţ�
            if ( ( new_size >> 12 ) >= capacity )
            {
                inflate( new_size, true );
            }
            // ��������������
            else if ( ( capacity > 1 ) && ( ( new_size >> 12 ) < ( capacity >> 1 ) ) )
            {
                // ���һ�����ڣ�buffer����Ч����ʹ�������ܴﵽ��ǰcapacity��1/2����ֱ�ӽ���ǰbuffer���ã�
                if ( abs( ( int )timeGetTime() - ( int )last_resize_time ) > 60000 )
                {
                    LOG( "capacity deflate reset\r\n" );

                    // ����Ϊ��Ч���ݵ�4kȡ��
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
            // �������ݰ����� MAX_BUFFER_SIZE
            if ( data_size > MAX_BUFFER_SIZE )
            {
                ErrorHandle( "�����͵ĵ������ݰ�����" );
                LOG( "�����͵ĵ������ݰ�����\r\n" );
                return FALSE;
            }

            // һ��������ݵ���󳤶�Ϊ MAX_BUFF_LIMITED
            // �ڲ��������ֵ֮ǰ�����д��������ݶ��ᱻ�����������ں��������ѹ����
            size_t new_size = ( size + data_size + sizeof( DNPHDR ) );
            if ( new_size > MAX_BUFF_LIMITED )
            {
                // ErrorHandle( "����ռ䳬�����ֵ��" );
                LOG( "����ռ䳬�����ֵ��\r\n" );
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

        // ׼����һ�η��͵����ݣ���Ҫ����䷢���ֶδ�С��
        BOOL send_prepare() 
        {
            if ( send_interrupted() )
                return false;

            assert( remain_size != 0 );
            if ( remain_size == 0 )
                return false; // ����Ѿ����˷������ݵ�ĩβ������Ҫ�ٴη�����

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

        // ��ǰ�鷢�ͳɹ�����ã�ע�⣩
        BOOL send_incompleted( DWORD nbytes_transferred )
        {
            buf += nbytes_transferred;
            len -= nbytes_transferred;

            // ���ֻ���ͳɹ�һ�룬����Ҫ�����ط�
            if ( send_interrupted() )
                return true;

            // �������ʣ�������δ���ͣ�Ҳ��Ҫ�����ط�
            if ( remain_size ) {
                len = ( u_long )remain_size;
                buf = send_ptr;
                return true;
            }

            return false;
        }

        // �Ƿ��͹���δ��ɣ�����ʣ����Ҫ���͵�����
        BOOL send_interrupted() {
            assert( remain_size <= size );
            assert( ( ( _W64 DWORD )send_ptr - ( _W64 DWORD )raw_buffer ) == ( size - remain_size ) );
            return ( len != 0 ) && ( send_ptr != buf );
        }

        // �Ƿ���������Ҫ���ͣ�������ֻ���ͳɹ�һ��������
        BOOL send_valid() {
            return send_interrupted() || ( remain_size != 0 );
        }

        // ����һ���߼������һ��ʱ���buffer����Ч����ʹ����һֱ���ܴﵽ��ǰcapacity��1/2��
        // �����buffer�ڷ�����ɺ���reset��ͬʱ��ɾ�����������������·���һ��������4k����ʱ���棡
        char *raw_buffer;
        size_t size;            // ��Ч�����������ڽ�����˵��size������Ҫ���յ���������С�����ڷ�����˵��size������Ҫ���͵���������С��
        size_t capacity;        // ��Ч�ռ䣡
        char *send_ptr;         // ��δ���͵����ݵ�ַ���������ڷ��ͣ�send_prepare�Ĺؼ����ݣ���
        size_t remain_size;     // ʣ��δ���͵���Ч�������������ڷ��ͣ�����ʣ����Ҫ���͵���������С���ǹؼ���������У�飡��
        DWORD last_resize_time;
    };
}
