#pragma once
#include "Socket.h"
#include "TcpClient.h"

class TcpServer : public Socket
{
	public:
	TcpServer(unsigned short port, std::string ip = "127.0.0.1", int backlog = SOMAXCONN);

	virtual ~TcpServer();

	TcpClient* Accept();
};

