#include "ExternalActions.h"

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