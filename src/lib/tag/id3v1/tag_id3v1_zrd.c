/******************************************************************************/
/* tag_id3v1_zrd.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading ID3v1 (Extended) Tags                                */
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
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_id3v1.h"
/** */
#define SRC_TAG_ID3V1_ZRD_C
#include "tag_id3v1_zrd-prot.h"
/*** */
#include "tag_id3v1-pp_flds.h"
#include "tag_id3v1_fnc-prot.h"
#include "tag_id3v1_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V1_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__rd_readV1_flds(Tst_id3v1_tag_intn *pTagI,
                        Tst_buf const *pBufPt);
static Tst_err
ST_TIV1__rd_readV1Ext_flds(Tst_id3v1_tag_intn *pTagI,
                           Tst_buf const *pBufPt);
/** */
static Tst_err
ST_TIV1__rd_readTF_s(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
                     const Tst_buf *pSrcBuf,
                     const Tst_id3v1_frID dstID,
                     const Tst_id3v1ext_frID dstIDext,
                     Tst_uint32 *pRdBytes);
static Tst_err
ST_TIV1__rd_readTF_year(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
                        const Tst_buf *pSrcBuf,
                        Tst_uint32 *pRdBytes);
static void
ST_TIV1__rd_readTF_b(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
                     const Tst_buf *pSrcBuf,
                     const Tst_id3v1_frID dstID,
                     const Tst_id3v1ext_frID dstIDext);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read ID3v1 Tag
 */
