
typedef struct _tagTRANSFER_PARAMETER_BLOCK
{
    TCHAR sourceFile[1024];
	TCHAR targetFile[1024];
	CShockwaveflash1 *flash_pointer;
} TRANSFER_PARAMETER_BLOCK;

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
		if (extension.Find(ext) == -1)
			continue;

		_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), finder.GetLength());
		CString ret_value;
		ret_value += mediatype;
		ret_value += _T(",");
		ret_value += fileSizeBuf;
		ret_value += _T(",");
		ret_value += finder.GetFilePath();
		TRACE(ret_value);
		TRACE(_T("\n"));
		flash_pointer->CallFunction(_T("<invoke name='FL_addLocalMedia'><arguments><string>") + ret_value + _T("</string></arguments></invoke>"));
	}
	finder.Close();

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
		if ( !file.Open(CString(tpb->sourceFile), CFile::modeRead ) )
		{
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