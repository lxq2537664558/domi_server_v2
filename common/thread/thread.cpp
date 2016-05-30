#include "thread.h"
#ifndef WIN32
#include<sys/types.h>
#include<errno.h>
#endif//WIN32

CThread::CThread()
{
#ifdef WIN32
	m_hThread		= INVALID_HANDLE_VALUE;
	m_uThreadID		= 0;
#else//linux
	m_pthreadID			= 0;
	//��ʼ���߳����Ի��
	m_iInit = pthread_attr_init(&m_threadAtrr);
	/*Ĭ��ֵ
	scope			PTHREAD_SCOPE_PROCESS ���߳�������е������̷߳��������� 
	detachstate 	PTHREAD_CREATE_JOINABLE �߳��˳��󣬱������״̬���߳� ID��
	stackaddr		NULL ���߳̾���ϵͳ�����ջ��ַ�� 
	stacksize		0 ���߳̾���ϵͳ�����ջ��С�� 
	priority		0 ���̵߳����ȼ�Ϊ 0�� 
	inheritsched	PTHREAD_EXPLICIT_SCHED ���̲߳��̳и��̵߳������ȼ��� 
	schedpolicy		SCHED_OTHER ���̶߳�ͬ����������ʹ�� Solaris ����Ĺ̶����ȼ����߳̽�һֱ���У�ֱ������ռ����ֱ���߳�������ֹͣΪֹ�� 
	*/
#endif//WIN32
}

CThread::~CThread(){
	shutdown();
#ifndef WIN32
	//�����߳�����
	pthread_attr_destroy(&m_threadAtrr);
#endif//WIN32
}

//��õ�ǰ�߳�ID
int64	CThread::getCurrentThreadID(){
#ifdef WIN32
	return int64(GetCurrentThreadId());
#else//linux
	return int64(pthread_self());
#endif//WIN32
}

#ifdef WIN32
bool CThread::create(unsigned(__stdcall*start_routine)(void*),void*arg){
	shutdown();
	m_hThread = (HANDLE)::_beginthreadex(nullptr
										,0
										,start_routine	 /*�����̺߳���*/ 
										,arg
										,0
										,&m_uThreadID);
	if (m_hThread == INVALID_HANDLE_VALUE){
		shutdown();
		return false;
	}
	return true;
}
#else//linux
bool CThread::create(void*(*start_routine)(void*),void*arg){
	shutdown();
	int32 _ret = pthread_create(&m_pthreadID
							,&m_threadAtrr	//tattr=NULLΪȱʡ�߳�
							,start_routine	//�߳����к���ָ�롾�˺����˳��߳̽���(���˳�״̬����Ϊ�� start_routine ���ص�ֵ)��
							,arg);			//�߳����к�������

	//_ret == EAGAIN///ϵͳ���ƴ����µ��߳�
	//_ret == EINVAL///�ڶ�������������߳�����ֵ�Ƿ�
	return (_ret == 0);
}
#endif//WIN32

void CThread::shutdown(){
	terminate();
	wait_exit();
}

//��ֹ�߳�
int32 CThread::terminate(uint64 exit_code){
	int32 iRet = 0;
#ifdef WIN32
	if(m_hThread != INVALID_HANDLE_VALUE && m_hThread != 0)
		iRet = ::TerminateThread(m_hThread,(DWORD)exit_code);
#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_cancel(m_pthreadID);
#endif//WIN32
	return iRet;
}

