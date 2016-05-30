/******************************************************************** 
����ʱ��:        2015/06/22 14:44
�ļ�����:        GameServer.cpp
�ļ�����:        Domi
*********************************************************************/

#include <signal.h>
#include <iostream>
#include "domiServer.h"
#include "../common/log/log.h"

// ������Ϣ
static uint64 STnowTime = getSecond();
static uint32 STnum = 0;
static uint32 STCount = 0;
static int g_exiting = 0;
static int g_stop = 0;

static void	onBreak(int){
#ifdef WIN32
	::AllocConsole();
#endif// WIN32
	g_exiting++;
	CDomiServer::GetSingleton().denyServices();
}

#ifdef WIN32
HANDLE	CDomiServer::m_hServerEvent = nullptr;
#endif

CDomiServer::CDomiServer(){
}

CDomiServer::~CDomiServer(){
}

void CDomiServer::installBreakHandlers(){
#ifdef WIN32
	::signal(SIGBREAK, onBreak);
#else
	::signal(SIGSTOP, onBreak);//ֹͣ����
	::signal(SIGTSTP, onBreak);//�ն��Ϸ�����ֹͣ�ź�
	::signal(SIGKILL, onBreak);//ɱ�������ź�
	::signal(SIGHUP, onBreak);//�ڿ����ն����ǹ����ź�, ���߿��ƽ��̽���
#endif
	::signal(SIGTERM, onBreak);	//ʹ��killָ��ʱ
	::signal(SIGINT, onBreak);	//Ctrl+C��ʱ������յ��źţ�����OnBreak����
}

bool CDomiServer::loadConfig(){
	return true;
}

void CDomiServer::showToConsole(const char* pFormat, ...){
	if (!pFormat || !*pFormat)
		return;

	char szStrint[2048] = { 0 };
	va_list	argptr;
	va_start(argptr, pFormat);
	dVsprintf(szStrint, sizeof(szStrint), pFormat, argptr);
	va_end(argptr);

	CConsoleColor clColor(_BIT32(CConsoleColor::color_green) | _BIT32(CConsoleColor::color_intensity));
	dPrintf("%s\n", szStrint);
}

void CDomiServer::setServicesTitle(const char* pTitle, ...){
#ifdef WIN32 // ���ÿ���̨����
	if (!pTitle && m_szTitle[0])
		return;

	char szTitle[256] = { 0 };
	if (pTitle) {
		va_list	argptr;
		va_start(argptr, pTitle);
		dVsprintf(szTitle, sizeof(szTitle), pTitle, argptr);
		va_end(argptr);
	}

	if (!m_tmStartTime)
		m_tmStartTime = getTime();

	tm	tmTime;
	getLocalTime(&tmTime, &m_tmStartTime);
	dSprintf(m_szTitle, sizeof(m_szTitle), "%s StartTime: %02d-%02d %02d:%02d:%02d", szTitle, tmTime.tm_mon + 1, tmTime.tm_mday, tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
	::SetConsoleTitle(m_szTitle);
#endif
}

void CDomiServer::showServerInfo(){
	showToConsole("***************************************************************");
	showToConsole("* Game Server Application Ver %d.%d", 0, 0);
	showToConsole("***************************************************************");
	setServicesTitle("Game:<%.4d>", 1);
}

bool CDomiServer::initialize(uint16 uServerID) {
	showServerInfo();

#ifdef WIN32
	char szEvent[256] = { 0 };	/*Ψһ�¼�*/
	dSprintf(szEvent, sizeof(szEvent), "%s_%d", "DomiServer", uServerID);
	// ������Դ�ָ���¼������¼���˵���Ѿ����ڸý���
	m_hServerEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, szEvent);
	if (m_hServerEvent)
		return false;
	m_hServerEvent = ::CreateEvent(nullptr, FALSE, FALSE, szEvent);
#endif

	char logprev[256] = { 0 };
	dSprintf(logprev, sizeof(logprev), "%s_%u", "test_log_", uServerID);
	CLog::initialize(NULL, logprev);// ��־��ʼ��

	// ���������CLog::initilize֮��
	// �ػ����� TODO
	if (m_bInitFlag) return true;

	::srand((uint32)getTime());		// �����������
	m_uServerID = uServerID;		// ���÷�����id

	if (!loadConfig()) {// load ����������
		CLog::error("�����ļ�װ��ʧ��!...");
		return false;
	}

	installBreakHandlers();
	m_tmStartTime = getTime();
	m_bInitFlag = true;
	return true;
}

bool CDomiServer::startServices(){
	if (!m_el.StartServer(7777)){
		return false;
	}
	return true;
}

void CDomiServer::denyServices(){
	m_el.StopServer();
}

void CDomiServer::stopServices(){
	CLog::info("�������ѹر�!...");
	// ���ֵײ��shutdown
	CLog::shutdown();
}