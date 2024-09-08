/******************************************************************************/
/* tag_id3v2_fldadd.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for adding ID3v2 Tag fields                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 15.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj.h>
#include <santag/common/string_mte.h>
#include <santag/common/sys_fnc.h>
#include <santag/common/sys_file.h>
#include <santag/utils/utils_fmt.h>
#include <santag/tag/tag_id3v2.h>
/** */
#define SRC_TAG_ID3V2_FLDADD_C
#include "tag_id3v2-prot.h"
#include "tag_id3v2_zchk-pp.h"
#include "tag_id3v2_stcs-prot.h"
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_FLDADD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef struct {
	Tst_uint32            uval;
	Tst_byte              bval;
	Tst_mtes_string const *pText;
	Tst_mtes_strArr       *pTextArr;
	Tst_binobj            *pBinDat;
	Tst_mtes_string const *pBinDatSrcFn;
	/* */
	Tst_mtes_string const *pDesc;
	Tst_mtes_string const *pGeoFilen;
	Tst_id3v2_sltTSF      sltTSF;
	Tst_id3v2_sltCTp      sltCTP;
	Tst_str const         *pGeoMime;
	Tst_id3v2_picTp       picPtp;
	Tst_bool              isPTPset;
	Tst_utilsFmt_picFmt   picFmtE;
	Tst_bool              isPFMset;
	Tst_str const         *pLang;
} Tst_iv2__addfldgen;

/** Tst_iv2__addfldgen */
#define ST_RSETSTC_TIV2AFG(afg)  { \
			afg.uval         = 0; \
			afg.bval         = 0; \
			afg.pText        = NULL; \
			afg.pTextArr     = NULL; \
			afg.pBinDat      = NULL; \
			afg.pBinDatSrcFn = NULL; \
			afg.pDesc        = NULL; \
			afg.pGeoFilen    = NULL; \
			afg.sltTSF       = ST_ID3V2_SLTTSF_NONE; \
			afg.sltCTP       = ST_ID3V2_SLTCTP_NONE; \
			afg.pGeoMime     = NULL; \
			afg.picPtp       = ST_ID3V2_PT_NONE; \
			afg.isPTPset     = ST_B_FALSE; \
			afg.picFmtE      = ST_UTILSFMT_PF_NONE; \
			afg.isPFMset     = ST_B_FALSE; \
			afg.pLang        = NULL; \
		}

static Tst_err
ST_TIV2__fa_add(Tst_id3v2_tag *pTag,
                Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
                const Tst_uint32 insertPos,
                const Tst_iv2__addfldgen *pAFG);
static Tst_err
ST_TIV2__fa_fldSet(Tst_id3v2_tag *pTag,
                   Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
                   const Tst_uint32 frNr, const Tst_iv2__addfldgen *pAFG);
static Tst_err
ST_TIV2__fa_setBinDat(const char *pFnc, Tst_id3v2_tag_intn *pTagI,
                      Tst_id3v2_fldData_intn *pFldI,
                      ST_OPTARG(Tst_binobj *pSrcBDat),
                      ST_OPTARG(const Tst_mtes_string *pSrcFn));
static Tst_err
ST_TIV2__fa_setFld_numeroidStr(Tst_id3v2_tag *pTag,
                               const Tst_id3v2_frID frID,
                               ST_OPTARG(const Tst_str *pFrIDstr),
                               const Tst_uint32 frNr,
                               const Tst_int32 num, const Tst_int32 numTot);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Check whether Tag already contains a field with similiar/equal data
 *   (only the main data will be compared,
 *    e.g. the string-array or binary data)
 *
 * pTag             MUST be set
 * pFld             MUST be set
 * pHasSimiliarFld  MUST be != NULL
 *
 * if tolerateDiffSLen is ST_B_TRUE then strings will be compared
 *   until max{str1.len, str2.len}
 */
Tst_err
st_id3v2_fldAdd_doesSimiliarFieldExist(Tst_id3v2_tag *pTag,
		const Tst_bool tolerateDiffSLen, const Tst_id3v2_fldData *pFld,
		Tst_bool *pHasSimiliarFld)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res    = ST_ERR_SUCC;
	Tst_bool   areEqu = ST_B_FALSE,
	           triDot = ST_B_FALSE;
	Tst_uint32 saCnt,
	           z,
	           sl1,
	           sl2;
	Tst_mtes_string const  *pStr1,
	                       *pStr2;
	Tst_mtes_string        str1Cp,
	                       str2Cp;
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL,
	                       *pFldI;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFld == NULL || pFld->pObInternal == NULL ||
			pHasSimiliarFld == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	*pHasSimiliarFld = ST_B_FALSE;
	if (! pFldI->isOK)
		return ST_ERR_SUCC;

	st_mtes_stc_initStr(&str1Cp);
	st_mtes_stc_initStr(&str2Cp);

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (! st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
					pFldI->fldPrI.id, pFldI->fldPrI.pIDstr))
			continue;

		switch (pFldI->fldPrI.typ) {
		case ST_ID3V2_FTP_SLT:      /* S[Y]LT */
		case ST_ID3V2_FTP_PIC:      /* [A]PIC */
		case ST_ID3V2_FTP_GEO:      /* GEO[B] */
		case ST_ID3V2_FTP_UFI:      /* UFI[D],PRIV */
		case ST_ID3V2_FTP_MCI:      /* MC[D]I */
		case ST_ID3V2_FTP_CDM:      /* CDM */
		case ST_ID3V2_FTP_CPY:      /* ... */
			*pHasSimiliarFld =
					st_binobj_areEqual(&pItFldI->dataBinDat, &pFldI->dataBinDat);
			break;
		case ST_ID3V2_FTP_CNT:      /* [P]CNT */
			*pHasSimiliarFld = (pItFldI->dataUVal == pFldI->dataUVal);
			break;
		case ST_ID3V2_FTP_POP:      /* POP[M] */
			*pHasSimiliarFld = (pItFldI->dataUVal == pFldI->dataUVal &&
					pItFldI->dataBVal == pFldI->dataBVal);
			break;
		case ST_ID3V2_FTP_TXT:      /* CM1,T00[0]-TZZ[Z] */
		case ST_ID3V2_FTP_URL:      /* W00[0]-WZZ[Z] */
		case ST_ID3V2_FTP_WXX:      /* TXX[X],WXX[X] */
		case ST_ID3V2_FTP_COM:      /* COM[M],U[S]LT */
		case ST_ID3V2_FTP_IPL:      /* IPL[S] */
			saCnt = st_mtes_strArrGetElemCount(&pFldI->dataSArr);
			if (st_mtes_strArrGetElemCount(&pItFldI->dataSArr) != saCnt)
				continue;
			for (z = 1; z <= saCnt; z++) {
				pStr1 = st_mtes_strArrGetElem(&pItFldI->dataSArr, z);
				pStr2 = st_mtes_strArrGetElem(&pFldI->dataSArr, z);
				sl1   = st_mtes_strlenCh(pStr1);
				sl2   = st_mtes_strlenCh(pStr2);
				if (! tolerateDiffSLen && sl1 != sl2)
					break;
				if (tolerateDiffSLen) {
					if (sl1 != sl2) {
						if (sl1 > sl2)
							res = st_tagCFnc_hasTripleDotEnding(pStr2, &triDot);
						else
							res = st_tagCFnc_hasTripleDotEnding(pStr1, &triDot);
						if (res != ST_ERR_SUCC)
							break;
					}
					if (sl1 > sl2)
						sl1 = sl2;
					if (triDot && sl1 > 3)
						sl1 -= 3;
				}
				areEqu = st_mtes_strNEqual(pStr1, pStr2, ST_B_FALSE, sl1);
				/* */
				if (! areEqu) {
					res = st_mtes_strCpy(pStr1, &str1Cp);
					if (res == ST_ERR_SUCC)
						res = st_mtes_strCpy(pStr2, &str2Cp);
					if (res != ST_ERR_SUCC)
						break;
					res = st_mtes_replaceUmlauts(&str1Cp, NULL);
					if (res == ST_ERR_SUCC)
						st_mtes_replaceUmlauts(&str2Cp, NULL);
					if (res != ST_ERR_SUCC)
						break;
					sl1 = st_mtes_strlenCh(pStr1);
					sl2 = st_mtes_strlenCh(pStr2);
					if (sl1 > sl2)
						sl1 = sl2;
					if (triDot && sl1 > 3)
						sl1 -= 3;
					areEqu = st_mtes_strNEqual(&str1Cp, &str2Cp, ST_B_FALSE, sl1);
				}
				/* */
				if (z == 1)
					*pHasSimiliarFld = areEqu;
				else
					*pHasSimiliarFld = *pHasSimiliarFld && areEqu;
				if (! areEqu)
					break;
			}
			break;
		default:
			continue;
		}
		/* */
		if (*pHasSimiliarFld)
			break;
	}

	st_mtes_stc_freeStr(&str1Cp);
	st_mtes_stc_freeStr(&str2Cp);

	/* */
	if (res == ST_ERR_SUCC && *pHasSimiliarFld &&
			ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		/* the pointer pItFldI is still valid here... */
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"new %s field is similiar to existing %s field #%02u",
				pFldI->fldPrI.pIDstr,
				pItFldI->fldPrI.pIDstr, pItFldI->fnr);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Add field to Tag
 *
 * pTag      MUST be set
 * pFldAdd   MUST be set
 */
