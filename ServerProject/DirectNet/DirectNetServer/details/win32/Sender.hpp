#pragma once

// #define USE_QUEUE
#include "../lzw.hpp"

namespace DreamNet_Win32
{
    // ��Ҫ˵����Sender��ص��̣߳�
    // A��[��Ϣ�����߳�] -> [InternalSend]
    // B��[�¼������߳�] -> [OnSendComplete] -> [InternalSend]
    // C��[�¼������߳�] -> [OnReceiveComplete] -> [handler] -> [PushPackage / InternalSend]
    // D��[ģ���ⲿ�߳�] -> [PushPackage / InternalSend]
    // E��[ģ���ⲿ�߳�] -> [OnClose] -> [OnSendComplete] -> [InternalSend]
    // F��[ģ���ⲿ�߳�] -> [OnClose] -> [OnReceiveComplete] -> [handler] -> [PushPackage / InternalSend]
    // ���ԣ�Sender�ڲ���3����Ҫ�������ٽ������ӵñȽ���������
    // E��F û����Դ������в����ΪOnClose���ڴ��������¼�ʱ�����еĹ����̶߳��Ѿ�ֹͣ�ˣ�����
    struct Sender
    {
        typedef boost::shared_ptr< Sender > reference_ptr;

        enum SEND_RESULT { SEND_OK, SEND_FAIL, SEND_NODATA, SEND_PENDING };

#ifndef USE_QUEUE
        // ���������κ�ʱ��������һ��buffer��Ч����
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
            bool  headerIndex;      // ��ǰ���ڵ�һλ��bufferλ�ã�
            bool  secondValidate;   // ��ǰ���ڵڶ�λ��buffer�Ƿ���Ч��
        };
#endif

        struct DummyBarrier
        {
            template < typename _lock > DummyBarrier( _lock& ) {}
            ~DummyBarrier() {}
        };

