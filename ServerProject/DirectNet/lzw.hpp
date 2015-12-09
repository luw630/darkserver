#pragma once

namespace lzw
{
    // LZW编解码中使用到的常量数据
    static const int BITS           = 12;                   /* largest code size */
    static const int SHIFT          = 4;                    /* shift for hashing */
    static const int THELIMIT       = 4095;                 /* NEVER generate this */
    static const int HSIZE          = 5003;                 /* hash table size */

    static const int CODE_CLEAR     = ( 1 << 8 );           /* clean the dictionary table */
    static const int CODE_EOF       = CODE_CLEAR + 1;       /* end of once package */

    struct lite_lzwEncoder
    {
        lite_lzwEncoder() { reset(); }

        inline size_t size() { return ( LPBYTE )cur_addr - ( LPBYTE )raw_buffer + ( cur_bits ? 4 : 0 ); }

        inline BOOL empty() { return emptySlot == CODE_CLEAR + 2; }

        inline void reset() 
        { 
            curCodeBits = 9; 
            emptySlot = CODE_CLEAR + 2; 
            curMaxCode = ( 1 << curCodeBits ) - 1;
            memset( hashTab, -1, sizeof( hashTab ) );
        }

        inline void SetEncoderData( LPVOID entry, size_t size ) 
        {
            // size 应该是4的整数倍，便于对齐，保证数据大小一直
            // entry 应该是8的整数倍，便于对齐，以提高读写性能
            assert( ( !( size & 0x3 ) ) && ( !( ( _W64 DWORD )entry & 0x7 ) ) );

            raw_buffer  = reinterpret_cast< LPDWORD >( entry );
            cur_addr    = raw_buffer;
            end_addr    = raw_buffer + ( size >> 2 );
            cur_bits    = 0;

            // entry 需要被置0，用于后续的连续"与"操作
            *cur_addr = 0;
        }

        inline BOOL put( DWORD bits, DWORD data )
        {
            *cur_addr |= ( data << cur_bits );
            cur_bits += bits;

            // 如果bit位数已经超过了4个字节（DWORD），则cur_addr需要增加
            if ( cur_bits & 0xffffffe0 )
            {
                cur_bits &= 0x1f, cur_addr ++;

                if ( cur_addr == end_addr )
                    return FALSE;

                *cur_addr = ( data >> ( bits - cur_bits ) );
            }

            return TRUE;
        }

    #define output( theCode )                           {       \
            /* 如果put失败，则说明缓存空间已经不够用了！*/      \
            if ( !put( curCodeBits, theCode ) ) return FALSE;   \
            else if ( theCode == CODE_CLEAR ) reset();          \
            else if ( emptySlot > curMaxCode )          {       \
                curCodeBits ++;                                 \
                if ( curCodeBits == BITS )                      \
                    curMaxCode = THELIMIT;                      \
                else                                            \
                    curMaxCode = ( 1 << curCodeBits ) - 1;      \
            } else if ( theCode == CODE_EOF ) { }               \
        }

        BOOL lzw_encode( LPVOID inbuf, size_t size )
        {
            LPBYTE buffer = ( LPBYTE )inbuf;

            int code, i, k, codek, disp;
            const LPBYTE endAddr = buffer + size;
            for ( code = *buffer++; buffer != endAddr; buffer ++ ) 
            {
                k     = *buffer;                                /* next symbol */
                codek = ( ( long ) k << BITS ) + code;          /* set full code */
                i     = ( k << SHIFT ) ^ code;                  /* xor hashing */

                if ( hashTab[i] == 0xffffffff )            /* empty slot */
                    goto NOMATCH;

                else if ( ( hashTab[i]&0xfffff ) == codek )          /* full code found */
                {
                    code = ( hashTab[i] >> 20 );
                    continue;
                }

                if ( i != 0 ) 
                    disp = HSIZE - i;                               /* secondary hash */
                else
                    disp = 1;

    PROBE:
                if ( ( i -= disp ) < 0 )
                    i += HSIZE;

                if ( hashTab[i] == 0xffffffff )
                    goto NOMATCH;

                else if ( ( hashTab[i]&0xfffff ) == codek )          /* full code found */
                {        
                    code = ( hashTab[i] >> 20 );
                    continue;
                }

                else if ( hashTab[i] != 0xffffffff )                 /* try again */
                    goto PROBE;

    NOMATCH:
                output( code );                                 /* full code not found */
                code = k;

                if ( emptySlot < THELIMIT ) 
                {
                    hashTab[i] = ( emptySlot << 20 ) | codek;   /* code -> hashtable */
                    emptySlot ++;
                }
                else
                    output( CODE_CLEAR );
            }

            output( code );
            output( CODE_EOF );

            return TRUE;
        }

        LPDWORD         raw_buffer;
        LPDWORD         end_addr;
        LPDWORD         cur_addr;
        DWORD           cur_bits;

