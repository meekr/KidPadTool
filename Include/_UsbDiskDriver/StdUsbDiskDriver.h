#ifndef __STDSTDUSBDISKDRIVER_H__
#define __STDSTDUSBDISKDRIVER_H__

#include "UsbDiskDriver.h"

class StdUsbDiskDriver : public UsbDiskDriver
{
public:
	StdUsbDiskDriver ();
	virtual ~StdUsbDiskDriver ();

	virtual BOOL onLineCheck ();	

protected:
	virtual int diskInit (PDISK_T *);
	virtual int diskRead (PDISK_T *, UINT32, INT, UINT8 *);
	virtual int diskWrite (PDISK_T *, UINT32, INT, UINT8 *, BOOL);
	virtual int diskIoctl (PDISK_T *, INT, void *);
};

#endif // #ifndef __STDSTDUSBDISKDRIVER_H__