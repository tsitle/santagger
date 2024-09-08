/******************************************************************************/
/* tag_id3v2.c                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for ID3v2 Tag handling                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj.h>
#include <santag/common/string_mte.h>
#include <santag/common/streamrd.h>
#include <santag/common/sys_file.h>
#include <santag/common/sys_fnc.h>
#include <santag/utils/utils_fmt.h>
#include <santag/tag/tag_comfnc.h>
#include <santag/tag/tag_id3v2.h>
/** */
#define SRC_TAG_ID3V2_C
#include "tag_id3v2-prot.h"
/*** */
#include "../tag_basics-prot.h"
#include "tag_id3v2-priv_flds.h"
#include "tag_id3v2_gs-prot.h"
#include "tag_id3v2_stcs-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#include "tag_id3v2_zrd-pp.h"
#include "tag_id3v2_zwr-pp.h"
#include "tag_id3v2_zchk-pp.h"
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2_ztidy-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__disableFld(const char *pFnc, Tst_id3v2_tag *pTag,
                    Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
                    const Tst_uint32 frNr, const Tst_bool beSilent);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Reads ID3v2 Tag
 *
 * pTag            MUST be set
 * pTagI->pStrrd   MUST be set
 * pTagI->pFilen   MUST be set
 *
 * returns: ERRCODE
 *          pTag will be updated
 */
