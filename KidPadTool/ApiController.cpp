#include "stdafx.h"
#include "ApiController.h"
#include "KidPadTool.h"
#include "base64.h"
#include "Base64Util.h"
#include "KidPadTool.h"
#include "threadAction.h"
#include <process.h>

#define _NUTIL_LIB_REFERENCE 1
#include <nutil.h>

#ifdef	_DEMO_USE_STDUSBDISKDRIVER_
#include "StdUsbDiskDriver.h"
#else
#include "NvtOffsetUsbDiskDriver.h"
#endif

#include "FileSystem.h"
#include <afx.h>
#include <gdiplus.h>


ApiController *api_pointer = 0 ;
CShockwaveflash1 *flash_pointer = 0;
CString m_downloadDirectory;
	

using namespace Gdiplus;

static void __cdecl TestCmdApp(void * p1, void * p2, void * p3)
{
	return api_pointer->ConvertAVCallback(p1, p2, p3);
}

void ApiController::ConvertAVCallback(void * information, void * percentage, void * milisecond)
{
	const char * buff = (char*)information;
	TCHAR v[24] = {0};
	unsigned int duration = (unsigned int)percentage;

	if(buff == 0)
	{
		::OutputDebugString(_T("Action Completed!\n"));
		flashUI->CallFunction(_T("<invoke name='FL_completeConvert'><arguments><string></string></arguments></invoke>"));
	}
	else
	{
		_itot(duration, v, 10);
		//::OutputDebugString(v);
		//::OutputDebugString(_T("%\r\n"));
		CString str;
		str.Format(L"%d", duration);
		flashUI->CallFunction(_T("<invoke name='FL_setConvertPercentage'><arguments><string>") + str + _T("</string></arguments></invoke>"));
	}
}

