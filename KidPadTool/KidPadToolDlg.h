
// KidPadToolDlg.h : ͷ�ļ�
//

#pragma once
#include "shockwaveflash1.h"
#include "ApiController.h"


// CKidPadToolDlg �Ի���
class CKidPadToolDlg : public CDialog
{
// ����
public:
	ApiController apiController;

	CKidPadToolDlg(CWnd* pParent = NULL);	// ��׼���캯��
	void TestFunction01(void * p1, void * p2, void * p3);

// �Ի�������
	enum { IDD = IDD_KIDPADTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	BOOL ListInstalled(CString path);


	// ���ɵ���Ϣӳ�亯��
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