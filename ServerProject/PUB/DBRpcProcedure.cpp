#include "stdafx.h"
#include "rpcop.h"
#include "traceinfo.h"
#include "dbserver/querylib/queryclass.h"
#include "networkmodule/refreshmsgs.h"
#include "liteserializer/lite.h"

extern BOOL SendMessage( DNID dnidClient, LPCVOID lpMsg, size_t wSize );
__declspec( thread ) BYTE RPCLocalBuffer[ 0x8000 ]; // �����л�RPC���ݱ�����ֵʱʹ��

#define LogErrorMsg( str ) _LogMsg( "DBRpcError.txt", str, dwRunIndex )

void _LogMsg( LPCSTR filename, LPCSTR szLogMessage, int ex = -1 )
{
    try
    {
        SYSTEMTIME s;
        char szLogTime[128];

        GetLocalTime(&s);
        sprintf(szLogTime, "[%d-%d-%d %d:%d:%d][%d]", s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond, ex );

        HANDLE hFile = ::CreateFile( filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, NULL, NULL );
        if ( hFile == INVALID_HANDLE_VALUE )
            return;

        DWORD sizeA = (DWORD)strlen( szLogTime );
        DWORD sizeB = (DWORD)strlen( szLogMessage );

        SetFilePointer( hFile, 0, 0, FILE_END );
        WriteFile( hFile, szLogTime, sizeA, &sizeA, 0 );
        WriteFile( hFile, szLogMessage, sizeB, &sizeB, 0 );
        WriteFile( hFile, "\r\n", 2, &sizeA, 0 );

        CloseHandle( hFile);
    }
    catch ( ... )
    {
    }
}

#undef Fields

