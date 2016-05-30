/******************************************************************** 
����ʱ��:        2015/06/22 14:07
�ļ�����:        main.cpp
�ļ�����:        Domi
����˵��:        main   
����˵��:         
*********************************************************************/

#include <iostream>
#include "logic/domiServer.h"
#include "common/basic/timeFunction.h"

int main(int argc, char* argv[])
{
	if (!CDomiServer::GetSingleton().initialize(7)){
		printf("CGameServer::Initialize()ʧ��!\n");
		return 0;
	}
	
	if (!CDomiServer::GetSingleton().startServices()) {
		printf(":StartServices()��������ʧ��!\n");
	}
	CDomiServer::GetSingleton().stopServices();
	dSleep(1000);
	return 0;
}