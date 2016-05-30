#pragma once
#include "basicTypes.h"
#include <time.h>
#ifdef WIN32	//WIN32
#include <winsock2.h>//δ�������綨���ͻ
#include <windows.h>
#else	//linux
#include <sys/time.h>
#endif // WIN32

#ifndef WIN32
struct SYSTEMTIME
{
	uint16 wYear;
	uint16 wMonth;
	uint16 wDayOfWeek;
	uint16 wDay;
	uint16 wHour;
	uint16 wMinute;
	uint16 wSecond;
	uint16 wMilliseconds;
};
#endif // WIN32

extern uint32	getTickCount	();//--- ���ϵͳ��������
extern uint64	getTickCount64	();//--- ���ϵͳ��������(64λ)
extern uint64	getSystemTick64	();//--- ���ϵͳ��������(64λ)����Ҫ�ֶ����¡�
extern uint64	setSystemTick64	();//--- ����ϵͳ��������
extern uint64	getSecond		();//--- ���ϵͳ��ʱ��
extern uint64	getMillisecond	();//--- ���ϵͳ����ʱ��
extern uint64	getMicroseconds	();//--- ���ϵͳ΢��ʱ��
extern uint64	getClockTime	();//--- ���ʱ��ʱ��(����)
extern void	    setStartTime    ( uint64 time );//--- ����ϵͳ��ʼʱ�䣬���ڵ���
extern uint64	getTime			();//--- ���ϵͳʱ��
extern uint32	getTime32		();
extern uint64	setSysTime		();//--- ���ϵͳʱ��
extern uint64	getSysTime		();
extern uint64	getDayBeginTime	(); //--- ��ý��տ�ʼʱ��
extern int32	getLocalTime	(struct tm* _Tm,const uint64* _Time); //--- ��ñ���ʱ��
extern int32	getLocalTime	(struct tm* _Tm,uint64 _Time);
extern int32	diffTimeDay		(uint32 _early,uint32 _late);//--- ���ʱ���������
extern int32	diffTimeWeek	(uint64 _early,uint64 _late);//--- ���ʱ���������
extern int32	diffTimeMonth	(uint64 _early,uint64 _late);//--- ���ʱ���������
extern pc_str	time_format		(uint64 _time);//--- ʱ���ʽ��(YYYY-MM-DD HH:MM:SS)
extern void		dSleep			(uint32 millisecond);//--- ˯��
extern bool		isDayExpired	(uint64 _early, uint64 _late, uint16 _limit);//--- �����Ƿ���
extern bool		isDayExpired	(uint16 _diff, uint16 _limit);
extern void		getLocalTime	(SYSTEMTIME&systime);
