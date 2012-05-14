
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
