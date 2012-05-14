#include "stdafx.h"
#include "ApiController.h"
#include "KidPadTool.h"
#include "base64.h"
#include "Base64Util.h"

#ifdef	_DEMO_USE_STDUSBDISKDRIVER_
#include "StdUsbDiskDriver.h"
#else
#include "NvtOffsetUsbDiskDriver.h"
#endif

#include "FileSystem.h"
#include "tinyxml.h"
#include <afx.h>


void ApiController::Initialize()
{
	m_pu8xmlBuffer = NULL;
	
	ScanUsbDisk();

	TCHAR szFolder[MAX_PATH * 2];
	HMODULE hModule = GetModuleHandle( 0 );
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	m_workingFolderName = szFolder;
	m_workingFolderName = m_workingFolderName.Left(m_workingFolderName.ReverseFind(_T('\\')));
	
	// load sync settings
	/*CString settingsFile = m_workingFolderName + _T("\\SyncSetting2.xml");
	flashUI->CallFunction(_T("<invoke name='FL_loadSyncSetting'><arguments><string>") + settingsFile + _T("</string></arguments></invoke>"));
	flashUI->CallFunction(_T("<invoke name='FL_initUI'><arguments><string></string></arguments></invoke>"));*/
}

/* 10 digits + 1 sign + 1 trailing nul */
static char buf[12];
char *itoa2(int i)
{
        char *pos = buf + sizeof(buf) - 1;
        unsigned int u;
        int negative = 0;

        if (i < 0) {
                negative = 1;
                u = ((unsigned int)(-(1+i))) + 1;
        } else {
                u = i;
        }

        *pos = 0;

        do {
                *--pos = '0' + (u % 10);
                u /= 10;
        } while (u);

        if (negative) {
                *--pos = '-';
        }

        return pos;
}

void ApiController::UpdateList()
{
	// List scanned logical disks which have been parsed by filesystem.
	PDISK_T *pDiskList = fsGetFullDiskInfomation ();
	PDISK_T *nextPDisk = pDiskList;
	while (nextPDisk) {
		UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (nextPDisk->ptDriver);
		PARTITION_T *partition = nextPDisk->ptPartList;
		while (partition) {
			LDISK_T *lDisk = partition->ptLDisk;
			if (lDisk) {
				int driveNo = lDisk->nDriveNo;
							
				TCHAR driveName[] = {driveNo, _T(':'), 0};

				CString strName;
				usbDiskDriver->getProductId(m_programUsbDiskName);
				strName = _T("nand1-2");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveTempName = driveName;
					m_driveNANDName = driveName;
				}
				strName = _T("USB Card Reader");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveTempName = driveName;
					m_driveNANDName = driveName;
				}
				strName = _T("MSC NAND");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveTempName = driveName;
					m_driveNANDName = driveName;
				}
				strName = _T("sd");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveTempName = driveName;
					m_driveSDName = driveName;
				}

				// talentech added
				strName = _T("Null");
				if (wcsstr( m_programUsbDiskName, strName ) != 0){
					flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
				}
				else{
					UINT32 blockSize, freeSize, diskSize;
					if (fsDiskFreeSpace(driveNo, &blockSize, &freeSize, &diskSize) == FS_OK){
						CString free(itoa2((INT)freeSize/1024));
						CString total(itoa2((INT)diskSize/1024));
						flashUI->CallFunction(_T("<invoke name='FL_setDiskVolumnStatus'><arguments><string>") + free + _T(",") + total + _T("</string></arguments></invoke>"));
					}
					flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>1</string></arguments></invoke>"));
				}
			}

			partition = partition->ptNextPart;
		}

		nextPDisk = nextPDisk->ptPDiskAllLink;
	}

	fsReleaseDiskInformation (pDiskList);
	pDiskList = NULL;
}


