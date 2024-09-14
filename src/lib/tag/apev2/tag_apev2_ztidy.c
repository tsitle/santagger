/******************************************************************************/
/* tag_apev2_ztidy.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for tidying up APEv2 Tags                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_ZTIDY_C
#include "tag_apev2_ztidy-prot.h"
/*** */
#include "tag_apev2_fnc-prot.h"
#include "tag_apev2_zfdeb-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_APEV2_ZTIDY_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_tidy_mapMergePosFlds(const char *pFnc, Tst_apev2_tag *pTag)
{
	Tst_err  res       = ST_ERR_SUCC;
	Tst_bool moddedTrk = ST_B_FALSE,
	         moddedDsc = ST_B_FALSE;
	Tst_uint32 trknCnt = 0,
	           trktCnt = 0,
	           dscnCnt = 0,
	           dsctCnt = 0,
	           tmpNum  = 0,
	           tmpTot  = 0;
	Tst_int32  trknVal = -1,
	           trktVal = -1,
	           dscnVal = -1,
	           dsctVal = -1;
	Tst_mtes_string        *pStr;
	//Tst_apev2_tag_intn     *pTagI;
	Tst_apev2_fldData      *pItFld    = NULL;
	Tst_apev2_fldData_intn *pItFldI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	//pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		/* increase counter */
		switch (pItFldI->fldPrI.id) {
		case ST_APEV2_FID_TRKN:
			++trknCnt;
			if (trknCnt > 1)
				moddedTrk = ST_B_TRUE;
			break;
		case ST_APEV2_FID_TRKT:
			++trktCnt;
			if (trktCnt > 1)
				moddedTrk = ST_B_TRUE;
			break;
		case ST_APEV2_FID_DSCN:
			++dscnCnt;
			if (dscnCnt > 1)
				moddedDsc = ST_B_TRUE;
			break;
		case ST_APEV2_FID_DSCT:
			++dsctCnt;
			if (dsctCnt > 1)
				moddedDsc = ST_B_TRUE;
			break;
		default:
			continue;
		}
		/* check for "x/y"-style strings */
		if (st_mtes_strArrGetElemCount(&pItFldI->dataSArr) == 0) {
			if (pItFldI->fldPrI.id == ST_APEV2_FID_TRKN ||
					pItFldI->fldPrI.id == ST_APEV2_FID_TRKT)
				moddedTrk = ST_B_TRUE;
			else
				moddedDsc = ST_B_TRUE;
			continue;
		}
		pStr = st_mtes_strArrGetElem(&pItFldI->dataSArr, 1);
		res  = st_tagCFnc_chk_getPosStringNums(pStr, &tmpNum, &tmpTot);
		if (res != ST_ERR_SUCC)
			break;
		if (tmpTot > 0) {
			/* we have a "x/y"-style string */
			if (pItFldI->fldPrI.id == ST_APEV2_FID_TRKN ||
					pItFldI->fldPrI.id == ST_APEV2_FID_TRKT) {
				moddedTrk = ST_B_TRUE;
				if (tmpNum > 0)
					trknVal = (Tst_int32)tmpNum;
				trktVal = (Tst_int32)tmpTot;
			} else {
				moddedDsc = ST_B_TRUE;
				if (tmpNum > 0)
					dscnVal = (Tst_int32)tmpNum;
				dsctVal = (Tst_int32)tmpTot;
			}
		} else if (tmpNum > 0) {
			/* we have an int-string */
			switch (pItFldI->fldPrI.id) {
			case ST_APEV2_FID_TRKN: trknVal = (Tst_int32)tmpNum; break;
			case ST_APEV2_FID_TRKT: trktVal = (Tst_int32)tmpNum; break;
			case ST_APEV2_FID_DSCN: dscnVal = (Tst_int32)tmpNum; break;
			case ST_APEV2_FID_DSCT: dsctVal = (Tst_int32)tmpNum; break;
			default:
				break;
			}
		}
	}
	/* disable old fields and add new ones, if necessary */
	if ((trknCnt > 0 && trknVal == -1) || (trktCnt > 0 && trktVal == -1))
		moddedTrk = ST_B_TRUE;
	if ((dscnCnt > 0 && dscnVal == -1) || (dsctCnt > 0 && dsctVal == -1))
		moddedDsc = ST_B_TRUE;
	if (res == ST_ERR_SUCC && moddedTrk) {
		if (trknCnt > 0)
			st_apev2_disableFld_byID_all(pTag, ST_APEV2_FID_TRKN, NULL);
		if (trktCnt > 0)
			st_apev2_disableFld_byID_all(pTag, ST_APEV2_FID_TRKT, NULL);
		if (trknVal > 0)
			res = st_apev2_fldSet_dataStr_int(pTag,
					ST_APEV2_FID_TRKN, NULL, 0, trknVal);
		if (res == ST_ERR_SUCC && trktVal > 0)
			res = st_apev2_fldSet_dataStr_int(pTag,
					ST_APEV2_FID_TRKT, NULL, 0, trktVal);
	}
	if (res == ST_ERR_SUCC && moddedDsc) {
		if (dscnCnt > 0)
			st_apev2_disableFld_byID_all(pTag, ST_APEV2_FID_DSCN, NULL);
		if (dsctCnt > 0)
			st_apev2_disableFld_byID_all(pTag, ST_APEV2_FID_DSCT, NULL);
		if (dscnVal > 0)
			res = st_apev2_fldSet_dataStr_int(pTag,
					ST_APEV2_FID_DSCN, NULL, 0, dscnVal);
		if (res == ST_ERR_SUCC && dsctVal > 0)
			res = st_apev2_fldSet_dataStr_int(pTag,
					ST_APEV2_FID_DSCT, NULL, 0, dsctVal);
	}

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_tidy_remDups(const char *pFnc, Tst_apev2_tag *pTag,
		Tst_apev2_fldData *pFld, Tst_bool *pDiscardedOne)
{
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool remove;
	Tst_apev2_fldData      *pItFld = pFld;
	Tst_apev2_fldData_intn *pFldI,
	                       *pItFldI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFld == NULL || pFld->pObInternal == NULL ||
			pDiscardedOne == NULL)

	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

	*pDiscardedOne = ST_B_FALSE;

	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (! st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
					pFldI->fldPrI.id, pFldI->fldPrI.pVIDstr))
			continue;
		/* */
		remove = ST_B_FALSE;
		switch (pItFldI->fldPrI.typ) {
		case ST_APEV2_FTP_YEAR:
		case ST_APEV2_FTP_TSTA:
			remove = ST_B_TRUE;
			break;
		case ST_APEV2_FTP_STR:
			switch (pItFldI->fldPrI.id) {
			case ST_APEV2_FID_ISBN:
			case ST_APEV2_FID_CATA:
			case ST_APEV2_FID_LABC:
			case ST_APEV2_FID_INDX:
			case ST_APEV2_FID_ISRC:
			case ST_APEV2_FID_INTR:
			case ST_APEV2_FID_RGTG:
			case ST_APEV2_FID_RGTP:
			case ST_APEV2_FID_RGAG:
			case ST_APEV2_FID_RGAP:
				remove = ST_B_TRUE;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		/* */
		if (! remove)
			continue;
		res = st_apev2_disableFld_byID(pTag,
				pItFldI->fldPrI.id, (const Tst_str*)pItFldI->fldPrI.pVIDstr,
				pItFldI->fnr);
		if (res != ST_ERR_SUCC)
			break;
		*pDiscardedOne = ST_B_TRUE;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_tidy_mergFields(const char *pFnc, Tst_apev2_tag *pTag,
		Tst_apev2_fldData *pFld)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 y;
	Tst_apev2_tag_intn     *pTagI;
	Tst_apev2_fldData      *pItFld = pFld;
	Tst_apev2_fldData_intn *pFldI,
	                       *pItFldI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFld == NULL || pFld->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.typ != ST_APEV2_FTP_STR ||
				! st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
						pFldI->fldPrI.id, pFldI->fldPrI.pVIDstr))
			continue;
		/* add string-array from pItFldI to pFldI */
		for (y = 1; y <= st_mtes_strArrGetElemCount(&pItFldI->dataSArr); y++) {
			res = st_mtes_strArrAdd(&pFldI->dataSArr,
					st_mtes_strArrGetElem(&pItFldI->dataSArr, y));
			if (res != ST_ERR_SUCC)
				break;
		}
		/* strip array */
		res = st_mtes_strArrStrip(&pFldI->dataSArr,
				(pItFldI->fldPrI.id == ST_APEV2_FID_IPLS), NULL);
		if (res != ST_ERR_SUCC)
			break;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_fdeb(&pTagI->opts, 0, pFnc, pFldI,
					"appended field #%02u", pItFldI->fnr);
		/* disable pItFld */
		res = st_apev2_disableFld_byID(pTag,
				pItFldI->fldPrI.id, (const Tst_str*)pItFldI->fldPrI.pVIDstr,
				pItFldI->fnr);
		if (res != ST_ERR_SUCC)
			break;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_tidy_mergMultStr(const char *pFnc, const Tst_apev2_opts *pOpts,
		Tst_apev2_fldData *pFld)
{
	Tst_err res;
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pOpts == NULL ||
			pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

	if (pFldI->fldPrI.id == ST_APEV2_FID_IPLS)
		res = st_tagCFnc_iplsMerge(&pFldI->dataSArr);
	else
		res = st_mtes_strArrMergeToOneString(&pFldI->dataSArr, (const Tst_str*)", ");

	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_apev2_d_fdebStr(pOpts, 0, pFnc, pFldI,
				"mer", st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_tidy_chkAllFields(const char *pFnc, Tst_apev2_tag *pTag)
{
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool discard,
	         ssMod,
	         ssHIC;
	Tst_mtes_strRestr      restr;
	Tst_mtes_string        *pStr;
	Tst_apev2_tag_intn     *pTagI;
	Tst_apev2_fldData      *pItFld = NULL;
	Tst_apev2_fldData_intn *pItFldI;
	Tst_apev2_fldd_strtp   strTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	/* init some stuff */
	/** */
	restr.hasR    = ST_B_TRUE;  /* we have restrictions */
	restr.ruleAlw = ST_B_TRUE;  /* mode: allow */
	st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
	/** */
	st_apev2_stc_rsetStrTp(&strTp);

	while (res == ST_ERR_SUCC &&
			(pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		discard = ST_B_FALSE;
		st_apev2_fnc_getFieldStringTypes(pItFldI->fldPrI.id, &strTp);
		switch (pItFldI->fldPrI.typ) {
		case ST_APEV2_FTP_BDAT:
			if (st_binobj_getDataSize(&pItFldI->dataBinDat) == 0)
				discard = ST_B_TRUE;
			break;
		default:
			ssMod = ST_B_FALSE;
			ssHIC = ST_B_FALSE;
			if (st_mtes_strArrGetElemCount(&pItFldI->dataSArr) == 0)
				discard = ST_B_TRUE;
			else {
				/* disallow '\n' and '\t' ? */
				restr.disNewln = ! strTp.hasFullStr;
				restr.disTab   = restr.disNewln;
				/* */
				pStr = st_mtes_strArrGetElem(&pItFldI->dataSArr, 1);
				res  = st_mtes_stripStr(pStr, &restr, &ssHIC, &ssMod);
			}
			if (res == ST_ERR_SUCC && ! discard && st_mtes_strlenCh(pStr) == 0)
				discard = ST_B_TRUE;
			if (res == ST_ERR_SUCC && ! discard && (ssMod || ssHIC) &&
					ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_apev2_d_fdebStr(&pTagI->opts, 0, pFnc, pItFldI,
						"ctx", pStr, "");
		}
		if (res != ST_ERR_SUCC || ! discard)
			continue;
		res = st_apev2_disableFld_byID(pTag,
				pItFldI->fldPrI.id, (const Tst_str*)pItFldI->fldPrI.pVIDstr,
				pItFldI->fnr);
	}
	return res;
}

/******************************************************************************/
