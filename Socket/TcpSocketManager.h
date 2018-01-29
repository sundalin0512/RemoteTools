#pragma once
#include "boost/serialization/singleton.hpp"
#include <WinSock2.h>
#include <vector>
#include "TcpClient.h"
#include "TcpServer.h"



class TcpSocketManager : public boost::serialization::singleton<TcpSocketManager>
{
	public:
	TcpSocketManager() {}
	~TcpSocketManager() {}
	static std::vector<WSAEVENT*> m_vecHEventAry;
	static std::vector<SOCKET*> m_vecHSocketAry;
	static std::vector<Socket**> m_vecClientsAry;
	static std::vector<size_t> m_vecSize;
	static void Start(TcpServer* server)
	{
		WSAEVENT* m_hEventAry = new WSAEVENT[WSA_MAXIMUM_WAIT_EVENTS];
		SOCKET* m_hSocketAry= new SOCKET[WSA_MAXIMUM_WAIT_EVENTS];
		Socket**  m_hClientAry = new Socket*[WSA_MAXIMUM_WAIT_EVENTS];
		

		m_hSocketAry[0] = *server;
		m_hEventAry[0] = WSACreateEvent();
		m_hClientAry[0] = server;

		int nRet = WSAEventSelect(m_hSocketAry[0],
			m_hEventAry[0],
			FD_ACCEPT | FD_CLOSE);

		if (SOCKET_ERROR == nRet)
		{
			//释放资源
			// WSACloseEvent
			return;
		}
		
		m_vecHEventAry.push_back(m_hEventAry);
		m_vecHSocketAry.push_back(m_hSocketAry);
		m_vecClientsAry.push_back(m_hClientAry);
		m_vecSize.push_back(1);
		CreateThread(NULL, 0, ThreadFunc, (LPVOID)(m_vecSize.size() - 1), 0, NULL);
	}

	static void Start(TcpClient* server)
	{
		WSAEVENT m_hEventAry[WSA_MAXIMUM_WAIT_EVENTS];
		SOCKET m_hSocketAry[WSA_MAXIMUM_WAIT_EVENTS];
		Socket*  m_hClientAry[WSA_MAXIMUM_WAIT_EVENTS];


		m_hSocketAry[0] = *server;
		m_hEventAry[0] = WSACreateEvent();
		m_hClientAry[0] = server;

		int nRet = WSAEventSelect(m_hSocketAry[0],
			m_hEventAry[0],
			FD_READ | FD_WRITE | FD_CLOSE);

		if (SOCKET_ERROR == nRet)
		{
			//释放资源
			// WSACloseEvent
			return;
		}
		m_vecSize.back()++;
		m_vecHEventAry.push_back(m_hEventAry);
		m_vecHSocketAry.push_back(m_hSocketAry);
		m_vecClientsAry.push_back(m_hClientAry);
		CreateThread(NULL, 0, ThreadFunc, (LPVOID)m_vecSize.size(), 0, NULL);
	}
	static DWORD WINAPI ThreadFunc(LPVOID lparam);
};




#define tcpSocketManager TcpSocketManager::get_mutable_instance()