// �ȴ��߳��˳�
int32 CThread::wait_exit(){
	int32 iRet = 0;
#ifdef WIN32
	if(m_hThread != nullptr && m_hThread != INVALID_HANDLE_VALUE){
		//�ȴ��߳���ֹ����һֱ���������̣߳�ֱ��ָ�����߳���ֹ��
		::WaitForSingleObject(m_hThread,10*1000/*INFINITE*/);
		/*����̱߳��*/
		DWORD	dwExit = 0;
		::GetExitCodeThread(m_hThread,&dwExit);
		//dwExit == STILL_ACTIVE;//��Ȼ������

		::CloseHandle( m_hThread );		/*�رվ��*/ 
		m_hThread = INVALID_HANDLE_VALUE;
	}

#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0){
		//�ȴ��߳���ֹ����һֱ���������̣߳�ֱ��ָ�����߳���ֹ��
		iRet = pthread_join(m_pthreadID	//���ȴ����̱߳�ʶ�����ڵ�ǰ�������Ҳ����Ƿ����̡߳�
			,nullptr/*void **status*/);//�߳��˳�״̬��һ��ʹ��NULL��

        //������ʽ����
		//pthread_detach(m_pthreadID);
	}
	m_pthreadID = 0;
#endif//WIN32
	return iRet;
}

//�Ƿ�������
bool CThread::is_runing(){
	bool _runing = false;
#ifdef WIN32
	/*����̱߳��*/
	DWORD	dwExit = 0;
	::GetExitCodeThread(m_hThread,&dwExit);
	if (dwExit == STILL_ACTIVE && ::WaitForSingleObject(m_hThread,1) == WAIT_TIMEOUT)
		_runing = true;

#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0){
		int pthread_kill_err;
		pthread_kill_err = pthread_kill(m_pthreadID,0);

		if(pthread_kill_err != ESRCH && pthread_kill_err != EINVAL)
			_runing = true;
	}
#endif//WIN32
	return _runing;
}

//�����̵߳����ȼ�
/*
policy = SCHED_OTHER
sched_param.sched_priority = priority;
*/
int32 CThread::set_sched_param(int32 policy,const struct sched_param *param){
	int32 iRet = 0;
#ifdef WIN32
	iRet = SetThreadPriority(m_hThread,policy);
#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_setschedparam(m_pthreadID,policy,param);
#endif//WIN32
	return iRet;
}

int32 CThread::get_sched_param(int32*policy,struct sched_param *param){
	int32 iRet = 0;
#ifdef WIN32
	if(policy) *policy = GetThreadPriority(m_hThread);
#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_getschedparam(m_pthreadID,policy,param);
#endif//WIN32
	return iRet;
}

//���̷߳����ź�
/*
�ź�		ȡֵ	Ĭ�϶���	���壨�����źŵ�ԭ��
SIGHUP		1		Term		�ն˵ĹҶϻ��������
SIGINT		2		Term		���Լ��̵��ж��ź�
SIGQUIT		3		Core		���Լ��̵��뿪�ź�
SIGILL		4		Core		�Ƿ�ָ��
SIGABRT		6		Core		����abort���쳣�ź�
SIGFPE		8		Core		��������
SIGKILL		9		Term		ɱ��
SIGSEGV		11		Core		�ηǷ�����(�ڴ�������Ч)
SIGPIPE		13		Term		�ܵ��𻵣���һ��û�ж����̵Ĺܵ�д����
SIGALRM		14		Term		����alarm�ļ�ʱ����ʱ�ź�
SIGTERM		15		Term		��ֹ
.......
*/
int32 CThread::kill(int32 sig){
	int32 iRet = 0;
#ifndef WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_kill(m_pthreadID,sig);
#endif//WIN32
	return iRet;
}

//�趨�̰߳�״̬
/*
@scope = PTHREAD_SCOPE_SYSTEM����)//���߳̽���ϵͳ�е������߳̽��о���
		/PTHREAD_SCOPE_PROCESS���ǰ󶨣�//���߳̽�������е������߳̽��о����� 
*/
int32 CThread::set_scope(int32 scope){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setdetachstate(&m_threadAtrr,scope);
#endif//WIN32
	return iRet;
}

int32 CThread::get_scope(int32*scope){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getscope(&m_threadAtrr,scope);
#endif//WIN32
	return iRet;
}