void ApiController::ScanUsbDisk()
{
	std::map <std::basic_string<TCHAR>, PDISK_T *> &usbDisks = (((CKidPadToolApp *)AfxGetApp ())->m_usbDisks);
	// Find out USB disks that have been attached.
	UINT32 usbDiskMask = UsbDiskDriver::scan ();

	// For each scanned USB disk, add it to list if it meets some rule.
	for (int i = 0; i < 26; i ++) {
		if (usbDiskMask & (1 << i)) {
			TCHAR diskDriveNameBuf[] = {_T('A') + i, _T(':'), 0};
				
			std::map <std::basic_string<TCHAR>, PDISK_T *>::iterator it = usbDisks.find (diskDriveNameBuf);
			if (it == usbDisks.end ()) {

#ifdef	_DEMO_USE_STDUSBDISKDRIVER_
				UsbDiskDriver *usbDiskDriver = new StdUsbDiskDriver;
#else
				UsbDiskDriver *usbDiskDriver = new NvtOffsetUsbDiskDriver;
#endif

				std::auto_ptr<UsbDiskDriver> scopedUsbDiskDriver (usbDiskDriver);
					
				usbDiskDriver->onDriveAttach (diskDriveNameBuf);
				if (! usbDiskDriver->onLineCheck ()) {
					continue;
				}
				
				// Check if it is a multi-partition case. 
				// If yes, don't add it to list to avoid duplicate list of the same physical disk.
				BOOL diskScanned = FALSE;
				for (it = usbDisks.begin (); it!= usbDisks.end (); it ++) {
					UsbDiskDriver *installedUsbDiskDriver = (UsbDiskDriver *) it->second->ptDriver;
							
					CString newDiskId, existDiskId;
					usbDiskDriver->getDiskId (newDiskId);
					installedUsbDiskDriver->getDiskId (existDiskId);
					if (newDiskId == existDiskId) {
						usbDiskDriver = NULL;
						diskScanned = TRUE;
						break;
					}
				}

				if (diskScanned) {
					continue;
				}

				// A defined USB disk has been found. Call filesystem to parse it.
				PDISK_T *usbDisk = new PDISK_T;
				std::auto_ptr<PDISK_T> scopedUsbDisk (usbDisk);

				DWORD usbDiskSize = usbDiskDriver->getDiskSize ();

				memset (usbDisk, 0x00, sizeof (PDISK_T));
				usbDisk->nDiskType = DISK_TYPE_SMART_MEDIA;
				usbDisk->uTotalSectorN = usbDiskSize / 512;
				usbDisk->nSectorSize = 512;
				usbDisk->uDiskSize = usbDiskSize;
				usbDisk->ptDriver = usbDiskDriver;
				usbDisk->pvPrivate = usbDiskDriver;
				if (fsPhysicalDiskConnected (usbDisk)) {
					// Error here.
					continue;
				}
				
				usbDisks[diskDriveNameBuf] = usbDisk;

				// List available logical disks according to filesystem's parsing.
				UpdateList();

				scopedUsbDisk.release();
				scopedUsbDiskDriver.release();

				// set variables on flash side
				flashUI->CallFunction(_T("<invoke name='FL_findNRDGameStoryAndAppRoot'><arguments><string>") + m_driveTempName + _T("</string></arguments></invoke>"));
			}
		}
	}
}

int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg==BFFM_INITIALIZED)
		SendMessage(hWnd, BFFM_SETSELECTION,TRUE, lpData);
	return 0;
}

CString ApiController::BrowsePC()
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

void ApiController::DispatchFlashCommand(CString command, CString args)
{
	if (command == "loaded")
	{
		Initialize();
	}
}

void ApiController::DispatchFlashCall(const char* request, const char* args)
{
	if (strcmp(request, "F2C_getDeviceFileContent") == 0) {
		CString cs("<string>");
		cs += GetDeviceFileContent(CString(args));
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDeviceIconBase64") == 0) {
		CString cs("<string>");
		cs += GetDeviceIconBase64(CString(args));
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDownloadedAppNames") == 0) {
		CString cs("<string>");
		cs += GetLocalAppNames(CString(args));
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getLocalIconBase64") == 0) {
		CString cs("<string>");
		cs += GetLocalIconBase64(CString(args));
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_deleteAppOnDevice") == 0) {
		CString cs("<string>");
		if (DeleteAppOnDevice(CString(args)))
			cs += _T('1');
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_installApp") == 0) {
		CString cs("<string>");
		if (InstallNPK(CString(args)))
			cs += CString("1");
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_saveFileFromBase64") == 0) {
		CString cs("<string>");
		CString str(args);
		CString filePath = str.Left(str.Find(_T(',')));
		CString base64Str = str.Right(str.GetLength() - str.Find(_T(',')) - 1);
		if (SaveFileFromBase64(base64Str, filePath))
			cs += CString("1");
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_cancelDownload") == 0) {
	}
}

