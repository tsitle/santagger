/******************************************************************************/
/* tag_vorbc_fnc.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for Vorbis Tag handling                                      */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_TAG_VORBC_FNC_C
#include "tag_vorbc_fnc-prot.h"
/*** */
#include "tag_vorbc_stcs-prot.h"
#undef SRC_TAG_VORBC_FNC_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
static void
ST_TVORBC__fnc_getFirstValidFld_byID(Tst_vorbc_tag *pTag,
                                     const Tst_vorbc_frID id,
                                     ST_OPTARG(const Tst_str *pIDstr),
                                     ST_OPTARG(Tst_vorbc_fldData **ppDat),
                                     ST_OPTARG(Tst_uint32 *pNr));
#endif
static Tst_uint32
ST_TVORBC__fnc_getNewFldNr(Tst_vorbc_tag *pTag,
                           const Tst_vorbc_frID frID,
                           ST_OPTARG(const Tst_str *pFrIDstr));

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
/*
 * Get the field number of the first valid field with the specified ID
 *
 * returns: field nr. or (ST_VORBC_MAX_TFLD_CNT+1) if no
 *          valid field was found
 */
Tst_uint32
st_vorbc_fnc_getFirstValidFld_nr_byID(Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID id, ST_OPTARG(const Tst_str *pIDstr))
{
	Tst_uint32 nr = 0;

	ST_TVORBC__fnc_getFirstValidFld_byID(pTag, id, pIDstr, NULL, &nr);
	return nr;
}

/*
 * Get a pointer to the first valid field with the specified ID
 *
 * returns: pointer to field or NULL if no valid field was found
 */
Tst_vorbc_fldData*
st_vorbc_fnc_getFirstValidFld_pt_byID(Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID id, ST_OPTARG(const Tst_str *pIDstr))
{
	Tst_vorbc_fldData *pDat = NULL;

	ST_TVORBC__fnc_getFirstValidFld_byID(pTag, id, pIDstr, &pDat, NULL);
	return pDat;
}
#endif

/*----------------------------------------------------------------------------*/

