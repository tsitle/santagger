/******************************************************************************/
/* ac_flac_zrda.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading (Ogg-)Flac audio packets                             */
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
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"
/** */
#define SRC_AC_FLAC_ZRDA_C
#include "ac_flac_zrda-pp.h"
/*** */
#include "ac_flac_stcs-prot.h"
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../../common/stream/streamrd-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_AC_FLAC_ZRDA_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Search for next frame header by checking for the synccode
 */
Tst_bool
st_contOgg_flacrda_searchNextFH(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_bool   res  = ST_B_FALSE;
	Tst_uint16 sh   = 0;
	Tst_uint32 skpd = 0;

	while (ST_B_TRUE) {
		/* synccode: 11111111 111110 */
		if (st_streamrd_rdAheadUInt16(pDPa->pSObj,
				ST_STREAMRD_IEND_BE, 14, &sh) != ST_ERR_SUCC) {  /* ENDIAN: BE-->HOST */
			res = ST_B_FALSE;
			break;
		}
		if (sh == 0x3ffe) { /* 111111 11111110 */
			res = ST_B_TRUE;
			break;
		}
		++skpd;
		st_streamrd_rdSkipBytes(pDPa->pSObj, 1, ST_B_TRUE);
	}
	if (res && skpd > 0) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPa->pBSI, LOC_FNCN_,
					"skipped %u bytes to find next synccode", skpd);
		++pDPa->pBSI->errorCnt;
	}
	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac audio packet - Frameheader
 */
