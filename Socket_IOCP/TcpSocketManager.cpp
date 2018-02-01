#include "stdafx.h"
#include "TcpSocketManager.h"
#include <iostream>

int TcpSocketManager::g_Port;
BOOL TcpSocketManager::g_bEndServer;
BOOL TcpSocketManager::g_bRestart;
HANDLE TcpSocketManager::g_hIOCP;
TcpServer* TcpSocketManager::g_tcpServer;
HANDLE *TcpSocketManager::g_ThreadHandles;
DWORD TcpSocketManager::dwThreadCount;

TcpSocketManager::TcpSocketManager()
{
	g_Port = 10086;
	g_bEndServer = FALSE;
	g_bRestart = FALSE;
	g_hIOCP = INVALID_HANDLE_VALUE;
}

TcpServer* TcpSocketManager::geTcpServer()
{
	return g_tcpServer;
}

bool TcpSocketManager::StartServer(int port)
{
	g_Port = port;
	SYSTEM_INFO systemInfo;
	WSADATA wsaData;

	GetSystemInfo(&systemInfo);
	dwThreadCount = systemInfo.dwNumberOfProcessors * 2;
	g_ThreadHandles = new HANDLE[dwThreadCount];
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup();
		return false;
	}
	g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (g_hIOCP == NULL)
	{
		return false;
	}
	CreateListenSocket();
	CreateAcceptSocket(TRUE);
	for (DWORD dwCPU = 0; dwCPU < dwThreadCount; dwCPU++)
	{
		//
		// Create worker threads to service the overlapped I/O requests.  The decision
		// to create 2 worker threads per CPU in the system is a heuristic.  Also,
		// note that thread handles are closed right away, because we will not need them
		// and the worker threads will continue to execute.
		//
		HANDLE hThread;
		DWORD dwThreadId;

		hThread = CreateThread(NULL, 0, WorkerThread, g_hIOCP, 0, &dwThreadId);
		if (hThread == NULL)
		{
			return false;
		}
		g_ThreadHandles[dwCPU] = hThread;
		hThread = INVALID_HANDLE_VALUE;
	}

	return true;
}

SOCKET TcpSocketManager::CreateSocket()
{
	int nRet = 0;
	int nZero = 0;
	SOCKET sdSocket = INVALID_SOCKET;

	sdSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sdSocket == INVALID_SOCKET)
	{
		return (sdSocket);
	}
	nZero = 0;
	nRet = setsockopt(sdSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero));
	if (nRet == SOCKET_ERROR)
	{
		return (sdSocket);
	}
	return (sdSocket);
}

