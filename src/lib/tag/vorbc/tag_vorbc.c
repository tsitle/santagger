/******************************************************************************/
/* tag_vorbc.c                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Vorbis Tags                                                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_TAG_VORBC_C
#include "tag_vorbc-prot.h"
#include "tag_vorbc-priv_flds.h"
/*** */
#include "tag_vorbc_fnc-prot.h"
#include "tag_vorbc_stcs-prot.h"
#include "tag_vorbc_ztidy-prot.h"
#include "tag_vorbc_zrd-pp.h"
#include "tag_vorbc_zwr-pp.h"
#include "tag_vorbc_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#include "../../utils/w-base64-prot.h"
#undef SRC_TAG_VORBC_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef enum {
	LOC_WRMD_UP,
	LOC_WRMD_SW,
	LOC_WRMD_BO
} Tst_vorbc__writeMode;

static Tst_err
ST_TVORBC__readTag(const char *pFnc,
                   Tst_streamrd *pSObjP, Tst_vorbc_tag *pTag);
static Tst_err
ST_TVORBC__disableFld(const char *pFnc, Tst_vorbc_tag *pTag,
                      const Tst_vorbc_frID frID,
                      ST_OPTARG(const Tst_str *pFrIDstr),
                      const Tst_uint32 frNr,
                      const Tst_bool beSilent);
static Tst_err
ST_TVORBC__writeTag(const char *pFnc, Tst_vorbc_tag *pTag,
                    const Tst_vorbc__writeMode wrMd, Tst_binobj *pBinDat);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read Vorbis Tag from stream
 *
 * pTag            MUST be set
 * pTagI->pFilen   MUST be set
 * pTagI->pStrrd   MUST be set
 *
 * returns: ERRCODE
 *          pTag              will be set
 *          pTagI->rawTagBDat will be set
 */
Tst_err
st_vorbc_readTagFromStream(Tst_vorbc_tag *pTag)
{
#	define LOC_FNCN_  __func__
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	return ST_TVORBC__readTag(LOC_FNCN_,
			((Tst_vorbc_tag_intn*)pTag->pObInternal)->pStrrd, pTag);
#	undef LOC_FNCN_
}

/*
 * Read Vorbis Tag from binary object
 *
 * pTag            MUST be set
 * pTagI->pFilen   MUST be set
 * pBinDat         MUST be set
 *
 * returns: ERRCODE
 *          pTag              will be set
 *          pTagI->rawTagBDat will be set
 */
Tst_err
st_vorbc_readTagFromBinObj(Tst_vorbc_tag *pTag, Tst_binobj *pBinDat)
{
#	define LOC_FNCN_  __func__
	Tst_err res;
	Tst_streamrd strrd;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pBinDat == NULL)

	if (st_binobj_getDataSize(pBinDat) == 0) {
		st_vorbc_stc_rsetTag_prot(pTag, ST_B_FALSE,
				ST_B_FALSE, NULL,
				ST_B_FALSE, NULL,
				ST_B_FALSE, NULL);
		return ST_ERR_SUCC;
	}

	/* init stream reader */
	res = st_streamrd_stc_initSObj(&strrd);
	if (res != ST_ERR_SUCC)
		return res;
	res = st_binobj_attachStreamrd(pBinDat, &strrd, 0);
	if (res != ST_ERR_SUCC) {
		st_streamrd_stc_freeSObj(&strrd);
		return res;
	}

	/* read tag */
	res = ST_TVORBC__readTag(LOC_FNCN_, &strrd, pTag);

	/* free stream reader */
	st_binobj_detachStreamrd(pBinDat);
	st_streamrd_stc_freeSObj(&strrd);

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Add pTagSrc to pTagDst
 *   This means that all fields from pTagSrc are appended
 *   to pTagDst, if possible
 */