CString ApiController::GetDeviceFileContent(CString filePath)
{
	int hdl, hdl2;
	hdl = hdl2 = 0;

	hdl = fsOpenFile((CHAR *)(LPCWSTR)filePath, NULL, O_RDONLY);
	INT nBytes = (INT)fsGetFileSize(hdl);
	if (m_pu8xmlBuffer != NULL)
	{
		delete [] m_pu8xmlBuffer;
		m_pu8xmlBuffer = NULL;
	}

	m_pu8xmlBuffer = new char[nBytes + 1];
	memset(m_pu8xmlBuffer, 0x00, nBytes+1);

	INT nReadCnt;
	
	// may need error handle here
	if ( (hdl2 = fsReadFile(hdl, (UINT8*)m_pu8xmlBuffer, nBytes, &nReadCnt)) != FS_OK)
	{
		hdl2 = fsCloseFile(hdl);
		return NULL;
	}
	fsCloseFile(hdl);
	return (CString)m_pu8xmlBuffer;
}

CString ApiController::GetDeviceIconBase64(CString iconFilePath)
{
	TRACE(iconFilePath);
	TRACE("\n");
	int hdl, hdl2;
	hdl = hdl2 = 0;

	hdl = fsOpenFile((CHAR *)(LPCWSTR)iconFilePath, NULL, O_RDONLY);
	INT nBytes = (INT)fsGetFileSize(hdl);
	if (m_pu8xmlBuffer != NULL)
	{
		delete [] m_pu8xmlBuffer;
		m_pu8xmlBuffer = NULL;
	}
	m_pu8xmlBuffer = new char[nBytes + 1];
	memset(m_pu8xmlBuffer, 0x00, nBytes+1);
	
	// may need error handle here
	INT nReadCnt;
	if ( (hdl2 = fsReadFile(hdl, (UINT8*)m_pu8xmlBuffer, nBytes, &nReadCnt)) != FS_OK)
	{
		hdl2 = fsCloseFile(hdl);
		return false;
	}
	fsCloseFile(hdl);

	std::string encoded;
	base64_encode(encoded, reinterpret_cast<const unsigned char*>(m_pu8xmlBuffer), nBytes);
	CString cs(encoded.c_str());
	return cs;
}

CString ApiController::GetLocalAppNames(CString localDirectoryPath)
{
	CFileFind finder;
	TCHAR fileSizeBuf[16];
	BOOL found = finder.FindFile(localDirectoryPath + _T("\\") + _T("*.png"));
	if (!found)
	{
		finder.Close();
		return _T("");
	}

	CString ret_value;
	while(found)
	{
		found = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), finder.GetLength());
		ret_value += finder.GetFileTitle();
		ret_value += ",";
	}
	finder.Close();

	if (ret_value.GetLength() > 0)
		ret_value = ret_value.Left(ret_value.GetLength() - 1);

	return ret_value;
}

CString ApiController::GetLocalIconBase64(CString iconFilePath)
{
	// get base64
	CFile file;
	if (!file.Open(iconFilePath, CFile::modeRead))
	{
		return NULL;
	}
	
	DWORD nBytes = (DWORD)file.GetLength();
	char *buffer = new char[nBytes + 1];
	memset(buffer, 0x00, nBytes+1);
	file.Read(buffer, nBytes);
	file.Close();

	std::string encoded;
	base64_encode(encoded, reinterpret_cast<const unsigned char*>(buffer), nBytes);
	CString cs(encoded.c_str());
	
	delete [] buffer;
	buffer = NULL;

	return cs;
}
BOOL ApiController::DeleteAppOnDevice(CString appDirectoryPaths)
{
	/*CString css("C:\\story\\tellMeYourWishes");
	DeleteDirectoryOnDevice(css);*/


	// appDirectoryPaths: appName,appDirectoryPath,appCategoryXmlFilePath
	CString appName = appDirectoryPaths.Left(appDirectoryPaths.Find(_T(',')));
	CString appDirectory = appDirectoryPaths.Left(appDirectoryPaths.ReverseFind(_T(',')));
	appDirectory = appDirectory.Right(appDirectory.GetLength() - appDirectory.Find(_T(',')) - 1);
	CString appCategoryXml = appDirectoryPaths.Right(appDirectoryPaths.GetLength() - appDirectoryPaths.ReverseFind(_T(',')) - 1);


	/*********************************
	*** delete files from device ****
	*********************************/
	if (DeleteDirectoryOnDevice(appDirectory) == false) {
		//MessageBox(_T("Delete device program failed"), _T("Uninstall Error"), MB_OK|MB_ICONSTOP);
		return false;
	}

	/*********************************
	*** update xml on device ****
	*********************************/
	DeleteAppNodeOnDeviceXml(appCategoryXml, appName);

	return true;
}

