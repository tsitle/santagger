/******************************************************************************/
/* tag_id3v2_zwr.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for writing ID3v2 Tags                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 05.03.2010 (dd.mm.yyyy)                                     */
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
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_ZWR_C
#include "tag_id3v2_zwr-pp.h"
/*** */
#include "tag_id3v2-prot.h"
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2_stcs-prot.h"
#include "tag_id3v2_zsync-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_ZWR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Renders a ID3v2 Tag
 *
 * returns: ERRCODE
 *          pTBasNew and *ppTagBO  will be set
 */
Tst_err
st_id3v2_wr_rendTagToBO(Tst_id3v2_tag *pTag,
		Tst_tagBasics *pTBasNew, Tst_binobj **ppTagBO)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 delta   = 0,
	           paddLen = 0;
	Tst_buf    dummyHd[ST_ID3V2_TAG_HEADFOOT_SZ];
	Tst_binobj         *pTagBOunsy = NULL;
	Tst_mtes_string    taggStr;
	Tst_id3v2_thdFlags tHdFlagsNew;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTBasNew == NULL || ppTagBO == NULL || *ppTagBO == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	st_tagBas_gs_setSize(pTBasNew, 0);
	st_id3v2_stc_rsetTHdF(&tHdFlagsNew);

	/* count valid fields */
	if (st_id3v2_gs_getTag_amountValidFlds_all(pTag) == 0) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"won't render empty tag");
		return ST_ERR_ABRT;
	}

	/* add Tagger field */
	st_mtes_stc_initStr(&taggStr);
	st_tagCFnc_getTaggerStr(&taggStr);
	res = st_id3v2_fldAdd_text(pTag, ST_ID3V2_FID_234_CTXT,
			(const Tst_str*)(pTagI->verMaj == 2 ? "TGA" : "TGGA"),
			NULL, &taggStr);
	st_mtes_stc_freeStr(&taggStr);

	/* write dummy tag header */
	memset(dummyHd, 0, sizeof(dummyHd));
	res = st_binobj_appendData(*ppTagBO, dummyHd, sizeof(dummyHd));
	if (res != ST_ERR_SUCC)
		return res;
	st_tagBas_gs_setSize(pTBasNew, sizeof(dummyHd));

	/* write dummy extended tag header */
#	if (ST_ID3V2_RENDER_EXTHEAD == 1)
	if (pTagI->verMaj == 4) {
		/* we're not writing this header for v2.3 because it
		 *   would complicate unsync'ing the entire tag */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"rendering dummy extended header...");
		res = ST_TIV2__rend_extHead(pTagI, pTBasNew, ST_B_FALSE, 0, *ppTagBO);
		if (res != ST_ERR_SUCC) {
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"rendering dummy extended header failed");
			return res;
		}
		tHdFlagsNew.hasExtHd = ST_B_TRUE;
	}
#	endif

	/* render all fields */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering fields for ID3v2.%u.%u tag...",
				pTagI->verMaj, pTagI->verMin);
	res = ST_TIV2__rend_allFlds(pTag, pTBasNew, &tHdFlagsNew, *ppTagBO);
	if (res != ST_ERR_SUCC) {
		if (res != ST_ERR_ABRT)
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"rendering fields failed");
		return res;
	}

	/* unsynchronisation */
	if (pTagI->verMaj < 4 && pTagI->opts.unsync) {
		/* unsync'ing currently breaks compatibility with iTunes 10.1 */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"unsyncing entire tag...");
		/* */
		ST_CALLOC(pTagBOunsy, Tst_binobj*, 1, sizeof(Tst_binobj))
		if (pTagBOunsy == NULL)
			return ST_ERR_OMEM;
		st_binobj_stc_initBO(pTagBOunsy);
		/* */
		res = st_id3v2_sync_doUnsync_bo2bo(*ppTagBO, pTagBOunsy, &delta);
		if (res != ST_ERR_SUCC) {
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"unsynchronizing tag failed");
			st_binobj_stc_freeBO(pTagBOunsy);
			ST_DELPOINT(pTagBOunsy)
			return res;
		}
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"unsynced %u bytes", delta);
	}
	tHdFlagsNew.isUnsy = (pTagI->verMaj < 4) && (delta > 0);
	if (tHdFlagsNew.isUnsy) {
		/* replace binary object with unsynced one */
		st_binobj_stc_freeBO(*ppTagBO);
		ST_DELPOINT(*ppTagBO)
		*ppTagBO = pTagBOunsy;
		st_tagBas_gs_setSize(pTBasNew, st_binobj_getDataSize(pTagBOunsy));
	} else if (pTagBOunsy != NULL) {
		st_binobj_stc_freeBO(pTagBOunsy);
		ST_DELPOINT(pTagBOunsy)
	}

	/* add some padding to buffer */
	res = ST_TIV2__rend_addPadd(&pTagI->opts, pTBasNew,
			*ppTagBO, (pTagI->verMaj == 4), &paddLen);
	if (res != ST_ERR_SUCC) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"adding padding failed");
		return res;
	}

	/* render tag footer */
	if (pTagI->verMaj == 4) {
		tHdFlagsNew.hasFoot = ST_B_TRUE;
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"rendering footer...");
		res = ST_TIV2__rend_headOrFoot(ST_B_FALSE, pTagI, pTBasNew,
				&tHdFlagsNew, *ppTagBO);
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* update tag header */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"rendering header...");
	res = ST_TIV2__rend_headOrFoot(ST_B_TRUE, pTagI, pTBasNew,
			&tHdFlagsNew, *ppTagBO);

	/* update extended tag header */
#	if (ST_ID3V2_RENDER_EXTHEAD == 1)
	if (res == ST_ERR_SUCC && pTagI->verMaj == 4) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"rendering extended header...");
		res = ST_TIV2__rend_extHead(pTagI, pTBasNew, ST_B_TRUE, paddLen, *ppTagBO);
	}