void ApiController::Initialize()
{
	api_pointer = this;
	flash_pointer = flashUI;
	
	m_pu8xmlBuffer = NULL;
	
	ScanUsbDisk();

	TCHAR szFolder[MAX_PATH * 2];
	HMODULE hModule = GetModuleHandle( 0 );
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	m_workingFolderName = szFolder;
	m_workingFolderName = m_workingFolderName.Left(m_workingFolderName.ReverseFind(_T('\\')));

	// create download directory and xml
	m_downloadDirectory = m_workingFolderName + _T("\\Download\\");
	m_mediaDirectory4Video = m_workingFolderName + _T("\\Video\\");
	m_mediaDirectory4Music = m_workingFolderName + _T("\\Music\\");
	m_mediaDirectory4Picture = m_workingFolderName + _T("\\Picture\\");
	m_downloadXml = m_downloadDirectory +  _T("downloads.xml");
	m_downloadDoc = new TiXmlDocument();
			
	DWORD attribs = ::GetFileAttributes(m_downloadDirectory);
	if (attribs == INVALID_FILE_ATTRIBUTES ||
		!(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
			::CreateDirectory(m_downloadDirectory, NULL);
	}
	flashUI->CallFunction(_T("<invoke name='FL_setDownloadDirectory'><arguments><string>") + m_downloadDirectory + _T("</string></arguments></invoke>"));

	attribs = ::GetFileAttributes(m_mediaDirectory4Video);
	if (attribs == INVALID_FILE_ATTRIBUTES ||
		!(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
			::CreateDirectory(m_mediaDirectory4Video, NULL);
	}

	attribs = ::GetFileAttributes(m_mediaDirectory4Music);
	if (attribs == INVALID_FILE_ATTRIBUTES ||
		!(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
			::CreateDirectory(m_mediaDirectory4Music, NULL);
	}

	attribs = ::GetFileAttributes(m_mediaDirectory4Picture);
	if (attribs == INVALID_FILE_ATTRIBUTES ||
		!(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
			::CreateDirectory(m_mediaDirectory4Picture, NULL);
	}

	attribs = ::GetFileAttributes(m_downloadXml);
	if (attribs == INVALID_FILE_ATTRIBUTES ||
		!(attribs & FILE_ATTRIBUTE_NORMAL)) {
			TiXmlDeclaration * xmlDec = new TiXmlDeclaration("1.0", "UTF-8", "yes");
			m_downloadDoc->LinkEndChild(xmlDec);
			TiXmlElement * element = new TiXmlElement("Downloads");
			m_downloadDoc->LinkEndChild(element);
			m_downloadDoc->SaveFile(CT2CA(m_downloadXml));
	}
	else {
		m_downloadDoc->LoadFile(CT2CA(m_downloadXml), TIXML_ENCODING_UTF8);
	}
}

void ApiController::Exit()
{
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
				TRACE(m_programUsbDiskName+_T(":________\n"));
				strName = _T("nand1-2");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveNANDName = driveName;
					m_driveNANDName = driveName;


					// talentech added
					strName = _T("Null");
					if (wcsstr( m_programUsbDiskName, strName ) != 0){
						flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
					}
					else{
						UINT32 blockSize, freeSize, diskSize;
						if (fsDiskFreeSpace(driveNo, &blockSize, &freeSize, &diskSize) == FS_OK){
							CString request;
							request.Format(_T("<invoke name='FL_setDiskVolumnStatus'><arguments><string>%i,%i</string></arguments></invoke>"), (INT)freeSize/1024, (INT)diskSize/1024);
							flashUI->CallFunction(request);
						}
						flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>1</string></arguments></invoke>"));
					}
				}
				strName = _T("USB Card Reader");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveNANDName = driveName;
					m_driveNANDName = driveName;
				}
				strName = _T("MSC NAND");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveNANDName = driveName;
					m_driveNANDName = driveName;
				}
				/*
				strName = _T("sd");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveNANDName = driveName;
					m_driveSDName = driveName;
				}
				*/
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

	BOOL found = FALSE;
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
				found = TRUE;

				scopedUsbDisk.release();
				scopedUsbDiskDriver.release();

				// set variables on flash side
				flashUI->CallFunction(_T("<invoke name='FL_findNRDGameStoryAndAppRoot'><arguments><string>") + m_driveNANDName + _T("</string></arguments></invoke>"));
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

	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		LPITEMIDLIST pidl;

		bi.hwndOwner = ownerWindow->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = m_pszBuffer;
		bi.lpszTitle = _T("选择一个文件夹");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = (LPARAM)m_pszBuffer;

		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, m_pszBuffer))
			{ 
				ret = m_pszBuffer;
			}
			pMalloc->Free(pidl);
		}
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
		CString ret = GetDeviceFileContent(CString(args));
		cs += ret;
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDeviceIconBase64") == 0) {
		CString cs("<string>");
		cs += GetDeviceIconBase64(CString(args));
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDownloadedApps") == 0) {
		GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
		memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
		_tcscat(tpb->parameter1, m_downloadDirectory.GetBuffer(m_downloadDirectory.GetLength()));
		tpb->flash_pointer = flashUI;
		_beginthread(threadGetDownloadedApps, 0, (void *)tpb);
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
		cs += InstallNPK(CString(args));
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
		CString str(args);
		CancelDownload(str);
	}
	else if (strcmp(request, "F2C_deleteAppOnPc") == 0) {
		CString appName(args);
		::DeleteFile(m_downloadDirectory + _T('\\') + appName + _T(".npk"));
		::DeleteFile(m_downloadDirectory + _T('\\') + appName + _T(".png"));
	}
	else if (strcmp(request, "F2C_import2Library") == 0) {
		Import2Library();
	}
	else if (strcmp(request, "F2C_getFirmwareVersion") == 0) {
		CString cs("<string>");
		cs += GetFirmwareVersion();
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_updateFirmware") == 0) {
		CString str(args);
		CString cs("<string>");
		if (UpdateFirmware(str))
			cs += CString("1");
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_TRACE") == 0) {
		TRACE(args);
		TRACE(_T("\n"));
	}
	else if (strcmp(request, "F2C_getLocalMediaPath") == 0) {
		CString cs("<string>");
		cs += BrowsePC();
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getLocalPictures") == 0) {
		CString directory(args);
		CString extension("gif,jpg,jpeg,png,bmp");
		CString mediatype("picture");

		GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
		memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
		_tcscat(tpb->parameter1, directory.GetBuffer(directory.GetLength()));
		_tcscat(tpb->parameter2, extension.GetBuffer(extension.GetLength()));
		_tcscat(tpb->parameter3, mediatype.GetBuffer(mediatype.GetLength()));
		tpb->flash_pointer = flashUI;
		_beginthread(threadGetLocalMedia, 0, (void *)tpb);
	}
	else if (strcmp(request, "F2C_getLocalMusics") == 0) {
		CString directory(args);
		CString extension("mp3,wav,wma,m4a");
		CString mediatype("music");

		GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
		memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
		_tcscat(tpb->parameter1, directory.GetBuffer(directory.GetLength()));
		_tcscat(tpb->parameter2, extension.GetBuffer(extension.GetLength()));
		_tcscat(tpb->parameter3, mediatype.GetBuffer(mediatype.GetLength()));
		tpb->flash_pointer = flashUI;
		_beginthread(threadGetLocalMedia, 0, (void *)tpb);
	}
	else if (strcmp(request, "F2C_getLocalVideos") == 0) {
		CString directory(args);
		CString extension("mp4,avi,flv,asf");
		CString mediatype("video");

		GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
		memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
		_tcscat(tpb->parameter1, directory.GetBuffer(directory.GetLength()));
		_tcscat(tpb->parameter2, extension.GetBuffer(extension.GetLength()));
		_tcscat(tpb->parameter3, mediatype.GetBuffer(mediatype.GetLength()));
		tpb->flash_pointer = flashUI;
		_beginthread(threadGetLocalMedia, 0, (void *)tpb);
	}
	else if (strcmp(request, "F2C_convertVideo") == 0) {
		CString str(args);
		ConvertVideo(str);
	}
	else if (strcmp(request, "F2C_convertMusic") == 0) {
		CString str(args);
		ConvertMusic(str);
	}
	else if (strcmp(request, "F2C_convertPicture") == 0) {
		CString str(args);
		ConvertPicture(str);
	}
	else if (strcmp(request, "F2C_transferMusic2Device") == 0) {
		CString sourceFile(args);
		sourceFile = m_mediaDirectory4Music + sourceFile + _T(".mp3");
		CString targetDirectory = m_driveNANDName + _T("\\media\\musics");
		Transfer2Device(sourceFile, targetDirectory);
	}
	else if (strcmp(request, "F2C_transferVideo2Device") == 0) {
		CString sourceFile(args);
		sourceFile = m_mediaDirectory4Video + sourceFile + _T(".flv");
		CString targetDirectory = m_driveNANDName + _T("\\media\\videos");
		Transfer2Device(sourceFile, targetDirectory);
	}
	else if (strcmp(request, "F2C_transferPicture2Device") == 0) {
		CString sourceFile(args);
		sourceFile = m_mediaDirectory4Picture + sourceFile + _T(".jpg");
		CString targetDirectory = m_driveNANDName + _T("\\media\\pictures");
		Transfer2Device(sourceFile, targetDirectory);
	}
	else if (strcmp(request, "F2C_getDeviceVideos") == 0) {
		CString xmlfile = m_driveNANDName + _T("\\media\\videoList.xml");
		CString directory = m_driveNANDName + _T("\\media\\videos\\");
		CString cs("<string>");
		cs += GetDeviceMedia(xmlfile, directory);
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDeviceMusics") == 0) {
		CString xmlfile = m_driveNANDName + _T("\\media\\musicList.xml");
		CString directory = m_driveNANDName + _T("\\media\\musics\\");
		CString cs("<string>");
		cs += GetDeviceMedia(xmlfile, directory);
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDevicePictures") == 0) {
		CString xmlfile = m_driveNANDName + _T("\\media\\pictureList.xml");
		CString directory = m_driveNANDName + _T("\\media\\pictures\\");
		CString cs("<string>");
		cs += GetDeviceMedia(xmlfile, directory);
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_insertMediaNode") == 0) {
		CString str(args);
		CString mediaType = str.Left(str.Find(_T(',')));
		CString filename = str.Right(str.GetLength() - str.Find(_T(',')) - 1);

		CString xmlfile = m_driveNANDName + _T("\\media\\") + mediaType + _T("List.xml");

		CString cs("<string>");
		cs += AppendMediaNodeToXml(xmlfile, filename, mediaType);
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_deleteDeviceMedia") == 0) {
		CString str(args);
		CString type = str.Left(str.Find(_T(',')));
		CString mediaFile = str.Right(str.GetLength() - str.Find(_T(',')) - 1);
		CString xmlFile = m_driveNANDName + _T("\\media\\") + type + _T("List.xml");

		DeleteDeviceMedia(xmlFile, mediaFile);
	}
}

