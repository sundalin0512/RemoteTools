#include "stdafx.h"
#include "TcpSocketManager.h"

std::vector<WSAEVENT*> TcpSocketManager::m_vecHEventAry;
std::vector<SOCKET*> TcpSocketManager::m_vecHSocketAry;
std::vector<Socket**> TcpSocketManager::m_vecClientsAry;
std::vector<size_t> TcpSocketManager::m_vecSize;

DWORD TcpSocketManager::ThreadFunc(LPVOID lparam)
{
	int threadID = (int)lparam;
	size_t& m_nCount = m_vecSize[threadID];


	WSAEVENT* m_hEventAry = m_vecHEventAry[threadID];
	SOCKET* m_hSocketAry = m_vecHSocketAry[threadID];
	Socket** m_hClientAry = m_vecClientsAry[threadID];
	//通过等待事件被触发，来判断和处理socket消息
	char szBuf[MAXBYTE] = { 0 };
	while (true)
	{

		DWORD  dwRet = WSAWaitForMultipleEvents(
			m_nCount,    //EventAry的个数
			m_hEventAry, //EventAry数组指针
			FALSE,       //是否等待所有事件对象被触发
			INFINITE,    //等待事件
			FALSE);

		if (dwRet == WSA_WAIT_FAILED)
		{
			//释放资源
			return 0;
		}

		int nIndex = dwRet - WSA_WAIT_EVENT_0;

		for (int i = nIndex; i < m_nCount; i++)
		{
			dwRet = WSAWaitForMultipleEvents(
				1,    //EventAry的个数
				(void *const *)m_hEventAry, //EventAry数组指针
				FALSE,       //是否等待所有事件对象被触发
				0,    //等待事件
				FALSE);

			WSANETWORKEVENTS wsaNetworkEvents;
			int nRet = WSAEnumNetworkEvents(m_hSocketAry[i],
				m_hEventAry[i],
				&wsaNetworkEvents);

			//表示成功
			if (nRet == 0)
			{

				if (wsaNetworkEvents.lNetworkEvents &FD_ACCEPT)
				{
					if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
					{
						//TODO: 这里可能越界 m_hSocketAry m_hEventAry
						m_hClientAry[m_nCount] = ((TcpServer*)m_hClientAry[i])->Accept();
						m_hSocketAry[m_nCount] = *m_hClientAry[m_nCount];
						m_hEventAry[m_nCount] = WSACreateEvent();
						nRet = WSAEventSelect(m_hSocketAry[m_nCount],
							m_hEventAry[m_nCount],
							FD_READ | FD_WRITE | FD_CLOSE);

						if (SOCKET_ERROR == nRet)
						{
							//释放资源
							// WSACloseEvent
							return 0;
						}
						m_nCount++;
					}
					else
					{
						//根据错误码处理错误
					}
				}
				else if (wsaNetworkEvents.lNetworkEvents &FD_READ)
				{
					if (wsaNetworkEvents.iErrorCode[FD_READ_BIT] == 0)
					{
						((TcpClient*)m_hClientAry[i])->Receive();
					}
					else
					{
						//根据错误码处理错误
					}
				}
				else if (wsaNetworkEvents.lNetworkEvents &FD_WRITE)
				{
					if (wsaNetworkEvents.iErrorCode[FD_WRITE_BIT] == 0)
					{
						((TcpClient*)m_hClientAry[i])->Send();
					}
					else
					{
						//根据错误码处理错误
					}
				}
			}

		}
	}
	return 0;
}