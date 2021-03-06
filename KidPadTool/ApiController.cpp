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

#define _NABASE_LIB_REFERENCE 1
#include <nbase.h>

#ifdef	_DEMO_USE_STDUSBDISKDRIVER_
#include "StdUsbDiskDriver.h"
#else
#include "NvtOffsetUsbDiskDriver.h"
#endif

#include "FileSystem.h"
#include <afx.h>
#include <gdiplus.h>

#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")


static int volatile usb_disk_status  ;
static int volatile usb_device_no  ;
static TCHAR usb_disk_flag[24] ;
static int volatile flash_app_inited  ;
static CString os_platform = _T("epad");
static CString chip_version = _T("93");
//static int volatile usb_rescan_flag  ;

using namespace Gdiplus;

static void __cdecl ConvertAVCmd(void * p1, void * p2, void * p3)
{
	//return api_pointer->ConvertAVCallback(p1, p2, p3);
	api_pointer->ConvertAVCallback(p1, p2, p3);
	return ;
}

static void __cdecl ExtractZipCmd(void * p1, void * p2, void * p3)
{
	//return api_pointer->ExtractZipCallback(p1, p2, p3);
	api_pointer->ExtractZipCallback(p1, p2, p3);
	return ;
}


ApiController::ApiController()
{
	usb_disk_status = 0 ;
	usb_device_no = 0 ;
	flash_app_inited = 0 ;
//	usb_rescan_flag = 0 ;
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
		CString str;
		str.Format(L"%d", duration);
		flashUI->CallFunction(_T("<invoke name='FL_setConvertPercentage'><arguments><string>") + str + _T("</string></arguments></invoke>"));
	}
	return ;
}

void ApiController::ExtractZipCallback(void * p1, void * p2, void * p3)
{
	const char * buff = (char *)p1 ;
	if(buff == 0)
	{
		::OutputDebugStringA("Extract Completed!\r\n");
		flashUI->CallFunction(_T("<invoke name='FL_installCompleteExtract'><arguments><string></string></arguments></invoke>"));
	}
	else
	{
		//::OutputDebugStringA(buff);
		CString output = CA2CT(buff, CP_UTF8);
		if (output.Find(_T("Extracting")) == 0)
		{
			flashUI->CallFunction(_T("<invoke name='FL_installSetExtractInformation'><arguments><string>") + output.Right(output.GetLength() - 13) + _T("</string></arguments></invoke>"));
		}
	}
	return ;
}

