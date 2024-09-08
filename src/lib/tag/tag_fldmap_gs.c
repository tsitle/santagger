/******************************************************************************/
/* tag_fldmap_gs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Getter/Setter functions for Generic Tag Fields                             */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj.h>
#include <santag/common/sys_fnc.h>
#include <santag/common/string_mte.h>
#include <santag/tag/tag_id3v1-defs.h>
#include <santag/tag/tag_apev2-defs.h>
#include <santag/tag/tag_vorbc-defs.h>
#include <santag/tag/tag_fldmap.h>
/** */
#define SRC_TAG_FLDMAP_GS_C
#include "tag_fldmap-prot.h"
#undef SRC_TAG_FLDMAP_GS_C

/*
// System-Includes
*/
#include <string.h>      /* memset() */
#include <stdlib.h>      /* free() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
st_tagFldMap_gs_setMode(Tst_tfldMap_genTagFld *pFld, const Tst_bool addOrSet)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->modeAddOrSet = addOrSet;
	return ST_ERR_SUCC;
}

Tst_err
st_tagFldMap_gs_setTagType(Tst_tfldMap_genTagFld *pFld,
		const Tst_str *pTagNameShort)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pTagNameShort == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	if (st_sysStrcmp2(ST_B_FALSE, pTagNameShort, ST_ID3V1_TAG_NAME_SH))
		pGTFI->ttp = ST_TFLDMAP_TTP_IV1;
	else if (st_sysStrcmp2(ST_B_FALSE, pTagNameShort, ST_ID3V2_TAG_NAME_SH))
		pGTFI->ttp = ST_TFLDMAP_TTP_IV2;
	else if (st_sysStrcmp2(ST_B_FALSE, pTagNameShort, ST_APEV2_TAG_NAME_SH))
		pGTFI->ttp = ST_TFLDMAP_TTP_AV2;
	else if (st_sysStrcmp2(ST_B_FALSE, pTagNameShort, ST_VORBC_TAG_NAME_SH))
		pGTFI->ttp = ST_TFLDMAP_TTP_VOR;
	else {
		pGTFI->ttp = ST_TFLDMAP_TTP_NONE;
		res        = ST_ERR_IARG;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_tagFldMap_gs_setFldID(Tst_tfldMap_genTagFld *pFld,
		const Tst_int32 fldID)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	pGTFI->fldID = fldID;
	ST_DELPOINT(pGTFI->pFldIDstr)
	return ST_ERR_SUCC;
}

Tst_err
st_tagFldMap_gs_setFldIDstr(Tst_tfldMap_genTagFld *pFld,
		const Tst_str *pFldIDstr)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFldIDstr == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	pGTFI->fldID = -1;

	res = st_sysStrcpy(pFldIDstr, &pGTFI->pFldIDstr);
	if (res == ST_ERR_SUCC)
		st_sysStrToUpper(pGTFI->pFldIDstr);
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_tagFldMap_gs_setFldNr(Tst_tfldMap_genTagFld *pFld,
		const Tst_int32 fldNr)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->fldNr = fldNr;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_tagFldMap_gs_setDataStr(Tst_tfldMap_genTagFld *pFld,
		const Tst_mtes_string *pValue)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pValue == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_mtes_stc_rsetSA(&pGTFI->dataStrArr);
	st_binobj_stc_rsetBO(&pGTFI->dataBin);
	pGTFI->isDataBinSet  = ST_B_FALSE;
	pGTFI->isDataUValSet = ST_B_FALSE;
	pGTFI->isDataBValSet = ST_B_FALSE;

	res = st_mtes_strArrAdd(&pGTFI->dataStrArr, pValue);
	/* */
	pGTFI->isDataStrSet = (res == ST_ERR_SUCC);
	return res;
}

Tst_err
st_tagFldMap_gs_setDataStr_int(Tst_tfldMap_genTagFld *pFld,
		const Tst_int32 value)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_mtes_stc_rsetSA(&pGTFI->dataStrArr);
	st_binobj_stc_rsetBO(&pGTFI->dataBin);
	pGTFI->isDataBinSet  = ST_B_FALSE;
	pGTFI->isDataStrSet  = ST_B_FALSE;
	pGTFI->isDataUValSet = ST_B_FALSE;
	pGTFI->isDataBValSet = ST_B_FALSE;

	if (value >= 0) {
		Tst_mtes_string tmpMS;

		st_mtes_stc_initStr(&tmpMS);
		res = st_mtes_int32ToStr(value, &tmpMS);
		if (res == ST_ERR_SUCC)
			res = st_mtes_strArrAdd(&pGTFI->dataStrArr, &tmpMS);
		st_mtes_stc_freeStr(&tmpMS);
		/* */
		pGTFI->isDataStrSet = (res == ST_ERR_SUCC);
	}
	return res;
}

