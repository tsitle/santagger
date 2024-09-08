/******************************************************************************/
/* mf_read_decvrb.c               [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for decoding Ogg-Vorbis audio                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_math.h"
/** */
#define SRC_MF_READ_DECVRB_C
#include "mf_read_decvrb-pp.h"
/*** */
#include "mf_read_dbuf-prot.h"
#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#undef SRC_MF_READ_DECVRB_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), random() */
#include <string.h>      /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (HAVE_LIBVORBIS == 1)

/*
 * Create new handle
 */
Tst_err
ast_mf_rddec_cbDecVorb_hndNew(void **ppHnd, const Tst_bool printUsedBPS)
{
	Tst_err res;
	Tast_mf__rddec_vorb *pVrb = NULL;

	ST_ASSERTN_IARG(ppHnd == NULL)

	ST_CALLOC(pVrb, Tast_mf__rddec_vorb*, 1, sizeof(Tast_mf__rddec_vorb))
	if (pVrb == NULL)
		return ST_ERR_OMEM;

	*ppHnd = pVrb;
	/* */
	res = AST_MF__rddec_stc_initVrb(pVrb);
	/* */
	pVrb->printUsedBPS = printUsedBPS;

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Free handle
 */
void
ast_mf_rddec_cbDecVorb_hndDel(void *pHnd)
{
	Tast_mf__rddec_vorb *pVrb = (Tast_mf__rddec_vorb*)pHnd;

	ST_ASSERTN_VOID(pHnd == NULL)

	AST_MF__rddec_stc_freeVrb(pVrb);
	ST_DELPOINT(pVrb)
}

/*----------------------------------------------------------------------------*/

/*
 * Set output format
 *
 * pHnd                : decoder handle
 * sampleRate          : sampleRate in Hz
 * channels            : amount of channels
 * desiredBitsPerSample: output resolution (0, 16, 24, 32)
 *                       0=let decoder decide
 * pUsedBitsPerSample  : output resolution set by decoder
 */
Tst_err
ast_mf_rddec_cbDecVorb_setOutputFmt(void *pHnd,
		const Tst_uint32 sampleRate, const Tst_uint32 channels,
		const Tst_uint32 desiredBitsPerSample,
		Tst_uint32 *pUsedBitsPerSample)
{
	Tast_mf_rddec_dbuf  *pDBuf;
	Tast_mf__rddec_vorb *pVrb = (Tast_mf__rddec_vorb*)pHnd;

	ST_ASSERTN_IARG(pHnd == NULL || pUsedBitsPerSample == NULL ||
			(desiredBitsPerSample != 0 && desiredBitsPerSample != 16 &&
				desiredBitsPerSample != 24 && desiredBitsPerSample != 32))

	*pUsedBitsPerSample = 0;

	pDBuf = &pVrb->dbuf;
	if (desiredBitsPerSample == 0)
		pDBuf->bps = 16;
	else
		pDBuf->bps = desiredBitsPerSample;

	/* */
	if (pVrb->printUsedBPS) {
		/*char msg[512];*/

		/*snprintf(msg, sizeof(msg), " - converted from %ubit float",
				(Tst_uint32)(sizeof(float) * 8));
		ast_mf_op_prMsg("(Using %ubit output%s, decoding with libvorbis)",
				pDBuf->bps, msg);*/
		ast_mf_op_prMsg("(Using %ubit output, decoding with libvorbis)",
				pDBuf->bps);
	}
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
ast_mf_rddec_cbDecVorb_openFeed(void *pHnd)
{
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* since libvorbis depends on libogg which we don't use
 *   for parsing ogg-files, we have to do some ugly hacking
 *   here - meening we have to access internal data of libvorbis  */
#define LOC_UPD_OGGPACKET_(mac_oggPack, mac_pInpBuf, mac_inpBufSz)  { \
			(mac_oggPack).packet = (unsigned char*)(mac_pInpBuf); \
			(mac_oggPack).bytes  = (long)(mac_inpBufSz); \
			}
#define LOC_SET_OGGPACKET_(mac_oggPack, mac_pInpBuf, mac_inpBufSz, \
				mac_isBOS, mac_granPos)  { \
			(mac_oggPack).packetno++; \
			LOC_UPD_OGGPACKET_(mac_oggPack, mac_pInpBuf, mac_inpBufSz) \
			(mac_oggPack).b_o_s      = (mac_isBOS); \
			(mac_oggPack).e_o_s      = 0;  /* FALSE */ \
			(mac_oggPack).granulepos = (ogg_int64_t)(mac_granPos).nativeU64; \
			}
#define LOC_GET_CONSUMED_(mac_pVorbBlock)  \
			(Tst_uint32)((mac_pVorbBlock)->opb.endbyte + \
				((mac_pVorbBlock)->opb.endbit + 7) / 8)

/*
 * Feed input data - Header  (exactly one Ogg-Packet)
 *
 * pHnd      : decoder handle
 * pInpBuf   : data buffer
 * inpBufSz  : length of data buffer
 * hdTypeBOS : is first packet of stream ?
 * granulePos: granule pos (should be 0)
 */
Tst_err
ast_mf_rddec_cbDecVorb_feedHeader(void *pHnd,
		Tst_buf *pInpBuf, const Tst_uint32 inpBufSz,
		const Tst_bool hdTypeBOS, const Tst_uint64 granulePos)
{
	const char *cFNCN = __FUNCTION__;
	int resI;
	Tast_mf__rddec_vorb *pVrb = (Tast_mf__rddec_vorb*)pHnd;

	ST_ASSERTN_IARG(pHnd == NULL || pInpBuf == NULL)

	if (inpBufSz == 0)
		return ST_ERR_SUCC;

	/**ast_mf_op_prBytes(pInpBuf, inpBufSz);**/

	/* since libvorbis depends on libogg which we don't use
	 *   for parsing ogg-files, we have to do some ugly hacking
	 *   here - meening we have to access internal data
	 *          of libvorbis and libogg  */

	LOC_SET_OGGPACKET_(pVrb->oggPckt, pInpBuf, inpBufSz,
			(hdTypeBOS ? 1 : 0), granulePos)

	resI = vorbis_synthesis_headerin(&pVrb->vrbInfo, &pVrb->vrbCmt,
			&pVrb->oggPckt);
	if (resI != 0) {
		AST_MF__rddec_printLibErr(cFNCN, resI);
		return ST_ERR_FAIL;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Feed input data - Audio  (exactly one Ogg-Packet)
 *   and decode it into raw PCM samples
 * If cbPCMclient != NULL and pPCMclientHandle != NULL
 *   then the decoded samples will be passed directly
 *   to the PCM client
 *
 * pHnd            : decoder handle
 * pInpBuf         : data buffer
 * inpBufSz        : length of data buffer
 * granulePos      : granule pos
 * cbPCMclient     : callback fnc of PCM client
 * pPCMclientHandle: PCM client handle
 */
Tst_err
ast_mf_rddec_cbDecVorb_feedAudioAndDecode(void *pHnd,
		Tst_buf *pInpBuf, const Tst_uint32 inpBufSz,
		const Tst_uint64 granulePos,
		Tst_av_cbPCM_samplesClient cbPCMclient,
		void *pPCMclientHandle)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res = ST_ERR_SUCC;
	int        resI,
	           availableSamples;
#	if (CONFIG_ST_ALL_DEBUG == 1)
	Tst_uint32 consumedOne;
#	endif
	Tast_mf_rddec_dbuf  *pDBuf;
	Tast_mf__rddec_vorb *pVrb = (Tast_mf__rddec_vorb*)pHnd;

	ST_ASSERTN_IARG(pHnd == NULL || pInpBuf == NULL ||
			! pVrb->dbuf.isOutpFmtSet)

	if (inpBufSz == 0)
		return ST_ERR_SUCC;

	/* */
	pDBuf = &pVrb->dbuf;

	LOC_SET_OGGPACKET_(pVrb->oggPckt, NULL, 0,
			0 /* FALSE */, granulePos)

	/* if this is the first non-header (aka audio) packet init some stuff */
	if (pVrb->pVrbDSP == NULL) {
		ST_CALLOC(pVrb->pVrbDSP, vorbis_dsp_state*,
				1, sizeof(vorbis_dsp_state))
		if (pVrb->pVrbDSP == NULL)
			return ST_ERR_OMEM;
		ST_CALLOC(pVrb->pVrbBlock, vorbis_block*,
				1, sizeof(vorbis_block))
		if (pVrb->pVrbBlock == NULL)
			return ST_ERR_OMEM;
		/* */
		if (vorbis_synthesis_init(pVrb->pVrbDSP, &pVrb->vrbInfo) != 0)
			return ST_ERR_FAIL;
		if (vorbis_block_init(pVrb->pVrbDSP, pVrb->pVrbBlock) != 0)
			return ST_ERR_FAIL;
	}

	/* decode samples */
	pDBuf->sampleCnt = 0;
	LOC_UPD_OGGPACKET_(pVrb->oggPckt, pInpBuf, inpBufSz)

	/* decode a Vorbis packet into a block of data */
	resI = vorbis_synthesis(pVrb->pVrbBlock, &pVrb->oggPckt);
	if (resI != 0) {
		AST_MF__rddec_printLibErr(cFNCN, resI);
		return ST_ERR_FAIL;
	}
	/**ast_mf_op_prf("\t\tendbyte %ld, endbit %d (%d)\n",
			pVrb->pVrbBlock->opb.endbyte, pVrb->pVrbBlock->opb.endbit,
			(pVrb->pVrbBlock->opb.endbit +7) / 8);**/
#	if (CONFIG_ST_ALL_DEBUG == 1)
	consumedOne = LOC_GET_CONSUMED_(pVrb->pVrbBlock);
	if (consumedOne != inpBufSz)
		return ST_ERR_IDAT;
#	endif

	/* submit a vorbis_block for assembly into the final decoded audio */
	resI = vorbis_synthesis_blockin(pVrb->pVrbDSP, pVrb->pVrbBlock);
	if (resI != 0) {
		AST_MF__rddec_printLibErr(cFNCN, resI);
		return ST_ERR_FAIL;
	}

	/* decode samples */
	availableSamples = vorbis_synthesis_pcmout(pVrb->pVrbDSP, &pVrb->ppPcmIn);
	if (availableSamples > 0) {
		if (cbPCMclient != NULL && pPCMclientHandle != NULL) {
			pDBuf->sampleCnt = (Tst_uint32)availableSamples;
			/* convert samples */
			res = AST_MF__rddec_convVorb_rawSamples(pVrb);
			if (res == ST_ERR_SUCC) {
				/* pass samples to client */
				res = cbPCMclient(pPCMclientHandle,
						pDBuf->sampleCnt, (const Tst_int32**)pDBuf->ppBufOutI32);
			}
		}
		/* */
		if (res == ST_ERR_SUCC)
			vorbis_synthesis_read(pVrb->pVrbDSP, availableSamples);
	}

	return res;
}

#undef LOC_UPD_OGGPACKET_
#undef LOC_SET_OGGPACKET_
#undef LOC_GET_CONSUMED_

/*----------------------------------------------------------------------------*/

/*
 * Close feed
 */
Tst_err
ast_mf_rddec_cbDecVorb_closeFeed(void *pHnd)
{
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define LOC_RESZ_DECBUFOUTI32_(mac_sampleCnt, mac_sampleRes, mac_pDBuf)  \
			if ((mac_sampleCnt) > (mac_sampleRes)) { \
				Tst_err res = ast_mf_rddec_stc_reszDecBufOutI32(mac_pDBuf, \
							mac_sampleCnt); \
				if (res != ST_ERR_SUCC) \
					return res; \
			}

/*
 * Returns next Tst_int32 smaller or equal to val
 */
#if 0
static ST_INLINE_S
Tst_int32 AST_MF__rddec_floor(const float val)
{
	Tst_int32 fl = (Tst_int32)(val + 0.5f);

	/*  1.8 + 0.5 =  2.3 -->  2,   2-1= 1,   1.0 <=  1.8
	 * -1.9 + 0.5 = -1.4 --> -1,  -1-1=-2,  -2.0 <= -1.9 */
	if ((float)fl > val)
		--fl;
	return fl;
}
#endif

#define LOC_FLOOR_(mac_ival, mac_fval)  { \
			(mac_ival) = (Tst_int32)(mac_fval + 0.5f); \
			if ((float)(mac_ival) > (mac_fval)) \
				--(mac_ival); \
			}

/* this code was adapted from libvorbis-1.3.1 */
#define LOC_CONVSAMPLE_(mac_inpSample, mac_outSample, mac_min, \
				mac_max, mac_tempIVal, mac_tempFVal)  { \
			(mac_tempFVal) = ((mac_inpSample) * (float)(mac_max)); \
			LOC_FLOOR_(mac_tempIVal, mac_tempFVal) \
			if ((mac_tempIVal) > (mac_max)) \
				(mac_tempIVal) = (mac_max); \
			if ((mac_tempIVal) < (mac_min)) \
				(mac_tempIVal) = (mac_min); \
			(mac_outSample) = (mac_tempIVal); \
			}

#define LOC_INT_MIN24_  (-8388608)
#define LOC_INT_MAX24_  8388607

static Tst_err
AST_MF__rddec_convVorb_rawSamples(Tast_mf__rddec_vorb *pVrb)
{
	const Tst_uint32    sCnt = pVrb->dbuf.sampleCnt;
	Tst_uint32          x;
	register Tst_uint32 y;
	Tst_int32           tempValI32,
	                    minVal,
	                    maxVal;
	float               tempValF;

	ST_ASSERTN_NUM(ST_ERR_FAIL,
			pVrb->dbuf.channels < 1 ||
			(pVrb->dbuf.bps != 16 && pVrb->dbuf.bps != 24 &&
				pVrb->dbuf.bps != 32) ||
			pVrb->ppPcmIn == NULL)

	LOC_RESZ_DECBUFOUTI32_(sCnt, pVrb->dbuf.sampleCntRes, &pVrb->dbuf)

	switch (pVrb->dbuf.bps) {
	case 16: minVal = ST_TINT16_MIN;  maxVal = ST_TINT16_MAX; break;
	case 24: minVal = LOC_INT_MIN24_; maxVal = LOC_INT_MAX24_; break;
	default: minVal = ST_TINT32_MIN;  maxVal = ST_TINT32_MAX;
	}

	if (pVrb->dbuf.channels == 2) {
		float     *pPCMch1       = pVrb->ppPcmIn[0],
		          *pPCMch2       = pVrb->ppPcmIn[1];
		Tst_int32 *pBufOutI32ch1 = pVrb->dbuf.ppBufOutI32[0];
		Tst_int32 *pBufOutI32ch2 = pVrb->dbuf.ppBufOutI32[1];

		for (y = 0; y < sCnt; y++) {
			/* channel 1 */
			LOC_CONVSAMPLE_(pPCMch1[y], pBufOutI32ch1[y],
					minVal, maxVal, tempValI32, tempValF)
			/* channel 2 */
			LOC_CONVSAMPLE_(pPCMch2[y], pBufOutI32ch2[y],
					minVal, maxVal, tempValI32, tempValF)
		}
	} else {
		Tst_int32 *pBufOutI32chX;
		float     *pPCMchX;

		for (x = 0; x < pVrb->dbuf.channels; x++) {
			pPCMchX       = pVrb->ppPcmIn[x];
			pBufOutI32chX = pVrb->dbuf.ppBufOutI32[x];

			for (y = 0; y < sCnt; y++) {
				LOC_CONVSAMPLE_(pPCMchX[y], pBufOutI32chX[y],
						minVal, maxVal, tempValI32, tempValF)
			}
		}
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

static void
AST_MF__rddec_printLibErr(const char *pFnc, const int libErr)
{
	switch (libErr) {
	case 0:
		break;
	case OV_ENOTAUDIO:
		ast_mf_op_prErr(pFnc,
				"error from libvorbis: packet is not an audio packet");
		break;
	case OV_EBADPACKET:
		ast_mf_op_prErr(pFnc,
				"error from libvorbis: there was an error in the packet");
		break;
	case OV_EINVAL:
		ast_mf_op_prErr(pFnc,
				"error from libvorbis: "
				"decoder is in an invalid state to accept blocks");
		break;
	case OV_ENOTVORBIS:
		ast_mf_op_prErr(pFnc,
				"error from libvorbis: "
				"packet is not a Vorbis header packet");
		break;
	case OV_EBADHEADER:
		ast_mf_op_prErr(pFnc,
				"error from libvorbis: "
				"there was an error interpreting the packet");
		break;
	case OV_EFAULT:
		ast_mf_op_prErr(pFnc,
				"error from libvorbis: internal error");
		break;
	default:
		ast_mf_op_prErr(pFnc,
				"unknown error from libvorbis: %d", libErr);
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tast_mf__rddec_vorb */
static Tst_err
AST_MF__rddec_stc_initVrb(Tast_mf__rddec_vorb *pVrb)
{
	ST_ASSERTN_IARG(pVrb == NULL)

	pVrb->printUsedBPS = ST_B_TRUE;
	/* */
	vorbis_info_init(&pVrb->vrbInfo);
	vorbis_comment_init(&pVrb->vrbCmt);
	pVrb->pVrbDSP   = NULL;
	pVrb->pVrbBlock = NULL;
	/* */
	pVrb->oggPckt.packet = NULL;
	ogg_packet_clear(&pVrb->oggPckt);
	/* */
	pVrb->ppPcmIn = NULL;
	/* */
	ast_mf_rddec_stc_initDB(&pVrb->dbuf);

	return ST_ERR_SUCC;
}

static void
AST_MF__rddec_stc_freeVrb(Tast_mf__rddec_vorb *pVrb)
{
	ST_ASSERTN_VOID(pVrb == NULL)

	if (pVrb->pVrbBlock != NULL) {
		vorbis_block_clear(pVrb->pVrbBlock);
		ST_DELPOINT(pVrb->pVrbBlock)
	}
	if (pVrb->pVrbDSP != NULL) {
		vorbis_dsp_clear(pVrb->pVrbDSP);
		ST_DELPOINT(pVrb->pVrbDSP)
	}
	vorbis_comment_clear(&pVrb->vrbCmt);
	vorbis_info_clear(&pVrb->vrbInfo);  /* must be called last */
	/* */
	ast_mf_rddec_stc_freeDB(&pVrb->dbuf);
	pVrb->ppPcmIn = NULL;
}

#endif  /* libvorbis */

/******************************************************************************/
