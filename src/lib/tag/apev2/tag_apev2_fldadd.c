/******************************************************************************/
/* tag_apev2_fldadd.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for adding APEv2 Tag fields                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/dynlist.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_FLDADD_C
#include "tag_apev2-prot.h"
#include "tag_apev2_fnc-prot.h"
#include "tag_apev2_stcs-prot.h"
#include "tag_apev2_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_APEV2_FLDADD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef struct {
	Tst_mtes_string const *pText;
	Tst_mtes_strArr       *pStrArr;
	Tst_bool              isBinOrStr;
	Tst_binobj            *pBinDat;
	Tst_mtes_string const *pBinDatSrcFn;
} Tst_av2__addfldgen;

/** Tst_av2__addfldgen */
#define ST_RSETSTC_TAV2AFG(afg)  { \
			afg.pText        = NULL; \
			afg.pStrArr      = NULL; \
			afg.isBinOrStr   = ST_B_TRUE; \
			afg.pBinDat      = NULL; \
			afg.pBinDatSrcFn = NULL; \
		}

static Tst_err
ST_TAV2__fa_add(Tst_apev2_tag *pTag,
                Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
                const Tst_uint32 insertPos, Tst_av2__addfldgen *pAFG);
static Tst_err
ST_TAV2__fa_fldSet(Tst_apev2_tag *pTag,
                   Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
                   const Tst_uint32 frNr, const Tst_av2__addfldgen *pAFG);
static Tst_err
ST_TAV2__fa_setBinDat(const char *pFnc, Tst_apev2_tag_intn *pTagI,
                      Tst_apev2_fldData_intn *pFldI,
                      ST_OPTARG(Tst_binobj *pSrcBDat),
                      ST_OPTARG(const Tst_mtes_string *pSrcFn));

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Check whether Tag already contains a field with similiar/equal data
 *   (only the main data will be compared,
 *     e.g. the string-array or binary data)
 *   If tolerateDiffSLen is ST_B_TRUE then strings will be compared
 *     until max{str1.len, str2.len}
 *
 * pTag             MUST be set
 * pFld             MUST be set
 * pHasSimiliarFld  MUST be != NULL
 */
Tst_err
st_apev2_fldAdd_doesSimiliarFieldExist(Tst_apev2_tag *pTag,
		const Tst_bool tolerateDiffSLen, const Tst_apev2_fldData *pFld,
		Tst_bool *pHasSimiliarFld)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_bool   areEqu = ST_B_FALSE,
	           triDot = ST_B_FALSE;
	Tst_uint32 saCnt,
	           z,
	           sl1,
	           sl2;
	Tst_mtes_string const  *pStr1,
	                       *pStr2;
	Tst_apev2_fldData      *pItFld  = NULL;
	Tst_apev2_fldData_intn *pItFldI = NULL,
	                       *pFldI;
	Tst_apev2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFld == NULL || pFld->pObInternal == NULL ||
			pHasSimiliarFld == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

	*pHasSimiliarFld = ST_B_FALSE;
	if (! pFldI->isOK)
		return ST_ERR_SUCC;
	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (! st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
					pFldI->fldPrI.id, pFldI->fldPrI.pVIDstr))
			continue;

		switch (pFldI->fldPrI.typ) {
		case ST_APEV2_FTP_BDAT:
			*pHasSimiliarFld =
					st_binobj_areEqual(&pItFldI->dataBinDat, &pFldI->dataBinDat);
			break;
		default:
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
							return res;
					}
					if (sl1 > sl2)
						sl1 = sl2;
					if (triDot && sl1 > 3)
						sl1 -= 3;
				}
				areEqu = st_mtes_strNEqual(pStr1, pStr2, ST_B_FALSE, sl1);
				if (z == 1)
					*pHasSimiliarFld = areEqu;
				else {
					*pHasSimiliarFld = *pHasSimiliarFld && areEqu;
					if (! areEqu)
						break;
				}
			}
		}
		/* */
		if (*pHasSimiliarFld)
			break;
	}
	/* */
	if (*pHasSimiliarFld && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		/* the pointer pItFldI is still valid here... */
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"new %s field is similiar to existing %s field #%02u",
				pFldI->fldPrI.pVIDstr,
				pItFldI->fldPrI.pVIDstr, pItFldI->fnr);
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Add field to Tag
 *
 * pTag      MUST be set
 * pFldAdd   MUST be set
 */
