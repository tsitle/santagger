/******************************************************************************/
/* tag_id3v2_fldget.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for getting ID3v2 Tag fields                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 15.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_FLDGET_C
#include "tag_id3v2-prot.h"
#include "tag_id3v2_zchk-pp.h"
#include "tag_id3v2_fnc-prot.h"
#undef SRC_TAG_ID3V2_FLDGET_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_id3v2_fldData*
st_id3v2_fldGet(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr)
{
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NULL(pItFldI == NULL)

		if ((frNr == 0 || pItFldI->fnr == frNr) &&
				st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
							frID, (const char*)pFrIDstr))
			return pItFld;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get text data from field
 *   If frNr==0 return data of first valid field with the ID frID
 *
 * pTag      MUST be set
 * pStrArr   MUST be set
 */
Tst_err
st_id3v2_fldGet_dataStrArr(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr, Tst_mtes_strArr *pStrArr)
{
	Tst_id3v2_fldData const *pFld;
	Tst_id3v2_fldData_intn  *pFldI = NULL;
	Tst_id3v2_fldd_dattp    datTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pStrArr == NULL)

	st_mtes_stc_rsetSA(pStrArr);

	frID = st_id3v2_fnc_adjustFrID(frID, pFrIDstr, NULL);
	if (frID == ST_ID3V2_FID_NONE)
		return ST_ERR_IARG;

	st_id3v2_stc_rsetDatTp(&datTp);
	st_id3v2_fnc_getFieldDataTypes(frID, &datTp);

	if (! (datTp.hasText || datTp.hasCTxt))
		return ST_ERR_TFNF;  /* field not found */

	pFld = st_id3v2_fldGet(pTag, frID, pFrIDstr, frNr);
	if (pFld == NULL)
		return ST_ERR_TFNF;  /* field not found */
	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	return st_mtes_strArrCpy(&pFldI->dataSArr, pStrArr);
}

/*
 * Get data from specific field that has 'PosString', like TRCK or TPOS
 *   If frNr==0 return first valid field with the ID frID
 *
 * pTag      MUST be set
 * pNum      MUST be != NULL
 * pNumTot   MUST be != NULL
 */
Tst_err
st_id3v2_fldGet_dataStr_pos(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID frID, const Tst_uint32 frNr,
		Tst_uint32 *pNum, Tst_uint32 *pNumTot)
{
	Tst_id3v2_fldData const      *pFld;
	Tst_id3v2_fldData_intn const *pFldI = NULL;
	Tst_id3v2_fldd_strtp         strTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pNum == NULL || pNumTot == NULL)

	*pNum    = 0;
	*pNumTot = 0;

	frID = st_id3v2_fnc_adjustFrID(frID, NULL, NULL);
	if (frID == ST_ID3V2_FID_NONE)
		return ST_ERR_IARG;

	st_id3v2_stc_rsetStrTp(&strTp);
	st_id3v2_fnc_getFieldStringTypes(frID, &strTp);
	if (! strTp.hasPosStr)
		return ST_ERR_TFNF;  /* field not found */

	pFld = st_id3v2_fldGet(pTag, frID, NULL, frNr);
	if (pFld == NULL)
		return ST_ERR_TFNF;  /* field not found */
	pFldI = (Tst_id3v2_fldData_intn const*)pFld->pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) == 0)
		return ST_ERR_SUCC;
	return st_tagCFnc_chk_getPosStringNums(st_mtes_strArrGetElem(&pFldI->dataSArr, 1),
			pNum, pNumTot);
}

/******************************************************************************/
