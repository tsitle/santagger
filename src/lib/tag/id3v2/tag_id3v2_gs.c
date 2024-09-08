/******************************************************************************/
/* tag_id3v2_gs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Getter/Setter functions for ID3v2 Tags                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 11.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj.h>
#include <santag/common/dynlist.h>
#include <santag/common/string_mte.h>
#include <santag/common/sys_fnc.h>
#include <santag/tag/tag_basics.h>
#include <santag/tag/tag_id3v2.h>
/** */
#define SRC_TAG_ID3V2_GS_C
#include "tag_id3v2_gs-prot.h"
/*** */
#include "tag_id3v2_zchk-pp.h"
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2_stcs-prot.h"
#undef SRC_TAG_ID3V2_GS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_id3v2_fldData*
ST_TIV2__ite_nextFld(const Tst_id3v2_tag *pTag,
                     const Tst_id3v2_frID frID,
                     ST_OPTARG(const Tst_str *pFrIDstr),
                     ST_OPTARG(const Tst_id3v2_fldData *pFldLast),
                     const Tst_bool onlyValid,
                     ST_OPTARG(Tst_uint32 *pIndex));
static Tst_id3v2_fldProp*
ST_TIV2__ite_nextTagFldProp(ST_OPTARG(Tst_id3v2_fldProp *pFldPrLast),
                            const Tst_bool withRestrictions,
                            Tst_id3v2_fldProp *pFldPrNext);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_id3v2_gs_setTag_strrd(Tst_id3v2_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v2_tag_intn*)pTag->pObInternal)->pStrrd = pStrrd;
}

void
st_id3v2_gs_setTag_strwr(Tst_id3v2_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v2_tag_intn*)pTag->pObInternal)->pStrwr = pStrwr;
}

Tst_err
st_id3v2_gs_setTag_filen(Tst_id3v2_tag *pTag, const Tst_str *pFilen)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFilen == NULL)

	return st_sysStrcpy(pFilen,
			&((Tst_id3v2_tag_intn*)pTag->pObInternal)->pFilen);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_gs_setTag_majorVersion(Tst_id3v2_tag *pTag, const Tst_uint32 vers)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	if (vers < 2 || vers > 4)
		return ST_ERR_IARG;
	((Tst_id3v2_tag_intn*)pTag->pObInternal)->verMaj = vers;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_gs_setTag_taggerInfo(Tst_id3v2_tag *pTag, const Tst_mtes_string *pTI)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTI == NULL)

	return st_mtes_strCpy(pTI,
			&((Tst_id3v2_tag_intn*)pTag->pObInternal)->tagger);
}

/*----------------------------------------------------------------------------*/

