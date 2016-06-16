﻿#pragma once
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <crtdbg.h>
#else//linux
#include <pthread.h>
#include <errno.h>
#endif//WIN32

#ifdef __cplusplus
extern "C" {
#endif

// 互斥
class CMutex
{
private:
#ifdef WIN32
	typedef CRITICAL_SECTION _MUTEX;
#else//WIN32
	typedef pthread_mutex_t _MUTEX;
#endif//WIN32

private:
	_MUTEX	m_critSect;
public:
	inline _MUTEX*get_mutex()	{return &m_critSect;}

#ifdef WIN32
public:
	CMutex()	{::InitializeCriticalSection(&m_critSect);}
	~CMutex()	{::DeleteCriticalSection(&m_critSect);}

public:
	inline void	lock(){::EnterCriticalSection(&m_critSect);}
	inline void	unlock(){::LeaveCriticalSection(&m_critSect);}
	inline bool	tryLock(){return (::TryEnterCriticalSection(&m_critSect) == 0);}
#else//WIN32
public:
	CMutex(){
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);

		pthread_mutex_init(&m_critSect,&attr);
	}
	virtual~CMutex(){pthread_mutex_destroy(&m_critSect);}

public:
	inline void lock(){pthread_mutex_lock(&m_critSect);}
	inline void unlock(){pthread_mutex_unlock(&m_critSect);}
	inline bool tryLock(){return (pthread_mutex_trylock(&m_critSect) != EBUSY);}
#endif//WIN32
};


//临界区锁
class CCritLocker
{
private:
	CMutex*	m_pLockable;

public:
	CCritLocker(CMutex* pLockable)	{m_pLockable = pLockable;	lock();	}
	CCritLocker(CMutex& clLockable)	{m_pLockable= &clLockable;	lock();	}
	virtual~CCritLocker()			{unlock();}

public:
	inline void	lock(){	if (m_pLockable)m_pLockable->lock();}
	inline void	unlock(){if (m_pLockable)m_pLockable->unlock();	}
};

#ifdef __cplusplus
}
#endif

/*
* linux
*
//静态锁【全局变量】
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER
*
//注销一个互斥锁
int pthread_mutex_destroy(pthread_mutex_t *mutex)
*
//加锁
int pthread_mutex_lock(pthread_mutex_t *mutex)
*
//解锁
int pthread_mutex_unlock(pthread_mutex_t *mutex)
*
//测试加锁
int pthread_mutex_trylock(pthread_mutex_t *mutex)
-锁已经被占据时返回EBUSY
*
*/