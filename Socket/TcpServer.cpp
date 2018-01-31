#include "stdafx.h"
#include "TcpServer.h"

TcpServer::TcpServer(unsigned short port, std::string ip, int backlog)
{
	Create();
	Bind(port, ip);
	Listen(backlog);
}

TcpServer::~TcpServer()
{
	Socket::Close();
}

TcpClient* TcpServer::Accept()
{
	return new TcpClient(Socket::Accept());
}