Tst_err
st_id3v2_readTagFromStream(Tst_id3v2_tag *pTag)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res       = ST_ERR_SUCC;
	Tst_bool   isOK      = ST_B_FALSE;
	Tst_uint32 ehdNfraSz = 0,
	           totRd     = 0;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	st_id3v2_stc_rsetTag_prot(pTag, ST_B_FALSE,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL);
	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pStrrd == NULL || pTagI->pFilen == NULL)

	/* read ID3v2 Header */
	res = st_id3v2_rd_readHdOrFoot(pTag, ST_B_FALSE,
			&isOK, &ehdNfraSz, &totRd);
	if (res != ST_ERR_SUCC || ! isOK) {
		if (res != ST_ERR_SUCC)
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"reading header failed");
		return res;
	}

	/* is tag size (without ID and header) big enough ? */
	if (ehdNfraSz < 1) {
		if (! pTagI->tgErrs.thIsSizeInv) {
			pTagI->tgErrs.thIsSizeInv = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"empty ID3v2 tag");
		isOK = ST_B_FALSE;
	}

	/* do we have a unknown flag ? */
	/*if (isOK && flUnkn) {
		if (! pTagI->tgErrs.hasUnknFl) {
			pTagI->tgErrs.hasUnknFl = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"sorry, but flag in 0x%02x is unknown", flagByte);
		isOK = ST_B_FALSE;
	}*/

	/* is tag compressed ? */
	if (isOK && pTagI->verMaj == 2 && pTagI->flagsTHd.isCompr) {
		if (! pTagI->tgErrs.thIsEntTagCompr) {
			pTagI->tgErrs.thIsEntTagCompr = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"sorry, compression method unknown");
		isOK = ST_B_FALSE;
	}

	if (! isOK || pTagI->opts.rdOnlyBasics) {
		ST_ASSERTN_NUM(ST_ERR_FAIL, st_tagBas_gs_getSize(&pTagI->tbas) < totRd)
		/* skip remaining tag incl. Footer */
		st_streamrd_rdSkipBytes(pTagI->pStrrd,
				st_tagBas_gs_getSize(&pTagI->tbas) - totRd, ST_B_TRUE);
		/* update/correct total Tag size */
		st_id3v2_rd_updateTagSize(pTagI, cFNCN, ST_B_TRUE, 0);
		return ST_ERR_SUCC;
	}

	/* read Extended Header and all ID3 frames */
	res = st_id3v2_rd_readEHdAndFlds(pTag, totRd);
	if (res != ST_ERR_SUCC) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"reading ext. header and tag fields failed");
		return res;
	}

	/* get tagger info */
	if (! pTagI->opts.rdOnlyBasics) {
		Tst_mtes_strArr tmpSArr;

		st_mtes_stc_initSA(&tmpSArr);
		res = st_id3v2_fldGet_dataStrArr(pTag, ST_ID3V2_FID_234_CTXT,
				(const Tst_str*)(pTagI->verMaj == 2 ? "TGA" : "TGGA"),
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
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* */
	st_tagBas_gs_setHasFields(&pTagI->tbas,
			st_id3v2_gs_getTag_amountValidFlds_all(pTag) > 0);
	if (! st_tagBas_gs_getHasFields(&pTagI->tbas)) {
		st_id3v2_fnc_updateFldErrorCnt(pTag);
	} else {
		/* tidy up tag fields (map fields, remove duplicates, ...) */
		res = st_id3v2_tidyUpTag(pTag, ST_B_FALSE);
		if (res != ST_ERR_SUCC) {
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"tidying up tag failed");
			return res;
		}
	}
	/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) && pTagI->flds.errCnt > 0)
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"%u errors/warnings in fields", pTagI->flds.errCnt);**/
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
			! st_tagBas_gs_getHasFields(&pTagI->tbas))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"tag is empty");

	/* read ID3v2 Footer */
	res = st_id3v2_rd_readHdOrFoot(pTag, ST_B_TRUE, &isOK, &ehdNfraSz, &totRd);
	if (res != ST_ERR_SUCC)
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"reading footer failed");

	if (res == ST_ERR_SUCC) {
		/* update/correct total Tag size */
		st_id3v2_rd_updateTagSize(pTagI, cFNCN, ST_B_TRUE, 0);
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Add pTagSrc to pTagDst
 *   This means that all fields from pTagSrc are appended
 *   to pTagDst, if possible
 */
Tst_err
st_id3v2_mergeTags(Tst_id3v2_tag *pTagDst, Tst_id3v2_tag *pTagSrc,
		const Tst_bool compareFlds)
{
	Tst_err  res = ST_ERR_SUCC;
	Tst_bool hasEquFld;
	Tst_id3v2_fldData const *pItFld = NULL;

	ST_ASSERTN_IARG(pTagDst == NULL || pTagSrc == NULL)

	while ((pItFld = st_id3v2_ite_nextValidFld(pTagSrc, pItFld)) != NULL) {
		if (compareFlds) {
			hasEquFld = ST_B_FALSE;
			res       = st_id3v2_fldAdd_doesSimiliarFieldExist(pTagDst,
					ST_B_TRUE, pItFld, &hasEquFld);
			if (res != ST_ERR_SUCC)
				break;
			if (hasEquFld)
				continue;
		}
		/* */
		res = st_id3v2_fldAdd(pTagDst, pItFld);
		if (res != ST_ERR_SUCC)
			break;
	}
	/* tidy up tag fields (map fields, remove duplicates, ...) */
	if (res == ST_ERR_SUCC)
		res = st_id3v2_tidyUpTag(pTagDst, ST_B_FALSE);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Disable all fields with the ID frID
 */
Tst_err
st_id3v2_disableFld_byID_all(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
#	define LOC_FNCN_  __FUNCTION__

	return ST_TIV2__disableFld(LOC_FNCN_, pTag, frID, pFrIDstr,
			/*frNr:*/0, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*
 * Disable field with the ID frID and the field number frNr
 */
Tst_err
st_id3v2_disableFld_byID(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr)
{
#	define LOC_FNCN_  __FUNCTION__

	if (frNr == 0)
		return ST_ERR_IARG;
	return ST_TIV2__disableFld(LOC_FNCN_, pTag, frID, pFrIDstr,
			frNr, /*silent:*/ST_B_FALSE);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Tidy up Tag, i.e. map fields, remove duplicate fields, ...
 *
 * pTag   MUST be set
 *
 * returns: ERRCODE
 *          pTag will be updated
 */
Tst_err
st_id3v2_tidyUpTag(Tst_id3v2_tag *pTag, const Tst_bool discardFields)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err        res = ST_ERR_SUCC;
	Tst_id3v2_frID srcID2,
	               srcID3,
	               destID1,
	               destID2,
	               destID3;
	Tst_bool       mapp,
	               discard,
	               made1uni,
	               made1uniF;
	Tst_id3v2_tag_intn     *pTagI;
	Tst_id3v2_fldData      *pItFld  = NULL,
	                       *pFld2,
	                       *pFld3;
	Tst_id3v2_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"starting to tidy up for ID3v2.%u.%u tag...",
				pTagI->verMaj, pTagI->verMin);

	/* remove Tagger field */
	res = ST_TIV2__disableFld(cFNCN, pTag, ST_ID3V2_FID_234_CTXT,
			(const Tst_str*)(pTagI->verMaj == 2 ? "TGA" : "TGGA"),
			/*frNr:*/0, /*silent:*/ST_B_TRUE);
	if (res != ST_ERR_SUCC)
		return res;

	/* check fields data once again (I like checking data) */
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		res = st_id3v2_chk_checkFld(&pTagI->opts, ST_B_FALSE, pItFld);
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* map or discard fields if necessary */
	pItFld = NULL;
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		srcID2  = ST_ID3V2_FID_NONE;
		srcID3  = ST_ID3V2_FID_NONE;
	    destID1 = ST_ID3V2_FID_NONE;
	    destID2 = ST_ID3V2_FID_NONE;
	    destID3 = ST_ID3V2_FID_NONE;
		pFld2   = NULL;
		pFld3   = NULL;
	    mapp    = ST_B_FALSE;
	    discard = ST_B_FALSE;
		if (st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS_IGN,
					pItFldI->fldPrI.id, NULL)) {
		    discard = ST_B_TRUE;
	    } else if (pItFldI->fldPrI.id != ST_ID3V2_FID_234_CTXT &&
	    		pItFldI->fldPrI.id != ST_ID3V2_FID_234_CURL)
			st_id3v2_tidy_getFldMap(pItFldI->fldPrI.id, &srcID2, &srcID3,
					pTagI->verMaj,
					&mapp, &discard, &destID1, &destID2, &destID3);
		else if (pItFldI->fldPrI.pIDstr == NULL ||
				pItFldI->fldPrI.pIDstr[0] == ' ' ||
				pItFldI->fldPrI.pIDstr[0] == '-' ||
				st_sysStrlen2(pItFldI->fldPrI.pIDstr) !=
						(pTagI->verMaj == 2 ? 3 : 4)) {
		    discard = ST_B_TRUE;
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
				if (! discardFields)
					st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pItFldI,
							"would be discarded (custom fields cannot be mapped)");
				else
					st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pItFldI,
							"will discard field (custom fields cannot be mapped)");
			}
		}
		/* discard field */
		if (discard) {
			if (! discardFields) {
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
					st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pItFldI,
							"should be discarded (worthless data)");
			} else {
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
					st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pItFldI,
							"discarding field (worthless data)");
				res = st_id3v2_disableFld_byID(pTag,
						pItFldI->fldPrI.id, (const Tst_str*)pItFldI->fldPrI.pIDstr,
						pItFldI->fnr);
				if (res != ST_ERR_SUCC)
					return res;
			}
			continue;
		}
		/* map field, from here on there are no custom fields */
		if (! mapp)
			continue;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI->fldPrI.id == ST_ID3V2_FID_234_CTXT ||
	    		pItFldI->fldPrI.id == ST_ID3V2_FID_234_CURL)
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pItFldI,
					"mapping field...");
		if (srcID2 != ST_ID3V2_FID_NONE) {
			pFld2 = st_id3v2_fnc_getFirstValidFld_pt_byID(pTag, srcID2, NULL);
			if (srcID3 != ST_ID3V2_FID_NONE)
				pFld3 = st_id3v2_fnc_getFirstValidFld_pt_byID(pTag, srcID3, NULL);
		}
		mapp = ST_B_FALSE;
		res  = st_id3v2_tidy_mapField(pTag, pItFld, pFld2, pFld3,
				destID1, destID2, destID3, &mapp);
		if (res != ST_ERR_SUCC)
			return res;
		if (mapp && ! pItFldI->errs.frWasMappd) {
			pItFldI->errs.frWasMappd = ST_B_TRUE;
			++pItFldI->frErrCnt;
		}
	}

	/* merge fields */
	pItFld = NULL;
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		switch (pItFldI->fldPrI.typ) {
		case ST_ID3V2_FTP_TXT:
		case ST_ID3V2_FTP_URL:
		case ST_ID3V2_FTP_IPL:
			res = st_id3v2_tidy_mergFields(cFNCN, pTag, pItFld);
			if (res != ST_ERR_SUCC)
				return res;
			break;
		default:
			break;
		}
	}

	/* discard duplicate fields or make them unique */
	do {
		pItFld   = NULL;
		made1uni = ST_B_FALSE;
		while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
			made1uniF = ST_B_FALSE;
			/* */
			res = st_id3v2_tidy_remDups(cFNCN, discardFields, pTag,
					pItFld, &made1uniF);
			if (res != ST_ERR_SUCC)
				return res;
			made1uni = made1uni || made1uniF;
		}
	} while (made1uni);

	/* merge multiple-strings */
	pItFld = NULL;
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (st_mtes_strArrGetElemCount(&pItFldI->dataSArr) < 2)
			continue;
		if (pItFldI->fldPrI.typ == ST_ID3V2_FTP_TXT) {
			if (pItFldI->fldPrI.id == ST_ID3V2_FID___4_TIPL ||
					pItFldI->fldPrI.id == ST_ID3V2_FID___4_TMCL)
				continue;
			if (pTagI->verMaj >= 4 &&
					(pItFldI->fldPrI.id == ST_ID3V2_FID_234_TLAN ||
					pItFldI->fldPrI.id == ST_ID3V2_FID_234_TCON))
				continue;
		} else if (pItFldI->fldPrI.typ == ST_ID3V2_FTP_SLT ||
				pItFldI->fldPrI.typ == ST_ID3V2_FTP_IPL)
			continue;
		/* ID3v2.4 supports multi-strings in all text-info frames,
		 *   but we merge them here anyway  */
		res = st_id3v2_tidy_mergMultStr(cFNCN, &pTagI->opts, pItFld);
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* if a IPLS field contains only a single string then
	 *   split it up into an string array  */
	pItFld = NULL;
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if (pItFldI->fldPrI.typ != ST_ID3V2_FTP_IPL ||
				st_mtes_strArrGetElemCount(&pItFldI->dataSArr) != 1)
			continue;
		res = st_tagCFnc_iplsSplit(&pItFldI->dataSArr);
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* sort fields */
	res = st_id3v2_fnc_sortFields_byPrio(pTag, ST_B_TRUE);
	if (res != ST_ERR_SUCC)
		return res;

	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"%s tidy now",
				(discardFields ? "everything very" : "more or less"));

	/* update hasFields */
	st_tagBas_gs_setHasFields(&pTagI->tbas,
			st_id3v2_gs_getTag_amountValidFlds_all(pTag) > 0);
	/* update error counter */
	st_id3v2_fnc_updateFldErrorCnt(pTag);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Write Tag to stream
 *   An ID3v2 Tag should be at the very beginning of a file,
 *   but there may be multiple ID3v2 Tags in one file
 *   and they may also appear directly after the audio data.
 *
 * pTag            MUST be set
 * pTagI->pStrwr   MUST be set
 *        tbas     MUST be set
 *        pFilen   MUST be set
 */
