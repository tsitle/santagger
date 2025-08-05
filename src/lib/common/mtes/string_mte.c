/******************************************************************************/
/* string_mte.c                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Strings with multiple text-encodings (Latin1 aka ISO, UTF8, UTF16-LE/BE)   */
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

/*
// Own-Includes
*/
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
/** */
#define SRC_STRING_MTE_C
#include "string_mte-priv.h"
/*** */
#include "w-utf-prot.h"
#undef SRC_STRING_MTE_C

/*
// System-Includes
*/
#include <string.h>      /* strlen(), memcpy() */
#include <stdlib.h>      /* calloc() */
#if (ST_STRING_MTE_DEB_ == 1)
#	include <stdarg.h>       /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Byte-Order-Marker  */
const Tst_str ST_UTF16_BOM_LE[] = {0xff, 0xfe};  /* Little-Endian */
const Tst_str ST_UTF16_BOM_BE[] = {0xfe, 0xff};  /* Big-Endian */

#define ST_MTES_USE_LIBC_STRLEN_  1

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_mtes_string */
Tst_err
st_mtes_stc_initStr(Tst_mtes_string *pStr)
{
	ST_ASSERTN_IARG(pStr == NULL)

	ST_CALLOC(pStr->pObInternal, Tst_mtes__string_intn*,
			1, sizeof(Tst_mtes__string_intn))
	if (pStr->pObInternal == NULL)
		return ST_ERR_OMEM;

	((Tst_mtes__string_intn*)pStr->pObInternal)->pData = NULL;

	st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
	return ST_ERR_SUCC;
}

void
st_mtes_stc_rsetStr(Tst_mtes_string *pStr,
		const Tst_bool resEncOrg, const Tst_bool resEncCur)
{
	Tst_mtes__string_intn *pSI;

	ST_ASSERTN_VOID(pStr == NULL || pStr->pObInternal == NULL)

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;
	/* */
	ST_DELPOINT(pSI->pData)
	pSI->strSz = 0;
	if (resEncOrg)
		pSI->encOrg = ST_MTES_TE_ISO;
	if (resEncCur)
		pSI->encCur = ST_MTES_TE_ISO;
}

void
st_mtes_stc_freeStr(Tst_mtes_string *pStr)
{
	ST_ASSERTN_VOID(pStr == NULL)

	if (pStr->pObInternal != NULL) {
		ST_DELPOINT(((Tst_mtes__string_intn*)pStr->pObInternal)->pData)
		ST_DELPOINT(pStr->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_mtes_strArr */
static void
ST_MTES__cbResetElemStr(void *pElem)
{
	st_mtes_stc_rsetStr((Tst_mtes_string*)pElem, ST_B_TRUE, ST_B_TRUE);
}

static void
ST_MTES__cbFreeElemStr(void *pElem)
{
	st_mtes_stc_freeStr((Tst_mtes_string*)pElem);
}

static Tst_err
ST_MTES__cbCloneInitElemStr(void *pElem)
{
	return st_mtes_stc_initStr((Tst_mtes_string*)pElem);
}

static Tst_err
ST_MTES__cbCloneCopyElemStr(const void *pElemSrc, void *pElemDst)
{
	return st_mtes_strCpy((Tst_mtes_string*)pElemSrc, (Tst_mtes_string*)pElemDst);
}

Tst_err
st_mtes_stc_initSA(Tst_mtes_strArr *pSArr)
{
	ST_ASSERTN_IARG(pSArr == NULL)

	ST_CALLOC(pSArr->pObInternal, Tst_mtes__strArr_intn*,
			1, sizeof(Tst_mtes__strArr_intn))
	if (pSArr->pObInternal == NULL)
		return ST_ERR_OMEM;
	return st_dynlist_stc_initDL(
			&((Tst_mtes__strArr_intn*)pSArr->pObInternal)->list,
			sizeof(Tst_mtes__string_intn),
			ST_MTES__cbResetElemStr, ST_MTES__cbFreeElemStr);
}

void
st_mtes_stc_rsetSA(Tst_mtes_strArr *pSArr)
{
	ST_ASSERTN_VOID(pSArr == NULL || pSArr->pObInternal == NULL)

	st_dynlist_stc_rsetDL(&((Tst_mtes__strArr_intn*)pSArr->pObInternal)->list);
}

void
st_mtes_stc_freeSA(Tst_mtes_strArr *pSArr)
{
	ST_ASSERTN_VOID(pSArr == NULL)

	if (pSArr->pObInternal != NULL) {
		st_dynlist_stc_freeDL(&((Tst_mtes__strArr_intn*)pSArr->pObInternal)->list);
		ST_DELPOINT(pSArr->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_mtes_strRestr */
void
st_mtes_stc_rsetRestr(Tst_mtes_strRestr *pRestr, const Tst_bool setAccToMode)
{
#	define LOC_SETRESTR_(v2r, v2rTypus, v2rVal) { \
				v2r.dis##v2rTypus = ! v2rVal; }

	ST_ASSERTN_VOID(pRestr == NULL)

	if (! setAccToMode) {
		pRestr->hasR    = ST_B_FALSE;
		pRestr->ruleAlw = ST_B_TRUE;
	}
	LOC_SETRESTR_((*pRestr), Alpha,    pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Dig,      pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Spa,      pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Tab,      pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Newln,    pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Slash,    pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Bracks,   pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Paren,    pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), LngSpec,  pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), NotPrint, pRestr->ruleAlw)
	LOC_SETRESTR_((*pRestr), Other,    pRestr->ruleAlw)
	pRestr->strMaxl = 0;
#	undef LOC_SETRESTR_
}

/*----------------------------------------------------------------------------*/

/* Charset: Tst_mtes_cset */
void
st_mtes_stc_rsetCharSet(Tst_mtes_cset *pCSet, const Tst_bool isU16_leOrBe)
{
#	define LOC_GET_CHAR_ISO_(csvar, v2csch)  { \
				if (isU16_leOrBe) { \
					pCSet->csvar[0] = ST_CSET_ISO_##v2csch; \
					pCSet->csvar[1] = 0x00; \
				} else { \
					pCSet->csvar[0] = 0x00; \
					pCSet->csvar[1] = ST_CSET_ISO_##v2csch; \
				} }
#	define LOC_GET_CHAR_U16_(csvar, by1, by2)  { \
				if (isU16_leOrBe) { \
					pCSet->csvar[0] = by1; \
					pCSet->csvar[1] = by2; \
				} else { \
					pCSet->csvar[0] = by2; \
					pCSet->csvar[1] = by1; \
				} }

	ST_ASSERTN_VOID(pCSet == NULL)

	pCSet->isLEorBE = isU16_leOrBe;
	/* these are equally encoded in ISO and U16: */
	/** */
	LOC_GET_CHAR_ISO_(lf,     LF);
	LOC_GET_CHAR_ISO_(cr,     CR);
	LOC_GET_CHAR_ISO_(tab,    TAB);
	LOC_GET_CHAR_ISO_(spa,    SPA);
	LOC_GET_CHAR_ISO_(slash,  SLA);
	LOC_GET_CHAR_ISO_(parth1, PARTH1);
	LOC_GET_CHAR_ISO_(parth2, PARTH2);
	LOC_GET_CHAR_ISO_(aCap,   ACAP);
	LOC_GET_CHAR_ISO_(zCap,   ZCAP);
	LOC_GET_CHAR_ISO_(aSma,   ASMA);
	LOC_GET_CHAR_ISO_(zSma,   ZSMA);
	LOC_GET_CHAR_ISO_(digZe,  DIG0);
	LOC_GET_CHAR_ISO_(digNi,  DIG9);
	LOC_GET_CHAR_ISO_(bra1B,  BRA1B);
	LOC_GET_CHAR_ISO_(bra1E,  BRA1E);
	LOC_GET_CHAR_ISO_(bra2B,  BRA2B);
	LOC_GET_CHAR_ISO_(bra2E,  BRA2E);
	LOC_GET_CHAR_ISO_(bra3B,  BRA3B);
	LOC_GET_CHAR_ISO_(bra3E,  BRA3E);
	LOC_GET_CHAR_ISO_(prnSma, PRNSMA);
	LOC_GET_CHAR_ISO_(prnBig, PRNBIG);
	/** */
	LOC_GET_CHAR_ISO_(umAeC,  UMAEC);
	LOC_GET_CHAR_ISO_(umOeC,  UMOEC);
	LOC_GET_CHAR_ISO_(umUeC,  UMUEC);
	LOC_GET_CHAR_ISO_(umAeS,  UMAES);
	LOC_GET_CHAR_ISO_(umOeS,  UMOES);
	LOC_GET_CHAR_ISO_(umUeS,  UMUES);
	LOC_GET_CHAR_ISO_(umSz,   UMSZ);
	LOC_GET_CHAR_ISO_(umAe2C, UMAE2C);
	LOC_GET_CHAR_ISO_(umOe2C, UMOE2C);
	LOC_GET_CHAR_ISO_(umAaC,  UMAAC);
	LOC_GET_CHAR_ISO_(umAe2S, UMAE2S);
	LOC_GET_CHAR_ISO_(umOe2S, UMOE2S);
	LOC_GET_CHAR_ISO_(umAaS,  UMAAS);
	LOC_GET_CHAR_ISO_(nTildC, NTILDC);
	LOC_GET_CHAR_ISO_(oAcuC,  OACUC);
	LOC_GET_CHAR_ISO_(uAcuC,  UACUC);
	LOC_GET_CHAR_ISO_(aAcuC,  AACUC);
	LOC_GET_CHAR_ISO_(eAcuC,  EACUC);
	LOC_GET_CHAR_ISO_(iAcuC,  IACUC);
	LOC_GET_CHAR_ISO_(yAcuC,  YACUC);
	LOC_GET_CHAR_ISO_(nTildS, NTILDS);
	LOC_GET_CHAR_ISO_(oAcuS,  OACUS);
	LOC_GET_CHAR_ISO_(uAcuS,  UACUS);
	LOC_GET_CHAR_ISO_(aAcuS,  AACUS);
	LOC_GET_CHAR_ISO_(eAcuS,  EACUS);
	LOC_GET_CHAR_ISO_(iAcuS,  IACUS);
	LOC_GET_CHAR_ISO_(yAcuS,  YACUS);
	/** */
	LOC_GET_CHAR_ISO_(invExM, IEXCM);
	LOC_GET_CHAR_ISO_(invQuM, IQUEM);
	/** */
	LOC_GET_CHAR_ISO_(copySg, COPY);
	LOC_GET_CHAR_ISO_(regSg,  REG);
	/* these are only for U16-encoded strings: */
	LOC_GET_CHAR_U16_(umOe3C_u, 0x52, 0x01);
	LOC_GET_CHAR_U16_(umOe3S_u, 0x53, 0x01);
#	undef LOC_GET_CHAR_ISO_
#	undef LOC_GET_CHAR_U16_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get original string encoding
 */
Tst_mtes_textEnc
st_mtes_getEncodingOrg(ST_OPTARG(const Tst_mtes_string *pStr))
{
	if (pStr == NULL || pStr->pObInternal == NULL)
		return ST_MTES_TE_NONE;

	return ((Tst_mtes__string_intn*)pStr->pObInternal)->encOrg;
}

/*
 * Get current string encoding
 */
Tst_mtes_textEnc
st_mtes_getEncodingCur(ST_OPTARG(const Tst_mtes_string *pStr))
{
	if (pStr == NULL || pStr->pObInternal == NULL)
		return ST_MTES_TE_NONE;

	return ((Tst_mtes__string_intn*)pStr->pObInternal)->encCur;
}

/*
 * Get size of string
 */
Tst_uint32
st_mtes_getStringSize(ST_OPTARG(const Tst_mtes_string *pStr))
{
	if (pStr == NULL || pStr->pObInternal == NULL)
		return 0;

	return ((Tst_mtes__string_intn*)pStr->pObInternal)->strSz;
}

/*
 * Get data pointer
 */
const Tst_buf*
st_mtes_getDataPt(ST_OPTARG(const Tst_mtes_string *pStr))
{
	if (pStr == NULL || pStr->pObInternal == NULL)
		return NULL;

	return (const Tst_buf*)((Tst_mtes__string_intn*)pStr->pObInternal)->pData;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Copy MTE-string to simple char pointer string
 *
 * pStr    may be NULL
 * ppChar  MUST be != NULL
 * pOutSz  may be NULL (but shouldn't if pStr->encCur is UTF16)
 *
 * returns: ERRCODE
 *          *ppChar will contain string encoded in pStr->encCur
 *          if pOutSz!=NULL it will contain the new buffer size of *ppChar
 */
Tst_err
st_mtes_copyToCharp(ST_OPTARG(const Tst_mtes_string *pStr),
		Tst_str **ppChar, ST_OPTARG(Tst_uint32 *pOutSz))
{
	Tst_mtes__string_intn *pSI = NULL;

	ST_ASSERTN_IARG(ppChar == NULL)

	if (pStr != NULL)
		pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	if (pOutSz != NULL)
		*pOutSz = 0;
	if (pSI != NULL)
		ST_REALLOC(*ppChar, Tst_str*, pSI->strSz + 1, 1)
	else
		ST_REALLOC(*ppChar, Tst_str*, 1, 1)
	if (*ppChar == NULL)
		return ST_ERR_OMEM;
	if (pSI != NULL && pSI->strSz > 0 && pSI->pData != NULL)
		memcpy(*ppChar, pSI->pData, pSI->strSz);
	else
		(*ppChar)[0] = 0;
	if (pOutSz != NULL) {
		if (pSI != NULL)
			*pOutSz = st_mtes_getStrBOMsz(pStr) + st_mtes_strlenBy(pStr) +
					st_mtes_getCharWidth(pSI->encCur);
		else
			*pOutSz = 1;
	}
	return ST_ERR_SUCC;
}

Tst_err
st_mtes_copyToCharp_iso(ST_OPTARG(const Tst_mtes_string *pStr),
		Tst_str **ppCharISO)
{
	Tst_err    res;
	Tst_uint32 outSz = 0;
	Tst_mtes_textEnc encCur;

	res = st_mtes_copyToCharp(pStr, ppCharISO, &outSz);
	if (res != ST_ERR_SUCC)
		return res;

	if (pStr != NULL && pStr->pObInternal != NULL) {
		encCur = ((Tst_mtes__string_intn*)pStr->pObInternal)->encCur;
		switch (encCur) {
		case ST_MTES_TE_UTF8:
		case ST_MTES_TE_UTF16LE:
		case ST_MTES_TE_UTF16BE:
			res = st_mtes_convCharp(*ppCharISO, outSz,
					encCur, ST_MTES_TE_ISO, ppCharISO, NULL);
			break;
		default:  /* it is already ISO */
			res = ST_ERR_SUCC;
		}
	}
	return res;
}

Tst_err
st_mtes_copyToCharp_utf8(ST_OPTARG(const Tst_mtes_string *pStr),
		Tst_str **ppCharU8)
{
	Tst_err    res;
	Tst_uint32 outSz = 0;
	Tst_mtes_textEnc encCur;

	res = st_mtes_copyToCharp(pStr, ppCharU8, &outSz);
	if (res != ST_ERR_SUCC)
		return res;

	if (pStr != NULL && pStr->pObInternal != NULL) {
		encCur = ((Tst_mtes__string_intn*)pStr->pObInternal)->encCur;
		switch (encCur) {
		case ST_MTES_TE_ISO:
		case ST_MTES_TE_UTF16LE:
		case ST_MTES_TE_UTF16BE:
			res = st_mtes_convCharp(*ppCharU8, outSz,
					encCur, ST_MTES_TE_UTF8, ppCharU8, NULL);
			break;
		default:  /* it is already UTF8 */
			res = ST_ERR_SUCC;
		}
	}
	return res;
}

/*
 * Copy simple char pointer string to MTE-string
 *
 * pChar     may be NULL
 * pcharSz   MUST contain the size of pChar
 * pStr      MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_copyFromCharp(ST_OPTARG(const Tst_str *pChar),
		const Tst_uint32 pcharSz,
		const Tst_mtes_textEnc srcTE, Tst_mtes_string *pStr)
{
	Tst_uint32 cwid;
	Tst_mtes__string_intn *pSI;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
	cwid        = st_mtes_getCharWidth(srcTE);
	pSI->encOrg = srcTE;
	pSI->encCur = srcTE;
	if (! st_mtes_isTextEncValid(pSI->encCur))
		pSI->encCur = ST_MTES_TE_ISO;  /* set to default */
	if (pcharSz > cwid) {
		ST_REALLOC(pSI->pData, Tst_str*, pcharSz + 2, 1)
		if (pSI->pData == NULL)
			return ST_ERR_OMEM;
		pSI->strSz = pcharSz;
		if (pChar != NULL)
			memcpy(pSI->pData, pChar, pSI->strSz);
		/* */
		if (cwid == 2)
			pSI->pData[pcharSz - 2] = 0;
		pSI->pData[pcharSz - 1] = 0;
	}
	return ST_ERR_SUCC;
}

Tst_err
st_mtes_copyFromCharp_iso(ST_OPTARG(const Tst_str *pCharISO),
		Tst_mtes_string *pStr)
{
	return st_mtes_copyFromCharp(pCharISO,
			(pCharISO != NULL ? st_mtes_strlenCharpBy_iso_utf8(pCharISO) + 1 : 0),
			ST_MTES_TE_ISO, pStr);
}

Tst_err
st_mtes_copyFromCharp_utf8(ST_OPTARG(const Tst_str *pCharU8),
		Tst_mtes_string *pStr)
{
	return st_mtes_copyFromCharp(pCharU8,
			(pCharU8 != NULL ? st_mtes_strlenCharpBy_iso_utf8(pCharU8) + 1 : 0),
			ST_MTES_TE_UTF8, pStr);
}

Tst_err
st_mtes_copyFromCharp_isoOrUtf8(ST_OPTARG(const Tst_str *pChar),
		const Tst_bool isISOorUTF8, Tst_mtes_string *pStr)
{
	return st_mtes_copyFromCharp(pChar,
			(pChar != NULL ? st_mtes_strlenCharpBy_iso_utf8(pChar) + 1 : 0),
			(isISOorUTF8 ? ST_MTES_TE_ISO : ST_MTES_TE_UTF8), pStr);
}

/*----------------------------------------------------------------------------*/

/*
 * Set string to value of pStrVal in the appropriate encoding
 *
 * pStr       MUST be set
 * pCharISO   may be NULL but if != NULL encoded in ISO
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_setStrTo(const Tst_mtes_textEnc targTE,
		ST_OPTARG(const Tst_str *pCharISO),
		Tst_mtes_string *pStr)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_mtes__string_intn *pSI;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
	pSI->encOrg = targTE;
	if (! st_mtes_isTextEncValid(pSI->encOrg))
		pSI->encOrg = ST_MTES_TE_ISO;  /* set to default */
	pSI->encCur = ST_MTES_TE_ISO;
	if (pCharISO == NULL) {
		pSI->encCur = pSI->encOrg;
		return ST_ERR_SUCC;
	}
	pSI->strSz = st_mtes_strlenCharpBy_iso_utf8(pCharISO) + 1;
	ST_REALLOC(pSI->pData, Tst_str*, pSI->strSz + 1, 1)
	if (pSI->pData == NULL)
		return ST_ERR_OMEM;
	strcpy((char*)pSI->pData, (char*)pCharISO);
	/* */
	switch (pSI->encOrg) {
	case ST_MTES_TE_UTF8:
		res = st_mtes_convStrTo_utf8(pStr);
		break;
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		res = st_mtes_convStrTo_utf16(pStr, pSI->encOrg);
		break;
	default:
		/* it is already ISO */
		res = ST_ERR_SUCC;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert char pointer string into other encoding
 *   pCharIn may be the same pointer as *ppCharOut
 *
 * pCharIn    MUST be != NULL
 * szIn       MUST contain the buffer size of pCharIn
 * teIn       MUST contain the encoding of pCharIn
 * ppCharOut  MUST be != NULL
 * pSzOut     may be NULL (but shouldn't if teOut is UTF16)
 *
 * returns: ERRCODE
 *          *ppCharOut will contain string encoded in teOut
 *          if pSzOut!=NULL it will contain the new buffer size of *ppCharOut
 */
Tst_err
st_mtes_convCharp(const Tst_str *pCharIn, const Tst_uint32 szIn,
		const Tst_mtes_textEnc teIn, const Tst_mtes_textEnc teOut,
		Tst_str **ppCharOut,
		ST_OPTARG(Tst_uint32 *pSzOut))
{
	Tst_err res;
	Tst_mtes_string temp;

	ST_ASSERTN_IARG(pCharIn == NULL || ppCharOut == NULL)

	if (pSzOut != NULL)
		*pSzOut = 0;
	res = st_mtes_stc_initStr(&temp);
	if (res == ST_ERR_SUCC)
		res = st_mtes_copyFromCharp(pCharIn, szIn, teIn, &temp);
	if (res == ST_ERR_SUCC)
		res = st_mtes_convStr(&temp, teOut);
	if (res == ST_ERR_SUCC)
		res = st_mtes_copyToCharp(&temp, ppCharOut, pSzOut);
	st_mtes_stc_freeStr(&temp);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert any string type to any other type but leave pStr->encOrg untouched
 *
 * pStr  MUST be set
 *
 * returns: ERRCODE
 *          pStr will contain the converted string
 */
Tst_err
st_mtes_convStr(Tst_mtes_string *pStr, const Tst_mtes_textEnc targTE)
{
	Tst_err res = ST_ERR_SUCC;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	if (((Tst_mtes__string_intn*)pStr->pObInternal)->encCur != targTE)
		switch (targTE) {
		case ST_MTES_TE_UTF8:
			res = st_mtes_convStrTo_utf8(pStr); break;
		case ST_MTES_TE_UTF16LE:
		case ST_MTES_TE_UTF16BE:
			res = st_mtes_convStrTo_utf16(pStr, targTE); break;
		default:  /* ISO */
			res = st_mtes_convStrTo_iso(pStr);
		}
	return res;
}

Tst_err
st_mtes_convStr_c(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest,
		const Tst_mtes_textEnc targTE)
{
	switch (targTE) {
	case ST_MTES_TE_UTF8:
		return st_mtes_convStrTo_utf8_c(pSrc, pDest);
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		return st_mtes_convStrTo_utf16_c(pSrc, pDest, targTE);
	default:  /* ISO */
		return st_mtes_convStrTo_iso_c(pSrc, pDest);
	}
}

/*
 * Convert any string type to ISO string but leave pStr->encOrg untouched
 *
 * pStr  MUST be set
 *
 * returns: ERRCODE
 *          pStr will contain the converted string
 */
Tst_err
st_mtes_convStrTo_iso(Tst_mtes_string *pStr)
{
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 delta = 0;
	Tst_str    *pTmp = NULL;
	Tst_mtes__string_intn *pSI;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	if (st_mtes_isStrEmptyCh(pStr)) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_TRUE);
		pSI->encCur = ST_MTES_TE_ISO;
		return ST_ERR_SUCC;
	}
	if (pSI->encCur == ST_MTES_TE_ISO)
		return ST_ERR_SUCC;
	switch (pSI->encCur) {
	case ST_MTES_TE_UTF8:
		pSI->strSz = st_utf_convStr_utf8_to_iso(pSI->pData, pSI->strSz,
				ST_B_FALSE, &pTmp);
		ST_DELPOINT(pSI->pData)
		if (pSI->strSz > 0)
			pSI->pData = pTmp;
		pTmp = NULL;
		break;
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		delta      = st_mtes_getStrBOMsz(pStr);
		pSI->strSz = st_utf_convStr_utf16_to_iso(&pSI->pData[delta],
				pSI->strSz - delta, ST_B_FALSE, &pTmp,
				ST_MAP_TV2_TE2BO(pSI->encCur));
		ST_DELPOINT(pSI->pData)
		if (pSI->strSz > 0)
			pSI->pData = pTmp;
		pTmp = NULL;
		break;
	default:  /* should be impossible to reach */
		res = ST_ERR_FAIL;
	}
	if (res == ST_ERR_FAIL || pSI->strSz < 1) {
		ST_DELPOINT(pSI->pData)
		res = ST_ERR_FAIL;
	} else
		pSI->encCur = ST_MTES_TE_ISO;
	return res;
}

Tst_err
st_mtes_convStrTo_iso_c(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest)
{
	Tst_err res;

	ST_ASSERTN_IARG(pSrc == NULL || pDest == NULL ||
			pSrc == pDest)

	res = st_mtes_strCpy(pSrc, pDest);
	if (res != ST_ERR_SUCC ||
			((Tst_mtes__string_intn*)pDest->pObInternal)->encCur == ST_MTES_TE_ISO)
		return res;
	return st_mtes_convStrTo_iso(pDest);
}

/*
 * Convert any string type to UTF8 string but leave pStr->encOrg untouched
 *
 * pStr  MUST be set
 *
 * returns: ERRCODE
 *          pStr will contain the converted string
 */
Tst_err
st_mtes_convStrTo_utf8(Tst_mtes_string *pStr)
{
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 delta = 0;
	Tst_str    *pTmp = NULL;
	Tst_mtes__string_intn *pSI;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	if (st_mtes_isStrEmptyCh(pStr)) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_TRUE);
		pSI->encCur = ST_MTES_TE_UTF8;
		return ST_ERR_SUCC;
	}
	if (pSI->encCur == ST_MTES_TE_UTF8)
		return ST_ERR_SUCC;
	switch (pSI->encCur) {
	case ST_MTES_TE_ISO:
		pSI->strSz = st_utf_convStr_iso_to_utf8(pSI->pData, pSI->strSz,
				ST_B_FALSE, &pTmp);
		ST_DELPOINT(pSI->pData)
		if (pSI->strSz > 0)
			pSI->pData = pTmp;
		pTmp = NULL;
		break;
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		delta      = st_mtes_getStrBOMsz(pStr);
		pSI->strSz = st_utf_convStr_utf16_to_utf8(&pSI->pData[delta],
				pSI->strSz - delta, ST_B_FALSE, &pTmp,
				ST_MAP_TV2_TE2BO(pSI->encCur));
		ST_DELPOINT(pSI->pData)
		if (pSI->strSz > 0)
			pSI->pData = pTmp;
		pTmp = NULL;
		break;
	default:  /* should be impossible to reach */
		res = ST_ERR_FAIL;
	}
	if (res == ST_ERR_FAIL || pSI->strSz < 1) {
		ST_DELPOINT(pSI->pData)
		res = ST_ERR_FAIL;
	} else
		pSI->encCur = ST_MTES_TE_UTF8;
	return res;
}

Tst_err
st_mtes_convStrTo_utf8_c(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest)
{
	Tst_err res;

	ST_ASSERTN_IARG(pSrc == NULL || pDest == NULL ||
			pSrc == pDest)

	res = st_mtes_strCpy(pSrc, pDest);
	if (res != ST_ERR_SUCC ||
			((Tst_mtes__string_intn*)pDest->pObInternal)->encCur == ST_MTES_TE_UTF8)
		return res;
	return st_mtes_convStrTo_utf8(pDest);
}

/*
 * Convert any string type to UTF16 string but leave pStr->encOrg untouched
 *
 * pStr     MUST be set
 * targTE   MUST be ST_MTES_TE_UTF16LE or ST_MTES_TE_UTF16BE
 *
 * returns: ERRCODE
 *          pStr will contain the converted string
 */
Tst_err
st_mtes_convStrTo_utf16(Tst_mtes_string *pStr, const Tst_mtes_textEnc targTE)
{
#	define LOC_WITH_UTF_BOM_  0  /* 1:always prepend BOM to UTF strings */
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 delta = 0;
	Tst_str    *pTmp = NULL;
	Tst_mtes__string_intn *pSI;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL ||
			! (targTE == ST_MTES_TE_UTF16LE ||
				targTE == ST_MTES_TE_UTF16BE))

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	if (st_mtes_isStrEmptyCh(pStr)) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_TRUE);
		pSI->encCur = targTE;
		return ST_ERR_SUCC;
	}
	if (pSI->encCur == targTE)
		return ST_ERR_SUCC;
	switch (pSI->encCur) {
	case ST_MTES_TE_UTF8:
		pSI->strSz = st_utf_convStr_utf8_to_utf16(pSI->pData, pSI->strSz,
				ST_B_FALSE, &pTmp, ST_MAP_TV2_TE2BO(targTE));
		break;
	case ST_MTES_TE_UTF16LE:  /* default for UTF16 */
	case ST_MTES_TE_UTF16BE:
		delta      = st_mtes_getStrBOMsz(pStr);
		pSI->strSz = st_utf_convStr_utf16_to_utf16(&pSI->pData[delta],
				pSI->strSz - delta, ST_B_FALSE, &pTmp,
				ST_MAP_TV2_TE2BO(pSI->encCur), ST_MAP_TV2_TE2BO(targTE));
		break;
	default:  /* ISO */
		pSI->strSz = st_utf_convStr_iso_to_utf16(pSI->pData, pSI->strSz,
				ST_B_FALSE, &pTmp, ST_MAP_TV2_TE2BO(targTE));
	}

	ST_DELPOINT(pSI->pData)
	if (pSI->strSz == 0) {
		res = ST_ERR_FAIL;
		ST_DELPOINT(pTmp)
	} else {
		pSI->encCur = targTE;
		pSI->pData  = pTmp;
		pTmp        = NULL;
#		if (LOC_WITH_UTF_BOM_ == 1)
		ST_REALLOC(pSI->pData, Tst_str*,
				pSI->strSz + 2 + 1, 1)
		if (pSI->pData == NULL)
			return ST_ERR_OMEM;
		memmove(&pSI->pData[2], pSI->pData,
				pSI->strSz);
		if (pSI->encCur == ST_MTES_TE_UTF16LE)
			memcpy(pSI->pData, &ST_UTF16_BOM_LE, 2);
		else
			memcpy(pSI->pData, &ST_UTF16_BOM_BE, 2);
		pSI->strSz += 2;
#		endif
	}
	return res;
#	undef LOC_WITH_UTF_BOM_
}

