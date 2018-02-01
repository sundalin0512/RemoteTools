#include "stdafx.h"
#include "ITask.h"
#include "FileProcess.h"
#include "RemoteToolClientDlg.h"
#include "TcpClient.h"

static std::vector<CString> split(std::wstring str, std::wstring pattern)
{
	std::wstring::size_type pos;
	std::vector<CString> result;
	size_t size = str.size();

	for (size_t i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			CString s;
			s = str.substr(i, pos - i).c_str();
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	if (result.back() == "")
		result.erase(result.end() - 1);
	return result;
}

void TaskCmdOpen::Run()
{
	if (thisDlg->m_CmdThreadHandle == INVALID_HANDLE_VALUE)
		thisDlg->m_CmdThreadHandle = CreateThread(NULL, 0, CRemoteToolClientDlg::CmdThreadProc, thisDlg, 0, NULL);
}

void TaskCmdCommandRequset::Run()
{
	if (thisDlg->m_CmdThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD dwWritedBytes = 0;
		WriteFile(thisDlg->m_hCmdWritePipe, message->data, message->length, &dwWritedBytes, NULL);
	}
}

void TaskScreenCommandRequset::Run()
{
	CRemoteToolClientDlg::PrintScreen(thisDlg);
}

void TaskFileQueryRequset::Run()
{
	FileProcess fileProcess;
	fileProcess.m_socketClient = thisDlg->m_client;
	std::wstring str((wchar_t*)message->data, message->length / 2);

	fileProcess.OnUploadQuere(str);
}

void TaskFileDownloadRequset::Run()
{
	FileProcess fileProcess;
	fileProcess.m_socketClient = thisDlg->m_client;
	size_t offset = *(size_t*)message->data;
	std::wstring str((wchar_t*)(message->data + sizeof(size_t)), (message->length - sizeof(size_t)) / 2);
	fileProcess.OnDownloadQuere(str, offset);
}

void TaskFileUploadReply::Run()
{
	FileProcess fileProcess;
	fileProcess.m_socketClient = thisDlg->m_client;
	fileProcess.SaveRemoteFile(message->data, message->length);
}

void TaskFileUploadReplyIsDir::Run()
{
	FileProcess fileProcess;
	fileProcess.m_socketClient = thisDlg->m_client;
	wchar_t* szbuf = new wchar_t[message->length / 2 + 1];
	memcpy_s(szbuf, message->length, message->data, message->length);
	szbuf[message->length / 2] = L'\0';
	std::vector<CString> vecPath = split(szbuf, L"?");
	assert(vecPath.size() % 2 == 0);
	for (size_t i = 0; i < vecPath.size(); i += 2)
	{
		fileProcess.SendDownloadRequset(vecPath[i], vecPath[i + 1]);
	}
}

void TaskProcessOpenRequest::Run()
{
	thisDlg->OnProcessOpenRequset();
}

void TaskProcessTerminateRequest::Run()
{
	std::wstring str((wchar_t*)message->data, message->length / 2);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _wtoi(str.c_str()));
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
}

void TaskHeartBeatReply::Run()
{
	thisDlg->KillTimer(2);
	thisDlg->SetTimer(2, 10000, NULL);
}
