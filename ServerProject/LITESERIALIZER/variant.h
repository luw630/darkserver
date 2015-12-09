#pragma once

//#include <atlstr.h>
#include <comutil.h>
#include <assert.h>

#define RPC_MAX_BUFSIZE  0xffff
#define CHAR_BUFSIZE 1024

namespace lite
{
    struct DecimalConvert : public DECIMAL
    {
	    DecimalConvert( const unsigned __int64 value )
	    {
		    Hi32 = 0;
		    scale = 0;
		    sign = ( value & 0x8000000000000000 ) != 0;
            Lo64 = value & 0x7fffffffffffffff;
	    }
    };

    class Variant
    {
    public:
        // 变量类型！
        enum VARIANT_TYPE
        {
            VT_EMPTY        = 0,        // 完全的空类型，变量未初始化
            VT_NULL         = 1,        // 空类型，表示没有数据

            VT_RPC_OP_LITE  = 4,        // 特殊控制字符，轻量级版本，但为了向下兼容，以前的VT_RPC_OP也保留

            VT_WORD         = 8,        // [EX]2字节无符号数据
            VT_SHORTINTEGER = 9,        // [EX]16位带符号整数

            VT_U32_24       = 0x0a,     // [EX] 原本是UINT32类型，数据压缩后变为24位数据
            VT_I32_24       = 0x0b,     // [EX] 原本是 INT32类型，数据压缩后变为24位数据
            VT_U64_24       = 0x0c,     // [EX] 原本是UINT64类型，数据压缩后变为24位数据
            VT_I64_24       = 0x0d,     // [EX] 原本是 INT64类型，数据压缩后变为24位数据

            VT_DWORD        = 0x10,     // 4字节无符号数据
            VT_INTEGER      = 0x11,     // 32位带符号整数
            VT_FLOAT        = 0x12,     // 单精度浮点数

            VT_U64_56       = 0x1a,     // [EX] 原本是UINT64类型，数据压缩后变为56位数据
            VT_I64_56       = 0x1b,     // [EX] 原本是 INT64类型，数据压缩后变为56位数据

            VT_RPC_OP       = 0x1f,     // 特殊控制字符

            VT_QWORD        = 0x20,     // 8字节无符号数据
            VT_LARGINTEGER  = 0x21,     // 64位带符号大整数
            VT_DOUBLE       = 0x22,     // 双精度浮点数
            VT_DATE64       = 0x23,     // 64位日期

            VT_POINTER      = 0x40,     // 指针类型数据
            VT_STRING       = 0x41,     // 标准字符串
            VT_BSTRING      = 0x42,     // BSTR字符串
            VT_UTF8         = 0x43,     // UTF8字符串
        };

        // 标记类型
        enum FLAG_TYPE
        {
            FT_ALLOCATED_MEMBLOCK   = 0x1,      // 标志该变量是分配出来的数据，需要释放

            // 安全数组，在该模式下，dataType指示了数组的元素类型，dataSize为整个数组的有效数据大小。。。
            // 只对 WORD SHORT DWORD INT FLOAT QWORD LARG DOUBLE DATE64 起效果！
            // FT_SAFEARRAY            = 0x2       // 标志该变量是一个安全数组类型
        };

	public:
		~Variant()
		{
            // 到目前为止, flags 只有前2位bit使用到了的, 所以其他位不应该有值!
            assert( ( dataType < VT_QWORD ) || !( flags & 0xfc ) );

	        if ( ( dataType >= VT_QWORD ) && ( flags & FT_ALLOCATED_MEMBLOCK ) )
		        free( ( void* )_pointer );
		}

    public:
        Variant()
        {
            flags    = 0;
            dataType = VT_EMPTY;
            dataSize = 0;
            _pointer = NULL;
        }

        Variant( const void *data, size_t size )
        {
            // [限制]不支持65535以上的数据块！
            if ( size > RPC_MAX_BUFSIZE )
                throw BadDataSize();

            flags    = 0;
            dataType = VT_POINTER; 
            dataSize = static_cast< WORD >( size );
            _pointer = data; 
        }

