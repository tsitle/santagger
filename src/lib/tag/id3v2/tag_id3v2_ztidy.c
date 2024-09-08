/******************************************************************************/
/* tag_id3v2_ztidy.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for tidying up ID3v2 Tags                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.03.2010 (dd.mm.yyyy)                                     */
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
#include <santag/tag/tag_comfnc.h>
#include <santag/tag/tag_id3v2.h>
/** */
#define SRC_TAG_ID3V2_ZTIDY_C
#include "tag_id3v2_ztidy-prot.h"
/*** */
#include "tag_id3v2_zchk-pp.h"
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2_stcs-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_ZTIDY_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__tidy_addFld_txtPChar(Tst_id3v2_tag *pTag,
                              const Tst_id3v2_frID frID,
                              const Tst_str *pTextISO);
static Tst_err
ST_TIV2__tidy_mergePosFields(Tst_id3v2_fldData_intn *pFldIsrc,
                             Tst_id3v2_fldData_intn *pFldIdst);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Check whether field must be mapped or discarded
 */
void
st_id3v2_tidy_getFldMap(const Tst_id3v2_frID srcID1, Tst_id3v2_frID *pSrcID2,
		Tst_id3v2_frID *pSrcID3, const Tst_byte tagVersionMajor,
		Tst_bool *pToMap, Tst_bool *pDiscard,
		Tst_id3v2_frID *pDestID1, Tst_id3v2_frID *pDestID2,
		Tst_id3v2_frID *pDestID3)
{
	ST_ASSERTN_VOID(pSrcID2 == NULL || pSrcID3 == NULL ||
			pToMap == NULL || pDiscard == NULL ||
			pDestID1 == NULL || pDestID2 == NULL || pDestID3 == NULL)

	*pToMap   = ST_B_FALSE;
	*pDiscard = ST_B_FALSE;
	*pDestID1 = ST_ID3V2_FID_NONE;
	*pDestID2 = ST_ID3V2_FID_NONE;
	*pDestID3 = ST_ID3V2_FID_NONE;
	*pSrcID2  = ST_ID3V2_FID_NONE;
	*pSrcID3  = ST_ID3V2_FID_NONE;

	switch (srcID1) {
	/** discard */
	case ST_ID3V2_FID_2___CDMF:
	case ST_ID3V2_FID_2___CRM1:
		if (tagVersionMajor > 2)
			*pDiscard = ST_B_TRUE;
		break;
	case ST_ID3V2_FID__34_COMR:
	case ST_ID3V2_FID__34_ENCR:
	case ST_ID3V2_FID__34_GRID:
	case ST_ID3V2_FID__34_OWNE:
	case ST_ID3V2_FID__34_POSS:
	case ST_ID3V2_FID__34_PRIV:
	case ST_ID3V2_FID__34_USER:
		if (tagVersionMajor < 3)
			*pDiscard = ST_B_TRUE;
		break;
	case ST_ID3V2_FID_23__EQUA:
	case ST_ID3V2_FID_23__RVAD:
		if (tagVersionMajor > 3)
			*pDiscard = ST_B_TRUE;
		break;
	case ST_ID3V2_FID___4_ASPI:
	case ST_ID3V2_FID___4_EQU2:
	case ST_ID3V2_FID___4_RVA2:
	case ST_ID3V2_FID___4_SEEK:
	case ST_ID3V2_FID___4_SIGN:
		if (tagVersionMajor < 4)
			*pDiscard = ST_B_TRUE;
		break;
	case ST_ID3V2_FID_234_LINK:
		*pDiscard = ST_B_TRUE;
		break;
	/** map */
	case ST_ID3V2_FID_2___COM1:
		*pDestID1 = ST_ID3V2_FID_234_COMM;
		break;
	case ST_ID3V2_FID__34_WORS:
	case ST_ID3V2_FID__34_WPAY:
		if (tagVersionMajor < 3)
			*pDestID1 = ST_ID3V2_FID_234_WXXX;
		break;
	case ST_ID3V2_FID_23__IPLS:
		if (tagVersionMajor > 3)
			*pDestID1 = ST_ID3V2_FID___4_TIPL;
		break;
	case ST_ID3V2_FID___4_TDRC:
	case ST_ID3V2_FID___4_TDRL:
		if (tagVersionMajor < 4) {
			*pDestID1 = ST_ID3V2_FID_23__TYER;
			*pDestID2 = ST_ID3V2_FID_23__TDAT;
			*pDestID3 = ST_ID3V2_FID_23__TIME;
		}
		break;
	case ST_ID3V2_FID___4_TDEN:
	case ST_ID3V2_FID___4_TDTG:
	case ST_ID3V2_FID___4_TMOO:
	case ST_ID3V2_FID___4_TSST:
	case ST_ID3V2_FID___4_TPRO:
		if (tagVersionMajor < 4)
			*pDestID1 = ST_ID3V2_FID_234_COMM;
		break;
	case ST_ID3V2_FID___4_TDOR:
		if (tagVersionMajor < 4)
			*pDestID1 = ST_ID3V2_FID_23__TORY;
		break;
	case ST_ID3V2_FID_23__TORY:
		if (tagVersionMajor > 3)
			*pDestID1 = ST_ID3V2_FID___4_TDOR;
		break;
	case ST_ID3V2_FID_23__TRDA:
		if (tagVersionMajor > 3)
			*pDestID1 = ST_ID3V2_FID_234_COMM;
		break;
	case ST_ID3V2_FID___4_TIPL:
	case ST_ID3V2_FID___4_TMCL:
		if (tagVersionMajor < 4)
			*pDestID1 = ST_ID3V2_FID_23__IPLS;
		break;
	case ST_ID3V2_FID_23__TDAT:
	case ST_ID3V2_FID_23__TIME:
	case ST_ID3V2_FID_23__TYER:
		if (tagVersionMajor > 3) {
			*pDestID1 = ST_ID3V2_FID___4_TDRL;
			if (srcID1 == ST_ID3V2_FID_23__TYER) {
				*pSrcID2 = ST_ID3V2_FID_23__TDAT;
				*pSrcID3 = ST_ID3V2_FID_23__TIME;
			} else if (srcID1 == ST_ID3V2_FID_23__TDAT) {
				*pSrcID2 = ST_ID3V2_FID_23__TYER;
				*pSrcID3 = ST_ID3V2_FID_23__TIME;
			} else {
				*pSrcID2 = ST_ID3V2_FID_23__TYER;
				*pSrcID3 = ST_ID3V2_FID_23__TDAT;
			}
		}
		break;
	case ST_ID3V2_FID__34_TRSN:
	case ST_ID3V2_FID__34_TRSO:
	case ST_ID3V2_FID__34_TOWN:
		if (tagVersionMajor < 3)
			*pDestID1 = ST_ID3V2_FID_234_COMM;
		break;
	/** */
	default:
		*pDiscard = ST_B_FALSE;
	}

	*pToMap = (*pDestID1 != ST_ID3V2_FID_NONE);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_tidy_mapField(Tst_id3v2_tag *pTag,
		Tst_id3v2_fldData *pFld1, Tst_id3v2_fldData *pFld2,
		Tst_id3v2_fldData *pFld3,
		const Tst_id3v2_frID destID1, const Tst_id3v2_frID destID2,
		const Tst_id3v2_frID destID3, Tst_bool *pWasMapped)
{
#	define LOC_GETTST_(mac_pFldI, mac_tst)  { \
			st_id3v2_fnc_getFieldStringTypes((mac_pFldI)->fldPrI.id, &strTp); \
			res = st_tagCFnc_chk_getTStamp((strTp.hasTStampStr || strTp.hasYearStr), \
					strTp.hasTimeStr, strTp.hasDateStr, \
					st_mtes_strArrGetElem(&(mac_pFldI)->dataSArr, 1), &mac_tst, &isTSok); \
			}
	Tst_err  res    = ST_ERR_SUCC;
	Tst_bool isTSok = ST_B_FALSE;
	Tst_str  tmpTSstr[100];
	Tst_tagCFnc_tstamp     tStamp,
	                       tStamp2,
	                       tStamp3;
	Tst_mtes_string        tmps;
	Tst_id3v2_fldData_intn *pFldI1,
	                       *pFldI2 = NULL,
	                       *pFldI3 = NULL;
	Tst_id3v2_fldd_strtp   strTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFld1 == NULL || pFld1->pObInternal == NULL ||
			pWasMapped == NULL)

	pFldI1 = (Tst_id3v2_fldData_intn*)pFld1->pObInternal;
	if (pFld2 != NULL)
		pFldI2 = (Tst_id3v2_fldData_intn*)pFld2->pObInternal;
	if (pFld3 != NULL)
		pFldI3 = (Tst_id3v2_fldData_intn*)pFld3->pObInternal;

	*pWasMapped = ST_B_TRUE;
	st_tagCFnc_stc_rsetTSta(&tStamp);
	st_tagCFnc_stc_rsetTSta(&tStamp2);
	st_tagCFnc_stc_rsetTSta(&tStamp3);
	st_id3v2_stc_rsetStrTp(&strTp);

	switch (pFldI1->fldPrI.id) {
	case ST_ID3V2_FID__34_WORS:
	case ST_ID3V2_FID__34_WPAY:
		/* --> ST_ID3V2_FID_234_WXXX */
		res = st_id3v2_fldAdd_textArr(pTag,
				ST_ID3V2_FID_234_WXXX, /*pFIDstr:*/NULL,
				/*pDesc:*/NULL, &pFldI1->dataSArr);
		break;
	case ST_ID3V2_FID_23__IPLS:
		/* --> ST_ID3V2_FID___4_TIPL */
		res = st_id3v2_fldAdd_textArr(pTag,
				ST_ID3V2_FID___4_TIPL, /*pFIDstr:*/NULL,
				/*pDesc:*/NULL, &pFldI1->dataSArr);
		break;
	case ST_ID3V2_FID___4_TIPL:
	case ST_ID3V2_FID___4_TMCL:
		/* --> ST_ID3V2_FID_23__IPLS */
		res = st_id3v2_fldAdd_textArr(pTag,
				ST_ID3V2_FID_23__IPLS, /*pFIDstr:*/NULL,
				/*pDesc:*/NULL, &pFldI1->dataSArr);
		break;
	case ST_ID3V2_FID___4_TDRC:
	case ST_ID3V2_FID___4_TDRL:
		/* --> ST_ID3V2_FID_23__TDAT && ..TIME && ..TYER */
		if (st_mtes_strArrGetElemCount(&pFldI1->dataSArr) > 0) {
			LOC_GETTST_(pFldI1, tStamp)
			if (res == ST_ERR_SUCC && tStamp.year > 0) {
				sprintf((char*)tmpTSstr, "%04u", tStamp.year);
				res = ST_TIV2__tidy_addFld_txtPChar(pTag,
						ST_ID3V2_FID_23__TYER, tmpTSstr);
			}
			if (res == ST_ERR_SUCC && tStamp.year > 0 &&
					tStamp.day > 0 && tStamp.month > 0) {
				sprintf((char*)tmpTSstr, "%02u%02u", tStamp.day, tStamp.month);
				res = ST_TIV2__tidy_addFld_txtPChar(pTag,
						ST_ID3V2_FID_23__TDAT, tmpTSstr);
			}
			if (res == ST_ERR_SUCC && tStamp.year > 0 &&
					tStamp.day > 0 && tStamp.month > 0 &&
					(tStamp.hour > 0 || tStamp.min > 0)) {
				sprintf((char*)tmpTSstr, "%02u%02u", tStamp.hour, tStamp.min);
				res = ST_TIV2__tidy_addFld_txtPChar(pTag,
						ST_ID3V2_FID_23__TIME, tmpTSstr);
			}
		}
		break;
	case ST_ID3V2_FID___4_TDOR:
		/* --> ST_ID3V2_FID_23__TORY */
		if (st_mtes_strArrGetElemCount(&pFldI1->dataSArr) > 0) {
			LOC_GETTST_(pFldI1, tStamp)
			if (res == ST_ERR_SUCC && tStamp.year > 0) {
				sprintf((char*)tmpTSstr, "%04u", tStamp.year);
				res = ST_TIV2__tidy_addFld_txtPChar(pTag,
						ST_ID3V2_FID_23__TORY, tmpTSstr);
			}
		}
		break;
	case ST_ID3V2_FID_23__TORY:
		/* --> ST_ID3V2_FID___4_TDOR */
		res = st_id3v2_fldAdd_textArr(pTag,
				ST_ID3V2_FID___4_TDOR, /*pFIDstr:*/NULL,
				/*pDesc:*/NULL, &pFldI1->dataSArr);
		break;
	case ST_ID3V2_FID_23__TDAT:
	case ST_ID3V2_FID_23__TIME:
	case ST_ID3V2_FID_23__TYER:
		/* --> ST_ID3V2_FID___4_TDRL */
		if (st_mtes_strArrGetElemCount(&pFldI1->dataSArr) > 0) {
			LOC_GETTST_(pFldI1, tStamp)
			if (res != ST_ERR_SUCC)
				return res;
		}
		if (pFldI2 != NULL && st_mtes_strArrGetElemCount(&pFldI2->dataSArr) > 0) {
			LOC_GETTST_(pFldI2, tStamp2)
			if (res != ST_ERR_SUCC)
				return res;
		}
		if (pFldI3 != NULL && st_mtes_strArrGetElemCount(&pFldI3->dataSArr) > 0) {
			LOC_GETTST_(pFldI3, tStamp3)
			if (res != ST_ERR_SUCC)
				return res;
		}
		tStamp.year  = (tStamp.year > 0 ? tStamp.year :
				(tStamp2.year > 0 ? tStamp2.year : tStamp3.year));
		tStamp.month = (tStamp.month > 0 ? tStamp.month :
				(tStamp2.month > 0 ? tStamp2.month : tStamp3.month));
		tStamp.day   = (tStamp.day > 0 ? tStamp.day :
				(tStamp2.day > 0 ? tStamp2.day : tStamp3.day));
		tStamp.hour  = (tStamp.hour > 0 ? tStamp.hour :
				(tStamp2.hour > 0 ? tStamp2.hour : tStamp3.hour));
		tStamp.min   = (tStamp.min > 0 ? tStamp.min :
				(tStamp2.min > 0 ? tStamp2.min : tStamp3.min));
		tStamp.sec   = (tStamp.sec > 0 ? tStamp.sec :
				(tStamp2.sec > 0 ? tStamp2.sec : tStamp3.sec));
		if (tStamp.year > 0) {
			sprintf((char*)tmpTSstr, "%04u", tStamp.year);
			if (tStamp.month > 0) {
				sprintf((char*)&tmpTSstr[4], "-%02u", tStamp.month);
				if (tStamp.day > 0) {
					sprintf((char*)&tmpTSstr[4 + 3], "-%02u", tStamp.day);
					if (tStamp.hour > 0 || tStamp.min > 0 || tStamp.sec > 0) {
						sprintf((char*)&tmpTSstr[4 + 6], "T%02u", tStamp.hour);
						if (tStamp.min > 0 || tStamp.sec > 0) {
							sprintf((char*)&tmpTSstr[4 + 9], ":%02u",
									tStamp.min);
							if (tStamp.sec > 0)
								sprintf((char*)&tmpTSstr[4 + 12], ":%02u",
										tStamp.sec);
						}
					}
				}
			}
			res = ST_TIV2__tidy_addFld_txtPChar(pTag,
					ST_ID3V2_FID___4_TDRL, tmpTSstr);
		}
		break;
	case ST_ID3V2_FID_2___COM1:
	case ST_ID3V2_FID___4_TDEN:
	case ST_ID3V2_FID___4_TDTG:
	case ST_ID3V2_FID___4_TMOO:
	case ST_ID3V2_FID_23__TRDA:
	case ST_ID3V2_FID___4_TSST:
	case ST_ID3V2_FID___4_TPRO:
	case ST_ID3V2_FID__34_TRSN:
	case ST_ID3V2_FID__34_TRSO:
	case ST_ID3V2_FID__34_TOWN:
		/* --> ST_ID3V2_FID_234_COMM */
		res = st_mtes_stc_initStr(&tmps);
		if (res != ST_ERR_SUCC)
			return res;
		res = st_mtes_setStrTo2(ST_MTES_TE_ISO, pFldI1->fldPrI.pIDstr, &tmps);
		if (res != ST_ERR_SUCC) {
			st_mtes_stc_freeStr(&tmps);
			return res;
		}
		res = st_id3v2_fldAdd_comm(pTag, NULL, &tmps, &pFldI1->dataSArr);
		st_mtes_stc_freeStr(&tmps);
		break;
	/** */
	default:
		*pWasMapped = ST_B_FALSE;  /* do nothing */
	}
	/* */
	if (*pWasMapped) {
		res = st_id3v2_disableFld_byID(pTag,
				pFldI1->fldPrI.id, (const Tst_str*)pFldI1->fldPrI.pIDstr,
				pFldI1->fnr);
		if (res == ST_ERR_SUCC && pFldI2 != NULL) {
			res = st_id3v2_disableFld_byID(pTag,
					pFldI2->fldPrI.id, (const Tst_str*)pFldI2->fldPrI.pIDstr,
					pFldI2->fnr);
			if (res == ST_ERR_SUCC && pFldI3 != NULL)
				res = st_id3v2_disableFld_byID(pTag,
						pFldI3->fldPrI.id, (const Tst_str*)pFldI3->fldPrI.pIDstr,
						pFldI3->fnr);
		}
	}
	return res;
