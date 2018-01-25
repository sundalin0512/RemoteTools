#include "stdafx.h"
#include "Socket.h"

void Socket::GlobalInit()
{
	WSADATA wsaData;
	int iResult = 0;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		throw "Error at WSAStartup()";
	}
}

void Socket::GlobalCleanUp()
{
	WSACleanup();
}

std::string Socket::GetIP()
{
	char buf[20];
	SOCKADDR_IN sockaddr;
	int size = 20;
	getsockname(m_socket, (SOCKADDR*)&sockaddr, &size);
	sprintf_s(buf, 20, "%d.%d.%d.%d", sockaddr.sin_addr.S_un.S_un_b.s_b1
		, sockaddr.sin_addr.S_un.S_un_b.s_b2
		, sockaddr.sin_addr.S_un.S_un_b.s_b3
		, sockaddr.sin_addr.S_un.S_un_b.s_b4);
	return buf;
}

unsigned short Socket::GetPort()
{
	SOCKADDR_IN sockaddr;
	int size = 20;
	getsockname(m_socket, (SOCKADDR*)&sockaddr, &size);
	return sockaddr.sin_port;
}

Socket::Socket(): m_socket(0)
{
}

Socket::Socket(SOCKET sock)
{
	m_socket = sock;
}

Socket::~Socket()
{
}

void Socket::Create(int af, int type, int protocol)
{
	m_socket = socket(af, type, protocol);
	if (m_socket == INVALID_SOCKET)
		throw "error create socket";
}

void Socket::Bind(unsigned short port, std::string ip)
{
	sockaddr_in service;
	service.sin_family = AF_INET;
	//service.sin_addr.s_addr = inet_addr(ip.c_str());
	inet_pton(AF_INET, ip.c_str(), &service.sin_addr.s_addr);
	service.sin_port = htons(port);

	int iResult = bind(m_socket, (SOCKADDR *)&service, sizeof(service));
	if (iResult == SOCKET_ERROR)
	{
		throw "bind failed";
	}
}

void Socket::Listen(int backlog)
{
	if (listen(m_socket, backlog) == SOCKET_ERROR)
		throw "listen failed";
}

Socket Socket::Accept()
{
	SOCKET AcceptSocket = accept(m_socket, NULL, NULL);
	if (AcceptSocket == INVALID_SOCKET)
	{
		throw "accept failed";
	}
	return AcceptSocket;
}

void Socket::Connect(unsigned short port, std::string ip)
{
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	//clientService.sin_addr.s_addr = inet_addr(ip.c_str());
	inet_pton(AF_INET, ip.c_str(), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(port);
	int iResult = connect(m_socket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR)
	{
		throw "connect function failed";
	}
}

void Socket::Close()
{
	closesocket(m_socket);
}
