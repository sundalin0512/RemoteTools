#pragma once
#include "Socket.h"
#include <vector>
#include <queue>

class TcpClient : public Socket
{
	public:	
	typedef struct _PER_IO_CONTEXT : public SOCKET_IO_CONTEXT
	{
	} PER_IO_CONTEXT, *PPER_IO_CONTEXT;
	typedef int (*ReceiveCallback) (MessageData* message, void* uesrParam);

	TcpClient(SOCKET sock);

	TcpClient(const TcpClient& other) = delete;

	virtual ~TcpClient();

	void Send(MessageData message);

	void Send(MessageType messageType, std::string messageData);

	void Send(MessageType messageType, std::wstring messageData);

	void Send(MessageType messageType, size_t someinfo, std::wstring messageData);

	void SetReceiveCallback(ReceiveCallback callback, void* uesrParam);
	//bool Received(MessageData& message);
	DWORD  Received(CHAR* buf, DWORD length);

	bool ToSend(char*& buffer, size_t & length);

	void Close();

	bool hasNextSend = false;
	HANDLE g_hIOCP = INVALID_HANDLE_VALUE;
	protected:

	protected:
	CRITICAL_SECTION m_csSendQueue;
	std::queue<MessageData> m_sendQueue;

	CRITICAL_SECTION m_csReceiveQueue;


	bool m_isValid;
	ReceiveCallback m_receiveCallback = nullptr;
	void* m_userParam;

	std::vector<char> m_recvDataQueue;

	public:
	PPER_IO_CONTEXT             pSendIOContext;
	PPER_IO_CONTEXT             pRecvIOContext;
	char*                       SendBuffer;
	size_t						SendBufferSize;
	char*                       RecvBuffer;
	size_t                      RecvBufferSize;
	WSABUF                      SendWsabuf;
	WSABUF                      RecvWsabuf;
	int                         nTotalBytes;
	int                         nSentBytes;
};

