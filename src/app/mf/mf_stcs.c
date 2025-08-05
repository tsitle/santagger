/******************************************************************************/
/* mf_stcs.c                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for structures for the 'main functions'                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/audvid/aud_mpeg1.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/tag/tag_meta.h"
/** */
#define SRC_MF_STCS_C
#include "mf_stcs-pp.h"
/*** */
#include "mf_outp-pp.h"
#include "../cln/cln_stcs-prot.h"
#undef SRC_MF_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), free(), ... */
#include <string.h>      /* memset(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tast_mf_finfo */
Tst_err
ast_mf_stc_initMF(Tast_mf_finfo *pMF)
{
	const Tst_str *cNOFILEN = (const Tst_str*)"-none-";
	Tst_err res;

	ST_ASSERTN_IARG(pMF == NULL)

	res = st_sys_stc_initFStc(&pMF->fstc);
	if (res == ST_ERR_SUCC)
		res = st_streamrd_stc_initSObj(&pMF->strrd);
	/* */
	if (res == ST_ERR_SUCC)
		res = st_mpeg1_stc_initMO(&pMF->audMpg1, cNOFILEN);
	if (res == ST_ERR_SUCC)
		res = st_contOgg_stc_initAObj(&pMF->avOgg, cNOFILEN);
	/* */
	if (res == ST_ERR_SUCC)
		res = st_tagMeta_stc_initTagArr(&pMF->tagArr, cNOFILEN);
	/* */
	if (res == ST_ERR_SUCC)
		ast_mf_stc_rsetMF(pMF);
	return res;
}

void
ast_mf_stc_rsetMF(Tast_mf_finfo *pMF)
{
	ST_ASSERTN_VOID(pMF == NULL)

	st_sys_stc_rsetFStc(&pMF->fstc, ST_B_TRUE);
	st_streamrd_stc_rsetSObj(&pMF->strrd);
	pMF->pFilen = NULL;
	/* */
	pMF->ffmt      = AST_MF_FFMT_NONE;
	pMF->contTypes = AST_MF_CTP_NONE;
	/* */
	st_tagMeta_gs_setArr_strrdForAllTags(&pMF->tagArr, &pMF->strrd);
	st_tagMeta_stc_rsetTagArr(&pMF->tagArr);
	/* */
	st_mpeg1_stc_rsetMO(&pMF->audMpg1);
	/* */
	st_contOgg_stc_rsetAObj(&pMF->avOgg);
	/* */
	pMF->cbAnaMsg = ast_mf_op_cb_anaMsg;
	/* */
	AST_MF__stc_rsetSetOptsData(&pMF->setOptsData);
}

void
ast_mf_stc_freeMF(Tast_mf_finfo *pMF)
{
	ST_ASSERTN_VOID(pMF == NULL)

	ast_mf_stc_rsetMF(pMF);
	/* */
	st_sys_stc_freeFStc(&pMF->fstc);
	st_streamrd_stc_freeSObj(&pMF->strrd);
	/* */
	st_mpeg1_stc_freeMO(&pMF->audMpg1);
	st_contOgg_stc_freeAObj(&pMF->avOgg);
	/* */
	st_tagMeta_stc_freeTagArr(&pMF->tagArr);
}

/*----------------------------------------------------------------------------*/

/* Tst_callbacks_dbg */
void
ast_mf_stc_setCBsDeb(Tst_callbacks_dbg *pCBs)
{
	ST_ASSERTN_VOID(pCBs == NULL)

	pCBs->cbDbgFnc = ast_mf_op_cb_dbgFnc;
	pCBs->cbErrFnc = ast_mf_op_cb_errFnc;
}

/*----------------------------------------------------------------------------*/

/* Tast_mf_editInfo */
void
ast_mf_stc_initEditInfo(Tast_mf_editInfo *pEdInf)
{
	ST_ASSERTN_VOID(pEdInf == NULL)

	pEdInf->obsUIDs.pArr = NULL;
	pEdInf->usdUIDs.pArr = NULL;

	ast_mf_stc_rsetEditInfo(pEdInf);
}

