/******************************************************************************/
/* cont_ogg_zcod.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for OGG/Flac codec handling                                      */
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
#include "src/includes/common/binobj.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_CONT_OGG_ZRD_COD_C
#include "cont_ogg_zrd_cod-pp.h"
/*** */
#include "cont_ogg_stcs-prot.h"
#include "cont_ogg_zfdeb-prot.h"
#include "../cont_ogg_aud_flac/ac_flac_zrd-prot.h"
#include "../cont_ogg_aud_flac/ac_flac_stcs-prot.h"
#include "../cont_ogg_aud_vorb/ac_vorb_zrd-prot.h"
#include "../cont_ogg_aud_vorb/ac_vorb_stcs-prot.h"
#include "../cont_ogg_aud_spx_clt/ac_spx_clt_stcs-prot.h"
#include "../cont_ogg_aud_spx_clt/ac_spx_clt_zrd-prot.h"
#include "../cont_ogg_vid_drc_theo/vc_drc_theo_zrd-prot.h"
#include "../cont_ogg_met_vorbcomm/mc_vorbcomm_zrd-prot.h"
#include "../cont_ogg_met_skel/mc_skel_zrd-prot.h"
#include "../cont_ogg_met_skel/mc_skel_zstcs-prot.h"
#include "../cont_ogg_vid_drc_theo/vc_drc_theo_stcs-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_OGG_ZRD_COD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse OGG packet
 *   packet data is in pBSI->packCur.pData
 */
