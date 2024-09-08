/******************************************************************************/
/* aud_mpeg1_zrd.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for MPEG-1 Frameheader and VBR Headers                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 30.12.2009 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/audvid/audvid.h"
#include "src/includes/utils/utils_fmt.h"
/** */
#define SRC_AUD_MPEG1_ZRD_C
#include "aud_mpeg1_zrd-prot.h"
/*** */
#include "aud_mpeg1_stcs-prot.h"
#include "aud_mpeg1_zfdeb-prot.h"
#include "aud_mpeg1_zrd2-pp.h"
#include "../av_zfdeb-prot.h"
#undef SRC_AUD_MPEG1_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Find and parse next MPEG Frameheader
 *
 * pFar->pMFHcomp     : may be pointer to Frameheader that we want to
 *                      compare the current Frameheader with
 *       vbrTp        : if pMFHcomp!=NULL this should be a valid VBR type
 *                      e.g. ST_MPEG1_VBRTYP_XING
 *       isFreeFmtAllw: is Free-Format allowed ?
 * pOpts->forceMLay   : force specific MPEG Layer ?
 *        forceMAv    : force specific MPEG Audio version ?
 *
 * returns: ERRCODE
 *          pFar->couldFindNxFra  will be set
 *                isAtCorrOffs    will be set
 *                isEOFreached    will be set
 *                isFreeFmt       will be set
 *                mfhCur          will contain curr. frameheader (valid or not)
 */
