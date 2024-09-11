/******************************************************************************/
/* tag_id3v1_zwr.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for writing ID3v1 (Extended) Tags                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamwr.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_id3_helper-defs.h"
#include "src/includes/tag/tag_id3v1.h"
/** */
#define SRC_TAG_ID3V1_ZWR_C
#include "tag_id3v1_zwr-prot.h"
/*** */
#include "tag_id3v1-pp_flds.h"
#include "tag_id3v1_fnc-prot.h"
#include "tag_id3v1_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V1_ZWR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__wr_rendTF_s(const char *pFnc, const Tst_id3v1_tag_intn *pTagI,
                     Tst_buf *pDstBuf,
                     const Tst_id3v1_frID dstID,
                     Tst_uint32 *pWrBytes);
static Tst_err
ST_TIV1__wr_rendTF_year(const char *pFnc, const Tst_id3v1_tag_intn *pTagI,
                        Tst_buf *pDstBuf, Tst_uint32 *pWrBytes);
static Tst_err
ST_TIV1__wr_rendTF_b(const char *pFnc, const Tst_id3v1_tag_intn *pTagI,
                     Tst_buf *pDstBuf,
                     const Tst_id3v1_frID dstID);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Render Tag to a buffer
 */
Tst_err
st_id3v1_wr_rendTagToBO(const Tst_id3v1_tag_intn *pTagI, Tst_binobj *pTagBO)
{
	const char *cFNCN = __FUNCTION__;
#	define LOC_RENDSTR_(mac_id)  \
				ST_TIV1__wr_rendTF_s(cFNCN, pTagI, pBufPt, mac_id, &wrBytes)
#	define LOC_RENDBYT_(mac_id)  \
				ST_TIV1__wr_rendTF_b(cFNCN, pTagI, pBufPt, mac_id)
	Tst_err    res     = ST_ERR_SUCC;
	Tst_buf    tbuf[ST_ID3V1_TAG_SZ],
	           *pBufPt = tbuf;
	Tst_uint32 wrBytes = 0;

	ST_ASSERTN_IARG(pTagI == NULL || pTagBO == NULL)

	memset(tbuf, 0, sizeof(tbuf));

	/* "TAG" */
	wrBytes = ST_ID3V1_TAG_HEAD_SLEN;
	memcpy(pBufPt, ST_ID3V1_TAG_HEAD, wrBytes);
	pBufPt += wrBytes;

	/* TITLE */
	res = LOC_RENDSTR_(ST_ID3V1_FID_TITL);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += wrBytes;

	/* ARTIST */
	res = LOC_RENDSTR_(ST_ID3V1_FID_ARTI);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += wrBytes;

	/* ALBUM */
	res = LOC_RENDSTR_(ST_ID3V1_FID_ALBU);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += wrBytes;

	/* YEAR */
	res = ST_TIV1__wr_rendTF_year(cFNCN, pTagI, pBufPt, &wrBytes);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += wrBytes;

	/* COMMENT/TRACKNR */
	res = LOC_RENDSTR_(ST_ID3V1_FID_COMM);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += wrBytes;
	/** 'write' 0 */
	++pBufPt;
	/** */
	res = LOC_RENDBYT_(ST_ID3V1_FID_TRKN);
	if (res != ST_ERR_SUCC)
		return res;
	++pBufPt;

	/* GENRE */
	res = LOC_RENDBYT_(ST_ID3V1_FID_GENR);
	if (res != ST_ERR_SUCC)
		return res;

	/* copy buffer to binary object */
	res = st_binobj_appendData(pTagBO, tbuf, sizeof(tbuf));

	return res;
#	undef LOC_RENDSTR_
#	undef LOC_RENDBYT_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__wr_rendTF_s(const char *pFnc, const Tst_id3v1_tag_intn *pTagI,
		Tst_buf *pDstBuf,
		const Tst_id3v1_frID dstID, Tst_uint32 *pWrBytes)
{
#	define LOC_REPL_  1  /* set to 0 for debugging */
	Tst_err    res;
	Tst_uint32 len,
	           csz    = 0,
	           toWr   = 0;
	Tst_bool   putEnd = ST_B_FALSE;
	Tst_str    *pTmpC = NULL,
	           bufPre[128];
	Tst_mtes_string        tmpS;
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_IARG(pTagI == NULL || pDstBuf == NULL ||
			dstID == ST_ID3V1_FID_NONE || pWrBytes == NULL)

	*pWrBytes = 0;

	/* get field pointer, caption and data length */
	switch (dstID) {
	case ST_ID3V1_FID_TITL:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdTit.pObInternal;
		toWr  = ST_ID3V1_TAG_LEN_TITL;
		break;
	case ST_ID3V1_FID_ALBU:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdAlb.pObInternal;
		toWr  = ST_ID3V1_TAG_LEN_ALBU;
		break;
	case ST_ID3V1_FID_ARTI:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdArt.pObInternal;
		toWr  = ST_ID3V1_TAG_LEN_ARTI;
		break;
	case ST_ID3V1_FID_COMM:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdCom.pObInternal;
		toWr  = ST_ID3V1_TAG_LEN_COMM - ST_ID3V1_TAG_LEN_TRKN - 1;
		break;
	default:
		ST_ASSERTN_IARG(dstID != ST_ID3V1_FID_TITL &&
				dstID != ST_ID3V1_FID_ALBU && dstID != ST_ID3V1_FID_ARTI &&
				dstID != ST_ID3V1_FID_COMM)  /* cause exit() here */
		return ST_ERR_IARG;
	}
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	/* prepare string */
	/** */
	res = st_mtes_stc_initStr(&tmpS);
	if (res != ST_ERR_SUCC)
		return res;
	/** */
	if (pFldI->isOK) {
#		if (LOC_REPL_ == 1)
		/** */
		res = st_mtes_strCpy(&pFldI->dataStr, &tmpS);
		/** strip string */
		if (res == ST_ERR_SUCC)
			res = st_id3v1_fnc_stripStr(&tmpS, NULL, NULL, NULL);
		/** convert to ISO */
		if (res == ST_ERR_SUCC && st_mtes_getEncodingCur(&tmpS) != ST_MTES_TE_ISO)
			res = st_mtes_convStr(&tmpS, ST_MTES_TE_ISO);
#		else
		res = st_mtes_convStr_c(&pFldI->dataStr, &tmpS, ST_MTES_TE_UTF8);
#		endif
		/** copy string to pTmpC */
		if (res == ST_ERR_SUCC)
			res = st_mtes_copyToCharp(&tmpS, &pTmpC, &csz);
	}
	st_mtes_stc_freeStr(&tmpS);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pTmpC)
		return res;
	}
	/** */
	len = st_sysStrlen(pTmpC);
	if (len > toWr) {
		len    = toWr;
		putEnd = (len > 3);
	}

	/* write data */
	/** */
	if (len > 0) {
		memcpy(bufPre, pTmpC, len);
		if (putEnd)
			memcpy(&bufPre[len - 3], "...", 3);
	}
	bufPre[len] = 0;
	ST_DELPOINT(pTmpC)
	/** */
	memcpy(pDstBuf, bufPre, len);
	*pWrBytes = toWr;

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v1_d_fdeb(&pTagI->opts, 2, pFnc, &pFldI->fldPrI,
				"\"%s\"", bufPre);

	return ST_ERR_SUCC;
