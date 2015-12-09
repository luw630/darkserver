#pragma once

#include "Xcpt.h"
#include "Variant.h"
#include "assert.h"

const int ALIGN_BYTE_4 = 2;
const int ALIGN_BYTE_8 = 3;
#define ALIGN( X, bit ) ( ( ( (X - 1) >> bit ) + 1 ) << bit )

namespace lite
{

    class Serializer
    {
    public:
        Serializer() : buffer( NULL ) {}

        // ʹ��һ��ָ����С���ڴ湹�����л�����
        Serializer( void *data, size_t size ) :
            buffer( data )
        {
            if ( buffer )
            {
                if ( size > RPC_MAX_BUFSIZE )
                    throw InvalidBuffer();

                curSize() = 4;
                maxSize() = static_cast<WORD>( size & 0xffff );
            }
        }

        // ����ָ��һ��ռ䣬����ʼ����
        void Assign( void *data, size_t size )
        {
            buffer = data;

            if ( buffer )
            {
                if ( size > RPC_MAX_BUFSIZE )
                    throw InvalidBuffer();

                curSize() = 4;
                maxSize() = static_cast<WORD>( size & 0xffff );
            }
        }

        inline WORD& curSize()
        {
            return static_cast<LPWORD>(buffer)[0];
        }

        inline WORD& maxSize()
        {
            return static_cast<LPWORD>(buffer)[1];
        }

        inline void* curAddr()
        {
            return static_cast<LPBYTE>(buffer) + curSize();
        }

        // ����δʹ�õ��ֽ���
        inline size_t EndEdition()
        {
            assert( buffer );
            assert( curSize() <= maxSize() );

            size_t ret = maxSize() - curSize();
            maxSize() = curSize();

            return ret;
        }

        void Rewind()
        {
            assert( buffer );
            assert( curSize() < maxSize() );

            curSize() = 4;
        }

        void PushVariant( Variant &var )
        {
            assert( buffer );

            if ( ( var.dataType & 0xf0 ) == 0 )
            {
                // 0x00�Σ�����չ��������
                if ( curSize() + 4 >= maxSize() )
                    throw FullBuffer();

                memcpy( curAddr(), &var, 4 );
                curSize() += 4;
            }
            else if ( ( var.dataType & 0xe0 ) == 0 )
            {
                // 0x10�Σ�4�ֽ�pod����
                if ( curSize() + 8 >= maxSize() )
                    throw FullBuffer();

                memcpy( curAddr(), &var, sizeof( Variant ) );
                curSize() += sizeof( Variant );
            }
            else
            {
                // 0x20�Σ�8�ֽ�ָ������
                // 0x40�Σ���������ָ������
                if ( curSize() + 4 + var.dataSize >= maxSize() )
                    throw FullBuffer();

                // ���������ݿ��ܻ�����Ѿ�����ָ��ı�ǣ���ת����֮ǰ����Ҫ��������ȡ����
                ( ( LPDWORD )curAddr() )[0] = ( ( ( LPDWORD )&var )[0] & 0xfffffeff );

                // ����4�ֽڵ����ݶ���ͨ��ָ��ķ�ʽ������ģ����л�ʱ����ָ�벿��ȡ��
                memcpy( &( ( LPDWORD )curAddr() )[1], var._pointer, var.dataSize );

                // 4�ֽڶ���
                curSize() += ALIGN( 4 + var.dataSize, ALIGN_BYTE_4 );
            }
        }

        Serializer & operator () ( Variant var )
        {
            PushVariant( var );

            return *this;
        }

        Serializer & operator () ( void *data, size_t size )
        {
            PushVariant( Variant( data, size ) );

            return *this;
        }

        Serializer & operator [] ( __int32 opType )
        {
            Variant var;
            var.dataType = Variant::VT_RPC_OP;
            var.dataSize = 4;
            var._rpcop = opType;

            PushVariant( var );

            return *this;
        }

    private:
        void *buffer;
    };

