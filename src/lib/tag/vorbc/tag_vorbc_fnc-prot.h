/******************************************************************************/
/* tag_vorbc_fnc.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected sub functions for Vorbis Tag handling                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 14.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_VORBC_FNC_PROT_H
#define HEAD_TAG_VORBC_FNC_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_vorbc-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_VORBC_C) || defined(SRC_TAG_VORBC_FNC_C) || \
			defined(SRC_TAG_VORBC_GS_C) || \
			defined(SRC_TAG_VORBC_FLDADD_C) || defined(SRC_TAG_VORBC_FLDGET_C) || \
			defined(SRC_TAG_VORBC_ZRD_C) || defined(SRC_TAG_VORBC_ZWR_C) || \
			defined(SRC_TAG_VORBC_ZTIDY_C)
	/** */
#	if 0
	Tst_uint32
	st_vorbc_fnc_getFirstValidFld_nr_byID(Tst_vorbc_tag *pTag,
    	                                  const Tst_vorbc_frID id,
    	                                  ST_OPTARG(const Tst_str *pIDstr));
	Tst_vorbc_fldData*
	st_vorbc_fnc_getFirstValidFld_pt_byID(Tst_vorbc_tag *pTag,
	                                      const Tst_vorbc_frID id,
	                                      ST_OPTARG(const Tst_str *pIDstr));
#	endif

	/** */
	Tst_bool
	st_vorbc_fnc_isFldPropInArr_byIDstr(const Tst_vorbc_fldProp_intn *pFldPrIArr,
	                                    const Tst_str *pIDstr,
	                                    ST_OPTARG(Tst_uint32 *pIx));
	Tst_bool
	st_vorbc_fnc_isFldPropInArr_byEnum(const Tst_vorbc_fldProp_intn *pFldPrIArr,
	                                   const Tst_vorbc_frID id,
	                                   ST_OPTARG(Tst_uint32 *pIx));
	/** */
	Tst_vorbc_frID
	st_vorbc_fnc_adjustFrID(Tst_vorbc_frID frID,
	                        ST_OPTARG(const Tst_str *pFrIDstr),
	                        ST_OPTARG(Tst_vorbc_fldProp_intn **ppFldPrI));
	/** */
	Tst_bool
	st_vorbc_fnc_matchFrIDs(Tst_vorbc_frID frID1,
	                        ST_OPTARG(const char *pFrID1str),
	                        Tst_vorbc_frID frID2,
	                        ST_OPTARG(const char *pFrID2str));

	/** */
	Tst_err
	st_vorbc_fnc_addElemToFlds(Tst_vorbc_tag *pTag,
	                           Tst_vorbc_frID srcFrID,
	                           ST_OPTARG(const Tst_str *pSrcFKey),
	                           Tst_bool *pSkipped,
	                           Tst_vorbc_fldData **ppNewFld);
	/** */
	Tst_err
	st_vorbc_fnc_moveFld(Tst_vorbc_tag *pTag,
	                     const Tst_uint32 curIx,
	                     const Tst_uint32 newIx);
	/** */
	/*Tst_vorbc_frTp
	st_vorbc_fnc_getFrameType_byID(const Tst_vorbc_frID frID);*/

	/** */
	Tst_err
	st_vorbc_fnc_setDataNstrTp(const Tst_vorbc_frID frID,
	                           Tst_vorbc_fldd_dattp *pTpDat,
	                           Tst_vorbc_fldd_strtp *pTpStr);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_FNC_PROT_H */

/******************************************************************************/