Tst_err
st_contOgg_rdcod_parsePackOgg(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
	Tst_err res;

	ST_ASSERTN_IARG(pOpts == NULL || pBSI == NULL)

	st_streamrd_stc_rsetSObj(&pBSI->sobjRdPackBuf);

	res = st_binobj_attachStreamrd(&pBSI->packCur.bdata,
			&pBSI->sobjRdPackBuf, 0);
	if (res == ST_ERR_SUCC) {
		res = ST_CONTOGG__rdcod_parsePack(pOpts, pBSI, &pBSI->sobjRdPackBuf);
		st_binobj_detachStreamrd(&pBSI->packCur.bdata);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac packet
 *   packet data is read from pSObj
 */
Tst_err
st_contOgg_rdcod_parsePackFlac(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj)
{
	Tst_byte packTp = 0;

	ST_ASSERTN_IARG(pOpts == NULL || pBSI == NULL || pSObj == NULL ||
			(pBSI->bsType != ST_CONTOGG_BSTP_NONE &&
				pBSI->bsType != ST_CONTOGG_BSTP_FLAC))

	return ST_CONTOGG__rdcod_pP_flac(pOpts, pBSI, packTp, pSObj);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_contOgg_rdcod_finishFlac(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI)
{
	ST_ASSERTN_IARG(pOpts == NULL || pBSI == NULL ||
			pBSI->bsType != ST_CONTOGG_BSTP_FLAC)

	st_contOgg_flacrd_finish(pOpts, pBSI);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse OGG or Flac packet
 *   OGG packets contain packets/frames of Audio/Video codecs like
 *     Vorbis, Flac or Theora
 *   Flac packets contain only Flac data, of course
 */
static Tst_err
ST_CONTOGG__rdcod_parsePack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj)
{
	Tst_err  res        = ST_ERR_SUCC;
	Tst_byte packTp     = 0;
	Tst_bool foundCodHd = ST_B_FALSE;

	/* parse packet-type and ID */
	if (pBSI->isSrcOggOrFlac)
		res = ST_CONTOGG__rdcod_pP_codecHead(pOpts, pBSI, pSObj,
				&packTp, &foundCodHd);

	/* parse packet */
	if (res == ST_ERR_SUCC)
		res = ST_CONTOGG__rdcod_pP_sub(pOpts, pBSI, pSObj, packTp, foundCodHd);

	return res;
}

/*
 * Parse codec packet-type and ID
 */
static Tst_err
ST_CONTOGG__rdcod_pP_codecHead(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj,
		Tst_byte *pPackTp, Tst_bool *pFound)
{
	Tst_err res;
	Tst_buf cmpB[16];

	*pPackTp = 0;
	*pFound  = ST_B_FALSE;

	/** PACKTYPE, needed for some codecs */
	res = st_streamrd_rdAheadByte(pSObj, 8, pPackTp);
	if (res != ST_ERR_SUCC)
		return res;

	/** check Ogg-Mapping CODEC ID */
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'fishead\0' */
		memcpy(cmpB, "fishead\0", 8);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_SKEL;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_VORB ||
			pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* '\x??vorbis' */
		cmpB[0] = *pPackTp;
		memcpy(&cmpB[1], "vorbis", 6);
		if (st_streamrd_rdMemcmpBytes(pSObj, 7, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 7, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_VORB;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* '\x7fFLAC', only first packet of OGG-Flac contains this */
		memcpy(cmpB, "\0FLAC", 5);
		cmpB[0] = 0x7f;
		if (st_streamrd_rdMemcmpBytes(pSObj, 5, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 5, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_FLAC;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_SPEE ||
			pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'Speex   ' */
		memcpy(cmpB, "Speex   ", 8);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_SPEE;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'CELT    ' */
		memcpy(cmpB, "CELT    ", 8);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_CELT;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_THEO ||
			pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* '\x??theora' */
		cmpB[0] = *pPackTp;
		memcpy(&cmpB[1], "theora", 6);
		if (st_streamrd_rdMemcmpBytes(pSObj, 7, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 7, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_THEO;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_DIRA ||
			pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'BBCD' */
		memcpy(cmpB, "BBCD", 4);
		if (st_streamrd_rdMemcmpBytes(pSObj, 4, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 4, ST_B_TRUE);
			/** PACKTYPE */
			res = st_streamrd_rdByte(pSObj, 8, pPackTp);
			if (res != ST_ERR_SUCC)
				return res;
			/** */
			pBSI->bsType = ST_CONTOGG_BSTP_DIRA;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'YUV4MPEG' */
		memcpy(cmpB, "YUV4MPEG", 8);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_YUV;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'PCM     ' */
		memcpy(cmpB, "PCM     ", 8);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_PCM;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'OggMIDI\0' */
		memcpy(cmpB, "OggMIDI\0", 8);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_MIDI;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* 'CMML\0\0\0\0' */
		memcpy(cmpB, "CMML\0\0\0\0", 8);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_CMML;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* '\x8bJNG\r\n\032\n' */
		cmpB[0] = 0x8b;
		memcpy(&cmpB[1], "JNG", 3);
		cmpB[4] = '\r';
		cmpB[5] = '\n';
		cmpB[6] = 0x1a;
		cmpB[7] = '\n';
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_JNG;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* '\x8aMNG\r\n\032\n' */
		cmpB[0] = 0x8a;
		memcpy(&cmpB[1], "MNG", 3);
		cmpB[4] = '\r';
		cmpB[5] = '\n';
		cmpB[6] = 0x1a;
		cmpB[7] = '\n';
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_MNG;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* '\x89PNG\r\n\032\n' */
		cmpB[0] = 0x89;
		memcpy(&cmpB[1], "PNG", 3);
		cmpB[4] = '\r';
		cmpB[5] = '\n';
		cmpB[6] = 0x1a;
		cmpB[7] = '\n';
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_PNG;
			*pFound      = ST_B_TRUE;
		}
	}
	if (pBSI->bsType == ST_CONTOGG_BSTP_NONE) {
		/* '\x80kate\0\0\0' */
		memset(cmpB, 0, 8);
		cmpB[0] = 0x80;
		memcpy(&cmpB[1], "kate", 4);
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, cmpB)) {
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			pBSI->bsType = ST_CONTOGG_BSTP_KATE;
			*pFound      = ST_B_TRUE;
		}
	}

	return ST_ERR_SUCC;
}

/*
 * Find next codec header
 *   Used for: Vorbis and Theora
 *   (Dirac uses a similiar algo)
 */
static Tst_err
ST_CONTOGG__rdcod_pP_findNextCodecHd(const char *pFnc,
		const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj,
		Tst_byte *pPackTp, Tst_bool *pFoundCodHd2)
{
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 skipd = 0;
	Tst_byte   byt   = 0;
	Tst_fsize  rem;

	*pFoundCodHd2 = ST_B_FALSE;

	rem = st_streamrd_getAmountRemainingBytes(pSObj);
	do {
		res = ST_CONTOGG__rdcod_pP_codecHead(pOpts, pBSI, pSObj,
				pPackTp, pFoundCodHd2);
		if (res != ST_ERR_SUCC)
			continue;  /* --> end while() */
		if (! *pFoundCodHd2) {
			st_streamrd_rdByte(pSObj, 8, &byt);
			if (byt != 0)
				++skipd;
			if (rem < 7)
				break;
			--rem;
		}
	} while (! *pFoundCodHd2 && ! st_streamrd_isEOF(pSObj));
	if (res == ST_ERR_SUCC && *pFoundCodHd2 && skipd > 0) {
		if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts))
			st_contOgg_d_debOrAnaBS3warn(pOpts, pBSI, pFnc,
					"codec-header in packet is at wrong position "
					"(d %u bytes)", skipd);
		++pBSI->errorCnt;
	}
	return res;
}

/*
 * Parse OGG or Flac packet
 *   OGG packets contain packets/frames of Audio/Video/... codecs like
 *     Vorbis, Flac or Theora
 *   Flac packets contain only Flac data, of course
 */
static Tst_err
ST_CONTOGG__rdcod_pP_sub(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj,
		const Tst_byte packTp, const Tst_bool foundCodHd)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err res = ST_ERR_SUCC;
	Tst_contOgg_bsType bsTp = pBSI->bsType;

	if (! pBSI->isSrcOggOrFlac)
		bsTp = ST_CONTOGG_BSTP_FLAC;

	/**if (pBSI->isSrcOggOrFlac && ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
				"found codec header: %c", (foundCodHd ? 'y' : 'n'));**/

	switch (bsTp) {
	/* audio */
	case ST_CONTOGG_BSTP_VORB:
		res = ST_CONTOGG__rdcod_pP_vorbis(pOpts, pBSI, packTp, pSObj, foundCodHd);
		break;
	case ST_CONTOGG_BSTP_FLAC:
		res = ST_CONTOGG__rdcod_pP_flac(pOpts, pBSI, packTp, pSObj);
		break;
	case ST_CONTOGG_BSTP_SPEE:
		res = ST_CONTOGG__rdcod_pP_speex(pOpts, pBSI, 0, pSObj);
		break;
	case ST_CONTOGG_BSTP_CELT:
		res = ST_CONTOGG__rdcod_pP_celt(pOpts, pBSI, packTp, pSObj);
		break;
	case ST_CONTOGG_BSTP_PCM:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"PCM packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	case ST_CONTOGG_BSTP_MIDI:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"MIDI packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	/* video */
	case ST_CONTOGG_BSTP_THEO:
		res = ST_CONTOGG__rdcod_pP_theora(pOpts, pBSI, packTp, pSObj, foundCodHd);
		break;
	case ST_CONTOGG_BSTP_DIRA:
		res = ST_CONTOGG__rdcod_pP_dirac(pOpts, pBSI, packTp, pSObj, foundCodHd);
		break;
	case ST_CONTOGG_BSTP_YUV:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"YUV4MPEG packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	/* Text */
	case ST_CONTOGG_BSTP_CMML:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"CMML packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	case ST_CONTOGG_BSTP_KATE:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Kate packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	/* Picture/Animation */
	case ST_CONTOGG_BSTP_JNG:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"JNG packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	case ST_CONTOGG_BSTP_PNG:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"PNG packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	case ST_CONTOGG_BSTP_MNG:
		if (pBSI->packCur.nr == 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"MNG packet found");
		} else
			++pBSI->codecNonHeadPckCnt;
		break;
	/* meta */
	case ST_CONTOGG_BSTP_SKEL:
		res = ST_CONTOGG__rdcod_pP_skeleton(pOpts, pBSI, 0, pSObj);
		break;
	/* */
	default:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
					"unknown codec, skipping packet");
		++pBSI->errorCnt;
	}

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Skeleton packet
 */
static Tst_err
ST_CONTOGG__rdcod_pP_skeleton(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_byte packTp, Tst_streamrd *pSObj)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err  res     = ST_ERR_SUCC;
	Tst_bool boneFnd = ST_B_FALSE;

	if (pBSI->pSkel == NULL) {
		ST_CALLOC(pBSI->pSkel, Tst_contOgg_skeleton*,
				1, sizeof(Tst_contOgg_skeleton))
		if (pBSI->pSkel == NULL)
			return ST_ERR_OMEM;
		st_contOgg_stc_initSkel(pBSI->pSkel);
	}

	do {
		if (st_streamrd_rdMemcmpBytes(pSObj, 8, "fisbone\0")) {
			boneFnd = ST_B_TRUE;
			st_streamrd_rdSkipBytes(pSObj, 8, ST_B_TRUE);
			res = st_contOgg_skelrd_parseBone(pOpts, pBSI, pSObj);
		} else if (! boneFnd && ! pBSI->pSkel->ident.isSet) {
			res = st_contOgg_skelrd_parseHPack(pOpts, pBSI, pSObj,
					ST_CONTOGG__rdcod_stripStr);
		} else {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Skeleton header remaining bytes "ST_TFSIZE_PRF_LU,
						(Tst_fsize)st_streamrd_getAmountRemainingBytes(pSObj));
			break;
		}
	} while (res == ST_ERR_SUCC && ! st_streamrd_isEOF(pSObj));
	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Vorbis packet
 */
static Tst_err
ST_CONTOGG__rdcod_pP_vorbis(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_byte packTp, Tst_streamrd *pSObj,
		const Tst_bool foundCodHd)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err    res          = ST_ERR_SUCC;
	Tst_bool   stopThen     = ST_B_FALSE,
	           rdCodID      = ST_B_FALSE,
	           fndCodHd2    = foundCodHd,
	           //isPureHeader = ST_B_TRUE,
	           //isPureAudio  = ST_B_TRUE,
	           isCmtSet;
	Tst_uint32 segsxPP      = 0,
	           outSzPP      = 0;
	Tst_buf    codHdBuf[7];
	Tst_streamrd *pSObjPP;

	if (pBSI->pVorb == NULL) {
		ST_CALLOC(pBSI->pVorb, Tst_contOgg_vorbis*,
				1, sizeof(Tst_contOgg_vorbis))
		if (pBSI->pVorb == NULL)
			return ST_ERR_OMEM;
		res = st_contOgg_stc_initVorb(pBSI->pVorb);
		if (res != ST_ERR_SUCC)
			return res;
		/* initialize decoder */
		res = st_contOgg_vorbrd_initDecoder(pOpts, pBSI);
		if (res != ST_ERR_SUCC)
			return res;
		/* init PCM client */
		res = ST_CONTOGG__rdcod_initPCMclient(pOpts, LOC_FNCN_, pBSI,
				&pBSI->pVorb->decObj.pPCMstrHnd);
		if (res != ST_ERR_SUCC)
			return res;
	}
	codHdBuf[0] = packTp;
	memcpy(&codHdBuf[1], "vorbis", 6);

	/* */
	do {
		pSObjPP = pSObj;
		/* parse packet-type and ID */
		if (rdCodID) {
			res = ST_CONTOGG__rdcod_pP_findNextCodecHd(LOC_FNCN_, pOpts, pBSI,
					pSObj, &packTp, &fndCodHd2);
			if (res != ST_ERR_SUCC)
				return res;
		}
		/* */
		if (fndCodHd2 && (packTp & 0x01) != 0) {
			//if (pBSI->pVorb->decObj.decToPCM) {
			//	isPureAudio = ST_B_FALSE;
			//}
			/* */
			if (pBSI->pVorb->decObj.decToPCM) {
				codHdBuf[0] = packTp;
				/* if header doesn't contain the vorbis comment
				 *   then we copy the header packet
				 *   to pBSI->pVorb->decObj.pDBuf */
				res = ST_CONTOGG__rdcod_readPartialPack(&pBSI->packCur,
						&segsxPP, &outSzPP, packTp != 0x03, pSObj,
						codHdBuf, sizeof(codHdBuf),
						&pBSI->pVorb->decObj.pDBuf,
						&pBSI->pVorb->decObj.dbufSz,
						&pBSI->pVorb->decObj.dbufUsd);
				if (res == ST_ERR_SUCC) {
					if (packTp == 0x03) {
						/* we feed the decoder a minimalistic
						 *   vorbis comment to save time and RAM  */
						res = ST_CONTOGG__rdcod_getBogusVCmt(codHdBuf,
								sizeof(codHdBuf), ST_B_TRUE,
								&pBSI->pVorb->decObj.pDBuf,
								&pBSI->pVorb->decObj.dbufSz,
								&pBSI->pVorb->decObj.dbufUsd);
						outSzPP = pBSI->pVorb->decObj.dbufUsd;
					} else if (pBSI->pVorb->decObj.dbufUsd > 0) {
						pSObjPP = &pBSI->pVorb->decObj.decStrrd;
						/* */
						ST_ASSERTN_NUM(ST_ERR_FAIL,
								pBSI->pVorb->decObj.pDBuf == NULL ||
								pBSI->pVorb->decObj.dbufUsd < 8)
						res = st_streamrd_setInput_buffer(pSObjPP,
								&pBSI->pVorb->decObj.pDBuf[7],
								pBSI->pVorb->decObj.dbufUsd - 7);
					}
				}
			}
			/* */
			if (res == ST_ERR_SUCC) {
				/* parse header packet (source is pSObjPP) */
				res = st_contOgg_vorbrd_parseHPack(pOpts, pBSI, packTp,
						pSObjPP, &stopThen);
			}
			if (res == ST_ERR_SUCC && pBSI->pVorb->decObj.decToPCM) {
				/* feed header packet to decoder
				 *   (source is pBSI->pVorb->decObj.pDBuf)  */
				res = st_contOgg_vorbrd_feedDecoderHeader(pOpts, pBSI);
			}
		} else {
			//isPureHeader = ST_B_FALSE;
			/* */
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"vorb audio packet");**/
			/* */
			if (pBSI->pVorb->decObj.decToPCM) {
				do {
					res = ST_CONTOGG__rdcod_readPartialPack(&pBSI->packCur,
							&segsxPP, &outSzPP, ST_B_TRUE, pSObj,
							NULL, 0,
							&pBSI->pVorb->decObj.pDBuf,
							&pBSI->pVorb->decObj.dbufSz,
							&pBSI->pVorb->decObj.dbufUsd);
					if (res == ST_ERR_SUCC && pBSI->pVorb->decObj.dbufUsd > 0)
						res = st_contOgg_vorbrd_feedDecoderAudio(pOpts, pBSI);
				} while (outSzPP > 0);
			}
			/* */
			++pBSI->pVorb->aPackCnt;
			++pBSI->codecNonHeadPckCnt;
			pBSI->codecNonHeadSize += st_streamrd_getAmountRemainingBytes(pSObj);
			/* don't parse packet any further */
			stopThen = ST_B_TRUE;
			continue;
		}
		/* */
		fndCodHd2 = ST_B_FALSE;
		isCmtSet  = st_contOgg_vorbComm_gs_getRC_isSet(&pBSI->pVorb->comm);
		rdCodID   = ! (pBSI->pVorb->ident.isSet && isCmtSet &&
					pBSI->pVorb->setup.isSet) &&
				(pBSI->codecNonHeadPckCnt == 0);
	} while (res == ST_ERR_SUCC && ! stopThen && ! st_streamrd_isEOF(pSObj));

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac packet
 */
