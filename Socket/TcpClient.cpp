#include "stdafx.h"
#include "TcpClient.h"
#include <cassert>
#include "zlib.h"

TcpClient::TcpClient(Socket sock)
{
	InitializeCriticalSection(&m_csSendQueue);
	//InitializeCriticalSection(&m_csReceiveQueue);
	Create(sock);
}

TcpClient::TcpClient(unsigned short port, std::string ip)
{
	InitializeCriticalSection(&m_csSendQueue);
	//InitializeCriticalSection(&m_csReceiveQueue);
	Socket::Create();
	Connect(port, ip);
	Init();
}

TcpClient::~TcpClient()
{
	Close();
	DeleteCriticalSection(&m_csSendQueue);
	//DeleteCriticalSection(&m_csReceiveQueue);
}

void TcpClient::Init()
{
	m_isValid = true;
	m_hSendThread = CreateThread(NULL, 0, SendThreadProc, this, 0, NULL);
	m_hReceiveThread = CreateThread(NULL, 4*1024*1024, ReceiveThreadProc, this, 0, NULL);
}

void TcpClient::Create(Socket sock)
{
	m_socket = sock.m_socket;
	Init();
}

void TcpClient::Send(MessageData message)
{	
	//在这压缩
	if (message.length)
	{
		//压缩原始数据

		int dwZibSize = compressBound(message.length);

		//分配缓冲区空间
		char* pZibBuf = new char[dwZibSize + sizeof(size_t)];

		//int compress (Bytef * dest , uLongf * destLen, constt Bytef * source , uLongf sourceLen)


		DWORD dwdestLen = dwZibSize;
		//开始压缩
		int nRet = compress((Bytef *)pZibBuf + sizeof(size_t),
			(uLongf *)&dwdestLen,
			(const unsigned char *)message.data,
			(uLongf)message.length);

		if (nRet == Z_OK)
		{
			*(size_t*)pZibBuf = message.length;
			message.length = dwdestLen + sizeof(size_t);
			delete message.data;
			message.data = pZibBuf;
		}
		else
		{
			throw "";
		}
	}
	EnterCriticalSection(&m_csSendQueue);
	m_sendQueue.push(message);
	LeaveCriticalSection(&m_csSendQueue);
}

void TcpClient::Send(MessageType messageType, std::string messageData)
{
	MessageData message;
	message.type = messageType;
	message.length = messageData.length();
	message.data = new char[message.length];
	messageData._Copy_s(message.data, message.length, message.length);
	Send(message);
}

void TcpClient::Send(MessageType messageType, std::wstring messageData)
{
	MessageData message;
	message.type = messageType;
	message.length = messageData.length() * 2;
	message.data = new char[message.length * 2];
	messageData._Copy_s((wchar_t*)message.data, message.length, message.length);
	Send(message);
}

void TcpClient::Send(MessageType messageType, size_t someinfo, std::wstring messageData)
{
	MessageData message;
	message.type = messageType;
	message.length = messageData.length() * 2 + sizeof(size_t);
	message.data = new char[message.length * 2 + sizeof(size_t)];
	memcpy_s(message.data, sizeof(size_t), &someinfo, sizeof(size_t));
	messageData._Copy_s((wchar_t*)(message.data + sizeof(size_t)), messageData.length() * 2, messageData.length() * 2);
	Send(message);
}

void TcpClient::SetReceiveCallback(ReceiveCallback callback, void* uesrParam)
{
	m_receiveCallback = callback;
	m_userParam = uesrParam;
}

//bool TcpClient::Receive(MessageData& message)
//{
//	EnterCriticalSection(&m_csReceiveQueue);
//	if (!m_receiveQueue.empty())
//	{
//		message = m_receiveQueue.front();
//		m_receiveQueue.pop();
//		LeaveCriticalSection(&m_csReceiveQueue);
//		return true;
//	}
//	LeaveCriticalSection(&m_csReceiveQueue);
//	return false;
//}

