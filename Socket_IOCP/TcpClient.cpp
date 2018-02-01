#include "stdafx.h"
#include "TcpClient.h"
#include <cassert>
#include "TcpSocketManager.h"
#include "zlib.h"

TcpClient::TcpClient(SOCKET sock) : Socket(sock)
{
	InitializeCriticalSection(&m_csSendQueue);
	InitializeCriticalSection(&m_csReceiveQueue);
	pSendIOContext = new PER_IO_CONTEXT;
	pSendIOContext->tcpClient = this;
	pRecvIOContext = new PER_IO_CONTEXT;
	pRecvIOContext->tcpClient = this;
	SendBuffer = nullptr;
	SendBufferSize = 0;
	RecvBuffer = nullptr;
	RecvBufferSize = 0;
	SendWsabuf.buf = nullptr;
	SendWsabuf.len = 0;
	RecvWsabuf.buf = nullptr;
	RecvWsabuf.len = 0;
	
	nTotalBytes = 0;
	nSentBytes = 0;
}


TcpClient::~TcpClient()
{
	Close();
	DeleteCriticalSection(&m_csSendQueue);
	DeleteCriticalSection(&m_csReceiveQueue);
	delete SendBuffer;
	delete RecvBuffer;
}

void TcpClient::Send(MessageData message)
{
	//在这压缩
	if(message.length)
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
	if (!hasNextSend)
	{

		DWORD dwSendNumBytes = 0;
		DWORD dwFlags = 0;

		ToSend(this->SendBuffer, this->SendBufferSize);
		SendWsabuf.buf = this->SendBuffer;
		SendWsabuf.len = this->SendBufferSize;
		this->nSentBytes = 0;
		this->nTotalBytes = this->SendBufferSize;
		this->pSendIOContext->IOOperation = IO_OPERATION::ClientIoWrite;
		int nRet = WSASend(
			this->m_socket,
			&SendWsabuf, 1, &dwSendNumBytes,
			dwFlags,
			(WSAOVERLAPPED*)&this->pSendIOContext->Overlapped, NULL);
		if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
			printf("WSASend() failed: %d\n", WSAGetLastError());
			tcpSocketManager.CloseClient(this, FALSE);
		}
	}
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

bool TcpClient::ToSend(char*& buffer, size_t & length)
{
	if (buffer != nullptr)
	{
		delete buffer;
		buffer = nullptr;
		length = 0;
	}

	EnterCriticalSection(&this->m_csSendQueue);
	bool isEmpty = this->m_sendQueue.empty();
	if (!isEmpty)
	{
		Socket::MessageData front = this->m_sendQueue.front();
		length = front.length + sizeof(size_t) + sizeof(Socket::MessageType);
		buffer = new char[length];
		memcpy_s(buffer, sizeof(size_t), &front.length, sizeof(size_t));
		memcpy_s(buffer + sizeof(size_t), sizeof(Socket::MessageType), &front.type, sizeof(Socket::MessageType));
		if (front.data != nullptr)
		{
			memcpy_s(buffer + sizeof(size_t) + sizeof(Socket::MessageType), front.length, front.data, front.length);
		}
		delete front.data;
		this->m_sendQueue.pop();
		hasNextSend = true;
	}
	else
	{
		hasNextSend = false;
	}
	LeaveCriticalSection(&this->m_csSendQueue);
	//if (!isEmpty)
	//{
	//	this->_Send(buffer, length);
	//	delete[] buffer;
	//}


	return !isEmpty;
}

DWORD TcpClient::Received(CHAR* buf, DWORD length)
{
	if(buf == nullptr && length ==0)
	{
		if (this->m_receiveCallback != nullptr)
			this->m_receiveCallback(nullptr, this->m_userParam);
	}
	EnterCriticalSection(&m_csReceiveQueue);
	m_recvDataQueue.insert(m_recvDataQueue.end(), buf, buf + length);
	while (m_recvDataQueue.size() > sizeof(size_t))
	{
		Socket::MessageData *last = new Socket::MessageData;
		last->length = *(size_t*)&m_recvDataQueue[0];
		if (last->length <= m_recvDataQueue.size() - sizeof(size_t) - sizeof(Socket::MessageType))
		{
			m_recvDataQueue.erase(m_recvDataQueue.begin(), m_recvDataQueue.begin() + sizeof(size_t));
			last->type = (Socket::MessageType)m_recvDataQueue[0];
			m_recvDataQueue.erase(m_recvDataQueue.begin(), m_recvDataQueue.begin() + sizeof(Socket::MessageType));
			last->data = new char[last->length];
			std::move(m_recvDataQueue.begin(), m_recvDataQueue.begin() + last->length, last->data);
			//(last.data, last.length, &dataQueue[0], last.length);
			m_recvDataQueue.erase(m_recvDataQueue.begin(), m_recvDataQueue.begin() + last->length);
			//在这解压
			if (last->length)
			{
				//解压数据

				DWORD dwSrcSize =last->length;
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
			if (this->m_receiveCallback != nullptr)
				this->m_receiveCallback(last, this->m_userParam);
			delete[] last->data;
			delete last;
		}
		else
		{
			break;
		}
	}
	LeaveCriticalSection(&m_csReceiveQueue);
	return 0;
}


void TcpClient::Close()
{
	Socket::Close();
}
