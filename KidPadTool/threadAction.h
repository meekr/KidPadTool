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
	GENERAL_PARAMETER_BLOCK * tpb = 0 ;
	CShockwaveflash1 *flash_pointer = 0 ;
	CString downloadDirectory = _T("");
	CFileFind finder;
	TCHAR fileSizeBuf[24] = {0};
	BOOL found = FALSE ;
	ULONGLONG l = 0 ;

	if(p == 0)	{ goto lb_exit ; }

	tpb = (GENERAL_PARAMETER_BLOCK *)p;
	flash_pointer = tpb->flash_pointer;
	if(flash_pointer == 0)	{ goto lb_exit ; }

	downloadDirectory = CString(tpb->parameter1);

	found = finder.FindFile(downloadDirectory + _T("*.npk"));

	while(found)
	{
		found = finder.FindNextFile();

		if (finder.IsDots() == TRUE)
		{
			continue;
		}
		
		memset(fileSizeBuf, 0, 24 * sizeof(TCHAR));
		l = finder.GetLength();
		::_i64tot(l, fileSizeBuf, 10);

		CString ret_value = _T("<invoke name=\"FL_addDownloadedApp\"><arguments><string>");
		ret_value += finder.GetFilePath();
		ret_value.Append(_T(","));
		ret_value.Append(fileSizeBuf);
		ret_value.Append(_T("</string></arguments></invoke>"));
		flash_pointer->CallFunction(ret_value);
		//flash_pointer->CallFunction(_T("<invoke name=\"FL_addDownloadedApp\"><arguments><string>") + ret_value + _T("</string></arguments></invoke>"));


		/*_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), finder.GetLength());
		CString ret_value;
		ret_value += finder.GetFilePath();
		ret_value += ",";
		ret_value += fileSizeBuf;
		flash_pointer->CallFunction(_T("<invoke name=\"FL_addDownloadedApp\"><arguments><string>") + ret_value + _T("</string></arguments></invoke>"));*/
	}
	finder.Close();

lb_exit:

	if(p != 0)	{ free(p); }
	_endthread();
}

void __cdecl threadGetLocalMedia(void * p)
{
	GENERAL_PARAMETER_BLOCK *tpb = 0;
	CShockwaveflash1 *flash_pointer =  0 ;
	CString directory = _T("");
	CString extension = _T("");
	CString mediatype = _T("");

	CFileFind finder;
	TCHAR fileSizeBuf[24] = {0};
	BOOL found = FALSE ;
	ULONGLONG l = 0 ;

	if(p == 0)	{ goto lb_exit ; }

	tpb = (GENERAL_PARAMETER_BLOCK *)p;
	flash_pointer = tpb->flash_pointer;
	if(flash_pointer == 0)	{ goto lb_exit ; }

	directory = CString(tpb->parameter1);
	extension = CString(tpb->parameter2);
	mediatype = CString(tpb->parameter3);

	found = finder.FindFile(directory + _T("\\") + _T("*.*"));
	while(found)
	{
		found = finder.FindNextFile();

		if (finder.IsDots() == TRUE)
		{
			continue;
		}

		CString filename = finder.GetFileName();
		CString ext = filename.Right(filename.GetLength() - filename.ReverseFind(_T('.')) - 1);
		if (extension.Find(ext.MakeLower()) == (-1))
		{
			continue;
		}
		
		memset(fileSizeBuf, 0, 24 * sizeof(TCHAR));
		l = finder.GetLength();
		::_i64tot(l, fileSizeBuf, 10);

		CString ret_value = _T("<invoke name=\"FL_addLocalMedia\"><arguments><string>");
		ret_value += mediatype ;
		ret_value.Append(_T(","));
		ret_value.Append(fileSizeBuf);
		ret_value.Append(_T(","));
		ret_value += finder.GetFilePath();
		ret_value.Append(_T("</string></arguments></invoke>"));
		flash_pointer->CallFunction(ret_value);


		/*_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), finder.GetLength());
		CString ret_value;
		ret_value += mediatype;
		ret_value += _T(",");
		ret_value += fileSizeBuf;
		ret_value += _T(",");
		ret_value += finder.GetFilePath();
		flash_pointer->CallFunction(_T("<invoke name='FL_addLocalMedia'><arguments><string>") + ret_value + _T("</string></arguments></invoke>"));*/
	}
	finder.Close();

lb_exit:

	if(p != 0)	{ free(p); }
	_endthread();
}

