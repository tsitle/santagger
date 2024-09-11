/******************************************************************************/
/* tag_fldmap-defs.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for mapping Tag Fields                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_FLDMAP_DEFS_H
#define HEAD_TAG_FLDMAP_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Types
*/
/** generic tag field */
typedef struct {
	void *pObInternal;  /* pointer to Tst_tfldMap_genTagFld_intn */
} Tst_tfldMap_genTagFld;


ST_END_C_DECLS

#endif  /* HEAD_TAG_FLDMAP_DEFS_H */

/******************************************************************************/
