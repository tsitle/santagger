/******************************************************************************/
/* tag_apev2_gs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Getter/setter functions for APEv2 Tags                                     */
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

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj.h>
#include <santag/common/dynlist.h>
#include <santag/common/sys_fnc.h>
#include <santag/common/string_mte.h>
#include <santag/tag/tag_apev2.h>
/** */
#define SRC_TAG_APEV2_GS_C
#include "tag_apev2_gs-prot.h"
/*** */
#include "tag_apev2_fnc-prot.h"
#include "tag_apev2_stcs-prot.h"
#undef SRC_TAG_APEV2_GS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_apev2_fldData*
ST_TAV2__ite_nextFld(const Tst_apev2_tag *pTag,
                     const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
                     ST_OPTARG(const Tst_apev2_fldData *pFldLast),
                     const Tst_bool onlyValid);
static Tst_apev2_fldProp*
ST_TAV2__ite_nextTagFldProp(ST_OPTARG(Tst_apev2_fldProp *pFldPrLast),
                            const Tst_bool withRestrictions,
                            Tst_apev2_fldProp *pFldPrNext);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_apev2_gs_setTag_strrd(Tst_apev2_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_apev2_tag_intn*)pTag->pObInternal)->pStrrd = pStrrd;
}

void
st_apev2_gs_setTag_strwr(Tst_apev2_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_apev2_tag_intn*)pTag->pObInternal)->pStrwr = pStrwr;
}

Tst_err
st_apev2_gs_setTag_filen(Tst_apev2_tag *pTag, const Tst_str *pFilen)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFilen == NULL)

	return st_sysStrcpy(pFilen,
			&((Tst_apev2_tag_intn*)pTag->pObInternal)->pFilen);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_gs_setTag_taggerInfo(Tst_apev2_tag *pTag, const Tst_mtes_string *pTI)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTI == NULL)

	return st_mtes_strCpy(pTI,
			&((Tst_apev2_tag_intn*)pTag->pObInternal)->tagger);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_gs_setField_props(Tst_apev2_fldData *pFld,
		Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
	Tst_err res;
	Tst_apev2_fldData_intn *pFldI;
	Tst_apev2_fldProp_intn *pFldPrIsrc = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;
	/* */
	st_apev2_stc_rsetFPr_i(&pFldI->fldPrI);
	pFldI->isOK = ST_B_FALSE;

	/* adjust frID */
	frID = st_apev2_fnc_adjustFrID(frID, pFrIDstr, &pFldPrIsrc);
	if (frID == ST_APEV2_FID_NONE)
		return ST_ERR_IARG;

	/* now we copy the original field props */
	res = st_apev2_stc_copyFPr_i(pFrIDstr, pFldPrIsrc, &pFldI->fldPrI);
	if (res == ST_ERR_SUCC && st_sysStrEmpty2(pFldI->fldPrI.pVIDstr))
		res = ST_ERR_FAIL;
	/* */
	if (res != ST_ERR_SUCC)
		st_apev2_stc_rsetFPr_i(&pFldI->fldPrI);
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_gs_setFieldData_stringArr(Tst_apev2_fldData *pFld,
		Tst_mtes_strArr *pStrArr)
{
	Tst_err res;
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStrArr == NULL)

	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

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
st_apev2_gs_setFieldData_geoBin(Tst_apev2_fldData *pFld, Tst_binobj *pBinDat)
{
	Tst_err res;
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pBinDat == NULL)

	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_binobj_copy(pBinDat, &pFldI->dataBinDat);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	pFldI->isOK = (st_binobj_getDataSize(&pFldI->dataBinDat) > 0);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const Tst_tagBasics*
st_apev2_gs_getTag_tbas(const Tst_apev2_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return &((Tst_apev2_tag_intn*)pTag->pObInternal)->tbas;
}

const Tst_str*
st_apev2_gs_getTag_filen(const Tst_apev2_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_apev2_tag_intn*)pTag->pObInternal)->pFilen;
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_apev2_gs_getTag_amountValidFlds(const Tst_apev2_tag *pTag)
{
	Tst_uint32 n = 0;
	Tst_apev2_fldData *pItFld = NULL;

	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL)
		++n;
	return n;
}

