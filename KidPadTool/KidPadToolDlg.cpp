
// KidPadToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KidPadTool.h"
#include "KidPadToolDlg.h"
#include "base64.h"
#include <iostream>
#include "tinyxml.h"
#include "Dbt.h"

#define _NUTIL_LIB_REFERENCE 1
#include <nutil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CKidPadToolDlg �Ի���


CKidPadToolDlg * app_ptr = 0 ;
static void __cdecl TestCmdApp(void * p1, void * p2, void * p3)
{
	return app_ptr->TestFunction01(p1, p2, p3);
}

void CKidPadToolDlg::TestFunction01(void * p1, void * p2, void * p3)
{
	const char * buff = (char*)p1 ;
	if(buff == 0)
	{
		::OutputDebugStringA("Completed!\n");
	}
	else
	{
		//::OutputDebugStringA("\r\n");
		::OutputDebugStringA(buff);
	}
	return ;
}


CKidPadToolDlg::CKidPadToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKidPadToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CKidPadToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH1, flashUI);
	//DDX_Control(pDX, IDC_SHOCKWAVEFLASH0, loader);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH0, CShockwaveflash0);
}

BEGIN_MESSAGE_MAP(CKidPadToolDlg, CDialog)
	ON_WM_CREATE()
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
	SetWindowText(_T("E����"));

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	TCHAR szFolder[MAX_PATH * 2];
	HMODULE hModule = GetModuleHandle( 0 );
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	m_workingFolderName = szFolder;
	m_workingFolderName = m_workingFolderName.Left(m_workingFolderName.ReverseFind(_T('\\')));
	
	this->flashUI.LoadMovie(0, m_workingFolderName + "\\EPadClient.swf");
	flashUI.MoveWindow(0, 0, 0, 0);
	this->CShockwaveflash0.LoadMovie(0, m_workingFolderName + "\\Splash.swf");
	CShockwaveflash0.MoveWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	CWnd::SetWindowPos(NULL, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_NOZORDER);
	
	

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
		CShockwaveflash0.DestroyWindow();
		CDialog::OnCancel();
	}
	else
	{
		if (0 == _tcscmp(_T("loaded"), command))
		{
			CShockwaveflash0.MoveWindow(0, 0, 0, 0);
			this->flashUI.MoveWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		}
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

afx_msg BOOL CKidPadToolDlg::OnCreate (LPCREATESTRUCT lpc)
{
	 if (CDialog::OnCreate(lpc) == -1)
	return -1;

	SetWindowTransparentForColor(this->GetSafeHwnd(), RGB(0xAB, 0xC1, 0x23));
	 
	return 0;
}