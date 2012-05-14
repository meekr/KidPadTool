
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
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnDeviceChange (UINT nEventType, DWORD_PTR dwData);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	CShockwaveflash1 flashUI;
	DECLARE_EVENTSINK_MAP()
	void FSCommandShockwaveflash1(LPCTSTR command, LPCTSTR args);
	afx_msg void OnSize(UINT nType, int cx, int cy);


public:
	TCHAR m_pszBuffer[MAX_PATH * 2];

protected:
	CString m_workingFolderName;
	CString	m_pcLocalHostName;
	CString m_programUsbDiskName;

	CString m_driveTempName;
	CString m_driveNANDName;
	CString m_driveSDName;
	CString m_deviceDriveName;

	char *m_pu8xmlBuffer;

public:
	void FlashCallShockwaveflash1(LPCTSTR request);
};
