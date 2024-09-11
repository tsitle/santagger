/******************************************************************************/
/* tag_id3v1_fnc.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for ID3v1 (Extended) Tags                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_id3v1.h"
/** */
#define SRC_TAG_ID3V1_FNC_C
#include "tag_id3v1_fnc-prot.h"
/*** */
#include "tag_id3v1-pp_flds.h"
#include "../tag_basics-prot.h"
#undef SRC_TAG_ID3V1_FNC_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Search for field properties entry by field ID
 */
Tst_bool
st_id3v1_fnc_isFldPropInArr_byEnum(const Tst_id3v1_fldProp_intn *pFldPrIArr,
		const Tst_id3v1_frID id, const Tst_id3v1ext_frID idExt, Tst_uint32 *pIx)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFldPrIArr == NULL ||
			(id == ST_ID3V1_FID_NONE && idExt == ST_ID3V1E_FID_NONE) ||
			(id != ST_ID3V1_FID_NONE && idExt != ST_ID3V1E_FID_NONE) ||
			pIx == NULL)

	*pIx = 0;
	while (pFldPrIArr[*pIx].pCIDstr != NULL) {
		if ((id != ST_ID3V1_FID_NONE && pFldPrIArr[*pIx].id == id) ||
				(idExt != ST_ID3V1E_FID_NONE && pFldPrIArr[*pIx].idExt == idExt))
			return ST_B_TRUE;
		++(*pIx);
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Strip a string
 */
Tst_err
st_id3v1_fnc_stripStr(Tst_mtes_string *pStr,
		ST_OPTARG(Tst_bool *pHadInvChars), ST_OPTARG(Tst_bool *pHadUmlauts),
		ST_OPTARG(Tst_bool *pWasModifiedByStripping))
{
	Tst_err res;
	Tst_mtes_strRestr restr;

	ST_ASSERTN_IARG(pStr == NULL)

	if (pHadInvChars != NULL)
		*pHadInvChars = ST_B_FALSE;
	if (pHadUmlauts != NULL)
		*pHadUmlauts = ST_B_FALSE;
	if (pWasModifiedByStripping != NULL)
		*pWasModifiedByStripping = ST_B_FALSE;

	res = st_mtes_replaceUmlauts(pStr, pHadUmlauts);

	if (res == ST_ERR_SUCC) {
		restr.hasR        = ST_B_TRUE;  /* we have restrictions */
		restr.ruleAlw     = ST_B_TRUE;  /* mode: allow */
		st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
		restr.disNewln    = ST_B_TRUE;  /* disallow '\n' */
		restr.disTab      = ST_B_TRUE;  /* disallow '\t' */
		restr.disNotPrint = ST_B_TRUE;  /* disallow not-printable chars */
		/* */
		res = st_mtes_stripStr(pStr, &restr,
				pHadInvChars, pWasModifiedByStripping);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

void
st_id3v1_fnc_updAreFldsSet(Tst_id3v1_tag_intn *pTagI)
{
#	define LOC_TESTFLD_(mac_fld)  \
				if (! hasFldSet && pTagI->mac_fld.pObInternal != NULL && \
						((Tst_id3v1_fldData_intn*)pTagI->mac_fld.pObInternal)->isOK) \
					hasFldSet = ST_B_TRUE;
	Tst_bool hasFldSet = ST_B_FALSE;

	LOC_TESTFLD_(fdArt)
	LOC_TESTFLD_(fdAlb)
	LOC_TESTFLD_(fdTit)
	LOC_TESTFLD_(fdCom)
	LOC_TESTFLD_(fdTrk)
	LOC_TESTFLD_(fdYea)
	LOC_TESTFLD_(fdGen)

	st_tagBas_gs_setHasFields(&pTagI->tbas, hasFldSet);
#	undef LOC_TESTFLD_
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_id3v1_fnc_hasIDstrInvalidChars(ST_OPTARG(const Tst_str *pFIDstr),
		ST_OPTARG(Tst_str *pInvalidCharFound))
{
	Tst_bool   resB = ST_B_FALSE;
	Tst_uint32 slen = st_sysStrlen(pFIDstr),
	           x;
	Tst_byte   uch;

	if (pInvalidCharFound != NULL)
		*pInvalidCharFound = 0x00;
	if (pFIDstr == NULL || slen != 4)
		return ST_B_TRUE;

	for (x = 0; x < slen; x++) {
		/* only A-Z and 0-9 allowed  */
		uch = (Tst_byte)pFIDstr[x];
		if (! ((uch >= ST_CSET_ISO_ACAP && uch <= ST_CSET_ISO_ZCAP) ||
					(uch >= ST_CSET_ISO_ASMA && uch <= ST_CSET_ISO_ZSMA))) {
			if (pInvalidCharFound != NULL)
				*pInvalidCharFound = (Tst_str)uch;
			resB = ST_B_TRUE;
			break;
		}
	}
	return resB;
}

/*----------------------------------------------------------------------------*/

Tst_id3v1_frID
st_id3v1_fnc_getID_byIDstr(const Tst_str *pFIDstr)
{
	Tst_id3v1_frID fid = ST_ID3V1_FID_NONE;

	ST_ASSERTN_NUM(ST_ID3V1_FID_NONE, pFIDstr == NULL)

	if (st_sysStrlen2(pFIDstr) != 4)
		return ST_ID3V1_FID_NONE;

	if (st_sysStrcmp2(ST_B_FALSE, pFIDstr, ST_ID3V1_TAG_FIDSTR_ARTI))
		fid = ST_ID3V1_FID_ARTI;
	else if (st_sysStrcmp2(ST_B_FALSE, pFIDstr, ST_ID3V1_TAG_FIDSTR_ALBU))
		fid = ST_ID3V1_FID_ALBU;
	else if (st_sysStrcmp2(ST_B_FALSE, pFIDstr, ST_ID3V1_TAG_FIDSTR_TITL))
		fid = ST_ID3V1_FID_TITL;
	else if (st_sysStrcmp2(ST_B_FALSE, pFIDstr, ST_ID3V1_TAG_FIDSTR_TRKN))
		fid = ST_ID3V1_FID_TRKN;
	else if (st_sysStrcmp2(ST_B_FALSE, pFIDstr, ST_ID3V1_TAG_FIDSTR_YEAR))
		fid = ST_ID3V1_FID_YEAR;
	else if (st_sysStrcmp2(ST_B_FALSE, pFIDstr, ST_ID3V1_TAG_FIDSTR_COMM))
		fid = ST_ID3V1_FID_COMM;
	else if (st_sysStrcmp2(ST_B_FALSE, pFIDstr, ST_ID3V1_TAG_FIDSTR_GENR))
		fid = ST_ID3V1_FID_GENR;

	return fid;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_fnc_getIDstr_byEnum(const Tst_id3v1_frID frID, Tst_str **ppFIDstr)
{
	Tst_err res;

	ST_ASSERTN_IARG(ppFIDstr == NULL)

	switch (frID) {
	case ST_ID3V1_FID_TITL:
		res = st_sysStrcpy2(ST_ID3V1_TAG_FIDSTR_TITL, ppFIDstr);
		break;
	case ST_ID3V1_FID_ALBU:
		res = st_sysStrcpy2(ST_ID3V1_TAG_FIDSTR_ALBU, ppFIDstr);
		break;
	case ST_ID3V1_FID_TRKN:
		res = st_sysStrcpy2(ST_ID3V1_TAG_FIDSTR_TRKN, ppFIDstr);
		break;
	case ST_ID3V1_FID_ARTI:
		res = st_sysStrcpy2(ST_ID3V1_TAG_FIDSTR_ARTI, ppFIDstr);
		break;
	case ST_ID3V1_FID_COMM:
		res = st_sysStrcpy2(ST_ID3V1_TAG_FIDSTR_COMM, ppFIDstr);
		break;
	case ST_ID3V1_FID_GENR:
		res = st_sysStrcpy2(ST_ID3V1_TAG_FIDSTR_GENR, ppFIDstr);
		break;
	case ST_ID3V1_FID_YEAR:
		res = st_sysStrcpy2(ST_ID3V1_TAG_FIDSTR_YEAR, ppFIDstr);
		break;
	default:
		res = st_sysStrcpy2("", ppFIDstr);
	}

	return res;
}


/******************************************************************************/
