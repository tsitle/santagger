/******************************************************************************/
/* ac_vorb_zrd.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading Ogg-Vorbis                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/cont_ogg.h"
/** */
#define SRC_AC_VORB_ZRD_C
#include "ac_vorb_zrd-prot.h"
/*** */
#include "ac_vorb_stcs-prot.h"
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../cont_ogg_met_vorbcomm/mc_vorbcomm_zrd-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_AC_VORB_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memset(),... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_CONTOGG__vorbrd_parseIdent(const Tst_contOgg_opts *pOpts,
                              Tst_contOgg_substr_intn *pBSI,
                              Tst_streamrd *pSObj);
/** */
static Tst_err
ST_CONTOGG__vorbrd_setOutputFmt(const Tst_contOgg_opts *pOpts,
                                Tst_contOgg_substr_intn *pBSI);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Init Vorbis decoder
 */
Tst_err
st_contOgg_vorbrd_initDecoder(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
	const char *cFNCN = __func__;
	Tst_err res = ST_ERR_SUCC;

	if (pOpts->vorbOpts.decodeFull && pOpts->vorbOpts.isDecoderSet) {
		res = pOpts->vorbOpts.cbDecHndNew(&pBSI->pVorb->decObj.pDecHnd,
					pOpts->printDecUsedBPS);
		if (res != ST_ERR_SUCC)
			pBSI->pVorb->decObj.pDecHnd = NULL;
		else
			res = pOpts->vorbOpts.cbDecOpenFeed(pBSI->pVorb->decObj.pDecHnd);
		if (res != ST_ERR_SUCC) {
			if (pBSI->pVorb->decObj.pDecHnd != NULL)
				pOpts->vorbOpts.cbDecHndDel(pBSI->pVorb->decObj.pDecHnd);
			st_contOgg_d_err(pOpts, cFNCN, pBSI->pFilen,
					"initialization of decoder failed");
		} else
			pBSI->pVorb->decObj.decToPCM = ST_B_TRUE;
	}
	return res;
}

/*
 * Free Vorbis decoder
 */