#	endif

	if (res == ST_ERR_SUCC) {
		st_tagBas_gs_setOffset(pTBasNew, st_tagBas_gs_getOffset(&pTagI->tbas));
		st_tagBas_gs_setHasTag(pTBasNew, ST_B_TRUE);
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Render header/footer
 */
static Tst_err
ST_TIV2__rend_headOrFoot(const Tst_bool hdOrFoot,
		const Tst_id3v2_tag_intn *pTagI, Tst_tagBasics *pTBasNew,
		const Tst_id3v2_thdFlags *pTHdFlagsNew, Tst_binobj *pOutp)
{
	const char *cFNCN = __func__;
	Tst_byte   flagByte = 0;
	Tst_uint32 totTagSz;
	Tst_buf    buf[ST_ID3V2_TAG_HEADFOOT_SZ],
	           *pBB = buf;

	if (! hdOrFoot) {
		st_tagBas_gs_setSize(pTBasNew,
				st_tagBas_gs_getSize(pTBasNew) + ST_ID3V2_TAG_HEADFOOT_SZ);
	}

	/* "ID3" or "3DI" */
	if (hdOrFoot)
		memcpy(pBB, ST_ID3V2_TAG_HEAD, ST_ID3V2_TAG_HEADFOOT_SLEN);
	else
		memcpy(pBB, ST_ID3V2_TAG_FOOT, ST_ID3V2_TAG_HEADFOOT_SLEN);
	pBB += ST_ID3V2_TAG_HEADFOOT_SLEN;

	/* VERSION */
	*pBB++ = pTagI->verMaj;
	*pBB++ = pTagI->verMin;

	/* FLAGS */
	switch (pTagI->verMaj) {
	case 2:
		/*** Unsynchronization */
		if (pTHdFlagsNew->isUnsy)
			flagByte = (flagByte | 0x80);
		/*** Compression */
		if (pTHdFlagsNew->isCompr)
			flagByte = (flagByte | 0x40);
		/*** Has Compressed Metaframe (?) */
		if (pTagI->verMin > 0 && pTHdFlagsNew->hasCDM)
			flagByte = (flagByte | 0x20);
		break;
	case 3:
	case 4:
		/*** Unsynchronization */
		if (pTHdFlagsNew->isUnsy)
			flagByte = (flagByte | 0x80);
		/*** Extended header */
		if (pTHdFlagsNew->hasExtHd)
			flagByte = (flagByte | 0x40);
		/*** Experimental indicator */
		if (pTHdFlagsNew->isExp)
			flagByte = (flagByte | 0x20);
		/*** Has Footer */
		if (pTagI->verMaj == 4 && pTHdFlagsNew->hasFoot)
			flagByte = (flagByte | 0x10);
		break;
	default:
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"unknown ID3v2 Major Tag-Version %u", pTagI->verMaj);
		return ST_ERR_IARG;
	}
	*pBB++ = flagByte;

	/* TAG SIZE (without ID and header) */
	/** */
	ST_ASSERTN_NUM(ST_ERR_FAIL,
			st_tagBas_gs_getSize(pTBasNew) < ST_ID3V2_TAG_HEADFOOT_SZ)
	totTagSz = st_tagBas_gs_getSize(pTBasNew) - ST_ID3V2_TAG_HEADFOOT_SZ;
	if (pTHdFlagsNew->hasFoot) {
		ST_ASSERTN_NUM(ST_ERR_FAIL, totTagSz < ST_ID3V2_TAG_HEADFOOT_SZ)
		totTagSz -= ST_ID3V2_TAG_HEADFOOT_SZ;
	}
	/** convert SyncSafe Integer */
	if (totTagSz > ST_ID3V2_MAX_SAFEINT) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"tag too big: "
				"%u bytes (%.2f MB), max=%u bytes (%u MB), aborting",
				totTagSz, (float)totTagSz / 1024.0 / 1024.0,
				ST_ID3V2_MAX_SAFEINT,
				(Tst_uint32)((float)ST_ID3V2_MAX_SAFEINT / 1024.0 / 1024.0));
		return ST_ERR_ABRT;
	}
	totTagSz = st_id3v2_sync_uint2syncsafe(totTagSz);  /* ENDIAN: HOST-->BE */
	memcpy(pBB, &totTagSz, 4);

	if (hdOrFoot)
		return st_binobj_replaceData(pOutp, 0, buf, sizeof(buf));
	return st_binobj_appendData(pOutp, buf, sizeof(buf));
}

/*
 * Render ID3v2.(3,4) extended header
 *   (actually we only render this for v2.4 tags)
 */
#if (ST_ID3V2_RENDER_EXTHEAD == 1)
static Tst_err
ST_TIV2__rend_extHead(const Tst_id3v2_tag_intn *pTagI,
		Tst_tagBasics *pTBasNew, const Tst_bool isFinal,
		const Tst_uint32 paddLen, Tst_binobj *pOutp)
{
#	define LOC_EXTHDSZ_  1
	Tst_uint32 ui32;
	Tst_buf    buf[4 + LOC_EXTHDSZ_];

	ST_ASSERTN_IARG(pTagI->verMaj != 4)

	/**st_id3v2_prf(" extHead: paddLen %u, ehdSz (4+)%u\n",
			paddLen, LOC_EXTHDSZ_);**/

	if (! isFinal) {
		st_tagBas_gs_setSize(pTBasNew,
				st_tagBas_gs_getSize(pTBasNew) + sizeof(buf));
	}

	/* SIZE (w/o SIZE itself), 4 bytes */
	/** convert SyncSafe Integer */
	ui32 = st_id3v2_sync_uint2syncsafe(LOC_EXTHDSZ_);  /* ENDIAN: HOST-->BE */
	memcpy(buf, &ui32, 4);

	/* NUMBER OF FLAG BYTES, 1 byte */
	buf[4] = 0;

	if (isFinal)
		return st_binobj_replaceData(pOutp, ST_ID3V2_TAG_HEADFOOT_SZ,
				buf, sizeof(buf));
	return st_binobj_appendData(pOutp, buf, sizeof(buf));
#	undef LOC_EXTHDSZ_
}
#endif

/*----------------------------------------------------------------------------*/

/*
 * Adding padding bytes (0x00) to buffer
 *   The resulting buffer size shall be dividable by 512
 */