void
st_id3v2_gs_setField_wasDisplayed(Tst_id3v2_fldData *pFld, const Tst_bool wasIt)
{
	ST_ASSERTN_VOID(pFld == NULL || pFld->pObInternal == NULL)

	((Tst_id3v2_fldData_intn*)pFld->pObInternal)->wasDispld = wasIt;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_gs_setField_props(Tst_id3v2_fldData *pFld,
		Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pFldI;
	Tst_id3v2_fldProp_intn *pFldPrIsrc = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;
	/* */
	st_id3v2_stc_rsetFPr_i(&pFldI->fldPrI);
	pFldI->isOK = ST_B_FALSE;

	/* adjust frID */
	frID = st_id3v2_fnc_adjustFrID(frID, pFrIDstr, &pFldPrIsrc);
	if (frID == ST_ID3V2_FID_NONE)
		return ST_ERR_IARG;

	/* now we copy the original field props */
	res = st_id3v2_stc_copyFPr_i((pFldPrIsrc->verFl == 1 ? 2 : 3),
			pFrIDstr, pFldPrIsrc, &pFldI->fldPrI);
	if (res != ST_ERR_SUCC)
		st_id3v2_stc_rsetFPr_i(&pFldI->fldPrI);
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_gs_setFieldData_stringArr(Tst_id3v2_fldData *pFld,
		Tst_mtes_strArr *pStrArr)
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStrArr == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_mtes_strArrCpy(pStrArr, &pFldI->dataSArr);
	if (res != ST_ERR_SUCC)
		return res;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldData_bin(Tst_id3v2_fldData *pFld, Tst_binobj *pBinDat)
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pBinDat == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_binobj_copy(pBinDat, &pFldI->dataBinDat);
	if (res != ST_ERR_SUCC)
		return res;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldData_uval(Tst_id3v2_fldData *pFld, const Tst_uint32 uval)
{
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->dataUVal = uval;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldData_bval(Tst_id3v2_fldData *pFld, const Tst_byte bval)
{
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->dataBVal = bval;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_gs_setFieldAttr_desc(Tst_id3v2_fldData *pFld, Tst_mtes_string *pStr)
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStr == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_mtes_strCpy(pStr, &pFldI->attrDesc);
	if (res != ST_ERR_SUCC)
		return res;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldAttr_lang(Tst_id3v2_fldData *pFld, const Tst_str *pLangISO)
{
	Tst_uint32 len;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pLangISO == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->isOK = ST_B_FALSE;
	/* */
	len = st_sysStrlen2(pLangISO);
	if (len > 3)
		len = 3;
	if (len > 0)
		memcpy(pFldI->attrLang, pLangISO, len);
	pFldI->attrLang[len] = 0;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldAttr_geobFn(Tst_id3v2_fldData *pFld, Tst_mtes_string *pFilen)
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFilen == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_mtes_strCpy(pFilen, &pFldI->attrGeoFn);
	if (res != ST_ERR_SUCC)
		return res;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldAttr_geobMime(Tst_id3v2_fldData *pFld,
		const Tst_str *pMimeISO)
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pMimeISO == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_sysStrcpy(pMimeISO, &pFldI->pAttrGeoMime);
	if (res != ST_ERR_SUCC)
		return res;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldAttr_picTp(Tst_id3v2_fldData *pFld,
		const Tst_id3v2_picTp picTp)
{
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->attrPicTp = picTp;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldAttr_picFmt(Tst_id3v2_fldData *pFld,
		const Tst_utilsFmt_picFmt picFmt)
{
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->attrPicFmt = picFmt;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldAttr_sltTSF(Tst_id3v2_fldData *pFld,
		const Tst_id3v2_sltTSF tsf)
{
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->attrSltTSF = tsf;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

Tst_err
st_id3v2_gs_setFieldAttr_sltCTP(Tst_id3v2_fldData *pFld,
		const Tst_id3v2_sltCTp ctp)
{
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->attrSltCTP = ctp;
	return st_id3v2_chk_checkFld(NULL, ST_B_FALSE, pFld);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const Tst_tagBasics*
st_id3v2_gs_getTag_tbas(const Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return (Tst_tagBasics*)&((Tst_id3v2_tag_intn*)pTag->pObInternal)->tbas;
}

const Tst_str*
st_id3v2_gs_getTag_filen(const Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v2_tag_intn*)pTag->pObInternal)->pFilen;
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_id3v2_gs_getTag_majorVersion(const Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v2_tag_intn*)pTag->pObInternal)->verMaj;
}

Tst_uint32
st_id3v2_gs_getTag_minorVersion(const Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v2_tag_intn*)pTag->pObInternal)->verMin;
}

Tst_uint32
st_id3v2_gs_getTag_amountFlds(const Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return st_dynlist_getElementCount(
			&((Tst_id3v2_tag_intn*)pTag->pObInternal)->flds.list);
}

Tst_uint32
st_id3v2_gs_getTag_amountValidFlds_byID(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
	Tst_uint32 n = 0;
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(0, pItFldI == NULL)

		if (st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
					frID, (const char*)pFrIDstr))
			++n;
	}
	return n;
}

Tst_uint32
st_id3v2_gs_getTag_amountValidFlds_all(Tst_id3v2_tag *pTag)
{
	Tst_uint32 n = 0;
	Tst_id3v2_fldData *pItFld = NULL;

	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL)
		++n;
	return n;
}

Tst_uint32
st_id3v2_gs_getTag_amountSkippedFields(Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v2_tag_intn*)pTag->pObInternal)->flds.skipCnt;
}

/*
 * Count errors/warnings in tag header and body,
 *   excluding errors in frames
 */
