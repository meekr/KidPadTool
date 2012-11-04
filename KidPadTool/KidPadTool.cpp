//extern  int volatile usb_disk_status ; 

// KidPadTool.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "KidPadTool.h"
#include "KidPadToolDlg.h"
#define _NUTIL_LIB_REFERENCE 1
#include <nutil.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool __cdecl IsFlashEnvOK()
{
	bool rtv = false ;
	CLSID clsid;
	int ret = 0 ;
	HRESULT hr = 0 ;
	IUnknown * p = 0 ;
	long flashver = 0 ;
	CLSID   CLSID_FLASH = {   0xd27cdb6e,   0xae6d,   0x11cf,   {   0x96,   0xb8,   0x44,   0x45,   0x53,   0x54,   0x0,   0x0   }   }; 

	CoInitialize(0);
	if ( CLSIDFromProgID(L"ShockwaveFlash.ShockwaveFlash", &clsid ) == S_FALSE)
	{
		//没有安装flash
		goto lb_exit; 
	}
	
	hr = CoCreateInstance(CLSID_FLASH, NULL, CLSCTX_INPROC_SERVER,  IID_IUnknown, (void**)&p) ;
	if(hr == S_OK)
	{
		DISPID dispid = 0 ;
		BSTR _b = ::SysAllocString(L"FlashVersion");
		DISPPARAMS disp_params = {0};
		VARIANT v = {0}, v2 = {0} ;
		EXCEPINFO excepInfo = {0};
		UINT nArgErr = (UINT)-1;


		hr = ((IDispatch*)p)->GetIDsOfNames(IID_NULL, &_b, 1,LOCALE_SYSTEM_DEFAULT, &dispid);

		hr = ((IDispatch*)p)->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp_params, &v, &excepInfo, &nArgErr);
		if(hr == S_OK)
		{
			flashver = v.lVal >> 16;
		}
		::SysFreeString(_b);
		if(flashver >= 11)	{ rtv = true ; }
		
	}
	else
	{
		//flash已经安装， 可是版本太低， 不能用于activex方式加载
		goto lb_exit;  
	}