Tst_err
st_vorbc_mergeTags(Tst_vorbc_tag *pTagDst, Tst_vorbc_tag *pTagSrc,
		const Tst_bool compareFlds)
{
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool hasEquFld;
	Tst_vorbc_fldData const *pItFld = NULL;

	ST_ASSERTN_IARG(pTagDst == NULL || pTagSrc == NULL)

	while ((pItFld = st_vorbc_ite_nextValidFld(pTagSrc, pItFld)) != NULL) {
		if (compareFlds) {
			hasEquFld = ST_B_FALSE;
			res       = st_vorbc_fldAdd_doesSimiliarFieldExist(pTagDst,
					ST_B_TRUE, pItFld, &hasEquFld);
			if (res != ST_ERR_SUCC)
				break;
			if (hasEquFld)
				continue;
		}
		/* */
		res = st_vorbc_fldAdd(pTagDst, pItFld);
		if (res != ST_ERR_SUCC)
			break;
	}
	if (res == ST_ERR_SUCC)
		st_vorbc_tidyUpTag(pTagDst);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Disable all fields
 */
Tst_err
st_vorbc_disableFld_all(Tst_vorbc_tag *pTag)
{
#	define LOC_FNCN_  __func__

	return ST_TVORBC__disableFld(LOC_FNCN_, pTag,
			ST_VORBC_FID_NONE, NULL, /*frNr:*/0, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*
 * Disable all fields with the ID frID
 */
Tst_err
st_vorbc_disableFld_byID_all(Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
#	define LOC_FNCN_  __func__

	return ST_TVORBC__disableFld(LOC_FNCN_, pTag,
			frID, pFrIDstr, /*frNr:*/0, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*
 * Disable field with the ID frID and the field number frNr
 */
Tst_err
st_vorbc_disableFld_byID(Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr)
{
#	define LOC_FNCN_  __func__

	if (frNr == 0)
		return ST_ERR_IARG;
	return ST_TVORBC__disableFld(LOC_FNCN_, pTag,
			frID, pFrIDstr, frNr, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Tidy up the tag a bit
 */
Tst_err
st_vorbc_tidyUpTag(Tst_vorbc_tag *pTag)
{
	const char *cFNCN = __func__;
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool discardedOne,
	         discardedOneF;
	Tst_vorbc_tag_intn     *pTagI;
	Tst_vorbc_fldData      *pItFld;
	Tst_vorbc_fldData_intn *pItFldI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
				"starting to tidy up for Vorbis tag...");

	/* remove Tagger field */
	res = ST_TVORBC__disableFld(cFNCN, pTag, ST_VORBC_FID_TAGG, NULL,
			/*frNr:*/0, /*silent:*/ST_B_TRUE);

	/* map and merge TRACK/TRACKTOTAL and DISC/DISCTOTAL fields */
	if (res == ST_ERR_SUCC) {
		res = st_vorbc_tidy_mapMergePosFlds(cFNCN, pTag);
		if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"mapping/merging pos-type fields failed");
	}

	/* discard duplicate fields */
	do {
		pItFld       = NULL;
		discardedOne = ST_B_FALSE;
		while (res == ST_ERR_SUCC &&
				(pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
			discardedOneF = ST_B_FALSE;
			/* */
			res = st_vorbc_tidy_remDups(cFNCN, pTag, pItFld, &discardedOneF);
			if (res != ST_ERR_SUCC)
				st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
						"removing duplicate fields failed");
			discardedOne = discardedOne || discardedOneF;
		}
	} while (res == ST_ERR_SUCC && discardedOne);

	/* convert all IPLS strings into arrays */
	pItFld = NULL;
	while (res == ST_ERR_SUCC &&
			(pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.id != ST_VORBC_FID_IPLS ||
				st_mtes_strArrGetElemCount(&pItFldI->dataSArr) != 1)
			continue;
		res = st_tagCFnc_iplsSplit(&pItFldI->dataSArr);
		if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"splitting up IPLS string failed");
	}

	/* merge fields */
	pItFld = NULL;
	while (res == ST_ERR_SUCC &&
			(pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.typ != ST_VORBC_FTP_STR)
			continue;
		res = st_vorbc_tidy_mergFields(cFNCN, pTag, pItFld);
		if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"merging fields failed");
	}

	/* merge multiple-strings */
	pItFld = NULL;
	while (res == ST_ERR_SUCC &&
			(pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.typ == ST_VORBC_FTP_PICT ||
				st_mtes_strArrGetElemCount(&pItFldI->dataSArr) < 2)
			continue;
		if (pItFldI->fldPrI.id == ST_VORBC_FID_IPLS) {
			/* convert IPLS array into single string */
			res = st_tagCFnc_iplsMerge(&pItFldI->dataSArr);
		} else
			res = st_vorbc_tidy_mergMultStr(cFNCN, &pTagI->opts, pItFld);
		if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"merging multi-strings failed");
	}

	/* re-check all fields */
	if (res == ST_ERR_SUCC)
		res = st_vorbc_tidy_chkAllFields(cFNCN, pTag);

	/* check/set vendor string */
	if (res == ST_ERR_SUCC && st_mtes_strlenCh(&pTagI->vendor) < 5) {
		/* --> "libsantag x.y.z" */
		st_tagCFnc_getTaggerStr(&pTagI->vendor, ST_B_FALSE);
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_fdebStr(&pTagI->opts, 0, cFNCN, NULL,
					"set vendor to", &pTagI->vendor, "");
	}

	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
				"everything very tidy now");

	/* update hasFields */
	if (res == ST_ERR_SUCC)
		st_tagBas_gs_setHasFields(&pTagI->tbas,
				st_vorbc_gs_getTag_amountValidFlds(pTag) > 0);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Update raw Tag
 *   (not required before using st_vorbc_writeTagToStream()
 *    or st_vorbc_writeTagToBinObj() )
 *
 * pTag            MUST be set
 * pTagI->pFilen   MUST be set
 *
 * returns: ERRCODE
 *          pTagI->rawTagBDat will be set
 */
Tst_err
st_vorbc_updateRawTag(Tst_vorbc_tag *pTag)
{
#	define LOC_FNCN_  __func__

	return ST_TVORBC__writeTag(LOC_FNCN_, pTag, LOC_WRMD_UP, NULL);
#	undef LOC_FNCN_
}

/*
 * Write Tag to stream (also updates the tag before writing)
 *
 * pTag            MUST be set
 * pTagI->pStrwr   MUST be set
 *        pFilen   MUST be set
 *
 * returns: ERRCODE
 *          pTagI->rawTagBDat will be set
 */
Tst_err
st_vorbc_writeTagToStream(Tst_vorbc_tag *pTag)
{
#	define LOC_FNCN_  __func__

	return ST_TVORBC__writeTag(LOC_FNCN_, pTag, LOC_WRMD_SW, NULL);
#	undef LOC_FNCN_
}

/*
 * Write Tag to pBinDat (also updates the tag before writing)
 *
 * pTag            MUST be set
 * pTagI->pFilen   MUST be set
 *
 * returns: ERRCODE
 *          pTagI->rawTagBDat will be set
 */
Tst_err
st_vorbc_writeTagToBinObj(Tst_vorbc_tag *pTag, Tst_binobj *pBinDat)
{
#	define LOC_FNCN_  __func__

	return ST_TVORBC__writeTag(LOC_FNCN_, pTag, LOC_WRMD_BO, pBinDat);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read Vorbis Tag from stream
 */
static Tst_err
ST_TVORBC__readTag(const char *pFnc,
		Tst_streamrd *pSObjP, Tst_vorbc_tag *pTag)
{
	Tst_err    res;
	Tst_bool   isOK   = ST_B_FALSE;
	Tst_uint32 nflds  = 0,
	           headSz = 0;
	Tst_vorbc_tag_intn *pTagI;

	ST_ASSERTN_IARG(pSObjP == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	st_vorbc_stc_rsetTag_prot(pTag, ST_B_FALSE,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL);

	/* read Pseudo Vorbis Header */
	res = st_vorbc_rd_readHeader(pSObjP, pTagI, &isOK, &nflds, &headSz);
	if (res != ST_ERR_SUCC || ! isOK) {
		if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"reading header failed");
		return res;
	}

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		Tst_uint32 tmpTagSz = st_tagBas_gs_getSize(&pTagI->tbas);

		if (tmpTagSz < 1024)
			st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
					"Vorbis tag size=%u bytes",
					tmpTagSz);
		else if (tmpTagSz < 1024 * 1024)
			st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
					"Vorbis tag size=%u bytes (%.2f KB)",
					tmpTagSz, (double)tmpTagSz / 1024.0);
		else
			st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
					"Vorbis tag size=%u bytes (%u MB)",
					tmpTagSz,
					(Tst_uint32)((double)tmpTagSz / (1024.0 * 1024.0)));
	}

	/* read tag fields */
	res = st_vorbc_rd_readFlds(pSObjP, pTag, headSz, nflds);
	if (res != ST_ERR_SUCC)
		st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
				"reading fields failed");
	else if (! pTagI->opts.rdOnlyBasics &&
			ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
			! st_tagBas_gs_getHasTag(&pTagI->tbas))
		st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
				"tag is empty");

	/* get tagger info */
	if (res == ST_ERR_SUCC && ! pTagI->opts.rdOnlyBasics) {
		Tst_mtes_strArr tmpSArr;

		st_mtes_stc_initSA(&tmpSArr);
		res = st_vorbc_fldGet_dataStrArr(pTag, ST_VORBC_FID_TAGG, NULL,
				/*frNr:*/0, &tmpSArr);
		if (res == ST_ERR_SUCC &&
				st_mtes_strArrGetElemCount(&tmpSArr) == 1) {
			res = st_mtes_strCpy(st_mtes_strArrGetElem(&tmpSArr, 1), &pTagI->tagger);
			if (res == ST_ERR_SUCC) {
				Tst_mtes_strRestr restr;

				/* strip Tagger string */
				/** */
				restr.hasR    = ST_B_TRUE;  /* we have restrictions */
				restr.ruleAlw = ST_B_TRUE;  /* mode: allow */
				st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
				/** disallow '\n' and '\t' */
				restr.disNewln = ST_B_TRUE;
				restr.disTab   = ST_B_TRUE;
				/** */
				res = st_mtes_stripStr(&pTagI->tagger, &restr, NULL, NULL);
				if (res == ST_ERR_SUCC)
					st_mtes_strReplace(&pTagI->tagger,
							(const Tst_str*)"%", NULL, NULL);
			}
		} else if (res == ST_ERR_TFNF)
			res = ST_ERR_SUCC;
		st_mtes_stc_freeSA(&tmpSArr);
	}

	/* tidy up tag */
	if (res == ST_ERR_SUCC)
		st_vorbc_tidyUpTag(pTag);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Disable field(s)
 */