        /* 目前还没有考虑好数组的应用方式,,, 所以这部分功能先放置起来...
        Variant( VARIANT_TYPE type, DWORD elements, const void *data )
        {
            if ( type <= VT_RPC_OP_LITE || type >= VT_POINTER )
                throw BadArrayType();

            // 计算元素大小，最小为WORD 2字节，通过判断高位，可扩展出1个DWORD和2个DWORD
            DWORD elementSize = 2 * ( ( type >> 4 ) * 2 );

            // [限制]不支持65535以上的数据块！
            if ( elementSize * elements > RPC_MAX_BUFSIZE )
                throw BadDataSize();

            // 检测数组的有效性！为了性能，只检测第一个和最后一个
            if ( IsBadReadPtr( data, elementSize ) || 
                IsBadReadPtr( ( LPBYTE )data + elementSize * ( elements - 1 ), elementSize ) )
                throw InvalidBuffer();

            flags    = FT_SAFEARRAY;
            dataType = type; 
            dataSize = elementSize * elements;
            _pointer = data; 
        }
        */
    public:
		
	#pragma push_macro( "new" )
	#undef new

        Variant( BYTE data ) { new ( this ) Variant( static_cast< WORD >( data ) ); }
        Variant( WORD data )
        { 
            ( flags = 0 ), ( dataType = VT_WORD ), ( dataSize = data ), ( _dword = 0 );
        } 

        Variant( char data ) { new ( this ) Variant( static_cast< __int16 >( data ) ); }
        Variant( __int16 data )
        { 
            ( flags = 0 ), ( dataType = VT_SHORTINTEGER ), ( dataSize = *( LPWORD )&data ), ( _dword = 0 );
        } 

        Variant( float data )
        { 
            ( flags = 0 ), ( dataType = VT_FLOAT ), ( dataSize = 0 ), ( _dword = *( unsigned long* )&data );
        } 

        Variant( const double &data )
        { 
            ( flags = 0 ), ( dataType = VT_DOUBLE ), ( dataSize = 8 ), ( _pointer = &data );
        } 

        static const unsigned long _UBIT_M32 = 0xff000000;
        static const unsigned long _IBIT_M32 = 0xff800000;
        static const unsigned __int64 _UBIT_M64 = 0xffffffffff000000;
        static const unsigned __int64 _IBIT_M64 = 0xffffffffff800000;

        Variant( unsigned __int32 data ) { new ( this ) Variant( static_cast< unsigned long >( data ) ); }
        Variant( unsigned long data )
        { 
            if ( data & _UBIT_M32 ) // 测试是否可以作数据压缩？
            {
                ( dataSize = 0 ), ( _dword = *( unsigned long* )&data ), ( dataType = VT_DWORD ), ( flags = 0 ); 
            }
            else 
            {
                // 将dataType直接赋值为一个调整后的32位数据（只有低24位有效），然后再覆盖dataType部分的1个BYTE
                ( reinterpret_cast< unsigned long& >( dataType ) = ( data << 8 ) );
                ( _dword = 0 ), ( dataType = VT_U32_24 );
            }
        } 

        Variant( long data ) { new ( this ) Variant( static_cast< long >( data ) ); }
        Variant( __int32 data )
        { 
            // 注意，测试常量里包含了最高位的一个符号位。。。
            if ( ( _IBIT_M32 & *( unsigned long* )&data ) && ( ( _IBIT_M32 & *( unsigned long* )&data ) != _IBIT_M32 ) )
            {
                ( dataSize = 0 ), ( _dword = *( unsigned long* )&data ), ( dataType = VT_INTEGER ), ( flags = 0 ); 
            }
            else 
            {
                // 同DWORD的处理，但注意，有效部分的最高位为符号位
                ( reinterpret_cast< unsigned long& >( dataType ) = ( ( ( unsigned long )data ) << 8 ) );
                ( _dword = 0 ), ( dataType = VT_I32_24 );
            }
        } 

        Variant( const unsigned __int64 &data )
        { 
#define __LPDW ( ( unsigned long* )&data )
            // 测试是否可以作数据压缩？
            if ( _UBIT_M32 & __LPDW[1] ) 
            {
                // 最高8位是否有值？ 如果有就只能当64bit来用了。。。
                ( dataSize = 8 ), ( _pointer = &data ), ( dataType = VT_QWORD ), ( flags = 0 ); 
            }
            else if ( _UBIT_M64 & data ) 
            {
                // 这个是测试是否能转为24bit，如果有值，就只能转为56bit。。。
                ( reinterpret_cast< unsigned __int64& >( dataType ) = ( data << 8 ) ), ( dataType = VT_U64_56 );
            }
            else 
            {
                // 剩余的就是直接转到24bit
                ( reinterpret_cast< unsigned long& >( dataType ) = ( __LPDW[0] << 8 ) );
                ( _dword = 0 ), ( dataType = VT_U64_24 );
            }
#undef __LPDW
        } 

