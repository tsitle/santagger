/******************************************************************************/
/* tag_id3_helper-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for ID3 Tags                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_ID3_HELPER_PROT_H
#define LIBSANTAGGER_TAG_ID3_HELPER_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_TAG_ID3_HELPER_C) || defined(SRC_TAG_COMFNC_CHK_C)
	/** ID3 genres */
	extern const Tst_id3_genre ST_ID3_GENRES[];
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_ID3_HELPER_PROT_H */

/******************************************************************************/
