#pragma once

#ifdef WIN32
#include <winsock2.h>
#include <mswsock.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment (lib,"wsock32.lib")
#else
#include <netinet/in.h>//������<sys/socket.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>//ϵͳ���Ͷ���
#include <arpa/inet.h>
#include <fcntl.h>//����ģʽͷ�ļ�
#include <netdb.h>
#include <unistd.h>//close����
#endif
#include <errno.h>	//�������ļ��ͻ�����errno������int geterror()��

#include "../common/basic/basicTypes.h"

#define MaxBuffLen 1024*10			// 10k
#define MaxBuffLenLimit 1024*20		// 20k

// Э���ͷ
struct PacketHead
{
	uint16	uHeadFlag;		// ��ͷ��ʾ
	uint16	uPacketSize;	// ����С
	PacketHead() {memset( this, 0, sizeof( *this ) );	}
};

class CTcpServer;
class CTcpSession		//״̬�С�������|������|�Ͽ��С�
{
public:
	CTcpSession();
	~CTcpSession();

public:
	inline int getPendingLen()	{ return m_inbufLen - m_readBegin; }// ʣ��δ������ֽ���
	inline int getFreeLen()		{ return MaxBuffLen - m_inbufLen; }	// ����append�ĳ���

	void initialize();	// ��ʼ��
	bool send(const char* pBuffer,int32 nSize);
	void disconnect();			// �Ͽ�����
	ulong remote_address();		// Զ�̵�ַ
	const char*	remote_ip();	// Զ�̵�ַ��ip

public:
	// char* p = new char[100]; delete []p;
	CTcpServer*	m_tcpServer;	// ������server
	int m_fd;					// socket fd
	int m_id;					// session id(���ϲ��õ�)
	int m_mask;					// ��ʾ
	char* m_income;	
	char* m_outcome;
	int m_inbufLen;				// inbuff ����λ��
	int m_readBegin;			// ��һ�ο�ʼ��ȡ��λ��
};
