/******************************************************************************/
/* cont_wav_wr.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for writing WAV containers                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.07.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/audvid/cont_wav.h"
/** */
#define SRC_CONT_WAV_WR_C
#include "cont_wav-prot.h"
/*** */
#include "cont_wav_stcs-prot.h"
#include "cont_wav_zfnc-prot.h"
#include "cont_wav_zfdeb-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_WAV_WR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* */
static Tst_err
ST_CONTWAV__wr_writeHd(const Tst_contWav_opts *pOpts,
                       Tst_contWav_handle_intn *pElemI);
static Tst_err
ST_CONTWAV__wr_writeRiff_head(const Tst_contWav_opts *pOpts,
                              Tst_contWav_handle_intn *pElemI,
                              const Tst_uint32 alignedDataSz,
                              const Tst_uint32 dataSz32);
static Tst_err
ST_CONTWAV__wr_writeAiff_head(const Tst_contWav_opts *pOpts,
                              Tst_contWav_handle_intn *pElemI,
                              const Tst_uint32 alignedDataSz,
                              const Tst_uint32 dataSz32);
/* */
static Tst_err
ST_CONTWAV__wr_writeRiff_waveFmtChunk(const Tst_contWav_opts *pOpts,
                                      Tst_contWav_handle_intn *pElemI);
static Tst_err
ST_CONTWAV__wr_writeAiff_formCommChunk(const Tst_contWav_opts *pOpts,
                                       Tst_contWav_handle_intn *pElemI);
/* */
static Tst_err
ST_CONTWAV__wr_writeSaneExtended(const Tst_contWav_opts *pOpts,
                                 Tst_sys_fstc *pFStcOut, Tst_uint32 val);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Callback for beginning to write a new stream
 *
 * pInputFilen: should contain a filename like "/foo/bar/fooiebary.flac"
 *              this will then be used to generate an output filename
 *                like "/foo/bar/fooiebary-1_01.wav"
 * bsIx:        bitstream index (>= 1)
 * bsSIx:       bitstream sub index (>= 1)
 *              bsIx and bsSIx will be also used to generate the output filenmae
 * appendBsIx:  append bsIx and bsSIx to output filename
 * ppHandle:    will contain the pointer to the Tst_contWav_handle used
 *                for this stream
 */