Tst_uint32
st_apev2_gs_getTag_amountSkippedFields(const Tst_apev2_tag *pTag)
{
	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_apev2_tag_intn*)pTag->pObInternal)->flds.skipCnt;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_gs_getTag_taggerInfo(const Tst_apev2_tag *pTag, Tst_mtes_string *pTI)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTI == NULL)

	return st_mtes_strCpy(&((Tst_apev2_tag_intn*)pTag->pObInternal)->tagger, pTI);
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_apev2_gs_getField_nr(const Tst_apev2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_apev2_fldData_intn*)pFld->pObInternal)->fnr;
}

Tst_err
st_apev2_gs_getField_props(const Tst_apev2_fldData *pFld,
		Tst_apev2_fldProp *pFldPr)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFldPr == NULL)

	pFldPr->pObInternal = &((Tst_apev2_fldData_intn*)pFld->pObInternal)->fldPrI;
	return ST_ERR_SUCC;
}

Tst_err
st_apev2_gs_getFieldData_stringArr(const Tst_apev2_fldData *pFld,
		Tst_mtes_strArr *pStrArr)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStrArr == NULL)

	return st_mtes_strArrCpy(
			&((Tst_apev2_fldData_intn*)pFld->pObInternal)->dataSArr,
			pStrArr);
}

Tst_uint32
st_apev2_gs_getFieldData_stringArr_elemCount(const Tst_apev2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return st_mtes_strArrGetElemCount(
			&((Tst_apev2_fldData_intn*)pFld->pObInternal)->dataSArr);
}

Tst_err
st_apev2_gs_getFieldData_stringArr_elem(const Tst_apev2_fldData *pFld,
		const Tst_uint32 ix, Tst_mtes_string *pStr)
{
	const Tst_mtes_string *pSrc;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			ix == 0 || pStr == NULL)

	pSrc = st_mtes_strArrGetElem(
			&((Tst_apev2_fldData_intn*)pFld->pObInternal)->dataSArr,
			ix);
	if (pSrc == NULL) {
		st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
		return ST_ERR_SUCC;
	}
	return st_mtes_strCpy(pSrc, pStr);
}

Tst_uint32
st_apev2_gs_getFieldData_geoBinSz(const Tst_apev2_fldData *pFld)
{
	ST_ASSERTN_NUM(0, pFld == NULL || pFld->pObInternal == NULL)

	return st_binobj_getDataSize(
			&((Tst_apev2_fldData_intn*)pFld->pObInternal)->dataBinDat);
}

Tst_err
st_apev2_gs_getFieldData_geoBin(const Tst_apev2_fldData *pFld,
		Tst_binobj *pBinDat)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pBinDat == NULL)

	return st_binobj_copy(
			&((Tst_apev2_fldData_intn*)pFld->pObInternal)->dataBinDat,
			pBinDat);
}

Tst_err
st_apev2_gs_getFieldData_geoBinToFile(const Tst_apev2_fldData *pFld,
		const Tst_str *pOutpFilen)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pOutpFilen == NULL)

	return st_binobj_copyIntoFile(
			&((Tst_apev2_fldData_intn*)pFld->pObInternal)->dataBinDat,
			pOutpFilen);
}

/*----------------------------------------------------------------------------*/

Tst_apev2_frTp
st_apev2_gs_getFieldProp_typ(const Tst_apev2_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_APEV2_FTP_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_apev2_fldProp_intn*)pFldPr->pObInternal)->typ;
}

Tst_apev2_frID
st_apev2_gs_getFieldProp_id(const Tst_apev2_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_APEV2_FID_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_apev2_fldProp_intn*)pFldPr->pObInternal)->id;
}

const Tst_str*
st_apev2_gs_getFieldProp_idStr(const Tst_apev2_fldProp *pFldPr)
{
	Tst_str const *pIDstr;
	Tst_apev2_fldProp_intn *pFldPrI;

	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	pFldPrI = (Tst_apev2_fldProp_intn*)pFldPr->pObInternal;
	pIDstr  = (const Tst_str*)pFldPrI->pVIDstr;
	if (pIDstr == NULL)
		pIDstr = (const Tst_str*)pFldPrI->pCIDstr;
	return pIDstr;
}