        Variant( const __int64 &data )
        { 
#define __LPDW ( ( unsigned long* )&data )
            if ( ( ( _IBIT_M32 & __LPDW[1] ) != 0 ) && ( ( _IBIT_M32 & __LPDW[1] ) != _IBIT_M32 ) )
            {
                ( dataSize = 8 ), ( _pointer = &data ), ( dataType = VT_LARGINTEGER ), ( flags = 0 ); 
            }
            else if ( ( _IBIT_M64 & data ) && ( ( _IBIT_M64 & data ) != _IBIT_M64 ) )
            {
                ( reinterpret_cast< unsigned __int64& >( dataType ) = ( data << 8 ) ), ( dataType = VT_I64_56 );
            }
            else
            {
                ( reinterpret_cast< unsigned long& >( dataType ) = ( __LPDW[0] << 8 ) );
                ( _dword = 0 ), ( dataType = VT_I64_24 );
            }
#undef __LPDW
        } 

        Variant( const char *data )
        {
            // [限制]不支持1024以上的字符串！
            if ( IsBadStringPtr( data, CHAR_BUFSIZE ) )
                throw BadString();

            flags    = 0; 
            dataType = VT_STRING;
            dataSize = static_cast< WORD >( strlen( data ) + 1 );
            _string = data;
        }

        Variant( const _variant_t &variant, BOOL deepCopy = FALSE )
        {
            switch ( variant.vt )
            {
            case VT_EMPTY:
            case VT_NULL:
                new ( this ) Variant();
                dataType = ( BYTE )variant.vt;
                return;

            case VT_UI4:
            case VT_UINT:
                new ( this ) Variant( static_cast< unsigned long >( variant.ulVal ) );
                return;

            case VT_I4:
            case VT_INT:
                new ( this ) Variant( static_cast< __int32 >( variant.lVal ) );
                return;

            case VT_R4:
                new ( this ) Variant( static_cast< float >( variant.fltVal ) );
                return;

            case VT_I8:
                new ( this ) Variant( static_cast< const __int64 >( variant.llVal ) );
                break;

            case VT_UI8:
                new ( this ) Variant( static_cast< const unsigned __int64 >( variant.ullVal ) );
                break;

            case VT_R8:
                new ( this ) Variant( static_cast< const double >( variant.dblVal ) );
                break;

			case VT_DATE:
                new ( this ) Variant( static_cast< const double >( variant.date ) );
                dataType = VT_DATE64;
				break;

			case ( VT_ARRAY | VT_UI1 ):
                new ( this ) Variant( variant.parray->pvData, variant.parray->rgsabound[0].cElements );
				break;

			case VT_BSTR:
				{
					_bstr_t bstr = variant;
					LPCSTR str = ( LPCSTR )bstr;

			        // [限制]不支持1024以上的字符串！
			        if ( IsBadStringPtr( str, CHAR_BUFSIZE ) )
				        throw BadString();

                    AllocateAndAssign( VT_STRING, str, strlen( str ) + 1 );
				}
				return; // 由于BSTR临时转换到char*，所以这里必定是作深拷贝，直接返回

            // use this convertion to avoid sqlserver-convert-issue with bigint <-> decimal
            case VT_DECIMAL:
                {
                    // variant.decVal.sign  = 符号位，只有最高位有效
                    // variant.decVal.scale = 定点小数位
                    // variant.decVal.Hi32  = 独立于后续  64bit 的 92bit 有效位中的  32bit 高位部分
                    // variant.decVal.Lo64  = 独立于前    32bit 的 92bit 有效位中的  64bit 底位部分

                    if ( variant.decVal.Hi32 != 0 )
                        // 考虑到于运算复杂度的问题，不支持大于64位的数值！
                        throw InvalidConvert();

                    __int64 temp64 = ( variant.decVal.sign == 0x80 ) 
                        ? -( __int64 )( variant.decVal.Lo64 & 0x7fffffffffffffff )
                        : ( __int64 )( variant.decVal.Lo64 & 0x7fffffffffffffff );

                    new ( this ) Variant( temp64 );

                    // 如果数据量无法直接写入4个字节内，则需要扩展空间！
                    if ( dataType >= VT_QWORD )
                        AllocateAndAssign( VT_LARGINTEGER, &temp64, 8 );
                }
				return; // 因为这里要么是简化后直接赋值，要么必定是深拷贝，所以也直接返回

            default:
                throw InvalidConvert();
            }

            if ( deepCopy && ( dataType >= VT_QWORD ) ) // 如果做深拷贝,则需要重新分配数据并拷贝!
                AllocateAndAssign( ( VARIANT_TYPE )dataType, _pointer, dataSize );
        }

#pragma pop_macro( "new" )

