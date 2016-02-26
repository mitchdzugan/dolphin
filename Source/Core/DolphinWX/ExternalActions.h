#pragma once

#include "InputCommon/GCPadStatus.h"

class ExternalAction 
{
public:
	int frame;
	virtual void PerformAction(GCPadStatus * PadStatus, int frame, int controllerId) const = 0;
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