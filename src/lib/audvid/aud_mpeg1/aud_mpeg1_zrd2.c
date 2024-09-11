/******************************************************************************/
/* aud_mpeg1_zrd2.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for MPEG-1 Frameheader and VBR Headers                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
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
/** */
#define SRC_AUD_MPEG1_ZRD2_C
#include "aud_mpeg1_zrd2-pp.h"
/*** */
#include "aud_mpeg1_stcs-prot.h"
#include "aud_mpeg1_zfdeb-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_AUD_MPEG1_ZRD2_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read and parse MPEG Frameheader
 *   current stream won't be modififed (e.g. advanced)
 *
 * returns: ERRCODE  (error from st_streamrd_rdAheadUInt32() or
 *                    ST_ERR_IARG, ST_ERR_MIHD or ST_ERR_SUCC)
 */
Tst_err
st_mpeg1_rd2_readFrHd(Tst_mpeg1_obj_intn *pMOI, Tst_mpeg1_farnfh *pFar)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_buf    uch;
	Tst_foffs  offsBeg = -1;
	Tst_uint32 rdBytes = 0;
	char       debMsg[256];
	Tst_mpeg1_fhd *pMFHnew;

	ST_ASSERTN_IARG(pMOI == NULL || pMOI->pStrrd == NULL ||
			pFar == NULL)

	pMFHnew = &pFar->mfhNew;
	ST_MPEG1_STC_RSETFHD_BUT_NOT_FRAMENR(pMFHnew)

	/* Frameheader einlesen */
	/** */
	if (ST_AVFDEB_ISVERBAUD_BD(pMOI->opts.basOpts))
		offsBeg = st_streamrd_getCurPos(pMOI->pStrrd);
	/** */
	res = st_streamrd_rdAheadBuffer(pMOI->pStrrd, sizeof(pMFHnew->rawFHd),
			pMFHnew->rawFHd, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != sizeof(pMFHnew->rawFHd)) {
		if (res != ST_ERR_FEOF && rdBytes == sizeof(pMFHnew->rawFHd))
			st_mpeg1_d_err(&pMOI->opts, cFNCN, pMOI->pFilen,
					"can't read from stream");
		else
			res = ST_ERR_FEOF;  /* this shouldn't happen */
		return res;
	}

	/* erstes Byte v. MPEG Frameheader pruefen */
	uch = pMFHnew->rawFHd[0];
	/** frame sync (11 bit), ersten 8 bit  */
	if (uch != 0xff) {
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": can't be a valid MPEG frameheader "
					"(fraSync 1, 0x%02x)", (Tst_foffs)offsBeg, uch);
		return ST_ERR_MIHD;
	}

	/* zweites Byte v. Frameheader pruefen */
	uch = pMFHnew->rawFHd[1];
	/** frame sync, naechsten 3 bit  */
	if ((uch & 0xe0) != 0xe0) {
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": can't be a valid MPEG frameheader "
					"(fraSync 2, 0x%02x)", (Tst_foffs)offsBeg, (uch & 0xe0));
		return ST_ERR_MIHD;
	}
	/** Audio version (2 bit) */
	switch (uch & 0x18) {
	case 0x00:
		pMFHnew->audioVers = ST_MPEG1_AUDVERS_25; break;  /* 2.5 */
	/*case 0x08:
		pMFHnew->audioVers = ST_MPEG1_AUDVERS_NONE; break;*/   /* reserved */
	case 0x10:
		pMFHnew->audioVers = ST_MPEG1_AUDVERS_2; break;   /* 2.0 */
	case 0x18:
		pMFHnew->audioVers = ST_MPEG1_AUDVERS_1; break;   /* 1.0, MP3 */
	default:
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": can't be a valid MPEG frameheader "
					"(aud vers)", (Tst_foffs)offsBeg);
		return ST_ERR_MIHD;
	}
	if (pMOI->opts.forceMAv != ST_MPEG1_AUDVERS_NONE &&
			pMFHnew->audioVers != pMOI->opts.forceMAv) {
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": ignoring possible MPEG frameheader "
					"(forced aud vers)", (Tst_foffs)offsBeg);
		return ST_ERR_MIHD;
	}
	/** Layer (2 bit) */
	switch (uch & 0x06) {
	/*case 0x00:
		pMFHnew->layer = ST_MPEG1_LAYER_NONE; break;*/   /* reserved */
	case 0x02:
		pMFHnew->layer = ST_MPEG1_LAYER_3; break;   /* III ^= MP3 */
	case 0x04:
		pMFHnew->layer = ST_MPEG1_LAYER_2; break;   /* II ^= MP2 */
	case 0x06:
		pMFHnew->layer = ST_MPEG1_LAYER_1; break;   /* I ^= MP1 */
	default:
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": "
					"can't be a valid MPEG frameheader (layer)",
					(Tst_foffs)offsBeg);
		return ST_ERR_MIHD;
	}
	if (pMOI->opts.forceMLay != ST_MPEG1_LAYER_NONE &&
			pMFHnew->layer != pMOI->opts.forceMLay) {
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": ignoring possible MPEG frameheader "
					"(forced layer)", (Tst_foffs)offsBeg);
		return ST_ERR_MIHD;
	}
	/** Protection Bit (1 bit) */
	if ((uch & 0x01) == 0x01) {
		/* no CRC */
		pMFHnew->hasCRC = 0;
	} else {
		/* protected by 16 bit CRC following header */
		pMFHnew->hasCRC = 1;
	}

	/* drittes Byte v. Frameheader pruefen */
	uch = pMFHnew->rawFHd[2];
	/** Bitrate Index (4 bit) */
	res = ST_MPEG1__rd2_calcBRI(uch & 0xf0, pMFHnew->audioVers,
			pMFHnew->layer, &pMFHnew->bitrate);
	if (res != ST_ERR_SUCC ||
			(pMFHnew->bitrate == 0 && ! pFar->isFreeFmtAllw)) {
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts)) {
			switch (res) {
			case ST_ERR_IARG:
				sprintf(debMsg, "IARG"); break;
			case ST_ERR_MICO:
				sprintf(debMsg, "MICO"); break;
			case ST_ERR_MIBI:
				sprintf(debMsg, "MIBI 0x%02x", (uch & 0xf0)); break;
			default:
				sprintf(debMsg, "%u", pMFHnew->bitrate);
			}
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": can't be a valid MPEG frameheader "
					"(bitrIx %s)", (Tst_foffs)offsBeg, debMsg);
		}
		return ST_ERR_MIHD;
	}
	/** Sampling rate frequency index (2 bit) */
	res = ST_MPEG1__rd2_calcSRFI(uch & 0x0c,
			pMFHnew->audioVers, &pMFHnew->samplerate);
	if (res != ST_ERR_SUCC || pMFHnew->samplerate == 0) {
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": can't be a valid MPEG frameheader "
					"(SampFreqIx)", (Tst_foffs)offsBeg);
		return ST_ERR_MIHD;
	}
	/** Padding Bit (1 bit) */
	if ((uch & 0x02) == 0x02) {
		switch (pMFHnew->layer) {
		case ST_MPEG1_LAYER_1: pMFHnew->paddLen = 4; break;
		case ST_MPEG1_LAYER_2:
		case ST_MPEG1_LAYER_3: pMFHnew->paddLen = 1; break;
		default:
			break;
		}
	}
	/** Private Bit (1 bit), ignored */

	/* viertes Byte v. Frameheader pruefen */
	uch = pMFHnew->rawFHd[3];
	/** Channel Mode (2 bit) */
	switch (uch & 0xc0) {
	case 0x00:  /* stereo */
		pMFHnew->channMode = ST_MPEG1_CMODE_STEREO; break;
	case 0x40:  /* joint stereo */
		pMFHnew->channMode = ST_MPEG1_CMODE_JSTEREO; break;
	case 0x80:  /* dual channel, two mono channels */
		pMFHnew->channMode = ST_MPEG1_CMODE_DUAL; break;
	case 0xc0:  /* single channel, mono */
		pMFHnew->channMode = ST_MPEG1_CMODE_SINGLE; break;
	}
	/** Mode Extension (only for Joint Stereo) (2 bit), ignored */
	/** Copyright Bit (1 bit), ignored */
	/** Original Bit (1 bit), ignored */
	/** Emphasis (2 bit), ignored */

	/* get frame size */
	ST_MPEG1__rd2_getFrameLength(pMFHnew);
	if (pMFHnew->frameSz < ST_MPEG1_FR_SZ_MIN) {
		if (pFar->frOffsDelta == 0 &&
				ST_AVFDEB_ISVERBAUDDET_BD(pMOI->opts.basOpts))
			st_mpeg1_d_deb(&pMOI->opts, cFNCN,
					"o "ST_TFOFFS_PRF_0X": can't be a valid MPEG frameheader "
					"(frameSz %u)", (Tst_foffs)offsBeg, pMFHnew->frameSz);
		return ST_ERR_MIHD;
	}

	/**st_mpeg1_prf("\t\t%s: ", cFNCN,
				"o "ST_TFOFFS_PRF_0X": found valid MPEG frameheader "
				"(fBrt=%3u  ver=%d, lay=%d, sr=%5u, crc=%c, pad=%u)\n",
				(Tst_foffs)st_streamrd_getCurPos(pMOI->pStrrd),
				pMFHnew->bitrate, (Tst_int32)pMFHnew->audioVers,
				(Tst_int32)pMFHnew->layer, pMFHnew->samplerate,
				(pMFHnew->hasCRC ? 'y' : 'n'),
				pMFHnew->upaddLen);**/
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Get MPEG side info size
 *
 * pMFH  MUST be set
 *
 * returns: side info size in bytes
 */