Tst_err
st_contOgg_flacrda_parseFH(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err    res  = ST_ERR_SUCC;
	Tst_byte   byt  = 0,
	           bs1,
	           sr1,
	           ca,
	           ssz;
	Tst_uint32 ui32 = 0;
	Tst_uint16 ui16 = 0;
	Tst_uint64 ui64;

	st_contOgg_stc_rsetFlacFH(&pDPa->pFra->fh);

	if (pOpts->flacOpts.checkCRCfh)
		st_streamrd_startCRC8(pDPa->pSObj);
	if (pOpts->flacOpts.checkCRCfr)
		st_streamrd_startCRC16(pDPa->pSObj);

	/* read SYNC CODE, RESERVED BIT, BLOCK STRAT */
	res = st_streamrd_rdUInt16(pDPa->pSObj,
			ST_STREAMRD_IEND_BE, 16, &ui16);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;

	/** SYNC CODE 14 bits
	 **   11111111 111110  */
	if ((ui16 & 0xff00) != 0xff00)
		return ST_ERR_IDAT;
	if ((ui16 & 0x00fc) != 0x00f8)
		return ST_ERR_IDAT;
	/** RESERVED, 1 bit */
	if ((ui16 & 0x0002) != 0x0000)
		return ST_ERR_IDAT;
	/** BLOCKING STRATEGY, 1 bit */
	pDPa->pFra->fh.isBlockStratVar = ((ui16 & 0x0001) == 0x0001);

	/** BLOCK SIZE IN INTER-CHANNEL SAMPLES, 4 bits */
	if (pOpts->flacOpts.checkCRCfh || pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 4, &bs1);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 4, &bs1);
	if (res != ST_ERR_SUCC)
		return res;
	if (bs1 == 0)
		return ST_ERR_IDAT;
	if (bs1 == 1)
		pDPa->pFra->fh.blkSz = 192;
	else if (bs1 >= 2 && bs1 <= 5)
		pDPa->pFra->fh.blkSz = 576 * (1 << (bs1 - 2));  /* 576 * (2^(n-2)) */
	else if (bs1 >= 8 && bs1 <= 15)
		pDPa->pFra->fh.blkSz = 256 * (1 << (bs1 - 8));  /* 256 * (2^(n-8)) */

	/** SAMPLE RATE, 4 bits */
	if (pOpts->flacOpts.checkCRCfh || pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 4, &sr1);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 4, &sr1);
	if (res != ST_ERR_SUCC)
		return res;
	if (sr1 == 0x0f)
		return ST_ERR_IDAT;
	switch (sr1) {
	case  0: pDPa->pFra->fh.sampleR = pDPa->pBSI->pFlac->ident.sampleR; break;
	case  1: pDPa->pFra->fh.sampleR =  88200; break;  /* 88.2kHz */
	case  2: pDPa->pFra->fh.sampleR = 176400; break;  /* 176.4kHz */
	case  3: pDPa->pFra->fh.sampleR = 192000; break;  /* 192kHz */
	case  4: pDPa->pFra->fh.sampleR =   8000; break;  /* 8kHz */
	case  5: pDPa->pFra->fh.sampleR =  16000; break;  /* 16kHz */
	case  6: pDPa->pFra->fh.sampleR =  22050; break;  /* 22.05kHz */
	case  7: pDPa->pFra->fh.sampleR =  24000; break;  /* 24kHz */
	case  8: pDPa->pFra->fh.sampleR =  32000; break;  /* 32kHz */
	case  9: pDPa->pFra->fh.sampleR =  44100; break;  /* 44.1kHz */
	case 10: pDPa->pFra->fh.sampleR =  48000; break;  /* 48kHz */
	case 11: pDPa->pFra->fh.sampleR =  96000; break;  /* 96kHz */
	}

	/** CHANNEL ASSIGNMENT, 4 bits */
	if (pOpts->flacOpts.checkCRCfh || pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 4, &ca);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 4, &ca);
	if (res != ST_ERR_SUCC)
		return res;
	if (ca >= 0x0b)
		return ST_ERR_IDAT;
	if (ca <= 7) {
		pDPa->pFra->fh.channAss = ST_CONTOGG_FLAC_CA_INDEP;
		pDPa->pFra->fh.channels = ca + 1;
	} else {
		if (ca == 8)
			pDPa->pFra->fh.channAss = ST_CONTOGG_FLAC_CA_LS;
		else if (ca == 9)
			pDPa->pFra->fh.channAss = ST_CONTOGG_FLAC_CA_RS;
		else if (ca == 10)
			pDPa->pFra->fh.channAss = ST_CONTOGG_FLAC_CA_MS;
		pDPa->pFra->fh.channels = 2;
	}

	/** SAMPLE SIZE IN BITS, 3 bits */
	if (pOpts->flacOpts.checkCRCfh || pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 3, &ssz);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 3, &ssz);
	if (res != ST_ERR_SUCC)
		return res;
	if (ssz == 0x03 || ssz == 0x07)
		return ST_ERR_IDAT;
	switch (ssz) {
	case 0: pDPa->pFra->fh.sampleSz = pDPa->pBSI->pFlac->ident.bitsPerS; break;
	case 1: pDPa->pFra->fh.sampleSz =  8; break;  /* 8 bits per sample */
	case 2: pDPa->pFra->fh.sampleSz = 12; break;  /* 12 bits per sample */
	case 4: pDPa->pFra->fh.sampleSz = 16; break;  /* 16 bits per sample */
	case 5: pDPa->pFra->fh.sampleSz = 20; break;  /* 20 bits per sample */
	case 6: pDPa->pFra->fh.sampleSz = 24; break;  /* 24 bits per sample */
	}
	/**st_contOgg_prf("st_contOgg_flacrda_parseFH(): apc #%u: ssz %u\n",
			pDPa->pBSI->pFlac->aPackCnt, pDPa->pFra->fh.sampleSz);**/

	/** RESERVED, 1 bit */
	if (pOpts->flacOpts.checkCRCfh || pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 1, &byt);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 1, &byt);
	if (res != ST_ERR_SUCC)
		return res;
	if (byt != 0x00)
		return ST_ERR_IDAT;

	/** CODED SAMPLE/FRAME NUMBER, 8-56/8-48 bits */
	if (pDPa->pFra->fh.isBlockStratVar) {
		/* sample nr., 1-7 bytes */
		ST_SYSMATH_STC_RSETUI64(ui64)
		res = st_streamrd_rdFlacUtf8Encoded_uint64(pDPa->pSObj, &ui64);
		if (res != ST_ERR_SUCC)
			return res;
		/**st_contOgg_prf("st_contOgg_flacrda_parseFH(): apc #%u: snr __X__\n",
				pDPa->pBSI->pFlac->aPackCnt);**/
	} else {
		/* frame nr., 1-6 bytes */
		res = st_streamrd_rdFlacUtf8Encoded_uint32(pDPa->pSObj, &ui32);
		if (res != ST_ERR_SUCC)
			return res;
		/**st_contOgg_prf("st_contOgg_flacrda_parseFH(): apc #%u: fnr %u\n",
				pDPa->pBSI->pFlac->aPackCnt, ui32);**/
	}

	/** BLOCK SIZE part 2, 0/8/16 bits */
	if (bs1 == 6) {
		if (pOpts->flacOpts.checkCRCfh || pOpts->flacOpts.checkCRCfr)
			res = st_streamrd_rdByte(pDPa->pSObj, 8, &byt);
		else
			res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 8, &byt);
		if (res != ST_ERR_SUCC)
			return res;
		pDPa->pFra->fh.blkSz = byt + 1;
	} else if (bs1 == 7) {
		res = st_streamrd_rdUInt32(pDPa->pSObj,
				ST_STREAMRD_IEND_BE, 16, &ui32); /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			return res;
		pDPa->pFra->fh.blkSz = ui32 + 1;
	}
	/**st_contOgg_prf("st_contOgg_flacrda_parseFH(): apc #%u: bSz %u\n",
			pDPa->pBSI->pFlac->aPackCnt, pDPa->pFra->fh.blkSz);**/

	/** SAMPLE RATE part 2, 0/8/16 bits */
	if ((sr1 & 0x0e) >= 0x0c) {
		if (sr1 == 0x0c) {
			/* kHz */
			if (pOpts->flacOpts.checkCRCfh || pOpts->flacOpts.checkCRCfr)
				res = st_streamrd_rdByte(pDPa->pSObj, 8, &byt);
			else
				res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 8, &byt);
			if (res != ST_ERR_SUCC)
				return res;
			pDPa->pFra->fh.sampleR = byt * 1000;
		} else {
			res = st_streamrd_rdUInt32(pDPa->pSObj,
					ST_STREAMRD_IEND_BE, 16, &ui32);  /* ENDIAN: BE-->HOST */
			if (res != ST_ERR_SUCC)
				return res;
			pDPa->pFra->fh.sampleR = ui32;
			if (sr1 == 0x0e)  /* tens of Hz */
				pDPa->pFra->fh.sampleR = pDPa->pFra->fh.sampleR * 10;
		}
	}
	/**st_contOgg_prf("st_contOgg_flacrda_parseFH(): apc #%u: bSz %u, sR %u\n",
			pDPa->pBSI->pFlac->aPackCnt, pDPa->pFra->fh.blkSz,
			pDPa->pFra->fh.sampleR);**/

	/** CRC-8, 8 bits */
	if (pOpts->flacOpts.checkCRCfh) {
		pDPa->pFra->fh.crc8_b = st_streamrd_getCRC8(pDPa->pSObj);
		st_streamrd_stopCRC8(pDPa->pSObj);
	}
	if (pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 8, &pDPa->pFra->fh.crc8_a);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 8, &pDPa->pFra->fh.crc8_a);
	if (res != ST_ERR_SUCC)
		return res;
	if (pOpts->flacOpts.checkCRCfh &&
			pDPa->pFra->fh.crc8_a != pDPa->pFra->fh.crc8_b) {
		if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts))
			st_contOgg_d_debOrAnaBS3warn(pOpts, pDPa->pBSI, LOC_FNCN_,
					"CRC8 of frame header DOESN'T MATCH "
					"(0x%02x != 0x%02x)",
					pDPa->pFra->fh.crc8_a, pDPa->pFra->fh.crc8_b);
		return ST_ERR_IDAT;
	}

	/**st_contOgg_prf("st_contOgg_flacrda_parseFH(): apc #%u: ", pDPa->pBSI->pFlac->aPackCnt);
	st_contOgg_prf("channAss %d, ", pDPa->pFra->fh.channAss);
	st_contOgg_prf("sampleSz %u, ", pDPa->pFra->fh.sampleSz);
	st_contOgg_prf("%s blkSz %u, ",
			(pDPa->pFra->fh.isBlockStratVar ? "var" : "fix"),
			pDPa->pFra->fh.blkSz);
	st_contOgg_prf("sampleR %u, ", pDPa->pFra->fh.sampleR);
	st_contOgg_prf("crc 0x%02x\n", pDPa->pFra->fh.crc8_a);**/

	/* count samples */
	if (pOpts->flacOpts.readAll) {
		st_sysUInt64_addUI32(&pDPa->pBSI->pFlac->ident.samplesTot_read,
				pDPa->pFra->fh.blkSz, NULL);
		/**st_contOgg_prf("samples: %u  (tot %u)\n",
				pDPa->pBSI->pFlac->ident.samplesTot_read.loU,
				pDPa->pBSI->pFlac->ident.samplesTot_fromHd.loU);
		st_contOgg_prf("         0x%08x\n",
				pDPa->pBSI->pFlac->ident.samplesTot_read.loU);**/
	}

	return ST_ERR_SUCC;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac audio packet - Subframe
 */
