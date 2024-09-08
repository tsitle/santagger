/******************************************************************************/
/* tag_vorbc_fldget.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for getting Vorbis Tag fields                                    */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/string_mte.h>
#include <santag/tag/tag_vorbc.h>
/** */
#define SRC_TAG_VORBC_FLDGET_C
#include "tag_vorbc-prot.h"
#include "tag_vorbc_fnc-prot.h"
#undef SRC_TAG_VORBC_FLDGET_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_vorbc_fldData*
st_vorbc_fldGet(Tst_vorbc_tag *pTag,
		Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr)
{
	Tst_vorbc_fldData      *pItFld  = NULL;
	Tst_vorbc_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NULL(pItFldI == NULL)

		if ((frNr == 0 || pItFldI->fnr == frNr) &&
				st_vorbc_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
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
st_vorbc_fldGet_dataStrArr(Tst_vorbc_tag *pTag,
		Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr, Tst_mtes_strArr *pStrArr)
{
	Tst_vorbc_fldData const *pFld;
	Tst_vorbc_fldData_intn  *pFldI = NULL;
	Tst_vorbc_fldd_dattp    datTp;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pStrArr == NULL)

	st_mtes_stc_rsetSA(pStrArr);

	st_vorbc_stc_rsetDatTp(&datTp);
	st_vorbc_fnc_getFieldDataTypes(frID, &datTp);

	if (! datTp.hasText)
		return ST_ERR_TFNF;  /* field not found */

	pFld = st_vorbc_fldGet(pTag, frID, pFrIDstr, frNr);
	if (pFld == NULL)
		return ST_ERR_TFNF;  /* field not found */
	pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	return st_mtes_strArrCpy(&pFldI->dataSArr, pStrArr);
}

/******************************************************************************/