Tst_err
st_apev2_fldAdd(Tst_apev2_tag *pTag, const Tst_apev2_fldData *pFldAdd)
{
	return st_apev2_fldIns(pTag, 0, pFldAdd);
}

/*
 * Insert field at position insertPos (^=field nr.) into Tag
 *   If insertPos is 0 then the field will simply be added
 *
 * pTag      MUST be set
 * pFldAdd   MUST be set
 */
Tst_err
st_apev2_fldIns(Tst_apev2_tag *pTag, const Tst_uint32 insertPos,
		const Tst_apev2_fldData *pFldAdd)
{
	const char *cFNCN = __FUNCTION__;
	Tst_av2__addfldgen     afg;
	Tst_apev2_fldData_intn *pFldAddI = NULL;
	Tst_apev2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFldAdd == NULL || pFldAdd->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	pFldAddI = (Tst_apev2_fldData_intn*)pFldAdd->pObInternal;

	if (! pFldAddI->isOK) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_fdeb(&pTagI->opts, 0, cFNCN, pFldAddI,
					"ignoring field");
		return ST_ERR_SUCC;
	}

	if (! st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS,
				pFldAddI->fldPrI.id, NULL))
		return ST_ERR_IARG;

	ST_RSETSTC_TAV2AFG(afg)
	afg.pStrArr    = &pFldAddI->dataSArr;
	afg.pBinDat    = &pFldAddI->dataBinDat;
	afg.isBinOrStr = (pFldAddI->fldPrI.typ == ST_APEV2_FTP_BDAT);
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_fdeb(&pTagI->opts, 0, cFNCN, pFldAddI,
				"%s field...", (insertPos > 0 ? "inserting" : "adding"));
	return ST_TAV2__fa_add(pTag,
			pFldAddI->fldPrI.id, (const Tst_str*)pFldAddI->fldPrI.pVIDstr,
			insertPos, &afg);
}

/*
 * Add text field to Tag
 *   If frID is ST_APEV2_FID_CTXT
 *     then a ID-String must be given in pFrIDstr
 *   If pFrIDstr contains the name of a custom field
 *     then frID must be ST_APEV2_FID_CTXT
 *
 * pTag    MUST be set
 * pText   MUST be set
 */
Tst_err
st_apev2_fldAdd_text(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_mtes_string *pText)
{
	return st_apev2_fldIns_text(pTag, frID, pFrIDstr, 0, pText);
}

/*
 * Insert text field at position insertPos (^=field nr.) into Tag
 *   If frID is ST_APEV2_FID_CTXT
 *     then a ID-String must be given in pFrIDstr
 *   If pFrIDstr contains the name of a custom field
 *     then frID must be ST_APEV2_FID_CTXT
 *   If insertPos is 0 then the field will simply be added
 *
 * pTag    MUST be set
 * pText   MUST be set
 */
Tst_err
st_apev2_fldIns_text(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 insertPos, const Tst_mtes_string *pText)
{
	const char *cFNCN = __FUNCTION__;
	Tst_av2__addfldgen afg;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pText == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.pText      = pText;
	afg.isBinOrStr = ST_B_FALSE;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
			frID != ST_APEV2_FID_TAGG)
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"%s field...", (insertPos > 0 ? "inserting" : "adding"));
	return ST_TAV2__fa_add(pTag, frID, pFrIDstr, insertPos, &afg);
}

Tst_err
st_apev2_fldAdd_textArr(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		Tst_mtes_strArr *pTextArr)
{
	return st_apev2_fldIns_textArr(pTag, frID, pFrIDstr, 0, pTextArr);
}