void
st_contOgg_vorbrd_endDecoder(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
	if (! pBSI->pVorb->decObj.decToPCM)
		return;

	pOpts->vorbOpts.cbDecCloseFeed(pBSI->pVorb->decObj.pDecHnd);
	pOpts->vorbOpts.cbDecHndDel(pBSI->pVorb->decObj.pDecHnd);
	/* */
	pBSI->pVorb->decObj.decToPCM = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Feed decoder with header
 */
Tst_err
st_contOgg_vorbrd_feedDecoderHeader(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
	const char *cFNCN = __func__;
	Tst_err res = ST_ERR_SUCC;
	Tst_contOgg_vorbDecObj *pDecObj = &pBSI->pVorb->decObj;

	if (pDecObj->dbufUsd > 0) {
		/* feed header to decoder */
		/**st_contOgg_prf("%s(): feeding header, sz %u\n",
				cFNCN, pDecObj->dbufUsd);**/
		res = pOpts->vorbOpts.cbDecFeedHd(pDecObj->pDecHnd,
				pDecObj->pDBuf, pDecObj->dbufUsd,
				pBSI->pFHdBeg->hdTypeBOS, pBSI->pFHdBeg->granulePos);
		if (res != ST_ERR_SUCC)
			st_contOgg_d_err(pOpts, cFNCN, pBSI->pFilen,
					"feeding header to decoder failed");
	}

	return res;
}

/*
 * Feed decoder with audio and decode samples
 */
Tst_err
st_contOgg_vorbrd_feedDecoderAudio(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
#	define LOC_FNCN_  __func__
	Tst_err res = ST_ERR_SUCC;
	Tst_contOgg_vorbDecObj *pDecObj = &pBSI->pVorb->decObj;

	if (pDecObj->dbufUsd > 0) {
		/* feed packet to decoder, let him decode the samples
		 *   and let him send the samples to the PCM client  */
		/**st_contOgg_prf("%s(): feeding audio, sz %u\n",
				LOC_FNCN_, pDecObj->dbufUsd);**/
		res = pOpts->vorbOpts.cbDecFeedAudAndDec(pDecObj->pDecHnd,
				pDecObj->pDBuf, pDecObj->dbufUsd,
				pBSI->pFHdBeg->granulePos,
				(pOpts->isPCMclientSet ? pOpts->cbPCMsamplesClient : NULL),
				(pOpts->isPCMclientSet ? pDecObj->pPCMstrHnd : NULL));
		if (res != ST_ERR_SUCC)
			st_contOgg_d_err(pOpts, LOC_FNCN_, pBSI->pFilen,
					"feeding audio to decoder failed");
	}

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Vorbis header packet
 */
Tst_err
st_contOgg_vorbrd_parseHPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, const Tst_byte packTp,
		Tst_streamrd *pSObj, Tst_bool *pStopThen)
{
	const char *cFNCN = __func__;
	Tst_err res = ST_ERR_SUCC;

	/* */
	switch (packTp) {
	case 1:  /* vorbis identification info */
		/* ident info should have its own packet (pack #1),
		 *   but to be sure we don't stop after it */
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"Vorbis ident header found");
		/* */
		res = ST_CONTOGG__vorbrd_parseIdent(pOpts, pBSI, pSObj);
		/* */
		if (res == ST_ERR_SUCC && pBSI->pVorb->decObj.decToPCM)
			res = ST_CONTOGG__vorbrd_setOutputFmt(pOpts, pBSI);
		break;
	case 3:  /* vorbis comment */
		/* comment+setup should have their own packet (pack #2) */
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"Vorbis comment header found");
		res = st_contOgg_vorbComm_parse(pBSI, pSObj,
				&pBSI->pVorb->comm,
				/*hasFramingBit:*/ST_B_TRUE, /*hasMaxSz:*/ST_B_FALSE,
				/*maxSz:*/0,
				/*srcCmtBegPos:*/(Tst_int32)st_streamrd_getCurPos(pSObj) - 7);
		break;
	case 5:  /* vorbis setup info */
		/* setup info comes at the end of the packet (pack #2),
		 *   therefore we stop parsing the packet after it */
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"Vorbis setup header found");
		pBSI->bsType              = ST_CONTOGG_BSTP_VORB;
		pBSI->pVorb->setup.isSet  = ST_B_TRUE;
		pBSI->pVorb->setup.packNr = pBSI->packCur.nr;
		/* */
		res        = ST_ERR_SUCC;
		*pStopThen = ST_B_TRUE;
		break;
	default:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid vorbis packType 0x%02x found, "
					"skipping packet", packTp);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}
	/* */
	if (res == ST_ERR_IDAT) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid data found in vorbis packet, "
					"skipping packet");
		++pBSI->errorCnt;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Vorbis Identification header
 */