        Sender( const ConnectionData::reference_ptr &connectionData ) : connectionData( connectionData )
        {
            // connectionData һ������Ч�Ķ���
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

            // Ӧ�ò����ڴ��ڷ����е����ݰ�
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

                // ʼ����ֱ������һ��buffer���д���ݣ�
                result = ( sendQ.front().push( data, size, sequence ++ ) ? 0 : -1 );

                if ( result )
                {
                    if ( ++ connectionData->pushFailureTimes > 10 )
                    {
                        result = -2;

                        // ���ʧ�ܵ���һ������ʱ��ֱ�ӽ����ӶϿ���ʧ�ܷ���
                        // ͨ�� SD_BOTH �������ӶϿ��������ͷ���Դ
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

        // ����������Ϻ�Ļص�
        void OnSendComplete( unsigned long nbytes_transferred )
        {
            // ���ڱ�֤�ٽ����ܹ���this��Ч�ķ�Χ�ڽ��в�����
            reference_ptr this_ptr;

            {
                UGE::CriticalLock::Barrier4ReadWrite barrier( lock4Q );

                assert( reference );                            // ��Ȼ�ǻص�����reference�ض�����Ч�ģ�
                assert( nbytes_transferred <= sending.len );    // ������ϵ����ݱض�ҪС�ڵ��������͵Ĵ�С
                assert( sendQ.size() == 1 );

                // ����reference���������ã����������ΪActivateSend������this���ͷţ�
                this_ptr = reference;

                // ��Ҫ�жϣ�������˵Ļ�������Ĳ����ᵼ�µ�������ʵ����Ӧ�ò�����֣�����
                if ( nbytes_transferred > sending.len )
                    throw exception( "nbytes_transferred > sending.len" );

                // 2010-1-18 �߼��޸�
                // ��Ϊ���ڸĳ��˷ֶη��ͣ����Կ����з���ʣ�µ����ݣ��ٴγ��Է��ͣ�
                if ( sending.send_incompleted( nbytes_transferred ) )
                    sendQ.insert_front( sending );
                else {
                    // ���ȷʵû����Ҫ���͵������ˣ���ת��������ǰ���߼�
                    sending.reset( 0 );
                    sendQ.push( sending );
                }

                // ��Ϊ�ϱ��Ѿ������ˣ�����ʹ��DummyBarrier���ε�ActivateSend�ڲ�����
                ActivateSend< DummyBarrier >( this_ptr );
            }
        }

        // �ⲿ��ֱ�ӵ��øú����������ǵ���InternalSend����Ϊ��Ҫ���ǵ�����ʧ�ܺ��reference�ͷ�
        // ����֮������ʹ��this_ptr���󴫵ݶ�����ʹ�����ô��ݣ�����Ϊ�±ߵ�reset���ܵ���this��ֱ���ͷţ�����
        // ��Ϊ�ĳ����õ����⣬�Ѿ�����һ�δ��ˣ���ס������this_ptr ���� this->reference���������Բ���ʹ�����ã�
        template < typename _barrier >
        BOOL ActivateSend( const reference_ptr &this_ptr )
        {
            // ��Ȼ�������������ͣ�����this_ptr�ض�����Ч�Ķ���
            assert( this_ptr );
            assert( this_ptr.get() == this );

            // ����ģ������������� lock4Q ��������ʽ��
            _barrier barrier( lock4Q );

            // �������ʧ�ܣ�����Ҫ�����Ͷ����referenceȡ��
            switch ( InternalSend( this_ptr ) )
            {
            case SEND_OK:
                return TRUE;

            case SEND_FAIL: 
                // ����ʧ��ʱ���Ͽ����ӣ��ͷ����ã�ʧ�ܷ���
                // ͨ�� SD_BOTH �������ӶϿ��������ͷ���Դ
                shutdown( connectionData->sock, SD_BOTH );
                // without break

            case SEND_NODATA:
                // û��������Ҫ����ʱ���ͷ����ã�ʧ�ܷ���
                // ���Լ������ü����ͷţ������ڴ�й©��ͬʱ��Ϊ�ϱߵ�this_ptr�����Բ��õ���this��reset�������ͷţ�
                // shared_ptr��֧�ֶ��߳�ͬʱд��so��������������ٽ����ʹ��ڰ�ȫ���⣡
                reference.reset();
                // without break

            // case SEND_PENDING:
                // ������������ڷ��ͣ�ֱ��ʧ�ܷ���

            default: 
                return FALSE;
            }
        }

    private:
        // 2010-1-18 �� �߼��޸�
        // Ϊ�˽������0xf000��С�����ݰ��ڷ��͹�����ѹ�����ݴ�С���ܻ�����쳣������DNPHDR�޷���ȷ��ȡ���ݵ�����
        // �����޸ķ����߼�Ϊ�ֶη��� ���� ��ÿ�η��͵��������޲����� MAX_BUFFER_SIZE
        void TryEncoding()
        {
            // ���ݰ���С������£���������ѹ��
            assert( sending.len <= MAX_BUFFER_SIZE );
            if ( sending.len < 32 )
                return;

            // �ô�ʹ���� TLS ( thread local storage )�������������ջ�ռ䣬ͬʱ��׼�����㹻��Ļ���Ԥ�����
            __declspec( thread ) static BYTE encoderBuffer[ 0x11000 ];
            static const int HEADSIZE = sizeof( DNPHDR ) + 2; // 4�ֽڵİ�ͷ��2�ֽڵĳ���

            LPBYTE bufferReference = encoderBuffer;

            // ���ڴ�����8�ֽڶ��룬����߱���ʱ�ڴ��дЧ��
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

                // BODYǰ��2���ֽڱ�ʾ��ѹ��ǰ�Ĵ�С
                reinterpret_cast< WORD& >( bufferReference[ -2 ] ) = ( WORD )sending.len;
                newHead.seqnum = ( WORD )( lencoder.size() + 2 );

                // �����Ƿֶη��ͣ��������reset���߼����������ˣ�
                // sending.reset( lencoder.size() + HEADSIZE ); 
                // ���ھ�ֱ�Ӹ���sending��ߵ�ԭʼ����
                sending.len = lencoder.size() + HEADSIZE;
                memcpy( sending.buf, &newHead, sending.len ); 
            }
            else
                lencoder.reset();
        }

        SEND_RESULT InternalSend( const reference_ptr &this_ptr )
        {
            assert( ( sendQ.size() >= 1 ) && ( sendQ.size() <= 2 ) );

            // �Ѿ������ݴ��ڷ���״̬�У����������ͷ����ã������ɷ��͵Ļص�������
            if ( sendQ.size() == 1 )
                return SEND_PENDING;

            // �Ƿ�����Ч�����ݵȴ�����
            if ( !sendQ.front().send_valid() )
            {
                // ���û��������Ҫ���ͣ���ʧ�ܷ���
                // ���ڲ���Ҫ��Ҳ��������������reset����Ϊ�ϱߵ�barrier������Ϊreset��ʧЧ������overrun
                // ͬʱ�����reference��Ч����ͨ�����ϱߵ�sendQ.size() == 1��
                // ��˵���ò����ض���OnSendComplete����ģ�����ֱ�ӷ���FALSE��reset�����ⲿ������

                return connectionData->validate ? SEND_NODATA : SEND_FAIL;
            }

            sending = sendQ.front();
            sendQ.pop();

            // ��֤�ڷ���ǰ��referenceһ������Ч�ģ�
            // ������Send֮ǰ�ͱ���reference������OnSendComplete�ܿ��ܻ��ȱ�����
            if ( !reference )
                reference = this_ptr;

            // �����͵����ݰ��ǵ�һ�η��ͣ��ų����ݰ�ֻ������һ���״����ʱ������ѹ������
            //if ( ( sending.raw_buffer == sending.buf ) && connectionData->compress )
            //    TryEncoding();
            // 2010-1-18 �߼��޸�
            // ����ǰ��⣬���η�����Ϊ�˲����ϴη���δ��ɵ����ݣ��������·���ʣ������ݶ�
            if ( sending.send_prepare() && connectionData->compress )
                TryEncoding();

            // �������ݣ���Ȼ��Ϊ��sendQ.size() == 1�ı��������ʱ�� sending �Ѿ����ᱻ����߳�������
            // ����sendQȴ�Ǳ��ٽ��������ģ����Ի���ֻ�����ٽ�����ʹ��WSASend��

            if ( SOCKET_ERROR == WSASend( connectionData->sock, &sending, 1, 
                &current_transferred, 0, &olap, NULL ) )
            {
                int eid = WSAGetLastError();
                if ( WSA_IO_PENDING != eid )
                {
                    // ErrorHandle( "send failure", FALSE, eid );
                    // ��Ϊ�ڷ���ʧ�ܺ��ⲿ�����shutdown������������Ҫ��sending���
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
        std::queue< Buffer >    sendQ;              // �ȴ����͵�˫����[����������ֱ�ӱ���1���󱸻������ݣ����еĻ������ݾ�ֱ��д��sendQ.front]
#else
        TwinsBuffer             sendQ;              // �ȴ����͵�˫����[����������ֱ�ӱ���1���󱸻������ݣ����еĻ������ݾ�ֱ��д��sendQ.front]
#endif
        Buffer                  sending;            // ���ڷ����е����ݣ���������ݱ�������Ϻ󣬾�ֱ��push�������У���Ϊ�����ĺ󱸻��棡
        UGE::CriticalLock       lock4Q;             // sendQ���ٽ�����
        // ---------------------------

        // ��ΪSEND�Ƕ�����RECV�����ģ�RECV����֪ͨ�Լ������ӹ������е����ü��ͷţ�
        // ���п�����RECV�ͷŵ�ͬʱSEND������ִ��״̬�У�
        // ���ԣ�ͨ��reference����֤SEND�Ĳ������󲻻�ʧЧ��
        // newly �� shared_ptr�����Ǿ��Զ��ȳ̰�ȫ�ģ���Ҫע�⣡�μ�boost document
        reference_ptr                           reference;
        // ---------------------------

        // �����ӵĻ������ݣ�const���ͣ��Թ��쿪ʼ�����ᱻ�޸ģ�ֻ��������ʱ���Զ��ͷţ�
        // �����������һ������Ч��
        const ConnectionData::reference_ptr     connectionData;

        // ʹ��LZW�ڷ����ѹ���������ݣ��ͻ���ֻ����ѹ
        lzw::lite_lzwEncoder                    lencoder;
    };
}
