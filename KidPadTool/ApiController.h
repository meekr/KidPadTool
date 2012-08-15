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
	CString GetDeviceFileContent(CString filePath);
	CString GetDeviceIconBase64(CString iconFilePath);
	void DeleteDeviceApp(CString appName);
	BOOL DeleteDirectoryOnDevice(CString directory);
	BOOL DeleteAppNodeOnDeviceXml(CString appCategoryXml, CString appName);
	BOOL IsApplicationInstalled(CString appName);
	BOOL InsertXMLBufferElement(CString xmlFile, CString appListXml, program_t& program);
	BOOL SaveFileFromBase64(CString base64String, CString filePath);
	BOOL CancelDownload(CString appPathWithoutExtention);
	BOOL Import2Library();
	CString GetFirmwareVersion();
	BOOL UpdateFirmware(CString zipFilePath);
	void DeletePcDirectory(CString szPath);
	void DispatchFlashCommand(CString command, CString args);
	void DispatchFlashCall(const char* request, const char* args);
	void Exit();

	void InstallApp(CString appName);
	void TransferApp(CString appName);
	CString InsertAppNode(CString appName);
	CString GetDeviceApps(CString xmlFile);
	CString GetDeviceMedia(CString xmlFile, CString directory);
	void DeleteDeviceMedia(CString xmlFile, CString mediaFile);

	void ExtractZipCallback(void * p1, void * p2, void * p3);
	void ConvertAVCallback(void * information, void * percentage, void * milisecond);
	void ConvertMusic(CString filepath);
	void ConvertVideo(CString filepath);
	void ConvertPicture(CString filepath);
	CString AppendMediaNodeToXml(CString xmlFile, CString filename, CString mediaType);
	void Transfer2Device(CString sourceFile, CString targetDirectory);

protected:
	CString m_workingFolderName;
	CString m_programUsbDiskName;

	CString m_mediaDirectory4Video;
	CString m_mediaDirectory4Music;
	CString m_mediaDirectory4Picture;
	CString m_downloadXml;
	TiXmlDocument *m_downloadDoc;

	CString m_driveSDName;

	char *m_pu8xmlBuffer;
	TCHAR m_pszBuffer[MAX_PATH * 2];
};