        int             emptySlot;
        int             curCodeBits;
        int             curMaxCode;
        unsigned long   hashTab[ HSIZE ];   /* hash table */
    };

    struct lzwDecoder
    {
        lzwDecoder() { reset(); }

        inline BOOL empty() { return emptySlot == CODE_CLEAR + 2; }

        inline void reset() 
        { 
            curCodeBits = 9; 
            emptySlot = CODE_CLEAR + 2; 
            curMaxCode = ( 1 << curCodeBits );

            FinChar = 0;
        }

        inline void SetEncoderData( LPVOID entry, size_t size ) 
        {
            raw_buffer  = reinterpret_cast< LPDWORD >( entry );
            end_addr    = raw_buffer + ( size >> 2 );
            cur_addr    = raw_buffer;
            cur_bits    = 0;

            // size 应该是4的整数倍，便于对齐
            assert( !( size & 0x3 ) );
        }

        inline DWORD get( DWORD bits )
        {
            //// data &= ( ( 1 << bits ) - 1 ); 将无效位数的数据清除！不过为了效率，直接默认外部传入的数据已经只存在有效位！
            const DWORD data = ( ( *cur_addr ) >> cur_bits ) & ( ( 1 << bits ) - 1 );
            cur_bits += bits;
            // 如果bit位数已经超过了4个字节（DWORD），则cur_addr需要增加
            if ( cur_bits & 0xffffffe0 )
            {
                cur_bits &= 0x1f, cur_addr ++;
                if ( cur_bits )
                    return data | ( ( ( *cur_addr ) & ( ( 1 << cur_bits ) - 1 ) ) << ( bits - cur_bits ) );
            }

            return data;
        }

        /*   L Z W   D E C O D E   */
        size_t lzw_decode( LPVOID outbuf, size_t size )
        {
            static const int CODEMASK = 0xff;

            #define OutByte( byte ) { if ( count >= size ) return -1; ( ( *buffer++ = byte ), count++ ); }

            LPBYTE buffer = ( LPBYTE )outbuf;

            int CurCode, OldCode, InCode, OutCount = 0;
            size_t count = 0;

            if ( empty() )
            {
                FinChar = OldCode = get( curCodeBits );
                OutByte( FinChar );
                CurCode = get( curCodeBits );
            }
            else
            {
                OldCode = CODE_EOF;
                CurCode = get( curCodeBits );
            }

            for ( ; count < size; ) 
            {
                if ( CurCode < 0 )
                    // fprintf(stderr,"\nGIFdecode: corrupted GIF (zero block length)\n");
                    return -2;

                if ( CurCode == CODE_EOF )
                {
                    // fprintf(stderr,"\nGIFdecode: corrupted GIF (unexpected EOF)\n");
                    return count;
                }

                if ( CurCode == CODE_CLEAR )                    /* clear code ... reset */
                {
                    reset();

                    FinChar = OldCode = get( curCodeBits );
                    OutByte( FinChar );
                } 
                else                                            /* image code */
                {
                    InCode = CurCode;
                    if ( CurCode >= emptySlot ) 
                    {
                        CurCode = OldCode;
                        OutCode[ OutCount++ ] = FinChar;
                    }

                    while ( CurCode > CODEMASK )                 /* build output pixel chain */
                    {               
                        if ( OutCount >= THELIMIT ) 
                            // fprintf(stderr,"\nGIFdecode: corrupted GIF (big output count)\n");
                            return -3;

                        OutCode[ OutCount++ ] = Suffix[ CurCode ];
                        CurCode = Prefix[ CurCode ];
                    }

                    FinChar = CurCode;
                    OutCode[ OutCount++ ] = FinChar;

                    for ( int i=OutCount-1; i>=0; i-- )         /* put out pixel chain */
                        OutByte( OutCode[i] );

                    OutCount = 0;

                    if ( OldCode != CODE_EOF )
                    {
                        Prefix[ emptySlot ] = OldCode;              /* build the tables */
                        Suffix[ emptySlot ] = FinChar;

                        emptySlot ++;                               /* move pointer */
                        if ( emptySlot >= curMaxCode ) 
                        {
                            if ( curCodeBits < BITS ) 
                            {
                                curCodeBits ++;
                                curMaxCode *= 2;
                            }
                        }
                    }

                    OldCode = InCode;
                }

                CurCode = get( curCodeBits );
            }

            return count;
        }

        LPDWORD         raw_buffer;
        LPDWORD         end_addr;
        LPDWORD         cur_addr;
        DWORD           cur_bits;

        BYTE            FinChar;

        int             curMaxCode;
        int             emptySlot;
        int             curCodeBits;
        int             Prefix[4096];          /* prefix table */
        BYTE            Suffix[4096];          /* suffix table */
        BYTE            OutCode[4096];         /* output stack */
    };
}