static Tst_err
ST_CONTOGG__rdcod_pP_flac(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_byte packTp, Tst_streamrd *pSObj)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err  res      = ST_ERR_SUCC;
	Tst_bool stopThen = ST_B_FALSE;

	if (pBSI->pFlac == NULL) {
		ST_CALLOC(pBSI->pFlac, Tst_contOgg_flac*,
				1, sizeof(Tst_contOgg_flac))
		if (pBSI->pFlac == NULL)
			return ST_ERR_OMEM;
		res = st_contOgg_stc_initFlac(pBSI->pFlac);
		if (res != ST_ERR_SUCC)
			return res;
		/* init PCM client */
		res = ST_CONTOGG__rdcod_initPCMclient(pOpts, LOC_FNCN_, pBSI,
				&pBSI->pFlac->decObj.pPCMstrHnd);
		if (res != ST_ERR_SUCC)
			return res;
	}

	do {
		/** PACKTYPE */
		if (! pBSI->isSrcOggOrFlac || pBSI->packCur.nr > 1) {
			/* every packet has a 'packet-type' byte */
			res = st_streamrd_rdAheadByte(pSObj, 8, &packTp);
			if (res != ST_ERR_SUCC)
				continue;  /* --> end while() */
		}

		if ((! pBSI->isSrcOggOrFlac && pBSI->packCur.nr == 1 && packTp == 'f') ||
				(/*packTp == 0x7f ||...*/ packTp != 0xff)) {
			/* pure flac files begin with 'fLaC' */
			res = st_contOgg_flacrd_parseHPack(pOpts, pBSI, packTp, pSObj);
		} else {
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"flac audio packet");**/
			if (pBSI->isSrcOggOrFlac)
				pBSI->codecNonHeadSize +=
						st_streamrd_getAmountRemainingBytes(pSObj);
			else if (pBSI->codecNonHeadPckCnt == 0)
				pBSI->flacOffsFirstAudPkt = st_streamrd_getCurPos(pSObj);
			/* */
			res = st_contOgg_flacrd_parseAPack(pOpts, pBSI, packTp, pSObj);
			/* */
			++pBSI->pFlac->aPackCnt;
			++pBSI->codecNonHeadPckCnt;
			/* don't parse packet any further */
			stopThen = ST_B_TRUE;
			continue;
		}
		if (! pBSI->isSrcOggOrFlac)
			stopThen = ST_B_TRUE;
	} while (res == ST_ERR_SUCC && ! stopThen && ! st_streamrd_isEOF(pSObj));

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Speex packet
 */
