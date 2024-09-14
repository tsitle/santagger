/******************************************************************************/
/* tag_apev2_fnc.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for APEv2 Tag handling                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 14.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_FNC_C
#include "tag_apev2_fnc-prot.h"
/*** */
#include "tag_apev2_stcs-prot.h"
#undef SRC_TAG_APEV2_FNC_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
static void
ST_TAV2__fnc_getFirstValidFld_byID(Tst_apev2_tag *pTag,
                                   const Tst_apev2_frID id,
                                   ST_OPTARG(const Tst_str *pIDstr),
                                   ST_OPTARG(Tst_apev2_fldData **ppDat),
                                   ST_OPTARG(Tst_uint32 *pNr));
#endif
static Tst_uint32
ST_TAV2__fnc_getNewFldNr(Tst_apev2_tag *pTag,
                         const Tst_apev2_frID frID,
                         ST_OPTARG(const Tst_str *pFrIDstr));

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
/*
 * Get the field number of the first valid field with the specified ID
 *
 * returns: field nr. or (ST_APEV2_MAX_TFLD_CNT+1) if no
 *          valid field was found
 */
Tst_uint32
st_apev2_fnc_getFirstValidFld_nr_byID(Tst_apev2_tag *pTag,
		const Tst_apev2_frID id, ST_OPTARG(const Tst_str *pIDstr))
{
	Tst_uint32 nr = 0;

	ST_TAV2__fnc_getFirstValidFld_byID(pTag, id, pIDstr, NULL, &nr);
	return nr;
}

/*
 * Get a pointer to the first valid field with the specified ID
 *
 * returns: pointer to field or NULL if no valid field was found
 */
Tst_apev2_fldData*
st_apev2_fnc_getFirstValidFld_pt_byID(Tst_apev2_tag *pTag,
		const Tst_apev2_frID id, ST_OPTARG(const Tst_str *pIDstr))
{
	Tst_apev2_fldData *pDat = NULL;

	ST_TAV2__fnc_getFirstValidFld_byID(pTag, id, pIDstr, &pDat, NULL);
	return pDat;
}
#endif

/*----------------------------------------------------------------------------*/

Tst_bool
st_apev2_fnc_hasIDstrInvalidChars(ST_OPTARG(const Tst_str *pFIDstr),
		ST_OPTARG(Tst_str *pInvalidCharFound))
{
	Tst_bool   resB = ST_B_FALSE;
	Tst_uint32 slen = st_sysStrlen(pFIDstr),
	           x;
	Tst_byte   uch;

	if (pInvalidCharFound != NULL)
		*pInvalidCharFound = 0x00;
	if (pFIDstr == NULL || slen == 0)
		return ST_B_TRUE;

	for (x = 0; x < slen; x++) {
		uch = (Tst_byte)pFIDstr[x];
		if (! (uch >= 0x20 && uch <= 0x7d) || uch == '=') {
			if (pInvalidCharFound != NULL)
				*pInvalidCharFound = (Tst_str)uch;
			resB = ST_B_TRUE;
			break;
		}
	}
	return resB;
}

Tst_apev2_frID
st_apev2_fnc_getID_byIDstr(const Tst_str *pFIDstr, const Tst_apev2_frTp frTyp)
{
	Tst_bool   fnd;
	Tst_uint32 arrIx = 0;
	Tst_apev2_frID fid = ST_APEV2_FID_NONE;

	ST_ASSERTN_NUM(ST_APEV2_FID_NONE, pFIDstr == NULL)

	if (st_apev2_fnc_hasIDstrInvalidChars(pFIDstr, NULL))
		return ST_APEV2_FID_NONE;

	fnd = st_apev2_fnc_isFldPropInArr_byIDstr(ST_APEV2_FLDPROPS,
			pFIDstr, &arrIx);
	if (fnd)
		fid = ST_APEV2_FLDPROPS[arrIx].id;
	else {
		if (frTyp == ST_APEV2_FTP_BDAT)
			fid = ST_APEV2_FID_CBIN;
		else
			fid = ST_APEV2_FID_CTXT;
	}
	return fid;
}

