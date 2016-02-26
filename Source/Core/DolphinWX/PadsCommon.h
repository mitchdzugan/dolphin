#pragma once

#include "InputCommon/GCPadStatus.h"

void buildPacket(char * buffer, int frame, int controller, GCPadStatus * padStatus);
int frameFromPacket(char * buffer);
int controllerFromPacket(char * buffer);
void padStatusFromPacket(char * buffer, GCPadStatus * padStatus);