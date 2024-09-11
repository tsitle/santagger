/******************************************************************************/
/* tag_id3v2_fnc-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for ID3v2 Tag handling                             */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_FNC_PROT_H
#define HEAD_TAG_ID3V2_FNC_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_ID3V2_FNC_C) || \
			defined(SRC_TAG_ID3V2_GS_C) || \
			defined(SRC_TAG_ID3V2_FLDADD_C) || defined(SRC_TAG_ID3V2_FLDGET_C) || \
			defined(SRC_TAG_ID3V2_STCS_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZRD2_C) || \
			defined(SRC_TAG_ID3V2_ZWR_C) || \
			defined(SRC_TAG_ID3V2_ZTIDY_C) || \
			defined(SRC_TAG_ID3V2_ZFDEB_C)
	/** */
	Tst_err
	st_id3v2_fnc_getFileBasename(const Tst_mtes_string *pFilen,
	                             ST_OPTARG(Tst_mtes_string *pBasenU8),
	                             ST_OPTARG(Tst_str **ppBasenU8PCh));
	Tst_err
	st_id3v2_fnc_getFilenameExt(const Tst_mtes_string *pFilen,
	                            Tst_str **ppFExtU8PCh);

	/** */
	Tst_id3v2_frTp
	st_id3v2_fnc_getFrameType_byID(const Tst_id3v2_frID frID);

	/** */
	Tst_err
	st_id3v2_fnc_setDataNstrTp(const Tst_id3v2_frID frID,
	                           Tst_id3v2_fldd_dattp *pTpDat,
	                           Tst_id3v2_fldd_strtp *pTpStr,
	                           ST_OPTARG(Tst_id3v2_fldd_rwordPArr pRdWrOrderArr),
	                           const Tst_uint32 rwoArrSz);

	/** */
	Tst_bool
	st_id3v2_fnc_isFldPropInArr_byIDstr(const Tst_id3v2_fldProp_intn *pFldPrIArr,
	                                    const Tst_str *pIDstr,
	                                    const Tst_byte verMaj,
	                                    ST_OPTARG(Tst_uint32 *pIx));
	Tst_bool
	st_id3v2_fnc_isFldPropInArr_byEnum(const Tst_id3v2_fldProp_intn *pFldPrIArr,
	                                   const Tst_id3v2_frID id,
	                                   ST_OPTARG(Tst_uint32 *pIx));
	/** */
	Tst_id3v2_frID
	st_id3v2_fnc_adjustFrID(Tst_id3v2_frID frID,
	                        ST_OPTARG(const Tst_str *pFrIDstr),
	                        ST_OPTARG(Tst_id3v2_fldProp_intn **ppFldPrI));
	/** */
	Tst_bool
	st_id3v2_fnc_matchFrIDs(Tst_id3v2_frID frID1,
	                        ST_OPTARG(const char *pFrID1str),
	                        Tst_id3v2_frID frID2,
	                        ST_OPTARG(const char *pFrID2str));
	Tst_bool
	st_id3v2_fnc_isTaggInfFr(Tst_id3v2_frID frID,
	                         ST_OPTARG(const char *pFrIDstr),
	                         const Tst_uint32 tagVerMaj);

	/** */
#	if 0
	Tst_uint32
	st_id3v2_fnc_getFirstValidFld_nr_byID(Tst_id3v2_tag *pTag,
	                                      Tst_id3v2_frID id,
	                                      ST_OPTARG(const Tst_str *pIDstr));
#	endif
	Tst_id3v2_fldData*
	st_id3v2_fnc_getFirstValidFld_pt_byID(Tst_id3v2_tag *pTag,
	                                      Tst_id3v2_frID id,
	                                      ST_OPTARG(const Tst_str *pIDstr));

	/** */
	Tst_err
	st_id3v2_fnc_addElemToFlds(Tst_id3v2_tag *pTag,
	                           Tst_id3v2_frID frID,
	                           ST_OPTARG(const Tst_str *pFrIDstr),
	                           Tst_bool *pSkipped,
	                           Tst_id3v2_fldData **ppNewFld);

	/** */
	Tst_err
	st_id3v2_fnc_sortFields_byPrio(Tst_id3v2_tag *pTag,
	                               const Tst_bool sortPicTypes);
	/** */
	void
	st_id3v2_fnc_updateFldErrorCnt(Tst_id3v2_tag *pTag);

	/** */
	Tst_err
	st_id3v2_fnc_moveFld(Tst_id3v2_tag *pTag,
	                     const Tst_uint32 curIx,
	                     const Tst_uint32 newIx);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_FNC_PROT_H */

/******************************************************************************/
