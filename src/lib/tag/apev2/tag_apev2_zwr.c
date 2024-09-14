/******************************************************************************/
/* tag_apev2_zwr.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for writing APEv2 Tags                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.12.2010 (dd.mm.yyyy)                                     */
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
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_ZWR_C
#include "tag_apev2_zwr-pp.h"
/*** */
#include "tag_apev2_fnc-prot.h"
#include "tag_apev2_zfdeb-prot.h"
#include "tag_apev2_stcs-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_APEV2_ZWR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Renders a APEv2 Tag
 *
 * returns: ERRCODE
 *          pTBasNew and pTagBO  will be set
 */
Tst_err
st_apev2_wr_rendTagToBO(Tst_apev2_tag *pTag,
		Tst_tagBasics *pTBasNew, Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_buf    dummyHd[ST_APEV2_TAG_HEADFOOT_SZ];
	Tst_uint32 numFlds;
	Tst_mtes_string    taggStr;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTBasNew == NULL || pTagBO == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	st_tagBas_gs_setSize(pTBasNew, 0);

	/* add Tagger field */
	st_mtes_stc_initStr(&taggStr);
	st_tagCFnc_getTaggerStr(&taggStr, ST_B_FALSE);
	res = st_apev2_fldAdd_text(pTag, ST_APEV2_FID_TAGG, NULL, &taggStr);
	st_mtes_stc_freeStr(&taggStr);

	/* count valid fields */
	numFlds = st_apev2_gs_getTag_amountValidFlds(pTag);
	if (numFlds == 0) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"won't render empty tag");
		return ST_ERR_ABRT;
	}

	/* write dummy header */
	memset(dummyHd, 0, sizeof(dummyHd));
	res = st_binobj_appendData(pTagBO, dummyHd, sizeof(dummyHd));
	if (res != ST_ERR_SUCC)
		return res;

	/* update tag size */
	st_tagBas_gs_setSize(pTBasNew, ST_APEV2_TAG_HEADFOOT_SZ);

	/* render all fields */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering fields for APEv2 tag...");
	res = ST_TAV2__rend_allFlds(pTag, pTBasNew, pTagBO);
	if (res != ST_ERR_SUCC) {
		if (res != ST_ERR_ABRT)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"rendering fields failed");
		return res;
	}

	/* render footer */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering footer...");
	res = ST_TAV2__rend_headOrFoot(ST_B_FALSE, pTagI, numFlds,
			pTBasNew, pTagBO);
	if (res != ST_ERR_SUCC)
		return res;

	/* update tag size */
	st_tagBas_gs_setSize(pTBasNew,
			st_tagBas_gs_getSize(pTBasNew) + ST_APEV2_TAG_HEADFOOT_SZ);

	/* update header */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering header...");
	res = ST_TAV2__rend_headOrFoot(ST_B_TRUE, pTagI, numFlds,
			pTBasNew, pTagBO);

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
ST_TAV2__rend_headOrFoot(const Tst_bool hdOrFoot, Tst_apev2_tag_intn *pTagI,
		const Tst_uint32 numFlds, Tst_tagBasics *pTBasNew,
		Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_buf    buf[ST_APEV2_TAG_HEADFOOT_SZ],
	           *pBB = buf;
	Tst_uint32 ui32;

	/* ID (8 bytes) */
	memcpy(pBB, ST_APEV2_TAG_HEADFOOT, ST_APEV2_TAG_HEADFOOT_SLEN);
	pBB += ST_APEV2_TAG_HEADFOOT_SLEN;

	/* VERSION (4 bytes), currently 2.0 (2000) is the max allowed */
	ui32 = 2000;
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(pBB, &ui32, 4);
	pBB += 4;

	/* SIZE excluding header, including fields and footer (4 bytes) */
	ui32 = st_tagBas_gs_getSize(pTBasNew);  /* Header + Fields + Footer */
	if (! hdOrFoot)
		ui32 += ST_APEV2_TAG_HEADFOOT_SZ;
	ui32 -= (ui32 >= ST_APEV2_TAG_HEADFOOT_SZ ?
				ST_APEV2_TAG_HEADFOOT_SZ : ui32);
	if (ui32 > ST_APEV2_MAX_TAGSZ) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"tag size too big: %u > %u bytes",
				ui32, ST_APEV2_MAX_TAGSZ);
		return ST_ERR_ABRT;
	}
	/**st_apev2_prf("%s(): size=%u in %s\n", cFNCN, ui32,
				(! hdOrFoot ? "Footer" : "Header"));**/
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(pBB, &ui32, 4);
	pBB += 4;

	/* FIELDS (4 bytes) */
	if (numFlds > 65536) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"invalid amount of fields: %u > %u",
				numFlds, 65536);
		return ST_ERR_ABRT;
	}
	ui32 = numFlds;
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(pBB, &ui32, 4);
	pBB += 4;

	/* FLAGS (4 bytes) */
	ui32  = ST_APEV2_TAG_FLAG_HASHEADER;
	ui32 |= (hdOrFoot ? ST_APEV2_TAG_FLAG_ISHEADER : 0);
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(pBB, &ui32, 4);
	pBB += 4;

	/* RESERVED (8 bytes) */
	memset(pBB, 0, 8);

	/* write header/footer to pTagBO */
	if (hdOrFoot)
		return st_binobj_replaceData(pTagBO, 0, buf, sizeof(buf));
	return st_binobj_appendData(pTagBO, buf, sizeof(buf));
}