DWORD TcpClient::SendThreadProc(LPVOID lpParameter)
{
	TcpClient* thisClient = (TcpClient*)lpParameter;
	char* buffer = nullptr;
	size_t length = 0;
	while (thisClient->m_isValid)
	{
		EnterCriticalSection(&thisClient->m_csSendQueue);
		bool isEmpty = thisClient->m_sendQueue.empty();
		if (!isEmpty)
		{
			Socket::MessageData front = thisClient->m_sendQueue.front();
			length = front.length + sizeof(size_t) + sizeof(Socket::MessageType);
			buffer = new char[length];
			memcpy_s(buffer, sizeof(size_t), &front.length, sizeof(size_t));
			memcpy_s(buffer + sizeof(size_t), sizeof(Socket::MessageType), &front.type, sizeof(Socket::MessageType));
			if (front.data != nullptr)
			{
				memcpy_s(buffer + sizeof(size_t) + sizeof(Socket::MessageType), front.length, front.data, front.length);
			}
			delete front.data;
			thisClient->m_sendQueue.pop();
		}
		LeaveCriticalSection(&thisClient->m_csSendQueue);
		if (!isEmpty)
		{
			if (!thisClient->_Send(buffer, length))
				break;
			delete [] buffer;
		}
		else
		{
			Sleep(10);
		}
	}
	return 0;
}

DWORD TcpClient::ReceiveThreadProc(LPVOID lpParameter)
{
	TcpClient* thisClient = (TcpClient*)lpParameter;
	const size_t bufferSize = 1024 * 1024;
	char buffer[bufferSize];
	std::vector<char> dataQueue;
	while (thisClient->m_isValid)
	{
		int length = bufferSize;
		if (!thisClient->_Receive(buffer, length))
		{
			break;
		}
		dataQueue.insert(dataQueue.end(), buffer, buffer + length);
		while (dataQueue.size() > sizeof(size_t))
		{
			Socket::MessageData *last = new Socket::MessageData;
			last->length = *(size_t*)&dataQueue[0];
			if (last->length <= dataQueue.size() - sizeof(size_t) - sizeof(Socket::MessageType))
			{
				dataQueue.erase(dataQueue.begin(), dataQueue.begin() + sizeof(size_t));
				last->type = (Socket::MessageType)dataQueue[0];
				dataQueue.erase(dataQueue.begin(), dataQueue.begin() + sizeof(Socket::MessageType));
				last->data = new char[last->length];
				std::move(dataQueue.begin(), dataQueue.begin() + last->length, last->data);
				//(last.data, last.length, &dataQueue[0], last.length);
				dataQueue.erase(dataQueue.begin(), dataQueue.begin() + last->length);
				//在这解压
				if (last->length)
				{
					//解压数据

					DWORD dwSrcSize = last->length;
					DWORD dwZibSize = 0;

					char* pZibBuf = last->data + sizeof(size_t);
					//创建用于解压的缓冲区

					DWORD dwSrcBufSize = *(size_t*)last->data;
					char* pSrcBuf = new char[dwSrcBufSize]; //为了原始数据分配足够大的缓冲区
					if (pSrcBuf == NULL)
					{
						return FALSE;
					}

					//开始解压缩
					//   int uncompress(Bytef *dest, uLongf *destLen,const Bytef *source, uLongf sourceLen)
					//     
					//   zlib的解压缩函数，将source处sourceLen个字节解压缩，放到大小为destLend的dest缓冲区中，将最终的长度放到destLen指向的地址中。
					//     
					//   所以调用前需赋值destLen

					//开始解压缩
					int nRet = uncompress((Bytef *)pSrcBuf,
						(uLongf *)&dwSrcBufSize, //填写解压缩缓冲区大小
						(const unsigned char *)pZibBuf,
						(uLongf)dwSrcSize - sizeof(size_t));

					if (nRet == Z_OK)
					{
						last->length = dwSrcBufSize;
						delete last->data;
						last->data = pSrcBuf;

					}
					else
					{
						throw "";
					}
				}
				if(thisClient->m_receiveCallback != nullptr)
					thisClient->m_receiveCallback(last, thisClient->m_userParam);
				delete [] last->data;
				delete last;
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}

bool TcpClient::_Send(const char* data, size_t length)
{
	int nTotalBytes = length;
	int nSendedBytes = 0;
	int nIndex = 0;
	while (nIndex < nTotalBytes)
	{
		nSendedBytes = send(m_socket,
			data + nIndex,
			nTotalBytes - nIndex,
			0);

		if (nSendedBytes == SOCKET_ERROR)
		{
			//处理资源释放
			return false;
		}

		nIndex += nSendedBytes;
	}
	assert(length == nIndex);
	return true;
}

bool TcpClient::_Receive(char* data, int& length)
{
	length = recv(m_socket, data, length, 0);
	if (length > 0)
		return true;
	else
		return false;
}

void TcpClient::Close()
{
	m_isValid = false;
	shutdown(m_socket, SD_BOTH);
	Socket::Close();
	HANDLE handles[2] = { m_hSendThread, m_hReceiveThread };

	WaitForMultipleObjects(2, handles, TRUE, INFINITE);
}
