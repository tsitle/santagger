/******************************************************************************/
/* cont_ogg_zwr.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for writing OGG containers / Flac files                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2011 (dd.mm.yyyy)                                     */
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
#include "src/includes/common/streamwr.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_CONT_OGG_ZWR_COD_C
#include "cont_ogg_zwr_cod-pp.h"
/*** */
#include "cont_ogg_zfdeb-prot.h"
#include "../cont_ogg_aud_flac/ac_flac_zrdh-pp.h"
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_OGG_ZWR_COD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Find next codec header packet
 */
Tst_int32
st_contOgg_wrcod_fndNextHd(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pStrrd)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_int32  resI      = -1;
	Tst_uint32 toRd,
	           totRd     = 0,
	           rdFromStr = 0,
	           offs      = 0;
	Tst_buf    buf[64];
	Tst_bool   fnd       = ST_B_FALSE;

	ST_ASSERTN_NUM(-1, pAObjI == NULL || pBSI == NULL || pStrrd == NULL)

	switch (pBSI->bsType) {
	case ST_CONTOGG_BSTP_CELT:
	case ST_CONTOGG_BSTP_SPEE:
	case ST_CONTOGG_BSTP_FLAC:
		return (-1);
	default:
		break;
	}

	toRd = sizeof(buf);
	while (! st_streamrd_isEOF(pStrrd)) {
		st_streamrd_rdAheadBuffer(pStrrd, toRd, buf, &rdFromStr);

		switch (pBSI->bsType) {
		case ST_CONTOGG_BSTP_VORB:
			fnd = st_sysBufSearch(buf, rdFromStr,
					(const Tst_buf*)"\x05vorbis", 7, &offs);
			break;
		case ST_CONTOGG_BSTP_THEO:
			fnd = st_sysBufSearch(buf, rdFromStr,
					(const Tst_buf*)"\x82theora", 7, &offs);
			break;
		default:
			break;
		}
		if (fnd || rdFromStr != toRd)
			break;
		st_streamrd_rdSkipBytes(pStrrd, toRd - 8, ST_B_FALSE);
		totRd += (toRd - 8);
	}

	if (fnd)
		resI = totRd + offs;
	return resI;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Write new vorbis comment
 */
