/******************************************************************************/
/* tag_vorbc_gs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Getter/setter functions for Vorbis Tags                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_TAG_VORBC_GS_C
#include "tag_vorbc-prot.h"
/*** */
#include "tag_vorbc_stcs-prot.h"
#include "tag_vorbc_fnc-prot.h"
#undef SRC_TAG_VORBC_GS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_vorbc_fldData*
ST_TVORBC__ite_nextFld(const Tst_vorbc_tag *pTag,
                       const Tst_vorbc_frID frID,
                       ST_OPTARG(const Tst_str *pFrIDstr),
                       ST_OPTARG(const Tst_vorbc_fldData *pFldLast),
                       const Tst_bool onlyValid);
static Tst_vorbc_fldProp*
ST_TVORBC__ite_nextTagFldProp(ST_OPTARG(Tst_vorbc_fldProp *pFldPrLast),
                              const Tst_bool withRestrictions,
                              Tst_vorbc_fldProp *pFldPrNext);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_vorbc_gs_setTag_strrd(Tst_vorbc_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_vorbc_tag_intn*)pTag->pObInternal)->pStrrd = pStrrd;
}

void
st_vorbc_gs_setTag_strwr(Tst_vorbc_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_vorbc_tag_intn*)pTag->pObInternal)->pStrwr = pStrwr;
}

Tst_err
st_vorbc_gs_setTag_filen(Tst_vorbc_tag *pTag, const Tst_str *pFilen)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFilen == NULL)

	return st_sysStrcpy(pFilen,
			&((Tst_vorbc_tag_intn*)pTag->pObInternal)->pFilen);
}

void
st_vorbc_gs_setTag_wasEmbedded(Tst_vorbc_tag *pTag, const Tst_bool yesOrNo)
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_vorbc_tag_intn*)pTag->pObInternal)->wasEmbedded = yesOrNo;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_vorbc_gs_setTag_taggerInfo(Tst_vorbc_tag *pTag, const Tst_mtes_string *pTI)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTI == NULL)

	return st_mtes_strCpy(pTI,
			&((Tst_vorbc_tag_intn*)pTag->pObInternal)->tagger);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_vorbc_gs_setField_props(Tst_vorbc_fldData *pFld,
		Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
	Tst_err res;
	Tst_vorbc_fldData_intn *pFldI;
	Tst_vorbc_fldProp_intn *pFldPrIsrc = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;
	/* */
	st_vorbc_stc_rsetFPr_i(&pFldI->fldPrI);
	pFldI->isOK = ST_B_FALSE;

	/* adjust frID */
	frID = st_vorbc_fnc_adjustFrID(frID, pFrIDstr, &pFldPrIsrc);
	if (frID == ST_VORBC_FID_NONE)
		return ST_ERR_IARG;

	/* now we copy the original field props */
	res = st_vorbc_stc_copyFPr_i(pFrIDstr, pFldPrIsrc, &pFldI->fldPrI);
	if (res == ST_ERR_SUCC && st_sysStrEmpty2(pFldI->fldPrI.pVIDstr))
		res = ST_ERR_FAIL;
	/* */
	if (res != ST_ERR_SUCC)
		st_vorbc_stc_rsetFPr_i(&pFldI->fldPrI);
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_vorbc_gs_setFieldData_stringArr(Tst_vorbc_fldData *pFld,
		Tst_mtes_strArr *pStrArr)
{
	Tst_err res;
	Tst_vorbc_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStrArr == NULL)

	pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;

	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_mtes_strArrCpy(pStrArr, &pFldI->dataSArr);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strArrStrip(&pFldI->dataSArr,
				/*arePairs:*/ST_B_FALSE, NULL);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	pFldI->isOK = (st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0);
	return ST_ERR_SUCC;
}

Tst_err
st_vorbc_gs_setFieldData_picBin(Tst_vorbc_fldData *pFld, Tst_binobj *pBinDat)
{
	Tst_err res;
	Tst_vorbc_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pBinDat == NULL)

	pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;

	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_binobj_copy(pBinDat, &pFldI->dataPicBDat);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	pFldI->isOK = (st_binobj_getDataSize(&pFldI->dataPicBDat) > 0);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_vorbc_gs_setFieldAttr_desc(Tst_vorbc_fldData *pFld, Tst_mtes_string *pStr)
{
	Tst_vorbc_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStr == NULL)

	pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;

	return st_mtes_strCpy(pStr, &pFldI->attrPic.desc);
}

