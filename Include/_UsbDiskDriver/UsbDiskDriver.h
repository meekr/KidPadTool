#ifndef __USBDISKDRIVER_H__
#define __USBDISKDRIVER_H__

#include "winioctl.h"
#include "FileSystem.h"


class UsbDiskDriver : public storage_driver_S
{
public:
	static UINT32 scan ();
	
	UsbDiskDriver ();
	virtual ~UsbDiskDriver ();

	void onDriveAttach (LPCTSTR drive);
	void onDriveDetach ();
	virtual BOOL onLineCheck ();
	virtual DWORD getDiskSize ();
	void getVendorId (CString &vendorId) const;
	void getProductId (CString &productId) const;
	void getProductRevision (CString &productRevision) const;
	void getSerialNumber (CString &serialNumber) const;
	void getDiskId (CString &diskId) const;

private:
	static INT s_init (PDISK_T *);
	static INT s_read (PDISK_T *, UINT32, INT, UINT8 *);
	static INT s_write (PDISK_T *, UINT32, INT, UINT8 *, BOOL);
	static INT s_ioctl (PDISK_T *, INT, void *);

protected:
	virtual int diskInit (PDISK_T *);
	virtual int diskRead (PDISK_T *, UINT32, INT, UINT8 *);
	virtual int diskWrite (PDISK_T *, UINT32, INT, UINT8 *, BOOL);
	virtual int diskIoctl (PDISK_T *, INT, void *);
	
	HANDLE getUsbDisk (LPCTSTR globalDiskDriveName);

protected:
	HANDLE m_hUsbDisk;
	STORAGE_DEVICE_DESCRIPTOR *m_storDevDesc;

	CString m_vendorId;
	CString m_productId;
	CString m_productRevision;
	CString m_serialNumber;
};

#endif // #ifndef __USBDISKDRIVER_H__