BOOL ApiController::Import2Library()
{
	CString folder = BrowsePC();
	if (folder && folder.GetLength() > 0)
	{
		CFileFind finder;
		BOOL found = finder.FindFile(folder + _T("\\*.npk"));
		if (!found){ finder.Close(); return FALSE; }

		while(found)
		{
			found = finder.FindNextFile();

			if (finder.IsDots())
				continue;

			CString npkFile = finder.GetFilePath();
			InstallNPK(npkFile);
		}
		finder.Close();
	}
	return TRUE;
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

	CString cstr_v = b64::UTF8ToCString(m_pu8xmlBuffer, (unsigned int) nBytes);
	return cstr_v;
}

CString ApiController::GetDeviceIconBase64(CString iconFilePath)
{
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

BOOL ApiController::DeleteAppOnDevice(CString appDirectoryPaths)
{
	// appDirectoryPaths: appName,appDirectoryPath,appCategoryXmlFilePath
	CString appName = appDirectoryPaths.Left(appDirectoryPaths.Find(_T(',')));
	CString appDirectory = appDirectoryPaths.Left(appDirectoryPaths.ReverseFind(_T(',')));
	appDirectory = appDirectory.Right(appDirectory.GetLength() - appDirectory.Find(_T(',')) - 1);
	CString appCategoryXml = appDirectoryPaths.Right(appDirectoryPaths.GetLength() - appDirectoryPaths.ReverseFind(_T(',')) - 1);


	/*********************************
	*** delete files from device ****
	*********************************/
	if (DeleteDirectoryOnDevice(appDirectory) == false) {
		MessageBox(ownerWindow->m_hWnd, _T("删除设备上内容\"") + appName + _T("\"失败"), _T("删除错误"), MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	/*********************************
	*** update xml on device ****
	*********************************/
	DeleteAppNodeOnDeviceXml(appCategoryXml, appName);

	return TRUE;
}

BOOL ApiController::DeleteDirectoryOnDevice(CString directory) {
	FILE_FIND_T fileFind;
	memset(&fileFind, 0x00, sizeof(FILE_FIND_T));
	CString tempName;

	// may need error handle here
	DWORD err = fsFindFirst((CHAR *)(LPCWSTR)(directory + _T('\\')), NULL, &fileFind);
	if (err != 0)
	{
		fsFindClose(&fileFind);
		//MessageBox(ownerWindow->m_hWnd, _T("找不到设备上的\"") + directory + _T("\"文件夹"), _T("错误"), MB_OK|MB_ICONSTOP);
		return TRUE;
	}

	do
	{
		if (CString((LPCWSTR)fileFind.suLongName) == _T("."))
			continue;
		if (CString((LPCWSTR)fileFind.suLongName) == _T(".."))
			continue;

		tempName = directory + _T('\\') + (LPCWSTR)fileFind.suLongName;
		int l = tempName.GetLength();

		if (fileFind.ucAttrib & A_DIR)
		{
			if (DeleteDirectoryOnDevice(tempName) == false)
			{
				// may need error handle here
				err = fsFindClose(&fileFind);
				return FALSE;
			}
		}
		else
		{
			// may need error handle here
			err = fsDeleteFile((CHAR *)(LPCWSTR)tempName, NULL);
			if (err != FS_OK)
			{
				MessageBox(ownerWindow->m_hWnd, _T("删除设备上的\"")+tempName+_T("\"文件失败"), _T("错误"), MB_OK|MB_ICONSTOP );
				fsFindClose(&fileFind);
				return FALSE;
			}
		}
	}
	while (fsFindNext(&fileFind) == FS_OK);

	// may need error handle here
	err = fsFindClose(&fileFind);

	err = fsRemoveDirectory((CHAR *)(LPCWSTR)directory, NULL);
	if (err != FS_OK)
	{
		MessageBox(ownerWindow->m_hWnd, __T("删除设备上的\"")+directory+_T("\"文件夹失败"), _T("错误"), MB_OK|MB_ICONSTOP );
		return FALSE;
	}
	return TRUE;
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
		MessageBox(ownerWindow->m_hWnd, _T("没有任何安装的内容"), _T("信息"), MB_OK );
		return TRUE;
	}

	CString tempName;
	BOOL found = FALSE;
	// may enhance here, looply found name and deploytype
	for (xElement = node->ToElement(); xElement; xElement = xElement->NextSiblingElement())
	{
		nameElement = xElement->FirstChildElement();
		tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
		if (tempName == _T("name"))
		{
			tempName = CA2CT(nameElement->GetText(), CP_UTF8);
			if (tempName == appName)
			{
				found = TRUE;
				break;
			}
		}
	}
	if (found == FALSE)
	{
		TRACE(_T("found no match node for ") + appName + _T("!!!\n"));
		return TRUE;
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
	if (hdl2 != FS_OK)
	{
		fsCloseFile(hdl);
		MessageBox(ownerWindow->m_hWnd, _T("写XML文件出错，可能磁盘已满"), _T("错误"), MB_OK|MB_ICONSTOP );
		return FALSE;
	}
	fsCloseFile(hdl);
	return TRUE;
}

CString ApiController::InstallNPK(CString npkFile)
{
	CString appName = npkFile.Right(npkFile.GetLength() - npkFile.ReverseFind('\\') - 1);
	appName = appName.Left(appName.GetLength() - 4);
	CString workFolder = npkFile.Left(npkFile.ReverseFind('\\'));
	CString tempFolder = workFolder + _T("\\") + appName;
	CString decompressedXmlFile;
	CString appCategoryXml;
	

	/*********************************
	*** uncompress to temp folder ****
	*********************************/
	if (!::CreateDirectory(tempFolder, NULL))
	{
		MessageBox(ownerWindow->m_hWnd, _T("创建\"") + appName + _T("\"临时目录失败，可能该文件夹已存在，或磁盘空间已满"), _T("Error"), MB_OK|MB_ICONSTOP);
		return _T("");
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

	decompressedXmlFile = tempFolder + _T("\\install.xml");
	
	/*********************************
	*** get deploy type **************
	*********************************/
	TiXmlDocument doc;
    doc.LoadFile(CT2CA(decompressedXmlFile), TIXML_ENCODING_UTF8);
    TiXmlHandle docHandle(&doc);
	TiXmlElement* elm = docHandle.FirstChild("install").FirstChild("program").FirstChildElement("uri").ToElement();
	const char *uri = elm->GetText();
	CString category = CA2CT(uri);
	category = category.Left(4);
	if (category == "yzyx")
		category = "zyyx";
	else if (category == "jyrz")
		category = "jyrz1";
	appCategoryXml = m_driveNANDName + _T("\\book\\storyList_") + category + _T(".xml");

    
	/*********************************
	*** reject if already exists *****
	*********************************/
	if (IsApplicationInstalled(appName, appCategoryXml))
	{
		MessageBox(ownerWindow->m_hWnd, _T("应用\"") + appName + _T("\"已经安装在设备上"), _T("信息"), MB_OK|MB_ICONSTOP);
		DeletePcDirectory(tempFolder);
		::RemoveDirectory(tempFolder);
		return category;
	}

	
	/*********************************
	*** update install xml ***********
	*********************************/
	program_t program;
	InsertXMLBufferElement(decompressedXmlFile, appCategoryXml, program);

	CString destName = m_driveNANDName + _T("\\book\\") + appName;
	
	if (!CopyDirectory(tempFolder, destName))
	{
		MessageBox(ownerWindow->m_hWnd, _T("将内容拷贝到设备失败"), _T("错误"), MB_OK|MB_ICONSTOP);
		return _T("");
	}

	// delete files on pc
	DeletePcDirectory(tempFolder);
	::RemoveDirectory(tempFolder);

	return category;
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
		return FALSE;

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
				return TRUE;
		}
	}
	return FALSE;
}

BOOL ApiController::InsertXMLBufferElement(CString xmlFile, CString appCategoryXml, program_t& program)
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
			const char* nn = element->GetText();
			memcpy(program.name, nn, strlen(element->GetText())+1);
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

	CString appName(program.name);
	appName = b64::UTF8ToCString(program.name, strlen(program.name));

	CString iconStr = appName + _T('/') + b64::UTF8ToCString(program.icon, strlen(program.icon));
	CString entryStr = appName + _T("/body/") + b64::UTF8ToCString(program.entry, strlen(program.entry));

	// read xml and export to program struct
	TiXmlElement* story = new TiXmlElement("story");

	TiXmlElement* name = new TiXmlElement("name" );
	TiXmlText* nameText = new TiXmlText(program.name);
	TiXmlElement* icon = new TiXmlElement( "icon" );
	TiXmlText* iconText = new TiXmlText(CW2A(iconStr, CP_UTF8));
	TiXmlElement* type = new TiXmlElement( "type" );
	TiXmlText* typeText = new TiXmlText(program.type);
	TiXmlElement* entry = new TiXmlElement( "entry" );
	TiXmlText* entryText = new TiXmlText(CW2A(entryStr, CP_UTF8));
	TiXmlElement* deployType = new TiXmlElement( "deployType" );
	TiXmlText* deployTypeText = new TiXmlText(program.deployType);

	name->LinkEndChild( nameText );
	story->LinkEndChild( name );
	icon->LinkEndChild(iconText);
	story->LinkEndChild( icon );
	type->LinkEndChild( typeText );
	story->LinkEndChild( type );
	entry->LinkEndChild( entryText );
	story->LinkEndChild( entry );
	deployType->LinkEndChild( deployTypeText );
	story->LinkEndChild( deployType );

	if (program.hasSecurity)
	{
		CString imageStr = appName + _T('/') + program.securityImage;
		CString passwdStr = _T("temppass.bin");
		CString mountStr = appName + _T("/body/");
		
		TiXmlElement* security = new TiXmlElement( "security" );

		TiXmlElement* image = new TiXmlElement( "image" );
		TiXmlText* imageText = new TiXmlText(CW2A(imageStr, CP_UTF8));
		TiXmlElement* passwd = new TiXmlElement( "passwd" );
		TiXmlText* passwdText = new TiXmlText(CW2A(passwdStr, CP_UTF8));
		TiXmlElement* mountPoint = new TiXmlElement( "mountPoint" );
		TiXmlText* mountPointText = new TiXmlText(CW2A(mountStr, CP_UTF8));

		image->LinkEndChild( imageText );
		security->LinkEndChild( image );
		passwd->LinkEndChild( passwdText );
		security->LinkEndChild( passwd );
		mountPoint->LinkEndChild( mountPointText );
		security->LinkEndChild( mountPoint );

		story->LinkEndChild( security );
	}

	if (program.bgColor && strlen(program.bgColor)>0)
	{
		TiXmlElement* bgColor = new TiXmlElement( "bgColor" );
		TiXmlText* bgColorText = new TiXmlText(program.bgColor);

		bgColor->LinkEndChild( bgColorText );
		story->LinkEndChild( bgColor );
	}

	if (program.asMain && strlen(program.asMain)>0)
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
	//node->LinkEndChild( story );
	node->InsertBeforeChild(doc2.RootElement()->FirstChildElement("story"), *story);
	TiXmlPrinter printer;
    doc2.Accept(&printer);
    const char* ret = printer.CStr();
	
	int hdl, hdl2;
	hdl = hdl2 = 0;

	hdl = fsOpenFile((CHAR *)(LPCWSTR)appCategoryXml, NULL, O_TRUNC);

	INT nWriteCnt;
	INT len = strlen(ret);
	hdl2 = fsWriteFile(hdl, (UINT8 *)ret, strlen(ret), &nWriteCnt);
	if (hdl2 != FS_OK)
	{
		hdl2 = fsCloseFile(hdl);
		MessageBox(ownerWindow->m_hWnd, _T("写XML文件失败，请检查磁盘空间是否已满"), _T("错误"), MB_OK|MB_ICONSTOP );
		return FALSE;
	}
	fsCloseFile(hdl);

	CString content = GetDeviceFileContent(appCategoryXml);
	return TRUE;
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
	if (err != FS_OK)
	{
		if (err != ERR_DIR_BUILD_EXIST)
		{
			MessageBox(ownerWindow->m_hWnd, _T("在设备上创建\"") + destName + _T("\"文件夹失败，可能已经存在或设备磁盘空间已满"), _T("错误"), MB_OK|MB_ICONSTOP );
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
			if ( CopyDirectory( srcTempName, destTempName ) == FALSE )
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
					MessageBox(ownerWindow->m_hWnd, _T("磁盘已满"), _T("错误"), MB_OK|MB_ICONSTOP);
					return FALSE;
				}
			}

			tempName = destName + _T("\\") + info.cFileName;
			CString suFileName = tempName;
			err = fsOpenFile( (CHAR *)(LPCWSTR)suFileName, NULL, O_CREATE|O_TRUNC );
			if ( err < 0 )
			{
				FindClose( hwnd );
				MessageBox(ownerWindow->m_hWnd, _T("在设备上创建\"") + suFileName + _T("\"文件失败，可能已经存在或设备磁盘空间已满"), _T("错误"), MB_OK|MB_ICONSTOP);
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
					MessageBox(ownerWindow->m_hWnd, srcTempName, _T("打开计算机上文件失败"), MB_OK|MB_ICONSTOP );
					return FALSE;
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
						MessageBox(ownerWindow->m_hWnd, _T("在设备上写\"") + suFileName + _T("\"文件失败，可能设备磁盘空间已满"), _T("错误"), MB_OK|MB_ICONSTOP );
						return FALSE;
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
				return FALSE;
			}

			err2 = fsCloseFile( err );
			if ( err2 != FS_OK )
			{
				FindClose( hwnd );
				MessageBox(ownerWindow->m_hWnd, _T("Close \"") + suFileName + _T("\" failed, may disk full"), _T("Error"), MB_OK|MB_ICONSTOP );
				return FALSE;
			}
		}
	}
	while ( FindNextFile( hwnd, &info ) );

	FindClose( hwnd );
	return TRUE;
}