Tst_err
st_id3v2_fldAdd(Tst_id3v2_tag *pTag, const Tst_id3v2_fldData *pFldAdd)
{
	return st_id3v2_fldIns(pTag, 0, pFldAdd);
}

/*
 * Insert field at position insertPos (^=field nr.) into Tag
 *   if insertPos is 0 then the field will simply be added
 *
 * pTag      MUST be set
 * pFldAdd   MUST be set
 */
Tst_err
st_id3v2_fldIns(Tst_id3v2_tag *pTag, const Tst_uint32 insertPos,
		const Tst_id3v2_fldData *pFldAdd)
{
	const char *cFNCN = __FUNCTION__;
	Tst_iv2__addfldgen     afg;
	Tst_id3v2_fldData_intn *pFldAddI = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFldAdd == NULL || pFldAdd->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;
	pFldAddI = (Tst_id3v2_fldData_intn*)pFldAdd->pObInternal;

	if (! pFldAddI->isOK || pFldAddI->fldPrI.id == ST_ID3V2_FID_2___CDMF) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldAddI,
					"ignoring field");
		return ST_ERR_SUCC;
	}

	if (! st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS,
				pFldAddI->fldPrI.id, NULL))
		return ST_ERR_IARG;

	ST_RSETSTC_TIV2AFG(afg)
	afg.pDesc     = &pFldAddI->attrDesc;
	afg.pGeoFilen = &pFldAddI->attrGeoFn;
	afg.pTextArr  = &pFldAddI->dataSArr;
	afg.pBinDat   = &pFldAddI->dataBinDat;
	afg.uval      = pFldAddI->dataUVal;
	afg.bval      = pFldAddI->dataBVal;
	afg.sltTSF    = pFldAddI->attrSltTSF;
	afg.sltCTP    = pFldAddI->attrSltCTP;
	afg.pGeoMime  = pFldAddI->pAttrGeoMime;
	afg.picPtp    = pFldAddI->attrPicTp;
	afg.isPTPset  = ST_B_TRUE;
	afg.picFmtE   = pFldAddI->attrPicFmt;
	afg.isPFMset  = ST_B_TRUE;
	afg.pLang     = (Tst_str*)&pFldAddI->attrLang;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
				pTagI->verMaj, pFldAddI->fldPrI.id, pFldAddI->fldPrI.pIDstr,
				"%s field...", (insertPos > 0 ? "inserting" : "adding"));
	return ST_TIV2__fa_add(pTag,
			pFldAddI->fldPrI.id, (const Tst_str*)pFldAddI->fldPrI.pIDstr,
			insertPos, &afg);
}

/*
 * Add text field to Tag
 *
 * pTag    MUST be set
 * pDesc   may be NULL
 * pText   MUST be set
 */
Tst_err
st_id3v2_fldAdd_text(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_mtes_string *pDesc), const Tst_mtes_string *pText)
{
	return st_id3v2_fldIns_text(pTag, frID, pFrIDstr, 0, pDesc, pText);
}

/*
 * Insert text field at position insertPos (^=field nr.) into Tag
 *   if insertPos is 0 then the field will simply be added
 *
 * pTag    MUST be set
 * pDesc   may be NULL
 * pText   MUST be set
 */
Tst_err
st_id3v2_fldIns_text(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 insertPos,
		ST_OPTARG(const Tst_mtes_string *pDesc), const Tst_mtes_string *pText)
{
	const char *cFNCN = __FUNCTION__;
	Tst_iv2__addfldgen afg;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pText == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	ST_RSETSTC_TIV2AFG(afg)
	afg.pDesc = pDesc;
	afg.pText = pText;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
			! st_id3v2_fnc_isTaggInfFr(frID, (const char*)pFrIDstr, pTagI->verMaj))
		st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
				pTagI->verMaj, frID, (const char*)pFrIDstr,
				"%s field...", (insertPos > 0 ? "inserting" : "adding"));
	return ST_TIV2__fa_add(pTag, frID, pFrIDstr, insertPos, &afg);
}

Tst_err
st_id3v2_fldAdd_textArr(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_mtes_string *pDesc), Tst_mtes_strArr *pTextArr)
{
	return st_id3v2_fldIns_textArr(pTag, frID, pFrIDstr, 0, pDesc, pTextArr);
}