Tst_uint32
st_id3v2_gs_getTag_tagErrorCount(const Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v2_tag_intn*)pTag->pObInternal)->tgErrCnt;
}

Tst_err
st_id3v2_gs_getTag_tagErrors(const Tst_id3v2_tag *pTag,
		Tst_id3v2_tagErrs *pTgErrors)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTgErrors == NULL)

	memcpy(pTgErrors,
			&((Tst_id3v2_tag_intn*)pTag->pObInternal)->tgErrs,
			sizeof(Tst_id3v2_tagErrs));
	return ST_ERR_SUCC;
}

/*
 * Count errors/warnings in all frames
 */
Tst_uint32
st_id3v2_gs_getTag_fldsErrorCount(const Tst_id3v2_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v2_tag_intn*)pTag->pObInternal)->flds.errCnt;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_gs_getTag_taggerInfo(const Tst_id3v2_tag *pTag, Tst_mtes_string *pTI)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTI == NULL)

	return st_mtes_strCpy(&((Tst_id3v2_tag_intn*)pTag->pObInternal)->tagger, pTI);
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_id3v2_gs_getField_isOK(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->isOK;
}

Tst_bool
st_id3v2_gs_getField_wasDisplayed(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->wasDispld;
}

Tst_uint32
st_id3v2_gs_getField_nr(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->fnr;
}

Tst_err
st_id3v2_gs_getField_props(const Tst_id3v2_fldData *pFld,
		Tst_id3v2_fldProp *pFldPr)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFldPr == NULL)

	pFldPr->pObInternal = &((Tst_id3v2_fldData_intn*)pFld->pObInternal)->fldPrI;
	return ST_ERR_SUCC;
}

Tst_uint32
st_id3v2_gs_getField_errorCount(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->frErrCnt;
}

Tst_err
st_id3v2_gs_getField_errors(const Tst_id3v2_fldData *pFld,
		Tst_id3v2_fldErrs *pFldErrors)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFldErrors == NULL)

	memcpy(pFldErrors,
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->errs,
			sizeof(Tst_id3v2_fldErrs));
	return ST_ERR_SUCC;
}

Tst_uint32
st_id3v2_gs_getFieldData_binSz(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return st_binobj_getDataSize(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataBinDat);
}

Tst_err
st_id3v2_gs_getFieldData_bin(const Tst_id3v2_fldData *pFld,
		Tst_binobj *pBinDat)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pBinDat == NULL)

	return st_binobj_copy(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataBinDat,
			pBinDat);
}

Tst_err
st_id3v2_gs_getFieldData_binToFile(const Tst_id3v2_fldData *pFld,
		const Tst_str *pOutpFilen)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pOutpFilen == NULL)

	return st_binobj_copyIntoFile(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataBinDat,
			pOutpFilen);
}

Tst_uint32
st_id3v2_gs_getFieldData_uval(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataUVal;
}

Tst_byte
st_id3v2_gs_getFieldData_bval(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataBVal;
}

Tst_err
st_id3v2_gs_getFieldData_stringArr(const Tst_id3v2_fldData *pFld,
		Tst_mtes_strArr *pStrArr)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	return st_mtes_strArrCpy(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataSArr,
			pStrArr);
}

Tst_uint32
st_id3v2_gs_getFieldData_stringArr_elemCount(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return st_mtes_strArrGetElemCount(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataSArr);
}

Tst_err
st_id3v2_gs_getFieldData_stringArr_elem(const Tst_id3v2_fldData *pFld,
		const Tst_uint32 ix, Tst_mtes_string *pStr)
{
	const Tst_mtes_string *pSrc;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			ix == 0 || pStr == NULL)

	pSrc = st_mtes_strArrGetElem(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->dataSArr,
			ix);
	if (pSrc == NULL) {
		st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
		return ST_ERR_SUCC;
	}
	return st_mtes_strCpy(pSrc, pStr);
}

