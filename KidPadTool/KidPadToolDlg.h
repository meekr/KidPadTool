
// KidPadToolDlg.h : 头文件
//

#pragma once
#include "shockwaveflash1.h"
#include "ApiController.h"


// CKidPadToolDlg 对话框
class CKidPadToolDlg : public CDialog
{
// 构造
public:
	ApiController apiController;

	CKidPadToolDlg(CWnd* pParent = NULL);	// 标准构造函数
	void TestFunction01(void * p1, void * p2, void * p3);

// 对话框数据
	enum { IDD = IDD_KIDPADTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	BOOL ListInstalled(CString path);


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnCreate (LPCREATESTRUCT lpc) ;
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC * _cdc);
	afx_msg void OnActivateApp(BOOL, DWORD);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnDeviceChange (UINT nEventType, DWORD_PTR dwData);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	CShockwaveflash1 flashUI;
	DECLARE_EVENTSINK_MAP()
	void FSCommandShockwaveflash1(LPCTSTR command, LPCTSTR args);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL CKidPadToolDlg::PreTranslateMessage(MSG *pMsg);
	BOOL uiLoaded;

public:
	TCHAR m_pszBuffer[MAX_PATH * 2];

protected:
	CString m_workingFolderName;

	char *m_pu8xmlBuffer;

public:
	void FlashCallShockwaveflash1(LPCTSTR request);

	CShockwaveflash1 CShockwaveflash0;
};