	protected:
		void AllocateAndAssign( VARIANT_TYPE type, const void *data, size_t size )
		{
            if ( size > RPC_MAX_BUFSIZE )
                throw BadDataSize();

            if ( type < VT_QWORD )
                throw InvalidConvert();

            flags    = FT_ALLOCATED_MEMBLOCK;
			dataType = static_cast< BYTE >( type );
			dataSize = static_cast< WORD >( size );
            _pointer = malloc( dataSize );
			memcpy( ( void* )_pointer, data, dataSize );
		}

    public:
        // 浅拷贝，直接使用外来的数据，包括内存指针
		Variant ( const Variant &variant )
		{
            // 这个地方需要清空内存分配标志
            ( ( LPDWORD )this )[0] = ( variant.dataType >= VT_QWORD ) ? 
				( ( ( LPDWORD )&variant )[0] & 0xfffffeff ) : ( ( LPDWORD )&variant )[0];
            //dataType = variant.dataType;
            //dataSize = variant.dataSize;
            //flags = variant.flags & 0xe; 
            _dword = variant._dword;
		}

        // 深拷贝，用在需要进行持久化的场合
		void DeepCopy( const Variant &variant )
        {
            // 先检测自己是否有释放标记？ 否则会形成资源泄露。。。
	        if ( flags & FT_ALLOCATED_MEMBLOCK )
		        free( ( void* )_pointer );

            // 如果是非引用类资源，则可以直接赋值
            if ( variant.dataType < VT_QWORD )
            {
                *this = variant;
            }
            // 除非引用类资源外，就是引用类资源了，既然需要进行数据维持，
            // 就只能分配一块新的空间来做数据的强制拷贝，避免引用指针被意外释放。。。
            else 
            {
                AllocateAndAssign( ( VARIANT_TYPE )variant.dataType, variant._pointer, variant.dataSize );
            }
        }

        operator unsigned __int32 () const { return operator unsigned long (); }
        operator unsigned long () const
        {
            switch ( dataType )
            {
            case VT_DWORD:
                return _dword;

            case VT_WORD:
                return _word;

            case VT_U32_24:
                return reinterpret_cast< const unsigned long& >( dataType ) >> 8;

            default:
                throw InvalidConvert();
            }
        }

        operator long () const { return operator __int32 (); }
        operator __int32 () const
        {
            switch ( dataType )
            {
            case VT_INTEGER:
                return _integer;

            case VT_WORD:
                return _word;

            case VT_SHORTINTEGER:
                return _shortinteger;

            case VT_I32_24:
                return ( _shortinteger < 0 ) // 根据有效数据部分的最高位（符号位）进行不同处理
                    ? ( ( reinterpret_cast< const unsigned long& >( dataType ) >> 8 ) | 0xff000000 )
                    : ( reinterpret_cast< const unsigned long& >( dataType ) >> 8 );

            default:
                throw InvalidConvert();
            }
        }

        operator unsigned __int64 () const
        {
            switch ( dataType )
            {
            case VT_QWORD:
                return *( unsigned __int64* )_pointer;

            case VT_DWORD:
                return _dword;

            case VT_WORD:
                return _word;

            case VT_U32_24:
            case VT_U64_24:
                return reinterpret_cast< const unsigned long& >( dataType ) >> 8;

            case VT_U64_56:
                return reinterpret_cast< const unsigned __int64& >( dataType ) >> 8;

            default:
                throw InvalidConvert();
            }
        }

