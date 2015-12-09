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

//// 根据实际研究，WSASend&WSARecv的lpNumberOfBytesSent是一个在操作结束时会更新的数据，所以，这个数据必须全程有效。。。
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
TCP状态

    作为一名Winsock程序员,通常没必要了解实际的TCP状态.但了解TCP状态,就能更好的
地理解Winsock API调用如何基层协议中的改变产生影响.此外,许多程序员在关闭套接字时
,会碰到一个常见问题;围绕套接字关闭的TCP状态是我们目前最感兴趣的问题.

    对每个套接字来说，它的初始状态都是CLOSED.诺客户机初始化了一个连接,就会向服务
器发送一个SYN包,同时将客户机套接字状态置为SYN_SENT.服务器收到 SYN包后，会发出一
个"SYN-ACK"包.作为客户机,需要用一个ACK包对它做出反应.此时,客户机的套接字会变成
ESTABLISHED状态.如果服务器一直不发送"SYN-ACK"包,客户机就会超时,并返回CLOSED状态.

    诺一个服务器的套接字同一个本地接口和端口绑定起来,并在它上面进行监听，那么套
接字的状态便是LISTEN.客户机试图与之连接时，服务器就会收到一个SYN包，并用一个
SYN-ACK包做出响应．服务器套接字的状态就变成SYN_RCVD.最后，客户机发出一个ACK包，
令服务器套接字的状态变成ESTABLISHED.

    一旦应用处于ESTABLISHED状态，可通过两种方法来关闭它．如果由应用程序来关闭，
便叫作”主动套接字关闭”；否则，套接字的关闭便是被动的．下图对两种关闭方法进行
了解释．如主动关闭，应用程序便会发出一个FIN包。应用程序调用closesocket或
shutdown时（把SD_SEND当作第二个参数），会向对方发出一个FIN包，而且套接字的状态
则变成FIN_WAIT_1.正常情况下，通信对方会回应一个ACK包，我们的套接字的状态随之变
成FIN_WAIT_2.如对方也关闭了连接，便会发出一个FIN包，我们的机器则会响应一个ACK
包，并将己方套接字的状态置为TIME_WAIT.

    TIME_WAIT状态也叫作2MSL等待状态．其中，MSL代表”分段最长生存时间
"(Maximum Segment Lifetime),表示一个数据包在丢弃之前，可在网络上存在多长时间。
每个IP包都含有一个”生存时间”(TTL) 字段，诺它递减为０，包便会被丢弃．一个包经
过网络上的每个路由器时，TTL值都会减１，然后继续传递．一旦应用程序进入TIME_WAIT
状态，那么就会一直持续MSL时间的两倍之久．这样一来，TCP就可以在最后一个ACK丢失
的前提下，重新发送它，也就是说，FIN会被重新传送出去．MSL时间两倍之久的等待状态
结束之后，套接字便进入CLOSED状态。

    采取主动关闭措施时，有两个路径会进入TIME_WAIT状态．在我们以前的讨论中，只
有一方发出一个FIN，并接受一个ACK响应．然而，另一方仍然可以自由地发送数据，直到
它也被关闭为止．因此，需要两个路径发挥作用．在一个路径中（即同步关闭），一台计
算机和它的通信双方会同时要求关闭；计算机向对方送出一个FIN数据包，并从它那里接
受一个FIN数据包．随后，计算机会发出一个ACK数据包，对对方的FIN包作出响应，并将
自己的套接字置为CLOSING状态．计算机从对方那里接收到最后一个ACK包之后，它的套接
字状态会变成TIME_WAIT.

    主动关闭时，另一个路径其实就是同步关闭的变体：套接字从FIN_WAIT_１状态直接
变成 TIME_WAIT.诺应用程序发出一个FIN数据包，但几乎同时便从对方那里接收到一个
FIN-ACK包，这种情况就会发生。在这种情况下，对方会确认收到应用程序的FIN包，并送
出自己的FIN包。对于这个包，应用程序会用一个ACk包做出响应．

    TIME_WAIT状态的主要作用是在TCP连接处于2MSL等待状态的时候，规定用于建立那个
连接的一对套接字不可被拒绝．这对套接字由本地IP端口以及远程IP端口组成．对某些
TCP实施方案来说，它们不允许拒绝处于TIME_WAIT状态的套接字建立连接，就会失败，并
返回WSAEADDRINUSE错误．要解决这一问题（除了等待使用那个本地端口来脱离TIME_WAIT
状态的套接字对），一个办法是使用套接字选项SO_REFUSEADDR,我们将在第９章对这个
选项进行详细讨论．

    被动关闭情况下，应用程序会从对方那里接受一个FIN包，并用一个ACK包做出响应．
此时，应用程序的套接字会变成CLOSE_WAIT状态．由于对方已关闭自己的套接字，所以不
能在发送数据了。但应用程序却不同，它能一直发送数据，直到对方的套接字状态变成
LAST_ACK.应用程序从对方收到一个ACK包后，它的套接字就会逆转成CLOSED状态．
*/