static Tst_err
ST_CONTOGG__rdcod_pP_speex(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_byte packTp, Tst_streamrd *pSObj)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err  res      = ST_ERR_SUCC;
	Tst_bool stopThen = ST_B_FALSE;

	if (pBSI->pSpee == NULL) {
		ST_CALLOC(pBSI->pSpee, Tst_contOgg_speex*,
				1, sizeof(Tst_contOgg_speex))
		if (pBSI->pSpee == NULL)
			return ST_ERR_OMEM;
		st_contOgg_stc_initSpee(pBSI->pSpee);
	}

	do {
		if (pBSI->packCur.nr == 1)
			res = st_contOgg_spxrd_parseHPack(pOpts, pBSI, pSObj,
					ST_CONTOGG__rdcod_stripStr);
		else if (pBSI->packCur.nr == 2) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Speex comment header found");
			res = st_contOgg_vorbComm_parse(pBSI, pSObj,
					&pBSI->pSpee->comm,
					/*hasFramingBit:*/ST_B_FALSE, /*hasMaxSz:*/ST_B_FALSE,
					/*maxSz:*/0,
					/*srcCmtBegPos:*/(Tst_int32)st_streamrd_getCurPos(pSObj));
		} else if (pBSI->pSpee->ident.extraHeaders > 0 &&
				pBSI->packCur.nr <= 2 + pBSI->pSpee->ident.extraHeaders) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Speex extra header found but skipped");
			/* don't parse packet any further */
			stopThen = ST_B_TRUE;
		} else {
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Speex audio packet");**/
			++pBSI->pSpee->aPackCnt;
			++pBSI->codecNonHeadPckCnt;
			pBSI->codecNonHeadSize += st_streamrd_getAmountRemainingBytes(pSObj);
			/* don't parse packet any further */
			stopThen = ST_B_TRUE;
		}
	} while (res == ST_ERR_SUCC && ! stopThen && ! st_streamrd_isEOF(pSObj));

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Celt packet
 */
