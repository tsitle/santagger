/******************************************************************************/
/* cont_ogg_stcs.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for OGG structures                                               */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj.h>
#include <santag/common/dynlist.h>
#include <santag/common/sys_math.h>
#include <santag/common/sys_fnc.h>
#include <santag/common/streamrd.h>
#include <santag/common/streamwr.h>
#include <santag/audvid/audvid.h>
#include <santag/audvid/cont_ogg.h>
/** */
#define SRC_CONT_OGG_STCS_C
#include "cont_ogg_stcs-prot.h"
/*** */
#include "../cont_ogg_aud_flac/ac_flac_stcs-prot.h"
#include "../cont_ogg_aud_spx_clt/ac_spx_clt_stcs-prot.h"
#include "../cont_ogg_aud_vorb/ac_vorb_stcs-prot.h"
#include "../cont_ogg_met_skel/mc_skel_zstcs-prot.h"
#include "../cont_ogg_vid_drc_theo/vc_drc_theo_stcs-prot.h"
#undef SRC_CONT_OGG_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void ST_CONTOGG__stc_cbRsetFHd(void *pElem);
static void ST_CONTOGG__stc_cbFreeFHd(void *pElem);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_obj */
Tst_err
st_contOgg_stc_initAObj(Tst_contOgg_obj *pAObj, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_contOgg_obj_intn *pAObjI;

	ST_ASSERTN_IARG(pAObj == NULL || pFilen == NULL)

	ST_CALLOC(pAObjI, Tst_contOgg_obj_intn*,
			1, sizeof(Tst_contOgg_obj_intn))
	if (pAObjI == NULL)
		return ST_ERR_OMEM;
	pAObj->pObInternal = pAObjI;

	pAObjI->pFilen = NULL;
	res            = st_sysStrcpy(pFilen, &pAObjI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	res = st_contOgg_stc_initFHdArr(&pAObjI->frHdArr);
	if (res == ST_ERR_SUCC)
		res = st_contOgg_stc_initBsArr(&pAObjI->bsArr, &pAObjI->opts);
	if (res != ST_ERR_SUCC)
		return res;

	st_contOgg_stc_rsetAObj_prot(pAObj, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetAObj(Tst_contOgg_obj *pAObj)
{
	st_contOgg_stc_rsetAObj_prot(pAObj, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_contOgg_stc_rsetAObj_prot(Tst_contOgg_obj *pAObj,
		const Tst_bool resetOpts,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetStrwr, Tst_streamwr *pStrwr,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_contOgg_obj_intn *pAObjI;

	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;

	if (resetStrrd)
		pAObjI->pStrrd = pStrrd;
	if (resetStrwr)
		pAObjI->pStrwr = pStrwr;
	if (resetFilen) {
		ST_ASSERTN_VOID(pFilen == NULL)

		if (st_sysStrcpy(pFilen, &pAObjI->pFilen) != ST_ERR_SUCC) {
			pAObjI->pFilen = NULL;  /* this is no good */

			ST_ASSERTN_VOID(pAObjI->pFilen == NULL)  /* cause exit() here */
		}
	}
	/* */
	st_contOgg_stc_rsetFHdArr(&pAObjI->frHdArr);
	st_contOgg_stc_rsetBsArr(&pAObjI->bsArr);
	/* */
	pAObjI->alrRead = ST_B_FALSE;
	/* */
	pAObjI->isSrcOggOrFlac = ST_B_TRUE;
	/* */
	pAObjI->hasOGGstream = ST_B_FALSE;
	pAObjI->hasFLACaudio = ST_B_FALSE;
	pAObjI->oggContTypes = (Tst_int32)ST_CONTOGG_BSCTP_NONE;
	/* */
	pAObjI->errorCnt = 0;
	/* */
	pAObjI->startOffs = -1;
	pAObjI->totSize   = 0;
	/* */
	if (resetOpts)
		st_contOgg_stc_rsetOpts(&pAObjI->opts);
}

void
st_contOgg_stc_freeAObj(Tst_contOgg_obj *pAObj)
{
	Tst_contOgg_obj_intn *pAObjI;

	ST_ASSERTN_VOID(pAObj == NULL)

	if (pAObj->pObInternal != NULL) {
		st_contOgg_stc_rsetAObj_prot(pAObj, ST_B_TRUE,
				ST_B_TRUE, NULL,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		/* */
		pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;

		ST_DELPOINT(pAObjI->pFilen)
		st_contOgg_stc_freeFHdArr(&pAObjI->frHdArr);
		st_contOgg_stc_freeBsArr(&pAObjI->bsArr);
		/* */
		ST_DELPOINT(pAObj->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_substream */
Tst_err
st_contOgg_stc_initBS(Tst_contOgg_substream *pBS,
		const Tst_str *pFilen, Tst_contOgg_opts *pOpts)
{
	Tst_err res;
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_IARG(pBS == NULL)

	ST_CALLOC(pBSI, Tst_contOgg_substr_intn*,
			1, sizeof(Tst_contOgg_substr_intn))
	if (pBSI == NULL)
		return ST_ERR_OMEM;
	pBS->pObInternal = pBSI;

	pBSI->pFilen = NULL;
	res          = st_sysStrcpy(pFilen, &pBSI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	pBSI->pSkel = NULL;
	pBSI->pVorb = NULL;
	pBSI->pFlac = NULL;
	pBSI->pSpee = NULL;
	pBSI->pCelt = NULL;
	pBSI->pTheo = NULL;
	pBSI->pDira = NULL;

	st_contOgg_stc_initPack(&pBSI->packCur);
	st_streamrd_stc_initSObj(&pBSI->sobjRdPackBuf);
	st_streamwr_stc_initSObj(&pBSI->sobjWrFBuf);
	st_streamrd_stc_initSObj(&pBSI->sobjRdFBuf);
	/* */
	pBSI->pOpts = pOpts;

	st_contOgg_stc_rsetBS(pBS);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetBS(Tst_contOgg_substream *pBS)
{
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_VOID(pBS == NULL || pBS->pObInternal == NULL)

	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;

	pBSI->isSrcOggOrFlac = ST_B_TRUE;
	/* */
	pBSI->started   = ST_B_FALSE;
	pBSI->ended     = ST_B_FALSE;
	pBSI->serial    = 0;
	pBSI->nr        = 0;
	pBSI->isGrouped = ST_B_FALSE;
	pBSI->grpIx     = 0;
	pBSI->grpSIx    = 0;
	/* */
	pBSI->pFHdBeg   = NULL;
	pBSI->fhdBegRNr = 0;
	/* */
	pBSI->frameCnt  = 0;
	pBSI->lastFrNr  = 0;
	pBSI->misFrames = 0;
	st_contOgg_stc_rsetPack(&pBSI->packCur);
	pBSI->packCnt  = 0;
	pBSI->tmpIs1st = ST_B_TRUE;
	/* */
	ST_SYSMATH_STC_RSETUI64(pBSI->maxGranulePos)
	ST_AV_STC_RSETPT(pBSI->playtime)
	pBSI->compBitrateAvgKBS   = 0;
	pBSI->codecNonHeadSize    = 0;
	pBSI->codecNonHeadPckCnt  = 0;
	pBSI->flacOffsFirstAudPkt = 0;
	/* */
	if (pBSI->pSkel != NULL) {
		st_contOgg_stc_freeSkel(pBSI->pSkel);
		ST_DELPOINT(pBSI->pSkel)
	}
	if (pBSI->pVorb != NULL) {
		st_contOgg_stc_freeVorb(pBSI->pVorb);
		ST_DELPOINT(pBSI->pVorb)
	}
	if (pBSI->pFlac != NULL) {
		st_contOgg_stc_freeFlac(pBSI->pFlac);
		ST_DELPOINT(pBSI->pFlac)
	}
	if (pBSI->pSpee != NULL) {
		st_contOgg_stc_freeSpee(pBSI->pSpee);
		ST_DELPOINT(pBSI->pSpee)
	}
	if (pBSI->pCelt != NULL) {
		st_contOgg_stc_freeCelt(pBSI->pCelt);
		ST_DELPOINT(pBSI->pCelt)
	}
	if (pBSI->pTheo != NULL) {
		st_contOgg_stc_freeTheo(pBSI->pTheo);
		ST_DELPOINT(pBSI->pTheo)
	}
	if (pBSI->pDira != NULL) {
		st_contOgg_stc_freeDira(pBSI->pDira);
		ST_DELPOINT(pBSI->pDira)
	}
	/* */
	pBSI->bsType = ST_CONTOGG_BSTP_NONE;
	/* */
	pBSI->errorCnt = 0;
	/* */
	st_streamrd_stc_rsetSObj(&pBSI->sobjRdPackBuf);
	st_streamwr_stc_rsetSObj(&pBSI->sobjWrFBuf);
	st_streamrd_stc_rsetSObj(&pBSI->sobjRdFBuf);
	/* */
	pBSI->wr.needVCmtUpd = ST_B_FALSE;
	pBSI->wr.vcmtPackNr  = 0;
	pBSI->wr.vcmtSrcBP   = -1;
	pBSI->wr.vcmtSrcEP   = -1;
	pBSI->wr.pendingPack = ST_B_FALSE;
	pBSI->wr.frameCnt    = 0;
	pBSI->wr.packCnt     = 0;
	pBSI->wr.eosWritten  = ST_B_FALSE;
	st_contOgg_stc_rsetFHd(&pBSI->wr.pendPckFHd);
	/* */
	pBSI->frameCnt = 0;
}

void
st_contOgg_stc_freeBS(Tst_contOgg_substream *pBS)
{
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_VOID(pBS == NULL)

	if (pBS->pObInternal != NULL) {
		st_contOgg_stc_rsetBS(pBS);
		/* */
		pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;

		st_contOgg_stc_freePack(&pBSI->packCur);
		ST_DELPOINT(pBSI->pFilen)
		st_streamrd_stc_freeSObj(&pBSI->sobjRdPackBuf);
		st_streamwr_stc_freeSObj(&pBSI->sobjWrFBuf);
		st_streamrd_stc_freeSObj(&pBSI->sobjRdFBuf);
		/* */
		ST_DELPOINT(pBS->pObInternal);
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_opts */
void
st_contOgg_stc_rsetOpts(Tst_contOgg_opts *pOpts)
{
	ST_ASSERTN_VOID(pOpts == NULL)

	ST_RSETSTC_BASOPTS(pOpts->basOpts)
	pOpts->anlz      = ST_B_FALSE;
	pOpts->quickScan = ST_B_FALSE;
	/* */
	pOpts->checkCRCofr     = ST_B_TRUE;
	pOpts->rdComments      = ST_B_TRUE;
	pOpts->forceRewrCmts   = ST_B_FALSE;
	pOpts->isPCMclientSet  = ST_B_FALSE;
	pOpts->decodeBPS       = 0;
	pOpts->printDecUsedBPS = ST_B_FALSE;
	pOpts->modeRdOrWr      = ST_B_TRUE;
	/* */
	pOpts->cbAnaMsg           = NULL;
	/* */
	pOpts->cbPCMprepareWriting = NULL;
	pOpts->cbPCMsetOutputFmt   = NULL;
	pOpts->cbPCMsamplesClient  = NULL;
	pOpts->cbPCMfinishWriting  = NULL;
	pOpts->pPCMclientObj       = NULL;
	/* */
	st_contOgg_stc_rsetFlacOpts(&pOpts->flacOpts);
	st_contOgg_stc_rsetVorbOpts(&pOpts->vorbOpts);
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_fhd */
void
st_contOgg_stc_rsetFHd(Tst_contOgg_fhd *pFH)
{
	ST_ASSERTN_VOID(pFH == NULL)

	pFH->offs        = -1;
	pFH->frOffsDelta = 0;
	pFH->hdSz        = 0;
	pFH->frSz        = 0;
	/* */
	pFH->vers       = 0;
	pFH->hdTypeCont = ST_B_FALSE;  /* fresh packet */
	pFH->hdTypeBOS  = ST_B_FALSE;  /* not first frame */
	pFH->hdTypeEOS  = ST_B_FALSE;  /* not last frame */
	ST_SYSMATH_STC_RSETUI64(pFH->granulePos)
	pFH->bsSerial = 0;
	pFH->frSequNr = 0;
	pFH->crc32_a  = 0;
	pFH->crc32_b  = 0;
	pFH->segsUsd  = 0;
	memset(pFH->segTab, 0, sizeof(pFH->segTab));
	/* */
	pFH->realNr = 0;
	/* */
	pFH->wasUsed = ST_B_FALSE;
}

void
st_contOgg_stc_copyFHd(const Tst_contOgg_fhd *pFHsrc, Tst_contOgg_fhd *pFHdst)
{
	ST_ASSERTN_VOID(pFHsrc == NULL || pFHdst == NULL)

	memcpy(pFHdst, pFHsrc, sizeof(Tst_contOgg_fhd));
}

/* Tst_contOgg_frameHdArr */
Tst_err
st_contOgg_stc_initFHdArr(Tst_contOgg_frameHdArr *pFA)
{
	Tst_err res;

	ST_ASSERTN_IARG(pFA == NULL)

	res = st_dynlist_stc_initDL(&pFA->list, sizeof(Tst_contOgg_fhd),
			ST_CONTOGG__stc_cbRsetFHd, ST_CONTOGG__stc_cbFreeFHd);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	st_contOgg_stc_rsetFHdArr(pFA);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetFHdArr(Tst_contOgg_frameHdArr *pFA)
{
	ST_ASSERTN_VOID(pFA == NULL)

	st_dynlist_stc_rsetDL(&pFA->list);
	pFA->used = 0;
}

void
st_contOgg_stc_freeFHdArr(Tst_contOgg_frameHdArr *pFA)
{
	ST_ASSERTN_VOID(pFA == NULL)

	st_contOgg_stc_rsetFHdArr(pFA);
	/* */
	st_dynlist_stc_freeDL(&pFA->list);
}

/*
 * Add some more elements to frame header array (or list)
 */
Tst_err
st_contOgg_stc_reszFHdArr(Tst_contOgg_frameHdArr *pFA,
		const Tst_uint32 newSz)
{
	Tst_err    res;
	Tst_uint32 x,
	           csz;
	Tst_contOgg_fhd *pNewFHd = NULL;

	ST_ASSERTN_IARG(pFA == NULL)

	csz = st_dynlist_getElementCount(&pFA->list);
	if (newSz > csz) {
		for (x = csz; x < newSz; x++) {
			ST_CALLOC(pNewFHd, Tst_contOgg_fhd*,
					1, sizeof(Tst_contOgg_fhd))
			if (pNewFHd == NULL)
				return ST_ERR_OMEM;
			st_contOgg_stc_rsetFHd(pNewFHd);
			/* */
			res = st_dynlist_addElement(&pFA->list, pNewFHd);
			if (res != ST_ERR_SUCC) {
				ST_DELPOINT(pNewFHd)
				return res;
			}
		}
	}

	return ST_ERR_SUCC;
}

/*
 * Get element from frame header array (or list)
 *   by its index
 */
Tst_contOgg_fhd*
st_contOgg_stc_getFHdArrElem_byIX(Tst_contOgg_frameHdArr *pFA,
		const Tst_uint32 ix)
{
	ST_ASSERTN_NULL(pFA == NULL || ix == 0)

	if (! st_dynlist_ite_goto(&pFA->list, ix))
		return NULL;
	return (Tst_contOgg_fhd*)st_dynlist_ite_getCurrent(&pFA->list);
}

/*
 * Get element from frame header array (or list)
 *   by its bitstream serial and frame number
 */
Tst_contOgg_fhd*
st_contOgg_stc_getFHdArrElem_bySNandFNR(Tst_contOgg_frameHdArr *pFA,
		const Tst_uint32 bsSerial, const Tst_uint32 fnr)
{
	Tst_uint32      cix = 0;
	Tst_contOgg_fhd *pIteFHd;

	ST_ASSERTN_NULL(pFA == NULL || fnr == 0)

	if (pFA->used == 0 || ! st_dynlist_ite_gotoFirst(&pFA->list))
		return NULL;
	do {
		++cix;
		pIteFHd = (Tst_contOgg_fhd*)st_dynlist_ite_getCurrent(&pFA->list);
		ST_ASSERTN_NULL(pIteFHd == NULL)

		if (pIteFHd->bsSerial == bsSerial &&
				fnr == pIteFHd->realNr)
			return pIteFHd;
	} while (cix < pFA->used && st_dynlist_ite_gotoNext(&pFA->list));
	return NULL;
}

/*
 * 'Deactivate' elements in frame header array (or list)
 *   by resetting their content and moving them to the end of the list
 *
 * This is done to save memory since all frame headers
 *   that belong to the bitstream with the serial no. bsSerial
 *   and those framenr. is smaller than fnrBeg
 *   are no longer needed because
 *   they belong to they previous packet
 *
 * These 'deactivated' frame hds. may be reused later on
 *
 * This algo can be time-consuming but it
 *   saves alot of RAM...
 */
Tst_err
st_contOgg_stc_deactFHdArrPrevElems(Tst_contOgg_frameHdArr *pFA,
		const Tst_uint32 bsSerial, const Tst_uint32 fnrBeg)
{
	Tst_uint32 cix = 0,
	           ecnt;
	Tst_bool   gotoPrev = ST_B_FALSE;
	Tst_contOgg_fhd *pIteFHd;

	ST_ASSERTN_IARG(pFA == NULL || fnrBeg == 0)

	if (pFA->used == 0)
		return ST_ERR_SUCC;
	ecnt = st_dynlist_getElementCount(&pFA->list);

	if (! st_dynlist_ite_gotoFirst(&pFA->list))
		return ST_ERR_FAIL;
	do {
		++cix;
		if (gotoPrev) {
			if (! st_dynlist_ite_gotoPrev(&pFA->list))
				return ST_ERR_FAIL;
			--cix;
			gotoPrev = ST_B_FALSE;
		}
		/* */
		pIteFHd = (Tst_contOgg_fhd*)st_dynlist_ite_getCurrent(&pFA->list);
		ST_ASSERTN_NUM(ST_ERR_FAIL, pIteFHd == NULL)

		if (pIteFHd->bsSerial == bsSerial &&
				pIteFHd->realNr < fnrBeg) {
			st_contOgg_stc_rsetFHd(pIteFHd);
			if (cix < ecnt &&
					! st_dynlist_moveElement(&pFA->list, cix, ecnt))
				return ST_ERR_FAIL;
			gotoPrev = ST_B_TRUE;
			--pFA->used;
			/* the following is necessary because the
			 *   internal iteration pointer was resetted by moveElement()  */
			if (! st_dynlist_ite_goto(&pFA->list, cix))
				return ST_ERR_FAIL;
		}
	} while (cix < pFA->used && st_dynlist_ite_gotoNext(&pFA->list));

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_substreamArr */
Tst_err
st_contOgg_stc_initBsArr(Tst_contOgg_substreamArr *pSA,
		Tst_contOgg_opts *pOpts)
{
	ST_ASSERTN_IARG(pSA == NULL)

	pSA->pArr  = NULL;
	pSA->sz    = 0;
	pSA->pOpts = pOpts;
	/* */
	st_contOgg_stc_rsetBsArr(pSA);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetBsArr(Tst_contOgg_substreamArr *pSA)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pSA == NULL)

	if (pSA->pArr != NULL) {
		for (x = 0; x < pSA->sz; x++)
			st_contOgg_stc_freeBS((Tst_contOgg_substream*)&pSA->pArr[x]);
		ST_DELPOINT(pSA->pArr)
	}
	pSA->sz  = 0;
	pSA->cnt = 0;
	/* */
	pSA->openBScnt = 0;
	pSA->grpCnt    = 0;
}

Tst_err
st_contOgg_stc_reszBsArr(const Tst_str *pFilen,
		Tst_contOgg_substreamArr *pSA,
		const Tst_uint32 newSz)
{
	Tst_err    res;
	Tst_uint32 x;

	ST_ASSERTN_IARG(pSA == NULL)

	if (newSz > pSA->sz) {
		ST_REALLOC(pSA->pArr, Tst_contOgg_substream*,
				newSz, sizeof(Tst_contOgg_substream))
		if (pSA->pArr == NULL)
			return ST_ERR_OMEM;
		for (x = pSA->sz; x < newSz; x++) {
			res = st_contOgg_stc_initBS(&pSA->pArr[x], pFilen, pSA->pOpts);
			if (res != ST_ERR_SUCC)
				return res;
		}
		pSA->sz = newSz;
	}

	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetBsArrPackData(Tst_contOgg_substreamArr *pSA)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pSA == NULL)

	for (x = 0; x < pSA->sz; x++)
		st_contOgg_stc_rsetPack(
				&((Tst_contOgg_substr_intn*)pSA->pArr[x].pObInternal)->packCur);
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_packet */
void
st_contOgg_stc_initPack(Tst_contOgg_packet *pPck)
{
	ST_ASSERTN_VOID(pPck == NULL)

	st_binobj_stc_initBO(&pPck->bdata);
	st_binobj_setThreshold(&pPck->bdata, ST_CONTOGG_PACKSZ_THRESHOLD);
	st_binobj_setDefaultReallcSize(&pPck->bdata, ST_CONTOGG_PACKSZ_REALLC);
	/* */
	pPck->pSegs  = NULL;
	pPck->segsSz = 0;
	/* */
	st_contOgg_stc_rsetPack(pPck);
}

void
st_contOgg_stc_rsetPack(Tst_contOgg_packet *pPck)
{
	ST_ASSERTN_VOID(pPck == NULL)

	st_contOgg_stc_rsetPack_dataOnly(pPck);
	pPck->nr = 0;
}

void
st_contOgg_stc_rsetPack_dataOnly(Tst_contOgg_packet *pPck)
{
	ST_ASSERTN_VOID(pPck == NULL)

	st_binobj_stc_rsetData(&pPck->bdata);
	pPck->segsUsd = 0;
}

void
st_contOgg_stc_freePack(Tst_contOgg_packet *pPck)
{
	ST_ASSERTN_VOID(pPck == NULL)

	st_binobj_stc_freeBO(&pPck->bdata);
	ST_DELPOINT(pPck->pSegs)
}

/*----------------------------------------------------------------------------*/

Tst_err
st_contOgg_stc_reszBuf(Tst_buf **ppBuf, Tst_uint32 *pBufSz,
		const Tst_uint32 newSz)
{
	Tst_err res = ST_ERR_SUCC;

	ST_ASSERTN_IARG(ppBuf == NULL || pBufSz == NULL)

	if (newSz > *pBufSz) {
		ST_REALLOC(*ppBuf, Tst_buf*, newSz, 1)
		if (*ppBuf == NULL)
			res = ST_ERR_OMEM;
		else
			*pBufSz = newSz;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_CONTOGG__stc_cbRsetFHd(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_contOgg_stc_rsetFHd((Tst_contOgg_fhd*)pElem);
}

static void
ST_CONTOGG__stc_cbFreeFHd(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_contOgg_stc_rsetFHd((Tst_contOgg_fhd*)pElem);
}

/******************************************************************************/
