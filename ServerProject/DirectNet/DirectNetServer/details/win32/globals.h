#include <mswsock.h>
#include <mmsystem.h>
#include <deque>
#include <vector>
#include <cassert>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>

#include "../../pub/thread.h"

// #include "../../../pub/waitqueue.h"

#pragma comment (lib, "mswsock.lib")
#pragma comment (lib, "winmm.lib")

#ifdef _DEBUG
#include <crtdbg.h>
#undef _AFX_NO_DEBUG_CRT
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
#endif _DEBUG

//// ����ʵ���о���WSASend&WSARecv��lpNumberOfBytesSent��һ���ڲ�������ʱ����µ����ݣ����ԣ�������ݱ���ȫ����Ч������
//static DWORD s_dwTransferredBytes = 0;
//static DWORD s_dwFlags = 0;

// the DirectNetProtocol header
struct DNPHDR {
    UINT16 seqnum;
    UINT16 paclen;
};

// the header that is passed one-way from client to server
struct CRYPTION {
    UINT32 crc32;
    DNPHDR dnphdr;
    UINT64 signature;
};

//struct OVERLAPPEDPLUS
//{
//    OVERLAPPED ol;
//    SOCKET s;
//    int OpCode;
//    WSABUF wbuf;
//    DWORD dwTransferredBytes;
//    DWORD dwTotalBytes;
//    DWORD dwFlags;
//    DWORD dwSequence;
//    LPVOID pvContext;
//    DNID dnidClient;
//
//    DWORD dwRecvBytes;
//    DWORD dwRecvTimes;
//
//    cAutoSizeMemory pOperationBuffer;
//};

#define OF_CLOSECONNECTION  (0x00000001)

#ifdef _DEBUG
BOOL trace( LPCSTR szFormat, ... );
#define LOG trace
#else
#define LOG
#endif