//�趨�̷߳���״̬ @detach_state = PTHREAD_CREATE_DETACHED�������̣߳�/PTHREAD_CREATE_JOINABLE���Ƿ����̣߳�
/*
*�����̣߳�	�������һ���߳�Ϊ�����̣߳�������߳������ַǳ��죬���ܿ�����pthread_create��������֮ǰ����ֹ��
����ֹ�Ժ�Ϳ��ܽ��̺߳ź�ϵͳ��Դ�ƽ����������߳�ʹ��
��������pthread_create���߳̾͵õ��˴�����̺߳ţ�ʹ��pthread_cond_timewait
*�Ƿ����̣߳�����ֹ�󣬱���Ҫ��һ���߳��� pthread_join ���ȴ��������򣬲����ͷŸ��̵߳���Դ�Թ����߳�ʹ�ã�����ͨ���ᵼ���ڴ�й©��
��ˣ������ϣ���̱߳��ȴ����뽫���߳���Ϊ�����߳���������
*/
int32 CThread::set_detach_state	(int32 detach_state){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setscope(&m_threadAtrr,detach_state);
#endif//WIN32
	return iRet;
}

int32 CThread::get_detach_state(int32*detach_state){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getdetachstate(&m_threadAtrr,detach_state);
#endif//WIN32
	return iRet;
}

// ջ�����������С
/*
1 ����������ܻᵼ��ϵͳ��Դ�˷ѡ����Ӧ�ó��򴴽������̣߳�������֪��Щ�߳���Զ���������ջ������Թر������������ͨ���ر���������������Խ�ʡϵͳ��Դ��
2 �߳���ջ�Ϸ���������ݽṹʱ��������Ҫ�ϴ����������������ջ�����
*��� guardsize Ϊ�㣬�򲻻�Ϊʹ�� attr �������߳��ṩ�������������� guardsize �����㣬���Ϊÿ��ʹ�� attr �������߳��ṩ��С����Ϊ guardsize �ֽڵ������������ȱʡ����£��߳̾���ʵ�ֶ���ķ��������������
*/
int32 CThread::set_guard_size(size_t guardsize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setguardsize(&m_threadAtrr,guardsize);
#endif//WIN32
	return iRet;
}

int32 CThread::get_guard_size(size_t*guardsize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getguardsize(&m_threadAtrr,guardsize);
#endif//WIN32
	return iRet;
}

//���õ��Ȳ���(SCHED_FIFO�������ȳ�����SCHED_RR��ѭ������ SCHED_OTHER��ʵ�ֶ���ķ������ĵ��Ȳ������ԡ�)
/*
SCHED_FIFO 
������ý��̾�����Ч���û� ID 0�������÷�ΧΪϵͳ (PTHREAD_SCOPE_SYSTEM) �������ȳ��߳�����ʵʱ (RT) �����ࡣ�����Щ�߳�δ�����ȼ����ߵ��߳���ռ��
������������̣߳�ֱ�����̷߳���������Ϊֹ�����ھ��н������÷�Χ (PTHREAD_SCOPE_PROCESS)) ���̻߳�����ý���û����Ч�û� ID 0 ���̣߳���ʹ�� SCHED_FIFO

SCHED_RR 
������ý��̾�����Ч���û� ID 0�������÷�ΧΪϵͳ (PTHREAD_SCOPE_SYSTEM)) ��ѭ���߳�����ʵʱ (RT) �����ࡣ�����Щ�߳�δ�����ȼ����ߵ��߳���ռ��������Щ�߳�û�з�����������
����ϵͳȷ����ʱ����ڽ�һֱִ����Щ�̡߳����ھ��н������÷�Χ (PTHREAD_SCOPE_PROCESS) ���̣߳���ʹ�� SCHED_RR������ TS �����ࣩ�����⣬��Щ�̵߳ĵ��ý���û����Ч���û� ID 0��

SCHED_FIFO �� SCHED_RR �� POSIX ��׼���ǿ�ѡ�ģ����ҽ�����ʵʱ�̡߳� 
*/
int32 CThread::set_sched_policy(int32 policy){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setschedpolicy(&m_threadAtrr,policy);
#endif//WIN32
	return iRet;
}

