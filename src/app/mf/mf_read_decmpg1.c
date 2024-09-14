/******************************************************************************/
/* mf_read_decmpg1.c              [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for decoding MPEG-1 audio                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 04.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/audvid/cont_wav.h"
/** */
#define SRC_MF_READ_DECMPG1_C
#include "mf_read_decmpg1-pp.h"
/*** */
#include "mf_read_dbuf-prot.h"
//#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#undef SRC_MF_READ_DECMPG1_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), random() */
#include <string.h>      /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)

Tst_err
ast_mf_rddec_initExtLibMpg(void)
{
#	if (HAVE_LIBMPG123 == 1)
	if (mpg123_init() != MPG123_OK)
		return ST_ERR_FAIL;
#	endif  /* libmpg123 */

	return ST_ERR_SUCC;
}

void
ast_mf_rddec_freeExtLibMpg(void)
{
#	if (HAVE_LIBMPG123 == 1)
	mpg123_exit();
#	endif  /* libmpg123 */
}

#endif  /* libmpg123 or libmad */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Callbacks for libmpg123 or libmad */
#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)

/*
 * Create new handle
 */
Tst_err
ast_mf_rddec_cbDecMpg_hndNew(void **ppHnd, const Tst_bool printUsedBPS)
{
	Tst_err res;
#	if (HAVE_LIBMPG123 == 1)
	Tast_mf__rddec_mpg123 *pM123 = NULL;
#	elif (HAVE_LIBMAD == 1)
	Tast_mf__rddec_mad    *pMad  = NULL;
#	endif  /* libmpg123 */

	ST_ASSERTN_IARG(ppHnd == NULL)

#	if (HAVE_LIBMPG123 == 1)
	ST_CALLOC(pM123, Tast_mf__rddec_mpg123*,
			1, sizeof(Tast_mf__rddec_mpg123))
	if (pM123 == NULL)
		return ST_ERR_OMEM;

	*ppHnd = pM123;
	/* */
	res = AST_MF__rddec_stc_initM123(pM123);
	/* */
	pM123->printUsedBPS = printUsedBPS;

#	elif (HAVE_LIBMAD == 1)
	ST_CALLOC(pMad, Tast_mf__rddec_mad*, 1, sizeof(Tast_mf__rddec_mad))
	if (pMad == NULL)
		return ST_ERR_OMEM;

	*ppHnd = pMad;
	/* */
	res = AST_MF__rddec_stc_initMad(pMad);
	/* */
	pMad->printUsedBPS = printUsedBPS;

#	endif  /* libmpg123 */

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Free handle
 */
void
ast_mf_rddec_cbDecMpg_hndDel(void *pHnd)
{
#	if (HAVE_LIBMPG123 == 1)
	Tast_mf__rddec_mpg123 *pM123 = (Tast_mf__rddec_mpg123*)pHnd;
#	elif (HAVE_LIBMAD == 1)
	Tast_mf__rddec_mad    *pMad  = (Tast_mf__rddec_mad*)pHnd;
#	endif  /* libmpg123 */

	ST_ASSERTN_VOID(pHnd == NULL)

#	if (HAVE_LIBMPG123 == 1)
	AST_MF__rddec_stc_freeM123(pM123);
	ST_DELPOINT(pM123)

#	elif (HAVE_LIBMAD == 1)
	AST_MF__rddec_stc_freeMad(pMad);
	ST_DELPOINT(pMad)

#	endif  /* libmpg123 */
}

/*----------------------------------------------------------------------------*/

/*
 * Set output format
 */
Tst_err
ast_mf_rddec_cbDecMpg_setOutputFmt(void *pHnd,
		const Tst_uint32 sampleRate, const Tst_uint32 channels,
		const Tst_uint32 desiredBitsPerSample,
		Tst_uint32 *pUsedBitsPerSample)
{
	Tast_mf_rddec_dbuf    *pDBuf;
#	if (HAVE_LIBMPG123 == 1)
	const char            *cFNCN = __func__;
	Tst_err               res;
	int                   resI = 0;
#		if (MPG123_API_VERSION >= 25)
	Tst_bool              is32supp = ST_B_FALSE;
#		endif
	Tast_mf__rddec_mpg123 *pM123   = (Tast_mf__rddec_mpg123*)pHnd;
	//Tst_uint32            srcBps;
#	elif (HAVE_LIBMAD == 1)
	Tast_mf__rddec_mad    *pMad = (Tast_mf__rddec_mad*)pHnd;
#	endif  /* libmpg123 */

	ST_ASSERTN_IARG(pHnd == NULL || pUsedBitsPerSample == NULL ||
			(desiredBitsPerSample != 0 && desiredBitsPerSample != 16 &&
				desiredBitsPerSample != 24 && desiredBitsPerSample != 32))

	*pUsedBitsPerSample = 0;

#	if (HAVE_LIBMPG123 == 1)
	if (desiredBitsPerSample == 0 || desiredBitsPerSample > 16) {
#		if (MPG123_API_VERSION >= 25)
		/* check whether libmpg123 supports signed 32bit output */
		pM123->encoding = MPG123_ENC_SIGNED_32;
		resI            = mpg123_format_support(pM123->pM123hnd,
				(long)sampleRate, pM123->encoding);
		switch (resI) {
		case 0:
			/* signed 32bit output is not supported */
			break;
		case MPG123_MONO:
			/* signed 32bit output is supported for mono */
			is32supp = (channels == 1);
			break;
		case MPG123_STEREO:
			/* signed 32bit output is supported for stereo */
			is32supp = (channels == 2);
			break;
		case MPG123_MONO|MPG123_STEREO:
			/* signed 32bit output is supported for mono and stereo */
			is32supp = (channels == 1 || channels == 2);
			break;
		default:
			ast_mf_op_prErr(cFNCN, "libmpg123 returned unknown value: %d",
					resI);
			return ST_ERR_FAIL;
		}
#		endif  /* MPG123_API_VERSION >= 25 */
	}

	/* 16, 24 and 32bit output are supported by us */
	pDBuf = &pM123->dbuf;
	if (desiredBitsPerSample == 0)
		pDBuf->bps = 16;
	else
		pDBuf->bps = desiredBitsPerSample;
#	if (MPG123_API_VERSION >= 25)
	if (pDBuf->bps > 16 && is32supp)
		pM123->encoding = MPG123_ENC_SIGNED_32;
	else
#	endif
		pM123->encoding = MPG123_ENC_SIGNED_16;
	//srcBps = (pM123->encoding == MPG123_ENC_SIGNED_16 ? 16 : 32);

	mpg123_format_none(pM123->pM123hnd);
	resI = mpg123_format(pM123->pM123hnd,
			(long)sampleRate,
			(int)(channels == 1 ? MPG123_MONO : MPG123_STEREO),
			(int)pM123->encoding);
	if (resI != MPG123_OK) {
		ast_mf_op_prErr(cFNCN, "failed to set libmpg123 output fmt");
		return ST_ERR_FAIL;
	}

	res = AST_MF__rddec_stc_reszDecBufTmp(pDBuf,
			mpg123_outblock(pM123->pM123hnd) * 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* */
	if (pM123->printUsedBPS) {
		/*char msg[512];*/

		/*if (pDBuf->bps == srcBps)
			msg[0] = 0;
		else
			snprintf(msg, sizeof(msg), " - converted from %ubit", srcBps);
		ast_mf_op_prMsg("(Using %ubit output%s, decoding with libmpg123)",
				pDBuf->bps, msg);*/
		ast_mf_op_prMsg("(Using %ubit output, decoding with libmpg123)",
				pDBuf->bps);
	}

#	elif (HAVE_LIBMAD == 1)
	/* 16, 24 and 32bit output are supported by us */
	pDBuf = &pMad->dbuf;
	if (desiredBitsPerSample == 0)
		pDBuf->bps = 16;
	else
		pDBuf->bps = desiredBitsPerSample;

	/* */
	if (pMad->printUsedBPS) {
		/*char msg[512];*/

		/*snprintf(msg, sizeof(msg), " - converted from 24bit float");
		ast_mf_op_prMsg("(Using %ubit output%s, decoding with libmad)",
				pDBuf->bps, msg);*/
		ast_mf_op_prMsg("(Using %ubit output, decoding with libmad)",
				pDBuf->bps);
	}

#	endif  /* libmpg123 */

	pDBuf->sampleR      = sampleRate;
	pDBuf->channels     = channels;
	pDBuf->isOutpFmtSet = ST_B_TRUE;
	*pUsedBitsPerSample = pDBuf->bps;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Open feed
 */
Tst_err
ast_mf_rddec_cbDecMpg_openFeed(void *pHnd)
{
#	if (HAVE_LIBMPG123 == 1)
	int resI;
	Tast_mf__rddec_mpg123 *pM123 = (Tast_mf__rddec_mpg123*)pHnd;
#	endif  /* libmpg123 */

	ST_ASSERTN_IARG(pHnd == NULL)

#	if (HAVE_LIBMPG123 == 1)
	resI = mpg123_open_feed(pM123->pM123hnd);
	if (resI != MPG123_OK)
		return ST_ERR_FAIL;
#	endif  /* libmpg123 */

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Feed input data (exactly one MPEG-1 frame)
 *   For libmpg123 the input buffer is feed to
 *     libmpg123 which appends the data to its internal buffer
 *   For libmad we have to use our own internal buffer
 *     since libmad operates directly on the input buffer
 *     because it doesn't have an internal buffer
 */
Tst_err
ast_mf_rddec_cbDecMpg_feedInp(void *pHnd,
		Tst_buf *pInpBuf, const Tst_uint32 inpBufSz)
{
	Tst_err               res;
#	if (HAVE_LIBMPG123 == 1)
	const char            *cFNCN = __func__;
	int                   resI;
	Tast_mf__rddec_mpg123 *pM123 = (Tast_mf__rddec_mpg123*)pHnd;
#	elif (HAVE_LIBMAD == 1)
	Tast_mf__rddec_ibuf   *pIBuf;
#	endif  /* libmpg123 */

	ST_ASSERTN_IARG(pHnd == NULL || pInpBuf == NULL)

	if (inpBufSz == 0)
		return ST_ERR_SUCC;

#	if (HAVE_LIBMPG123 == 1)
	/** */
	ST_ASSERTN_NUM(ST_ERR_FAIL, ! pM123->dbuf.isOutpFmtSet)
	/** */
	resI = mpg123_decode(pM123->pM123hnd,
			(unsigned char*)pInpBuf,
			(size_t)inpBufSz,
			NULL, 0,
			NULL);
	if (resI == MPG123_NEW_FORMAT) {
		res = AST_MF__rddec_getMpg123NewFmt(cFNCN, pM123);
		if (res != ST_ERR_SUCC)
			return res;
		resI = MPG123_OK;
	}
	if (resI != MPG123_OK && resI != MPG123_NEED_MORE) {
		ast_mf_op_prErr(cFNCN, "error from libmpg123: '%s'",
				(resI == MPG123_ERR ?
					mpg123_strerror(pM123->pM123hnd) :
					mpg123_plain_strerror(resI)));
		return ST_ERR_FAIL;
	}

#	elif (HAVE_LIBMAD == 1)
	/** */
	ST_ASSERTN_NUM(ST_ERR_FAIL,
			! ((Tast_mf__rddec_mad*)pHnd)->dbuf.isOutpFmtSet)
	/** */
	pIBuf = &((Tast_mf__rddec_mad*)pHnd)->ibuf;

	if (pIBuf->wasDecoded) {
		if (pIBuf->procInp > 0) {
			/* remove already processed input */
			res = AST_MF__rddec_removeProcInp(pIBuf);
			if (res != ST_ERR_SUCC)
				return res;
		}
		pIBuf->wasDecoded = ST_B_FALSE;
	}
	if (pIBuf->inpBufUsed + inpBufSz > pIBuf->inpBufSz) {
		ST_REALLOC(pIBuf->pInpBuf, Tst_buf*, pIBuf->inpBufUsed + inpBufSz, 1)
		if (pIBuf->pInpBuf == NULL)
			return ST_ERR_OMEM;
		pIBuf->inpBufSz = pIBuf->inpBufUsed + inpBufSz;
	}
	memcpy(&pIBuf->pInpBuf[pIBuf->inpBufUsed], pInpBuf, inpBufSz);
	pIBuf->inpBufUsed += inpBufSz;
	/**ast_mf_op_prf("feed %u  (+ %u)\n", pIBuf->inpBufUsed, inpBufSz);**/

#	endif  /* libmpg123 */

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Decode data that has been feed to the stream into raw PCM samples
 * If cbPCMclient is != NULL and pPCMclientHandle != NULL
 *   then the decoded sample will be passed directly
 *   to the PCM client
 */
Tst_err
ast_mf_rddec_cbDecMpg_decode(void *pHnd,
		const Tst_bool isLastCall, Tst_bool *pNeedMore,
		Tst_av_cbPCM_samplesClient cbPCMclient,
		void *pPCMclientHandle)
{
	Tst_err  res;
	Tst_bool passSamplesToClient;
	Tast_mf_rddec_dbuf *pDBuf;

#	if (HAVE_LIBMPG123 == 1)
	pDBuf = &((Tast_mf__rddec_mpg123*)pHnd)->dbuf;
#	elif (HAVE_LIBMAD == 1)
	pDBuf = &((Tast_mf__rddec_mad*)pHnd)->dbuf;
#	endif  /* libmpg123 */
	passSamplesToClient = (cbPCMclient != NULL && pPCMclientHandle != NULL);

	do {
#		if (HAVE_LIBMPG123 == 1)
		res = AST_MF__rddec_cbDecMpg_decodeOnly_mpg123(pHnd, isLastCall,
				pNeedMore, passSamplesToClient);
#		elif (HAVE_LIBMAD == 1)
		res = AST_MF__rddec_cbDecMpg_decodeOnly_mad(pHnd, isLastCall,
				pNeedMore, passSamplesToClient);
#		endif  /* libmpg123 */
		if (res != ST_ERR_SUCC)
			break;

		/* pass samples to client */
		if (passSamplesToClient && pDBuf->sampleCnt > 0)
			res = cbPCMclient(pPCMclientHandle,
					pDBuf->sampleCnt, (const Tst_int32**)pDBuf->ppBufOutI32);
	} while (res == ST_ERR_SUCC && pDBuf->sampleCnt > 0);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Close feed of libmpg123, not necessary for libmad
 */
Tst_err
ast_mf_rddec_cbDecMpg_closeFeed(void *pHnd)
{
#	if (HAVE_LIBMPG123 == 1)
	int resI;
	Tast_mf__rddec_mpg123 *pM123 = (Tast_mf__rddec_mpg123*)pHnd;
#	endif  /* libmpg123 */

	ST_ASSERTN_IARG(pHnd == NULL)

#	if (HAVE_LIBMPG123 == 1)
	resI = mpg123_close(pM123->pM123hnd);
	if (resI != MPG123_OK)
		return ST_ERR_FAIL;
#	endif  /* libmpg123 */

	return ST_ERR_SUCC;
}

#endif  /* libmpg123 or libmad */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
#define LOC_RESZ_DECBUFOUTI32_(mac_sampleCnt, mac_sampleRes, mac_pDBuf)  { \
			if ((mac_sampleCnt) > (mac_sampleRes)) { \
				Tst_err res = ast_mf_rddec_stc_reszDecBufOutI32(mac_pDBuf, mac_sampleCnt); \
				if (res != ST_ERR_SUCC) \
					return res; \
			} \
			}
#endif  /* libmpg123 or libmad */

#if (HAVE_LIBMPG123 == 1)

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Decode data that has been feed to the stream into raw PCM samples
 */
static Tst_err
AST_MF__rddec_cbDecMpg_decodeOnly_mpg123(void *pHnd,
		const Tst_bool isLastCall, Tst_bool *pNeedMore,
		const Tst_bool passSamplesToClient)
{
#	define LOC_FNCN_  __func__
	Tst_err res    = ST_ERR_SUCC;
	int     resI;
	size_t  done   = 0;
	Tast_mf_rddec_dbuf    *pDBuf;
	Tast_mf__rddec_mpg123 *pM123 = (Tast_mf__rddec_mpg123*)pHnd;

	ST_ASSERTN_IARG(pHnd == NULL || pNeedMore == NULL ||
			! pM123->dbuf.isOutpFmtSet)

	/* */
	pDBuf            = &pM123->dbuf;
	pDBuf->sampleCnt = 0;

	/* */
	resI = mpg123_decode(pM123->pM123hnd,
			NULL, 0,
			(unsigned char*)pDBuf->pBufTmp,
			(size_t)pDBuf->bufTmpSz,
			&done);
	if (resI == MPG123_NEW_FORMAT) {
		res = AST_MF__rddec_getMpg123NewFmt(LOC_FNCN_, pM123);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		resI = mpg123_decode(pM123->pM123hnd,
				NULL, 0,
				(unsigned char*)pDBuf->pBufTmp,
				(size_t)pDBuf->bufTmpSz,
				&done);
	}
	/* */
	switch (resI) {
	case MPG123_OK:
	case MPG123_NEED_MORE:
		*pNeedMore = (resI == MPG123_NEED_MORE);
		/* */
		if (done > 0) {
			pDBuf->sampleCnt = (Tst_uint32)(done / pDBuf->channels);
			/* */
			if (pM123->encoding == MPG123_ENC_SIGNED_16) {
				pDBuf->sampleCnt /= (Tst_uint32)sizeof(Tst_int16);
				if (passSamplesToClient) {
					if (pDBuf->bps == 16)
						res = AST_MF__rddec_convMpg123_rawSamples_inp16out16(pM123);
					else
						res = AST_MF__rddec_convMpg123_rawSamples_inp16shift(pM123);
				}
			} else {
				pDBuf->sampleCnt /= (Tst_uint32)sizeof(Tst_int32);
				if (passSamplesToClient) {
					if (pDBuf->bps == 32)
						res = AST_MF__rddec_convMpg123_rawSamples_inp32out32(pM123);
					else
						res = AST_MF__rddec_convMpg123_rawSamples_inp32shift(pM123);
				}
			}
		}
		break;
	case MPG123_NEW_FORMAT:
		res = ST_ERR_FAIL;
		break;
	default:
		ast_mf_op_prErr(LOC_FNCN_, "error from libmpg123: '%s'",
				(resI == MPG123_ERR ?
					mpg123_strerror(pM123->pM123hnd) :
					mpg123_plain_strerror(resI)));
		res = ST_ERR_FAIL;
	}

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * libmpg123 has detected a new input format
 */
static Tst_err
AST_MF__rddec_getMpg123NewFmt(const char *pFnc,
		Tast_mf__rddec_mpg123 *pM123)
{
	long rate     = 0;
	int  channels = 0,
	     encoding = 0;

	mpg123_getformat(pM123->pM123hnd,
			&rate, &channels, &encoding);
	if (channels != (int)pM123->dbuf.channels ||
			encoding != (int)pM123->encoding ||
			rate != (long)pM123->dbuf.sampleR) {
		ast_mf_op_prErr(pFnc, "error from libmpg123: "
				"amount of channels (%u~%u), encoding (%d~%d) "
				"or samplerate (%ld~%ld) changed",
				channels, (int)pM123->dbuf.channels,
				encoding, (int)pM123->encoding,
				rate, (long)pM123->dbuf.sampleR);
		return ST_ERR_FAIL;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert the raw samples format of libmpg123 to our own format
 *   Input is 16bit
 */
static Tst_err
AST_MF__rddec_convMpg123_rawSamples_inp16out16(Tast_mf__rddec_mpg123 *pM123)
{
	register Tst_uint32 sampleCnt = pM123->dbuf.sampleCnt,
	                    y,
	                    offs      = 0;
	Tst_int16 const     *pMBuf16  = (Tst_int16*)pM123->dbuf.pBufTmp;
	Tst_int32           *pBufOutI32ch1;

	ST_ASSERTN_NUM(ST_ERR_FAIL,
			pM123->dbuf.bufTmpSz <
				pM123->dbuf.channels *
					pM123->dbuf.sampleCnt * sizeof(Tst_int16) ||
			pM123->dbuf.channels < 1 || pM123->dbuf.channels > 2 ||
			pM123->dbuf.bps != 16 ||
			pM123->encoding != MPG123_ENC_SIGNED_16)

	LOC_RESZ_DECBUFOUTI32_(sampleCnt, pM123->dbuf.sampleCntRes, &pM123->dbuf)
	pBufOutI32ch1 = pM123->dbuf.ppBufOutI32[0];

	if (pM123->dbuf.channels == 2) {
		Tst_int32 *pBufOutI32ch2 = pM123->dbuf.ppBufOutI32[1];

		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = (Tst_int32)pMBuf16[offs++];
			pBufOutI32ch2[y] = (Tst_int32)pMBuf16[offs++];
		}
	} else {
		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = (Tst_int32)pMBuf16[y];
		}
	}
	return ST_ERR_SUCC;
}

static Tst_err
AST_MF__rddec_convMpg123_rawSamples_inp16shift(Tast_mf__rddec_mpg123 *pM123)
{
	const Tst_uint32    shiftL    = pM123->dbuf.bps - 16;
	register Tst_uint32 sampleCnt = pM123->dbuf.sampleCnt,
	                    y,
	                    offs      = 0;
	Tst_int16 const     *pMBuf16  = (Tst_int16*)pM123->dbuf.pBufTmp;
	Tst_int32           *pBufOutI32ch1;

	ST_ASSERTN_NUM(ST_ERR_FAIL,
			pM123->dbuf.bufTmpSz <
				pM123->dbuf.channels *
					pM123->dbuf.sampleCnt * sizeof(Tst_int16) ||
			pM123->dbuf.channels < 1 || pM123->dbuf.channels > 2 ||
			pM123->dbuf.bps < 16 || pM123->dbuf.bps > 32 ||
			pM123->encoding != MPG123_ENC_SIGNED_16)

	LOC_RESZ_DECBUFOUTI32_(sampleCnt, pM123->dbuf.sampleCntRes, &pM123->dbuf)
	pBufOutI32ch1 = pM123->dbuf.ppBufOutI32[0];

	if (pM123->dbuf.channels == 2) {
		Tst_int32 *pBufOutI32ch2 = pM123->dbuf.ppBufOutI32[1];

		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = ((Tst_int32)pMBuf16[offs++] << shiftL);
			pBufOutI32ch2[y] = ((Tst_int32)pMBuf16[offs++] << shiftL);
		}
	} else {
		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = ((Tst_int32)pMBuf16[y] << shiftL);
		}
	}
	return ST_ERR_SUCC;
}

/*
 * Convert the raw samples format of libmpg123 to our own format
 *   Input is 32bit
 */
static Tst_err
AST_MF__rddec_convMpg123_rawSamples_inp32out32(Tast_mf__rddec_mpg123 *pM123)
{
	register Tst_uint32 sampleCnt = pM123->dbuf.sampleCnt,
	                    y,
	                    offs      = 0;
	Tst_int32 const     *pMBuf32  = (Tst_int32*)pM123->dbuf.pBufTmp;
	Tst_int32           *pBufOutI32ch1;

	ST_ASSERTN_NUM(ST_ERR_FAIL,
			pM123->dbuf.bufTmpSz <
				pM123->dbuf.channels *
					pM123->dbuf.sampleCnt * sizeof(Tst_int32) ||
			pM123->dbuf.channels < 1 || pM123->dbuf.channels > 2 ||
			pM123->dbuf.bps != 32)
#	if (MPG123_API_VERSION >= 25)
	ST_ASSERTN_NUM(ST_ERR_FAIL, pM123->encoding != MPG123_ENC_SIGNED_32)
#	else
	ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* input can't be 32bit */
#	endif

	LOC_RESZ_DECBUFOUTI32_(sampleCnt, pM123->dbuf.sampleCntRes, &pM123->dbuf)
	pBufOutI32ch1 = pM123->dbuf.ppBufOutI32[0];

	if (pM123->dbuf.channels == 2) {
		Tst_int32 *pBufOutI32ch2 = pM123->dbuf.ppBufOutI32[1];

		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = pMBuf32[offs++];
			pBufOutI32ch2[y] = pMBuf32[offs++];
		}
	} else {
		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = pMBuf32[y];
		}
	}
	return ST_ERR_SUCC;
}

static Tst_err
AST_MF__rddec_convMpg123_rawSamples_inp32shift(Tast_mf__rddec_mpg123 *pM123)
{
	const Tst_uint32    shiftR    = 32 - pM123->dbuf.bps;
	register Tst_uint32 sampleCnt = pM123->dbuf.sampleCnt,
	                    y,
	                    offs      = 0;
	Tst_int32 const     *pMBuf32  = (Tst_int32*)pM123->dbuf.pBufTmp;
	Tst_int32           *pBufOutI32ch1;

	ST_ASSERTN_NUM(ST_ERR_FAIL,
			pM123->dbuf.bufTmpSz <
				pM123->dbuf.channels *
					pM123->dbuf.sampleCnt * sizeof(Tst_int32) ||
			pM123->dbuf.channels < 1 || pM123->dbuf.channels > 2 ||
			pM123->dbuf.bps < 16 || pM123->dbuf.bps > 32)
#	if (MPG123_API_VERSION >= 25)
	ST_ASSERTN_NUM(ST_ERR_FAIL, pM123->encoding != MPG123_ENC_SIGNED_32)
#	else
	ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* input can't be 32bit */
#	endif

	LOC_RESZ_DECBUFOUTI32_(sampleCnt, pM123->dbuf.sampleCntRes, &pM123->dbuf)
	pBufOutI32ch1 = pM123->dbuf.ppBufOutI32[0];

	if (pM123->dbuf.channels == 2) {
		Tst_int32 *pBufOutI32ch2 = pM123->dbuf.ppBufOutI32[1];

		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = (pMBuf32[offs++] >> shiftR);
			pBufOutI32ch2[y] = (pMBuf32[offs++] >> shiftR);
		}
	} else {
		for (y = 0; y < sampleCnt; y++) {
			pBufOutI32ch1[y] = (pMBuf32[y] >> shiftR);
		}
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#elif (HAVE_LIBMAD == 1)

/*
 * Decode data that has been feed to the stream into raw PCM samples
 */
static Tst_err
AST_MF__rddec_cbDecMpg_decodeOnly_mad(void *pHnd,
		const Tst_bool isLastCall, Tst_bool *pNeedMore,
		const Tst_bool passSamplesToClient)
{
#	define LOC_FNCN_  __func__
	Tst_err    res;
	Tst_fsize  lastPrInp;
	Tst_uint32 remain;
	Tast_mf_rddec_dbuf *pDBuf;
	Tast_mf__rddec_mad *pMad  = (Tast_mf__rddec_mad*)pHnd;

	ST_ASSERTN_IARG(pHnd == NULL || pNeedMore == NULL ||
			! pMad->dbuf.isOutpFmtSet)

	/* */
	pDBuf            = &pMad->dbuf;
	pDBuf->sampleCnt = 0;

	if (! pMad->ibuf.wasDecoded && isLastCall) {
		Tst_err res;

		/* this is necessary for libmad because it expects
		 *   some padding at end of the input  */
		res = ast_mf_rddec_cbDecMpg_feedInp(pHnd, pMad->guardBuf,
				sizeof(pMad->guardBuf));
		if (res != ST_ERR_SUCC) {
			ast_mf_op_prErr(LOC_FNCN_, "feeding input to MPEG-1 decoder failed");
			return res;
		}
	}

	/* this does only tell libmad the current address of
	 *   the input buffer and its size  */
	mad_stream_buffer(&pMad->mStream,
			(unsigned char*)pMad->ibuf.pInpBuf,
			(size_t)pMad->ibuf.inpBufUsed);

	*pNeedMore = ST_B_FALSE;
	if (mad_frame_decode(&pMad->mFrame, &pMad->mStream) != 0) {
		if (pMad->mStream.error == MAD_ERROR_BUFLEN) {
			*pNeedMore = ST_B_TRUE;
			/**ast_mf_op_prErr(LOC_FNCN_,
					"error from libmad: '%s'",
					mad_stream_errorstr(&pMad->mStream));**/
			return ST_ERR_SUCC;
		} else if (pMad->mStream.error == MAD_ERROR_BADCRC) {
			ast_mf_op_prErr(LOC_FNCN_,
					"warning from libmad: CRC of frame didn't match");
		} else if (MAD_RECOVERABLE(pMad->mStream.error)) {
			if (pMad->mStream.error != MAD_ERROR_LOSTSYNC) {
				ast_mf_op_prErr(LOC_FNCN_,
						"recoverable error from libmad: '%s'",
						mad_stream_errorstr(&pMad->mStream));
			} else if (! isLastCall)
				ast_mf_op_prErr(LOC_FNCN_,
						"recoverable error from libmad: lost sync");
			*pNeedMore = ST_B_TRUE;
			return ST_ERR_SUCC;
		} else {
			ast_mf_op_prErr(LOC_FNCN_,
					"error from libmad: #%d '%s'",
					pMad->mStream.error, mad_stream_errorstr(&pMad->mStream));
			return ST_ERR_FAIL;
		}
	}

	/* Synthesize decoded frame to PCM samples */
	mad_synth_frame(&pMad->mSynth, &pMad->mFrame);
	ST_ASSERTN_NUM(ST_ERR_FAIL, pMad->mStream.next_frame == NULL ||
			pMad->mStream.bufend == NULL ||
			pMad->mStream.bufend < pMad->mStream.next_frame)

	if (pMad->mSynth.pcm.channels != pMad->dbuf.channels ||
			pMad->mSynth.pcm.samplerate != pMad->dbuf.sampleR) {
		ast_mf_op_prErr(LOC_FNCN_,
				"error from libmad: "
				"amount of channels or samplerate changed");
		return ST_ERR_FAIL;
	}

	pMad->dbuf.sampleCnt = pMad->mSynth.pcm.length;

	if (passSamplesToClient) {
		if (pDBuf->bps <= 24)
			res = AST_MF__rddec_convMad_rawSamples_inp24noshift(pMad);
		else
			res = AST_MF__rddec_convMad_rawSamples_inp24shift(pMad);
		if (res != ST_ERR_SUCC)
			return res;
	}

	remain                 = (pMad->mStream.bufend - pMad->mStream.next_frame);
	pMad->ibuf.wasDecoded  = ST_B_TRUE;
	lastPrInp              = (Tst_fsize)pMad->ibuf.procInp;
	pMad->ibuf.procInp     = (pMad->ibuf.inpBufUsed - remain);
	pMad->ibuf.procInpTot += ((Tst_fsize)pMad->ibuf.procInp - lastPrInp);
	/**ast_mf_op_prf("samples +%u, procInpT "ST_TFSIZE_PRF_LU" (+ %u)\n\n",
			pMad->mSynth.pcm.length, (Tst_fsize)pMad->ibuf.procInpTot,
			(Tst_uint32)(pMad->ibuf.procInp - lastPrInp));**/

	/* remove already processed input */
	return AST_MF__rddec_removeProcInp(&pMad->ibuf);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Remove already processed input
 */
static Tst_err
AST_MF__rddec_removeProcInp(Tast_mf__rddec_ibuf *pIBuf)
{
	ST_ASSERTN_NUM(ST_ERR_FAIL, pIBuf->inpBufUsed < pIBuf->procInp)

	memmove(pIBuf->pInpBuf, &pIBuf->pInpBuf[pIBuf->procInp],
			pIBuf->inpBufUsed - pIBuf->procInp);
	pIBuf->inpBufUsed -= pIBuf->procInp;
	pIBuf->procInp     = 0;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Very basic sample quantize routine
 *   This code was adapted from madlld-1.1p1
 */
#define LOC_MADSIMPLEROUND_(mac_bits, mac_sampleFixed, mac_sampleI32)  { \
			/* Clipping */ \
			if (mac_sampleFixed >= MAD_F_ONE) \
				mac_sampleI32 = (ST_TINT32_MAX); \
			else if (mac_sampleFixed <= -MAD_F_ONE) \
				mac_sampleI32 = (ST_TINT32_MIN); \
			else  /* quantize and scaling */ \
				mac_sampleI32 = (Tst_int32)(mac_sampleFixed >> \
							(MAD_F_FRACBITS + 1 - mac_bits)); \
			}

/*
 * Generic linear sample quantize routine
 *   This code was adapted from madplay-0.15.2b
 */
#define LOC_MIN_  (-MAD_F_ONE)
#define LOC_MAX_  (MAD_F_ONE - 1)
#define LOC_MADLINEARROUND_(mac_bits, mac_sampleFixed, mac_stats, mac_sampleI32)  { \
			/* round */ \
			mac_sampleFixed += (1L << (MAD_F_FRACBITS - mac_bits)); \
			\
			/* clip */ \
			if (mac_sampleFixed >= (mac_stats).peak_sample) { \
				if (mac_sampleFixed > LOC_MAX_) { \
					++(mac_stats).clipped_samples; \
					if (mac_sampleFixed - LOC_MAX_ > (mac_stats).peak_clipping) \
						(mac_stats).peak_clipping = mac_sampleFixed - LOC_MAX_; \
					\
					mac_sampleFixed = LOC_MAX_; \
				} \
				(mac_stats).peak_sample = mac_sampleFixed; \
			} else if (mac_sampleFixed < -(mac_stats).peak_sample) { \
				if (mac_sampleFixed < LOC_MIN_) { \
					++(mac_stats).clipped_samples; \
					if (LOC_MIN_ - mac_sampleFixed > (mac_stats).peak_clipping) \
						(mac_stats).peak_clipping = LOC_MIN_ - mac_sampleFixed; \
					\
					mac_sampleFixed = LOC_MIN_; \
				} \
				(mac_stats).peak_sample = -(mac_sampleFixed); \
			} \
			\
			/* quantize and scale */ \
			mac_sampleI32 = (mac_sampleFixed >> (MAD_F_FRACBITS + 1 - mac_bits)); \
			}

/*
 * Convert the raw samples format of libmad to our own format
 *   Input is always libmad's propietary float format with max. 24bit precision
 */
static Tst_err
AST_MF__rddec_convMad_rawSamples_inp24noshift(Tast_mf__rddec_mad *pMad)
{
	const Tst_uint32    bps       = pMad->dbuf.bps;
	register Tst_uint32 sampleCnt = pMad->dbuf.sampleCnt,
	                    y;
	mad_fixed_t         *pInpCh1  = (mad_fixed_t*)pMad->mSynth.pcm.samples[0],
	                    *pInpCh2  = (mad_fixed_t*)pMad->mSynth.pcm.samples[1];
	Tst_int32           *pBufOutI32ch1;

	ST_ASSERTN_NUM(ST_ERR_FAIL, pMad->dbuf.channels < 1 || pMad->dbuf.channels > 2 ||
			pMad->dbuf.bps < 16 || pMad->dbuf.bps > 24)

	LOC_RESZ_DECBUFOUTI32_(sampleCnt, pMad->dbuf.sampleCntRes, &pMad->dbuf)
	pBufOutI32ch1 = pMad->dbuf.ppBufOutI32[0];

	if (pMad->dbuf.channels == 2) {
		Tst_int32 *pBufOutI32ch2 = pMad->dbuf.ppBufOutI32[1];

		for (y = 0; y < sampleCnt; y++) {
			LOC_MADLINEARROUND_(bps, pInpCh1[y], pMad->aStats, pBufOutI32ch1[y])
			LOC_MADLINEARROUND_(bps, pInpCh2[y], pMad->aStats, pBufOutI32ch2[y])
		}
	} else {
		for (y = 0; y < sampleCnt; y++) {
			LOC_MADLINEARROUND_(bps, pInpCh1[y], pMad->aStats, pBufOutI32ch1[y])
		}
	}

	return ST_ERR_SUCC;
}

static Tst_err
AST_MF__rddec_convMad_rawSamples_inp24shift(Tast_mf__rddec_mad *pMad)
{
	register Tst_uint32 sampleCnt = pMad->dbuf.sampleCnt,
	                    y;
	mad_fixed_t         *pInpCh1  = (mad_fixed_t*)pMad->mSynth.pcm.samples[0],
	                    *pInpCh2  = (mad_fixed_t*)pMad->mSynth.pcm.samples[1];
	Tst_int32           *pBufOutI32ch1;

	ST_ASSERTN_NUM(ST_ERR_FAIL, pMad->dbuf.channels < 1 || pMad->dbuf.channels > 2 ||
			pMad->dbuf.bps != 32)

	LOC_RESZ_DECBUFOUTI32_(sampleCnt, pMad->dbuf.sampleCntRes, &pMad->dbuf)
	pBufOutI32ch1 = pMad->dbuf.ppBufOutI32[0];

	if (pMad->dbuf.channels == 2) {
		Tst_int32 *pBufOutI32ch2 = pMad->dbuf.ppBufOutI32[1];

		for (y = 0; y < sampleCnt; y++) {
			LOC_MADLINEARROUND_(24, pInpCh1[y], pMad->aStats, pBufOutI32ch1[y])
			pBufOutI32ch1[y] <<= 8;
			LOC_MADLINEARROUND_(24, pInpCh2[y], pMad->aStats, pBufOutI32ch2[y])
			pBufOutI32ch2[y] <<= 8;
		}
	} else {
		for (y = 0; y < sampleCnt; y++) {
			LOC_MADLINEARROUND_(24, pInpCh1[y], pMad->aStats, pBufOutI32ch1[y])
			pBufOutI32ch1[y] <<= 8;
		}
	}

	return ST_ERR_SUCC;
}

#endif  /* libmpg123 */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (HAVE_LIBMPG123 == 1)

/* Tast_mf__rddec_mpg123 */
static Tst_err
AST_MF__rddec_stc_initM123(Tast_mf__rddec_mpg123 *pM123)
{
	int resI = MPG123_OK;

	ST_ASSERTN_IARG(pM123 == NULL)

	pM123->pM123hnd = mpg123_new(NULL, &resI);
	if (resI != MPG123_OK)
		return ST_ERR_FAIL;

	pM123->printUsedBPS = ST_B_TRUE;
	/* */
	pM123->encoding = MPG123_ENC_ANY;
	ast_mf_rddec_stc_initDB(&pM123->dbuf);
	/* */
	return ST_ERR_SUCC;
}

static void
AST_MF__rddec_stc_freeM123(Tast_mf__rddec_mpg123 *pM123)
{
	ST_ASSERTN_VOID(pM123 == NULL)

	if (pM123->pM123hnd != NULL) {
		mpg123_delete(pM123->pM123hnd);
		pM123->pM123hnd = NULL;
	}
	ast_mf_rddec_stc_freeDB(&pM123->dbuf);
}

#elif (HAVE_LIBMAD == 1)

/* Tast_mf__rddec_mad */
static Tst_err
AST_MF__rddec_stc_initMad(Tast_mf__rddec_mad *pMad)
{
	ST_ASSERTN_IARG(pMad == NULL)

	pMad->printUsedBPS = ST_B_TRUE;
	/* */
	mad_stream_init(&pMad->mStream);
	mad_frame_init(&pMad->mFrame);
	mad_synth_init(&pMad->mSynth);
	/* */
	pMad->aStats.clipped_samples = 0;
	pMad->aStats.peak_clipping   = 0;
	pMad->aStats.peak_sample     = 0;
	/* */
	memset(pMad->guardBuf, 0, sizeof(pMad->guardBuf));

	ast_mf_rddec_stc_initDB(&pMad->dbuf);
	AST_MF__rddec_stc_initIB(&pMad->ibuf);

	return ST_ERR_SUCC;
}

static void
AST_MF__rddec_stc_freeMad(Tast_mf__rddec_mad *pMad)
{
	ST_ASSERTN_VOID(pMad == NULL)

	mad_synth_finish(&pMad->mSynth);
	mad_frame_finish(&pMad->mFrame);
	mad_stream_finish(&pMad->mStream);

	ast_mf_rddec_stc_freeDB(&pMad->dbuf);
	AST_MF__rddec_stc_freeIB(&pMad->ibuf);
}

#endif  /* libmpg123 */

/*----------------------------------------------------------------------------*/

#if (HAVE_LIBMPG123 != 1) && (HAVE_LIBMAD == 1)
/* Tast_mf__rddec_ibuf */
static void
AST_MF__rddec_stc_initIB(Tast_mf__rddec_ibuf *pIBuf)
{
	ST_ASSERTN_VOID(pIBuf == NULL)

	pIBuf->pInpBuf = NULL;
	/* */
	AST_MF__rddec_stc_rsetIB(pIBuf);
}

static void
AST_MF__rddec_stc_rsetIB(Tast_mf__rddec_ibuf *pIBuf)
{
	ST_ASSERTN_VOID(pIBuf == NULL)

	pIBuf->inpBufUsed = 0;
	pIBuf->inpBufSz   = 0;
	pIBuf->procInp    = 0;
	pIBuf->procInpTot = 0;
	pIBuf->wasDecoded = ST_B_FALSE;
	ST_DELPOINT(pIBuf->pInpBuf)
}
#endif  /* !libmpg123 && libmad */

#if (HAVE_LIBMPG123 == 1)
static Tst_err
AST_MF__rddec_stc_reszDecBufTmp(Tast_mf_rddec_dbuf *pDBuf,
		const Tst_uint32 newSz)
{
	ST_ASSERTN_IARG(pDBuf == NULL)

	if (newSz > pDBuf->bufTmpSz) {
		ST_REALLOC(pDBuf->pBufTmp, Tst_buf*, newSz, 1)
		if (pDBuf->pBufTmp == NULL)
			return ST_ERR_OMEM;
		memset(&pDBuf->pBufTmp[pDBuf->bufTmpSz], 0, newSz - pDBuf->bufTmpSz);
		pDBuf->bufTmpSz = newSz;
	}
	return ST_ERR_SUCC;
}
#endif  /* libmpg123 */

/******************************************************************************/
