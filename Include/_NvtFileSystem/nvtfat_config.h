/*************************************************************************
 * Nuvoton Electronics Corporation confidential
 *
 * Copyright (c) 2008 by Nuvoton Electronics Corporation
 * All rights reserved
 * 
 * FILENAME
 *     nvtfat_config.h
 *
 * REMARK
 *     None
 **************************************************************************/

#ifndef _NVTFAT_CONFIG_H_
#define _NVTFAT_CONFIG_H_

#include "nvtfat_platform.h"

/*----------------------------------------------------------------------------------------*/
/*  Support FAT file system only?                                                         */
/*----------------------------------------------------------------------------------------*/
#define FAT_ONLY					1

/*----------------------------------------------------------------------------------------*/
/*  Support FAT2 maintaining?                                                             */
/*----------------------------------------------------------------------------------------*/
#define USE_FAT2 

/*----------------------------------------------------------------------------------------*/
/*  Maximum number of file opened at the same time                                        */
/*----------------------------------------------------------------------------------------*/
#define MAX_OPEN_FILE           	12		

/*----------------------------------------------------------------------------------------*/
/*  Supported maximum physical sector size, suggest 512                                   */
/*----------------------------------------------------------------------------------------*/
#define MAX_SEC_SZ					512 

/*----------------------------------------------------------------------------------------*/
/*  Sector buffers for FAT table                                                          */
/*----------------------------------------------------------------------------------------*/
#define	NUM_FAT_BUFFER	   			8		/* !!at least 8!!                             */
#define FAT_BUF_SZ					8		/* Number of sectors each FAT buffer have     */

#if (NUM_FAT_BUFFER < 8)
#error "NUM_FAT_BUFFER too small!!"
#endif

/*----------------------------------------------------------------------------------------*/
/*  Number of FAT sector buffer                                                           */
/*----------------------------------------------------------------------------------------*/
#define NUM_SECTOR_BUFFER			8		/* !!at least 8!!                             */
#define RW_BUFF_SIZE				(4096)

#if (NUM_SECTOR_BUFFER < 8)
#error "NUM_SECTOR_BUFFER too small!!"
#endif

#if (RW_BUFF_SIZE < 4096)
#error "RW_BUFF_SIZE too small!!"
#endif


/*----------------------------------------------------------------------------------------*/
/*  I/O queue                                                                             */
/*----------------------------------------------------------------------------------------*/
#define IOC_BLOCK_CNT		8			/* number of I/O queue blocks                     */
#define IOC_BLOCK_LEN		8			/* secotrs in one block                           */

#define DIRECT_WRITE_TH		64          /* direct write threshold                         */
#define DIRECT_READ_TH		64		    /* direct read threshold                          */
#define MAX_WRITE_CNT		128

/*----------------------------------------------------------------------------------------*/
/*  File Search Support                                                                   */
/*----------------------------------------------------------------------------------------*/
#define FILE_SEARCH_EXT
#define SEARCH_PATTERN_MAX		8


/*----------------------------------------------------------------------------------------*/
/*  debugging output                                                                      */
/*----------------------------------------------------------------------------------------*/
//#define _debug_msg				sysprintf
//#define _info_msg				sysprintf		\
//#define _error_msg				sysprintf
//#define _debug_msg(...)	
#define _debug_msg		printf	
//#define _info_msg(...)	
#define _info_msg        printf	
//#define _error_msg(...)	
#define _error_msg		printf
#ifdef ECOS
#define sysprintf				diag_printf
#endif


/*----------------------------------------------------------------------------------------*/
/*  redefined timer ticks function                                                        */
/*----------------------------------------------------------------------------------------*/
#ifdef ECOS
//#define get_timer_ticks()		cyg_current_time()
#else
#define get_timer_ticks()		sysGetTicks(TIMER0)
//#define get_timer_ticks()		cyg_current_time()

#endif



#endif	/* _NVTFAT_CONFIG_H_ */

