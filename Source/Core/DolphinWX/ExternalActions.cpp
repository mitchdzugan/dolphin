#include "ExternalActions.h"
#include "Core/Core.h"

AlterPadStatus::AlterPadStatus(GCPadStatus * PadStatusToUse, int frame, int controllerId)
{
	this->frame = frame;
	this->PadStatusToUse = PadStatusToUse;
	this->controllerId = controllerId;
}

void AlterPadStatus::PerformAction(GCPadStatus * PadStatus, int frame, int controllerId)
{
	if (this->frame == frame && this->controllerId == controllerId)
	{
		*PadStatus = *PadStatusToUse;
	}
}

TakeScreenshot::TakeScreenshot(int frame, std::string filename)
{
	this->frame = frame;
	this->taken = false;
	this->filename = filename;
}

void TakeScreenshot::PerformAction(GCPadStatus * p, int frame, int c)
{
	if (this->frame == frame && !this->taken)
	{
		Core::SaveScreenShot(this->filename);
		this->taken = true;
	}
}