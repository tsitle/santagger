/******************************************************************************/
/* string_mte.h                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for strings with multiple text-encoding                          */
/*   (Latin1 aka ISO, UTF8, UTF16-LE/BE)                                      */
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

#ifndef HEAD_STRING_MTE_H
#define HEAD_STRING_MTE_H

/*
// Own-Includes
*/
#include <santag/common/string_mte-defs.h>


ST_BEGIN_C_DECLS

/*
// Functions
*/
/** */
ST_EXPORT Tst_mtes_textEnc
st_mtes_getEncodingOrg(ST_OPTARG(const Tst_mtes_string *pStr));

ST_EXPORT Tst_mtes_textEnc
st_mtes_getEncodingCur(ST_OPTARG(const Tst_mtes_string *pStr));

ST_EXPORT Tst_uint32
st_mtes_getStringSize(ST_OPTARG(const Tst_mtes_string *pStr));

ST_EXPORT const Tst_buf*
st_mtes_getDataPt(ST_OPTARG(const Tst_mtes_string *pStr));

/** */
ST_EXPORT Tst_err
st_mtes_copyToCharp(ST_OPTARG(const Tst_mtes_string *pStr),
                    Tst_str **ppChar,
                    ST_OPTARG(Tst_uint32 *pOutSz));

ST_EXPORT Tst_err
st_mtes_copyToCharp_iso(ST_OPTARG(const Tst_mtes_string *pStr),
                        Tst_str **ppCharISO);

ST_EXPORT Tst_err
st_mtes_copyToCharp_utf8(ST_OPTARG(const Tst_mtes_string *pStr),
                         Tst_str **ppCharU8);

