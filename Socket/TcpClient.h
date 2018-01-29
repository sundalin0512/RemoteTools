#pragma once
#include "Socket.h"
#include <vector>
#include <queue>

class TcpClient : public Socket
{
	public:
	typedef int (*ReceiveCallback) (MessageData* message, void* uesrParam);
	TcpClient() {}

	TcpClient(Socket sock);

	TcpClient(unsigned short port, std::string ip = "127.0.0.1");

	TcpClient(const TcpClient& other) = delete;

	virtual ~TcpClient();

	void Create(Socket sock);

	void Send(MessageData message);

	void Send(MessageType messageType, std::string messageData);

	void Send(MessageType messageType, std::wstring messageData);

	void SetReceiveCallback(ReceiveCallback callback, void* uesrParam);
	//bool Receive(MessageData& message);
	DWORD  Receive();

	DWORD Send();
	protected:

	void Init();

	

	

	void _Send(const char* data, size_t length);

	bool _Receive(char* data, int& length);

	void Close();

	protected:
	CRITICAL_SECTION m_csSendQueue;
	std::queue<MessageData> m_sendQueue;
	HANDLE m_hSendThread;

	//CRITICAL_SECTION m_csReceiveQueue;
	//std::queue<MessageData> m_receiveQueue;
	HANDLE m_hReceiveThread;

	bool m_isValid;
	ReceiveCallback m_receiveCallback = nullptr;
	void* m_userParam;

	const size_t m_recvBufferSize = 1024 * 1024;
	char *m_recvBuffer = new char[m_recvBufferSize];
	std::vector<char> m_recvDataQueue;
};

