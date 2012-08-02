// Nuvoton CCLi8 (2010.08.12)
#include <string>
#include <map>
#include "shockwaveflash1.h"
#include "tinyxml.h"


#define NAME_LENGTH 256

typedef struct program_s {
	char name[ NAME_LENGTH ];
    char icon[ NAME_LENGTH ];
	char type[7];
	char deployType[8];
	char entry[ NAME_LENGTH ];
	bool hasSecurity;
	char securityImage[ NAME_LENGTH ];
	char securityPassword[ NAME_LENGTH ];
	char bgColor[ NAME_LENGTH ];
	char asMain[ NAME_LENGTH ];
    
    program_s() {  
        name[0] = 0;
		icon[0] = 0;
		type[0] = 0;
		deployType[0] = 0;
		entry[0] = 0;
		hasSecurity = false;
		securityImage[0] = 0;
		securityPassword[0] = 0;
		bgColor[0] = 0;
		asMain[0] = 0;
    }
} program_t;


class ApiController
{
public:
	CWnd *ownerWindow;
	CShockwaveflash1 *flashUI;

	void Initialize();
	void UpdateList();
	void ScanUsbDisk();
	CString BrowsePC();
	CString GetLocalAppNames(CString localDirectoryPath);
	CString GetLocalIconBase64(CString iconFilePath);
	CString GetDeviceFileContent(CString filePath);
	CString GetDeviceIconBase64(CString iconFilePath);
	BOOL DeleteAppOnDevice(CString appDirectoryPaths);
	BOOL DeleteAppOnPc(CString appName);
	BOOL DeleteDirectoryOnDevice(CString directory);
	BOOL DeleteAppNodeOnDeviceXml(CString appCategoryXml, CString appName);
	CString InstallNPK(CString npkFile);
	BOOL IsApplicationInstalled(CString appName, CString appCategoryXml);
	BOOL InsertXMLBufferElement(CString xmlFile, CString appCategoryXml, program_t& program);
	BOOL CopyDirectory(CString srcName, CString destName);
	BOOL SaveFileFromBase64(CString base64String, CString filePath);
	BOOL CancelDownload(CString appPathWithoutExtention);
	BOOL Import2Library();
	CString GetFirmwareVersion();
	BOOL UpdateFirmware(CString zipFilePath);
	void DeletePcDirectory(CString szPath);
	void DispatchFlashCommand(CString command, CString args);
	void DispatchFlashCall(const char* request, const char* args);
	void Exit();

	CString ImportVideos();
	CString GetLocalPictures(CString directory);

protected:
	CString m_workingFolderName;
	CString m_programUsbDiskName;

	CString m_downloadDirectory;
	CString m_downloadXml;
	TiXmlDocument *m_downloadDoc;

	CString m_driveTempName;
	CString m_driveNANDName;
	CString m_driveSDName;
	CString m_deviceDriveName;

	char *m_pu8xmlBuffer;
	TCHAR m_pszBuffer[MAX_PATH * 2];
};