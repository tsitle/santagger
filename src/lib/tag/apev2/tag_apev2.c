/******************************************************************************/
/* tag_apev2.c                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for APEv2 Tags                                                   */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_C
#include "tag_apev2-prot.h"
#include "tag_apev2-priv_flds.h"
/*** */
#include "tag_apev2_fnc-prot.h"
#include "tag_apev2_stcs-prot.h"
#include "tag_apev2_zfdeb-prot.h"
#include "tag_apev2_ztidy-prot.h"
#include "tag_apev2_zrd-pp.h"
#include "tag_apev2_zwr-pp.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_APEV2_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TAV2__disableFld(const char *pFnc, Tst_apev2_tag *pTag,
                    const Tst_bool useIDorFTP,
                    const Tst_apev2_frID frID,
                    ST_OPTARG(const Tst_str *pFrIDstr),
                    const Tst_uint32 frNr,
                    const Tst_apev2_frTp frTp,
                    const Tst_bool beSilent);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read APEv2 (Monkey Audio Codec's Tag) Tag
 *
 * pTag            MUST be set
 * pTagI->pFilen   MUST be set
 * pTagI->pStrrd   MUST be set
 *
 * returns: ERRCODE
 *          pTag  will be set
 */
Tst_err
st_apev2_readTagFromStream(Tst_apev2_tag *pTag)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_bool   isOK  = ST_B_FALSE;
	Tst_uint32 tsizeH = 0,
	           tsizeF = 0,
	           nflds  = 0,
	           remSz;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	st_apev2_stc_rsetTag_prot(pTag, ST_B_FALSE,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL);

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL || pTagI->pStrrd == NULL)

	/* APE Tag shouldn't be present, if ID3v1 Tag is,
	 * but people often don't care about standards    */

	/* read APE Header */
	res = st_apev2_rd_readHdOrFoot(pTagI, ST_B_FALSE, &isOK, &tsizeH, &nflds);
	if (res != ST_ERR_SUCC || ! isOK) {
		if (res != ST_ERR_SUCC)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"reading header failed");
		return res;
	}

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		Tst_uint32 tmpTagSz = st_tagBas_gs_getSize(&pTagI->tbas);

		if (tmpTagSz < 1024)
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"APEv2 tag size=%u bytes",
					tmpTagSz);
		else if (tmpTagSz < 1024 * 1024)
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"APEv2 tag size=%u bytes (%.2f KB)",
					tmpTagSz, (double)tmpTagSz / 1024.0);
		else
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"APEv2 tag size=%u bytes (%u MB)",
					tmpTagSz, (Tst_uint32)((double)tmpTagSz / (1024.0 * 1024.0)));
	}

	ST_ASSERTN_NUM(ST_ERR_FAIL,
			st_tagBas_gs_getSize(&pTagI->tbas) < ST_APEV2_TAG_HEADFOOT_SZ)
	remSz = st_tagBas_gs_getSize(&pTagI->tbas) - ST_APEV2_TAG_HEADFOOT_SZ;

	if (pTagI->opts.rdOnlyBasics) {
		/* skip remaining tag incl. Footer */
		st_streamrd_rdSkipBytes(pTagI->pStrrd, remSz, ST_B_TRUE);
		/* update/correct tag size */
		st_apev2_rd_updateTagSize(pTagI, cFNCN, ST_B_TRUE, 0);
		return ST_ERR_SUCC;
	}

	/* read tag fields */
	res = st_apev2_rd_readTagFlds(pTag, nflds, &remSz);
	if (res != ST_ERR_SUCC)
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"reading tag fields failed");
	else if (! st_tagBas_gs_getHasFields(&pTagI->tbas) &&
			ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"tag is empty");

	/* read APE Footer */
	if (res == ST_ERR_SUCC && ! st_streamrd_isEOF(pTagI->pStrrd) &&
			remSz >= ST_APEV2_TAG_HEADFOOT_SZ) {
		res    = st_apev2_rd_readHdOrFoot(pTagI, ST_B_TRUE, &isOK, &tsizeF, &nflds);
		remSz -= ST_APEV2_TAG_HEADFOOT_SZ;
		if (res != ST_ERR_SUCC)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"reading footer failed");
	}

	/* skip remaining bytes */
	if (remSz > 0 && ! st_streamrd_isEOF(pTagI->pStrrd))
		st_streamrd_rdSkipBytes(pTagI->pStrrd, remSz, ST_B_TRUE);

	/* update/correct tag size */
	if (res == ST_ERR_SUCC)
		st_apev2_rd_updateTagSize(pTagI, cFNCN, ST_B_TRUE, 0);

	/* get tagger info */
	if (res == ST_ERR_SUCC && ! pTagI->opts.rdOnlyBasics) {
		Tst_mtes_strArr tmpSArr;

		st_mtes_stc_initSA(&tmpSArr);
		res = st_apev2_fldGet_dataStrArr(pTag, ST_APEV2_FID_TAGG, NULL,
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

	/* tidy up */
	if (res == ST_ERR_SUCC)
		res = st_apev2_tidyUpTag(pTag);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read APEv2 (Monkey Audio Codec's Tag) Tag Footer
 *   and get relative offset of Tag Header
 *
 * pTag                      MUST be set
 * pTagI->pFilen             MUST be set
 * pTagI->pStrrd             MUST be set
 * pRelativeOffsetOfHeader   MUST be != NULL
 *
 * returns: ERRCODE
 *          pRelativeOffsetOfHeader will be set
 */
Tst_err
st_apev2_readTagFooterFromStream(Tst_apev2_tag *pTag,
		Tst_foffs *pRelativeOffsetOfHeader)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_bool   isOK   = ST_B_FALSE;
	Tst_uint32 tsizeF = 0,
	           nflds  = 0;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pRelativeOffsetOfHeader == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL || pTagI->pStrrd == NULL)

	*pRelativeOffsetOfHeader = 0;

	res = st_apev2_rd_readHdOrFoot(pTagI, ST_B_TRUE, &isOK, &tsizeF, &nflds);
	if (res != ST_ERR_SUCC)
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"reading footer failed");
	else if (! isOK)
		res = ST_ERR_IDAT;
	else
		*pRelativeOffsetOfHeader = -1 * (Tst_foffs)tsizeF;

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Add pTagSrc to pTagDst
 *   This means that all fields from pTagSrc are appended
 *   to pTagDst, if possible
 */
Tst_err
st_apev2_mergeTags(Tst_apev2_tag *pTagDst, Tst_apev2_tag *pTagSrc,
		const Tst_bool compareFlds)
{
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool hasEquFld;
	Tst_apev2_fldData const *pItFld = NULL;

	ST_ASSERTN_IARG(pTagDst == NULL || pTagSrc == NULL)

	while ((pItFld = st_apev2_ite_nextValidFld(pTagSrc, pItFld)) != NULL) {
		if (compareFlds) {
			hasEquFld = ST_B_FALSE;
			res       = st_apev2_fldAdd_doesSimiliarFieldExist(pTagDst,
					ST_B_TRUE, pItFld, &hasEquFld);
			if (res != ST_ERR_SUCC)
				break;
			if (hasEquFld)
				continue;
		}
		/* */
		res = st_apev2_fldAdd(pTagDst, pItFld);
		if (res != ST_ERR_SUCC)
			break;
	}

	/* tidy up */
	if (res == ST_ERR_SUCC)
		res = st_apev2_tidyUpTag(pTagDst);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Disable all fields with the ID frID
 *   If frID is ST_APEV2_FID_CTXT
 *     then a ID-String must be given in pFrIDstr
 *   If pFrIDstr contains the name of a custom field
 *     then frID must be either ST_APEV2_FID_CTXT or ST_APEV2_FID_CBIN
 */
Tst_err
st_apev2_disableFld_byID_all(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
#	define LOC_FNCN_  __func__

	return ST_TAV2__disableFld(LOC_FNCN_, pTag,
			/*useIDorFTP:*/ST_B_TRUE,
			frID, pFrIDstr, /*frNr:*/0,
			ST_APEV2_FTP_NONE, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*
 * Disable field with the ID frID and the field number frNr
 *   If frID is ST_APEV2_FID_CTXT
 *     then a ID-String must be given in pFrIDstr
 *   If pFrIDstr contains the name of a custom field
 *     then frID must be either ST_APEV2_FID_CTXT or ST_APEV2_FID_CBIN
 */
Tst_err
st_apev2_disableFld_byID(Tst_apev2_tag *pTag,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr)
{
#	define LOC_FNCN_  __func__

	if (frNr == 0)
		return ST_ERR_IARG;
	return ST_TAV2__disableFld(LOC_FNCN_, pTag,
			/*useIDorFTP:*/ST_B_TRUE,
			frID, pFrIDstr, frNr,
			ST_APEV2_FTP_NONE, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*
 * Disable all fields of the type frTp
 */
Tst_err
st_apev2_disableFld_byFTP_all(Tst_apev2_tag *pTag, const Tst_apev2_frTp frTp)
{
#	define LOC_FNCN_  __func__

	return ST_TAV2__disableFld(LOC_FNCN_, pTag,
			/*useIDorFTP:*/ST_B_FALSE,
			ST_APEV2_FID_NONE, /*pFrIDstr*/NULL, /*frNr:*/0,
			frTp, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*
 * Disable field of the type frTp and the field number frNr
 */
Tst_err
st_apev2_disableFld_byFTP(Tst_apev2_tag *pTag,
		const Tst_apev2_frTp frTp, const Tst_uint32 frNr)
{
#	define LOC_FNCN_  __func__

	if (frNr == 0)
		return ST_ERR_IARG;
	return ST_TAV2__disableFld(LOC_FNCN_, pTag,
			/*useIDorFTP:*/ST_B_FALSE,
			ST_APEV2_FID_NONE, /*pFrIDstr*/NULL, frNr,
			frTp, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Tidy up the tag
 */
Tst_err
st_apev2_tidyUpTag(Tst_apev2_tag *pTag)
{
	const char *cFNCN = __func__;
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool discardedOne,
	         discardedOneF;
	Tst_apev2_tag_intn     *pTagI;
	Tst_apev2_fldData      *pItFld;
	Tst_apev2_fldData_intn *pItFldI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"starting to tidy up for APEv2 tag...");

	/* remove Tagger field */
	res = ST_TAV2__disableFld(cFNCN, pTag, /*useIDorFTP:*/ST_B_TRUE,
			ST_APEV2_FID_TAGG, NULL,
			/*frNr:*/0, ST_APEV2_FTP_NONE, /*silent:*/ST_B_TRUE);

	/* map and merge TRACK/TRACKTOTAL and DISC/DISCTOTAL fields */
	if (res == ST_ERR_SUCC) {
		res = st_apev2_tidy_mapMergePosFlds(cFNCN, pTag);
		if (res != ST_ERR_SUCC)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"mapping/merging pos-type fields failed");
	}

	/* discard duplicate fields */
	do {
		pItFld       = NULL;
		discardedOne = ST_B_FALSE;
		while (res == ST_ERR_SUCC &&
				(pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
			discardedOneF = ST_B_FALSE;
			/* */
			res = st_apev2_tidy_remDups(cFNCN, pTag, pItFld, &discardedOneF);
			if (res != ST_ERR_SUCC)
				st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
						"removing duplicate fields failed");
			discardedOne = discardedOne || discardedOneF;
		}
	} while (res == ST_ERR_SUCC && discardedOne);

	/* convert all IPLS strings into arrays */
	pItFld = NULL;
	while (res == ST_ERR_SUCC &&
			(pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.id != ST_APEV2_FID_IPLS ||
				st_mtes_strArrGetElemCount(&pItFldI->dataSArr) != 1)
			continue;
		res = st_tagCFnc_iplsSplit(&pItFldI->dataSArr);
		if (res != ST_ERR_SUCC)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"splitting up IPLS string failed");
	}

	/* merge fields */
	pItFld = NULL;
	while (res == ST_ERR_SUCC &&
			(pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.typ != ST_APEV2_FTP_STR)
			continue;
		res = st_apev2_tidy_mergFields(cFNCN, pTag, pItFld);
		if (res != ST_ERR_SUCC)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"merging fields failed");
	}

	/* merge multiple-strings */
	pItFld = NULL;
	while (res == ST_ERR_SUCC &&
			(pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.typ == ST_APEV2_FTP_BDAT ||
				st_mtes_strArrGetElemCount(&pItFldI->dataSArr) < 2)
			continue;
		if (pItFldI->fldPrI.id == ST_APEV2_FID_IPLS) {
			/* convert IPLS array into single string */
			res = st_tagCFnc_iplsMerge(&pItFldI->dataSArr);
		} else
			res = st_apev2_tidy_mergMultStr(cFNCN, &pTagI->opts, pItFld);
		if (res != ST_ERR_SUCC)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"merging multi-strings failed");
	}

	/* re-check all fields */
	if (res == ST_ERR_SUCC)
		res = st_apev2_tidy_chkAllFields(cFNCN, pTag);

	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"everything very tidy now");

	/* update hasFields */
	if (res == ST_ERR_SUCC)
		st_tagBas_gs_setHasFields(&pTagI->tbas,
				st_apev2_gs_getTag_amountValidFlds(pTag) > 0);

	return res;
}

/*----------------------------------------------------------------------------*/
/*
 * Write Tag to stream
 *   An APEv2 Tag should be at the end of a file but before any ID3v1 Tag
 *
 * pTag            MUST be set
 * pTagI->pStrwr   MUST be set
 *        tbas     MUST be set
 *        pFilen   MUST be set
 */
Tst_err
st_apev2_writeTagToStream(Tst_apev2_tag *pTag)
{
	const char *cFNCN = __func__;
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 lastTSz;
	Tst_binobj             tagBO;
	Tst_tagBasics          tbasNew;
	Tst_apev2_fldData      *pItFld  = NULL;
	Tst_apev2_fldData_intn *pItFldI = NULL;
	Tst_apev2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pStrwr == NULL)

	if (! st_tagBas_gs_getHasFields(&pTagI->tbas)) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"won't write empty tag");
		return ST_ERR_ABRT;
	}

	st_tagBas_stc_initTB(&tbasNew);

	lastTSz = (st_tagBas_gs_getHasTag(&pTagI->tbas) ?
			st_tagBas_gs_getSize(&pTagI->tbas) : 0);
	while ((pItFld = st_apev2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		pItFldI->wasRendered = ST_B_FALSE;
	}

	/* tidy up tag fields (remove duplicates, ...) */
	res = st_apev2_tidyUpTag(pTag);
	if (res != ST_ERR_SUCC) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"tidying up tag failed");
		st_tagBas_stc_freeTB(&tbasNew);
		return res;
	}
	if (! st_tagBas_gs_getHasFields(&pTagI->tbas)) {
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"tag is empty after having tidied up, won't write empty tag");
		st_tagBas_stc_freeTB(&tbasNew);
		return ST_ERR_ABRT;
	}

	/* init output binary object */
	st_binobj_stc_initBO(&tagBO);

	/* render tag (write everything to binary object) */
	res = st_apev2_wr_rendTagToBO(pTag, &tbasNew, &tagBO);
	if (res != ST_ERR_SUCC) {
		if (res != ST_ERR_ABRT)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"rendering tag failed");
		st_tagBas_stc_freeTB(&tbasNew);
		st_binobj_stc_freeBO(&tagBO);
		return res;
	}
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		Tst_uint32 newTSz = st_tagBas_gs_getSize(&tbasNew);

		if (newTSz < 1024)
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"tag size=%u bytes (d %+d bytes)",
					newTSz, (Tst_int32)newTSz - (Tst_int32)lastTSz);
		else if (newTSz < 1024 * 1024)
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"tag size=%u bytes (%.2f KB, d %+d bytes)",
					newTSz, (double)newTSz / 1024.0,
					(Tst_int32)newTSz - (Tst_int32)lastTSz);
		else
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"tag size=%u bytes (%u MB, d %+d bytes)",
					newTSz,
					(Tst_uint32)((double)newTSz / (1024.0 * 1024.0)),
					(Tst_int32)newTSz - (Tst_int32)lastTSz);
	}

	/* write tag to stream */
	res = st_tagCFnc_writeTagBOtoStream(&pTagI->opts.basOpts, &pTagI->opts,
			cFNCN, st_apev2_d_debCB, st_apev2_d_errCB,
			pTagI->pFilen, &pTagI->tbas,
			pTagI->pStrwr, &tagBO);
	st_tagBas_stc_freeTB(&tbasNew);
	st_binobj_stc_freeBO(&tagBO);
	if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"writing tag failed");

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Disable field(s)
 */