Tst_err
st_id3v2_fldIns_textArr(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 insertPos,
		ST_OPTARG(const Tst_mtes_string *pDesc), Tst_mtes_strArr *pTextArr)
{
	const char *cFNCN = __FUNCTION__;
	Tst_iv2__addfldgen afg;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTextArr == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	ST_RSETSTC_TIV2AFG(afg)
	afg.pDesc    = pDesc;
	afg.pTextArr = pTextArr;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
			! st_id3v2_fnc_isTaggInfFr(frID, (const char*)pFrIDstr, pTagI->verMaj))
		st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
				pTagI->verMaj, frID, (const char*)pFrIDstr,
				"%s field...", (insertPos > 0 ? "inserting" : "adding"));
	return ST_TIV2__fa_add(pTag, frID, pFrIDstr, insertPos, &afg);
}

/*
 * Add comment field to Tag
 *
 * pTag             MUST be set
 * ppLangShortISO   may be NULL, if != NULL maxlen=3
 * pDesc            may be NULL
 * pTextArr         MUST be set
 */
Tst_err
st_id3v2_fldAdd_comm(Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_str *pLangShortISO),
		ST_OPTARG(const Tst_mtes_string *pDesc),
		Tst_mtes_strArr *pTextArr)
{
	return st_id3v2_fldIns_comm(pTag, 0, pLangShortISO, pDesc, pTextArr);
}

Tst_err
st_id3v2_fldIns_comm(Tst_id3v2_tag *pTag,
		const Tst_uint32 insertPos, ST_OPTARG(const Tst_str *pLangShortISO),
		ST_OPTARG(const Tst_mtes_string *pDesc),
		Tst_mtes_strArr *pTextArr)
{
	const char           *cFNCN    = __FUNCTION__;
	const Tst_str        *cLANGXXX = (Tst_str*)"xxx";
	const Tst_id3v2_frID cFID      = ST_ID3V2_FID_234_COMM;
	Tst_iv2__addfldgen afg;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTextArr == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	ST_RSETSTC_TIV2AFG(afg)
	afg.pDesc = pDesc;
	if (st_sysStrEmpty(pLangShortISO) || st_sysStrlen(pLangShortISO) > 3)
		afg.pLang = cLANGXXX;
	else
		afg.pLang = pLangShortISO;
	afg.pTextArr = pTextArr;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
				pTagI->verMaj, cFID, NULL,
				"%s field...", (insertPos > 0 ? "inserting" : "adding"));
	return ST_TIV2__fa_add(pTag, cFID, NULL, insertPos, &afg);
}

/*
 * Add GEOB field to Tag
 *
 * pTag       MUST be set
 * pDesc      may be NULL
 * pMimeISO   may be NULL, if != NULL must be ISO-encoded
 * pFilen     may be NULL
 * pBinDat    MUST be set
 */
Tst_err
st_id3v2_fldAdd_geob(Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_mtes_string *pDesc),
		ST_OPTARG(const Tst_str *pMimeISO),
		ST_OPTARG(const Tst_mtes_string *pFilen),
		Tst_binobj *pBinDat)
{
	const char           *cFNCN = __FUNCTION__;
	const Tst_id3v2_frID cFID   = ST_ID3V2_FID_234_GEOB;
	Tst_iv2__addfldgen afg;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pBinDat == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	ST_RSETSTC_TIV2AFG(afg)
	afg.pDesc     = pDesc;
	afg.pGeoFilen = pFilen;
	afg.pBinDat   = pBinDat;
	if (st_sysStrEmpty(pMimeISO))
		afg.pGeoMime = (Tst_str*)ST_ID3V2_MIMEDEF;
	else
		afg.pGeoMime = pMimeISO;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
				pTagI->verMaj, cFID, NULL,
				"adding field...");
	return ST_TIV2__fa_add(pTag, cFID, NULL, 0, &afg);
}

/*
 * Read file and add it as GEOB field to Tag
 *
 * pTag       MUST be set
 * pDesc      may be NULL
 * pMimeISO   may be NULL, if != NULL must be ISO-encoded
 * pFilen     MUST be set
 */
Tst_err
st_id3v2_fldAdd_geob_rdFromFile(Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_mtes_string *pDesc),
		ST_OPTARG(const Tst_str *pMimeISO),
		const Tst_mtes_string *pFilen)
{
	const char           *cFNCN = __FUNCTION__;
	const Tst_id3v2_frID cFID   = ST_ID3V2_FID_234_GEOB;
	Tst_err       res  = ST_ERR_SUCC;
	Tst_str const *pMI = NULL;
	Tst_iv2__addfldgen afg;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			st_mtes_isStrEmptyCh(pFilen))

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	/* get mime-type */
	if (st_sysStrEmpty(pMimeISO)) {
		Tst_str *pFnBaseU8PCh = NULL;

		/* get basename of file *
		 *   (e.g. "/root/blackbible.html" --> "blackbible.html") */
		res = st_id3v2_fnc_getFileBasename(pFilen, NULL, &pFnBaseU8PCh);
		if (res == ST_ERR_SUCC)
			pMI = st_utilsFmt_getMime_defMime_byFn(pFnBaseU8PCh);
		/* */
		ST_DELPOINT(pFnBaseU8PCh)
	} else
		pMI = pMimeISO;

	/* add file */
	if (res == ST_ERR_SUCC) {
		ST_RSETSTC_TIV2AFG(afg)
		afg.pDesc        = pDesc;
		afg.pGeoFilen    = pFilen;
		afg.pBinDatSrcFn = pFilen;
		afg.pGeoMime     = pMI;
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
					pTagI->verMaj, cFID, NULL,
					"adding field...");
		res = ST_TIV2__fa_add(pTag, cFID, NULL, 0, &afg);
	}

	return res;
}

/*
 * Add APIC field to Tag
 *
 * pTag      MUST be set
 * pDesc     may be NULL
 * pBinDat   MUST be set
 * picTpE    MUST be != ST_ID3V2_PT_NONE
 * picFmtE   may be ST_UTILSFMT_PF_NONE
 */
Tst_err
st_id3v2_fldAdd_apic(Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_mtes_string *pDesc), const Tst_id3v2_picTp picTpE,
		const Tst_utilsFmt_picFmt picFmtE,
		Tst_binobj *pBinDat)
{
	const char           *cFNCN = __FUNCTION__;
	const Tst_id3v2_frID cFID   = ST_ID3V2_FID_234_APIC;
	Tst_iv2__addfldgen afg;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pBinDat == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	/* quick check picture type */
	if (picTpE == ST_ID3V2_PT_NONE)
		return ST_ERR_PITP;  /* invalid picture type */

	ST_RSETSTC_TIV2AFG(afg)
	afg.pDesc    = pDesc;
	afg.pBinDat  = pBinDat;
	afg.picPtp   = picTpE;
	afg.isPTPset = ST_B_TRUE;
	afg.picFmtE  = picFmtE;
	afg.isPFMset = ST_B_TRUE;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
				pTagI->verMaj, cFID, NULL,
				"adding field...");
	return ST_TIV2__fa_add(pTag, cFID, NULL, 0, &afg);
}