Tst_id3v2_picTp
st_id3v2_gs_getFieldAttr_picType(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(ST_ID3V2_PT_NONE,
			pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrPicTp;
}

Tst_utilsFmt_picFmt
st_id3v2_gs_getFieldAttr_picFmt(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(ST_UTILSFMT_PF_NONE,
			pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrPicFmt;
}

Tst_id3v2_sltTSF
st_id3v2_gs_getFieldAttr_syltTSF(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(ST_ID3V2_SLTTSF_NONE,
			pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrSltTSF;
}

Tst_id3v2_sltCTp
st_id3v2_gs_getFieldAttr_syltContTp(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NUM(ST_ID3V2_SLTCTP_NONE,
			pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrSltCTP;
}

const Tst_str*
st_id3v2_gs_getFieldAttr_geobMime(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NULL(pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->pAttrGeoMime;
}

Tst_err
st_id3v2_gs_getFieldAttr_geobFilen(const Tst_id3v2_fldData *pFld,
		Tst_mtes_string *pFn)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFn == NULL)

	return st_mtes_strCpy(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrGeoFn,
			pFn);
}

Tst_bool
st_id3v2_gs_getFieldAttr_geobFilen_isEmpty(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pFld == NULL || pFld->pObInternal == NULL)

	return st_mtes_isStrEmptyBy(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrGeoFn);
}

const Tst_str*
st_id3v2_gs_getFieldAttr_lang(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_NULL(pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrLang;
}

Tst_err
st_id3v2_gs_getFieldAttr_desc(const Tst_id3v2_fldData *pFld,
		Tst_mtes_string *pSCD)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pSCD == NULL)

	return st_mtes_strCpy(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrDesc,
			pSCD);
}

Tst_bool
st_id3v2_gs_getFieldAttr_desc_isEmpty(const Tst_id3v2_fldData *pFld)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pFld == NULL || pFld->pObInternal == NULL)

	return st_mtes_isStrEmptyBy(
			&((Tst_id3v2_fldData_intn*)pFld->pObInternal)->attrDesc);
}

/*----------------------------------------------------------------------------*/

Tst_id3v2_frTp
st_id3v2_gs_getFieldProp_typ(const Tst_id3v2_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_ID3V2_FTP_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_id3v2_fldProp_intn*)pFldPr->pObInternal)->typ;
}

Tst_id3v2_frID
st_id3v2_gs_getFieldProp_id(const Tst_id3v2_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_ID3V2_FID_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_id3v2_fldProp_intn*)pFldPr->pObInternal)->id;
}

const Tst_str*
st_id3v2_gs_getFieldProp_idStr(const Tst_id3v2_fldProp *pFldPr)
{
	Tst_str const *pIDstr;
	Tst_id3v2_fldProp_intn const *pFldPrI;

	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	pFldPrI = (Tst_id3v2_fldProp_intn*)pFldPr->pObInternal;
	pIDstr  = (Tst_str const*)pFldPrI->pIDstr;
	if (pIDstr == NULL || pIDstr[0] == ' ' || pIDstr[0] == '-') {
		if (pFldPrI->verFl == 1) {  /* only if props are exclusively for v2.2 */
			pIDstr = (Tst_str const*)pFldPrI->vid02str;
			if (pIDstr[0] == 0x00 || pIDstr[0] == ' ' || pIDstr[0] == '-')
				pIDstr = (Tst_str const*)pFldPrI->pCID02str;
		} else if ((pFldPrI->verFl & 2) == 2 || (pFldPrI->verFl & 4) == 4) {
			pIDstr = (Tst_str const*)pFldPrI->vid34str;
			if (pIDstr[0] == 0x00 || pIDstr[0] == ' ' || pIDstr[0] == '-')
				pIDstr = (Tst_str const*)pFldPrI->pCID34str;
		}
	}
	if (pIDstr != NULL && (pIDstr[0] == ' ' || pIDstr[0] == '-'))
		pIDstr = NULL;
	return pIDstr;
}

const Tst_str*
st_id3v2_gs_getFieldProp_idStr_v02(const Tst_id3v2_fldProp *pFldPr)
{
	Tst_str const *pIDstr = NULL;
	Tst_id3v2_fldProp_intn const *pFldPrI;

	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	pFldPrI = (Tst_id3v2_fldProp_intn*)pFldPr->pObInternal;
	/*if ((pFldPrI->verFl & 1) == 1)*/ {
		pIDstr = (const Tst_str*)pFldPrI->vid02str;
		if (pIDstr[0] == 0x00 || pIDstr[0] == ' ' || pIDstr[0] == '-')
			pIDstr = (Tst_str const*)pFldPrI->pCID02str;
	}
	if (pIDstr != NULL && (pIDstr[0] == ' ' || pIDstr[0] == '-'))
		pIDstr = NULL;
	return pIDstr;
}