Tst_err
st_apev2_fldIns_textArr(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 insertPos, Tst_mtes_strArr *pTextArr)
{
	const char *cFNCN = __FUNCTION__;
	Tst_av2__addfldgen afg;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pTextArr == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.pStrArr    = pTextArr;
	afg.isBinOrStr = ST_B_FALSE;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
			frID != ST_APEV2_FID_TAGG)
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"%s field...", (insertPos > 0 ? "inserting" : "adding"));
	return ST_TAV2__fa_add(pTag, frID, pFrIDstr, insertPos, &afg);
}

/*
 * Add custom binary field to Tag
 *
 * pTag       MUST be set
 * pFrIDstr   MUST be set
 * pBinDat    MUST be set
 */
Tst_err
st_apev2_fldAdd_binary(Tst_apev2_tag *pTag,
		const Tst_str *pFrIDstr, Tst_binobj *pBinDat)
{
	const char *cFNCN = __FUNCTION__;
	Tst_av2__addfldgen afg;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFrIDstr == NULL || pBinDat == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.isBinOrStr = ST_B_TRUE;
	afg.pBinDat    = pBinDat;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"adding field...");
	return ST_TAV2__fa_add(pTag, ST_APEV2_FID_CBIN, pFrIDstr, 0, &afg);
}

Tst_err
st_apev2_fldAdd_binary_rdFromFile(Tst_apev2_tag *pTag,
		const Tst_str *pFrIDstr,
		const Tst_mtes_string *pFilen)
{
	const char *cFNCN = __FUNCTION__;
	Tst_av2__addfldgen afg;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFrIDstr == NULL || st_mtes_isStrEmptyCh(pFilen))

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.isBinOrStr   = ST_B_TRUE;
	afg.pBinDatSrcFn = pFilen;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"adding field...");
	return ST_TAV2__fa_add(pTag, ST_APEV2_FID_CBIN, pFrIDstr, 0, &afg);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set text field in Tag
 *   If frID is ST_APEV2_FID_CTXT
 *     then a ID-String must be given in pFrIDstr
 *   If pFrIDstr contains the name of a custom field
 *     then frID must be ST_APEV2_FID_CTXT
 *
 * pTag    MUST be set
 * pText   MUST be set
 */
Tst_err
st_apev2_fldSet_dataStr(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_mtes_string *pText)
{
	Tst_av2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pText == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.pText      = pText;
	afg.isBinOrStr = ST_B_FALSE;
	return ST_TAV2__fa_fldSet(pTag, frID, pFrIDstr, frNr, &afg);
}

/*
 * Set numeric text field in Tag
 *   If num is < 0 then no new field will be added
 *
 * pTag   MUST be set
 */
Tst_err
st_apev2_fldSet_dataStr_int(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_int32 num)
{
	Tst_err res;
	Tst_str tmps[100];
	Tst_mtes_string tmpis;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	tmps[0] = 0x00;
	if (num >= 0)
		sprintf((char*)tmps, "%d", num);
	/* disable old fields */
	if (frNr == 0) {
		res = st_apev2_disableFld_byID_all(pTag, frID, pFrIDstr);
		if (res != ST_ERR_SUCC)
			return res;
	}
	if (tmps[0] == 0x00)
		return ST_ERR_SUCC;
	/* add new field */
	res = st_mtes_stc_initStr(&tmpis);
	if (res != ST_ERR_SUCC)
		return res;
	res = st_mtes_copyFromCharp_isoOrUtf8(tmps, ST_B_TRUE, &tmpis);
	if (res == ST_ERR_SUCC)
		res = st_apev2_fldSet_dataStr(pTag, frID, pFrIDstr, frNr, &tmpis);
	st_mtes_stc_freeStr(&tmpis);
	return res;
}

/*
 * Set text field in Tag
 *
 * pTag       MUST be set
 * pTextArr   MUST be set
 */
Tst_err
st_apev2_fldSet_dataStrArr(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		Tst_mtes_strArr *pTextArr)
{
	Tst_av2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTextArr == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.pStrArr    = pTextArr;
	afg.isBinOrStr = ST_B_FALSE;
	return ST_TAV2__fa_fldSet(pTag, frID, pFrIDstr, frNr, &afg);
}