Tst_err
st_contOgg_flacrda_parseSF(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
	Tst_err    res;
	Tst_byte   byt = 0,
	           sft = 0,
	           cha;
	Tst_uint32 i,
	           wb,
	           bsz;
	Tst_int32  *pRS;

	/* parse subframe header */

	/** ZERO PADD BIT, 1 bit */
	if (pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 1, &byt);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 1, &byt);
	if (res != ST_ERR_SUCC)
		return res;
	if (byt != 0x00)
		return ST_ERR_IDAT;

	/** SUBFRAME TYPE, 6 bit */
	if (pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 6, &sft);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 6, &sft);
	if (res != ST_ERR_SUCC)
		return res;
	if (sft == 0x00)
		pDPa->pFra->pCSFr->subFrTp = ST_CONTOGG_FLAC_SFRTP_CONST;
	else if (sft == 0x01)
		pDPa->pFra->pCSFr->subFrTp = ST_CONTOGG_FLAC_SFRTP_VERB;
	else if ((sft & 0x20) == 0x20) {
		pDPa->pFra->pCSFr->subFrTp   = ST_CONTOGG_FLAC_SFRTP_LPC;
		pDPa->pFra->pCSFr->predOrder = (sft & 0x1f) + 1;
	} else if ((sft & 0x10) == 0x10) {
		return ST_ERR_IDAT;
	} else if ((sft & 0x08) == 0x08) {
		pDPa->pFra->pCSFr->subFrTp   = ST_CONTOGG_FLAC_SFRTP_FIX;
		pDPa->pFra->pCSFr->predOrder = (sft & 0x07);
	} else
		return ST_ERR_IDAT;
	/**st_contOgg_prf("SF(): apc #%u sf #%u: order %u\n",
			pDPa->pBSI->pFlac->aPackCnt, pDPa->pFra->pCSFr->channel,
			pDPa->pFra->pCSFr->predOrder);**/

	/** WASTED BITS FLAG, 1 bit (+k bits) */
	if (pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 1, &byt);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 1, &byt);
	if (res != ST_ERR_SUCC)
		return res;
	if (byt == 1) {
		res = st_streamrd_rdUnary(pDPa->pSObj, &pDPa->pFra->pCSFr->wastedBits);
		if (res != ST_ERR_SUCC)
			return res;
		++pDPa->pFra->pCSFr->wastedBits;
		if (pDPa->pFra->pCSFr->sampleSz < pDPa->pFra->pCSFr->wastedBits)
			return ST_ERR_IDAT;
		pDPa->pFra->pCSFr->sampleSz -= pDPa->pFra->pCSFr->wastedBits;
		if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts))
			st_contOgg_d_debBS3d_flacSF(pOpts, pDPa->pFra->pCSFr, "__SF",
					"wasted bits %u --> bps %u",
					pDPa->pFra->pCSFr->wastedBits,
					pDPa->pFra->pCSFr->sampleSz);
	}

	switch (pDPa->pFra->pCSFr->subFrTp) {
	case ST_CONTOGG_FLAC_SFRTP_CONST:
		res = ST_CONTOGG__flacrda_parseSF_const(pOpts, pDPa);
		break;
	case ST_CONTOGG_FLAC_SFRTP_FIX:
		res = ST_CONTOGG__flacrda_parseSF_fix(pOpts, pDPa);
		break;
	case ST_CONTOGG_FLAC_SFRTP_VERB:
		res = ST_CONTOGG__flacrda_parseSF_verbatim(pOpts, pDPa);
		break;
	case ST_CONTOGG_FLAC_SFRTP_LPC:
		res = ST_CONTOGG__flacrda_parseSF_lpc(pOpts, pDPa);
		break;
	default:
		res = ST_ERR_IDAT;
	}
	if (res != ST_ERR_SUCC)
		return res;

	/* */
	if (pOpts->flacOpts.decodeFull && pDPa->pFra->pCSFr->wastedBits > 0) {
		wb  = pDPa->pFra->pCSFr->wastedBits;
		cha = pDPa->pFra->pCSFr->channel;
		bsz = pDPa->pFra->fh.blkSz;
		pRS = pDPa->pFra->rawSamp.ppSamples[cha];
		/* */
		for (i = 0; i < bsz; i++)
			pRS[i] <<= wb;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Undo any special channel coding
 */
void
st_contOgg_flacrda_undoSpecChannEnc(
		const Tst_contOgg_opts *pOpts, Tst_contOgg_codFlac_defParsA *pDPa)
{
#	define LOC_RS_  pDPa->pFra->rawSamp.ppSamples
	Tst_uint32 i,
	           bsz;
	Tst_int32  mid,
	           side;

	/*if (pDPa->pFra->fh.channAss == ST_CONTOGG_FLAC_CA_INDEP ||
			pDPa->pFra->fh.channels != 2)
		return;*/

	bsz = pDPa->pFra->fh.blkSz;
	switch (pDPa->pFra->fh.channAss) {
	case ST_CONTOGG_FLAC_CA_LS:
		for (i = 0; i < bsz; i++)
			LOC_RS_[1][i] = LOC_RS_[0][i] - LOC_RS_[1][i];
		break;
	case ST_CONTOGG_FLAC_CA_RS:
		for (i = 0; i < bsz; i++)
			LOC_RS_[0][i] += LOC_RS_[1][i];
		break;
	case ST_CONTOGG_FLAC_CA_MS:
		for (i = 0; i < bsz; i++) {
			mid   = LOC_RS_[0][i];
			side  = LOC_RS_[1][i];
			mid <<= 1;
			mid  |= (side & 1); /* i.e. if 'side' is odd... */
			/* */
			LOC_RS_[0][i] = (mid + side) >> 1;
			LOC_RS_[1][i] = (mid - side) >> 1;
		}
		break;
	default:
		break;
	}

	/*DEB*
	st_contOgg_prE("\tUndo(): buf 0x%08x %08x - 0x%08x %08x\n"
			"\t            0x%08x %08x - 0x%08x %08x, len %u\n",
			LOC_RS_[0][0], LOC_RS_[0][1],
			LOC_RS_[0][bsz - 2], LOC_RS_[0][bsz - 1],
			LOC_RS_[1][0], LOC_RS_[1][1],
			LOC_RS_[1][bsz - 2], LOC_RS_[1][bsz - 1], bsz);**/

#	undef LOC_RS_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Flac audio packet - Subframe type=constant
 */
static Tst_err
ST_CONTOGG__flacrda_parseSF_const(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
	Tst_err    res;
	Tst_int32  i32 = 0,
	           *pRS;
	Tst_uint32 i,
	           bsz;
	Tst_byte   cha;

	/*DEB*st_contOgg_prE("\nread subframe  CONST\n");**/

	/** Unencoded constant value of the subblock */
	res = st_streamrd_rdInt32(pDPa->pSObj,
			ST_STREAMRD_IEND_BE, pDPa->pFra->pCSFr->sampleSz, &i32);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts))
		st_contOgg_d_debBS3d_flacSF(pOpts, pDPa->pFra->pCSFr, "__SF_const",
				"type=const, bitsPerS=%u\n\t\t\t"
				"unenc const val of subblock =%d",
				pDPa->pFra->pCSFr->sampleSz, i32);

	/* decode the subframe */
	if (pOpts->flacOpts.decodeFull) {
		cha = pDPa->pFra->pCSFr->channel;
		bsz = pDPa->pFra->fh.blkSz;
		pRS = pDPa->pFra->rawSamp.ppSamples[cha];
		/* */
		for (i = 0; i < bsz; i++)
			pRS[i] = i32;
	}

	return ST_ERR_SUCC;
}