Tst_err
st_contOgg_wrcod_appendVCmt(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substream *pBS)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 pdSzDelta,
	           pdSzBeg,
	           x   = 0,
	           *pUsd;
	Tst_byte   byt;
	Tst_bool   doCopyRVC = ST_B_FALSE;
	Tst_binobj                       *pRawCmtBO;
	Tst_contOgg_substr_intn          *pBSI;
	Tst_contOgg_vorbCommentRaw const *pVCR;

	ST_ASSERTN_IARG(pAObjI == NULL || pBS == NULL || pBS->pObInternal == NULL)

	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	pVCR = st_contOgg_vorbComm_gs_getRawComment(pBS);
	if (pVCR == NULL)
		return ST_ERR_FAIL;
	pRawCmtBO = st_contOgg_vorbComm_gs_getRC_rawDataPointer(pVCR);
	ST_ASSERTN_NUM(ST_ERR_FAIL, pRawCmtBO == NULL)

	pdSzBeg = st_binobj_getDataSize(&pBSI->packCur.bdata);

	switch (pBSI->bsType) {
	case ST_CONTOGG_BSTP_VORB:
		res = st_binobj_appendData(&pBSI->packCur.bdata,
				(const Tst_buf*)"\x03vorbis", 7);
		doCopyRVC = (res == ST_ERR_SUCC);
		break;
	case ST_CONTOGG_BSTP_THEO:
		res = st_binobj_appendData(&pBSI->packCur.bdata,
				(const Tst_buf*)"\x81theora", 7);
		doCopyRVC = (res == ST_ERR_SUCC);
		break;
	case ST_CONTOGG_BSTP_FLAC:
		byt = (Tst_byte)ST_CONTOGG_FLAC_BT_COMM;
		if (! pAObjI->isSrcOggOrFlac ||
				(pBSI->pFlac != NULL && pBSI->pFlac->commIsLastHdPck))
			byt |= 0x80;  /* 0x80 ^= last header packet */
		if (pAObjI->isSrcOggOrFlac)
			res = st_binobj_appendData(&pBSI->packCur.bdata,
					(const Tst_buf*)&byt, 1);
		else {
			if (! pAObjI->opts.basOpts.pretWr)
				res = st_streamwr_wrByte(pAObjI->pStrwr, 8, byt);
		}
		if (res == ST_ERR_SUCC) {
			x = st_binobj_getDataSize(pRawCmtBO);
			if (x > 0xffffff) {
				st_contOgg_d_err(&pAObjI->opts, LOC_FNCN_, pAObjI->pFilen,
						"vorbis comment too big for Flac");
				res = ST_ERR_ABRT;
			} else {
				x -= (x >= 8 + 4 ? 8 + 4 : x);
				if (pAObjI->isSrcOggOrFlac) {
#					if (WORDS_BIGENDIAN != 1)
					x = st_sysReverseByteOrder_UI32(x) >> 8;  /* ENDIAN: LE-->BE */
#					endif
					res = st_binobj_appendData(&pBSI->packCur.bdata,
							(const Tst_buf*)&x, 3);
				} else {
					if (! pAObjI->opts.basOpts.pretWr)
						res = st_streamwr_wrUInt32(pAObjI->pStrwr,
								ST_STREAMWR_OEND_BE, 24, x);
					if (res == ST_ERR_SUCC)
						pAObjI->totSize += 4;
				}
			}
		}
		doCopyRVC = (res == ST_ERR_SUCC);
		break;
	case ST_CONTOGG_BSTP_SPEE:
	case ST_CONTOGG_BSTP_CELT:
		doCopyRVC = ST_B_TRUE;
		break;
	default:
		break;
	}
	if (res != ST_ERR_SUCC)
		return res;

	if (doCopyRVC) {
		Tst_uint32 wrRVCbytes = 0;

		/* append the actual raw vorbis comment */
		res = ST_CONTOGG__wrcod_copyRVC(&pAObjI->opts, pRawCmtBO,
				(pAObjI->isSrcOggOrFlac ? &pBSI->packCur.bdata : NULL),
				(pAObjI->isSrcOggOrFlac ? NULL : pAObjI->pStrwr),
				&pBSI->sobjRdFBuf,
				(pAObjI->isSrcOggOrFlac ? NULL : &wrRVCbytes));
		if (res != ST_ERR_SUCC)
			return res;

		if (! pAObjI->isSrcOggOrFlac)
			pAObjI->totSize += wrRVCbytes;

		if (pBSI->bsType == ST_CONTOGG_BSTP_VORB) {
			/* append 'framing bit' */
			res = st_binobj_appendData(&pBSI->packCur.bdata,
					(const Tst_buf*)"\x01", 1);
			if (res != ST_ERR_SUCC)
				return res;
		}
	}

	if (pAObjI->isSrcOggOrFlac) {
		/* get size of entire vorbis comment */
		pdSzDelta = st_binobj_getDataSize(&pBSI->packCur.bdata) - pdSzBeg;

		/* append some padding */
		if ((pdSzDelta % 255) == 0) {
			Tst_buf tmpPaddBuf[127];

			memset(tmpPaddBuf, 0, sizeof(tmpPaddBuf));
			res = st_binobj_appendData(&pBSI->packCur.bdata,
					tmpPaddBuf, sizeof(tmpPaddBuf));
			if (res != ST_ERR_SUCC)
				return res;
			pdSzDelta += sizeof(tmpPaddBuf);
		}

		/* store lacing values for vorbis comment */
		/** */
		if (pBSI->packCur.segsUsd + ((pdSzDelta / 255) + 2) > pBSI->packCur.segsSz) {
			pBSI->packCur.segsSz = pBSI->packCur.segsUsd + ((pdSzDelta / 255) + 2);
			ST_REALLOC(pBSI->packCur.pSegs, Tst_byte*, pBSI->packCur.segsSz, 1)
			if (pBSI->packCur.pSegs == NULL)
				return ST_ERR_OMEM;
		}
		/** */
		pUsd = &pBSI->packCur.segsUsd;
		while (pdSzDelta > 0) {
			pBSI->packCur.pSegs[*pUsd] = (pdSzDelta > 255 ? 255 : pdSzDelta);
			pdSzDelta                 -= pBSI->packCur.pSegs[*pUsd];
			/**st_contOgg_prf("%s(): segTab %3u: 0x%02x (%u)\n",
					LOC_FNCN_, *pUsd,
					pBSI->packCur.pSegs[*pUsd], pBSI->packCur.pSegs[*pUsd]);**/
			++(*pUsd);
		}

#		if 0
		{
			Tst_uint32 ttt = 0;

			st_contOgg_prf("%s(): checking segTab\n", LOC_FNCN_);
			for (x = 0; x < pBSI->packCur.segsUsd; x++)
				ttt += pBSI->packCur.pSegs[x];
			pdSzDelta = st_binobj_getDataSize(&pBSI->packCur.bdata);
			if (ttt != pdSzDelta) {
				st_contOgg_prf("%s(): segTab faulty: sz %u != %u\n",
						LOC_FNCN_, ttt, pdSzDelta);
				return ST_ERR_IDAT;
			}
		}
#		endif
	}

	return ST_ERR_SUCC;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Write Flac headers
 */
