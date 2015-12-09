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

        // 使用一段指定大小的内存构建序列化区域
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

        // 重新指定一块空间，并初始化！
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

        // 返回未使用的字节数
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
                // 0x00段，无扩展数据类型
                if ( curSize() + 4 >= maxSize() )
                    throw FullBuffer();

                memcpy( curAddr(), &var, 4 );
                curSize() += 4;
            }
            else if ( ( var.dataType & 0xe0 ) == 0 )
            {
                // 0x10段，4字节pod类型
                if ( curSize() + 8 >= maxSize() )
                    throw FullBuffer();

                memcpy( curAddr(), &var, sizeof( Variant ) );
                curSize() += sizeof( Variant );
            }
            else
            {
                // 0x20段，8字节指针类型
                // 0x40段，不定长度指针类型
                if ( curSize() + 4 + var.dataSize >= maxSize() )
                    throw FullBuffer();

                // 这类型数据可能会存在已经分配指针的标记，在转数据之前，需要将这个标记取消！
                ( ( LPDWORD )curAddr() )[0] = ( ( ( LPDWORD )&var )[0] & 0xfffffeff );

                // 大于4字节的数据都是通过指针的方式来保存的，序列化时将其指针部分取消
                memcpy( &( ( LPDWORD )curAddr() )[1], var._pointer, var.dataSize );

                // 4字节对齐
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

        // 恢复序列化区域，同时检查其有效性
        Serialreader( void *data, size_t size ) :
            buffer( data )
        {
            buffer = static_cast<char *>( data );

            assert( buffer );

            if ( raw_curSize() > maxSize() )
                throw InvalidBuffer();

            if ( maxSize() > size )
                throw InvalidBuffer();

            // 因为头4个字节是数据长度，小于8也代表没有数据或数据异常
            if ( maxSize() < 8 )
                throw InvalidBuffer();

            curSize() = 4;
        }

        // 将指定的数据流恢复为序列化管理状态，
        // 必须是EndEdition后的数据！！
        Serialreader( void *data ) :
            buffer( data )
        {
            if ( buffer == NULL )
                throw NullPointer();

            if ( raw_curSize() != maxSize() )
                throw InvalidBuffer();

            // 因为头4个字节是数据长度，小于8也代表没有数据或数据异常
            if ( maxSize() < 8 )
                throw InvalidBuffer();

            curSize() = 4;
        }

        // 恢复序列化区域，同时检查其有效性
        void Attach( void *data, size_t size )
        {
            buffer = static_cast<char *>( data );

            assert( buffer );

            if ( raw_curSize() > maxSize() )
                throw InvalidBuffer();

            if ( maxSize() > size )
                throw InvalidBuffer();

            // 因为头4个字节是数据长度，小于8也代表没有数据或数据异常
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

            // 这个地方的 sizeof( Variant ) 替换为 4，因为可能这个数据是无数据类型的值，只有4字节大小！
            if ( maxSize() - curSize() < 4/*sizeof( Variant )*/ )
                throw CurrentIsEnd();

            Variant var;
            *(LPDWORD)&var = *(LPDWORD)curAddr();

            if ( ( var.dataType & 0xf0 ) == Variant::VT_EMPTY )
            {
                // 无数据类型
                if ( curSize() + 4 > maxSize() )
                    throw CurrentIsEnd();

                curSize() += 4;
            }
            else if ( ( var.dataType & 0xf0 ) == Variant::VT_DWORD )
            {
                // 4字节实数类型
                if ( curSize() + 8 > maxSize() )
                    throw CurrentIsEnd();

                ( (LPDWORD)&var )[1] = ( (LPDWORD)curAddr() )[1];
                curSize() += 8;
            }
            else
            {
                if ( curSize() + 4 + var.dataSize > maxSize() )
                    throw CurrentIsEnd();

                // 指针带数据类型
                var._pointer = &( (LPDWORD)curAddr() )[1];

                // 4字节对齐
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