BOOL ApiController::DeleteDirectoryOnDevice(CString directory) {
	FILE_FIND_T fileFind;
	memset(&fileFind, 0x00, sizeof(FILE_FIND_T));
	CString tempName;

	// may need error handle here
	DWORD err = fsFindFirst((CHAR *)(LPCWSTR)(directory + _T('\\')), NULL, &fileFind);
	if (err != 0)
	{
		//MessageBox( _T("Cannot find the device directory \"") + directory, _T("Error"), MB_OK|MB_ICONSTOP);
		return false;
	}

	do
	{
		if (CString((LPCWSTR)fileFind.suLongName) == _T("."))
			continue;
		if (CString((LPCWSTR)fileFind.suLongName) == _T(".."))
			continue;

		tempName = directory + _T('\\') + (LPCWSTR)fileFind.suLongName;
		int l = tempName.GetLength();

		if (fileFind.ucAttrib == A_DIR)
		{
			if (DeleteDirectoryOnDevice(tempName) == false)
			{
				// may need error handle here
				err = fsFindClose(&fileFind);
				return false;
			}
		}
		else
		{
			// may need error handle here
			err = fsDeleteFile((CHAR *)(LPCWSTR)tempName, NULL);
			if (err != FS_OK)
			{
				//MessageBox( _T("Delete file \"") + tempName + _T("\" failed on device"), _T("Error"), MB_OK|MB_ICONSTOP );
				return false;
			}
		}
	}
	while (fsFindNext(&fileFind) == FS_OK);

	// may need error handle here
	err = fsFindClose(&fileFind);

	err = fsRemoveDirectory((CHAR *)(LPCWSTR)directory, NULL);
	if (err != FS_OK)
	{
		MessageBox(ownerWindow->m_hWnd, _T("Delete directory \"") + directory + _T("\" failed on device"), _T("Error"), MB_OK|MB_ICONSTOP );
		return false;
	}
	return true;
}

