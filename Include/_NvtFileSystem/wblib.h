/***************************************************************************
 *                                                                         *
 * Copyright (c) 2009 Nuvoton Technology. All rights reserved.             *
 *                                                                         *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     WBLIB.h
 *
 * VERSION
 *     1.1
 *
 * DESCRIPTION
 *     The header file of NUC900 system library.
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     2008-06-26  Ver 1.0 draft by Min-Nan Cheng
 *     2009-02-26  Ver 1.1 Changed for NUC900 MCU
 *
 * REMARK
 *     None
 **************************************************************************/
#ifndef _WBLIB_H
#define _WBLIB_H

#include "NUC930_reg.h"
#include "wberrcode.h"
#include "wbio.h"

//-- function return value
#define	   Successful  0
#define	   Fail        1

#define EXTERNAL_CRYSTAL_CLOCK  27000000


/* Define constants for use external IO in service parameters.  */
#define EXT0			0
#define EXT1			1
#define EXT2			2


#define SIZE_256K		4
#define SIZE_512K		5
#define SIZE_1M			6
#define SIZE_2M			7
#define SIZE_4M			8
#define SIZE_8M			9
#define SIZE_16M		10
#define SIZE_32M		11

#define BUS_DISABLE		12
#define BUS_BIT_8		13
#define BUS_BIT_16		14
#define BUS_BIT_32		15


/* Define the vector numbers associated with each interrupt */
typedef enum int_source_e
{
	IRQ_WDT=1,		// Watch Dog Timer
	IRQ_EXTINT0,	// External Interrupt 0
	IRQ_EXTINT1,	// External Interrupt 1
	IRQ_EXTINT2,	// External Interrupt 2
	IRQ_EXTINT3,	// External Interrupt 3
	IRQ_USBH,		// USB Host interrupt
	IRQ_APU,		// AudioProcessing Unit Interrupt
	IRQ_VPOST,		// Display Interface Controller Interrupt
	IRQ_ADC,		// ADC interrupt
	IRQ_UART,		// UART interrupt
	IRQ_TIMER,		// Timer interrupt
	IRQ_RESERVED0,  // reserved
	IRQ_RESERVED1,	// reserved
	IRQ_RESERVED2,	// reserved
	IRQ_RESERVED3,	// reserved
	IRQ_RESERVED4,	// reserved
	IRQ_VIN,		// Video-In Interface Interrupt
	IRQ_USBD,		// USB Device interrupt
	IRQ_RESERVED5,	// reserved
	IRQ_GDMA0,		// GDMA Channel 0 interrupt
	IRQ_GDMA1,		// GDMA Channel 1 interrupt
	IRQ_SD,			// SD Host Interrupt
	IRQ_JPEG,		// JPEG Interrupt
	IRQ_SPI0,		// SPI0 (Master/Slave) Serial Interface Interrupt
	IRQ_SPI1,		// SPI(Master) Serial Interface Interrupt
	IRQ_RTC,		// RTC interrupt
	IRQ_PWM0,		// PWM/Capture 0 Interrupt
	IRQ_PWM1,		// PWM/Capture 1 Interrupt
	IRQ_PWM2,		// PWM/Capture 2 Interrupt
	IRQ_PWM3,		// PWM/Capture 3 Interrupt
	IRQ_RESERVED6	// reserved
	
} INT_SOURCE_E;


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


#define ONE_SHOT_MODE     0
#define PERIODIC_MODE     1


#define ONE_HALF_SECS     0
#define FIVE_SECS         1
#define TEN_SECS          2
#define TWENTY_SECS       3

/* Define constants for use UART in service parameters.  */
#define WB_UART_0		0


#define WB_DATA_BITS_5    0x00
#define WB_DATA_BITS_6    0x01
#define WB_DATA_BITS_7    0x02
#define WB_DATA_BITS_8    0x03

#define WB_STOP_BITS_1    0x00
#define WB_STOP_BITS_2    0x04

#define WB_PARITY_NONE    0x00
#define WB_PARITY_ODD     0x00
#define WB_PARITY_EVEN    0x10

//#define WB_DTR_Low        0x01
//#define WB_RTS_Low        0x02
//#define WB_MODEM_En       0x08

#define LEVEL_1_BYTE      0x00
#define LEVEL_4_BYTES     0x40
#define LEVEL_8_BYTES     0x80
#define LEVEL_14_BYTES    0xC0