#	undef LOC_GETTST_
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_tidy_mergFields(const char *pFnc, Tst_id3v2_tag *pTag,
		Tst_id3v2_fldData *pFld)
{
	Tst_err  res         = ST_ERR_SUCC;
	Tst_bool modStripArr = ST_B_FALSE,
	         modSrcFld   = ST_B_FALSE;
	Tst_id3v2_fldData      *pItFld  = pFld;
	Tst_id3v2_fldData_intn *pFldI,
	                       *pItFldI = NULL;
	Tst_id3v2_tag_intn     *pTagI;
	Tst_id3v2_fldd_strtp   strTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFld == NULL || pFld->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	st_id3v2_stc_rsetStrTp(&strTp);
	st_id3v2_fnc_getFieldStringTypes(pFldI->fldPrI.id, &strTp);

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (! st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
					pFldI->fldPrI.id, pFldI->fldPrI.pIDstr))
			continue;

		if (strTp.hasNumStr || strTp.hasYearStr ||
				strTp.hasDateStr || strTp.hasTimeStr ||
				strTp.hasTStampStr) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, pFnc, pItFldI,
						"disabling w/o merging field #%02u (numeroid)",
						pItFldI->fnr);
		} else if (strTp.hasPosStr) {
			/* add values from pItFldI to pFldI */
			res = ST_TIV2__tidy_mergePosFields(pItFldI, pFldI);
			if (res != ST_ERR_SUCC)
				break;
			/* */
			modSrcFld = ST_B_TRUE;
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, pFnc, pFldI,
						"merged with field #%02u", pItFldI->fnr);
		} else {
			Tst_uint32 y;

			/* add string-array from pItFldI to pFldI */
			for (y = 1; y <= st_mtes_strArrGetElemCount(&pItFldI->dataSArr); y++) {
				res = st_mtes_strArrAdd(&pFldI->dataSArr,
						st_mtes_strArrGetElem(&pItFldI->dataSArr, y));
				if (res != ST_ERR_SUCC)
					break;
				/* */
				modSrcFld = ST_B_TRUE;
			}
			if (res != ST_ERR_SUCC)
				break;
			/* strip array */
			if (modSrcFld) {
				res = st_mtes_strArrStrip(&pFldI->dataSArr,
							strTp.hasMappedStr, &modStripArr);
				if (res != ST_ERR_SUCC)
					break;
			}
			/* */
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, pFnc, pFldI,
						"appended field #%02u", pItFldI->fnr);
		}
		if (! pItFldI->errs.frWasMergd) {
			pItFldI->errs.frWasMergd = ST_B_TRUE;
			++pItFldI->frErrCnt;
		}
		/* disable pItFld */
		res = st_id3v2_disableFld_byID(pTag,
				pItFldI->fldPrI.id, (const Tst_str*)pItFldI->fldPrI.pIDstr,
				pItFldI->fnr);
		if (res != ST_ERR_SUCC)
			break;
	}

	if (res == ST_ERR_SUCC && modSrcFld)
		res = st_id3v2_chk_checkFld(&pTagI->opts, ST_B_FALSE, pFld);
	return res;
}

