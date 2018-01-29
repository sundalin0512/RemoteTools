#pragma once
#include "Socket.h"
#include "TcpClient.h"

class TcpServer : public Socket
{

	public:
	typedef int(*AcceptCallback) (TcpClient* client, void* uesrParam);

	TcpServer(unsigned short port, std::string ip = "127.0.0.1", int backlog = SOMAXCONN);

	virtual ~TcpServer();

	TcpClient* Accept();

	void SetAcceptCallback(AcceptCallback callback, void* uesrParam)
	{
		m_acceptCallback = callback;
		m_userParam = uesrParam;
	}

	void* m_userParam;
	AcceptCallback m_acceptCallback = nullptr;

};

