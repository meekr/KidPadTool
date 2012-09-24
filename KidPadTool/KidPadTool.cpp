
// KidPadTool.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "KidPadTool.h"
#include "KidPadToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool __cdecl IsFlashEnvOK()
{
	bool rtv = false ;
	CLSID clsid;
	int ret = 0 ;
	HRESULT hr = 0 ;
	IUnknown * p = 0 ;
	CLSID   CLSID_FLASH = {   0xd27cdb6e,   0xae6d,   0x11cf,   {   0x96,   0xb8,   0x44,   0x45,   0x53,   0x54,   0x0,   0x0   }   }; 

	CoInitialize(0);
	if ( CLSIDFromProgID(L"ShockwaveFlash.ShockwaveFlash", &clsid ) == S_FALSE)
	{
		//û�а�װflash
		goto lb_exit; 
	}
	
	hr = CoCreateInstance(CLSID_FLASH, NULL, CLSCTX_INPROC_SERVER,  IID_IUnknown, (void**)&p) ;
	if(hr == S_OK)
	{
		rtv = true ;
	}
	else
	{
		//flash�Ѿ���װ�� ���ǰ汾̫�ͣ� ��������activex��ʽ����
		goto lb_exit;  
	}

lb_exit:
	if(p != 0)	{ p->Release(); }
	return rtv ;
}

// CKidPadToolApp

BEGIN_MESSAGE_MAP(CKidPadToolApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKidPadToolApp ����

CKidPadToolApp::CKidPadToolApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CKidPadToolApp ����

CKidPadToolApp theApp;


// CKidPadToolApp ��ʼ��

BOOL CKidPadToolApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	if (!IsFlashEnvOK()) {
		MessageBox(NULL, _T("��������û�а�װFlash Player�������԰�װ�汾̫�͡������Ȱ�װFlash PlayerȻ�����б�����"), _T("ERROR"), MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	// Nuvoton CCLi8 (2010.08.12)
	if (fsInitFileSystem()) {
		// Error here.
		return FALSE;
	}

	CKidPadToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
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
		fsUnmountPhysicalDisk (usbDisk);
		usbDiskDriver->onDriveDetach ();
//		m_usbDisks.erase (it->first);
		delete usbDisk;
		delete usbDiskDriver;
	}
	m_usbDisks.clear();

	return TRUE;
}

int CKidPadToolApp::ExitInstance()
{
	ExitUsb();

	return CWinAppEx::ExitInstance();
}