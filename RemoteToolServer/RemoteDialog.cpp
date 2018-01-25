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
}

void RemoteDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RemoteDialog, CDialogEx)
END_MESSAGE_MAP()


// RemoteDialog 消息处理程序

