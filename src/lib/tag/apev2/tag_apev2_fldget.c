/******************************************************************************/
/* tag_apev2_fldget.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for getting APEv2 Tag fields                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 26.05.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_FLDGET_C
#include "tag_apev2-prot.h"
#include "tag_apev2_fnc-prot.h"
#undef SRC_TAG_APEV2_FLDGET_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_apev2_fldData*
st_apev2_fldGet(Tst_apev2_tag *pTag,
		Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr)
{
	Tst_apev2_fldData      *pItFld  = NULL;
	Tst_apev2_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NULL(pItFldI == NULL)

		if ((frNr == 0 || pItFldI->fnr == frNr) &&
				st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
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
st_apev2_fldGet_dataStrArr(Tst_apev2_tag *pTag,
		Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr, Tst_mtes_strArr *pStrArr)
{
	Tst_apev2_fldData const *pFld;
	Tst_apev2_fldData_intn  *pFldI = NULL;
	Tst_apev2_fldd_dattp    datTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pStrArr == NULL)

	st_mtes_stc_rsetSA(pStrArr);

	st_apev2_stc_rsetDatTp(&datTp);
	st_apev2_fnc_getFieldDataTypes(frID, &datTp);

	if (! datTp.hasText)
		return ST_ERR_TFNF;  /* field not found */

	pFld = st_apev2_fldGet(pTag, frID, pFrIDstr, frNr);
	if (pFld == NULL)
		return ST_ERR_TFNF;  /* field not found */
	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	return st_mtes_strArrCpy(&pFldI->dataSArr, pStrArr);
}

/******************************************************************************/