Tst_err
st_mtes_convStrTo_utf16_c(const Tst_mtes_string *pSrc,
		Tst_mtes_string *pDest,
		const Tst_mtes_textEnc targTE)
{
	Tst_err res;

	ST_ASSERTN_IARG(pSrc == NULL || pDest == NULL ||
			pSrc == pDest)

	res = st_mtes_strCpy(pSrc, pDest);
	if (res != ST_ERR_SUCC ||
			((Tst_mtes__string_intn*)pDest->pObInternal)->encCur == targTE)
		return res;
	return st_mtes_convStrTo_utf16(pDest, targTE);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_mtes_int32ToStr(const Tst_int32 int32, Tst_mtes_string *pStr)
{
	char tmpNS[64];

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	snprintf(tmpNS, sizeof(tmpNS), "%d", int32);
	return st_mtes_copyFromCharp_iso((const Tst_str*)tmpNS, pStr);
}

Tst_err
st_mtes_uint32ToStr(const Tst_uint32 uint32, Tst_mtes_string *pStr)
{
	char tmpNS[64];

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	snprintf(tmpNS, sizeof(tmpNS), "%u", uint32);
	return st_mtes_copyFromCharp_iso((const Tst_str*)tmpNS, pStr);
}

Tst_int32
st_mtes_toInt32(const Tst_mtes_string *pStr)
{
	Tst_int32 resI = -1;

	ST_MTES__toInt(pStr, &resI, NULL);
	return resI;
}

Tst_uint32
st_mtes_toUInt32(const Tst_mtes_string *pStr)
{
	Tst_uint32 resUI = 0;

	ST_MTES__toInt(pStr, NULL, &resUI);
	return resUI;
}

/*----------------------------------------------------------------------------*/

/*
 * Simply check if the text-encoding is valid
 */
Tst_bool
st_mtes_isTextEncValid(const Tst_mtes_textEnc te)
{
	switch (te) {
	case ST_MTES_TE_ISO:
	case ST_MTES_TE_UTF8:
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		return ST_B_TRUE;
	default:
		return ST_B_FALSE;
	}
}

/*
 * Get char width for encoding
 *
 * returns: ISO/UTF8: 1
 *          UTF16   : 2
 */
Tst_uint32
st_mtes_getCharWidth(const Tst_mtes_textEnc te)
{
	switch (te) {
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		return 2;
	default:  /* ISO / UTF8 */
		return 1;
	}
}

/*
 * Get size of Byte-Order-Marker in UTF16 string
 *   If string is not UTF16 encoded or doesn't contain BOM,
 *   length 0 will be returned
 *
 * pStr  may be NULL
 *
 * returns: size of BOM (if present)
 */
Tst_uint32
st_mtes_getStrBOMsz(ST_OPTARG(const Tst_mtes_string *pStr))
{
	Tst_mtes__string_intn *pSI;

	if (pStr == NULL || pStr->pObInternal == NULL)
		return 0;

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	return st_mtes_getCharpBOMsz(pSI->pData, pSI->strSz, pSI->encCur);
}

Tst_uint32
st_mtes_getCharpBOMsz(ST_OPTARG(const Tst_str *pChar), const Tst_uint32 bufSz,
		const Tst_mtes_textEnc te)
{
	if (pChar == NULL)
		return 0;

	if ((te != ST_MTES_TE_UTF16LE && te != ST_MTES_TE_UTF16BE) ||
			bufSz < 2)
		return 0;
	if (te == ST_MTES_TE_UTF16BE &&
			pChar[0] == ST_UTF16_BOM_BE[0] && pChar[1] == ST_UTF16_BOM_BE[1])
		return 2;
	if (te == ST_MTES_TE_UTF16LE &&
			pChar[0] == ST_UTF16_BOM_LE[0] && pChar[1] == ST_UTF16_BOM_LE[1])
		return 2;
	return 0;
}

/*
 * Determines whether string is NUL-terminated
 *
 * pStr  may be NULL
 *
 * returns: true if NUL-term.
 */
Tst_bool
st_mtes_isStrNULterm(ST_OPTARG(const Tst_mtes_string *pStr))
{
	Tst_mtes__string_intn *pSI;

	if (pStr == NULL || pStr->pObInternal == NULL)
		return ST_B_FALSE;

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	switch (pSI->encCur) {
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		return st_utf_isStrNullTerm_u16(pSI->pData, pSI->strSz);
	default:  /* ISO / UTF8 */
		return st_utf_isStrNullTerm_iso_u8(pSI->pData, pSI->strSz);
	}
}

/*
 * Does string contain more than just BOM and NUL-Term ?
 *
 * pStr  may be NULL
 */
Tst_bool
st_mtes_isStrEmptyBy(ST_OPTARG(const Tst_mtes_string *pStr))
{
	Tst_bool   isterm = ST_B_FALSE;
	Tst_uint32 len;
	Tst_mtes__string_intn *pSI;

	if (pStr == NULL || pStr->pObInternal == NULL)
		return ST_B_TRUE;

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	if (pSI->pData == NULL || pSI->strSz < 2)
		return ST_B_TRUE;
	if (pSI->encCur == ST_MTES_TE_ISO || pSI->encCur == ST_MTES_TE_UTF8)
		return (pSI->pData[0] == 0x00);
	len = ST_MTES__strlen_safe(pSI->pData, pSI->strSz, pSI->encCur,
			ST_B_FALSE, ST_B_TRUE, &isterm, NULL);
	if (! isterm)
		len = 0;
	return (len < 1);
}

Tst_bool
st_mtes_isStrEmptyCh(ST_OPTARG(const Tst_mtes_string *pStr))
{
	Tst_bool   isterm = ST_B_FALSE;
	Tst_uint32 len;
	Tst_mtes__string_intn *pSI;

	if (pStr == NULL || pStr->pObInternal == NULL)
		return ST_B_TRUE;

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	if (pSI->pData == NULL || pSI->strSz < 2)
		return ST_B_TRUE;
	if (pSI->encCur == ST_MTES_TE_ISO)
		return (pSI->pData[0] == 0x00);
	len = ST_MTES__strlen_safe(pSI->pData, pSI->strSz, pSI->encCur,
			ST_B_TRUE, ST_B_TRUE, &isterm, NULL);
	if (! isterm)
		len = 0;
	return (len < 1);
}

/*----------------------------------------------------------------------------*/

/*
 * Test if char is alphabetical 'a'-'z' or 'A'-'Z'
 */
Tst_bool
st_mtes_isAlphaChar(const Tst_str ch,
		const Tst_bool allwUpper, const Tst_bool allwLower,
		Tst_bool *pIsUpperOrLower)
{
	Tst_byte wch[2];

	wch[0] = (Tst_byte)ch;
	wch[1] = 0;
	return st_mtes_isAlphaWChar(wch, ST_B_TRUE, allwUpper, allwLower,
			pIsUpperOrLower);
}

Tst_bool
st_mtes_isAlphaWChar(const Tst_byte wch[], const Tst_bool isLEorBE,
		const Tst_bool allwUpper, const Tst_bool allwLower,
		Tst_bool *pIsUpperOrLower)
{
	Tst_mtes_cset cset;

	ST_ASSERTN_BOOL(ST_B_FALSE, pIsUpperOrLower == NULL)

	st_mtes_stc_rsetCharSet(&cset, isLEorBE);
	return ST_MTES__isAlphaWChar(wch, &cset, allwUpper, allwLower,
			pIsUpperOrLower);
}

/*
 * Test if char is numerical '0'-'9'
 */
Tst_bool
st_mtes_isNumChar(const Tst_str ch)
{
	Tst_byte wch[2];

	wch[0] = (Tst_byte)ch;
	wch[1] = 0;
	return st_mtes_isNumWChar(wch, ST_B_TRUE);
}

Tst_bool
st_mtes_isNumWChar(const Tst_byte wch[], const Tst_bool isLEorBE)
{
	Tst_mtes_cset cset;

	st_mtes_stc_rsetCharSet(&cset, isLEorBE);
	return ST_MTES__isNumWChar(wch, &cset);
}

/*
 * Test if char is umlaut '&auml;', '&Auml;', ...
 */
Tst_bool
st_mtes_isUmlautChar(const Tst_str ch,
		const Tst_bool allwUpper, const Tst_bool allwLower,
		Tst_bool *pIsUpperOrLower)
{
	Tst_byte wch[2];

	wch[0] = (Tst_byte)ch;
	wch[1] = 0;
	return st_mtes_isUmlautWChar(wch, ST_B_TRUE, allwUpper, allwLower,
			pIsUpperOrLower);
}

Tst_bool
st_mtes_isUmlautWChar(const Tst_byte wch[], const Tst_bool isLEorBE,
		const Tst_bool allwUpper, const Tst_bool allwLower,
		Tst_bool *pIsUpperOrLower)
{
	Tst_mtes_cset cset;

	ST_ASSERTN_BOOL(ST_B_FALSE, pIsUpperOrLower == NULL)

	st_mtes_stc_rsetCharSet(&cset, isLEorBE);
	return ST_MTES__isUmlautWChar(wch, &cset, allwUpper, allwLower,
			pIsUpperOrLower);
}

/*----------------------------------------------------------------------------*/

/*
 * Get !length! of string in bytes (without BOM and NUL-Term)
 *
 * pStr  may be NULL, if not it MUST be NUL-terminated
 */
Tst_uint32
st_mtes_strlenBy(ST_OPTARG(const Tst_mtes_string *pStr))
{
	Tst_bool   isterm = ST_B_FALSE;
	Tst_uint32 len;
	Tst_mtes__string_intn *pSI;

	if (pStr == NULL || pStr->pObInternal == NULL)
		return 0;

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	len = ST_MTES__strlen_safe(pSI->pData, pSI->strSz, pSI->encCur,
			ST_B_FALSE, ST_B_FALSE, &isterm, NULL);
	if (! isterm)
		len = 0;
	return len;
}

/*
 * Get amount of !chars! in string (without BOM and NUL-Term)
 *
 * pStr  may be NULL, if not it MUST be NUL-terminated
 */
Tst_uint32
st_mtes_strlenCh(ST_OPTARG(const Tst_mtes_string *pStr))
{
	Tst_bool   isterm = ST_B_FALSE;
	Tst_uint32 len;
	Tst_mtes__string_intn *pSI;

	if (pStr == NULL || pStr->pObInternal == NULL)
		return 0;

	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;

	len = ST_MTES__strlen_safe(pSI->pData, pSI->strSz, pSI->encCur,
			ST_B_TRUE, ST_B_FALSE, &isterm, NULL);
	if (! isterm)
		len = 0;
	return len;
}

/*
 * Get !length! of string in bytes (without NUL-Term)
 *
 * pChar  may be NULL, if not it MUST be NUL-terminated
 */
Tst_uint32
st_mtes_strlenCharpBy_iso_utf8(ST_OPTARG(const Tst_str *pChar))
{
#	if (ST_MTES_USE_LIBC_STRLEN_ == 1)
	return (pChar ? strlen((char*)pChar) : 0);
#	else
	Tst_str const *pC = pChar;

	if (pChar == NULL)
		return 0;
	while (*pC)
		++pC;
	return (pC - pChar);
#	endif
}

/*
 * Get !length! of string in bytes (without BOM and NUL-Term)
 *
 * pChar  may be NULL, if not it MUST be NUL-terminated
 */
Tst_uint32
st_mtes_strlenCharpBy_utf16(ST_OPTARG(const Tst_str *pChar),
		const Tst_uint32 bufSz, const Tst_mtes_textEnc te)
{
	Tst_bool   isterm = ST_B_FALSE;
	Tst_uint32 len;

	len = ST_MTES__strlen_safe(pChar, bufSz, te,
			ST_B_FALSE, ST_B_FALSE, &isterm, NULL);
	if (! isterm)
		len = 0;
	return len;
}

Tst_uint32
st_mtes_strlenCharpCh_iso(ST_OPTARG(const Tst_str *pChar))
{
	return st_mtes_strlenCharpBy_iso_utf8(pChar);
}

/*
 * Get amount of !chars! in string (without NUL-Term)
 *
 * pChar  may be NULL, if not it MUST be NUL-terminated
 */
Tst_uint32
st_mtes_strlenCharpCh_utf8(ST_OPTARG(const Tst_str *pChar))
{
	if (pChar == NULL)
		return 0;
	return st_utf_strlen_utf8(pChar,
			st_mtes_strlenCharpBy_iso_utf8(pChar) + 1, ST_B_FALSE);
}

/*
 * Get amount of !chars! in string (without BOM and NUL-Term)
 *
 * pChar  may be NULL, if not it MUST be NUL-terminated
 */
Tst_uint32
st_mtes_strlenCharpCh_utf16(ST_OPTARG(const Tst_str *pChar),
		const Tst_uint32 bufSz, const Tst_mtes_textEnc te)
{
	Tst_bool isterm = ST_B_FALSE;

	return ST_MTES__strlen_safe(pChar, bufSz, te,
			ST_B_TRUE, ST_B_FALSE, &isterm, NULL);
}

/*
 * Get !length! of string in bytes (without NUL-Term)
 *
 * pCharBuf   may be NULL, if not it may be NUL-terminated
 * pHasTerm   may be NULL
 */
Tst_uint32
st_mtes_strlenCharpBy_safe_iso_utf8(ST_OPTARG(const Tst_str *pCharBuf),
		const Tst_uint32 bufSz, ST_OPTARG(Tst_bool *pHasTerm))
{
	Tst_bool isterm = ST_B_FALSE,
	         *pHT   = (pHasTerm == NULL ? &isterm : pHasTerm);

	return ST_MTES__strlen_safe(pCharBuf, bufSz, ST_MTES_TE_ISO,
			ST_B_FALSE, ST_B_FALSE, pHT, NULL);
}

/*
 * Get !length! of string in bytes (without BOM and NUL-Term)
 *
 * pChar     may be NULL, if not it may be NUL-terminated
 * pHasTerm  may be NULL
 * pHasBOM   may be NULL
 */
Tst_uint32
st_mtes_strlenCharpBy_safe_utf16(ST_OPTARG(const Tst_str *pCharBuf),
		const Tst_uint32 bufSz, const Tst_mtes_textEnc te,
		ST_OPTARG(Tst_bool *pHasTerm), ST_OPTARG(Tst_bool *pHasBOM))
{
	Tst_bool isterm = ST_B_FALSE,
	         hasb   = ST_B_FALSE,
	         *pIT   = (pHasTerm == NULL ? &isterm : pHasTerm),
	         *pHB   = (pHasBOM == NULL ? &hasb : pHasBOM);

	return ST_MTES__strlen_safe(pCharBuf, bufSz, te,
			ST_B_FALSE, ST_B_FALSE, pIT, pHB);
}

/*----------------------------------------------------------------------------*/

/*
 * Compare strings char-wise
 *   If compCasesens==ST_B_TRUE then compare case-sensitive
 *
 * pStr1   MUST be set
 * pStr2   MUST be set
 * pRelat  MUST be != NULL
 *
 * returns: ERRCODE
 *          pRelat will be <0 if pStr1 is 'smaller' than pStr2
 *                        ==0 if pStr1 equals pStr2
 *                         >0 if pStr1 is 'bigger' than pStr2
 */
Tst_err
st_mtes_strCompare(const Tst_mtes_string *pStr1, const Tst_mtes_string *pStr2,
		const Tst_bool compCasesens, Tst_int32 *pRelat)
{
	Tst_mtes_cset cset;

	st_mtes_stc_rsetCharSet(&cset, ST_B_TRUE);
	return ST_MTES__strcompare_n(pStr1, pStr2, &cset, compCasesens, -1, pRelat);
}

Tst_bool
st_mtes_strEqual(const Tst_mtes_string *pStr1, const Tst_mtes_string *pStr2,
		const Tst_bool compCasesens)
{
	Tst_int32 relat = -1;
	Tst_mtes_cset cset;

	st_mtes_stc_rsetCharSet(&cset, ST_B_TRUE);
	return (ST_MTES__strcompare_n(pStr1, pStr2, &cset, compCasesens,
			-1, &relat) == ST_ERR_SUCC && relat == 0);
}

/*
 * Compare strings char-wise, but only compare first n chars
 *   If compCasesens==ST_B_TRUE then compare case-sensitive
 *
 * pStr1    MUST be set
 * pStr2    MUST be set
 * pRelat   MUST be != NULL
 *
 * returns: ERRCODE
 *          pRelat will be <0 if pStr1 is 'smaller' than pStr2
 *                        ==0 if pStr1 equals pStr2
 *                         >0 if pStr1 is 'bigger' than pStr2
 */
Tst_err
st_mtes_strNCompare(const Tst_mtes_string *pStr1, const Tst_mtes_string *pStr2,
		const Tst_bool compCasesens, const Tst_uint32 maxChars,
		Tst_int32 *pRelat)
{
	Tst_mtes_cset cset;

	st_mtes_stc_rsetCharSet(&cset, ST_B_TRUE);
	return ST_MTES__strcompare_n(pStr1, pStr2, &cset, compCasesens,
			(Tst_int32)maxChars, pRelat);
}

Tst_bool
st_mtes_strNEqual(const Tst_mtes_string *pStr1, const Tst_mtes_string *pStr2,
		const Tst_bool compCasesens, const Tst_uint32 maxChars)
{
	Tst_int32 relat = -1;
	Tst_mtes_cset cset;

	st_mtes_stc_rsetCharSet(&cset, ST_B_TRUE);
	return (ST_MTES__strcompare_n(pStr1, pStr2, &cset, compCasesens,
			(Tst_int32)maxChars, &relat) == ST_ERR_SUCC && relat == 0);
}

/*----------------------------------------------------------------------------*/

/*
 * Replace pReplISO with pWithISO in pStr
 *
 * pStr      MUST be set
 * pReplISO  MUST be != NULL
 * pWithISO  may be empty or NULL --> remove pRepl from pStr
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strReplace(Tst_mtes_string *pStr,
		const Tst_str *pReplISO, ST_OPTARG(const Tst_str *pWithISO),
		ST_OPTARG(Tst_bool *pModded))
{
	const Tst_mtes_textEnc cTE_INT = ST_MTES_TE_UTF16LE;
	Tst_err    res;
	Tst_bool   chgd = ST_B_FALSE;
	Tst_uint32 lenr = 0,
	           lenw = 0;
	Tst_mtes_string  tmps,
	                 tmpr,
	                 *pS   = pStr;
	Tst_mtes_textEnc teOrg = ST_MTES_TE_ISO;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL || pReplISO == NULL)

	if (pModded != NULL)
		*pModded = ST_B_FALSE;

	res = st_mtes_stc_initStr(&tmps);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&tmpr);
	if (res != ST_ERR_SUCC)
		return res;

	res = st_mtes_setStrTo(cTE_INT, pReplISO, &tmpr);
	if (res == ST_ERR_SUCC)
		lenr = st_mtes_strlenBy(&tmpr);

	if (res == ST_ERR_SUCC && lenr > 0) {
		teOrg = ((Tst_mtes__string_intn*)pStr->pObInternal)->encCur;
		if (teOrg != cTE_INT) {
			pS  = &tmps;
			res = st_mtes_convStr_c(pStr, pS, cTE_INT);
		}
	}
	/* tmpr and pS are now both encoded as cTE_INT */

	if (res == ST_ERR_SUCC && lenr > 0 && lenr <= st_mtes_strlenBy(pS)) {
		Tst_str *pWU16 = NULL;
		Tst_mtes_string tmpw;

		res = st_mtes_stc_initStr(&tmpw);
		if (res == ST_ERR_SUCC && pWithISO != NULL) {
			res = st_mtes_setStrTo(cTE_INT, pWithISO, &tmpw);
			if (res == ST_ERR_SUCC)
				lenw = st_mtes_strlenBy(&tmpw);
			if (lenw > 0)
				pWU16 = ((Tst_mtes__string_intn*)tmpw.pObInternal)->pData;
		}
		/* pWU16 is now also encoded as cTE_INT or pWU16==NULL */

		if (res == ST_ERR_SUCC) {
			Tst_uint32 skipN = 0,
			           bomR  = 0,
			           bomW  = 0;
			Tst_mtes__string_intn *pSint,
			                      *pTmpRint;

			pSint    = (Tst_mtes__string_intn*)pS->pObInternal;
			pTmpRint = (Tst_mtes__string_intn*)tmpr.pObInternal;

			skipN = st_mtes_getStrBOMsz(pS);
			bomR  = st_mtes_getStrBOMsz(&tmpr);
			bomW  = st_mtes_getStrBOMsz(&tmpw);
			res   = ST_MTES__bufReplace(&pSint->pData, &pSint->strSz,
					&pTmpRint->pData[bomR], lenr, skipN,
					(pWU16 != NULL ? &pWU16[bomW] : NULL), lenw, &chgd);
		}
		st_mtes_stc_freeStr(&tmpw);
	}
	/* convert pStr back to teOrg */
	if (res == ST_ERR_SUCC && chgd && teOrg != cTE_INT)
		res = st_mtes_convStr_c(&tmps, pStr, teOrg);

	if (pModded != NULL)
		*pModded = chgd;

	st_mtes_stc_freeStr(&tmps);
	st_mtes_stc_freeStr(&tmpr);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Search for a substring
 *
 * returns: true if found
 *          if pOffsCh is != NULL then it will contain the position of
 *            pNeedleISO in pStr as index of the first char
 */
Tst_bool
st_mtes_strPos(const Tst_mtes_string *pStr, const Tst_str *pNeedleISO,
		ST_OPTARG(Tst_uint32 *pOffsCh))
{
	const Tst_mtes_textEnc cTE_INT = ST_MTES_TE_UTF16LE;
	const Tst_uint32       cTE_DIV = 2;
	Tst_bool   resB = ST_B_FALSE;
	Tst_err    resF;
	Tst_uint32 lenN = 0;
	Tst_mtes_string       tmpStr,
	                      tmpNee;
	Tst_mtes_string const *pS = pStr;

	ST_ASSERTN_BOOL(ST_B_FALSE, pStr == NULL || pNeedleISO == NULL)

	if (pOffsCh != NULL)
		*pOffsCh = 0;

	resF = st_mtes_stc_initStr(&tmpStr);
	if (resF == ST_ERR_SUCC)
		resF = st_mtes_stc_initStr(&tmpNee);
	if (resF != ST_ERR_SUCC)
		return ST_B_FALSE;

	resF = st_mtes_setStrTo(cTE_INT, pNeedleISO, &tmpNee);
	if (resF == ST_ERR_SUCC)
		lenN = st_mtes_strlenBy(&tmpNee);

	if (resF == ST_ERR_SUCC && lenN > 0 &&
			((Tst_mtes__string_intn*)pStr->pObInternal)->encCur != cTE_INT) {
		pS   = &tmpStr;
		resF = st_mtes_convStr_c(pStr, &tmpStr, cTE_INT);
	}
	/* tmpNee and pS are now both encoded as cTE_INT */

	if (resF == ST_ERR_SUCC && lenN > 0 && lenN <= st_mtes_strlenBy(pS)) {
		Tst_uint32 bomS;
		Tst_mtes__string_intn *pSint,
		                      *pNint;

		pSint = (Tst_mtes__string_intn*)pS->pObInternal;
		pNint = (Tst_mtes__string_intn*)tmpNee.pObInternal;

		bomS = st_mtes_getStrBOMsz(pS);
		resB = st_sysBufSearch(&pSint->pData[bomS], pSint->strSz - bomS,
					pNint->pData, lenN, pOffsCh);
		if (resB && pOffsCh != NULL)
			*pOffsCh = *pOffsCh / cTE_DIV;
	}

	st_mtes_stc_freeStr(&tmpStr);
	st_mtes_stc_freeStr(&tmpNee);
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Split up string into two halfs
 *   (e.g. str="E1;E2",sep=";" --> pLeft="E1", pRight="E2")
 */
Tst_err
st_mtes_strSplit(const Tst_mtes_string *pStr, const Tst_str *pSeparatorISO,
		Tst_mtes_string *pLeft, Tst_mtes_string *pRight)
{
	const Tst_mtes_textEnc cTE_INT = ST_MTES_TE_UTF16LE;
	const Tst_uint32       cTE_DIV = 2;
	Tst_err    res;
	Tst_uint32 slenStrBy,
	           slenSepCh;
	Tst_mtes_string const *pS = pStr;
	Tst_mtes_string       tmpS;
	Tst_mtes_textEnc      teOrg;

	ST_ASSERTN_IARG(pStr == NULL || pSeparatorISO == NULL ||
			pLeft == NULL || pRight == NULL)

	st_mtes_stc_rsetStr(pLeft, ST_B_TRUE, ST_B_TRUE);
	st_mtes_stc_rsetStr(pRight, ST_B_TRUE, ST_B_TRUE);

	slenSepCh = st_sysStrlen(pSeparatorISO);
	if (slenSepCh == 0)
		return st_mtes_strCpy(pStr, pLeft);

	res = st_mtes_stc_initStr(&tmpS);
	if (res != ST_ERR_SUCC)
		return res;

	teOrg = st_mtes_getEncodingCur(pStr);
	if (teOrg != cTE_INT) {
		res = st_mtes_convStr_c(pStr, &tmpS, cTE_INT);
		pS  = &tmpS;
	}
	/* pS is now encoded as cTE_INT */

	slenStrBy = st_mtes_strlenBy(pS);
	if (res == ST_ERR_SUCC && slenStrBy > 0) {
		Tst_uint32 offs,
		           toCp;

		/* search for separator */
		offs = 0;
		if (! st_mtes_strPos(pS, pSeparatorISO, &offs))
			offs = (slenStrBy / cTE_DIV);  /* e.g. UTF16 uses 2 bytes for one char */
		/* copy left half (str="E1;E2" --> pLeft="E1") */
		res = st_mtes_strNCpy(pS, pLeft, offs);
		/* remove left half and separator from pS
		 *   [aa;bbbb]  slenStrBy=14, toCp=7, offs=2, toCp=5, toCp=4
		 *   [aaaaaa;]  slenStrBy=14, toCp=7, offs=6, toCp=1, toCp=0
		 *   [;aaaaaa]  slenStrBy=14, toCp=7, offs=0, toCp=7, toCp=6
		 *   [aaaaaa]   slenStrBy=12, toCp=6, offs=6, toCp=0, toCp=0  */
		if (res == ST_ERR_SUCC) {
			/* copy right half (str="E1;E2" --> pRight="E2") */
			toCp  = (slenStrBy / cTE_DIV);  /* e.g. UTF16 uses 2 bytes for one char */
			toCp -= (offs < toCp ? offs : toCp);
			toCp -= (toCp >= slenSepCh ? slenSepCh : 0);
			res   = st_mtes_strRNCpy(pS, pRight, toCp);
		}
	}

	st_mtes_stc_freeStr(&tmpS);

	/* convert pLeft and pRight to teOrg */
	if (res == ST_ERR_SUCC && teOrg != cTE_INT)
		res = st_mtes_convStr(pLeft, teOrg);
	if (res == ST_ERR_SUCC && teOrg != cTE_INT)
		res = st_mtes_convStr(pRight, teOrg);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy from pSrc to pDest
 *
 * pSrc   MUST be set
 * pDest  MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strCpy(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest)
{
	Tst_mtes__string_intn *pSrcI,
	                      *pDestI;

	ST_ASSERTN_IARG(pSrc == NULL || pSrc->pObInternal == NULL ||
			pDest == NULL || pDest->pObInternal == NULL ||
			pSrc == pDest)

	st_mtes_stc_rsetStr(pDest, ST_B_TRUE, ST_B_TRUE);

	pSrcI  = (Tst_mtes__string_intn*)pSrc->pObInternal;
	pDestI = (Tst_mtes__string_intn*)pDest->pObInternal;

	pDestI->encOrg = pSrcI->encOrg;
	pDestI->encCur = pSrcI->encCur;
	if (! st_mtes_isTextEncValid(pDestI->encCur))
		pDestI->encCur = ST_MTES_TE_ISO;  /* set to default */
	if (pSrcI->pData != NULL && pSrcI->strSz > 0) {
		ST_REALLOC(pDestI->pData, Tst_str*, pSrcI->strSz, 1)
		if (pDestI->pData == NULL)
			return ST_ERR_OMEM;
		memcpy(pDestI->pData, pSrcI->pData, pSrcI->strSz);
		pDestI->strSz = pSrcI->strSz;
	}
	return ST_ERR_SUCC;
}

/*
 * Copy up to maxCharsToCp chars from pSrc to pDest
 *   maxCharsToCpFromLeft refers to chars and not to bytes !
 *   (e.g. pSrc="hello there", maxCh=5  --> pDest="hello")
 *
 * pSrc   MUST be set
 * pDest  MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strNCpy(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest,
		Tst_uint32 maxCharsToCpFromLeft)
{
	return ST_MTES__strcopy_n(pSrc, pDest, maxCharsToCpFromLeft, ST_B_TRUE);
}

/*
 * Copy up to maxCharsToCpFromRight chars from pSrc to pDest,
 *   but starting copying from the end of pSrc descending to the beginning
 *   maxCharsToCpFromRight refers to chars and not to bytes !
 *   (e.g. pSrc="hello there", maxCh=5  --> pDest="there")
 *
 * pSrc   MUST be set
 * pDest  MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strRNCpy(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest,
		Tst_uint32 maxCharsToCpFromRight)
{
	return ST_MTES__strcopy_n(pSrc, pDest, maxCharsToCpFromRight, ST_B_FALSE);
}

/*----------------------------------------------------------------------------*/

/*
 * Concatenate pSrc to pDest,  pDest will keep its encoding
 *   e.g. pDest="abcd", pSrc="efg" --> pDest="abcdefg"
 *
 * pSrc   MUST be set
 * pDest  MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strConcat(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest)
{
#	define LOC_FREESTR_  {if (pS == &tmpS) st_mtes_stc_freeStr(&tmpS); \
				if (pD == &tmpD) st_mtes_stc_freeStr(&tmpD);}
	const Tst_mtes_textEnc cTE_INT = ST_MTES_TE_UTF16LE;
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 dstBom = 0,
	           srcBom = 0,
	           lenS,
	           lenD,
	           cwid;
	Tst_mtes_string       tmpS,
	                      tmpD,
	                      *pD = &tmpD;
	Tst_mtes_string const *pS = &tmpS;
	Tst_mtes_textEnc      teD;
	Tst_mtes__string_intn *pSrcI,
	                      *pDestI;

	ST_ASSERTN_IARG(pSrc == NULL || pSrc->pObInternal == NULL ||
			pDest == NULL || pDest->pObInternal == NULL)

	if (st_mtes_isStrEmptyCh(pSrc))
		return ST_ERR_SUCC;
	pSrcI  = (Tst_mtes__string_intn*)pSrc->pObInternal;
	pDestI = (Tst_mtes__string_intn*)pDest->pObInternal;
	teD    = pDestI->encCur;
	if (st_mtes_isStrEmptyCh(pDest))
		return st_mtes_convStr_c(pSrc, pDest, teD);

	if (! (pSrcI->encCur == pDestI->encCur &&
			(pSrcI->encCur == ST_MTES_TE_ISO ||
				pSrcI->encCur == ST_MTES_TE_UTF16LE ||
				pSrcI->encCur == ST_MTES_TE_UTF16BE))) {
		if (pSrcI->encCur != cTE_INT) {
			res = st_mtes_stc_initStr(&tmpS);
			if (res != ST_ERR_SUCC)
				return res;
			res = st_mtes_convStr_c(pSrc, &tmpS, cTE_INT);
		} else
			pS = pSrc;
		if (res == ST_ERR_SUCC && pDestI->encCur != cTE_INT) {
			res = st_mtes_stc_initStr(&tmpD);
			if (res != ST_ERR_SUCC)
				return res;
			res = st_mtes_convStr_c(pDest, &tmpD, cTE_INT);
		} else
			pD = pDest;
		if (res != ST_ERR_SUCC) {
			LOC_FREESTR_
			return res;
		}
	} else {
		pS = pSrc;
		pD = pDest;
	}
	pSrcI  = (Tst_mtes__string_intn*)pS->pObInternal;
	pDestI = (Tst_mtes__string_intn*)pD->pObInternal;
	cwid   = st_mtes_getCharWidth(pDestI->encCur);
	lenS   = st_mtes_strlenBy(pS);
	lenD   = st_mtes_strlenBy(pD);
	if (cwid == 2) {
		srcBom = st_mtes_getStrBOMsz(pS);
		dstBom = st_mtes_getStrBOMsz(pD);
		lenD  += dstBom;
	}
	/* */
	pDestI->strSz = lenD + lenS + cwid;
	ST_REALLOC(pDestI->pData, Tst_str*, pDestI->strSz + 1, 1)
	if (pDestI->pData == NULL) {
		LOC_FREESTR_
		return ST_ERR_OMEM;
	}
	memcpy(&pDestI->pData[lenD], &pSrcI->pData[srcBom],
			lenS + cwid);
	/* */
	if (pD == &tmpD)
		res = st_mtes_convStr_c(&tmpD, pDest, teD);

	LOC_FREESTR_
	return res;
#	undef LOC_FREESTR_
}

/*----------------------------------------------------------------------------*/

/*
 * Shorten string for output
 *   (replaceEndOrCenter==TRUE:
 *      e.g. pStr="abcdefgh", maxCh=7, pPlaceh="..."   --> pStr="abcd..."
 *           pStr="abcdefgh", maxCh=8, pPlaceh="..."   --> pStr="abcdefgh"
 *           pStr="abc",      maxCh=3, pPlaceh="..."   --> pStr="..."
 *           pStr="abc",      maxCh=2, pPlaceh="..."   --> pStr=".."
 *           pStr="abc",      maxCh=2, pPlaceh=""      --> pStr="ab"
 *    replaceEndOrCenter==FALSE:
 *      e.g. pStr="abcdefgh", maxCh=7, pPlaceh="..."   --> pStr="ab...gh")
 *
 * pStr          MUST be set
 * maxChars      MUST contain the maximum total length of the output string
 * pPlaceholder  MUST be set
 */
Tst_err
st_mtes_shortenStr(Tst_mtes_string *pStr, const Tst_uint32 maxChars,
		const Tst_mtes_string *pPlaceholder, const Tst_bool replaceEndOrCenter)
{
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 lenps = 0,
	           elen;
	Tst_mtes_string tmpS;

	ST_ASSERTN_IARG(pStr == NULL || pPlaceholder == NULL)

	if (maxChars == 0) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
		return ST_ERR_SUCC;
	}
	if (st_mtes_strlenCh(pStr) <= maxChars)
		return ST_ERR_SUCC;
	/* */
	elen = st_mtes_strlenCh(pPlaceholder);
	if (maxChars > elen) {
		if (replaceEndOrCenter)
			lenps = maxChars - elen;
		else
			lenps = maxChars / 2 - elen / 2;
	}

	/* */
	res = st_mtes_stc_initStr(&tmpS);
	if (res != ST_ERR_SUCC)
		return res;

	/* */
	if (replaceEndOrCenter) {
		if (lenps > 0) {
			res = st_mtes_strNCpy(pStr, &tmpS, lenps);
			if (res == ST_ERR_SUCC)
				res = st_mtes_strConcat(pPlaceholder, &tmpS);
			if (res == ST_ERR_SUCC)
				res = st_mtes_strCpy(&tmpS, pStr);
		} else
			res = st_mtes_strRNCpy(pPlaceholder, pStr, maxChars);
	} else {
		if (lenps > 0)
			res = st_mtes_strRNCpy(pStr, &tmpS, lenps);
		if (res == ST_ERR_SUCC)
			res = st_mtes_shortenStr(pStr, lenps + elen,
					pPlaceholder, /*endOrCenter:*/ST_B_TRUE);
		if (res == ST_ERR_SUCC)
			res = st_mtes_strConcat(&tmpS, pStr);
	}

	st_mtes_stc_freeStr(&tmpS);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Replace umlauts with their plain ascii substitute
 *   (e.g. &auml; --> "ae")
 *
 * pStr     MUST be set
 * pModded  may be NULL
 */
Tst_err
st_mtes_replaceUmlauts(Tst_mtes_string *pStr, ST_OPTARG(Tst_bool *pModded))
{
#	define LOC_REPL_(toR, rW1, rW2, rW3, rW4, rW5, lenRW)  { \
				memset(&replW, 0, sizeof(replW)); \
				replW[0] = rW1; replW[2] = rW2; replW[4] = rW3; \
				replW[6] = rW4; replW[8] = rW5; \
				res = ST_MTES__bufReplace(&pSI->pData, &pSI->strSz, \
						(Tst_str*)&cset.toR, 2, skipN, \
						(Tst_str*)&replW, lenRW * 2, &chgd); \
				if (res != ST_ERR_SUCC) return res; \
				*pM = *pM | chgd; \
			}
	Tst_err    res  = ST_ERR_SUCC;
	Tst_bool   tM   = ST_B_FALSE,
	           *pM  = &tM,
	           chgd = ST_B_FALSE;
	Tst_uint32 skipN;
	Tst_str    replW[20];
	Tst_mtes_cset    cset;
	Tst_mtes_textEnc te;
	Tst_mtes__string_intn *pSI;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	if (st_mtes_isStrEmptyCh(pStr)) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
		return ST_ERR_SUCC;
	}
	pSI = (Tst_mtes__string_intn*)pStr->pObInternal;
	if (pModded != NULL)
		pM = pModded;
	te = pSI->encCur;
	/* */
	st_mtes_stc_rsetCharSet(&cset, ST_B_TRUE);
	if (te != ST_MTES_TE_UTF16LE) {
		res = st_mtes_convStr(pStr, ST_MTES_TE_UTF16LE);
		if (res != ST_ERR_SUCC)
			return res;
	}
	skipN = st_mtes_getStrBOMsz(pStr);
	/* */
	/** */
	LOC_REPL_(umAeC, 'A', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umOeC, 'O', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umUeC, 'U', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umAeS, 'a', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umOeS, 'o', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umUeS, 'u', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umSz, 's', 'z', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umAe2C, 'A', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umOe2C, 'O', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umAaC, 'A', 'a', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umAe2S, 'a', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umOe2S, 'o', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umAaS, 'a', 'a', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(nTildC, 'N', 'j', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(oAcuC, 'O', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(uAcuC, 'U', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(aAcuC, 'A', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(eAcuC, 'E', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(iAcuC, 'I', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(yAcuC, 'Y', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(nTildS, 'n', 'j', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(oAcuS, 'o', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(uAcuS, 'u', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(aAcuS, 'a', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(eAcuS, 'e', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(iAcuS, 'i', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(yAcuS, 'y', 0x00, 0x00, 0x00, 0x00, 1)
	/** */
	LOC_REPL_(invExM, '!', 0x00, 0x00, 0x00, 0x00, 1)
	LOC_REPL_(invQuM, '?', 0x00, 0x00, 0x00, 0x00, 1)
	/** */
	LOC_REPL_(copySg, 'C', 'o', 'p', 'y', 'r', 5)
	LOC_REPL_(regSg, 'R', 'e', 'g', 0x00, 0x00, 3)
	/** */
	LOC_REPL_(umOe3C_u, 'O', 'e', 0x00, 0x00, 0x00, 2)
	LOC_REPL_(umOe3S_u, 'o', 'e', 0x00, 0x00, 0x00, 2)

	/* */
	if (te != pSI->encCur)
		res = st_mtes_convStr(pStr, te);
	/* */
	return res;
#	undef LOC_REPL_
}

/*----------------------------------------------------------------------------*/

/*
 * Strip and correct string
 *   Rremoves useless leading/trailing whitespaces and
 *   replaces invalid characters like '\r'
 *   Not all restrictions apply to every string-encoding
 *
 * pStr   MUST be set and NUL-Terminated or have the size 0
 * pRestr, pHadInvChars and pModded may be NULL
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_stripStr(Tst_mtes_string *pStr,
		ST_OPTARG(const Tst_mtes_strRestr *pRestr),
		ST_OPTARG(Tst_bool *pHadInvChars), ST_OPTARG(Tst_bool *pModded))
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_bool   dummyM,
	           *pM = &dummyM,
	           dummyIC,
	           *pIC = &dummyIC,
	           wasU8;
	Tst_uint32 oldsz;
	Tst_mtes_strRestr       dummyR;
	Tst_mtes_strRestr const *pR = &dummyR;
	Tst_mtes__string_intn   *pSI;

	ST_ASSERTN_IARG(pStr == NULL || pStr->pObInternal == NULL)

	if (st_mtes_isStrEmptyCh(pStr)) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
		return ST_ERR_SUCC;
	}

	if (pRestr == NULL)
		st_mtes_stc_rsetRestr(&dummyR, ST_B_FALSE);
	else
		pR = pRestr;
	if (pModded != NULL)
		pM = pModded;
	if (pHadInvChars != NULL)
		pIC = pHadInvChars;
	*pM  = ST_B_FALSE;
	*pIC = ST_B_FALSE;
	/* */
	pSI   = (Tst_mtes__string_intn*)pStr->pObInternal;
	oldsz = pSI->strSz;
	wasU8 = (pSI->encCur == ST_MTES_TE_UTF8);
	if (wasU8) {
		if ((res = st_mtes_convStrTo_utf16(pStr, ST_MTES_TE_UTF16LE)) != ST_ERR_SUCC)
			return res;
	}
	res = ST_MTES__stripStr_iso_utf16(pStr, pR, pIC, pM);
	if (res != ST_ERR_SUCC)
		return res;
	if (wasU8) {
		if ((res = st_mtes_convStrTo_utf8(pStr)) != ST_ERR_SUCC)
			return res;
	}
	/* */
	if (st_mtes_isStrEmptyCh(pStr))
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
	else if (oldsz > pSI->strSz + 1024) {
		ST_REALLOC(pSI->pData, Tst_str*, pSI->strSz + 1, 1)
		if (pSI->pData == NULL)
			return ST_ERR_OMEM;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get amount of elements in string array
 *
 * pSArr  may be NULL
 */
Tst_uint32
st_mtes_strArrGetElemCount(ST_OPTARG(const Tst_mtes_strArr *pSArr))
{
	if (pSArr == NULL || pSArr->pObInternal == NULL)
		return 0;

	return st_dynlist_getElementCount(
			&((Tst_mtes__strArr_intn*)pSArr->pObInternal)->list);
}

/*----------------------------------------------------------------------------*/

/*
 * Get element from string array
 *   First element has index 1
 *
 * pSArr  may be NULL
 */
Tst_mtes_string*
st_mtes_strArrGetElem(ST_OPTARG(const Tst_mtes_strArr *pSArr),
		const Tst_uint32 ix)
{
	if (pSArr == NULL || pSArr->pObInternal == NULL || ix < 1)
		return NULL;

	return (Tst_mtes_string*)st_dynlist_getElement(
			&((Tst_mtes__strArr_intn*)pSArr->pObInternal)->list, ix);
}

/*----------------------------------------------------------------------------*/

/*
 * Add string to string array
 *
 * pSArr       MUST be set
 * pStrToAdd   MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strArrAdd(Tst_mtes_strArr *pSArr, const Tst_mtes_string *pStrToAdd)
{
	return ST_MTES__strArr_add(pSArr, pStrToAdd, NULL, NULL);
}

Tst_err
st_mtes_strArrAddCharp_iso(Tst_mtes_strArr *pSArr, const Tst_str *pCharToAddISO)
{
	return ST_MTES__strArr_add(pSArr, NULL, pCharToAddISO, NULL);
}

Tst_err
st_mtes_strArrAddCharp_utf8(Tst_mtes_strArr *pSArr, const Tst_str *pCharToAddU8)
{
	return ST_MTES__strArr_add(pSArr, NULL, NULL, pCharToAddU8);
}

/*----------------------------------------------------------------------------*/

/*
 * Remove string from array
 *   First element has index 1
 *
 * pSArr  MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strArrRem(Tst_mtes_strArr *pSArr, const Tst_uint32 ix)
{
	Tst_mtes__strArr_intn *pSAI;

	ST_ASSERTN_IARG(pSArr == NULL || pSArr->pObInternal == NULL || ix < 1)

	pSAI = (Tst_mtes__strArr_intn*)pSArr->pObInternal;

	if (ix > st_mtes_strArrGetElemCount(pSArr))
		return ST_ERR_SUCC;

	if (! st_dynlist_remElement(&pSAI->list, ix))
		return ST_ERR_FAIL;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy string array from pSArrSrc to pSArrDst
 *
 * pSArrSrc   MUST be set
 * pSArrDst   MUST be set
 */
Tst_err
st_mtes_strArrCpy(Tst_mtes_strArr *pSArrSrc, Tst_mtes_strArr *pSArrDst)
{
	Tst_mtes__strArr_intn *pSAIsrc,
	                      *pSAIdst;

	ST_ASSERTN_IARG(pSArrSrc == NULL || pSArrSrc->pObInternal == NULL ||
			pSArrDst == NULL || pSArrDst->pObInternal == NULL)

	pSAIsrc = (Tst_mtes__strArr_intn*)pSArrSrc->pObInternal;
	pSAIdst = (Tst_mtes__strArr_intn*)pSArrDst->pObInternal;

	return st_dynlist_clone(&pSAIsrc->list, &pSAIdst->list,
			ST_MTES__cbCloneInitElemStr, ST_MTES__cbCloneCopyElemStr);
}

/*----------------------------------------------------------------------------*/

/*
 * Strip string array, i.e. remove empty strings and duplicates
 *   If arePairs then strings will be compared and checked pair-wise,
 *     meaning that x&x+1 will be removed if both are empty or
 *                  x&x+1 and x+y*2&x+y*2+1 are equal
 *
 * pSArr    MUST be set
 * pModded  may be NULL, if not it will be set to ST_B_TRUE if
 *          the array was modified
 *
 * returns: ERRCODE
 */
Tst_err
st_mtes_strArrStrip(Tst_mtes_strArr *pSArr, const Tst_bool arePairs,
		ST_OPTARG(Tst_bool *pModded))
{
	Tst_err    res;
	Tst_uint32 x,
	           y,
	           slen1,
	           slen2,
	           incDec = 0,
	           ecnt;
	Tst_bool   equ1,
	           equ2,
	           dummyM,
	           *pM    = &dummyM;
	Tst_mtes_string *pStrA,
	                *pStrB;

	ST_ASSERTN_IARG(pSArr == NULL)

	if (pModded != NULL)
		pM = pModded;
	*pM = ST_B_FALSE;

	ecnt = st_mtes_strArrGetElemCount(pSArr);
	if (arePairs) {
		if (ecnt < 2 || ecnt % 2 != 0) {
			st_mtes_stc_rsetSA(pSArr);
			return ST_ERR_SUCC;
		}
		incDec = 1;
	}
	x = 1;
	while (x + incDec <= ecnt) {
		pStrA = st_mtes_strArrGetElem(pSArr, x);
		slen1 = st_mtes_strlenBy(pStrA);
		if (arePairs) {
			pStrA = st_mtes_strArrGetElem(pSArr, x + 1);
			slen2 = st_mtes_strlenBy(pStrA);
		} else
			slen2 = slen1;
		if (slen1 < 1 && slen2 < 1) {
			res = st_mtes_strArrRem(pSArr, x);
			if (res == ST_ERR_SUCC && arePairs)
				res = st_mtes_strArrRem(pSArr, x);
			if (res != ST_ERR_SUCC)
				return res;
			ecnt -= (ecnt >= 2 ? 2 : ecnt);
			*pM   = ST_B_TRUE;
			continue;  /* x remains the same */
		}
		y = x + 1 + incDec;
		while (y + incDec <= ecnt) {
			pStrA = st_mtes_strArrGetElem(pSArr, x);
			pStrB = st_mtes_strArrGetElem(pSArr, y);
			equ1  = st_mtes_strEqual(pStrA, pStrB, ST_B_TRUE);
			if (arePairs) {
				pStrA = st_mtes_strArrGetElem(pSArr, x + 1);
				pStrB = st_mtes_strArrGetElem(pSArr, y + 1);
				equ2  = st_mtes_strEqual(pStrA, pStrB, ST_B_TRUE);
			} else
				equ2 = equ1;
			if (equ1 && equ2) {
				res = st_mtes_strArrRem(pSArr, y);
				if (res == ST_ERR_SUCC && arePairs)
					res = st_mtes_strArrRem(pSArr, y);
				if (res != ST_ERR_SUCC)
					return res;
				ecnt -= (ecnt >= 2 ? 2 : ecnt);
				*pM   = ST_B_TRUE;
				continue;  /* y remains the same */
			}
			y += 1 + incDec;
		}
		x += 1 + incDec;
	}
	/* */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Sort string array
 *
 * pSArr   MUST be set
 */
static Tst_int32
ST_MTES__cbCompareElemsStr(const void *pElem1,
		const void *pElem2, void *pCustomCmpOpts)
{
	Tst_mtes__strArr_sortOpts *pCSO = (Tst_mtes__strArr_sortOpts*)pCustomCmpOpts;

	pCSO->relat = 0;
	if (ST_MTES__strcompare_n((const Tst_mtes_string*)pElem1,
			(const Tst_mtes_string*)pElem2, &pCSO->cset,
			pCSO->compCasesens, -1, &pCSO->relat) != ST_ERR_SUCC)
		return 0;
	return pCSO->relat;
}

Tst_err
st_mtes_strArrSort(Tst_mtes_strArr *pSArr, const Tst_bool ascOrDesc,
		const Tst_bool compCasesens)
{
	Tst_mtes__strArr_sortOpts cso;

	ST_ASSERTN_IARG(pSArr == NULL)

	st_mtes_stc_rsetCharSet(&cso.cset, ST_B_TRUE);
	cso.ascOrDesc    = ascOrDesc;
	cso.compCasesens = compCasesens;
	cso.relat        = 0;

	if (! st_dynlist_sortElements(
			&((Tst_mtes__strArr_intn*)pSArr->pObInternal)->list,
			ST_DYNLIST_SA_DEF, ST_MTES__cbCompareElemsStr, NULL, &cso))
		return ST_ERR_FAIL;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Merge all strings into one
 *
 * pSArr           MUST be set
 * pSeparatorISO   may be != NULL
 */
Tst_err
st_mtes_strArrMergeToOneString(Tst_mtes_strArr *pSArr,
		ST_OPTARG(const Tst_str *pSeparatorISO))
{
	Tst_err    res;
	Tst_uint32 x,
	           ecnt;
	Tst_mtes_string tmps,
	                *pElemX;

	ST_ASSERTN_IARG(pSArr == NULL)

	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;
	if (pSeparatorISO != NULL) {
		res = st_mtes_setStrTo2(ST_MTES_TE_UTF16LE,
				(const char*)pSeparatorISO, &tmps);
		if (res != ST_ERR_SUCC) {
			st_mtes_stc_freeStr(&tmps);
			return res;
		}
	}

	ecnt = st_mtes_strArrGetElemCount(pSArr);
	for (x = 2; x <= ecnt; x++) {
		pElemX = st_mtes_strArrGetElem(pSArr, x);
		if (st_mtes_isStrEmptyCh(pElemX))
			continue;
		if (pSeparatorISO != NULL) {
			/* elem1 = elem1 + ", " */
			res = st_mtes_strConcat(&tmps, st_mtes_strArrGetElem(pSArr, 1));
		}
		if (res == ST_ERR_SUCC) {
			/* elem1 = elem1 + elemX */
			res = st_mtes_strConcat(pElemX, st_mtes_strArrGetElem(pSArr, 1));
		}
		if (res != ST_ERR_SUCC)
			break;
	}

	st_mtes_stc_freeStr(&tmps);

	if (res == ST_ERR_SUCC)
		for (x = ecnt; x > 1; x--)
			st_mtes_strArrRem(pSArr, x);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Get total length of all strings
 *
 * pSArr   MUST be set
 */
Tst_uint32
st_mtes_strArrTotalStrlenBy(const Tst_mtes_strArr *pSArr)
{
	Tst_uint32 sum = 0,
	           ecnt,
	           x   = 1;
	Tst_mtes_string const *pStr;

	ST_ASSERTN_NUM(0, pSArr == NULL)

	ecnt = st_mtes_strArrGetElemCount(pSArr);
	while (x <= ecnt) {
		pStr = st_mtes_strArrGetElem(pSArr, x++);
		ST_ASSERTN_NUM(ST_ERR_FAIL, pStr == NULL)

		sum += st_mtes_strlenBy(pStr);
	}
	return sum;
}

/*----------------------------------------------------------------------------*/

/*
 * Test if string in char-pointer is ASCII-encoded
 *
 * pStr  may be NULL
 */
Tst_bool
st_mtes_testIfIsASCII(ST_OPTARG(const Tst_str *pStr), const Tst_uint32 sz)
{
	Tst_str const *pS = pStr,
	              *pE;

	if (pStr == NULL)
		return ST_B_FALSE;

	pE = pS + sz;
	while (pS < pE && *pS) {
		if (*pS != ST_CSET_ISO_LF && *pS != ST_CSET_ISO_CR &&
				*pS != ST_CSET_ISO_TAB &&
				(*pS < ST_CSET_ISO_PRNSMA || *pS > ST_CSET_ISO_PRNBIG))
			return ST_B_FALSE;
		++pS;
	}
	return ST_B_TRUE;
}

/*
 * Test if string in char-pointer is U8-encoded
 *
 * pStr  may be NULL
 */
Tst_bool
st_mtes_testIfIsUTF8(ST_OPTARG(const Tst_str *pStr), const Tst_uint32 sz)
{
#	define LOC_REMSZ_  (sz - (pS - pStr))
	Tst_str const *pS = pStr;

	if (pStr == NULL || sz < 1)
		return ST_B_FALSE;

	while (LOC_REMSZ_ > 0 && *pS) {
		if ((pS[0] & 0xe0) == 0xc0 && LOC_REMSZ_ >= 2 &&
				(pS[1] & 0xc0) == 0x80) {
			/*pS += 2;*/
			return ST_B_TRUE;
		} else if ((pS[0] & 0xf0) == 0xe0 && LOC_REMSZ_ >= 3 &&
				(pS[1] & 0xc0) == 0x80 &&
				(pS[2] & 0xc0) == 0x80) {
			/*pS += 3;*/
			return ST_B_TRUE;
		} else if ((pS[0] & 0xf8) == 0xf0 && LOC_REMSZ_ >= 4 &&
				(pS[1] & 0xc0) == 0x80 &&
				(pS[2] & 0xc0) == 0x80 &&
				(pS[3] & 0xc0) == 0x80) {
			/*pS += 4;*/
			return ST_B_TRUE;
		} else if ((pS[0] & 0xfc) == 0xf8 && LOC_REMSZ_ >= 5 &&
				(pS[1] & 0xc0) == 0x80 &&
				(pS[2] & 0xc0) == 0x80 &&
				(pS[3] & 0xc0) == 0x80 &&
				(pS[4] & 0xc0) == 0x80) {
			/*pS += 5;*/
			return ST_B_TRUE;
		} else if ((pS[0] & 0xfe) == 0xfc && LOC_REMSZ_ >= 6 &&
				(pS[1] & 0xc0) == 0x80 &&
				(pS[2] & 0xc0) == 0x80 &&
				(pS[3] & 0xc0) == 0x80 &&
				(pS[4] & 0xc0) == 0x80 &&
				(pS[5] & 0xc0) == 0x80) {
			/*pS += 6;*/
			return ST_B_TRUE;
		} else
			++pS;
	}
	return ST_B_FALSE;
#	undef LOC_REMSZ_
}

/*
 * Test if string in char-pointer is U16-encoded
 *   Only detects simple cases of U16-encoding
 *
 * pStr       may be NULL
 * pIsLEorBE  MUST be != NULL
 */
Tst_bool
st_mtes_testIfIsUTF16(ST_OPTARG(const Tst_str *pStr), const Tst_uint32 sz,
		Tst_bool *pIsLEorBE)
{
	Tst_str const *pS = pStr,
	              *pE;
	Tst_uint32    bom;

	ST_ASSERTN_BOOL(ST_B_FALSE, pIsLEorBE == NULL)

	if (pStr == NULL || sz < 2)
		return ST_B_FALSE;

	*pIsLEorBE = ST_B_TRUE;
	pE         = pS + sz;
	if (sz % 2 != 0)
		--pE;
	/* */
	bom = st_mtes_getCharpBOMsz(pStr, sz, ST_MTES_TE_UTF16LE);
	if (bom == 0) {
		bom = st_mtes_getCharpBOMsz(pStr, sz, ST_MTES_TE_UTF16BE);
		if (bom == 2)
			*pIsLEorBE = ST_B_FALSE;
	}
	if (bom == 2)
		return ST_B_TRUE;
	/* */
	while (pS + 1 < pE)
		if (*pS != 0x00 && *(Tst_str*)(pS + 1) != 0x00)
			return ST_B_FALSE;
		else if (*pS == 0x00 && *(Tst_str*)(pS + 1) == 0x00)
			break;
		else
			pS += 2;
	/* */
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Generate a nice string
 *
 * pStr       MUST be set
 * pCharset   may be NULL, if != NULL only chars in pCharset will be used
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_mtes_generateString(Tst_mtes_string *pStr, const Tst_mtes_textEnc te,
		const Tst_uint32 len, ST_OPTARG(const Tst_str *pCharset))
{
#	define LOC_COPY_(csetCh)  { \
					wch[0] = cset.csetCh[0]; \
					wch[1] = cset.csetCh[1]; }
	Tst_err    res;
	Tst_uint32 min    = 1,
	           max    = 1,
	           rnd,
	           x,
	           sz;
	Tst_byte   wch[2] = {0, 0};
	Tst_str    *pTmp  = NULL;
	Tst_mtes_cset cset;

	ST_ASSERTN_BOOL(ST_B_FALSE, pStr == NULL)

	st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
	if (len < 1)
		return ST_B_TRUE;
	switch (te) {
	case ST_MTES_TE_ISO:
		sz = len + 1;
		ST_CALLOC(pTmp, Tst_str*, len + 1, 1)
		break;
	default:
		st_mtes_stc_rsetCharSet(&cset, te != ST_MTES_TE_UTF16BE);
		sz = (len + 1) * 2;
		ST_CALLOC(pTmp, Tst_str*, sz, 1)
		break;
	}
	if (pTmp == NULL)
		return ST_B_FALSE;
	if (pCharset != NULL) {
		min = 1;
		max = st_mtes_strlenCharpBy_iso_utf8(pCharset);
	}

	for (x = 0; x < len; x++) {
		if (pCharset != NULL) {
			rnd = st_sysGetRand(min, max);
			if (rnd < min || rnd > max) {
				ST_DELPOINT(pTmp)
				return ST_B_FALSE;
			}
			if (te == ST_MTES_TE_ISO)
				pTmp[x] = pCharset[rnd - 1];
			else {
				if (te != ST_MTES_TE_UTF16BE) {
					wch[0] = pCharset[rnd - 1];
					wch[1] = 0;
				} else {
					wch[0] = 0;
					wch[1] = pCharset[rnd - 1];
				}
				memcpy(&pTmp[x * 2], &wch, 2);
			}
			continue;
		}

		switch (te) {
		case ST_MTES_TE_ISO:
			min = 32;
			max = 126;
			rnd = st_sysGetRand(min, max);
			if (rnd < min || rnd > max) {
				ST_DELPOINT(pTmp)
				return ST_B_FALSE;
			}
			pTmp[x] = rnd;
			break;
		default:
			switch (st_sysGetRand(0, 1)) {
			case 0:
				min = 32;
				max = 126;
				rnd = st_sysGetRand(min, max);
				if (rnd < min || rnd > max) {
					ST_DELPOINT(pTmp)
					return ST_B_FALSE;
				}
				if (te != ST_MTES_TE_UTF16BE) {
					wch[0] = rnd;
					wch[1] = 0;
				} else {
					wch[0] = 0;
					wch[1] = rnd;
				}
				break;
			case 1:
				min = 1;
				max = 29;
				rnd = st_sysGetRand(min, max);
				switch (rnd) {
				case  1: LOC_COPY_(umAeC) break;
				case  2: LOC_COPY_(umOeC) break;
				case  3: LOC_COPY_(umUeC) break;
				case  4: LOC_COPY_(umAeS) break;
				case  5: LOC_COPY_(umOeS) break;
				case  6: LOC_COPY_(umUeS) break;
				case  7: LOC_COPY_(umSz) break;
				case  8: LOC_COPY_(umAe2C) break;
				case  9: LOC_COPY_(umOe2C) break;
				case 10: LOC_COPY_(umAaC) break;
				case 11: LOC_COPY_(umAe2S) break;
				case 12: LOC_COPY_(umOe2S) break;
				case 13: LOC_COPY_(umAaS) break;
				case 14: LOC_COPY_(nTildC) break;
				case 15: LOC_COPY_(oAcuC) break;
				case 16: LOC_COPY_(uAcuC) break;
				case 17: LOC_COPY_(aAcuC) break;
				case 18: LOC_COPY_(eAcuC) break;
				case 19: LOC_COPY_(iAcuC) break;
				case 20: LOC_COPY_(yAcuC) break;
				case 21: LOC_COPY_(nTildS) break;
				case 22: LOC_COPY_(oAcuS) break;
				case 23: LOC_COPY_(uAcuS) break;
				case 24: LOC_COPY_(aAcuS) break;
				case 25: LOC_COPY_(eAcuS) break;
				case 26: LOC_COPY_(iAcuS) break;
				case 27: LOC_COPY_(yAcuS) break;
				case 28: LOC_COPY_(umOe3C_u) break;
				case 29: LOC_COPY_(umOe3S_u) break;
				default:
					ST_DELPOINT(pTmp)
					return ST_B_FALSE;
				}
				break;
			default:
				ST_DELPOINT(pTmp)
				return ST_B_FALSE;
			}
			memcpy(&pTmp[x * 2], &wch, 2);
		}
	}
	pTmp[sz - 1] = 0;
	if (te != ST_MTES_TE_ISO)
		pTmp[sz - 2] = 0;
	if (te == ST_MTES_TE_ISO || te == ST_MTES_TE_UTF16BE)
		res = st_mtes_copyFromCharp(pTmp, sz, te, pStr);
	else
		res = st_mtes_copyFromCharp(pTmp, sz, ST_MTES_TE_UTF16LE, pStr);
	ST_DELPOINT(pTmp)
	if (res != ST_ERR_SUCC)
		return ST_B_FALSE;
	if (te == ST_MTES_TE_UTF8) {
		res = st_mtes_convStr(pStr, ST_MTES_TE_UTF8);
		if (res != ST_ERR_SUCC)
			return ST_B_FALSE;
	}
	return ST_B_TRUE;
#	undef LOC_COPY_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_MTES__toInt(const Tst_mtes_string *pStr,
		ST_OPTARG(Tst_int32 *pI32), ST_OPTARG(Tst_uint32 *pUI32))
{
	Tst_err res = ST_ERR_SUCC;
	Tst_mtes_string       tmpISO;
	Tst_mtes_string const *pInpStr = pStr;

	ST_ASSERTN_VOID(pStr == NULL || pStr->pObInternal == NULL)

	if (pI32 != NULL)
		*pI32 = -1;
	else if (pUI32 != NULL)
		*pUI32 = 0;

	if (st_mtes_stc_initStr(&tmpISO) != ST_ERR_SUCC)
		return;

	if (st_mtes_getEncodingCur(pStr) != ST_MTES_TE_ISO) {
		res = st_mtes_convStrTo_iso_c(pStr, &tmpISO);
		/* */
		pInpStr = &tmpISO;
	}

	if (res == ST_ERR_SUCC) {
		char const *pCh;
		Tst_uint32 len   = 0;
		Tst_bool   isInt = ST_B_TRUE;
		Tst_mtes__string_intn const *pIsoI;

		pIsoI = (Tst_mtes__string_intn const*)pInpStr->pObInternal;
		pCh   = (char const*)pIsoI->pData;

		while (*pCh) {
			if (*pCh == '-' || *pCh == '+') {
				if (pI32 != NULL && pCh == (char const*)pIsoI->pData) {
					++pCh;
					continue;
				}
				isInt = ST_B_FALSE;
				break;
			}
			if (*pCh < '0' || *pCh > '9') {
				isInt = ST_B_FALSE;
				break;
			}
			++len;
			++pCh;
		}
		if (isInt) {
			if (len == 0) {
				if (pI32 != NULL)
					*pI32 = 0;
			} else if (len < 11) {
				if (pI32 != NULL)
					sscanf((const char*)pIsoI->pData, "%d", pI32);
				else if (pUI32 != NULL)
					sscanf((const char*)pIsoI->pData, "%u", pUI32);
			}
		}
	}

	st_mtes_stc_freeStr(&tmpISO);
}

/*----------------------------------------------------------------------------*/

/*
 * Determines whether string is NUL-Terminated
 * and returns length of string either in chars or bytes
 */
static Tst_uint32
ST_MTES__strlen_safe(const Tst_str *pSData, const Tst_uint32 datSz,
		const Tst_mtes_textEnc te, const Tst_bool asIso, const Tst_bool only1st,
		Tst_bool *pIsTerm, Tst_bool *pHasBOM)
{
	Tst_str const *pS,
	              *pE;
	Tst_bool      wide;
	Tst_uint32    len   = 0,
	              bomSz = 0,
	              inc   = 0;

	*pIsTerm = ST_B_FALSE;
	if (pHasBOM != NULL)
		*pHasBOM = ST_B_FALSE;
	if (pSData == NULL || datSz < 1)
		return 0;

	wide = (te == ST_MTES_TE_UTF16LE || te == ST_MTES_TE_UTF16BE);
	if (wide) {
		bomSz = st_mtes_getCharpBOMsz(pSData, datSz, te);  /* don't count BOM */
		if (pHasBOM != NULL && bomSz == 2)
			*pHasBOM = ST_B_TRUE;
		inc = 1;
	}

	/* */
	pS = pSData;
	pE = pS + datSz;
	while (pS + inc < pE) {
		if (*pS == 0x00) {
			if (! wide) {
				*pIsTerm = ST_B_TRUE;
				break;
			}
			++pS;
			if (*pS == 0x00) {
				*pIsTerm = ST_B_TRUE;
				break;
			}
			++pS;
			len += 2;
		} else {
			pS  += inc + 1;
			len += inc + 1;
		}
		if (only1st &&
				((bomSz > 0 && len > bomSz) ||
					(bomSz == 0 &&
						(! asIso || te != ST_MTES_TE_UTF8 || len > 6))))
			break;
	}
	/* */
	if (only1st && ! *pIsTerm) {
		if (te == ST_MTES_TE_UTF16LE || te == ST_MTES_TE_UTF16BE)
			*pIsTerm = st_utf_isStrNullTerm_u16(pSData, datSz);
		else
			*pIsTerm = st_utf_isStrNullTerm_iso_u8(pSData, datSz);
	}
	/* */
	if (len > 0) {
		if (wide) {
			if ((len % 2) != 0)
				--len;  /* shouldn't happen */
			if (len < bomSz)
				len = 0;
			else
				len -= bomSz;  /* don't count BOM */
			if (asIso)
				len = len / 2;
		} else if (asIso && te == ST_MTES_TE_UTF8) {
			if (! *pIsTerm)
				len = 0;
			else
				len = st_utf_strlen_utf8(pSData, datSz, ST_B_FALSE);
		}
	}
	return len;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy n chars from pSrc to pDest, either in ascending or descending direction
 */
static Tst_err
ST_MTES__strcopy_n(const Tst_mtes_string *pSrc, Tst_mtes_string *pDest,
		Tst_uint32 maxCharsToCp, const Tst_bool fromBegOrEnd)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 bomOut = 0,
	           nulTer,
	           slenInCh,
	           curSSz;
	Tst_bool   cvU16;
	Tst_mtes__string_intn *pSrcI,
	                      *pDestI;

	ST_ASSERTN_IARG(pSrc == NULL || pSrc->pObInternal == NULL ||
			pDest == NULL || pDest->pObInternal == NULL ||
			pSrc == pDest)

	st_mtes_stc_rsetStr(pDest, ST_B_TRUE, ST_B_TRUE);

	pSrcI  = (Tst_mtes__string_intn*)pSrc->pObInternal;
	pDestI = (Tst_mtes__string_intn*)pDest->pObInternal;

	pDestI->encOrg = pSrcI->encOrg;
	pDestI->encCur = pSrcI->encCur;
	if (maxCharsToCp == 0)
		return ST_ERR_SUCC;
	slenInCh = st_mtes_strlenCh(pSrc);
	if (slenInCh == 0)
		return ST_ERR_SUCC;
	if (maxCharsToCp > slenInCh)
		maxCharsToCp = slenInCh;
	/* convert pSrc to nice fixed-charwidth UTF16 ? */
	cvU16 = (pDestI->encCur != ST_MTES_TE_ISO &&
			pDestI->encCur != ST_MTES_TE_UTF16LE &&
			pDestI->encCur != ST_MTES_TE_UTF16BE);
	if (cvU16)
		res = st_mtes_convStrTo_utf16_c(pSrc, pDest, ST_MTES_TE_UTF16LE);
	else
		res = st_mtes_strCpy(pSrc, pDest);
	if (res != ST_ERR_SUCC)
		return res;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pDestI->pData == NULL)

	/* */
	nulTer = st_mtes_getCharWidth(pDestI->encCur);
	curSSz = pDestI->strSz;
	if (nulTer == 2) {
		maxCharsToCp *= nulTer;  /* nulTer ^= charWidth */
		bomOut        = st_mtes_getStrBOMsz(pDest);
	}
	pDestI->strSz = bomOut + maxCharsToCp + nulTer;  /* BOM + str + NUL-Term */
	if (pDestI->strSz > curSSz)
		pDestI->strSz = curSSz;

	if (curSSz > pDestI->strSz) {
		if (! fromBegOrEnd) {
			/* [xxABCDEFGHIJyy], curSSz=14, maxCh=4, d.ssz=12
			 *   --> [xxCDEFGHIJyy]  */
			/* [ABCDEy], curSSz=6, maxCh=4, d.ssz=5
			 *   --> [BCDEy] */
			memmove(&pDestI->pData[bomOut],
					&pDestI->pData[bomOut + (curSSz - pDestI->strSz)],
					pDestI->strSz - bomOut);
		}
		if (nulTer == 2)
			pDestI->pData[pDestI->strSz - 2] = 0x00;
		pDestI->pData[pDestI->strSz - 1] = 0x00;
	}
	/* */
	if (res == ST_ERR_SUCC && cvU16)
		res = st_mtes_convStr(pDest, pSrcI->encCur);
	return res;
}

/* input is U16-LE */
static void
ST_MTES__toLowerWChar(const Tst_mtes_cset *pCSet, Tst_byte *pWCh)
{
#	define LOC_COMP_W_C_(csetCh)  ST_MTES__compWideCh(pWCh, \
				pCSet->csetCh, ST_B_TRUE)
#	define LOC_REPL_(csetCh1, csetCh2) \
				if (pCSet->csetCh1[0]==pWCh[0] && pCSet->csetCh1[1]==pWCh[1]) { \
					pWCh[0] = pCSet->csetCh2[0]; \
					pWCh[1] = pCSet->csetCh2[1]; }

	/**ST_MTES__prf("  tolow 0x%02x%02x\n", pWCh[0], pWCh[1]);**/
	if (pWCh[1] == 0 &&
			((LOC_COMP_W_C_(aCap) >= 0 &&          /* A-Z */
					LOC_COMP_W_C_(zCap) <= 0) ||
				(LOC_COMP_W_C_(aSma) >= 0 &&       /* a-z */
					LOC_COMP_W_C_(zSma) <= 0))) {
		/* only pWCh[0] is relevant, since U16-LE */
		if (pWCh[0] >= ST_CSET_ISO_ACAP && pWCh[0] <= ST_CSET_ISO_ZCAP)
			pWCh[0] += (ST_CSET_ISO_ASMA - ST_CSET_ISO_ACAP);
	} else {
		LOC_REPL_(umAeC, umAeS)
		else LOC_REPL_(umOeC, umOeS)
		else LOC_REPL_(umUeC, umUeS)
		else LOC_REPL_(umAe2C, umAe2S)
		else LOC_REPL_(umOe2C, umOe2S)
		else LOC_REPL_(umAaC, umAaS)
		else LOC_REPL_(nTildC, nTildS)
		else LOC_REPL_(oAcuC, oAcuS)
		else LOC_REPL_(uAcuC, uAcuS)
		else LOC_REPL_(aAcuC, aAcuS)
		else LOC_REPL_(eAcuC, eAcuS)
		else LOC_REPL_(iAcuC, iAcuS)
		else LOC_REPL_(yAcuC, yAcuS)
		else LOC_REPL_(umOe3C_u, umOe3S_u)
	}
	/**ST_MTES__prf("        0x%02x%02x\n", pWCh[0], pWCh[1]);**/
#	undef LOC_COMP_W_C_
#	undef LOC_REPL_
}

static void
ST_MTES__compareLetters(const Tst_byte wch1[], const Tst_byte wch2[],
		const Tst_mtes_cset *pCSet, Tst_int32 *pRelat)
{
	Tst_bool alpha1UC = ST_B_FALSE,
	         alpha1LC = ST_B_FALSE,
	         alpha2UC = ST_B_FALSE,
	         alpha2LC = ST_B_FALSE,
	         num1     = ST_B_FALSE,
	         num2     = ST_B_FALSE,
	         uml1UC   = ST_B_FALSE,
	         uml1LC   = ST_B_FALSE,
	         uml2UC   = ST_B_FALSE,
	         uml2LC   = ST_B_FALSE,
	         isUoL    = ST_B_FALSE,
	         isAlp    = ST_B_FALSE,
	         isUml    = ST_B_FALSE;
	/**Tst_str str[3] = {0,0,0};**/

	/* classify characters */
	if (ST_MTES__isSomeWChar(wch1, pCSet, ST_B_TRUE, ST_B_TRUE,
			&isUoL, &isAlp, &isUml, &num1)) {
		if (isAlp) {
			alpha1UC = isUoL;
			alpha1LC = ! isUoL;
		} else if (isUml) {
			uml1UC = isUoL;
			uml1LC = ! isUoL;
		}
	}
	/**str[0] = wch1[0];
	str[1] = wch1[1];
	ST_MTES__prf("a=%d, u=%d, n=%d, U=%d,  '%02x%02x'\n",
			isAlp, isUml, num1, isUoL, wch1[0], wch1[1]);**/
	if (ST_MTES__isSomeWChar(wch2, pCSet, ST_B_TRUE, ST_B_TRUE,
			&isUoL, &isAlp, &isUml, &num2)) {
		if (isAlp) {
			alpha2UC = isUoL;
			alpha2LC = ! isUoL;
		} else if (isUml) {
			uml2UC = isUoL;
			uml2LC = ! isUoL;
		}
	}
	/**str[0] = wch2[0];
	str[1] = wch2[1];
	ST_MTES__prf("a=%d, u=%d, n=%d, U=%d,  '%s'\n\n",
			isAlp, isUml, num2, isUoL, &str);**/

	if (! (alpha1UC || alpha1LC || num1 || num2 || alpha2UC || alpha2LC ||
			uml1UC || uml1LC || uml2UC || uml2LC))
		return;
	if ((alpha1UC || alpha1LC || uml1UC || uml1LC || num1) &&
			! (alpha2UC || alpha2LC || uml2UC || uml2LC || num2)) {
		*pRelat = 1;  /* letters|nums > other */
		return;
	}
	if (! (alpha1UC || alpha1LC || uml1UC || uml1LC || num1) &&
			(alpha2UC || alpha2LC || uml2UC || uml2LC || num2)) {
		*pRelat = -1;  /* others < letters|nums */
		return;
	}

	if (alpha1UC && alpha2LC) {
		*pRelat = -1;  /* 'A' < 'a' */
	} else if (alpha1LC && alpha2UC) {
		*pRelat = 1;  /* 'a' > 'A' */
	} else if (uml1UC && uml2LC) {
		*pRelat = -1;  /* '&Auml;' < '&auml;' */
	} else if (uml1LC && uml2UC) {
		*pRelat = 1;  /* '&auml;' > '&Auml;' */
	} else
	/* */
	if (alpha1UC && uml2LC) {
		*pRelat = -1;  /* 'A' < '&auml;' */
	} else if (uml1LC && alpha2UC) {
		*pRelat = 1;  /* '&auml;' > 'A' */
	} else if (alpha1LC && uml2UC) {
		*pRelat = 1;  /* 'a' > '&Auml;' */
	} else if (uml1UC && alpha2LC) {
		*pRelat = -1;  /* '&Auml;' < 'a' */
	} else if (uml1LC && alpha2LC) {
		*pRelat = 1;  /* '&auml;' > 'a' */
	} else if (alpha1LC && uml2LC) {
		*pRelat = -1;  /* 'a' < '&auml;' */
	} else if (uml1UC && alpha2UC) {
		*pRelat = 1;  /* '&Auml;' > 'A' */
	} else if (alpha1UC && uml2UC) {
		*pRelat = -1;  /* 'A' < '&Auml;' */
	}
}

/*
 * Compare strings char-wise
 *   If maxChars>=0 then only compare first n chars
 *   If compCasesens==ST_B_TRUE then compare case-sensitive
 *   pCSet must be init'd as Little-Endian Charset
 */
static Tst_err
ST_MTES__strcompare_n(const Tst_mtes_string *pStr1,
		const Tst_mtes_string *pStr2, const Tst_mtes_cset *pCSet,
		const Tst_bool compCasesens, const Tst_int32 maxChars,
		Tst_int32 *pRelat)
{
#	define LOC_FREESTR_  {if (pC1 == &sIn1) st_mtes_stc_freeStr(&sIn1); \
			if (pC2 == &sIn2) st_mtes_stc_freeStr(&sIn2); }
#	if (ST_MTES_USE_LIBC_STRLEN_ == 1)
#		define LOC_GETSTRLEN_CH_(mac_pStr, mac_len)  { \
				if (mac_pStr->encCur == ST_MTES_TE_ISO) \
					mac_len = (mac_pStr->pData ? \
							strlen((char*)mac_pStr->pData) : 0); \
				else { \
					mac_len = ST_MTES__strlen_safe(mac_pStr->pData, \
							mac_pStr->strSz, mac_pStr->encCur, \
							ST_B_TRUE, ST_B_FALSE, &isStrTerm, NULL); \
					if (! isStrTerm) mac_len = 0; \
				} }
#	else
#		define LOC_GETSTRLEN_CH_(mac_pStr, mac_len)  { \
				if (mac_pStr->encCur == ST_MTES_TE_ISO) \
					mac_len = st_mtes_strlenCharpBy_iso_utf8(mac_pStr->pData); \
				else { \
					mac_len = ST_MTES__strlen_safe(mac_pStr->pData, \
							mac_pStr->strSz, mac_pStr->encCur, \
							ST_B_TRUE, ST_B_FALSE, &isStrTerm, NULL); \
					if (! isStrTerm) mac_len = 0; \
				} }
#	endif
#	define LOC_WCH_EQU_(thech1, thech2) \
				(thech1[0]==thech2[0] && thech1[1]==thech2[1])
#	define LOC_COMP_W_W_(thech1, thech2)  ST_MTES__compWideCh(thech1, \
				thech2, ST_B_TRUE)
#	define LOC_GET_WCH_(thechar, thestr)  { \
				if (wide) { \
					thechar[0] = thestr[0]; thechar[1] = thestr[1]; \
				} else thechar[0] = thestr[0]; }
	Tst_err       res       = ST_ERR_SUCC;
	Tst_uint32    len1,
	              len2;
	Tst_str const *pCS1,
	              *pCS1end,
	              *pCS2;
	Tst_bool      wide      = ST_B_FALSE,
	              isStrTerm = ST_B_FALSE;
	Tst_byte      wch1[2]   = {0, 0},
	              wch2[2]   = {0, 0};
	Tst_mtes_string       sIn1,
	                      sIn2;
	Tst_mtes_string const *pC1 = &sIn1,
	                      *pC2 = &sIn2;
	Tst_mtes__string_intn *pStrI1,
	                      *pStrI2;

	ST_ASSERTN_IARG(pStr1 == NULL || pStr1->pObInternal == NULL ||
			pStr2 == NULL || pStr2->pObInternal == NULL ||
			pCSet == NULL || pRelat == NULL)

	pStrI1 = (Tst_mtes__string_intn*)pStr1->pObInternal;
	pStrI2 = (Tst_mtes__string_intn*)pStr2->pObInternal;

	LOC_GETSTRLEN_CH_(pStrI1, len1)
	LOC_GETSTRLEN_CH_(pStrI2, len2)
	if (len1 == len2 && len1 == 0) {
		*pRelat = 0;  /* empty but equal */
		return ST_ERR_SUCC;
	}

	if (pStrI1->encCur == pStrI2->encCur && pStrI1->encCur == ST_MTES_TE_ISO) {
		/* both ISO strings */
		pC1 = pStr1;
		pC2 = pStr2;
	} else {
		/* both U16-LE strings */
		wide = ST_B_TRUE;
		if (pStrI1->encCur != ST_MTES_TE_UTF16LE) {
			res = st_mtes_stc_initStr(&sIn1);
			if (res != ST_ERR_SUCC)
				return res;
			res = st_mtes_convStrTo_utf16_c(pStr1, &sIn1, ST_MTES_TE_UTF16LE);
		} else
			pC1 = pStr1;
		if (res == ST_ERR_SUCC && pStrI2->encCur != ST_MTES_TE_UTF16LE) {
			res = st_mtes_stc_initStr(&sIn2);
			if (res != ST_ERR_SUCC)
				return res;
			res = st_mtes_convStrTo_utf16_c(pStr2, &sIn2, ST_MTES_TE_UTF16LE);
		} else
			pC2 = pStr2;
		if (res != ST_ERR_SUCC) {
			LOC_FREESTR_
			return res;
		}
	}

	*pRelat = 0;  /* equal */
	if (maxChars >= 0) {
		if (len1 > (Tst_uint32)maxChars)
			len1 = (Tst_uint32)maxChars;
		if (len2 > (Tst_uint32)maxChars)
			len2 = (Tst_uint32)maxChars;
	}
	if (len1 != len2) {
		/* strings are not equal */
		if (len1 > len2) {
			*pRelat = 1;  /* pStr1 > pStr2 */
			len1    = len2;
		} else
			*pRelat = -1;  /* pStr1 < pStr2 */
	}
	pCS1 = ((Tst_mtes__string_intn*)pC1->pObInternal)->pData;
	pCS2 = ((Tst_mtes__string_intn*)pC2->pObInternal)->pData;
	if (wide) {
		len1  = len1 * 2;
		pCS1 += st_mtes_getStrBOMsz(pC1);
		pCS2 += st_mtes_getStrBOMsz(pC2);
	}
	pCS1end = pCS1 + len1;

	/* compare strings */
	/**ST_MTES__prf("\ndo comp len %u\n", (wide ? len1 / 2 : len1));**/
	while ((wide && pCS1 + 1 < pCS1end) ||
			(! wide && pCS1 < pCS1end)) {
		LOC_GET_WCH_(wch1, pCS1)
		LOC_GET_WCH_(wch2, pCS2)
		/**ST_MTES__prf("comp 0x%02x%02x   0x%02x%02x\n",
				wch1[0], wch1[1], wch2[0], wch2[1]);**/
		if (! LOC_WCH_EQU_(wch1, wch2)) {
			if (! compCasesens) {
				ST_MTES__toLowerWChar(pCSet, (Tst_byte*)&wch1);
				ST_MTES__toLowerWChar(pCSet, (Tst_byte*)&wch2);
			}
			if (compCasesens || ! LOC_WCH_EQU_(wch1, wch2)) {
				*pRelat = LOC_COMP_W_W_(wch1, wch2);
				ST_MTES__compareLetters(wch1, wch2, pCSet, pRelat);
				break;  /* strings are not equal */
			}
		}
		if (wide) {
			pCS1 += 2;
			pCS2 += 2;
		} else {
			++pCS1;
			++pCS2;
		}
	}
	/* */
	LOC_FREESTR_
	return ST_ERR_SUCC;
#	undef LOC_FREESTR_
#	undef LOC_GETSTRLEN_CH_
#	undef LOC_GET_WCH_
#	undef LOC_COMP_W_W_
#	undef LOC_WCH_EQU_
}

/*----------------------------------------------------------------------------*/

#define LOC_COMP_WIDE_CH_(macShCh1, macShCh2, \
				macWCh1, macWCh2, macIsLE, macRel)  { \
			if (macIsLE) { \
				macShCh1 = (macWCh1[0] << 8) | macWCh1[1]; \
				macShCh2 = (macWCh2[0] << 8) | macWCh2[1]; \
			} else { \
				macShCh1 = (macWCh1[1] << 8) | macWCh1[0]; \
				macShCh2 = (macWCh2[1] << 8) | macWCh2[0]; \
			} \
			if (macShCh1 < macShCh2) \
				macRel  = -1;  /* 0x41 'A' < 0x61 'a' */ \
			else if (macShCh1 > macShCh2) \
				macRel = 1;  /* 0x61 'a' > 0x41 'A' */ \
			else \
				macRel = 0; \
			}
static Tst_int32
ST_MTES__compWideCh(const Tst_byte wch1[], const Tst_byte wch2[],
		const Tst_bool isLEorBE)
{
	Tst_uint16 shch1,
	           shch2;
	Tst_int32  rel;

	LOC_COMP_WIDE_CH_(shch1, shch2, wch1, wch2, isLEorBE, rel)
	return rel;
}

static Tst_bool
ST_MTES__isAlphaWChar(const Tst_byte wch[], const Tst_mtes_cset *pCSet,
		const Tst_bool allwUpper, const Tst_bool allwLower,
		Tst_bool *pIsUpperOrLower)
{
	Tst_bool isAlpha = ST_B_FALSE,
	         isNum   = ST_B_FALSE,
	         isUml   = ST_B_FALSE;

	if (! ST_MTES__isSomeWChar(wch, pCSet, allwUpper, allwLower,
			pIsUpperOrLower, &isAlpha, &isUml, &isNum))
		return ST_B_FALSE;
	return isAlpha;
}

static Tst_bool
ST_MTES__isNumWChar(const Tst_byte wch[], const Tst_mtes_cset *pCSet)
{
	Tst_bool isAlpha = ST_B_FALSE,
	         isNum   = ST_B_FALSE,
	         isUml   = ST_B_FALSE,
	         isUoL   = ST_B_FALSE;

	if (! ST_MTES__isSomeWChar(wch, pCSet, ST_B_FALSE, ST_B_FALSE,
			&isUoL, &isAlpha, &isUml, &isNum))
		return ST_B_FALSE;
	return isNum;
}

static Tst_bool
ST_MTES__isUmlautWChar(const Tst_byte wch[], const Tst_mtes_cset *pCSet,
		const Tst_bool allwUpper, const Tst_bool allwLower,
		Tst_bool *pIsUpperOrLower)
{
	Tst_bool isAlpha = ST_B_FALSE,
	         isNum   = ST_B_FALSE,
	         isUml   = ST_B_FALSE;

	if (! ST_MTES__isSomeWChar(wch, pCSet, allwUpper, allwLower,
			pIsUpperOrLower, &isAlpha, &isUml, &isNum))
		return ST_B_FALSE;
	return isUml;
}

/*
 * Classify character
 *   allwUpper/Lower: are upper-/lowercase chars allowed ?
 *   pIsUpperOrLower: if char is uppercase this will be set to ST_B_TRUE
 *   pIsAlpha       : if char is 'a'-'z' or 'A'-'Z' this will be set to ST_B_TRUE
 *   pIsUmlaut      : if char is some umlaut, e.g. '&auml;' --> ST_B_TRUE
 *   pIsNum         : if char is '0'-'9' --> ST_B_TRUE
 *
 * returns: ST_B_TRUE if char is allowed and either pIsAlpha==ST_B_TRUE,
 *            pIsUmlaut==ST_B_TRUE or pIsNum==ST_B_TRUE
 */
static Tst_bool
ST_MTES__isSomeWChar(const Tst_byte wch[], const Tst_mtes_cset *pCSet,
		const Tst_bool allwUpper, const Tst_bool allwLower,
		Tst_bool *pIsUpperOrLower, Tst_bool *pIsAlpha, Tst_bool *pIsUmlaut,
		Tst_bool *pIsNum)
{
#	define LOC_WCH_CS_EQU_(csetCh)  (wch[0]==pCSet->csetCh[0] && \
				wch[1]==pCSet->csetCh[1])
#	define LOC_COMP_W_C_(csetCh)  \
				LOC_COMP_WIDE_CH_(shch1, shch2, wch, \
						pCSet->csetCh, pCSet->isLEorBE, rel)
	Tst_uint16 shch1,
	           shch2;
	Tst_int32  rel;

	*pIsUpperOrLower = ST_B_FALSE;
	*pIsAlpha        = ST_B_FALSE;
	*pIsUmlaut       = ST_B_FALSE;
	*pIsNum          = ST_B_FALSE;

	if (allwUpper) {
		/* umlaut */
		if (LOC_WCH_CS_EQU_(umAeC) ||
				LOC_WCH_CS_EQU_(umOeC) ||
				LOC_WCH_CS_EQU_(umUeC) ||
				LOC_WCH_CS_EQU_(umAe2C) ||
				LOC_WCH_CS_EQU_(umOe2C) ||
				LOC_WCH_CS_EQU_(umAaC) ||
				LOC_WCH_CS_EQU_(nTildC) ||
				LOC_WCH_CS_EQU_(oAcuC) ||
				LOC_WCH_CS_EQU_(uAcuC) ||
				LOC_WCH_CS_EQU_(aAcuC) ||
				LOC_WCH_CS_EQU_(eAcuC) ||
				LOC_WCH_CS_EQU_(iAcuC) ||
				LOC_WCH_CS_EQU_(yAcuC) ||
				LOC_WCH_CS_EQU_(umOe3C_u) ||
				LOC_WCH_CS_EQU_(umSz)) {
			*pIsUpperOrLower = ST_B_TRUE;
			*pIsUmlaut       = ST_B_TRUE;
			return ST_B_TRUE;
		}
	}
	if (allwLower) {
		/* umlaut */
		if (LOC_WCH_CS_EQU_(umAeS) ||
				LOC_WCH_CS_EQU_(umOeS) ||
				LOC_WCH_CS_EQU_(umUeS) ||
				LOC_WCH_CS_EQU_(umAe2S) ||
				LOC_WCH_CS_EQU_(umOe2S) ||
				LOC_WCH_CS_EQU_(umAaS) ||
				LOC_WCH_CS_EQU_(nTildS) ||
				LOC_WCH_CS_EQU_(oAcuS) ||
				LOC_WCH_CS_EQU_(uAcuS) ||
				LOC_WCH_CS_EQU_(aAcuS) ||
				LOC_WCH_CS_EQU_(eAcuS) ||
				LOC_WCH_CS_EQU_(iAcuS) ||
				LOC_WCH_CS_EQU_(yAcuS) ||
				LOC_WCH_CS_EQU_(umOe3S_u) ||
				LOC_WCH_CS_EQU_(umSz)) {
			*pIsUmlaut = ST_B_TRUE;
			return ST_B_TRUE;
		}
	}

	if (allwUpper &&
			((pCSet->isLEorBE && wch[1] == 0) ||
				(! pCSet->isLEorBE && wch[0] == 0))) {
		/* alpha */
		LOC_COMP_W_C_(aCap)  /* A- */
		if (rel >= 0) {
			if (rel > 0)
				LOC_COMP_W_C_(zCap)  /* -Z */
			if (rel <= 0) {
				*pIsUpperOrLower = ST_B_TRUE;
				*pIsAlpha        = ST_B_TRUE;
				return ST_B_TRUE;
			}
		}
	}
	if (allwLower &&
			((pCSet->isLEorBE && wch[1] == 0) ||
				(! pCSet->isLEorBE && wch[0] == 0))) {
		/* alpha */
		LOC_COMP_W_C_(aSma)  /* a- */
		if (rel >= 0) {
			if (rel > 0)
				LOC_COMP_W_C_(zSma)  /* -z */
			if (rel <= 0) {
				*pIsAlpha = ST_B_TRUE;
				return ST_B_TRUE;
			}
		}
	}

	/* num */
	if ((pCSet->isLEorBE && wch[1] == 0) ||
				(! pCSet->isLEorBE && wch[0] == 0)) {
		LOC_COMP_W_C_(digZe)  /* 0- */
		if (rel >= 0) {
			if (rel > 0)
				LOC_COMP_W_C_(digNi)  /* -9 */
			if (rel <= 0) {
				*pIsNum = ST_B_TRUE;
				return ST_B_TRUE;
			}
		}
	}
	return ST_B_FALSE;
#	undef LOC_COMP_W_C_
#	undef LOC_WCH_CS_EQU_
}

#undef LOC_COMP_WIDE_CH_

/*----------------------------------------------------------------------------*/

#define LOC_IS_CH_ALLOWED_(pRestrSTC, typus)  (! pRestrSTC->dis##typus)

static Tst_bool
ST_MTES__isCharAllowed(const Tst_mtes_strRestr *pRestr,
		const Tst_byte wch[], const Tst_mtes_cset *pCSet)
{
#	define LOC_WCH_CS_EQU_(csetCh)  (wch[0]==pCSet->csetCh[0] && \
				wch[1]==pCSet->csetCh[1])
#	define LOC_COMP_W_C_(csetCh)  ST_MTES__compWideCh(wch, \
				pCSet->csetCh, pCSet->isLEorBE)
	Tst_bool isUoL = ST_B_FALSE;

	if (LOC_WCH_CS_EQU_(lf))                           /* '\n' */
		return LOC_IS_CH_ALLOWED_(pRestr, Newln);
	if (LOC_WCH_CS_EQU_(cr))                           /* '\r' */
		return ST_B_FALSE;
	if (LOC_WCH_CS_EQU_(tab))                          /* '\t' */
		return LOC_IS_CH_ALLOWED_(pRestr, Tab);
	if (LOC_COMP_W_C_(prnSma) < 0)                     /* <SPA */
		return ST_B_FALSE;
	if (LOC_COMP_W_C_(prnBig) > 0)                     /* >'~' */
		return LOC_IS_CH_ALLOWED_(pRestr, NotPrint);

	if (LOC_WCH_CS_EQU_(spa))                          /* ' ' */
		return LOC_IS_CH_ALLOWED_(pRestr, Spa);
	if (ST_MTES__isAlphaWChar(wch, pCSet, ST_B_TRUE, ST_B_TRUE, &isUoL))   /* A-Z | a-z */
		return LOC_IS_CH_ALLOWED_(pRestr, Alpha);
	if (ST_MTES__isUmlautWChar(wch, pCSet, ST_B_TRUE, ST_B_TRUE, &isUoL))  /* &auml; etc. */
		return LOC_IS_CH_ALLOWED_(pRestr, LngSpec);
	if (ST_MTES__isNumWChar(wch, pCSet))                 /* 0-9 */
		return LOC_IS_CH_ALLOWED_(pRestr, Dig);
	if (LOC_WCH_CS_EQU_(slash))                               /* '/' */
		return LOC_IS_CH_ALLOWED_(pRestr, Slash);
	if (LOC_WCH_CS_EQU_(bra1B) || LOC_WCH_CS_EQU_(bra1E) ||      /* '[]' */
			LOC_WCH_CS_EQU_(bra2B) || LOC_WCH_CS_EQU_(bra2E) ||  /* '<>' */
			LOC_WCH_CS_EQU_(bra3B) || LOC_WCH_CS_EQU_(bra3E))    /* '{}' */
		return LOC_IS_CH_ALLOWED_(pRestr, Bracks);
	if (LOC_WCH_CS_EQU_(parth1) ||                            /* '()' */
			LOC_WCH_CS_EQU_(parth2))
		return LOC_IS_CH_ALLOWED_(pRestr, Paren);

	return LOC_IS_CH_ALLOWED_(pRestr, Other);
#	undef LOC_WCH_CS_EQU_
#	undef LOC_COMP_W_C_
}

/*
 * Strip and correct ISO-8859-1 / UTF16 string
 *  Rremoves useless leading/trailing whitespaces and
 *  replaces invalid characters like '\r'
 *
 * pStr->strSz MUST be >= 4 for UTF16 and >= 2 for ISO
 */
static Tst_err
ST_MTES__stripStr_iso_utf16(Tst_mtes_string *pStr,
		const Tst_mtes_strRestr *pRestr, Tst_bool *pHadInvChars,
		Tst_bool *pModded)
{
#	define LOC_WCH_CS_EQU_(csetCh)  (wch[0]==cset.csetCh[0] && \
				wch[1]==cset.csetCh[1])
#	define LOC_GET_WCH_(delt)  { \
				if (wid == 1) wch[0] = pS[x + delt]; \
				else { wch[0] = pS[x + delt]; wch[1] = pS[x + 1 + delt]; } }
	Tst_uint32 slen,
	           slenOrg,
	           nssz,
	           x,
	           start,
	           wid;
	Tst_byte   wch[2] = {0, 0};
	Tst_str    *pS;
	Tst_bool   hasBOM = ST_B_FALSE;
	Tst_mtes_cset         cset;
	Tst_mtes__string_intn *pSI;

	pSI           = (Tst_mtes__string_intn*)pStr->pObInternal;
	*pModded      = ST_B_FALSE;
	*pHadInvChars = ST_B_FALSE;
	if (pSI->pData == NULL) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
		return ST_ERR_SUCC;
	}
	slen    = st_mtes_strlenBy(pStr);
	slenOrg = slen;
	if (slen < 1) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
		return ST_ERR_SUCC;
	}
	st_mtes_stc_rsetCharSet(&cset, pSI->encCur != ST_MTES_TE_UTF16BE);

	pS = pSI->pData;
	switch (pSI->encCur) {
	case ST_MTES_TE_ISO:
		wid   = 1;
		start = 0;
		break;
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		wid   = 2;
		start = 0;
		if (slen % wid != 0)
			--slen;
		if (slen < wid) {
			st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
			return ST_ERR_SUCC;
		}
		if (st_mtes_getStrBOMsz(pStr) > 0) {
			start  = wid;  /* skip BOM */
			slen  += wid;
			hasBOM = ST_B_TRUE;
		}
		break;
	default:  /* UTF8, this shouldn't happen */
		st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
		*pModded = ST_B_TRUE;
		return ST_ERR_SUCC;
	}

	/* elimate invalid chars */
	for (x = start; x + (wid - 1) < slen; x += wid) {
		LOC_GET_WCH_(0)
		if (LOC_WCH_CS_EQU_(cr)) {
			/* '\r' found */
			*pHadInvChars = ST_B_TRUE;
			if (slen > wid + x) {
				LOC_GET_WCH_(wid)
				if (LOC_WCH_CS_EQU_(lf)) {
					/* '\r' is followed by '\n' --> remove '\r'
					 *     [cr BB CC DD 00], x=0, 4-0=4
					 *     [bo bo cr cr CC DD EE FF 00 00], x=2, 8-2=6 */
					memmove(&pS[x], &pS[x + wid], slen - x);
					slen -= wid;
					if (x >= wid)
						x -= wid;
					pS[slen] = 0;
					if (wid > 1)
						pS[slen + 1] = 0;
				} else {
					/* replace '\r' with '\n' */
					if (wid == 1) {
						pS[x] = cset.lf[0];
					} else {
						pS[x]     = cset.lf[0];
						pS[x + 1] = cset.lf[1];
					}
				}
			} else {
				/* we're at the last char anyway */
				pS[x] = 0;
				if (wid > 1)
					pS[x + 1] = 0;
			}
			*pModded = ST_B_TRUE;
		}
		/* */
		LOC_GET_WCH_(0)
		while (pRestr->hasR &&
				((wid == 1 && wch[0] != 0) ||
					(wid > 1 && (wch[0] != 0 || wch[1] != 0))) &&
				! ST_MTES__isCharAllowed(pRestr, wch, &cset) &&
				x < slen) {
			*pHadInvChars = ST_B_TRUE;
			if (LOC_WCH_CS_EQU_(lf) && ! LOC_IS_CH_ALLOWED_(pRestr, Newln)) {
				/* replace '\n' with whitespace */
				if (wid == 1) {
					pS[x] = cset.spa[0];
				} else {
					pS[x]     = cset.spa[0];
					pS[x + 1] = cset.spa[1];
				}
			} else {
				memmove(&pS[x], &pS[x + wid], slen - x);
				slen    -= wid;
				pS[slen] = 0;
				if (wid > 1)
					pS[slen + 1] = 0;
			}
			*pModded = ST_B_TRUE;
			LOC_GET_WCH_(0)
		}
	} /* end elim inv chars */

	/* remove leading whitespaces */
	while (((wid == 1 && pS[start] == cset.spa[0]) ||
				(wid > 1 && pS[start] == cset.spa[0] &&
					pS[start + 1] == cset.spa[1])) &&
			slen > 0) {
		/* [AA BB CC DD 00], x=0, 4-0=4 */
		/* [bo bo AA BB CC DD EE FF 00 00], slen=8  8-2=6 */
		*pModded = ST_B_TRUE;
		memmove(&pS[start], &pS[start + wid], slen - wid);
		slen    -= wid;
		pS[slen] = 0;
		if (wid > 1)
			pS[slen + 1] = 0;
	}

	/* elimate trailing whitespaces/'\n' and useless zeros */
	nssz = 0;
	x    = slen;
	if (x >= wid) {
		do {
			x -= wid;
			LOC_GET_WCH_(0)
			if (wch[0] == 0 && wch[1] == 0)
				continue;
			if (LOC_WCH_CS_EQU_(spa) || LOC_WCH_CS_EQU_(lf) ||
					(pRestr->strMaxl > 0 && x >= pRestr->strMaxl * wid)) {
				/* trailing whitespace/'\n' found or string still too long */
				pS[x] = 0;
				if (wid > 1)
					pS[x + 1] = 0;
				*pModded = ST_B_TRUE;
			} else {
				/* something else found, stop here */
				nssz = x + 2 * wid;
				break;
			}
		} while (x > wid - 1);
	}
	pSI->strSz = nssz;
	slen       = nssz - (nssz > 0 ? (hasBOM ? 2 * wid : wid) : 0);
	if (slen != slenOrg)
		*pModded = ST_B_TRUE;

	return ST_ERR_SUCC;
#	undef LOC_WCH_CS_EQU_
#	undef LOC_GET_WCH_
}

#undef LOC_IS_CH_ALLOWED_

/*----------------------------------------------------------------------------*/

/*
 * Replace all occurences of pRepl with pWith in ppBuf
 *   If skipFirstN > 0 then the first n bytes of ppBuf will be skipped
 *   If pWith==NULL or wSz < 1 then pRepl will simply be removed
 *
 * pWith may be NULL and wSz may be 0
 */
static Tst_err
ST_MTES__bufReplace(Tst_buf **ppBuf, Tst_uint32 *pBSz,
		const Tst_buf *pRepl, const Tst_uint32 rSz,
		const Tst_uint32 skipFirstN,
		const Tst_buf *pWith, const Tst_uint32 wSz,
		Tst_bool *pChgd)
{
	Tst_uint32 x         = 0,           /* index where pRepl was found */
	           offs      = skipFirstN,  /* where to begin with search */
	           bufRealSz = 0;
	Tst_bool   fnd       = ST_B_TRUE;
	Tst_int32  delta;

	*pChgd = ST_B_FALSE;
	/**ST_MTES__prf("\t  bsz %u, rsz %u, %p %p\n",
			*pBSz, rSz, *ppBuf, pRepl);**/
	delta = rSz - wSz;
	if (delta < 0) {
		bufRealSz = *pBSz + 1024;
		ST_REALLOC(*ppBuf, Tst_buf*, bufRealSz, 1)
		if (*ppBuf == NULL)
			return ST_ERR_OMEM;
	}

	/**ST_MTES__prf("  r>"); ST_MTES__prBytes(pRepl, rSz);
	ST_MTES__prf("  b>"); ST_MTES__prBytes(&(*ppBuf)[offs], *pBSz - offs);**/
	while (fnd && *pBSz > offs) {
		fnd = st_sysBufSearch((Tst_buf*)&(*ppBuf)[offs],
				*pBSz - offs, pRepl, rSz, &x);
		if (! fnd)
			continue;
		x   += offs;
		offs = x + wSz;
		/**ST_MTES__prf("\t\tbfRepl at %u, delta %d, bsz %u\n", x, delta, *pBSz);**/
		if (delta < 0) {
			/* [abcEFGhij] -> [abcWXYZhij], d=-1, x=3+rSz=6, bsz=9 ->10 */
			if (*pBSz + (-delta) > bufRealSz) {
				do {
					bufRealSz += 512;
				} while (*pBSz + (-delta) > bufRealSz);
				ST_REALLOC(*ppBuf, Tst_buf*, bufRealSz, 1)
				if (*ppBuf == NULL)
					return ST_ERR_OMEM;
			}
		} else if (delta >= 0) {
			/* [abcEFGhij] -> [abcXYhij], d=1, x=3+rSz=6, bsz=9 ->8 */
		}
		/**ST_MTES__prf("\t\td%c0: move %u, %u->%u\n",
				(delta >= 0 ? '>':'<'), *pBSz - (x + rSz), x+rSz,x+wSz);**/
		memmove(&(*ppBuf)[x + wSz],
				&(*ppBuf)[x + rSz], *pBSz - (x + rSz));
		*pBSz += (-delta);
		if (pWith != NULL && wSz > 0) {
			/**ST_MTES__prf("\t\t: mc %u\n", wSz);**/
			memcpy(&(*ppBuf)[x], pWith, wSz);
		}
		*pChgd = ST_B_TRUE;
		/**ST_MTES__prBytes(*ppBuf, *pBSz);**/
	}

	if (delta < 0 && bufRealSz > 4096 + *pBSz) {
		ST_REALLOC(*ppBuf, Tst_buf*, *pBSz, 1)
		if (*ppBuf == NULL)
			return ST_ERR_OMEM;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_MTES__strArr_add(Tst_mtes_strArr *pSArr, const Tst_mtes_string *pStr,
		const Tst_str *pCharISO, const Tst_str *pCharU8)
{
	Tst_err res;
	Tst_mtes_string *pNew = NULL;

	ST_ASSERTN_IARG(pSArr == NULL || pSArr->pObInternal == NULL ||
			(pStr == NULL && pCharISO == NULL &&
				pCharU8 == NULL))

	ST_CALLOC(pNew, Tst_mtes_string*, 1, sizeof(Tst_mtes_string))
	if (pNew == NULL)
		return ST_ERR_OMEM;
	res = st_mtes_stc_initStr(pNew);
	if (res != ST_ERR_SUCC)
		return res;

	if (pStr != NULL)
		res = st_mtes_strCpy(pStr, pNew);
	else if (pCharISO != NULL)
		res = st_mtes_copyFromCharp_iso(pCharISO, pNew);
	else
		res = st_mtes_copyFromCharp_utf8(pCharU8, pNew);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(pNew);
		ST_DELPOINT(pNew)
		return res;
	}
	res = st_dynlist_addElement(
			&((Tst_mtes__strArr_intn*)pSArr->pObInternal)->list, pNew);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(pNew);
		ST_DELPOINT(pNew)
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* for debugging */
#if (ST_STRING_MTE_DEB_ == 1)
static void ST_MTES__prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

static void ST_MTES__prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}

static void ST_MTES__prBytes(const Tst_buf *pB, const Tst_uint32 sz)
{
	Tst_uint32 x;

	printf("\t\t0x");
	for (x = 0; x < sz; x++)
		printf("%02x ", pB[x]);
	printf("\n");
}
#endif

/******************************************************************************/