const Tst_str*
st_id3v2_gs_getFieldProp_idStr_v34(const Tst_id3v2_fldProp *pFldPr)
{
	Tst_str const *pIDstr = NULL;
	Tst_id3v2_fldProp_intn const *pFldPrI;

	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	pFldPrI = (Tst_id3v2_fldProp_intn*)pFldPr->pObInternal;
	/*if ((pFldPrI->verFl & 2) == 2 || (pFldPrI->verFl & 4) == 4)*/ {
		pIDstr = (const Tst_str*)pFldPrI->vid34str;
		if (pIDstr[0] == 0x00 || pIDstr[0] == ' ' || pIDstr[0] == '-')
			pIDstr = (Tst_str const*)pFldPrI->pCID34str;
	}
	if (pIDstr != NULL && (pIDstr[0] == ' ' || pIDstr[0] == '-'))
		pIDstr = NULL;
	return pIDstr;
}

const Tst_str*
st_id3v2_gs_getFieldProp_caption(const Tst_id3v2_fldProp *pFldPr)
{
	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	return (Tst_str const*)((Tst_id3v2_fldProp_intn const*)pFldPr->pObInternal)->pCCap;
}

Tst_uint32
st_id3v2_gs_getFieldProp_verFl(const Tst_id3v2_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(0, pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_id3v2_fldProp_intn*)pFldPr->pObInternal)->verFl;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_id3v2_fldData*
st_id3v2_ite_nextFld_wIx(const Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_id3v2_fldData *pFldLast),
		ST_OPTARG(Tst_uint32 *pIndex))
{
	return ST_TIV2__ite_nextFld(pTag, ST_ID3V2_FID_NONE, NULL,
			pFldLast, /*onlyVal:*/ST_B_FALSE, pIndex);
}

Tst_id3v2_fldData*
st_id3v2_ite_nextFld(const Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_id3v2_fldData *pFldLast))
{
	return ST_TIV2__ite_nextFld(pTag, ST_ID3V2_FID_NONE, NULL,
			pFldLast, /*onlyVal:*/ST_B_FALSE, NULL);
}

Tst_id3v2_fldData*
st_id3v2_ite_nextValidFld_wIx(const Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_id3v2_fldData *pFldLast),
		ST_OPTARG(Tst_uint32 *pIndex))
{
	return ST_TIV2__ite_nextFld(pTag, ST_ID3V2_FID_NONE, NULL,
			pFldLast, /*onlyVal:*/ST_B_TRUE, pIndex);
}

Tst_id3v2_fldData*
st_id3v2_ite_nextValidFld(const Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_id3v2_fldData *pFldLast))
{
	return ST_TIV2__ite_nextFld(pTag, ST_ID3V2_FID_NONE, NULL,
			pFldLast, /*onlyVal:*/ST_B_TRUE, NULL);
}

Tst_id3v2_fldData*
st_id3v2_ite_nextValidFld_byID(const Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_id3v2_fldData *pFldLast))
{
	return ST_TIV2__ite_nextFld(pTag, frID, pFrIDstr,
			pFldLast, /*onlyVal:*/ST_B_TRUE, NULL);
}

/*----------------------------------------------------------------------------*/

Tst_id3v2_fldProp*
st_id3v2_ite_nextTagFldProp(ST_OPTARG(Tst_id3v2_fldProp *pFldPrLast),
		Tst_id3v2_fldProp *pFldPrNext)
{
	return ST_TIV2__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_TRUE, pFldPrNext);
}

Tst_id3v2_fldProp*
st_id3v2_ite_nextTagFldProp_all(ST_OPTARG(Tst_id3v2_fldProp *pFldPrLast),
		Tst_id3v2_fldProp *pFldPrNext)
{
	return ST_TIV2__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_FALSE, pFldPrNext);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_id3v2_fldData*