Tst_err
st_mpeg1_rd_findAndReadNextFrHd(Tst_mpeg1_obj_intn *pMOI,
		Tst_mpeg1_farnfh *pFar)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err  res     = ST_ERR_SUCC,
	         rfhRes  = ST_ERR_SUCC;
	Tst_bool isNewF2 = ST_B_TRUE;
	Tst_mpeg1_fhd       *pMFHnew;
	Tst_utilsFmt_mimeTp mimeTp;

	ST_ASSERTN_IARG(pMOI == NULL || pMOI->pFilen == NULL ||
			pMOI->pStrrd == NULL || pFar == NULL)

	st_mpeg1_stc_rsetFar(pFar, ST_B_TRUE);

	if (st_streamrd_isEOF(pMOI->pStrrd)) {
		/* EOF */
		pFar->isEOFreached = ST_B_TRUE;
		return ST_ERR_SUCC;
	}

	/* */
	pMFHnew              = &pFar->mfhNew;
	pMFHnew->fraNr       = ++pMOI->frames.cnt;
	pFar->couldFindNxFra = ST_B_TRUE;
	pFar->frOffs         = st_streamrd_getCurPos(pMOI->pStrrd);

	/**if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts))
		st_mpeg1_d_deb(&pMOI->opts, cFNCN,
				"searching for next frame at "ST_TFOFFS_PRF_LD,
				(Tst_foffs)pFar->frOffs);**/

	while (rfhRes == ST_ERR_SUCC) {
		/* read and parse MPEG frameheader */
		rfhRes = st_mpeg1_rd2_readFrHd(pMOI, pFar);

		if (rfhRes == ST_ERR_MIHD) {
			/* check whether some tag begins here */
			mimeTp = st_utilsFmt_getMime_enum_byStream_tag(pMOI->pStrrd);
			if (mimeTp != ST_UTILSFMT_MTP_NONE) {
				if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts))
					st_mpeg1_d_deb(&pMOI->opts, cFNCN,
							"found some tag at "ST_TFOFFS_PRF_LD", stopping here",
							(Tst_foffs)st_streamrd_getCurPos(pMOI->pStrrd));
				pFar->isEOFreached   = ST_B_TRUE;
				pFar->couldFindNxFra = ST_B_FALSE;
				rfhRes               = ST_ERR_SUCC;
				break;  /* --> end while() */
			}
		}

		/* eval result */
		if (rfhRes != ST_ERR_SUCC) {
			switch (rfhRes) {
			case ST_ERR_MIHD:  /* invalid header */
				rfhRes = ST_ERR_SUCC;  /* --> continue while() */
				/* advance stream by one byte */
				st_streamrd_rdSkipBytes(pMOI->pStrrd, 1, ST_B_TRUE);
				++pFar->frOffsDelta;  /* increase amount skipped bytes */
				break;
			case ST_ERR_FCRD:  /* can't read from file */
			case ST_ERR_FEOF:  /* EOF */
				rfhRes               = ST_ERR_FAIL;  /* --> end while() */
				pFar->isEOFreached   = ST_B_TRUE;
				pFar->couldFindNxFra = ST_B_FALSE;
				break;
			default:  /* error */
				rfhRes               = ST_ERR_FAIL;  /* --> end while() */
				res                  = ST_ERR_FAIL;
				pFar->couldFindNxFra = ST_B_FALSE;
				st_mpeg1_d_err(&pMOI->opts, cFNCN, pMOI->pFilen,
						"reading frame header failed");
			}
		} else { /* else (rfhRes == ST_ERR_SUCC) */
			/* compare frameheader with first frameheader */
			if (pFar->pMFHcomp != NULL &&
					(pMFHnew->audioVers != pFar->pMFHcomp->audioVers ||
						pMFHnew->layer != pFar->pMFHcomp->layer ||
						pMFHnew->samplerate != pFar->pMFHcomp->samplerate ||
						((pFar->vbrTp == ST_MPEG1_VBRTYP_XING ||
								pFar->vbrTp == ST_MPEG1_VBRTYP_INFO) &&
							(pMFHnew->hasCRC != pFar->pMFHcomp->hasCRC ||
								pMFHnew->channMode !=
									pFar->pMFHcomp->channMode)))) {
				/* invalid frame */
				if (isNewF2) {
					if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts)) {
						st_mpeg1_d_deb(&pMOI->opts, cFNCN,
								"found false frame #%d/%d at "ST_TFOFFS_PRF_LD,
								pMFHnew->fraNr, pMOI->frames.cnt,
								(Tst_foffs)(pFar->frOffs + pFar->frOffsDelta));
						st_mpeg1_d_deb(&pMOI->opts, cFNCN,
								" -->fBrt=%u  ver=%d~%d, lay=%d~%d, "
								"sr=%d~%d, cm=%d~%d, crc=%c~%c",
								pMFHnew->bitrate,
								(Tst_int32)pMFHnew->audioVers,
								(Tst_int32)pFar->pMFHcomp->audioVers,
								(Tst_int32)pMFHnew->layer,
								(Tst_int32)pFar->pMFHcomp->layer,
								pMFHnew->samplerate, pFar->pMFHcomp->samplerate,
								(Tst_int32)pMFHnew->channMode,
								(Tst_int32)pFar->pMFHcomp->channMode,
								(pMFHnew->hasCRC ? 'y' : 'n'),
								(pFar->pMFHcomp->hasCRC ? 'y' : 'n'));
					}
					isNewF2 = ST_B_FALSE;
				}
				/* advance stream by one byte */
				st_streamrd_rdSkipBytes(pMOI->pStrrd, 1, ST_B_TRUE);
				++pFar->frOffsDelta;  /* increase amount skipped bytes */
			} else {
				/* valid frame */
				rfhRes = ST_ERR_FAIL;  /* --> end while() */
				if (pMFHnew->bitrate == 0)
					pFar->isFreeFmt = ST_B_TRUE;
				/* skip current frame header since it already has
				 *   been read  */
				st_streamrd_rdSkipBytes(pMOI->pStrrd,
						ST_MPEG1_FR_HD_SZ, ST_B_TRUE);
			}
		} /* end (rfhRes != ST_ERR_SUCC) */
	} /* end while */

	if (res == ST_ERR_SUCC && pFar->couldFindNxFra) {
		pMFHnew->isOffsOK = (pFar->frOffsDelta == 0);
		/**if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"found next frame at "ST_TFOFFS_PRF_LD,
					(Tst_foffs)(pFar->frOffs + pFar->frOffsDelta));**/
	} else
		--pMOI->frames.cnt;

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Check if frame contains CBR/VBR header
 */
