ApiController *api_pointer = 0 ;
CShockwaveflash1 *flash_pointer = 0;
CString m_downloadDirectory;
CString m_driveNANDName;


typedef struct _tagTRANSFER_PARAMETER_BLOCK
{
    TCHAR sourceFile[1024];
	TCHAR targetFile[1024];
	CShockwaveflash1 *flash_pointer;
} TRANSFER_PARAMETER_BLOCK;

typedef struct _tagBIG_UI_DATA_PARAMETER_BLOCK
{
    TCHAR method[1024];
	TCHAR argument[1024];
	CShockwaveflash1 *flash_pointer;
} BIG_UI_DATA_PARAMETER_BLOCK;

typedef struct _tagGENERAL_PARAMETER_BLOCK
{
    TCHAR parameter1[1024];
	TCHAR parameter2[1024];
	TCHAR parameter3[1024];
	CShockwaveflash1 *flash_pointer;
} GENERAL_PARAMETER_BLOCK;

void __cdecl threadGetDownloadedApps(void * p)
{
	GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)p;
	CShockwaveflash1 *flash_pointer = tpb->flash_pointer;
	CString downloadDirectory = CString(tpb->parameter1);

	CFileFind finder;
	TCHAR fileSizeBuf[16];
	BOOL found = finder.FindFile(downloadDirectory + _T("*.npk"));
	
	while(found)
	{
		found = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), finder.GetLength());
		CString ret_value;
		ret_value += finder.GetFilePath();
		ret_value += ",";
		ret_value += fileSizeBuf;
		flash_pointer->CallFunction(_T("<invoke name='FL_addDownloadedApp'><arguments><string>") + ret_value + _T("</string></arguments></invoke>"));
	}
	finder.Close();

	free(p);
	_endthread();
}

void __cdecl threadGetLocalMedia(void * p)
{
	GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)p;
	CShockwaveflash1 *flash_pointer = tpb->flash_pointer;
	CString directory = CString(tpb->parameter1);
	CString extension = CString(tpb->parameter2);
	CString mediatype = CString(tpb->parameter3);

	CFileFind finder;
	TCHAR fileSizeBuf[16];
	BOOL found = finder.FindFile(directory + _T("\\") + _T("*.*"));
	while(found)
	{
		found = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		CString filename = finder.GetFileName();
		CString ext = filename.Right(filename.GetLength() - filename.ReverseFind(_T('.')) - 1);
		if (extension.Find(ext.MakeLower()) == -1)
			continue;

		_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), finder.GetLength());
		CString ret_value;
		ret_value += mediatype;
		ret_value += _T(",");
		ret_value += fileSizeBuf;
		ret_value += _T(",");
		ret_value += finder.GetFilePath();
		flash_pointer->CallFunction(_T("<invoke name='FL_addLocalMedia'><arguments><string>") + ret_value + _T("</string></arguments></invoke>"));
	}
	finder.Close();

	free(p);
	_endthread();
}

void __cdecl threadSendMessage2Flash(void * p)
{
	BIG_UI_DATA_PARAMETER_BLOCK *tpb = (BIG_UI_DATA_PARAMETER_BLOCK *)p;
	CShockwaveflash1 *flash_pointer = tpb->flash_pointer;
	CString method = CString(tpb->method);
	CString argument = CString(tpb->argument);

	flash_pointer->CallFunction(_T("<invoke name='") + method + _T("'><arguments><string>") + argument + _T("</string></arguments></invoke>"));

	free(p);
	_endthread();
}

