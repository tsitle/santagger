/******************************************************************************/
/* tag_vorbc_zwr.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for writing Vorbis Tags                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamwr.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_TAG_VORBC_ZWR_C
#include "tag_vorbc_zwr-pp.h"
/*** */
#include "tag_vorbc-prot.h"
#include "tag_vorbc_fnc-prot.h"
#include "tag_vorbc_zfdeb-prot.h"
#include "tag_vorbc_stcs-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#include "../../utils/w-base64-prot.h"
#undef SRC_TAG_VORBC_ZWR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Renders a Vorbis Tag
 *
 * returns: ERRCODE
 *          pTBasNew and pTagBO  will be set
 */
Tst_err
st_vorbc_wr_rendTagToBO(Tst_vorbc_tag *pTag,
		Tst_tagBasics *pTBasNew, Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 numFlds,
	           vendLen = 0;
	Tst_mtes_string    taggStr;
	Tst_vorbc_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTBasNew == NULL || pTagBO == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

	st_tagBas_gs_setSize(pTBasNew, 0);

	/* add Tagger field */
	st_mtes_stc_initStr(&taggStr);
	st_tagCFnc_getTaggerStr(&taggStr, ST_B_FALSE);
	res = st_vorbc_fldAdd_text(pTag, ST_VORBC_FID_TAGG, NULL, &taggStr);
	st_mtes_stc_freeStr(&taggStr);

	/* count valid fields */
	numFlds = st_vorbc_gs_getTag_amountValidFlds(pTag);

	/* rendering preliminary header */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering preliminary header...");
	res = ST_TVORBC__rend_head(pTagI, 0, pTBasNew, ST_B_FALSE,
			&vendLen, pTagBO);
	if (res != ST_ERR_SUCC)
		return res;

	/* render all fields */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering fields for Vorbis tag...");
	res = ST_TVORBC__rend_allFlds(pTag, pTBasNew, pTagBO);
	if (res != ST_ERR_SUCC) {
		if (res != ST_ERR_ABRT)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"rendering fields failed");
		return res;
	}

	/* update header */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering header...");
	res = ST_TVORBC__rend_head(pTagI, numFlds, pTBasNew, ST_B_TRUE,
			&vendLen, pTagBO);

	if (res == ST_ERR_SUCC) {
		st_tagBas_gs_setOffset(pTBasNew, st_tagBas_gs_getOffset(&pTagI->tbas));
		st_tagBas_gs_setHasTag(pTBasNew, ST_B_TRUE);
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Render tag header or footer
 */
static Tst_err
ST_TVORBC__rend_head(Tst_vorbc_tag_intn *pTagI,
		const Tst_uint32 numFlds, Tst_tagBasics *pTBasNew,
		const Tst_bool isFinal, Tst_uint32 *pVendLen,
		Tst_binobj *pTagBO)
{
	Tst_err    res  = ST_ERR_SUCC;
	Tst_buf    buf[ST_VORBC_TAG_PHEAD_SLEN + 4 + 4 + /*vlen +*/ 4],
	           *pBB = buf;
	Tst_uint32 ui32,
	           hdSize;
	Tst_mtes_string tmpVend,
	                *pVend = &pTagI->vendor;

	if (! isFinal)
		*pVendLen = 0;
	st_mtes_stc_initStr(&tmpVend);

	/* get VENDOR string and VENDOR LENGTH */
	if (! isFinal) {
		if (st_mtes_getEncodingCur(pVend) != ST_MTES_TE_UTF8) {
			res = st_mtes_convStrTo_utf8_c(pVend, &tmpVend);
			if (res != ST_ERR_SUCC) {
				st_mtes_stc_freeStr(&tmpVend);
				return res;
			}
			pVend = &tmpVend;
		}
		*pVendLen = st_mtes_strlenBy(pVend);
	}

	/* header size: ["VORBCOMM" + TOTAL SZ] + VEND LEN + VEND + ELEM CNT */
	hdSize = ST_VORBC_TAG_PHEAD_SLEN + 4 + 4 + *pVendLen + 4;

	/* pseudo header: "VORBCOMM" + TOTAL SIZE */
	memcpy(pBB, ST_VORBC_TAG_PHEAD, ST_VORBC_TAG_PHEAD_SLEN);
	pBB += ST_VORBC_TAG_PHEAD_SLEN;
	if (isFinal) {
		ui32  = st_tagBas_gs_getSize(pTBasNew);
		ui32 -= (ui32 >= ST_VORBC_TAG_PHEAD_SLEN + 4 ?
				ST_VORBC_TAG_PHEAD_SLEN + 4 : ui32);
#		if (WORDS_BIGENDIAN == 1)
		ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#		endif
	} else
		ui32 = 0;
	memcpy(pBB, &ui32, 4);
	pBB += 4;

	/* VENDOR LENGTH */
	ui32 = *pVendLen;
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(pBB, &ui32, 4);
	pBB += 4;

	/* VENDOR - we'll do this later */

	/* ELEMENT COUNT */
	ui32 = numFlds;
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(pBB, &ui32, 4);

	/* write header to pTagBO */
	if (isFinal) {
		/* replace: [TOTAL SZ] */
		ui32 = ST_VORBC_TAG_PHEAD_SLEN;
		pBB  = &buf[ui32];
		res  = st_binobj_replaceData(pTagBO, ui32, pBB, ui32);
		if (res == ST_ERR_SUCC) {
			/* replace: ELEM CNT */
			ui32 = ST_VORBC_TAG_PHEAD_SLEN + 4 + 4;
			pBB  = &buf[ui32];
			res  = st_binobj_replaceData(pTagBO, ui32 + *pVendLen, pBB, 4);
		}
	} else {
		/* write: ["VORBCOMM" + TOTAL SZ] + VEND LEN */
		ui32 = ST_VORBC_TAG_PHEAD_SLEN + 4 + 4;
		res  = st_binobj_appendData(pTagBO, buf, ui32);
		if (res == ST_ERR_SUCC && *pVendLen > 0) {
			/* write: VENDOR string */
			res = st_binobj_appendData(pTagBO,
					(const Tst_buf*)st_mtes_getDataPt(pVend), *pVendLen);
		}
		if (res == ST_ERR_SUCC) {
			/* write: ELEM CNT */
			pBB = &buf[ui32];
			res = st_binobj_appendData(pTagBO, pBB, 4);
		}

		/* update tag size */
		if (res == ST_ERR_SUCC)
			st_tagBas_gs_setSize(pTBasNew, hdSize);
	}

	st_mtes_stc_freeStr(&tmpVend);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Render all tag fields
 */
static Tst_err
ST_TVORBC__rend_allFlds(Tst_vorbc_tag *pTag, Tst_tagBasics *pTBasNew,
		Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 ix  = 0,
	           zeta;
	Tst_vorbc_fldData      *pItFld;
	Tst_vorbc_fldData_intn *pItFldI;
	Tst_vorbc_tag_intn     *pTagI;

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

	/* first we write the main information (e.g. Title,Album,...)
	 *   then the remaining text fields and then the binary fields  */
	for (zeta = 0; zeta < 3; zeta++) {
		pItFld = NULL;
		while ((pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
			pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
			ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

			if (zeta < 2) {
				if (pItFldI->fldPrI.typ == ST_VORBC_FTP_PICT)
					continue;
				switch (pItFldI->fldPrI.id) {
				case ST_VORBC_FID_TITL:  /* Title */
				case ST_VORBC_FID_ALBU:  /* Album */
				case ST_VORBC_FID_TRKN:  /* Track */
				case ST_VORBC_FID_TRKT:  /* Tracks total (unofficial) */
				case ST_VORBC_FID_DSCN:  /* Disc (unofficial) */
				case ST_VORBC_FID_DSCT:  /* Discs total (unofficial) */
				case ST_VORBC_FID_ARTI:  /* Artist (unofficial) */
				case ST_VORBC_FID_PERF:  /* Performer */
				case ST_VORBC_FID_DESC:  /* Description */
				case ST_VORBC_FID_GENR:  /* Genre */
				case ST_VORBC_FID_RECD:  /* Recording Date */
				case ST_VORBC_FID_RECL:  /* Recording Location */
					if (zeta == 1)
						continue;
					break;
				default:
					if (zeta == 0)
						continue;
				}
			} else if (pItFldI->fldPrI.typ != ST_VORBC_FTP_PICT)
				continue;
			/* */
			if (! st_vorbc_fnc_isFldPropInArr_byEnum(ST_VORBC_FLDPROPS,
						pItFldI->fldPrI.id, &ix)) {
				st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
						"frame ID %d invalid", pItFldI->fldPrI.id);
				continue;
			}

			/* render it */
			/** */
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
					pItFldI->fldPrI.id != ST_VORBC_FID_TAGG)
				st_vorbc_d_fdeb(&pTagI->opts, 0, cFNCN, pItFldI,
						"rendering frame");
			/** */
			res = ST_TVORBC__rend_fld(pTagI, pTBasNew, pItFldI, pTagBO);
			if (res != ST_ERR_SUCC)
				break;
			/** */
			pItFldI->wasRendered = ST_B_TRUE;
		}
		if (res != ST_ERR_SUCC)
			break;
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Render one field
 */
static Tst_err
ST_TVORBC__rend_fld(Tst_vorbc_tag_intn *pTagI, Tst_tagBasics *pTBasNew,
		Tst_vorbc_fldData_intn *pFldI, Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 frBeg,
	           frSz,
	           frHdSz = 0;

	frBeg = st_binobj_getDataSize(pTagBO);

	/* render dummy frame header */
	res = ST_TVORBC__rend_fldHead(pFldI, ST_B_FALSE,
			0, 0, &frHdSz, pTagBO);
	if (res != ST_ERR_SUCC) {
		st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"rendering field header failed");
		return res;
	}

	/* render frame data */
	res = ST_TVORBC__rend_fldData(&pTagI->opts, pFldI, pTagBO);
	if (res != ST_ERR_SUCC) {
		st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"rendering field data failed");
		return res;
	}

	/* get frame size */
	ST_ASSERTN_NUM(ST_ERR_FAIL, frBeg + frHdSz > st_binobj_getDataSize(pTagBO))
	frSz = st_binobj_getDataSize(pTagBO) - frBeg - frHdSz;

	/* update frame header */
	res = ST_TVORBC__rend_fldHead(pFldI, ST_B_TRUE,
			frBeg, frHdSz + frSz, NULL, pTagBO);

	/* update tag size */
	if (res == ST_ERR_SUCC)
		st_tagBas_gs_setSize(pTBasNew,
				st_tagBas_gs_getSize(pTBasNew) + frHdSz + frSz);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Render field header
 */
static Tst_err
ST_TVORBC__rend_fldHead(Tst_vorbc_fldData_intn *pFldI, const Tst_bool isFinal,
		const Tst_uint32 frBeg, const Tst_uint32 frSz,
		Tst_uint32 *pFrHdSz, Tst_binobj *pTagBO)
{
	Tst_err    res;
	Tst_uint32 ui32;
	char       equSign = '=';

	if (pFrHdSz != NULL)
		*pFrHdSz = 0;

	/* SIZE (4 bytes) */
	ui32 = (frSz >= 4 ? frSz - 4 : 0);
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif

	if (isFinal)  /* update only frame size */
		return st_binobj_replaceData(pTagBO, frBeg,
				(const Tst_buf*)&ui32, 4);

	res = st_binobj_appendData(pTagBO, (const Tst_buf*)&ui32, 4);
	if (pFrHdSz != NULL)
		*pFrHdSz = 4;

	/* ITEM KEY - '='-terminated (x bytes) */
	if (res == ST_ERR_SUCC) {
		ui32 = st_sysStrlen2(pFldI->fldPrI.pVIDstr);
		if (ui32 == 0)
			res = ST_ERR_FAIL;
		else
			res = st_binobj_appendData(pTagBO,
					(const Tst_buf*)pFldI->fldPrI.pVIDstr, ui32);
		if (res == ST_ERR_SUCC)
			res = st_binobj_appendData(pTagBO,
					(const Tst_buf*)&equSign, 1);
		if (pFrHdSz != NULL)
			*pFrHdSz += ui32 + 1;
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Render field data
 */
static Tst_err
ST_TVORBC__rend_encStr(Tst_binobj *pTagBO,
		const Tst_mtes_string *pStr, const Tst_bool withTerm)
{
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 slen,
	           termSz  = (withTerm ? 1 : 0);
	Tst_buf    term[1] = {0};

	ST_ASSERTN_NUM(ST_ERR_FAIL,
			st_mtes_getEncodingCur(pStr) != ST_MTES_TE_UTF8)

	slen = st_mtes_strlenBy(pStr);

	/* the string itself */
	if (slen > 0)
		res = st_binobj_appendData(pTagBO,
				(const Tst_buf*)st_mtes_getDataPt(pStr), slen);
	/* NUL-Terminator */
	if (res == ST_ERR_SUCC && termSz > 0)
		res = st_binobj_appendData(pTagBO, term, termSz);

	return res;
}

static Tst_err
ST_TVORBC__rend_multStrU8(Tst_vorbc_opts *pOpts, Tst_vorbc_fldData_intn *pFldI,
		Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 x,
	           saCnt;
	Tst_mtes_string       tmpS,
	                      concS,
	                      divS;
	Tst_mtes_string const *pOutS = NULL;

	st_mtes_stc_initStr(&tmpS);
	st_mtes_stc_initStr(&concS);
	st_mtes_stc_initStr(&divS);
	res = st_mtes_copyFromCharp_utf8((const Tst_str*)", ", &divS);

	saCnt = st_mtes_strArrGetElemCount(&pFldI->dataSArr);
	if (res == ST_ERR_SUCC && saCnt > 1) {
		for (x = 1; x <= saCnt; x++) {
			/* */
			pOutS = st_mtes_strArrGetElem(&pFldI->dataSArr, x);
			if (st_mtes_getEncodingCur(pOutS) != ST_MTES_TE_UTF8) {
				res = st_mtes_convStrTo_utf8_c(pOutS, &tmpS);
				if (res != ST_ERR_SUCC)
					break;
				pOutS = &tmpS;
			}
			/* */
			if (x > 1)
				res = st_mtes_strConcat(&divS, &concS);
			if (res == ST_ERR_SUCC)
				res = st_mtes_strConcat(pOutS, &concS);
			if (res != ST_ERR_SUCC)
				break;
		}
		pOutS = &concS;
	} else if (res == ST_ERR_SUCC && saCnt == 1) {
		pOutS = st_mtes_strArrGetElem(&pFldI->dataSArr, 1);
	}

	if (res == ST_ERR_SUCC && pOutS != NULL &&
			st_mtes_getEncodingCur(pOutS) != ST_MTES_TE_UTF8) {
		res   = st_mtes_convStrTo_utf8_c(pOutS, &tmpS);
		pOutS = &tmpS;
	}

	if (res == ST_ERR_SUCC && pOutS != NULL) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts) &&
				pFldI->fldPrI.id != ST_VORBC_FID_TAGG)
			st_vorbc_d_fdebStr(pOpts, 2, cFNCN, pFldI,
					"txt", pOutS, "");
		res = ST_TVORBC__rend_encStr(pTagBO, pOutS, ST_B_FALSE);
	}

	st_mtes_stc_freeStr(&tmpS);
	st_mtes_stc_freeStr(&concS);
	st_mtes_stc_freeStr(&divS);
	return res;
}

static Tst_err
ST_TVORBC__rend_pict(Tst_vorbc_opts *pOpts, Tst_vorbc_fldData_intn *pFldI,
		Tst_binobj *pTagBO)
{
#	if (WORDS_BIGENDIAN != 1)
#		define LOC_APPUI32_CVEND_(mac_ui32)  \
					mac_ui32 = st_sysReverseByteOrder_UI32(mac_ui32);  /* ENDIAN: LE-->BE */
#	else
#		define LOC_APPUI32_CVEND_(mac_ui32)  /* empty */
#	endif
#	define LOC_APPUI32_(mac_ui32Val)  { \
				ui32 = (Tst_uint32)(mac_ui32Val); \
				LOC_APPUI32_CVEND_(ui32) \
				res = st_binobj_appendData(&binDatRaw, \
						(const Tst_buf*)&ui32, 4); \
				}
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 ui32 = 0,
	           mlen = 0,
	           blen = 0,
	           dlen = 0;
	Tst_binobj binDatRaw,
	           binDatE64;
	Tst_mtes_string tmpDesc,
	                *pDesc;

	/* this is actually the FLAC picture-field format
	 *   that is inofficially supported by Vorbis comment */

	st_binobj_stc_initBO(&binDatRaw);

	/* encode raw data */
	/** PICTURE TYPE */
	LOC_APPUI32_(pFldI->attrPic.ptp)
	/** MIME LENGTH */
	if (res == ST_ERR_SUCC) {
		mlen = st_sysStrlen(pFldI->attrPic.pMime);
		LOC_APPUI32_(mlen)
	}
	/** MIME, ASCII-encoded */
	if (res == ST_ERR_SUCC && mlen > 0) {
		res = st_binobj_appendData(&binDatRaw,
				(const Tst_buf*)pFldI->attrPic.pMime, mlen);
	}
	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_vorbc_d_fdeb(pOpts, 2, cFNCN, pFldI,
				"mime='%s', ptp=%u",
				pFldI->attrPic.pMime, pFldI->attrPic.ptp);
	/** DESC LENGTH */
	st_mtes_stc_initStr(&tmpDesc);
	pDesc = &pFldI->attrPic.desc;
	if (res == ST_ERR_SUCC &&
			st_mtes_getEncodingCur(pDesc) != ST_MTES_TE_UTF8) {
		res   = st_mtes_convStrTo_utf8_c(pDesc, &tmpDesc);
		pDesc = &tmpDesc;
	}
	if (res == ST_ERR_SUCC) {
		dlen = st_mtes_strlenBy(pDesc);
		LOC_APPUI32_(dlen)
	}
	/** DESCRIPTION, UTF8-encoded */
	if (res == ST_ERR_SUCC && dlen > 0) {
		res = st_binobj_appendData(&binDatRaw,
				(const Tst_buf*)st_mtes_getDataPt(pDesc), dlen);
	}
	st_mtes_stc_freeStr(&tmpDesc);
	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_vorbc_d_fdebStr(pOpts, 2, cFNCN, pFldI,
				"scd", &pFldI->attrPic.desc, "");
	/** IMAGE WIDTH, HEIGHT, COLOR DEPTH */
	if (res == ST_ERR_SUCC) {
		LOC_APPUI32_(pFldI->attrPic.imgWid)
		LOC_APPUI32_(pFldI->attrPic.imgHei)
		LOC_APPUI32_(pFldI->attrPic.imgDep)
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_vorbc_d_fdeb(pOpts, 2, cFNCN, pFldI,
					"pic dim %ux%u",
					pFldI->attrPic.imgWid, pFldI->attrPic.imgHei);
	}
	/** NUMBER OF COLORS USED */
	if (res == ST_ERR_SUCC) {
		LOC_APPUI32_(pFldI->attrPic.imgCUs)
	}
	/** BIN LENGTH */
	if (res == ST_ERR_SUCC) {
		blen = st_binobj_getDataSize(&pFldI->dataPicBDat);
		LOC_APPUI32_(blen)
	}
	/** BINARY DATA */
	if (res == ST_ERR_SUCC && blen > 0) {
		res = st_binobj_appendData_binobj(&binDatRaw, &pFldI->dataPicBDat);
	}
	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_vorbc_d_fdeb(pOpts, 2, cFNCN, pFldI,
				"bin %u bytes", blen);
	/** */
	if (res != ST_ERR_SUCC) {
		st_binobj_stc_freeBO(&binDatRaw);
		return res;
	}

	/* encode raw data as base64 */
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_vorbc_d_fdeb(pOpts, 0, cFNCN, pFldI,
				"encoding to base64");
	st_binobj_stc_initBO(&binDatE64);
	res = st_base64_encode_binobj(&binDatRaw,
			/*writeNULterm:*/ST_B_FALSE, &binDatE64);

	/* write base64-encoded NUL-term. string to pTagBO */
	if (res == ST_ERR_SUCC)
		res = st_binobj_appendData_binobj(pTagBO, &binDatE64);

	st_binobj_stc_freeBO(&binDatE64);
	st_binobj_stc_freeBO(&binDatRaw);
	return res;
#	undef LOC_APPUI32_
#	undef LOC_APPUI32_CVEND_
}

static Tst_err
ST_TVORBC__rend_fldData(Tst_vorbc_opts *pOpts, Tst_vorbc_fldData_intn *pFldI,
		Tst_binobj *pTagBO)
{
	if (pFldI->fldPrI.typ != ST_VORBC_FTP_PICT)
		return ST_TVORBC__rend_multStrU8(pOpts, pFldI, pTagBO);

	return ST_TVORBC__rend_pict(pOpts, pFldI, pTagBO);
}

/******************************************************************************/