Tst_err
st_apev2_fnc_getIDstr_byEnum(Tst_apev2_frID frID, Tst_str **ppFIDstr)
{
	Tst_apev2_fldProp_intn *pFldPrI = NULL;

	ST_ASSERTN_IARG(ppFIDstr == NULL)

	/* adjust frID */
	frID = st_apev2_fnc_adjustFrID(frID, NULL, &pFldPrI);
	if (frID == ST_APEV2_FID_NONE) {
		Tst_err res;

		res = st_sysStrcpy2("", ppFIDstr);
		if (res == ST_ERR_SUCC)
			res = ST_ERR_IARG;
		return res;
	}

	return st_sysStrcpy2(pFldPrI->pCIDstr, ppFIDstr);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_fnc_getFieldDataTypes(const Tst_apev2_frID frID,
		Tst_apev2_fldd_dattp *pDatTp)
{
	Tst_apev2_fldd_strtp tpStr;

	ST_ASSERTN_IARG(pDatTp == NULL)

	st_apev2_stc_rsetStrTp(&tpStr);

	return st_apev2_fnc_setDataNstrTp(frID, pDatTp, &tpStr);
}

Tst_err
st_apev2_fnc_getFieldStringTypes(const Tst_apev2_frID frID,
		Tst_apev2_fldd_strtp *pStrTp)
{
	Tst_apev2_fldd_dattp tpDat;

	ST_ASSERTN_IARG(pStrTp == NULL)

	st_apev2_stc_rsetDatTp(&tpDat);

	return st_apev2_fnc_setDataNstrTp(frID, &tpDat, pStrTp);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Is frame ID in array ?
 *
 * pFldPrIArr  MUST be set
 * pIDstr      MUST be set
 *
 * returns: true if found
 */
Tst_bool
st_apev2_fnc_isFldPropInArr_byIDstr(const Tst_apev2_fldProp_intn *pFldPrIArr,
		const Tst_str *pIDstr, ST_OPTARG(Tst_uint32 *pIx))
{
	Tst_uint32 iix,
	           *pIIx = &iix;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFldPrIArr == NULL || pIDstr == NULL)

	/* the ID string is unique */

	if (pIx != NULL)
		pIIx = pIx;
	*pIIx = 0;
	if (st_sysStrEmpty(pIDstr))
		return ST_B_FALSE;
	while (pFldPrIArr[*pIIx].pCIDstr != NULL) {
		if (st_sysStrcmp2(ST_B_FALSE, pFldPrIArr[*pIIx].pCIDstr, pIDstr))
			return (pFldPrIArr[*pIIx].id != ST_APEV2_FID_NONE);
		++(*pIIx);
	}
	return ST_B_FALSE;
}

Tst_bool
st_apev2_fnc_isFldPropInArr_byEnum(const Tst_apev2_fldProp_intn *pFldPrIArr,
		const Tst_apev2_frID id, ST_OPTARG(Tst_uint32 *pIx))
{
	Tst_uint32 iix,
	           *pIIx = &iix;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFldPrIArr == NULL)

	if (pIx != NULL)
		pIIx = pIx;
	*pIIx = 0;
	if (id == ST_APEV2_FID_NONE)
		return ST_B_FALSE;
	while (pFldPrIArr[*pIIx].id != ST_APEV2_FID_NONE) {
		if (pFldPrIArr[*pIIx].id == id)
			return ST_B_TRUE;
		++(*pIIx);
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Get field props
 */
Tst_err
st_apev2_fnc_getFldProps(const Tst_str *pFIDstr,
		const Tst_bool binOrU8, Tst_apev2_fldProp_intn *pFldPrI)
{
	Tst_err    res;
	Tst_uint32 ix = 0;
	Tst_bool   fnd;

	ST_ASSERTN_IARG(pFIDstr == NULL || pFldPrI == NULL)

	fnd = st_apev2_fnc_isFldPropInArr_byIDstr(ST_APEV2_FLDPROPS, pFIDstr, &ix);
	if (! fnd) {
		fnd = st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS,
				(binOrU8 ? ST_APEV2_FID_CBIN : ST_APEV2_FID_CTXT), &ix);
		if (! fnd)
			return ST_ERR_FAIL;
	}

	/* copy field properties */
	res = st_apev2_stc_copyFPr_i(pFIDstr,
				(const Tst_apev2_fldProp_intn*)&ST_APEV2_FLDPROPS[ix],
				pFldPrI);
	if (res != ST_ERR_SUCC)
		return res;

	/* set/correct field type */
	if (binOrU8 && pFldPrI->typ != ST_APEV2_FTP_BDAT)
		pFldPrI->typ = ST_APEV2_FTP_BDAT;
	else if (! binOrU8 && pFldPrI->typ == ST_APEV2_FTP_BDAT)
		pFldPrI->typ = ST_APEV2_FTP_STR;
	if (pFldPrI->typ == ST_APEV2_FTP_NONE)
		pFldPrI->typ = ST_APEV2_FTP_STR;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Adjust the frame ID
 *   If frID is ST_APEV2_FID_CTXT or ST_APEV2_FID_CBIN
 *     then an ID-String must be given in pFrIDstr
 *   If pFrIDstr contains the name of a custom field
 *     then frID should be either ST_APEV2_FID_CTXT or ST_APEV2_FID_CBIN,
 *     if not frID will be set to ST_APEV2_FID_CTXT
 *
 * returns: the 'adjusted' frame ID
 *          or ST_APEV2_FID_NONE if frID/pFrIDstr is a custum one (see above) and
 *             no ID-String/ID was given
 */
Tst_apev2_frID
st_apev2_fnc_adjustFrID(Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(Tst_apev2_fldProp_intn **ppFldPrI))
{
	Tst_uint32 ix      = 0;
	Tst_bool   isInArr = ST_B_FALSE,
	           isEmpty = st_sysStrEmpty(pFrIDstr);

	if (ppFldPrI != NULL)
		*ppFldPrI = NULL;

	/* search by ID-String */
	if (isEmpty)
		frID = ST_APEV2_FID_NONE;
	if (! isEmpty) {
		isInArr = st_apev2_fnc_isFldPropInArr_byIDstr(ST_APEV2_FLDPROPS,
				pFrIDstr, &ix);
		if (isInArr)
			frID = ST_APEV2_FLDPROPS[ix].id;
		else {
			if (frID != ST_APEV2_FID_CBIN)
				frID = ST_APEV2_FID_CTXT;
			isInArr = st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS,
					frID, &ix);
		}
	}
	/* search by numeric ID */
	if (! isInArr && frID != ST_APEV2_FID_NONE) {
		isInArr = st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS,
				frID, &ix);
		if (! isInArr)
			frID = ST_APEV2_FID_NONE;
	}

	if (ppFldPrI != NULL) {
		if (isInArr)
			*ppFldPrI = (Tst_apev2_fldProp_intn*)&ST_APEV2_FLDPROPS[ix];
	}
	return frID;
}

