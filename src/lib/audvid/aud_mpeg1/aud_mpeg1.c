/******************************************************************************/
/* aud_mpeg1.c                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for MPEG-1 audio                                                 */
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
#include "src/includes/common/streamrd.h"
#include "src/includes/audvid/aud_mpeg1.h"
/** */
#define SRC_AUD_MPEG1_C
#include "aud_mpeg1-prot.h"
/*** */
#include "aud_mpeg1_stcs-prot.h"
#include "aud_mpeg1_zfdeb-prot.h"
#include "aud_mpeg1_zrd-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_AUD_MPEG1_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_MPEG1__readFrame(Tst_mpeg1_obj_intn *pMOI, const char *pFnc,
                    const Tst_bool decToPCM, Tst_bool *pWasLastVBRhd,
                    Tst_uint32 *pOrgBrt,
                    Tst_mpeg1_decoder *pDecObj, Tst_mpeg1_farnfh *pNFI,
                    Tst_uint32 **ppBrCnt);
/** */
static Tst_err
ST_MPEG1__setOutputFmt(Tst_mpeg1_obj_intn *pMOI, const char *pFnc,
                       Tst_mpeg1_decoder *pDecObj);
/** */
static Tst_err
ST_MPEG1__decodeToPCM(Tst_mpeg1_obj_intn *pMOI, const char *pFnc,
                      const Tst_bool isLastCall,
                      Tst_mpeg1_decoder *pDecObj);
/** */
static void
ST_MPEG1__calcAvgBitrateEtc(Tst_mpeg1_obj_intn *pMOI,
                            const Tst_uint32 **ppBrCnt);
static void
ST_MPEG1__calcAudioSizeEtc(Tst_mpeg1_obj_intn *pMOI);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read MPEG stream
 *
 * pMO           MUST be set
 * pMOI->pStrrd  MUST be set
 *
 * returns: ERRCODE
 *          updates pMO
 */