#	undef LOC_REPL_
}

static Tst_err
ST_TIV1__wr_rendTF_year(const char *pFnc, const Tst_id3v1_tag_intn *pTagI,
		Tst_buf *pDstBuf, Tst_uint32 *pWrBytes)
{
	Tst_id3v1_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pTagI == NULL || pDstBuf == NULL || pWrBytes == NULL)

	pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdYea.pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	*pWrBytes = ST_ID3V1_TAG_LEN_YEAR;

	if (pFldI->isOK) {
		Tst_str strBuf[64];

		snprintf((char*)strBuf, *pWrBytes + 1, "%04d", pFldI->dataIVal);
		memcpy(pDstBuf, strBuf, *pWrBytes);
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_fdeb(&pTagI->opts, 2, pFnc, &pFldI->fldPrI,
					"%d", pFldI->dataIVal);
	} else {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_fdeb(&pTagI->opts, 2, pFnc, &pFldI->fldPrI,
					"0");
	}

	return ST_ERR_SUCC;
}

static Tst_err
ST_TIV1__wr_rendTF_b(const char *pFnc, const Tst_id3v1_tag_intn *pTagI,
		Tst_buf *pDstBuf, const Tst_id3v1_frID dstID)
{
	Tst_byte bytOut = 0;
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_IARG(pTagI == NULL || pDstBuf == NULL ||
			dstID == ST_ID3V1_FID_NONE)

	/* get field pointer and caption */
	switch (dstID) {
	case ST_ID3V1_FID_TRKN:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdTrk.pObInternal;
		break;
	case ST_ID3V1_FID_GENR:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdGen.pObInternal;
		break;
	default:
		ST_ASSERTN_IARG(dstID != ST_ID3V1_FID_TRKN &&
				dstID != ST_ID3V1_FID_GENR)  /* cause exit() here */
		return ST_ERR_IARG;
	}
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	/* prepare output data */
	switch (dstID) {
	case ST_ID3V1_FID_TRKN:
		if (pFldI->isOK)
			bytOut = (Tst_byte)pFldI->dataIVal;
		else
			bytOut = 0;
		break;
	case ST_ID3V1_FID_GENR:
		if (pFldI->isOK)
			bytOut = (Tst_byte)pFldI->dataIVal;
		else
			bytOut = 255;
		break;
	default:
		break;
	}

	/* write data */
	*pDstBuf = bytOut;

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v1_d_fdeb(&pTagI->opts, 2, pFnc, &pFldI->fldPrI,
				"%u", bytOut);
	return ST_ERR_SUCC;
}

/******************************************************************************/