ST_TIV2__ite_nextFld(const Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_id3v2_fldData *pFldLast), const Tst_bool onlyValid,
		ST_OPTARG(Tst_uint32 *pIndex))
{
	Tst_uint32 luix,
	           z,
	           fcnt;
	Tst_id3v2_tag_intn     *pTagI;
	Tst_id3v2_fldData      *pFld;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	if (pIndex != NULL)
		*pIndex = 0;

	luix = (pFldLast == NULL ? 0 :
			((Tst_id3v2_fldData_intn*)pFldLast->pObInternal)->uniqueIx);
	fcnt = st_dynlist_getElementCount(&pTagI->flds.list);
	for (z = 1; z <= fcnt; z++) {
		pFld = (Tst_id3v2_fldData*)st_dynlist_getElement(&pTagI->flds.list, z);
		ST_ASSERTN_NULL(pFld == NULL || pFld->pObInternal == NULL)
		pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

		if ((onlyValid && ! pFldI->isOK) || pFldI->uniqueIx <= luix ||
				((frID != ST_ID3V2_FID_NONE || pFrIDstr != NULL) &&
					! st_id3v2_fnc_matchFrIDs(pFldI->fldPrI.id, pFldI->fldPrI.pIDstr,
								frID, (const char*)pFrIDstr)))
			continue;
		if (pIndex != NULL)
			*pIndex = z;
		return pFld;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/

static Tst_id3v2_fldProp*
ST_TIV2__ite_nextTagFldProp(ST_OPTARG(Tst_id3v2_fldProp *pFldPrLast),
		const Tst_bool withRestrictions, Tst_id3v2_fldProp *pFldPrNext)
{
	Tst_uint32 z,
	           whichArr;
	Tst_bool   lastFnd      = ST_B_TRUE,
	           wasLastInIgn = ST_B_FALSE;
	char const *pLastCID02  = NULL,
	           *pLastCID34  = NULL;
	Tst_id3v2_frID               lastID = ST_ID3V2_FID_NONE;
	Tst_id3v2_fldProp_intn const *pFldPrArr;
	Tst_id3v2_fldProp_intn       *pFldPrI;

	ST_ASSERTN_NULL(pFldPrNext == NULL)

	if (pFldPrLast != NULL && pFldPrLast->pObInternal != NULL) {
		pFldPrI    = (Tst_id3v2_fldProp_intn*)pFldPrLast->pObInternal;
		lastID     = pFldPrI->id;
		pLastCID02 = pFldPrI->pCID02str;
		pLastCID34 = pFldPrI->pCID34str;
		lastFnd    = ST_B_FALSE;
		if (lastID != ST_ID3V2_FID_NONE)
			wasLastInIgn = st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS_IGN,
					lastID, NULL);
	}
	pFldPrNext->pObInternal = NULL;

	for (whichArr = (wasLastInIgn ? 1 : 0); whichArr < 2; whichArr++) {
		pFldPrArr = (Tst_id3v2_fldProp_intn const*)(whichArr == 0 ?
					ST_ID3V2_FLDPROPS : ST_ID3V2_FLDPROPS_IGN);
		z         = 0;
		while (ST_B_TRUE) {
			pFldPrI = (Tst_id3v2_fldProp_intn*)&pFldPrArr[z++];

			if (pFldPrI->id == ST_ID3V2_FID_NONE || pFldPrI->pCID02str == NULL ||
					pFldPrI->pCID34str == NULL || pFldPrI->pCCap == NULL)
				break;
			if (withRestrictions && ! pFldPrI->retFromIte)
				continue;
			if (! lastFnd) {
				if (pFldPrI->id == lastID &&
						(pLastCID02 == NULL ||
							st_sysStrcmp2(ST_B_TRUE, pLastCID02, pFldPrI->pCID02str)) &&
						(pLastCID34 == NULL ||
							st_sysStrcmp2(ST_B_TRUE, pLastCID34, pFldPrI->pCID34str)))
					lastFnd = ST_B_TRUE;
				continue;
			}
			pFldPrNext->pObInternal = pFldPrI;
			return pFldPrNext;
		}
	}
	return NULL;
}

/******************************************************************************/