Tst_int32
st_mpeg1_rd2_getSideinfoSize(const Tst_mpeg1_fhd *pMFH)
{
	Tst_int32 ret = 0;

	ST_ASSERTN_NUM(0, pMFH == NULL)  /* ret 0 */

	switch (pMFH->channMode) {
	case ST_MPEG1_CMODE_STEREO:
	case ST_MPEG1_CMODE_JSTEREO:
	case ST_MPEG1_CMODE_DUAL:
		if (pMFH->audioVers == ST_MPEG1_AUDVERS_1)
			ret = 32;
		else if (pMFH->audioVers == ST_MPEG1_AUDVERS_2 ||
				pMFH->audioVers == ST_MPEG1_AUDVERS_25)
			ret = 17;
		break;
	case ST_MPEG1_CMODE_SINGLE:
		if (pMFH->audioVers == ST_MPEG1_AUDVERS_1)
			ret = 17;
		else if (pMFH->audioVers == ST_MPEG1_AUDVERS_2 ||
				pMFH->audioVers == ST_MPEG1_AUDVERS_25)
			ret = 9;
		break;
	default:
		break;
	}
	return ret;
}

/*----------------------------------------------------------------------------*/

ST_INLINE_S Tst_uint32
st_mpeg1_rd2_getSamplesPerFrame(const Tst_mpeg1_layer lay,
		const Tst_mpeg1_audioVers vers)
{
	if (lay == ST_MPEG1_LAYER_1)
		return 384;
	if (lay == ST_MPEG1_LAYER_2)
		return 1152;
	if (lay == ST_MPEG1_LAYER_3) {
		if (vers == ST_MPEG1_AUDVERS_1)
			return 1152;
		if (vers == ST_MPEG1_AUDVERS_2 || vers == ST_MPEG1_AUDVERS_25)
			return 576;
	}
	return 0;
}