Tst_err
st_mpeg1_rd_checkFrameType(Tst_mpeg1_obj_intn *pMOI, Tst_mpeg1_fhd *pMFH)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err       res;
	Tst_uint32    bufSz,
	              rdBytes = 0,
	              skip    = 0,
	              delta;
	Tst_buf       *pBuf   = NULL;
	Tst_buf const *pBB,
	              *pXing  = NULL,
	              *pInfo  = NULL,
	              *pVBRI  = NULL,
	              *pTID   = NULL;
	char          debMsg[512];

	ST_ASSERTN_IARG(pMOI == NULL || pMOI->pFilen == NULL ||
			pMOI->pStrrd == NULL || pMFH == NULL ||
			pMFH->frameSz < ST_MPEG1_FR_HD_SZ ||
			pMOI->frames.cnt == 0)

	bufSz = (Tst_uint32)(pMFH->frameSz - ST_MPEG1_FR_HD_SZ);
	ST_CALLOC(pBuf, Tst_buf*, bufSz + 2, 1)
	if (pBuf == NULL)
		return ST_ERR_OMEM;

	/* read in the entire frame (not very big) */
	res = st_streamrd_rdAheadBuffer(pMOI->pStrrd,
			bufSz, pBuf, &rdBytes);
	if (res != ST_ERR_SUCC || bufSz != rdBytes) {
		if (res == ST_ERR_SUCC && bufSz != rdBytes)
			res = ST_ERR_FEOF;
		ST_DELPOINT(pBuf)
		return res;
	}

	/* goto first first possible position for VBR header */
	/*if (pMOI->hasCRC)
		skip += 2;*/    /* VBR header frame has no CRC */
	skip += st_mpeg1_rd2_getSideinfoSize(pMFH);
	pBB   = pBuf + skip;

	pXing = st_mpeg1_rd2_memfind(pBB, bufSz - skip,
			(const Tst_buf*)ST_MPEG1_TAGVBR_XING, ST_MPEG1_TAGVBR_XING_SLEN);
	pTID  = pXing;
	if (pTID == NULL) {
		pVBRI = st_mpeg1_rd2_memfind(pBB, bufSz - skip,
				(const Tst_buf*)ST_MPEG1_TAGVBR_VBRI, ST_MPEG1_TAGVBR_VBRI_SLEN);
		pTID  = pVBRI;
		if (pTID == NULL) {
			pInfo = st_mpeg1_rd2_memfind(pBB, bufSz - skip,
					(const Tst_buf*)ST_MPEG1_TAGVBR_INFO, ST_MPEG1_TAGVBR_INFO_SLEN);
			pTID  = pInfo;
		}
	}
	if (pTID != NULL) {
		delta = (Tst_uint32)(pTID - pBB);
		/* skip input until header */
		pBB = pTID;
		/* calc offs of VBR header */
		pMOI->mhdVbr.offs = pMOI->frames.offsFrFirst +
				pMOI->frames.offsFrFirstDelta +
				(Tst_foffs)ST_MPEG1_FR_HD_SZ +
				(Tst_foffs)skip + (Tst_foffs)delta;
		/* */
		pMFH->hasVBRhd = ST_B_TRUE;
		if (delta != 0) {
			pMOI->mhdVbr.isOffsOK = ST_B_FALSE;
			/* */
			pMOI->frames.errIsVBRhdAtWrongOffs = ST_B_TRUE;
			/* */
			if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts))
				st_mpeg1_d_deb(&pMOI->opts, cFNCN,
						"CBR/VBR header at wrong offset: "
						"should've been at "ST_TFOFFS_PRF_LD" is at "
						ST_TFOFFS_PRF_LD" (d %u)",
						(Tst_foffs)pMOI->mhdVbr.offs - (Tst_foffs)delta,
						(Tst_foffs)pMOI->mhdVbr.offs, delta);
		} else
			pMOI->mhdVbr.isOffsOK = ST_B_TRUE;

		/* set type and read header */
		if (pXing != NULL || pInfo != NULL) {
			if (pXing != NULL)
				pMOI->mhdVbr.vbrHdType = ST_MPEG1_VBRTYP_XING;  /* Xing */
			else
				pMOI->mhdVbr.vbrHdType = ST_MPEG1_VBRTYP_INFO;  /* CBR */
			/* */
			res = st_mpeg1_rd2_readVBRheadXing(pBB, bufSz - delta, &pMOI->mhdVbr);
			if (res != ST_ERR_IDAT && res != ST_ERR_SUCC) {
				st_mpeg1_d_err(&pMOI->opts, cFNCN, pMOI->pFilen,
						"reading VBR header (Xing/Info) failed");
				ST_DELPOINT(pBuf)
				return res;
			}
		} else {
			pMOI->mhdVbr.vbrHdType = ST_MPEG1_VBRTYP_VBRI;  /* VBRI */
			/* */
			res = st_mpeg1_rd2_readVBRheadFraun(pBB, bufSz - delta,
					&pMOI->mhdVbr);
			if (res != ST_ERR_IDAT && res != ST_ERR_SUCC) {
				st_mpeg1_d_err(&pMOI->opts, cFNCN, pMOI->pFilen,
						"reading VBR header (VBRI) failed");
				ST_DELPOINT(pBuf)
				return res;
			}
		}
		if (res == ST_ERR_IDAT &&
				ST_AVFDEB_ISVERBAUD_OR_ANA_D(pMOI->opts))
			st_mpeg1_d_debOrAna(&pMOI->opts, ST_B_FALSE, cFNCN,
					"CBR/VBR header contains invalid data");
		res = ST_ERR_SUCC;
		/* */
		if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts)) {
			sprintf(debMsg, "found ");
			switch (pMOI->mhdVbr.vbrHdType) {
			case ST_MPEG1_VBRTYP_XING:
				sprintf((char*)&debMsg[6], "XING"); break;
			case ST_MPEG1_VBRTYP_VBRI:
				sprintf((char*)&debMsg[6], "VBRI"); break;
			default:
				sprintf((char*)&debMsg[6], "INFO");
			}
			sprintf((char*)&debMsg[6 + 4],
					" header [fra %u, byt %u]",
					pMOI->mhdVbr.aFraCnt, pMOI->mhdVbr.fsz);
			st_mpeg1_d_deb(&pMOI->opts, cFNCN, debMsg);
		}
	} /* end (pTID != NULL) */

	if (pMOI->mhdVbr.vbrHdType == ST_MPEG1_VBRTYP_NONE)
		pMOI->mhdVbr.isOffsOK = ST_B_TRUE;

	ST_DELPOINT(pBuf)
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Compute playtime
 */