/*
 * Set binary data of field
 *
 * pTag      MUST be set
 * pBinDat   MUST be set
 */
Tst_err
st_apev2_fldSet_dataBin(Tst_apev2_tag *pTag,
		const Tst_uint32 frNr,
		Tst_binobj *pBinDat)
{
	Tst_av2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pBinDat == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.pBinDat = pBinDat;
	return ST_TAV2__fa_fldSet(pTag, ST_APEV2_FID_CBIN, NULL, frNr, &afg);
}

Tst_err
st_apev2_fldSet_dataBin_rdFromFile(Tst_apev2_tag *pTag,
		const Tst_uint32 frNr,
		const Tst_mtes_string *pFilen)
{
	Tst_av2__addfldgen afg;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFilen == NULL)

	ST_RSETSTC_TAV2AFG(afg)
	afg.pBinDatSrcFn = pFilen;
	return ST_TAV2__fa_fldSet(pTag, ST_APEV2_FID_CBIN, NULL, frNr, &afg);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Move the new field to another position
 */
static Tst_err
ST_TAV2__fa_add_moveToIP(const char *pFnc,
		Tst_apev2_tag *pTag, Tst_apev2_tag_intn *pTagI,
		const Tst_uint32 insertPos,
		Tst_apev2_fldData_intn *pFldItoMove)
{
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 ixFIPC  = 0,
	           ixNF;
	Tst_bool   fndFIPC = ST_B_FALSE;
	Tst_apev2_fldData      *pItFld = NULL;
	Tst_apev2_fldData_intn *pItFldI;

	/* get list index of field that is currently at
	 *   the position insertPos  */
	while ((pItFld = st_apev2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		++ixFIPC;
		if (pItFldI->fnr == insertPos &&
				st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
						pFldItoMove->fldPrI.id, pFldItoMove->fldPrI.pVIDstr)) {
			fndFIPC = ST_B_TRUE;
			break;
		}
	}
	if (! fndFIPC) {
		st_apev2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
				"could not find index of field at insertPos");
		return ST_ERR_FAIL;
	}
	/* increment field number of all 'bigger' fields */
	pItFld = NULL;
	while ((pItFld = st_apev2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fnr == pFldItoMove->fnr || pItFldI->fnr < insertPos ||
				! st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
						pFldItoMove->fldPrI.id, pFldItoMove->fldPrI.pVIDstr)) {
			continue;
		}
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			st_apev2_d_fdeb(&pTagI->opts, 0, pFnc, pItFldI,
					"inc fldNr");
		}
		++pItFldI->fnr;
	}
	/* change new fields' number */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		st_apev2_d_fdeb(&pTagI->opts, 0, pFnc, pFldItoMove,
				"set fldNr to %u", insertPos);
	}
	pFldItoMove->fnr = insertPos;
	/* move new field to other position in list and
	 *   update unique field indices  */
	ixNF = st_dynlist_getElementCount(&pTagI->flds.list);
	if (ixNF > ixFIPC) {
		res = st_apev2_fnc_moveFld(pTag, ixNF, ixFIPC);
		if (res != ST_ERR_SUCC) {
			st_apev2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"moving field failed");
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Actually add a new field
 */
static Tst_err
ST_TAV2__fa_add(Tst_apev2_tag *pTag,
		Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 insertPos, Tst_av2__addfldgen *pAFG)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 w;
	Tst_bool   skippedField = ST_B_FALSE;
	Tst_apev2_fldData      *pFld  = NULL;
	Tst_apev2_fldData_intn *pFldI = NULL;
	Tst_apev2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	/* adjust frID */
	frID = st_apev2_fnc_adjustFrID(frID, pFrIDstr, NULL);
	if (frID == ST_APEV2_FID_NONE)
		return ST_ERR_IARG;
	if ((frID == ST_APEV2_FID_CTXT || frID == ST_APEV2_FID_CBIN) &&
			st_apev2_fnc_hasIDstrInvalidChars(pFrIDstr, NULL)) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_deb(&pTagI->opts, 3, cFNCN,
					"custom ID-String '%s' is invalid",
					(pFrIDstr == NULL ? "" : (const char*)pFrIDstr));
		return ST_ERR_SUCC;
	}

	if (st_dynlist_getElementCount(&pTagI->flds.list) >= ST_APEV2_MAX_TFLD_CNT) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"too many frames for one tag, aborting (max %u)",
				ST_APEV2_MAX_TFLD_CNT);
		return ST_ERR_ABRT;
	}

	/* add new field to tag */
	res = st_apev2_fnc_addElemToFlds(pTag,
			frID, pFrIDstr, pAFG->isBinOrStr,
			&skippedField, &pFld);
	if (res == ST_ERR_SUCC)
		pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;
	if (res != ST_ERR_SUCC || skippedField) {
		if (res == ST_ERR_SUCC && skippedField &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			st_apev2_d_fdeb(&pTagI->opts, 3, cFNCN, pFldI,
					"too many frames for this ID, skipping (%u > %u)",
					pFldI->fnr, ST_APEV2_MAX_TFLDID_CNT);
		}
		return res;
	}

	/* copy data to field */
	/** text */
	if (pAFG->pText != NULL && pFldI->fldPrI.typ != ST_APEV2_FTP_BDAT &&
			! st_mtes_isStrEmptyCh(pAFG->pText)) {
		res = st_mtes_strArrAdd(&pFldI->dataSArr, pAFG->pText);
		if (res != ST_ERR_SUCC)
			return res;
		pFldI->isOK = ST_B_TRUE;
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
				st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
				pFldI->fldPrI.id != ST_APEV2_FID_TAGG)
			st_apev2_d_fdebStr(&pTagI->opts, 3, cFNCN, pFldI,
					"txt", st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
	} else
		if (pAFG->pStrArr != NULL &&
				pFldI->fldPrI.typ != ST_APEV2_FTP_BDAT &&
				st_mtes_strArrGetElemCount(pAFG->pStrArr) > 0) {
			for (w = 1; w <= st_mtes_strArrGetElemCount(pAFG->pStrArr); w++) {
				res = st_mtes_strArrAdd(&pFldI->dataSArr,
						st_mtes_strArrGetElem(pAFG->pStrArr, w));
				if (res != ST_ERR_SUCC)
					return res;
				pFldI->isOK = ST_B_TRUE;
				/* */
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
						st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
						pFldI->fldPrI.id != ST_APEV2_FID_TAGG) {
					if (st_mtes_strArrGetElemCount(pAFG->pStrArr) == 1)
						st_apev2_d_fdebStr(&pTagI->opts, 3, cFNCN, pFldI,
								"txt",
								st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
					else
						st_apev2_d_fdebStr(&pTagI->opts, 3, cFNCN, pFldI,
								"mul",
								st_mtes_strArrGetElem(&pFldI->dataSArr, w), "");
				}
			}
		}
	/** binary data */
	if ((pAFG->pBinDat != NULL || pAFG->pBinDatSrcFn != NULL) &&
			pFldI->fldPrI.typ == ST_APEV2_FTP_BDAT) {
		res = ST_TAV2__fa_setBinDat(cFNCN, pTagI, pFldI,
					pAFG->pBinDat, pAFG->pBinDatSrcFn);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		pFldI->isOK = (st_binobj_getDataSize(&pFldI->dataBinDat) > 0);
	}

	/* tag should now have at least one valid field */
	if (pFldI->isOK)
		st_tagBas_gs_setHasFields(&pTagI->tbas, ST_B_TRUE);

	/* change field's number if desired */
	if (res == ST_ERR_SUCC && pFldI->isOK &&
			insertPos > 0 && insertPos < pFldI->fnr) {
		res = ST_TAV2__fa_add_moveToIP(cFNCN, pTag, pTagI, insertPos, pFldI);
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Set data of field in Tag
 */
static Tst_err
ST_TAV2__fa_fldSet(Tst_apev2_tag *pTag,
		Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_av2__addfldgen *pAFG)
{
#	define LOC_DBG_SETFDAT_  { \
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) \
					st_apev2_d_fdeb(&pTagI->opts, 3, cFNCN, pItFldI, \
							"setting data"); \
				}