BOOL ApiController::SaveFileFromBase64(CString base64String, CString filePath)
{
	//base64String = GetBase64DataFromTextFile(_T("C:\\pic_2.txt"));
	BOOL rtv = FALSE ; 
	unsigned char * c_result = 0 ;
	unsigned int len_result = 0 ;
	
	len_result = b64::Base64Decode(base64String, (void**)(&c_result));	

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
		return FALSE;
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

CString ApiController::GetFirmwareVersion()
{
	if (m_driveNANDName.GetLength() > 0) {
		CString xmlFile = m_driveNANDName + _T("\\builtIn\\ui.xml");

		
		FILE_STAT_T stat;
		CString s = m_driveNANDName + _T("\\builtIn\\ui.xml");
		INT nStatus = fsGetFileStatus(-1, (CHAR *)(LPCWSTR)s, NULL, &stat);
		if (nStatus == 0)
		{
			CString cs = GetDeviceFileContent(s);
			int start = cs.Find(_T("<version>")) + 9;
			int end = cs.Find(_T("</version>"));
			cs = cs.Right(cs.GetLength() - start);
			return cs.Left(end - start);
		}
	}
	return _T("0.9");
}

BOOL ApiController::UpdateFirmware(CString zipFilePath)
{
	CString builtInPath = m_driveNANDName + _T("\\builtIn");
	CString decompressPath = m_downloadDirectory + _T("\\builtIn");

	/***************************
	** decompress builtIn.zip **
	****************************/
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	CString	strCmd;
	strCmd.Format(_T("\"%s\\7za.exe\" x -o\"%s\" -y \"%s\""), m_workingFolderName, m_downloadDirectory, zipFilePath);
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


	/***************************
	** delete obsolete builtIn *
	****************************/
	if (DeleteDirectoryOnDevice(builtInPath) == false) {
		MessageBox(ownerWindow->m_hWnd, _T("删除设备上系统文件失败"), _T("错误"), MB_OK|MB_ICONSTOP);

		DeletePcDirectory(decompressPath);
		::RemoveDirectory(decompressPath);
		::DeleteFile(zipFilePath);

		return FALSE;
	}


	/***************************
	** copy builtIn to device **
	****************************/
	if (!CopyDirectory(m_downloadDirectory + _T("\\builtIn"), builtInPath))
	{
		MessageBox(ownerWindow->m_hWnd, _T("将系统文件拷贝到设备失败"), _T("错误"), MB_OK|MB_ICONSTOP);

		DeletePcDirectory(decompressPath);
		::RemoveDirectory(decompressPath);
		::DeleteFile(zipFilePath);

		return FALSE;
	}

	DeletePcDirectory(decompressPath);
	::RemoveDirectory(decompressPath);
	::DeleteFile(zipFilePath);

	return TRUE;
}

CString ApiController::GetDeviceMedia(CString xmlFile, CString directory)
{
	CString xml = GetDeviceFileContent(xmlFile);
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
		return _T('');

	CString tempName;
	CString ret_value;
	TCHAR fileSizeBuf[16];

	CString entry = _T("entry");
	if (directory.Find(_T("pictures")) > 0)
		entry = _T("icon");

	// may enhance here, looply found name and deploytype
	for (xElement = node->ToElement(); xElement; xElement = xElement->NextSiblingElement())
	{
		for (nameElement = xElement->FirstChildElement(); nameElement; nameElement = nameElement->NextSiblingElement())
		{
			tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
			if (tempName == entry)
			{
				tempName = CA2CT(nameElement->GetText(), CP_UTF8);
				CString file = tempName.Right(tempName.GetLength() - tempName.Find('/') - 1);
				file = directory + file;

				int hdl = fsOpenFile((CHAR *)(LPCWSTR)file, NULL, O_RDONLY);
				INT nBytes = (INT)fsGetFileSize(hdl);
				fsCloseFile(hdl);
				
				_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), nBytes);
				ret_value += file;
				ret_value += _T("#");
				ret_value += fileSizeBuf;
				ret_value += ",";

				break;
			}
		}
	}

	if (ret_value.GetLength() > 0)
		ret_value = ret_value.Left(ret_value.GetLength() - 1);

	return ret_value;
}

