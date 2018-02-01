#pragma once
class CRemoteToolClientDlg;
class TcpClient;
struct TcpClient::MessageData;

class ITask
{
	public:
	ITask(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : thisDlg(thisDlg), message(message){}
	virtual ~ITask()
	{
		delete message->data;
		delete message;
	}
	virtual void Run() = 0;
	CRemoteToolClientDlg* thisDlg;
	TcpClient::MessageData* message;
};

class TaskCmdOpen :
	public ITask
{
	public:
	TaskCmdOpen(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg,  message){}
	~TaskCmdOpen(){}

	virtual void Run() override;
};
class TaskCmdCommandRequset :
	public ITask
{
	public:
	TaskCmdCommandRequset(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskCmdCommandRequset() {}

	virtual void Run() override;
};
class TaskScreenCommandRequset :
	public ITask
{
	public:
	TaskScreenCommandRequset(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskScreenCommandRequset() {}

	virtual void Run() override;
};
class TaskFileQueryRequset :
	public ITask
{
	public:
	TaskFileQueryRequset(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskFileQueryRequset() {}

	virtual void Run() override;
};
class TaskFileDownloadRequset :
	public ITask
{
	public:
	TaskFileDownloadRequset(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskFileDownloadRequset() {}

	virtual void Run() override;
};
class TaskFileUploadReply :
	public ITask
{
	public:
	TaskFileUploadReply(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskFileUploadReply() {}

	virtual void Run() override;
};
class TaskFileUploadReplyIsDir :
	public ITask
{
	public:
	TaskFileUploadReplyIsDir(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskFileUploadReplyIsDir() {}

	virtual void Run() override;
};
class TaskProcessOpenRequest :
	public ITask
{
	public:
	TaskProcessOpenRequest(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskProcessOpenRequest() {}

	virtual void Run() override;
};
class TaskProcessTerminateRequest :
	public ITask
{
	public:
	TaskProcessTerminateRequest(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskProcessTerminateRequest() {}

	virtual void Run() override;
};
class TaskHeartBeatReply :
	public ITask
{
	public:
	TaskHeartBeatReply(CRemoteToolClientDlg* thisDlg, TcpClient::MessageData* message) : ITask(thisDlg, message) {}
	~TaskHeartBeatReply() {}

	virtual void Run() override;
};
