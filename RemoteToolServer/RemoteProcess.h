#pragma once
#include "afxcmn.h"


// RemoteProcess 对话框

class RemoteProcess : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteProcess)

public:
	RemoteProcess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RemoteProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	public:
	CListCtrl m_listProcess;
	virtual BOOL OnInitDialog();
	TcpClient* m_socketClient;
	BOOL UpdateListCtrl(std::wstring data);

	afx_msg void OnRclickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTerminateProcess();
	CString selectPID;
};
