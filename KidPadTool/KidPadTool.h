
// KidPadTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

// Nuvoton CCLi8 (2010.08.12)
#include <string>
#include <map>

#ifdef	_DEMO_USE_STDUSBDISKDRIVER_
#include "StdUsbDiskDriver.h"
#else
#include "NvtOffsetUsbDiskDriver.h"
#endif

#include "FileSystem.h"


// CKidPadToolApp:
// �йش����ʵ�֣������ KidPadTool.cpp
//

class CKidPadToolApp : public CWinAppEx
{
public:
	CKidPadToolApp();

// ��д
	public:
	virtual BOOL InitInstance();

	// Implementation
	// Nuvoton CCLi8 (2010.08.12)
	std::map <std::basic_string<TCHAR>, PDISK_T *> m_usbDisks;

// ʵ��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL ExitUsb();


	virtual int ExitInstance();
};

extern CKidPadToolApp theApp;