void __cdecl threadSendMessage2Flash(void * p)
{
	BIG_UI_DATA_PARAMETER_BLOCK *tpb = 0 ;
	CShockwaveflash1 *flash_pointer = 0 ;
	CString method = _T("");
	CString argument = _T(""); 

	if(p == 0)	{ goto lb_exit ; }

	tpb = (BIG_UI_DATA_PARAMETER_BLOCK *)p;
	flash_pointer = tpb->flash_pointer;
	if(flash_pointer == 0)	{ goto lb_exit ; }

	method = CString(tpb->method);
	argument = CString(tpb->argument);

	flash_pointer->CallFunction(_T("<invoke name='") + method + _T("'><arguments><string>") + argument + _T("</string></arguments></invoke>"));

lb_exit:

	if(p != 0)	{ free(p); }
	_endthread();
}

void __cdecl threadUpdatePercentage(void * p)
{
	TRANSFER_PARAMETER_BLOCK *tpb = 0 ;
	CShockwaveflash1 *flash_pointer = 0 ;

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

	if(p == 0)	{ goto lb_exit ; }

	tpb = (TRANSFER_PARAMETER_BLOCK *)p;
	flash_pointer = tpb->flash_pointer;
	if(flash_pointer == 0)	{ goto lb_exit ; }
	

	if(api_pointer->IsConnected(false) == false)
	{
		MessageBox(api_pointer->ownerWindow->m_hWnd, _T("�豸���ڶϿ�״̬�� �ļ�����ʧ�ܡ�\r\n�����������豸�� Ȼ�������ԣ�"), _T("����"), MB_OK|MB_ICONSTOP );
		goto lb_exit ;
	}


	target = CString(tpb->targetFile);
	tmpfile = target ;
	tmpfile.Append(_T(".tmp"));
	hdl = fsOpenFile((CHAR *)(LPCWSTR)tmpfile, NULL, O_CREATE|O_TRUNC );
	if(hdl < 0)
	{
		::OutputDebugString(_T("���豸�ϵ��ļ�["));
		::OutputDebugString((LPCWSTR)target);
		::OutputDebugString(_T("]ʧ�ܣ�\r\n"));
		goto lb_exit ; 
	}
	::OutputDebugString(_T("���豸�ϵ��ļ�["));
	::OutputDebugString((LPCWSTR)target);
	::OutputDebugString(_T("]�ɹ���\r\n"));

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
				//::PostMessage(api_pointer->ownerWindow->GetSafeHwnd(), WM_USER+123, 0,0);
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
		Sleep(1000);
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

	if(p != 0)	{ free(p); }
	_endthread();
}

