#pragma once

class GcnPadsManager;

void LaunchTcpServer(GcnPadsManager * padsManager);

class TcpClient
{
public:
	virtual void read(char * recvbuf, int recvlen) = 0;
	virtual void write(const char * writebuf, int writelen) = 0;
};