static Tst_err
ST_CONTOGG__rdcod_pP_celt(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_byte packTp, Tst_streamrd *pSObj)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err  res      = ST_ERR_SUCC;
	Tst_bool stopThen = ST_B_FALSE;

	if (pBSI->pCelt == NULL) {
		ST_CALLOC(pBSI->pCelt, Tst_contOgg_celt*,
				1, sizeof(Tst_contOgg_celt))
		if (pBSI->pCelt == NULL)
			return ST_ERR_OMEM;
		st_contOgg_stc_initCelt(pBSI->pCelt);
	}

	do {
		if (pBSI->packCur.nr == 1)
			res = st_contOgg_celtrd_parseHPack(pOpts, pBSI, pSObj,
					ST_CONTOGG__rdcod_stripStr);
		else if (pBSI->packCur.nr == 2) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Celt comment header found");
			res = st_contOgg_vorbComm_parse(pBSI, pSObj,
					&pBSI->pCelt->comm,
					/*hasFramingBit:*/ST_B_FALSE, /*hasMaxSz:*/ST_B_FALSE,
					/*maxSz:*/0,
					/*srcCmtBegPos:*/(Tst_int32)st_streamrd_getCurPos(pSObj));
		} else if (pBSI->pCelt->ident.extraHeaders > 0 &&
				pBSI->packCur.nr <= 2 + pBSI->pCelt->ident.extraHeaders) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Celt extra header found but skipped");
			/* don't parse packet any further */
			stopThen = ST_B_TRUE;
		} else {
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"celt audio packet");**/
			++pBSI->pCelt->aPackCnt;
			++pBSI->codecNonHeadPckCnt;
			pBSI->codecNonHeadSize += st_streamrd_getAmountRemainingBytes(pSObj);
			/* don't parse packet any further */
			stopThen = ST_B_TRUE;
		}
	} while (res == ST_ERR_SUCC && ! stopThen && ! st_streamrd_isEOF(pSObj));
	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Theora packet
 */