void ApiController::ConvertMusic(CString filepath)
{
	CString target = filepath.Right(filepath.GetLength() - filepath.ReverseFind(_T('\\')) - 1);
	target = target.Left(target.ReverseFind(_T('.')));
	target = m_mediaDirectory4Music + target + _T(".mp3");

	CmdAppParam * cap = (CmdAppParam*)malloc(sizeof(CmdAppParam)) ;
	cap->cmd_str = (TCHAR*)malloc(2048);
	memset(cap->cmd_str, 0, 2048);
#ifdef _DEBUG
	_tcscat(cap->cmd_str, _T("C:\\ffmpeg.exe -i \"") + filepath + _T("\" -vn -ar 44100 -ac 2 -ab 19200 -f mp3 -y \"") + target + _T('\"'));
#else
	_tcscat(cap->cmd_str, _T("\"") + m_workingFolderName + _T("\\ffmpeg.exe\" -i \"") + filepath + _T("\" -vn -ar 44100 -ac 2 -ab 19200 -f mp3 -y \"") + target + _T('\"'));
#endif
	cap->fn = TestCmdApp;
	TRACE(cap->cmd_str);
	TRACE(_T("\n"));
	RunCmdAppForFFMpeg(cap);
}

void ApiController::ConvertVideo(CString filepath)
{
	CString target = filepath.Right(filepath.GetLength() - filepath.ReverseFind(_T('\\')) - 1);
	target = target.Left(target.ReverseFind(_T('.')));
	target = m_mediaDirectory4Video + target + _T(".flv");

	CmdAppParam * cap = (CmdAppParam*)malloc(sizeof(CmdAppParam)) ;
	cap->cmd_str = (TCHAR*)malloc(2048);
	memset(cap->cmd_str, 0, 2048);
	_tcscat(cap->cmd_str, _T("\"") + m_workingFolderName + _T("\\ffmpeg.exe\" -i \"") + filepath + _T("\" -b:v 512k -ar 44100 -s 480x272 -y \"") + target + _T('\"'));
	cap->fn = TestCmdApp;
	RunCmdAppForFFMpeg(cap);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)   
{   
    UINT num = 0;
    UINT size = 0;
    ImageCodecInfo* pImageCodecInfo = NULL;   
    GetImageEncodersSize(&num, &size);   
    if(size == 0)   
        return -1;
    
    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));   
    if(pImageCodecInfo == NULL)   
        return -1;

    GetImageEncoders(num, size, pImageCodecInfo);   
    for(UINT j = 0; j < num; ++j)   
    {   
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )   
        {   
            *pClsid = pImageCodecInfo[j].Clsid;   
            free(pImageCodecInfo);   
            return j;
        }           
    }   
    free(pImageCodecInfo);   
    return -1;
}

