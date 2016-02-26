#pragma once

#include "PadsCommon.h"
#include "InputCommon/GCPadStatus.h"
#include "GcnPadsManager.h"
#include <vector>

#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>

class PadAtFrame
{
public:
	PadAtFrame(int f, GCPadStatus * ps) { frame = f; PadStatus = ps; }
	int frame;
	GCPadStatus * PadStatus;
};

class GcnPadManager
{
public:
	GcnPadManager(GcnPadsManager * p);
	~GcnPadManager();

	GcnPadsManager * PadsManager;
	std::vector <PadAtFrame *> PadQueue;
	HANDLE PadQueueMutex;

	void PadManipFunction(GCPadStatus * PadStatus, int controllerID);
};

void handlePacket(GcnPadsManager * padsManager, char * recvbuf, SOCKET s);
int distanceToChar(char * buff, char v);