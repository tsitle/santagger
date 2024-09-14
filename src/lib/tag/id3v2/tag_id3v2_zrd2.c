/******************************************************************************/
/* tag_id3v2_zrd2.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for reading ID3v2 Tags                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_ZRD2_C
#include "tag_id3v2_zrd2-pp.h"
/*** */
#include "tag_id3v2-prot.h"
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#include "tag_id3v2_stcs-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_ZRD2_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy(), ... */
#include <stdlib.h>      /* calloc(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Reads ID3v2 Tag field's values
 *
 * pTag        MUST be set
 * pFld        MUST be set
 * pStrrdFld   MUST be set
 */
Tst_err
st_id3v2_rd2_readFldData(Tst_id3v2_tag *pTag,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_streamrd *pStrrdFld, Tst_uint32 frSize)
{
	const char *cFNCN = __func__;
	Tst_err    res      = ST_ERR_SUCC;
	Tst_uint32 copied   = 0,
	           rwoX,
	           uncBufSz = 0,
	           rwoArrSz;   /* size of rwoArr (=6) */
	Tst_mtes_textEnc         te = ST_MTES_TE_ISO;
	Tst_iv2__readBuf         rb;
	Tst_id3v2_tag_intn       *pTagI;
	Tst_id3v2_fldd_strtp     strTp;
	Tst_id3v2_fldd_dattp     datTp;
	Tst_id3v2_fldd_rword_typ rwoArr[6];  /* order to read/write data in */

	/* ______________________________________________________________ */
	/* COMM,USLT:     te lang                      scd  text          */
	/* TXXX,WXXX:     te                           scd  text          */
	/* APIC:          te [PIC:imgfor|APIC:mime] pt desc      BINdata  */
	/* CM1,T000-TZZZ: te                                string(s)     */
	/* W000-WZZZ:                                       url           */
	/* UFID,PRIV:                                  own       BINdata  */
	/* IPLS:          te                                strpairs      */
	/* MCDI:                                                 BINdata  */
	/* GEO[B]:        te mime filen                scd       BINdata  */
	/* ______________________________________________________________ */
	/* CDM:     comprMeth uncomprSz comprBINdata                      */
	/* [P]CNT:  4bytes [byte ...]                                     */
	/* POP[M]:  email rating 4bytes [byte ...]                        */
	/* S[Y]LT:  te lang timestampfmt conttp contdesc BINdata          */
	/* ______________________________________________________________ */

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFldI == NULL || pStrrdFld == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	/**if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 0, cFNCN, pFldI,
				"tp %u: ...", pFldI->fldPrI.typ);**/

	if (frSize == 0) {
		/* shouldn't be empty */
		if (! pFldI->errs.frHasInvData) {
			pFldI->errs.frHasInvData = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
		return ST_ERR_SUCC;
	}

	/* set data/string type and order to read data in */
	st_id3v2_stc_rsetStrTp(&strTp);
	st_id3v2_stc_rsetDatTp(&datTp);
	rwoArrSz = sizeof(rwoArr) / sizeof(Tst_id3v2_fldd_rword_typ);
	st_id3v2_fnc_setDataNstrTp(pFldI->fldPrI.id, &datTp, &strTp,
			(Tst_id3v2_fldd_rwordPArr)rwoArr, rwoArrSz);

	/* */
	rb.pStrrdFld = pStrrdFld;
	rb.frSize    = frSize;

	/* parse data in order defined by rwoArr */
	rwoX = 0;
	while (rwoX < rwoArrSz && ! st_streamrd_isEOF(pStrrdFld) &&
			rwoArr[rwoX] != ST_ID3V2_TFLD_RWO_TP_NONE) {
		/* */
		switch (rwoArr[rwoX]) {
		case ST_ID3V2_TFLD_RWO_TP_TENC:  /* text encoding */
			res = ST_TIV2__rd2_fd_decTE(&pTagI->opts, cFNCN,
					&rb, pFldI, &te);
			break;
		case ST_ID3V2_TFLD_RWO_TP_LANG:  /* language */
			res = ST_TIV2__rd2_fd_decLang(&pTagI->opts, cFNCN,
					&rb, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_SCD:   /* short content desc, ... */
			/* read 'short content description' / 'description' /
			 *   'owner' / 'email' */
			/**st_id3v2_prf("copy scd string\n");**/
			if (rb.frSize > 0) {
				res = ST_TIV2__rd2_fd_copyStr(&pTagI->opts, cFNCN,
						ST_B_FALSE, te,
						&rb, rb.frSize,
						&pFldI->attrDesc, &copied, pFldI);
				if (res == ST_ERR_SUCC &&
						ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
					st_id3v2_d_fdeb3Str(&pTagI->opts, 1, cFNCN, pFldI,
							ST_B_TRUE, "scd", &pFldI->attrDesc, "");
			}
			break;
		case ST_ID3V2_TFLD_RWO_TP_TEXT:  /* text (single/multi string) */
			/* read 'text' / 'info' / 'url' / 'People list' */
			/**st_id3v2_prf("copy multi string\n");**/
			res = ST_TIV2__rd2_fd_decMultStr(&pTagI->opts, cFNCN,
					&rb, pFldI, &strTp, te);
			break;
		case ST_ID3V2_TFLD_RWO_TP_BDAT:  /* binary data */
			/* read 'picture data' / 'identifier' (max 64 bytes) /
			 *      'private data' / 'CD TOC' / 'content desc' / ... */
			res = ST_TIV2__rd2_fd_decBinDat(&pTagI->opts, cFNCN,
					&rb, pFldI, uncBufSz, te);
			break;
		case ST_ID3V2_TFLD_RWO_TP_UVAL:  /* unsigned int value */
			res = ST_TIV2__rd2_fd_decUVal(&pTagI->opts, cFNCN,
					&rb, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_BVAL:  /* unsigned byte value */
			res = ST_TIV2__rd2_fd_decBVal(&pTagI->opts, cFNCN,
					&rb, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_PICT:  /* [A]PIC imgfmt/mime + pic type */
			res = ST_TIV2__rd2_fd_decPic1(&pTagI->opts, cFNCN,
					&rb, pFldI, pTagI->verMaj);
			break;
		case ST_ID3V2_TFLD_RWO_TP_GEOB:  /* GEO[B] mime + filen */
			res = ST_TIV2__rd2_fd_decGeo1(&pTagI->opts, cFNCN,
					&rb, pFldI, te);
			break;
		case ST_ID3V2_TFLD_RWO_TP_CDMF:  /* CDM comprMeth + uncomprSz */
			res = ST_TIV2__rd2_fd_decCdm1(&pTagI->opts, cFNCN,
					&rb, pFldI, &uncBufSz);
			break;
		case ST_ID3V2_TFLD_RWO_TP_SYLT:  /* S[Y]LT timestampfmt + conttp */
			res = ST_TIV2__rd2_fd_decSlt1(&pTagI->opts, cFNCN,
					&rb, pFldI);
			break;
		default:
			res = ST_ERR_FAIL;
		}
		/* */
		if (res != ST_ERR_SUCC)
			break;
		++rwoX;
	}

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Decode and correct Text Encoding
 */
static Tst_err
ST_TIV2__rd2_fd_decTE(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI, Tst_mtes_textEnc *pTE)
{
	Tst_byte byt = 0;

	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
		return ST_ERR_IDAT;  /* we're missing something */

	/* */
	st_streamrd_rdByte_noCRC(pRB->pStrrdFld, 8, &byt);
	--pRB->frSize;
	/* */
	switch (byt) {
	case 0x00: *pTE = ST_MTES_TE_ISO; break;      /* ID3v2.(2,3,4) */
	case 0x01: *pTE = ST_MTES_TE_UTF16LE; break;  /* ID3v2.(2,3,4) */
	case 0x02: *pTE = ST_MTES_TE_UTF16BE; break;  /* ID3v2.4 */
	case 0x03: *pTE = ST_MTES_TE_UTF8; break;     /* ID3v2.4 */
	default:
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"invalid text-encoding, setting it to ISO");
		/* */
		*pTE = ST_MTES_TE_ISO;
		/* */
		if (! pFldI->errs.frHasInvTE) {
			pFldI->errs.frHasInvTE = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}
	return ST_ERR_SUCC;
}

/*
 * Read 3 byte language identifier
 */
static Tst_err
ST_TIV2__rd2_fd_decLang(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI)
{
	Tst_str    *pUCh     = (Tst_str*)pFldI->attrLang;
	Tst_uint32 rdFromStr = 0;

	if (pRB->frSize < 3)
		return ST_ERR_IDAT;  /* we're missing something */

	pFldI->attrLang[0] = 0x00;
	st_streamrd_rdBuffer_noCRC(pRB->pStrrdFld, 3,
			(Tst_buf*)pFldI->attrLang, &rdFromStr);
	if (rdFromStr != 3)
		return ST_ERR_IDAT;  /* we're missing something */
	pRB->frSize       -= rdFromStr;
	pFldI->attrLang[3] = 0x00;

	if (st_sysStrlen(pFldI->attrLang) < 2) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
				"invalid language, setting it to 'xxx'");
		/* reset 'language' entirely */
		pFldI->attrLang[0] = 'x';
		pFldI->attrLang[1] = 'x';
		pFldI->attrLang[2] = 'x';
		/* */
		if (! pFldI->errs.frHasInvLng) {
			pFldI->errs.frHasInvLng = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
		return ST_ERR_SUCC;
	}

	/* convert to lower case + elimate invalid chars */
	st_sysStrToLower((Tst_str*)pFldI->attrLang);
	while (*pUCh) {
		/* only a-z and A-Z are allowed */
		if (! ((*pUCh >= ST_CSET_ISO_ACAP && *pUCh <= ST_CSET_ISO_ZCAP) ||
				(*pUCh >= ST_CSET_ISO_ASMA && *pUCh <= ST_CSET_ISO_ZSMA))) {
			*pUCh = ST_CSET_ISO_SPA;
			/* */
			if (! pFldI->errs.frHasInvLng) {
				if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
						"invalid language, modifying it");
				/* */
				pFldI->errs.frHasInvLng = ST_B_TRUE;
				++pFldI->frErrCnt;
				break;
			}
		}
		++pUCh;
	}
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
				"lng '%s'", (Tst_str*)&pFldI->attrLang);
	return ST_ERR_SUCC;
}

/*
 * Read binary data
 */
static Tst_err
ST_TIV2__rd2_fd_decBinDat(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI,
		const Tst_uint32 uncBufSz, const Tst_mtes_textEnc sltTE)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_bool   isLE   = ST_B_FALSE;
	Tst_buf    *pTmpB = NULL;
	Tst_uint32 binDatSz;

	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
		return ST_ERR_SUCC;  /* nothing left to read */

	if (pFldI->fldPrI.typ == ST_ID3V2_FTP_CDM && uncBufSz > 0) {
		/* decompress data */
		res = ST_TIV2__rd2_fd_cdm_decmpr(pOpts, pFnc,
				pRB, pFldI, uncBufSz);
	} else {
		/* copy bin data */
		/** */
		res = st_binobj_setData_streamrd(&pFldI->dataBinDat,
				pRB->pStrrdFld, pRB->frSize, ST_B_FALSE);
		/** */
		pRB->frSize = 0;
		binDatSz    = st_binobj_getDataSize(&pFldI->dataBinDat);
		/* */
		if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts)) {
			if (binDatSz < 1024)
				st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
						"bin %u bytes", binDatSz);
			else if (binDatSz < 1024 * 1024)
				st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
						"bin %u bytes (%.2f KB)",
						binDatSz, (double)binDatSz / 1024.0);
			else
				st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
						"bin %u bytes (%u MB)",
						binDatSz,
						(Tst_uint32)((double)binDatSz / (1024.0 * 1024.0)));
		}
		/* */
		if (res == ST_ERR_SUCC && pFldI->fldPrI.typ == ST_ID3V2_FTP_SLT) {
			/* synced lyrics, bin -> txt */
			res = ST_TIV2__rd2_fd_slt_b2t(pOpts, pFnc, pFldI, sltTE);
		} else if (res == ST_ERR_SUCC &&
				binDatSz > 4 && binDatSz <= 4096 &&
				(pFldI->fldPrI.typ == ST_ID3V2_FTP_UFI ||
					pFldI->fldPrI.typ == ST_ID3V2_FTP_MCI)) {
			/* re-copy data from binary object to temp. buffer */
			binDatSz = 0;
			res      = st_binobj_copyIntoBuffer(&pFldI->dataBinDat, &pTmpB, &binDatSz);
			/* check if binary data is ISO/UTF16-encoded text */
			if (st_mtes_testIfIsUTF16(pTmpB, binDatSz, &isLE))
				res = ST_TIV2__rd2_fd_UfiMci_b2t(pOpts, pFnc, pFldI,
						(isLE ? ST_MTES_TE_UTF16LE : ST_MTES_TE_UTF16BE),
						pTmpB, binDatSz);
			else if (st_mtes_testIfIsASCII(pTmpB, binDatSz))
				res = ST_TIV2__rd2_fd_UfiMci_b2t(pOpts, pFnc,
						pFldI, ST_MTES_TE_ISO,
						pTmpB, binDatSz);
			ST_DELPOINT(pTmpB)
		}
	}

	pRB->frSize = 0;  /* !!! */
	return res;
}