BOOL ApiController::DeleteAppNodeOnDeviceXml(CString appCategoryXml, CString appName)
{
	CString xml = GetDeviceFileContent(appCategoryXml);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);

	TiXmlNode *node = 0;
	TiXmlElement *listElement = 0;
	TiXmlElement *xElement = 0;
	TiXmlElement *nameElement = 0;
	
	node = doc.RootElement();
	listElement = node->ToElement();
	node = listElement->FirstChildElement();
	if (!node)
	{
		//MessageBox( _T("No installed programs"), _T("Info"), MB_OK );
		return false;
	}

	CString tempName;
	// may enhance here, looply found name and deploytype
	for (xElement = node->ToElement(); xElement; xElement = xElement->NextSiblingElement())
	{
		nameElement = xElement->FirstChildElement();
		tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
		if (tempName == _T("name"))
		{
			tempName = CA2CT(nameElement->GetText(), CP_UTF8);
			if (tempName == appName)
				break;
		}
	}
	listElement->RemoveChild(xElement);

	TiXmlPrinter printer;
    doc.Accept(&printer);
    const char* ret = printer.CStr();

	int hdl, hdl2;
	hdl = hdl2 = 0;

	hdl = fsOpenFile((CHAR *)(LPCWSTR)appCategoryXml, NULL, O_TRUNC);

	INT nWriteCnt;
	INT len = strlen(ret);
	hdl2 = fsWriteFile(hdl, (UINT8 *)ret, strlen(ret), &nWriteCnt);
	if (hdl2 != FS_OK )
	{
		hdl2 = fsCloseFile(hdl);
		MessageBox(ownerWindow->m_hWnd, _T("Write device temp XML failed, or disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
		return false;
	}
	fsCloseFile(hdl);

	CString content = GetDeviceFileContent(appCategoryXml);
	
	return true;
}

BOOL ApiController::InstallNPK(CString npkFile)
{
	CString appName = npkFile.Right(npkFile.GetLength() - npkFile.ReverseFind('\\') - 1);
	appName = appName.Left(appName.GetLength() - 4);
	CString workFolder = npkFile.Left(npkFile.ReverseFind('\\'));
	CString tempFolder = workFolder + _T("\\_temp_") + appName;
	CString decompressedXmlFile;
	CString appCategoryXml, listElementName, programElementName;
	

	/*********************************
	*** upcompress to temp folder ****
	*********************************/
	if (!::CreateDirectory(tempFolder, NULL))
	{
		MessageBox(ownerWindow->m_hWnd, _T("Uncompress folder failed, working folder may exist, or disk may full"), _T("Error"), MB_OK|MB_ICONSTOP);
		return false;
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	CString	strCmd;
	strCmd.Format(_T("\"%s\\7za.exe\" x -o\"%s\" -y \"%s\""), m_workingFolderName, tempFolder, npkFile);
	::CreateProcess( NULL, strCmd.AllocSysString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	while (1)
	{
		// Check if the shutdown event has been set, if so exit the thread
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(pi.hProcess, 0)) {
			break;
		}
	}

	while (1)
	{
		// Check if the shutdown event has been set, if so exit the thread
		if (WAIT_OBJECT_0 == WaitForSingleObject(pi.hThread, 0)) {
			break;
		}
	}

	// Close process and thread handles.
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

	CFileFind finder;
	BOOL found = finder.FindFile(tempFolder + _T("\\") + _T("*.*"));
	if (!found)
	{
		finder.Close();
		return false;
	}

	while (found)
	{
		found = finder.FindNextFile();
		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			decompressedXmlFile = tempFolder + _T("\\") + finder.GetFileName() + _T("\\install.xml");
		}
		else
		{
			MessageBox(ownerWindow->m_hWnd, _T("Wrong NPK file"), _T("Error"), MB_OK|MB_ICONSTOP);
			finder.Close();
			return false;
		}
	}
	finder.Close();

	/*********************************
	*** get deploy type **************
	*********************************/
	TiXmlDocument doc;
    doc.LoadFile(CT2CA(decompressedXmlFile), TIXML_ENCODING_UTF8);
    TiXmlHandle docHandle(&doc);
	TiXmlElement* elm = docHandle.FirstChild("install").FirstChild("program").FirstChildElement("deployType").ToElement();
	const char *deployType = elm->GetText();
	if (strcmp(deployType, "game") == 0) {
		appCategoryXml = m_driveTempName + _T("\\game\\gameList.xml");
		listElementName = _T("gameList");
		programElementName = _T("game");
	}
	else if (strcmp(deployType, "story") == 0) {
		appCategoryXml = m_driveTempName + _T("\\story\\storyList.xml");
		listElementName = _T("storyList");
		programElementName = _T("story");
	}
	else if (strcmp(deployType, "app") == 0) {
		appCategoryXml = m_driveTempName + _T("\\installed\\appList.xml");
		listElementName = _T("appList");
		programElementName = _T("app");
	}
    
	/*********************************
	*** reject if already exists *****
	*********************************/
	if (IsApplicationInstalled(appName, appCategoryXml))
	{
		MessageBox(ownerWindow->m_hWnd, _T("Application already installed"), _T("Error"), MB_OK|MB_ICONSTOP);
		DeletePcDirectory(tempFolder);
		::RemoveDirectory(tempFolder);
		return false;
	}

	
	/*********************************
	*** update install xml ***********
	*********************************/
	program_t program;
	InsertXMLBufferElement(decompressedXmlFile, appCategoryXml, listElementName, programElementName, program);

	CString srcName = decompressedXmlFile.Left(decompressedXmlFile.ReverseFind(_T('\\'))) + _T('\\') + program.name;
	CString folder(deployType);
	CString destName = m_driveTempName + _T("\\") + folder + _T("\\") + appName;
	
	if (!CopyDirectory(srcName, destName))
	{
		MessageBox(ownerWindow->m_hWnd, _T("Copy to device failed"), _T("Error"), MB_OK|MB_ICONSTOP);
		return false;
	}

	// delete files on pc
	DeletePcDirectory(tempFolder);
	::RemoveDirectory(tempFolder);

	return true;
}

BOOL ApiController::IsApplicationInstalled(CString appName, CString appCategoryXml)
{
	CString xml = GetDeviceFileContent(appCategoryXml);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);

	TiXmlNode *node = 0;
	TiXmlElement *listElement = 0;
	TiXmlElement *xElement = 0;
	TiXmlElement *nameElement = 0;
	
	node = doc.RootElement();
	listElement = node->ToElement();
	node = listElement->FirstChildElement();

	if (!node)
		return false;

	CString tempName;
	// may enhance here, looply found name and deploytype
	for (xElement = node->ToElement(); xElement; xElement = xElement->NextSiblingElement())
	{
		nameElement = xElement->FirstChildElement();
		tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
		if (tempName == _T("name"))
		{
			tempName = CA2CT(nameElement->GetText(), CP_UTF8);
			if (tempName == appName)
				return true;
		}
	}

	return false;
}