Tst_err
st_id3v2_tidy_mergMultStr(const char *pFnc, const Tst_id3v2_opts *pOpts,
		Tst_id3v2_fldData *pFld)
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pOpts == NULL ||
			pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	res = st_mtes_strArrMergeToOneString(&pFldI->dataSArr, (const Tst_str*)", ");

	if (res == ST_ERR_SUCC) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
					ST_B_TRUE, "mer", st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
		res = st_id3v2_chk_checkFld(pOpts, ST_B_FALSE, pFld);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_tidy_remDups(const char *pFnc, const Tst_bool discardFields,
		Tst_id3v2_tag *pTag, Tst_id3v2_fldData *pFld,
		Tst_bool *pMadeOneUnique)
{
	const char *cDESC_PIC = "attached picture";
	const char *cDESC_GEO = "attached file";
	const char *cDESC_COM = "comment";
	const char *cDESC_ULT = "unsynced lyrics";
	const char *cDESC_SLT = "synced lyrics";
	const char *cDESC_UFI = "www.no-owner-address.com";
	const char *cDESC_TXX = "custom txt-field";
	const char *cDESC_WXX = "custom url-field";
	Tst_err         res    = ST_ERR_SUCC;
	Tst_bool        remove = ST_B_FALSE,
	                lngEqu = ST_B_FALSE,
	                mkUni  = ST_B_FALSE;
	Tst_str         tmps[1024];
	Tst_mtes_string tmpis;
	char const      *pDescDef;
	Tst_id3v2_fldData      *pItFld  = pFld;
	Tst_id3v2_fldData_intn *pItFldI = NULL,
	                       *pFldI   = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFld == NULL || pFld->pObInternal == NULL ||
			pMadeOneUnique == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	*pMadeOneUnique = ST_B_FALSE;
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->errs.frIsDup ||
				! st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
					pFldI->fldPrI.id, pFldI->fldPrI.pIDstr))
			continue;
		/* remove unconditionally */
		switch (pFldI->fldPrI.id) {
		case ST_ID3V2_FID_234_MCDI:
		case ST_ID3V2_FID_234_ETCO:
		case ST_ID3V2_FID_234_MLLT:
		case ST_ID3V2_FID_234_SYTC:
		case ST_ID3V2_FID_23__RVAD:
		case ST_ID3V2_FID_23__EQUA:
		case ST_ID3V2_FID_234_RVRB:
		case ST_ID3V2_FID_234_PCNT:
		case ST_ID3V2_FID_234_RBUF:
		case ST_ID3V2_FID__34_POSS:
		case ST_ID3V2_FID__34_OWNE:
		case ST_ID3V2_FID___4_SEEK:
		case ST_ID3V2_FID___4_ASPI:
		case ST_ID3V2_FID_234_WCOP:
		case ST_ID3V2_FID_234_WOAF:
		case ST_ID3V2_FID_234_WOAS:
		case ST_ID3V2_FID__34_WORS:
		case ST_ID3V2_FID__34_WPAY:
		case ST_ID3V2_FID_234_WPUB:
			remove = ST_B_TRUE;
			break;
		default:
			remove = ST_B_FALSE;  /* do nothing */
		}
		/* remove conditionally */
		res = ST_ERR_SUCC;
		switch (pFldI->fldPrI.id) {
		case ST_ID3V2_FID_234_POPM:  /* email */
			remove = st_mtes_strEqual(&pFldI->attrDesc, &pItFldI->attrDesc, ST_B_FALSE);
			break;
		case ST_ID3V2_FID__34_PRIV:  /* owner & data */
			remove = (st_binobj_getDataSize(&pFldI->dataBinDat) ==
					st_binobj_getDataSize(&pItFldI->dataBinDat));
			remove = remove &&
					st_mtes_strEqual(&pFldI->attrDesc, &pItFldI->attrDesc, ST_B_FALSE);
			remove = remove &&
					st_binobj_areEqual(&pFldI->dataBinDat, &pItFldI->dataBinDat);
			break;
		case ST_ID3V2_FID_234_LINK:  /* everything */
		case ST_ID3V2_FID__34_COMR:  /*  --- " --- */
		case ST_ID3V2_FID___4_SIGN:  /*  --- " --- */
			remove = (pFldI->fldPrI.typ != ST_ID3V2_FTP_CPY) ||
					(st_binobj_getDataSize(&pFldI->dataBinDat) ==
							st_binobj_getDataSize(&pItFldI->dataBinDat) &&
						st_binobj_areEqual(&pFldI->dataBinDat, &pItFldI->dataBinDat));
			break;
		/* AENC/CRM1/RVA2/EQU2: compare by ident */
		/* ENCR/GRID: There may be several "ENCR" frames in a tag
		     but only one containing the same symbol and only one containing the
		     same owner identifier */
		/* USER: compare by language */
		case ST_ID3V2_FID_234_WCOM:  /* url */
		case ST_ID3V2_FID_234_WOAR:  /* url */
			if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) >= 1 &&
					st_mtes_strArrGetElemCount(&pItFldI->dataSArr) >= 1)
				remove = st_mtes_strEqual(st_mtes_strArrGetElem(&pFldI->dataSArr, 1),
						st_mtes_strArrGetElem(&pItFldI->dataSArr, 1), ST_B_FALSE);
			break;
		default:
			remove = ST_B_FALSE;  /* do nothing */
		}
		if (res != ST_ERR_SUCC)
			return res;
		if (remove) {
			if (! discardFields && ! pItFldI->errs.frIsDup &&
					ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, pFnc, pItFldI,
						"should be discarded (dup fld data)");
			/* */
			if (! pItFldI->errs.frIsDup) {
				pItFldI->errs.frIsDup = ST_B_TRUE;
				++pItFldI->frErrCnt;
			}
			/* */
			if (discardFields) {
				res = st_id3v2_disableFld_byID(pTag,
						pItFldI->fldPrI.id, (const Tst_str*)pItFldI->fldPrI.pIDstr,
						pItFldI->fnr);
				if (res != ST_ERR_SUCC)
					return res;
			}
			continue;
		}
		/* make unique if not already unique */
		/** */
		lngEqu = ST_B_TRUE;
		switch (pFldI->fldPrI.id) {
		case ST_ID3V2_FID_234_USLT:  /* language & content descriptor */
		case ST_ID3V2_FID_234_SYLT:  /*  ---- " ---- */
		case ST_ID3V2_FID_234_COMM:  /*  ---- " ---- */
			lngEqu = (st_sysStrcmpN2(ST_B_FALSE, sizeof(pItFldI->attrLang),
						pFldI->attrLang, pItFldI->attrLang));
		case ST_ID3V2_FID_234_UFID:  /* owner */
		case ST_ID3V2_FID_234_APIC:  /* content descriptor */
		case ST_ID3V2_FID_234_GEOB:  /*  ---- " ---- */
		case ST_ID3V2_FID_234_TXXX:  /*  ---- " ---- */
		case ST_ID3V2_FID_234_WXXX:  /*  ---- " ---- */
			mkUni = lngEqu &&
					st_mtes_strEqual(&pFldI->attrDesc, &pItFldI->attrDesc, ST_B_FALSE);
			break;
		default:
			mkUni = ST_B_FALSE;  /* do nothing */
		}
		if (! mkUni)
			continue;
		*pMadeOneUnique = ST_B_TRUE;
		/** */
		switch (pFldI->fldPrI.id) {
		case ST_ID3V2_FID_234_USLT: pDescDef = cDESC_ULT; break;
		case ST_ID3V2_FID_234_SYLT: pDescDef = cDESC_SLT; break;
		case ST_ID3V2_FID_234_COMM: pDescDef = cDESC_COM; break;
		case ST_ID3V2_FID_234_UFID: pDescDef = cDESC_UFI; break;
		case ST_ID3V2_FID_234_APIC: pDescDef = cDESC_PIC; break;
		case ST_ID3V2_FID_234_GEOB: pDescDef = cDESC_GEO; break;
		case ST_ID3V2_FID_234_TXXX: pDescDef = cDESC_TXX; break;
		case ST_ID3V2_FID_234_WXXX: pDescDef = cDESC_WXX; break;
		default:
			pDescDef = NULL;
		}
		if (! st_mtes_isStrEmptyBy(&pItFldI->attrDesc))
			sprintf((char*)tmps, " (#%02u)", pItFldI->fnr);
		else if (pDescDef != NULL)
			sprintf((char*)tmps, "%s (#%02u)", pDescDef, pItFldI->fnr);
		else
			sprintf((char*)tmps, "(#%02u)", pItFldI->fnr);
		res = st_mtes_stc_initStr(&tmpis);
		if (res != ST_ERR_SUCC)
			return res;
		res = st_mtes_setStrTo(st_mtes_getEncodingCur(&pItFldI->attrDesc),
				tmps, &tmpis);
		if (res != ST_ERR_SUCC) {
			st_mtes_stc_freeStr(&tmpis);
			return res;
		}
		res = st_mtes_strConcat(&tmpis, &pItFldI->attrDesc);
		st_mtes_stc_freeStr(&tmpis);
		if (res != ST_ERR_SUCC)
			return res;
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3Str(&pTagI->opts, 0, pFnc, pItFldI,
					ST_B_TRUE, "dup", &pItFldI->attrDesc, "");
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__tidy_addFld_txtPChar(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, const Tst_str *pTextISO)
{
	Tst_err res;
	Tst_mtes_string tmps;

	res = st_mtes_stc_initStr(&tmps);
	if (res == ST_ERR_SUCC) {
		res = st_mtes_setStrTo(ST_MTES_TE_ISO, pTextISO, &tmps);
		if (res == ST_ERR_SUCC)
			res = st_id3v2_fldAdd_text(pTag, frID, /*pFIDstr:*/NULL,
					/*pDesc:*/NULL, &tmps);
		st_mtes_stc_freeStr(&tmps);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__tidy_mergePosFields(Tst_id3v2_fldData_intn *pFldIsrc,
		Tst_id3v2_fldData_intn *pFldIdst)
{
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 num1    = 0,
	           num2    = 0,
	           num1Tot = 0,
	           num2Tot = 0;
	Tst_mtes_string *pStr;

	/* get values from pFldIsrc */
	pStr = st_mtes_strArrGetElem(&pFldIsrc->dataSArr, 1);
	if (pStr != NULL) {
		/* split string up into NUMBER and TOTAL */
		res = st_tagCFnc_chk_getPosStringNums(pStr, &num1, &num1Tot);
		if (res != ST_ERR_SUCC)
			return res;
	}
	/* get values from pFldIdst */
	pStr = st_mtes_strArrGetElem(&pFldIdst->dataSArr, 1);
	if (pStr != NULL) {
		/* split string up into NUMBER and TOTAL */
		res = st_tagCFnc_chk_getPosStringNums(pStr, &num2, &num2Tot);
		if (res != ST_ERR_SUCC)
			return res;
	}
	/* */
	if (num2 == 0)
		num2 = num1;
	if (num2Tot == 0)
		num2Tot = num1Tot;
	/* set new string value */
	st_mtes_stc_rsetSA(&pFldIdst->dataSArr);
	if (num2 != 0 || num2Tot != 0) {
		char tmpNS[128];
		Tst_mtes_string tmpMS;

		st_mtes_stc_initStr(&tmpMS);
		if (num2 > 0) {
			if (num2Tot > 0)
				snprintf(tmpNS, sizeof(tmpNS), "%u/%u", num2, num2Tot);
			else
				snprintf(tmpNS, sizeof(tmpNS), "%u", num2);
		} else
			snprintf(tmpNS, sizeof(tmpNS), "0/%u", num2Tot);
		res = st_mtes_copyFromCharp_iso((const Tst_str*)tmpNS, &tmpMS);
		if (res == ST_ERR_SUCC)
			res = st_mtes_strArrAdd(&pFldIdst->dataSArr, &tmpMS);
		st_mtes_stc_freeStr(&tmpMS);
	}
	return res;
}

/******************************************************************************/