void
ast_mf_stc_rsetEditInfo(Tast_mf_editInfo *pEdInf)
{
	ST_ASSERTN_VOID(pEdInf == NULL)

	ST_DELPOINT(pEdInf->obsUIDs.pArr)
	pEdInf->obsUIDs.cnt = 0;
	pEdInf->obsUIDs.sz  = 0;
	ST_DELPOINT(pEdInf->usdUIDs.pArr)
	pEdInf->usdUIDs.cnt = 0;
	pEdInf->usdUIDs.sz  = 0;
	/* */
	pEdInf->uidMainIV1 = 0;
	pEdInf->uidMainIV2 = 0;
	pEdInf->uidMainAV2 = 0;
	pEdInf->uidMainVOR = 0;
	/* */
	pEdInf->modEmbVorTag = ST_B_FALSE;
	/* */
	pEdInf->hasAudio   = ST_B_FALSE;
	pEdInf->wroteAudio = ST_B_FALSE;
	pEdInf->wroteIV1   = ST_B_FALSE;
	pEdInf->wroteIV2   = ST_B_FALSE;
	pEdInf->wroteAV2   = ST_B_FALSE;
	pEdInf->wroteVOR   = ST_B_FALSE;
	/* */
	pEdInf->allwNonEmbTags = ST_B_FALSE;
	/* */
	ast_cln_stc_rsetT_taglist(pEdInf->allwLst, ST_B_FALSE);
	ast_cln_stc_rsetT_taglist(pEdInf->tagEdLst, ST_B_FALSE);
}

Tst_err
ast_mf_stc_editInfo_addUIDtoObs(Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uid)
{
	ST_ASSERTN_IARG(pEdInf == NULL || uid == 0)

	return AST_MF__stc_editInfo_addUIDtoArr(&pEdInf->obsUIDs, uid);
}

Tst_bool
ast_mf_stc_editInfo_isUIDinObs(const Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uid)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pEdInf == NULL)

	return AST_MF__stc_editInfo_isUIDinArr(&pEdInf->obsUIDs, uid);
}

Tst_err
ast_mf_stc_editInfo_addUIDtoUsd(Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uid)
{
	ST_ASSERTN_IARG(pEdInf == NULL || uid == 0)

	return AST_MF__stc_editInfo_addUIDtoArr(&pEdInf->usdUIDs, uid);
}

Tst_bool
ast_mf_stc_editInfo_isUIDinUsd(const Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uid)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pEdInf == NULL)

	return AST_MF__stc_editInfo_isUIDinArr(&pEdInf->usdUIDs, uid);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tast_mf_setOptsData */
static void
AST_MF__stc_rsetSetOptsData(Tast_mf_setOptsData *pSOD)
{
	pSOD->cbAnaMsg = NULL;
	pSOD->pCmdln   = NULL;
}

/*----------------------------------------------------------------------------*/

/* Tast_mf_editInfo_uidArr */
static Tst_err
AST_MF__stc_editInfo_addUIDtoArr(Tast_mf_editInfo_uidArr *pUA,
		const Tst_uint32 uid)
{
	Tst_uint32 nsz;

	ST_ASSERTN_IARG(pUA == NULL || uid == 0)

	if (pUA->cnt + 1 >= pUA->sz) {
		nsz = pUA->sz + 5;
		ST_REALLOC(pUA->pArr, Tst_uint32*, nsz, sizeof(Tst_uint32))
		if (pUA->pArr == NULL)
			return ST_ERR_OMEM;
		memset(&pUA->pArr[pUA->sz], 0,
				(nsz - pUA->sz) * sizeof(Tst_uint32));
		pUA->sz = nsz;
	}
	pUA->pArr[pUA->cnt++] = uid;
	return ST_ERR_SUCC;
}

static Tst_bool
AST_MF__stc_editInfo_isUIDinArr(const Tast_mf_editInfo_uidArr *pUA,
		const Tst_uint32 uid)
{
	Tst_uint32 x;

	ST_ASSERTN_BOOL(ST_B_FALSE, pUA == NULL)

	if (pUA->cnt > 0 && pUA->pArr != NULL)
		for (x = 0; x < pUA->cnt; x++)
			if (pUA->pArr[x] == uid)
				return ST_B_TRUE;
	return ST_B_FALSE;
}

/******************************************************************************/
