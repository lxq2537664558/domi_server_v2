// �ڴ������غ���

#pragma once
#include <iostream>
#include <stdio.h>
#include "basicTypes.h"

#ifdef WIN32	//WIN32
extern int32 dMemcpy(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);  //�ڴ濽��
extern int32 dMemmove(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount); //�ڴ��ƶ�
#else	// linux
extern void* dMemcpy(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);  //�ڴ濽��
extern void* dMemmove(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount); //�ڴ��ƶ�
#endif