void
st_mpeg1_rd_computePlaytime(Tst_mpeg1_obj_intn *pMOI)
{
	Tst_uint32 spf;
	Tst_uint64 secsTotal,
	           tmp64;
	/**Tst_str debST[50];**/

	ST_ASSERTN_VOID(pMOI == NULL)

	ST_AV_STC_RSETPT(pMOI->playt)

	/* Duration = Number of Frames * Samples Per Frame / Sampling Rate */
	spf = st_mpeg1_rd2_getSamplesPerFrame(pMOI->mhd.layer, pMOI->mhd.audioVers);
	if (pMOI->mhdVbr.aFraCnt == 0 || spf == 0 ||
			pMOI->mhd.samplerate == 0)
		return;

	ST_SYSMATH_STC_RSETUI64(secsTotal)
	st_sysUInt64_setUI32(&secsTotal, pMOI->mhdVbr.aFraCnt);
	st_sysUInt64_multUI32(&secsTotal, spf, NULL);
	st_sysUInt64_divUI32(&secsTotal, pMOI->mhd.samplerate);

	/**st_sysUInt64_toDecStr(&secsTotal, debST, sizeof(debST));
	st_mpeg1_prf("cptime: fra %u, spf %u, sr %u ---> playt %u (%s) secs\n",
			pMOI->mhdVbr.aFraCnt, spf, pMOI->mhd.samplerate,
			(pMOI->mhdVbr.aFraCnt * spf) / pMOI->mhd.samplerate, debST);**/

	/* tmp64 = secsTotal / (60.0 * 60.0) */
	ST_SYSMATH_STC_RSETUI64(tmp64)
	st_sysUInt64_setUI64(&tmp64, &secsTotal);
	st_sysUInt64_divUI32(&tmp64, 60 * 60);
	if (st_sysUInt64_cmpLoHi(&tmp64, 256, 0) < 0) {
		/* ptHour = tmp64 */
		pMOI->playt.ptHours = (Tst_byte)st_sysUInt64_toUI32(&tmp64, NULL);
		/* secsTotal -= ptHour * 60 * 60 */
		st_sysUInt64_subUI32(&secsTotal, pMOI->playt.ptHours * 60 * 60, NULL);
		/* tmp64 = secsTotal / 60.0 */
		st_sysUInt64_setUI64(&tmp64, &secsTotal);
		st_sysUInt64_divUI32(&tmp64, 60);
		/* ptMin = tmp64 */
		pMOI->playt.ptMins = (Tst_byte)st_sysUInt64_toUI32(&tmp64, NULL);
		/* secsTotal -= ptMin * 60 */
		st_sysUInt64_subUI32(&secsTotal, pMOI->playt.ptMins * 60, NULL);
		/* ptSec = secsTotal */
		pMOI->playt.ptSecs = (Tst_byte)st_sysUInt64_toUI32(&secsTotal, NULL);
	}

	/**st_mpeg1_prf("cptime: %u:%02u:%02u\n",
			pMOI->playt.ptHours, pMOI->playt.ptMins, pMOI->playt.ptSecs);**/
}

/******************************************************************************/
