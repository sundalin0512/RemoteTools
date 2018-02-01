#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// RemoteFileDialog 对话框

class RemoteFileDialog : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteFileDialog)

public:
	RemoteFileDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RemoteFileDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	public:
	CComboBox m_comboDisplayTypeRemote;
	CString m_strDisplayTypeRemote;
	CEdit m_editDirPathRemote;
	CString m_strDirPathRemote;
	CListCtrl m_listFileRemote;

	CComboBox m_comboDisplayTypeLocal;
	CString m_strDisplayTypeLocal;
	CEdit m_editDirPathLocal;
	CString m_strDirPathLocal;
	CListCtrl m_listFileLocal;
	std::vector<CString> m_vecPathRemote;
	std::vector<CString> m_vecFilenameRemote;
	std::vector<CString> m_vecPathLocal;
	std::vector<CString> m_vecFilenameLocal;
	CImageList* m_pImageList;
	afx_msg void OnSelchangeRemoteComboDisplaytype();
	afx_msg void OnDblclkRemoteListFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangeLocalComboDisplaytype();
	afx_msg void OnDblclkLocalListFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	void FillRemoteFileList(wchar_t* data);
	TcpClient* m_socketClient;
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonUpload();
	void FindLocalFile();

	void FillLocalFileList();
	void SaveRemoteFile(char* data, size_t length);

	void SendDownloadRequset(CString remoteFileName, CString LocalPathFileName);
	void OnDownloadQuere(std::wstring path, size_t offsetRemote);
};