/*
 * Read unsigned int value
 */
static Tst_err
ST_TIV2__rd2_fd_decUVal(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI)
{
	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize < 4) {
		if (pFldI->fldPrI.id == ST_ID3V2_FID_234_POPM)
			return ST_ERR_SUCC;
		return ST_ERR_IDAT;  /* we're missing something */
	}

	/* play counter with >4 bytes is possible but very unlikely,
	 *   so if it's longer we'll simply ignore it */
	if (pRB->frSize == 4) {
		st_streamrd_rdUInt32(pRB->pStrrdFld,
				ST_STREAMRD_IEND_BE, 32, &pFldI->dataUVal);  /* ENDIAN: BE-->HOST */
	} else {
		st_streamrd_rdSkipBytes(pRB->pStrrdFld, pRB->frSize, ST_B_TRUE);
	}
	pRB->frSize = 0;
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
				"uva %u", pFldI->dataUVal);
	return ST_ERR_SUCC;
}

/*
 * Read unsigned byte value
 */
static Tst_err
ST_TIV2__rd2_fd_decBVal(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI)
{
	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
		return ST_ERR_IDAT;  /* we're missing something */

	/* rating (1 byte) */
	st_streamrd_rdByte_noCRC(pRB->pStrrdFld, 8, &pFldI->dataBVal);
	--pRB->frSize;
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
				"byt %u", (Tst_uint32)pFldI->dataBVal);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read mime type  (stripped and lowercase)
 *
 * maxlen == 0 means infinite length
 */