static Tst_err
ST_TVORBC__disableFld(const char *pFnc, Tst_vorbc_tag *pTag,
		const Tst_vorbc_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr, const Tst_bool beSilent)
{
	Tst_vorbc_fldData      *pItFld = NULL;
	Tst_vorbc_fldData_intn *pItFldI;
	Tst_vorbc_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

	while ((pItFld = st_vorbc_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if ((frNr != 0 && pItFldI->fnr != frNr) ||
				! st_vorbc_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
							frID, (const char*)pFrIDstr))
			continue;
		if (! beSilent &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_fdeb(&pTagI->opts, 0, pFnc, pItFldI,
					"disabling field...");
		st_vorbc_stc_rsetFDat_onlyBufs(pItFld);
		pItFldI->isOK = ST_B_FALSE;
		if (frNr != 0)
			break;
	}
	st_tagBas_gs_setHasFields(&pTagI->tbas,
			st_vorbc_gs_getTag_amountValidFlds(pTag) > 0);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Write Tag
 *
 * pTag            MUST be set
 * pTagI->pStrwr   MUST be set
 *        tbas     MUST be set
 *        pFilen   MUST be set
 *
 * returns: ERRCODE
 *          pTagI->rawTagBDat will be set
 */
static Tst_err
ST_TVORBC__writeTag(const char *pFnc, Tst_vorbc_tag *pTag,
		const Tst_vorbc__writeMode wrMd, Tst_binobj *pBinDat)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 lastTSz,
	           newTSz;
	Tst_tagBasics          tbasNew;
	Tst_vorbc_fldData      *pItFld  = NULL;
	Tst_vorbc_fldData_intn *pItFldI = NULL;
	Tst_vorbc_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(wrMd == LOC_WRMD_SW && pTagI->pStrwr == NULL)

	/* reset output binary object */
	if (wrMd == LOC_WRMD_BO)
		st_binobj_stc_rsetBO(pBinDat);
	st_binobj_stc_rsetBO(&pTagI->rawTagBDat);

	st_tagBas_stc_initTB(&tbasNew);

	lastTSz = (st_tagBas_gs_getHasTag(&pTagI->tbas) ?
			st_tagBas_gs_getSize(&pTagI->tbas) : 0);
	while ((pItFld = st_vorbc_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_vorbc_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		pItFldI->wasRendered = ST_B_FALSE;
	}

	/* tidy up tag (remove duplicates, ...) */
	res = st_vorbc_tidyUpTag(pTag);
	if (res != ST_ERR_SUCC) {
		st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
				"tidying up tag failed");
		st_tagBas_stc_freeTB(&tbasNew);
		return res;
	}

	/* render tag (write everything to binary object) */
	res = st_vorbc_wr_rendTagToBO(pTag, &tbasNew, &pTagI->rawTagBDat);
	if (res != ST_ERR_SUCC) {
		if (res != ST_ERR_ABRT)
			st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"rendering tag failed");
		st_tagBas_stc_freeTB(&tbasNew);
		return res;
	}

	newTSz = st_tagBas_gs_getSize(&tbasNew);
	if (newTSz > ST_VORBC_MAX_TAGSZ) {
		st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
				"tag too big: "
				"%u bytes (%.2f MB), max=%u bytes (%u MB), aborting",
				newTSz, (float)newTSz / 1024.0 / 1024.0,
				ST_VORBC_MAX_TAGSZ,
				(Tst_uint32)((float)ST_VORBC_MAX_TAGSZ / 1024.0 / 1024.0));
		st_tagBas_stc_freeTB(&tbasNew);
		return ST_ERR_ABRT;
	}

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		if (newTSz < 1024)
			st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
					"tag size=%u bytes (d %+d bytes)",
					newTSz, (Tst_int32)newTSz - (Tst_int32)lastTSz);
		else if (newTSz < 1024 * 1024)
			st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
					"tag size=%u bytes (%.2f KB, d %+d bytes)",
					newTSz, (double)newTSz / 1024.0,
					(Tst_int32)newTSz - (Tst_int32)lastTSz);
		else
			st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
					"tag size=%u bytes (%u MB, d %+d bytes)",
					newTSz,
					(Tst_uint32)((double)newTSz / (1024.0 * 1024.0)),
					(Tst_int32)newTSz - (Tst_int32)lastTSz);
	}

	if (wrMd == LOC_WRMD_BO) {
		/* copy raw tag to pBinDat */
		res = st_binobj_copy(&pTagI->rawTagBDat, pBinDat);
		if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"copying raw tag to output failed");
	}

	/* write tag to stream */
	if (wrMd == LOC_WRMD_SW && res == ST_ERR_SUCC)
		res = st_tagCFnc_writeTagBOtoStream(&pTagI->opts.basOpts, &pTagI->opts,
				pFnc, st_vorbc_d_debCB, st_vorbc_d_errCB,
				pTagI->pFilen, &pTagI->tbas,
				pTagI->pStrwr, &pTagI->rawTagBDat);
	st_tagBas_stc_freeTB(&tbasNew);
	if (wrMd == LOC_WRMD_SW && res != ST_ERR_SUCC && res != ST_ERR_ABRT)
		st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
				"writing tag failed");

	return res;
}

/******************************************************************************/
