/******************************************************************************/
/* tag_meta.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Meta Tag (can contain any tag type)                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_id3v1.h"
#include "src/includes/tag/tag_vorbc.h"
#include "src/includes/tag/tag_meta.h"
/** */
#define SRC_TAG_META_C
#include "tag_meta-prot.h"
/*** */
#include "tag_meta_stcs-prot.h"
#include "tag_meta_fnc-prot.h"
#undef SRC_TAG_META_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const char *ST_MTAG_TAGDSC_IV1[2] = {ST_ID3V1_TAG_NAME, ST_ID3V1_TAG_NAME_SH};
const char *ST_MTAG_TAGDSC_IV2[2] = {ST_ID3V2_TAG_NAME, ST_ID3V2_TAG_NAME_SH};
const char *ST_MTAG_TAGDSC_AV2[2] = {ST_APEV2_TAG_NAME, ST_APEV2_TAG_NAME_SH};
const char *ST_MTAG_TAGDSC_VOR[2] = {ST_VORBC_TAG_NAME, ST_VORBC_TAG_NAME_SH};
const char *ST_MTAG_TAGDSC_MRG[2] = {"Merged", "mrg"};
const char *ST_MTAG_TAGDSC_NA[2]  = {"Unknown", "n/a"};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Create new tag and add it to pMA
 *
 * pMA            MUST be set
 * pMAI->pFilen   MUST be set
 * pMAI->pStrrd   MUST be set
 */
Tst_tagMeta_mt*
st_tagMeta_addNewToArr(Tst_tagMeta_mtArr *pMA,
		const Tst_tagMeta_type mtType,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx)
{
	Tst_err res;
	Tst_tagMeta_mt         *pMT = NULL;
	Tst_tagMeta_mt_intn    *pMTI;
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_NULL(pMA == NULL || pMA->pObInternal == NULL)

	pMAI = (Tst_tagMeta_mtArr_intn*)pMA->pObInternal;
	ST_ASSERTN_NULL(pMAI->pFilen == NULL || pMAI->pStrrd == NULL)

	/* check mtType */
	if (mtType >= ST_MTAG_TTP_NONE) {
		ST_ASSERTN_NULL(mtType >= ST_MTAG_TTP_NONE)  /* cause exit() here */
		return NULL;
	}

	/* add new element to array */
	ST_CALLOC(pMT, Tst_tagMeta_mt*, 1, sizeof(Tst_tagMeta_mt))
	if (pMT == NULL)
		return NULL;
	res = st_tagMeta_stc_initTag(pMT, pMAI->pFilen);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pMT)
		return NULL;
	}
	res = st_dynlist_addElement(&pMAI->list, pMT);
	if (res != ST_ERR_SUCC) {
		st_tagMeta_stc_freeTag(pMT);
		ST_DELPOINT(pMT)
		return NULL;
	}
	pMTI           = (Tst_tagMeta_mt_intn*)pMT->pObInternal;
	pMTI->tp       = mtType;
	pMTI->bsIx     = bsIx;
	pMTI->bsSIx    = bsSIx;
	pMTI->uid      = st_tagMeta_getNewUID(pMAI);
	pMTI->uniqueIx = pMAI->uniqueIxLast++;

	/* */
	switch (mtType) {
	case ST_MTAG_TTP_IV1:
		ST_CALLOC(pMTI->pIV1, Tst_id3v1_tag*, 1, sizeof(Tst_id3v1_tag))
		if (pMTI->pIV1 == NULL)
			return NULL;
		if (st_id3v1_stc_initTag(pMTI->pIV1, pMTI->pFilen) != ST_ERR_SUCC)
			return NULL;
		st_id3v1_gs_setTag_strrd(pMTI->pIV1, pMAI->pStrrd);
		st_id3v1_gs_setTag_strwr(pMTI->pIV1, pMAI->pStrwr);
		if (pMAI->cbSetOptsIV1 != NULL)
			pMAI->cbSetOptsIV1(pMAI->pSetOptsDataIV1, pMTI->pIV1);
		break;
	case ST_MTAG_TTP_AV2:
		ST_CALLOC(pMTI->pAV2, Tst_apev2_tag*, 1, sizeof(Tst_apev2_tag))
		if (pMTI->pAV2 == NULL)
			return NULL;
		if (st_apev2_stc_initTag(pMTI->pAV2, pMTI->pFilen) != ST_ERR_SUCC)
			return NULL;
		st_apev2_gs_setTag_strrd(pMTI->pAV2, pMAI->pStrrd);
		st_apev2_gs_setTag_strwr(pMTI->pAV2, pMAI->pStrwr);
		if (pMAI->cbSetOptsAV2 != NULL)
			pMAI->cbSetOptsAV2(pMAI->pSetOptsDataAV2, pMTI->pAV2);
		break;
	case ST_MTAG_TTP_VOR:
		ST_CALLOC(pMTI->pVOR, Tst_vorbc_tag*, 1, sizeof(Tst_vorbc_tag))
		if (pMTI->pVOR == NULL)
			return NULL;
		if (st_vorbc_stc_initTag(pMTI->pVOR, pMTI->pFilen) != ST_ERR_SUCC)
			return NULL;
		st_vorbc_gs_setTag_strrd(pMTI->pVOR, pMAI->pStrrd);
		st_vorbc_gs_setTag_strwr(pMTI->pVOR, pMAI->pStrwr);
		if (pMAI->cbSetOptsVOR != NULL)
			pMAI->cbSetOptsVOR(pMAI->pSetOptsDataVOR, pMTI->pVOR);
		break;
	case ST_MTAG_TTP_MRG:
	default:  /* ST_MTAG_TTP_IV2 */
		ST_CALLOC(pMTI->pIV2, Tst_id3v2_tag*, 1, sizeof(Tst_id3v2_tag))
		if (pMTI->pIV2 == NULL)
			return NULL;
		if (st_id3v2_stc_initTag(pMTI->pIV2, pMAI->pFilen) != ST_ERR_SUCC)
			return NULL;
		st_id3v2_gs_setTag_strrd(pMTI->pIV2, pMAI->pStrrd);
		st_id3v2_gs_setTag_strwr(pMTI->pIV2, pMAI->pStrwr);
		if (pMAI->cbSetOptsIV2 != NULL)
			pMAI->cbSetOptsIV2(pMAI->pSetOptsDataIV2, pMTI->pIV2);
	}
	return pMT;
}

/*----------------------------------------------------------------------------*/

/*
 * Remove tag in pMA given by UID
 *
 * pMA   MUST be set
 */
Tst_err
st_tagMeta_removeTagFromArr_byUID(Tst_tagMeta_mtArr *pMA, const Tst_uint32 uid)
{
	Tst_uint32 ix = 0;
	Tst_tagMeta_mt *pMT;

	ST_ASSERTN_IARG(pMA == NULL || pMA->pObInternal == NULL)

	if (uid == 0)
		return ST_ERR_SUCC;
	pMT = st_tagMeta_getTag_byUidOrBsIx(pMA, &ix, &uid, NULL, NULL);
	if (pMT == NULL)
		return ST_ERR_FAIL;

	st_dynlist_remElement(
			&((Tst_tagMeta_mtArr_intn*)pMA->pObInternal)->list, ix);
	return ST_ERR_SUCC;
}

/******************************************************************************/