Tst_bool
st_vorbc_fnc_hasIDstrInvalidChars(ST_OPTARG(const Tst_str *pFIDstr),
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

Tst_vorbc_frID
st_vorbc_fnc_getID_byIDstr(const Tst_str *pFIDstr)
{
	ST_ASSERTN_NUM(ST_VORBC_FID_NONE, pFIDstr == NULL)

	if (st_vorbc_fnc_hasIDstrInvalidChars(pFIDstr, NULL))
		return ST_VORBC_FID_NONE;

	return st_vorbc_fnc_adjustFrID(ST_VORBC_FID_NONE, pFIDstr, NULL);
}

Tst_err
st_vorbc_fnc_getIDstr_byEnum(Tst_vorbc_frID frID, Tst_str **ppFIDstr)
{
	Tst_vorbc_fldProp_intn *pFldPrI = NULL;

	ST_ASSERTN_IARG(ppFIDstr == NULL)

	/* adjust frID */
	frID = st_vorbc_fnc_adjustFrID(frID, NULL, &pFldPrI);
	if (frID == ST_VORBC_FID_NONE) {
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
st_vorbc_fnc_getFieldDataTypes(const Tst_vorbc_frID frID,
		Tst_vorbc_fldd_dattp *pDatTp)
{
	Tst_vorbc_fldd_strtp tpStr;

	ST_ASSERTN_IARG(pDatTp == NULL)

	st_vorbc_stc_rsetStrTp(&tpStr);

	return st_vorbc_fnc_setDataNstrTp(frID, pDatTp, &tpStr);
}

Tst_err
st_vorbc_fnc_getFieldStringTypes(const Tst_vorbc_frID frID,
		Tst_vorbc_fldd_strtp *pStrTp)
{
	Tst_vorbc_fldd_dattp tpDat;

	ST_ASSERTN_IARG(pStrTp == NULL)

	st_vorbc_stc_rsetDatTp(&tpDat);

	return st_vorbc_fnc_setDataNstrTp(frID, &tpDat, pStrTp);
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
st_vorbc_fnc_isFldPropInArr_byIDstr(const Tst_vorbc_fldProp_intn *pFldPrIArr,
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
			return (pFldPrIArr[*pIIx].id != ST_VORBC_FID_NONE);
		++(*pIIx);
	}
	return ST_B_FALSE;
}

Tst_bool
st_vorbc_fnc_isFldPropInArr_byEnum(const Tst_vorbc_fldProp_intn *pFldPrIArr,
		const Tst_vorbc_frID id, ST_OPTARG(Tst_uint32 *pIx))
{
	Tst_uint32 iix,
	           *pIIx = &iix;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFldPrIArr == NULL)

	if (pIx != NULL)
		pIIx = pIx;
	*pIIx = 0;
	if (id == ST_VORBC_FID_NONE)
		return ST_B_FALSE;
	while (pFldPrIArr[*pIIx].id != ST_VORBC_FID_NONE) {
		if (pFldPrIArr[*pIIx].id == id)
			return ST_B_TRUE;
		++(*pIIx);
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Adjust the frame ID
 *   If frID is ST_VORBC_FID_CTXT
 *   then an ID-String must be given in pFrIDstr
 *
 * returns: the 'adjusted' frame ID
 *          or ST_VORBC_FID_NONE if frID is a custum one (see above) and
 *             no ID-String was given
 */
Tst_vorbc_frID
st_vorbc_fnc_adjustFrID(Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(Tst_vorbc_fldProp_intn **ppFldPrI))
{
	Tst_uint32 ix      = 0;
	Tst_bool   isInArr = ST_B_FALSE,
	           isEmpty = st_sysStrEmpty(pFrIDstr);

	if (ppFldPrI != NULL)
		*ppFldPrI = NULL;

	/* search by ID-String */
	if (isEmpty && frID == ST_VORBC_FID_CTXT)
		frID = ST_VORBC_FID_NONE;
	if (! isEmpty) {
		isInArr = st_vorbc_fnc_isFldPropInArr_byIDstr(ST_VORBC_FLDPROPS,
				pFrIDstr, &ix);
		if (isInArr)
			frID = ST_VORBC_FLDPROPS[ix].id;
		else {
			frID    = ST_VORBC_FID_CTXT;
			isInArr = st_vorbc_fnc_isFldPropInArr_byEnum(ST_VORBC_FLDPROPS,
					frID, &ix);
		}
	}
	/* search by numeric ID */
	if (! isInArr && frID != ST_VORBC_FID_NONE) {
		isInArr = st_vorbc_fnc_isFldPropInArr_byEnum(ST_VORBC_FLDPROPS,
					frID, &ix);
		if (! isInArr)
			frID = ST_VORBC_FID_NONE;
	}

	if (ppFldPrI != NULL) {
		if (isInArr)
			*ppFldPrI = (Tst_vorbc_fldProp_intn*)&ST_VORBC_FLDPROPS[ix];
	}
	return frID;
}

/*----------------------------------------------------------------------------*/

/*
 * Match two IDs
 */
Tst_bool
st_vorbc_fnc_matchFrIDs(Tst_vorbc_frID frID1, ST_OPTARG(const char *pFrID1str),
		Tst_vorbc_frID frID2, ST_OPTARG(const char *pFrID2str))
{
	frID1 = st_vorbc_fnc_adjustFrID(frID1, (const Tst_str*)pFrID1str, NULL);
	frID2 = st_vorbc_fnc_adjustFrID(frID2, (const Tst_str*)pFrID2str, NULL);

	return (frID1 == frID2 &&
				(frID1 != ST_VORBC_FID_CTXT ||
					(frID1 != ST_VORBC_FID_NONE &&
						pFrID1str != NULL && pFrID2str != NULL &&
						st_sysStrcmp2(ST_B_FALSE, pFrID1str, pFrID2str))));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Creates and adds a new element to the field list
 */
Tst_err
st_vorbc_fnc_addElemToFlds(Tst_vorbc_tag *pTag,
		Tst_vorbc_frID srcFrID, ST_OPTARG(const Tst_str *pSrcFKey),
		Tst_bool *pSkipped,
		Tst_vorbc_fldData **ppNewFld)
{
	Tst_err       res;
	Tst_str const *pIDstr = NULL;
	Tst_vorbc_tag_intn     *pTagI;
	Tst_vorbc_fldData_intn *pNFldI;
	Tst_vorbc_fldProp_intn *pFldPrIsrc = NULL;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			ppNewFld == NULL || pSkipped == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

	*pSkipped = ST_B_FALSE;
	*ppNewFld = NULL;

	/* adjust frID */
	srcFrID = st_vorbc_fnc_adjustFrID(srcFrID, pSrcFKey, &pFldPrIsrc);
	if (srcFrID == ST_VORBC_FID_NONE)
		return ST_ERR_IARG;

	/* add new field and init field data */
	/** */
	ST_CALLOC(*ppNewFld, Tst_vorbc_fldData*, 1, sizeof(Tst_vorbc_fldData))
	if (*ppNewFld == NULL)
		return ST_ERR_OMEM;
	res = st_vorbc_stc_initFDat(*ppNewFld);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(*ppNewFld)
		return res;
	}
	/** */
	pNFldI           = (Tst_vorbc_fldData_intn*)(*ppNewFld)->pObInternal;
	pNFldI->uniqueIx = pTagI->flds.uniqueIxLast++;
	/** */
	res = st_dynlist_addElement(&pTagI->flds.list, *ppNewFld);
	if (res != ST_ERR_SUCC) {
		st_vorbc_stc_freeFDat(*ppNewFld);
		ST_DELPOINT(*ppNewFld)
		return res;
	}
	/** copy field properties */
	if (srcFrID == ST_VORBC_FID_CTXT)
		pIDstr = pSrcFKey;
	res = st_vorbc_stc_copyFPr_i(pIDstr, pFldPrIsrc, &pNFldI->fldPrI);
	if (res != ST_ERR_SUCC)
		return res;

	/* set field number, minimum number is 1 */
	pNFldI->fnr = ST_TVORBC__fnc_getNewFldNr(pTag,
				pNFldI->fldPrI.id, (const Tst_str*)pNFldI->fldPrI.pVIDstr);
	if (pNFldI->fnr == 0 || pNFldI->fnr > ST_VORBC_MAX_TFLDID_CNT)
		*pSkipped = ST_B_TRUE;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_vorbc_fnc_moveFld(Tst_vorbc_tag *pTag, const Tst_uint32 curIx,
		const Tst_uint32 newIx)
{
	Tst_uint32 uix = 1;
	Tst_dynlist        *pDL;
	Tst_vorbc_fldData  *pItFld;
	Tst_vorbc_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			curIx == 0 || newIx == 0)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

	/**st_vorbc_prf(">>>>st_vorbc_fnc_moveFld: %u-->%u\n",
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
		pItFld = (Tst_vorbc_fldData*)st_dynlist_ite_getCurrent(pDL);
		ST_ASSERTN_NUM(ST_ERR_FAIL,
				pItFld == NULL || pItFld->pObInternal == NULL)

		uix = ((Tst_vorbc_fldData_intn*)pItFld->pObInternal)->uniqueIx + 1;
		if (! st_dynlist_ite_gotoNext(pDL))
			return ST_ERR_FAIL;
	} else if (! st_dynlist_ite_gotoFirst(pDL))
		return ST_ERR_FAIL;
	/** */
	do {
		pItFld = (Tst_vorbc_fldData*)st_dynlist_ite_getCurrent(pDL);
		ST_ASSERTN_NUM(ST_ERR_FAIL,
				pItFld == NULL || pItFld->pObInternal == NULL)

		((Tst_vorbc_fldData_intn*)pItFld->pObInternal)->uniqueIx = uix++;
	} while (st_dynlist_ite_gotoNext(pDL));
	pTagI->flds.uniqueIxLast = uix;

	/* DEBUG *
	st_dynlist_ite_gotoFirst(pDL);
	do {
		pItFld = (Tst_vorbc_fldData*)st_dynlist_ite_getCurrent(pDL);

		st_vorbc_prf(">>> uix %u\n",
				((Tst_vorbc_fldData_intn*)pItFld->pObInternal)->uniqueIx);
	} while (st_dynlist_ite_gotoNext(pDL));
	st_vorbc_prf(">>> lastuix %u\n",
				pTagI->flds.uniqueIxLast);
	* DEBUG */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*Tst_vorbc_frTp
st_vorbc_fnc_getFrameType_byID(const Tst_vorbc_frID frID)
{
	Tst_str const *pFrIDstr = NULL;
	Tst_vorbc_fldProp_intn *pFldPrI = NULL;

	if (frID == ST_VORBC_FID_CTXT)
		pFrIDstr = (Tst_str const*)"CUSTOMTXT";

	if (st_vorbc_fnc_adjustFrID(frID, pFrIDstr, &pFldPrI) == ST_VORBC_FID_NONE)
		return ST_VORBC_FTP_NONE;

	return pFldPrI->typ;
}*/

/*----------------------------------------------------------------------------*/

Tst_err
st_vorbc_fnc_setDataNstrTp(const Tst_vorbc_frID frID,
		Tst_vorbc_fldd_dattp *pTpDat, Tst_vorbc_fldd_strtp *pTpStr)
{
	Tst_bool   fndArr;
	Tst_uint32 arrIx = 0;

	ST_ASSERTN_IARG(pTpDat == NULL || pTpStr == NULL)

	/* get frame type */
	fndArr = st_vorbc_fnc_isFldPropInArr_byEnum(ST_VORBC_FLDPROPS,
			frID, &arrIx);
	if (! fndArr)
		return ST_ERR_IARG;
	/* init some stuff */
	st_vorbc_stc_rsetDatTp(pTpDat);
	st_vorbc_stc_rsetStrTp(pTpStr);

	switch (ST_VORBC_FLDPROPS[arrIx].typ) {
	case ST_VORBC_FTP_PICT:
		pTpDat->hasBDat = ST_B_TRUE;
		pTpDat->hasPict = ST_B_TRUE;
		break;
	case ST_VORBC_FTP_STR:
		pTpDat->hasText = ST_B_TRUE;
		/* */
		switch (frID) {
		case ST_VORBC_FID_RECD:
			pTpStr->hasTStampStr = ST_B_TRUE; break;
		case ST_VORBC_FID_COPY:
		case ST_VORBC_FID_LICS:
		case ST_VORBC_FID_DESC:
		case ST_VORBC_FID_LYRI:
			pTpStr->hasFullStr = ST_B_TRUE; break;
		case ST_VORBC_FID_GENR:
			pTpStr->hasGenreStr = ST_B_TRUE; break;
		case ST_VORBC_FID_TRKN:
		case ST_VORBC_FID_TRKT:
		case ST_VORBC_FID_DSCN:
		case ST_VORBC_FID_DSCT:
		case ST_VORBC_FID_BPMI:
		case ST_VORBC_FID_TMPO:
			pTpStr->hasNumStr = ST_B_TRUE; break;
		default:
			pTpStr->hasSimpleStr = ST_B_TRUE;
		}
		break;
	default:  /* ST_VORBC_FTP_NONE */
		return ST_ERR_IARG;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
static void
ST_TVORBC__fnc_getFirstValidFld_byID(Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID id, ST_OPTARG(const Tst_str *pIDstr),
		ST_OPTARG(Tst_vorbc_fldData **ppDat), ST_OPTARG(Tst_uint32 *pNr))
{
	Tst_uint32 inr,
	           *pINr = &inr;
	Tst_vorbc_fldData      *pItFld = NULL;
	Tst_vorbc_fldData_intn *pItFldI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL ||
			(ppDat == NULL && pNr == NULL))

	if (ppDat != NULL)
		*ppDat = NULL;
	if (pNr != NULL)
		pINr = pNr;
	*pINr = ST_VORBC_MAX_TFLD_CNT + 1;
	if (id == ST_VORBC_FID_NONE && pIDstr == NULL)
		return;

	while ((pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_VOID(pItFldI == NULL)

		if (pItFldI->fnr < *pINr &&
				st_vorbc_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
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
ST_TVORBC__fnc_getNewFldNr(Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
	Tst_uint32 nr = 0;
	Tst_vorbc_fldData      *pItFld  = NULL;
	Tst_vorbc_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_vorbc_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(0, pItFldI == NULL)

		if (st_vorbc_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
					frID, (const char*)pFrIDstr))
			++nr;
	}
	return nr;
}

/******************************************************************************/