static Tst_err
ST_CONTOGG__vorbrd_parseIdent(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj)
{
	const char *cFNCN = __func__;
	Tst_err  res;
	Tst_byte frBit = 0;
	Tst_contOgg_vorbIdent *pVID = &pBSI->pVorb->ident;

	if (pVID->isSet) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"already read vorbIdent header, skipping packet");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/* parse ident data */
	/** VERSION */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pVID->vers);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (pVID->vers != 0) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"unknown version %u found, skipping packet", pVID->vers);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** CHANNELS */
	res = st_streamrd_rdByte_noCRC(pSObj, 8, &pVID->channels);
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("ST_CONTOGG__vorbrd_parseIdent(): chann %u\n", pVID->chann);**/

	/** AUDIO SAMPLE RATE */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pVID->sampleRate);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("ST_CONTOGG__vorbrd_parseIdent(): "
			"sampleR %u\n", pVID->sampleRate);**/
	if (pVID->channels < 1 || pVID->sampleRate < 1) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"channels or samplerate == 0, skipping packet");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** BITRATE MAX */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pVID->bitrateMaxKBS);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/** BITRATE NOMINAL */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pVID->bitrateNomKBS);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/** BITRATE MIN */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pVID->bitrateMinKBS);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("ST_CONTOGG__vorbrd_parseIdent(): "
			"bitrates %u (%d)/ %u / %u (%d)\n",
			pVID->bitrateMinKBS, (int)pVID->bitrateMinKBS, pVID->bitrateNomKBS,
			pVID->bitrateMaxKBS, (int)pVID->bitrateMaxKBS);**/
	if ((int)pVID->bitrateMinKBS == -1)  /* == 0xffffffff */
		pVID->bitrateMinKBS = 0;
	else
		pVID->bitrateMinKBS /= 1000;
	if ((int)pVID->bitrateNomKBS == -1)
		pVID->bitrateNomKBS = 0;
	else
		pVID->bitrateNomKBS /= 1000;
	if ((int)pVID->bitrateMaxKBS == -1)
		pVID->bitrateMaxKBS = 0;
	else
		pVID->bitrateMaxKBS /= 1000;

	/** BLOCKSIZES */
	res = st_streamrd_rdByte_noCRC(pSObj, 8, &pVID->blockSz0);
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("ST_CONTOGG__vorbrd_parseIdent(): blockSz 0x%02x\n",
			pVID->blockSz0);**/
	pVID->blockSz1 = (pVID->blockSz0 & 0xf0) >> 4;
	pVID->blockSz0 = pVID->blockSz0 & 0x0f;
	/**st_contOgg_prf("ST_CONTOGG__vorbrd_parseIdent(): blockSz %u / %u\n",
			pVID->blockSz0, pVID->blockSz1);**/
	if (pVID->blockSz0 > pVID->blockSz1 || pVID->blockSz0 < 6 ||
			pVID->blockSz1 < 6 ||
			pVID->blockSz0 > 13 || pVID->blockSz1 > 13) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid blocksize(s) found: "
					"blocksize0=%u, blocksize1=%u, skipping packet",
					pVID->blockSz0, pVID->blockSz1);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** FRAMING BIT */
	res = st_streamrd_rdByte_noCRC(pSObj, 8, &frBit);
	if (res != ST_ERR_SUCC)
		return res;
	if ((frBit & 0x01) == 0) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"framing bit invalid, skipping packet");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/* */
	pBSI->bsType = ST_CONTOGG_BSTP_VORB;
	pVID->isSet  = ST_B_TRUE;
	pVID->packNr = pBSI->packCur.nr;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Set output format for decoder and PCM client
 */
static Tst_err
ST_CONTOGG__vorbrd_setOutputFmt(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
	const char *cFNCN = __func__;
	Tst_err res;
	Tst_contOgg_vorbDecObj *pDecObj = &pBSI->pVorb->decObj;

	pDecObj->channels = pBSI->pVorb->ident.channels;
	pDecObj->sampleR  = pBSI->pVorb->ident.sampleRate;

	/* set output format for decoder */
	res = pOpts->vorbOpts.cbDecSetOutpFmt(pDecObj->pDecHnd,
			pDecObj->sampleR, pDecObj->channels,
			pOpts->decodeBPS, &pDecObj->bps);
	if (res != ST_ERR_SUCC) {
		st_contOgg_d_err(pOpts, cFNCN, pBSI->pFilen,
				"setting decoder output format failed");
		return res;
	}
	/* set output format for PCM client */
	if (pOpts->isPCMclientSet && ! pDecObj->isOutpFmtSet) {
		res = pOpts->cbPCMsetOutputFmt(pDecObj->pPCMstrHnd,
				pDecObj->sampleR, pDecObj->bps,
				0, pDecObj->channels);
		if (res != ST_ERR_SUCC) {
			st_contOgg_d_err(pOpts, cFNCN, pBSI->pFilen,
					"setting PCM client output format failed");
			return res;
		}
		pDecObj->isOutpFmtSet = ST_B_TRUE;
	}
	return ST_ERR_SUCC;
}

/******************************************************************************/