Tst_err
st_tagFldMap_gs_setDataStr_pos(Tst_tfldMap_genTagFld *pFld,
		const Tst_int32 valueNr, const Tst_int32 valueTot)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_mtes_stc_rsetSA(&pGTFI->dataStrArr);
	st_binobj_stc_rsetBO(&pGTFI->dataBin);
	pGTFI->isDataBinSet  = ST_B_FALSE;
	pGTFI->isDataStrSet  = ST_B_FALSE;
	pGTFI->isDataUValSet = ST_B_FALSE;
	pGTFI->isDataBValSet = ST_B_FALSE;

	if (valueNr >= 0 || valueTot > 0) {
		char tmpNS[128];
		Tst_mtes_string tmpMS;

		st_mtes_stc_initStr(&tmpMS);
		if (valueNr >= 0) {
			if (valueTot > 0)
				snprintf(tmpNS, sizeof(tmpNS), "%d/%d", valueNr, valueTot);
			else
				snprintf(tmpNS, sizeof(tmpNS), "%d", valueNr);
		} else
			snprintf(tmpNS, sizeof(tmpNS), "/%d", valueTot);
		res = st_mtes_copyFromCharp_iso((const Tst_str*)tmpNS, &tmpMS);
		if (res == ST_ERR_SUCC)
			res = st_mtes_strArrAdd(&pGTFI->dataStrArr, &tmpMS);
		st_mtes_stc_freeStr(&tmpMS);
		/* */
		pGTFI->isDataStrSet = (res == ST_ERR_SUCC);
	}
	return res;
}

Tst_err
st_tagFldMap_gs_setDataStr_sarr(Tst_tfldMap_genTagFld *pFld,
		Tst_mtes_strArr *pSArr)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pSArr == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_binobj_stc_rsetBO(&pGTFI->dataBin);
	pGTFI->isDataBinSet  = ST_B_FALSE;
	pGTFI->isDataUValSet = ST_B_FALSE;
	pGTFI->isDataBValSet = ST_B_FALSE;

	res = st_mtes_strArrCpy(pSArr, &pGTFI->dataStrArr);
	/* */
	pGTFI->isDataStrSet = (res == ST_ERR_SUCC);
	return res;
}

Tst_err
st_tagFldMap_gs_setDataBinary(Tst_tfldMap_genTagFld *pFld,
		Tst_binobj *pBinDat)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pBinDat == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_mtes_stc_rsetSA(&pGTFI->dataStrArr);
	pGTFI->isDataStrSet  = ST_B_FALSE;
	pGTFI->isDataUValSet = ST_B_FALSE;
	pGTFI->isDataBValSet = ST_B_FALSE;

	res = st_binobj_copy(pBinDat, &pGTFI->dataBin);
	/* */
	pGTFI->isDataBinSet = (res == ST_ERR_SUCC);
	return res;
}

Tst_err
st_tagFldMap_gs_setDataUVal(Tst_tfldMap_genTagFld *pFld,
		const Tst_uint32 uval)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_mtes_stc_rsetSA(&pGTFI->dataStrArr);
	st_binobj_stc_rsetBO(&pGTFI->dataBin);
	pGTFI->isDataStrSet  = ST_B_FALSE;
	pGTFI->isDataBinSet  = ST_B_FALSE;
	pGTFI->isDataBValSet = ST_B_FALSE;

	pGTFI->isDataUValSet = ST_B_TRUE;
	pGTFI->dataUVal      = uval;
	return ST_ERR_SUCC;
}

Tst_err
st_tagFldMap_gs_setDataBVal(Tst_tfldMap_genTagFld *pFld,
		const Tst_byte bval)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_mtes_stc_rsetSA(&pGTFI->dataStrArr);
	st_binobj_stc_rsetBO(&pGTFI->dataBin);
	pGTFI->isDataStrSet  = ST_B_FALSE;
	pGTFI->isDataBinSet  = ST_B_FALSE;
	pGTFI->isDataUValSet = ST_B_FALSE;

	pGTFI->isDataBValSet = ST_B_TRUE;
	pGTFI->dataBVal      = bval;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_tagFldMap_gs_setAttrDesc(Tst_tfldMap_genTagFld *pFld,
		const Tst_mtes_string *pDesc)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pDesc == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	res = st_mtes_strCpy(pDesc, &pGTFI->attrDesc);
	/* */
	pGTFI->isAttrDescSet = (res == ST_ERR_SUCC);
	return res;
}