BOOL TcpSocketManager::CreateListenSocket()
{
	int nRet = 0;

	g_tcpServer = new TcpServer(CreateSocket());
	if (g_tcpServer->m_socket == INVALID_SOCKET)
	{
		return (FALSE);
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(g_Port);
	addr.sin_addr.S_un.S_addr = ADDR_ANY;

	nRet = bind(g_tcpServer->m_socket, (sockaddr*)&addr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		return (FALSE);
	}

	nRet = listen(g_tcpServer->m_socket, SOMAXCONN);
	if (nRet == SOCKET_ERROR)
	{
		return (FALSE);
	}

	return (TRUE);
}

BOOL TcpSocketManager::CreateAcceptSocket(BOOL fUpdateIOCP)
{
	int nRet = 0;
	DWORD dwRecvNumBytes = 0;
	DWORD bytes = 0;


	GUID acceptex_guid = WSAID_ACCEPTEX;


	if (fUpdateIOCP)
	{
		g_tcpServer->pIOContext->IOOperation = IO_OPERATION::ClientIoAccept;
		g_hIOCP = CreateIoCompletionPort((HANDLE)g_tcpServer->m_socket, g_hIOCP, (DWORD_PTR)g_tcpServer, 0);
		if (g_hIOCP == NULL)
		{
			return false;
		}

		// Load the AcceptEx extension function from the provider for this socket
		nRet = WSAIoctl(
			g_tcpServer->m_socket,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&acceptex_guid,
			sizeof(acceptex_guid),
			&g_tcpServer->fnAcceptEx,
			sizeof(g_tcpServer->fnAcceptEx),
			&bytes,
			NULL,
			NULL
		);
		if (nRet == SOCKET_ERROR)
		{
			return false;
		}
	}
	g_tcpServer->pIOContext->tcpClient = new TcpClient(CreateSocket());
	if (g_tcpServer->pIOContext->tcpClient->m_socket == INVALID_SOCKET)
	{
		return false;
	}

	g_tcpServer->pIOContext->tcpClient->RecvBuffer = new char[8 + (2 * (sizeof(SOCKADDR_STORAGE) + 16))];
	g_tcpServer->pIOContext->tcpClient->RecvBufferSize = 8 + (2 * (sizeof(SOCKADDR_STORAGE) + 16));
	//
	// pay close attention to these parameters and buffer lengths
	//
	nRet = g_tcpServer->fnAcceptEx(g_tcpServer->m_socket, g_tcpServer->pIOContext->tcpClient->m_socket,
		(LPVOID)(g_tcpServer->pIOContext->tcpClient->RecvBuffer),
		8,
		sizeof(SOCKADDR_STORAGE) + 16, sizeof(SOCKADDR_STORAGE) + 16,
		&dwRecvNumBytes,
		(LPOVERLAPPED)&(g_tcpServer->pIOContext->Overlapped));
	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
	{
		return false;
	}

	return true;
}

DWORD TcpSocketManager::WorkerThread(LPVOID WorkThreadContext)
{
	HANDLE hIOCP = (HANDLE)WorkThreadContext;
	BOOL bSuccess = FALSE;
	int nRet = 0;
	LPWSAOVERLAPPED lpOverlapped = NULL;
	WSABUF buffRecv;
	WSABUF buffSend;
	DWORD dwRecvNumBytes = 0;
	DWORD dwSendNumBytes = 0;
	DWORD dwIoSize = 0;
	DWORD dwFlags = 0;
	Socket* pSock;
	HRESULT hRet;
	while (TRUE)
	{
		bSuccess = GetQueuedCompletionStatus(
			hIOCP,
			&dwIoSize,
			(PDWORD_PTR)&pSock,
			(LPOVERLAPPED *)&lpOverlapped,
			INFINITE
		);
		if (!bSuccess)
		{
			printf("GetQueuedCompletionStatus() failed: %d\n", GetLastError());

		}

		if (g_bEndServer)
		{
			return (0);
		}

		Socket::SOCKET_IO_CONTEXT* lpIOContext = (Socket::SOCKET_IO_CONTEXT*)lpOverlapped;
		TcpClient* tcpClient = lpIOContext->tcpClient;
		if (lpIOContext->IOOperation != IO_OPERATION::ClientIoAccept)
		{
			if (!bSuccess || (bSuccess && (0 == dwIoSize)))
			{
				//
				// client connection dropped, continue to service remaining (and possibly 
				// new) client connections
				//
				CloseClient(((TcpServer*)pSock), FALSE);
				continue;
			}
		}

		switch (lpIOContext->IOOperation)
		{
		case IO_OPERATION::ClientIoAccept:
		{
			tcpClient->g_hIOCP = g_hIOCP;
			nRet = setsockopt(
				tcpClient->m_socket,
				SOL_SOCKET,
				SO_UPDATE_ACCEPT_CONTEXT,
				(char *)&g_tcpServer->m_socket,
				sizeof(g_tcpServer->m_socket)
			);
			if (nRet == SOCKET_ERROR)
			{
				printf("setsockopt(SO_UPDATE_ACCEPT_CONTEXT) failed to update accept socket\n");
				return (0);
			}

			g_hIOCP = CreateIoCompletionPort((HANDLE)tcpClient->m_socket, g_hIOCP, (DWORD_PTR)tcpClient, 0);
			if (g_hIOCP == NULL)
			{
				printf("CreateIoCompletionPort() failed: %d\n", GetLastError());
				return (NULL);
			}
			if (dwIoSize)
			{
				tcpClient->pRecvIOContext->IOOperation = IO_OPERATION::ClientIoRead;
				tcpClient->nTotalBytes = dwIoSize;
				tcpClient->nSentBytes = 0;
				tcpClient->RecvWsabuf.len = MAX_BUFF_SIZE;
				tcpClient->RecvBuffer = new char[MAX_BUFF_SIZE];
				tcpClient->RecvBufferSize = MAX_BUFF_SIZE;
				hRet = StringCbCopyNA(tcpClient->RecvBuffer,
					MAX_BUFF_SIZE,
					tcpClient->RecvBuffer,
					MAX_BUFF_SIZE
				);
				tcpClient->RecvWsabuf.buf = tcpClient->RecvBuffer;
				dwFlags = 0;
				nRet = WSARecv(
					tcpClient->m_socket,
					&tcpClient->RecvWsabuf, 1,
					&dwSendNumBytes,
					&dwFlags,
					(WSAOVERLAPPED*)&(tcpClient->pRecvIOContext->Overlapped), NULL);

				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
				{

					printf("WSASend() failed: %d\n", WSAGetLastError());
					CloseClient(((TcpServer*)pSock), FALSE);
				}
			}
			g_tcpServer->Accept(tcpClient);
			if (!CreateAcceptSocket(FALSE))
			{
				printf("Please shut down and reboot the server.\n");
				return (0);
			}
		}
		break;
		case IO_OPERATION::ClientIoRead:
		{
			tcpClient->pRecvIOContext->IOOperation = IO_OPERATION::ClientIoRead;
			tcpClient->Received(tcpClient->RecvWsabuf.buf, dwIoSize);
			//if (tcpClient->RecvBuffer != nullptr)
			//{
			//	delete tcpClient->RecvBuffer;
			//	tcpClient->RecvBuffer = nullptr;
			//	tcpClient->RecvBufferSize = 0;
			//}
			//tcpClient->RecvBuffer = new char[MAX_BUFF_SIZE];
			//tcpClient->RecvBufferSize = MAX_BUFF_SIZE;

			dwRecvNumBytes = 0;
			dwFlags = 0;
			tcpClient->RecvWsabuf.buf = tcpClient->RecvBuffer,
				tcpClient->RecvWsabuf.len = tcpClient->RecvBufferSize;
			nRet = WSARecv(
				tcpClient->m_socket,
				&tcpClient->RecvWsabuf, 1, &dwRecvNumBytes,
				&dwFlags,
				(WSAOVERLAPPED*)&(tcpClient->pRecvIOContext->Overlapped), NULL);
			if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
			{
				CloseClient(tcpClient, FALSE);
			}
		}
		break;
		case IO_OPERATION::ClientIoWrite:
		{
			tcpClient->pSendIOContext->IOOperation = IO_OPERATION::ClientIoWrite;
			tcpClient->nSentBytes += dwIoSize;
			dwFlags = 0;
			if (tcpClient->nSentBytes < tcpClient->nTotalBytes)
			{
				tcpClient->SendWsabuf.buf = tcpClient->SendBuffer + tcpClient->nSentBytes;
				tcpClient->SendWsabuf.len = tcpClient->nTotalBytes - tcpClient->nSentBytes;
				nRet = WSASend(
					tcpClient->m_socket,
					&tcpClient->SendWsabuf, 1, &dwSendNumBytes,
					dwFlags,
					(WSAOVERLAPPED*)&(tcpClient->pSendIOContext->Overlapped), NULL);
				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
				{
					printf("WSASend() failed: %d\n", WSAGetLastError());
					CloseClient(tcpClient, FALSE);
				}
			}
			else
			{
				if (tcpClient->ToSend(tcpClient->SendBuffer, tcpClient->SendBufferSize))
				{
					tcpClient->nSentBytes = 0;
					tcpClient->nTotalBytes = tcpClient->SendBufferSize;
					tcpClient->SendWsabuf.buf = tcpClient->SendBuffer;
					tcpClient->SendWsabuf.len = tcpClient->SendBufferSize;
					nRet = WSASend(
						tcpClient->m_socket,
						&tcpClient->SendWsabuf, 1, &dwSendNumBytes,
						dwFlags,
						(WSAOVERLAPPED*)&(tcpClient->pSendIOContext->Overlapped), NULL);
					if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
					{
						printf("WSASend() failed: %d\n", WSAGetLastError());
						CloseClient(tcpClient, FALSE);
					}
				}
			}
		}
		break;
		}
	}
}

void TcpSocketManager::CloseClient(Socket* lpPerSocketContext, BOOL bGraceful)
{
	((TcpClient*)lpPerSocketContext)->Received(nullptr, 0);
	if (lpPerSocketContext)
	{
		if (!bGraceful)
		{
			//
			// force the subsequent closesocket to be abortative.
			//
			LINGER lingerStruct;

			lingerStruct.l_onoff = 1;
			lingerStruct.l_linger = 0;
			setsockopt(lpPerSocketContext->m_socket, SOL_SOCKET, SO_LINGER,
				(char *)&lingerStruct, sizeof(lingerStruct));
		}
		if (lpPerSocketContext->m_socket != INVALID_SOCKET)
		{
			closesocket(lpPerSocketContext->m_socket);
			lpPerSocketContext->m_socket = INVALID_SOCKET;
		};

		closesocket(lpPerSocketContext->m_socket);
		lpPerSocketContext->m_socket = INVALID_SOCKET;
		delete lpPerSocketContext;
		lpPerSocketContext = NULL;
	}
	else
	{
		printf("CloseClient: lpPerSocketContext is NULL\n");
	}


	return;
}