static Tst_err
ST_TIV2__rd2_fd_mimeOrPicFmt(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_iv2__readBuf *pRB, const Tst_mtes_strRestr *pRestr,
		Tst_str **ppStr, const Tst_uint32 maxlen)
{
	Tst_err    res;
	Tst_bool   hic    = ST_B_FALSE;
	Tst_uint32 copied = 0,
	           x;
	Tst_mtes_string strTemp;

	res = st_mtes_stc_initStr(&strTemp);
	if (res != ST_ERR_SUCC)
		return res;
	res = ST_TIV2__rd2_fd_copyStr(pOpts, pFnc, ST_B_FALSE, ST_MTES_TE_ISO,
			pRB, maxlen,
			&strTemp, &copied, pFldI);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&strTemp);
		return res;
	}
	/* */
	res = st_mtes_stripStr(&strTemp, pRestr, &hic, NULL);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&strTemp);
		return res;
	}
	/* set error flag and update counter */
	if (hic && ! pFldI->errs.frHasInvChars) {
		pFldI->errs.frHasInvChars = ST_B_TRUE;
		++pFldI->frErrCnt;
	}
	/* */
	if (st_mtes_getDataPt(&strTemp) == NULL || st_mtes_getStringSize(&strTemp) < 1)
		ST_DELPOINT((*ppStr))
	else {
		/* copy string to *ppStr and convert to lowercase */
		ST_REALLOC(*ppStr, Tst_str*, st_mtes_getStringSize(&strTemp) + 1, 1)
		if (*ppStr == NULL) {
			st_mtes_stc_freeStr(&strTemp);
			return ST_ERR_OMEM;
		}
		for (x = 0; x < st_mtes_getStringSize(&strTemp); x++)
			(*ppStr)[x] = st_mtes_getDataPt(&strTemp)[x];
		(*ppStr)[st_mtes_getStringSize(&strTemp)] = 0x00;
		st_sysStrToLower(*ppStr);
	}
	st_mtes_stc_freeStr(&strTemp);
	return ST_ERR_SUCC;
}

/*
 * Read picture stuff (image format/mime, fileextension, pict. type)
 */