Tst_err
st_mpeg1_readStream(Tst_mpeg1_obj *pMO)
{
	const char *cFNCN = __func__;
	Tst_err    res          = ST_ERR_SUCC;
	Tst_uint32 **ppBrCnt    = NULL,
	           orgBrt       = 0;
	Tst_foffs  offsBeg,               /* offset of beginning of stream */
	           offsEnd;
	Tst_bool   decToPCM     = ST_B_FALSE,
	           wasLastVBRhd = ST_B_FALSE;  /* did last frame contain VBR head. ? */
	Tst_mpeg1_farnfh   nfi;       /* new frame header info */
	Tst_mpeg1_obj_intn *pMOI;
	Tst_mpeg1_decoder  decObj;

	ST_ASSERTN_IARG(pMO == NULL || pMO->pObInternal == NULL)

	st_mpeg1_stc_rsetMO_prot(pMO, ST_B_FALSE,
			ST_B_FALSE, NULL, ST_B_FALSE, NULL);

	pMOI = (Tst_mpeg1_obj_intn*)pMO->pObInternal;
	ST_ASSERTN_IARG(pMOI->pFilen == NULL || pMOI->pStrrd == NULL)

	if (ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
		st_mpeg1_d_deb(&pMOI->opts, cFNCN,
				"reading MPEG-1 stream...");

	st_mpeg1_stc_initDec(&decObj);

	/* init nfi */
	st_mpeg1_stc_initFar(&nfi);
	nfi.isFreeFmtAllw = ST_B_TRUE;

	/* initialize pcm writer */
	if (pMOI->opts.isPCMclientSet) {
		res = pMOI->opts.cbPCMprepareWriting(pMOI->opts.pPCMclientObj,
				pMOI->pFilen, 1, 1, ST_B_FALSE, &decObj.pPCMstrHnd);
		if (res != ST_ERR_SUCC) {
			st_mpeg1_stc_freeFar(&nfi);
			st_mpeg1_stc_freeDec(&decObj);
			/* */
			st_mpeg1_d_err(&pMOI->opts, cFNCN, pMOI->pFilen,
					"initialization of PCM client failed");
			return res;
		}
	}

	/* init bitrate counter */
	res = st_mpeg1_stc_initBrCounta(&ppBrCnt);
	if (res != ST_ERR_SUCC) {
		if (pMOI->opts.isPCMclientSet)
			pMOI->opts.cbPCMfinishWriting(decObj.pPCMstrHnd);
		st_mpeg1_stc_freeFar(&nfi);
		st_mpeg1_stc_freeDec(&decObj);
		return res;
	}

	/* initialize decoder */
	if (pMOI->opts.decodeFull && pMOI->opts.isDecoderSet) {
		res = pMOI->opts.cbDecHndNew(&decObj.pDecHnd, pMOI->opts.printDecUsedBPS);
		if (res != ST_ERR_SUCC)
			decObj.pDecHnd = NULL;
		else
			res = pMOI->opts.cbDecOpenFeed(decObj.pDecHnd);
		if (res != ST_ERR_SUCC) {
			if (pMOI->opts.isPCMclientSet)
				pMOI->opts.cbPCMfinishWriting(decObj.pPCMstrHnd);
			if (decObj.pDecHnd != NULL)
				pMOI->opts.cbDecHndDel(&decObj.pDecHnd);
			st_mpeg1_stc_freeFar(&nfi);
			st_mpeg1_stc_freeBrCounta(&ppBrCnt);
			st_mpeg1_stc_freeDec(&decObj);
			/* */
			st_mpeg1_d_err(&pMOI->opts, cFNCN, pMOI->pFilen,
					"initialization of decoder failed");
			return res;
		}
		decToPCM = ST_B_TRUE;
	}

	/* */
	offsBeg = st_streamrd_getCurPos(pMOI->pStrrd);

	while (res == ST_ERR_SUCC) {
		/* find and read next frame header */
		res = st_mpeg1_rd_findAndReadNextFrHd(pMOI, &nfi);
		if (res != ST_ERR_SUCC) {
			st_mpeg1_d_err(&pMOI->opts, cFNCN, pMOI->pFilen,
					"finding and reading next frame header failed");
			break;
		}
		/* eval result */
		if (! nfi.couldFindNxFra || nfi.isEOFreached) {
			if (ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
				st_mpeg1_d_deb(&pMOI->opts, cFNCN,
						"found end of stream");
			break;
		}

		/* we've found a valid MPEG Frameheader */

		/* read frame */
		res = ST_MPEG1__readFrame(pMOI, cFNCN, decToPCM,
				&wasLastVBRhd, &orgBrt, &decObj, &nfi, ppBrCnt);
		if (res == ST_ERR_ABRT) {
			res = ST_ERR_SUCC;
			break;
		}
		/* if we're doing a quick scan stop after the first 5 frames */
		if (pMOI->opts.quickScan && pMOI->frames.cnt >= 5) {
			if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts))
				st_mpeg1_d_deb(&pMOI->opts, cFNCN,
						"stopping here since we're only doing a quick scan");
			break;
		}
	}

	/* decode remaining frames */
	if (res == ST_ERR_SUCC && decToPCM) {
		res = ST_MPEG1__decodeToPCM(pMOI, cFNCN, ST_B_TRUE, &decObj);
	}

	/* free decoder */
	if (decToPCM) {
		pMOI->opts.cbDecCloseFeed(decObj.pDecHnd);
		pMOI->opts.cbDecHndDel(decObj.pDecHnd);
	}

	/* finish PCM writer */
	if (pMOI->opts.isPCMclientSet)
		pMOI->opts.cbPCMfinishWriting(decObj.pPCMstrHnd);

	if (res == ST_ERR_SUCC) {
		/* update total stream size */
		offsEnd               = st_streamrd_getCurPos(pMOI->pStrrd);
		pMOI->frames.totStrSz = (Tst_fsize)(offsEnd >= offsBeg ?
				offsEnd - offsBeg : 0);

		if (pMOI->mhdVbr.vbrHdType == ST_MPEG1_VBRTYP_NONE &&
				! pMOI->mhdVbr.isCBR) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
				st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, cFNCN,
						"no CBR/VBR header present");
			/* */
			pMOI->frames.errIsVBRwoHead = ST_B_TRUE;
		}

		if (pMOI->frames.errIsStrIncompl) {
			if (pMOI->frames.cnt > 0)
				--pMOI->frames.cnt;
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
				st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, cFNCN,
						"stream ended where it shouldn't");
		}

		/* calc avg, maximum and most used bitrate
		 *   and output some analyzing info  */
		ST_MPEG1__calcAvgBitrateEtc(pMOI, (const Tst_uint32**)ppBrCnt);
		/* calc total amount of audio bytes and frames
		 *   and output some analyzing info  */
		ST_MPEG1__calcAudioSizeEtc(pMOI);
		/* output amount of erroneous frame offsets */
		if (pMOI->frames.errAtWrongOffsCnt > 0 &&
				ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts)) {
			st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_TRUE, cFNCN,
					"(amount of frames at wrong offset= %u)",
					pMOI->frames.errAtWrongOffsCnt);
		}
		/* compute playtime */
		st_mpeg1_rd_computePlaytime(pMOI);
	}

	if (res == ST_ERR_SUCC &&
			ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
		st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_TRUE, cFNCN,
				"end of MPEG-1 stream");

	st_mpeg1_stc_freeFar(&nfi);
	st_mpeg1_stc_freeBrCounta(&ppBrCnt);
	st_mpeg1_stc_freeDec(&decObj);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Handle current MPEG frame
 */