/** */
ST_EXPORT Tst_err
st_mtes_copyFromCharp(ST_OPTARG(const Tst_str *pChar),
                      const Tst_uint32 pcharSz,
                      const Tst_mtes_textEnc srcTE,
                      Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_mtes_copyFromCharp_iso(ST_OPTARG(const Tst_str *pCharISO),
                          Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_mtes_copyFromCharp_utf8(ST_OPTARG(const Tst_str *pCharU8),
                           Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_mtes_copyFromCharp_isoOrUtf8(ST_OPTARG(const Tst_str *pChar),
                                const Tst_bool isISOorUTF8,
                                Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_mtes_setStrTo(const Tst_mtes_textEnc targTE,
                 ST_OPTARG(const Tst_str *pCharISO),
                 Tst_mtes_string *pStr);

#define st_mtes_setStrTo2(mac_targTE, mac_pCharISO, mac_pStr)  \
			st_mtes_setStrTo(mac_targTE, (Tst_str*)mac_pCharISO, mac_pStr)

/** */
ST_EXPORT Tst_err
st_mtes_convCharp(const Tst_str *pCharIn,
                  const Tst_uint32 szIn,
                  const Tst_mtes_textEnc teIn,
                  const Tst_mtes_textEnc teOut,
                  Tst_str **ppCharOut,
                  ST_OPTARG(Tst_uint32 *pSzOut));

/** */
ST_EXPORT Tst_err
st_mtes_convStr(Tst_mtes_string *pStr, const Tst_mtes_textEnc targTE);

ST_EXPORT Tst_err
st_mtes_convStr_c(const Tst_mtes_string *pSrc,
                  Tst_mtes_string *pDest,
                  const Tst_mtes_textEnc targTE);

ST_EXPORT Tst_err
st_mtes_convStrTo_iso(Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_mtes_convStrTo_iso_c(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest);

ST_EXPORT Tst_err
st_mtes_convStrTo_utf8(Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_mtes_convStrTo_utf8_c(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest);

ST_EXPORT Tst_err
st_mtes_convStrTo_utf16(Tst_mtes_string *pStr, const Tst_mtes_textEnc targTE);

ST_EXPORT Tst_err
st_mtes_convStrTo_utf16_c(const Tst_mtes_string *pSrc,
                          Tst_mtes_string *pDest,
                          const Tst_mtes_textEnc targTE);

/** */
ST_EXPORT Tst_err
st_mtes_int32ToStr(const Tst_int32 int32, Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_mtes_uint32ToStr(const Tst_uint32 uint32, Tst_mtes_string *pStr);

ST_EXPORT Tst_int32
st_mtes_toInt32(const Tst_mtes_string *pStr);

ST_EXPORT Tst_uint32
st_mtes_toUInt32(const Tst_mtes_string *pStr);

/** */
ST_EXPORT Tst_bool
st_mtes_isTextEncValid(const Tst_mtes_textEnc te);

ST_EXPORT Tst_uint32
st_mtes_getCharWidth(const Tst_mtes_textEnc te);

ST_EXPORT Tst_uint32
st_mtes_getStrBOMsz(ST_OPTARG(const Tst_mtes_string *pStr));

ST_EXPORT Tst_uint32
st_mtes_getCharpBOMsz(ST_OPTARG(const Tst_str *pChar),
                      const Tst_uint32 bufSz,
                      const Tst_mtes_textEnc te);

ST_EXPORT Tst_bool
st_mtes_isStrNULterm(ST_OPTARG(const Tst_mtes_string *pStr));

ST_EXPORT Tst_bool
st_mtes_isStrEmptyBy(ST_OPTARG(const Tst_mtes_string *pStr));

ST_EXPORT Tst_bool
st_mtes_isStrEmptyCh(ST_OPTARG(const Tst_mtes_string *pStr));

/** */
ST_EXPORT Tst_bool
st_mtes_isAlphaChar(const Tst_str ch,
                    const Tst_bool allwUpper,
                    const Tst_bool allwLower,
                    Tst_bool *pIsUpperOrLower);

ST_EXPORT Tst_bool
st_mtes_isAlphaWChar(const Tst_byte wch[],
                     const Tst_bool isLEorBE,
                     const Tst_bool allwUpper,
                     const Tst_bool allwLower,
                     Tst_bool *pIsUpperOrLower);

ST_EXPORT Tst_bool
st_mtes_isNumChar(const Tst_str ch);

ST_EXPORT Tst_bool
st_mtes_isNumWChar(const Tst_byte wch[], const Tst_bool isLEorBE);

ST_EXPORT Tst_bool
st_mtes_isUmlautChar(const Tst_str ch,
                     const Tst_bool allwUpper,
                     const Tst_bool allwLower,
                     Tst_bool *pIsUpperOrLower);

ST_EXPORT Tst_bool
st_mtes_isUmlautWChar(const Tst_byte wch[],
                      const Tst_bool isLEorBE,
                      const Tst_bool allwUpper,
                      const Tst_bool allwLower,
                      Tst_bool *pIsUpperOrLower);

/** */
/*** */
ST_EXPORT Tst_uint32
st_mtes_strlenBy(ST_OPTARG(const Tst_mtes_string *pStr));

ST_EXPORT Tst_uint32
st_mtes_strlenCh(ST_OPTARG(const Tst_mtes_string *pStr));

/*** */
ST_EXPORT Tst_uint32
st_mtes_strlenCharpBy_iso_utf8(ST_OPTARG(const Tst_str *pChar));

ST_EXPORT Tst_uint32
st_mtes_strlenCharpBy_utf16(ST_OPTARG(const Tst_str *pChar),
                            const Tst_uint32 bufSz,
                            const Tst_mtes_textEnc te);

/*** */
ST_EXPORT Tst_uint32
st_mtes_strlenCharpCh_iso(ST_OPTARG(const Tst_str *pChar));

ST_EXPORT Tst_uint32
st_mtes_strlenCharpCh_utf8(ST_OPTARG(const Tst_str *pChar));

ST_EXPORT Tst_uint32
st_mtes_strlenCharpCh_utf16(ST_OPTARG(const Tst_str *pChar),
                            const Tst_uint32 bufSz,
                            const Tst_mtes_textEnc te);

/*** */
ST_EXPORT Tst_uint32
st_mtes_strlenCharpBy_safe_iso_utf8(ST_OPTARG(const Tst_str *pCharBuf),
                                    const Tst_uint32 bufSz,
                                    ST_OPTARG(Tst_bool *pHasTerm));
ST_EXPORT Tst_uint32
st_mtes_strlenCharpBy_safe_utf16(ST_OPTARG(const Tst_str *pCharBuf),
                                 const Tst_uint32 bufSz,
                                 const Tst_mtes_textEnc te,
                                 ST_OPTARG(Tst_bool *pHasTerm),
                                 ST_OPTARG(Tst_bool *pHasBOM));

/** */
ST_EXPORT Tst_err
st_mtes_strCompare(const Tst_mtes_string *pStr1,
                   const Tst_mtes_string *pStr2,
                   const Tst_bool compCasesens,
                   int *pRelat);

ST_EXPORT Tst_bool
st_mtes_strEqual(const Tst_mtes_string *pStr1,
                 const Tst_mtes_string *pStr2,
                 const Tst_bool compCasesens);

ST_EXPORT Tst_err
st_mtes_strNCompare(const Tst_mtes_string *pStr1,
                    const Tst_mtes_string *pStr2,
                    const Tst_bool compCasesens,
                    const Tst_uint32 maxChars,
                    int *pRelat);

ST_EXPORT Tst_bool
st_mtes_strNEqual(const Tst_mtes_string *pStr1,
                  const Tst_mtes_string *pStr2,
                  const Tst_bool compCasesens,
                  const Tst_uint32 maxChars);

ST_EXPORT Tst_err
st_mtes_strReplace(Tst_mtes_string *pStr,
                   const Tst_str *pReplISO,
                   ST_OPTARG(const Tst_str *pWithISO),
                   ST_OPTARG(Tst_bool *pModded));

ST_EXPORT Tst_bool
st_mtes_strPos(const Tst_mtes_string *pStr,
               const Tst_str *pNeedleISO,
               ST_OPTARG(Tst_uint32 *pOffsCh));

ST_EXPORT Tst_err
st_mtes_strSplit(const Tst_mtes_string *pStr,
                 const Tst_str *pSeparatorISO,
                 Tst_mtes_string *pLeft,
                 Tst_mtes_string *pRight);

ST_EXPORT Tst_err
st_mtes_strCpy(const Tst_mtes_string *pSrc,
               Tst_mtes_string *pDest);

ST_EXPORT Tst_err
st_mtes_strNCpy(const Tst_mtes_string *pSrc,
                Tst_mtes_string *pDest,
                Tst_uint32 maxCharsToCpFromLeft);

ST_EXPORT Tst_err
st_mtes_strRNCpy(const Tst_mtes_string *pSrc,
                 Tst_mtes_string *pDest,
                 Tst_uint32 maxCharsToCpFromRight);

ST_EXPORT Tst_err
st_mtes_strConcat(const Tst_mtes_string *pSrc,
                  Tst_mtes_string *pDest);

/** */
ST_EXPORT Tst_err
st_mtes_shortenStr(Tst_mtes_string *pStr,
                   const Tst_uint32 maxChars,
                   const Tst_mtes_string *pPlaceholder,
                   const Tst_bool replaceEndOrCenter);

/** */
ST_EXPORT Tst_err
st_mtes_replaceUmlauts(Tst_mtes_string *pStr, ST_OPTARG(Tst_bool *pModded));

ST_EXPORT Tst_err
st_mtes_stripStr(Tst_mtes_string *pStr,
                 ST_OPTARG(const Tst_mtes_strRestr *pRestr),
                 ST_OPTARG(Tst_bool *pHadInvChars),
                 ST_OPTARG(Tst_bool *pModded));

/** */
ST_EXPORT Tst_uint32
st_mtes_strArrGetElemCount(ST_OPTARG(const Tst_mtes_strArr *pSArr));

ST_EXPORT Tst_mtes_string*
st_mtes_strArrGetElem(ST_OPTARG(const Tst_mtes_strArr *pSArr),
                      const Tst_uint32 ix);

/** */
ST_EXPORT Tst_err
st_mtes_strArrAdd(Tst_mtes_strArr *pSArr, const Tst_mtes_string *pStrToAdd);

ST_EXPORT Tst_err
st_mtes_strArrAddCharp_iso(Tst_mtes_strArr *pSArr, const Tst_str *pCharToAddISO);

ST_EXPORT Tst_err
st_mtes_strArrAddCharp_utf8(Tst_mtes_strArr *pSArr, const Tst_str *pCharToAddU8);

ST_EXPORT Tst_err
st_mtes_strArrRem(Tst_mtes_strArr *pSArr, const Tst_uint32 ix);

ST_EXPORT Tst_err
st_mtes_strArrCpy(Tst_mtes_strArr *pSArrSrc, Tst_mtes_strArr *pSArrDst);

/** */
ST_EXPORT Tst_err
st_mtes_strArrStrip(Tst_mtes_strArr *pSArr,
                    const Tst_bool arePairs,
                    ST_OPTARG(Tst_bool *pModded));

/** */
ST_EXPORT Tst_err
st_mtes_strArrSort(Tst_mtes_strArr *pSArr,
                   const Tst_bool ascOrDesc,
                   const Tst_bool compCasesens);

/** */
ST_EXPORT Tst_err
st_mtes_strArrMergeToOneString(Tst_mtes_strArr *pSArr,
                               ST_OPTARG(const Tst_str *pSeparatorISO));

/** */
ST_EXPORT Tst_uint32
st_mtes_strArrTotalStrlenBy(const Tst_mtes_strArr *pSArr);

/** */
ST_EXPORT Tst_bool
st_mtes_testIfIsASCII(ST_OPTARG(const Tst_str *pStr), const Tst_uint32 sz);

ST_EXPORT Tst_bool
st_mtes_testIfIsUTF8(ST_OPTARG(const Tst_str *pStr), const Tst_uint32 sz);

ST_EXPORT Tst_bool
st_mtes_testIfIsUTF16(ST_OPTARG(const Tst_str *pStr),
                      const Tst_uint32 sz,
                      Tst_bool *pIsLEorBE);

/** mainly for testing purposes */
ST_EXPORT Tst_bool
st_mtes_generateString(Tst_mtes_string *pStr,
                       const Tst_mtes_textEnc te,
                       const Tst_uint32 len,
                       ST_OPTARG(const Tst_str *pCharset));


/*
// Functions
*/
/** Structs */
/*** */
ST_EXPORT Tst_err
st_mtes_stc_initStr(Tst_mtes_string *pStr);

ST_EXPORT void
st_mtes_stc_rsetStr(Tst_mtes_string *pStr,
                    const Tst_bool resEncOrg,
                    const Tst_bool resEncCur);

ST_EXPORT void
st_mtes_stc_freeStr(Tst_mtes_string *pStr);

/*** */
ST_EXPORT Tst_err
st_mtes_stc_initSA(Tst_mtes_strArr *pSArr);

ST_EXPORT void
st_mtes_stc_rsetSA(Tst_mtes_strArr *pSArr);

ST_EXPORT void
st_mtes_stc_freeSA(Tst_mtes_strArr *pSArr);

/*** */
ST_EXPORT void
st_mtes_stc_rsetRestr(Tst_mtes_strRestr *pRestr,
                      const Tst_bool setAccToMode);

/*** */
ST_EXPORT void
st_mtes_stc_rsetCharSet(Tst_mtes_cset *pCSet, const Tst_bool isU16_leOrBe);


ST_END_C_DECLS

#endif  /* HEAD_STRING_MTE_H */

/******************************************************************************/
