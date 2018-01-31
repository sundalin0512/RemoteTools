#include "stdafx.h"
#include "TcpServer.h"

TcpServer::TcpServer(SOCKET socket): Socket(socket)
{
	pIOContext = new PER_IO_CONTEXT;
	pIOContext->tcpServer = this;
}

TcpServer::~TcpServer()
{
	delete pIOContext;
	Socket::Close();
}

TcpClient* TcpServer::Accept(TcpClient* tcpClient)
{
	if (m_acceptCallback != nullptr)
		m_acceptCallback(tcpClient, m_userParam);
	return tcpClient;
}

void TcpServer::SetAcceptCallback(AcceptCallback callback, void* uesrParam)
{
	m_acceptCallback = callback;
	m_userParam = uesrParam;
}
