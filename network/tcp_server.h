// tcp ����

#pragma once
#include <map>
#include <queue>
#include "tcp_session.h"
#include "../common/basic/basicTypes.h"
#include "../common/thread/thread.h"

#ifndef WIN32
#define HAVE_EPOLL
#endif // WIN32


// aenet
extern "C" {
#include "ae.h"
#include "anet.h"
}

class CTcpServer
{
public:
	CTcpServer();
	virtual ~CTcpServer();

private:
	inline void SetListenPort(uint16 port)	{ m_port = port; }

public:
	void Initialize();				// ��ʼ��
	bool StartServer(uint16 port);	// tcpserver��ʼ��
	bool StopServer();				// �رշ���

	static void DoAccept(aeEventLoop* el, int fd, void* privdata, int mask);
	static void DoRead(aeEventLoop* el, int fd, void* privdata, int mask);
	static THREAD_RETURN _el_thread_(void* _param);
private:
	uint16 m_port;				//�����˿�

public:
	aeEventLoop* m_event_loop;	//eventloop
	int m_stop;		// �˳���ʾ
	uint16 m_idGen;						//sessionid������
	std::map<int, CTcpSession*> m_sessions;
	std::queue<CTcpSession*> m_free;
};