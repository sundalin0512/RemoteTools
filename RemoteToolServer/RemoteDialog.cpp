// RemoteDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteToolServer.h"
#include "RemoteDialog.h"
#include "afxdialogex.h"


// RemoteDialog 对话框

IMPLEMENT_DYNAMIC(RemoteDialog, CDialogEx)

RemoteDialog::RemoteDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DESKTOP, pParent)
{

}

RemoteDialog::~RemoteDialog()
{
	DeleteCriticalSection(&m_cs);
}

void RemoteDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RemoteDialog, CDialogEx)
	ON_COMMAND(ID_ACCELERATOR32775, &RemoteDialog::OnAcceleratorSaveBMP)
END_MESSAGE_MAP()


// RemoteDialog 消息处理程序



void RemoteDialog::OnAcceleratorSaveBMP()
{
	// TODO: 在此添加命令处理程序代码
	EnterCriticalSection(&m_cs);
	m_isSaveBMP = true;
	LeaveCriticalSection(&m_cs);
}


BOOL RemoteDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	InitializeCriticalSection(&m_cs);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL RemoteDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
		return true;
	return CDialogEx::PreTranslateMessage(pMsg);
}
