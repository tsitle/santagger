/******************************************************************************/
/* ac_flac_zrd.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading (Ogg-)Flac                                           */
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
#include "src/includes/common/sys_math.h"
#include "src/includes/common/streamrd.h"
/** */
#define SRC_AC_FLAC_ZRD_C
#include "ac_flac_zrd-prot.h"
/*** */
#include "ac_flac_zrdh-pp.h"
#include "ac_flac_zrda-pp.h"
#include "ac_flac_stcs-prot.h"
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_AC_FLAC_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Flac header packet (which may contain >=1 flac headers)
 */
Tst_err
st_contOgg_flacrd_parseHPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, const Tst_byte packTp,
		Tst_streamrd *pSObj)
{
	const char *cFNCN = __func__;
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint16 shVal = 0;
	Tst_contOgg_flacIdent        *pFID;
	Tst_contOgg_codFlac_defParsH defPars;

	defPars.pBSI  = pBSI;
	defPars.pSObj = pSObj;
	pFID          = &pBSI->pFlac->ident;

	/* */
	if (pBSI->packCur.nr == 1) {
		if (pBSI->isSrcOggOrFlac) {
			/* beginning of frame already read: 0x7f FLAC (5 bytes) */

			if (packTp != 0x7f) {
				if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
							"invalid flac header packet found "
							"(packTp 0x%02x) [1], skipping packet", packTp);
				++pBSI->errorCnt;
				res = ST_ERR_IDAT;
			}
			/* */
			/** VERSION MAJOR, MINOR */
			if (res == ST_ERR_SUCC)
				res = st_streamrd_rdByte(pSObj, 8, &pFID->verMaj);
			if (res == ST_ERR_SUCC)
				res = st_streamrd_rdByte(pSObj, 8, &pFID->verMin);
			if (res == ST_ERR_SUCC && pFID->verMaj != 1) {
				if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
							"can't handle flac major version %u",
							pFID->verMaj);
				++pBSI->errorCnt;
				res = ST_ERR_IDAT;
			}
			/** HEADER PACKET CNT */
			if (res == ST_ERR_SUCC) {
				res = st_streamrd_rdUInt16(pSObj,
						ST_STREAMRD_IEND_BE, 16, &shVal);  /* ENDIAN: BE-->HOST */
			}
			if (res == ST_ERR_SUCC)
				pFID->hdPcks_fromHd = shVal;
		}
		/** Flac ID */
		if (res == ST_ERR_SUCC) {
			if (! st_streamrd_rdMemcmpBytes(pSObj, 4, "fLaC")) {
				if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
							"invalid flac header found [2], %s",
							(pBSI->isSrcOggOrFlac ?
								"skipping packet" : "aborting"));
				++pBSI->errorCnt;
				res = ST_ERR_IDAT;
			} else
				st_streamrd_rdSkipBytes(pSObj, 4, ST_B_TRUE);
		}
		/* */
		if (res == ST_ERR_SUCC) {
			/**st_contOgg_prf("st_contOgg_flacrd_parseHPack(): "
					"flac %u.%u hdPcks %u\n",
					pFID->verMaj, pFID->verMin, pFID->hdPckCnt);**/
			/* */
			pFID->isSet  = ST_B_TRUE;
			pFID->packNr = pBSI->packCur.nr;
		}
	} else {
		/* beginning of frame already read (1 byte) but rewinded to beginning
		 *   first byte is now PACKTYPE again  */

		if (packTp == 0x00 || packTp == 0xff ||
				packTp == 0x7f || packTp == 0x80) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
						"invalid flac header packet found "
						"(packTp 0x%02x) [3], %s", packTp,
						(pBSI->isSrcOggOrFlac ? "skipping packet" : "aborting"));
			++pBSI->errorCnt;
			return ST_ERR_IDAT;
		}
	}
	if (res != ST_ERR_SUCC)
		return res;

	/* parse all headers in this packet */
	do {
		res = st_contOgg_flacrdh_parseH_MD(pOpts, &defPars);
		if (res == ST_ERR_SUCC) {
			pBSI->bsType = ST_CONTOGG_BSTP_FLAC;
			++pFID->hdPcks_read;
		} else if (res == ST_ERR_IDAT && ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid data found in flac header [4], %s",
					(pBSI->isSrcOggOrFlac ? "skipping packet" : "aborting"));
	} while (res == ST_ERR_SUCC &&
			pBSI->isSrcOggOrFlac && ! st_streamrd_isEOF(pSObj));
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac audio packet (which may contain >=1 flac frames)
 */