static Tst_err
ST_TAV2__disableFld(const char *pFnc, Tst_apev2_tag *pTag,
		const Tst_bool useIDorFTP,
		const Tst_apev2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr,
		const Tst_apev2_frTp frTp, const Tst_bool beSilent)
{
	Tst_str const *pIntFrIDstr = pFrIDstr;
	Tst_apev2_fldData      *pItFld = NULL;
	Tst_apev2_fldData_intn *pItFldI;
	Tst_apev2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	if (pFrIDstr == NULL && frID == ST_APEV2_FID_CBIN)
		pIntFrIDstr = (Tst_str const*)"DUMMYGEOB";

	while ((pItFld = st_apev2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_apev2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (frNr != 0 && pItFldI->fnr != frNr)
			continue;
		if (useIDorFTP) {
			if (! st_apev2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pVIDstr,
						frID, (const char*)pIntFrIDstr))
				continue;
		} else if (pItFldI->fldPrI.typ != frTp)
			continue;
		if (! beSilent &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_fdeb(&pTagI->opts, 0, pFnc, pItFldI,
					"disabling field...");
		st_apev2_stc_rsetFDat_onlyBufs(pItFld);
		pItFldI->isOK = ST_B_FALSE;
		if (frNr != 0)
			break;
	}
	st_tagBas_gs_setHasFields(&pTagI->tbas,
			st_apev2_gs_getTag_amountValidFlds(pTag) > 0);
	return ST_ERR_SUCC;
}

/******************************************************************************/