/*
 * Parse Flac audio packet - Subframe type=fixed
 */
static Tst_err
ST_CONTOGG__flacrda_parseSF_fix(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
	Tst_err    res;
	Tst_int32  warmArr[32],  /* 32 is the max predictor order */
	           debMsz,
	           *pRS,
	           *pResids;
	Tst_uint32 i,
	           bsz,
	           ssz      = pDPa->pFra->pCSFr->sampleSz;
	Tst_byte   cha,
	           prOrd    = pDPa->pFra->pCSFr->predOrder;
	char       *pDebMsg = NULL,
	           debMsgTmp[64];

	if (prOrd > 32)
		return ST_ERR_IDAT;

	/*DEB*st_contOgg_prE("\nread subframe  FIXED\n");**/

	/** Unencoded warm-up samples */
	for (i = 0; i < prOrd; i++) {
		warmArr[i] = 0;
		/* */
		res = st_streamrd_rdInt32(pDPa->pSObj,
				ST_STREAMRD_IEND_BE, ssz, &warmArr[i]);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			return res;
		/*DEB*st_contOgg_prE("\tFIXED\twu[%u]=%d\n",
				i, warmArr[i]);**/
	}

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		debMsz  = sizeof(debMsgTmp) * (prOrd + 4);
		ST_CALLOC(pDebMsg, char*, debMsz, 1)
		if (pDebMsg == NULL)
			return ST_ERR_OMEM;
		pDebMsg[0] = 0x00;
		for (i = 0; i < prOrd; i++) {
			snprintf((char*)&debMsgTmp, sizeof(debMsgTmp),
					"unenc warm-up sample [%2u]=%+d%s",
					i, warmArr[i],
					(i + 1 < prOrd ? "\n\t\t\t" : ""));
			strncat(pDebMsg, (char*)&debMsgTmp, debMsz);
		}
		st_contOgg_d_debBS3d_flacSF(pOpts, pDPa->pFra->pCSFr, "__SF_fix",
				"type=fixed, bitsPerS=%u, order=%u%s%s",
				pDPa->pFra->pCSFr->sampleSz, prOrd,
				(prOrd > 0 ? "\n\t\t\t" : ""), pDebMsg);
		ST_DELPOINT(pDebMsg)
	}

	/** RESIDUAL */
	res = ST_CONTOGG__flacrda_parseSF_resid(pOpts, pDPa);

	/* decode the subframe */
	if (res == ST_ERR_SUCC && pOpts->flacOpts.decodeFull) {
		cha     = pDPa->pFra->pCSFr->channel;
		bsz     = pDPa->pFra->fh.blkSz;
		pRS     = pDPa->pFra->rawSamp.ppSamples[cha];
		pResids = pDPa->pFra->pCSFr->rawResid.pResids;
		/* */
		memcpy(pRS, warmArr, sizeof(Tst_int32) * prOrd);
		res = ST_CONTOGG__flacrda_restoreFixed(pResids,
				bsz - prOrd, prOrd, pRS + prOrd);
	}
	return res;
}

/*
 * Parse Flac audio packet - Subframe type=verbatim
 */
static Tst_err
ST_CONTOGG__flacrda_parseSF_verbatim(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
	Tst_err    res      = ST_ERR_SUCC;
	Tst_int32  *pI32arr = NULL,
	           *pRS;
	Tst_uint32 bsz      = pDPa->pFra->fh.blkSz,
	           ssz      = pDPa->pFra->pCSFr->sampleSz,
	           x;
	Tst_byte   cha;

	/*DEB*st_contOgg_prE("\nread subframe  VERB\n");**/

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts))
		st_contOgg_d_debBS3d_flacSF(pOpts, pDPa->pFra->pCSFr, "__SF_verbatim",
				"type=verbatim, bitsPerS=%u\n\t\t\t"
				"unenc subblock cnt =%u",
				pDPa->pFra->pCSFr->sampleSz, pDPa->pFra->fh.blkSz);

	if (bsz > pDPa->pFra->tempSF_verbArr.maxBlkSz) {
		res = st_contOgg_stc_reszFlacTempSF_verbArr(
					&pDPa->pFra->tempSF_verbArr, bsz);
		if (res != ST_ERR_SUCC)
			return res;
	}
	pI32arr = pDPa->pFra->tempSF_verbArr.pI32arr;

	/** Unencoded subblock */
	for (x = 0; x < bsz; x++) {
		pI32arr[x] = 0;
		/* */
		res = st_streamrd_rdInt32(pDPa->pSObj,
				ST_STREAMRD_IEND_BE, ssz, pI32arr + x);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			break;
	}
	if (res != ST_ERR_SUCC)
		return res;

	/* decode the subframe */
	if (pOpts->flacOpts.decodeFull) {
		cha = pDPa->pFra->pCSFr->channel;
		pRS = pDPa->pFra->rawSamp.ppSamples[cha];
		/* */
		memcpy(pRS, pI32arr, sizeof(Tst_int32) * bsz);
	}
	return ST_ERR_SUCC;
}

/*
 * Parse Flac audio packet - Subframe type=lpc
 */