static Tst_err
ST_MPEG1__readFrame(Tst_mpeg1_obj_intn *pMOI, const char *pFnc,
		const Tst_bool decToPCM, Tst_bool *pWasLastVBRhd,
		Tst_uint32 *pOrgBrt,
		Tst_mpeg1_decoder *pDecObj, Tst_mpeg1_farnfh *pNFI,
		Tst_uint32 **ppBrCnt)
{
	Tst_err    res;
	Tst_uint32 toRdOrSkip;
	Tst_bool   abortAnyway = ST_B_FALSE;

	/* save frame's offset */
	/** save first frame's offset */
	if (pMOI->frames.cnt == 1) {
		pMOI->frames.offsFrFirst      = pNFI->frOffs;
		pMOI->frames.offsFrFirstDelta = pNFI->frOffsDelta;
	}
	/** save last frame's offset */
	pMOI->frames.offsFrLast      = pNFI->frOffs;
	pMOI->frames.offsFrLastDelta = pNFI->frOffsDelta;

	if (! pNFI->mfhNew.isOffsOK) {
		if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
			st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, pFnc,
					"Frameheader at wrong offset (d "ST_TFOFFS_PRF_LD")",
					(Tst_foffs)pNFI->frOffsDelta);
		/* */
		++pMOI->frames.errAtWrongOffsCnt;
	} /**else
		st_mpeg1_prf("\t\tst_mpeg1_readStream: "
				"found Frameheader at offset "ST_TFOFFS_PRF_LD", "
				"frameSz= %u\n",
				(Tst_foffs)pNFI->frOffs, pNFI->mfhNew.frameSz);**/

	if (pMOI->frames.cnt == 1 || *pWasLastVBRhd) {
		pMOI->hasMPEGstream = ST_B_TRUE;
		pMOI->isFreeFmt     = pMOI->isFreeFmt || pNFI->isFreeFmt;

		/* if we've found Free-Format stop here
		 *   (it's not possible to determine frame lengths
		 *    in Free-Format MPEGs yet)  */
		if (pMOI->isFreeFmt) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
				st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, pFnc,
						"found Free-Format MPEG stream");
			if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts))
				st_mpeg1_d_deb(&pMOI->opts, pFnc,
						"aborting...");
			return ST_ERR_ABRT;
		}

		/* what type of frame do we have here ? */
		res = st_mpeg1_rd_checkFrameType(pMOI, &pNFI->mfhNew);
		if (res != ST_ERR_SUCC) {
			if (res == ST_ERR_FEOF) {
				res = ST_ERR_ABRT;
				/* */
				pMOI->frames.errIsStrIncompl = ST_B_TRUE;
			} else
				st_mpeg1_d_err(&pMOI->opts, pFnc, pMOI->pFilen,
						"checking frame type failed");
			return res;
		}
		if (pNFI->mfhNew.hasVBRhd) {
			*pWasLastVBRhd = ST_B_TRUE;
			/* */
			if (pMOI->opts.anlz && ! pMOI->mhdVbr.isOffsOK &&
					ST_AVFDEB_HASCB_ANAL_D(pMOI->opts))
				pMOI->opts.cbAnaMsg("AUD: ! CBR/VBR header at wrong offset");
			/* */
			pMOI->mhdVbr.isCBR = (pMOI->mhdVbr.vbrHdType == ST_MPEG1_VBRTYP_INFO ||
					pMOI->mhdVbr.vbrHdType == ST_MPEG1_VBRTYP_NONE);
		} else {
			/* set main frame header info */
			*pWasLastVBRhd = ST_B_FALSE;
			st_mpeg1_stc_copyFHd(&pNFI->mfhNew, &pMOI->mhd);
			pNFI->pMFHcomp = &pMOI->mhd;
			*pOrgBrt       = pMOI->mhd.bitrate;
			/* set output format for decoder */
			if (decToPCM) {
				res = ST_MPEG1__setOutputFmt(pMOI, pFnc, pDecObj);
				if (res != ST_ERR_SUCC)
					return res;
			}
		}
	}

	/* goto next frameheader
	 * OR read entire frame for decoding
	 *   (currently we're behind the frameheader,
	 *    frameSz includes ST_MPEG1_FR_HD_SZ)  */
	toRdOrSkip = (Tst_uint32)(pNFI->mfhNew.frameSz - ST_MPEG1_FR_HD_SZ);
	if (toRdOrSkip > 32 &&
			((pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE &&
					pMOI->frames.cnt >= pMOI->mhdVbr.aFraCnt) ||
				(pMOI->mhdVbr.vbrHdType == ST_MPEG1_VBRTYP_NONE))) {
		Tst_buf    tmpLastBuf[ST_MPEG1_FR_SZ_MAX];
		Tst_uint32 tmpLastRd = 0;

		ST_ASSERTN_NUM(ST_ERR_FAIL, toRdOrSkip > sizeof(tmpLastBuf))

		/* sometimes the last frame is supposed to also contain
		 *   the ID3v1 tag, to avoid skipping it here,
		 *   we check if that's the case and then correct some values  */

		/**if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts)) {
			if (pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE)
				st_mpeg1_d_deb(&pMOI->opts, pFnc, "checking last frame for IV1");
			else
				st_mpeg1_d_deb(&pMOI->opts, pFnc, "checking frame for IV1");
		}**/
		res = st_streamrd_rdAheadBuffer(pMOI->pStrrd,
					(toRdOrSkip <= sizeof(tmpLastBuf) ? toRdOrSkip : sizeof(tmpLastBuf)),
					tmpLastBuf, &tmpLastRd);
		/* check for ID3v1 */
		if (res == ST_ERR_SUCC && tmpLastRd >= 128 && tmpLastRd == toRdOrSkip) {
			if (memcmp(&tmpLastBuf[tmpLastRd - 128], "TAG", 3) == 0) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
					st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, pFnc,
							"last frame includes ID3v1 tag");
				toRdOrSkip -= 128;
			}
		} else if (res == ST_ERR_SUCC && tmpLastRd + 128 == toRdOrSkip) {
			/* seems like ID3v1 tag was originally included in last frame
			 *   but is now missing  */
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
				st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, pFnc,
						"last frame probably should've contained a ID3v1 tag, "
						"but it doesn't");
			toRdOrSkip -= 128;
		} else if (res == ST_ERR_SUCC && tmpLastRd >= 128) {
			if (memcmp(&tmpLastBuf[tmpLastRd - 128], "TAG", 3) == 0) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
					st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, pFnc,
							"last frame is incomplete but includes ID3v1 tag");
				toRdOrSkip  = tmpLastRd - 128;
				abortAnyway = ST_B_TRUE;
			}
		}
		/* check for Lame version */
		/**{
			TODO
			st_mpeg1_prf("Frame:\n");
			st_mpeg1_prBuf(tmpLastBuf, tmpLastRd);
			st_mpeg1_prf("\n");
		}**/
		res = ST_ERR_SUCC;  /* ignore any error here */
	}
	if (decToPCM && ! *pWasLastVBRhd) {
		if (pNFI->mfhNew.frameSz > pDecObj->bufInSz) {
			res = st_mpeg1_stc_reszDecBufInp(pDecObj, pNFI->mfhNew.frameSz);
			if (res != ST_ERR_SUCC)
				return res;
		}
		memcpy(pDecObj->pBufIn, pNFI->mfhNew.rawFHd, ST_MPEG1_FR_HD_SZ);
		res = st_streamrd_rdBuffer_noCRC(pMOI->pStrrd, toRdOrSkip,
				&pDecObj->pBufIn[ST_MPEG1_FR_HD_SZ], &pDecObj->bufInUsed);
		if (res == ST_ERR_SUCC && pDecObj->bufInUsed != toRdOrSkip)
			res = ST_ERR_FEOF;
		/* */
		pDecObj->bufInUsed += ST_MPEG1_FR_HD_SZ;
	} else {
		res = st_streamrd_rdSkipBytes(pMOI->pStrrd, toRdOrSkip, ST_B_TRUE);
	}
	if (res != ST_ERR_SUCC || abortAnyway) {
		if (res == ST_ERR_FEOF) {
			res = ST_ERR_ABRT;
			/* */
			pMOI->frames.errIsStrIncompl = ST_B_TRUE;
		} else if (! abortAnyway)
			st_mpeg1_d_err(&pMOI->opts, pFnc, pMOI->pFilen,
					"reading input bytes failed");
		return res;
	}

	if (! *pWasLastVBRhd) {
		/* check whether bitrates differ */
		if (pMOI->mhdVbr.isCBR && pNFI->mfhNew.bitrate != *pOrgBrt) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
				st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, pFnc,
						"found differing bitrate --> VBR");
			pMOI->mhdVbr.isCBR = ST_B_FALSE;
			/* */
			pMOI->frames.errIsCBRwithVBR = ST_B_TRUE;
		}
		/* update bitrate counter */
		st_mpeg1_stc_incBrCounta(ppBrCnt, pNFI->mfhNew.bitrate);

		/* decode frame */
		if (decToPCM) {
			res = pMOI->opts.cbDecFeedInp(pDecObj->pDecHnd,
					pDecObj->pBufIn, pDecObj->bufInUsed);
			if (res != ST_ERR_SUCC) {
				st_mpeg1_d_err(&pMOI->opts, pFnc, pMOI->pFilen,
						"feeding frame failed");
				return res;
			}

			if (++pDecObj->feedFrames >= 5) {
				res = ST_MPEG1__decodeToPCM(pMOI, pFnc, ST_B_FALSE, pDecObj);
				if (res != ST_ERR_SUCC)
					return res;
			}
		}
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Set output format for decoder and PCM client
 */