Gdiplus::Bitmap* ResizeClone(Bitmap *bmp, INT width, INT height)
{
    UINT o_height = bmp->GetHeight();
    UINT o_width = bmp->GetWidth();
    INT n_width = width;
    INT n_height = height;
    double ratio = ((double)o_width) / ((double)o_height);
    if (o_width > o_height) {
        n_height = static_cast<UINT>(((double)n_width) / ratio);
    } else {
        n_width = static_cast<UINT>(n_height * ratio);
    }
    Gdiplus::Bitmap* newBitmap = new Gdiplus::Bitmap(n_width, n_height, bmp->GetPixelFormat());
    Gdiplus::Graphics graphics(newBitmap);
    graphics.DrawImage(bmp, 0, 0, n_width, n_height);
    return newBitmap;
}

void ApiController::ConvertPicture(CString filepath)
{
	CString target = filepath.Right(filepath.GetLength() - filepath.ReverseFind(_T('\\')) - 1);
	target = target.Left(target.ReverseFind(_T('.')));
	target = m_mediaDirectory4Picture + target + _T(".jpg");

	GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		CLSID             encoderClsid;
		EncoderParameters encoderParameters;
	    
		Gdiplus::Bitmap bmp(filepath);
		Gdiplus::Bitmap *image = ResizeClone(&bmp, 480, 272);
		
		GetEncoderClsid(L"image/jpeg", &encoderClsid);
		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;
	    
		ULONG quality = 80;
		encoderParameters.Parameter[0].Value = &quality;
		image->Save(target, &encoderClsid, &encoderParameters);

		delete image;
		image = NULL;
	}

    GdiplusShutdown(gdiplusToken);

	flashUI->CallFunction(_T("<invoke name='FL_completeConvert'><arguments><string></string></arguments></invoke>"));
}