/*----------------------------------------------------------------------------*/

/*
 * Match two IDs
 */
Tst_bool
st_apev2_fnc_matchFrIDs(Tst_apev2_frID frID1, ST_OPTARG(const char *pFrID1str),
		Tst_apev2_frID frID2, ST_OPTARG(const char *pFrID2str))
{
	frID1 = st_apev2_fnc_adjustFrID(frID1, (const Tst_str*)pFrID1str, NULL);
	frID2 = st_apev2_fnc_adjustFrID(frID2, (const Tst_str*)pFrID2str, NULL);

	return (frID1 == frID2 &&
				(frID1 == ST_APEV2_FID_CBIN ||
					frID1 != ST_APEV2_FID_CTXT ||
					(frID1 != ST_APEV2_FID_NONE &&
						pFrID1str != NULL && pFrID2str != NULL &&
						st_sysStrcmp2(ST_B_FALSE, pFrID1str, pFrID2str))));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Creates and adds a new element to the field list
 */
Tst_err
st_apev2_fnc_addElemToFlds(Tst_apev2_tag *pTag,
		const Tst_apev2_frID srcFrID, ST_OPTARG(const Tst_str *pSrcFKey),
		const Tst_bool isBinOrU8,
		Tst_bool *pSkipped, Tst_apev2_fldData **ppNewFld)
{
	Tst_err       res;
	Tst_uint32    ix      = 0;
	Tst_bool      fnd     = ST_B_FALSE;
	Tst_str const *pIDstr = NULL;
	Tst_apev2_tag_intn     *pTagI;
	Tst_apev2_fldData_intn *pNFldI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			ppNewFld == NULL || pSkipped == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	*pSkipped = ST_B_FALSE;
	*ppNewFld = NULL;

	/* add new field and init field data */
	/** */
	ST_CALLOC(*ppNewFld, Tst_apev2_fldData*, 1, sizeof(Tst_apev2_fldData))
	if (*ppNewFld == NULL)
		return ST_ERR_OMEM;
	res = st_apev2_stc_initFDat(*ppNewFld);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(*ppNewFld)
		return res;
	}
	/** */
	pNFldI           = (Tst_apev2_fldData_intn*)(*ppNewFld)->pObInternal;
	pNFldI->uniqueIx = pTagI->flds.uniqueIxLast++;
	/** */
	res = st_dynlist_addElement(&pTagI->flds.list, *ppNewFld);
	if (res != ST_ERR_SUCC) {
		st_apev2_stc_freeFDat(*ppNewFld);
		ST_DELPOINT(*ppNewFld)
		return res;
	}
	/** copy field properties */
	if (srcFrID != ST_APEV2_FID_NONE)
		fnd = st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS, srcFrID, &ix);
	else if (pSrcFKey != NULL)
		fnd = st_apev2_fnc_isFldPropInArr_byIDstr(ST_APEV2_FLDPROPS, pSrcFKey, &ix);
	if (fnd) {
		if (pSrcFKey != NULL)
			pIDstr = pSrcFKey;
		else
			pIDstr = (const Tst_str*)ST_APEV2_FLDPROPS[ix].pCIDstr;
	} else {
		if (pSrcFKey == NULL)
			return ST_ERR_IARG;
		fnd = st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS,
				(isBinOrU8 ? ST_APEV2_FID_CBIN : ST_APEV2_FID_CTXT), &ix);
		if (! fnd)
			return ST_ERR_FAIL;
		pIDstr = pSrcFKey;
	}
	res = st_apev2_stc_copyFPr_i(pIDstr,
			(const Tst_apev2_fldProp_intn*)&ST_APEV2_FLDPROPS[ix],
			&pNFldI->fldPrI);
	if (res != ST_ERR_SUCC)
		return res;

	/* set field number, minimum number is 1 */
	pNFldI->fnr = ST_TAV2__fnc_getNewFldNr(pTag,
				pNFldI->fldPrI.id, (const Tst_str*)pNFldI->fldPrI.pVIDstr);
	if (pNFldI->fnr == 0 || pNFldI->fnr > ST_APEV2_MAX_TFLDID_CNT)
		*pSkipped = ST_B_TRUE;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_fnc_moveFld(Tst_apev2_tag *pTag, const Tst_uint32 curIx,
		const Tst_uint32 newIx)
{
	Tst_uint32 uix = 1;
	Tst_dynlist        *pDL;
	Tst_apev2_fldData  *pItFld;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			curIx == 0 || newIx == 0)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	/**st_apev2_prf(">>>>st_apev2_fnc_moveFld: %u-->%u\n",
			curIx, newIx);**/

	pDL = &pTagI->flds.list;
	/* move element */
	if (! st_dynlist_moveElement(pDL, curIx, newIx))
		return ST_ERR_FAIL;
	/* update uniqueIx */
	/** */
	if (newIx > 1) {
		if (! st_dynlist_ite_goto(pDL, newIx - 1))
			return ST_ERR_FAIL;
		pItFld = (Tst_apev2_fldData*)st_dynlist_ite_getCurrent(pDL);
		ST_ASSERTN_NUM(ST_ERR_FAIL,
				pItFld == NULL || pItFld->pObInternal == NULL)

		uix = ((Tst_apev2_fldData_intn*)pItFld->pObInternal)->uniqueIx + 1;
		if (! st_dynlist_ite_gotoNext(pDL))
			return ST_ERR_FAIL;
	} else if (! st_dynlist_ite_gotoFirst(pDL))
		return ST_ERR_FAIL;
	/** */
	do {
		pItFld = (Tst_apev2_fldData*)st_dynlist_ite_getCurrent(pDL);
		ST_ASSERTN_NUM(ST_ERR_FAIL,
				pItFld == NULL || pItFld->pObInternal == NULL)

		((Tst_apev2_fldData_intn*)pItFld->pObInternal)->uniqueIx = uix++;
	} while (st_dynlist_ite_gotoNext(pDL));
	pTagI->flds.uniqueIxLast = uix;

	/* DEBUG *
	st_dynlist_ite_gotoFirst(pDL);
	do {
		pItFld = (Tst_apev2_fldData*)st_dynlist_ite_getCurrent(pDL);

		st_apev2_prf(">>> uix %u\n",
				((Tst_apev2_fldData_intn*)pItFld->pObInternal)->uniqueIx);
	} while (st_dynlist_ite_gotoNext(pDL));
	st_apev2_prf(">>> lastuix %u\n",
				pTagI->flds.uniqueIxLast);
	* DEBUG */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*Tst_apev2_frTp
