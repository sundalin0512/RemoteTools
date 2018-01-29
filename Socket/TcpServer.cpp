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
	auto ret = new TcpClient(Socket::Accept());
	if (m_acceptCallback != nullptr)
		m_acceptCallback(ret, m_userParam);
	return ret;
}
