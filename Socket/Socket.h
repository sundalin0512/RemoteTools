#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>

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

	protected:
	Socket();

	Socket(SOCKET sock);

	virtual ~Socket();

	void Create(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);

	void Bind(unsigned short port, std::string ip = "127.0.0.1");

	void Listen(int backlog = SOMAXCONN);

	Socket Accept();

	void Connect(unsigned short port, std::string ip = "127.0.0.1");

	void Close();

	protected:
	SOCKET m_socket;

	friend class TcpServer;
	friend class TcpClient;
};