/*----------------------------------------------------------------------------*/

/*
 * Read and parse a Xing/Info header
 *
 * pBuf      MUST be != NULL
 * pVBRhd    MUST be != NULL
 *
 * returns: ERRCODE
 *          pVBRhd will contain header info
 */
Tst_err
st_mpeg1_rd2_readVBRheadXing(const Tst_buf *pBuf, const Tst_uint32 bufSz,
		Tst_mpeg1_vbr *pVBRhd)
{
#	define LOC_BUFMAXSZ_  (ST_MPEG1_TAGVBR_XING_SLEN + 4 + 4 + 4 + 100 + 4)
	Tst_buf const *pBB = pBuf,
	              *pBEnd;
	/**Tst_str    str[4 + 1];**/
	Tst_uint32    val  = 0,
	              flags;

	ST_ASSERTN_IARG(pBuf == NULL || pVBRhd == NULL)

	if (bufSz < ST_MPEG1_TAGVBR_XING_SLEN + 4) {
		/* we need at least ID and FLAGS */
		return ST_ERR_IDAT;
	}

	pBEnd = pBB + bufSz;

	/* ID field (4 bytes) */
	if (! st_sysStrcmpN2(ST_B_TRUE, ST_MPEG1_TAGVBR_XING_SLEN,
				pBB, ST_MPEG1_TAGVBR_XING) &&
			! st_sysStrcmpN2(ST_B_TRUE, ST_MPEG1_TAGVBR_INFO_SLEN,
						pBB, ST_MPEG1_TAGVBR_INFO))
		return ST_ERR_IDAT;
	/**strncpy((char*)&str, (char*)pBB, ST_MPEG1_TAGVBR_XING_SLEN);
	str[ST_MPEG1_TAGVBR_XING_SLEN] = 0;
	st_mpeg1_prE("      hdID: %s\n", (char*)&str);**/
	pBB += ST_MPEG1_TAGVBR_XING_SLEN;

	/* all numeric values are read in Big-Endian */

	/* Flags (4 bytes) */
	memcpy(&val, pBB, 4);
#	if (WORDS_BIGENDIAN != 1)
	val = st_sysReverseByteOrder_UI32(val);  /* ENDIAN: BE-->LE */
#	endif
	val = val & 0x0f;
	if (val == 0)
		return ST_ERR_SUCC;
	flags = val;
	/**st_mpeg1_prE("      hdFlags: %u\n", flags);**/
	pBB += 4;

	/* Frames - not counting the VBR header (4 bytes) */
	if ((flags & 0x0001) == 1) {
		if (pBB + 4 > pBEnd)
			return ST_ERR_IDAT;
		memcpy(&val, pBB, 4);
#		if (WORDS_BIGENDIAN != 1)
		val = st_sysReverseByteOrder_UI32(val);  /* ENDIAN: BE-->LE */
#		endif
		pVBRhd->aFraCnt = val;
		/**st_mpeg1_prE("      hdFrames: %u\n", val);**/
		pBB += 4;
	}

	/* Bytes - including the VBR header (4 bytes) */
	if ((flags & 0x0002) == 2) {
		if (pBB + 4 > pBEnd)
			return ST_ERR_IDAT;
		memcpy(&val, pBB, 4);
#		if (WORDS_BIGENDIAN != 1)
		val = st_sysReverseByteOrder_UI32(val);  /* ENDIAN: BE-->LE */
#		endif
		pVBRhd->fsz = val;
		/**st_mpeg1_prE("      hdBytes: %u\n", val);**/
		pBB += 4;
	}

	/* TOC (100 bytes), skipped */
	if ((flags & 0x0004) == 4) {
		if (pBB + 100 > pBEnd)
			return ST_ERR_IDAT;
		/**st_mpeg1_prE("      hdTOC: skip\n");**/
		pBB += 100;
	}

	/* Quality (4 bytes) */
	if ((flags & 0x0008) == 8) {
		if (pBB + 4 > pBEnd)
			return ST_ERR_IDAT;
		memcpy(&val, pBB, 4);
#		if (WORDS_BIGENDIAN != 1)
		val = st_sysReverseByteOrder_UI32(val);  /* ENDIAN: BE-->LE */
#		endif
		if (val > 100)
			return ST_ERR_IDAT;
		pVBRhd->qual = val;
		/**st_mpeg1_prE("      hdQual: %u\n", val);**/
	}

	/**if (pBB < pBEnd)
		st_mpeg1_prf("\t\tst_mpeg1_rd2_readVBRheadXing: "
				"buffer not read completely\n\n");**/
	return ST_ERR_SUCC;
#	undef LOC_BUFMAXSZ_
}