Tst_err
st_vorbc_gs_setFieldAttr_picTp(Tst_vorbc_fldData *pFld,
		const Tst_vorbc_picTp picTp)
{
	Tst_vorbc_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;

	pFldI->attrPic.ptp = picTp;
	return ST_ERR_SUCC;
}

Tst_err
st_vorbc_gs_setFieldAttr_picFmt(Tst_vorbc_fldData *pFld,
		const Tst_utilsFmt_picFmt picFmt)
{
	Tst_str const *pMim;
	Tst_vorbc_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;

	pMim = st_utilsFmt_getPicFmt_mime_byEnum(picFmt);
	return st_sysStrcpy(pMim, &pFldI->attrPic.pMime);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_uint32
st_vorbc_gs_getRawTagSize(const Tst_vorbc_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return st_binobj_getDataSize(
			&((Tst_vorbc_tag_intn*)pTag->pObInternal)->rawTagBDat);
}

Tst_err
st_vorbc_gs_getRawTag(const Tst_vorbc_tag *pTag,
		Tst_binobj *pRawTag)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pRawTag == NULL)

	return st_binobj_copy(
			&((Tst_vorbc_tag_intn*)pTag->pObInternal)->rawTagBDat,
			pRawTag);
}

Tst_binobj*
st_vorbc_gs_getRawTagPointer(const Tst_vorbc_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return &((Tst_vorbc_tag_intn*)pTag->pObInternal)->rawTagBDat;
}

Tst_err
st_vorbc_gs_getRawTagIntoFile(const Tst_vorbc_tag *pTag,
		const Tst_str *pOutpFilen)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pOutpFilen == NULL)

	return st_binobj_copyIntoFile(
			&((Tst_vorbc_tag_intn*)pTag->pObInternal)->rawTagBDat,
			pOutpFilen);
}

/*----------------------------------------------------------------------------*/

const Tst_tagBasics*
st_vorbc_gs_getTag_tbas(const Tst_vorbc_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return &((Tst_vorbc_tag_intn*)pTag->pObInternal)->tbas;
}

const Tst_str*
st_vorbc_gs_getTag_filen(const Tst_vorbc_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_vorbc_tag_intn*)pTag->pObInternal)->pFilen;
}

Tst_bool
st_vorbc_gs_getTag_wasEmbedded(const Tst_vorbc_tag *pTag)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_vorbc_tag_intn*)pTag->pObInternal)->wasEmbedded;
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_vorbc_gs_getTag_amountValidFlds(const Tst_vorbc_tag *pTag)
{
	Tst_uint32 n = 0;
	Tst_vorbc_fldData *pItFld = NULL;

	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL)
		++n;
	return n;
}

Tst_uint32
st_vorbc_gs_getTag_amountSkippedFields(const Tst_vorbc_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_vorbc_tag_intn*)pTag->pObInternal)->flds.skipCnt;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_vorbc_gs_getTag_taggerInfo(const Tst_vorbc_tag *pTag, Tst_mtes_string *pTI)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTI == NULL)

	return st_mtes_strCpy(&((Tst_vorbc_tag_intn*)pTag->pObInternal)->tagger, pTI);
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_vorbc_gs_getField_nr(const Tst_vorbc_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_vorbc_fldData_intn*)pFld->pObInternal)->fnr;
}

Tst_err
st_vorbc_gs_getField_props(const Tst_vorbc_fldData *pFld,
		Tst_vorbc_fldProp *pFldPr)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFldPr == NULL)

	pFldPr->pObInternal = &((Tst_vorbc_fldData_intn*)pFld->pObInternal)->fldPrI;
	return ST_ERR_SUCC;
}

Tst_err
st_vorbc_gs_getFieldData_stringArr(const Tst_vorbc_fldData *pFld,
		Tst_mtes_strArr *pStrArr)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStrArr == NULL)

	return st_mtes_strArrCpy(
			&((Tst_vorbc_fldData_intn*)pFld->pObInternal)->dataSArr,
			pStrArr);
}

Tst_uint32
st_vorbc_gs_getFieldData_stringArr_elemCount(const Tst_vorbc_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return st_mtes_strArrGetElemCount(
			&((Tst_vorbc_fldData_intn*)pFld->pObInternal)->dataSArr);
}

