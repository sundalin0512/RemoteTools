#pragma once
#include "boost/serialization/singleton.hpp"
#include <WinSock2.h>
#include <vector>
#include "TcpClient.h"
#include "TcpServer.h"
#include <MSWSock.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <minwinbase.h>
#include <atlsimpcoll.h>

#define DEFAULT_PORT        "10086"
#define MAX_BUFF_SIZE       8192
#define MAX_WORKER_THREAD   16


//
// data to be associated for every I/O operation on a socket
//


class TcpSocketManager : public boost::serialization::singleton<TcpSocketManager>
{
	public:
	TcpSocketManager();
	~TcpSocketManager() {}
	static int g_Port;
	static BOOL g_bEndServer;
	static BOOL g_bRestart;
	static HANDLE g_hIOCP;
	static TcpServer* g_tcpServer;
	static HANDLE *g_ThreadHandles;
	static DWORD dwThreadCount;

	static TcpServer* geTcpServer();

	static bool StartServer(int port);

	static bool StartClient(std::string ip, DWORD port)
	{
		
	}

	static SOCKET CreateSocket(void);

	static BOOL CreateListenSocket(void);

	static BOOL CreateAcceptSocket(BOOL fUpdateIOCP);

	static DWORD WINAPI WorkerThread(LPVOID WorkThreadContext);

	static VOID CloseClient(Socket* lpPerSocketContext, BOOL bGraceful);
};




#define tcpSocketManager TcpSocketManager::get_mutable_instance()