/*
 * Read and parse a Fraunhofer VBRI header
 *
 * pBuf      MUST be != NULL
 * pVBRhd    MUST be != NULL
 *
 * returns: ERRCODE
 *          pVBRhd will contain header info
 */
Tst_err
st_mpeg1_rd2_readVBRheadFraun(const Tst_buf *pBuf, const Tst_uint32 bufSz,
		Tst_mpeg1_vbr *pVBRhd)
{
#	define LOC_BUFMINSZ_  (ST_MPEG1_TAGVBR_VBRI_SLEN + 2 + 2 + 2 + 4 + 4 + 2 * 4)
	Tst_buf const *pBB   = pBuf;
	/**Tst_str    str[4 + 1];**/
	Tst_uint32    valUI  = 0,
	              tocSz;
	Tst_uint16    valUS  = 0,
	              entCnt = 0,
	              entSz  = 0;

	ST_ASSERTN_IARG(pBuf == NULL || pVBRhd == NULL)

	if (bufSz < LOC_BUFMINSZ_) {
		/* we need everything till TOC table */
		return ST_ERR_IDAT;
	}

	/* all numeric values are read as BigEndian values */

	/* ID field (4 bytes) */
	if (! st_sysStrcmpN2(ST_B_TRUE, ST_MPEG1_TAGVBR_VBRI_SLEN,
				pBB, ST_MPEG1_TAGVBR_VBRI))
		return ST_ERR_IDAT;
	/**strncpy((char*)str, (char*)pBB, ST_MPEG1_TAGVBR_VBRI_SLEN);
	str[ST_MPEG1_TAGVBR_VBRI_SLEN] = 0;
	st_mpeg1_prE("      hdID: %s\n", (char*)str);**/
	pBB += ST_MPEG1_TAGVBR_VBRI_SLEN;

	/* all values are read in Big-Endian */

	/* Version ID (2 bytes), skip */
	pBB += 2;

	/* Delay as float (2 bytes), skip */
	pBB += 2;

	/* Quality (2 bytes) */
	memcpy(&valUS, pBB, 2);
#	if (WORDS_BIGENDIAN != 1)
	valUS = st_sysReverseByteOrder_UI16(valUS);  /* ENDIAN: BE-->LE */
#	endif
	if (valUS > 100)
		return ST_ERR_IDAT;
	pVBRhd->qual = (Tst_uint32)valUS;
	/**st_mpeg1_prE("      hdQual: %u\n", (Tst_uint32)valUS);**/
	pBB += 2;

	/* Bytes - including the VBR header (4 bytes) */
	memcpy(&valUI, pBB, 4);
#	if (WORDS_BIGENDIAN != 1)
	valUI = st_sysReverseByteOrder_UI32(valUI);  /* ENDIAN: BE-->LE */
#	endif
	pVBRhd->fsz = valUI;
	/**st_mpeg1_prE("      hdBytes: %u\n", valUI);**/
	pBB += 4;

	/* Frames - including the VBR header (4 bytes) */
	memcpy(&valUI, pBB, 4);
#	if (WORDS_BIGENDIAN != 1)
	valUI = st_sysReverseByteOrder_UI32(valUI);  /* ENDIAN: BE-->LE */
#	endif
	pVBRhd->aFraCnt = valUI;
	if (pVBRhd->aFraCnt > 0)
		--pVBRhd->aFraCnt;  /* since FH also counts VBR header frame */
	/**st_mpeg1_prE("      hdFrames: %u\n", valUI);**/
	pBB += 4;

	/* TOC: Number of entries within TOC table (2 bytes) */
	memcpy(&entCnt, pBB, 2);
#	if (WORDS_BIGENDIAN != 1)
	entCnt = st_sysReverseByteOrder_UI16(entCnt);  /* ENDIAN: BE-->LE */
#	endif
	/**st_mpeg1_prE("      hdEntCnt: %u\n", (Tst_uint32)entCnt);**/
	pBB += 2;

	/* TOC: Scale factor of TOC table entries (2 bytes) */
	pBB += 2;

	/* TOC: Size per table entry in bytes, max 4 (2 bytes) */
	memcpy(&entSz, pBB, 2);
#	if (WORDS_BIGENDIAN != 1)
	entSz = st_sysReverseByteOrder_UI16(entSz);  /* ENDIAN: BE-->LE */
#	endif
	if (entSz > 2)
		return ST_ERR_IDAT;
	/**st_mpeg1_prE("      hdEntSz: %u\n", (Tst_uint32)entSz);**/
	pBB += 2;

	/* TOC: Frames per table entry (2 bytes) */
	pBB += 2;

	/* TOC: Entries for seeking (x bytes) */
	tocSz = (Tst_uint32)(entCnt * entSz);
	/**st_mpeg1_prE("      hdTocSz: %u\n", tocSz);**/

	if (pBB + tocSz > pBuf + bufSz) {
		/* buffer not big enough */
		return ST_ERR_IDAT;
	}
	pBB += tocSz;

	/**if (pBB < pBuf + bufSz)
		st_mpeg1_prf("\t\tst_mpeg1_rd2_readVBRheadFraun: "
				"buffer not read completely, %u bytes left\n\n",
				(Tst_uint32)(pBuf + bufSz - pBB));**/
	return ST_ERR_SUCC;
#	undef LOC_BUFMINSZ_
}