Tst_err
st_vorbc_gs_getFieldData_stringArr_elem(const Tst_vorbc_fldData *pFld,
		const Tst_uint32 ix, Tst_mtes_string *pStr)
{
	const Tst_mtes_string *pSrc;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			ix == 0 || pStr == NULL)

	pSrc = st_mtes_strArrGetElem(
			&((Tst_vorbc_fldData_intn*)pFld->pObInternal)->dataSArr,
			ix);
	if (pSrc == NULL) {
		st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
		return ST_ERR_SUCC;
	}
	return st_mtes_strCpy(pSrc, pStr);
}

Tst_uint32
st_vorbc_gs_getFieldData_picBinSz(const Tst_vorbc_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return st_binobj_getDataSize(
			&((Tst_vorbc_fldData_intn*)pFld->pObInternal)->dataPicBDat);
}

Tst_err
st_vorbc_gs_getFieldData_picBin(const Tst_vorbc_fldData *pFld,
		Tst_binobj *pBinDat)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pBinDat == NULL)

	return st_binobj_copy(
			&((Tst_vorbc_fldData_intn*)pFld->pObInternal)->dataPicBDat,
			pBinDat);
}

Tst_err
st_vorbc_gs_getFieldData_picBinToFile(const Tst_vorbc_fldData *pFld,
		const Tst_str *pOutpFilen)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pOutpFilen == NULL)

	return st_binobj_copyIntoFile(
			&((Tst_vorbc_fldData_intn*)pFld->pObInternal)->dataPicBDat,
			pOutpFilen);
}

const Tst_str*
st_vorbc_gs_getFieldAttr_picMime(const Tst_vorbc_fldData *pFld)
{
	ST_ASSERTN_NULL(pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_vorbc_fldData_intn*)pFld->pObInternal)->attrPic.pMime;
}

Tst_err
st_vorbc_gs_getFieldAttr_picDesc(const Tst_vorbc_fldData *pFld,
		Tst_mtes_string *pDesc)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL || pDesc == NULL)

	return st_mtes_strCpy(
			&((Tst_vorbc_fldData_intn*)pFld->pObInternal)->attrPic.desc,
			pDesc);
}

Tst_vorbc_picTp
st_vorbc_gs_getFieldAttr_picType(const Tst_vorbc_fldData *pFld)
{
	ST_ASSERTN_NUM(ST_VORBC_PT_NONE,
			pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_vorbc_fldData_intn*)pFld->pObInternal)->attrPic.ptp;
}

/*----------------------------------------------------------------------------*/

Tst_vorbc_frTp
st_vorbc_gs_getFieldProp_typ(const Tst_vorbc_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_VORBC_FTP_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_vorbc_fldProp_intn*)pFldPr->pObInternal)->typ;
}

Tst_vorbc_frID
st_vorbc_gs_getFieldProp_id(const Tst_vorbc_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_VORBC_FID_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_vorbc_fldProp_intn*)pFldPr->pObInternal)->id;
}

const Tst_str*
st_vorbc_gs_getFieldProp_idStr(const Tst_vorbc_fldProp *pFldPr)
{
	Tst_str const *pID;
	Tst_vorbc_fldProp_intn *pFldPrI;

	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	pFldPrI = (Tst_vorbc_fldProp_intn*)pFldPr->pObInternal;
	pID     = (const Tst_str*)pFldPrI->pVIDstr;
	if (pID == NULL)
		pID = (const Tst_str*)pFldPrI->pCIDstr;
	return pID;
}

const Tst_str*
st_vorbc_gs_getFieldProp_caption(const Tst_vorbc_fldProp *pFldPr)
{
	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	return (const Tst_str*)((Tst_vorbc_fldProp_intn const*)pFldPr->pObInternal)->pCCap;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_vorbc_fldData*
st_vorbc_ite_nextFld(const Tst_vorbc_tag *pTag,
		ST_OPTARG(const Tst_vorbc_fldData *pFldLast))
{
	return ST_TVORBC__ite_nextFld(pTag, ST_VORBC_FID_NONE, NULL,
			pFldLast, /*onlyValid:*/ST_B_FALSE);
}

Tst_vorbc_fldData*
st_vorbc_ite_nextValidFld(const Tst_vorbc_tag *pTag,
		ST_OPTARG(const Tst_vorbc_fldData *pFldLast))
{
	return ST_TVORBC__ite_nextFld(pTag, ST_VORBC_FID_NONE, NULL,
			pFldLast, /*onlyValid:*/ST_B_TRUE);
}

Tst_vorbc_fldData*
st_vorbc_ite_nextValidFld_byID(const Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_vorbc_fldData *pFldLast))
{
	return ST_TVORBC__ite_nextFld(pTag, frID, pFrIDstr,
			pFldLast, /*onlyValid:*/ST_B_TRUE);
}