/*----------------------------------------------------------------------------*/

/*
 * Render all tag fields
 */
static Tst_err
ST_TAV2__rend_allFlds(Tst_apev2_tag *pTag, Tst_tagBasics *pTBasNew,
		Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 zeta;
	Tst_apev2_fldData      *pItFld;
	Tst_apev2_fldData_intn *pItFldI;
	Tst_apev2_tag_intn     *pTagI;

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	/* first we write the main information (e.g. Title,Album,...)
	 *   then the remaining text fields and then the binary fields  */
	for (zeta = 0; zeta < 3; zeta++) {
		pItFld = NULL;
		while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
			pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
			ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

			if (zeta < 2) {
				if (pItFldI->fldPrI.typ == ST_APEV2_FTP_BDAT)
					continue;
				switch (pItFldI->fldPrI.id) {
				case ST_APEV2_FID_TITL:  /* Title */
				case ST_APEV2_FID_SUBT:  /* Subtitle */
				case ST_APEV2_FID_ARTI:  /* Artist */
				case ST_APEV2_FID_ALBU:  /* Album */
				case ST_APEV2_FID_TRKN:  /* Track number */
				case ST_APEV2_FID_TRKT:  /* Tracks total (unofficial) */
				case ST_APEV2_FID_DSCN:  /* Disc number (unofficial) */
				case ST_APEV2_FID_DSCT:  /* Discs total (unofficial) */
				case ST_APEV2_FID_COMM:  /* Comment */
				case ST_APEV2_FID_YEAR:  /* Year */
				case ST_APEV2_FID_RECD:  /* Recording Date */
				case ST_APEV2_FID_RECL:  /* Recording Location */
				case ST_APEV2_FID_GENR:  /* Genre */
					if (zeta == 1)
						continue;
					break;
				default:
					if (zeta == 0)
						continue;
				}
			} else if (pItFldI->fldPrI.typ != ST_APEV2_FTP_BDAT)
				continue;
			/* */
			if (! st_apev2_fnc_isFldPropInArr_byEnum(ST_APEV2_FLDPROPS,
						pItFldI->fldPrI.id, NULL)) {
				st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
						"frame ID %d invalid", pItFldI->fldPrI.id);
				continue;
			}

			/* render it */
			/** */
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
					pItFldI->fldPrI.id != ST_APEV2_FID_TAGG)
				st_apev2_d_fdeb(&pTagI->opts, 0, cFNCN, pItFldI,
						"rendering frame");
			/** */
			res = ST_TAV2__rend_fld(pTagI, pTBasNew, pItFldI, pTagBO);
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
ST_TAV2__rend_fld(Tst_apev2_tag_intn *pTagI, Tst_tagBasics *pTBasNew,
		Tst_apev2_fldData_intn *pFldI, Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 frBeg,
	           frSz,
	           frHdSz = 0;

	frBeg = st_binobj_getDataSize(pTagBO);

	/* render dummy frame header */
	res = ST_TAV2__rend_fldHead(pFldI, ST_B_FALSE,
			0, 0, &frHdSz, pTagBO);
	if (res != ST_ERR_SUCC) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"rendering field header failed");
		return res;
	}

	/* render frame data */
	res = ST_TAV2__rend_fldData(&pTagI->opts, pFldI, pTagBO);
	if (res != ST_ERR_SUCC) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"rendering field data failed");
		return res;
	}

	/* get frame size */
	ST_ASSERTN_NUM(ST_ERR_FAIL, frBeg + frHdSz > st_binobj_getDataSize(pTagBO))
	frSz = st_binobj_getDataSize(pTagBO) - frBeg - frHdSz;

	/* update frame header */
	res = ST_TAV2__rend_fldHead(pFldI, ST_B_TRUE,
			frBeg, frSz, NULL, pTagBO);

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
ST_TAV2__rend_fldHead(Tst_apev2_fldData_intn *pFldI, const Tst_bool isFinal,
		const Tst_uint32 frBeg, const Tst_uint32 frSz,
		Tst_uint32 *pFrHdSz, Tst_binobj *pTagBO)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 ui32;
	Tst_buf    buf[8];

	if (pFrHdSz != NULL)
		*pFrHdSz = 0;

	/* SIZE (4 bytes) */
	ui32 = frSz;
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(buf, &ui32, 4);

	if (isFinal)  /* update only frame size */
		return st_binobj_replaceData(pTagBO, frBeg, buf, 4);

	/* FLAGS (4 bytes) */
	ui32 = (pFldI->fldPrI.typ == ST_APEV2_FTP_BDAT ? 0x02 : 0);
