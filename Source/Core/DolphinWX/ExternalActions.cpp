#include "ExternalActions.h"

AlterPadStatus::AlterPadStatus(GCPadStatus * PadStatusToUse, int frame, int controllerId)
{
	this->frame = frame;
	this->PadStatusToUse = PadStatusToUse;
	this->controllerId = controllerId;
}

void AlterPadStatus::PerformAction(GCPadStatus * PadStatus, int controllerId)
{
	if (this->controllerId == controllerId)
	{
		*PadStatus = *PadStatusToUse;
	}
}