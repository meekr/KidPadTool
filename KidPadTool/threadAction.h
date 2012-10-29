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

	BYTE buffer[api_pointer->file_buff_size] = { 0 };
	TCHAR buff01[20] = {0};
	CFile file;
	CString target = _T(""), tmpfile = _T("");
	int hdl, status;
	DWORD totalBytes =  0 ;
	int completePercentage = 0, pre_completePercentage = 0;
	DWORD dwBytesRemaining = 0;
	INT nWriteCnt = 0;
	UINT nBytesRead = 0;
	CString str = _T("");
	float fv = 0.00 ;

	if(api_pointer->IsConnected(false) == false)
	{
		MessageBox(api_pointer->ownerWindow->m_hWnd, _T("设备处于断开状态， 文件传输失败。\r\n请先连接上设备， 然后再重试！"), _T("错误"), MB_OK|MB_ICONSTOP );
		goto lb_exit ;
	}


	target = CString(tpb->targetFile);
	tmpfile = target ;
	tmpfile.Append(_T(".tmp"));
	hdl = fsOpenFile((CHAR *)(LPCWSTR)tmpfile, NULL, O_CREATE|O_TRUNC );
	if(hdl < 0)
	{
		::OutputDebugString(_T("打开设备上的文件["));
		::OutputDebugString((LPCWSTR)target);
		::OutputDebugString(_T("]失败！\r\n"));
		goto lb_exit ; 
	}
	::OutputDebugString(_T("打开设备上的文件["));
	::OutputDebugString((LPCWSTR)target);
	::OutputDebugString(_T("]成功！\r\n"));

	try
	{
		CFileException ex;
		if (file.Open(CString(tpb->sourceFile), CFile::modeRead | CFile::shareDenyWrite, &ex) == FALSE )
		{
			TCHAR szError[1024];
			ex.GetErrorMessage(szError, 1024);
			::OutputDebugString(szError);
			::OutputDebugString(_T("\r\n"));
			goto lb_exit ;
		}

		totalBytes = (DWORD)file.GetLength();
		completePercentage = 0;
		dwBytesRemaining = totalBytes;

		while ( dwBytesRemaining > 0 )
		{
			memset(buffer, 0, api_pointer->file_buff_size);
			nBytesRead = file.Read(buffer, api_pointer->file_buff_size);
			status = fsWriteFile(hdl, buffer, nBytesRead, &nWriteCnt );
			if( status != FS_OK )
			{
				file.Close();
				status = fsCloseFile( hdl );
				hdl = 0 ;
				goto lb_exit ;
			}
			dwBytesRemaining -= nBytesRead;
			if(nWriteCnt != (INT)nBytesRead)
			{
				file.Close();
				status = fsCloseFile( hdl );
				hdl = 0 ;
				goto lb_exit ;
			}

			// transfer percentage
			fv = (float)dwBytesRemaining  / (float)totalBytes ;
			fv *= 100 ;
			completePercentage = 100 - (int)fv;
			if( completePercentage > pre_completePercentage)
			{
				memset(buff01, 0, 20);
				::_itot(completePercentage, buff01, 10);
				str = _T("<invoke name='FL_setTransferPercentage'><arguments><string>");
				str.Append(buff01);
				str.Append(_T("</string></arguments></invoke>"));
				flash_pointer->CallFunction((LPCTSTR)str);
				pre_completePercentage = completePercentage;
#ifdef _DEBUG
				if((completePercentage % 10) == 0)
				{
					CString out;
					out.Format(L"total=%d, complete=%d, remain=%d, complete%%=%d%%\n", totalBytes, (totalBytes-dwBytesRemaining), dwBytesRemaining, completePercentage);
					::OutputDebugString(out);
				}
#endif
			}
		}
		file.Close();
	}
	catch ( CFileException* pEx )
	{
		status = fsCloseFile( hdl );
		hdl = 0 ;
		pEx->Delete();
		fsDeleteFile((CHAR*)(LPCWSTR)tmpfile, NULL);
		goto lb_exit ;
	}
	if(hdl > 0)	{ fsCloseFile( hdl ); }

	if(api_pointer->WriteStream2DeviceFile(target, 0, 0, true) > 0)
	{
		TRACE(_T("COMPLETE TRANSFER\n"));
		flash_pointer->CallFunction(_T("<invoke name='FL_completeTransfer'><arguments><string></string></arguments></invoke>"));

	}


