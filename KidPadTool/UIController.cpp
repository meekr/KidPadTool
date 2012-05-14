#include "stdafx.h"
#include "base64.h"
#include <iostream>
#include "UIController.h"



int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg==BFFM_INITIALIZED)
		SendMessage(hWnd, BFFM_SETSELECTION,TRUE, lpData);
	return 0;
}

CString UIController::BrowsePC()
{
	LPMALLOC pMalloc;
	CString ret;

	// Gets the Shell's default allocator
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		LPITEMIDLIST pidl;

		bi.hwndOwner = ownerWindow->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = m_pszBuffer;
		bi.lpszTitle = _T("Select a working directory...");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = (LPARAM)m_pszBuffer;

		// This next call issues the dialog box.
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, m_pszBuffer))
			{ 
				// At this point pszBuffer contains the selected path
				ret = m_pszBuffer;
				//m_pcLocalHostName = m_pszBuffer;
				//flashUI.CallFunction(_T("<invoke name='FL_setPCPath'><arguments><string>") + m_pcLocalHostName + _T("</string></arguments></invoke>"));
				//GetPCLocalHostList();
			}

			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free(pidl);
		}
		// Release the shell's allocator.
		pMalloc->Release();
	}
	return ret;
}

void UIController::DispatchFlashCommand(CString command, CString args)
{
	if (command == "loaded")
	{
		
	}
	else if (command == "quit")
	{
		
	}
	else if (command == "browse")
	{
	}
}