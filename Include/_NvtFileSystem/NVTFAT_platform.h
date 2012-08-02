/*************************************************************************
 * Nuvoton Electronics Corporation confidential
 *
 * Copyright (c) 2008 by Nuvoton Electronics Corporation
 * All rights reserved
 * 
 * FILENAME
 *     NVTFAT_platform.h
 *
 * REMARK
 *     None
 **************************************************************************/
 
#ifndef _NVTFAT_PLATFORM_H_
#define _NVTFAT_PLATFORM_H_

#ifdef ECOS
#include "drv_api.h"
#include "diag.h"
#endif

#include "wbio.h"
//#include "wbtypes.h"
#include "NVTTypes.h"

//#define CACHE_BIT					0x80000000
#define CACHE_BIT					0x00000000
#define flush_cache()		


#define GET16_L(bptr,n)   	(bptr[n] | (bptr[n+1] << 8))
#define GET32_L(bptr,n)   	(bptr[n] | (bptr[n+1] << 8) | (bptr[n+2] << 16) | (bptr[n+3] << 24))
#define PUT16_L(bptr,n,val)	bptr[n] = val & 0xFF;				\
							bptr[n+1] = (val >> 8) & 0xFF;
#define PUT32_L(bptr,n,val)	bptr[n] = val & 0xFF;				\
							bptr[n+1] = (val >> 8) & 0xFF;		\
							bptr[n+2] = (val >> 16) & 0xFF;		\
							bptr[n+3] = (val >> 24) & 0xFF;

#define GET16_B(bptr,n)   	((bptr[n]) << 8 | bptr[n+1])
#define GET32_B(bptr,n)   	((bptr[n] << 24) | (bptr[n+1] << 16) | (bptr[n+2] << 8) | bptr[n+3])
#define PUT16_B(bptr,n,val)	bptr[n+1] = val & 0xFF;				\
							bptr[n] = (val >> 8) & 0xFF;
#define PUT32_B(bptr,n,val)	bptr[n+3] = val & 0xFF;				\
							bptr[n+2] = (val >> 8) & 0xFF;		\
							bptr[n+1] = (val >> 16) & 0xFF;		\
							bptr[n] = (val >> 24) & 0xFF;
#define	min(x,y)			(((x) <	(y)) ? (x) : (y))
#define	MIN(x,y)			(((x) <	(y)) ? (x) : (y))
#define	max(x,y)			(((x) >	(y)) ? (x) : (y))
#define	MAX(x,y)			(((x) >	(y)) ? (x) : (y))


typedef struct datetime_t
{
	UINT32	year;
	UINT32	mon;
	UINT32	day;
	UINT32	hour;
	UINT32	min;
	UINT32	sec;
} DateTime_T;


/* Define constants for use timer in service parameters.  */
#define TIMER0            0
#define TIMER1            1

/* The parameters for sysSetGlobalInterrupt() use */
#define ENABLE_ALL_INTERRUPTS      0
#define DISABLE_ALL_INTERRUPTS     1

/* The parameters for sysSetLocalInterrupt() use */
#define ENABLE_IRQ                 0x7F
#define ENABLE_FIQ                 0xBF
#define ENABLE_FIQ_IRQ             0x3F
#define DISABLE_IRQ                0x80
#define DISABLE_FIQ                0x40
#define DISABLE_FIQ_IRQ            0xC0


/* Define system library Timer functions */
UINT32	sysGetTicks (INT32 nTimeNo);

INT32	 sysSetTimerEvent(INT32 nTimeNo, UINT32 uTimeTick, PVOID pvFun);
VOID	sysClearTimerEvent(INT32 nTimeNo, UINT32 uTimeEventNo);
VOID	sysSetLocalTime(DateTime_T ltime);
VOID	sysGetCurrentTime(DateTime_T *curTime);

/* Define system library UART functions */
VOID	sysPrintf (PINT8 pcStr,...);
VOID	sysprintf (PINT8 pcStr,...);
VOID	sysPutChar (UINT8 ucCh);

/* Define system library AIC functions */
INT32	sysSetGlobalInterrupt (INT32 nIntState);
INT32	sysSetLocalInterrupt (INT32 nIntState);


extern VOID *fmiGetpDisk(UINT32 uCard);

#endif  /* _NVTFAT_PLATFORM_H_ */

