
// RemoteToolServerDlg.h: 头文件
//

#pragma once
#include "afxcmn.h"
#include "RemoteDialog.h"
#include "RemoteCmdDialog.h"
#include "RemoteFileDialog.h"
#include "RemoteProcess.h"

// CRemoteToolServerDlg 对话框
class CRemoteToolServerDlg : public CDialog
{
	// 构造
	public:
	CRemoteToolServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTETOOLSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	typedef struct PPPPPPQQQ
	{
		TcpClient* tcpClient;
		RemoteDialog* remoteDialog;
		RemoteCmdDialog* remoteCmdDialog;
		RemoteFileDialog* remoteFileDialog;
		RemoteProcess* remoteProcessDialog;

	}TcpClientData;
	// 实现
	protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	public:
	CListCtrl m_clientList;
	afx_msg void OnMenuCmd();
	afx_msg void OnMenuProcess();
	afx_msg void OnMenuDesktop();
	afx_msg void OnRclickListClient(NMHDR *pNMHDR, LRESULT *pResult);

	public:
	TcpClientData * m_selectClient;
	static int AcceptCallback(TcpClient* client, void* uesrParam);
	virtual BOOL DestroyWindow();
	static int ReceiveCallback(TcpClient::MessageData* message, void* uesrParam);
	afx_msg void OnMenuFile();
	TcpServer* m_server;
	static void SaveBitmap(CString strFilePath, CBitmap &Bitmap);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
