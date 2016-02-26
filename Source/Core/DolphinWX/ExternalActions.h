#pragma once

#include "InputCommon/GCPadStatus.h"
#include <string>

class ExternalAction 
{
public:
	int frame;
	virtual void PerformAction(GCPadStatus * PadStatus, int frame, int controllerId) = 0;
};

class AlterPadStatus : public ExternalAction
{
public:
	AlterPadStatus(GCPadStatus * padStatusToUse, int frame, int controllerId);
	void PerformAction(GCPadStatus * PadStatus, int frame, int controllerId);

private:
	GCPadStatus * PadStatusToUse;
	int controllerId;
};

class TakeScreenshot : public ExternalAction
{
public:
	TakeScreenshot(int frame, std::string filename);
	void PerformAction(GCPadStatus * p, int frame, int c);

private:
	bool taken;
	std::string filename;
};