#	define LOC_DBG_ADDF_  { \
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) \
					st_apev2_d_deb(&pTagI->opts, 0, cFNCN, \
							"adding new field"); \
				}
	const char *cFNCN = __FUNCTION__;
	Tst_err       res          = ST_ERR_SUCC;
	Tst_bool      fnd          = ST_B_FALSE,
	              wasHnd;
	Tst_str const *pIntFrIDstr = pFrIDstr;
	Tst_apev2_fldData      *pItFld = NULL;
	Tst_apev2_fldData_intn *pItFldI;
	Tst_apev2_tag_intn     *pTagI;
	Tst_apev2_fldd_dattp   datTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pAFG == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	if (pFrIDstr == NULL && frID == ST_APEV2_FID_CBIN)
		pIntFrIDstr = (Tst_str const*)"DUMMYGEOB";

	/* adjust frID */
	frID = st_apev2_fnc_adjustFrID(frID, pIntFrIDstr, NULL);
	if (frID == ST_APEV2_FID_NONE)
		return ST_ERR_IARG;
	if (frID == ST_APEV2_FID_CTXT &&
			st_apev2_fnc_hasIDstrInvalidChars(pFrIDstr, NULL)) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_deb(&pTagI->opts, 3, cFNCN,
					"custom ID-String '%s' is invalid",
					(pFrIDstr == NULL ? "" : (const char*)pFrIDstr));
		return ST_ERR_SUCC;
	}

	if (frID == ST_APEV2_FID_TAGG)
		return ST_ERR_SUCC;  /* tagger info cannot be modified */

	st_apev2_stc_rsetDatTp(&datTp);
	st_apev2_fnc_getFieldDataTypes(frID, &datTp);

	/* set data in existing field(s) */
	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if ((frNr != 0 && pItFldI->fnr != frNr) ||
				! st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
							frID, (const char*)pIntFrIDstr))
			continue;
		/* */
		wasHnd = ST_B_FALSE;

		/* reset field ? */
		if (! datTp.hasText &&
				(pAFG->pText != NULL || pAFG->pStrArr != NULL)) {
			fnd    = ST_B_TRUE;
			wasHnd = ST_B_TRUE;
			if ((pAFG->pText != NULL && st_mtes_isStrEmptyCh(pAFG->pText)) ||
					(pAFG->pStrArr != NULL &&
						st_mtes_strArrTotalStrlenBy(pAFG->pStrArr) == 0)) {
				res = st_apev2_disableFld_byID(pTag,
							frID, pFrIDstr, pItFldI->fnr);
				if (res != ST_ERR_SUCC)
					break;
			}
		}

		/* set field data */
		if (! wasHnd && pAFG->pText != NULL && datTp.hasText) {
			LOC_DBG_SETFDAT_
			fnd = ST_B_TRUE;
			st_mtes_stc_rsetSA(&pItFldI->dataSArr);
			res = st_mtes_strArrAdd(&pItFldI->dataSArr, pAFG->pText);
		} else if (! wasHnd && pAFG->pStrArr != NULL && datTp.hasText) {
			LOC_DBG_SETFDAT_
			fnd = ST_B_TRUE;
			st_mtes_stc_rsetSA(&pItFldI->dataSArr);
			res = st_mtes_strArrCpy(pAFG->pStrArr, &pItFldI->dataSArr);
		} else if (! wasHnd && (pAFG->pBinDat != NULL || pAFG->pBinDatSrcFn != NULL) &&
				datTp.hasBDat) {
			LOC_DBG_SETFDAT_
			fnd = ST_B_TRUE;
			/* copy binary data */
			res = ST_TAV2__fa_setBinDat(cFNCN, pTagI, pItFldI,
						pAFG->pBinDat, pAFG->pBinDatSrcFn);
		}
		if (res != ST_ERR_SUCC)
			break;
		/* */
		if (frNr != 0)
			break;
	}

	/* if no existing field was found we add a new one */
	if (res == ST_ERR_SUCC && ! fnd && frNr == 0) {
		if (pAFG->pText != NULL && datTp.hasText &&
				! st_mtes_isStrEmptyCh(pAFG->pText)) {
			LOC_DBG_ADDF_
			fnd = ST_B_TRUE;
			res = st_apev2_fldAdd_text(pTag, frID, pFrIDstr, pAFG->pText);
		} else if (pAFG->pStrArr != NULL && datTp.hasText &&
				st_mtes_strArrTotalStrlenBy(pAFG->pStrArr) > 0) {
			LOC_DBG_ADDF_
			fnd = ST_B_TRUE;
			res = st_apev2_fldAdd_textArr(pTag, frID, pFrIDstr, pAFG->pStrArr);
		}
	}
	if (res == ST_ERR_SUCC && ! fnd && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"couldn't find field to set data for");
	return res;