static Tst_err
ST_CONTOGG__rdcod_pP_theora(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_byte packTp, Tst_streamrd *pSObj,
		const Tst_bool foundCodHd)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err    res       = ST_ERR_SUCC;
	Tst_bool   stopThen  = ST_B_FALSE,
	           rdCodID   = ST_B_FALSE,
	           fndCodHd2 = foundCodHd,
	           isCmtSet;
	Tst_uint64 lkf64;

	if (pBSI->pTheo == NULL) {
		ST_CALLOC(pBSI->pTheo, Tst_contOgg_theora*,
				1, sizeof(Tst_contOgg_theora))
		if (pBSI->pTheo == NULL)
			return ST_ERR_OMEM;
		st_contOgg_stc_initTheo(pBSI->pTheo);
	}

	do {
		/* parse packet-type and ID */
		if (rdCodID) {
			res = ST_CONTOGG__rdcod_pP_findNextCodecHd(LOC_FNCN_, pOpts, pBSI,
					pSObj, &packTp, &fndCodHd2);
			if (res != ST_ERR_SUCC)
				return res;
		}
		/* */
		if (fndCodHd2 && (packTp & 0x80) != 0) {
			res = st_contOgg_theord_parseHPack(pOpts, pBSI, packTp, pSObj,
					&stopThen);
		} else {
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Theora video packet");**/
			++pBSI->pTheo->vPackCnt;
			++pBSI->codecNonHeadPckCnt;
			pBSI->codecNonHeadSize += st_streamrd_getAmountRemainingBytes(pSObj);

			/* shift granulePos to get frame index */
			/**st_contOgg_prf("KfgSh: %u\n", pBSI->pTheo->ident.kfgshift);
			st_contOgg_prf64("GraP: ", &pBSI->pFHdBeg->granulePos);**/
			if (st_sysUInt64_cmpLoHi(&pBSI->pFHdBeg->granulePos,
						(Tst_uint32)-1, (Tst_uint32)-1) < 0) {
				/* */
				ST_SYSMATH_STC_RSETUI64(lkf64)
				st_sysUInt64_setUI64(&lkf64,
						&pBSI->pFHdBeg->granulePos);
				st_sysUInt64_shiftLeft(&lkf64,
						64 - pBSI->pTheo->ident.kfgshift);
				st_sysUInt64_shiftRight(&lkf64,
						64 - pBSI->pTheo->ident.kfgshift);
				/**st_contOgg_prf64("LKF : ", &lkf64);**/
				st_sysUInt64_shiftRight(&pBSI->pFHdBeg->granulePos,
						pBSI->pTheo->ident.kfgshift);
				/**st_contOgg_prf64("FrIx: ", &pBSI->pFHdBeg->granulePos);**/
				st_sysUInt64_setUI64(&pBSI->pTheo->maxFrIx,
						&pBSI->pFHdBeg->granulePos);
			}
			/**st_contOgg_prf("\n");**/

			/* don't parse packet any further */
			stopThen = ST_B_TRUE;
			continue;
		}
		/* */
		fndCodHd2 = ST_B_FALSE;
		isCmtSet  = st_contOgg_vorbComm_gs_getRC_isSet(&pBSI->pTheo->comm);
		rdCodID   = ! (pBSI->pTheo->ident.isSet && isCmtSet &&
					pBSI->pTheo->isSetupSet) &&
				(pBSI->codecNonHeadPckCnt == 0);
	} while (res == ST_ERR_SUCC && ! stopThen && ! st_streamrd_isEOF(pSObj));

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Dirac packet
 */