static Tst_err
ST_TIV2__rend_addPadd(const Tst_id3v2_opts *pOpts,
		Tst_tagBasics *pTBasNew,
		Tst_binobj *pOutp, const Tst_bool willFooterBeAdded,
		Tst_uint32 *pPaddLen)
{
	const Tst_uint32 cDEFADDSZ = 512;
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 inc,
	           foot   = 0,
	           newSz;
	Tst_buf    *pPadd = NULL;

	*pPaddLen = 0;
	newSz     = st_tagBas_gs_getSize(pTBasNew);

	inc = cDEFADDSZ - (newSz % cDEFADDSZ);
	if (willFooterBeAdded) {
		foot = ST_ID3V2_TAG_HEADFOOT_SZ;
		if (inc >= foot)
			inc -= foot;
		else
			inc = cDEFADDSZ - foot;
	}
	if (inc == 0)
		return ST_ERR_SUCC;
	/* */
	ST_CALLOC(pPadd, Tst_buf*, inc, 1)
	if (pPadd == NULL)
		return ST_ERR_OMEM;
	memset(pPadd, 0, inc);
	res = st_binobj_appendData(pOutp, pPadd, inc);
	if (res == ST_ERR_SUCC)
		st_tagBas_gs_setSize(pTBasNew, newSz + inc);
	ST_DELPOINT(pPadd)
	*pPaddLen = inc;

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Render all tag fields
 */
static Tst_err
ST_TIV2__rend_allFlds(Tst_id3v2_tag *pTag,
		Tst_tagBasics *pTBasNew, Tst_id3v2_thdFlags *pTHdFlagsNew,
		Tst_binobj *pOutp)
{
	const char *cFNCN = __func__;
	Tst_err    res  = ST_ERR_SUCC;
	Tst_uint32 ix   = 0,
	           rend = 0;
	Tst_str    tmpIDstr[5];
	Tst_bool   isIDstrOK;
	Tst_binobj             *pBOfld = NULL;
	Tst_id3v2_fldData      *pItFld = NULL;
	Tst_id3v2_fldData_intn *pItFldI;
	Tst_id3v2_tag_intn     *pTagI;

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	ST_CALLOC(pBOfld, Tst_binobj*, 1, sizeof(Tst_binobj))
	if (pBOfld == NULL)
		return ST_ERR_OMEM;
	st_binobj_stc_initBO(pBOfld);

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		/* check ID */
		if (! st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS,
					pItFldI->fldPrI.id, &ix)) {
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"frame ID %d invalid", pItFldI->fldPrI.id);
			continue;
		}
		/* copy ID-String */
		tmpIDstr[0] = tmpIDstr[3] = 0;
		if (pTagI->verMaj == 2) {
			strncpy((char*)tmpIDstr, pItFldI->fldPrI.vid02str, 3);
		} else if (pTagI->verMaj > 2) {
			strncpy((char*)tmpIDstr, pItFldI->fldPrI.vid34str, 4);
		}
		tmpIDstr[4] = 0;

		/* render it */
		/** get fresh field-props to make sure everything is set properly,
		 **   for example the ID string "TSA " would be replaced by "TSOA"  */
		st_id3v2_stc_rsetFPr_i(&pItFldI->fldPrI);
		res = st_id3v2_stc_copyFPr_i(pTagI->verMaj,
				tmpIDstr,
				(Tst_id3v2_fldProp_intn*)&ST_ID3V2_FLDPROPS[ix],
				&pItFldI->fldPrI);
		if (res != ST_ERR_SUCC)
			break;
		/** re-check final ID-String */
		isIDstrOK = ST_B_TRUE;
		if (pItFldI->fldPrI.pIDstr == NULL)
			isIDstrOK = ST_B_FALSE;
		else {
			Tst_uint32 idX,
			           idSlen = st_sysStrlen2(pItFldI->fldPrI.pIDstr);

			if (idSlen != (pTagI->verMaj == 2 ? 3 : 4))
				isIDstrOK = ST_B_FALSE;
			else
				for (idX = 0; idX < idSlen; idX++)
					if (pItFldI->fldPrI.pIDstr[idX] == ' ' ||
							pItFldI->fldPrI.pIDstr[idX] == '-') {
						isIDstrOK = ST_B_FALSE;
						break;
					}
		}
		if (isIDstrOK)
			isIDstrOK = ! st_id3v2_fnc_hasIDstrInvalidChars(
						(Tst_str*)pItFldI->fldPrI.pIDstr,
						/*mod:*/ST_B_FALSE, NULL);
		/** */
		if (isIDstrOK) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
					! st_id3v2_fnc_isTaggInfFr(pItFldI->fldPrI.id,
								pItFldI->fldPrI.pIDstr, pTagI->verMaj))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pItFldI,
						"rendering frame");
			/* */
			res = ST_TIV2__rend_fld(pTagI, pTBasNew, pTHdFlagsNew,
					pItFldI, &pBOfld);
			if (res != ST_ERR_SUCC)
				break;
			/* */
			++rend;
			pItFldI->wasRendered = ST_B_TRUE;

			/* append field to tag */
			res = st_binobj_appendData_binobj(pOutp, pBOfld);
			if (res != ST_ERR_SUCC)
				break;
		} else {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pItFldI,
						"not rendering frame since ID-String '%s' is invalid",
						pItFldI->fldPrI.pIDstr);
		}
	}

	st_binobj_stc_freeBO(pBOfld);
	ST_DELPOINT(pBOfld)

	/* last bastion */
	if (res == ST_ERR_SUCC &&
			rend != st_id3v2_gs_getTag_amountValidFlds_all(pTag)) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"rendered less fields than we should have");
		res = ST_ERR_FAIL;
	}

	return res;
}

/*
 * Render one field
 */
