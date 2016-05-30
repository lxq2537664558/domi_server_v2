// server

#include "../common/singleton/singleton.h"
#include "../common/basic/stringFunctions.h"
#include "../network/tcp_server.h"

class CDomiServer : public Singleton<CDomiServer>
{
public:
	CDomiServer();
	~CDomiServer();

public:
	void installBreakHandlers();
	bool loadConfig();
	void showServerInfo();
	void showToConsole(const char* pFormat, ...);
	void setServicesTitle(const char* pTitle, ...);
	bool initialize(uint16 uServerID);	// ��ʼ��server
	bool startServices();				// ����server
	void denyServices();				// �ܾ�����
	void stopServices();				// �ر�server
	
public:
	CTcpServer	m_el;					// eventloop
	bool		m_bInitFlag;			// server�Ƿ��Ѿ���ʼ�����
	uint32		m_uLoadFlag;			// ��ȡ������ɱ�ʾ
	uint64		m_tmStartTime;			// ����ʱ��
	char		m_szTitle[256];			// ����
	uint8		m_uServerType;			// ����������
	uint16		m_uServerID;			// ������id
#ifdef WIN32
	static HANDLE m_hServerEvent;		// ȫ���¼������windows����Ψһ���̱�ʾ
#endif
};
