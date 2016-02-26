#pragma once

class GcnPadsManager;

void LaunchTcpServer(GcnPadsManager * padsManager);

class TcpClient
{
public:
	virtual int read(char * recvbuf, int recvlen) = 0;
	virtual int write(const char * writebuf, int writelen) = 0;
};