st_apev2_fnc_getFrameType_byID(const Tst_apev2_frID frID)
{
	Tst_str const *pFrIDstr = NULL;
	Tst_apev2_fldProp_intn *pFldPrI = NULL;

	if (frID == ST_APEV2_FID_CTXT)
		pFrIDstr = (Tst_str const*)"CUSTOMTXT";
	else if (frID == ST_APEV2_FID_CBIN)
		pFrIDstr = (Tst_str const*)"CUSTOMBIN";

	if (st_apev2_fnc_adjustFrID(frID, pFrIDstr, &pFldPrI) == ST_APEV2_FID_NONE)
		return ST_APEV2_FTP_NONE;

	return pFldPrI->typ;
}*/

/*----------------------------------------------------------------------------*/

Tst_err
st_apev2_fnc_setDataNstrTp(const Tst_apev2_frID frID,
		Tst_apev2_fldd_dattp *pTpDat, Tst_apev2_fldd_strtp *pTpStr)
{
	Tst_bool   fndArr;
	Tst_uint32 arrIx = 0;

	ST_ASSERTN_IARG(pTpDat == NULL || pTpStr == NULL)

	/* get frame type */
	fndArr = st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS,
			frID, &arrIx);
	if (! fndArr)
		return ST_ERR_IARG;

	/* init some stuff */
	st_apev2_stc_rsetDatTp(pTpDat);
	st_apev2_stc_rsetStrTp(pTpStr);

	switch (ST_APEV2_FLDPROPS[arrIx].typ) {
	case ST_APEV2_FTP_BDAT:
		pTpDat->hasBDat = ST_B_TRUE;
		break;
	case ST_APEV2_FTP_INT:
	case ST_APEV2_FTP_FLT:
	case ST_APEV2_FTP_TSTA:
	case ST_APEV2_FTP_LYR:
	case ST_APEV2_FTP_URL:
	case ST_APEV2_FTP_STR:
		pTpDat->hasText = ST_B_TRUE;
		/* */
		switch (ST_APEV2_FLDPROPS[arrIx].typ) {
		case ST_APEV2_FTP_INT:
			pTpStr->hasNumStr = ST_B_TRUE;
			break;
		case ST_APEV2_FTP_YEAR:
			pTpStr->hasYearStr = ST_B_TRUE;
			break;
		case ST_APEV2_FTP_TSTA:
			pTpStr->hasTStampStr = ST_B_TRUE;
			break;
		case ST_APEV2_FTP_URL:
			pTpStr->hasUrlStr = ST_B_TRUE;
			break;
		default:
			switch (frID) {
			case ST_APEV2_FID_COMM:
			case ST_APEV2_FID_COPY:
			case ST_APEV2_FID_PUBR:
			case ST_APEV2_FID_ABST:
			case ST_APEV2_FID_BIBL:
				pTpStr->hasFullStr = ST_B_TRUE; break;
			case ST_APEV2_FID_GENR:
				pTpStr->hasGenreStr = ST_B_TRUE; break;
			default:
				pTpStr->hasSimpleStr = ST_B_TRUE;
			}
		}
		break;
	default:  /* ST_APEV2_FTP_NONE */
		return ST_ERR_IARG;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
static void
ST_TAV2__fnc_getFirstValidFld_byID(Tst_apev2_tag *pTag,
		const Tst_apev2_frID id, ST_OPTARG(const Tst_str *pIDstr),
		ST_OPTARG(Tst_apev2_fldData **ppDat), ST_OPTARG(Tst_uint32 *pNr))
{
	Tst_uint32 inr,
	           *pINr = &inr;
	Tst_apev2_fldData      *pItFld = NULL;
	Tst_apev2_fldData_intn *pItFldI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL ||
			(ppDat == NULL && pNr == NULL))

	if (ppDat != NULL)
		*ppDat = NULL;
	if (pNr != NULL)
		pINr = pNr;
	*pINr = ST_APEV2_MAX_TFLD_CNT + 1;
	if (id == ST_APEV2_FID_NONE && pIDstr == NULL)
		return;

	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_VOID(pItFldI == NULL)

		if (pItFldI->fnr < *pINr &&
				st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
						id, (const char*)pIDstr)) {
			*pINr = pItFldI->fnr;
			if (ppDat != NULL)
				*ppDat = pItFld;
		}
	}
}
#endif

/*----------------------------------------------------------------------------*/

/*
 * Get field number (amount of fields with the same ID) for a new field
 *
 * pTag   MUST be set
 *
 * returns: number of field
 *          or 0 on failure
 */
static Tst_uint32
ST_TAV2__fnc_getNewFldNr(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
	Tst_uint32 nr = 0;
	Tst_apev2_fldData      *pItFld  = NULL;
	Tst_apev2_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_apev2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(0, pItFldI == NULL)

		if (st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id,
					pItFldI->fldPrI.pVIDstr,
					frID, (const char*)pFrIDstr))
			++nr;
	}
	return nr;
}

/******************************************************************************/