static Tst_err
ST_TIV2__rd2_fd_decPic1(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI,
		const Tst_byte verMaj)
{
	Tst_err  res;
	Tst_str  *pImfFmtS = NULL,
	         *pPicMime = NULL;
	Tst_byte byt       = 0;
	Tst_mtes_strRestr restr;

	restr.hasR        = ST_B_TRUE;  /* we have restrictions */
	restr.ruleAlw     = ST_B_TRUE;  /* mode: allow */
	st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
	restr.disNotPrint = ST_B_TRUE;  /* don't allow not-printable chars */
	restr.strMaxl     = ST_ID3V2_MAX_STRLEN_M * 1024 * 1024;

	/* image format / mime type */
	if (verMaj == 2) {
		/* read 'image format' as ISO encoded string (3 bytes) */
		if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize < 3)
			return ST_ERR_IDAT;  /* we're missing something */
		res = ST_TIV2__rd2_fd_mimeOrPicFmt(pOpts, pFnc,
				pFldI, pRB,
				&restr, &pImfFmtS, 3);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pImfFmtS)
			return res;
		}
		/* set picture format */
		if (pImfFmtS != NULL) {
			pFldI->attrPicFmt = st_utilsFmt_getPicFmt_enum_byFExt(pImfFmtS);
			ST_DELPOINT(pImfFmtS)
		}
	} else {
		/* read 'mime type' as ISO encoded string */
		if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
			return ST_ERR_IDAT;  /* we're missing something */
		res = ST_TIV2__rd2_fd_mimeOrPicFmt(pOpts, pFnc,
				pFldI, pRB,
				&restr, &pPicMime, 0);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pPicMime)
			return res;
		}
		/* set picture format */
		if (pPicMime != NULL) {
			pFldI->attrPicFmt = st_utilsFmt_getPicFmt_enum_byMime(pPicMime);
			ST_DELPOINT(pPicMime)
		}
	}
	/* check picture format */
	if (st_utilsFmt_getPicFmt_fext_byEnum(pFldI->attrPicFmt) == NULL) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"unknown picture format, setting it to JPG");
		/* */
		pFldI->attrPicFmt = ST_UTILSFMT_PF_JPG;
		/* */
		if (! pFldI->errs.frHasInvPF) {
			pFldI->errs.frHasInvPF = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}

	/* picture type (1 byte) */
	/** */
	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
		return ST_ERR_IDAT;  /* we're missing something */
	st_streamrd_rdByte_noCRC(pRB->pStrrdFld, 8, &byt);
	--pRB->frSize;
	/** */
	pFldI->attrPicTp = st_id3v2_fnc_getPicTp_enum_byBin(byt);
	if (pFldI->attrPicTp == ST_ID3V2_PT_NONE) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"invalid picture type 0x%02x, setting it to 'other'",
					byt);
		pFldI->attrPicTp = ST_ID3V2_PT_OTHER;
		/* */
		if (! pFldI->errs.frHasInvPT) {
			pFldI->errs.frHasInvPT = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}

	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts)) {
		if (verMaj == 2)
			st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
					"fmt='%s',  ptp='%s'",
					st_utilsFmt_getPicFmt_fext_byEnum(pFldI->attrPicFmt),
					st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp));
		else
			st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
					"mim=\"%s\",  ptp='%s'",
					st_utilsFmt_getPicFmt_mime_byEnum(pFldI->attrPicFmt),
					st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp));
	}
	return ST_ERR_SUCC;
}

/*
 * Read GEOB stuff (mime, filename)
 */
static Tst_err
ST_TIV2__rd2_fd_decGeo1(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI,
		const Tst_mtes_textEnc te)
{
	Tst_err    res;
	Tst_uint32 copied = 0;
	Tst_mtes_strRestr restr;

	restr.hasR        = ST_B_TRUE;  /* we have restrictions */
	restr.ruleAlw     = ST_B_TRUE;  /* mode: allow */
	st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
	restr.disNotPrint = ST_B_TRUE;  /* don't allow not-printable chars */
	restr.strMaxl     = ST_ID3V2_MAX_STRLEN_M * 1024 * 1024;

	/* read 'mime type' as ISO encoded string */
	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
		return ST_ERR_IDAT;  /* we're missing something */
	res = ST_TIV2__rd2_fd_mimeOrPicFmt(pOpts, pFnc,
			pFldI, pRB,
			&restr, &pFldI->pAttrGeoMime, 0);
	if (res != ST_ERR_SUCC)
		return res;
	if (pFldI->pAttrGeoMime == NULL) {
		ST_CALLOC(pFldI->pAttrGeoMime, Tst_str*, 1, 1)
		pFldI->pAttrGeoMime[0] = 0;
	}

	/* read 'filename' */
	/**st_id3v2_prf("copy single string\n");**/
	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
		return ST_ERR_IDAT;  /* we're missing something */
	res = ST_TIV2__rd2_fd_copyStr(pOpts, pFnc,
			ST_B_FALSE, te,
			pRB, pRB->frSize,
			&pFldI->attrGeoFn, &copied, pFldI);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts)) {
		st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
				"mim=\"%s\"", pFldI->pAttrGeoMime);
		/* */
		st_id3v2_d_fdeb3Str(pOpts, 1, pFnc, pFldI,
				ST_B_TRUE, "fna", &pFldI->attrGeoFn, "");
	}
	return ST_ERR_SUCC;
}

/*
 * Read CDM stuff (compression method, uncomressed size)
 */