static Tst_err
ST_MPEG1__setOutputFmt(Tst_mpeg1_obj_intn *pMOI, const char *pFnc,
		Tst_mpeg1_decoder *pDecObj)
{
	Tst_err res;

	pDecObj->channels = (pMOI->mhd.channMode == ST_MPEG1_CMODE_STEREO ||
			pMOI->mhd.channMode == ST_MPEG1_CMODE_JSTEREO ||
			pMOI->mhd.channMode == ST_MPEG1_CMODE_DUAL ? 2 : 1);
	pDecObj->sampleR  = pMOI->mhd.samplerate;
	/* set output format for decoder */
	res = pMOI->opts.cbDecSetOutpFmt(pDecObj->pDecHnd,
			pDecObj->sampleR, pDecObj->channels,
			pMOI->opts.decodeBPS, &pDecObj->bps);
	if (res != ST_ERR_SUCC) {
		st_mpeg1_d_err(&pMOI->opts, pFnc, pMOI->pFilen,
				"setting decoder output format failed");
		return res;
	}
	/* set output format for PCM client */
	if (pMOI->opts.isPCMclientSet && ! pDecObj->isOutpFmtSet) {
		res = pMOI->opts.cbPCMsetOutputFmt(pDecObj->pPCMstrHnd,
				pDecObj->sampleR, pDecObj->bps,
				0, pDecObj->channels);
		if (res != ST_ERR_SUCC) {
			st_mpeg1_d_err(&pMOI->opts, pFnc, pMOI->pFilen,
					"setting PCM client output format failed");
			return res;
		}
		pDecObj->isOutpFmtSet = ST_B_TRUE;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_MPEG1__decodeToPCM(Tst_mpeg1_obj_intn *pMOI, const char *pFnc,
		const Tst_bool isLastCall, Tst_mpeg1_decoder *pDecObj)
{
	Tst_err res;

	res = pMOI->opts.cbDecDecode(pDecObj->pDecHnd,
			isLastCall, &pDecObj->needMore,
			(pMOI->opts.isPCMclientSet ?
				pMOI->opts.cbPCMsamplesClient : NULL),
			(pMOI->opts.isPCMclientSet ? pDecObj->pPCMstrHnd : NULL));
	if (res != ST_ERR_SUCC)
		st_mpeg1_d_err(&pMOI->opts, pFnc, pMOI->pFilen,
				"decoding frame failed");

	pDecObj->feedFrames = 0;

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Determine average, maximum and most used bitrate
 *   and output analyzing info
 */
static void
ST_MPEG1__calcAvgBitrateEtc(Tst_mpeg1_obj_intn *pMOI,
		const Tst_uint32 **ppBrCnt)
{
	Tst_int32  most   = -1;
	Tst_uint32 x,
	           mostX  = 0,
	           deltaFCnt;
	double     avgBtr = 0.0;
	char       debMsg[512];

	ST_ASSERTN_VOID(pMOI == NULL || ppBrCnt == NULL)

	pMOI->mhd.bitrate = 0;
	pMOI->bitrateMost = 0;
	pMOI->bitrateMax  = 0;

	if (pMOI->opts.quickScan)
		return;

	deltaFCnt = (pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE ? 1 : 0);
	if (pMOI->frames.cnt == 0)
		deltaFCnt = 0;

	if (pMOI->frames.cnt > 0) {
		if (pMOI->opts.anlz && ST_AVFDEB_HASCB_ANAL_D(pMOI->opts))
			pMOI->opts.cbAnaMsg("AUD:  __bitrate__  _count_  _percent_");

		for (x = 0; x < ST_MPEG1_AMOUNT_BITRS; x++) {
			if (ppBrCnt[x][1] == 0)
				continue;
			if ((Tst_int32)ppBrCnt[x][1] > most) {
				most  = (Tst_int32)ppBrCnt[x][1];
				mostX = x;
			}
			/* update max. bitrate */
			if (ppBrCnt[x][0] > pMOI->bitrateMax)
				pMOI->bitrateMax = ppBrCnt[x][0];
			/* output some info */
			if (pMOI->opts.anlz &&
					ST_AVFDEB_HASCB_ANAL_D(pMOI->opts))
				pMOI->opts.cbAnaMsg("      %3u kbit/s:  %7u    %6.2f%%",
						ppBrCnt[x][0], ppBrCnt[x][1],
						((double)ppBrCnt[x][1] /
							(double)(pMOI->frames.cnt - deltaFCnt)) * 100.0);
			/* */
			avgBtr += ((double)(ppBrCnt[x][0] * ppBrCnt[x][1])/
							(double)(pMOI->frames.cnt - deltaFCnt));
		}
		if (most >= 0)
			pMOI->bitrateMost = ppBrCnt[mostX][0];

		if (! pMOI->mhdVbr.isCBR) {
			pMOI->mhd.bitrate = (Tst_uint32)avgBtr;
			if ((Tst_uint32)(avgBtr + (double)0.5) > (Tst_uint32)avgBtr)
				++pMOI->mhd.bitrate;
		} else
			pMOI->mhd.bitrate = pMOI->bitrateMost;

		/* output some info */
		if (pMOI->opts.anlz &&
				ST_AVFDEB_HASCB_ANAL_D(pMOI->opts)) {
			debMsg[0] = 0;
			if (! pMOI->mhdVbr.isCBR)
				snprintf(debMsg, sizeof(debMsg),
						", most used bitrate= %u, max bitrate used= %u",
						pMOI->bitrateMost, pMOI->bitrateMax);
			pMOI->opts.cbAnaMsg("AUD: %s Bitrate= %u%s",
					(pMOI->mhdVbr.isCBR ? "CBR" : "VBR"), pMOI->mhd.bitrate,
					debMsg);
		}
	}
}

/*
 * Calc total amount of audio bytes and frames
 *   and output some analyzing info
 */
static void
ST_MPEG1__calcAudioSizeEtc(Tst_mpeg1_obj_intn *pMOI)
{
	const char *cFNCN = __func__;
	Tst_uint32 deltaFCnt,
	           tmpUI;
	char       debMsg[512];

	ST_ASSERTN_VOID(pMOI == NULL)

	if (pMOI->opts.quickScan) {
		pMOI->mhdVbr.aFraCnt = 0;
		pMOI->mhdVbr.fsz     = 0;
		return;
	}

	deltaFCnt = (pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE ? 1 : 0);
	if (pMOI->frames.cnt == 0)
		deltaFCnt = 0;

	/* correct amount of frames */
	if (pMOI->mhdVbr.aFraCnt != pMOI->frames.cnt - deltaFCnt) {
		if (pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE &&
				ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts)) {
			st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, cFNCN,
					"correcting frame count %u -> %u",
					pMOI->mhdVbr.aFraCnt, pMOI->frames.cnt - deltaFCnt);
		}
		/* */
		pMOI->mhdVbr.aFraCnt = pMOI->frames.cnt - deltaFCnt;
		/* */
		pMOI->frames.errWasFraCntCorrd =
				(pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE);
	}

	if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts)) {
		debMsg[0] = 0;
		if (pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE)
			sprintf(debMsg, " + %u", deltaFCnt);
		st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_TRUE, cFNCN,
				"Total amount of audio frames= %u%s",
				pMOI->mhdVbr.aFraCnt, debMsg);
	}

	/* correct/set byte-counter */
	if (pMOI->opts.anlz && ST_AVFDEB_HASCB_ANAL_D(pMOI->opts)) {
		pMOI->opts.cbAnaMsg("AUD: Total stream size= "ST_TFSIZE_PRF_LU,
				(Tst_fsize)pMOI->frames.totStrSz);
	}

	if (pMOI->mhdVbr.fsz != pMOI->frames.totStrSz ||
			(pMOI->mhdVbr.aFraCnt == 0 && pMOI->mhdVbr.fsz > 0)) {
		if (pMOI->frames.totStrSz > (Tst_fsize)0xffffffff) {
			tmpUI = 0;
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
				st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, cFNCN,
						"total stream size exceeds 4GB limit: "ST_TFSIZE_PRF_LU,
						(Tst_fsize)pMOI->frames.totStrSz);
		} else
			tmpUI = (Tst_uint32)pMOI->frames.totStrSz;
		if (pMOI->mhdVbr.aFraCnt == 0)
			tmpUI = 0;
		if ((pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE ||
					(pMOI->mhdVbr.aFraCnt == 0 && pMOI->mhdVbr.fsz > 0)) &&
				ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts)) {
			st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, cFNCN,
					"correcting total audio size %u -> %u",
					pMOI->mhdVbr.fsz, tmpUI);
		}
		/* */
		pMOI->frames.errWasBytCntCorrd = (pMOI->mhdVbr.fsz != tmpUI &&
				pMOI->mhdVbr.vbrHdType != ST_MPEG1_VBRTYP_NONE);
		/* */
		pMOI->mhdVbr.fsz = tmpUI;
	}
}

/******************************************************************************/
