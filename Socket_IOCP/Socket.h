#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>
class TcpServer;
class TcpClient;

enum class IO_OPERATION {
	ClientIoAccept,
	ClientIoRead,
	ClientIoWrite
};

class Socket
{
	public:
	enum class MessageType
	{
		Nop,

		CmdOpen,
		CmdClose,
		CmdCommandRequset,
		CmdCommandReply,

		ScreenOpen,
		ScreenClose,
		ScreenCommandRequset,
		ScreenCommandReply,

		FileQueryRequset,
		FileQueryReply,
		FileDownloadRequset,
		FileDownloadReply,
		FileDownloadReplyIsDir,
		FileUploadRequset,
		FileUploadReply,
		FileUploadReplyIsDir,

		ProcessOpenRequest,
		ProcessOpenReply,
		ProcessTerminateRequest,
		ProcessTerminateReply,
		ProcessCloseRequest,
		ProcessCloseReply,

		HeartBeatRequset,
		HeartBeatReply,
	};

	struct SOCKET_IO_CONTEXT {
		WSAOVERLAPPED               Overlapped;
		IO_OPERATION                IOOperation;
		TcpClient*                  tcpClient;

		SOCKET_IO_CONTEXT()
		{
			ZeroMemory(&Overlapped, sizeof(WSAOVERLAPPED));

			tcpClient = nullptr;
		}
		~SOCKET_IO_CONTEXT()
		{
		}
	};


#pragma pack(push)
#pragma pack(1)
	struct MessageData
	{
		size_t length;
		MessageType type;
		char* data;
	};
#pragma pack(pop)
	public:
	static void GlobalInit();

	static void GlobalCleanUp();

	std::string GetIP();

	unsigned short GetPort();

	//operator const SOCKET&() const
	//{
	//	return m_socket;
	//}

	//operator SOCKET&() 
	//{
	//	return m_socket;
	//}
	SOCKET m_socket;

	Socket(SOCKET sock);

	virtual ~Socket();

	void Close();

	protected:


	friend class TcpServer;
	friend class TcpClient;
	public:




};