static Tst_err
ST_CONTOGG__rdcod_pP_dirac(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_byte packTp, Tst_streamrd *pSObj,
		const Tst_bool foundCodHd)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err    res       = ST_ERR_SUCC;
	Tst_bool   rdCodID   = ST_B_FALSE,
	           fndCodHd2 = foundCodHd;
	Tst_byte   byt       = 0;
	Tst_uint32 skipd;
	Tst_int32  sizeI     = 0;
	Tst_fsize  rem,
	           abr;

	if (pBSI->pDira == NULL) {
		ST_CALLOC(pBSI->pDira, Tst_contOgg_dirac*,
				1, sizeof(Tst_contOgg_dirac))
		if (pBSI->pDira == NULL)
			return ST_ERR_OMEM;
		st_contOgg_stc_initDira(pBSI->pDira);
	}

	/**if (fndCodHd2)
		st_contOgg_prf("dirac: pcktp+ %u\n", packTp);
	else
		st_contOgg_prf("dirac: pcktp ##\n");**/

	/*
	 * One header is at least 13 bytes big:
	 *   'BBCD' (4byt) + packTp (1byt) + frameSz (4byt) + last offset (4byt)
	 */
	do {
		/* parse packet-type and ID */
		skipd = 0;
		if (! fndCodHd2) {
			rem = st_streamrd_getAmountRemainingBytes(pSObj);
			do {
				if (rem >= 13 && rdCodID) {
					/* every packet has a 'codec-header' */
					res = ST_CONTOGG__rdcod_pP_codecHead(pOpts, pBSI, pSObj,
							&packTp, &fndCodHd2);
					if (res != ST_ERR_SUCC)
						continue;  /* --> end while() */
				}
				if (! fndCodHd2) {
					st_streamrd_rdByte(pSObj, 8, &byt);
					if (byt != 0)
						++skipd;
					if (rem < 13)
						break;
					--rem;
				}
			} while (! fndCodHd2 && ! st_streamrd_isEOF(pSObj));
			if (! fndCodHd2) {
				if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
							"codec header missing in Dirac packet");
				++pBSI->errorCnt;
				res = ST_ERR_IDAT;
				continue;
			}
			if (skipd > 1) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_P(pOpts))
					st_contOgg_d_debOrAnaBS3warn(pOpts, pBSI, LOC_FNCN_,
							"codec-header in packet is at wrong position "
							"(d %u bytes)", skipd);
				++pBSI->errorCnt;
			}
			/**st_contOgg_prf("dirac: pcktp- %u\n", packTp);**/
		}

		/* get 'n check data frame size
		 *   frame size excludes 'BBCD' + packTp + frameSz  */
		/** */
		res = st_streamrd_rdInt32(pSObj,
				ST_STREAMRD_IEND_BE, 32, &sizeI);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			break;
		/** */
		if (sizeI == 0)
			sizeI = 13;
		else if (sizeI < 13 || sizeI > 16 * 1024 * 1024) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"invalid data frame size %d", sizeI);
			++pBSI->errorCnt;
			res = ST_ERR_IDAT;
			continue;
		}
		sizeI -= 13;
		/**st_contOgg_prf("dirac: sizeI %d, rem "ST_TFSIZE_PRF_LU"\n",
					sizeI + 4,
					(Tst_fsize)st_streamrd_getAmountRemainingBytes(pSObj));**/

		/** skip last_offset */
		st_streamrd_rdSkipBytes(pSObj, 4, ST_B_TRUE);
		/** */
		rem = st_streamrd_getAmountRemainingBytes(pSObj);
		if ((Tst_fsize)sizeI > rem) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"frame size %d > "ST_TFSIZE_PRF_LU,
						sizeI, (Tst_fsize)rem);
			++pBSI->errorCnt;
			sizeI = (Tst_int32)rem;
		}
		/** sizeI now contains the amount of remaining bytes in this frame */

		/* */
		if (fndCodHd2 && packTp == 0x00) {
			if (pBSI->pDira->ident.isSet && pBSI->packCur.nr > 2) {
				/** dirac should but doesn't have a second
				 **   ident header, which is only a copy of the first
				 **   I even found that the header is repeated every x packets */
				/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
							"Dirac ident header already read, reading again");
				++pBSI->errorCnt;**/
			}
			/* */
			abr = st_streamrd_getAmountBytesRead(pSObj);
			/* */
			res = st_contOgg_drcrd_parseHPack(pOpts, pBSI, packTp, pSObj);
			/**st_contOgg_prf("ST_CONTOGG__rdcod_pP_dirac remSz "ST_TFSIZE_PRF_LU"\n",
					(Tst_fsize)st_streamrd_getAmountRemainingBytes(pSObj));**/

			if (res == ST_ERR_SUCC) {
				/* update remaining frame size */
				abr    = st_streamrd_getAmountBytesRead(pSObj) - abr;
				sizeI -= (Tst_int32)abr;
			}
		} else if (fndCodHd2 && packTp == 0x10) {
			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Dirac end-of-sequence packet found");
			++pBSI->codecNonHeadPckCnt;
		} else {
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS3(pOpts, pBSI, LOC_FNCN_,
						"Dirac video packet");**/
			++pBSI->pDira->vPackCnt;
			++pBSI->codecNonHeadPckCnt;
			pBSI->codecNonHeadSize += st_streamrd_getAmountRemainingBytes(pSObj);
		}
		/* if some bytes of this frame are left, skip them... */
		if (sizeI > 0)
			st_streamrd_rdSkipBytes(pSObj, (Tst_uint32)sizeI, ST_B_TRUE);
		/* */
		fndCodHd2 = ST_B_FALSE;
		rdCodID   = ST_B_TRUE;
	} while (res == ST_ERR_SUCC && ! st_streamrd_isEOF(pSObj));
	/**st_contOgg_prf("\n");**/

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_CONTOGG__rdcod_stripStr(Tst_str *pStr, const Tst_uint32 len)
{
	Tst_uint32 x = len;

	while (x > 0 && pStr[x - 1] == ' ') {
		pStr[x - 1] = 0;
		--x;
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Read partial packet from pSObjIn
 *   If copyData==ST_B_TRUE then the partial packet will be
 *     copied to *ppBuf and *pBufUsd will be equal to *pOutBytes
 *   To determine the size of the part. pack the segment table
 *     from pCurPack will be used. *pSegsX is the current
 *     position in the segment table
 */
static Tst_err
ST_CONTOGG__rdcod_readPartialPack(Tst_contOgg_packet *pCurPack,
		Tst_uint32 *pSegsX, Tst_uint32 *pOutBytes,
		const Tst_bool copyData, Tst_streamrd *pSObjIn,
		const Tst_buf *pCodecHeadBuf, const Tst_uint32 codHdSz,
		Tst_buf **ppBuf, Tst_uint32 *pBufSz, Tst_uint32 *pBufUsd)
{
	Tst_err res = ST_ERR_SUCC;

	ST_ASSERTN_IARG(pCurPack == NULL || pSegsX == NULL || pOutBytes == NULL ||
			(copyData &&
				(pSObjIn == NULL || ppBuf == NULL ||
					pBufSz == NULL || pBufUsd == NULL)))

	*pOutBytes = 0;
	while (*pSegsX < pCurPack->segsUsd) {
		*pOutBytes += pCurPack->pSegs[*pSegsX];
		if (pCurPack->pSegs[(*pSegsX)++] != 255)
			break;
	}
	if (copyData) {
		Tst_uint32 addSz = (pCodecHeadBuf == NULL ? 0 : codHdSz);

		*pBufUsd = 0;
		if (*pOutBytes < addSz)
			*pOutBytes = 0;
		else if (*pOutBytes > *pBufSz)
			res = st_contOgg_stc_reszBuf(ppBuf, pBufSz, *pOutBytes);
		if (res == ST_ERR_SUCC && *pOutBytes > 0) {
			if (addSz > 0)
				memcpy(*ppBuf, pCodecHeadBuf, addSz);
			res = st_streamrd_rdBuffer_noCRC(pSObjIn, *pOutBytes - addSz,
					&(*ppBuf)[addSz], pBufUsd);
			if (res == ST_ERR_SUCC)
				*pBufUsd += addSz;
		} else
			*pBufUsd = 0;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Set *ppBuf to bogus vorbis comment
 */
static Tst_err
ST_CONTOGG__rdcod_getBogusVCmt(const Tst_buf *pCodecHeadBuf,
		const Tst_uint32 codHdSz, const Tst_bool withFramingBit,
		Tst_buf **ppBuf, Tst_uint32 *pBufSz, Tst_uint32 *pBufUsd)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 ui32;
	Tst_buf    *pBufP;

	ST_ASSERTN_IARG(pCodecHeadBuf == NULL ||
			ppBuf == NULL || pBufSz == NULL || pBufUsd == NULL ||
			(codHdSz > 0 && pCodecHeadBuf == NULL))

	*pBufUsd = 0;
	pBufP    = *ppBuf;

	if (13 + codHdSz + 1 > *pBufSz)
		res = st_contOgg_stc_reszBuf(ppBuf, pBufSz, 13 + codHdSz + 1);

	if (res == ST_ERR_SUCC) {
		/* codec header */
		if (codHdSz > 0) {
			memcpy(pBufP, pCodecHeadBuf, codHdSz);
			pBufP += codHdSz;
		}
		/* VENDOR LENGTH */
		ui32 = 5;
#		if (WORDS_BIGENDIAN == 1)
		ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#		endif
		memcpy(pBufP, &ui32, 4);
		pBufP += 4;
		/* VENDOR */
		memcpy(pBufP, "dummy", 5);
		pBufP += 5;
		/* ELEMENT COUNT */
		memset(pBufP, 0, 4);
		pBufP += 4;
		/* */
		*pBufUsd = 13 + codHdSz;
		/* FRAMING BIT */
		if (withFramingBit) {
			*pBufP = 0x01;
			++(*pBufUsd);
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Init PCM client
 */
static Tst_err
ST_CONTOGG__rdcod_initPCMclient(const Tst_contOgg_opts *pOpts,
		const char *pFnc, Tst_contOgg_substr_intn *pBSI,
		void **ppPCMstrHnd)
{
	Tst_err res = ST_ERR_SUCC;

	if (pOpts->isPCMclientSet) {
		res = pOpts->cbPCMprepareWriting(pOpts->pPCMclientObj,
				pBSI->pFilen, pBSI->grpIx, pBSI->grpSIx,
				pBSI->isSrcOggOrFlac, ppPCMstrHnd);
		if (res != ST_ERR_SUCC)
			st_contOgg_d_err(pOpts, pFnc, pBSI->pFilen,
					"initialization of PCM client failed");
	}
	return res;
}

/******************************************************************************/