static Tst_err
ST_TIV2__rd2_fd_decCdm1(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_uint32 *pUncBufSz)
{
	Tst_byte byt = 0;

	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize < 5)
		return ST_ERR_IDAT;  /* we're missing something */

	if (pOpts == NULL || pOpts->cbZLibDecmpr == NULL) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"no zlib callback set for decompression");
		return ST_ERR_IDAT;
	}
	/* eval compression method */
	st_streamrd_rdByte_noCRC(pRB->pStrrdFld, 8, &byt);
	--pRB->frSize;
	if ((char)byt != 'z') {
		/* invalid compression method */
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"invalid compression method");
		/* */
		if (! pFldI->errs.frHasInvComprM) {
			pFldI->errs.frHasInvComprM = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
		return ST_ERR_IDAT;
	}
	/* read uncomressed size */
	st_streamrd_rdUInt32(pRB->pStrrdFld,
			ST_STREAMRD_IEND_BE, 32, pUncBufSz);  /* ENDIAN: BE-->HOST */
	pRB->frSize -= 4;
	return ST_ERR_SUCC;
}

/*
 * Read SYLT stuff (time stamp format, content type)
 */
static Tst_err
ST_TIV2__rd2_fd_decSlt1(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI)
{
	Tst_byte byt = 0;

	if (st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize < 2)
		return ST_ERR_IDAT;  /* we're missing something */

	/* time stamp format (1 byte) */
	/** */
	st_streamrd_rdByte_noCRC(pRB->pStrrdFld, 8, &byt);
	--pRB->frSize;
	/** */
	switch (byt) {
	case 0x01: pFldI->attrSltTSF = ST_ID3V2_SLTTSF_MPEG; break;
	case 0x02: pFldI->attrSltTSF = ST_ID3V2_SLTTSF_MILLI; break;
	default:
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"invalid SYLT timestamp-format, setting it to 'millisecs'");
		/* */
		pFldI->attrSltTSF = ST_ID3V2_SLTTSF_MILLI;
		/* */
		if (! pFldI->errs.frHasInvSLTtsf) {
			pFldI->errs.frHasInvSLTtsf = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}

	/* content type (1 byte) */
	/** */
	st_streamrd_rdByte_noCRC(pRB->pStrrdFld, 8, &byt);
	--pRB->frSize;
	/** */
	switch (byt) {
	case 0x00: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_OTHER; break;
	case 0x01: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_LYR; break;
	case 0x02: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_TRANS; break;
	case 0x03: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_MOVE; break;
	case 0x04: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_EVENT; break;
	case 0x05: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_CHORD; break;
	case 0x06: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_TRIV; break;
	case 0x07: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_URLWP; break;
	case 0x08: pFldI->attrSltCTP = ST_ID3V2_SLTCTP_URLIMG; break;
	default:
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"invalid SYLT content-type, setting it to 'other'");
		/* */
		pFldI->attrSltCTP = ST_ID3V2_SLTCTP_OTHER;
		/* */
		if (! pFldI->errs.frHasInvSLTctp) {
			pFldI->errs.frHasInvSLTctp = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}

	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
				"tsf %d, ctp %d", pFldI->attrSltTSF, pFldI->attrSltCTP);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read compressed data from stream and decompress it
 */
static Tst_err
ST_TIV2__rd2_fd_cdm_decmpr(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI,
		const Tst_uint32 uncBufSz)
{
	Tst_err res;
	Tst_binobj boTmp;

	if (uncBufSz < 1 || st_streamrd_isEOF(pRB->pStrrdFld) || pRB->frSize == 0)
		return ST_ERR_SUCC;

	if (pOpts == NULL || pOpts->cbZLibDecmpr == NULL) {
		/* can't decompress */
		st_binobj_stc_rsetData(&pFldI->dataBinDat);
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"no zlib callback set for decompression");
		return ST_ERR_IDAT;
	}

	/* read from stream */
	st_binobj_stc_initBO(&boTmp);
	res = st_binobj_setData_streamrd(&boTmp,
			pRB->pStrrdFld, pRB->frSize, ST_B_FALSE);
	if (res != ST_ERR_SUCC) {
		st_binobj_stc_freeBO(&boTmp);
		return res;
	}
	pRB->frSize = 0;

	/* decompress */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
				"decompressing meta frame...");
	res = pOpts->cbZLibDecmpr(&boTmp, uncBufSz, &pFldI->dataBinDat);
	st_binobj_stc_freeBO(&boTmp);
	if (res != ST_ERR_SUCC) {
		st_binobj_stc_rsetData(&pFldI->dataBinDat);
		/* couldn't decompress */
		if (! pFldI->errs.frDatWasntDcmpr) {
			pFldI->errs.frDatWasntDcmpr = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
		return ST_ERR_IDAT;
	}
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 1, pFnc, pFldI,
				"dcb %u bytes", st_binobj_getDataSize(&pFldI->dataBinDat));
	return ST_ERR_SUCC;
}

/*
 * SYLT: Convert binary data into text
 */