#	if (WORDS_BIGENDIAN == 1)
	ui32 = st_sysReverseByteOrder_UI32(ui32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(&buf[4], &ui32, 4);

	res = st_binobj_appendData(pTagBO, buf, sizeof(buf));
	if (pFrHdSz != NULL)
		*pFrHdSz = sizeof(buf);

	/* ITEM KEY - NUL-terminated (x bytes) */
	if (res == ST_ERR_SUCC) {
		ui32 = st_sysStrlen2(pFldI->fldPrI.pVIDstr);
		if (ui32 == 0)
			res = ST_ERR_FAIL;
		else
			res = st_binobj_appendData(pTagBO,
					(const Tst_buf*)pFldI->fldPrI.pVIDstr, ui32 + 1);
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
ST_TAV2__rend_encStr(Tst_binobj *pTagBO,
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
ST_TAV2__rend_multStrU8(Tst_apev2_opts *pOpts, Tst_apev2_fldData_intn *pFldI,
		Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 x,
	           saCnt;
	Tst_mtes_string       tmpS;
	Tst_mtes_string const *pOutS;

	st_mtes_stc_initStr(&tmpS);

	saCnt = st_mtes_strArrGetElemCount(&pFldI->dataSArr);
	for (x = 1; x <= saCnt; x++) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts) &&
				pFldI->fldPrI.id != ST_APEV2_FID_TAGG)
			st_apev2_d_fdebStr(pOpts, 2, cFNCN, pFldI,
					(saCnt > 1 ? "mul" : "txt"),
					st_mtes_strArrGetElem(&pFldI->dataSArr, x), "");
		/* */
		pOutS = st_mtes_strArrGetElem(&pFldI->dataSArr, x);
		if (st_mtes_getEncodingCur(pOutS) != ST_MTES_TE_UTF8) {
			res = st_mtes_convStrTo_utf8_c(pOutS, &tmpS);
			if (res != ST_ERR_SUCC)
				break;
			pOutS = &tmpS;
		}
		/* */
		res = ST_TAV2__rend_encStr(pTagBO, pOutS, (x + 1 <= saCnt));
		if (res != ST_ERR_SUCC)
			break;
	}

	st_mtes_stc_freeStr(&tmpS);
	return res;
}

static Tst_err
ST_TAV2__rend_binData(Tst_apev2_opts *pOpts, Tst_apev2_fldData_intn *pFldI,
		Tst_binobj *pTagBO)
{
	const char *cFNCN = __func__;

	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_apev2_d_fdeb(pOpts, 2, cFNCN, pFldI,
				"bin %u bytes",
				st_binobj_getDataSize(&pFldI->dataBinDat));
	return st_binobj_appendData_binobj(pTagBO, &pFldI->dataBinDat);
}

static Tst_err
ST_TAV2__rend_fldData(Tst_apev2_opts *pOpts, Tst_apev2_fldData_intn *pFldI,
		Tst_binobj *pTagBO)
{
	if (pFldI->fldPrI.typ != ST_APEV2_FTP_BDAT)
		return ST_TAV2__rend_multStrU8(pOpts, pFldI, pTagBO);

	return ST_TAV2__rend_binData(pOpts, pFldI, pTagBO);
}

/******************************************************************************/