Tst_err
st_contOgg_flacrd_parseAPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, const Tst_byte packTp,
		Tst_streamrd *pSObj)
{
#	define LOC_FNCN_  __func__
	Tst_err    res        = ST_ERR_SUCC;
	Tst_uint32 fraCntLast = pBSI->pFlac->aFramesCnt;
	Tst_byte   cha,
	           tmpB;
	char const *pChaAss;
	Tst_contOgg_flacAudFrame     *pCFra = &pBSI->pFlac->curAFra;
	Tst_contOgg_codFlac_defParsA defPars;

	/*
	 * FRAME_HEADER
	 * SUBFRAME+     One SUBFRAME per channel
	 * <?>           Zero-padding to byte alignment
	 * FRAME_FOOTER
	 */

	if (packTp != 0xff) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
					"invalid flac audio packet found (packTp 0x%02x) [1], %s",
					packTp,
					(pBSI->isSrcOggOrFlac ? "skipping packet" : "aborting"));
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	if (! pOpts->flacOpts.readAll &&
			pBSI->pFlac->aFramesCnt > 0) {
		/* we stop here since we don't need the remaining frames */
		++pBSI->pFlac->aFramesCnt;
		return ST_ERR_SUCC;
	}

	/* save offset of 1st frame */
	/**if (pBSI->pFlac->aFramesCnt == 0 && ! pBSI->isSrcOggOrFlac) {
		pBSI->pFlac->ident.offsAbs1st = st_streamrd_getCurPos(pSObj);
		st_contOgg_prf("\n1st aframe offset: 0x%08x\n",
				pBSI->pFlac->ident.offsAbs1st);
	}**/

	/* init some stuff */
	defPars.pBSI  = pBSI;
	defPars.pSObj = pSObj;
	defPars.pFra  = pCFra;

	/* set PCM output format */
	if (pOpts->isPCMclientSet && ! pBSI->pFlac->decObj.isOutpFmtSet) {
		res = pOpts->cbPCMsetOutputFmt(pBSI->pFlac->decObj.pPCMstrHnd,
				pBSI->pFlac->ident.sampleR, pBSI->pFlac->ident.bitsPerS,
				0, pBSI->pFlac->ident.chann);
		if (res != ST_ERR_SUCC) {
			st_contOgg_d_err(pOpts, LOC_FNCN_, pBSI->pFilen,
					"setting PCM output format failed");
			return res;
		}
		pBSI->pFlac->decObj.isOutpFmtSet = ST_B_TRUE;
	}

	do {
		/* audio frame begins at fresh byte */

		/**if (pBSI->pFlac->ident.offsAbs1st >= 0 && ! pBSI->isSrcOggOrFlac) {
			st_contOgg_prf("\naframe rel offset: 0x%08x\n",
					st_streamrd_getCurPos(pSObj) -
						pBSI->pFlac->ident.offsAbs1st);
			st_contOgg_prf64("sampleC", &pBSI->pFlac->ident.sampleCntReal);
		}**/

		st_contOgg_stc_rsetFlacAFra(pCFra, ST_B_FALSE);

		/* parse frameheader */
		res = st_contOgg_flacrda_parseFH(pOpts, &defPars);
		if (res == ST_ERR_IDAT && ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
					"invalid flac audio frameHd found [2], %s",
					(pBSI->isSrcOggOrFlac ? "skipping packet" : "aborting"));
		if (res != ST_ERR_SUCC)
			break;

		/* reserve mem for sample-output */
		if (pOpts->flacOpts.decodeFull) {
			res = st_contOgg_stc_reszFlacRSamps(&pCFra->rawSamp,
					pCFra->fh.channels, pCFra->fh.blkSz);
			if (res != ST_ERR_SUCC)
				break;
		}

		/* parse subframes */
		/** */
		if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
			const char *cCHANNASS[] = {"LEFT/SIDE", "SIDE/RIGHT",
			                           "MID/SIDE", "INDEP"};

			if (pCFra->fh.channAss == ST_CONTOGG_FLAC_CA_LS)
				pChaAss = cCHANNASS[0];
			else if (pCFra->fh.channAss == ST_CONTOGG_FLAC_CA_RS)
				pChaAss = cCHANNASS[1];
			else if (pCFra->fh.channAss == ST_CONTOGG_FLAC_CA_MS)
				pChaAss = cCHANNASS[2];
			else
				pChaAss = cCHANNASS[3];
			st_contOgg_d_debBS3d(pOpts, pBSI, LOC_FNCN_,
					"%s blkSz %u, sampleR %u, "
					/*crc 0x%02x, */
					"channAss %s",
					(pCFra->fh.isBlockStratVar ? "var" : "fix"),
					pCFra->fh.blkSz,
					pCFra->fh.sampleR, /*pCFra->fh.crc8_a,*/ pChaAss);
		}
		/** */
		for (cha = 0; cha < pCFra->fh.channels; cha++) {
			/* init subframe data */
			/** */
			pCFra->pCSFr          = &pCFra->sfr[cha];
			pCFra->pCSFr->wasUsed = ST_B_TRUE;
			pCFra->pCSFr->frameNr = pBSI->pFlac->aFramesCnt;
			pCFra->pCSFr->channel = cha;
			/** reserve mem for residual samples */
			if (pOpts->flacOpts.decodeFull) {
				if (pCFra->fh.blkSz > pCFra->pCSFr->rawResid.maxBlkSz) {
					res = st_contOgg_stc_reszFlacSubFrResid(pCFra->pCSFr,
							pCFra->fh.blkSz);
					if (res != ST_ERR_SUCC)
						break;
				}
				pCFra->pCSFr->rawResid.cnt = pCFra->fh.blkSz;
			}

			pCFra->pCSFr->sampleSz = pCFra->fh.sampleSz;
			if (pCFra->fh.channAss != ST_CONTOGG_FLAC_CA_INDEP) {
				if (cha == 0 &&
						pCFra->fh.channAss == ST_CONTOGG_FLAC_CA_RS)
					++pCFra->pCSFr->sampleSz;
				else if (cha == 1 &&
						(pCFra->fh.channAss == ST_CONTOGG_FLAC_CA_LS ||
							pCFra->fh.channAss == ST_CONTOGG_FLAC_CA_MS))
					++pCFra->pCSFr->sampleSz;
			}

			res = st_contOgg_flacrda_parseSF(pOpts, &defPars);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_IDAT && ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
							"invalid flac audio subframe (chan #%u) found [3], %s",
							pCFra->pCSFr->channel,
							(pBSI->isSrcOggOrFlac ? "skipping packet" : "aborting"));
				break;
			}
		}
		if (res != ST_ERR_SUCC)
			break;

		/** ZERO PADDING to byte-alignment, x bits */
		tmpB = st_streamrd_getAmountRemainingBitsInCurByte(pSObj);
		if (tmpB > 0 && tmpB < 8) {
			/**st_contOgg_prf("zero padd %u bits\n", tmpB);**/
			if (pOpts->flacOpts.checkCRCfr)
				res = st_streamrd_rdByte(pSObj, tmpB, &tmpB);
			else
				res = st_streamrd_rdByte_noCRC(pSObj, tmpB, &tmpB);
			if (res != ST_ERR_SUCC)
				break;
		}

		/* parse frame footer */
		/** CRC-16, 16 bits */
		if (pOpts->flacOpts.checkCRCfr) {
			pCFra->fh.crc16_b = st_streamrd_getCRC16(pSObj);
			st_streamrd_stopCRC16(pSObj);
		}
		res = st_streamrd_rdUInt16(pSObj,
				ST_STREAMRD_IEND_BE, 16, &pCFra->fh.crc16_a);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"invalid flac audio packet found [4], %s",
						(pBSI->isSrcOggOrFlac ? "skipping packet" : "aborting"));
			++pBSI->errorCnt;
			break;
		}
		if (pOpts->flacOpts.checkCRCfr &&
				pCFra->fh.crc16_a != pCFra->fh.crc16_b) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts))
				st_contOgg_d_debOrAnaBS3warn(pOpts, pBSI, LOC_FNCN_,
						"CRC16 of frame DOESN'T MATCH "
						"(0x%04x != 0x%04x)",
						pCFra->fh.crc16_a, pCFra->fh.crc16_b);
			/*res = ST_ERR_IDAT;
			break;*/
		}

		/* Undo any special channel coding */
		if (pOpts->flacOpts.decodeFull &&
				pCFra->fh.channAss != ST_CONTOGG_FLAC_CA_INDEP) {
			st_contOgg_flacrda_undoSpecChannEnc(pOpts, &defPars);
		}

		if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts))
			st_contOgg_d_debBS3d(pOpts, pBSI, LOC_FNCN_,
					"--end frame----------------\n\n\n");
		++pBSI->pFlac->aFramesCnt;

		if (! pOpts->flacOpts.readAll) {
			/* we stop here since we don't need the remaining frames */
			break;
		}

		/* update MD5 */
		if (pOpts->flacOpts.decodeFull && pOpts->flacOpts.checkMD5da &&
				pBSI->pFlac->ident.isMD5set) {
			res = st_md5_updateFlac(&pBSI->pFlac->ident.md5_bCtx,
					(const Tst_int32**)pCFra->rawSamp.ppSamples,
					pCFra->fh.channels, pCFra->fh.blkSz,
					(pCFra->fh.sampleSz + 7) / 8);
			if (res != ST_ERR_SUCC) {
				res = ST_ERR_IDAT;
				break;
			}
		}

		/* send decoded samples to someone cares about them */
		if (pOpts->isPCMclientSet && pBSI->pFlac->decObj.isOutpFmtSet) {
			res = pOpts->cbPCMsamplesClient(pBSI->pFlac->decObj.pPCMstrHnd,
					pCFra->fh.blkSz, (const Tst_int32**)pCFra->rawSamp.ppSamples);
			if (res != ST_ERR_SUCC) {
				st_contOgg_d_err(pOpts, LOC_FNCN_, pBSI->pFilen,
						"PCM samples client failed somehow");
				break;
			}
		}

		/* find next frameheader */
		if ((pBSI->isSrcOggOrFlac &&
					st_streamrd_getAmountRemainingBytes(pSObj) < 2) ||
				! st_contOgg_flacrda_searchNextFH(pOpts, &defPars))
			break;
		if (! pBSI->isSrcOggOrFlac)
			break;
	} while (ST_B_TRUE);

	if (pBSI->isSrcOggOrFlac && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
				"aframes in packet: %u\n\n",
				pBSI->pFlac->aFramesCnt - fraCntLast);

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Finish Flac stream reading
 */