CString ApiController::AppendMediaNodeToXml(CString xmlFile, CString filename, CString mediaType)
{
	CString xml = GetDeviceFileContent(xmlFile);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);

	TiXmlNode *node = 0;
	TiXmlElement *listElement = 0;
	TiXmlElement *xElement = 0;
	TiXmlElement *nameElement = 0;
	
	node = doc.RootElement();
	listElement = node->ToElement();

	// check duplicate
	node = doc.RootElement();
	listElement = node->ToElement();

	TiXmlElement *childElement = listElement->FirstChildElement();

	if (childElement)
	{
		CString tempName;
		for (xElement = childElement; xElement; xElement = xElement->NextSiblingElement())
		{
			nameElement = xElement->FirstChildElement();
			tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
			if (tempName == _T("name"))
			{
				tempName = CA2CT(nameElement->GetText(), CP_UTF8);
				if (tempName == filename)
					return _T("");
			}
		}
	}

	CString directory = m_driveNANDName + _T("\\media\\") + mediaType + _T("s\\");
	CString extension;
	if (mediaType == _T("video"))
		extension = _T(".flv");
	else if (mediaType == _T("music"))
		extension = _T(".mp3");
	else if (mediaType == _T("picture"))
		extension = _T(".jpg");
	

	TiXmlElement* media = new TiXmlElement(CW2A(mediaType));
	TiXmlElement* name = new TiXmlElement("name");
	TiXmlText* nameText = new TiXmlText(CW2A(filename, CP_UTF8));
	TiXmlElement* icon = new TiXmlElement("icon");

	TiXmlText* iconText;
	if (mediaType == _T("picture"))
		iconText = new TiXmlText(CW2A(mediaType+"s/"+filename+extension, CP_UTF8));
	else
		iconText = new TiXmlText(CW2A(mediaType+"s/icon.png", CP_UTF8));
	
	TiXmlElement* type = new TiXmlElement( "type" );
	TiXmlText* typeText = new TiXmlText("flash");
	TiXmlElement* entry = new TiXmlElement( "entry" );
	TiXmlText* entryText = new TiXmlText(CW2A(mediaType+"s/"+filename+extension, CP_UTF8));
	TiXmlElement* deployType = new TiXmlElement( "deployType" );
	TiXmlText* deployTypeText = new TiXmlText("media");

	name->LinkEndChild( nameText );
	media->LinkEndChild( name );
	icon->LinkEndChild(iconText);
	media->LinkEndChild( icon );
	type->LinkEndChild( typeText );
	media->LinkEndChild( type );
	entry->LinkEndChild( entryText );
	media->LinkEndChild( entry );
	deployType->LinkEndChild( deployTypeText );
	media->LinkEndChild( deployType );

	TiXmlElement *firstChild = doc.RootElement()->FirstChildElement();
	if (firstChild)
		node->InsertBeforeChild(firstChild, *media);
	else
		node->InsertEndChild(*media);

	TiXmlPrinter printer;
    doc.Accept(&printer);
    const char* ret = printer.CStr();

	int hdl, hdl2;
	hdl = hdl2 = 0;

	hdl = fsOpenFile((CHAR *)(LPCWSTR)xmlFile, NULL, O_TRUNC);

	INT nWriteCnt;
	INT len = strlen(ret);
	hdl2 = fsWriteFile(hdl, (UINT8 *)ret, strlen(ret), &nWriteCnt);
	if (hdl2 != FS_OK)
	{
		fsCloseFile(hdl);
		MessageBox(ownerWindow->m_hWnd, _T("写XML文件出错，可能磁盘已满"), _T("错误"), MB_OK|MB_ICONSTOP );
		return _T("");
	}
	fsCloseFile(hdl);

	CString filepath = directory + filename + extension;
	CString ret_value = filepath;
	ret_value += _T("#");
	TCHAR fileSizeBuf[16];
	hdl = fsOpenFile((CHAR *)(LPCWSTR)filepath, NULL, O_RDONLY);
	INT nBytes = (INT)fsGetFileSize(hdl);
	fsCloseFile(hdl);
	_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), nBytes);
	ret_value += fileSizeBuf;
	return ret_value;
}

