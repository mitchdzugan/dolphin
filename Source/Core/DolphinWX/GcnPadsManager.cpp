#include "stdafx.h"
#include "GcnPadsManager.h"
#include "PlatformDependent.h"
#include "json.h"
#include "Core/Movie.h"
#include <thread>
#include <string>
#include <algorithm>
#include <iostream>

#define DEFAULT_BUFLEN 512

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

void processJson(GcnPadsManager * padsManager, const char * json)
{
	Json::Value actions;
	Json::Reader reader;
	if (reader.parse(json, actions, false))
	{
		if (actions["PadManipActions"].isArray())
		{
			for (int i=0; i<actions["PadManipActions"].size(); i++)
			{
				if (actions["PadManipActions"][i].isObject())
				{
					GCPadStatus * padStatus = new GCPadStatus();
					padStatus->button = actions["PadManipActions"][i].get("button", 0).asInt();
					padStatus->stickX = actions["PadManipActions"][i].get("stickX", 128).asInt();
					padStatus->stickY = actions["PadManipActions"][i].get("stickY", 128).asInt();
					padStatus->substickX = actions["PadManipActions"][i].get("substickX", 128).asInt();
					padStatus->substickY = actions["PadManipActions"][i].get("substickY", 128).asInt();
					padStatus->triggerLeft = actions["PadManipActions"][i].get("triggerLeft", 0).asInt();
					padStatus->triggerRight = actions["PadManipActions"][i].get("triggerRight", 0).asInt();
					padStatus->analogA = actions["PadManipActions"][i].get("analogA", 0).asInt();
					padStatus->analogB = actions["PadManipActions"][i].get("analogB", 0).asInt();

					padsManager->mutexExternalActions.lock();
					padsManager->externalActions.push_back(new AlterPadStatus(
						padStatus,
						actions["PadManipActions"][i].get("frame", -1).asInt(),
						actions["PadManipActions"][i].get("controller", -1).asInt()
						));
					padsManager->mutexExternalActions.unlock();
				}
			}
		}
		if (actions["TakeScreenshotActions"].isArray())
		{
			for (int i=0; i<actions["TakeScreenshotActions"].size(); i++)
			{
				if (actions["TakeScreenshotActions"][i].isObject())
				{
					padsManager->mutexExternalActions.lock();
					padsManager->externalActions.push_back(new TakeScreenshot(
						actions["TakeScreenshotActions"][i].get("frame", -1).asInt(),
						actions["TakeScreenshotActions"][i].get("filename", "/tmp/dpio.png").asString()
						));
					padsManager->mutexExternalActions.unlock();
				}
			}
		}
	}
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
	int readsize = 0;

	// Receive until the peer shuts down the connection
	do {

		memset(recvbuf, 0, DEFAULT_BUFLEN);
		readsize = client->read(recvbuf, recvbuflen);

		if (readsize > 0)
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
					recvstrPreAt = recvstr.substr(0, atPos);
					recvstrPostAt = recvstr.substr(atPos + 1, recvstr.size() - atPos - 1);
					recvstr = recvstrPostAt;
					if (receivingJson)
					{
						nextJson += recvstrPreAt;
						receivingJson = false;
						processJson(padsManager, nextJson.c_str());
						nextJson = std::string("");
					}
					else
					{
						receivingJson = true;
					}
				}
			}
		}
	} while (readsize > 0);
}