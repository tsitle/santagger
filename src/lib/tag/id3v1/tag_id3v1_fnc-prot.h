/******************************************************************************/
/* tag_id3v1_fnc-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for ID3v1 (Extended) Tags                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_ID3V1_FNC_PROT_H
#define LIBSANTAGGER_TAG_ID3V1_FNC_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/string_mte-defs.h"
#include "src/includes/tag/tag_id3v1-defs.h"
/** */
#include "tag_id3v1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V1_FNC_C) || defined(SRC_TAG_ID3V1_C) || \
			defined(SRC_TAG_ID3V1_GS_C) || defined(SRC_TAG_ID3V1_STCS_C) || \
			defined(SRC_TAG_ID3V1_ZRD_C) || defined(SRC_TAG_ID3V1_ZWR_C)
	/** */
	Tst_bool
	st_id3v1_fnc_isFldPropInArr_byEnum(const Tst_id3v1_fldProp_intn *pFldPrIArr,
	                                   const Tst_id3v1_frID id,
	                                   const Tst_id3v1ext_frID idExt,
	                                   Tst_uint32 *pIx);
	/** */
	Tst_err
	st_id3v1_fnc_stripStr(Tst_mtes_string *pStr,
	                      ST_OPTARG(Tst_bool *pHadInvChars),
	                      ST_OPTARG(Tst_bool *pHadUmlauts),
	                      ST_OPTARG(Tst_bool *pWasModifiedByStripping));
	/** */
	void
	st_id3v1_fnc_updAreFldsSet(Tst_id3v1_tag_intn *pTagI);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_ID3V1_FNC_PROT_H */

/******************************************************************************/