Tst_err
st_contOgg_wrcod_flacHeaders(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substream *pBS)
{
#	define LOC_FNCN_  __FUNCTION__
	Tst_err    res    = ST_ERR_SUCC;
	Tst_byte   byt    = 0;
	Tst_bool   isLast = ST_B_FALSE,
	           doSkip,
	           doCopy;
	Tst_uint32 mdSz   = 0;
	Tst_contOgg_flacBlockType bt = ST_CONTOGG_FLAC_BT_NONE;
	Tst_contOgg_substr_intn   *pBSI;

	ST_ASSERTN_IARG(pAObjI == NULL || pBS == NULL || pBS->pObInternal == NULL)

	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;

	/* copy/skip all header packets */
	while (res == ST_ERR_SUCC && ! st_streamrd_isEOF(pAObjI->pStrrd)) {
		/* PACKTYPE */
		res = st_streamrd_rdAheadByte(pAObjI->pStrrd, 8, &byt);
		if (res != ST_ERR_SUCC || byt == 0xff)
			break;
		/* get blockheader info */
		res = st_contOgg_flacrdh_parseBH(pAObjI->pStrrd, &bt, &isLast, &mdSz);
		if (res != ST_ERR_SUCC)
			break;
		/* copy/skip metadata */
		/** */
		doSkip = ST_B_FALSE;
		doCopy = ST_B_FALSE;
		switch (bt) {
		case ST_CONTOGG_FLAC_BT_STRINF:
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
						"copying Flac ident header");
			doCopy = ST_B_TRUE;
			break;
		case ST_CONTOGG_FLAC_BT_PADD:
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
						"skipping Flac padding");
			doSkip = ST_B_TRUE;
			break;
		case ST_CONTOGG_FLAC_BT_APP:
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
						"copying Flac app header");
			doCopy = ST_B_TRUE;
			break;
		case ST_CONTOGG_FLAC_BT_SEEK:
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
						"copying Flac seek header");
			doCopy = ST_B_TRUE;
			break;
		case ST_CONTOGG_FLAC_BT_COMM:
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
						"skipping Flac comment header");
			doSkip = ST_B_TRUE;
			break;
		case ST_CONTOGG_FLAC_BT_CUE:
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
						"copying Flac cue header");
			doCopy = ST_B_TRUE;
			break;
		case ST_CONTOGG_FLAC_BT_PICT:
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
						"skipping Flac picture header");
			doSkip = ST_B_TRUE;
			break;
		default:
			res = ST_ERR_IDAT;
		}
		if (res != ST_ERR_SUCC)
			break;
		/** */
		if (doSkip)
			st_streamrd_rdSkipBytes(pAObjI->pStrrd, mdSz, ST_B_TRUE);
		else {
			Tst_uint32 copied = 0;

			/* write blockheader */
			if (! pAObjI->opts.basOpts.pretWr) {
				byt = (Tst_byte)bt & 0x7f;  /* !0x80 ^= not last header packet */
				res = st_streamwr_wrByte(pAObjI->pStrwr, 8, byt);
				if (res == ST_ERR_SUCC)
					res = st_streamwr_wrUInt32(pAObjI->pStrwr,
							ST_STREAMRD_IEND_BE, 24, mdSz);  /* ENDIAN: HOST-->BE */
			}
			if (res == ST_ERR_SUCC)
				pAObjI->totSize += 4;
			/* copy data */
			if (res == ST_ERR_SUCC && mdSz > 0) {
				res = st_contOgg_wrcod_copyData(&pAObjI->opts,
						pAObjI->pStrrd, pAObjI->pStrwr, mdSz, &copied);
				if (res == ST_ERR_SUCC)
					pAObjI->totSize += copied;
			}
		}
	}
	/* write vorbis comment header packet */
	if (res == ST_ERR_SUCC) {
		if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
			st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
					"writing new Vorbis comment header");
		res = st_contOgg_wrcod_appendVCmt(pAObjI, pBS);
	}

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