/*
 * Read file and add it as APIC field to Tag
 *
 * pTag      MUST be set
 * pDesc     may be NULL
 * pFilen    MUST be set
 * picTpE    MUST be != ST_ID3V2_PT_NONE
 * picFmtE   may be ST_UTILSFMT_PF_NONE
 *
 * returns: ERRCODE
 *          (ST_ERR_PIFM if invalid picture format was found
 *           ST_ERR_PITP if invalid picture type was found)
 */
Tst_err
st_id3v2_fldAdd_apic_rdFromFile(Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_mtes_string *pDesc), const Tst_id3v2_picTp picTpE,
		Tst_utilsFmt_picFmt picFmtE, const Tst_mtes_string *pFilen)
{
	const char           *cFNCN = __FUNCTION__;
	const Tst_id3v2_frID cFID   = ST_ID3V2_FID_234_APIC;
	Tst_iv2__addfldgen afg;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			st_mtes_isStrEmptyCh(pFilen))

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	/* quick check picture type */
	if (picTpE == ST_ID3V2_PT_NONE)
		return ST_ERR_PITP;  /* invalid picture type */

	/* add field */
	ST_RSETSTC_TIV2AFG(afg)
	afg.pDesc        = pDesc;
	afg.pBinDatSrcFn = pFilen;
	afg.picPtp       = picTpE;
	afg.isPTPset     = ST_B_TRUE;
	afg.picFmtE      = picFmtE;
	afg.isPFMset     = ST_B_TRUE;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_fdeb1(&pTagI->opts, 0, cFNCN,
				pTagI->verMaj, cFID, NULL,
				"adding field...");
	return ST_TIV2__fa_add(pTag, cFID, NULL, 0, &afg);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set text value of field
 *
 * pTag    MUST be set
 * pText   MUST be set
 */
Tst_err
st_id3v2_fldSet_dataStr(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_mtes_string *pText)
{
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pText == NULL)

	ST_RSETSTC_TIV2AFG(afg)
	afg.pText = pText;
	return ST_TIV2__fa_fldSet(pTag, frID, pFrIDstr, frNr, &afg);
}

/*
 * Set text value of field of numeric-string type (numeric text field: "x")
 *   --> ST_ID3V2_FID_234_TBPM etc.
 *   If num is < 0 then no new field will be added
 *
 * pTag   MUST be set
 */
Tst_err
st_id3v2_fldSet_dataStr_int(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_int32 num)
{
	/* will eventually call st_id3v2_fldSet_dataStr() */
	return ST_TIV2__fa_setFld_numeroidStr(pTag, frID, pFrIDstr, frNr,
				num, -1);
}

/*
 * Set text value of field of position-string type (numeric text field: "x/y")
 *   --> ST_ID3V2_FID_234_TRCK or ST_ID3V2_FID_234_TPOS
 *   If num and numTot are < 0 then no new field will be added
 *   If only one of num and numTot is >= 0 then only that value will be modified
 *
 * pTag   MUST be set
 */
Tst_err
st_id3v2_fldSet_dataStr_pos(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_int32 num, const Tst_int32 numTot)
{
	/* will eventually call st_id3v2_fldSet_dataStr() */
	return ST_TIV2__fa_setFld_numeroidStr(pTag, frID, pFrIDstr, frNr,
			num, numTot);
}

/*
 * Set text value of field
 *
 * pTag       MUST be set
 * pTextArr   MUST be set
 */
Tst_err
st_id3v2_fldSet_dataStrArr(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		Tst_mtes_strArr *pTextArr)
{
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTextArr == NULL)

	ST_RSETSTC_TIV2AFG(afg)
	afg.pTextArr = pTextArr;
	return ST_TIV2__fa_fldSet(pTag, frID, pFrIDstr, frNr, &afg);
}

/*
 * Set binary data of field
 *
 * pTag      MUST be set
 * pBinDat   MUST be set
 */
Tst_err
st_id3v2_fldSet_dataBin(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, const Tst_uint32 frNr,
		Tst_binobj *pBinDat)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pBinDat == NULL)

	switch (st_id3v2_fnc_getFrameType_byID(frID)) {
	case ST_ID3V2_FTP_GEO:
	case ST_ID3V2_FTP_PIC:
		ST_RSETSTC_TIV2AFG(afg)
		afg.pBinDat = pBinDat;
		/* */
		res = ST_TIV2__fa_fldSet(pTag, frID, NULL, frNr, &afg);
		break;
	default:
		break;
	}
	return res;
}

Tst_err
st_id3v2_fldSet_dataBin_rdFromFile(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, const Tst_uint32 frNr,
		const Tst_mtes_string *pFilen)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFilen == NULL)

	switch (st_id3v2_fnc_getFrameType_byID(frID)) {
	case ST_ID3V2_FTP_GEO:
	case ST_ID3V2_FTP_PIC:
		ST_RSETSTC_TIV2AFG(afg)
		afg.pBinDatSrcFn = pFilen;
		/* */
		res = ST_TIV2__fa_fldSet(pTag, frID, NULL, frNr, &afg);
		break;
	default:
		break;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Set description-attribute of a field
 */
Tst_err
st_id3v2_fldSet_attrDesc(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, const Tst_uint32 frNr,
		const Tst_mtes_string *pDesc)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pDesc == NULL)

	switch (st_id3v2_fnc_getFrameType_byID(frID)) {
	case ST_ID3V2_FTP_COM:
	case ST_ID3V2_FTP_GEO:
	case ST_ID3V2_FTP_PIC:
	case ST_ID3V2_FTP_SLT:
	case ST_ID3V2_FTP_UFI:
	case ST_ID3V2_FTP_WXX:
		ST_RSETSTC_TIV2AFG(afg)
		afg.pDesc = pDesc;
		/* */
		res = ST_TIV2__fa_fldSet(pTag, frID, NULL, frNr, &afg);
		break;
	default:
		break;
	}
	return res;
}

/*
 * Set mime-type-attribute of a field
 */
Tst_err
st_id3v2_fldSet_attrMime(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, const Tst_uint32 frNr,
		const Tst_str *pMimeISO)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pMimeISO == NULL)

	switch (st_id3v2_fnc_getFrameType_byID(frID)) {
	case ST_ID3V2_FTP_GEO:
		ST_RSETSTC_TIV2AFG(afg)
		afg.pGeoMime = pMimeISO;
		/* */
		res = ST_TIV2__fa_fldSet(pTag, frID, NULL, frNr, &afg);
		break;
	default:
		break;
	}
	return res;
}

/*
 * Set picture-type-attribute of a field
 */
Tst_err
st_id3v2_fldSet_attrPicTp(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, const Tst_uint32 frNr,
		const Tst_id3v2_picTp picTpE)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	switch (st_id3v2_fnc_getFrameType_byID(frID)) {
	case ST_ID3V2_FTP_PIC:
		ST_RSETSTC_TIV2AFG(afg)
		afg.picPtp   = picTpE;
		afg.isPTPset = ST_B_TRUE;
		/* */
		res = ST_TIV2__fa_fldSet(pTag, frID, NULL, frNr, &afg);
		break;
	default:
		break;
	}
	return res;
}

