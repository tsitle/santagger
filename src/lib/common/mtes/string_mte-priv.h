/******************************************************************************/
/* string_mte-priv.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private definitions for strings with multiple text-encoding                */
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

#ifndef HEAD_STRING_MTE_PRIV_H
#define HEAD_STRING_MTE_PRIV_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_STRING_MTE_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "src/includes/common/string_mte-defs.h"
#include "src/includes/common/dynlist-defs.h"


ST_BEGIN_C_DECLS

/*
// Types  (private)
*/
#if defined(SRC_STRING_MTE_C)
	/** mte-string */
	typedef struct {
		Tst_mtes_textEnc encOrg;   /* original text encoding */
		Tst_mtes_textEnc encCur;   /* current text encoding */
		Tst_str          *pData;   /* the string itself */
		Tst_uint32       strSz;    /* size of string buffer */
	} Tst_mtes__string_intn;

	/** list of mte-strings */
	typedef struct {
		Tst_dynlist list;
	} Tst_mtes__strArr_intn;

	/** */
	typedef struct {
		Tst_mtes_cset cset;
		Tst_bool      ascOrDesc;
		Tst_bool      compCasesens;
		Tst_int32     relat;
	} Tst_mtes__strArr_sortOpts;
#endif  /* private */


/*
// Functions  (private)
*/
#if defined(SRC_STRING_MTE_C)
	static void
	ST_MTES__toInt(const Tst_mtes_string *pStr,
	               ST_OPTARG(Tst_int32 *pI32),
	               ST_OPTARG(Tst_uint32 *pUI32));
	/** */
	static Tst_uint32
	ST_MTES__strlen_safe(const Tst_str *pSData,
	                     const Tst_uint32 datSz,
	                     const Tst_mtes_textEnc te,
	                     const Tst_bool asIso,
	                     const Tst_bool only1st,
	                     Tst_bool *pIsTerm,
	                     Tst_bool *pHasBOM);
	/** */
	static Tst_err
	ST_MTES__strcopy_n(const Tst_mtes_string *pSrc,
	                   Tst_mtes_string *pDest,
	                   Tst_uint32 maxCharsToCp,
	                   const Tst_bool fromBegOrEnd);
	static Tst_err
	ST_MTES__strcompare_n(const Tst_mtes_string *pStr1,
	                      const Tst_mtes_string *pStr2,
	                      const Tst_mtes_cset *pCSet,
	                      const Tst_bool cmpCasesens,
	                      const Tst_int32 maxChars,
	                      Tst_int32 *pRelat);
	/** */
	static Tst_int32
	ST_MTES__compWideCh(const Tst_byte wch1[],
	                    const Tst_byte wch2[],
	                    const Tst_bool isLEorBE);
	static Tst_bool
	ST_MTES__isAlphaWChar(const Tst_byte wch[],
	                      const Tst_mtes_cset *pCSet,
	                      const Tst_bool allwUpper,
	                      const Tst_bool allwLower,
	                      Tst_bool *pIsUpperOrLower);
	static Tst_bool
	ST_MTES__isNumWChar(const Tst_byte wch[],
	                    const Tst_mtes_cset *pCSet);
	static Tst_bool
	ST_MTES__isUmlautWChar(const Tst_byte wch[],
	                       const Tst_mtes_cset *pCSet,
	                       const Tst_bool allwUpper,
	                       const Tst_bool allwLower,
	                       Tst_bool *pIsUpperOrLower);
	static Tst_bool
	ST_MTES__isSomeWChar(const Tst_byte wch[],
	                     const Tst_mtes_cset *pCSet,
	                     const Tst_bool allwUpper,
	                     const Tst_bool allwLower,
	                     Tst_bool *pIsUpperOrLower,
	                     Tst_bool *pIsAlpha,
	                     Tst_bool *pIsUmlaut,
	                     Tst_bool *pIsNum);
	/** */
	static Tst_err
	ST_MTES__stripStr_iso_utf16(Tst_mtes_string *pStr,
	                            const Tst_mtes_strRestr *pRestr,
	                            Tst_bool *pHadInvChars,
	                            Tst_bool *pModded);
	/** */
	static Tst_err
	ST_MTES__bufReplace(Tst_buf **ppBuf,
	                    Tst_uint32 *pBSz,
	                    const Tst_buf *pRepl,
	                    const Tst_uint32 rSz,
	                    const Tst_uint32 skipFirstN,
	                    const Tst_buf *pWith,
	                    const Tst_uint32 wSz,
	                    Tst_bool *pChgd);
	/** */
	static Tst_err
	ST_MTES__strArr_add(Tst_mtes_strArr *pSArr,
	                    const Tst_mtes_string *pStr,
	                    const Tst_str *pCharISO,
	                    const Tst_str *pCharU8);

	/** */
#	if (ST_STRING_MTE_DEB_ == 1)
	static void ST_MTES__prf(const char *pFmt, ...);
	static void ST_MTES__prE(const char *pFmt, ...);
	static void ST_MTES__prBytes(const Tst_buf *pB, const Tst_uint32 sz);
#	endif
#endif  /* private */


/*
// Macros  (private)
*/
#if defined(SRC_STRING_MTE_C)
	/** UTF16: mapping of byteorder enums */
#	define ST_MAP_TV2_TE2BO(te)  (te == ST_MTES_TE_UTF16LE ? \
				ST_UTF16_BYTEORDER_LE : (te == ST_MTES_TE_UTF16BE ? \
					ST_UTF16_BYTEORDER_BE : ST_UTF16_BYTEORDER_ANY))
#	define ST_MAP_TV2_BO2TE(bo)  (bo == ST_UTF16_BYTEORDER_LE ? \
				ST_MTES_TE_UTF16LE : (bo == ST_UTF16_BYTEORDER_BE ? \
					ST_MTES_TE_UTF16BE : ST_MTES_TE_NONE))
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_STRING_MTE_PRIV_H */

/******************************************************************************/