#	undef LOC_DBG_SETFDAT_
#	undef LOC_DBG_ADDF_
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TAV2__fa_checkBinDatSize(const char *pFnc, Tst_apev2_tag_intn *pTagI,
		Tst_apev2_fldData_intn *pFldI, const Tst_fsize boSize,
		ST_OPTARG(const Tst_str *pFnU8))
{
	const Tst_uint32 cMAX = (Tst_uint32)(ST_APEV2_MAX_AF_FSZ_M * 1024 * 1024);
	char dbgmsg[128];

	if (pFnU8 == NULL)
		snprintf(dbgmsg, sizeof(dbgmsg), "%s", "data");
	else
		snprintf(dbgmsg, sizeof(dbgmsg), "file '%s'", (char*)pFnU8);

	if (boSize > (Tst_fsize)cMAX) {
		st_apev2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
				"%s is too big, "
				"size="ST_TFSIZE_PRF_LU" bytes (%.2f MB), max=%u bytes (%u MB), "
				"aborting",
				dbgmsg,
				(Tst_fsize)boSize, (double)boSize / 1024.0 / 1024.0,
				cMAX, (Tst_uint32)ST_APEV2_MAX_AF_FSZ_M);
		return ST_ERR_ABRT;
	}
	return ST_ERR_SUCC;
}

