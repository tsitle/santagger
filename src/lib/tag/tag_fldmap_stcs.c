/******************************************************************************/
/* tag_fldmap_stcs.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for Generic Tag Field structures                                 */
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
#include "src/includes/common/binobj.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_fldmap.h"
/** */
#define SRC_TAG_FLDMAP_STCS_C
#include "tag_fldmap-prot.h"
#undef SRC_TAG_FLDMAP_STCS_C

/*
// System-Includes
*/
#include <string.h>      /* memset() */
#include <stdlib.h>      /* free() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_tfldMap_genTagFld */
Tst_err
st_tagFldMap_stc_initGTF(Tst_tfldMap_genTagFld *pGTF)
{
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pGTF == NULL)

	ST_CALLOC(pGTFI, Tst_tfldMap_genTagFld_intn*,
			1, sizeof(Tst_tfldMap_genTagFld_intn))
	if (pGTFI == NULL)
		return ST_ERR_OMEM;
	pGTF->pObInternal = pGTFI;

	pGTFI->pFldIDstr = NULL;
	st_mtes_stc_initSA(&pGTFI->dataStrArr);
	st_mtes_stc_initStr(&pGTFI->attrDesc);
	st_binobj_stc_initBO(&pGTFI->dataBin);
	st_mtes_stc_initStr(&pGTFI->attrGeoFilen);
	pGTFI->pAttrGeoMimeISO = NULL;

	st_tagFldMap_stc_rsetGTF(pGTF);
	return ST_ERR_SUCC;
}

void
st_tagFldMap_stc_rsetGTF(Tst_tfldMap_genTagFld *pGTF)
{
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_VOID(pGTF == NULL || pGTF->pObInternal == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pGTF->pObInternal;

	/* */
	pGTFI->modeAddOrSet = ST_B_TRUE;
	pGTFI->ttp          = ST_TFLDMAP_TTP_NONE;
	/* */
	pGTFI->fldID = -1;
	ST_DELPOINT(pGTFI->pFldIDstr)
	/* */
	pGTFI->fldNr = -1;
	/* */
	pGTFI->isDataStrSet  = ST_B_FALSE;
	pGTFI->isDataBinSet  = ST_B_FALSE;
	pGTFI->isDataUValSet = ST_B_FALSE;
	pGTFI->isDataBValSet = ST_B_FALSE;
	st_mtes_stc_rsetSA(&pGTFI->dataStrArr);
	st_binobj_stc_rsetBO(&pGTFI->dataBin);
	pGTFI->dataUVal = 0;
	pGTFI->dataBVal = 0;
	/* */
	pGTFI->isAttrDescSet    = ST_B_FALSE;
	pGTFI->isAttrLangSet    = ST_B_FALSE;
	pGTFI->isAttrGeoFnSet   = ST_B_FALSE;
	pGTFI->isAttrGeoMimeSet = ST_B_FALSE;
	pGTFI->isAttrPicTpSet   = ST_B_FALSE;
	pGTFI->isAttrPicFmtSet  = ST_B_FALSE;
	pGTFI->isAttrSltTsfSet  = ST_B_FALSE;
	pGTFI->isAttrSltCtpSet  = ST_B_FALSE;
	st_mtes_stc_rsetStr(&pGTFI->attrDesc, ST_B_TRUE, ST_B_TRUE);
	memset(pGTFI->attrLangISO, 0, sizeof(pGTFI->attrLangISO));
	st_mtes_stc_rsetStr(&pGTFI->attrGeoFilen, ST_B_TRUE, ST_B_TRUE);
	ST_DELPOINT(pGTFI->pAttrGeoMimeISO)
	pGTFI->attrPicTp  = ST_ID3V2_PT_NONE;
	pGTFI->attrPicFmt = ST_UTILSFMT_PF_NONE;
	pGTFI->attrSltTSF = ST_ID3V2_SLTTSF_NONE;
	pGTFI->attrSltCTP = ST_ID3V2_SLTCTP_NONE;
}

void
st_tagFldMap_stc_freeGTF(Tst_tfldMap_genTagFld *pGTF)
{
	ST_ASSERTN_VOID(pGTF == NULL)

	if (pGTF->pObInternal != NULL) {
		Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

		st_tagFldMap_stc_rsetGTF(pGTF);

		pGTFI = (Tst_tfldMap_genTagFld_intn*)pGTF->pObInternal;
		st_mtes_stc_freeSA(&pGTFI->dataStrArr);
		st_mtes_stc_freeStr(&pGTFI->attrDesc);
		st_binobj_stc_freeBO(&pGTFI->dataBin);
		st_mtes_stc_freeStr(&pGTFI->attrGeoFilen);

		ST_DELPOINT(pGTFI)
	}
}

/******************************************************************************/
