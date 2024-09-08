/******************************************************************************/
/* tag_meta_stcs.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for Meta Tag structures                                          */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/dynlist.h>
#include <santag/common/sys_fnc.h>
#include <santag/tag/tag_basics.h>
#include <santag/tag/tag_apev2.h>
#include <santag/tag/tag_id3v2.h>
#include <santag/tag/tag_id3v1.h>
#include <santag/tag/tag_vorbc.h>
#include <santag/tag/tag_meta.h>
/** */
#define SRC_TAG_META_STCS_C
#include "tag_meta_stcs-prot.h"
/*** */
#include "../apev2/tag_apev2_stcs-prot.h"
#include "../id3v1/tag_id3v1_stcs-prot.h"
#include "../id3v2/tag_id3v2_stcs-prot.h"
#include "../vorbc/tag_vorbc_stcs-prot.h"
#undef SRC_TAG_META_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void ST_TMETA__stc_cbRsetTag(void *pElem);
static void ST_TMETA__stc_cbFreeTag(void *pElem);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_tagMeta_mt */
Tst_err
st_tagMeta_stc_initTag(Tst_tagMeta_mt *pTag, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_tagMeta_mt_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pFilen == NULL)

	ST_CALLOC(pTagI, Tst_tagMeta_mt_intn*,
			1, sizeof(Tst_tagMeta_mt_intn))
	if (pTagI == NULL)
		return ST_ERR_OMEM;
	pTag->pObInternal = pTagI;

	pTagI->pFilen = NULL;
	res           = st_sysStrcpy(pFilen, &pTagI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	res = st_tagBas_stc_initTB(&pTagI->tbasOrg);
	if (res != ST_ERR_SUCC)
		return res;
	pTagI->pIV1 = NULL;
	pTagI->pIV2 = NULL;
	pTagI->pAV2 = NULL;
	pTagI->pVOR = NULL;
	st_tagMeta_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_tagMeta_stc_rsetTag(Tst_tagMeta_mt *pTag)
{
	st_tagMeta_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_tagMeta_stc_rsetTag_prot(Tst_tagMeta_mt *pTag,
		const Tst_bool resetOpts,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetStrwr, Tst_streamwr *pStrwr,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_tagMeta_mt_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_tagMeta_mt_intn*)pTag->pObInternal;

	if (resetFilen) {
		ST_ASSERTN_VOID(pFilen == NULL)

		if (st_sysStrcpy(pFilen, &pTagI->pFilen) != ST_ERR_SUCC) {
			pTagI->pFilen = NULL;  /* this is no good */

			ST_ASSERTN_VOID(pTagI->pFilen == NULL)  /* cause exit() here */
		}
	}
	st_tagBas_stc_rsetTB(&pTagI->tbasOrg);
	pTagI->tbasOrgSet = ST_B_FALSE;
	/* */
	pTagI->tp       = ST_MTAG_TTP_NONE;
	pTagI->tpOrg    = ST_MTAG_TTP_NONE;
	pTagI->bsIx     = 0;
	pTagI->bsSIx    = 0;
	pTagI->uid      = 0;
	pTagI->uniqueIx = 0;
	/* */
	if (pTagI->pIV1 != NULL)
		st_id3v1_stc_rsetTag_prot(pTagI->pIV1, resetOpts,
				resetStrrd, pStrrd,
				resetStrwr, pStrwr,
				resetFilen, pFilen);
	else if (pTagI->pIV2 != NULL)
		st_id3v2_stc_rsetTag_prot(pTagI->pIV2, resetOpts,
				resetStrrd, pStrrd,
				resetStrwr, pStrwr,
				resetFilen, pFilen);
	else if (pTagI->pAV2 != NULL)
		st_apev2_stc_rsetTag_prot(pTagI->pAV2, resetOpts,
				resetStrrd, pStrrd,
				resetStrwr, pStrwr,
				resetFilen, pFilen);
	else if (pTagI->pVOR != NULL)
		st_vorbc_stc_rsetTag_prot(pTagI->pVOR, resetOpts,
				resetStrrd, pStrrd,
				resetStrwr, pStrwr,
				resetFilen, pFilen);
}

void
st_tagMeta_stc_freeTag(Tst_tagMeta_mt *pTag)
{
	Tst_tagMeta_mt_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL)

	if (pTag->pObInternal != NULL) {
		pTagI = (Tst_tagMeta_mt_intn*)pTag->pObInternal;

		if (pTagI->pIV1 != NULL) {
			st_id3v1_stc_freeTag(pTagI->pIV1);
			ST_DELPOINT(pTagI->pIV1)
		} else if (pTagI->pIV2 != NULL) {
			st_id3v2_stc_freeTag(pTagI->pIV2);
			ST_DELPOINT(pTagI->pIV2)
		} else if (pTagI->pAV2 != NULL) {
			st_apev2_stc_freeTag(pTagI->pAV2);
			ST_DELPOINT(pTagI->pAV2)
		} else if (pTagI->pVOR != NULL) {
			st_vorbc_stc_freeTag(pTagI->pVOR);
			ST_DELPOINT(pTagI->pVOR)
		}
		/* */
		st_tagMeta_stc_rsetTag_prot(pTag, ST_B_TRUE,
				ST_B_TRUE, NULL,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		st_tagBas_stc_freeTB(&pTagI->tbasOrg);
		ST_DELPOINT(pTagI->pFilen)
		/* */
		ST_DELPOINT(pTag->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_tagMeta_mtArr */
Tst_err
st_tagMeta_stc_initTagArr(Tst_tagMeta_mtArr *pMA, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_IARG(pMA == NULL || pFilen == NULL)

	ST_CALLOC(pMAI, Tst_tagMeta_mtArr_intn*,
			1, sizeof(Tst_tagMeta_mtArr_intn))
	if (pMAI == NULL)
		return ST_ERR_OMEM;
	pMA->pObInternal = pMAI;

	pMAI->pFilen = NULL;
	res          = st_sysStrcpy(pFilen, &pMAI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	res = st_dynlist_stc_initDL(&pMAI->list, sizeof(Tst_tagMeta_mt),
			ST_TMETA__stc_cbRsetTag, ST_TMETA__stc_cbFreeTag);
	if (res != ST_ERR_SUCC)
		return res;
	st_tagMeta_stc_rsetTagArr_prot(pMA,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_tagMeta_stc_rsetTagArr(Tst_tagMeta_mtArr *pMA)
{
	st_tagMeta_stc_rsetTagArr_prot(pMA,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_tagMeta_stc_rsetTagArr_prot(Tst_tagMeta_mtArr *pMA,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetStrwr, Tst_streamwr *pStrwr,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_VOID(pMA == NULL || pMA->pObInternal == NULL)

	pMAI = (Tst_tagMeta_mtArr_intn*)pMA->pObInternal;

	if (resetStrrd)
		pMAI->pStrrd = pStrrd;
	if (resetStrwr)
		pMAI->pStrwr = pStrwr;
	if (resetFilen) {
		ST_ASSERTN_VOID(pFilen == NULL)

		if (st_sysStrcpy(pFilen, &pMAI->pFilen) != ST_ERR_SUCC) {
			pMAI->pFilen = NULL;  /* this is no good */

			ST_ASSERTN_VOID(pMAI->pFilen == NULL)  /* cause exit() here */
		}
	}
	/* */
	pMAI->uniqueIxLast = 1;
	/* */
	st_dynlist_stc_rsetDL(&pMAI->list);
	/* */
	pMAI->cbSetOptsIV1    = NULL;
	pMAI->pSetOptsDataIV1 = NULL;
	pMAI->cbSetOptsIV2    = NULL;
	pMAI->pSetOptsDataIV2 = NULL;
	pMAI->cbSetOptsAV2    = NULL;
	pMAI->pSetOptsDataAV2 = NULL;
	pMAI->cbSetOptsVOR    = NULL;
	pMAI->pSetOptsDataVOR = NULL;
}

void
st_tagMeta_stc_freeTagArr(Tst_tagMeta_mtArr *pMA)
{
	ST_ASSERTN_VOID(pMA == NULL)

	if (pMA->pObInternal != NULL) {
		st_tagMeta_stc_rsetTagArr_prot(pMA,
				ST_B_TRUE, NULL,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		/* */
		st_dynlist_stc_freeDL(
				&((Tst_tagMeta_mtArr_intn*)pMA->pObInternal)->list);
		ST_DELPOINT(((Tst_tagMeta_mtArr_intn*)pMA->pObInternal)->pFilen)
		/* */
		ST_DELPOINT(pMA->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_TMETA__stc_cbRsetTag(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_tagMeta_stc_rsetTag((Tst_tagMeta_mt*)pElem);
}

static void
ST_TMETA__stc_cbFreeTag(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_tagMeta_stc_freeTag((Tst_tagMeta_mt*)pElem);
}

/******************************************************************************/