static Tst_err
ST_TIV2__rd2_fd_slt_b2t(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_id3v2_fldData_intn *pFldI,
		const Tst_mtes_textEnc te)
{
	Tst_err    res;
	Tst_uint32 copied = 1,
	           slen   = 0,
	           pchPos = 0,
	           cwid,
	           pchSz,
	           tsUI   = 0,
	           tsMin,
	           tsSec;
	Tst_str    *pCh   = NULL,
	           tsStr[64];
	Tst_bool   hasNL;
	Tst_mtes_string  tmps;
	Tst_streamrd     strrdBD;
	Tst_iv2__readBuf rbTmp;

	st_mtes_stc_rsetSA(&pFldI->dataSArr);
	if (! st_mtes_isTextEncValid(te))
		return ST_ERR_FAIL;

	cwid         = st_mtes_getCharWidth(te);
	rbTmp.frSize = st_binobj_getDataSize(&pFldI->dataBinDat);

	pchSz = rbTmp.frSize + (1024 - (rbTmp.frSize % 1024));
	ST_CALLOC(pCh, Tst_str*, pchSz, 1)
	if (pCh == NULL)
		return ST_ERR_OMEM;
	pCh[0] = 0x00;

	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pCh)
		return res;
	}

	st_streamrd_stc_initSObj(&strrdBD);
	res = st_binobj_attachStreamrd(&pFldI->dataBinDat, &strrdBD, 0);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pCh)
		st_mtes_stc_freeStr(&tmps);
		st_streamrd_stc_freeSObj(&strrdBD);
		return res;
	}
	rbTmp.pStrrdFld = &strrdBD;

	while (copied > 0 && rbTmp.frSize > 0) {
		/* read text fragment */
		/**st_id3v2_prf("copy single string\n");**/
		copied = 0;
		res    = ST_TIV2__rd2_fd_copyStr(pOpts, pFnc,
				ST_B_TRUE, te,
				&rbTmp, rbTmp.frSize,
				&tmps, &copied, NULL);
		if (res != ST_ERR_SUCC || copied < cwid || rbTmp.frSize < 4)
			break;
		hasNL = (st_mtes_getDataPt(&tmps)[0] == '\n');

		/* read time stamp */
		res = st_streamrd_rdUInt32(rbTmp.pStrrdFld,
				ST_STREAMRD_IEND_BE, 32, &tsUI);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			break;
		rbTmp.frSize -= (rbTmp.frSize >= 4 ? 4 : rbTmp.frSize);

		/* append timestamp to pCh
		 *   either [#xxxxxxxxxx] or [mm:ss.dd]  */
		if (pchPos + 16 > pchSz) {
			pchSz = pchPos + 16 + (1024 - ((pchPos + 16) % 1024));
			ST_REALLOC(pCh, Tst_str*, pchSz, 1)
			if (pCh == NULL)
				return ST_ERR_OMEM;
		}
		if (pFldI->attrSltTSF == ST_ID3V2_SLTTSF_MPEG)
			sprintf((char*)tsStr, "%s[#%u]", (hasNL ? "\n" : ""), tsUI);
		else {
			tsSec = 0;
			tsMin = tsUI / (1000 * 60);
			if (tsMin * (1000 * 60) < tsUI) {
				tsSec = (tsUI - (tsMin * (1000 * 60))) / 1000;
				if ((tsMin * (1000 * 60)) + (tsSec * 1000) < tsUI)
					tsUI = (tsUI - (tsMin * (1000 * 60)) - (tsSec * 1000)) / 10;
				else
					tsUI = 0;
			} else
				tsUI = 0;
			sprintf((char*)tsStr, "%s[%02u:%02u.%02u]",
					(hasNL ? "\n" : ""), tsMin, tsSec, tsUI);
		}
		slen = st_sysStrlen(tsStr);
		memcpy(&pCh[pchPos], tsStr, slen);
		pchPos += slen;

		/* append text fragment to pCh */
		slen = st_mtes_strlenBy(&tmps);
		if (slen == 0)
			continue;
		if (te != ST_MTES_TE_ISO && te != ST_MTES_TE_UTF8) {
			res = st_mtes_convStrTo_utf8(&tmps);
			if (res != ST_ERR_SUCC)
				break;
		}
		/**if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3Str(pOpts, 2, 1, pFnc, pFldI,
					ST_B_TRUE, "stm", &tmps, "");**/
		if (pchPos + slen + 1 > pchSz) {
			pchSz = pchPos + slen + 1 + (1024 - ((pchPos + slen + 1) % 1024));
			ST_REALLOC(pCh, Tst_str*, pchSz, 1)
			if (pCh == NULL)
				return ST_ERR_OMEM;
		}
		if (hasNL) {
			if (slen > 1)
				memcpy(&pCh[pchPos], &st_mtes_getDataPt(&tmps)[1], slen - 1);
			--slen;
		} else
			memcpy(&pCh[pchPos], st_mtes_getDataPt(&tmps), slen);
		pchPos += slen;
	}

	st_mtes_stc_rsetStr(&tmps, ST_B_TRUE, ST_B_TRUE);
	st_binobj_detachStreamrd(&pFldI->dataBinDat);
	st_streamrd_stc_freeSObj(&strrdBD);
	/* */
	if (res == ST_ERR_SUCC && pchPos > 0) {
		/* remove leading NEWLINEs */
		slen      = pchPos;
		pCh[slen] = 0x00;
		while (pCh[0] == '\n' && slen > 0) {
			memmove(pCh, &pCh[1], slen);
			--slen;
		}
		/* remove trailing NEWLINEs */
		while (slen > 0 && pCh[slen - 1] == '\n') {
			pCh[slen - 1] = 0x00;
			--slen;
		}
		if (slen > 0)
			res = st_mtes_copyFromCharp(pCh, slen + 1, te, &tmps);
	}
	ST_DELPOINT(pCh)
	/* */
	if (res == ST_ERR_SUCC && slen > 0)
		res = st_mtes_strArrAdd(&pFldI->dataSArr, &tmps);
	st_mtes_stc_freeStr(&tmps);

	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts) &&
			st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0)
		st_id3v2_d_fdeb3Str(pOpts, 1, pFnc, pFldI,
				ST_B_TRUE, "slt",
				st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
	return res;
}

/*
 * UFID/PRIV/MCDI: Convert binary data into text
 */