/*
 * Set picture-format-attribute of a field
 */
Tst_err
st_id3v2_fldSet_attrPicFmt(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, const Tst_uint32 frNr,
		const Tst_utilsFmt_picFmt picFmtE)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_iv2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	switch (st_id3v2_fnc_getFrameType_byID(frID)) {
	case ST_ID3V2_FTP_PIC:
		ST_RSETSTC_TIV2AFG(afg)
		afg.picFmtE  = picFmtE;
		afg.isPFMset = ST_B_TRUE;
		/* */
		res = ST_TIV2__fa_fldSet(pTag, frID, NULL, frNr, &afg);
		break;
	default:
		break;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Move new field to another position
 */
static Tst_err
ST_TIV2__fa_add_moveToIP(const char *pFnc,
		Tst_id3v2_tag *pTag, Tst_id3v2_tag_intn *pTagI,
		const Tst_uint32 insertPos,
		Tst_id3v2_fldData_intn *pFldItoMove)
{
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 ixFIPC  = 0,
	           ixNF;
	Tst_bool   fndFIPC = ST_B_FALSE;
	Tst_id3v2_fldData      *pItFld = NULL;
	Tst_id3v2_fldData_intn *pItFldI;

	/* get list index of field that is currently at
	 *   the position insertPos  */
	while ((pItFld = st_id3v2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		++ixFIPC;
		if (pItFldI->fnr == insertPos &&
				st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
							pFldItoMove->fldPrI.id, pFldItoMove->fldPrI.pIDstr)) {
			fndFIPC = ST_B_TRUE;
			break;
		}
	}
	/* increment field number of all 'bigger' fields */
	pItFld = NULL;
	while ((pItFld = st_id3v2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fnr == pFldItoMove->fnr || pItFldI->fnr < insertPos ||
				! st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
							pFldItoMove->fldPrI.id, pFldItoMove->fldPrI.pIDstr))
			continue;
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, pFnc, pItFldI,
					"inc fldNr");
		++pItFldI->fnr;
	}
	/* change new fields' number */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_fdeb3(&pTagI->opts, 0, pFnc, pFldItoMove,
				"set fldNr to %u", insertPos);
	pFldItoMove->fnr = insertPos;
	/* move new field to other position in list and
	 *   update unique field indices  */
	ixNF = st_id3v2_gs_getTag_amountFlds(pTag);
	if (ixNF > ixFIPC) {
		res = st_id3v2_fnc_moveFld(pTag, ixNF, ixFIPC);
		if (res != ST_ERR_SUCC)
			st_id3v2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"moving field failed");
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Actually add a new field to the Tag
 *
 * returns: ERRCODE
 *          (ST_ERR_PIFM if invalid picture format was found
 *           ST_ERR_PITP if invalid picture type was found)
 */
