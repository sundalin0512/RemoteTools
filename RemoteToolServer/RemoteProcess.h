#pragma once
#include "afxcmn.h"


// RemoteProcess �Ի���

class RemoteProcess : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteProcess)

public:
	RemoteProcess(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~RemoteProcess();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	public:
	CListCtrl m_listProcess;
	virtual BOOL OnInitDialog();
};