/******************************************************************************/
/* ac_flac_stcs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Flac structures                                              */
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
#include "src/includes/common/sys_math.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_AC_FLAC_STCS_C
#include "ac_flac_stcs-prot.h"
/*** */
#include "../../utils/w-md5-prot.h"
#undef SRC_AC_FLAC_STCS_C

/*
// System-Includes
*/
#include <string.h>      /* memset() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacInfo */
void
st_contOgg_stc_rsetFlacInfo(Tst_contOgg_flacInfo *pFI)
{
	ST_ASSERTN_VOID(pFI == NULL)

	pFI->oggFlacVersMaj           = 0;
	pFI->oggFlacVersMin           = 0;
	pFI->sampleRate               = 0;
	pFI->channels                 = 0;
	pFI->bitsPerSample            = 0;
	pFI->md5OfDecodedAudioChecked = ST_B_FALSE;
	pFI->md5OfDecodedAudioOK      = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacOpts */
void
st_contOgg_stc_rsetFlacOpts(Tst_contOgg_flacOpts *pFO)
{
	ST_ASSERTN_VOID(pFO == NULL)

	pFO->readAll    = ST_B_TRUE;
	pFO->decodeFull = ST_B_FALSE;
	pFO->checkMD5da = ST_B_TRUE;
	pFO->checkCRCfh = ST_B_TRUE;
	pFO->checkCRCfr = ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flac */
Tst_err
st_contOgg_stc_initFlac(Tst_contOgg_flac *pAud)
{
	Tst_err res;

	ST_ASSERTN_IARG(pAud == NULL)

	res = st_contOgg_stc_initFlacIdent(&pAud->ident);
	if (res == ST_ERR_SUCC)
		res = st_contOgg_stc_initVorbCommRaw(&pAud->comm);
	if (res == ST_ERR_SUCC)
		st_contOgg_stc_initFlacAFra(&pAud->curAFra);
	if (res == ST_ERR_SUCC)
		res = st_contOgg_stc_initFDO(&pAud->decObj);

	if (res == ST_ERR_SUCC)
		st_contOgg_stc_rsetFlac(pAud);
	return res;
}

void
st_contOgg_stc_rsetFlac(Tst_contOgg_flac *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetFlacIdent(&pAud->ident);
	st_contOgg_stc_rsetVorbCommRaw(&pAud->comm);
	pAud->commIsLastHdPck = ST_B_FALSE;
	st_contOgg_stc_rsetFlacAFra(&pAud->curAFra, ST_B_TRUE);
	pAud->aPackCnt   = 0;
	pAud->aFramesCnt = 0;
	st_contOgg_stc_rsetFDO(&pAud->decObj);
}

void
st_contOgg_stc_freeFlac(Tst_contOgg_flac *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetFlac(pAud);
	/* */
	st_contOgg_stc_freeFlacIdent(&pAud->ident);
	st_contOgg_stc_freeVorbCommRaw(&pAud->comm);
	st_contOgg_stc_freeFlacAFra(&pAud->curAFra);
	st_contOgg_stc_freeFDO(&pAud->decObj);
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacIdent */
Tst_err
st_contOgg_stc_initFlacIdent(Tst_contOgg_flacIdent *pID)
{
	Tst_err res;

	ST_ASSERTN_IARG(pID == NULL)

	res = st_md5_stc_initCtx(&pID->md5_bCtx);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	st_contOgg_stc_rsetFlacIdent(pID);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetFlacIdent(Tst_contOgg_flacIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->isSet  = ST_B_FALSE;
	pID->packNr = 0;
	/* */
	pID->verMaj        = 0;
	pID->verMin        = 0;
	pID->hdPcks_fromHd = 0;
	pID->hdPcks_read   = 0;
	/* */
	pID->blkSzMin = 0;
	pID->blkSzMax = 0;
	pID->frSzMin  = 0;
	pID->frSzMax  = 0;
	pID->sampleR  = 0;
	pID->chann    = 0;
	pID->bitsPerS = 0;
	ST_SYSMATH_STC_RSETUI64(pID->samplesTot_fromHd)
	ST_SYSMATH_STC_RSETUI64(pID->samplesTot_read)
	/* */
	pID->isMD5set          = ST_B_FALSE;
	pID->md5OfDecAudioChkd = ST_B_FALSE;
	pID->md5OfDecAudioOK   = ST_B_FALSE;
	memset(&pID->md5_a, 0, sizeof(pID->md5_a));
	memset(&pID->md5_b, 0, sizeof(pID->md5_b));
	st_md5_stc_rsetCtx(&pID->md5_bCtx);
	/* */
	pID->offsAbs1st = -1;
}

void
st_contOgg_stc_freeFlacIdent(Tst_contOgg_flacIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	st_contOgg_stc_rsetFlacIdent(pID);
	/* */
	st_md5_stc_freeCtx(&pID->md5_bCtx);
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacAudFrame */
void
st_contOgg_stc_initFlacAFra(Tst_contOgg_flacAudFrame *pFra)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pFra == NULL)

	st_contOgg_stc_initFlacFH(&pFra->fh);
	for (x = 0; x < 8; x++)
		st_contOgg_stc_initFlacSubFr(&pFra->sfr[x]);
	st_contOgg_stc_initFlacRSamps(&pFra->rawSamp);
	st_contOgg_stc_initFlacTempSF_verbArr(&pFra->tempSF_verbArr);
	pFra->tempSF_residEParsSz = 0;
	pFra->pTempSF_residEPars  = NULL;
	/* */
	st_contOgg_stc_rsetFlacAFra(pFra, ST_B_FALSE);
}

void
st_contOgg_stc_rsetFlacAFra(Tst_contOgg_flacAudFrame *pFra,
		const Tst_bool freeBufs)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pFra == NULL)

	st_contOgg_stc_rsetFlacFH(&pFra->fh);
	for (x = 0; x < 8; x++) {
		if (! pFra->sfr[x].wasUsed)
			break;
		st_contOgg_stc_rsetFlacSubFr(&pFra->sfr[x], freeBufs);
	}
	if (freeBufs) {
		st_contOgg_stc_rsetFlacRSamps(&pFra->rawSamp);
		st_contOgg_stc_rsetFlacTempSF_verbArr(&pFra->tempSF_verbArr);
		pFra->tempSF_residEParsSz = 0;
		ST_DELPOINT(pFra->pTempSF_residEPars)
	} else if (pFra->pTempSF_residEPars != NULL)
		memset(pFra->pTempSF_residEPars, 0, pFra->tempSF_residEParsSz);
	/* */
	pFra->pCSFr = NULL;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacFH */
void
st_contOgg_stc_rsetFlacFH(Tst_contOgg_flacFH *pFH)
{
	ST_ASSERTN_VOID(pFH == NULL)

	pFH->sampleSz        = 0;
	pFH->crc8_a          = 0;
	pFH->crc8_b          = 0;
	pFH->crc16_a         = 0;
	pFH->crc16_b         = 0;
	pFH->isBlockStratVar = ST_B_FALSE;
	pFH->sampleR         = 0;
	pFH->blkSz           = 0;
	pFH->channels        = 0;
	pFH->channAss        = ST_CONTOGG_FLAC_CA_NONE;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacSubFr */
void
st_contOgg_stc_initFlacSubFr(Tst_contOgg_flacSubFr *pSFr)
{
	ST_ASSERTN_VOID(pSFr == NULL)

	pSFr->rawResid.pResids  = NULL;
	pSFr->rawResid.maxBlkSz = 0;
	/* */
	st_contOgg_stc_rsetFlacSubFr(pSFr, ST_B_FALSE);
}

void
st_contOgg_stc_rsetFlacSubFr(Tst_contOgg_flacSubFr *pSFr,
		const Tst_bool freeBuf)
{
	ST_ASSERTN_VOID(pSFr == NULL)

	pSFr->wasUsed    = ST_B_FALSE;
	pSFr->frameNr    = 0;
	pSFr->wastedBits = 0;
	pSFr->predOrder  = 0;
	pSFr->channel    = 0;
	pSFr->sampleSz   = 0;
	pSFr->subFrTp    = ST_CONTOGG_FLAC_SFRTP_NONE;
	/* */
	if (freeBuf) {
		ST_DELPOINT(pSFr->rawResid.pResids)
		pSFr->rawResid.maxBlkSz = 0;
	}
	pSFr->rawResid.cnt = 0;
}

Tst_err
st_contOgg_stc_reszFlacSubFrResid(Tst_contOgg_flacSubFr *pSFr,
		const Tst_uint32 samplesPerChann)
{
	ST_ASSERTN_IARG(pSFr == NULL)

	if (samplesPerChann > pSFr->rawResid.maxBlkSz) {
		ST_REALLOC(pSFr->rawResid.pResids, Tst_int32*,
				samplesPerChann, sizeof(Tst_int32))
		if (pSFr->rawResid.pResids == NULL)
			return ST_ERR_OMEM;
		pSFr->rawResid.maxBlkSz = samplesPerChann;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacRawSamples */
void
st_contOgg_stc_initFlacRSamps(Tst_contOgg_flacRawSamples *pSam)
{
	ST_ASSERTN_VOID(pSam == NULL)

	pSam->maxChannCnt  = 0;
	pSam->maxSamPerCha = 0;
	pSam->ppSamples = NULL;
	/* */
	st_contOgg_stc_rsetFlacRSamps(pSam);
}

void
st_contOgg_stc_rsetFlacRSamps(Tst_contOgg_flacRawSamples *pSam)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pSam == NULL)

	if (pSam->ppSamples != NULL) {
		for (x = 0; x < pSam->maxChannCnt; x++)
			ST_DELPOINT(pSam->ppSamples[x])
		ST_DELPOINT(pSam->ppSamples)
	}
	pSam->maxChannCnt  = 0;
	pSam->maxSamPerCha = 0;
}

Tst_err
st_contOgg_stc_reszFlacRSamps(Tst_contOgg_flacRawSamples *pSam,
		const Tst_uint32 channels, const Tst_uint32 samplesPerChann)
{
	Tst_uint32 x;

	ST_ASSERTN_IARG(pSam == NULL)

	if (pSam->ppSamples == NULL) {
		ST_CALLOC(pSam->ppSamples, Tst_int32**, channels, sizeof(Tst_int32*))
		if (pSam->ppSamples == NULL)
			return ST_ERR_OMEM;
		for (x = 0; x < channels; x++)
			pSam->ppSamples[x] = NULL;
		/* */
		pSam->maxChannCnt = channels;
	} else if (pSam->maxChannCnt != channels)
		return ST_ERR_FAIL;

	if (samplesPerChann > pSam->maxSamPerCha) {
		for (x = 0; x < channels; x++) {
			ST_REALLOC(pSam->ppSamples[x], Tst_int32*,
					samplesPerChann, sizeof(Tst_int32))
			if (pSam->ppSamples[x] == NULL)
				return ST_ERR_OMEM;
		}
		pSam->maxSamPerCha = samplesPerChann;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacTempSF_verbArr */
void
st_contOgg_stc_initFlacTempSF_verbArr(Tst_contOgg_flacTempSF_verbArr *pSFV)
{
	ST_ASSERTN_VOID(pSFV == NULL)

	pSFV->pI32arr = NULL;
	/* */
	st_contOgg_stc_rsetFlacTempSF_verbArr(pSFV);
}

void
st_contOgg_stc_rsetFlacTempSF_verbArr(Tst_contOgg_flacTempSF_verbArr *pSFV)
{
	ST_ASSERTN_VOID(pSFV == NULL)

	ST_DELPOINT(pSFV->pI32arr)
	pSFV->maxBlkSz = 0;
}

Tst_err
st_contOgg_stc_reszFlacTempSF_verbArr(Tst_contOgg_flacTempSF_verbArr *pSFV,
		const Tst_uint32 blkSz)
{
	ST_ASSERTN_IARG(pSFV == NULL)

	if (blkSz > pSFV->maxBlkSz) {
		ST_REALLOC(pSFV->pI32arr, Tst_int32*,
					blkSz, sizeof(Tst_int32))
		if (pSFV->pI32arr == NULL)
			return ST_ERR_OMEM;
		pSFV->maxBlkSz = blkSz;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_flacDecObj */
Tst_err
st_contOgg_stc_initFDO(Tst_contOgg_flacDecObj *pDec)
{
	ST_ASSERTN_IARG(pDec == NULL)

	st_contOgg_stc_rsetFDO(pDec);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetFDO(Tst_contOgg_flacDecObj *pDec)
{
	ST_ASSERTN_VOID(pDec == NULL)

	pDec->pPCMstrHnd   = NULL;
	pDec->isOutpFmtSet = ST_B_FALSE;
}

void
st_contOgg_stc_freeFDO(Tst_contOgg_flacDecObj *pDec)
{
	ST_ASSERTN_VOID(pDec == NULL)

	st_contOgg_stc_rsetFDO(pDec);
}

/******************************************************************************/