static Tst_err
ST_TIV2__fa_add(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 insertPos, const Tst_iv2__addfldgen *pAFG)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err       res = ST_ERR_SUCC;
	Tst_uint32    w;
	Tst_str const *pFExt;
	Tst_bool      skippedField = ST_B_FALSE;
	Tst_id3v2_fldData      *pFld  = NULL;
	Tst_id3v2_fldData_intn *pFldI = NULL;
	Tst_id3v2_tag_intn     *pTagI;
	Tst_id3v2_fldd_dattp   datTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	/* adjust frID */
	frID = st_id3v2_fnc_adjustFrID(frID, pFrIDstr, NULL);
	if (frID == ST_ID3V2_FID_NONE)
		return ST_ERR_IARG;
	if ((frID == ST_ID3V2_FID_234_CTXT || frID == ST_ID3V2_FID_234_CURL) &&
			st_id3v2_fnc_hasIDstrInvalidChars((Tst_str*)pFrIDstr,
						/*mod:*/ST_B_FALSE, NULL)) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb1(&pTagI->opts, 3, cFNCN, pTagI->verMaj,
					frID, (const char*)pFrIDstr,
					"custom ID-String is invalid");
		return ST_ERR_SUCC;
	}

	if (st_id3v2_gs_getTag_amountFlds(pTag) >= ST_ID3V2_MAX_TFLD_CNT) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"too many frames for one tag, aborting (max %u)",
				ST_ID3V2_MAX_TFLD_CNT);
		return ST_ERR_ABRT;
	}

	/* add new field to tag */
	res = st_id3v2_fnc_addElemToFlds(pTag, frID, pFrIDstr,
			&skippedField, &pFld);
	if (res == ST_ERR_SUCC)
		pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;
	if (res != ST_ERR_SUCC || skippedField) {
		if (res == ST_ERR_SUCC && skippedField &&
				pFldI->fnr > ST_ID3V2_MAX_TFLDID_CNT &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			st_id3v2_d_fdeb2(&pTagI->opts, 3, cFNCN, &pFldI->fldPrI,
					"too many frames for this ID, skipping (%u > %u)",
					pFldI->fnr, ST_ID3V2_MAX_TFLDID_CNT);
		} else if (res == ST_ERR_SUCC && skippedField &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			st_id3v2_d_fdeb2(&pTagI->opts, 3, cFNCN, &pFldI->fldPrI,
					"frame is invalid for this tag version");
		}
		return res;
	}

	/* get data types */
	st_id3v2_stc_rsetDatTp(&datTp);
	st_id3v2_fnc_getFieldDataTypes(pFldI->fldPrI.id, &datTp);

	/* copy data to field */
	/** short content description  */
	if (pAFG->pDesc != NULL && datTp.hasScd &&
			! st_mtes_isStrEmptyCh(pAFG->pDesc)) {
		res = st_mtes_strCpy(pAFG->pDesc, &pFldI->attrDesc);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3Str(&pTagI->opts, 3, cFNCN, pFldI,
					ST_B_TRUE, "scd", &pFldI->attrDesc, "");
	}
	/** text */
	if (pAFG->pText != NULL &&
			(datTp.hasText || datTp.hasCTxt) &&
			! st_mtes_isStrEmptyCh(pAFG->pText)) {
		res = st_mtes_strArrAdd(&pFldI->dataSArr, pAFG->pText);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
				st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
				! st_id3v2_fnc_isTaggInfFr(frID, (const char*)pFrIDstr, pTagI->verMaj))
			st_id3v2_d_fdeb3Str(&pTagI->opts, 3, cFNCN, pFldI,
					ST_B_TRUE, "txt", st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
	} else
		if (pAFG->pTextArr != NULL &&
				(datTp.hasText || datTp.hasCTxt) &&
				st_mtes_strArrGetElemCount(pAFG->pTextArr) > 0) {
			for (w = 1; w <= st_mtes_strArrGetElemCount(pAFG->pTextArr); w++) {
				res = st_mtes_strArrAdd(&pFldI->dataSArr,
						st_mtes_strArrGetElem(pAFG->pTextArr, w));
				if (res != ST_ERR_SUCC)
					return res;
				/* */
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
						st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
						! st_id3v2_fnc_isTaggInfFr(frID, (const char*)pFrIDstr,
									pTagI->verMaj)) {
					if (st_mtes_strArrGetElemCount(pAFG->pTextArr) == 1)
						st_id3v2_d_fdeb3Str(&pTagI->opts, 3, cFNCN, pFldI,
								ST_B_TRUE, "txt",
								st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
					else
						st_id3v2_d_fdeb3Str(&pTagI->opts, 3, cFNCN, pFldI,
								ST_B_TRUE, "mul",
								st_mtes_strArrGetElem(&pFldI->dataSArr, w), "");
				}
			}
		}
	/** GEOB Filename */
	if (pAFG->pGeoFilen != NULL && datTp.hasGeob &&
			! st_mtes_isStrEmptyCh(pAFG->pGeoFilen)) {
		res = st_id3v2_fnc_getFileBasename(pAFG->pGeoFilen,
					&pFldI->attrGeoFn, NULL);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3Str(&pTagI->opts, 3, cFNCN, pFldI,
					ST_B_TRUE, "fna", &pFldI->attrGeoFn, "");
	}
	/** GEOB Mime */
	if (pAFG->pGeoMime != NULL && datTp.hasGeob &&
			! st_sysStrEmpty(pAFG->pGeoMime)) {
		res = st_sysStrcpy(pAFG->pGeoMime, &pFldI->pAttrGeoMime);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
					"mim \"%s\"", pFldI->pAttrGeoMime);
	}
	/** binary data */
	if ((pAFG->pBinDat != NULL || pAFG->pBinDatSrcFn != NULL) && datTp.hasBDat) {
		res = ST_TIV2__fa_setBinDat(cFNCN, pTagI, pFldI,
					pAFG->pBinDat, pAFG->pBinDatSrcFn);
		if (res != ST_ERR_SUCC)
			return res;
	}
	/** unsigned int value */
	if (datTp.hasUVal) {
		pFldI->dataUVal = pAFG->uval;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
					"uva %u", pFldI->dataUVal);
	}
	/** byte value */
	if (datTp.hasBVal) {
		pFldI->dataBVal = pAFG->bval;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
					"bva %u", pFldI->dataBVal);
	}
	/** language */
	if (pAFG->pLang != NULL && datTp.hasLang) {
		Tst_uint32 tmpLngLen = st_sysStrlen2(pAFG->pLang);

		if (tmpLngLen > 3)
			tmpLngLen = 3;
		if (tmpLngLen > 0)
			memcpy(pFldI->attrLang, pAFG->pLang, tmpLngLen);
		pFldI->attrLang[tmpLngLen] = 0;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
					"lng '%s'", (char*)pFldI->attrLang);
	}
	/** picture info */
	if (datTp.hasPict) {
		pFldI->attrPicTp = pAFG->picPtp;
		if (st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp) == NULL)
			return ST_ERR_PITP;  /* invalid picture type */
		pFldI->attrPicFmt = pAFG->picFmtE;
		pFExt             = st_utilsFmt_getPicFmt_fext_byEnum(pFldI->attrPicFmt);
		if (pFExt == NULL) {
			pFldI->attrPicFmt = st_utilsFmt_getPicFmt_enum_byBObj(&pFldI->dataBinDat);
			pFExt             = st_utilsFmt_getPicFmt_fext_byEnum(pFldI->attrPicFmt);
		}
		if (pFExt == NULL)
			return ST_ERR_PIFM;  /* invalid picture format */
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			if (pTagI->verMaj == 2)
				st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
						"pic imgfmt='%s', ptp='%s'", pFExt,
						st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp));
			else
				st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
						"pic mime='%s', ptp='%s'",
						st_utilsFmt_getPicFmt_mime_byEnum(pFldI->attrPicFmt),
						st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp));
		}
	}
	/** synchr. lyrics info */
	if (datTp.hasSylt) {
		pFldI->attrSltTSF = pAFG->sltTSF;
		pFldI->attrSltCTP = pAFG->sltCTP;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
					"slt tsf=%d, ctp=%d", pFldI->attrSltTSF, pFldI->attrSltCTP);
	}

	/* now do some error correction */
	res = st_id3v2_chk_checkFld(&pTagI->opts, ST_B_FALSE, pFld);
	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		if (pFldI->isOK) {
			/**st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
					"accepted");**/
		} else
			st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pFldI,
					"empty");
	}
	if (res == ST_ERR_SUCC && pFldI->isOK)
		st_tagBas_gs_setHasFields(&pTagI->tbas, ST_B_TRUE);

	/* change field's number if desired */
	if (res == ST_ERR_SUCC && pFldI->isOK &&
			insertPos > 0 && insertPos < pFldI->fnr) {
		res = ST_TIV2__fa_add_moveToIP(cFNCN, pTag, pTagI, insertPos, pFldI);
	}

	/* update error counter */
	if (res == ST_ERR_SUCC)
		st_id3v2_fnc_updateFldErrorCnt(pTag);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Set data of field in Tag
 *
 * returns: ERRCODE
 *          (ST_ERR_PIFM if invalid picture format was found
 *           ST_ERR_PITP if invalid picture type was found)
 */
static Tst_err
ST_TIV2__fa_fldSet(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_iv2__addfldgen *pAFG)
{
#	define LOC_DBG_SETFDAT_  { \
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) \
					st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pItFldI, \
							"setting data"); \
				}