BOOL ApiController::InsertXMLBufferElement(CString xmlFile, CString appCategoryXml, CString listElementName, CString programElementName, program_t& program)
{
	TiXmlDocument doc;
	doc.LoadFile(CT2CA(xmlFile), TIXML_ENCODING_UTF8);

	// read xml and export to program struct
	TiXmlNode *node = 0;


	TiXmlElement *element = 0;
	TiXmlElement *element2 = 0;

	node = doc.RootElement();
	TiXmlElement* installElement = node->ToElement();
	node = installElement->FirstChild();
	TiXmlElement* programElement = node->ToElement();

	for (element = programElement->FirstChildElement(); element; element = element->NextSiblingElement())
	{
		if (strcmp(element->Value(), "name") == 0) {
			memcpy(program.name, element->GetText(), strlen(element->GetText())+1);
		}
		else if (strcmp(element->Value(), "icon") == 0) {
			memcpy(program.icon, element->GetText(), strlen(element->GetText())+1);
		}
		else if (strcmp(element->Value(), "type") == 0) {
			memcpy(program.type, element->GetText(), strlen(element->GetText())+1);
		}
		else if (strcmp(element->Value(), "deployType") == 0) {
			memcpy(program.deployType, element->GetText(), strlen(element->GetText())+1);
		}
		else if (strcmp(element->Value(), "entry") == 0) {
			memcpy(program.entry, element->GetText(), strlen(element->GetText())+1);
		}
		else if (strcmp(element->Value(), "security") == 0) {
			program.hasSecurity = true;
			for (element2 = element->FirstChildElement(); element2; element2 = element2->NextSiblingElement()) {
				if (strcmp(element2->Value(), "image") == 0)
				{
					memcpy(program.securityImage, element2->GetText(), strlen(element2->GetText())+1);
				}
				else if (strcmp(element2->Value(), "passwd"))
				{
					memcpy(program.securityPassword, element2->GetText(), strlen(element2->GetText())+1);
				}
			}
		}
		else if (strcmp(element->Value(), "bgColor") == 0) {
			memcpy(program.bgColor, element->GetText(), strlen(element->GetText())+1);
		}
		else if (strcmp(element->Value(), "asMain") == 0) {
			memcpy(program.asMain, element->GetText(), strlen(element->GetText())+1);
		}
	}

	CString installName(program.entry);
	if (installName.ReverseFind('.') > -1)
		installName = installName.Left(installName.ReverseFind('.'));
	installName = installName.Right(installName.GetLength() - installName.ReverseFind('/') - 1);

	CString cs;
	cs.Format(_T("%S"), program.icon);
	cs = installName + _T('/') + cs;
	CT2CA psz1 (cs);
	memcpy(program.icon, psz1, strlen(psz1)+1);
	
	cs.Format(_T("/body/%S"), program.entry);
	cs = installName + cs;
	CT2CA psz2 (cs);
	memcpy(program.entry, psz2, strlen(psz2)+1);

	// read xml and export to program struct
	TiXmlElement* story = new TiXmlElement( CT2CA(programElementName, CP_UTF8) );

	TiXmlElement* name = new TiXmlElement("name" );
	TiXmlText* nameText = new TiXmlText(program.name);
	TiXmlElement* icon = new TiXmlElement( "icon" );
	TiXmlText* iconText = new TiXmlText(program.icon);
	TiXmlElement* type = new TiXmlElement( "type" );
	TiXmlText* typeText = new TiXmlText(program.type);
	TiXmlElement* entry = new TiXmlElement( "entry" );
	TiXmlText* entryText = new TiXmlText(program.entry);
	TiXmlElement* deployType = new TiXmlElement( "deployType" );
	TiXmlText* deployTypeText = new TiXmlText(program.deployType);

	name->LinkEndChild( nameText );
	story->LinkEndChild( name );
	icon->LinkEndChild( iconText );
	story->LinkEndChild( icon );
	type->LinkEndChild( typeText );
	story->LinkEndChild( type );
	entry->LinkEndChild( entryText );
	story->LinkEndChild( entry );
	deployType->LinkEndChild( deployTypeText );
	story->LinkEndChild( deployType );

	if (program.hasSecurity)
	{
		cs.Format(_T("%S"), program.securityImage);
		cs = installName + _T('/') + cs;
		CT2CA psz3 (cs);
		memcpy(program.securityImage, psz3, strlen(psz3)+1);

		cs.Format(_T("%S"), program.securityPassword);
		cs = installName + _T('/') + cs;
		CT2CA psz4 (cs);
		memcpy(program.securityPassword, psz4, strlen(psz4)+1);

		char mountPointStr[255];
		cs = installName + _T("/body/");
		CT2CA psz5 (cs);
		memcpy(mountPointStr, psz5, strlen(psz5)+1);
		
		TiXmlElement* security = new TiXmlElement( "security" );

		TiXmlElement* image = new TiXmlElement( "image" );
		TiXmlText* imageText = new TiXmlText(program.securityImage);
		TiXmlElement* passwd = new TiXmlElement( "passwd" );
		TiXmlText* passwdText = new TiXmlText(program.securityPassword);
		TiXmlElement* mountPoint = new TiXmlElement( "mountPoint" );
		TiXmlText* mountPointText = new TiXmlText(mountPointStr);

		image->LinkEndChild( imageText );
		security->LinkEndChild( image );
		passwd->LinkEndChild( passwdText );
		security->LinkEndChild( passwd );
		mountPoint->LinkEndChild( mountPointText );
		security->LinkEndChild( mountPoint );

		story->LinkEndChild( security );
	}

	if (program.bgColor)
	{
		TiXmlElement* bgColor = new TiXmlElement( "bgColor" );
		TiXmlText* bgColorText = new TiXmlText(program.bgColor);

		bgColor->LinkEndChild( bgColorText );
		story->LinkEndChild( bgColor );
	}

	if (program.asMain)
	{
		TiXmlElement* asMain = new TiXmlElement( "asMain" );
		TiXmlText* asMainText = new TiXmlText(program.asMain);

		asMain->LinkEndChild( asMainText );
		story->LinkEndChild( asMain );
	}

	// add xml node from program struct
	GetDeviceFileContent(appCategoryXml);
	TiXmlDocument doc2;
	doc2.Parse(m_pu8xmlBuffer);
	node = doc2.RootElement();
	node->LinkEndChild( story );
	TiXmlPrinter printer;
    doc2.Accept(&printer);
    const char* ret = printer.CStr();
	

	int hdl, hdl2;
	hdl = hdl2 = 0;

	hdl = fsOpenFile((CHAR *)(LPCWSTR)appCategoryXml, NULL, O_TRUNC);

	INT nWriteCnt;
	INT len = strlen(ret);
	hdl2 = fsWriteFile(hdl, (UINT8 *)ret, strlen(ret), &nWriteCnt);
	if (hdl2 != FS_OK )
	{
		hdl2 = fsCloseFile(hdl);
		MessageBox(ownerWindow->m_hWnd, _T("Write device temp XML failed, or disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
		return false;
	}
	fsCloseFile(hdl);

	CString content = GetDeviceFileContent(appCategoryXml);


	return true;
}

void ApiController::DeletePcDirectory(CString szPath)
{
	CFileFind ff;
	CString strPath = szPath;

	if ( strPath.Right( 1 ) != _T("\\") )
		strPath += _T("\\");
	strPath += _T("*.*");
	BOOL bRet;
	if ( ff.FindFile( strPath ) )
	{
		do
		{
			bRet = ff.FindNextFile();
			if (ff.IsDots()) continue;
			strPath = ff.GetFilePath();
			if ( !ff.IsDirectory() )
			{
				::SetFileAttributes( strPath, FILE_ATTRIBUTE_NORMAL );
				::DeleteFile( strPath );
			}
			else
			{
				DeletePcDirectory(strPath);

				::SetFileAttributes( strPath, FILE_ATTRIBUTE_NORMAL );
				::RemoveDirectory( strPath );
			}
		}
		while (bRet);
	}
}

BOOL ApiController::CopyDirectory(CString srcName, CString destName)
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
	if ( err != FS_OK )
	{
		if ( err != ERR_DIR_BUILD_EXIST )
		{
			MessageBox(ownerWindow->m_hWnd, _T("Create directory \"") + destName + _T("\" failed, may existed on device or disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
			return false;
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
			if ( CopyDirectory( srcTempName, destTempName ) == FALSE )
			{
				FindClose( hwnd );
				return false;
			}
		}
		else
		{
			int tempFileSize = (int)((info.nFileSizeHigh * MAXDWORD) + info.nFileSizeLow);
			unsigned int blockSize, freeSize, diskSize;
			err2 = fsDiskFreeSpace( m_deviceDriveName.GetAt( 0 ), &blockSize, &freeSize, &diskSize );
			if ( err2 == FS_OK )
			{
				double freeSize2 = freeSize;
				double tempFileSize2 = tempFileSize / 1024.0;
				double temptemp = (freeSize2 - tempFileSize2);
				if ( temptemp < 10240.0 )
				{
					FindClose( hwnd );
					MessageBox(ownerWindow->m_hWnd, _T("Disk full"), _T("Error"), MB_OK|MB_ICONSTOP);
					return false;
				}
			}

			tempName = destName + _T("\\") + info.cFileName;
			CString suFileName = tempName;
			err = fsOpenFile( (CHAR *)(LPCWSTR)suFileName, NULL, O_CREATE|O_TRUNC );
			if ( err < 0 )
			{
				FindClose( hwnd );
				MessageBox(ownerWindow->m_hWnd, _T("Create \"") + suFileName + _T("\" failed, may existed on device or disk full"), _T("Error"), MB_OK|MB_ICONSTOP);
				return false;
			}

			BYTE buffer[4096];
			CFile file;
			try
			{
				if ( !file.Open( srcTempName, CFile::modeRead ) )
				{
					err2 = fsCloseFile( err );
					FindClose( hwnd );
					MessageBox(ownerWindow->m_hWnd, srcTempName, _T("Open PC file failed"), MB_OK|MB_ICONSTOP );
					return false;
				}

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
						MessageBox(ownerWindow->m_hWnd, _T("Write \"") + suFileName + _T("\" failed, may disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
						return false;
					}
					dwBytesRemaining -= nBytesRead;
				}
				file.Close();
			}
			catch ( CFileException* pEx )
			{
				err2 = fsCloseFile( err );
				FindClose( hwnd );
				MessageBox(ownerWindow->m_hWnd, pEx->m_strFileName, _T("Get length, read, or close PC file failed"), MB_OK|MB_ICONSTOP );
				pEx->Delete();
				return false;
			}

			err2 = fsCloseFile( err );
			if ( err2 != FS_OK )
			{
				FindClose( hwnd );
				MessageBox(ownerWindow->m_hWnd, _T("Close \"") + suFileName + _T("\" failed, may disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
				return false;
			}
		}
	}
	while ( FindNextFile( hwnd, &info ) );

	FindClose( hwnd );
	return true;
}

BOOL ApiController::SaveFileFromBase64(CString base64String, CString filePath)
{
	//base64String = GetBase64DataFromTextFile(_T("C:\\pic_2.txt"));
	BOOL rtv = FALSE ;
	unsigned char * c_result = 0 ;
	unsigned int len_result = 0 ;
	
	len_result = __Base64Decode(base64String, (void**)(&c_result));	

	HANDLE mhd = (HANDLE)::CreateFile(filePath.GetBuffer(filePath.GetLength()+1) ,
			GENERIC_WRITE,	//GENERIC_READ| GENERIC_WRITE,
			0,		//FILE_SHARE_READ | FILE_SHARE_WRITE ,
			0,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,//|FILE_FLAG_OVERLAPPED,
			0
			);

	if(mhd == INVALID_HANDLE_VALUE) { return FALSE; }

	DWORD wlen = 0;
	if( ::WriteFile(mhd , (LPCVOID)c_result ,len_result, &wlen , 0) == 0)
	{
		::CloseHandle(mhd);
		return FALSE ;
	}

	::CloseHandle(mhd);
	
	free(c_result);
	
	return TRUE;
}

BOOL ApiController::CancelDownload(CString appPathWithoutExtention)
{
	CFileFind finder;
	BOOL found = finder.FindFile(appPathWithoutExtention + _T(".png"));
	if (found)
	{
		::DeleteFile(appPathWithoutExtention + _T(".png"));
	}
	return TRUE;
}