Tst_err
st_id3v1_rd_readV1(const Tst_bool rdOnlyBasics,
		Tst_id3v1_tag_intn *pTagI, Tst_buf *pBuf)
{
	const char *cFNCN = __func__;
	Tst_err       res;
	Tst_buf const *pBB = pBuf;
	Tst_foffs     offs;
	Tst_uint32    tagSz,
	              rd   = 0;

	ST_ASSERTN_IARG(pTagI == NULL || pBuf == NULL)

	offs = st_streamrd_getCurPos(pTagI->pStrrd);
	/* read entire tag into buffer */
	res = st_streamrd_rdBuffer_noCRC(pTagI->pStrrd,
			ST_ID3V1_TAG_SZ, pBuf, &rd);
	if (res != ST_ERR_SUCC || rd != ST_ID3V1_TAG_SZ) {
		if (res != ST_ERR_SUCC)
			st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
				"ID3v1 tag found");

	/* skip "TAG" */
	pBB += ST_ID3V1_TAG_HEAD_SLEN;

	/* read field values */
	if (! rdOnlyBasics)
		res = ST_TIV1__rd_readV1_flds(pTagI, pBB);

	/* */
	if (res == ST_ERR_SUCC) {
		if (! rdOnlyBasics && ! st_tagBas_gs_getHasFields(&pTagI->tbas) &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
					"tag is empty");
		/* */
		tagSz = st_tagBas_gs_getSize(&pTagI->tbas);
		st_tagBas_gs_setHasTag(&pTagI->tbas, ST_B_TRUE);
		if (tagSz == 0)
			st_tagBas_gs_setOffset(&pTagI->tbas, offs);
		st_tagBas_gs_setSize(&pTagI->tbas, tagSz + ST_ID3V1_TAG_SZ);
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read extended ID3v1 Tag
 */
Tst_err
st_id3v1_rd_readV1Ext(const Tst_bool rdOnlyBasics,
		Tst_id3v1_tag_intn *pTagI, Tst_buf *pBuf)
{
	const char *cFNCN = __func__;
	Tst_err       res;
	Tst_foffs     offs;
	Tst_buf const *pBB = pBuf;
	Tst_uint32    rd   = 0;

	ST_ASSERTN_IARG(pTagI == NULL || pBuf == NULL)

	/* ID3v1 Extended Tag should be only present, if ID3v1 is too */

	offs = st_streamrd_getCurPos(pTagI->pStrrd);
	/* read entire tag into buffer */
	res = st_streamrd_rdBuffer_noCRC(pTagI->pStrrd,
			ST_ID3V1_TAG_EXT_SZ, pBuf, &rd);
	if (res != ST_ERR_SUCC || rd != ST_ID3V1_TAG_EXT_SZ) {
		if (res != ST_ERR_SUCC)
			st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
				"ID3v1 Extended tag found");

	/* skip "TAG+" */
	pBB += ST_ID3V1_TAG_HEAD_EXT_SLEN;

	/* read field values */
	if (! rdOnlyBasics)
		res = ST_TIV1__rd_readV1Ext_flds(pTagI, pBB);

	if (res == ST_ERR_SUCC) {
		pTagI->hasExtTag = ST_B_TRUE;
		/* */
		st_tagBas_gs_setHasTag(&pTagI->tbas, ST_B_TRUE);
		st_tagBas_gs_setSize(&pTagI->tbas, ST_ID3V1_TAG_EXT_SZ);
		st_tagBas_gs_setOffset(&pTagI->tbas, offs);
	}

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read ID3v1 Tag field values
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TIV1__rd_readV1_flds(Tst_id3v1_tag_intn *pTagI,
		Tst_buf const *pBufPt)
{
	const char *cFNCN = __func__;
#	define LOC_READSTR_(mac_id)  \
				ST_TIV1__rd_readTF_s(cFNCN, pTagI, pBufPt, \
						mac_id, ST_ID3V1E_FID_NONE, &rdBytes)
#	define LOC_READBYT_(mac_id)  \
				ST_TIV1__rd_readTF_b(cFNCN, pTagI, pBufPt, \
						mac_id, ST_ID3V1E_FID_NONE)
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 rdBytes = 0;

	/* TITLE */
	res = LOC_READSTR_(ST_ID3V1_FID_TITL);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* ARTIST */
	res = LOC_READSTR_(ST_ID3V1_FID_ARTI);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* ALBUM */
	res = LOC_READSTR_(ST_ID3V1_FID_ALBU);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* YEAR */
	res = ST_TIV1__rd_readTF_year(cFNCN, pTagI, pBufPt, &rdBytes);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* COMMENT/TRACKNR */
	/** comment */
	res = LOC_READSTR_(ST_ID3V1_FID_COMM);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;
	/** */
	if (*pBufPt == 0x00) {
		++pBufPt;
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
					"found ID3v1.1 Tag");
		/* tracknr  (ID3v1.1) */
		LOC_READBYT_(ST_ID3V1_FID_TRKN);
		++pBufPt;
	} else {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
					"found ID3v1.0 Tag");
		pBufPt += 2;
	}

	/* GENRE */
	LOC_READBYT_(ST_ID3V1_FID_GENR);

	return ST_ERR_SUCC;
#	undef LOC_READSTR_
#	undef LOC_READBYT_
}

/*----------------------------------------------------------------------------*/

/*
 * Read extended tag fields
 */
static Tst_err
ST_TIV1__rd_readV1Ext_flds(Tst_id3v1_tag_intn *pTagI,
		Tst_buf const *pBufPt)
{
	const char *cFNCN = __func__;
#	define LOC_READSTR_(mac_idExt)  \
				ST_TIV1__rd_readTF_s(cFNCN, pTagI, pBufPt, \
						ST_ID3V1_FID_NONE, mac_idExt, &rdBytes)
#	define LOC_READBYT_(mac_idExt)  \
				ST_TIV1__rd_readTF_b(cFNCN, pTagI, pBufPt, \
						ST_ID3V1_FID_NONE, mac_idExt)
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 rdBytes = 0;

	/* Extended TITLE */
	res = LOC_READSTR_(ST_ID3V1E_FID_ETITL);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* Extended ARTIST */
	res = LOC_READSTR_(ST_ID3V1E_FID_EARTI);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* Extended ALBUM */
	res = LOC_READSTR_(ST_ID3V1E_FID_EALBU);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* SPEED */
	LOC_READBYT_(ST_ID3V1E_FID_SPEED);
	++pBufPt;

	/* Extended GENRE */
	res = LOC_READSTR_(ST_ID3V1E_FID_EGENR);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* START TIME  (format: "mmm:ss") */
	res = LOC_READSTR_(ST_ID3V1E_FID_TIMES);
	if (res != ST_ERR_SUCC)
		return res;
	pBufPt += rdBytes;

	/* END TIME  (format: "mmm:ss") */
	res = LOC_READSTR_(ST_ID3V1E_FID_TIMEE);

	return res;
#	undef LOC_READSTR_
#	undef LOC_READBYT_
}

/*----------------------------------------------------------------------------*/

/*
 * Read string
 */
static Tst_err
ST_TIV1__rd_readTF_s(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
		const Tst_buf *pSrcBuf,
		const Tst_id3v1_frID dstID, const Tst_id3v1ext_frID dstIDext,
		Tst_uint32 *pRdBytes)
{
	Tst_err    res;
	Tst_str    sbuf[256];
	Tst_bool   hIC  = ST_B_FALSE,  /* had invalid chars ? */
	           mod1 = ST_B_FALSE,  /* had umlauts ? */
	           mod2 = ST_B_FALSE;  /* was modified by stripping ? */
	Tst_uint32 toRd = 0;
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_IARG(pTagI == NULL || pSrcBuf == NULL || pRdBytes == NULL ||
			(dstID == ST_ID3V1_FID_NONE && dstIDext == ST_ID3V1E_FID_NONE) ||
			(dstID != ST_ID3V1_FID_NONE && dstIDext != ST_ID3V1E_FID_NONE))

	*pRdBytes = 0;

	/* get field pointer, caption and data length */
	/** */
	switch (dstID) {
	case ST_ID3V1_FID_TITL:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdTit.pObInternal;
		toRd  = ST_ID3V1_TAG_LEN_TITL;
		break;
	case ST_ID3V1_FID_ALBU:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdAlb.pObInternal;
		toRd  = ST_ID3V1_TAG_LEN_ALBU;
		break;
	case ST_ID3V1_FID_ARTI:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdArt.pObInternal;
		toRd  = ST_ID3V1_TAG_LEN_ARTI;
		break;
	case ST_ID3V1_FID_COMM:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdCom.pObInternal;
		toRd  = ST_ID3V1_TAG_LEN_COMM - ST_ID3V1_TAG_LEN_TRKN - 1;
		break;
	case ST_ID3V1_FID_NONE:
		break;
	default:
		ST_ASSERTN_IARG(dstID != ST_ID3V1_FID_TITL &&
				dstID != ST_ID3V1_FID_ALBU && dstID != ST_ID3V1_FID_ARTI &&
				dstID != ST_ID3V1_FID_COMM)  /* cause exit() here */
		return ST_ERR_IARG;
	}
	/** */
	switch (dstIDext) {
	case ST_ID3V1E_FID_ETITL:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdExtTit.pObInternal;
		toRd  = ST_ID3V1_TAG_EXT_LEN_ETITL;
		break;
	case ST_ID3V1E_FID_EALBU:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdExtAlb.pObInternal;
		toRd  = ST_ID3V1_TAG_EXT_LEN_EALBU;
		break;
	case ST_ID3V1E_FID_EARTI:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdExtArt.pObInternal;
		toRd  = ST_ID3V1_TAG_EXT_LEN_EARTI;
		break;
	case ST_ID3V1E_FID_EGENR:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdExtGen.pObInternal;
		toRd  = ST_ID3V1_TAG_EXT_LEN_EGENR;
		break;
	case ST_ID3V1E_FID_TIMES:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdExtTiS.pObInternal;
		toRd  = ST_ID3V1_TAG_EXT_LEN_TIMES;
		break;
	case ST_ID3V1E_FID_TIMEE:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdExtTiE.pObInternal;
		toRd  = ST_ID3V1_TAG_EXT_LEN_TIMEE;
		break;
	case ST_ID3V1E_FID_NONE:
		break;
	default:
		ST_ASSERTN_IARG(dstIDext != ST_ID3V1E_FID_ETITL &&
				dstIDext != ST_ID3V1E_FID_EALBU &&
				dstIDext != ST_ID3V1E_FID_EARTI &&
				dstIDext != ST_ID3V1E_FID_EGENR &&
				dstIDext != ST_ID3V1E_FID_TIMES &&
				dstIDext != ST_ID3V1E_FID_TIMEE)  /* cause exit() here */
		return ST_ERR_IARG;
	}
	/** */
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	/* reset field data */
	st_mtes_stc_rsetStr(&pFldI->dataStr, ST_B_TRUE, ST_B_TRUE);
	pFldI->isOK = ST_B_FALSE;

	/* read data from buffer */
	memcpy(sbuf, pSrcBuf, toRd);
	sbuf[toRd] = 0;
	*pRdBytes  = toRd;

	/* is utf8 ? */
	if (st_mtes_testIfIsUTF8(sbuf, st_sysStrlen(sbuf) + 1)) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_fdeb(&pTagI->opts, 0, pFnc, &pFldI->fldPrI,
					"string seems to be UTF8 encoded");
		res = st_mtes_copyFromCharp_utf8(sbuf, &pFldI->dataStr);
	} else
		res = st_mtes_copyFromCharp_iso(sbuf, &pFldI->dataStr);
	if (res != ST_ERR_SUCC)
		return res;

	/* strip string */
	res = st_id3v1_fnc_stripStr(&pFldI->dataStr, &hIC, &mod1, &mod2);
	if (res != ST_ERR_SUCC)
		return res;

	/* */
	pFldI->wereCorrd = mod1 || mod2;
	/* */
	pTagI->wereCorrd = pTagI->wereCorrd || pFldI->wereCorrd;
	if (! st_mtes_isStrEmptyCh(&pFldI->dataStr)) {
		pFldI->isOK = ST_B_TRUE;
		st_tagBas_gs_setHasFields(&pTagI->tbas, ST_B_TRUE);
	}

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		if (hIC)
			st_id3v1_d_fdeb(&pTagI->opts, 0, pFnc, &pFldI->fldPrI,
					"string contained invalid chars");
		st_id3v1_d_fdebStr(&pTagI->opts, 1, pFnc, &pFldI->fldPrI,
				&pFldI->dataStr, "");
	}
	return ST_ERR_SUCC;
}