#	define LOC_DBG_ADDF_  { \
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) \
					st_id3v2_d_deb(&pTagI->opts, 0, cFNCN, \
							"adding new field"); \
				}
	const char *cFNCN = __FUNCTION__;
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool fnd = ST_B_FALSE,
	         wasHnd;
	Tst_id3v2_fldData      *pItFld = NULL;
	Tst_id3v2_fldData_intn *pItFldI;
	Tst_id3v2_tag_intn     *pTagI;
	Tst_id3v2_fldd_dattp   datTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pAFG == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	/**st_id3v2_prf("%s(): starting ID=%d (%s), fnr=%u\n",
			cFNCN, frID, (pFrIDstr == NULL ? "" : (char*)pFrIDstr), frNr);**/

	/* adjust frID */
	frID = st_id3v2_fnc_adjustFrID(frID, pFrIDstr, NULL);
	if (frID == ST_ID3V2_FID_NONE)
		return ST_ERR_IARG;
	if ((frID == ST_ID3V2_FID_234_CTXT || frID == ST_ID3V2_FID_234_CURL) &&
			st_id3v2_fnc_hasIDstrInvalidChars((Tst_str*)pFrIDstr,
						/*mod:*/ST_B_FALSE, NULL)) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb1(&pTagI->opts, 3, cFNCN, pTagI->verMaj,
					frID, (const char*)pFrIDstr,
					"custom ID-String is invalid");
		return ST_ERR_SUCC;
	}

	if (st_id3v2_fnc_isTaggInfFr(frID, (const char*)pFrIDstr,
				(st_sysStrlen(pFrIDstr) == 3 ? 2 : 3)))
		return ST_ERR_SUCC;  /* tagger info cannot be modified */

	st_id3v2_stc_rsetDatTp(&datTp);
	st_id3v2_fnc_getFieldDataTypes(frID, &datTp);

	/* set data in existing field(s) */
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if ((frNr != 0 && pItFldI->fnr != frNr) ||
				! st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
							frID, (const char*)pFrIDstr))
			continue;
		/* */
		wasHnd = ST_B_FALSE;

		/* reset field ? */
		if (! (datTp.hasText || datTp.hasCTxt) &&
				(pAFG->pText != NULL || pAFG->pTextArr != NULL)) {
			fnd    = ST_B_TRUE;
			wasHnd = ST_B_TRUE;
			if ((pAFG->pText != NULL && st_mtes_isStrEmptyCh(pAFG->pText)) ||
					(pAFG->pTextArr != NULL &&
						st_mtes_strArrTotalStrlenBy(pAFG->pTextArr) == 0)) {
				res = st_id3v2_disableFld_byID(pTag,
							frID, pFrIDstr, pItFldI->fnr);
				if (res != ST_ERR_SUCC)
					break;
			}
		}

		/* set field data */
		if (! wasHnd && pAFG->pText != NULL &&
				(datTp.hasText || datTp.hasCTxt)) {
			LOC_DBG_SETFDAT_
			fnd = ST_B_TRUE;
			st_mtes_stc_rsetSA(&pItFldI->dataSArr);
			res = st_mtes_strArrAdd(&pItFldI->dataSArr, pAFG->pText);
		} else if (! wasHnd && pAFG->pTextArr != NULL &&
				(datTp.hasText || datTp.hasCTxt)) {
			LOC_DBG_SETFDAT_
			fnd = ST_B_TRUE;
			st_mtes_stc_rsetSA(&pItFldI->dataSArr);
			res = st_mtes_strArrCpy(pAFG->pTextArr, &pItFldI->dataSArr);
		} else if (! wasHnd && (pAFG->pBinDat != NULL || pAFG->pBinDatSrcFn != NULL) &&
				datTp.hasBDat) {
			LOC_DBG_SETFDAT_
			fnd = ST_B_TRUE;
			/* copy binary data */
			res = ST_TIV2__fa_setBinDat(cFNCN, pTagI, pItFldI,
						pAFG->pBinDat, pAFG->pBinDatSrcFn);
			/* set GEOB filename attribute */
			if (res == ST_ERR_SUCC && pAFG->pBinDatSrcFn != NULL && datTp.hasGeob) {
				res = st_id3v2_fnc_getFileBasename(pAFG->pBinDatSrcFn,
							&pItFldI->attrGeoFn, NULL);
			}
			/* set APIC picture-format attribute */
			if (res == ST_ERR_SUCC && datTp.hasPict) {
				pItFldI->attrPicFmt = st_utilsFmt_getPicFmt_enum_byBObj(&pItFldI->dataBinDat);
				if (st_utilsFmt_getPicFmt_fext_byEnum(pItFldI->attrPicFmt) == NULL)
					res = ST_ERR_PIFM;  /* invalid picture format */
			}
		}
		if (res != ST_ERR_SUCC)
			break;

		/* set field attribute */
		if (! wasHnd && pAFG->pDesc != NULL && datTp.hasScd) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pItFldI,
						"setting description");
			fnd = ST_B_TRUE;
			res = st_mtes_strCpy(pAFG->pDesc, &pItFldI->attrDesc);
		} else if (! wasHnd && pAFG->pGeoMime != NULL && datTp.hasGeob) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pItFldI,
						"setting mime");
			fnd = ST_B_TRUE;
			res = st_sysStrcpy(pAFG->pGeoMime, &pItFldI->pAttrGeoMime);
		} else if (! wasHnd && pAFG->isPTPset && datTp.hasPict) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pItFldI,
						"setting picture-type");
			fnd                = ST_B_TRUE;
			pItFldI->attrPicTp = pAFG->picPtp;
			if (st_id3v2_fnc_getPicTp_capShort_byEnum(pItFldI->attrPicTp) == NULL)
				res = ST_ERR_PITP;  /* invalid picture type */
		} else if (! wasHnd && pAFG->isPFMset && datTp.hasPict) {
			Tst_str const *pFExt;

			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 3, cFNCN, pItFldI,
						"setting picture-format");
			fnd                 = ST_B_TRUE;
			pItFldI->attrPicFmt = pAFG->picFmtE;
			pFExt               = st_utilsFmt_getPicFmt_fext_byEnum(pItFldI->attrPicFmt);
			if (pFExt == NULL) {
				pItFldI->attrPicFmt = st_utilsFmt_getPicFmt_enum_byBObj(&pItFldI->dataBinDat);
				pFExt               = st_utilsFmt_getPicFmt_fext_byEnum(pItFldI->attrPicFmt);
			}
			if (pFExt == NULL)
				res = ST_ERR_PIFM;  /* invalid picture format */
		}
		if (res != ST_ERR_SUCC)
			break;

		/* */
		if (frNr != 0)
			break;
	}

	/* if no existing field was found we add a new one */
	if (res == ST_ERR_SUCC && ! fnd && frNr == 0) {
		if (pAFG->pText != NULL &&
				(datTp.hasText || datTp.hasCTxt) &&
				! st_mtes_isStrEmptyCh(pAFG->pText)) {
			LOC_DBG_ADDF_
			fnd = ST_B_TRUE;
			res = st_id3v2_fldAdd_text(pTag, frID, pFrIDstr,
					NULL, pAFG->pText);
		} else if (pAFG->pTextArr != NULL &&
				(datTp.hasText || datTp.hasCTxt) &&
				st_mtes_strArrTotalStrlenBy(pAFG->pTextArr) > 0) {
			LOC_DBG_ADDF_
			fnd = ST_B_TRUE;
			res = st_id3v2_fldAdd_textArr(pTag, frID, pFrIDstr,
					NULL, pAFG->pTextArr);
		}
	}
	if (res == ST_ERR_SUCC && ! fnd &&
			ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"couldn't find field to set data for");

	return res;
