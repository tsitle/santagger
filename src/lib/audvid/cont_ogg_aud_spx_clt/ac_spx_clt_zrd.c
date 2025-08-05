/******************************************************************************/
/* ac_spx_clt_zrd.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading Ogg-Speex and Ogg-Celt                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/cont_ogg.h"
/** */
#define SRC_AC_SPX_CLT_ZRD_C
#include "ac_spx_clt_zrd-prot.h"
/*** */
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_AC_SPX_CLT_ZRD_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Speex header packet
 */
Tst_err
st_contOgg_spxrd_parseHPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj,
		Tst_contOgg_cbStripStr cbStripStr)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 ui32    = 0,
	           rdBytes = 0;
	Tst_contOgg_speeIdent *pSID = &pBSI->pSpee->ident;

	if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
				"Speex header found");

	if (st_streamrd_getAmountRemainingBitsInCurByte(pSObj) != 8)
		return ST_ERR_IDAT;

	/* */
	/** VERSION */
	ST_REALLOC(pSID->pVersStr, Tst_str*, 20 + 1, 1)
	if (pSID->pVersStr == NULL)
		return ST_ERR_OMEM;
	res = st_streamrd_rdBuffer_noCRC(pSObj, 20, pSID->pVersStr, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != 20) {
		if (res == ST_ERR_SUCC)
			res = ST_ERR_IDAT;
		return res;
	}
	pSID->pVersStr[20] = 0x00;
	cbStripStr(pSID->pVersStr, 20);

	/** SPEEX_VERSION_ID */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->speexVers);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" speex svers 0x%08x\n", pSID->speexVers);**/
	if ((Tst_int32)pSID->speexVers == -1) {
		pSID->speexVers = (1 << 31);
		/**st_contOgg_prf("   --> svers 0x%08x\n", pSID->speexVers);**/
	}

	/** HEADER_SZ */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->headerSize);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (pSID->headerSize != 80) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"correcting header size %u --> %u",
					pSID->headerSize, 80);
		pSID->headerSize = 80;
	}

 	/** SAMPLERATE */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->sampleRate);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" speex srate %u\n", pSID->sampleRate);**/

	/** MODE */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &ui32);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" speex mode %u\n", ui32);**/
	switch (ui32) {
	case 0: pSID->mode = ST_CONTOGG_SPEEXMODE_NAR; break;
	case 1: pSID->mode = ST_CONTOGG_SPEEXMODE_WID; break;
	case 2: pSID->mode = ST_CONTOGG_SPEEXMODE_UW; break;
	default: pSID->mode = ST_CONTOGG_SPEEXMODE_NONE;
	}

	/** BITSTREAM_VERSION */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->bitstrVers);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if ((Tst_int32)pSID->bitstrVers > 4) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"can't handle Speex bitstream version 0x%08x",
					pSID->bitstrVers);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}
	/**st_contOgg_prf(" speex bsvers 0x%08x\n", pSID->bitstrVers);**/

	/** CHANNELS */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->channels);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (pSID->channels < 1 || pSID->channels > 2) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid channel count %u found in header");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** BITRATE */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->bitrateKBS);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" speex bitrate %u (0x%08x)\n",
			pSID->bitrateKBS, pSID->bitrateKBS);**/
	if ((Tst_int32)pSID->bitrateKBS < 0) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"correcting bitrate %d --> 0",
					(Tst_int32)pSID->bitrateKBS);
		pSID->bitrateKBS = 0;
	} else
		pSID->bitrateKBS /= 1000;

	/** FRAME_SIZE */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->frameSize);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" speex frSz %u\n", pSID->frameSize);**/

	/** VBR */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &ui32);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	pSID->isVBR = (ui32 == 1);
	/**st_contOgg_prf(" speex vbr=%c\n", (pSID->isVBR ? 'y' : 'n'));**/

	/** FRAMES_PER_PACKET */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->framesPerPacket);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" speex fPp %u\n", pSID->framesPerPacket);**/

	/** EXTRA_HEADERS */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSID->extraHeaders);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" speex exHd %u\n", pSID->extraHeaders);**/

	/** RESERVED */
	st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);

	/* */
	pBSI->bsType = ST_CONTOGG_BSTP_SPEE;
	pSID->isSet  = ST_B_TRUE;
	pSID->packNr = pBSI->packCur.nr;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Celt header packet
 */
Tst_err
st_contOgg_celtrd_parseHPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj,
		Tst_contOgg_cbStripStr cbStripStr)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 rdBytes = 0;
	Tst_contOgg_celtIdent *pCID = &pBSI->pCelt->ident;

	if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
				"Celt header found");

	if (st_streamrd_getAmountRemainingBitsInCurByte(pSObj) != 8)
		return ST_ERR_IDAT;

	/* */
	/** VERSION */
	ST_REALLOC(pCID->pVersStr, Tst_str*, 20 + 1, 1)
	if (pCID->pVersStr == NULL)
		return ST_ERR_OMEM;
	res = st_streamrd_rdBuffer_noCRC(pSObj, 20, pCID->pVersStr, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != 20) {
		if (res == ST_ERR_SUCC)
			res = ST_ERR_IDAT;
		return res;
	}
	pCID->pVersStr[20] = 0x00;
	cbStripStr(pCID->pVersStr, 20);

	/** BITSTREAM_VERSION */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->bitstrVers);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if ((Tst_int32)pCID->bitstrVers > 0) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"can't handle Celt version %u",
					pCID->bitstrVers);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** HEADER_SZ */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->headerSize);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" celt hdSz %u\n", pCID->headerSize);**/
	if (pCID->headerSize != 60) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"correcting header size %u --> %u",
					pCID->headerSize, 60);
		pCID->headerSize = 60;
	}

	/** SAMPLERATE */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->sampleRate);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;

	/** CHANNELS */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->channels);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (pCID->channels < 1 || pCID->channels > 2) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid channel count %u found in header",
					pCID->channels);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** FRAME_SIZE */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->frameSize);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" celt frSz %u\n", pCID->frameSize);**/

	/** OVERLAP */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->overlap);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" celt overl %u\n", pCID->overlap);**/

	/** BYTES_PER_PACKET */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->bytesPerPacket);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" celt bPp %u\n", pCID->bytesPerPacket);**/

	/** EXTRA_HEADERS */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pCID->extraHeaders);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf(" celt exHd %u\n", pCID->extraHeaders);**/

	/* bitrate for CBR-encoded files */
	pCID->bitrateKBS = (Tst_uint32)((((double)pCID->sampleRate /
					(double)pCID->frameSize) *
				8.0 * (double)pCID->bytesPerPacket) / 1000.0);
	/**st_contOgg_prf("br= ((%d/%.2f) * 8 * %d) / 1000.0 = %u\n",
			pCID->sampleRate, (float)pCID->frameSize, pCID->bytesPerPacket,
			pCID->bitrateKBS);**/

	/* */
	pBSI->bsType = ST_CONTOGG_BSTP_CELT;
	pCID->isSet  = ST_B_TRUE;
	pCID->packNr = pBSI->packCur.nr;
	return ST_ERR_SUCC;
}

/******************************************************************************/
