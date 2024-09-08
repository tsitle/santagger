/******************************************************************************/
/* tag_conv-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for converting Tags                                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_CONV_DEFS_H
#define HEAD_TAG_CONV_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
typedef enum {
	ST_TCNV_TTP_IV1,
	ST_TCNV_TTP_IV2,
	ST_TCNV_TTP_AV2,
	ST_TCNV_TTP_VOR
} Tst_tagConv_tagTp;

ST_END_C_DECLS

#endif  /* HEAD_TAG_CONV_DEFS_H */

/******************************************************************************/