/* Define constants for use AIC in service parameters.  */
#define WB_SWI                     0
#define WB_D_ABORT                 1
#define WB_I_ABORT                 2
#define WB_UNDEFINE                3

/* The parameters for sysSetInterruptPriorityLevel() and 
   sysInstallISR() use */
#define FIQ_LEVEL_0                0
#define IRQ_LEVEL_1                1
#define IRQ_LEVEL_2                2
#define IRQ_LEVEL_3                3
#define IRQ_LEVEL_4                4
#define IRQ_LEVEL_5                5
#define IRQ_LEVEL_6                6
#define IRQ_LEVEL_7                7

/* The parameters for sysSetGlobalInterrupt() use */
#define ENABLE_ALL_INTERRUPTS      0
#define DISABLE_ALL_INTERRUPTS     1

/* The parameters for sysSetInterruptType() use */
#define LOW_LEVEL_SENSITIVE        0x00
#define HIGH_LEVEL_SENSITIVE       0x40
#define NEGATIVE_EDGE_TRIGGER      0x80
#define POSITIVE_EDGE_TRIGGER      0xC0

/* The parameters for sysSetLocalInterrupt() use */
#define ENABLE_IRQ                 0x7F
#define ENABLE_FIQ                 0xBF
#define ENABLE_FIQ_IRQ             0x3F
#define DISABLE_IRQ                0x80
#define DISABLE_FIQ                0x40
#define DISABLE_FIQ_IRQ            0xC0

/* Define Cache type  */
#define CACHE_WRITE_BACK		0
#define CACHE_WRITE_THROUGH		1
#define CACHE_DISABLE			-1

#define MMU_DIRECT_MAPPING	0
#define MMU_INVERSE_MAPPING	1


/* Define constants for use Cache in service parameters.  */
#define CACHE_4M		2
#define CACHE_8M		3
#define CACHE_16M		4
#define CACHE_32M		5
#define I_CACHE			6
#define D_CACHE			7
#define I_D_CACHE		8


/* Define UART initialization data structure */
typedef struct UART_INIT_STRUCT
{
	UINT32		uart_no;
    UINT32		uiFreq;
    UINT32		uiBaudrate;
    UINT8		uiDataBits;
    UINT8		uiStopBits;
    UINT8		uiParity;
    UINT8		uiRxTriggerLevel;
} WB_UART_T;

//extern UINT32 UART_BA;

/* UART return value */
#define WB_INVALID_PARITY       -1
#define WB_INVALID_DATA_BITS    -2
#define WB_INVALID_STOP_BITS    -3
#define WB_INVALID_BAUD         -4


/* Define PLL initialization data structure */
typedef struct PLL_INIT_STRUCT
{
	UINT32		pll0;		/* PLL0 output frequency */
    UINT32		cpu_src;	/* Select CPU clock from PLL0 or PLL1 */
    UINT32		ahb_clk;	/* the ratio of CPU : AHB clock */
    UINT32		apb_clk;	/* the ratio of AHB : APB clock */
} WB_PLL_T;


/* Define PLL freq. setting */
#define PLL_DISABLE	0x14F27
#define	PLL_80MHZ	0xFF01  //user defined
#define	PLL_96MHZ	0x3F67 
#define PLL_132MHZ	0x2B27 
#define	PLL_192MHZ	0x3F47 
#define	PLL_200MHZ	0x4227 
#define	PLL_240MHZ	0x4F47 

/* Define CPU clock source */
#define CPU_FROM_PLL0			0
#define CPU_FROM_EXTERNAL		1
#define CPU_FROM_SYS32K			2


/* Define AHB clock */
#define	AHB_CPUCLK_1_1	0
#define	AHB_CPUCLK_1_2	1
#define	AHB_CPUCLK_1_4	2
#define	AHB_CPUCLK_1_8	3

/* Define APB clock */
#define APB_AHB_1_1		0
#define APB_AHB_1_2		1
#define APB_AHB_1_4		2
#define APB_AHB_1_8		3

/* Define to get clock freq. */
typedef struct CLK_FREQ_STRUCT
{
	INT32		pll_clk_freq;	/* PLL output frequency, MHz */
    INT32		cpu_clk_freq;	/* CPU frequency, MHz */
    INT32		ahb_clk_freq;	/* the ratio of CPU : AHB clock */
    INT32		apb_clk_freq;	/* the ratio of AHB : APB clock */
} WB_CLKFREQ_T;


/* Define the constant values of PM */
#define WB_PM_IDLE		1
#define WB_PM_PD		2
#define WB_PM_MIDLE	    5

