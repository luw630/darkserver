#include "stdafx.h"
#include "windows.h"
#include "details/win32/globals.h"

static LPCSTR GetStringTime( char *outBuffer, size_t bufferSize )
{
    SYSTEMTIME s;
    GetLocalTime(&s);

    _snprintf( outBuffer, bufferSize, "[%d-%d-%d %d:%d:%d]", 
        s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond );

    outBuffer[ bufferSize - 1 ] = 0;

    return outBuffer;
}

static void Trace( LPCSTR szFormat, ... )
{
    char timeBuffer[64];
    char infoBuffer[1024];

    try
    {
        va_list header;
        va_start( header, szFormat );
        _vsnprintf( infoBuffer, sizeof( infoBuffer ) - 1, szFormat, header );
        va_end( header );

        infoBuffer[ sizeof( infoBuffer ) - 1 ] = 0;

        std::ofstream stream( "server_dinfo.txt", std::ios_base::app );
        stream << GetStringTime( timeBuffer, sizeof( timeBuffer ) ) << infoBuffer;
    }
    catch ( ... )
    {
    }
}

static void ErrorHandle( LPCSTR info, BOOL le = FALSE, int eid = 0 )
{
    if ( le )
    {
        Trace( "%s - [%d]\r\n", info, WSAGetLastError() );
    }
    else if ( eid != 0 )
    {
        Trace( "%s - [%d]\r\n", info, eid );
    }
    else
    {
        Trace( "%s\r\n", info );
    }
}

static DNID dnid_gen()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    DNID dnid = *(DNID*)&li;
	if (!(dnid & 0x00000000ffff0000))
		dnid += 0x10000;
    return dnid;
}

static void _my_assert( LPCSTR exp, LPCSTR filename, int line )
{
    Trace( "%s(%d):%s\r\n", filename, line, exp );
}

static void SpecialLog( const void *raw_buffer, size_t size, const void* data, size_t data_size, DWORD sequence )
{
    static volatile LONG log_count = 0;
    LONG index = InterlockedIncrement( &log_count );
    if ( ( DWORD )index >= 10 )
        return;

    CreateDirectory( "SpecialLog", NULL );

    char filename[256];
    sprintf( filename, "speciallog\\%d_%d.mem", index, sequence );
	HANDLE hFile = CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL );
    if ( hFile == INVALID_HANDLE_VALUE )
        return;

    DNPHDR header;
    header.seqnum = ( WORD )sequence;
    header.paclen = ( UINT16 )data_size;

    DWORD cbSize;
    WriteFile( hFile, raw_buffer, ( DWORD )size, &cbSize, NULL );
    WriteFile( hFile, &header, sizeof( header ), &cbSize, NULL );
    WriteFile( hFile, data, ( DWORD ) data_size, &cbSize, NULL );
    CloseHandle( hFile );
}

#include "details/win32/connection.hpp"
#include "details/win32/server.hpp"

iDirectNetServer* iDirectNetServer::CreateInstance()
{
#ifdef _DEBUG
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flags = ( flags & 0x0000FFFF ) | _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag( flags );
#endif

    return new DreamNet_Win32::cDirectNetServer;
}

void iDirectNetServer::Release()
{
    delete this;
}