Tst_err
st_contOgg_wrcod_copyData(const Tst_contOgg_opts *pOpts,
		Tst_streamrd *pStrrd, Tst_streamwr *pStrwr,
		const Tst_uint32 toCopy, Tst_uint32 *pCopied)
{
	Tst_err    res       = ST_ERR_SUCC;
	Tst_uint32 toRd,
	           rdFromStr = 0;
	Tst_buf    buf[4096];

	ST_ASSERTN_IARG(pOpts == NULL || pStrrd == NULL ||
			pStrwr == NULL || pCopied == NULL)

	*pCopied = 0;

	toRd = sizeof(buf);
	while (*pCopied < toCopy) {
		if (*pCopied + toRd > toCopy)
			toRd = toCopy - *pCopied;
		res = st_streamrd_rdBuffer_noCRC(pStrrd,
				toRd, buf, &rdFromStr);
		if (res != ST_ERR_SUCC && res != ST_ERR_FEOF)
			break;
		/* */
		if (! pOpts->basOpts.pretWr) {
			res = st_streamwr_wrBuffer(pStrwr, rdFromStr, buf);
			if (res != ST_ERR_SUCC)
				break;
		}
		*pCopied += rdFromStr;
		if (rdFromStr != toRd)
			break;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Copy raw vorbis comment from pBOsrc to pBOdst or pStrwr
 */
static Tst_err
ST_CONTOGG__wrcod_copyRVC(const Tst_contOgg_opts *pOpts,
		Tst_binobj *pBOsrc, Tst_binobj *pBOdst,
		Tst_streamwr *pStrwr, Tst_streamrd *pStrrdTmp,
		Tst_uint32 *pCopied)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 cmtSz;

	ST_ASSERTN_IARG(pOpts == NULL || pBOsrc == NULL || pStrrdTmp == NULL ||
			(pBOdst == NULL && pStrwr == NULL) ||
			(pStrwr != NULL && pCopied == NULL))

	cmtSz = st_binobj_getDataSize(pBOsrc);

	if (cmtSz > 8 + 4) {
		/* we're skipping the pseudo header here */
		res = st_binobj_attachStreamrd(pBOsrc, pStrrdTmp, 8 + 4);
		if (res != ST_ERR_SUCC)
			return res;
		if (pBOdst != NULL)
			res = st_binobj_appendData_streamrd(pBOdst, pStrrdTmp,
					cmtSz - (8 + 4), ST_B_FALSE);
		else if (pStrwr != NULL && pCopied != NULL)
			res = st_contOgg_wrcod_copyData(pOpts,
					pStrrdTmp, pStrwr, cmtSz - (8 + 4), pCopied);
		st_binobj_detachStreamrd(pBOsrc);
	}

	return res;
}

/******************************************************************************/