static Tst_err
ST_CONTOGG__flacrda_parseSF_lpc(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
	Tst_err    res;
	Tst_int32  warmArr[32],  /* 32 is the max predictor order */
	           *pRS;
	Tst_byte   x,
	           cha;
	char       *pDebMsg = NULL,
	           debMsgTmp[64];
	Tst_uint32 debMsz;
	Tst_contOgg__codFlac_restoreLPC lpc;

	/*DEB*st_contOgg_prE("\nread subframe  LPC\n");**/

	lpc.predOrder = pDPa->pFra->pCSFr->predOrder;
	lpc.sampleSz  = pDPa->pFra->pCSFr->sampleSz;
	lpc.coeffPrec = 0;
	lpc.quantLev  = 0;
	lpc.pResids   = pDPa->pFra->pCSFr->rawResid.pResids;
	lpc.blockSz   = pDPa->pFra->fh.blkSz;
	/* */
	if (lpc.predOrder > 32)
		return ST_ERR_IDAT;

	/** Unencoded warm-up samples */
	for (x = 0; x < lpc.predOrder; x++) {
		warmArr[x] = 0;
		/* */
		res = st_streamrd_rdInt32(pDPa->pSObj,
				ST_STREAMRD_IEND_BE, lpc.sampleSz, &warmArr[x]);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			return res;
	}

	/** Quantized linear predictor coefficients' precision in bits */
	res = st_streamrd_rdByte(pDPa->pSObj, 4, &lpc.coeffPrec);
	if (res != ST_ERR_SUCC)
		return res;
	if (lpc.coeffPrec == 0x0f)
		return ST_ERR_IDAT;
	++lpc.coeffPrec;

	/** Quantized linear predictor coefficient shift needed in bits
	 **   NOTE: this number is signed two's-complement  */
	res = st_streamrd_rdInt32(pDPa->pSObj,
			ST_STREAMRD_IEND_BE, 5, &lpc.quantLev);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;

	/** Unencoded predictor coefficients
	 **   NOTE: this number is signed two's-complement */
	for (x = 0; x < lpc.predOrder; x++) {
		lpc.coeffArr[x] = 0;
		/* */
		res = st_streamrd_rdInt32(pDPa->pSObj,
				ST_STREAMRD_IEND_BE, lpc.coeffPrec,
				&lpc.coeffArr[x]);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			return res;
	}

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		debMsz  = sizeof(debMsgTmp) * (lpc.predOrder + 4) * 2;
		ST_CALLOC(pDebMsg, char*, debMsz, 1)
		if (pDebMsg == NULL)
			return ST_ERR_OMEM;
		pDebMsg[0] = 0x00;
		snprintf(pDebMsg, debMsz,
				"type=lpc, bitsPerS=%u, order=%u\n\t\t\t"
				"quant lin pred coeff prec =%u\n\t\t\t"
				"quant level =%+d%s",
				lpc.sampleSz, lpc.predOrder,
				lpc.coeffPrec, lpc.quantLev,
				(lpc.predOrder > 0 ? "\n\t\t\t" : ""));

		for (x = 0; x < lpc.predOrder; x++) {
			snprintf((char*)debMsgTmp, sizeof(debMsgTmp),
					"unenc pred coeff [%2u]=%+d\n\t\t\t",
					x, lpc.coeffArr[x]);
			strncat(pDebMsg, (char*)debMsgTmp, debMsz);
		}

		for (x = 0; x < lpc.predOrder; x++) {
			snprintf((char*)debMsgTmp, sizeof(debMsgTmp),
					"unenc warm-up sample [%2u]=%+d%s",
					x, warmArr[x],
					(x + 1 < lpc.predOrder ? "\n\t\t\t" : ""));
			strncat(pDebMsg, (char*)debMsgTmp, debMsz);
		}

		st_contOgg_d_debBS3d_flacSF(pOpts, pDPa->pFra->pCSFr, "__SF_lpc",
				pDebMsg);
		ST_DELPOINT(pDebMsg)
	}

	/** RESIDUAL */
	res = ST_CONTOGG__flacrda_parseSF_resid(pOpts, pDPa);

	/* decode the subframe */
	if (res == ST_ERR_SUCC && pOpts->flacOpts.decodeFull) {
		if (lpc.blockSz < lpc.predOrder ||
				pDPa->pFra->pCSFr->rawResid.cnt < lpc.blockSz - lpc.predOrder)
			return ST_ERR_FAIL;
		cha = pDPa->pFra->pCSFr->channel;
		pRS = pDPa->pFra->rawSamp.ppSamples[cha];
		/* */
		memcpy(pRS, &warmArr, sizeof(Tst_int32) * lpc.predOrder);
		res = ST_CONTOGG__flacrda_restoreLPC(&lpc, &pRS[lpc.predOrder]);
	}
	return res;
}

/*
 * Parse Flac audio packet - Subframe residual
 */
static Tst_err
ST_CONTOGG__flacrda_parseSF_resid(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsA *pDPa)
{
	Tst_err    res;
	Tst_byte   cod     = 0,
	           partOrd = 0,
	           len1    = 4,  /* RICE1, 4 bits */
	           prOrd   = pDPa->pFra->pCSFr->predOrder,
	           *pEPars;
	Tst_uint32 partSamples,
	           partitions,
	           x,
	           y,
	           n,
	           bsz      = pDPa->pFra->fh.blkSz;
	Tst_int32  i32      = 0,
	           *pResids = pDPa->pFra->pCSFr->rawResid.pResids;
	char       *pDebMsg = NULL,
	           debMsgTmp[32];
	Tst_uint32 debMsz;

	/* read Residual coding method (2 bits) and Partition order (4 bits) */
	if (pOpts->flacOpts.checkCRCfr)
		res = st_streamrd_rdByte(pDPa->pSObj, 6, &cod);
	else
		res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 6, &cod);
	if (res != ST_ERR_SUCC)
		return res;
	partOrd = (cod & 0x0f);
	cod     = (cod >> 4) & 0x03;

	/** Residual coding method, 2 bits */
	if ((cod & 0x02) == 0x02)
		return ST_ERR_IDAT;

	/* amount of partitions */
	partitions = (1 << partOrd);
	if (partitions > pDPa->pFra->tempSF_residEParsSz) {
		ST_REALLOC(pDPa->pFra->pTempSF_residEPars, Tst_byte*,
				(partitions > 64 ? partitions : 64), 1)
		if (pDPa->pFra->pTempSF_residEPars == NULL)
			return ST_ERR_OMEM;
		pDPa->pFra->tempSF_residEParsSz = (partitions > 64 ? partitions : 64);
	}
	pEPars = pDPa->pFra->pTempSF_residEPars;
	/* amount of partition samples */
	if (partOrd < 1 && bsz < prOrd)
		return ST_ERR_IDAT;
	partSamples = (partOrd > 0 ? bsz >> partOrd : bsz - prOrd);

	if (cod == 1)
		len1 = 5;  /* RICE2, 5 bits */

	/** Partitions */
	for (x = 0; x < partitions; x++) {
		pEPars[x] = 0;
		/** Encoding parameter, len1 bits */
		if (pOpts->flacOpts.checkCRCfr)
			res = st_streamrd_rdByte(pDPa->pSObj, len1, pEPars + x);
		else
			res = st_streamrd_rdByte_noCRC(pDPa->pSObj, len1, pEPars + x);
		if (res != ST_ERR_SUCC)
			return res;
		if ((cod == 0 && pEPars[x] == 0x0f) ||
				(cod == 1 && pEPars[x] == 0x1f)) {
			/** Encoding parameter, 5 bits */
			if (pOpts->flacOpts.checkCRCfr)
				res = st_streamrd_rdByte(pDPa->pSObj, 5, pEPars + x);
			else
				res = st_streamrd_rdByte_noCRC(pDPa->pSObj, 5, pEPars + x);
			if (res != ST_ERR_SUCC)
				return res;

			/*DEB*st_contOgg_prE("\tresid\tUnEnc partOrd=%u, partSam=%u\n",
					partOrd, partSamples);**/

			/** unencoded binary form using epar bits per sample */
			for (y = (partOrd == 0 || x > 0) ? 0 : prOrd;
					y < partSamples; y++) {
				res = st_streamrd_rdInt32(pDPa->pSObj,
						ST_STREAMRD_IEND_BE, pEPars[x], &i32);  /* ENDIAN: BE-->HOST */
				if (res != ST_ERR_SUCC)
					return res;
				if (pResids != NULL)
					*pResids++ = i32;
			}
		} else {
			/** Encoded residual */
			/*** number of samples (n) in the partition */
			if (partOrd == 0 || x > 0) {
				n = partSamples;
			} else {
				if (partSamples < prOrd)
					return ST_ERR_IDAT;
				n = partSamples - prOrd;
			}
			/*** */

			/*DEB*st_contOgg_prE("\tresid\tEncRi n=%u\n", n);**/

			res = st_streamrd_rdFlacRiceSamples(pDPa->pSObj, n,
					pEPars[x], pResids);
			if (res != ST_ERR_SUCC)
				return res;
			if (pResids != NULL)
				pResids += n;
		}
	}

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		debMsz = sizeof(debMsgTmp) * (partitions + 4);
		ST_CALLOC(pDebMsg, char*, debMsz, 1)
		if (pDebMsg == NULL)
			return ST_ERR_OMEM;
		pDebMsg[0] = 0x00;
		snprintf(pDebMsg, debMsz,
				"residual RICE%d, partitions =%u%s",
				(cod == 1 ? 2 : 1), partitions,
				(partitions > 0 ? "\n\t\t\t" : ""));

		for (x = 0; x < partitions; x++) {
			snprintf((char*)&debMsgTmp, sizeof(debMsgTmp),
					"rice_p [%2u]=%u%s",
					x, pEPars[x],
					(x + 1 < partitions ? "\n\t\t\t" : ""));
			strncat(pDebMsg, (char*)&debMsgTmp, debMsz);
		}

		st_contOgg_d_debBS3d_flacSF(pOpts, pDPa->pFra->pCSFr, "__SF_resid",
				pDebMsg);
		ST_DELPOINT(pDebMsg)
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Restore signal of Fixed subframe
 */