lb_exit:

	if(hdl > 0)	{ fsCloseFile( hdl ); }

	free(p);
	_endthread();
}

BOOL CopyDirectory(CString srcName, CString destName, CShockwaveflash1 *flash_pointer, CString command, CString failCommand)
{
	WIN32_FIND_DATA info;
	HANDLE hwnd;
	CString ret ;
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
			ret = flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
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
			if (CopyDirectory(srcTempName, destTempName, flash_pointer, command, failCommand) == FALSE)
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
					ret = flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
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
				ret = flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
				return FALSE;
			}

			BYTE buffer[api_pointer->file_buff_size] = {0};
			CFile file;
			try
			{
				if ( !file.Open( srcTempName, CFile::modeRead ) )
				{
					err2 = fsCloseFile( err );
					FindClose( hwnd );
					MessageBox(api_pointer->ownerWindow->m_hWnd, srcTempName, _T("打开计算机上文件失败"), MB_OK|MB_ICONSTOP );
					ret = flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
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
					nBytesRead = file.Read( buffer, api_pointer->file_buff_size );
					err2 = fsWriteFile( err, buffer, nBytesRead, &nWriteCnt );
					if( err2 != FS_OK )
					{
						file.Close();
						FindClose( hwnd );
						err2 = fsCloseFile( err );
						MessageBox(api_pointer->ownerWindow->m_hWnd, _T("在设备上写\"") + suFileName + _T("\"文件失败，可能设备磁盘空间已满"), _T("错误"), MB_OK|MB_ICONSTOP );
						CString ret = flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
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
					ret = flash_pointer->CallFunction(arg);
				}
				file.Close();
			}
			catch ( CFileException* pEx )
			{
				err2 = fsCloseFile( err );
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, pEx->m_strFileName, _T("Get length, read, or close PC file failed"), MB_OK|MB_ICONSTOP );
				pEx->Delete();
				ret = flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
				return FALSE;
			}

			err2 = fsCloseFile( err );
			if ( err2 != FS_OK )
			{
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, _T("Close \"") + suFileName + _T("\" failed, may disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
				ret = flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
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

	if(api_pointer->IsConnected(false) == false)
	{
		MessageBox(api_pointer->ownerWindow->m_hWnd, _T("设备处于断开状态， 文件传输失败。\r\n请先连接上设备， 然后再重试！"), _T("错误"), MB_OK|MB_ICONSTOP );
	}
	else
	{
		if (CopyDirectory(srcName, destName, flash_pointer, _T("FL_installSetTransferInformation"), _T("FL_failInstall")) == TRUE)
		{
			CString ret = flash_pointer->CallFunction(_T("<invoke name='FL_installCompleteTransfer'><arguments><string></string></arguments></invoke>"));
		}
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

	if(api_pointer->IsConnected(false) == false)
	{
		MessageBox(api_pointer->ownerWindow->m_hWnd, _T("设备处于断开状态， 文件传输失败。\r\n请先连接上设备， 然后再重试！"), _T("错误"), MB_OK|MB_ICONSTOP );
		goto lb_exit ;
	}

	if (CopyDirectory(srcParent+_T("builtIn"), destParent+_T("builtIn2"), flash_pointer, _T("FL_updateSetTransferInformation"), _T("FL_failUpdate")))
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

	if (CopyDirectory(srcParent+_T("base"), destParent+_T("base2"), flash_pointer, _T("FL_updateSetTransferInformation"), _T("FL_failUpdate")))
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

lb_exit:

	free(p);
	_endthread();
}