/*----------------------------------------------------------------------------*/

Tst_vorbc_fldProp*
st_vorbc_ite_nextTagFldProp(ST_OPTARG(Tst_vorbc_fldProp *pFldPrLast),
		Tst_vorbc_fldProp *pFldPrNext)
{
	return ST_TVORBC__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_TRUE, pFldPrNext);
}

Tst_vorbc_fldProp*
st_vorbc_ite_nextTagFldProp_all(ST_OPTARG(Tst_vorbc_fldProp *pFldPrLast),
		Tst_vorbc_fldProp *pFldPrNext)
{
	return ST_TVORBC__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_FALSE, pFldPrNext);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_vorbc_fldData*
ST_TVORBC__ite_nextFld(const Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_vorbc_fldData *pFldLast), const Tst_bool onlyValid)
{
	Tst_uint32 luix,
	           z,
	           fcnt;
	Tst_vorbc_tag_intn     *pTagI;
	Tst_vorbc_fldData      *pFld;
	Tst_vorbc_fldData_intn *pFldI;

	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

	luix = (pFldLast == NULL ? 0 :
			((Tst_vorbc_fldData_intn*)pFldLast->pObInternal)->uniqueIx);
	fcnt = st_dynlist_getElementCount(&pTagI->flds.list);
	for (z = 1; z <= fcnt; z++) {
		pFld = (Tst_vorbc_fldData*)st_dynlist_getElement(&pTagI->flds.list, z);
		if (pFld == NULL)
			return NULL;
		pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;
		if ((onlyValid && ! pFldI->isOK) ||
				pFldI->fldPrI.typ == ST_VORBC_FTP_NONE ||
				pFldI->uniqueIx <= luix ||
				((frID != ST_VORBC_FID_NONE || pFrIDstr != NULL) &&
					! st_vorbc_fnc_matchFrIDs(pFldI->fldPrI.id, pFldI->fldPrI.pVIDstr,
								frID, (const char*)pFrIDstr)))
			continue;
		return pFld;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/

static Tst_vorbc_fldProp*
ST_TVORBC__ite_nextTagFldProp(ST_OPTARG(Tst_vorbc_fldProp *pFldPrLast),
		const Tst_bool withRestrictions, Tst_vorbc_fldProp *pFldPrNext)
{
	Tst_uint32 z         = 0;
	Tst_bool   lastFnd   = ST_B_TRUE;
	char const *pLastCID = NULL;
	Tst_vorbc_frID         lastID = ST_VORBC_FID_NONE;
	Tst_vorbc_fldProp_intn *pFldPrI;

	ST_ASSERTN_NULL(pFldPrNext == NULL)

	if (pFldPrLast != NULL && pFldPrLast->pObInternal != NULL) {
		pFldPrI  = (Tst_vorbc_fldProp_intn*)pFldPrLast->pObInternal;
		lastID   = pFldPrI->id;
		pLastCID = pFldPrI->pCIDstr;
		lastFnd  = ST_B_FALSE;
	}
	pFldPrNext->pObInternal = NULL;

	while (ST_B_TRUE) {
		pFldPrI = (Tst_vorbc_fldProp_intn*)&ST_VORBC_FLDPROPS[z++];

		if (pFldPrI->id == ST_VORBC_FID_NONE || pFldPrI->pCIDstr == NULL ||
				pFldPrI->pCCap == NULL)
			break;
		if (withRestrictions && ! pFldPrI->retFromIte)
			continue;
		if (! lastFnd) {
			if (pFldPrI->id == lastID &&
					(pLastCID == NULL ||
						st_sysStrcmp2(ST_B_FALSE, pLastCID, pFldPrI->pCIDstr)))
				lastFnd = ST_B_TRUE;
			continue;
		}
		pFldPrNext->pObInternal = pFldPrI;
		return pFldPrNext;
	}
	return NULL;
}

/******************************************************************************/
