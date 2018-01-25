// RemoteCmdDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteToolServer.h"
#include "RemoteCmdDialog.h"
#include "afxdialogex.h"


// RemoteCmdDialog 对话框

IMPLEMENT_DYNAMIC(RemoteCmdDialog, CDialogEx)

RemoteCmdDialog::RemoteCmdDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CMD, pParent)
{

}

RemoteCmdDialog::~RemoteCmdDialog()
{
}

void RemoteCmdDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_editInput);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_editOutput);
	DDX_Text(pDX, IDC_EDIT_OUTPUT, m_strOutput);
}


BEGIN_MESSAGE_MAP(RemoteCmdDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, &RemoteCmdDialog::OnClickedButtonConfirm)
	ON_MESSAGE(WM_UPDATE_OUTPUT, &RemoteCmdDialog::OnUpdateOutput)
END_MESSAGE_MAP()


// RemoteCmdDialog 消息处理程序

BOOL RemoteCmdDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT_INPUT))  //根据不同控件焦点判断是那个在执行  
		{
			UpdateData(TRUE);
			// TODO: 发消息
			PostMessageToClient();
			return 0;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void RemoteCmdDialog::PostMessageToClient()
{
	CString strInput;
	char szUsrInput[MAXBYTE] = { 0 };
	m_editInput.GetWindowText(strInput);
	WideCharToMultiByte(936, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK, strInput.GetBuffer(), strInput.GetLength(), szUsrInput, MAXBYTE, NULL, NULL);
	//scanf_s("%s", szUsrInput, MAXBYTE);
	strcat_s(szUsrInput, MAXBYTE, "\n");
	m_socketClient->Send(Socket::MessageType::CmdCommandRequset, szUsrInput);
}


void RemoteCmdDialog::OnClickedButtonConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	PostMessageToClient();
}

afx_msg LRESULT RemoteCmdDialog::OnUpdateOutput(WPARAM wParam, LPARAM lParam)
{
	//char* pszReadBuf = (char*)lParam;
	////TODO: 输出pszReadBuf
	//m_strOutput += pszReadBuf;
	//UpdateData(FALSE);
	//delete pszReadBuf;

	char *pAddonData = (char *)lParam;
	m_strOutput += pAddonData;
	UpdateData(FALSE);
	m_editOutput.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	delete pAddonData;
	return 0;

	return 0;
}