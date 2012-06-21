
// KidPadToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KidPadTool.h"
#include "KidPadToolDlg.h"
#include "base64.h"
#include <iostream>
#include "tinyxml.h"
#include "Dbt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 703

// CKidPadToolDlg �Ի���




CKidPadToolDlg::CKidPadToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKidPadToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CKidPadToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH1, flashUI);
}

BEGIN_MESSAGE_MAP(CKidPadToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
	ON_WM_DEVICECHANGE()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

LRESULT CKidPadToolDlg::OnNcHitTest(CPoint point)
{
	ScreenToClient(&point);
	if (point.x > 0 && point.x < 950 && point.y > 0 && point.y < 27)
		return HTCAPTION;
	return CWnd::OnNcHitTest(point);
}

// CKidPadToolDlg ��Ϣ�������

BOOL CKidPadToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	apiController.ownerWindow = this;
	apiController.flashUI = &flashUI;

	m_pu8xmlBuffer = NULL;

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	SetWindowText(_T("ͯ��1��"));

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	TCHAR szFolder[MAX_PATH * 2];
	HMODULE hModule = GetModuleHandle( 0 );
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	m_workingFolderName = szFolder;
	m_workingFolderName = m_workingFolderName.Left(m_workingFolderName.ReverseFind(_T('\\')));
	
	this->flashUI.LoadMovie(0, m_workingFolderName + "\\KidPadUI.swf");
	CWnd::SetWindowPos(NULL, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_NOZORDER);
	this->flashUI.MoveWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CKidPadToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CKidPadToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

afx_msg BOOL CKidPadToolDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	DEV_BROADCAST_DEVICEINTERFACE * dbd = (DEV_BROADCAST_DEVICEINTERFACE*) dwData;
	switch(nEventType)
	{
		case DBT_DEVICEREMOVECOMPLETE:
			((CKidPadToolApp *)AfxGetApp ())->ExitUsb();
	flashUI.CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
			break;
		case DBT_DEVICEARRIVAL:
			fsInitFileSystem();
			apiController.ScanUsbDisk();
			break;
	}
	return true;
}

BEGIN_EVENTSINK_MAP(CKidPadToolDlg, CDialog)
	ON_EVENT(CKidPadToolDlg, IDC_SHOCKWAVEFLASH1, 150, CKidPadToolDlg::FSCommandShockwaveflash1, VTS_BSTR VTS_BSTR)
	ON_EVENT(CKidPadToolDlg, IDC_SHOCKWAVEFLASH1, 197, CKidPadToolDlg::FlashCallShockwaveflash1, VTS_BSTR)
END_EVENTSINK_MAP()

void CKidPadToolDlg::FSCommandShockwaveflash1(LPCTSTR command, LPCTSTR args)
{
	if (0 == _tcscmp(_T("quit"), command))
	{
		CDialog::OnCancel();
	}
	else
	{
		apiController.DispatchFlashCommand(command, args);
	}
}

void CKidPadToolDlg::FlashCallShockwaveflash1(LPCTSTR request)
{
	CString sXml = CString(request);
	
	// parse request  
    TiXmlDocument request_xml;
	request_xml.Parse(CT2CA(sXml));
    const char* request_name = request_xml.RootElement()->Attribute("name");

	TiXmlElement *node = (TiXmlElement *)request_xml.RootElement()->FirstChild()->FirstChild();
	const char* request_args = node->GetText();

	apiController.DispatchFlashCall(request_name, request_args);
}

void CKidPadToolDlg::OnSize(UINT nType, int cx, int cy)
{
	if (NULL != flashUI.GetSafeHwnd())
		flashUI.MoveWindow(0, 0, cx, cy);
}

BOOL CKidPadToolDlg::PreTranslateMessage(MSG *pmsg)
{
	if(pmsg->message == WM_KEYDOWN || pmsg->message == WM_CHAR)
	{
		SendMessage(pmsg->message,pmsg->wParam,pmsg->lParam);
	}
	else if((WM_RBUTTONDOWN == pmsg->message) || (WM_RBUTTONDBLCLK == pmsg->message))
	{
		return TRUE;
	}
	else
	{
		return CDialog::PreTranslateMessage(pmsg);
	}
	
	return CDialog::PreTranslateMessage(pmsg);
}