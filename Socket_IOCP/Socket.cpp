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

Socket::Socket(SOCKET sock)
{
	m_socket = sock;
	
}

Socket::~Socket()
{
	
}

void Socket::Close()
{
	closesocket(m_socket);
}
