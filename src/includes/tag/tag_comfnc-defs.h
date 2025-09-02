/******************************************************************************/
/* tag_comfnc-defs.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Common definitions for Tags                                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 10.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_COMFNC_DEFS_H
#define LIBSANTAGGER_TAG_COMFNC_DEFS_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Types
*/
/** Timestamp values */
typedef struct {
	Tst_uint32 year;
	Tst_byte   month;
	Tst_byte   day;
	Tst_byte   hour;
	Tst_byte   min;
	Tst_byte   sec;
} Tst_tagCFnc_tstamp;


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_COMFNC_DEFS_H */

/******************************************************************************/