BOOL CopyDirectory(CString srcName, CString destName, CShockwaveflash1 *flash_pointer, CString command, CString failCommand)
{
	WIN32_FIND_DATA info = {0};
	HANDLE hwnd = 0;
	CString ret = _T("");
	CString tempName= _T("");
	CString tempName1= _T("");
	CString srcTempName= _T("");
	CString destTempName= _T("");
	tempName1 = srcName + _T("\\") + _T("*.*");

	int err2;
	int err = fsMakeDirectory( (CHAR *)(LPCWSTR)destName, NULL );
	if (err != FS_OK)
	{
		if (err != ERR_DIR_BUILD_EXIST)
		{
			MessageBox(api_pointer->ownerWindow->m_hWnd, _T("���豸�ϴ���\"") + destName + _T("\"�ļ���ʧ�ܣ������Ѿ����ڻ��豸���̿ռ�����"), _T("����"), MB_OK|MB_ICONSTOP );
			flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
			return FALSE;
		}
	}

	hwnd = FindFirstFile( tempName1, &info );
	do
	{
		if (CString( info.cFileName ) == _T(".") ||  CString( info.cFileName ) == _T(".."))
		{
			continue;
		}

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
					MessageBox(api_pointer->ownerWindow->m_hWnd, _T("��������"), _T("����"), MB_OK|MB_ICONSTOP);
					flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
					return FALSE;
				}
			}

			tempName = destName + _T("\\") + info.cFileName;
			CString suFileName = tempName;
			err = fsOpenFile( (CHAR *)(LPCWSTR)suFileName, NULL, O_CREATE|O_TRUNC );
			if ( err < 0 )
			{
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, _T("���豸�ϴ���\"") + suFileName + _T("\"�ļ�ʧ�ܣ������Ѿ����ڻ��豸���̿ռ�����"), _T("����"), MB_OK|MB_ICONSTOP);
				flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
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
					MessageBox(api_pointer->ownerWindow->m_hWnd, srcTempName, _T("�򿪼�������ļ�ʧ��"), MB_OK|MB_ICONSTOP );
					flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
					return FALSE;
				}

				DWORD totalBytes = (DWORD)file.GetLength();
				int completePercentage = 0, pre_completePercentage = 0;
				DWORD dwBytesRemaining = totalBytes;
				float fv = 0.00 ;
				TCHAR buff01[20] = {0};
				CString str = _T("");

				// may add check device size here
				INT nWriteCnt = 0;
				UINT nBytesRead = 0;
				while ( dwBytesRemaining )
				{
					nBytesRead = file.Read( buffer, api_pointer->file_buff_size );
					err2 = fsWriteFile( err, buffer, nBytesRead, &nWriteCnt );
					if( err2 != FS_OK )
					{
						file.Close();
						FindClose( hwnd );
						err2 = fsCloseFile( err );
						MessageBox(api_pointer->ownerWindow->m_hWnd, _T("���豸��д\"") + suFileName + _T("\"�ļ�ʧ�ܣ������豸���̿ռ�����"), _T("����"), MB_OK|MB_ICONSTOP );
						flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
						return FALSE;
					}
					dwBytesRemaining -= nBytesRead;

					fv = (float)dwBytesRemaining  / (float)totalBytes ;
					fv *= 100 ;
					completePercentage = 100 - (int)fv;
					if( completePercentage > pre_completePercentage)
					{
						memset(buff01, 0, 20);
						::_itot(completePercentage, buff01, 10);
						str = _T("<invoke name=\"");
						str += command;
						str.Append(_T("\"><arguments><string>"));
						str.Append(info.cFileName);
						str.Append(_T(" "));
						str.Append(buff01);
						//::OutputDebugStringW(buff01);
						//::OutputDebugStringW(_T("\r\n"));
						str.Append(_T("</string></arguments></invoke>"));
						flash_pointer->CallFunction((LPCTSTR)str);
						pre_completePercentage = completePercentage;
					}

					// transfer percentage
					/*completePercentage = (totalBytes-dwBytesRemaining)*100/totalBytes;
					CString str;
					str.Format(L" %d", completePercentage);

					CString arg = _T("<invoke name='") + command + _T("'><arguments><string>");
					arg += info.cFileName;
					arg += str;
					arg += _T("</string></arguments></invoke>");
					ret = flash_pointer->CallFunction(arg);
					*/
				}
				file.Close();
				Sleep(1000);
			}
			catch ( CFileException* pEx )
			{
				err2 = fsCloseFile( err );
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, pEx->m_strFileName, _T("Get length, read, or close PC file failed"), MB_OK|MB_ICONSTOP );
				pEx->Delete();
				flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
				return FALSE;
			}

			err2 = fsCloseFile( err );
			if ( err2 != FS_OK )
			{
				FindClose( hwnd );
				MessageBox(api_pointer->ownerWindow->m_hWnd, _T("Close \"") + suFileName + _T("\" failed, may disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
				flash_pointer->CallFunction(_T("<invoke name='") + failCommand + _T("'><arguments><string></string></arguments></invoke>"));
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
	GENERAL_PARAMETER_BLOCK *tpb =  0 ;
	CShockwaveflash1 *flash_pointer = 0 ;
	CString srcName = _T("");
	CString destName = _T("");

	if(p == 0)	{ goto lb_exit ; }

	tpb = (GENERAL_PARAMETER_BLOCK *)p;
	flash_pointer = tpb->flash_pointer;
	if(flash_pointer == 0)	{ goto lb_exit ; }

	srcName = CString(tpb->parameter1);
	destName = CString(tpb->parameter2);

	if(api_pointer->IsConnected(false) == false)
	{
		MessageBox(api_pointer->ownerWindow->m_hWnd, _T("�豸���ڶϿ�״̬�� �ļ�����ʧ�ܡ�\r\n�����������豸�� Ȼ�������ԣ�"), _T("����"), MB_OK|MB_ICONSTOP );
	}
	else
	{
		if (CopyDirectory(srcName, destName, flash_pointer, _T("FL_installSetTransferInformation"), _T("FL_failInstall")) == TRUE)
		{
			flash_pointer->CallFunction(_T("<invoke name='FL_installCompleteTransfer'><arguments><string></string></arguments></invoke>"));
		}
	}

lb_exit:

	if(p != 0)	{ free(p); }
	_endthread();
}

void __cdecl threadInstallBuiltIn(void * p)
{
	GENERAL_PARAMETER_BLOCK *tpb =  0 ;
	CShockwaveflash1 *flash_pointer = 0 ;
	CString srcParent = _T("");
	CString destParent = _T("");

	if(p == 0)	{ goto lb_exit ; }

	tpb = (GENERAL_PARAMETER_BLOCK *)p;
	flash_pointer = tpb->flash_pointer;
	if(flash_pointer == 0)	{ goto lb_exit ; }

	srcParent = CString(tpb->parameter1);
	destParent = CString(tpb->parameter2);

	if(api_pointer->IsConnected(false) == false)
	{
		MessageBox(api_pointer->ownerWindow->m_hWnd, _T("�豸���ڶϿ�״̬�� �ļ�����ʧ�ܡ�\r\n�����������豸�� Ȼ�������ԣ�"), _T("����"), MB_OK|MB_ICONSTOP );
		goto lb_exit ;
	}

	if (CopyDirectory(srcParent+_T("builtIn"), destParent+_T("builtIn2"), flash_pointer, _T("FL_updateSetTransferInformation"), _T("FL_failUpdate")) == TRUE)
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

	if (CopyDirectory(srcParent+_T("base"), destParent+_T("base2"), flash_pointer, _T("FL_updateSetTransferInformation"), _T("FL_failUpdate")) == TRUE)
	{
		api_pointer->DeletePcDirectory(srcParent+_T("base"));
		::RemoveDirectory(srcParent+_T("base"));

		// delete obsolete base on device
		api_pointer->DeleteDirectoryOnDevice(destParent+_T("base"));

		// rename base2 to base
		flash_pointer->CallFunction(_T("<invoke name='FL_applyUpdate'><arguments><string></string></arguments></invoke>"));
		if (fsRenameFile((CHAR *)(LPCWSTR)(destParent+_T("base2")), NULL, (CHAR *)(LPCWSTR)(destParent+_T("base")), NULL, TRUE) == FS_OK)
		{
			flash_pointer->CallFunction(_T("<invoke name='FL_updateCompleteTransfer'><arguments><string></string></arguments></invoke>"));
		}
	}

lb_exit:

	if(p != 0)	{ free(p); }
	_endthread();
}