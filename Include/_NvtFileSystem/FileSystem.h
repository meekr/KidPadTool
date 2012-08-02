#include <stdio.h>   

#include "NVTTypes.h"
#include "nvtfat_config.h"
#include "nvtfat.h"

void FK(int a);

/*===================================================== Exported Functions ==*/
/* NVTFAT File System APIs */
 INT  fsInitFileSystem(VOID);
 INT  fsAssignDriveNumber(INT nDriveNo, INT disk_type, INT instance, INT partition);
 VOID fsInstallIoWriteCallBack(FS_DW_CB *cb_func);
 VOID fsInstallFileDelCallBack(FS_DEL_CB_T *cb_func);

/* Disk operations */
 INT  fsUnmountPhysicalDisk(PDISK_T *ptPDisk);
 INT  fsMountLogicalDisk(LDISK_T *ptLDisk);
 INT  fsUnmountLogicalDisk(LDISK_T *ptLDisk);
 INT  fsDiskFreeSpace(INT nDriveNo, UINT32 *puBlockSize, UINT32 *puFreeSize, UINT32 *puDiskSize);
 PDISK_T  *fsGetFullDiskInfomation(VOID);
 VOID fsReleaseDiskInformation(PDISK_T *ptPDiskList);
 INT  fsScanDisk(INT nDriveNo);
 INT  fsFormatFlashMemoryCard(PDISK_T *ptPDisk);
 INT  fsFormatFixedDrive(INT nDriveNo);
 INT  fsFormatDisk(INT disk_type, INT nand_part);
 VOID fsDiskWriteComplete(UINT8 *pucBuff);
 INT  fsGetLDiskVolID(INT nDriveNo, UINT32 *uVolID);
 INT  fsSetVolumeLabel(INT nDriveNo, CHAR *szLabel, INT nLen);
 INT  fsGetVolumeLabel(INT nDriveNo, CHAR *szLabel, INT nLen);
 INT  get_vdisk(INT nDriveNo, LDISK_T **plDisk);

/* File operations */
 INT  fsOpenFile(CHAR *suFileName, CHAR *szAsciiName, UINT32 uFlag);
 INT  fsCloseFile(INT hFile);
 INT  fsReadFile(INT hFile, UINT8 *pucPtr, INT nBytes, INT *pnReadCnt);
 INT  fsWriteFile(INT hFile, UINT8 *pucBuff, INT nBytes, INT *pnWriteCnt);
 INT64  fsFileSeek(INT hFile, INT64 n64Offset, INT16 usWhence);
 BOOL	fsIsEOF(INT hFile);
 INT  fsSetFileSize(INT hFile, CHAR *suFileName, CHAR *szAsciiName, INT64 n64NewSize);
 INT  fsGetFilePosition(INT hFile, UINT32 *puPos);
 INT64  fsGetFileSize(INT hFile);
 INT  fsGetFileStatus(INT hFile, CHAR *suFileName, CHAR *szAsciiName, FILE_STAT_T *ptFileStat);
 INT  fsSetFileStatus(INT hFile, CHAR *suFileName, CHAR *szAsciiName, FILE_STAT_T *ptFileStat);
 INT  fsSetFileAttribute(INT hFile, CHAR *suFileName, CHAR *szAsciiName, UINT8 ucAttrib, FILE_STAT_T *ptFileStat);
 INT  fsSetFileTime(INT hFile, CHAR *suFileName, CHAR *szAsciiName, UINT8 ucYear, UINT8 ucMonth, UINT8 ucDay, UINT8 ucHour, UINT8 unMin, UINT8 ucSec);
 INT  fsMergeFile(CHAR *suFileNameA, CHAR *szAsciiNameA, CHAR *suFileNameB, CHAR *szAsciiNameB);
 INT  fsDeleteFile(CHAR *suFileName, CHAR *szAsciiName);
 INT  fsRenameFile(CHAR *suOldName, CHAR *szOldAsciiName, CHAR *suNewName, CHAR *szNewAsciiName, BOOL bIsDirectory);
 INT  fsMoveFile(CHAR *suOldName, CHAR *szOldAsciiName, CHAR *suNewName, CHAR *szNewAsciiName, INT bIsDirectory);
 INT  fsCopyFile(CHAR *suSrcName, CHAR *szSrcAsciiName, CHAR *suDstName, CHAR *szDstAsciiName);
 INT	fsFindFirst(CHAR *suDirName, CHAR *szAsciiName, FILE_FIND_T *ptFindObj);