void ApiController::Initialize()
{
	CString user_data_folder = _T("");

	api_pointer = this;
	flash_pointer = flashUI;
	
	m_pu8xmlBuffer = NULL;
	
	if(this->IsConnected(false) == false)
	{
		ScanUsbDisk();
	}

	TCHAR szFolder[MAX_PATH * 2];
	HMODULE hModule = GetModuleHandle( 0 );
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	GetModuleFileName(hModule, szFolder, sizeof(szFolder));
	m_workingFolderName = szFolder;
	m_workingFolderName = m_workingFolderName.Left(m_workingFolderName.ReverseFind(_T('\\')));

	TCHAR app_data_path[2048] = {0};
	//获取环境变量
	if(GetEnvironmentVariable(_T("APPDATA"), app_data_path, 2048) < 1)
	{
		//如果环境变量读取失败， 用exe所在的目录
		user_data_folder = m_workingFolderName ;
	}
	else
	{
		user_data_folder.Append(app_data_path);
		CString path_comp = _T("\\E巧派 助手");
#ifdef _BYM
	path_comp = _T("儿童学习机 助手");
#endif
#ifdef _XBW
	path_comp = _T("小霸王学习机 助手");
#endif
#ifdef _XBW
	path_comp = _T("童伴熊小米 助手");
#endif
		user_data_folder.Append(path_comp);
		::CreateDirectory(user_data_folder+_T('\\'), NULL);
	}

#ifdef _DEBUG
	m_workingFolderName = _T("C:\\Debug");
	//debug模式， 用固定目录
	user_data_folder = _T("C:\\Debug");
#endif

	// create download directory and xml
	m_downloadDirectory = user_data_folder + _T("\\Download\\");
	m_mediaDirectory4Video = user_data_folder + _T("\\Video\\");
	m_mediaDirectory4Music = user_data_folder + _T("\\Music\\");
	m_mediaDirectory4Picture = user_data_folder + _T("\\Picture\\");
	/*m_downloadDirectory = m_workingFolderName + _T("\\Download\\");
	m_mediaDirectory4Video = m_workingFolderName + _T("\\Video\\");
	m_mediaDirectory4Music = m_workingFolderName + _T("\\Music\\");
	m_mediaDirectory4Picture = m_workingFolderName + _T("\\Picture\\");*/
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
	PDISK_T *pDiskList = fsGetFullDiskInfomation();
	PDISK_T *nextPDisk = pDiskList;
	while (nextPDisk) 
	{
		UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (nextPDisk->ptDriver);
		PARTITION_T *partition = nextPDisk->ptPartList;
		while (partition)
		{
			LDISK_T *lDisk = partition->ptLDisk;
			if (lDisk)
			{
				int driveNo = lDisk->nDriveNo;
							
				TCHAR driveName[] = {driveNo, _T(':'), 0};

				CString strName;
				usbDiskDriver->getProductId(m_programUsbDiskName);
				TRACE(m_programUsbDiskName+_T(":________1\n"));
				strName = _T("nand1-2");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					m_driveNANDName = driveName;
					flashUI->CallFunction(_T("<invoke name='FL_findNRDGameStoryAndAppRoot'><arguments><string>") + m_driveNANDName + _T("</string></arguments></invoke>"));

					// talentech added
					strName = _T("Null");
					if (wcsstr( m_programUsbDiskName, strName ) != 0)
					{
						usb_disk_status = 0 ;
						usb_device_no = 0 ;

						flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
						TRACE("E巧派已断开1!\n");
					}
					else
					{
						usb_disk_status = 1 ;
						usb_device_no = driveNo ;

						UINT32 blockSize, freeSize, diskSize;
						if (fsDiskFreeSpace(driveNo, &blockSize, &freeSize, &diskSize) == FS_OK)
						{
							CString request;
							request.Format(_T("<invoke name='FL_setDiskVolumnStatus'><arguments><string>%i,%i</string></arguments></invoke>"), (INT)freeSize/1024, (INT)diskSize/1024);
							flashUI->CallFunction(request);
						}
						flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>1</string></arguments></invoke>"));
						TRACE("E巧派已连接1\n");
					}
				}
				strName = _T("USB Card Reader");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					usb_disk_status = 1 ;
					usb_device_no = driveNo ;
					m_driveNANDName = driveName;
				}
				strName = _T("MSC NAND");
				if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
				{
					usb_disk_status = 1 ;
					usb_device_no = driveNo ;
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

bool ApiController::UpdateList2(void * pdisk, TCHAR * disk_name)
{
	bool rtv = false ;
	// List scanned logical disks which have been parsed by filesystem.
	if(pdisk == 0)	{ return rtv ; }

	UsbDiskDriver *usbDiskDriver = static_cast<UsbDiskDriver *> (((PDISK_T*)pdisk)->ptDriver);
	PARTITION_T *partition = ((PDISK_T*)pdisk)->ptPartList;
	if(partition == 0)	{ return rtv ; }

	LDISK_T *lDisk = partition->ptLDisk;
	if (lDisk)
	{
		int driveNo = lDisk->nDriveNo;

		TCHAR driveName[] = {driveNo, _T(':'), 0};

		CString strName;
		usbDiskDriver->getProductId(m_programUsbDiskName);
		TRACE(m_programUsbDiskName+_T(":________2\n"));
		strName = _T("nand1-2");
		if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
		{
			m_driveNANDName = driveName;
			flashUI->CallFunction(_T("<invoke name='FL_findNRDGameStoryAndAppRoot'><arguments><string>") + m_driveNANDName + _T("</string></arguments></invoke>"));

			// talentech added
			strName = _T("Null");
			if (wcsstr( m_programUsbDiskName, strName ) != 0)
			{
				usb_disk_status = 0 ;
				usb_device_no = 0 ;
				memset(usb_disk_flag, 0, 24 *sizeof(TCHAR));
				flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
				TRACE("E巧派已断开2!\n");
			}
			else
			{
				usb_disk_status = 1 ;
				usb_device_no = driveNo ;
				memset(usb_disk_flag, 0, 24 *sizeof(TCHAR));
				::_tcscat(usb_disk_flag, disk_name);
				UINT32 blockSize, freeSize, diskSize;
				if (fsDiskFreeSpace(driveNo, &blockSize, &freeSize, &diskSize) == FS_OK)
				{
					CString request;
					request.Format(_T("<invoke name='FL_setDiskVolumnStatus'><arguments><string>%i,%i</string></arguments></invoke>"), (INT)freeSize/1024, (INT)diskSize/1024);
					flashUI->CallFunction(request);
					rtv = true ;
				}
				flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>1</string></arguments></invoke>"));
				TRACE("E巧派已连接2\n");
			}
		}
		strName = _T("USB Card Reader");
		if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
		{
			//usb_disk_status = 1 ;
			//usb_device_no = driveNo ;
			m_driveNANDName = driveName;
		}
		strName = _T("MSC NAND");
		if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
		{
			//usb_disk_status = 1 ;
			//usb_device_no = driveNo ;
			m_driveNANDName = driveName;
		}
		
		//strName = _T("sd");
		//if ( wcsstr( m_programUsbDiskName, strName ) != 0 )
		//{
		//	//m_driveNANDName = driveName;
		//	//m_driveSDName = driveName;
		//	if( (usb_disk_status == 0) || (usb_device_no == 0) )
		//	{
		//		usb_rescan_flag = 1 ;
		//	}
		//}
		
	}
	return rtv ;
	
}
unsigned char ApiController::GetConnectUsbDeviceCode()
{
	return (unsigned char)usb_device_no;
}
TCHAR * ApiController::GetConnectUsbDiskName()
{
	return usb_disk_flag ;
}
bool ApiController::IsConnected(bool scan_flag)
{
	bool rtv = false ;

	if( (usb_disk_status != 1) || (usb_device_no == 0) )
	{
		rtv = false ;
	}
	else
	{
		flash_app_inited = 1 ;

		UINT32 blockSize, freeSize, diskSize;
		if (fsDiskFreeSpace(usb_device_no, &blockSize, &freeSize, &diskSize) == FS_OK)
		{
			rtv = true ;
		}
		else
		{
			usb_disk_status = 0 ;
			usb_device_no  = 0 ;
			if(scan_flag == true)
			{
				ScanUsbDisk();
			}
			if(usb_disk_status == 1)	{ rtv = true ; }
		}
	}
	if(rtv == false)
	{
		if(flash_app_inited == 1)
		{
			flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
		}
	}
	return rtv ;
}
void ApiController::SetConnStatus(bool v)
{
	usb_disk_status = (v == true) ? 1 : 0 ;
	if(v == false)	{ usb_device_no  = 0 ; }
	if(v == false)	{ memset(usb_disk_flag, 0, 24 * sizeof(TCHAR)); }
	return ;
}
bool ApiController::IsDeviceFileExist(CString full_file_name)
{
	bool rtv = false ;
	int hdl = 0;
	
	if(this->IsConnected(false) == false)	{ goto lb_exit ; }
	hdl = fsOpenFile((CHAR *)(LPCWSTR)full_file_name, NULL, O_RDONLY);
	if(hdl < 0)	{ goto lb_exit ; }
	rtv = true ;

lb_exit:

	if(hdl >=0)	{ fsCloseFile(hdl) ; }
	return rtv ;
}
INT64 ApiController::WriteStream2DeviceFile(CString target_file_name, void * stream_buff, INT64 buff_len, bool is_use_temp_flag)
{
	INT64 rtv = 0 ;
	int hdl = 0, hdl2 = 0 ;
	//unsigned char file_buff[this->file_buff_size] = {0} ;
	INT64 write_total_bytes = 0 ;
	INT nWriteCnt = 0,  write_buff_size = 0;
	UINT8 * fptr = 0 ;
	CString tmpfile = _T(""), tmpfile_src = _T("");
	bool is_write_ok = false, is_rename_src = false ;

	
	if(this->IsConnected(false) == false)
	{
		flashUI->CallFunction(_T("<invoke name='FL_setDeviceConnection'><arguments><string>0</string></arguments></invoke>"));
		MessageBox(ownerWindow->m_hWnd, _T("文件操作出错，设备未连接！"), _T("错误"), MB_OK|MB_ICONSTOP );
		goto lb_exit ; 
	}

	if(is_use_temp_flag == true)
	{
		tmpfile = target_file_name;
		tmpfile.Append(_T(".tmp"));
		tmpfile_src = target_file_name;
		tmpfile_src.Append(_T(".bak"));
	}
	else
	{
		tmpfile = target_file_name;
	}
	
	if(stream_buff != 0)
	{

		hdl = fsOpenFile((CHAR *)(LPCWSTR)tmpfile, NULL, O_CREATE | O_TRUNC);
		if(hdl < 0)	{ goto lb_exit ; }

		write_total_bytes = buff_len;
		fptr = (UINT8*)stream_buff;
		while(1)
		{
			write_buff_size = (write_total_bytes >= this->file_buff_size) ? this->file_buff_size : write_total_bytes ;
			hdl2 = fsWriteFile(hdl, fptr, write_buff_size, &nWriteCnt);
			if(hdl2 != FS_OK)
			{
				MessageBox(ownerWindow->m_hWnd, _T("往设备写文件出错，可能磁盘已满"), _T("错误"), MB_OK|MB_ICONSTOP );
				break ;
			}
			write_total_bytes -= nWriteCnt ;
			if(write_total_bytes < 1)
			{
				is_write_ok = true ;
				break ; 
			} 
			fptr = (UINT8*)((unsigned int)fptr + (size_t)nWriteCnt) ;
		}
		fsCloseFile(hdl);
		hdl = 0 ;
		Sleep(500);
		if(is_write_ok == false)	{ goto lb_exit ; }
	}

	
	//如果启用临时文件机制， 还需要下面的处理
	if(is_use_temp_flag == true)
	{
		if(this->IsConnected(false) == false)		{ goto lb_exit ; }
		//如果目标文件以前存在， 先将目标文件重命名， 加上后缀.bak
		if(this->IsDeviceFileExist(target_file_name) == true)
		{
			int err = fsDeleteFile((CHAR*)(LPCWSTR)tmpfile_src, NULL);
			err = fsRenameFile((CHAR*)(LPCWSTR)target_file_name, NULL, (CHAR*)(LPCWSTR)tmpfile_src, NULL, FALSE);
			if(err == FS_OK)
			{
				//目标文件备份成功， 下面操作成功后， 需要删除该bak文件
				is_rename_src = true ;
			}
			else
			{
				//重命名目标文件失败， 删除tmp文件， 提前跳转到退出标签
				fsDeleteFile((CHAR*)(LPCWSTR)tmpfile, NULL);
				goto lb_exit ;
			}
		}
		//将临时文件更名为目标文件
		if(fsRenameFile((CHAR*)(LPCWSTR)tmpfile, NULL, (CHAR*)(LPCWSTR)target_file_name, NULL, FALSE) == FS_OK)
		{	
			//更名操作成功
			if(is_rename_src == true)
			{
				//如果目标文件已经备份， 删除它
				fsDeleteFile((CHAR*)(LPCWSTR)tmpfile_src, NULL);
			}
		}
		else
		{
			//更名操作失败
			if(is_rename_src == true)
			{
				//如果目标文件已经备份， 恢复
				fsRenameFile((CHAR*)(LPCWSTR)tmpfile_src, NULL, (CHAR*)(LPCWSTR)target_file_name, NULL, FALSE);
			}
			//删除创建的临时文件
			fsDeleteFile((CHAR*)(LPCWSTR)tmpfile, NULL);

			goto lb_exit ;
		}
	}

	//代码能执行到这里， 表示文件操作成功， 返回新文件的大小
	hdl = fsOpenFile((CHAR *)(LPCWSTR)target_file_name, NULL, O_RDWR);
	if(hdl >= 0)
	{
		rtv = fsGetFileSize(hdl);
		fsCloseFile(hdl);
		hdl = 0 ;
	}

lb_exit:
	
	if(hdl > 0)	{ fsCloseFile(hdl); }

	return rtv;
}
void ApiController::ScanUsbDisk()
{
	std::map <std::basic_string<TCHAR>, PDISK_T *> &usbDisks = (((CKidPadToolApp *)AfxGetApp ())->m_usbDisks);
	// Find out USB disks that have been attached.
	UINT32 usbDiskMask = UsbDiskDriver::scan ();
	if(usbDiskMask == 0)	{ return ; } 
	// For each scanned USB disk, add it to list if it meets some rule.
	for (int i = 0; i < 26; i ++)
	{
		if (usbDiskMask & (1 << i))
		{
			TCHAR diskDriveNameBuf[] = {_T('A') + i, _T(':'), 0};

#ifdef	_DEMO_USE_STDUSBDISKDRIVER_
			UsbDiskDriver *usbDiskDriver = new StdUsbDiskDriver;
#else
			UsbDiskDriver *usbDiskDriver = new NvtOffsetUsbDiskDriver();
#endif

			std::auto_ptr<UsbDiskDriver> scopedUsbDiskDriver (usbDiskDriver);

			usbDiskDriver->onDriveAttach (diskDriveNameBuf);
			if (usbDiskDriver->onLineCheck() == FALSE)
			{
				continue;
			}
			// A defined USB disk has been found. Call filesystem to parse it.
			PDISK_T *usbDisk = new PDISK_T();
			std::auto_ptr<PDISK_T> scopedUsbDisk (usbDisk);

			DWORD usbDiskSize = usbDiskDriver->getDiskSize ();

			memset (usbDisk, 0x00, sizeof (PDISK_T));
			usbDisk->nDiskType = DISK_TYPE_SMART_MEDIA;
			usbDisk->uTotalSectorN = usbDiskSize / 512;
			usbDisk->nSectorSize = 512;
			usbDisk->uDiskSize = usbDiskSize;
			usbDisk->ptDriver = usbDiskDriver;
			usbDisk->pvPrivate = usbDiskDriver;

			if (fsPhysicalDiskConnected (usbDisk))
			{
				// Error here.
				TRACE("usb已连接\r\n");
				//continue;
			}

			if(UpdateList2(usbDisk, diskDriveNameBuf) == true)
			{
				usbDisks[diskDriveNameBuf] = usbDisk;
				scopedUsbDisk.release();
				scopedUsbDiskDriver.release();
				break;
			}
			else
			{
				usbDiskDriver->onDriveDetach() ;
				delete usbDiskDriver;
				//delete usbDisk;
				scopedUsbDisk.release();
				scopedUsbDiskDriver.release();
			}
			::OutputDebugStringW(_T("diskDriveNameBuf--"));
			::OutputDebugStringW(diskDriveNameBuf);
			::OutputDebugStringW(_T("\r\n"));
			// List available logical disks according to filesystem's parsing.
		}
	}

	CheckClientOS();

	return ;
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

	//if(this->IsConnected(false) == false)
	//{
	//	this->ScanUsbDisk();
	//}
	return ret;
}

void ApiController::DispatchFlashCommand(CString command, CString args)
{
	//if (command == "loaded")
	if(command.Compare(_T("loaded")) == 0 )
	{
		Initialize();
	}
}

void ApiController::DispatchFlashCall(const char* request, const char* args)
{
	BSTR b = _com_util::ConvertStringToBSTR(args);
	CString str(b);
	if(b !=0 )	{ SysFreeString(b) ;}
	
	//::OutputDebugStringA(request);
	//::OutputDebugStringA("\r\n");
	
	if (strcmp(request, "F2C_getDeviceIconBase64") == 0) {
		CString cs("<string>");
		cs += GetDeviceIconBase64(str);
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
	else if (strcmp(request, "F2C_deleteDeviceApp") == 0) {
		DeleteDeviceApp(str);
	}
	else if (strcmp(request, "F2C_installApp") == 0) {
		InstallApp(str);
	}
	else if (strcmp(request, "F2C_installTransferApp") == 0) {
		TransferApp(str);
	}
	else if (strcmp(request, "F2C_insertAppNode") == 0) {
		CString cs = _T("<string>");
		cs += InsertAppNode(str);
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
		
	}
	else if (strcmp(request, "F2C_saveFileFromBase64") == 0) {
		CString cs("<string>");
		CString filePath = str.Left(str.Find(','));
		CString base64Str = str.Right(str.GetLength() - str.Find(',') - 1);
		if (SaveFileFromBase64(base64Str, filePath))
			cs += CString("1");
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_cancelDownload") == 0) {
		CancelDownload(str);
	}
	else if (strcmp(request, "F2C_deleteAppOnPc") == 0) {
		::DeleteFile(m_downloadDirectory + _T('\\') + str + _T(".npk"));
		::DeleteFile(m_downloadDirectory + _T('\\') + str + _T(".png"));
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
		CString cs("<string>");
		if (UpdateFirmware(str))
			cs += CString("1");
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_TRACE") == 0) {
		::OutputDebugString((LPCWSTR)str);
		::OutputDebugString(_T("\r\n"));
	}
	else if (strcmp(request, "F2C_getLocalMediaPath") == 0) {
		CString cs("<string>");
		cs += BrowsePC();
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getLocalPictures") == 0) {
		CString extension("gif,jpg,jpeg,png,bmp");
		CString mediatype("picture");

		GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
		memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
		_tcscat(tpb->parameter1, str.GetBuffer(str.GetLength()));
		_tcscat(tpb->parameter2, extension.GetBuffer(extension.GetLength()));
		_tcscat(tpb->parameter3, mediatype.GetBuffer(mediatype.GetLength()));
		tpb->flash_pointer = flashUI;
		_beginthread(threadGetLocalMedia, 0, (void *)tpb);
	}
	else if (strcmp(request, "F2C_getLocalMusics") == 0) {
		CString extension("mp3,wav,wma,m4a");
		CString mediatype("music");

		GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
		memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
		_tcscat(tpb->parameter1, str.GetBuffer(str.GetLength()));
		_tcscat(tpb->parameter2, extension.GetBuffer(extension.GetLength()));
		_tcscat(tpb->parameter3, mediatype.GetBuffer(mediatype.GetLength()));
		tpb->flash_pointer = flashUI;
		_beginthread(threadGetLocalMedia, 0, (void *)tpb);
	}
	else if (strcmp(request, "F2C_getLocalVideos") == 0) {
		CString extension("mp4,avi,flv,asf,mpg,vob");
		CString mediatype("video");

		GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
		memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
		_tcscat(tpb->parameter1, str.GetBuffer(str.GetLength()));
		_tcscat(tpb->parameter2, extension.GetBuffer(extension.GetLength()));
		_tcscat(tpb->parameter3, mediatype.GetBuffer(mediatype.GetLength()));
		tpb->flash_pointer = flashUI;
		_beginthread(threadGetLocalMedia, 0, (void *)tpb);
	}
	else if (strcmp(request, "F2C_convertVideo") == 0) {
		ConvertVideo(str);
	}
	else if (strcmp(request, "F2C_convertMusic") == 0) {
		ConvertMusic(str);
	}
	else if (strcmp(request, "F2C_convertPicture") == 0) {
		ConvertPicture(str);
	}
	else if (strcmp(request, "F2C_transferMusic2Device") == 0) {
		CString sourceFile = str;
		sourceFile = m_mediaDirectory4Music + sourceFile + _T(".mp3");
		CString targetDirectory = m_driveNANDName + _T("\\media\\musics");
		Transfer2Device(sourceFile, targetDirectory);
	}
	else if (strcmp(request, "F2C_transferVideo2Device") == 0) {
		CString sourceFile = str;
		sourceFile = m_mediaDirectory4Video + sourceFile + _T(".flv");
		CString targetDirectory = m_driveNANDName + _T("\\media\\videos");
		Transfer2Device(sourceFile, targetDirectory);
	}
	else if (strcmp(request, "F2C_transferPicture2Device") == 0) {
		CString sourceFile = str;
		sourceFile = m_mediaDirectory4Picture + sourceFile + _T(".jpg");
		CString targetDirectory = m_driveNANDName + _T("\\media\\pictures");
		Transfer2Device(sourceFile, targetDirectory);
	}
	else if (strcmp(request, "F2C_getDeviceVideos") == 0) {
		CString cs("<string>");
		
		CString xmlfile = m_driveNANDName + _T("\\media\\videoList.xml");
		CString directory = m_driveNANDName + _T("\\media\\videos\\");
		cs += GetDeviceMedia(xmlfile, directory, FALSE);
#ifdef _BYM
		xmlfile = m_driveNANDName + _T("\\media\\videoList_bbxly_xwxggfp.xml");
		cs += _T(",") + GetDeviceMedia(xmlfile, directory, TRUE);

		xmlfile = m_driveNANDName + _T("\\media\\videoList_xhzdl_czzdp.xml");
		cs += _T(",") + GetDeviceMedia(xmlfile, directory, TRUE);
#endif

		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDeviceMusics") == 0) {
		CString xmlfile = m_driveNANDName + _T("\\media\\musicList.xml");
		CString directory = m_driveNANDName + _T("\\media\\musics\\");
		CString cs("<string>");
		cs += GetDeviceMedia(xmlfile, directory, FALSE);
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDevicePictures") == 0) {
		CString xmlfile = m_driveNANDName + _T("\\media\\pictureList.xml");
		CString directory = m_driveNANDName + _T("\\media\\pictures\\");
		CString cs("<string>");
		cs += GetDeviceMedia(xmlfile, directory, FALSE);
		cs += CString("</string>");
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getDeviceApps") == 0) {
		CString xmlfile = m_driveNANDName + _T("\\builtIn\\appList.xml");
		CString cs(_T("<string>"));
		cs += GetDeviceApps(xmlfile);
		cs += CString(_T("</string>"));

		
		flashUI->SetReturnValue(cs);
	}
	else if (strcmp(request, "F2C_insertMediaNode") == 0) {
		CString mediaType = str.Left(str.Find(_T(',')));
		CString filename = str.Right(str.GetLength() - str.Find(_T(',')) - 1);

		CString xmlfile = m_driveNANDName + _T("\\media\\") + mediaType + _T("List.xml");

		CString cs("<string>");
		cs += AppendMediaNodeToXml(xmlfile, filename, mediaType);
		cs += CString("</string>");
		::OutputDebugStringW((LPCWSTR)cs);
		::OutputDebugStringW(_T("\r\n"));
		flashUI->SetReturnValue((LPCTSTR)cs);
		//DispatchFlashCall("F2C_getDevicePictures", "");
		
	}
	else if (strcmp(request, "F2C_deleteDeviceMedia") == 0) {
		CString type = str.Left(str.Find(_T(',')));
		CString xmlFile = m_driveNANDName + _T("\\media\\") + type + _T("List.xml");
		CString mediaFile = str.Right(str.GetLength() - str.Find(_T(',')) - 1);
		if (mediaFile.Find(_T(',')) > -1)
		{
			xmlFile = mediaFile.Right(mediaFile.GetLength() - mediaFile.Find(_T(',')) - 1);
			mediaFile = mediaFile.Left(mediaFile.Find(_T(',')));
		}
		
		DeleteDeviceMedia(xmlFile, mediaFile);
	}
	else if (strcmp(request, "F2C_deleteLocalApp") == 0) {
		::DeleteFile(m_downloadDirectory + str + _T(".npk"));
		::DeleteFile(m_downloadDirectory + str + _T(".png"));
	}
	else if (strcmp(request, "F2C_updateVolumnStatus") == 0) {
		UINT32 blockSize, freeSize, diskSize;
		if (fsDiskFreeSpace(m_driveNANDName[0], &blockSize, &freeSize, &diskSize) == FS_OK){
			CString request;
			request.Format(_T("<invoke name='FL_setDiskVolumnStatus'><arguments><string>%i,%i</string></arguments></invoke>"), (INT)freeSize/1024, (INT)diskSize/1024);
			flashUI->CallFunction(request);
		}
	}
	else if (strcmp(request, "F2C_getCategoryStats") == 0) {
		CString cs = GetCategoryStats();
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_getAppStats") == 0) {
		CString cs = GetAppStats();
		flashUI->SetReturnValue((LPCTSTR)cs);
	}
	else if (strcmp(request, "F2C_diagnose") == 0) {
		Diagnose();
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
			// TODO: install
		}
		finder.Close();
	}
	return TRUE;
}

CString ApiController::GetDeviceFileContent(CString filePath)
{
	int hdl = 0, hdl2 = 0;
	INT nBytes = 0 , nReadCnt = 0 ;
	CString rtv = _T("");
	CString * p1 = 0 ;
	
	if(this->IsConnected(false) == false)	{ goto lb_exit ; }
	
	hdl = fsOpenFile((CHAR *)(LPCWSTR)filePath, NULL, O_RDONLY);
	if(hdl < 0)	{ goto lb_exit ; }

	nBytes = (INT)fsGetFileSize(hdl);
	if(nBytes < 1)	{ goto lb_exit; }

	if (m_pu8xmlBuffer != NULL)
	{
		delete [] m_pu8xmlBuffer;
		m_pu8xmlBuffer = NULL;
	}

	m_pu8xmlBuffer = new char[nBytes + 1];
	memset(m_pu8xmlBuffer, 0x00, nBytes+1);
	
	// may need error handle here
	hdl2 = fsReadFile(hdl, (UINT8*)m_pu8xmlBuffer, nBytes, &nReadCnt) ;
	if (  hdl2 != FS_OK)
	{
		nBytes = 0 ;
		delete [] m_pu8xmlBuffer;
		m_pu8xmlBuffer = NULL;
		goto lb_exit ;
	}

	p1 = new CString(b64::UTF8ToCString(m_pu8xmlBuffer, (unsigned int) nBytes));
	if( (p1 != 0) && (p1->GetAllocLength() > 0) )
	{
		rtv.Append(*p1);
	}
	else
	{
		nBytes = 0 ;
	}


lb_exit:

	if(hdl >= 0)	{ fsCloseFile(hdl); }
	if(p1 != 0)	{ delete p1 ; }

	return (nBytes>0) ? rtv : CString(_T("")) ;
	
}


CString ApiController::GetDeviceIconBase64(CString iconFilePath)
{
	int hdl = 0, hdl2 = 0;
	INT nBytes = 0 , nReadCnt = 0 ;
	CString rtv = _T("");
	CString * p1 = 0 ;
	UINT8 *pucBuff = 0, * fptr = 0 ;
	std::string encoded;
	unsigned char file_buff[this->file_buff_size] = {0} ;
	
	if(this->IsConnected(false) == false)	{ goto lb_exit ; }

	hdl = fsOpenFile((CHAR *)(LPCWSTR)iconFilePath, NULL, O_RDONLY);
	if(hdl < 0)	{ goto lb_exit ; }

	nBytes = (INT)fsGetFileSize(hdl);
	if(nBytes < 1)	{ goto lb_exit; }

	pucBuff = (UINT8 *)malloc(sizeof(UINT) * (nBytes+2));
	memset(pucBuff, 0, nBytes+2);

	// may need error handle here
	fptr = pucBuff;
	while(1)
	{
		if(this->IsConnected(false) == false)	{ goto lb_exit ; }

		memset(file_buff, 0, this->file_buff_size);
		hdl2 = fsReadFile(hdl, file_buff, this->file_buff_size, &nReadCnt);
		if(hdl2 != FS_OK)	{ break ; }
		memcpy(fptr, file_buff, (size_t)nReadCnt);
		if(this->file_buff_size != nReadCnt)	{ break ; }
		fptr = (UINT8*)((unsigned int)fptr + this->file_buff_size) ;
	}
	fsCloseFile(hdl);
	hdl = 0 ;

	base64_encode(encoded, reinterpret_cast<const unsigned char*>(pucBuff), nBytes);
	p1 = new CString(encoded.c_str());
	if( (p1 != 0) && (p1->GetAllocLength() > 0) )
	{
		rtv.Append(*p1);
	}
	else
	{
		nBytes = 0 ;
	}

lb_exit:

	if(hdl >= 0)	{ fsCloseFile(hdl); }
	if(p1 != 0)	{ delete p1 ; }
	if(pucBuff != 0) { free(pucBuff); }
	return (nBytes>0) ? rtv : NULL ;
}

void ApiController::DeleteDeviceApp(CString appName)
{
	if(this->IsConnected(false) == false)
	{
		MessageBox(ownerWindow->m_hWnd, _T("请先连接上设备， 然后重新尝试本次操作！\""), _T("删除错误"), MB_OK|MB_ICONSTOP);
		return ;
	}

	CString appListXml = m_driveNANDName + _T("\\builtIn\\appList.xml");
	CString appDirectory = m_driveNANDName + _T("\\book\\apps\\") + appName;

	// delete files from device
	if (DeleteDirectoryOnDevice(appDirectory) == FALSE) {
		MessageBox(ownerWindow->m_hWnd, _T("删除设备上内容\"") + appName + _T("\"失败"), _T("删除错误"), MB_OK|MB_ICONSTOP);
		return;
	}

	// update xml on device
	if(this->IsConnected(false) == false)	{ return ; }
	DeleteAppNodeOnDeviceXml(appListXml, appName);
}

BOOL ApiController::DeleteDirectoryOnDevice(CString directory)
{
	if(this->IsConnected(false) == false)	{ return FALSE ; }

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
		if (CString((LPCWSTR)fileFind.suLongName) == _T("."))	{ continue; }
		if (CString((LPCWSTR)fileFind.suLongName) == _T(".."))	{ continue; }

		tempName = directory + _T('\\') + (LPCWSTR)fileFind.suLongName;

		if (fileFind.ucAttrib & A_DIR)
		{
			if (DeleteDirectoryOnDevice(tempName) == FALSE)
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
	
	if(this->IsConnected(false) == false)	{ return FALSE;}

	err = fsRemoveDirectory((CHAR *)(LPCWSTR)directory, NULL);
	if (err != FS_OK)
	{
		MessageBox(ownerWindow->m_hWnd, __T("删除设备上的\"")+directory+_T("\"文件夹失败"), _T("错误"), MB_OK|MB_ICONSTOP );
		return FALSE;
	}
	return TRUE;
}

BOOL ApiController::DeleteAppNodeOnDeviceXml(CString appListXml, CString appName)
{
	BOOL rtv = FALSE ;

	CString xml = _T(""), name = _T("");
	CString tmp_fn = _T("");
	TiXmlDocument doc;
	TiXmlHandle docHandle(&doc);
	BOOL found = FALSE;
	TiXmlPrinter printer;
	TiXmlElement *appElement = 0 ;

	int write_total_bytes = 0 ;
	const char* ret = 0 ;

	xml = GetDeviceFileContent(appListXml);
	doc.Parse(m_pu8xmlBuffer, 0, TIXML_ENCODING_UTF8);
	
	appElement = docHandle.FirstChild("appList").Child("app", 0).ToElement();
	for (appElement; appElement; appElement=appElement->NextSiblingElement())
	{
		name = CA2CT(appElement->FirstChild("name")->ToElement()->GetText(), CP_UTF8);
		if (name == appName)
		{
			found = TRUE;
			break;
		}
	}
	
	if (found == FALSE)	{ goto lb_exit ; }

	if(doc.RootElement()->RemoveChild(appElement) == false)	{ goto lb_exit ; }
	if(doc.Accept(&printer) == false)	{ goto lb_exit ; }
	ret = printer.CStr();
	write_total_bytes = strlen(ret);

	this->WriteStream2DeviceFile(appListXml, (void*)ret, write_total_bytes, true);

lb_exit:
	

	return rtv;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApiController::InstallApp(CString appName)
{
	if (IsApplicationInstalled(appName) == TRUE)
	{
		MessageBox(ownerWindow->m_hWnd, _T("内容\"") + appName + _T("\"已经安装在设备上"), _T("信息"), MB_OK|MB_ICONSTOP);
		flashUI->CallFunction(_T("<invoke name='FL_duplicateInstall'><arguments><string>") + appName + _T("</string></arguments></invoke>"));
		return;
	}

	CString tempFolder = m_downloadDirectory + appName;
	::CreateDirectory(tempFolder, NULL);
	CString npkFile = m_downloadDirectory + appName + _T(".npk");

	CmdAppParam * cap = (CmdAppParam*)malloc(sizeof(CmdAppParam));
	cap->cmd_str = (TCHAR*)malloc(2048);
	memset(cap->cmd_str, 0, 2048);
	_tcscat(cap->cmd_str, _T("\"") + m_workingFolderName + _T("\\7za.exe\" x \"") + npkFile + _T("\" -o\"") + tempFolder + _T("\" -y"));
	cap->fn = ExtractZipCmd;
	RunCmdApp(cap);

	return ;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApiController::TransferApp(CString appName)
{
	CString tempFolder = m_downloadDirectory + appName;
	CString installXml = tempFolder + _T("\\install.xml");
	CString appListXml = m_driveNANDName + _T("\\builtIn\\appList.xml");

	GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
	memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
	_tcscat(tpb->parameter1, tempFolder);
	_tcscat(tpb->parameter2, m_driveNANDName + _T("\\book\\apps\\") + appName);
	tpb->flash_pointer = flashUI;
	_beginthread(threadCopyPcDirectoryToDevice, 0, (void *)tpb);

	return ;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char * __cdecl WstrToUTF8(const TCHAR * buff_src)
{
	char * buff = 0 ;

	int buff_size = ::WideCharToMultiByte(CP_UTF8, NULL, (wchar_t*)buff_src, -1, (char*)buff, 0, NULL, NULL);
	if(buff_size>0)
	{
		buff = (char*)malloc(buff_size + 4);
		if(::WideCharToMultiByte(CP_UTF8, NULL, (wchar_t*)buff_src, -1, (char*)buff, buff_size, NULL, NULL)< 1)
		{
			free(buff); 
			buff = 0;
		}
	}
	return buff ;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString ApiController::InsertAppNode(CString appName)
{
	
	CString rtv = _T("");
	CString tempFolder = m_downloadDirectory + appName;
	CString installXml = tempFolder + _T("\\install.xml");
	CString appListXml = m_driveNANDName + _T("\\builtIn\\appList.xml");

	TiXmlDocument doc;
	char sz[256] = {0};
	WideCharToMultiByte(CP_ACP, 0, installXml, -1, sz, 256, NULL, NULL);
	doc.LoadFile(sz, TIXML_ENCODING_UTF8);
	TiXmlHandle docHandle(&doc);
	TiXmlElement *pElement = docHandle.FirstChild("install").Child("program", 0).ToElement();
	const char *aName = pElement->FirstChild("name")->ToElement()->GetText();

	// new app node
	TiXmlElement* app = new TiXmlElement("app");

	TiXmlElement* name = new TiXmlElement("name");
	name->LinkEndChild(new TiXmlText(aName));
	app->LinkEndChild(name);

	TiXmlElement* icon = new TiXmlElement("icon");
	char str[80] = {0};
	if (chip_version == _T("95")) {
		strcpy(str, "book/");
		strcat(str, "apps/");
	}
	else {
		strcpy(str, "apps/");
	}
	strcat(str, aName);
	strcat(str, "/");
	strcat(str, pElement->FirstChild("icon")->ToElement()->GetText());
	icon->LinkEndChild(new TiXmlText(str));
	app->LinkEndChild(icon);

	CString iconStr = CA2CT(icon->GetText(), CP_UTF8);

	TiXmlElement* type = new TiXmlElement("type");
	type->LinkEndChild(new TiXmlText("flash"));
	app->LinkEndChild(type);

	TiXmlElement* entry = new TiXmlElement("entry");
	memset(str, 0, 80);
	if (chip_version == _T("95")) {
		strcpy(str, "book/");
		strcat(str, "apps/");
	}
	else {
		strcpy(str, "apps/");
	}
	strcat(str, aName);
	strcat(str, "/body/");
	strcat(str, pElement->FirstChild("entry")->ToElement()->GetText());
	entry->LinkEndChild(new TiXmlText(str));
	app->LinkEndChild(entry);

	TiXmlElement* category = new TiXmlElement("category");
	category->LinkEndChild(new TiXmlText(pElement->FirstChild("category")->ToElement()->GetText()));
	app->LinkEndChild(category);

	TiXmlElement* deploy = new TiXmlElement("deployType");
	deploy->LinkEndChild(new TiXmlText("book"));
	app->LinkEndChild(deploy);

	TiXmlElement* security = new TiXmlElement("security");
	app->LinkEndChild(security);

	TiXmlElement* image = new TiXmlElement("image");
	memset(str, 0, 80);
	if (chip_version == _T("95")) {
		strcpy(str, "book/");
		strcat(str, "apps/");
	}
	else {
		strcpy(str, "apps/");
	}
	strcat(str, aName);
	strcat(str, "/");
	strcat(str, pElement->FirstChild("security")->FirstChild("image")->ToElement()->GetText());
	image->LinkEndChild(new TiXmlText(str));
	security->LinkEndChild(image);

	TiXmlElement* passwd = new TiXmlElement("passwd");
	passwd->LinkEndChild(new TiXmlText("temppass.bin"));
	security->LinkEndChild(passwd);

	TiXmlElement* mount = new TiXmlElement("mountPoint");
	memset(str, 0, 80);
	if (chip_version == _T("95")) {
		strcpy(str, "book/");
		strcat(str, "apps/");
	}
	else {
		strcpy(str, "apps/");
	}
	strcat(str, aName);
	strcat(str, "/body/");
	mount->LinkEndChild(new TiXmlText(str));
	security->LinkEndChild(mount);

	// add xml node from program struct
	GetDeviceFileContent(appListXml);
	TiXmlDocument doc2;
	doc2.Parse(m_pu8xmlBuffer, NULL, TIXML_ENCODING_UTF8);
	doc2.RootElement()->LinkEndChild(app);
	TiXmlPrinter printer;
	doc2.Accept(&printer);
	const char* ret = printer.CStr();


	/*if(name != 0)	{ delete name ; }
	if(icon != 0)	{ delete icon ; }
	if(type != 0)	{ delete type ; }
	if(entry != 0)	{ delete entry ; }
	if(category != 0)	{ delete category ; }

	if(deploy != 0)	{ delete deploy ; }
	if(security != 0)	{ delete security ; }
	if(image != 0)	{ delete image ; }
	if(passwd != 0)	{ delete passwd ; }
	if(mount != 0)	{ delete mount ; }
	if(app != 0)	{ delete app ; }*/

	if(this->WriteStream2DeviceFile(appListXml, (void*)ret, strlen(ret), true) > 0)
	{
		// delete temp
		DeletePcDirectory(tempFolder);
		::RemoveDirectory(tempFolder);
	}

	

	return appName + _T("#") + iconStr;
}

BOOL ApiController::IsApplicationInstalled(CString appName)
{
	BOOL rtv = FALSE ;
	CString xmlFile = _T(""), xml = _T(""), ret_value = _T(""), name = _T("");
	TiXmlDocument doc;
	TiXmlHandle docHandle(&doc);
	TiXmlElement *appElement = 0 ;

	xmlFile = m_driveNANDName + _T("\\builtIn\\appList.xml");
	xml = GetDeviceFileContent(xmlFile);
	if(xml.GetLength() < 1)	{ goto lb_exit ; }
	doc.Parse(m_pu8xmlBuffer);

	appElement = docHandle.FirstChild("appList").Child("app", 0).ToElement();
	for (appElement; appElement; appElement=appElement->NextSiblingElement())
	{
		name = CA2CT(appElement->FirstChild("name")->ToElement()->GetText(), CP_UTF8);
		if (name == appName)
		{
			rtv = TRUE ;
			break ;
		}
	}


lb_exit:

	return rtv;
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

BOOL ApiController::SaveFileFromBase64(CString base64String, CString filePath)
{
	//base64String = GetBase64DataFromTextFile(_T("C:\\pic_2.txt"));
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
		FILE_STAT_T stat;
		CString s = m_driveNANDName + _T("\\builtIn\\ui.xml");
		INT nStatus = fsGetFileStatus(-1, (CHAR *)(LPCWSTR)s, NULL, &stat);
		if (nStatus == 0)
		{
			CString cs = GetDeviceFileContent(s);
			TiXmlDocument doc;
			doc.Parse(m_pu8xmlBuffer);
			TiXmlHandle docHandle(&doc);
			TiXmlElement *verElem = docHandle.FirstChild("ui").Child("version", 0).ToElement();
			TiXmlElement *osElem = docHandle.FirstChild("ui").Child("os_platform", 0).ToElement();
			TiXmlElement *chipElem = docHandle.FirstChild("ui").Child("chip_version", 0).ToElement();

			if (osElem)
				os_platform = CA2CT(osElem->GetText(), CP_UTF8);
			if (chipElem)
				chip_version = CA2CT(chipElem->GetText(), CP_UTF8);

			if (verElem)
				return CA2CT(verElem->GetText(), CP_UTF8);
		}
	}
	return _T("1.0");
}

void ApiController::CheckClientOS()
{
	GetFirmwareVersion();

	if (this->IsConnected(false))
	{
		// check target client os
		BOOL checked = FALSE;
	#ifdef _BYM
		checked = TRUE;
		if (os_platform != _T("bym")){
			MessageBox(ownerWindow->m_hWnd, _T("本程序只能连接儿童学习机，即将关闭"), _T("Error"), MB_OK|MB_ICONSTOP );
			exit(0);
		}
	#endif
	#ifdef _XBW
		checked = TRUE;
		/*if (os_platform != _T("xbw")){
			MessageBox(ownerWindow->m_hWnd, _T("本程序只能连接小霸王学习机，即将关闭"), _T("Error"), MB_OK|MB_ICONSTOP );
			exit(0);
		}*/
	#endif
	#ifdef _XXM
		checked = TRUE;
		if (os_platform != _T("xxm")){
			MessageBox(ownerWindow->m_hWnd, _T("本程序只能连接童伴熊小米，即将关闭"), _T("Error"), MB_OK|MB_ICONSTOP );
			exit(0);
		}
	#endif
	
		if (checked == FALSE && os_platform != _T("epad")){
			MessageBox(ownerWindow->m_hWnd, _T("本程序只能连接E巧派，即将关闭"), _T("Error"), MB_OK|MB_ICONSTOP );
			exit(0);
		}
	}
}

BOOL ApiController::UpdateFirmware(CString zipFilePath)
{
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

	::DeleteFile(zipFilePath);


	// backup appList.xml
	if (!CopyDeviceFile2PC(m_driveNANDName+_T("\\builtIn\\appList.xml"), m_downloadDirectory+_T("builtIn\\appList.xml")))
		return FALSE;

	// make sure temp folder builtIn2/base2 not existing
	DeleteDirectoryOnDevice(m_driveNANDName + _T("\\builtIn2"));
	DeleteDirectoryOnDevice(m_driveNANDName + _T("\\base2"));

	// copy to device builtIn2/base2
	GENERAL_PARAMETER_BLOCK *tpb = (GENERAL_PARAMETER_BLOCK *)malloc(sizeof(GENERAL_PARAMETER_BLOCK));
	memset(tpb, 0, sizeof(GENERAL_PARAMETER_BLOCK));
	_tcscat(tpb->parameter1, m_downloadDirectory);
	_tcscat(tpb->parameter2, m_driveNANDName + _T("\\"));
	tpb->flash_pointer = flashUI;
	_beginthread(threadInstallBuiltIn, 0, (void *)tpb);

	return TRUE;
}

BOOL ApiController::CopyDeviceFile2PC(CString deviceFile, CString pcFile)
{
	int err = 0;
	int err2 = 0;
	err = fsOpenFile( (CHAR *)(LPCWSTR)deviceFile, NULL, O_RDONLY );
	if ( err < 0 )
	{
		MessageBox(ownerWindow->m_hWnd, _T("打开设备上文件\"") + deviceFile + _T("\"失败"), _T("Error"), MB_OK|MB_ICONSTOP );
		return FALSE;
	}
	INT nBytes = (INT)fsGetFileSize( err );

	CFile ff;
	unsigned char pu8DeviceFileBuffer[4096];
	try
	{
		INT nReadCnt;
		ff.Open(pcFile, CFile::modeReadWrite | CFile::modeCreate );
		while ( nBytes )
		{
			if ( (err2 = fsReadFile( err, (UINT8*)pu8DeviceFileBuffer, sizeof(pu8DeviceFileBuffer), &nReadCnt )) != FS_OK )
			{
				err2 = fsCloseFile( err );
				MessageBox(ownerWindow->m_hWnd, _T("读取设备上文件\"") + deviceFile + _T("\"失败"), _T("Error"), MB_OK|MB_ICONSTOP );
				return FALSE;
			}
			ff.Write( pu8DeviceFileBuffer, nReadCnt );
			nBytes -= nReadCnt;
		}
		if ( (err2 = fsCloseFile( err )) != FS_OK )
		{
			ff.Close();
			MessageBox(ownerWindow->m_hWnd, _T("关闭设备上文件\"") + deviceFile + _T("\"失败"), _T("Error"), MB_OK|MB_ICONSTOP );
			return FALSE;
		}
		ff.Close();
	}
	catch ( CFileException *pEx )
	{
		if ( (err2 = fsCloseFile( err )) != FS_OK )
		{
			MessageBox(ownerWindow->m_hWnd, _T("关闭设备上文件\"") + deviceFile + _T("\"失败"), _T("Error"), MB_OK|MB_ICONSTOP );
		}
		ff.Close();
		MessageBox(ownerWindow->m_hWnd, pEx->m_strFileName, _T("Open PC failed"), MB_OK|MB_ICONSTOP );
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString ApiController::GetDeviceApps(CString xmlFile)
{
	CString xml = GetDeviceFileContent(xmlFile);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);
	TiXmlHandle docHandle(&doc);

	CString ret_value= _T(""), name = _T(""), icon = _T("");
	TiXmlElement *appElement = docHandle.FirstChild("appList").Child("app", 0).ToElement();
	for (appElement; appElement; appElement=appElement->NextSiblingElement())
	{
		if (strstr(appElement->FirstChild("entry")->ToElement()->GetText(), "apps/"))
		{
			name = CA2CT(appElement->FirstChild("name")->ToElement()->GetText(), CP_UTF8);
			icon = CA2CT(appElement->FirstChild("icon")->ToElement()->GetText(), CP_UTF8);
			if (chip_version == _T("95"))
				icon = icon.Right(icon.GetLength() - 5);
	
			ret_value += name;
			ret_value += _T("#");
			ret_value += icon;
			ret_value += _T(",");
		}
	}

	if (ret_value.GetLength() > 0)
	{
		ret_value = ret_value.Left(ret_value.GetLength() - 1);
	}

	return ret_value;
}

CString ApiController::GetDeviceMedia(CString xmlFile, CString directory, BOOL isSubMedia)
{
	CString tempName = _T(""), ret_value = _T(""), file = _T("");
	CString entry = _T("entry");
	INT64 nBytes = 0 ;
	int hdl = 0 ;
	TCHAR fileSizeBuf[32] = {0};

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
	{
		return _T("");
	}
	
	if (directory.Find(_T("pictures")) > 0)
	{
		entry = _T("icon");
	}

	
	// may enhance here, looply found name and deploytype
	for (xElement = node->ToElement(); xElement; xElement = xElement->NextSiblingElement())
	{
		for (nameElement = xElement->FirstChildElement(); nameElement; nameElement = nameElement->NextSiblingElement())
		{
			tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
			if (tempName == entry)
			{
				tempName = CA2CT(nameElement->GetText(), CP_UTF8);
				file = tempName.Right(tempName.GetLength() - tempName.Find('/') - 1);
				file.Replace(_T('/'), _T('\\'));
				file = directory + file;
				
				nBytes = 0 ;
				hdl = fsOpenFile((CHAR *)(LPCWSTR)file, NULL, O_RDONLY);
				if(hdl > 0)
				{
					nBytes = fsGetFileSize(hdl);
					fsCloseFile(hdl);
				}
				hdl = 0 ;
				::_i64tot(nBytes, fileSizeBuf, 10);

				//_stprintf_s(fileSizeBuf, sizeof(fileSizeBuf) / sizeof(TCHAR), _T("%d"), nBytes);
				ret_value += file;
				ret_value += _T("#");
				ret_value += fileSizeBuf;
				
				if (isSubMedia)
				{
					ret_value += _T("#");
					ret_value += xmlFile;
				}

				ret_value += ",";

				break;
			}
		}
	}

	if (ret_value.GetLength() > 0)
	{
		ret_value = ret_value.Left(ret_value.GetLength() - 1);
	}

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

	CString cmd;
	cmd.Format(b64::ReadAllFromTextFile(m_workingFolderName + _T("\\convert_music.txt")), m_workingFolderName+_T("\\ffmpeg.exe"), filepath, target);
	_tcscat(cap->cmd_str, cmd);

	cap->fn = ConvertAVCmd;
	RunCmdAppForFFMpeg(cap);
}

void ApiController::ConvertVideo(CString filepath)
{
	CString target = filepath.Right(filepath.GetLength() - filepath.ReverseFind(_T('\\')) - 1);
	target = target.Left(target.ReverseFind(_T('.')));
	target = m_mediaDirectory4Video + target + _T(".flv");

	CmdAppParam * cap = (CmdAppParam*)malloc(sizeof(CmdAppParam));
	cap->cmd_str = (TCHAR*)malloc(2048);
	memset(cap->cmd_str, 0, 2048);

	CString cmd;
	cmd.Format(b64::ReadAllFromTextFile(m_workingFolderName + _T("\\convert_video.txt")), m_workingFolderName+_T("\\ffmpeg.exe"), filepath, target);
	_tcscat(cap->cmd_str, cmd);
	
	cap->fn = ConvertAVCmd;
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

		ULONG quality = 100;
		encoderParameters.Parameter[0].Value = &quality;
		image->Save(target, &encoderClsid, &encoderParameters);

		delete image;
		image = NULL;
	}

	GdiplusShutdown(gdiplusToken);

	flashUI->CallFunction(_T("<invoke name='FL_completeConvert'><arguments><string></string></arguments></invoke>"));

	return ;
}

CString ApiController::AppendMediaNodeToXml(CString xmlFile, CString filename, CString mediaType)
{
	CString ret_value = _T(""), filepath = _T(""), tmpfile = _T("");
	CString xml = _T(""), tempName = _T(""), directory = _T(""), extension = _T("");
	TiXmlDocument doc;
	TiXmlNode *node = 0;
	TiXmlElement *listElement = 0;
	TiXmlElement *xElement = 0;
	TiXmlElement *nameElement = 0;
	TiXmlElement *childElement = 0;

	TiXmlElement* media = 0;
	TiXmlElement* name = 0;
	TiXmlText* nameText = 0;
	TiXmlElement* icon = 0;

	TiXmlText* iconText = 0;
	TiXmlElement* type = 0 ;
	TiXmlText* typeText = 0;
	TiXmlElement* entry = 0;
	TiXmlText* entryText = 0;
	TiXmlElement* deployType = 0;
	TiXmlText* deployTypeText = 0;

	TiXmlElement *firstChild = 0 ;
	TiXmlPrinter printer;


	const char* ret =  0 ;
	int hdl = 0 ;
	TCHAR fileSizeBuf[32] = {0};
	INT64 nBytes = 0 ;
		

	if(this->IsConnected(false) == false)	{ goto lb_exit; }

	xml = GetDeviceFileContent(xmlFile);
	doc.Parse(m_pu8xmlBuffer);

	node = doc.RootElement();
	listElement = node->ToElement();

	// check duplicate
	node = doc.RootElement();
	listElement = node->ToElement();

	childElement = listElement->FirstChildElement();

	if (childElement)
	{
		tempName = _T("");
		for (xElement = childElement; xElement; xElement = xElement->NextSiblingElement())
		{
			nameElement = xElement->FirstChildElement();
			tempName = CString(CA2CT(nameElement->Value(), CP_UTF8));
			if (tempName == _T("name"))
			{
				tempName = CA2CT(nameElement->GetText(), CP_UTF8);
				if (tempName == filename)
				{
					goto lb_get_fsize ;
				}
			}
		}
	}
	
	directory = m_driveNANDName + _T("\\media\\") + mediaType + _T("s\\");
	if (mediaType == _T("video"))
		extension = _T(".flv");
	else if (mediaType == _T("music"))
		extension = _T(".mp3");
	else if (mediaType == _T("picture"))
		extension = _T(".jpg");


	media = new TiXmlElement(CW2A(mediaType));
	name = new TiXmlElement("name");
	nameText = new TiXmlText(CW2A(filename, CP_UTF8));
	icon = new TiXmlElement("icon");

	if (mediaType == _T("picture"))
	{
		iconText = new TiXmlText(CW2A(mediaType+"s/"+filename+extension, CP_UTF8));
	}
	else
	{
		iconText = new TiXmlText(CW2A(mediaType+"s/icon.png", CP_UTF8));
	}

	type = new TiXmlElement( "type" );
	typeText = new TiXmlText("flash");
	entry = new TiXmlElement( "entry" );
	entryText = new TiXmlText(CW2A(mediaType+"s/"+filename+extension, CP_UTF8));
	deployType = new TiXmlElement( "deployType" );
	deployTypeText = new TiXmlText("media");

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

	firstChild = doc.RootElement()->FirstChildElement();
	if (firstChild)
	{
		node->InsertBeforeChild(firstChild, *media);
	}
	else
	{
		node->InsertEndChild(*media);
	}
	if(doc.Accept(&printer) == false)	{ goto lb_exit ; }

	ret = printer.CStr();
	if(this->WriteStream2DeviceFile(xmlFile, (void*)ret, strlen(ret), true) == false)
	{
		goto lb_exit ;
	}

	
lb_get_fsize:

	filepath = directory + filename + extension;
	hdl = fsOpenFile((CHAR *)(LPCWSTR)filepath, NULL, O_RDWR);
	if(hdl < 0)	{ goto lb_exit ; }
	nBytes = fsGetFileSize(hdl);
	fsCloseFile(hdl);
	hdl = 0 ;
	
	memset(fileSizeBuf, 0, 32 * sizeof(TCHAR));
	::_i64tot(nBytes, fileSizeBuf, 10);
	
	ret_value = filepath ;
	ret_value.Append(_T("#"));
	ret_value.Append(fileSizeBuf);

lb_exit:

	
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////to be modified on 2012-10-28////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	if (found == FALSE)	{  return; }

	listElement->RemoveChild(xElement);

	TiXmlPrinter printer;
	doc.Accept(&printer);
	const char* ret = printer.CStr();
	if(ret == 0)	{ return ; }

	if(this->WriteStream2DeviceFile(xmlFile, (void*)ret, strlen(ret), true) > 0)
	{
		int err = fsDeleteFile((CHAR *)(LPCWSTR)mediaFile, NULL);
		if (err != FS_OK)
		{
			MessageBox(ownerWindow->m_hWnd, _T("删除设备上的\"")+mediaFile+_T("\"文件失败"), _T("错误"), MB_OK|MB_ICONSTOP );
		}
	}


	return ;
}

CString ApiController::GetCategoryStats()
{
	CString xmlfile = m_driveNANDName + _T("\\builtIn\\appList.xml");
	CString xml = GetDeviceFileContent(xmlfile);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);
	TiXmlHandle docHandle(&doc);

	int nGame=0, nApp=0, nVideo=0, nMusic=0, nBook=0;

	TiXmlElement *appElement = docHandle.FirstChild("appList").Child("app", 0).ToElement();
	for (appElement; appElement; appElement=appElement->NextSiblingElement())
	{
		if (strstr(appElement->FirstChild("entry")->ToElement()->GetText(), "apps/"))
		{
			if (strcmp(appElement->FirstChild("type")->ToElement()->GetText(), "directory") == 0)
			{
				//xmlfile = m_driveNANDName + _T("\\") + 
			}
			else
			{
				if (appElement->FirstChild("category") != NULL)
				{
					const char* category = appElement->FirstChild("category")->ToElement()->GetText();
					if (strcmp(category, "game") == 0)		nGame++;
					else if (strcmp(category, "app") == 0)	nApp++;
					else if (strcmp(category, "video") == 0)nVideo++;
					else if (strcmp(category, "music") == 0)nMusic++;
					else if (strcmp(category, "book") == 0)	nBook++;
				}
			}
		}
	}

	CString str;
	str.Format(L"{game:%d,app:%d,video:%d,music:%d,book:%d}", nGame, nApp, nVideo, nMusic, nBook);
	return str;
}

CString ApiController::GetAppStats()
{
	CString xmlfile = m_driveNANDName + _T("\\builtIn\\appList.xml");
	CString xml = GetDeviceFileContent(xmlfile);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);
	TiXmlHandle docHandle(&doc);

	CString retStr = _T("");

	TiXmlElement *appElement = docHandle.FirstChild("appList").Child("app", 0).ToElement();
	for (appElement; appElement; appElement=appElement->NextSiblingElement())
	{
		if (strstr(appElement->FirstChild("entry")->ToElement()->GetText(), "apps/"))
		{
			if (strcmp(appElement->FirstChild("type")->ToElement()->GetText(), "directory") == 0)
			{
				//xmlfile = m_driveNANDName + _T("\\") + 
			}
			else
			{
				if (appElement->FirstChild("openTimes") != NULL && appElement->FirstChild("useTime") != NULL)
				{
					retStr += CA2CT(appElement->FirstChild("name")->ToElement()->GetText(), CP_UTF8);
					retStr += _T("#");
					retStr += appElement->FirstChild("openTimes")->ToElement()->GetText();
					retStr += _T("#");
					retStr += appElement->FirstChild("useTime")->ToElement()->GetText();
					retStr += _T(",");
				}
			}
		}
	}

	if (retStr.GetLength() > 0)
		retStr = retStr.Left(retStr.GetLength() - 1);
	return retStr;
}

void ApiController::Diagnose()
{
	CString xmlfile = m_driveNANDName + _T("\\builtIn\\appList.xml");
	CString xml = GetDeviceFileContent(xmlfile);
	TiXmlDocument doc;
	doc.Parse(m_pu8xmlBuffer);
	TiXmlHandle docHandle(&doc);

	TiXmlElement *appElement = docHandle.FirstChild("appList").Child("app", 0).ToElement();
	int count = 0;
	for (appElement; appElement; appElement=appElement->NextSiblingElement())
	{
		if (strstr(appElement->FirstChild("entry")->ToElement()->GetText(), "apps/"))
		{
			/*
			const char *iconFile = CA2CT(appElement->FirstChild("icon")->ToElement()->GetText(), CP_UTF8);
			size_t len = strlen(iconFile);
			BSTR buff = _com_util::ConvertStringToBSTR(iconFile) ; 
			for (int i=0; i<len; i++)
			{
				//0x5c-->'\',  0x2F-->'/'   
				if(buff[i] == 0x2F)  { buff[i] = 0x5C ; }
			}
			
			CString str = _T("<invoke name='FL_reportDiagnosis'><arguments><string>");
			str += m_driveNANDName + _T("\\book\\");
			str += buff;
			str += _T("</string></arguments></invoke>");
			flashUI->CallFunction(str);
			
			SysFreeString(buff);
			*/

			CString iconFile = CA2CT(appElement->FirstChild("icon")->ToElement()->GetText(), CP_UTF8);
			int n = iconFile.Replace(_T("/"), _T("\\"));
			iconFile = m_driveNANDName + _T("\\book\\") + iconFile;
			
			INT hFile = fsOpenFile((CHAR *)(LPCWSTR)iconFile, NULL, O_RDONLY);
			if (hFile < 0)
			{
				CString name = CA2CT(appElement->FirstChild("name")->ToElement()->GetText(), CP_UTF8);
				DeleteDeviceApp(name);
				Sleep(1000);
				flashUI->CallFunction(_T("<invoke name='FL_reportDiagnosis'><arguments><string>发现有问题内容\"") + name + _T("\"，已删除</string></arguments></invoke>"));
				count++;
			}
			fsCloseFile(hFile);
		}
	}
	CString str;
	if (count > 0)
		str.Format(L"<invoke name='FL_completeDiagnose'><arguments><string>清除%d个问题内容</string></arguments></invoke>", count);
	else
		str.Format(L"<invoke name='FL_completeDiagnose'><arguments><string>没有发现问题内容</string></arguments></invoke>");
	flashUI->CallFunction(str);
}