#include "stdafx.h"
#include "GcnPadsManager.h"
#include "PlatformDependent.h"
#include "json.h"
#include "Core/Movie.h"
#include <thread>
#include <string>
#include <algorithm>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

GcnPadsManager::GcnPadsManager()
{
	std::thread t(LaunchTcpServer, this);
	t.detach();
}

GcnPadsManager::~GcnPadsManager()
{
}

void GcnPadsManager::PadManipFunction(GCPadStatus * PadStatus, int controllerId)
{
	int current_frame = Movie::g_currentFrame;

	Json::Value padJson;
	padJson["frame"] = current_frame;
	padJson["controller"] = controllerId;
	padJson["button"] = PadStatus->button;
	padJson["stickX"] = PadStatus->stickX;
	padJson["stickY"] = PadStatus->stickY;
	padJson["substickX"] = PadStatus->substickX;
	padJson["substickY"] = PadStatus->substickY;
	padJson["triggerLeft"] = PadStatus->triggerLeft;
	padJson["triggerRight"] = PadStatus->triggerRight;
	padJson["analogA"] = PadStatus->analogA;
	padJson["analogB"] = PadStatus->analogB;
	std::string json = padJson.toStyledString();

	this->mutexTcpClients.lock();
	std::for_each(
		this->tcpClients.begin(), 
		this->tcpClients.end(), 
		[&](TcpClient * c){ c->write(json.c_str(), json.size()); });
	this->mutexTcpClients.unlock();

	this->mutexExternalActions.lock();
	std::for_each(
		this->externalActions.begin(), 
		this->externalActions.end(), 
		[&](ExternalAction * action){ action->PerformAction(PadStatus, current_frame, controllerId); });
	std::remove_if(
		this->externalActions.begin(), 
		this->externalActions.end(), 
		[&](ExternalAction * action){ return action->frame < current_frame; });
	this->mutexExternalActions.unlock();
}

void clientManager(GcnPadsManager * padsManager, TcpClient * client)
{

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	std::string nextJson("");
	std::string recvstr;
	std::string recvstrPreAt;
	std::string recvstrPostAt;
	bool receivingJson = false;
	int atPos = 0;

	// Receive until the peer shuts down the connection
	do {

		memset(recvbuf, 0, DEFAULT_BUFLEN);
		client->read(recvbuf, recvbuflen);

		int iResult = 1;

		if (iResult > 0)
		{
			recvstr = std::string(recvbuf);
			while (recvstr.size() > 0)
			{
				atPos = recvstr.find('@');
				if (atPos == std::string::npos)
				{
					if (receivingJson)
					{
						nextJson += recvstr;
						break;
					}
					else
					{
						break;
					}
				}
				else 
				{
					recvstrPreAt = recvstr.substr(0, atPos - 1);
					recvstrPostAt = recvstr.substr(atPos + 1, recvstr.size() - atPos - 1);
					recvstr = recvstrPostAt;
					if (receivingJson)
					{
						nextJson += recvstrPreAt;
						receivingJson = false;
						/* processJson(nextJson); */ 
					}
					else
					{
						receivingJson = true;
					}
				}
			}
		}
	} while (1);
}