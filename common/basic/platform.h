#pragma once
#include "basicTypes.h"

// ������
extern long get_error();//��ô�����
extern const char*	get_error_str(long _error);//��ô����ַ���
extern const char*	get_herror_str();//��ô����ַ���(linux�µ���ʱ�޴��������ӡ���)
extern const char*	get_herror_str(long _error);//��ô����ַ���

// �ļ�Ŀ¼���
extern bool	findDirectory(pc_str _dir);//��ѯĿ¼�Ƿ����
extern bool	createDirectory(pc_str _dir,char _break = '/');//����Ŀ¼