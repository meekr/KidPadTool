/****************************************************************
 *                                                              *
 * Copyright (c) Winbond Electronics Corp. All rights reserved. *
 *                                                              *
 ****************************************************************/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__


#include "../../HW/Include/SFR.h"		/* H/W SFR definition */

#define	__WINDOWS__						/* Windows platform */

// Active following definitions to avoid some types redefinition
#define __NVTTYPE_CHAR_DEFINED__
#define __NVTTYPE_PCHAR_DEFINED__
#define __NVTTYPE_PSTR_DEFINED__
#define __NVTTYPE_PCSTR_DEFINED__
#define __NVTTYPE_SIZE_T_DEFINED__
#include "NVTTypes.h"					/* Nuvoton type definition */

#endif /* __PLATFORM_H__ */