void __cdecl threadUpdatePercentage(void * p)
{
	TRANSFER_PARAMETER_BLOCK *tpb = (TRANSFER_PARAMETER_BLOCK *)p;
	CShockwaveflash1 *flash_pointer = tpb->flash_pointer;
	
	int err, err2;
	CString target = CString(tpb->targetFile);
	err = fsOpenFile((CHAR *)(LPCWSTR)target, NULL, O_CREATE|O_TRUNC );
	
	BYTE buffer[1024*8];
	CFile file;
	try
	{
		CFileException ex;
		if ( !file.Open(CString(tpb->sourceFile), CFile::modeRead | CFile::shareDenyWrite, &ex) )
		{
			TCHAR szError[1024];
			ex.GetErrorMessage(szError, 1024);
			return;
		}

		DWORD totalBytes = (DWORD)file.GetLength();
		int completePercentage = 0;
		DWORD dwBytesRemaining = (DWORD)file.GetLength();

		INT nWriteCnt;
		UINT nBytesRead;
		while ( dwBytesRemaining )
		{
			nBytesRead = file.Read( buffer, sizeof(buffer) );
			err2 = fsWriteFile( err, buffer, nBytesRead, &nWriteCnt );
			if( err2 != FS_OK )
			{
				file.Close();
				err2 = fsCloseFile( err );
				return;
			}
			dwBytesRemaining -= nBytesRead;

			// transfer percentage
			completePercentage = (totalBytes-dwBytesRemaining)*100/totalBytes;
			CString str;
			str.Format(L"%d", completePercentage);
			
			flash_pointer->CallFunction(_T("<invoke name='FL_setTransferPercentage'><arguments><string>") + str + _T("</string></arguments></invoke>"));
			
			/*CString out;
			out.Format(L"total=%d, remain=%d, complete=%d%%\n", totalBytes, dwBytesRemaining, completePercentage);
			::OutputDebugString(out);*/
		}
		file.Close();
	}
	catch ( CFileException* pEx )
	{
		err2 = fsCloseFile( err );
		pEx->Delete();
		return;
	}

	fsCloseFile( err );

	TRACE(_T("COMPLETE TRANSFER\n"));
	flash_pointer->CallFunction(_T("<invoke name='FL_completeTransfer'><arguments><string></string></arguments></invoke>"));

	free(p);
	_endthread();
}

BOOL CopyDirectory(CString srcName, CString destName, CShockwaveflash1 *flash_pointer, CString command)
{
	WIN32_FIND_DATA info;
	HANDLE hwnd;

	CString tempName;
	CString tempName1;
	CString srcTempName;
	CString destTempName;
	tempName1 = srcName + _T("\\") + _T("*.*");

	int err2;
	int err = fsMakeDirectory( (CHAR *)(LPCWSTR)destName, NULL );
	if (err != FS_OK)
	{
		if (err != ERR_DIR_BUILD_EXIST)
		{
			MessageBox(api_pointer->ownerWindow->m_hWnd, _T("在设备上创建\"") + destName + _T("\"文件夹失败，可能已经存在或设备磁盘空间已满"), _T("错误"), MB_OK|MB_ICONSTOP );
			return FALSE;
		}
	}

	hwnd = FindFirstFile( tempName1, &info );
	do
	{
		if (CString( info.cFileName ) == _T(".") ||  CString( info.cFileName ) == _T(".."))
			continue;
		srcTempName = srcName + _T("\\") + info.cFileName;
		if ( info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
		{
			destTempName = destName + _T("\\") + info.cFileName;
			if (CopyDirectory(srcTempName, destTempName, flash_pointer, command) == FALSE)
			{
				FindClose( hwnd );
				return FALSE;
			}
		}
		else
		{
			int tempFileSize = (int)((info.nFileSizeHigh * MAXDWORD) + info.nFileSizeLow);
			unsigned int blockSize, freeSize, diskSize;
			err2 = fsDiskFreeSpace( m_driveNANDName.GetAt( 0 ), &blockSize, &freeSize, &diskSize );
			if ( err2 == FS_OK )
			{
				double freeSize2 = freeSize;
				double tempFileSize2 = tempFileSize / 1024.0;
				double temptemp = (freeSize2 - tempFileSize2);
				if ( temptemp < 10240.0 )
				{
					FindClose( hwnd );
					MessageBox(api_pointer->ownerWindow->m_hWnd, _T("磁盘已满"), _T("错误"), MB_OK|MB_ICONSTOP);
					return FALSE;
				}
			}

			tempName = destName + _T("\\") + info.cFileName;
			CString suFileName = tempName;
			err = fsOpenFile( (CHAR *)(LPCWSTR)suFileName, NULL, O_CREATE|O_TRUNC );
			if ( err < 0 )
			{
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, _T("在设备上创建\"") + suFileName + _T("\"文件失败，可能已经存在或设备磁盘空间已满"), _T("错误"), MB_OK|MB_ICONSTOP);
				return FALSE;
			}

			BYTE buffer[4096];
			CFile file;
			try
			{
				if ( !file.Open( srcTempName, CFile::modeRead ) )
				{
					err2 = fsCloseFile( err );
					FindClose( hwnd );
					MessageBox(api_pointer->ownerWindow->m_hWnd, srcTempName, _T("打开计算机上文件失败"), MB_OK|MB_ICONSTOP );
					return FALSE;
				}

				DWORD totalBytes = (DWORD)file.GetLength();
				int completePercentage = 0;
				DWORD dwBytesRemaining = (DWORD)file.GetLength();

				// may add check device size here
				INT nWriteCnt;
				UINT nBytesRead;
				while ( dwBytesRemaining )
				{
					nBytesRead = file.Read( buffer, sizeof(buffer) );
					err2 = fsWriteFile( err, buffer, nBytesRead, &nWriteCnt );
					if( err2 != FS_OK )
					{
						file.Close();
						FindClose( hwnd );
						err2 = fsCloseFile( err );
						MessageBox(api_pointer->ownerWindow->m_hWnd, _T("在设备上写\"") + suFileName + _T("\"文件失败，可能设备磁盘空间已满"), _T("错误"), MB_OK|MB_ICONSTOP );
						return FALSE;
					}
					dwBytesRemaining -= nBytesRead;

					// transfer percentage
					completePercentage = (totalBytes-dwBytesRemaining)*100/totalBytes;
					CString str;
					str.Format(L" %d", completePercentage);

					CString arg = _T("<invoke name='") + command + _T("'><arguments><string>");
					arg += info.cFileName;
					arg += str;
					arg += _T("</string></arguments></invoke>");
					flash_pointer->CallFunction(arg);
				}
				file.Close();
			}
			catch ( CFileException* pEx )
			{
				err2 = fsCloseFile( err );
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, pEx->m_strFileName, _T("Get length, read, or close PC file failed"), MB_OK|MB_ICONSTOP );
				pEx->Delete();
				return FALSE;
			}

			err2 = fsCloseFile( err );
			if ( err2 != FS_OK )
			{
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, _T("Close \"") + suFileName + _T("\" failed, may disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
				return FALSE;
			}
		}
	}
	while ( FindNextFile( hwnd, &info ) );

	FindClose( hwnd );
	return TRUE;
}