#	undef LOC_DBG_SETFDAT_
#	undef LOC_DBG_ADDF_
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__fa_checkBinDatSize(const char *pFnc, Tst_id3v2_tag_intn *pTagI,
		Tst_id3v2_fldData_intn *pFldI, const Tst_fsize boSize,
		ST_OPTARG(const Tst_str *pFnU8))
{
	const Tst_uint32 cMAXV2  = (Tst_uint32)(ST_ID3V2_MAX_AF_FSZ_V2_M * 1024 * 1024),
	                 cMAXV34 = (Tst_uint32)(ST_ID3V2_MAX_AF_FSZ_V34_M * 1024 * 1024);
	char dbgmsg[128];

	if (pFnU8 == NULL)
		snprintf(dbgmsg, sizeof(dbgmsg), "%s", "data");
	else
		snprintf(dbgmsg, sizeof(dbgmsg), "file '%s'", (char*)pFnU8);

	if (pTagI->verMaj == 2 && boSize > (Tst_fsize)cMAXV2) {
		st_id3v2_d_ferr3(&pTagI->opts, pFnc, pTagI->pFilen, pFldI,
				"%s is too big for v2.2 tag, "
				"size="ST_TFSIZE_PRF_LU" bytes (%.2f MB), max=%u bytes (%u MB), "
				"aborting",
				dbgmsg,
				(Tst_fsize)boSize, (double)boSize / 1024.0 / 1024.0,
				cMAXV2, (Tst_uint32)ST_ID3V2_MAX_AF_FSZ_V2_M);
		return ST_ERR_ABRT;
	}
	if (boSize > (Tst_fsize)cMAXV34) {
		st_id3v2_d_ferr3(&pTagI->opts, pFnc, pTagI->pFilen, pFldI,
				"%s is too big for v2.3/4 tag, "
				"size="ST_TFSIZE_PRF_LU" bytes (%.2f MB), max=%u bytes (%u MB), "
				"aborting",
				dbgmsg,
				(Tst_fsize)boSize, (double)boSize / 1024.0 / 1024.0,
				cMAXV34, (Tst_uint32)ST_ID3V2_MAX_AF_FSZ_V34_M);
		return ST_ERR_ABRT;
	}
	return ST_ERR_SUCC;
}

/*
 * Copy binary data either from pSrcBDat or pSrcFn to pFldI->dataBinDat
 */
static Tst_err
ST_TIV2__fa_setBinDat(const char *pFnc, Tst_id3v2_tag_intn *pTagI,
		Tst_id3v2_fldData_intn *pFldI,
		ST_OPTARG(Tst_binobj *pSrcBDat), ST_OPTARG(const Tst_mtes_string *pSrcFn))
{
	Tst_err   res       = ST_ERR_SUCC;
	Tst_fsize tmpDataSz = 0;
	Tst_str   *pFnU8PCh = NULL;

	ST_ASSERTN_IARG(pSrcBDat == NULL && pSrcFn == NULL)

	if (pSrcFn != NULL) {
		res = st_mtes_copyToCharp_utf8(pSrcFn, &pFnU8PCh);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pFnU8PCh)
			return res;
		}
	}
	/* */
	if (pSrcBDat != NULL)
		tmpDataSz = (Tst_fsize)st_binobj_getDataSize(pSrcBDat);
	else {
		Tst_bool errIsTooBig = ST_B_FALSE;

		tmpDataSz = st_sysGetFileSz_byFn(pFnU8PCh, &errIsTooBig);
		if (errIsTooBig) {
			st_id3v2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"file \"%s\" is too big, aborting",
					pFnU8PCh);
			ST_DELPOINT(pFnU8PCh)
			return ST_ERR_ABRT;
		}
	}
	res = ST_TIV2__fa_checkBinDatSize(pFnc, pTagI, pFldI,
			tmpDataSz, pFnU8PCh);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pFnU8PCh)
		return res;
	}
	/* copy the data */
	if (tmpDataSz == 0)
		st_binobj_stc_rsetData(&pFldI->dataBinDat);
	else {
		if (pSrcBDat != NULL)
			res = st_binobj_copy(pSrcBDat, &pFldI->dataBinDat);
		else {
			res = st_binobj_setData_file(&pFldI->dataBinDat, pFnU8PCh,
						pTagI->opts.basOpts.allowLnkInpF);
			if (res != ST_ERR_SUCC)
				st_id3v2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
						"can't copy file '%s'", pFnU8PCh);
		}
	}
	ST_DELPOINT(pFnU8PCh)
	/* */
	if (res == ST_ERR_SUCC &&
			ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		tmpDataSz = (Tst_fsize)st_binobj_getDataSize(&pFldI->dataBinDat);

		if ((Tst_uint32)tmpDataSz < 1024)
			st_id3v2_d_fdeb3(&pTagI->opts, 3, pFnc, pFldI,
					"bin %u bytes", (Tst_uint32)tmpDataSz);
		else if ((Tst_uint32)tmpDataSz < 1024 * 1024)
			st_id3v2_d_fdeb3(&pTagI->opts, 3, pFnc, pFldI,
					"bin %u bytes (%.2f KB)", (Tst_uint32)tmpDataSz,
					(double)tmpDataSz / 1024.0);
		else
			st_id3v2_d_fdeb3(&pTagI->opts, 3, pFnc, pFldI,
					"bin %u bytes (%u MB)", (Tst_uint32)tmpDataSz,
					(Tst_uint32)((double)tmpDataSz / (1024.0 * 1024.0)));
	}
	return res;
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__fa_setFld_numeroidStr(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_int32 num, const Tst_int32 numTot)
{
	Tst_err   res      = ST_ERR_SUCC;
	Tst_str   tmps[100];
	Tst_int32 ftNum    = -1,
	          ftNumTot = -1;
	Tst_mtes_string tmpis;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	/* get old value */
	if ((num < 0 || numTot < 0) &&
			(frID == ST_ID3V2_FID_234_TRCK || frID == ST_ID3V2_FID_234_TPOS)) {
		Tst_uint32 ftNumUI    = 0,
		           ftNumTotUI = 0;

		res = st_id3v2_fldGet_dataStr_pos(pTag, frID, 0, &ftNumUI, &ftNumTotUI);
		if (res != ST_ERR_SUCC && res != ST_ERR_TFNF)
			return res;
		ftNum    = (Tst_int32)ftNumUI;
		ftNumTot = (Tst_int32)ftNumTotUI;
	}
	/* */
	if (num >= 0)
		ftNum = num;
	if (numTot >= 0)
		ftNumTot = numTot;
	/* */
	tmps[0] = 0x00;
	if (ftNum >= 0) {
		if (ftNumTot > 0)
			sprintf((char*)tmps, "%d/%d", ftNum, ftNumTot);
		else
			sprintf((char*)tmps, "%d", ftNum);
	} else if (ftNumTot > 0)
		sprintf((char*)tmps, "0/%d", ftNumTot);
	/* disable old field(s) */
	if (frNr == 0 ||
			frID == ST_ID3V2_FID_234_TRCK ||
			frID == ST_ID3V2_FID_234_TPOS) {
		res = st_id3v2_disableFld_byID_all(pTag, frID, pFrIDstr);
		if (res != ST_ERR_SUCC)
			return res;
	}
	if (tmps[0] == 0x00)
		return ST_ERR_SUCC;

	/* add new field or change field's value */
	if (tmps[0] != 0x00) {
		res = st_mtes_stc_initStr(&tmpis);
		if (res == ST_ERR_SUCC)
			res = st_mtes_copyFromCharp_isoOrUtf8(tmps, ST_B_TRUE, &tmpis);
		if (res == ST_ERR_SUCC)
			res = st_id3v2_fldSet_dataStr(pTag, frID, pFrIDstr, frNr, &tmpis);
		st_mtes_stc_freeStr(&tmpis);
	}
	return res;
}

/******************************************************************************/