static Tst_err
ST_TIV2__rend_fld(const Tst_id3v2_tag_intn *pTagI,
		Tst_tagBasics *pTBasNew, Tst_id3v2_thdFlags *pTHdFlagsNew,
		Tst_id3v2_fldData_intn *pFldI, Tst_binobj **ppOutp)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 frEndBP,
	           frHdIdSz = (pTagI->verMaj == 2 ? 3 : 4),
	           frHdSz,
	           frSz,
	           delta    = 0;
	Tst_buf    dummyHd[10];
	Tst_bool   isTaggInf;
	Tst_binobj          *pOutpRFD    = *ppOutp,
	                    *pOutpUncmpr = NULL,
	                    *pOutpCmpr   = NULL,
	                    *pOutpUnsy   = NULL;
	Tst_id3v2_frhdFlags frHdFlags;

	st_id3v2_stc_rsetFHdF(&frHdFlags);

	isTaggInf = st_id3v2_fnc_isTaggInfFr(pFldI->fldPrI.id,
				pFldI->fldPrI.pIDstr, pTagI->verMaj);

	/* write dummy frame header */
	frHdSz = 2 * frHdIdSz;
	if (pTagI->verMaj > 2)
		frHdSz += 2;
	memset(dummyHd, 0, sizeof(dummyHd));
	res = st_binobj_setData(*ppOutp, dummyHd, frHdSz);
	if (res != ST_ERR_SUCC)
		return res;

	/* render frame data */
	/** */
	if (pTagI->opts.cbZLibCmpr != NULL && pTagI->opts.compress &&
			pTagI->verMaj > 2) {
		/* compression currently breaks compatibility with iTunes 10.1 */
		Tst_uint32 tmpBDSZ  = st_binobj_getDataSize(&pFldI->dataBinDat),
		           tmpTSLEN = st_mtes_strArrTotalStrlenBy(&pFldI->dataSArr);

		if ((tmpBDSZ >= ST_ID3V2_CMPR_MIN && tmpBDSZ <= ST_ID3V2_CMPR_MAX) ||
				(tmpTSLEN >= ST_ID3V2_CMPR_MIN && tmpTSLEN <= ST_ID3V2_CMPR_MAX)) {
			/* render to pOutpUncmpr */
			ST_CALLOC(pOutpUncmpr, Tst_binobj*, 1, sizeof(Tst_binobj))
			if (pOutpUncmpr == NULL)
				return ST_ERR_OMEM;
			st_binobj_stc_initBO(pOutpUncmpr);
			pOutpRFD = pOutpUncmpr;
		}
	}
	/** */
	res = ST_TIV2__rend_fldData(&pTagI->opts, pTagI->verMaj,
			pFldI, &frHdFlags, pOutpRFD);
	if (res != ST_ERR_SUCC) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"rendering field data failed");
		st_binobj_stc_freeBO(pOutpUncmpr);
		ST_DELPOINT(pOutpUncmpr)
		return res;
	}
	/** */
	if (pOutpUncmpr != NULL) {
		Tst_uint32 tmpUSOF = st_binobj_getDataSize(pOutpUncmpr);

		/* compress pOutpUncmpr --> pOutpCmpr */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) && ! isTaggInf)
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"compressing frame...");
		ST_CALLOC(pOutpCmpr, Tst_binobj*, 1, sizeof(Tst_binobj))
		if (pOutpCmpr == NULL) {
			st_binobj_stc_freeBO(pOutpUncmpr);
			ST_DELPOINT(pOutpUncmpr)
			return ST_ERR_OMEM;
		}
		st_binobj_stc_initBO(pOutpCmpr);
		res = pTagI->opts.cbZLibCmpr(pOutpUncmpr, pOutpCmpr);
		st_binobj_stc_freeBO(pOutpUncmpr);
		ST_DELPOINT(pOutpUncmpr)
		if (res != ST_ERR_SUCC) {
			st_binobj_stc_freeBO(pOutpCmpr);
			ST_DELPOINT(pOutpCmpr)
			return res;
		}
		/* append frame header flag data */
		frHdFlags.isV34compr = ST_B_TRUE;
		if (pTagI->verMaj == 4)
			frHdFlags.hasV4dli = ST_B_TRUE;
		if (pTagI->verMaj == 3) {
			/* V3: append uncomressed size of frame */
#			if (WORDS_BIGENDIAN != 1)
			tmpUSOF = st_sysReverseByteOrder_UI32(tmpUSOF);  /* ENDIAN: LE-->BE */
#			endif
		} else {
			/* V4: append data length indicator */
			/** convert SyncSafe Integer */
			tmpUSOF = st_id3v2_sync_uint2syncsafe(tmpUSOF);  /* ENDIAN: HOST-->BE */
		}
		res = st_binobj_appendData(*ppOutp, (const Tst_buf*)&tmpUSOF, 4);
		/* append compressed data to *ppOutp */
		if (res == ST_ERR_SUCC)
			res = st_binobj_appendData_binobj(*ppOutp, pOutpCmpr);
		st_binobj_stc_freeBO(pOutpCmpr);
		ST_DELPOINT(pOutpCmpr)
		if (res != ST_ERR_SUCC)
			return res;
		/*pOutpRFD = *ppOutp;*/
	}

	/* get frame size */
	frEndBP = st_binobj_getDataSize(*ppOutp);
	ST_ASSERTN_NUM(ST_ERR_FAIL, frEndBP < frHdSz)
	frSz = frEndBP - frHdSz;

	/* unsynchronisation */
	if (pTagI->verMaj == 4 && pTagI->opts.unsync) {
		/* unsync'ing currently breaks compatibility with iTunes 10.1 */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) && ! isTaggInf)
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"unsyncing frame...");
		/* */
		ST_CALLOC(pOutpUnsy, Tst_binobj*, 1, sizeof(Tst_binobj))
		if (pOutpUnsy == NULL)
			return ST_ERR_OMEM;
		st_binobj_stc_initBO(pOutpUnsy);
		/* */
		res = st_id3v2_sync_doUnsync_bo2bo(*ppOutp, pOutpUnsy, &delta);
		if (res != ST_ERR_SUCC) {
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"unsynchronizing field failed");
			st_binobj_stc_freeBO(pOutpUnsy);
			ST_DELPOINT(pOutpUnsy)
			return res;
		}
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) && ! isTaggInf)
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"unsynced %u bytes", delta);
	}
	/**/
	frHdFlags.isV4unsy = (pTagI->verMaj == 4) && (delta > 0);
	if (frHdFlags.isV4unsy) {
		/* replace binary object with unsynced one */
		st_binobj_stc_freeBO(*ppOutp);
		ST_DELPOINT(*ppOutp)
		*ppOutp = pOutpUnsy;
		/* */
		frSz    += delta;
		frEndBP += delta;
		/* to indicate that all frames are unsynced */
		pTHdFlagsNew->isUnsy = ST_B_TRUE;
	} else if (pOutpUnsy != NULL) {
		st_binobj_stc_freeBO(pOutpUnsy);
		ST_DELPOINT(pOutpUnsy)
	}

	/* update frame header */
	/** */
	if (pTagI->verMaj == 2 && frSz > ST_ID3V2_MAX_TFLD_SZ_V2) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"field too big for v2.2 tag: %u bytes (%.2f MB), "
				"max=%u bytes (%u MB), aborting",
				frSz, (double)frSz / 1024.0 / 1024.0,
				ST_ID3V2_MAX_TFLD_SZ_V2,
				(Tst_uint32)((double)ST_ID3V2_MAX_TFLD_SZ_V2 / 1024.0 / 1024.0));
		return ST_ERR_ABRT;
	} else if (frSz > ST_ID3V2_MAX_SAFEINT) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"field too big: %u bytes (%.2f MB), "
				"max=%u bytes (%u MB), aborting",
				frSz, (double)frSz / 1024.0 / 1024.0,
				ST_ID3V2_MAX_SAFEINT,
				(Tst_uint32)((double)ST_ID3V2_MAX_SAFEINT / 1024.0 / 1024.0));
		return ST_ERR_ABRT;
	}
	/** */
	res = ST_TIV2__rend_fldHead(&pTagI->opts, pTagI->verMaj,
			pFldI, &frHdFlags, *ppOutp, frSz);

	/* update tag size */
	if (res == ST_ERR_SUCC) {
		st_tagBas_gs_setSize(pTBasNew,
				st_tagBas_gs_getSize(pTBasNew) + frSz + frHdSz);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Render field header
 */
static Tst_err
ST_TIV2__rend_fldHead(const Tst_id3v2_opts *pOpts, const Tst_byte verMaj,
		Tst_id3v2_fldData_intn *pFldI,
		const Tst_id3v2_frhdFlags *pFrHdFlags,
		Tst_binobj *pOutp, const Tst_uint32 frSize)
{
	Tst_err    res;
	Tst_uint32 flagBytes = 0,
	           frHdIdSz  = (verMaj == 2 ? 3 : 4),
	           tmp;
	Tst_uint16 tmpWrd;

	if (frSize > ST_ID3V2_MAX_SAFEINT ||
			st_sysStrlen2(pFldI->fldPrI.pIDstr) < frHdIdSz)
		return ST_ERR_FAIL;

	res = st_binobj_replaceData(pOutp, 0,
			(Tst_buf*)pFldI->fldPrI.pIDstr, frHdIdSz);
	if (res != ST_ERR_SUCC)
		return res;

	/* FRAME SIZE */
	if (verMaj < 4) {
#		if (WORDS_BIGENDIAN != 1)
		tmp   = st_sysReverseByteOrder_UI32(frSize);  /* ENDIAN: LE-->BE */
		tmp >>= ((4 - frHdIdSz) * 8);
#		else
		tmp   = frSize;
		tmp <<= ((4 - frHdIdSz) * 8);
#		endif
	} else
		tmp = st_id3v2_sync_uint2syncsafe(frSize);  /* ENDIAN: HOST-->BE */
	res = st_binobj_replaceData(pOutp, (Tst_foffs)frHdIdSz,
			(Tst_buf*)&tmp, frHdIdSz);
	if (res != ST_ERR_SUCC)
		return res;

	/* FRAME FLAGS */
	if (verMaj > 2) {
		ST_TIV2__rend_fldFlags(pOpts, verMaj, pFrHdFlags, &flagBytes);
		tmpWrd = flagBytes;
#		if (WORDS_BIGENDIAN != 1)
		tmpWrd = st_sysReverseByteOrder_UI16(tmpWrd);  /* ENDIAN: LE-->BE */
#		endif
		res = st_binobj_replaceData(pOutp, (Tst_foffs)(frHdIdSz * 2),
				(Tst_buf*)&tmpWrd, 2);
		if (res != ST_ERR_SUCC)
			return res;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Render field flags for ID3v2.(3,4)
 */
static void
ST_TIV2__rend_fldFlags(const Tst_id3v2_opts *pOpts, const Tst_byte verMaj,
		const Tst_id3v2_frhdFlags *pFrHdFlags, Tst_uint32 *pFlagBytes)
{
	*pFlagBytes = 0;
	if (verMaj == 3) {
		/* %abc00000 %ijk00000 */
		/** a - Tag alter preservation */
		if (pFrHdFlags->doV34tap) *pFlagBytes = (*pFlagBytes | 0x8000);
		/** b - File alter preservation */
		if (pFrHdFlags->doV34fap) *pFlagBytes = (*pFlagBytes | 0x4000);
		/** c - Read only */
		if (pFrHdFlags->isV34ro) *pFlagBytes = (*pFlagBytes | 0x2000);
		/** i - Compression */
		if (pFrHdFlags->isV34compr) *pFlagBytes = (*pFlagBytes | 0x0080);
		/** j - Encryption */
		if (pFrHdFlags->isV34enc) *pFlagBytes = (*pFlagBytes | 0x0040);
		/** k - Grouping identity */
		if (pFrHdFlags->isV34gi) *pFlagBytes = (*pFlagBytes | 0x0020);
	} else {
		/* %0abc0000 %0h00kmnp */
		/** a - Tag alter preservation */
		if (pFrHdFlags->doV34tap) *pFlagBytes = (*pFlagBytes | 0x4000);
		/** b - File alter preservation */
		if (pFrHdFlags->doV34fap) *pFlagBytes = (*pFlagBytes | 0x2000);
		/** c - Read only */
		if (pFrHdFlags->isV34ro) *pFlagBytes = (*pFlagBytes | 0x1000);
		/** h - Grouping identity */
		if (pFrHdFlags->isV34gi) *pFlagBytes = (*pFlagBytes | 0x0040);
		/** k - Compression */
		if (pFrHdFlags->isV34compr) *pFlagBytes = (*pFlagBytes | 0x0008);
		/** m - Encryption */
		if (pFrHdFlags->isV34enc) *pFlagBytes = (*pFlagBytes | 0x0004);
		/** n - Unsynchronisation */
		if (pFrHdFlags->isV4unsy) *pFlagBytes = (*pFlagBytes | 0x0002);
		/** p - Data length indicator */
		if (pFrHdFlags->hasV4dli) *pFlagBytes = (*pFlagBytes | 0x0001);
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Render field data
 */
static Tst_err
ST_TIV2__rend_fldData(const Tst_id3v2_opts *pOpts, const Tst_byte verMaj,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_id3v2_frhdFlags *pFrHdFlags,
		Tst_binobj *pOutp)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 rwoX,
	           rwoArrSz;   /* size of rwoArr (=6) */
	Tst_bool   isTaggInf;
	Tst_mtes_textEnc         te = ST_MTES_TE_ISO;
	Tst_id3v2_fldProp_intn   *pFldPrI;
	Tst_id3v2_fldd_rword_typ rwoArr[6];  /* order to read/write data in */
	Tst_id3v2_fldd_strtp     strTp;
	Tst_id3v2_fldd_dattp     datTp;

	pFldPrI   = &pFldI->fldPrI;
	isTaggInf = st_id3v2_fnc_isTaggInfFr(pFldI->fldPrI.id,
				pFldI->fldPrI.pIDstr, verMaj);

	/* get order to write data in */
	st_id3v2_stc_rsetStrTp(&strTp);
	st_id3v2_stc_rsetDatTp(&datTp);
	rwoArrSz = sizeof(rwoArr) / sizeof(Tst_id3v2_fldd_rword_typ);
	st_id3v2_fnc_setDataNstrTp(pFldPrI->id, &datTp, &strTp,
			(Tst_id3v2_fldd_rwordPArr)rwoArr, rwoArrSz);
	/* correct text-encoding */
	res = ST_TIV2__rend_fd_corrTEs(verMaj, pFldI, &te);
	if (res != ST_ERR_SUCC)
		return res;

	/* render data in order defined by rwoArr */
	rwoX = 0;
	while (rwoX < rwoArrSz &&
			rwoArr[rwoX] != ST_ID3V2_TFLD_RWO_TP_NONE) {
		/* */
		switch (rwoArr[rwoX]) {
		case ST_ID3V2_TFLD_RWO_TP_TENC:  /* text encoding */
			/* text encoding (1 byte) */
			res = ST_TIV2__rend_fd_encTE(pOpts, pOutp, te);
			break;
		case ST_ID3V2_TFLD_RWO_TP_LANG:  /* language */
			/* language (3 bytes) */
			res = ST_TIV2__rend_fd_encLang(pOpts, cFNCN, pOutp, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_SCD:   /* short content desc, ... */
			/* render 'short content description' / 'owner' / 'email' / ... */
			if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts) &&
					st_mtes_getStringSize(&pFldI->attrDesc) > 0)
				st_id3v2_d_fdeb3Str(pOpts, 2, cFNCN, pFldI,
						ST_B_TRUE, "scd", &pFldI->attrDesc, "");
			res = ST_TIV2__rend_fd_encStr(pOutp, &pFldI->attrDesc, ST_B_TRUE);
			break;
		case ST_ID3V2_TFLD_RWO_TP_TEXT:  /* text (single/multi string) */
			/* render 'text' / 'info' / 'url' / 'People list' */
			res = ST_TIV2__rend_fd_encMultStr(pOpts, cFNCN,
					pOutp, &pFldI->dataSArr, pFldI, isTaggInf);
			break;
		case ST_ID3V2_TFLD_RWO_TP_BDAT:  /* binary data */
			/* render 'picture data' / ... */
			res = ST_TIV2__rend_fd_encBinDat(pOpts, cFNCN, pOutp, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_UVAL:  /* unsigned int value */
			/* 'play counter' */
			res = ST_TIV2__rend_fd_encUVal(pOpts, cFNCN, pOutp, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_BVAL:  /* unsigned byte value */
			/* 'rating' (1 byte) */
			res = ST_TIV2__rend_fd_encBVal(pOpts, cFNCN, pOutp, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_PICT:  /* [A]PIC imgfmt/mime + pic type */
			res = ST_TIV2__rend_fd_encPic1(pOpts, cFNCN, verMaj, pOutp, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_GEOB:  /* GEO[B] mime + filen */
			res = ST_TIV2__rend_fd_encGeo1(pOpts, cFNCN, pOutp, pFldI);
			break;
		case ST_ID3V2_TFLD_RWO_TP_CDMF:  /* CDM comprMeth + uncomprSz */
			if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
				st_id3v2_d_fdeb3(pOpts, 0, cFNCN, pFldI,
						"skipping frame...");
			break;
		case ST_ID3V2_TFLD_RWO_TP_SYLT:  /* S[Y]LT timestampfmt + conttp */
			res = ST_TIV2__rend_fd_encSlt1(pOpts, cFNCN, pOutp, pFldI);
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

/*
 * Make sure all strings have the correct encoding
 */
static Tst_err
ST_TIV2__rend_fd_corrTEs(const Tst_byte verMaj,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_mtes_textEnc *pTE)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_mtes_textEnc sltTEcur = ST_MTES_TE_ISO;

	*pTE = ST_MTES_TE_ISO;
	switch (pFldI->fldPrI.typ) {
	case ST_ID3V2_FTP_COM:  /* te: scd & text */
	case ST_ID3V2_FTP_WXX:  /* te: scd & text */
		res = ST_TIV2__rend_fd_corrTEs_equalize(verMaj,
				&pFldI->attrDesc, &pFldI->dataSArr, NULL, pTE);
		break;
	case ST_ID3V2_FTP_SLT:  /* te: scd */
	case ST_ID3V2_FTP_PIC:  /* (ISO: mime) te: scd */
		if (pFldI->fldPrI.typ == ST_ID3V2_FTP_SLT)
			sltTEcur = st_mtes_getEncodingCur(&pFldI->attrDesc);
		res = ST_TIV2__rend_fd_corrTEs_equalize(verMaj,
				&pFldI->attrDesc, NULL, NULL, pTE);
		if (res == ST_ERR_SUCC &&
				pFldI->fldPrI.typ == ST_ID3V2_FTP_SLT &&
				sltTEcur != *pTE)
			res = ST_TIV2__rend_fd_corrTEs_sylt(pFldI, sltTEcur, *pTE);
		break;
	case ST_ID3V2_FTP_TXT:  /* te: text */
	case ST_ID3V2_FTP_IPL:  /* te: text */
		res = ST_TIV2__rend_fd_corrTEs_equalize(verMaj,
				NULL, &pFldI->dataSArr, NULL, pTE);
		break;
	case ST_ID3V2_FTP_UFI:  /* ISO: scd */
	case ST_ID3V2_FTP_POP:  /* ISO: scd */
		if (st_mtes_getEncodingCur(&pFldI->attrDesc) != ST_MTES_TE_ISO)
			res = st_mtes_convStrTo_iso(&pFldI->attrDesc);
		break;
	case ST_ID3V2_FTP_URL:  /* ISO: text */
		if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
				st_mtes_getEncodingCur(
					st_mtes_strArrGetElem(&pFldI->dataSArr, 1)) != ST_MTES_TE_ISO)
			res = st_mtes_convStrTo_iso(st_mtes_strArrGetElem(&pFldI->dataSArr, 1));
		break;
	case ST_ID3V2_FTP_GEO:  /* (ISO: mime) te: filen & scd */
		res = ST_TIV2__rend_fd_corrTEs_equalize(verMaj,
				&pFldI->attrDesc, NULL, &pFldI->attrGeoFn, pTE);
		break;
	default:
		res = ST_ERR_SUCC;  /* do nothing */
	}
	return res;
}

static Tst_err
ST_TIV2__rend_fd_corrTEs_equalize(const Tst_byte verMaj,
		Tst_mtes_string *pScd, Tst_mtes_strArr *pSArr, Tst_mtes_string *pFn,
		Tst_mtes_textEnc *pTE)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 x;
	Tst_bool   allISO = ST_B_TRUE;
	Tst_mtes_textEnc saeTEcur;

	if (pScd != NULL && st_mtes_getEncodingCur(pScd) != ST_MTES_TE_ISO)
		allISO = ST_B_FALSE;
	if (allISO && pFn != NULL && st_mtes_getEncodingCur(pFn) != ST_MTES_TE_ISO)
		allISO = ST_B_FALSE;
	if (allISO && pSArr != NULL)
		for (x = 1; x <= st_mtes_strArrGetElemCount(pSArr); x++) {
			saeTEcur = st_mtes_getEncodingCur(st_mtes_strArrGetElem(pSArr, x));
			if (saeTEcur != ST_MTES_TE_ISO) {
				allISO = ST_B_FALSE;
				break;
			}
		}
	if (allISO)
		*pTE = ST_MTES_TE_ISO;  /* ID3v2.* */
	else if (verMaj < 4)
		*pTE = ST_MTES_TE_UTF16LE;  /* ID3v2.2/3 */
	else
		*pTE = ST_MTES_TE_UTF8;  /* ID3v2.4 */

	/* */
	if (pScd != NULL && st_mtes_getEncodingCur(pScd) != *pTE)
		res = st_mtes_convStr(pScd, *pTE);
	if (res == ST_ERR_SUCC &&
			pFn != NULL && st_mtes_getEncodingCur(pFn) != *pTE)
		res = st_mtes_convStr(pFn, *pTE);
	if (res == ST_ERR_SUCC && pSArr != NULL)
		for (x = 1; x <= st_mtes_strArrGetElemCount(pSArr); x++) {
			saeTEcur = st_mtes_getEncodingCur(st_mtes_strArrGetElem(pSArr, x));
			if (saeTEcur != *pTE) {
				res = st_mtes_convStr(st_mtes_strArrGetElem(pSArr, x), *pTE);
				if (res != ST_ERR_SUCC)
					break;
			}
		}
	return res;
}

/*
 * Rewrite SYLT binary data
 */
static Tst_err
ST_TIV2__rend_fd_corrTEs_sylt(Tst_id3v2_fldData_intn *pFldI,
		const Tst_mtes_textEnc teSrc,
		const Tst_mtes_textEnc teDst)
{
	Tst_err    res;
	Tst_uint32 blen,
	           copied   = 1,
	           toCpMax,
	           cpTot    = 0,
	           pchLenIn = 0,
	           pchSzOut = 0,
	           cwidSrc
	           /*cwidDst*/;
	Tst_str    *pChIn   = NULL,
	           *pChOut  = NULL;
	Tst_bool   hadBOM   = ST_B_FALSE;
	Tst_buf    tstamp[4];
	Tst_binobj   newBD;
	Tst_streamrd strrdBD;

	st_binobj_stc_initBO(&newBD);
	st_streamrd_stc_initSObj(&strrdBD);
	res = st_binobj_attachStreamrd(&pFldI->dataBinDat, &strrdBD, 0);
	if (res != ST_ERR_SUCC) {
		st_binobj_stc_freeBO(&newBD);
		st_streamrd_stc_freeSObj(&strrdBD);
		return res;
	}
	blen    = st_binobj_getDataSize(&pFldI->dataBinDat);
	cwidSrc = st_mtes_getCharWidth(teSrc);
	//cwidDst = st_mtes_getCharWidth(teDst);

	toCpMax = blen;
	while (copied > 0 && cpTot < blen) {
		/* read text fragment */
		copied = 0;
		res    = st_tagCFnc_readStr(cwidSrc, teSrc,
				&strrdBD, toCpMax, &copied,
				&pChIn, &pchLenIn, &hadBOM);
		if (res != ST_ERR_SUCC || copied < cwidSrc || cpTot + 4 > blen)
			break;
		cpTot += copied;
		/* read time stamp */
		res = st_streamrd_rdBuffer_noCRC(&strrdBD, 4, tstamp, &copied);
		if (res != ST_ERR_SUCC || copied != 4)
			break;
		cpTot += 4;

		/* convert text */
		res = st_mtes_convCharp(pChIn, pchLenIn + cwidSrc,
				teSrc, teDst, &pChOut, &pchSzOut);
		/* write text */
		if (res == ST_ERR_SUCC)
			res = st_binobj_appendData(&newBD, pChOut, pchSzOut);
		/* write timestamp */
		if (res == ST_ERR_SUCC)
			res = st_binobj_appendData(&newBD, tstamp, 4);
		if (res != ST_ERR_SUCC)
			break;
	}

	ST_DELPOINT(pChIn)
	ST_DELPOINT(pChOut)
	st_binobj_detachStreamrd(&pFldI->dataBinDat);
	st_streamrd_stc_freeSObj(&strrdBD);

	if (res == ST_ERR_SUCC)
		res = st_binobj_copy(&newBD, &pFldI->dataBinDat);

	st_binobj_stc_freeBO(&newBD);

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Text-Encoding
 */
static Tst_err
ST_TIV2__rend_fd_encTE(const Tst_id3v2_opts *pOpts, Tst_binobj *pOutp,
		const Tst_mtes_textEnc te)
{
	Tst_buf buf[1] = {0};

	switch (te) {
	case ST_MTES_TE_ISO:     buf[0] = 0x00; break;  /* ID3v2.(2,3,4) */
	case ST_MTES_TE_UTF16LE: buf[0] = 0x01; break;  /* ID3v2.(2,3,4) */
	case ST_MTES_TE_UTF16BE: buf[0] = 0x02; break;  /* ID3v2.4 */
	case ST_MTES_TE_UTF8:    buf[0] = 0x03; break;  /* ID3v2.4 */
	default:
		return ST_ERR_FAIL;
	}
	return st_binobj_appendData(pOutp, buf, 1);
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Language ("eng" / ...)
 */
static Tst_err
ST_TIV2__rend_fd_encLang(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_binobj *pOutp,
		const Tst_id3v2_fldData_intn *pFldI)
{
	const char *cDEFLANG = "xxx";
	char const *pL = cDEFLANG;

	ST_ASSERTN_NUM(ST_ERR_FAIL, sizeof(pFldI->attrLang) < 4)

	if (pFldI->attrLang[3] != 0x00)
		return ST_ERR_FAIL;

	if (st_sysStrlen(pFldI->attrLang) == 3)
		pL = (char*)pFldI->attrLang;
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
				"lng '%s'", pL);

	return st_binobj_appendData(pOutp, (Tst_buf*)pL, 3);
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Binary data
 */
static Tst_err
ST_TIV2__rend_fd_encBinDat(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_binobj *pOutp,
		Tst_id3v2_fldData_intn *pFldI)
{
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
				"bin %u bytes",
				st_binobj_getDataSize(&pFldI->dataBinDat));

	if (st_binobj_getDataSize(&pFldI->dataBinDat) > 0)
		return st_binobj_appendData_binobj(pOutp, &pFldI->dataBinDat);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Unsigned int value
 */
static Tst_err
ST_TIV2__rend_fd_encUVal(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_binobj *pOutp,
		const Tst_id3v2_fldData_intn *pFldI)
{
	Tst_uint32 tmp;

	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
				"uva %u", pFldI->dataUVal);
#	if (WORDS_BIGENDIAN != 1)
	tmp = st_sysReverseByteOrder_UI32(pFldI->dataUVal);  /* ENDIAN: LE-->BE */
#	else
	tmp = pFldI->dataUVal;
#	endif

	return st_binobj_appendData(pOutp, (Tst_buf*)&tmp, 4);
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Unsigned byte value
 */
static Tst_err
ST_TIV2__rend_fd_encBVal(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_binobj *pOutp,
		const Tst_id3v2_fldData_intn *pFldI)
{
	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
				"bva %u", pFldI->dataBVal);

	return st_binobj_appendData(pOutp, (Tst_buf*)&pFldI->dataBVal, 1);
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Picture: imgfmt/mime + pic type
 */
static Tst_err
ST_TIV2__rend_fd_encPic1(const Tst_id3v2_opts *pOpts, const char *pFnc,
		const Tst_byte verMaj,
		Tst_binobj *pOutp,
		const Tst_id3v2_fldData_intn *pFldI)
{
	Tst_err       res = ST_ERR_SUCC;
	Tst_str const *pDefFE;
	Tst_buf       buf[1];
	char const    *pFE = "xxx",
	              *pMi = ST_ID3V2_MIMEDEF;

	/* image format / mime type */
	pDefFE = st_utilsFmt_getPicFmt_fext_byEnum(pFldI->attrPicFmt);
	if (st_sysStrlen(pDefFE) == 3) {
		pFE = (char*)pDefFE;
		pMi = (char*)st_utilsFmt_getPicFmt_mime_byEnum(pFldI->attrPicFmt);
		if (st_sysStrEmpty(pMi))
			pMi = ST_ID3V2_MIMEDEF;
	}
	res = ST_TIV2__rend_fd_encPic1_im(pOpts, verMaj, pOutp, pFE, pMi);

	/* render picture type (1 byte) */
	if (res == ST_ERR_SUCC) {
		buf[0] = st_id3v2_fnc_getPicTp_bin_byEnum(pFldI->attrPicTp);
		res    = st_binobj_appendData(pOutp, buf, 1);
	}

	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts)) {
		if (verMaj == 2)
			st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
					"fmt='%s',  ptp='%s'",
					pFE,
					st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp));
		else
			st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
					"mim=\"%s\",  ptp='%s'",
					pMi,
					st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp));
	}

	return res;
}

static Tst_err
ST_TIV2__rend_fd_encPic1_im(const Tst_id3v2_opts *pOpts,
		const Tst_byte verMaj,
		Tst_binobj *pOutp, const char *pIF, const char *pMim)
{
	Tst_err    res;
	Tst_uint32 slen;
	Tst_buf    buf[1] = {0};

	if (verMaj == 2) {
		/* render 'image format' as ISO encoded string (3 bytes) */
		res = st_binobj_appendData(pOutp, (Tst_buf*)pIF, 3);
	} else {
		/* render 'mime type' as ISO encoded string */
		slen = st_sysStrlen2(pMim);
		if (slen > 0)
			res = st_binobj_appendData(pOutp, (Tst_buf*)pMim, slen + 1);
		else
			res = st_binobj_appendData(pOutp, buf, 1);
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Encapsulated object: mime + filen
 */
static Tst_err
ST_TIV2__rend_fd_encGeo1(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_binobj *pOutp,
		const Tst_id3v2_fldData_intn *pFldI)
{
	Tst_err    res;
	Tst_uint32 slen;
	Tst_buf    buf[1] = {0};

	slen = st_sysStrlen(pFldI->pAttrGeoMime);

	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts)) {
		st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
				"mim=\"%s\"", pFldI->pAttrGeoMime);
		st_id3v2_d_fdeb3Str(pOpts, 2, pFnc, pFldI,
				ST_B_TRUE, "fna", &pFldI->attrGeoFn, "");
	}

	/* render 'mime type' as ISO encoded string */
	if (slen > 0)
		res = st_binobj_appendData(pOutp,
					(Tst_buf*)pFldI->pAttrGeoMime, slen + 1);
	else
		res = st_binobj_appendData(pOutp, buf, 1);
	if (res != ST_ERR_SUCC)
		return res;

	/* render 'filename' */
	return ST_TIV2__rend_fd_encStr(pOutp, &pFldI->attrGeoFn, ST_B_TRUE);
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: Synced lyrics: timestampfmt + conttp
 */
static Tst_err
ST_TIV2__rend_fd_encSlt1(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_binobj *pOutp,
		const Tst_id3v2_fldData_intn *pFldI)
{
	Tst_buf buf[2];
	Tst_id3v2_sltTSF tsf = pFldI->attrSltTSF;
	Tst_id3v2_sltCTp ctp = pFldI->attrSltCTP;

	/* time stamp format (1 byte) */
	switch (pFldI->attrSltTSF) {
	case ST_ID3V2_SLTTSF_MILLI:
		buf[0] = 0x02;
		break;
	default:  /* ST_ID3V2_SLTTSF_MPEG */
		buf[0] = 0x01;
		tsf    = ST_ID3V2_SLTTSF_MPEG;
	}
	/* content type (1 byte) */
	switch (pFldI->attrSltCTP) {
	case ST_ID3V2_SLTCTP_LYR:    buf[1] = 0x01; break;
	case ST_ID3V2_SLTCTP_TRANS:  buf[1] = 0x02; break;
	case ST_ID3V2_SLTCTP_MOVE:   buf[1] = 0x03; break;
	case ST_ID3V2_SLTCTP_EVENT:  buf[1] = 0x04; break;
	case ST_ID3V2_SLTCTP_CHORD:  buf[1] = 0x05; break;
	case ST_ID3V2_SLTCTP_TRIV:   buf[1] = 0x06; break;
	case ST_ID3V2_SLTCTP_URLWP:  buf[1] = 0x07; break;
	case ST_ID3V2_SLTCTP_URLIMG: buf[1] = 0x08; break;
	default:  /* ST_ID3V2_SLTCTP_OTHER */
		buf[1] = 0x00;
		ctp    = ST_ID3V2_SLTCTP_OTHER;
	}

	if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_id3v2_d_fdeb3(pOpts, 2, pFnc, pFldI,
				"tsf %d, ctp %d", tsf, ctp);
	return st_binobj_appendData(pOutp, buf, 2);
}

/*----------------------------------------------------------------------------*/

/*
 * Render frame data: String
 */
static Tst_err
ST_TIV2__rend_fd_encStr(Tst_binobj *pOutp,
		const Tst_mtes_string *pStr, const Tst_bool withTerm)
{
	Tst_err    res      = ST_ERR_SUCC;
	Tst_uint32 slen,
	           newBomSz = 0,
	           termSz   = 0,
	           spos     = 0;
	Tst_buf    term[2]  = {0, 0};

	switch (st_mtes_getEncodingCur(pStr)) {
	case ST_MTES_TE_ISO:
	case ST_MTES_TE_UTF8:
		termSz = (withTerm ? 1 : 0);
		break;
	case ST_MTES_TE_UTF16LE:
	case ST_MTES_TE_UTF16BE:
		newBomSz = (st_mtes_getEncodingCur(pStr) == ST_MTES_TE_UTF16LE ? 2 : 0);
		termSz   = (withTerm ? 2 : 0);
		spos     = st_mtes_getStrBOMsz(pStr);
		break;
	default:
		return ST_ERR_FAIL;  /* no compromises here my friend */
	}
	slen = st_mtes_strlenBy(pStr);

	/* UTF16-LE BOM */
	if (newBomSz > 0)
		res = st_binobj_appendData(pOutp,
				(Tst_buf*)&ST_UTF16_BOM_LE, newBomSz);
	/* the string itself */
	if (res == ST_ERR_SUCC && slen > 0)
		res = st_binobj_appendData(pOutp,
				(const Tst_buf*)&st_mtes_getDataPt(pStr)[spos], slen);
	/* NUL-Terminator */
	if (res == ST_ERR_SUCC && termSz > 0)
		res = st_binobj_appendData(pOutp, term, termSz);

	return res;
}

/*
 * Render frame data: String
 */
static Tst_err
ST_TIV2__rend_fd_encMultStr(const Tst_id3v2_opts *pOpts, const char *pFnc,
		Tst_binobj *pOutp,
		const Tst_mtes_strArr *pStrArr,
		const Tst_id3v2_fldData_intn *pFldI, const Tst_bool isTaggInf)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 x,
	           saCnt;

	saCnt = st_mtes_strArrGetElemCount(pStrArr);
	for (x = 1; x <= saCnt; x++) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts) && ! isTaggInf)
			st_id3v2_d_fdeb3Str(pOpts, 2, pFnc, pFldI,
					ST_B_TRUE,
					(saCnt > 1 ? "mul" : "txt"),
					st_mtes_strArrGetElem(pStrArr, x), "");
		res = ST_TIV2__rend_fd_encStr(pOutp,
				st_mtes_strArrGetElem(pStrArr, x),
				(x + 1 <= saCnt));
		if (res != ST_ERR_SUCC)
			break;
	}
	return res;
}

/******************************************************************************/