lb_exit:

	if(p != 0)	{ p->Release(); }

	return rtv ;
}
bool __cdecl IsFlashInstallCabValid()
{
	bool rtv = false ;
	TCHAR buff[2048] = {0}, * p = 0 ;
	size_t l = 0 ;
	HANDLE mhd = 0 ;
	
	::GetModuleFileName(NULL, buff, 2048 / sizeof(TCHAR));
	l = ::_tcslen(buff);

	p = buff + l - 1 ;
	while(1)
	{
		if( (p[0] == 0x5C) || (p[0] == 0x2F) )
		{
			p[1] = 0 ;
			break ;
		}
		p --;
		if(p < buff)	{ break ; }
	}

	_tcscat(buff, _T("FlashPlayerInstall.exe"));

	mhd = ::CreateFile(buff
			, GENERIC_READ
			,0		//FILE_SHARE_READ | FILE_SHARE_WRITE ,
			,0
			,OPEN_EXISTING
			,FILE_ATTRIBUTE_NORMAL//|FILE_FLAG_OVERLAPPED,
			,0
			);
	if(mhd != INVALID_HANDLE_VALUE)
	{
		rtv = true ;
		::CloseHandle(mhd);
	}

	return rtv ; 
}
bool __cdecl InstalFlashPlayer()
{
	size_t l = 0 ;
	TCHAR * p = 0 ;
	CmdAppParam * cap = (CmdAppParam*)malloc(sizeof(CmdAppParam));
	memset(cap, 0, sizeof(CmdAppParam));
	cap->cmd_str = (TCHAR*)malloc(2048);
	memset(cap->cmd_str, 0, 2048);
	::GetModuleFileName(NULL, cap->cmd_str, 2048 / sizeof(TCHAR));
	l = ::_tcslen(cap->cmd_str);

	p = cap->cmd_str + l - 1 ;
	while(1)
	{
		if( (p[0] == 0x5C) || (p[0] == 0x2F) )
		{
			p[1] = 0 ;
			break ;
		}
		p --;
		if(p < cap->cmd_str)	{ break ; }
	}

	_tcscat(cap->cmd_str, _T("FlashPlayerInstall.exe"));

	

	::RunWinAppAsync(cap);

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CKidPadToolApp

BEGIN_MESSAGE_MAP(CKidPadToolApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKidPadToolApp 构造

CKidPadToolApp::CKidPadToolApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CKidPadToolApp 对象

CKidPadToolApp theApp;


// CKidPadToolApp 初始化

BOOL CKidPadToolApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	//For testing
	
	//End Testing

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//检测及安装adobe flash player
	if (IsFlashEnvOK() == false)
	{
		if(IsFlashInstallCabValid() == true)
		{
			int ret = MessageBox(NULL, _T("程序功能需要Adobe Flash Player 11及以上版本,\r\n点击【是】现在安装Adobe Flash Player 11.4, \r\n点击【否】手工安装其他版本。"),_T("安装Flash Player"), MB_YESNO );
			if(ret == IDYES)
			{
				InstalFlashPlayer();
				if (IsFlashEnvOK() == false)
				{
					MessageBox(NULL, _T("您机器上没有安装Flash Player，或者以安装版本太低。请您先安装Flash Player 11或以上版本然后运行本程序！"), _T("ERROR"), MB_OK|MB_ICONSTOP);
					return FALSE;
				}
			}
		}
		else
		{
			MessageBox(NULL, _T("您机器上没有安装Flash Player，或者以安装版本太低。请您先安装Flash Player 11或以上版本然后运行本程序！"), _T("ERROR"), MB_OK|MB_ICONSTOP);
			return FALSE;
		}
	}
	//结束检测

	// Nuvoton CCLi8 (2010.08.12)
	int ret = fsInitFileSystem();
	if (ret) {
		// Error here.
		return FALSE;
	}

	CKidPadToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CKidPadToolApp::ExitUsb()
{
	// TODO: Add your specialized code here and/or call the base class
	// Nuvoton CCLi8 (2010.08.12)
	std::map <std::basic_string<TCHAR>, PDISK_T *>::iterator it;
	for (it = m_usbDisks.begin (); it != m_usbDisks.end (); it ++) {
		PDISK_T *usbDisk = it->second;
		UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (usbDisk->ptDriver);
		//fsUnmountPhysicalDisk (usbDisk);
		//usbDiskDriver->onDriveDetach ();
//		m_usbDisks.erase (it->first);
		//delete usbDisk;
		//delete usbDiskDriver;
		//fsPhysicalDiskDisconnected(usbDisk);
		//delete usbDisk;
		usbDiskDriver->onDriveDetach ();
		delete usbDiskDriver;
	}
	m_usbDisks.clear();

	/*if(m_usbDisks.empty() == false)
	{
		PDISK_T *usbDisk = m_usbDisks.begin()->second;
		UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (usbDisk->ptDriver);

		fsUnmountPhysicalDisk (usbDisk);

		m_usbDisks.erase(m_usbDisks.begin()->first);
		usbDiskDriver->onDriveDetach ();

		delete usbDisk;
		delete usbDiskDriver;


		m_usbDisks.clear();
	}*/

	return TRUE;
}

BOOL CKidPadToolApp::ExitUsb2()
{
	// TODO: Add your specialized code here and/or call the base class
	// Nuvoton CCLi8 (2010.08.12)
//	std::map <std::basic_string<TCHAR>, PDISK_T *>::iterator it;
//	for (it = m_usbDisks.begin (); it != m_usbDisks.end (); it ++) {
//		PDISK_T *usbDisk = it->second;
//		UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (usbDisk->ptDriver);
//		//fsUnmountPhysicalDisk (usbDisk);
//		//usbDiskDriver->onDriveDetach ();
////		m_usbDisks.erase (it->first);
//		//delete usbDisk;
//		//delete usbDiskDriver;
//		//fsPhysicalDiskDisconnected(usbDisk);
//		//delete usbDisk;
//		usbDiskDriver->onDriveDetach ();
//		delete usbDiskDriver;
//	}
//	m_usbDisks.clear();
	if(m_usbDisks.empty() == false)
	{
		std::map <std::basic_string<TCHAR>, PDISK_T *>::iterator it;
		for (it = m_usbDisks.begin(); it != m_usbDisks.end(); it ++) {
			PDISK_T *usbDisk = it->second;
			UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (usbDisk->ptDriver);
			fsUnmountPhysicalDisk (usbDisk);
			usbDiskDriver->onDriveDetach ();
			//m_usbDisks.erase (it->first);
			delete usbDisk;
			delete usbDiskDriver;
		}
	}

	return TRUE;
}

int CKidPadToolApp::ExitInstance()
{
	//ExitUsb();
	ExitUsb2();
	return CWinAppEx::ExitInstance();
}