Tst_err
st_contWav_wr_prepareWritingCB(void *pWObj,
		const Tst_str *pInputFilen,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		const Tst_bool appendBsIx,
		void **ppHandle)
{
	const char *cFNCN = __func__;
	Tst_err res;
	Tst_contWav_handle      *pElem = NULL;
	Tst_contWav_handle_intn *pEI;
	Tst_contWav_obj_intn    *pWObjI;

	ST_ASSERTN_IARG(pWObj == NULL || pInputFilen == NULL ||
			bsIx == 0 || bsSIx == 0 || ppHandle == NULL)
	pWObjI = (Tst_contWav_obj_intn*)((Tst_contWav_obj*)pWObj)->pObInternal;
	ST_ASSERTN_IARG(pWObjI == NULL)

	*ppHandle = NULL;

	/* add new element to list */
	res = st_contwav_fnc_addNewElem(pWObjI, bsIx, bsSIx, &pElem);
	if (res != ST_ERR_SUCC)
		return res;
	pEI = (Tst_contWav_handle_intn*)pElem->pObInternal;

	/* create nice output filename */
	res = st_contwav_fnc_createOutpFn(pInputFilen, bsIx, bsSIx, appendBsIx,
			pWObjI->opts.wrFmtRIFForAIFF, pWObjI->opts.pOutpdir,
			pWObjI->opts.owExFiles, &pEI->pFilen);
	if (res != ST_ERR_SUCC) {
		st_contWav_d_err(&pWObjI->opts, cFNCN, pInputFilen,
				"couldn't create output filename for bs#%u.%02u",
				bsIx, bsSIx);
		return res;
	}

	/* */
	if (ST_AVFDEB_ISVERBAUD_BD(pWObjI->opts.basOpts)) {
		st_contWav_d_deb(&pWObjI->opts, cFNCN,
				"prepared %s ouput stream bs#%u.%02u for writing%s",
				(pWObjI->opts.wrFmtRIFForAIFF ? "RIFF" : "AIFF"),
				bsIx, bsSIx,
				(pWObjI->opts.basOpts.pretWr ? "  (pretending)" : ""));
	}

	/* create and open file */
	if (! pWObjI->opts.basOpts.pretWr) {
		if (pWObjI->opts.owExFiles && st_sysDoesFileExist(pEI->pFilen)) {
			/* remove existing file */
			if (! st_sysUnlinkFile(pEI->pFilen)) {
				st_contWav_d_err(&pWObjI->opts, cFNCN, pInputFilen,
						"couldn't delete output file for bs#%u.%02u ('%s')",
						bsIx, bsSIx, pEI->pFilen);
				return ST_ERR_FAIL;
			}
		}
		res = st_sysFStc_setFilen(&pEI->fstcOut, pEI->pFilen);
		if (res == ST_ERR_SUCC)
			res = st_sysFStc_openNew(&pEI->fstcOut);
		if (res != ST_ERR_SUCC) {
			st_contWav_d_err(&pWObjI->opts, cFNCN, pEI->pFilen,
					"couldn't create file '%s'",
					(char*)pEI->pFilen);
			return ST_ERR_FCWR;
		}
	}

	if (pWObjI->opts.cbNewOutpStr != NULL)
		pWObjI->opts.cbNewOutpStr(bsIx, bsSIx,
				pEI->pFilen, pWObjI->opts.basOpts.pretWr);
	*ppHandle = pElem;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Callback for setting the format of the output stream
 *
 * pHandle:       pointer to the Tst_contWav_handle used for this stream
 * sampleRate:    samplerate in Hz
 * bitsPerSample: used bits per sample, between 1-24 or exactly 32
 *                  if uneven then samples will be shifted
 * channMask:     for special channel encoding
 *                  default=0
 * channels:      amount of channels
 *
 * sampleRate, bitsPerSample, channMask and channels may not change
 *   after the first call of this function
 */
Tst_err
st_contWav_wr_setOutputFmtCB(void *pHandle,
		const Tst_uint32 sampleRate, const Tst_uint32 bitsPerSample,
		const Tst_uint32 channMask, const Tst_uint32 channels)
{
#	define LOC_SHIFT_  ((bitsPerSample % 8) > 0 ? 8 - (bitsPerSample % 8) : 0)
	const char *cFNCN = __func__;
	Tst_err res;
	Tst_contWav_handle_intn *pEI;

	ST_ASSERTN_IARG(pHandle == NULL ||
			sampleRate == 0 || bitsPerSample == 0 ||
			(bitsPerSample > 24 && bitsPerSample != 32) ||
			channels == 0)
	pEI = (Tst_contWav_handle_intn*)((Tst_contWav_handle*)pHandle)->pObInternal;
	ST_ASSERTN_IARG(pEI == NULL || pEI->pOpts == NULL)

	if (! pEI->pOpts->basOpts.pretWr && ! st_sysFStc_isOpen(&pEI->fstcOut)) {
		st_contWav_d_err(pEI->pOpts, cFNCN, pEI->pFilen,
				"output stream for bs#%u.%02u not open",
				pEI->bsIx, pEI->bsSIx);
		return ST_ERR_FAIL;
	}
	if (pEI->hdInfo.hdWritten) {
		st_contWav_d_err(pEI->pOpts, cFNCN, pEI->pFilen,
				"already wrote %s header for bs#%u.%02u",
				(pEI->pOpts->wrFmtRIFForAIFF ? "RIFF" : "AIFF"),
				pEI->bsIx, pEI->bsSIx);
		return ST_ERR_FAIL;
	}

	if (bitsPerSample + LOC_SHIFT_ != 8 &&
			bitsPerSample + LOC_SHIFT_ != 16 &&
			bitsPerSample + LOC_SHIFT_ != 24 &&
			bitsPerSample != 32) {
		st_contWav_d_err(pEI->pOpts, cFNCN, pEI->pFilen,
				"invalid bitsPerSample %u for bs#%u.%02u",
				bitsPerSample, pEI->bsIx, pEI->bsSIx);
		return ST_ERR_FAIL;
	}

	st_sysUInt64_setLoHi(&pEI->hdInfo.totSampleCnt, 1, 0);
	pEI->hdInfo.sampleRate    = sampleRate;
	pEI->hdInfo.bitsPerSample = bitsPerSample;
	pEI->hdInfo.channMask     = channMask;
	pEI->hdInfo.channels      = channels;
	/* write preliminary header */
	if (! pEI->pOpts->basOpts.pretWr) {
		res = ST_CONTWAV__wr_writeHd(pEI->pOpts, pEI);
		if (res != ST_ERR_SUCC) {
			st_contWav_d_err(pEI->pOpts, cFNCN, pEI->pFilen,
					"writing %s header failed for bs#%u.%02u",
					(pEI->pOpts->wrFmtRIFForAIFF ? "RIFF" : "AIFF"),
					pEI->bsIx, pEI->bsSIx);
			return res;
		}
	}
	/* */
	st_sysUInt64_setLoHi(&pEI->hdInfo.totSampleCnt, 0, 0);
	pEI->hdInfo.hdWritten = ST_B_TRUE;
	/* */
	if (ST_AVFDEB_ISVERBAUD_BD(pEI->pOpts->basOpts)) {
		st_contWav_d_deb(pEI->pOpts, cFNCN,
				"set %s header for bs#%u.%02u: "
				"sr=%u, bps=%u, ch=%u, cm=0x%04x",
				(pEI->pOpts->wrFmtRIFForAIFF ? "RIFF" : "AIFF"),
				pEI->bsIx, pEI->bsSIx,
				pEI->hdInfo.sampleRate, pEI->hdInfo.bitsPerSample,
				pEI->hdInfo.channels, pEI->hdInfo.channMask);
	}
	return ST_ERR_SUCC;
#	undef LOC_SHIFT_
}

/*----------------------------------------------------------------------------*/

/*
 * Callback for writing samples to output stream
 *
 * pHandle:      pointer to the Tst_contWav_handle used for this stream
 * sampleCnt:    amount of samples per channel
 * ppRawSamples: raw samples array
 *                 ppRawSamples[x][y] x=channel, y=samplenr.
 */
Tst_err
st_contWav_wr_writeRawSamplesCB(void *pHandle,
		const Tst_uint32 sampleCnt, const Tst_int32 **ppRawSamples)
{
#	define LOC_FNCN_      __func__
#	define LOC_WR_AS_BE_  (! pElemI->pOpts->wrFmtRIFForAIFF)
#	define LOC_BPS_       (pElemI->hdInfo.bitsPerSample)
#	define LOC_WR_AS_US_  (pElemI->pOpts->wrFmtRIFForAIFF ? \
				LOC_BPS_ <= 8 : ST_B_FALSE)
#	define LOC_SHIFT_     ((LOC_BPS_ % 8) > 0 ? 8 - (LOC_BPS_ % 8) : 0)
	Tst_err             res;
	Tst_int32 const     **ppSrcBuf = ppRawSamples;
	Tst_uint32          channels;
	register Tst_uint32 wideSample,
	                    sample,
	                    channel,
	                    byte,
	                    lbyte,
	                    byteCnt;
	Tst_contWav_handle_intn *pElemI;

	ST_ASSERTN_IARG(pHandle == NULL || ppRawSamples == NULL)
	pElemI = (Tst_contWav_handle_intn*)((Tst_contWav_handle*)pHandle)->pObInternal;
	ST_ASSERTN_IARG(pElemI == NULL || pElemI->pOpts == NULL)

	if ((! pElemI->pOpts->basOpts.pretWr &&
				! st_sysFStc_isOpen(&pElemI->fstcOut)) ||
			! pElemI->hdInfo.hdWritten)
		return ST_ERR_FAIL;

	channels = pElemI->hdInfo.channels;

	/* alloc some mem */
	if (sampleCnt * channels > pElemI->dbuf.dstBufSz) {
		res = st_contWav_stc_reszDBuf_dstBuf(&pElemI->dbuf,
				sampleCnt * channels);
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* shift raw samples */
	if (LOC_SHIFT_ > 0) {
		Tst_uint32 lshift = LOC_SHIFT_;

		if (sampleCnt * channels > pElemI->dbuf.srcBufShSz) {
			res = st_contWav_stc_reszDBuf_srcBufSh(&pElemI->dbuf,
					sampleCnt * channels);
			if (res != ST_ERR_SUCC)
				return res;
		}
		for (wideSample = 0; wideSample < sampleCnt; wideSample++)
			for (channel = 0; channel < channels; channel++)
				pElemI->dbuf.ppSrcBufSh[channel][wideSample] =
						ppRawSamples[channel][wideSample] << lshift;
		ppSrcBuf = (Tst_int32 const**)pElemI->dbuf.ppSrcBufSh;
	}

	/* first some special code for common cases */
	if (LOC_WR_AS_BE_ == pElemI->pOpts->isSystemBE &&
			! LOC_WR_AS_US_ &&
			channels == 2 &&
			LOC_BPS_ + LOC_SHIFT_ == 16) {
		Tst_int16 *pTmpS16 = pElemI->dbuf.pS16buf + 1;

		if (LOC_WR_AS_BE_)
			memcpy(pElemI->dbuf.pS16buf,
					((Tst_uint8*)(ppSrcBuf[0])) + 2,
					sizeof(Tst_int32) * sampleCnt - 2);
		else
			memcpy(pElemI->dbuf.pS16buf,
					ppSrcBuf[0],
					sizeof(Tst_int32) * sampleCnt);
		for (sample = 0; sample < sampleCnt; sample++, pTmpS16 += 2)
			*pTmpS16 = (Tst_int16)ppSrcBuf[1][sample];
		pElemI->dbuf.bytesToWrite = 4 * sample;
	}
	else if (LOC_WR_AS_BE_ == pElemI->pOpts->isSystemBE &&
			! LOC_WR_AS_US_ &&
			channels == 1 &&
			LOC_BPS_ + LOC_SHIFT_ == 16) {
		Tst_int16 *pTmpS16 = pElemI->dbuf.pS16buf;

		for (sample = 0; sample < sampleCnt; sample++)
			*pTmpS16++ = (Tst_int16)ppSrcBuf[0][sample];
		pElemI->dbuf.bytesToWrite = 2 * sample;
	}
	/* generic code for the rest */
	else if (LOC_BPS_ + LOC_SHIFT_ == 16) {
		if (LOC_WR_AS_US_) {
			if (channels == 2) {
				for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++) {
					pElemI->dbuf.pU16buf[sample++] =
							(Tst_uint16)(ppSrcBuf[0][wideSample] + 0x8000);
					pElemI->dbuf.pU16buf[sample++] =
							(Tst_uint16)(ppSrcBuf[1][wideSample] + 0x8000);
				}
			} else if (channels == 1) {
				for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++)
					pElemI->dbuf.pU16buf[sample++] =
							(Tst_uint16)(ppSrcBuf[0][wideSample] + 0x8000);
			} else {
				/* works for any 'channels' but above flavors are faster for 1 and 2 */
				for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++)
					for (channel = 0; channel < channels; channel++, sample++)
						pElemI->dbuf.pU16buf[sample] =
								(Tst_uint16)(ppSrcBuf[channel][wideSample] + 0x8000);
			}
		} else {
			if (channels == 2) {
				for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++) {
					pElemI->dbuf.pS16buf[sample++] =
							(Tst_int16)(ppSrcBuf[0][wideSample]);
					pElemI->dbuf.pS16buf[sample++] =
							(Tst_int16)(ppSrcBuf[1][wideSample]);
				}
			} else if (channels == 1) {
				for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++)
					pElemI->dbuf.pS16buf[sample++] =
							(Tst_int16)(ppSrcBuf[0][wideSample]);
			} else {
				/* works for any 'channels' but above flavors are faster for 1 and 2 */
				for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++)
					for (channel = 0; channel < channels; channel++, sample++)
						pElemI->dbuf.pS16buf[sample] =
								(Tst_int16)(ppSrcBuf[channel][wideSample]);
			}
		}
		if (LOC_WR_AS_BE_ != pElemI->pOpts->isSystemBE) {
			register Tst_uint8 tmpU8;

			byteCnt = sample * 2;
			for (byte = 0; byte < byteCnt; byte += 2) {
				tmpU8                         = pElemI->dbuf.pU8buf[byte];
				pElemI->dbuf.pU8buf[byte]     = pElemI->dbuf.pU8buf[byte + 1];
				pElemI->dbuf.pU8buf[byte + 1] = tmpU8;
			}
		}
		pElemI->dbuf.bytesToWrite = 2 * sample;
	}
	else if (LOC_BPS_ + LOC_SHIFT_ == 24 || LOC_BPS_ == 32) {
		for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++)
			for (channel = 0; channel < channels; channel++, sample++)
				pElemI->dbuf.pS32buf[sample] =
						ppSrcBuf[channel][wideSample];
		if (LOC_WR_AS_BE_ != pElemI->pOpts->isSystemBE) {
			register Tst_uint8 tmpU8;

			byteCnt = sample * 4;
			for (byte = 0; byte < byteCnt; byte += 4) {
				tmpU8                         = pElemI->dbuf.pU8buf[byte];
				pElemI->dbuf.pU8buf[byte]     = pElemI->dbuf.pU8buf[byte+3];
				pElemI->dbuf.pU8buf[byte + 3] = tmpU8;
				tmpU8                         = pElemI->dbuf.pU8buf[byte+1];
				pElemI->dbuf.pU8buf[byte + 1] = pElemI->dbuf.pU8buf[byte+2];
				pElemI->dbuf.pU8buf[byte + 2] = tmpU8;
			}
		}
		if (LOC_BPS_ == 32) {
			pElemI->dbuf.bytesToWrite = 4 * sample;
		} else {
			byteCnt = sample * 4;
			if (LOC_WR_AS_BE_) {
				for (lbyte = byte = 0; byte < byteCnt; ) {
					byte++;
					pElemI->dbuf.pU8buf[lbyte++] = pElemI->dbuf.pU8buf[byte++];
					pElemI->dbuf.pU8buf[lbyte++] = pElemI->dbuf.pU8buf[byte++];
					pElemI->dbuf.pU8buf[lbyte++] = pElemI->dbuf.pU8buf[byte++];
				}
			} else {
				for (lbyte = byte = 0; byte < byteCnt; ) {
					pElemI->dbuf.pU8buf[lbyte++] = pElemI->dbuf.pU8buf[byte++];
					pElemI->dbuf.pU8buf[lbyte++] = pElemI->dbuf.pU8buf[byte++];
					pElemI->dbuf.pU8buf[lbyte++] = pElemI->dbuf.pU8buf[byte++];
					byte++;
				}
			}
			pElemI->dbuf.bytesToWrite = 3 * sample;
		}
	}
	else if (LOC_BPS_ + LOC_SHIFT_ == 8) {
		if (LOC_WR_AS_US_) {
			for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++)
				for (channel = 0; channel < channels; channel++, sample++)
					pElemI->dbuf.pU8buf[sample] =
							(Tst_uint8)(ppSrcBuf[channel][wideSample] + 0x80);
		} else {
			for (sample = wideSample = 0; wideSample < sampleCnt; wideSample++)
				for (channel = 0; channel < channels; channel++, sample++)
					pElemI->dbuf.pDstBuf[sample] =
							(Tst_int8)(ppSrcBuf[channel][wideSample]);
		}
		pElemI->dbuf.bytesToWrite = sample;
	}

	/* */
	if (pElemI->dbuf.bytesToWrite > 0) {
		/* write samples to output stream */
		if (! pElemI->pOpts->basOpts.pretWr) {
			res = st_sysFStc_writeBuf(&pElemI->fstcOut,
					pElemI->dbuf.pU8buf, pElemI->dbuf.bytesToWrite);
			if (res != ST_ERR_SUCC)
				return res;
		}
		/* accumulate written samples */
		st_sysUInt64_addUI32(&pElemI->hdInfo.totSampleCnt, sampleCnt, NULL);
	}

	return ST_ERR_SUCC;