Tst_err
st_id3v2_writeTagToStream(Tst_id3v2_tag *pTag)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 lastTSz;
	Tst_binobj             *pTagBO  = NULL;
	Tst_tagBasics          tbasNew;
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pStrwr == NULL)
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	if (! st_tagBas_gs_getHasFields(&pTagI->tbas)) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"won't write empty tag");
		return ST_ERR_ABRT;
	}

	st_tagBas_stc_initTB(&tbasNew);

	lastTSz = (st_tagBas_gs_getHasTag(&pTagI->tbas) ?
			st_tagBas_gs_getSize(&pTagI->tbas) : 0);
	while ((pItFld = st_id3v2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		pItFldI->wasRendered = ST_B_FALSE;
	}

	/* tidy up tag fields (map fields, remove duplicates, ...) */
	res = st_id3v2_tidyUpTag(pTag, ST_B_TRUE);
	if (res != ST_ERR_SUCC) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"tidying up tag failed");
		st_tagBas_stc_freeTB(&tbasNew);
		return res;
	}
	if (! st_tagBas_gs_getHasFields(&pTagI->tbas)) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"tag is empty after having tidied up, won't write empty tag");
		st_tagBas_stc_freeTB(&tbasNew);
		return ST_ERR_ABRT;
	}

	/* init output binary object */
	ST_CALLOC(pTagBO, Tst_binobj*, 1, sizeof(Tst_binobj))
	if (pTagBO == NULL)
		return ST_ERR_OMEM;
	st_binobj_stc_initBO(pTagBO);

	/* render tag (write everything to binary object) */
	res = st_id3v2_wr_rendTagToBO(pTag, &tbasNew, &pTagBO);
	if (res != ST_ERR_SUCC) {
		if (res != ST_ERR_ABRT)
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"rendering tag failed");
		st_tagBas_stc_freeTB(&tbasNew);
		st_binobj_stc_freeBO(pTagBO);
		ST_DELPOINT(pTagBO)
		return res;
	}
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		Tst_uint32 newTSz = st_tagBas_gs_getSize(&tbasNew);

		if (newTSz < 1024)
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"tag size=%u bytes (d %+d bytes)",
					newTSz, (Tst_int32)newTSz - (Tst_int32)lastTSz);
		else if (newTSz < 1024 * 1024)
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"tag size=%u bytes (%.2f KB, d %+d bytes)",
					newTSz, (double)newTSz / 1024.0,
					(Tst_int32)newTSz - (Tst_int32)lastTSz);
		else
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"tag size=%u bytes (%u MB, d %+d bytes)",
					newTSz,
					(Tst_uint32)((double)newTSz / (1024.0 * 1024.0)),
					(Tst_int32)newTSz - (Tst_int32)lastTSz);
	}

	/* write tag to stream */
	res = st_tagCFnc_writeTagBOtoStream(&pTagI->opts.basOpts, &pTagI->opts,
			cFNCN, st_id3v2_d_debCB, st_id3v2_d_errCB,
			pTagI->pFilen, &pTagI->tbas,
			pTagI->pStrwr, pTagBO);
	st_tagBas_stc_freeTB(&tbasNew);
	st_binobj_stc_freeBO(pTagBO);
	ST_DELPOINT(pTagBO)
	if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"writing tag failed");

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Disable field(s)
 */
static Tst_err
ST_TIV2__disableFld(const char *pFnc, Tst_id3v2_tag *pTag,
		Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		const Tst_uint32 frNr, const Tst_bool beSilent)
{
	Tst_id3v2_fldData      *pItFld = NULL;
	Tst_id3v2_fldData_intn *pItFldI;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	if (pTagI->verMaj < 2 || pTagI->verMaj > 4)
		return ST_ERR_IARG;

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		if ((frNr != 0 && pItFldI->fnr != frNr) ||
				! st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
							frID, (const char*)pFrIDstr))
			continue;
		if (! beSilent &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, pFnc, pItFldI,
					"disabling field...");
		st_id3v2_stc_rsetFDat_onlyBufs(pItFld);
		pItFldI->isOK = ST_B_FALSE;
		if (frNr != 0)
			break;
	}
	st_tagBas_gs_setHasFields(&pTagI->tbas,
			st_id3v2_gs_getTag_amountValidFlds_all(pTag) > 0);
	return ST_ERR_SUCC;
}

/******************************************************************************/
