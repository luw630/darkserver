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
        // �������ͣ�
        enum VARIANT_TYPE
        {
            VT_EMPTY        = 0,        // ��ȫ�Ŀ����ͣ�����δ��ʼ��
            VT_NULL         = 1,        // �����ͣ���ʾû������

            VT_RPC_OP_LITE  = 4,        // ��������ַ����������汾����Ϊ�����¼��ݣ���ǰ��VT_RPC_OPҲ����

            VT_WORD         = 8,        // [EX]2�ֽ��޷�������
            VT_SHORTINTEGER = 9,        // [EX]16λ����������

            VT_U32_24       = 0x0a,     // [EX] ԭ����UINT32���ͣ�����ѹ�����Ϊ24λ����
            VT_I32_24       = 0x0b,     // [EX] ԭ���� INT32���ͣ�����ѹ�����Ϊ24λ����
            VT_U64_24       = 0x0c,     // [EX] ԭ����UINT64���ͣ�����ѹ�����Ϊ24λ����
            VT_I64_24       = 0x0d,     // [EX] ԭ���� INT64���ͣ�����ѹ�����Ϊ24λ����

            VT_DWORD        = 0x10,     // 4�ֽ��޷�������
            VT_INTEGER      = 0x11,     // 32λ����������
            VT_FLOAT        = 0x12,     // �����ȸ�����

            VT_U64_56       = 0x1a,     // [EX] ԭ����UINT64���ͣ�����ѹ�����Ϊ56λ����
            VT_I64_56       = 0x1b,     // [EX] ԭ���� INT64���ͣ�����ѹ�����Ϊ56λ����

            VT_RPC_OP       = 0x1f,     // ��������ַ�

            VT_QWORD        = 0x20,     // 8�ֽ��޷�������
            VT_LARGINTEGER  = 0x21,     // 64λ�����Ŵ�����
            VT_DOUBLE       = 0x22,     // ˫���ȸ�����
            VT_DATE64       = 0x23,     // 64λ����

            VT_POINTER      = 0x40,     // ָ����������
            VT_STRING       = 0x41,     // ��׼�ַ���
            VT_BSTRING      = 0x42,     // BSTR�ַ���
            VT_UTF8         = 0x43,     // UTF8�ַ���
        };

        // �������
        enum FLAG_TYPE
        {
            FT_ALLOCATED_MEMBLOCK   = 0x1,      // ��־�ñ����Ƿ�����������ݣ���Ҫ�ͷ�

            // ��ȫ���飬�ڸ�ģʽ�£�dataTypeָʾ�������Ԫ�����ͣ�dataSizeΪ�����������Ч���ݴ�С������
            // ֻ�� WORD SHORT DWORD INT FLOAT QWORD LARG DOUBLE DATE64 ��Ч����
            // FT_SAFEARRAY            = 0x2       // ��־�ñ�����һ����ȫ��������
        };

	public:
		~Variant()
		{
            // ��ĿǰΪֹ, flags ֻ��ǰ2λbitʹ�õ��˵�, ��������λ��Ӧ����ֵ!
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
            // [����]��֧��65535���ϵ����ݿ飡
            if ( size > RPC_MAX_BUFSIZE )
                throw BadDataSize();

            flags    = 0;
            dataType = VT_POINTER; 
            dataSize = static_cast< WORD >( size );
            _pointer = data; 
        }

        /* Ŀǰ��û�п��Ǻ������Ӧ�÷�ʽ,,, �����ⲿ�ֹ����ȷ�������...
        Variant( VARIANT_TYPE type, DWORD elements, const void *data )
        {
            if ( type <= VT_RPC_OP_LITE || type >= VT_POINTER )
                throw BadArrayType();

            // ����Ԫ�ش�С����СΪWORD 2�ֽڣ�ͨ���жϸ�λ������չ��1��DWORD��2��DWORD
            DWORD elementSize = 2 * ( ( type >> 4 ) * 2 );

            // [����]��֧��65535���ϵ����ݿ飡
            if ( elementSize * elements > RPC_MAX_BUFSIZE )
                throw BadDataSize();

            // ����������Ч�ԣ�Ϊ�����ܣ�ֻ����һ�������һ��
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
            if ( data & _UBIT_M32 ) // �����Ƿ����������ѹ����
            {
                ( dataSize = 0 ), ( _dword = *( unsigned long* )&data ), ( dataType = VT_DWORD ), ( flags = 0 ); 
            }
            else 
            {
                // ��dataTypeֱ�Ӹ�ֵΪһ���������32λ���ݣ�ֻ�е�24λ��Ч����Ȼ���ٸ���dataType���ֵ�1��BYTE
                ( reinterpret_cast< unsigned long& >( dataType ) = ( data << 8 ) );
                ( _dword = 0 ), ( dataType = VT_U32_24 );
            }
        } 

        Variant( long data ) { new ( this ) Variant( static_cast< long >( data ) ); }
        Variant( __int32 data )
        { 
            // ע�⣬���Գ�������������λ��һ������λ������
            if ( ( _IBIT_M32 & *( unsigned long* )&data ) && ( ( _IBIT_M32 & *( unsigned long* )&data ) != _IBIT_M32 ) )
            {
                ( dataSize = 0 ), ( _dword = *( unsigned long* )&data ), ( dataType = VT_INTEGER ), ( flags = 0 ); 
            }
            else 
            {
                // ͬDWORD�Ĵ�����ע�⣬��Ч���ֵ����λΪ����λ
                ( reinterpret_cast< unsigned long& >( dataType ) = ( ( ( unsigned long )data ) << 8 ) );
                ( _dword = 0 ), ( dataType = VT_I32_24 );
            }
        } 

        Variant( const unsigned __int64 &data )
        { 
#define __LPDW ( ( unsigned long* )&data )
            // �����Ƿ����������ѹ����
            if ( _UBIT_M32 & __LPDW[1] ) 
            {
                // ���8λ�Ƿ���ֵ�� ����о�ֻ�ܵ�64bit�����ˡ�����
                ( dataSize = 8 ), ( _pointer = &data ), ( dataType = VT_QWORD ), ( flags = 0 ); 
            }
            else if ( _UBIT_M64 & data ) 
            {
                // ����ǲ����Ƿ���תΪ24bit�������ֵ����ֻ��תΪ56bit������
                ( reinterpret_cast< unsigned __int64& >( dataType ) = ( data << 8 ) ), ( dataType = VT_U64_56 );
            }
            else 
            {
                // ʣ��ľ���ֱ��ת��24bit
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
            // [����]��֧��1024���ϵ��ַ�����
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

			        // [����]��֧��1024���ϵ��ַ�����
			        if ( IsBadStringPtr( str, CHAR_BUFSIZE ) )
				        throw BadString();

                    AllocateAndAssign( VT_STRING, str, strlen( str ) + 1 );
				}
				return; // ����BSTR��ʱת����char*����������ض����������ֱ�ӷ���

            // use this convertion to avoid sqlserver-convert-issue with bigint <-> decimal
            case VT_DECIMAL:
                {
                    // variant.decVal.sign  = ����λ��ֻ�����λ��Ч
                    // variant.decVal.scale = ����С��λ
                    // variant.decVal.Hi32  = �����ں���  64bit �� 92bit ��Чλ�е�  32bit ��λ����
                    // variant.decVal.Lo64  = ������ǰ    32bit �� 92bit ��Чλ�е�  64bit ��λ����

                    if ( variant.decVal.Hi32 != 0 )
                        // ���ǵ������㸴�Ӷȵ����⣬��֧�ִ���64λ����ֵ��
                        throw InvalidConvert();

                    __int64 temp64 = ( variant.decVal.sign == 0x80 ) 
                        ? -( __int64 )( variant.decVal.Lo64 & 0x7fffffffffffffff )
                        : ( __int64 )( variant.decVal.Lo64 & 0x7fffffffffffffff );

                    new ( this ) Variant( temp64 );

                    // ����������޷�ֱ��д��4���ֽ��ڣ�����Ҫ��չ�ռ䣡
                    if ( dataType >= VT_QWORD )
                        AllocateAndAssign( VT_LARGINTEGER, &temp64, 8 );
                }
				return; // ��Ϊ����Ҫô�Ǽ򻯺�ֱ�Ӹ�ֵ��Ҫô�ض������������Ҳֱ�ӷ���

            default:
                throw InvalidConvert();
            }

            if ( deepCopy && ( dataType >= VT_QWORD ) ) // ��������,����Ҫ���·������ݲ�����!
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
        // ǳ������ֱ��ʹ�����������ݣ������ڴ�ָ��
		Variant ( const Variant &variant )
		{
            // ����ط���Ҫ����ڴ�����־
            ( ( LPDWORD )this )[0] = ( variant.dataType >= VT_QWORD ) ? 
				( ( ( LPDWORD )&variant )[0] & 0xfffffeff ) : ( ( LPDWORD )&variant )[0];
            //dataType = variant.dataType;
            //dataSize = variant.dataSize;
            //flags = variant.flags & 0xe; 
            _dword = variant._dword;
		}

        // �����������Ҫ���г־û��ĳ���
		void DeepCopy( const Variant &variant )
        {
            // �ȼ���Լ��Ƿ����ͷű�ǣ� ������γ���Դй¶������
	        if ( flags & FT_ALLOCATED_MEMBLOCK )
		        free( ( void* )_pointer );

            // ����Ƿ���������Դ�������ֱ�Ӹ�ֵ
            if ( variant.dataType < VT_QWORD )
            {
                *this = variant;
            }
            // ������������Դ�⣬������������Դ�ˣ���Ȼ��Ҫ��������ά�֣�
            // ��ֻ�ܷ���һ���µĿռ��������ݵ�ǿ�ƿ�������������ָ�뱻�����ͷš�����
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
                return ( _shortinteger < 0 ) // ������Ч���ݲ��ֵ����λ������λ�����в�ͬ����
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
                return ( _shortinteger < 0 ) // ������Ч���ݲ��ֵ����λ������λ�����в�ͬ����
                    ? ( ( reinterpret_cast< const unsigned long& >( dataType ) >> 8 ) | 0xffffffffff000000 )
                    : ( reinterpret_cast< const unsigned long& >( dataType ) >> 8 );

            case VT_I64_56:
                return ( _integer < 0 ) // ������Ч���ݲ��ֵ����λ������λ�����в�ͬ����
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
        // ϵͳ��׼API::IsBadStringPtrֻ���ж���ָ���ַ�����ָ�������ڵĶ���Ч�ԡ��� 
        // ������ǵĳ��Բ�һ�¡���������Ҫ��֤���ַ�����������[null terminate]������
        // �����Լ���д��һ������
        //**************************************************************************** 
        inline BOOL IsBadStringPtr( LPCSTR string, unsigned long size ) throw()
        {
            try
            {
                while ( *string != 0 )
                {
                    size--;
                    string++;

                    // ���ʵ�ʳ���[+'\0']����Ĭ�ϳ��ȣ�����ʧ��
                    if ( size == 0  )
                        return true;
                }

                return false;
            }
            catch ( ... )
            {
                // ����������г����쳣����ʧ��
            }

            return true;
        }

    public:
        BYTE dataType;  // ��ǰ���ݶ������������
        BYTE flags;     // ���������������0x20��֮ǰ�������ݱض��������������ôflags���������ݿռ� 

        union 
        {
            // ������ݲ��������ݶ��룬��¼����ʵ����Ч�����ݴ�С
            WORD                dataSize;

            // ���������������������㹻С������ֱ�ӱ��浽dataSize���
            unsigned __int16    _word;
            __int16             _shortinteger;
        };

        union 
        {
            __int32             _rpcop;

            unsigned long       _dword;
            __int32             _integer;
            float               _float;

            // 64λ���������Ͳ����������У��������ɽṹ���͵�12�ֽ�
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
