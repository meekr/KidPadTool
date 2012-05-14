#ifndef __NVTOFFSETSTDUSBDISKDRIVER_H__
#define __NVTOFFSETSTDUSBDISKDRIVER_H__

#include "UsbDiskDriver.h"
#include <vector>

class NvtOffsetUsbDiskDriver : public UsbDiskDriver
{
public:
	NvtOffsetUsbDiskDriver ();
	virtual ~NvtOffsetUsbDiskDriver ();

	virtual BOOL onLineCheck ();	

protected:
	virtual int diskInit (PDISK_T *);
	virtual int diskRead (PDISK_T *, UINT32, INT, UINT8 *);
	virtual int diskWrite (PDISK_T *, UINT32, INT, UINT8 *, BOOL);
	virtual int diskIoctl (PDISK_T *, INT, void *);

	void calcOffset ();

protected:
	std::vector<CString> m_offsetDisks;
	BOOL m_offsetConfirmed;
	UINT32 m_offset; // in sectors;
};

#endif // #ifndef __NVTOFFSETSTDUSBDISKDRIVER_H__