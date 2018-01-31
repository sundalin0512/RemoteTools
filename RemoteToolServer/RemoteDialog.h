#pragma once


// RemoteDialog 对话框

class RemoteDialog : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteDialog)

public:
	RemoteDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RemoteDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DESKTOP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnAcceleratorSaveBMP();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	HACCEL m_hAccel;
	CRITICAL_SECTION m_cs;
	bool m_isSaveBMP = false;
};