        operator __int64 () const
        {
            switch ( dataType )
            {
            case VT_LARGINTEGER:
                return *( __int64* )_pointer;

            case VT_DWORD:
                return _dword;

            case VT_INTEGER:
                return _integer;

            case VT_WORD:
                return _word;

            case VT_SHORTINTEGER:
                return _shortinteger;

            case VT_I32_24:
            case VT_I64_24:
                return ( _shortinteger < 0 ) // 根据有效数据部分的最高位（符号位）进行不同处理
                    ? ( ( reinterpret_cast< const unsigned long& >( dataType ) >> 8 ) | 0xffffffffff000000 )
                    : ( reinterpret_cast< const unsigned long& >( dataType ) >> 8 );

            case VT_I64_56:
                return ( _integer < 0 ) // 根据有效数据部分的最高位（符号位）进行不同处理
                    ? ( ( reinterpret_cast< const unsigned __int64& >( dataType ) >> 8 ) | 0xff00000000000000 )
                    : ( reinterpret_cast< const unsigned __int64& >( dataType ) >> 8 );

            default:
                throw InvalidConvert();
            }
        }

        operator unsigned __int16 () const
        {
            if ( dataType != VT_WORD )
                throw InvalidConvert();

            return _word;
        }

        operator __int16 () const
        {
            if ( dataType != VT_SHORTINTEGER )
                throw InvalidConvert();

            return _shortinteger;
        }

        operator float () const
        {
            if ( dataType != VT_FLOAT )
                throw InvalidConvert();

            return _float;
        }

        operator double () const
        {
            if ( dataType != VT_DOUBLE )
                throw InvalidConvert();

            return *(double *)_pointer;
        }

        operator const void * () const
        {
            if ( dataType != VT_POINTER )
                throw InvalidConvert();

            return _pointer;
        }

        operator LPCSTR () const
        {
            if ( dataType == VT_NULL )
                return NULL;

            if ( dataType != VT_STRING )
                throw InvalidConvert();

            return _string;
        }

        operator _bstr_t () const
        {
            if ( dataType == VT_STRING )
                return _string;

            if ( dataType != VT_BSTRING )
                throw InvalidConvert();
                
            return _bstr;
        }

        operator const _variant_t () const
        {
            _variant_t ret;

            switch ( dataType )
            {
            case VT_STRING:
                ret = _string;
                break;

            case VT_BSTRING:
                ret = _bstr;
                break;

            case VT_DWORD:
            case VT_WORD:
            case VT_U32_24:
                ret = operator unsigned long ();
                break;

            case VT_INTEGER:
            case VT_SHORTINTEGER:
            case VT_I32_24:
                ret = operator __int32 ();
                break;

            case VT_QWORD:
            case VT_U64_24:
            case VT_U64_56:
                ret = DecimalConvert( operator unsigned __int64 () );
                break;

            case VT_LARGINTEGER:
            case VT_I64_24:
            case VT_I64_56:
                ret = DecimalConvert( ( unsigned __int64 ) operator __int64 () );
                break;

            case VT_FLOAT:
                ret = _float;
                break;

            default:
                throw InvalidConvert();
            }

            return ret;
        }

        //****************************************************************************
        // 系统标准API::IsBadStringPtr只是判断了指定字符串在指定长度内的读有效性…… 
        // 这和我们的初衷不一致——至少需要保证该字符串是完整的[null terminate]！！！
        // 所以自己来写了一个……
        //**************************************************************************** 
        inline BOOL IsBadStringPtr( LPCSTR string, unsigned long size ) throw()
        {
            try
            {
                while ( *string != 0 )
                {
                    size--;
                    string++;

                    // 如果实际长度[+'\0']超过默认长度，则检测失败
                    if ( size == 0  )
                        return true;
                }

                return false;
            }
            catch ( ... )
            {
                // 如果检测过程中出现异常，则失败
            }

            return true;
        }

    public:
        BYTE dataType;  // 当前数据对象的数据类型
        BYTE flags;     // 如果数据类型是在0x20段之前，则数据必定不会做深拷贝，那么flags可用作数据空间 

        union 
        {
            // 这个数据不负责数据对齐，记录的是实际有效的数据大小
            WORD                dataSize;

            // 这两个类型由于数据量足够小，所以直接保存到dataSize里边
            unsigned __int16    _word;
            __int16             _shortinteger;
        };

        union 
        {
            __int32             _rpcop;

            unsigned long       _dword;
            __int32             _integer;
            float               _float;

            // 64位大数据类型不并入联合中，否则会造成结构膨胀到12字节
            unsigned __int64    const * _qword;
            __int64             const * _larginteger;
            double              const * _double;

            void                const * _pointer;
            char                const * _string;
            BSTR                _bstr;
            char                const * _utf8;
       };
    };

}
