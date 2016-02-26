#pragma once

#include "PlatformDependent.h"
#include "ExternalActions.h"
#include "PadsCommon.h"
#include <vector>
#include <mutex>

class GcnPadsManager
{
public:
	GcnPadsManager();
	~GcnPadsManager();
	void PadManipFunction(GCPadStatus * PadStatus, int controllerID);

	std::vector<ExternalAction *> externalActions;
	std::vector<TcpClient *> tcpClients;
	std::mutex mutexExternalActions;
	std::mutex mutexTcpClients;
};

void clientManager(GcnPadsManager * padsManager, TcpClient * client);