/*
 * Copy binary data either from pSrcBDat or pSrcFn to pFldI->dataBinDat
 */
static Tst_err
ST_TAV2__fa_setBinDat(const char *pFnc, Tst_apev2_tag_intn *pTagI,
		Tst_apev2_fldData_intn *pFldI,
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
			st_apev2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"file \"%s\" is too big, aborting",
					pFnU8PCh);
			ST_DELPOINT(pFnU8PCh)
			return ST_ERR_ABRT;
		}
	}
	res = ST_TAV2__fa_checkBinDatSize(pFnc, pTagI, pFldI,
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
				st_apev2_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
						"can't copy file '%s'", pFnU8PCh);
		}
	}
	ST_DELPOINT(pFnU8PCh)
	/* */
	if (res == ST_ERR_SUCC &&
			ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		Tst_uint32 tmpDataSz = st_binobj_getDataSize(&pFldI->dataBinDat);

		if (tmpDataSz < 1024)
			st_apev2_d_fdeb(&pTagI->opts, 3, pFnc, pFldI,
					"bin %u bytes", tmpDataSz);
		else if (tmpDataSz < 1024 * 1024)
			st_apev2_d_fdeb(&pTagI->opts, 3, pFnc, pFldI,
					"bin %u bytes (%.2f KB)", tmpDataSz,
					(double)tmpDataSz / 1024.0);
		else
			st_apev2_d_fdeb(&pTagI->opts, 3, pFnc, pFldI,
					"bin %u bytes (%u MB)", tmpDataSz,
					(Tst_uint32)((double)tmpDataSz / (1024.0 * 1024.0)));
	}
	return res;
}

/******************************************************************************/
