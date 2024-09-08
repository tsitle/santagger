/******************************************************************************/
/* tag_comfnc_chk-pp.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Private/Protected functions for validating/parsing field data              */
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

#ifndef HEAD_TAG_COMFNC_CHK_PROT_H
#define HEAD_TAG_COMFNC_CHK_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/string_mte-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part II
*/
#if defined(SRC_TAG_COMFNC_CHK_C) || \
			defined(SRC_TAG_COMFNC_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C)
	/** */
	Tst_bool
	st_tagCFnc_chk_cGenre(Tst_mtes_string *pStr,
	                      ST_OPTARG(Tst_bool *pModded));
	Tst_bool
	st_tagCFnc_chk_cGenre_sub(const Tst_mtes_string *pInp,
	                          Tst_mtes_string *pCap,
	                          Tst_int32 *pGennr,
	                          Tst_str **ppRstr,
	                          ST_OPTARG(Tst_bool *pModded));
	Tst_bool
	st_tagCFnc_chk_cTrack(Tst_mtes_string *pStr,
	                      ST_OPTARG(Tst_uint32 *pNum),
	                      ST_OPTARG(Tst_uint32 *pNumTot),
	                      ST_OPTARG(Tst_bool *pModded));
	Tst_bool
	st_tagCFnc_chk_cTimeOrDate(Tst_mtes_string *pStr,
	                           const Tst_bool isTime,
	                           ST_OPTARG(Tst_tagCFnc_tstamp *pTStamp),
	                           ST_OPTARG(Tst_bool *pModded));
	Tst_bool
	st_tagCFnc_chk_cTStamp(Tst_mtes_string *pStr,
	                       ST_OPTARG(Tst_tagCFnc_tstamp *pTStamp),
	                       ST_OPTARG(Tst_bool *pModded));
	Tst_bool
	st_tagCFnc_chk_cPeopleArr(Tst_mtes_strArr *pPeopleArr,
	                          ST_OPTARG(Tst_bool *pModded));
	Tst_bool
	st_tagCFnc_chk_cPeopleEntry(Tst_mtes_string *pEntry,
	                            ST_OPTARG(Tst_bool *pModded));
	Tst_bool
	st_tagCFnc_chk_cBPM(Tst_mtes_string *pStr,
	                    ST_OPTARG(Tst_bool *pModded));
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_COMFNC_CHK_C
	/** */
	static Tst_err
	ST_TCFNC__chk_getGenre(const Tst_mtes_string *pGin,
	                       Tst_mtes_string *pCapOut,
	                       const Tst_mtes_textEnc textEncOut,
	                       Tst_int32 *pGNrOut,
	                       ST_OPTARG(Tst_bool *pModded));
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_COMFNC_CHK_PROT_H */

/******************************************************************************/