    class Serialreader
    {
    public:
        Serialreader() : buffer( NULL ) {}

        // �ָ����л�����ͬʱ�������Ч��
        Serialreader( void *data, size_t size ) :
            buffer( data )
        {
            buffer = static_cast<char *>( data );

            assert( buffer );

            if ( raw_curSize() > maxSize() )
                throw InvalidBuffer();

            if ( maxSize() > size )
                throw InvalidBuffer();

            // ��Ϊͷ4���ֽ������ݳ��ȣ�С��8Ҳ����û�����ݻ������쳣
            if ( maxSize() < 8 )
                throw InvalidBuffer();

            curSize() = 4;
        }

        // ��ָ�����������ָ�Ϊ���л�����״̬��
        // ������EndEdition������ݣ���
        Serialreader( void *data ) :
            buffer( data )
        {
            if ( buffer == NULL )
                throw NullPointer();

            if ( raw_curSize() != maxSize() )
                throw InvalidBuffer();

            // ��Ϊͷ4���ֽ������ݳ��ȣ�С��8Ҳ����û�����ݻ������쳣
            if ( maxSize() < 8 )
                throw InvalidBuffer();

            curSize() = 4;
        }

        // �ָ����л�����ͬʱ�������Ч��
        void Attach( void *data, size_t size )
        {
            buffer = static_cast<char *>( data );

            assert( buffer );

            if ( raw_curSize() > maxSize() )
                throw InvalidBuffer();

            if ( maxSize() > size )
                throw InvalidBuffer();

            // ��Ϊͷ4���ֽ������ݳ��ȣ�С��8Ҳ����û�����ݻ������쳣
            if ( maxSize() < 8 )
                throw InvalidBuffer();

            curSize() = 4;
        }

        inline WORD& curSize()
        {
            return curPos;
        }

        inline WORD raw_curSize()
        {
            return reinterpret_cast< const WORD * >(buffer)[0];
        }

        inline WORD maxSize()
        {
            return reinterpret_cast< const WORD * >(buffer)[1];
        }

        inline const void* curAddr()
        {
            return reinterpret_cast< const BYTE * >(buffer) + curSize();
        }

        void Rewind()
        {
            assert( buffer );
            assert( raw_curSize() < maxSize() );

            curSize() = 4;
        }

        Variant operator () ()
        {
            assert( buffer );

            // ����ط��� sizeof( Variant ) �滻Ϊ 4����Ϊ����������������������͵�ֵ��ֻ��4�ֽڴ�С��
            if ( maxSize() - curSize() < 4/*sizeof( Variant )*/ )
                throw CurrentIsEnd();

            Variant var;
            *(LPDWORD)&var = *(LPDWORD)curAddr();

            if ( ( var.dataType & 0xf0 ) == Variant::VT_EMPTY )
            {
                // ����������
                if ( curSize() + 4 > maxSize() )
                    throw CurrentIsEnd();

                curSize() += 4;
            }
            else if ( ( var.dataType & 0xf0 ) == Variant::VT_DWORD )
            {
                // 4�ֽ�ʵ������
                if ( curSize() + 8 > maxSize() )
                    throw CurrentIsEnd();

                ( (LPDWORD)&var )[1] = ( (LPDWORD)curAddr() )[1];
                curSize() += 8;
            }
            else
            {
                if ( curSize() + 4 + var.dataSize > maxSize() )
                    throw CurrentIsEnd();

                // ָ�����������
                var._pointer = &( (LPDWORD)curAddr() )[1];

                // 4�ֽڶ���
                curSize() += ALIGN( 4 + var.dataSize, ALIGN_BYTE_4 );

				if ( var.dataType == Variant::VT_STRING )
				//if ( IsBadStringPtr( var._string, var.dataSize ) )
				if ( var._string[ var.dataSize - 1 ] != 0 )
					throw BadString();
            }

            return var;
        }

    private:
        const void *buffer;
        WORD curPos;
    };
}