static Tst_err
ST_TIV2__rd2_fd_UfiMci_b2t(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_id3v2_fldData_intn *pFldI,
		const Tst_mtes_textEnc te,
		const Tst_buf *pBuf, const Tst_uint32 bufSz)
{
	Tst_err    res  = ST_ERR_SUCC;
	Tst_str    *pCh = NULL;
	Tst_uint32 chSz;
	Tst_mtes_string tmps;

	chSz = bufSz + 2;
	ST_CALLOC(pCh, Tst_str*, chSz, 1)
	if (pCh == NULL)
		return ST_ERR_OMEM;
	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pCh)
		return res;
	}

	memcpy(pCh, pBuf, bufSz);
	pCh[chSz - 1] = 0;
	pCh[chSz - 2] = 0;

	res = st_mtes_copyFromCharp(pCh, chSz, te, &tmps);
	ST_DELPOINT(pCh)
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmps);
		return res;
	}

	if (te != ST_MTES_TE_ISO)
		res = st_mtes_convStr(&tmps, ST_MTES_TE_UTF8);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strArrAdd(&pFldI->dataSArr, &tmps);
	if (res == ST_ERR_SUCC &&
			st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
			ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3Str(pOpts, 1, pFnc, pFldI,
				ST_B_TRUE, "cvb", st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
	st_mtes_stc_freeStr(&tmps);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Copy multiple NUL-terminated strings
 *   (last string doesn't have to be NUL-term.)
 */
static Tst_err
ST_TIV2__rd2_fd_decMultStr(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_iv2__readBuf *pRB,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_id3v2_fldd_strtp *pStrTp,
		const Tst_mtes_textEnc textEnc)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 copied = 0,
	           x;
	Tst_mtes_string tmpstr;

	st_mtes_stc_rsetSA(&pFldI->dataSArr);
	res = st_mtes_stc_initStr(&tmpstr);
	if (res != ST_ERR_SUCC)
		return res;
	while (pRB->frSize > 0) {
		/* read one string */
		res = ST_TIV2__rd2_fd_copyStr(pOpts, pFnc,
				pStrTp->hasFullStr, textEnc,
				pRB, pRB->frSize,
				&tmpstr, &copied, pFldI);
		if (res != ST_ERR_SUCC)
			break;
		if (copied < 1)
			break;
		/* */
		if (! pStrTp->hasMappedStr && st_mtes_isStrEmptyCh(&tmpstr))
			continue;
		/* */
		if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) >= ST_ID3V2_MAX_MSTR_CNT) {
			if (! pFldI->errs.frTooManyMS &&
					ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
				st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
						"too many strings in multi-string (max %u)",
						ST_ID3V2_MAX_MSTR_CNT);
			if (! pFldI->errs.frTooManyMS) {
				pFldI->errs.frTooManyMS = ST_B_TRUE;
				++pFldI->frErrCnt;
			}
		} else {
			res = st_mtes_strArrAdd(&pFldI->dataSArr, &tmpstr);
			if (res != ST_ERR_SUCC)
				break;
		}
	}
	st_mtes_stc_freeStr(&tmpstr);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts) &&
			st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
			! st_id3v2_fnc_isTaggInfFr(pFldI->fldPrI.id, pFldI->fldPrI.pIDstr,
						(st_sysStrlen2(pFldI->fldPrI.pIDstr) == 3 ? 2 : 3))) {
		if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) == 1 &&
				! pStrTp->hasMappedStr)
			st_id3v2_d_fdeb3Str(pOpts, 1, pFnc, pFldI,
					ST_B_TRUE, "txt",
					st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
		else
			for (x = 1; x <= st_mtes_strArrGetElemCount(&pFldI->dataSArr); x++)
				st_id3v2_d_fdeb3Str(pOpts, 1, pFnc, pFldI,
						ST_B_TRUE, "mul",
						st_mtes_strArrGetElem(&pFldI->dataSArr, x), "");
	}
	/* */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy UTF16 string including trailing NUL-Term
 */
static Tst_err
ST_TIV2__rd2_fd_copyStrU16(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_mtes_textEnc textEnc,
		Tst_iv2__readBuf *pRB, const Tst_uint32 maxCopy,
		Tst_mtes_string *pMStr, Tst_uint32 *pCopied,
		Tst_id3v2_fldData_intn *pFldI)
{
	const Tst_uint32 cWID = 2;
	Tst_err    res;
	Tst_uint32 pchLen   = 0;
	Tst_bool   hasBOM   = ST_B_FALSE,
	           bomCorrd = ST_B_FALSE;
	Tst_str    *pCh     = NULL;

	ST_ASSERTN_IARG(maxCopy > pRB->frSize ||
			(textEnc != ST_MTES_TE_UTF16LE && textEnc != ST_MTES_TE_UTF16BE))

	st_mtes_stc_rsetStr(pMStr, ST_B_TRUE, ST_B_TRUE);
	*pCopied = 0;

	if (pRB->frSize == 0 || maxCopy == 0)
		return st_mtes_setStrTo2(textEnc, "", pMStr);

	/* copy string from pStrrdFld */
	/** */
	res = st_tagCFnc_readStr(cWID, textEnc, pRB->pStrrdFld,
			maxCopy, pCopied, &pCh, &pchLen, &hasBOM);
	/** */
	pRB->frSize -= (pRB->frSize >= *pCopied ? *pCopied : pRB->frSize);
	/** */
	if (res != ST_ERR_SUCC)
		return res;

	/* check BOM */
	/** */
	if (hasBOM) {
		if (pCh[0] == ST_UTF16_BOM_BE[0] &&
				pCh[1] == ST_UTF16_BOM_BE[1]) {
			if (textEnc != ST_MTES_TE_UTF16BE) {
				if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
							"correcting UTF16 encoding from LE -> BE");
				/* */
				textEnc  = ST_MTES_TE_UTF16BE;
				bomCorrd = ST_B_TRUE;
			} else {
				if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
							"UTF16-BE string shouldn't have had BOM");
				/* */
				bomCorrd = ST_B_TRUE;
			}
			hasBOM = ST_B_TRUE;
		} else if (pCh[0] == ST_UTF16_BOM_LE[0] &&
				pCh[1] == ST_UTF16_BOM_LE[1]) {
			if (textEnc != ST_MTES_TE_UTF16LE) {
				if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
							"correcting UTF16 encoding from BE -> LE");
				/* */
				textEnc  = ST_MTES_TE_UTF16LE;
				bomCorrd = ST_B_TRUE;
			}
			hasBOM = ST_B_TRUE;
		}
		/* replace Byte-Order-Marker */
		if (bomCorrd) {
			if (textEnc == ST_MTES_TE_UTF16LE)
				memcpy(pCh, &ST_UTF16_BOM_LE, cWID);
			else
				memcpy(pCh, &ST_UTF16_BOM_BE, cWID);
		}
	} else if (textEnc == ST_MTES_TE_UTF16LE) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"UTF16-LE string should have had BOM");
		/* */
		bomCorrd = ST_B_TRUE;
	}
	/** */
	if (bomCorrd && ! pFldI->errs.frCorrdU16) {
		pFldI->errs.frCorrdU16 = ST_B_TRUE;
		++pFldI->frErrCnt;
	}

	/* set pMStr */
	res = st_mtes_copyFromCharp(pCh, pchLen + cWID, textEnc, pMStr);
	ST_DELPOINT(pCh)
	return res;
}

