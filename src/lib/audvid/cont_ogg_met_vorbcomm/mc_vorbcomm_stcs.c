/******************************************************************************/
/* mc_vorbcomm_stcs.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Vorbis Comment structures                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_MC_VORBCOMM_STCS_C
#include "mc_vorbcomm-prot.h"
#undef SRC_MC_VORBCOMM_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_vorbCommentRaw */
Tst_err
st_contOgg_stc_initVorbCommRaw(Tst_contOgg_vorbCommentRaw *pVCR)
{
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_IARG(pVCR == NULL)

	ST_CALLOC(pVCRI, Tst_contOgg_vorbCmtRaw_intn*,
			1, sizeof(Tst_contOgg_vorbCmtRaw_intn))
	if (pVCRI == NULL)
		return ST_ERR_OMEM;
	pVCR->pObInternal = pVCRI;

	ST_CALLOC(pVCRI->pRawCmtBO, Tst_binobj*, 1, sizeof(Tst_binobj))
	if (pVCRI->pRawCmtBO == NULL)
		return ST_ERR_OMEM;
	st_binobj_stc_initBO(pVCRI->pRawCmtBO);
	pVCRI->pVendor = NULL;
	/* */
	st_contOgg_stc_rsetVorbCommRaw(pVCR);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetVorbCommRaw(Tst_contOgg_vorbCommentRaw *pVCR)
{
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_VOID(pVCR == NULL || pVCR->pObInternal == NULL)

	pVCRI = (Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal;

	pVCRI->isSet  = ST_B_FALSE;
	pVCRI->packNr = 0;
	/* */
	pVCRI->srcCmtEndPos = -1;
	pVCRI->srcCmtBegPos = -1;
	st_binobj_stc_rsetData(pVCRI->pRawCmtBO);
	pVCRI->elemCnt = 0;
	ST_DELPOINT(pVCRI->pVendor)
	/* */
	pVCRI->wasModified = ST_B_FALSE;
}

void
st_contOgg_stc_freeVorbCommRaw(Tst_contOgg_vorbCommentRaw *pVCR)
{
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_VOID(pVCR == NULL)

	if (pVCR->pObInternal != NULL) {
		st_contOgg_stc_rsetVorbCommRaw(pVCR);
		/* */
		pVCRI = (Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal;

		st_binobj_stc_freeBO(pVCRI->pRawCmtBO);
		ST_DELPOINT(pVCRI->pRawCmtBO)
		ST_DELPOINT(pVCR->pObInternal)
	}
}

/******************************************************************************/
