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

	void Send(MessageType messageType, size_t someinfo, std::wstring messageData);

	void SetReceiveCallback(ReceiveCallback callback, void* uesrParam);
	//bool Receive(MessageData& message);

	protected:

	void Init();

	static DWORD __stdcall SendThreadProc(LPVOID lpParameter);

	static DWORD __stdcall ReceiveThreadProc(LPVOID lpParameter);

	bool _Send(const char* data, size_t length);

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
};