static Tst_err
ST_CONTOGG__flacrda_restoreFixed(const Tst_int32 *pResids,
		const Tst_uint32 sampleCnt, const Tst_uint32 predOrder,
		Tst_int32 *pRawSamples)
{
#	define LOC_RAWSAMP_V2_(noffs)  (*(Tst_int32*)(pRawSamples + i - noffs))
#	define LOC_RAWSAMP_(noffs)  (pRawSamples[i - noffs])
	Tst_int32 i,
	          scnt = (Tst_int32)sampleCnt;

	switch (predOrder) {
		case 0:
			memcpy(pRawSamples, pResids, sizeof(Tst_int32) * sampleCnt);
			break;
		case 1:
			for (i = 0; i < scnt; i++)
				pRawSamples[i] = pResids[i] +
						LOC_RAWSAMP_(1);
			break;
		case 2:
			for (i = 0; i < scnt; i++)
				pRawSamples[i] = pResids[i] +
						(LOC_RAWSAMP_(1) << 1) -
						LOC_RAWSAMP_(2);
			break;
		case 3:
			for (i = 0; i < scnt; i++)
				pRawSamples[i] = pResids[i] +
						(((LOC_RAWSAMP_(1) - LOC_RAWSAMP_(2)) << 1) +
							(LOC_RAWSAMP_(1) - LOC_RAWSAMP_(2))) +
						LOC_RAWSAMP_(3);
			break;
		case 4:
			for (i = 0; i < scnt; i++)
				pRawSamples[i] = pResids[i] +
						((LOC_RAWSAMP_(1) + LOC_RAWSAMP_(3)) << 2) -
						((LOC_RAWSAMP_(2) << 2) + (LOC_RAWSAMP_(2) << 1)) -
						LOC_RAWSAMP_(4);
			break;
		default:
			return ST_ERR_IDAT;
	}

	/*DEB*
	st_contOgg_prE("\tRestFix(): resid 0x%08x %08x - 0x%08x %08x\n"
			"\t           rawsa 0x%08x %08x - 0x%08x %08x, len %d, prOrd %u\n",
			pResids[0], pResids[1], pResids[scnt - 2], pResids[scnt - 1],
			pRawSamples[0], pRawSamples[1], pRawSamples[scnt - 2],
			pRawSamples[scnt - 1], scnt, predOrder); **/

	return ST_ERR_SUCC;
#	undef LOC_RAWSAMP_
#	undef LOC_RAWSAMP_V2_
}

/*----------------------------------------------------------------------------*/

/*
 * Restore signal of LPC subframe
 */
static Tst_err
ST_CONTOGG__flacrda_restoreLPC(
		const Tst_contOgg__codFlac_restoreLPC *pLPC, Tst_int32 *pRawSamples)
{
	Tst_contOgg__codFlac_restoreLPC2 lpc2;

	if (pLPC->blockSz < pLPC->predOrder)
		return ST_ERR_IARG;

	lpc2.predOrder = pLPC->predOrder;
	lpc2.coeffPrec = pLPC->coeffPrec;
	lpc2.pCoeffArr = (Tst_int32 const*)&pLPC->coeffArr;
	lpc2.quantLev  = pLPC->quantLev;
	lpc2.pResids   = pLPC->pResids;
	lpc2.sampleCnt = pLPC->blockSz - pLPC->predOrder;
	/* */
	if (pLPC->sampleSz + pLPC->coeffPrec +
			sysILog2_UI8(pLPC->predOrder) <= 32)
		return ST_CONTOGG__flacrda_restoreLPC_32bit(&lpc2, pRawSamples);
	return ST_CONTOGG__flacrda_restoreLPC_64bit(&lpc2, pRawSamples);
}