int32 CThread::get_sched_policy(int32*policy){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getschedpolicy(&m_threadAtrr,policy);
#endif//WIN32
	return iRet;
}

// �̳еĵ��Ȳ���
/*
inherit = PTHREAD_INHERIT_SCHED ��ʾ�½����߳̽��̳д������߳��ж���ĵ��Ȳ��ԡ��������� pthread_create() �����ж�������е������ԡ�
inherit = PTHREAD_EXPLICIT_SCHED��ȱʡֵ������ʹ�� pthread_create() �����е����ԡ�
*/
int32 CThread::set_inherit_sched(int32 inherit){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setinheritsched(&m_threadAtrr,inherit);
#endif//WIN32
	return iRet;
}

int32 CThread::get_inherit_sched(int32*inherit){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getinheritsched(&m_threadAtrr,inherit);
#endif//WIN32
	return iRet;
}

// ���Ȳ���
/*
sched_param param;����֧�����ȼ�������
newprio = 30;
param.sched_priority = newprio;
���Բ������ַ�ʽ֮һ������ pthreads ���ȼ���
1�� �������߳�֮ǰ�������������ȼ�����
2�����Ը��ĸ��̵߳����ȼ���Ȼ���ٽ������ȼ��Ļ���
   (��ȡ���в��� �������ȼ� �������߳� �ָ�ԭʼ���ȼ�)
*/
int32 CThread::set_sched_param(const struct sched_param *param){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setschedparam(&m_threadAtrr,param);
#endif//WIN32
	return iRet;
}

int32 CThread::get_sched_param(struct sched_param *param){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0)return m_iInit;
	iRet = pthread_attr_getschedparam(&m_threadAtrr,param);
#endif//WIN32
	return iRet;
}

// ջ��С
/*
һ������£�����ҪΪ�̷߳���ջ�ռ䡣ϵͳ��Ϊÿ���̵߳�ջ���� 1 MB������ 32 λϵͳ���� 2 MB������ 64 λϵͳ���������ڴ棬���������κν����ռ䡣ϵͳ��ʹ�� mmap() �� MAP_NORESERVE ѡ�������з���
PTHREAD_STACK_MIN=Ĭ��ջ��С
stacksize ���Զ���ϵͳ�����ջ��С�����ֽ�Ϊ��λ����size ��ӦС��ϵͳ�������Сջ��С����� size Ϊ�㣬��ʹ��ȱʡ��С
PTHREAD_STACK_MIN �������߳������ջ�ռ�������ջ�ռ�û�п���ִ��Ӧ�ó������������߳�����Ҫ��
��:size = (PTHREAD_STACK_MIN + 0x4000);
ָ��ջʱ����Ӧʹ�� PTHREAD_CREATE_JOINABLE �����̡߳��ڸ��̵߳� pthread_join(3C) ���÷���֮ǰ�������ͷŸ�ջ
*/
int32 CThread::set_stack_size(size_t stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setstacksize(&m_threadAtrr,stacksize);
#endif//WIN32
	return iRet;
}

int32 CThread::get_stack_size(size_t*stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getstacksize(&m_threadAtrr,stacksize);
#endif//WIN32
	return iRet;
}

// ջ��ַ�ʹ�С
/*
PTHREAD_STACK_MIN=Ĭ��ջ��С
void *base;
size_t size;
int ret;
base = (void *) malloc(PTHREAD_STACK_MIN + 0x4000);
ret = pthread_attr_setstack(&tattr, base,PTHREAD_STACK_MIN + 0x4000);
*/
int32 CThread::set_stack(void *stackaddr,size_t stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setstack(&m_threadAtrr,stackaddr,stacksize);
#endif//WIN32
	return iRet;
}

int32 CThread::get_stack(void **stackaddr,size_t*stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getstack(&m_threadAtrr,stackaddr,stacksize);
#endif//WIN32
	return iRet;
}