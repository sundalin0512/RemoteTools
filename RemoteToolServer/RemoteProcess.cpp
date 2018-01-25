// RemoteProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteToolServer.h"
#include "RemoteProcess.h"
#include "afxdialogex.h"


// RemoteProcess 对话框

IMPLEMENT_DYNAMIC(RemoteProcess, CDialogEx)

RemoteProcess::RemoteProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PROCESS, pParent)
{

}

RemoteProcess::~RemoteProcess()
{
}

void RemoteProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_listProcess);
}


BEGIN_MESSAGE_MAP(RemoteProcess, CDialogEx)
END_MESSAGE_MAP()


// RemoteProcess 消息处理程序


BOOL RemoteProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_listProcess.ModifyStyle(LVS_ICON, LVS_REPORT | LVS_SINGLESEL);
	m_listProcess.SetExtendedStyle(m_listProcess.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listProcess.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 200);
	m_listProcess.InsertColumn(1, _T("进程ID"), LVCFMT_LEFT, 50);
	m_listProcess.InsertColumn(2, _T("父进程ID"), LVCFMT_LEFT, 50);
	m_listProcess.InsertColumn(3, _T("映像路径"), LVCFMT_LEFT, 400);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
