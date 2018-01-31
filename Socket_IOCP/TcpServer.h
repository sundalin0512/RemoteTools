#pragma once
#include "Socket.h"
#include "TcpClient.h"
#include <MSWSock.h>

class TcpServer : public Socket
{
	public:
	typedef struct _PER_IO_CONTEXT : public SOCKET_IO_CONTEXT {
		TcpServer*                  tcpServer;
		_PER_IO_CONTEXT()
		{
			ZeroMemory(&Overlapped, sizeof(WSAOVERLAPPED));
			tcpServer = nullptr;
			tcpClient = nullptr;
		}
		~_PER_IO_CONTEXT()
		{
		}
	} PER_IO_CONTEXT, *PPER_IO_CONTEXT;

	typedef int(*AcceptCallback) (TcpClient* client, void* uesrParam);

	TcpServer(SOCKET socket);

	virtual ~TcpServer();

	TcpClient* Accept(TcpClient* tcpClient);

	void SetAcceptCallback(AcceptCallback callback, void* uesrParam);

	void* m_userParam;
	AcceptCallback m_acceptCallback = nullptr;

public:
	PPER_IO_CONTEXT             pIOContext;
	LPFN_ACCEPTEX               fnAcceptEx;
};