/*
 * Read YEAR
 */
static Tst_err
ST_TIV1__rd_readTF_year(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
		const Tst_buf *pSrcBuf, Tst_uint32 *pRdBytes)
{
	Tst_uint32 tmpUI = 0;
	Tst_str    tmpYearStr[ST_ID3V1_TAG_LEN_YEAR + 1],
	           *pCh;
	Tst_id3v1_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pTagI == NULL || pSrcBuf == NULL || pRdBytes == NULL)

	/* read YEAR-String from buffer */
	*pRdBytes = ST_ID3V1_TAG_LEN_YEAR;
	memcpy(tmpYearStr, pSrcBuf, *pRdBytes);
	tmpYearStr[*pRdBytes] = 0;

	pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdYea.pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	/* reset field data */
	pFldI->dataIVal = -1;
	pFldI->isOK     = ST_B_FALSE;

	/* check YEAR-String and convert to integer */
	pCh              = tmpYearStr;
	pFldI->wereCorrd = ST_B_FALSE;
	while (*pCh)
		if (*pCh < '0' || *pCh > '9') {
			memset(tmpYearStr, 0, sizeof(tmpYearStr));
			pFldI->wereCorrd = ST_B_TRUE;
			break;
		} else
			++pCh;
	if (! pFldI->wereCorrd && ! st_sysStrEmpty(tmpYearStr))
		sscanf((char*)tmpYearStr, "%u", &tmpUI);
	if (pFldI->wereCorrd || st_sysStrEmpty(tmpYearStr) ||
			tmpUI < ST_ID3_TFLD_YEAR_MIN || tmpUI > ST_ID3_TFLD_YEAR_MAX) {
		pFldI->wereCorrd = pFldI->wereCorrd ||
				((tmpUI < ST_ID3_TFLD_YEAR_MIN ||
					tmpUI > ST_ID3_TFLD_YEAR_MAX) &&
					tmpUI != 0);
		pFldI->dataIVal  = ST_ID3_TFLD_YEAR_NONE_IV1;
	} else {
		pFldI->dataIVal = (Tst_int32)tmpUI;
		pFldI->isOK     = ST_B_TRUE;
		st_tagBas_gs_setHasFields(&pTagI->tbas, ST_B_TRUE);
	}

	/* */
	pTagI->wereCorrd = pTagI->wereCorrd || pFldI->wereCorrd;

	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		if (pFldI->wereCorrd)
			st_id3v1_d_fdeb(&pTagI->opts, 0, pFnc, &pFldI->fldPrI,
					"value was invalid");
		st_id3v1_d_fdeb(&pTagI->opts, 1, pFnc, &pFldI->fldPrI,
				"%d%s", pFldI->dataIVal,
				(pFldI->dataIVal == ST_ID3_TFLD_YEAR_NONE_IV1 ?
					" (==none)" : ""));
	}
	return ST_ERR_SUCC;
}