#define WB_PM_PD_IRQ_Fail			-1
#define WB_PM_Type_Fail			    -2
#define WB_PM_INVALID_IRQ_NUM		-3
#define WB_PM_CACHE_OFF				-4


/* Define system library Timer functions */
UINT32	sysGetTicks (INT32 nTimeNo);
INT32	sysResetTicks (INT32 nTimeNo);
INT32	sysUpdateTickCount(INT32 nTimeNo, UINT32 uCount);
INT32	sysSetTimerReferenceClock (INT32 nTimeNo, UINT32 uClockRate);
INT32	sysStartTimer (INT32 nTimeNo, UINT32 uTicksPerSecond, INT32 nOpMode);
INT32	sysStopTimer (INT32 nTimeNo);
VOID	sysClearWatchDogTimerCount (VOID);
VOID	sysClearWatchDogTimerInterruptStatus(VOID);
VOID	sysDisableWatchDogTimer (VOID);
VOID	sysDisableWatchDogTimerReset(VOID);
VOID	sysEnableWatchDogTimer (VOID);
VOID	sysEnableWatchDogTimerReset(VOID);
PVOID	sysInstallWatchDogTimerISR (INT32 nIntTypeLevel, PVOID pvNewISR);
INT32	sysSetWatchDogTimerInterval (INT32 nWdtInterval);
INT32	sysSetTimerEvent(INT32 nTimeNo, UINT32 uTimeTick, PVOID pvFun);
VOID	sysClearTimerEvent(INT32 nTimeNo, UINT32 uTimeEventNo);
VOID	sysSetLocalTime(DateTime_T ltime);
VOID	sysGetCurrentTime(DateTime_T *curTime);
VOID	sysDelay(UINT32 uTicks);

/* Define system library UART functions */
INT8	sysGetChar (VOID);
INT32	sysInitializeUART (WB_UART_T *uart);
VOID	sysPrintf (PINT8 pcStr,...);
VOID	sysprintf (PINT8 pcStr,...);
VOID	sysPutChar (UINT8 ucCh);

/* Define system library AIC functions */
INT32	sysDisableInterrupt (INT_SOURCE_E eIntNo);
INT32	sysEnableInterrupt (INT_SOURCE_E eIntNo);
BOOL	sysGetIBitState(VOID);
UINT32	sysGetInterruptEnableStatus(VOID);
PVOID	sysInstallExceptionHandler (INT32 nExceptType, PVOID pvNewHandler);
PVOID	sysInstallFiqHandler (PVOID pvNewISR);
PVOID	sysInstallIrqHandler (PVOID pvNewISR);
PVOID	sysInstallISR (INT32 nIntTypeLevel, INT_SOURCE_E eIntNo, PVOID pvNewISR);
INT32	sysSetGlobalInterrupt (INT32 nIntState);
INT32	sysSetInterruptPriorityLevel (INT_SOURCE_E eIntNo, UINT32 uIntLevel);
INT32	sysSetInterruptType (INT_SOURCE_E eIntNo, UINT32 uIntSourceType);
INT32	sysSetLocalInterrupt (INT32 nIntState);
INT32	sysSetAIC2SWMode(VOID);


/* Define system library Cache functions */
VOID	sysDisableCache(VOID);
INT32	sysEnableCache(UINT32 uCacheOpMode);
VOID	sysFlushCache(INT32 nCacheType);
BOOL	sysGetCacheState(VOID);
INT32	sysGetSdramSizebyMB(VOID);
VOID	sysInvalidCache(VOID);
INT32 	sysSetCachePages(UINT32 addr, INT32 size, INT32 cache_mode);


/* Define system clock functions */
INT32 sysGetPLLConfig(WB_PLL_T *sysClk);
INT32 sysSetPLLConfig(WB_PLL_T *sysClk);
INT32 sysGetClockFreq(WB_CLKFREQ_T *sysFreq);


/* Define system library External IO functions */
INT32	sysSetExternalIO(INT extNo, UINT32 extBaseAddr, UINT32 extSize, INT extBusWidth);
INT32	sysSetExternalIOTiming1(INT extNo, INT tACC, INT tACS);
INT32	sysSetExternalIOTiming2(INT extNo, INT tCOH, INT tCOS);


/* Define system power management functions */
VOID sysDisableAllPM_IRQ(VOID);
INT sysEnablePM_IRQ(INT irq_no);
INT sysPMStart(INT pd_type);

#endif  /* _WBLIB_H */

