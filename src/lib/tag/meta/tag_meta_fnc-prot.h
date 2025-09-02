/******************************************************************************/
/* tag_meta_fnc-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for Meta Tags                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_META_FNC_PROT_H
#define LIBSANTAGGER_TAG_META_FNC_PROT_H

/*
// Own-Includes
*/
/** */
#include "tag_meta-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_META_C) || defined(SRC_TAG_META_GS_C) || \
			defined(SRC_TAG_META_STCS_C) || defined(SRC_TAG_META_FNC_C)
	/** */
	Tst_uint32
	st_tagMeta_getNewUID(Tst_tagMeta_mtArr_intn *pMAI);
	/** */
	Tst_tagMeta_mt*
	st_tagMeta_getTag_byUidOrBsIx(const Tst_tagMeta_mtArr *pMA,
	                              Tst_uint32 *pIndex,
	                              const Tst_uint32 *pUID,
	                              const Tst_uint32 *pBSIx,
	                              const Tst_uint32 *pBSSIx);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_META_FNC_PROT_H */

/******************************************************************************/