void ApiController::Transfer2Device(CString sourceFile, CString targetDirectory)
{
	TRANSFER_PARAMETER_BLOCK *tpb = (TRANSFER_PARAMETER_BLOCK *)malloc(sizeof(TRANSFER_PARAMETER_BLOCK));
	memset(tpb, 0, sizeof(TRANSFER_PARAMETER_BLOCK));

	CString targetFile = targetDirectory + sourceFile.Right(sourceFile.GetLength() - sourceFile.ReverseFind(_T('\\')));
	_tcscat(tpb->sourceFile, sourceFile.GetBuffer(sourceFile.GetLength()));
	_tcscat(tpb->targetFile, targetFile.GetBuffer(targetFile.GetLength()));
	tpb->flash_pointer = flashUI;
	
	_beginthread(threadUpdatePercentage, 0, (void *)tpb);
}

void ApiController::DeleteDeviceMedia(CString xmlFile, CString mediaFile)
{
	CString name = mediaFile.Right(mediaFile.GetLength() - mediaFile.ReverseFind(_T('\\')) - 1);
	name = name.Left(name.ReverseFind(_T('.')));

	// delete xml node
	CString xml = GetDeviceFileContent(xmlFile);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);

	TiXmlNode *node = 0;
	TiXmlElement *listElement = 0;
	TiXmlElement *xElement = 0;
	TiXmlElement *nameElement = 0;
	
	node = doc.RootElement();
	listElement = node->ToElement();
	node = listElement->FirstChildElement();
	if (!node) return;
	
	CString tempName;
	BOOL found = FALSE;
	// may enhance here, looply found name and deploytype
	for (xElement = node->ToElement(); xElement; xElement = xElement->NextSiblingElement())
	{
		nameElement = xElement->FirstChildElement();
		tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
		if (tempName == _T("name"))
		{
			tempName = CA2CT(nameElement->GetText(), CP_UTF8);
			if (tempName == name)
			{
				found = TRUE;
				break;
			}
		}
	}
	if (found == FALSE)
		return;
	
	listElement->RemoveChild(xElement);

	TiXmlPrinter printer;
    doc.Accept(&printer);
    const char* ret = printer.CStr();

	int hdl, hdl2;
	hdl = hdl2 = 0;

	hdl = fsOpenFile((CHAR *)(LPCWSTR)xmlFile, NULL, O_TRUNC);

	INT nWriteCnt;
	INT len = strlen(ret);
	hdl2 = fsWriteFile(hdl, (UINT8 *)ret, strlen(ret), &nWriteCnt);
	if (hdl2 != FS_OK)
	{
		fsCloseFile(hdl);
		MessageBox(ownerWindow->m_hWnd, _T("写XML文件出错，可能磁盘已满"), _T("错误"), MB_OK|MB_ICONSTOP );
		return;
	}
	fsCloseFile(hdl);

	// delete media file on device
	int err = fsDeleteFile((CHAR *)(LPCWSTR)mediaFile, NULL);
	if (err != FS_OK)
	{
		MessageBox(ownerWindow->m_hWnd, _T("删除设备上的\"")+mediaFile+_T("\"文件失败"), _T("错误"), MB_OK|MB_ICONSTOP );
	}
}