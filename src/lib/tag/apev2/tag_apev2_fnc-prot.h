/******************************************************************************/
/* tag_apev2_fnc-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected functions for APEv2 Tags                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_APEV2_FNC_PROT_H
#define HEAD_TAG_APEV2_FNC_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_apev2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_FNC_C) || \
			defined(SRC_TAG_APEV2_GS_C) || \
			defined(SRC_TAG_APEV2_FLDADD_C) || defined(SRC_TAG_APEV2_FLDGET_C) || \
			defined(SRC_TAG_APEV2_ZRD_C) || defined(SRC_TAG_APEV2_ZWR_C) || \
			defined(SRC_TAG_APEV2_ZTIDY_C)
	/** */
#	if 0
	Tst_uint32
	st_apev2_fnc_getFirstValidFld_nr_byID(Tst_apev2_tag *pTag,
	                                      const Tst_apev2_frID id,
	                                      ST_OPTARG(const Tst_str *pIDstr));
	Tst_apev2_fldData*
	st_apev2_fnc_getFirstValidFld_pt_byID(Tst_apev2_tag *pTag,
	                                      const Tst_apev2_frID id,
	                                      ST_OPTARG(const Tst_str *pIDstr));
#	endif
	/** */
	Tst_bool
	st_apev2_fnc_isFldPropInArr_byIDstr(const Tst_apev2_fldProp_intn *pFldPrIArr,
	                                    const Tst_str *pIDstr,
	                                    ST_OPTARG(Tst_uint32 *pIx));
	Tst_bool
	st_apev2_fnc_isFldPropInArr_byEnum(const Tst_apev2_fldProp_intn *pFldPrIArr,
	                                   const Tst_apev2_frID id,
	                                   ST_OPTARG(Tst_uint32 *pIx));
	/** */
	Tst_apev2_frID
	st_apev2_fnc_adjustFrID(Tst_apev2_frID frID,
	                        ST_OPTARG(const Tst_str *pFrIDstr),
	                        ST_OPTARG(Tst_apev2_fldProp_intn **ppFldPrI));
	/** */
	Tst_bool
	st_apev2_fnc_matchFrIDs(Tst_apev2_frID frID1,
	                        ST_OPTARG(const char *pFrID1str),
	                        Tst_apev2_frID frID2,
	                        ST_OPTARG(const char *pFrID2str));

	/** */
	Tst_err
	st_apev2_fnc_getFldProps(const Tst_str *pFIDstr,
	                         const Tst_bool binOrU8,
	                         Tst_apev2_fldProp_intn *pFldPrI);
	/** */
	Tst_err
	st_apev2_fnc_addElemToFlds(Tst_apev2_tag *pTag,
	                           const Tst_apev2_frID srcFrID,
	                           ST_OPTARG(const Tst_str *pSrcFKey),
	                           const Tst_bool isBinOrU8,
	                           Tst_bool *pSkipped,
	                           Tst_apev2_fldData **ppNewFld);
	/** */
	Tst_err
	st_apev2_fnc_moveFld(Tst_apev2_tag *pTag,
	                     const Tst_uint32 curIx,
	                     const Tst_uint32 newIx);
	/** */
	/*Tst_apev2_frTp
	st_apev2_fnc_getFrameType_byID(const Tst_apev2_frID frID);*/

	/** */
	Tst_err
	st_apev2_fnc_setDataNstrTp(const Tst_apev2_frID frID,
	                           Tst_apev2_fldd_dattp *pTpDat,
	                           Tst_apev2_fldd_strtp *pTpStr);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_APEV2_FNC_PROT_H */

/******************************************************************************/