void RpcProcedure( Query &query, DNID srcDnid, LPVOID rpcBuffer, DNID passDnid )
{
    #define COMMAND query.m_pCommand
    #define RECORDSET query.m_pRecordset
    #define GetVariant() ( ( dwRunIndex ++ ), ls() )

    DWORD dwRunIndex = 0;
    size_t lvtErrAddr = -1;

    // �漰��ѭ�����������ݣ�
    size_t lvtTraverseLoopAddr = -1;    // ѭ�����
    DWORD loopRunIndexSegment = -1;     // ��������ִ������ ��� dwRunIndex
    int loopNumber = -1;                // ����ѭ������

    const static int HEADER_SIZE = offsetof( SAGameServerRPCOPMsg, SAGameServerRPCOPMsg::streamData );

    char retBuffer[ 4096 + HEADER_SIZE ];
    char errorBuffer[ 1024 ];

    // �������ķ��ؿռ䳬��Ĭ�ϴ�Сʱ�����ᱻ���·��䣡
    char *ptr_retBuffer = NULL;
    LPCSTR procName = "null";

    lite::Serialreader ls;
    lite::Serializer retBox, errorBox;

    try
    {
        // ������һ�������������Ϊ����������ܲ����쳣�����ⲿ����Ҫʹ�õ�ls�����Ծ��������ˣ���
        new ( &ls ) lite::Serialreader( rpcBuffer );

        lite::Variant lvt = GetVariant();

        while ( lvt.dataType == lite::Variant::VT_RPC_OP )
        {
            switch ( lvt._rpcop )
            {
            case OP_BEGIN_PARAMS:
                {
                    while ( TRUE )
                    {
                        lite::Variant lvt = GetVariant();
                        _ParameterPtr lpParam = NULL;

                        if ( lvt.dataType == lite::Variant::VT_RPC_OP )
                        {
                            if ( lvt._rpcop == OP_END_PARAMS )
                                break;

                            // ������� VT_RPC_OP ��ôӦ��һ���ǽ�������
                            throw lite::InvalidConvert();
                        }
                        else if ( lvt.dataType == lite::Variant::VT_STRING )
                        {
                            lpParam = COMMAND->Parameters->GetItem( _bstr_t( lvt._string ) );
                        }
                        else 
                        {
                            ULONG dwTemp = 0;
                            _variant_t vPtr;

                            IEnumVARIANTPtr vtEnum = COMMAND->Parameters->_NewEnum();
                            vtEnum->Reset();
                            vtEnum->Skip( ( int )lvt );
                            vtEnum->Next( 1, &vPtr, &dwTemp );

                            lpParam = vPtr;
                        }

                        if ( lpParam == NULL )
                            throw lite::NullPointer();

                        lite::Variant lvtParamValue = GetVariant();

                        if ( lvtParamValue.dataType == lite::Variant::VT_RPC_OP )
                            throw lite::InvalidConvert();

                        lpParam->Value = lvtParamValue;
                    }
                }
                break;

            case OP_BOX_VARIANT:
                retBox.PushVariant( GetVariant() );
                break;

            case OP_BOX_PARAM:
                {
                    lite::Variant lvt = GetVariant();

                    _ParameterPtr lpParam = NULL;

                    if ( lvt.dataType == lite::Variant::VT_STRING )
                    {
                        lpParam = COMMAND->Parameters->GetItem( _bstr_t( lvt._string ) );
                    }
                    else
                    {
                        ULONG dwTemp = 0;
                        _variant_t vPtr;

                        IEnumVARIANTPtr vtEnum = COMMAND->Parameters->_NewEnum();
                        vtEnum->Reset();
                        vtEnum->Skip( ( int )lvt );
                        vtEnum->Next( 1, &vPtr, &dwTemp );

                        lpParam = vPtr;
                    }

                    if ( lpParam == NULL )
                        throw lite::NullPointer();

                    retBox.PushVariant( lite::Variant( lpParam->Value ) );
                }
                break;

            case OP_BOX_FIELD:
                {
                    lite::Variant lvt = GetVariant();
                    FieldPtr lpField = NULL;

                    if ( lvt.dataType == lite::Variant::VT_STRING )
                        lpField = RECORDSET->Fields->GetItem( _bstr_t( lvt._string ) );
                    else
                        lpField = RECORDSET->Fields->GetItem( ( long )lvt );

                    if ( lpField == NULL )
                        throw lite::NullPointer();

                    retBox.PushVariant( lite::Variant( lpField->Value ) );
                }
                break;

            case OP_PREPARE_STOREDPROC:
                {
                    lite::Variant lvtCallName = GetVariant();
                    procName = lvtCallName;
                    COMMAND->CommandText = lvtCallName;
                    COMMAND->CommandType = adCmdStoredProc;
                    COMMAND->Parameters->Refresh();
                }
                break;

            case OP_CALL_STOREDPROC:
                RECORDSET = COMMAND->Execute( NULL, NULL, adCmdStoredProc );
                break;

            case OP_INIT_RETBOX:
                {
                    int allocSize = GetVariant();
                    if ( ptr_retBuffer != NULL )
                        throw lite::InvalidBuffer();

                    if ( allocSize >= sizeof( retBuffer ) - HEADER_SIZE )
                        ptr_retBuffer = new char[ allocSize + HEADER_SIZE ];
                    else
                        ptr_retBuffer = retBuffer;

                    retBox.Assign( ptr_retBuffer + HEADER_SIZE, allocSize );
                }
                break;

            case OP_RETURN_BOX:
                {
                    SAGameServerRPCOPMsg *pMsg = new ( ptr_retBuffer ) SAGameServerRPCOPMsg();
                    pMsg->dnidClient = passDnid;
                    retBox.EndEdition();
                    SendMessage( srcDnid, ptr_retBuffer, HEADER_SIZE + retBox.maxSize() );
                }
                break;

            case OP_TRAVERSE_RECORDSET:
                {
                    loopNumber = GetVariant();

                    // ���������ѭ����һ��ʼ���Ѿ�û�����ݣ���ֱ�Ӻ��Ե�ѭ������
                    if (  RECORDSET == NULL || ( !RECORDSET->State ) || RECORDSET->adoEOF)
                    {
                        while ( TRUE )
                        {
                            lite::Variant lvt = GetVariant();

                            if ( ( lvt.dataType == lite::Variant::VT_RPC_OP ) && 
                                ( lvt._rpcop == OP_CHECK_TRAVERSE_LOOP ) )
                                break;
                        }
                    }
                    else
                    {
                        // ��¼��ǰ��ѭ��������ַ���Ͳ�ѯ��ַ
                        lvtTraverseLoopAddr = ls.curSize();
                        loopRunIndexSegment = dwRunIndex;
                    }
                }
                break;

            case OP_CHECK_TRAVERSE_LOOP:
                {
                    if ( ( RECORDSET != NULL ) && ( RECORDSET->adoEOF || ( ( -- loopNumber ) == 0 ) ) )
                    {
                        // ѭ������
                        RECORDSET->Close();
                        break;
                    }

                    RECORDSET->MoveNext();

                    if ( RECORDSET->adoEOF )
                    {
                        // ѭ������
                        RECORDSET->Close();
                        break;
                    }

                    // �ָ�ѭ��������ַ���Ͳ�ѯ��ַ
                    ls.curSize() = static_cast< WORD >( lvtTraverseLoopAddr );
                    dwRunIndex = loopRunIndexSegment;
                }
                break;

            case OP_BEGIN_ERRHANDLE:
                {
                    // ��¼��ǰ��ERRHANDLE��ַ��
                    lvtErrAddr = ls.curSize();

                    while ( TRUE )
                    {
                        lite::Variant lvt = GetVariant();

                        if ( ( lvt.dataType == lite::Variant::VT_RPC_OP ) && ( lvt._rpcop == OP_END_ERRHANDLE ) )
                            break;
                    }
                }
                break;

			case OP_TABLE_DATA:
				{
					int retNumber = GetVariant(); // ֻ��ȡָ�������ļ�¼��
					int tableType = GetVariant(); // �������ݵı�����ʽ

                    if ( RECORDSET == NULL || ( !RECORDSET->State ) || RECORDSET->adoEOF )
                        retNumber = 0;
                    else
					    retNumber = min( retNumber, RECORDSET->RecordCount );

                    if ( retNumber == 0  )
                    {
                        lite::Variant lvt;
                        lvt.dataType = lite::Variant::VT_NULL;
                        retBox.PushVariant( lvt ); // ������ݼ�Ϊ�գ��򷵻�null
                    }
                    else
                    {
                        // ���л��������ֻ֧��0xffff��С�Ļ�����������
					    lite::Serializer ls( RPCLocalBuffer, sizeof( RPCLocalBuffer ) ); 

				        // �ȴ����ֶ������ٴ������������������ݿ�������fetchʧ�ܣ����²�׼ȷ��
                        DWORD fc = RECORDSET->Fields->GetCount();
					    ls( fc )( ( DWORD )retNumber );

                        for ( int i = 0; i < retNumber; i++ ) 
                        {
                            IEnumVARIANTPtr vtEnum = RECORDSET->Fields->_NewEnum();
                            _variant_t vPtr;
                            DWORD dwTemp = 0;
                            for ( DWORD j = 0; j < fc; j++ )
                            {
                                vtEnum->Next( 1, &vPtr, &dwTemp );
                                FieldPtr fPtr = vPtr;
                                ls( fPtr->Value );
                            }

                            RECORDSET->MoveNext();
                            if ( RECORDSET->adoEOF )
                                break;
                        }

                        // ������ǰ����������ݣ�������ߵ�2�����л�������Ҫ�������Ϊlua_table
                        if ( tableType != '@' ) retBox[ OP_TABLE_DATA ] ( tableType ); 

                        // ѹ��������л����ݡ�����
                        retBox( RPCLocalBuffer, ( sizeof( RPCLocalBuffer ) - ls.EndEdition() ) ); 
                    }

                    if ( RECORDSET ) 
                        RECORDSET->Close();
                }
                break;

            case OP_RPC_END:
                goto __finally_release;
            }

            lvt = GetVariant();
        }

        throw lite::CurrentIsEnd();
    }
    catch ( _com_error &e )
    {
        _variant_t nLine = dwRunIndex;
        nLine.ChangeType( VT_BSTR );
        _bstr_t info = e.Description();
        if ( info.length() == 0 )
        {
            _variant_t hErr = e.Error();
            hErr.ChangeType( VT_BSTR );
            info = hErr.bstrVal;
        }

        LogErrorMsg( "RPC_OP [" + (_bstr_t)procName + "] opNo.[" + (_bstr_t)nLine + "] ComError[" + info + "]" );

        try
        {
            COMMAND->CommandText = "null_sp";
            COMMAND->CommandType = adCmdStoredProc;
            COMMAND->Parameters->Refresh();
        }
        catch ( _com_error &e )
        {
            _variant_t nLine = dwRunIndex;
            nLine.ChangeType( VT_BSTR );
            _bstr_t info = e.Description();
            if ( info.length() == 0 )
            {
                _variant_t hErr = e.Error();
                hErr.ChangeType( VT_BSTR );
                info = hErr.bstrVal;
            }

            LogErrorMsg( "RPC_OP com_inner opNo.[" + (_bstr_t)nLine + "] ComError[" + info + "]" );
        }
    }
    catch ( lite::Xcpt &e )
    {
        _variant_t nLine = dwRunIndex;
        nLine.ChangeType( VT_BSTR );
        LogErrorMsg( "RPC_OP [" + (_bstr_t)procName + "] opNo.[" + (_bstr_t)nLine + "] LiteError[" + e.GetErrInfo() + "]" );
    }

    if ( lvtErrAddr == -1 )
        goto __finally_release;

    // Error-handle
    errorBox.Assign( errorBuffer + HEADER_SIZE, sizeof( errorBuffer ) - HEADER_SIZE );

    // �ش�dnidClient�����ڱ�ʶ�ǵ�½������ֱ�����󣬻������������������
    ( new ( errorBuffer ) SAGameServerRPCOPMsg() )->dnidClient = passDnid;

    ls.curSize() = (WORD)lvtErrAddr;

    try
    {
        while ( TRUE )
        {
            lite::Variant lvt = GetVariant();

            if ( ( lvt.dataType == lite::Variant::VT_RPC_OP ) && ( lvt._rpcop == OP_END_ERRHANDLE ) )
                break;

            errorBox.PushVariant( lvt );
        }
    }
    catch ( lite::Xcpt &e )
    {
        _variant_t nLine = dwRunIndex;
        nLine.ChangeType( VT_BSTR );
        LogErrorMsg( "RPC_OP ehandle [" + (_bstr_t)procName + "] opNo.[" + (_bstr_t)nLine + "] LiteError handle [" + e.GetErrInfo() + "]" );
    }

    errorBox.EndEdition();

    SendMessage( srcDnid, errorBuffer, HEADER_SIZE + errorBox.maxSize() );

__finally_release:

    // ��Ҫ�ͷŴ�����������ʱ�ռ䣡
    if ( ptr_retBuffer != retBuffer )
        delete ptr_retBuffer;

    if ( ( RECORDSET != NULL ) && ( RECORDSET->State ) )
        RECORDSET->Close();
}