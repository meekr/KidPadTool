// KidPadToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KidPadTool.h"
#include "KidPadToolDlg.h"
#include "base64.h"
#include <iostream>
#include <time.h>
#include "tinyxml.h"
#include "Dbt.h"

#define _NUTIL_LIB_REFERENCE 1
#include <nutil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



clock_t volatile time_flag = 0 ;
// CKidPadToolDlg 对话框
CKidPadToolDlg * app_ptr = 0 ;


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

	OSVERSIONINFO   OsVersionInfo = {0}; 
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OsVersionInfo); 
	if( (OsVersionInfo.dwMajorVersion == 5) && (OsVersionInfo.dwMinorVersion == 1))
	{
		//xp
		this->flashUI.put_WMode(_T("Transparent"));
	}
	else
	{
		//2003, vista, win7, 2008
		this->flashUI.put_WMode(_T("Opaque"));
	}
}

BEGIN_MESSAGE_MAP(CKidPadToolDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
	ON_WM_DEVICECHANGE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

LRESULT CKidPadToolDlg::OnNcHitTest(CPoint point)
{
	ScreenToClient(&point);
	if (point.x > 0 && point.x < 918 && point.y > 0 && point.y < 27)
		return HTCAPTION;
	return CWnd::OnNcHitTest(point);
}

// CKidPadToolDlg 消息处理程序

BOOL CKidPadToolDlg::OnInitDialog()
{
	uiLoaded = FALSE;

	SetWindowTransparentForColor(this->GetSafeHwnd(), RGB(0xAB, 0xC1, 0x23));
	CDialog::OnInitDialog();

	apiController.ownerWindow = this;
	apiController.flashUI = &flashUI;

	m_pu8xmlBuffer = NULL;

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetWindowText(_T("E巧派 助手"));
#ifdef _BYM
	SetWindowText(_T("儿童学习机 助手"));	
#endif
#ifdef _XBW
	SetWindowText(_T("小霸王学习机 助手"));	
#endif

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
	
	app_ptr = this ;
		
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKidPadToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{

		/*CRect rect2;
		CDC *pCDC;

		CBrush RedBrush(RGB(0xAB, 0xC1, 0x23));
		CBrush BlueBrush(RGB(0xAB, 0xC1, 0x23));

		GetClientRect(&rect2);
		pCDC = GetDC();
		pCDC->FillRect(rect2, &RedBrush);
		ReleaseDC(pCDC);

		GetClientRect(&rect2);
		pCDC = GetDC();
		pCDC->FillRect(rect2, &BlueBrush);
		ReleaseDC(pCDC);*/


		CDialog::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKidPadToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CKidPadToolDlg::DetatchUsbDevice()
{
	TCHAR * diskDriveNameBuf = apiController.GetConnectUsbDiskName();
	if(diskDriveNameBuf[0] >0)
	{

		std::map <std::basic_string<TCHAR>, PDISK_T *> &usbDisks = (((CKidPadToolApp *)AfxGetApp())->m_usbDisks);
		if(usbDisks.empty() == false)
		{
			std::map <std::basic_string<TCHAR>, PDISK_T *>::iterator it = usbDisks.find (diskDriveNameBuf);
			if (it != usbDisks.end ())
			{
				PDISK_T *usbDisk = it->second;
				UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (usbDisk->ptDriver);

				//fsUnmountPhysicalDisk (usbDisk);
				//fsPhysicalDiskDisconnected(usbDisk);

				usbDisks.erase (diskDriveNameBuf);
				usbDiskDriver->onDriveDetach ();

				//delete usbDisk;
				delete usbDiskDriver;
			}
		}
		(((CKidPadToolApp *)AfxGetApp ())->m_usbDisks).clear();
	}
	return ;
}

afx_msg BOOL CKidPadToolDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	
	/*DEV_BROADCAST_HDR *dbh = (DEV_BROADCAST_HDR *) dwData;
	
	if( (dbh == NULL) || (dbh->dbch_devicetype != DBT_DEVTYP_VOLUME) )
	{
		return FALSE ;
	}
	DEV_BROADCAST_DEVICEINTERFACE * dbd = (DEV_BROADCAST_DEVICEINTERFACE*) dwData;*/
	switch(nEventType)
	{
		case DBT_DEVICEREMOVECOMPLETE:
		{
			::OutputDebugString(_T("DBT_DEVICEREMOVECOMPLETE\r\n"));
			if(apiController.IsConnected(false) == true)
			{
				//((CKidPadToolApp *)AfxGetApp ())->ExitUsb();
				DetatchUsbDevice();
				TRACE("USB disconnected\n");
				//设备物理断开完成
				flashUI.CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>3</string></arguments></invoke>"));
				time_flag = ::clock();
				apiController.SetConnStatus(false);
				//设备完全断开， 可以进行新的连接
				flashUI.CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
				return TRUE;
			}

			break;
		}
		case DBT_DEVICEARRIVAL:
		{
			::OutputDebugString(_T("DBT_DEVICEARRIVAL\r\n"));
			if(time_flag != 0)
			{
				clock_t duration;
				clock_t tv = ::clock();
				duration = tv - time_flag;
				time_flag = 0 ;
				if(duration < 3000)
				{
					::OutputDebugStringA("设备插拔时间间隔不能小于3秒！\r\n");
					return TRUE;
				}
			}
			if(apiController.IsConnected(false) == false)
			{
				//设备物理插入完成
				flashUI.CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>2</string></arguments></invoke>"));
				//延时3秒让os完成它需要的处理
				//Sleep(3000);
				//进行扫描， 判断是否可用的设备
				apiController.ScanUsbDisk();
				return TRUE;
			}
			if(apiController.IsConnected(false) == true)
			{
				flashUI.CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>1</string></arguments></invoke>"));
				TRACE("USB connected\n");
			}
			else
			{
				flashUI.CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>-1</string></arguments></invoke>"));
			}
			break;
		}
	}
	return FALSE;
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
	else if (0 == _tcscmp(_T("minimize"), command))
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (0 == _tcscmp(_T("maximize"), command))
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else
	{
		if (0 == _tcscmp(_T("loaded"), command))
		{
			apiController.IsConnected(true);
			CShockwaveflash0.MoveWindow(0, 0, 0, 0);
			this->flashUI.MoveWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			uiLoaded = TRUE;
		}
		apiController.DispatchFlashCommand(command, args);
	}
}

void CKidPadToolDlg::FlashCallShockwaveflash1(LPCTSTR request)
{
	CString sXml = CString(request);

	// parse request  
	TiXmlDocument request_xml;
	BSTR b = SysAllocString(request);
	char * p = _com_util::ConvertBSTRToString(b);
	request_xml.Parse(p);
	const char* request_name = request_xml.RootElement()->Attribute("name");

	TiXmlElement *node = (TiXmlElement *)request_xml.RootElement()->FirstChild()->FirstChild();
	const char* request_args = node->GetText();

	apiController.DispatchFlashCall(request_name, request_args);
	if(b !=0 )	{ SysFreeString(b) ;}
	if(p != 0)	{ delete[] p; }
}

void CKidPadToolDlg::OnSize(UINT nType, int cx, int cy)
{
	if (NULL != flashUI.GetSafeHwnd())
		flashUI.MoveWindow(0, 0, cx, cy);
}

BOOL CKidPadToolDlg::PreTranslateMessage(MSG *pmsg)
{
	/*if(pmsg->message == WM_KEYDOWN || pmsg->message == WM_CHAR)
	{
		SendMessage(pmsg->message,pmsg->wParam,pmsg->lParam);
	}*/
	if((pmsg->message == WM_KEYDOWN) || ((pmsg->message == WM_KEYUP) ) )
	{
		if( (pmsg->wParam == VK_TAB) 
			|| (pmsg->wParam == VK_LEFT) 
			|| (pmsg->wParam == VK_RIGHT) 
			|| (pmsg->wParam == VK_UP) 
			|| (pmsg->wParam == VK_DOWN) 
			|| (pmsg->wParam == VK_RETURN) 
			|| (pmsg->wParam == 0xA) 
		)
		{
			if(pmsg->message == WM_KEYDOWN)
			{
				::TranslateMessage(pmsg);
				SendMessage(pmsg->message,pmsg->wParam,pmsg->lParam);
			}
			return TRUE ;
		}
	}
	else if((pmsg->message == WM_CHAR)|| (pmsg->message == WM_SYSCHAR) )
	//else if((pmsg->message == WM_CHAR))
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
afx_msg BOOL CKidPadToolDlg::OnEraseBkgnd(CDC * _cdc)
{
	OSVERSIONINFO   OsVersionInfo = {0}; 
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OsVersionInfo); 
	if( ((OsVersionInfo.dwMajorVersion == 5) && (OsVersionInfo.dwMinorVersion == 1))
		|| !uiLoaded )
	{
		//xp
		FixBkgndTransparentForColor2(this->GetSafeHwnd(), RGB(0xAB, 0xC1, 0x23), _cdc->m_hDC);
		return TRUE;
	}
	else
	{
		//2003, vista, win7, 2008
		
		return FALSE;
	}
}
afx_msg void CKidPadToolDlg::OnActivateApp(BOOL v1, DWORD v2)
{
	/*RECT rc = {0} ;
	this->GetClientRect(&rc);
	this->flashUI.MoveWindow(0, 0, rc.right-rc.left, rc.bottom-rc.top, FALSE);
	this->RedrawWindow(0,0, RDW_UPDATENOW|RDW_ALLCHILDREN);
	return  ;*/
}