void
st_contOgg_flacrd_finish(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
	const char *cFNCN = __func__;
	Tst_bool comp = ST_B_FALSE;
	Tst_str  numStr1[50],
	         numStr2[50];

	if (pOpts->quickScan)
		return;

	if (pOpts->flacOpts.readAll) {
		/* total samples */
		if (st_sysUInt64_cmpLoHi(&pBSI->pFlac->ident.samplesTot_fromHd,
					(Tst_uint32)-1, (Tst_uint32)-1) == 0 ||
				st_sysUInt64_cmpLoHi(&pBSI->pFlac->ident.samplesTot_fromHd,
							0, 0) == 0) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts)) {
				st_contOgg_d_debOrAnaBS1(pOpts, pBSI, cFNCN,
						"amount of samples not set in header");
			}
		} else
			if (st_sysUInt64_cmpUI64(&pBSI->pFlac->ident.samplesTot_fromHd,
						&pBSI->pFlac->ident.samplesTot_read) != 0) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts)) {
					st_contOgg_d_debOrAnaBS1(pOpts, pBSI, cFNCN,
							"amount of samples read from file "
							"doesn't match header info");
					st_sysUInt64_toDecStr(&pBSI->pFlac->ident.samplesTot_fromHd,
							numStr1, sizeof(numStr1));
					st_sysUInt64_toDecStr(&pBSI->pFlac->ident.samplesTot_read,
							numStr2, sizeof(numStr2));
					st_contOgg_d_debOrAnaBS1(pOpts, pBSI, cFNCN,
							"  %s != %s", numStr1, numStr2);
				}
				++pBSI->errorCnt;
			}
	}

	if (! (pOpts->flacOpts.decodeFull && pOpts->flacOpts.checkMD5da))
		return;

	if (pBSI->pFlac->ident.isMD5set) {
		st_md5_final(pBSI->pFlac->ident.md5_b, &pBSI->pFlac->ident.md5_bCtx);
		/**st_contOgg_prf("st_contOgg_flacrd_finish():            MD5 0x");
		st_contOgg_prBuf((Tst_buf*)pBSI->pFlac->ident.md5_a,
				sizeof(pBSI->pFlac->ident.md5_a));
		st_contOgg_prf("\n");**/
	} else {
		if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts))
			st_contOgg_d_debOrAnaBS1(pOpts, pBSI, cFNCN,
					"original MD5 checksum not set in header");
		return;
	}

	pBSI->pFlac->ident.md5OfDecAudioChkd = ST_B_TRUE;

	comp = (memcmp(&pBSI->pFlac->ident.md5_a, &pBSI->pFlac->ident.md5_b,
			sizeof(pBSI->pFlac->ident.md5_b)) == 0);
	if (! comp)
		++pBSI->errorCnt;
	pBSI->pFlac->ident.md5OfDecAudioOK = comp;
	if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts)) {
		if (comp)
			st_contOgg_d_debOrAnaBS1(pOpts, pBSI, cFNCN,
					"MD5 checksum of decoded audio data does match");
		else
			st_contOgg_d_debOrAnaBS1warn(pOpts, pBSI, cFNCN,
					"MD5 checksum of decoded audio data DOESN'T MATCH");
	}
}

/******************************************************************************/