Tst_err
st_tagFldMap_gs_setAttrLang(Tst_tfldMap_genTagFld *pFld,
		const Tst_str *pLangISO)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pLangISO == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	if (st_sysStrlen2(pLangISO) >= 3)
		memcpy(pGTFI->attrLangISO, pLangISO, 3);
	else
		memset(pGTFI->attrLangISO, 0, 3);
	pGTFI->isAttrLangSet = ST_B_TRUE;

	return ST_ERR_SUCC;
}

Tst_err
st_tagFldMap_gs_setAttrGEOBfilen(Tst_tfldMap_genTagFld *pFld,
		const Tst_mtes_string *pFilen)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pFilen == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	res = st_mtes_strCpy(pFilen, &pGTFI->attrGeoFilen);
	/* */
	pGTFI->isAttrGeoFnSet = (res == ST_ERR_SUCC);
	return res;
}

Tst_err
st_tagFldMap_gs_setAttrGEOBmime(Tst_tfldMap_genTagFld *pFld,
		const Tst_str *pMimeISO)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pMimeISO == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	res = st_sysStrcpy(pMimeISO, &pGTFI->pAttrGeoMimeISO);
	/* */
	pGTFI->isAttrGeoMimeSet = (res == ST_ERR_SUCC);
	return res;
}

Tst_err
st_tagFldMap_gs_setAttrPICTtype(Tst_tfldMap_genTagFld *pFld,
		const Tst_id3v2_picTp picTp)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->attrPicTp      = picTp;
	((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isAttrPicTpSet = ST_B_TRUE;
	return ST_ERR_SUCC;
}

Tst_err
st_tagFldMap_gs_setAttrPICTformat(Tst_tfldMap_genTagFld *pFld,
		const Tst_utilsFmt_picFmt picFmt)
{
	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->attrPicFmt      = picFmt;
	((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isAttrPicFmtSet = ST_B_TRUE;
	return ST_ERR_SUCC;
}

Tst_err
st_tagFldMap_gs_setAttrSYLTtsf(Tst_tfldMap_genTagFld *pFld,
		const Tst_id3v2_sltTSF tsf)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	pGTFI->isAttrSltTsfSet = ST_B_TRUE;
	pGTFI->attrSltTSF      = tsf;
	return ST_ERR_SUCC;
}

Tst_err
st_tagFldMap_gs_setAttrSYLTctp(Tst_tfldMap_genTagFld *pFld,
		const Tst_id3v2_sltCTp ctp)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	pGTFI->isAttrSltCtpSet = ST_B_TRUE;
	pGTFI->attrSltCTP      = ctp;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool
st_tagFldMap_gs_getHasData_text(const Tst_tfldMap_genTagFld *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isDataStrSet;
}

Tst_bool
st_tagFldMap_gs_getHasData_bin(const Tst_tfldMap_genTagFld *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isDataBinSet;
}

Tst_bool
st_tagFldMap_gs_getHasData_uval(const Tst_tfldMap_genTagFld *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isDataUValSet;
}

Tst_bool
st_tagFldMap_gs_getHasData_bval(const Tst_tfldMap_genTagFld *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isDataBValSet;
}

Tst_bool
st_tagFldMap_gs_getHasAttr_desc(const Tst_tfldMap_genTagFld *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isAttrDescSet;
}

Tst_bool
st_tagFldMap_gs_getHasAttr_lang(const Tst_tfldMap_genTagFld *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isAttrLangSet;
}

Tst_bool
st_tagFldMap_gs_getHasAttr_pict(const Tst_tfldMap_genTagFld *pFld)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	return (pGTFI->isAttrPicTpSet || pGTFI->isAttrPicFmtSet);
}

Tst_bool
st_tagFldMap_gs_getHasAttr_geob(const Tst_tfldMap_genTagFld *pFld)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	return ((Tst_tfldMap_genTagFld_intn*)pFld->pObInternal)->isAttrGeoMimeSet;
}

Tst_bool
st_tagFldMap_gs_getHasAttr_sylt(const Tst_tfldMap_genTagFld *pFld)
{
	Tst_tfldMap_genTagFld_intn *pGTFI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFld == NULL || pFld->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	return (pGTFI->isAttrSltTsfSet || pGTFI->isAttrSltCtpSet);
}

/******************************************************************************/