/*
 * Read one byte
 */
static void
ST_TIV1__rd_readTF_b(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
		const Tst_buf *pSrcBuf,
		const Tst_id3v1_frID dstID, const Tst_id3v1ext_frID dstIDext)
{
	Tst_int32 noneVal = -1;
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_VOID(pTagI == NULL || pSrcBuf == NULL ||
			(dstID == ST_ID3V1_FID_NONE && dstIDext == ST_ID3V1E_FID_NONE) ||
			(dstID != ST_ID3V1_FID_NONE && dstIDext != ST_ID3V1E_FID_NONE))

	/* get field pointer and caption */
	/** */
	switch (dstID) {
	case ST_ID3V1_FID_TRKN:
		pFldI   = (Tst_id3v1_fldData_intn*)pTagI->fdTrk.pObInternal;
		noneVal = ST_ID3_TFLD_TRACK_NONE_IV1;
		break;
	case ST_ID3V1_FID_GENR:
		pFldI   = (Tst_id3v1_fldData_intn*)pTagI->fdGen.pObInternal;
		noneVal = ST_ID3_TFLD_GENRE_NONE_IV1;
		break;
	case ST_ID3V1_FID_NONE:
		break;
	default:
		ST_ASSERTN_VOID(dstID != ST_ID3V1_FID_TRKN &&
				dstID != ST_ID3V1_FID_GENR)  /* cause exit() here */
		return;
	}
	/** */
	switch (dstIDext) {
	case ST_ID3V1E_FID_SPEED:
		pFldI   = (Tst_id3v1_fldData_intn*)pTagI->fdExtSpd.pObInternal;
		noneVal = -1;
		break;
	case ST_ID3V1E_FID_NONE:
		break;
	default:
		ST_ASSERTN_VOID(dstIDext != ST_ID3V1E_FID_SPEED)  /* cause exit() here */
		return;
	}
	/** */
	ST_ASSERTN_VOID(pFldI == NULL)

	/* reset field data */
	pFldI->dataIVal = -1;
	pFldI->isOK     = ST_B_FALSE;

	/* read data from buffer */
	pFldI->dataIVal = (Tst_int32)*pSrcBuf;

	/* handle data */
	/** */
	switch (dstID) {
	case ST_ID3V1_FID_TRKN:
		if (pFldI->dataIVal == 0)
			pFldI->dataIVal = noneVal;
		break;
	case ST_ID3V1_FID_GENR:
		if (pFldI->dataIVal == 255)
			pFldI->dataIVal = noneVal;
		break;
	default:
		break;
	}
	/** */
	switch (dstIDext) {
	case ST_ID3V1E_FID_SPEED:
		if (pFldI->dataIVal < 1 || pFldI->dataIVal > 4)
			pFldI->dataIVal = noneVal;
		break;
	default:
		break;
	}
	/** */
	if (pFldI->dataIVal != noneVal) {
		pFldI->isOK = ST_B_TRUE;
		st_tagBas_gs_setHasFields(&pTagI->tbas, ST_B_TRUE);
	}

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v1_d_fdeb(&pTagI->opts, 1, pFnc, &pFldI->fldPrI,
				"%d%s", pFldI->dataIVal,
				(pFldI->dataIVal == noneVal ? " (==none)" : ""));
}

/******************************************************************************/
