/******************************************************************************/
/* sys_math-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for doing some math with 64bit integers                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_SYS_MATH_DEFS_H
#define HEAD_SYS_MATH_DEFS_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_SYSMATH_OVERFLOW_CHECK_  0  /* enable overflow checks ? */


/*
// Types
*/
/** */
typedef struct {
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	Tst_int64_native nativeS64;
#	else
	Tst_uint32 loS;  /* low-bits */
	Tst_uint32 hiS;  /* high-bits */
#	endif
} Tst_int64;

/** */
typedef struct {
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	Tst_uint64_native nativeU64;
#	else
	Tst_uint32 loU;  /* low-bits */
	Tst_uint32 hiU;  /* high-bits */
#	endif
} Tst_uint64;


ST_END_C_DECLS

#endif  /* HEAD_SYS_MATH_DEFS_H */

/******************************************************************************/
