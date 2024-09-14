/******************************************************************************/
/* tag_id3v2_zchk-pp.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Priv/Protected functions for validating ID3v2 Tag field data               */
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

#ifndef HEAD_TAG_ID3V2_ZCHK_PP_H
#define HEAD_TAG_ID3V2_ZCHK_PP_H

/*
// Own-Includes
*/
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V2_ZCHK_C) || \
			defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_ID3V2_GS_C) || \
			defined(SRC_TAG_ID3V2_FLDADD_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZTIDY_C)
	/** */
	Tst_err
	st_id3v2_chk_checkFld(ST_OPTARG(const Tst_id3v2_opts *pOpts),
	                      const Tst_bool verbShowMappedStr,
	                      Tst_id3v2_fldData *pFld);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_ID3V2_ZCHK_C
	/** */
	static Tst_err
	ST_TIV2__chk_chkFld_partI(ST_OPTARG(const Tst_id3v2_opts *pOpts),
	                          const char *pFnc,
	                          Tst_id3v2_fldData_intn *pFldI,
	                          Tst_id3v2_fldProp_intn *pFldPrI,
	                          Tst_id3v2_fldErrs *pFErrs,
	                          Tst_bool *pIsOK);
	static Tst_err
	ST_TIV2__chk_chkFld_partII(ST_OPTARG(const Tst_id3v2_opts *pOpts),
	                           const char *pFnc,
	                           Tst_id3v2_fldData_intn *pFldI,
	                           Tst_id3v2_fldProp_intn *pFldPrI,
	                           Tst_id3v2_fldErrs *pFErrs,
	                           Tst_bool *pIsOK);
	static Tst_err
	ST_TIV2__chk_chkFld_partIII(ST_OPTARG(const Tst_id3v2_opts *pOpts),
	                            const char *pFnc,
	                            const Tst_bool verbShowMappedStr,
	                            Tst_id3v2_fldData_intn *pFldI,
	                            Tst_id3v2_fldProp_intn *pFldPrI,
	                            Tst_id3v2_fldErrs *pFErrs,
	                            Tst_bool *pIsOK);
	static Tst_err
	ST_TIV2__chk_chkFld_partIV(ST_OPTARG(const Tst_id3v2_opts *pOpts),
	                           const char *pFnc,
	                           Tst_id3v2_fldData_intn *pFldI,
	                           Tst_id3v2_fldProp_intn *pFldPrI,
	                           Tst_id3v2_fldErrs *pFErrs);
	/** */
	static Tst_bool
	ST_TIV2__chksub_cLang(Tst_str *pLang,
	                      ST_OPTARG(Tst_bool *pModded));
	/** */
	static Tst_bool
	ST_TIV2__chksub_cPict(Tst_id3v2_fldData_intn *pFldI,
	                      ST_OPTARG(Tst_bool *pModded));
	static void
	ST_TIV2__chksub_cSylt(Tst_id3v2_fldData_intn *pFldI,
	                      ST_OPTARG(Tst_bool *pModded));
	/** */
	static void
	ST_TIV2__chksub_cPeople_print(ST_OPTARG(const Tst_id3v2_opts *pOpts),
	                              const char *pFnc,
	                              Tst_id3v2_fldData_intn *pFldI,
	                              const int modlev);
	/** */
	static Tst_bool
	ST_TIV2__chksub_isDataAllNulls(Tst_binobj *pBinDat);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_ZCHK_PP_H */

/******************************************************************************/
