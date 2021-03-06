
// KidPadTool.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

// Nuvoton CCLi8 (2010.08.12)
#include <string>
#include <map>

#ifdef	_DEMO_USE_STDUSBDISKDRIVER_
#include "StdUsbDiskDriver.h"
#else
#include "NvtOffsetUsbDiskDriver.h"
#endif

#include "FileSystem.h"

#define WINDOW_WIDTH 978
#define WINDOW_HEIGHT 618


// CKidPadToolApp:
// 有关此类的实现，请参阅 KidPadTool.cpp
//

class CKidPadToolApp : public CWinAppEx
{
public:
	CKidPadToolApp();

// 重写
	public:
	virtual BOOL InitInstance();

	// Implementation
	// Nuvoton CCLi8 (2010.08.12)
	std::map <std::basic_string<TCHAR>, PDISK_T *> m_usbDisks;

// 实现

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL ExitUsb();
	BOOL ExitUsb2();

	virtual int ExitInstance();
};

extern CKidPadToolApp theApp;