/*----------------------------------------------------------------------------*/

/*
 * Find pNeedle in pHay
 *
 * returns: pointer to first occurance of pNeedle or NULL
 */
const Tst_buf*
st_mpeg1_rd2_memfind(const Tst_buf *pHay, const Tst_uint32 haySz,
		const Tst_buf *pNeedle, const Tst_uint32 needleSz)
{
	Tst_buf const *pBB = pHay,
	              *pBEnd;

	ST_ASSERTN_NULL(pHay == NULL || pNeedle == NULL)

	if (haySz == 0 || needleSz == 0 || needleSz > haySz)
		return NULL;

	pBEnd = pBB + haySz;
	for (; pBB + needleSz <= pBEnd; pBB++) {
		if (memcmp(pBB, pNeedle, needleSz) == 0)
			return pBB;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get frame length
 *   frame length includes the length of the frame header
 *
 * pMFH  MUST be set
 *
 * returns: frame length in bytes, using slots of 4 or 1 byte size
 *          (min samplerate=8000, max bitr=448, max paddLen=4, max s/f=144byte
 *           --> max frameSz=8068
 *           max samplerate=48000, min bitr=8, min paddLen=0, min s/f=32byte
 *           --> min frameSz=8)
 */
static void
ST_MPEG1__rd2_getFrameLength(Tst_mpeg1_fhd *pMFH)
{
	double    ret;
	Tst_int32 flr;

	ST_ASSERTN_VOID(pMFH == NULL)

	/* samples per frame */
	ret = (double)st_mpeg1_rd2_getSamplesPerFrame(pMFH->layer, pMFH->audioVers);
	if (ret < 1) {
		pMFH->frameSz = 0;  /* invalid data */
		return;
	}
	/* */
	ret = (ret / (pMFH->layer == ST_MPEG1_LAYER_1 ? 32.0 : 8.0)) *
			(double)pMFH->bitrate * 1000.0;
	ret = ret / (double)pMFH->samplerate;
	flr = ST_MPEG1__rd2_floor(ret);
	if (flr >= 0) {
		pMFH->frameSz = (Tst_uint32)flr *
				(pMFH->layer == ST_MPEG1_LAYER_1 ? 4 : 1) + pMFH->paddLen;
	} else {
		pMFH->frameSz = 0;  /* invalid data */
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Get Samplingrate Frequency in Hz
 *
 * returns: ERRCODE  (ST_ERR_MISF, ST_ERR_IARG, ST_ERR_SUCC)
 */
static Tst_err
ST_MPEG1__rd2_calcSRFI(const Tst_byte byt,
		const Tst_mpeg1_audioVers avers, Tst_uint32 *pSFreq)
{
	Tst_byte byt2 = (byt & 0x0c) >> 2;

	*pSFreq = 0;
	if (byt2 == 0x03)  /* reserved */
		return ST_ERR_MISF;  /* invalid sampl. frequ. */

	switch (avers) {
	case ST_MPEG1_AUDVERS_1:
		if (byt2 == 0x00) *pSFreq = 44100;
		else if (byt2 == 0x01) *pSFreq = 48000;
		else /*if (byt2 == 0x02)*/ *pSFreq = 32000;
		break;
	case ST_MPEG1_AUDVERS_2:
		if (byt2 == 0x00) *pSFreq = 22050;
		else if (byt2 == 0x01) *pSFreq = 24000;
		else /*if (byt2 == 0x02)*/ *pSFreq = 16000;
		break;
	case ST_MPEG1_AUDVERS_25:
		if (byt2 == 0x00) *pSFreq = 11025;
		else if (byt2 == 0x01) *pSFreq = 12000;
		else /*if (byt2 == 0x02)*/ *pSFreq = 8000;
		break;
	default:
		return ST_ERR_IARG;
	}
	return ST_ERR_SUCC;
}

/*
 * Get bitrate in kbit/s
 *
 * returns: ERRCODE  (ST_ERR_MIBI, ST_ERR_IARG, ST_ERR_MICO, ST_ERR_SUCC)
 */
static Tst_err
ST_MPEG1__rd2_calcBRI(const Tst_byte byt,
		const Tst_mpeg1_audioVers avers, const Tst_mpeg1_layer lay,
		Tst_uint32 *pBRate)
{
	Tst_byte byt2 = ((byt & 0xf0) >> 4);

	*pBRate = 0;
	if (byt2 == 0x0f)
		return ST_ERR_MIBI;  /* invalid BitrateIndex */
	if (byt2 == 0x00) {
		*pBRate = 0;  /* Free-Format */
		return ST_ERR_SUCC;
	}
	/* */
	if (avers == ST_MPEG1_AUDVERS_1)
		switch (lay) {
		case ST_MPEG1_LAYER_1:  /* MP1 */
			return ST_MPEG1__rd2_calcBRI_v1_l1(byt2, pBRate);
		case ST_MPEG1_LAYER_2:  /* MP2 */
			return ST_MPEG1__rd2_calcBRI_v1_l2(byt2, pBRate);
		case ST_MPEG1_LAYER_3:  /* MP3 */
			return ST_MPEG1__rd2_calcBRI_v1_l3(byt2, pBRate);
		default:
			break;
		}
	else if (avers == ST_MPEG1_AUDVERS_2 || avers == ST_MPEG1_AUDVERS_25)
		switch (lay) {
		case ST_MPEG1_LAYER_1:  /* MP1 */
			return ST_MPEG1__rd2_calcBRI_v2_25_l1(byt2, pBRate);
		case ST_MPEG1_LAYER_2:  /* MP2 */
		case ST_MPEG1_LAYER_3:  /* MP3 */
			return ST_MPEG1__rd2_calcBRI_v2_25_l2_3(byt2, pBRate);
		default:
			break;
		}
	return ST_ERR_IARG;
}

/*----------------------------------------------------------------------------*/

/*
 * Get Bitrate for MPEG v1, Layer I
 *
 * returns: ERRCODE  (ST_ERR_MICO, ST_ERR_SUCC)
 */
static Tst_err
ST_MPEG1__rd2_calcBRI_v1_l1(const Tst_byte byt, Tst_uint32 *pBRate)
{
	switch (byt) {
		case 0x01: *pBRate = 32; break;
		case 0x02: *pBRate = 64; break;
		case 0x03: *pBRate = 96; break;
		case 0x04: *pBRate = 128; break;
		case 0x05: *pBRate = 160; break;
		case 0x06: *pBRate = 192; break;
		case 0x07: *pBRate = 224; break;
		case 0x08: *pBRate = 256; break;
		case 0x09: *pBRate = 288; break;
		case 0x0a: *pBRate = 320; break;
		case 0x0b: *pBRate = 352; break;
		case 0x0c: *pBRate = 384; break;
		case 0x0d: *pBRate = 416; break;
		case 0x0e: *pBRate = 448; break;
		default  : return ST_ERR_MICO;  /* invalid brate-ver-lay combo */
	}
	return ST_ERR_SUCC;
}

/*
 * Get Bitrate for MPEG v1, Layer II
 *
 * returns: ERRCODE  (ST_ERR_MICO, ST_ERR_SUCC)
 */
static Tst_err
ST_MPEG1__rd2_calcBRI_v1_l2(const Tst_byte byt, Tst_uint32 *pBRate)
{
	switch (byt) {
		case 0x01: *pBRate = 32; break;
		case 0x02: *pBRate = 48; break;
		case 0x03: *pBRate = 56; break;
		case 0x04: *pBRate = 64; break;
		case 0x05: *pBRate = 80; break;
		case 0x06: *pBRate = 96; break;
		case 0x07: *pBRate = 112; break;
		case 0x08: *pBRate = 128; break;
		case 0x09: *pBRate = 160; break;
		case 0x0a: *pBRate = 192; break;
		case 0x0b: *pBRate = 224; break;
		case 0x0c: *pBRate = 256; break;
		case 0x0d: *pBRate = 320; break;
		case 0x0e: *pBRate = 384; break;
		default  : return ST_ERR_MICO;  /* invalid brate-ver-lay combo */
	}
	return ST_ERR_SUCC;
}

/*
 * Get Bitrate for MPEG v1, Layer III
 *
 * returns: ERRCODE  (ST_ERR_MICO, ST_ERR_SUCC)
 */
static Tst_err
ST_MPEG1__rd2_calcBRI_v1_l3(const Tst_byte byt, Tst_uint32 *pBRate)
{
	switch (byt) {
		case 0x01: *pBRate = 32; break;
		case 0x02: *pBRate = 40; break;
		case 0x03: *pBRate = 48; break;
		case 0x04: *pBRate = 56; break;
		case 0x05: *pBRate = 64; break;
		case 0x06: *pBRate = 80; break;
		case 0x07: *pBRate = 96; break;
		case 0x08: *pBRate = 112; break;
		case 0x09: *pBRate = 128; break;
		case 0x0a: *pBRate = 160; break;
		case 0x0b: *pBRate = 192; break;
		case 0x0c: *pBRate = 224; break;
		case 0x0d: *pBRate = 256; break;
		case 0x0e: *pBRate = 320; break;
		default  : return ST_ERR_MICO;  /* invalid brate-ver-lay combo */
	}
	return ST_ERR_SUCC;
}

/*
 * Get Bitrate for MPEG v2/2.5, Layer I
 *
 * returns: ERRCODE  (ST_ERR_MICO, ST_ERR_SUCC)
 */
static Tst_err
ST_MPEG1__rd2_calcBRI_v2_25_l1(const Tst_byte byt, Tst_uint32 *pBRate)
{
	switch (byt) {
		case 0x01: *pBRate = 32; break;
		case 0x02: *pBRate = 48; break;
		case 0x03: *pBRate = 56; break;
		case 0x04: *pBRate = 64; break;
		case 0x05: *pBRate = 80; break;
		case 0x06: *pBRate = 96; break;
		case 0x07: *pBRate = 112; break;
		case 0x08: *pBRate = 128; break;
		case 0x09: *pBRate = 144; break;
		case 0x0a: *pBRate = 160; break;
		case 0x0b: *pBRate = 176; break;
		case 0x0c: *pBRate = 192; break;
		case 0x0d: *pBRate = 224; break;
		case 0x0e: *pBRate = 256; break;
		default  : return ST_ERR_MICO;  /* invalid brate-ver-lay combo */
	}
	return ST_ERR_SUCC;
}

/*
 * Get Bitrate for MPEG v2/2.5, Layer II or III
 *
 * returns: ERRCODE  (ST_ERR_MICO, ST_ERR_SUCC)
 */
static Tst_err
ST_MPEG1__rd2_calcBRI_v2_25_l2_3(const Tst_byte byt, Tst_uint32 *pBRate)
{
	switch (byt) {
		case 0x01: *pBRate = 8; break;
		case 0x02: *pBRate = 16; break;
		case 0x03: *pBRate = 24; break;
		case 0x04: *pBRate = 32; break;
		case 0x05: *pBRate = 40; break;
		case 0x06: *pBRate = 48; break;
		case 0x07: *pBRate = 56; break;
		case 0x08: *pBRate = 64; break;
		case 0x09: *pBRate = 80; break;
		case 0x0a: *pBRate = 96; break;
		case 0x0b: *pBRate = 112; break;
		case 0x0c: *pBRate = 128; break;
		case 0x0d: *pBRate = 144; break;
		case 0x0e: *pBRate = 160; break;
		default  : return ST_ERR_MICO;  /* invalid brate-ver-lay combo */
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Returns next Tst_int32 smaller or equal to val
 */
static ST_INLINE_S
Tst_int32 ST_MPEG1__rd2_floor(const double val)
{
	Tst_int32 fl = (Tst_int32)(val + (double)0.5f);

	if ((double)fl > val)
		--fl;
	return fl;
}

/******************************************************************************/
