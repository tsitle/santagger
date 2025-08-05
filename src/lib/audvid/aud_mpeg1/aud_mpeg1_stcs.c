/******************************************************************************/
/* aud_mpeg1_stcs.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for MPEG-1 structures                                            */
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
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/audvid.h"
#include "src/includes/audvid/aud_mpeg1.h"
/** */
#define SRC_AUD_MPEG1_STCS_C
#include "aud_mpeg1_stcs-prot.h"
#undef SRC_AUD_MPEG1_STCS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_mpeg1_obj */
Tst_err
st_mpeg1_stc_initMO(Tst_mpeg1_obj *pMO, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_mpeg1_obj_intn *pMOI;

	ST_ASSERTN_IARG(pMO == NULL || pFilen == NULL)

	ST_CALLOC(pMOI, Tst_mpeg1_obj_intn*,
			1, sizeof(Tst_mpeg1_obj_intn))
	if (pMOI == NULL)
		return ST_ERR_OMEM;
	pMO->pObInternal = pMOI;

	pMOI->pFilen = NULL;
	res          = st_sysStrcpy(pFilen, &pMOI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	st_mpeg1_stc_initFHd(&pMOI->mhd);
	st_mpeg1_stc_initVI(&pMOI->mhdVbr);
	/* */
	st_mpeg1_stc_rsetMO_prot(pMO, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_mpeg1_stc_rsetMO(Tst_mpeg1_obj *pMO)
{
	st_mpeg1_stc_rsetMO_prot(pMO, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_mpeg1_stc_rsetMO_prot(Tst_mpeg1_obj *pMO,
		const Tst_bool resetOpts,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_mpeg1_obj_intn *pMOI;

	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	pMOI = (Tst_mpeg1_obj_intn*)pMO->pObInternal;

	if (resetStrrd)
		pMOI->pStrrd = pStrrd;
	if (resetFilen) {
		ST_ASSERTN_VOID(pFilen == NULL)

		if (st_sysStrcpy(pFilen, &pMOI->pFilen) != ST_ERR_SUCC) {
			pMOI->pFilen = NULL;  /* this is no good */

			ST_ASSERTN_VOID(pMOI->pFilen == NULL)  /* cause exit() here */
		}
	}
	st_mpeg1_stc_rsetFHd(&pMOI->mhd, ST_B_TRUE);
	st_mpeg1_stc_rsetVI(&pMOI->mhdVbr);
	pMOI->bitrateMax  = 0;
	pMOI->bitrateMost = 0;
	ST_AV_STC_RSETPT(pMOI->playt)
	/* */
	pMOI->hasMPEGstream = ST_B_FALSE;
	/* */
	st_mpeg1_stc_rsetFrms(&pMOI->frames);
	/* */
	if (resetOpts)
		st_mpeg1_stc_rsetOpts(&pMOI->opts);
}

void
st_mpeg1_stc_freeMO(Tst_mpeg1_obj *pMO)
{
	Tst_mpeg1_obj_intn *pMOI;

	ST_ASSERTN_VOID(pMO == NULL)

	if (pMO->pObInternal != NULL) {
		st_mpeg1_stc_rsetMO_prot(pMO, ST_B_TRUE,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		pMOI = (Tst_mpeg1_obj_intn*)pMO->pObInternal;
		/* */
		ST_DELPOINT(pMOI->pFilen)
		st_mpeg1_stc_freeFHd(&pMOI->mhd);
		st_mpeg1_stc_freeVI(&pMOI->mhdVbr);
		/* */
		ST_DELPOINT(pMO->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_mpeg1_opts */
void
st_mpeg1_stc_rsetOpts(Tst_mpeg1_opts *pOpts)
{
	ST_ASSERTN_VOID(pOpts == NULL)

	ST_RSETSTC_BASOPTS(pOpts->basOpts)
	pOpts->anlz      = ST_B_FALSE;
	pOpts->quickScan = ST_B_FALSE;
	/* */
	pOpts->decodeFull      = ST_B_FALSE;
	pOpts->isDecoderSet    = ST_B_FALSE;
	pOpts->isPCMclientSet  = ST_B_FALSE;
	pOpts->decodeBPS       = 0;
	pOpts->printDecUsedBPS = ST_B_FALSE;
	/* */
	pOpts->forceMLay = ST_MPEG1_LAYER_NONE;
	pOpts->forceMAv  = ST_MPEG1_AUDVERS_NONE;
	/* */
	pOpts->cbAnaMsg = NULL;
	/* */
	pOpts->cbDecHndNew     = NULL;
	pOpts->cbDecHndDel     = NULL;
	pOpts->cbDecSetOutpFmt = NULL;
	pOpts->cbDecOpenFeed   = NULL;
	pOpts->cbDecFeedInp    = NULL;
	pOpts->cbDecDecode     = NULL;
	pOpts->cbDecCloseFeed  = NULL;
	/* */
	pOpts->cbPCMprepareWriting = NULL;
	pOpts->cbPCMsetOutputFmt   = NULL;
	pOpts->cbPCMsamplesClient  = NULL;
	pOpts->cbPCMfinishWriting  = NULL;
	pOpts->pPCMclientObj       = NULL;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_mpeg1_fhd */
void
st_mpeg1_stc_rsetFHd(Tst_mpeg1_fhd *pFH, const Tst_bool resetFNr)
{
	ST_ASSERTN_VOID(pFH == NULL)

	ST_MPEG1_STC_RSETFHD_BUT_NOT_FRAMENR(pFH)
	if (resetFNr)
		pFH->fraNr = 0;
}

void
st_mpeg1_stc_copyFHd(const Tst_mpeg1_fhd *pSrc, Tst_mpeg1_fhd *pDst)
{
	ST_ASSERTN_VOID(pSrc == NULL || pDst == NULL)

	memcpy(pDst, pSrc, sizeof(Tst_mpeg1_fhd));
}

/*----------------------------------------------------------------------------*/

/* Tst_mpeg1_vbr */
void
st_mpeg1_stc_initVI(Tst_mpeg1_vbr *pVBR)
{
	st_mpeg1_stc_rsetVI(pVBR);
}

void
st_mpeg1_stc_rsetVI(Tst_mpeg1_vbr *pVBR)
{
	ST_ASSERTN_VOID(pVBR == NULL)

	pVBR->vbrHdType = ST_MPEG1_VBRTYP_NONE;
	pVBR->offs      = -1;
	pVBR->aFraCnt   = 0;
	pVBR->fsz       = 0;
	pVBR->qual      = 0;
	/* */
	pVBR->isOffsOK = ST_B_FALSE;
	pVBR->isCBR    = ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/* Tst_mpeg1_farnfh */
void
st_mpeg1_stc_initFar(Tst_mpeg1_farnfh *pFar)
{
	ST_ASSERTN_VOID(pFar == NULL)

	st_mpeg1_stc_initFHd(&pFar->mfhNew);
	/* */
	st_mpeg1_stc_rsetFar(pFar, ST_B_FALSE);
}

void
st_mpeg1_stc_rsetFar(Tst_mpeg1_farnfh *pFar,
		const Tst_bool resetOnlyOutp)
{
	ST_ASSERTN_VOID(pFar == NULL)

	if (! resetOnlyOutp)
		pFar->pMFHcomp = NULL;
	st_mpeg1_stc_rsetFHd(&pFar->mfhNew, ST_B_TRUE);
	/* */
	if (! resetOnlyOutp) {
		pFar->vbrTp         = ST_MPEG1_VBRTYP_NONE;
		pFar->isFreeFmtAllw = ST_B_FALSE;
	}
	/* */
	pFar->couldFindNxFra = ST_B_FALSE;
	pFar->isEOFreached   = ST_B_FALSE;
	pFar->isFreeFmt      = ST_B_FALSE;
	pFar->frOffs         = -1;
	pFar->frOffsDelta    = 0;
}

/*----------------------------------------------------------------------------*/

void
st_mpeg1_stc_rsetFrms(Tst_mpeg1_frames *pFrms)
{
	ST_ASSERTN_VOID(pFrms == NULL)

	pFrms->offsFrFirst      = -1;
	pFrms->offsFrFirstDelta = -1;
	pFrms->offsFrLast       = -1;
	pFrms->offsFrLastDelta  = -1;
	pFrms->cnt              = 0;
	/* */
	pFrms->errAtWrongOffsCnt     = 0;
	pFrms->errIsCBRwithVBR       = ST_B_FALSE;
	pFrms->errIsVBRwoHead        = ST_B_FALSE;
	pFrms->errIsVBRhdAtWrongOffs = ST_B_FALSE;
	pFrms->errWasFraCntCorrd     = ST_B_FALSE;
	pFrms->errWasBytCntCorrd     = ST_B_FALSE;
	pFrms->errIsStrIncompl       = ST_B_FALSE;
	/* */
	pFrms->totStrSz = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_mpeg1_decoder */
Tst_err
st_mpeg1_stc_initDec(Tst_mpeg1_decoder *pDec)
{
	pDec->pBufIn = NULL;
	/* */
	st_mpeg1_stc_rsetDec(pDec);
	return ST_ERR_SUCC;
}

void
st_mpeg1_stc_rsetDec(Tst_mpeg1_decoder *pDec)
{
	pDec->pDecHnd    = NULL;
	pDec->bufInSz    = 0;
	pDec->bufInUsed  = 0;
	pDec->feedFrames = 0;
	pDec->needMore   = ST_B_TRUE;
	ST_DELPOINT(pDec->pBufIn)
	/* */
	pDec->sampleR  = 0;
	pDec->channels = 0;
	pDec->bps      = 0;
	/* */
	pDec->pPCMstrHnd   = NULL;
	pDec->isOutpFmtSet = ST_B_FALSE;
}

void
st_mpeg1_stc_freeDec(Tst_mpeg1_decoder *pDec)
{
	st_mpeg1_stc_rsetDec(pDec);
}

Tst_err
st_mpeg1_stc_reszDecBufInp(Tst_mpeg1_decoder *pDec,
		const Tst_uint32 newSz)
{
	if (newSz > pDec->bufInSz) {
		ST_REALLOC(pDec->pBufIn, Tst_buf*, newSz, 1)
		if (pDec->pBufIn == NULL)
			return ST_ERR_OMEM;
		memset(&pDec->pBufIn[pDec->bufInSz], 0, newSz - pDec->bufInSz);
		pDec->bufInSz = newSz;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Initialize Bitrate-Counter array
 */
Tst_err
st_mpeg1_stc_initBrCounta(Tst_uint32 ***pppBrCnt)
{
	Tst_uint32 x;

	ST_ASSERTN_IARG(pppBrCnt == NULL)

	ST_CALLOC(*pppBrCnt, Tst_uint32**,
			ST_MPEG1_AMOUNT_BITRS, sizeof(Tst_uint32*))
	if (*pppBrCnt == NULL)
		return ST_ERR_OMEM;

	for (x = 0; x < ST_MPEG1_AMOUNT_BITRS; x++) {
		ST_CALLOC((*pppBrCnt)[x], Tst_uint32*, 2, sizeof(Tst_uint32))
		if ((*pppBrCnt)[x] == NULL)
			return ST_ERR_OMEM;
		(*pppBrCnt)[x][0] = 0;
		(*pppBrCnt)[x][1] = 0;
	}

	for (x = 0; x < 24; x++) {
		switch (x) {
		case  0: (*pppBrCnt)[x][0] =   8; break;
		case  1: (*pppBrCnt)[x][0] =  16; break;
		case  2: (*pppBrCnt)[x][0] =  24; break;
		case  3: (*pppBrCnt)[x][0] =  32; break;
		case  4: (*pppBrCnt)[x][0] =  40; break;
		case  5: (*pppBrCnt)[x][0] =  48; break;
		case  6: (*pppBrCnt)[x][0] =  56; break;

		case  7: (*pppBrCnt)[x][0] =  64; break;
		case  8: (*pppBrCnt)[x][0] =  80; break;
		case  9: (*pppBrCnt)[x][0] =  96; break;
		case 10: (*pppBrCnt)[x][0] = 112; break;
		case 11: (*pppBrCnt)[x][0] = 128; break;
		case 12: (*pppBrCnt)[x][0] = 144; break;
		case 13: (*pppBrCnt)[x][0] = 160; break;
		case 14: (*pppBrCnt)[x][0] = 176; break;

		case 15: (*pppBrCnt)[x][0] = 192; break;
		case 16: (*pppBrCnt)[x][0] = 224; break;
		case 17: (*pppBrCnt)[x][0] = 256; break;
		case 18: (*pppBrCnt)[x][0] = 288; break;
		case 19: (*pppBrCnt)[x][0] = 320; break;
		case 20: (*pppBrCnt)[x][0] = 352; break;
		case 21: (*pppBrCnt)[x][0] = 384; break;
		case 22: (*pppBrCnt)[x][0] = 416; break;
		default: (*pppBrCnt)[x][0] = 448;
		}
	}
	return ST_ERR_SUCC;
}

/*
 * Increase Bitrate-Counter
 */
void
st_mpeg1_stc_incBrCounta(Tst_uint32 **ppBrCnt, const Tst_uint32 br)
{
	ST_ASSERTN_VOID(ppBrCnt == NULL /*|| br > 448*/)

	switch (br) {
	case   8: ppBrCnt[ 0][1]++; break;
	case  16: ppBrCnt[ 1][1]++; break;
	case  24: ppBrCnt[ 2][1]++; break;
	case  32: ppBrCnt[ 3][1]++; break;
	case  40: ppBrCnt[ 4][1]++; break;
	case  48: ppBrCnt[ 5][1]++; break;
	case  56: ppBrCnt[ 6][1]++; break;

	case  64: ppBrCnt[ 7][1]++; break;
	case  80: ppBrCnt[ 8][1]++; break;
	case  96: ppBrCnt[ 9][1]++; break;
	case 112: ppBrCnt[10][1]++; break;
	case 128: ppBrCnt[11][1]++; break;
	case 144: ppBrCnt[12][1]++; break;
	case 160: ppBrCnt[13][1]++; break;
	case 176: ppBrCnt[14][1]++; break;

	case 192: ppBrCnt[15][1]++; break;
	case 224: ppBrCnt[16][1]++; break;
	case 256: ppBrCnt[17][1]++; break;
	case 288: ppBrCnt[18][1]++; break;
	case 320: ppBrCnt[19][1]++; break;
	case 352: ppBrCnt[20][1]++; break;
	case 384: ppBrCnt[21][1]++; break;
	case 416: ppBrCnt[22][1]++; break;
	case 448: ppBrCnt[23][1]++; break;
	default:
		/**st_mpeg1_prE("\t\tst_mpeg1_stc_incBrCounta(): "
				"invalid bitrate\n");**/
		ST_ASSERTN_VOID(ppBrCnt != NULL)  /* cause exit() here */
		break;
	}
}

/*
 * Free Bitrate-Counter
 */
void
st_mpeg1_stc_freeBrCounta(Tst_uint32 ***pppBrCnt)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pppBrCnt == NULL)

	for (x = 0; x < ST_MPEG1_AMOUNT_BITRS; x++)
		ST_DELPOINT((*pppBrCnt)[x])
	ST_DELPOINT(*pppBrCnt)
}

/******************************************************************************/