/* the following code is an almost exact copy from libflac-1.2.1 */
static Tst_err
ST_CONTOGG__flacrda_restoreLPC_32bit(
		const Tst_contOgg__codFlac_restoreLPC2 *pLPC2, Tst_int32 *pRawSamples2)
{
#	define LOC_SIMPLE_  0
#	if (LOC_SIMPLE_ == 1)
	Tst_int32       sum,
	                *pRScur    = pRawSamples2,
	                *pRScur2,
	                quLev      = pLPC2->quantLev;
	Tst_int32 const *pResidCur = pLPC2->pResids,
	                *pCoeffs   = pLPC2->pCoeffArr;
	Tst_uint32      i,
	                j,
	                scnt       = pLPC2->sampleCnt,
	                prOrd      = pLPC2->predOrder;

	for (i = 0; i < scnt; i++) {
		sum     = 0;
		pRScur2 = pRScur;
		for (j = 0; j < prOrd; j++)
			sum += pCoeffs[j] * (*(--pRScur2));
		*pRScur++ = *pResidCur++ + (sum >> quLev);
	}
#	else
#	define LOC_RAWSAM_  pRawSamples2
#	define LOC_COEFFS_  pCoeffs
#	define LOC_RESID_   pResids
	const Tst_int32 cPREDORD = pLPC2->predOrder,
	                cQUALEV  = pLPC2->quantLev;
	register Tst_int32 i,
	                   sum,
	                   samCnt = pLPC2->sampleCnt;
	Tst_int32 const *pCoeffs = pLPC2->pCoeffArr,
	                *pResids = pLPC2->pResids;

	/*if (order < 1 || order > 32)
		return ST_ERR_IARG;*/

	if (cPREDORD == 8) {
		/* order == 8 */
		for (i = 0; i < samCnt; i++) {
			sum  = 0;
			sum += LOC_COEFFS_[7] * LOC_RAWSAM_[i-8];
			sum += LOC_COEFFS_[6] * LOC_RAWSAM_[i-7];
			sum += LOC_COEFFS_[5] * LOC_RAWSAM_[i-6];
			sum += LOC_COEFFS_[4] * LOC_RAWSAM_[i-5];
			sum += LOC_COEFFS_[3] * LOC_RAWSAM_[i-4];
			sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
			sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
			sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
			LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
		}
	} else if (cPREDORD < 13) {
		/* 1 <= order <= 12 */
		if (cPREDORD < 7) {
			/* 1 <= order <= 6 */
			if (cPREDORD < 4) {
				/* 1 <= order <= 3 */
				switch (cPREDORD) {
				case 1:
					for (i = 0; i < samCnt; i++)
						LOC_RAWSAM_[i] = LOC_RESID_[i] +
								((LOC_COEFFS_[0] * LOC_RAWSAM_[i-1]) >> cQUALEV);
					break;
				case 2:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
					break;
				default:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
				}
			} else {
				/* 4 <= order <= 6 */
				switch (cPREDORD) {
				case 4:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
					break;
				case 5:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[4] * LOC_RAWSAM_[i-5];
						sum += LOC_COEFFS_[3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
					break;
				default:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[5] * LOC_RAWSAM_[i-6];
						sum += LOC_COEFFS_[4] * LOC_RAWSAM_[i-5];
						sum += LOC_COEFFS_[3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
				}
			}
		} else {
			/* 7 <= order <= 12 */
			if (cPREDORD < 10) {
				/* 7 <= order <= 9 */
				switch (cPREDORD) {
				case 7:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[6] * LOC_RAWSAM_[i-7];
						sum += LOC_COEFFS_[5] * LOC_RAWSAM_[i-6];
						sum += LOC_COEFFS_[4] * LOC_RAWSAM_[i-5];
						sum += LOC_COEFFS_[3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
					break;
				default:
					for (i = 0; i < samCnt; i++) {
						sum = 0;
						sum += LOC_COEFFS_[8] * LOC_RAWSAM_[i-9];
						sum += LOC_COEFFS_[7] * LOC_RAWSAM_[i-8];
						sum += LOC_COEFFS_[6] * LOC_RAWSAM_[i-7];
						sum += LOC_COEFFS_[5] * LOC_RAWSAM_[i-6];
						sum += LOC_COEFFS_[4] * LOC_RAWSAM_[i-5];
						sum += LOC_COEFFS_[3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
				}
			} else {
				/* 10 <= order <= 12 */
				switch (cPREDORD) {
				case 10:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[9] * LOC_RAWSAM_[i-10];
						sum += LOC_COEFFS_[8] * LOC_RAWSAM_[i-9];
						sum += LOC_COEFFS_[7] * LOC_RAWSAM_[i-8];
						sum += LOC_COEFFS_[6] * LOC_RAWSAM_[i-7];
						sum += LOC_COEFFS_[5] * LOC_RAWSAM_[i-6];
						sum += LOC_COEFFS_[4] * LOC_RAWSAM_[i-5];
						sum += LOC_COEFFS_[3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
					break;
				case 11:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[10] * LOC_RAWSAM_[i-11];
						sum += LOC_COEFFS_[ 9] * LOC_RAWSAM_[i-10];
						sum += LOC_COEFFS_[ 8] * LOC_RAWSAM_[i-9];
						sum += LOC_COEFFS_[ 7] * LOC_RAWSAM_[i-8];
						sum += LOC_COEFFS_[ 6] * LOC_RAWSAM_[i-7];
						sum += LOC_COEFFS_[ 5] * LOC_RAWSAM_[i-6];
						sum += LOC_COEFFS_[ 4] * LOC_RAWSAM_[i-5];
						sum += LOC_COEFFS_[ 3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[ 2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[ 1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[ 0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
					break;
				default:
					for (i = 0; i < samCnt; i++) {
						sum  = 0;
						sum += LOC_COEFFS_[11] * LOC_RAWSAM_[i-12];
						sum += LOC_COEFFS_[10] * LOC_RAWSAM_[i-11];
						sum += LOC_COEFFS_[ 9] * LOC_RAWSAM_[i-10];
						sum += LOC_COEFFS_[ 8] * LOC_RAWSAM_[i-9];
						sum += LOC_COEFFS_[ 7] * LOC_RAWSAM_[i-8];
						sum += LOC_COEFFS_[ 6] * LOC_RAWSAM_[i-7];
						sum += LOC_COEFFS_[ 5] * LOC_RAWSAM_[i-6];
						sum += LOC_COEFFS_[ 4] * LOC_RAWSAM_[i-5];
						sum += LOC_COEFFS_[ 3] * LOC_RAWSAM_[i-4];
						sum += LOC_COEFFS_[ 2] * LOC_RAWSAM_[i-3];
						sum += LOC_COEFFS_[ 1] * LOC_RAWSAM_[i-2];
						sum += LOC_COEFFS_[ 0] * LOC_RAWSAM_[i-1];
						LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
					}
				}
			}
		}
	} else {
		/* order >= 13 */
		for (i = 0; i < samCnt; i++) {
			sum = 0;
			switch (cPREDORD) {
				case 32: sum += LOC_COEFFS_[31] * LOC_RAWSAM_[i-32];
				case 31: sum += LOC_COEFFS_[30] * LOC_RAWSAM_[i-31];
				case 30: sum += LOC_COEFFS_[29] * LOC_RAWSAM_[i-30];
				case 29: sum += LOC_COEFFS_[28] * LOC_RAWSAM_[i-29];
				case 28: sum += LOC_COEFFS_[27] * LOC_RAWSAM_[i-28];
				case 27: sum += LOC_COEFFS_[26] * LOC_RAWSAM_[i-27];
				case 26: sum += LOC_COEFFS_[25] * LOC_RAWSAM_[i-26];
				case 25: sum += LOC_COEFFS_[24] * LOC_RAWSAM_[i-25];
				case 24: sum += LOC_COEFFS_[23] * LOC_RAWSAM_[i-24];
				case 23: sum += LOC_COEFFS_[22] * LOC_RAWSAM_[i-23];
				case 22: sum += LOC_COEFFS_[21] * LOC_RAWSAM_[i-22];
				case 21: sum += LOC_COEFFS_[20] * LOC_RAWSAM_[i-21];
				case 20: sum += LOC_COEFFS_[19] * LOC_RAWSAM_[i-20];
				case 19: sum += LOC_COEFFS_[18] * LOC_RAWSAM_[i-19];
				case 18: sum += LOC_COEFFS_[17] * LOC_RAWSAM_[i-18];
				case 17: sum += LOC_COEFFS_[16] * LOC_RAWSAM_[i-17];
				case 16: sum += LOC_COEFFS_[15] * LOC_RAWSAM_[i-16];
				case 15: sum += LOC_COEFFS_[14] * LOC_RAWSAM_[i-15];
				case 14: sum += LOC_COEFFS_[13] * LOC_RAWSAM_[i-14];
				case 13: sum += LOC_COEFFS_[12] * LOC_RAWSAM_[i-13];
				         sum += LOC_COEFFS_[11] * LOC_RAWSAM_[i-12];
				         sum += LOC_COEFFS_[10] * LOC_RAWSAM_[i-11];
				         sum += LOC_COEFFS_[ 9] * LOC_RAWSAM_[i-10];
				         sum += LOC_COEFFS_[ 8] * LOC_RAWSAM_[i- 9];
				         sum += LOC_COEFFS_[ 7] * LOC_RAWSAM_[i- 8];
				         sum += LOC_COEFFS_[ 6] * LOC_RAWSAM_[i- 7];
				         sum += LOC_COEFFS_[ 5] * LOC_RAWSAM_[i- 6];
				         sum += LOC_COEFFS_[ 4] * LOC_RAWSAM_[i- 5];
				         sum += LOC_COEFFS_[ 3] * LOC_RAWSAM_[i- 4];
				         sum += LOC_COEFFS_[ 2] * LOC_RAWSAM_[i- 3];
				         sum += LOC_COEFFS_[ 1] * LOC_RAWSAM_[i- 2];
				         sum += LOC_COEFFS_[ 0] * LOC_RAWSAM_[i- 1];
			}
			LOC_RAWSAM_[i] = LOC_RESID_[i] + (sum >> cQUALEV);
		}
	}

#	undef LOC_RAWSAM_
#	undef LOC_COEFFS_
#	undef LOC_RESID_
#	endif
#	undef LOC_SIMPLE_
	return ST_ERR_SUCC;
}

/* the following code is an almost exact copy from libflac-1.2.1 */
static Tst_err
ST_CONTOGG__flacrda_restoreLPC_64bit(
		const Tst_contOgg__codFlac_restoreLPC2 *pLPC2,
		Tst_int32 *pRawSamples2)
{
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	register Tst_int64_native sum64;
	register Tst_uint32       i,
	                          j;
	Tst_uint32      prOrd      = pLPC2->predOrder,
	                scnt       = pLPC2->sampleCnt;
	Tst_int32       *pRScur    = pRawSamples2,
	                *pRScur2;
	Tst_int32 const *pResidCur = pLPC2->pResids;

	/**st_contOgg_prf("restoreLPC_64bit native\n");**/
	for (i = 0; i < scnt; i++) {
		sum64   = 0;
		pRScur2 = pRScur;
		for (j = 0; j < prOrd; j++) {
			sum64 += ((Tst_int64_native)pLPC2->pCoeffArr[j] *
					(Tst_int64_native)(*(--pRScur2)));
		}
		sum64   >>= pLPC2->quantLev;
		*pRScur++ = *pResidCur++ + (Tst_int32)sum64;
	}
#	else
	Tst_int64           sum64,
	                    mult64;
	Tst_uint64          mult64UI;
	register Tst_uint32 i,
	                    j;
	Tst_uint32          prOrd      = pLPC2->predOrder,
	                    scnt       = pLPC2->sampleCnt;
	Tst_int32           *pRScur    = pRawSamples2,
	                    *pRScur2;
	Tst_int32 const     *pResidCur = pLPC2->pResids,
	                    *pCoeffs   = pLPC2->pCoeffArr;
	const Tst_int32     quLev      = pLPC2->quantLev;

	/**st_contOgg_prf("restoreLPC_64bit\n");**/
	ST_SYSMATH_STC_RSETSI64(mult64)
	ST_SYSMATH_STC_RSETUI64(mult64UI)
	for (i = 0; i < scnt; i++) {
		ST_SYSMATH_STC_RSETSI64(sum64)
		pRScur2 = pRScur;
		for (j = 0; j < prOrd; j++) {
			--pRScur2;
			if (pCoeffs[j] >= 0 && *pRScur2 >= 0) {
				st_sysUInt64_multUI32s(&mult64UI, pCoeffs[j], *pRScur2);
				/* sum64 += mult64 */
				sum64.loS += mult64UI.loU;
				if (sum64.loS < mult64UI.loU)
					++sum64.hiS;
				sum64.hiS += mult64UI.hiU;
			} else if (pCoeffs[j] < 0 && *pRScur2 < 0) {
				st_sysUInt64_multUI32s(&mult64UI, -pCoeffs[j], -*pRScur2);
				/* sum64 += mult64 */
				sum64.loS += mult64UI.loU;
				if (sum64.loS < mult64UI.loU)
					++sum64.hiS;
				sum64.hiS += mult64UI.hiU;
			} else {
				st_sysSInt64_multI32s(&mult64, pCoeffs[j], *pRScur2);
				/* sum64 += mult64 */
				sum64.loS += mult64.loS;
				if (sum64.loS < mult64.loS)
					++sum64.hiS;
				sum64.hiS += mult64.hiS;
			}
		}
		st_sysSInt64_shiftRight(&sum64, quLev);
		*pRScur++ = *pResidCur++ + (Tst_int32)sum64.loS;
	}
#	endif
	return ST_ERR_SUCC;
}

/******************************************************************************/