#	undef LOC_FNCN_
#	undef LOC_WR_AS_BE_
#	undef LOC_WR_AS_US_
#	undef LOC_BPS_
#	undef LOC_SHIFT_
}

/*----------------------------------------------------------------------------*/

/*
 * Callback for finishing a stream
 *   Updates the file header and closes the stream
 *
 * pHandle: pointer to the Tst_contWav_handle used for this stream
 */
Tst_err
st_contWav_wr_finishWritingCB(void *pHandle)
{
	const char *cFNCN = __func__;
	Tst_err res = ST_ERR_SUCC;
	Tst_str decUI[50];
	Tst_contWav_handle_intn *pEI;

	ST_ASSERTN_IARG(pHandle == NULL)
	pEI = (Tst_contWav_handle_intn*)((Tst_contWav_handle*)pHandle)->pObInternal;
	ST_ASSERTN_IARG(pEI == NULL || pEI->pOpts == NULL)

	if (pEI->pOpts->basOpts.pretWr || st_sysFStc_isOpen(&pEI->fstcOut)) {
		if (pEI->hdInfo.hdWritten && ! pEI->pOpts->basOpts.pretWr &&
				st_sysUInt64_cmpLoHi(&pEI->hdInfo.totSampleCnt, 0, 0) > 0) {
			st_sysFStc_setFPos(&pEI->fstcOut, 0, ST_SYS_SETFPOS_BEG);
			/* update header */
			res = ST_CONTWAV__wr_writeHd(pEI->pOpts, pEI);
			if (res != ST_ERR_SUCC) {
				st_contWav_d_err(pEI->pOpts, cFNCN, pEI->pFilen,
						"updating %s header failed for bs#%u.%02u",
						(pEI->pOpts->wrFmtRIFForAIFF ? "RIFF" : "AIFF"),
						pEI->bsIx, pEI->bsSIx);
			}
		}
		/* */
		if (! pEI->pOpts->basOpts.pretWr)
			st_sysFStc_close(&pEI->fstcOut);
		/* */
		if (ST_AVFDEB_ISVERBAUD_BD(pEI->pOpts->basOpts)) {
			st_contWav_d_deb(pEI->pOpts, cFNCN,
					"finished writing %s ouput stream bs#%u.%02u",
					(pEI->pOpts->wrFmtRIFForAIFF ? "RIFF" : "AIFF"),
					pEI->bsIx, pEI->bsSIx);
			/* */
			st_sysUInt64_toDecStr(&pEI->hdInfo.totSampleCnt, decUI, sizeof(decUI));
			st_contWav_d_deb(pEI->pOpts, cFNCN,
					"wrote %s samples to stream bs#%u.%02u",
					(char*)decUI, pEI->bsIx, pEI->bsSIx);
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_CONTWAV__wr_writeHd(const Tst_contWav_opts *pOpts,
		Tst_contWav_handle_intn *pElemI)
{
	Tst_uint64 dataSz64;
	Tst_uint32 alignedDataSz32;

	ST_ASSERTN_IARG(pElemI == NULL)

	/* */
	if (pElemI->hdInfo.channMask == 0) {
		switch (pElemI->hdInfo.channels) {
		case 1: pElemI->hdInfo.channMask = 0x0001; break;
		case 2: pElemI->hdInfo.channMask = 0x0003; break;
		case 3: pElemI->hdInfo.channMask = 0x0007; break;
		case 4: pElemI->hdInfo.channMask = 0x0033; break;
		case 5: pElemI->hdInfo.channMask = 0x0607; break;
		case 6: pElemI->hdInfo.channMask = 0x060f; break;
		default: break;
		}
	}
	/* */
	/** */
	pElemI->hdInfo.isExtensibleWFmt = (pElemI->hdInfo.channMask == 2 ||
			pElemI->hdInfo.channMask > 0x0003 ||
			pElemI->hdInfo.bitsPerSample % 8 != 0 ||
			pElemI->hdInfo.channels > 2);
	/** */
	ST_SYSMATH_STC_RSETUI64(dataSz64)
	st_sysUInt64_setUI64(&dataSz64, &pElemI->hdInfo.totSampleCnt);
	st_sysUInt64_multUI32(&dataSz64, pElemI->hdInfo.channels, NULL);
	st_sysUInt64_multUI32(&dataSz64, (pElemI->hdInfo.bitsPerSample + 7) / 8, NULL);
	if (st_sysUInt64_cmpLoHi(&dataSz64, 0xffffffff - 60 - 8, 0) >= 0)
		return ST_ERR_FAIL;
	/** aligned data size needs to be even */
	alignedDataSz32 = (st_sysUInt64_toUI32(&dataSz64, NULL) + 1) & (~1);

	if (pOpts->wrFmtRIFForAIFF)
		return ST_CONTWAV__wr_writeRiff_head(pOpts, pElemI, alignedDataSz32,
				st_sysUInt64_toUI32(&dataSz64, NULL));
	return ST_CONTWAV__wr_writeAiff_head(pOpts, pElemI, alignedDataSz32,
			st_sysUInt64_toUI32(&dataSz64, NULL));
}

static Tst_err
ST_CONTWAV__wr_writeRiff_head(const Tst_contWav_opts *pOpts,
		Tst_contWav_handle_intn *pElemI,
		const Tst_uint32 alignedDataSz, const Tst_uint32 dataSz32)
{
	Tst_err    res;
	Tst_uint32 ui32;

	/* Form Chunk */
	/** chunkID: "RIFF" */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "RIFF", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** chunkSize: filesize - 8 */
	ui32 = alignedDataSz + (pElemI->hdInfo.isExtensibleWFmt ? 60 : 36);
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** formType: "WAVE" */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "WAVE", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* Format Chunk */
	/** */
	res = ST_CONTWAV__wr_writeRiff_waveFmtChunk(pOpts, pElemI);
	if (res != ST_ERR_SUCC)
		return res;

	/* Sound Data Chunk */
	/** chunkID: "data" */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "data", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** chunkSize: data size */
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(dataSz32);  /* ENDIAN: BE-->LE */
#	else
	ui32 = dataSz32;
#	endif
	return st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
}

static Tst_err
ST_CONTWAV__wr_writeAiff_head(const Tst_contWav_opts *pOpts,
		Tst_contWav_handle_intn *pElemI,
		const Tst_uint32 alignedDataSz, const Tst_uint32 dataSz32)
{
	Tst_err    res;
	Tst_uint32 ssndOffsetSz = 0,
	           ui32;

	/* Form Chunk */
	/** chunkID: "FORM" */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "FORM", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** chunkSize: filesize - 8 */
	ui32 = alignedDataSz + 46 + ssndOffsetSz;
#	if (WORDS_BIGENDIAN != 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** formType: "AIFF" */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "AIFF", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* Format Version Chunk */
	/** */
	res = ST_CONTWAV__wr_writeAiff_formCommChunk(pOpts, pElemI);
	if (res != ST_ERR_SUCC)
		return res;

	/* Sound Data Chunk */
	/** chunkID: "SSND" */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "SSND", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** chunkSize: data size */
	ui32 = dataSz32 + 8 + ssndOffsetSz;
#	if (WORDS_BIGENDIAN != 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** offset (offset to sound data) */
#	if (WORDS_BIGENDIAN != 1)
	ui32 = st_sysReverseByteOrder_UI32(ssndOffsetSz);  /* ENDIAN: LE-->BE */
#	else
	ui32 = ssndOffsetSz;
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/** blockSize */
#	if (WORDS_BIGENDIAN != 1)
	ui32 = /*st_sysReverseByteOrder_UI32(0);*/0;  /* ENDIAN: LE-->BE */
#	else
	ui32 = 0;
#	endif
	return st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_CONTWAV__wr_writeRiff_waveFmtChunk(const Tst_contWav_opts *pOpts,
		Tst_contWav_handle_intn *pElemI)
{
	Tst_err    res;
	Tst_uint32 ui32;
	Tst_uint16 ui16;

	/* chunkID: "fmt " */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "fmt ", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* chunkSize */
	ui32 = (pElemI->hdInfo.isExtensibleWFmt ? 40 : 16);
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* formatTag (compression code) */
	ui16 = (pElemI->hdInfo.isExtensibleWFmt ?
				ST_CONTWAV_WAVEFMT_EXTENSIBLE :
				ST_CONTWAV_WAVEFMT_PCM);
#	if (WORDS_BIGENDIAN == 1)
	ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* channels */
	ui16 = pElemI->hdInfo.channels;
#	if (WORDS_BIGENDIAN == 1)
	ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* sample rate */
	ui32 = pElemI->hdInfo.sampleRate;
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* average bytes per second */
	ui32 = pElemI->hdInfo.sampleRate * pElemI->hdInfo.channels *
			((pElemI->hdInfo.bitsPerSample + 7) / 8);
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* block align */
	ui16 = pElemI->hdInfo.channels * ((pElemI->hdInfo.bitsPerSample + 7) / 8);
#	if (WORDS_BIGENDIAN == 1)
	ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* bits per sample */
	ui16 = ((pElemI->hdInfo.bitsPerSample + 7) / 8) * 8;
#	if (WORDS_BIGENDIAN == 1)
	ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: BE-->LE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* */
	if (pElemI->hdInfo.isExtensibleWFmt) {
		/* cbSize (size of the extension) */
		ui16 = 22;
#		if (WORDS_BIGENDIAN == 1)
		ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: BE-->LE */
#		endif
		res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
		if (res != ST_ERR_SUCC)
			return res;

		/* validBitsPerSample */
		ui16 = pElemI->hdInfo.bitsPerSample;
#		if (WORDS_BIGENDIAN == 1)
		ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: BE-->LE */
#		endif
		res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
		if (res != ST_ERR_SUCC)
			return res;

		/* channel mask (Speaker position mask) */
		ui32 = pElemI->hdInfo.channMask;
#		if (WORDS_BIGENDIAN == 1)
		ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#		endif
		res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
		if (res != ST_ERR_SUCC)
			return res;

		/* SubFormat
		 *   GUID = {0x00000001, 0x0000, 0x0010,
		 *           {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}} */
		res = st_sysFStc_writeBuf(&pElemI->fstcOut,
				"\x01\x00\x00\x00"
					"\x00\x00"
					"\x10\x00"
					"\x80\x00\x00\xaa\x00\x38\x9b\x71",
				16);
		if (res != ST_ERR_SUCC)
			return res;
	}

	return ST_ERR_SUCC;
}

static Tst_err
ST_CONTWAV__wr_writeAiff_formCommChunk(const Tst_contWav_opts *pOpts,
		Tst_contWav_handle_intn *pElemI)
{
	Tst_err    res;
	Tst_uint32 ui32;
	Tst_uint16 ui16;

	/* "COMM" */
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, "COMM", 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* chunkSize */
	ui32 = 18;
#	if (WORDS_BIGENDIAN != 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* channels */
	ui16 = pElemI->hdInfo.channels;
#	if (WORDS_BIGENDIAN != 1)
	ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* total samples */
	ui32 = st_sysUInt64_toUI32(&pElemI->hdInfo.totSampleCnt, NULL);
#	if (WORDS_BIGENDIAN != 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* bits per sample */
	ui16 = pElemI->hdInfo.bitsPerSample;
#	if (WORDS_BIGENDIAN != 1)
	ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(&pElemI->fstcOut, &ui16, 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* sample rate */
	return ST_CONTWAV__wr_writeSaneExtended(pOpts, &pElemI->fstcOut,
			pElemI->hdInfo.sampleRate);
}

/*----------------------------------------------------------------------------*/

/*
 * SANE extended is an 80-bit IEEE-754 representation with sign bit, 15 bits
 * of exponent, and 64 bits of significand (mantissa).  Unlike most IEEE-754
 * representations, it does not imply a 1 above the MSB of the significand.
 *
 * Preconditions:
 *  val != 0
 */
static Tst_err
ST_CONTWAV__wr_writeSaneExtended(const Tst_contWav_opts *pOpts,
		Tst_sys_fstc *pFStcOut, Tst_uint32 val)
{
	Tst_err    res;
	Tst_uint32 shift,
	           exponent,
	           ui32;
	Tst_uint16 ui16;

	ST_ASSERTN_IARG(val == 0)

	for (shift = 0; (val >> (31 - shift)) == 0; ++shift) ;
	val    <<= shift;
	exponent = 63 - (shift + 32);  /* add 32 for unused second word */

	/* exponent */
	ui16 = exponent + 0x3FFF;
#	if (WORDS_BIGENDIAN != 1)
	ui16 = st_sysReverseByteOrder_UI16(ui16);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(pFStcOut, &ui16, 2);
	if (res != ST_ERR_SUCC)
		return res;

	/* mantissa, first word */
	ui32 = val;
#	if (WORDS_BIGENDIAN != 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: LE-->BE */
#	endif
	res = st_sysFStc_writeBuf(pFStcOut, &ui32, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* mantissa, unused second word */
	ui32 = 0;
	return st_sysFStc_writeBuf(pFStcOut, &ui32, 4);
}

/******************************************************************************/