void __cdecl threadCopyPcDirectoryToDevice(void * p)
{
	GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)p;
	CShockwaveflash1 *flash_pointer = tpb->flash_pointer;
	CString srcName(tpb->parameter1);
	CString destName(tpb->parameter2);

	if (CopyDirectory(srcName, destName, flash_pointer, _T("FL_installSetTransferInformation")))
	{
		flash_pointer->CallFunction(_T("<invoke name='FL_installCompleteTransfer'><arguments><string></string></arguments></invoke>"));
	}

	free(p);
	_endthread();
}

void __cdecl threadInstallBuiltIn(void * p)
{
	GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)p;
	CShockwaveflash1 *flash_pointer = tpb->flash_pointer;
	CString srcParent(tpb->parameter1);
	CString destParent(tpb->parameter2);
	
	if (CopyDirectory(srcParent+_T("builtIn"), destParent+_T("builtIn2"), flash_pointer, _T("FL_updateSetTransferInformation")))
	{
		api_pointer->DeletePcDirectory(srcParent+_T("builtIn"));
		::RemoveDirectory(srcParent+_T("builtIn"));
		
		// delete obsolete builtIn on device
		api_pointer->DeleteDirectoryOnDevice(destParent+_T("builtIn"));

		// rename builtIn2 to builtIn
		if (fsRenameFile((CHAR *)(LPCWSTR)(destParent+_T("builtIn2")), NULL, (CHAR *)(LPCWSTR)(destParent+_T("builtIn")), NULL, TRUE) == FS_OK)
		{
		}
	}

	if (CopyDirectory(srcParent+_T("base"), destParent+_T("base2"), flash_pointer, _T("FL_updateSetTransferInformation")))
	{
		api_pointer->DeletePcDirectory(srcParent+_T("base"));
		::RemoveDirectory(srcParent+_T("base"));
		
		// delete obsolete base on device
		api_pointer->DeleteDirectoryOnDevice(destParent+_T("base"));

		// rename base2 to base
		flash_pointer->CallFunction(_T("<invoke name='FL_applyUpdate'><arguments><string></string></arguments></invoke>"));
		if (fsRenameFile((CHAR *)(LPCWSTR)(destParent+_T("base2")), NULL, (CHAR *)(LPCWSTR)(destParent+_T("base")), NULL, TRUE) == FS_OK)
			flash_pointer->CallFunction(_T("<invoke name='FL_updateCompleteTransfer'><arguments><string></string></arguments></invoke>"));
	}

	free(p);
	_endthread();
}