//extern INT fsFindFirst(CHAR *szDirName, FILE_FIND_T *ptFindObj);
 INT  fsFindNext(FILE_FIND_T *ptFindObj);
 INT	fsFindClose(FILE_FIND_T *ptFindObj);
 BOOL fsIsFileOpened(CHAR *suFileName);

/* Directory operations */
 INT  fsMakeDirectory(CHAR *suDirName, CHAR *szAsciiName);
//extern INT  fsMakeDirectory(CHAR *suDirName);
 INT  fsRemoveDirectory(CHAR *suDirName, CHAR *szAsciiName);
 INT  fsDeleteDirTree(CHAR *suDirName, CHAR *szAsciiName);
 INT  fsGetDirectoryInfo(CHAR *suDirName, CHAR *szAsciiName,
						INT *nFileCnt, INT *nDirCnt, UINT64 *u64TotalSize, BOOL bSearchSubDir);
 INT  fsGetDirectoryInfo2(CHAR *suDirName, CHAR *szAsciiName, CHAR **suPattern,
						INT *pnFileCnt, INT *pnDirCnt, UINT64 *u64TotalSize, BOOL bSearchSubDir);

/* language support */
 CHAR fsToUpperCase(CHAR chr);
 INT  fsUnicodeToAscii(VOID *pvUniStr, VOID *pvASCII, BOOL bIsNullTerm);
 INT  fsAsciiToUnicode(VOID *pvASCII, VOID *pvUniStr, BOOL bIsNullTerm);
 VOID fsAsciiToUpperCase(VOID *pvASCII);
 INT  fsAsciiNonCaseCompare(VOID *pvASCII1, VOID *pvASCII2);
 VOID fsUnicodeToUpperCase(VOID *pvUnicode);
 VOID fsUnicodeToLowerCase(VOID *pvUnicode);
 INT  fsUnicodeStrLen(VOID *pvUnicode);
 INT  fsUnicodeNonCaseCompare(VOID *pvUnicode1, VOID *pvUnicode2);
 INT  fsUnicodeCopyStr(VOID *pvStrDst, VOID *pvStrSrc);
 INT  fsUnicodeStrCat(VOID *pvUniStrHead, VOID *pvUniStrTail);
 VOID fsGetAsciiFileName(VOID *pvUniStr, VOID *pvAscii);
 INT  fsUnicodeWildCardCompare(CHAR *suStr1, CHAR *suStr2);

/* Driver supporting routines */
 INT  fsPhysicalDiskConnected(PDISK_T *ptPDisk);
 INT  fsPhysicalDiskDisconnected(PDISK_T *ptPDisk);


/* For debug and internal use, not exported funcions */
//extern CHAR *fsFindFirstSlash(CHAR *szFullName);
 CHAR *fsFindLastSlash(CHAR *suFullName);
 INT  fsTruncatePath(CHAR *szPath, CHAR *szToken);
 VOID  fsTrimEndingSpace(CHAR *szStr);
 FILE_T *fsHandleToDescriptor(INT hFile);
 INT  fsDescriptorToHandle(FILE_T *ptFile);

 VOID fsDumpBufferHex(UINT8 *pucBuff, INT nSize);
 VOID fsDumpSectorHex(INT uSectorNo, UINT8 *pucBuff, INT nSize);
 INT  fsDumpDiskSector(UINT32 uSectorNo, INT nSecNum);

 LDISK_T *fsAllocateDisk(PDISK_T *ptPDisk, PARTITION_T *ptPartition);
 UINT8 *fsAllocateSector(VOID);
 INT  fsFreeSector(UINT8 *pucSecAddr);

 CHAR  *fsDebugUniStr(CHAR *suStr);
//extern BOOL fsIsValidLongEntName(CHAR *szDirEntName);
//extern BOOL fsIsValidShortNameChar(CHAR cChar);

 VOID  lname_to_sname(CHAR *szAsciiName, INT nTildeNum, CHAR *szShortName);

 INT  fsFlushIOCache(VOID);
 INT  fsIOWrite(PDISK_T *ptPDisk, UINT32 uStartSecNo, INT nCount, UINT8 *pucOutBuff);
 INT  fsIORead(PDISK_T *ptPDisk, UINT32 uStartSecNo, INT nCount, UINT8 *in_buf);
 VOID fs_enable_iow_cache(VOID);
 INT  fs_flush_iow_cache(VOID);
 VOID *fsCheckDriverPointer(VOID *pvDrive);


//=========
 void fsResetSector(VOID);
 INT fsGetFinalSetor(VOID);
 void fsSetDirTime(int year,int mon,int day,int hour, int min, int sec);
