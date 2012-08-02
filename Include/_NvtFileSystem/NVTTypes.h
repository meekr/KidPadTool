/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/

#ifndef __NVTTYPES_H__
#define __NVTTYPES_H__

#define CONST				const

#define FALSE				0
#define TRUE				1

#ifdef __GNUC__
# define INLINE  inline 
# define ALIGN(x)   __attribute__ ((aligned((x))))
#elif defined(__ICCARM__)
# define INLINE  inline
# define ALIGN(x)   _Pragma("data_alignment=(x)")
#else
# define INLINE     __inline
# define ALIGN(x)   __align(x)
#endif




#define VOID                void
typedef void *				PVOID;
typedef signed int			BOOL;
typedef signed int *		PBOOL;
typedef signed char			INT8;
typedef signed char *		PINT8;
typedef unsigned char		UINT8;
typedef unsigned char *		PUINT8;
typedef signed short		INT16;
typedef signed short *		PINT16;
typedef unsigned short		UINT16;
typedef unsigned short *	PUINT16;
typedef signed int			INT32;
typedef signed int *		PINT32;
typedef unsigned int		UINT32;
typedef unsigned int *		PUINT32;
typedef signed long long	INT64;
typedef signed long long *	PINT64;
typedef unsigned long long	UINT64;
typedef unsigned long long  *PUINT64;
typedef float				FLOAT;
typedef float *				PFLOAT;
typedef double				DOUBLE;
typedef double *			PDOUBLE;

//typedef signed char			CHAR;
typedef char			CHAR;


//typedef signed char *		PCHAR;
typedef  char *		PCHAR;

//typedef signed char *		PSTR;
typedef char *		PSTR;

//typedef const signed char *	PCSTR;
typedef const char *	PCSTR;
//typedef	UINT16				WCHAR;
//typedef	UINT16 *			PWCHAR;
//typedef	UINT16 *			PWSTR;
//typedef	const UINT16 *		PCWSTR;
#ifdef __cplusplus
typedef	wchar_t				WCHAR;
typedef	wchar_t *			PWCHAR;
typedef	wchar_t *			PWSTR;
typedef	const wchar_t *		PCWSTR;
#endif
//typedef UINT32				SIZE_T;
typedef volatile UINT8		REG8;
typedef volatile UINT16		REG16;
typedef volatile UINT32		REG32;
typedef	unsigned char		BYTE;
typedef INT32				ERRCODE;
typedef int               INT;

// SFR does not recommend to access by byte or half-word.
//#define outp8(port,value)		(*((REG8 *) (port))=(value))
//#define inp8(port)			(*((REG8 *) (port)))
//#define outp16(port,value)	(*((REG16 *)(port))=(value))
//#define inp16(port)			(*((REG16 *)(port)))
#define outp32(port,value)	(*((REG32 *)(port))=(value))
#define inp32(port)			(*((REG32 *)(port)))



#endif /* __NVTTYPES_H__ */

