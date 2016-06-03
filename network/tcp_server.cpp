#include "tcp_server.h"
#include <functional>
#include "../common/log/log.h"

static char g_err_string[1024] = {0};
CTcpServer::CTcpServer()
	: m_port(0),
	m_idGen(0),
	m_event_loop(NULL){
#ifdef WIN32
	WSADATA WSAData;
	WSAStartup(0x0201, &WSAData);
#endif
	m_stop = 0;
}

CTcpServer::~CTcpServer(){
#ifdef WIN32
	WSACleanup();
#endif
	if (m_event_loop){
		aeDeleteEventLoop(m_event_loop);
	}

	std::map<int, CTcpSession*>::iterator it = m_sessions.begin();
	for (; it != m_sessions.end(); ++it) {
		if (it->second) delete it->second;
	}

	CTcpSession* lp_t = nullptr;
	while (!m_free.empty()) {
		lp_t = m_free.front();
		m_free.pop();
		if (lp_t) delete lp_t;
	}
}

void CTcpServer::Initialize() {
	m_sessions.clear();
	CTcpSession* pSession;
	for (int i = 0; i < 5;++i){
		pSession = new CTcpSession();//new �����ţ���ʾ��ʽ���ù��캯�����������ű�ʾ��ʽ���ù��캯��
		pSession->m_id = i;
		pSession->m_tcpServer = this;
		m_free.push(pSession);
		this->m_idGen = i;
	}
}

bool CTcpServer::StartServer(uint16 port) {
	Initialize();

	m_event_loop = aeCreateEventLoop(1024 * 10);
	int fd = anetTcpServer(g_err_string, port, NULL, 1024);
	if (ANET_ERR == fd) {
		fprintf(stderr, "Open port %d error: %s\n", port, g_err_string);
		return false;
	}
	
	if (aeCreateFileEvent(m_event_loop, fd, AE_READABLE, DoAccept, this) == AE_ERR) {
		fprintf(stderr, "Unrecoverable error creating server.ipfd file event.");
		return false;
	}

	aeMain(m_event_loop);
	return true;
}

bool CTcpServer::StopServer(){
	aeStop(m_event_loop);
	CLog::info("[CTcpServer::StopServer],��ʼ�ر� tcp server����");
	timeval delay = { 0, 0 };

	CLog::info("[CTcpServer::StopServer],�߳�ȫ���رա���");
	return true;
}

// static method
void CTcpServer::DoAccept(aeEventLoop* el, int fd, void* privdata, int mask){
	int cfd, cport;
	char ip_addr[128] = { 0 };
	cfd = anetTcpAccept(g_err_string, fd, ip_addr, 128, &cport);
	printf("Connected from %s:%d\n", ip_addr, cport);
	
	CTcpServer* pEl = (CTcpServer*)privdata;
	if (!pEl){
		fprintf(stderr, "privdata Error! \n");
		return;
	}

	// ������Ҫ����һ��session
	CTcpSession* pSession = nullptr;
	if (pEl->m_free.empty()){
		printf("fuck ----->\n");
		pSession = new CTcpSession();
		pSession->m_id = ++pEl->m_idGen;
		pSession->m_tcpServer = pEl;
	}else {
		pSession = pEl->m_free.front();
		pEl->m_free.pop();
	}
	if (!pSession) {
		fprintf(stderr, "Push Session Error! \n");
		return;
	}
	pEl->m_sessions[pSession->m_id] = pSession;
	CLog::info("sid = %d\n", pSession->m_id);
	if (aeCreateFileEvent(el, cfd, AE_READABLE, DoRead, pSession) == AE_ERR){
		fprintf(stderr, "client connect fail: %d\n", fd);
#ifdef WIN32	
		closesocket(fd);
#else
		close(fd);
#endif
	}
}

void CTcpServer::DoRead(aeEventLoop* el, int fd, void* privdata, int mask){
	char buffer[1024] = { 0 };
	int res;
#ifdef WIN32
	res = recv(fd, buffer, 1024, 0);
#else
	res = read(fd, buffer, 1024);
#endif
	if (res <= 0){
		fprintf(stderr, "rece err len: %d\n", res);
		CTcpSession* pSession = (CTcpSession*)privdata;
		pSession->m_tcpServer->m_free.push(pSession);
		pSession->m_tcpServer->m_sessions.erase(pSession->m_id);
		aeDeleteFileEvent(el, fd, AE_READABLE);
#ifdef WIN32	
		closesocket(fd);
#else
		close(fd);
#endif
	} else {
		//res = send(fd, buffer, 1024, 0);
		printf("read len = %d\n",res);
		printf("%s\n", buffer);
	}
}

THREAD_RETURN CTcpServer::_el_thread_(void* _param){
	CLog::info("[CTcpServer]�߳�����,id = %d����", CThread::getCurrentThreadID());
	CTcpServer* _this = (CTcpServer*)_param;
	if (!_this) {
#ifdef WIN32
		return -1;
#else
		return nullptr;
#endif
	}

	_this->StartServer(17777);
	_this->m_stop = 1;
	printf("prt = %d\n", _this->m_event_loop);
	CLog::info("[CTcpServer]�߳̽�������");
	return 0;
}

/*
bool CTcpServer::OnProcessPacket(CTcpContext* pContext)
{
while (pContext->m_inbuf && pContext->getPendingLen() > sizeof(PacketHead))
{
//һ��Э���������ͷ��û���꣬�����ѭ�������ߵȴ���һ��libeventʱ�����ѭ����
PacketHead* pHead = (PacketHead*)(pContext->m_inbuf + pContext->m_readBegin);
if (pHead->uPacketSize > MaxBuffLen || pHead->uPacketSize < sizeof(PacketHead)){ // ��Ϣ��ͷ���Ϸ�
pContext->disconnect();
return false;
}

if (pHead->uPacketSize > pContext->getPendingLen()){ // ʣ�����ݲ���һ������������
printf("ʣ�����ݲ���һ�����������գ�\n");
break;
}

if (pContext->m_readBegin > pContext->m_inbufLen){ // �������ˣ��Ѷ����ֽ�����Ȼ����������ֱ�Ӷϵ�����
pContext->disconnect();
return false;
}

// ѹ��message
{
CCritLocker connMapLock(ConnectLock());
CTcpSession* pConn = ConnectFind(pContext);
if (pConn)
this->m_clMessageQueue.push(pConn->getApplyKey(), pContext, pContext->m_inbuf + pContext->m_readBegin + sizeof(PacketHead), pHead->uPacketSize - sizeof(PacketHead));
}

//������һ��Э���
pContext->m_readBegin += pHead->uPacketSize;
}

// ����һ���������ݣ��ƶ�����ʼλ��
int pending = pContext->getPendingLen();
if (pending > 0)	// ����Ҫ�ƶ����ֽ�
memmove(pContext->m_inbuf, pContext->m_inbuf + pContext->m_readBegin, pending);

pContext->m_inbufLen = pending;
pContext->m_readBegin = 0;

return true;
}
*/