/*
 * Copy ISO-8859-1 / UTF8 string including NUL-Term
 */
static Tst_err
ST_TIV2__rd2_fd_copyStrIU8(const Tst_id3v2_opts *pOpts,
		Tst_mtes_textEnc textEnc,
		Tst_iv2__readBuf *pRB, const Tst_uint32 maxCopy,
		Tst_mtes_string *pMStr, Tst_uint32 *pCopied)
{
	const Tst_uint32 cWID = 1;
	Tst_err    res;
	Tst_str    *pCh   = NULL;
	Tst_uint32 pchLen = 0;

	ST_ASSERTN_IARG(maxCopy > pRB->frSize)

	st_mtes_stc_rsetStr(pMStr, ST_B_TRUE, ST_B_TRUE);
	*pCopied = 0;

	if (textEnc != ST_MTES_TE_UTF8)
		textEnc = ST_MTES_TE_ISO;
	if (pRB->frSize == 0 || maxCopy == 0)
		return st_mtes_setStrTo2(textEnc, "", pMStr);

	/* copy string from pStrrdFld */
	/** */
	res = st_tagCFnc_readStr(cWID, textEnc, pRB->pStrrdFld,
			maxCopy, pCopied, &pCh, &pchLen, NULL);
	/** */
	pRB->frSize -= (pRB->frSize >= *pCopied ? *pCopied : pRB->frSize);

	/* */
	if (res == ST_ERR_SUCC)
		res = st_mtes_copyFromCharp(pCh, pchLen + cWID, textEnc, pMStr);
	ST_DELPOINT(pCh)
	return res;
}

/*
 * Copy string including NUL-Term
 *   but without useless trailing whitespaces
 */
static Tst_err
ST_TIV2__rd2_fd_copyStr(const Tst_id3v2_opts *pOpts, const char *pFnc,
		const Tst_bool newLnAllowed, Tst_mtes_textEnc textEnc,
		Tst_iv2__readBuf *pRB, const Tst_uint32 maxlen,
		Tst_mtes_string *pMStr, Tst_uint32 *pCopied,
		Tst_id3v2_fldData_intn *pFldI)
{
#	define LOC_MAXLEN_  (maxlen == 0 ? pRB->frSize : \
				(pRB->frSize >= maxlen ? maxlen : pRB->frSize))
	Tst_err  res;
	Tst_bool hic = ST_B_FALSE;
	Tst_mtes_strRestr restr;

	st_mtes_stc_rsetStr(pMStr, ST_B_TRUE, ST_B_TRUE);
	if (pRB->frSize == 0)
		return st_mtes_setStrTo2(textEnc, "", pMStr);
	st_mtes_stc_rsetRestr(&restr, ST_B_FALSE);
	restr.strMaxl = ST_ID3V2_MAX_STRLEN_M * 1024 * 1024;

	/* copy string */
	switch (textEnc) {
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		/* ... as UTF16 encoded string */
		res = ST_TIV2__rd2_fd_copyStrU16(pOpts, pFnc,
				textEnc,
				pRB, LOC_MAXLEN_,
				pMStr, pCopied, pFldI);
		break;
	default:
		/* ... as ISO / UTF8 encoded string */
		res = ST_TIV2__rd2_fd_copyStrIU8(pOpts, textEnc,
				pRB, LOC_MAXLEN_,
				pMStr, pCopied);
	}
	if (res != ST_ERR_SUCC)
		return res;

	/* strip string */
	if (! st_mtes_isStrEmptyBy(pMStr)) {
		restr.hasR     = ST_B_TRUE;  /* we have restrictions */
		restr.ruleAlw  = ST_B_TRUE;  /* mode: allow */
		st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
		restr.disNewln = ! newLnAllowed;  /* allow newline ? */
		restr.disTab   = ! newLnAllowed;  /* allow tab ? */
		restr.strMaxl  = ST_ID3V2_MAX_STRLEN_M * 1024 * 1024;
		if ((res = st_mtes_stripStr(pMStr, &restr, &hic, NULL)) != ST_ERR_SUCC)
			return res;
		/* set error flag and update counter */
		if (hic && ! pFldI->errs.frHasInvChars) {
			pFldI->errs.frHasInvChars = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}
	return ST_ERR_SUCC;
#	undef LOC_MAXLEN_
}

/******************************************************************************/