/*
TCP״̬

    ��Ϊһ��Winsock����Ա,ͨ��û��Ҫ�˽�ʵ�ʵ�TCP״̬.���˽�TCP״̬,���ܸ��õ�
�����Winsock API������λ���Э���еĸı����Ӱ��.����,������Ա�ڹر��׽���ʱ
,������һ����������;Χ���׽��ֹرյ�TCP״̬������Ŀǰ�����Ȥ������.

    ��ÿ���׽�����˵�����ĳ�ʼ״̬����CLOSED.ŵ�ͻ�����ʼ����һ������,�ͻ������
������һ��SYN��,ͬʱ���ͻ����׽���״̬��ΪSYN_SENT.�������յ� SYN���󣬻ᷢ��һ
��"SYN-ACK"��.��Ϊ�ͻ���,��Ҫ��һ��ACK������������Ӧ.��ʱ,�ͻ������׽��ֻ���
ESTABLISHED״̬.���������һֱ������"SYN-ACK"��,�ͻ����ͻᳬʱ,������CLOSED״̬.

    ŵһ�����������׽���ͬһ�����ؽӿںͶ˿ڰ�����,������������м�������ô��
���ֵ�״̬����LISTEN.�ͻ�����ͼ��֮����ʱ���������ͻ��յ�һ��SYN��������һ��
SYN-ACK��������Ӧ���������׽��ֵ�״̬�ͱ��SYN_RCVD.��󣬿ͻ�������һ��ACK����
��������׽��ֵ�״̬���ESTABLISHED.

    һ��Ӧ�ô���ESTABLISHED״̬����ͨ�����ַ������ر����������Ӧ�ó������رգ�
������������׽��ֹرա��������׽��ֵĹرձ��Ǳ����ģ���ͼ�����ֹرշ�������
�˽��ͣ��������رգ�Ӧ�ó����ᷢ��һ��FIN����Ӧ�ó������closesocket��
shutdownʱ����SD_SEND�����ڶ���������������Է�����һ��FIN���������׽��ֵ�״̬
����FIN_WAIT_1.��������£�ͨ�ŶԷ����Ӧһ��ACK�������ǵ��׽��ֵ�״̬��֮��
��FIN_WAIT_2.��Է�Ҳ�ر������ӣ���ᷢ��һ��FIN�������ǵĻ��������Ӧһ��ACK
�������������׽��ֵ�״̬��ΪTIME_WAIT.

    TIME_WAIT״̬Ҳ����2MSL�ȴ�״̬�����У�MSL�����ֶ������ʱ��
"(Maximum Segment Lifetime),��ʾһ�����ݰ��ڶ���֮ǰ�����������ϴ��ڶ೤ʱ�䡣
ÿ��IP��������һ��������ʱ�䡱(TTL) �ֶΣ�ŵ���ݼ�Ϊ��������ᱻ������һ������
�������ϵ�ÿ��·����ʱ��TTLֵ���������Ȼ��������ݣ�һ��Ӧ�ó������TIME_WAIT
״̬����ô�ͻ�һֱ����MSLʱ�������֮�ã�����һ����TCP�Ϳ��������һ��ACK��ʧ
��ǰ���£����·�������Ҳ����˵��FIN�ᱻ���´��ͳ�ȥ��MSLʱ������֮�õĵȴ�״̬
����֮���׽��ֱ����CLOSED״̬��

    ��ȡ�����رմ�ʩʱ��������·�������TIME_WAIT״̬����������ǰ�������У�ֻ
��һ������һ��FIN��������һ��ACK��Ӧ��Ȼ������һ����Ȼ�������ɵط������ݣ�ֱ��
��Ҳ���ر�Ϊֹ����ˣ���Ҫ����·���������ã���һ��·���У���ͬ���رգ���һ̨��
���������ͨ��˫����ͬʱҪ��رգ��������Է��ͳ�һ��FIN���ݰ��������������
��һ��FIN���ݰ�����󣬼�����ᷢ��һ��ACK���ݰ����ԶԷ���FIN��������Ӧ������
�Լ����׽�����ΪCLOSING״̬��������ӶԷ�������յ����һ��ACK��֮�������׽�
��״̬����TIME_WAIT.

    �����ر�ʱ����һ��·����ʵ����ͬ���رյı��壺�׽��ִ�FIN_WAIT_��״ֱ̬��
��� TIME_WAIT.ŵӦ�ó��򷢳�һ��FIN���ݰ���������ͬʱ��ӶԷ�������յ�һ��
FIN-ACK������������ͻᷢ��������������£��Է���ȷ���յ�Ӧ�ó����FIN��������
���Լ���FIN���������������Ӧ�ó������һ��ACk��������Ӧ��

    TIME_WAIT״̬����Ҫ��������TCP���Ӵ���2MSL�ȴ�״̬��ʱ�򣬹涨���ڽ����Ǹ�
���ӵ�һ���׽��ֲ��ɱ��ܾ�������׽����ɱ���IP�˿��Լ�Զ��IP�˿���ɣ���ĳЩ
TCPʵʩ������˵�����ǲ�����ܾ�����TIME_WAIT״̬���׽��ֽ������ӣ��ͻ�ʧ�ܣ���
����WSAEADDRINUSE����Ҫ�����һ���⣨���˵ȴ�ʹ���Ǹ����ض˿�������TIME_WAIT
״̬���׽��ֶԣ���һ���취��ʹ���׽���ѡ��SO_REFUSEADDR,���ǽ��ڵڣ��¶����
ѡ�������ϸ���ۣ�

    �����ر�����£�Ӧ�ó����ӶԷ��������һ��FIN��������һ��ACK��������Ӧ��
��ʱ��Ӧ�ó�����׽��ֻ���CLOSE_WAIT״̬�����ڶԷ��ѹر��Լ����׽��֣����Բ�
���ڷ��������ˡ���Ӧ�ó���ȴ��ͬ������һֱ�������ݣ�ֱ���Է����׽���״̬���
LAST_ACK.Ӧ�ó���ӶԷ��յ�һ��ACK���������׽��־ͻ���ת��CLOSED״̬��
*/