const Tst_str*
st_apev2_gs_getFieldProp_caption(const Tst_apev2_fldProp *pFldPr)
{
	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	return (const Tst_str*)((Tst_apev2_fldProp_intn const*)pFldPr->pObInternal)->pCCap;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_apev2_fldData*
st_apev2_ite_nextFld(const Tst_apev2_tag *pTag,
		ST_OPTARG(const Tst_apev2_fldData *pFldLast))
{
	return ST_TAV2__ite_nextFld(pTag, ST_APEV2_FID_NONE, NULL,
			pFldLast, /*onlyValid:*/ST_B_FALSE);
}

Tst_apev2_fldData*
st_apev2_ite_nextValidFld(const Tst_apev2_tag *pTag,
		ST_OPTARG(const Tst_apev2_fldData *pFldLast))
{
	return ST_TAV2__ite_nextFld(pTag, ST_APEV2_FID_NONE, NULL,
			pFldLast, /*onlyValid:*/ST_B_TRUE);
}

Tst_apev2_fldData*
st_apev2_ite_nextValidFld_byID(const Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_apev2_fldData *pFldLast))
{
	return ST_TAV2__ite_nextFld(pTag, frID, pFrIDstr,
			pFldLast, /*onlyValid:*/ST_B_TRUE);
}

/*----------------------------------------------------------------------------*/

Tst_apev2_fldProp*
st_apev2_ite_nextTagFldProp(ST_OPTARG(Tst_apev2_fldProp *pFldPrLast),
		Tst_apev2_fldProp *pFldPrNext)
{
	return ST_TAV2__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_TRUE, pFldPrNext);
}

Tst_apev2_fldProp*
st_apev2_ite_nextTagFldProp_all(ST_OPTARG(Tst_apev2_fldProp *pFldPrLast),
		Tst_apev2_fldProp *pFldPrNext)
{
	return ST_TAV2__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_FALSE, pFldPrNext);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_apev2_fldData*
ST_TAV2__ite_nextFld(const Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(const Tst_apev2_fldData *pFldLast), const Tst_bool onlyValid)
{
	Tst_uint32 luix,
	           z,
	           fcnt;
	Tst_apev2_tag_intn     *pTagI;
	Tst_apev2_fldData      *pFld;
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	luix = (pFldLast == NULL ? 0 :
			((Tst_apev2_fldData_intn*)pFldLast->pObInternal)->uniqueIx);
	fcnt = st_dynlist_getElementCount(&pTagI->flds.list);
	for (z = 1; z <= fcnt; z++) {
		pFld = (Tst_apev2_fldData*)st_dynlist_getElement(&pTagI->flds.list, z);
		if (pFld == NULL)
			return NULL;
		pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;
		if ((onlyValid && ! pFldI->isOK) ||
				pFldI->fldPrI.typ == ST_APEV2_FTP_NONE ||
				pFldI->uniqueIx <= luix ||
				((frID != ST_APEV2_FID_NONE || pFrIDstr != NULL) &&
					! st_apev2_fnc_matchFrIDs(pFldI->fldPrI.id, pFldI->fldPrI.pVIDstr,
								frID, (const char*)pFrIDstr)))
			continue;
		return pFld;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/

static Tst_apev2_fldProp*
ST_TAV2__ite_nextTagFldProp(ST_OPTARG(Tst_apev2_fldProp *pFldPrLast),
		const Tst_bool withRestrictions, Tst_apev2_fldProp *pFldPrNext)
{
	Tst_uint32 z         = 0;
	Tst_bool   lastFnd   = ST_B_TRUE;
	char const *pLastCID = NULL;
	Tst_apev2_frID         lastID = ST_APEV2_FID_NONE;
	Tst_apev2_fldProp_intn *pFldPrI;

	ST_ASSERTN_NULL(pFldPrNext == NULL)

	if (pFldPrLast != NULL && pFldPrLast->pObInternal != NULL) {
		pFldPrI  = (Tst_apev2_fldProp_intn*)pFldPrLast->pObInternal;
		lastID   = pFldPrI->id;
		pLastCID = pFldPrI->pCIDstr;
		lastFnd  = ST_B_FALSE;
	}
	pFldPrNext->pObInternal = NULL;

	while (ST_B_TRUE) {
		pFldPrI = (Tst_apev2_fldProp_intn*)&ST_APEV2_FLDPROPS[z++];

		if (pFldPrI->id == ST_APEV2_FID_NONE || pFldPrI->pCIDstr == NULL ||
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
