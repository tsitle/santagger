/******************************************************************************/
/* tag_id3v1.c                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for ID3v1 (Extended) Tags                                        */
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
#include "src/includes/common/streamrd.h"
#include "src/includes/common/streamwr.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_id3v1.h"
/** */
#define SRC_TAG_ID3V1_C
#include "tag_id3v1-prot.h"
/*** */
#include "tag_id3v1-pp_flds.h"
#include "tag_id3v1_fnc-prot.h"
#include "tag_id3v1_zrd-prot.h"
#include "tag_id3v1_zwr-prot.h"
#include "tag_id3v1_stcs-prot.h"
#include "tag_id3v1_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V1_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__concatStrFlds(Tst_id3v1_tag_intn *pTagI, const Tst_id3v1ext_frID idExt);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read ID3v1 Tag from stream
 *
 * pTagI->pStrrd   MUST be set
 *        pFilen   MUST be set
 *
 * returns: ERRCODE
 *          pTag  will be set
 */
Tst_err
st_id3v1_readTagFromStream(Tst_id3v1_tag *pTag)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err     res = ST_ERR_SUCC;
	Tst_buf     buf[ST_ID3V1_TAG_EXT_SZ + ST_ID3V1_TAG_SZ],
	            *pBuf;
	Tst_uint32  rd  = 0,
	            toRd;
	Tst_id3v1_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	st_id3v1_stc_rsetTag_prot(pTag, ST_B_FALSE,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL);
	pTagI = (Tst_id3v1_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pStrrd == NULL || pTagI->pFilen == NULL)

	/* check whether next tag is IV1, IV1E or even APE */
	toRd = ST_ID3V1_TAG_EXT_SZ + ST_ID3V1_TAG_SZ;
	res  = st_streamrd_rdAheadBuffer(pTagI->pStrrd, toRd, buf, &rd);
	if (res != ST_ERR_SUCC || rd < ST_ID3V1_TAG_SZ) {
		if (res == ST_ERR_FEOF)
			res = ST_ERR_SUCC;  /* everything's fine */
		else if (res != ST_ERR_SUCC)
			st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}
	pBuf = buf;
	if (memcmp(buf, "TAGEX", 5) == 0) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
					"ID3v1 tag might be APEv2 tag instead...");
	} else if (rd == toRd &&
			memcmp(buf, ST_ID3V1_TAG_HEAD_EXT, ST_ID3V1_TAG_HEAD_EXT_SLEN) == 0 &&
			memcmp(&buf[ST_ID3V1_TAG_EXT_SZ],
						ST_ID3V1_TAG_HEAD, ST_ID3V1_TAG_HEAD_SLEN) == 0) {
		/* read extended ID3v1 tag */
		res = st_id3v1_rd_readV1Ext(pTagI->opts.rdOnlyBasics, pTagI, buf);
		if (res != ST_ERR_SUCC) {
			st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"reading ID3v1 Extended tag failed");
			return res;
		}
		pBuf = &buf[ST_ID3V1_TAG_EXT_SZ];
	}
	if (memcmp(pBuf, ST_ID3V1_TAG_HEAD, ST_ID3V1_TAG_HEAD_SLEN) == 0) {
		/* read ID3v1 tag */
		res = st_id3v1_rd_readV1(pTagI->opts.rdOnlyBasics, pTagI, pBuf);
		if (res != ST_ERR_SUCC) {
			st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"reading ID3v1 tag failed");
			return res;
		}
	} else {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
					"couldn't find ID3v1 tag");
	}

	/* */
	if (st_tagBas_gs_getHasTag(&pTagI->tbas) &&
			pTagI->hasExtTag && ! pTagI->opts.rdOnlyBasics) {
		/* append field values of extended tag to normal tag */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_deb(&pTagI->opts, 0, cFNCN,
					"appending ID3v1 Extended tag to normal tag");
		res = ST_TIV1__concatStrFlds(pTagI, ST_ID3V1E_FID_EARTI);
		if (res == ST_ERR_SUCC)
			res = ST_TIV1__concatStrFlds(pTagI, ST_ID3V1E_FID_EALBU);
		if (res == ST_ERR_SUCC)
			res = ST_TIV1__concatStrFlds(pTagI, ST_ID3V1E_FID_ETITL);
		if (res != ST_ERR_SUCC)
			st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"appending extended fields failed");
	}

	/* tidy up */
	if (res == ST_ERR_SUCC)
		res = st_id3v1_tidyUpTag(pTag);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Add pTagSrc to pTagDst
 *   Since ID3v1 is a very limited tag, we only copy fields from pTagSrc
 *   to pTagDst, if the field isn't already set in pTagDst
 */
Tst_err
st_id3v1_mergeTags(Tst_id3v1_tag *pTagDst, Tst_id3v1_tag *pTagSrc)
{
	Tst_err   res;
	Tst_int32 i32;
	Tst_mtes_string tmps;

	ST_ASSERTN_IARG(pTagDst == NULL || pTagDst->pObInternal == NULL ||
			pTagSrc == NULL || pTagSrc->pObInternal == NULL)

	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;

	/* artist */
	res = st_id3v1_gs_getField_artist(pTagSrc, &tmps);
	if (res == ST_ERR_SUCC && st_mtes_strlenBy(&tmps) > 0)
		res = st_id3v1_gs_setField_artist(pTagDst, &tmps);
	/* album */
	res = st_id3v1_gs_getField_album(pTagSrc, &tmps);
	if (res == ST_ERR_SUCC && st_mtes_strlenBy(&tmps) > 0)
		res = st_id3v1_gs_setField_album(pTagDst, &tmps);
	/* song */
	res = st_id3v1_gs_getField_title(pTagSrc, &tmps);
	if (res == ST_ERR_SUCC && st_mtes_strlenBy(&tmps) > 0)
		res = st_id3v1_gs_setField_title(pTagDst, &tmps);
	/* comment */
	res = st_id3v1_gs_getField_comment(pTagSrc, &tmps);
	if (res == ST_ERR_SUCC && st_mtes_strlenBy(&tmps) > 0)
		res = st_id3v1_gs_setField_comment(pTagDst, &tmps);

	st_mtes_stc_freeStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;

	/* track nr. */
	i32 = st_id3v1_gs_getField_tracknr(pTagSrc);
	if (i32 >= ST_ID3_TFLD_TRACK_MIN_IV1 &&
			i32 <= ST_ID3_TFLD_TRACK_MAX_IV1)
		res = st_id3v1_gs_setField_tracknr(pTagDst, i32);
	/* year */
	i32 = st_id3v1_gs_getField_year(pTagSrc);
	if (res == ST_ERR_SUCC &&
			i32 >= ST_ID3_TFLD_YEAR_MIN &&
			i32 <= ST_ID3_TFLD_YEAR_MAX)
		res = st_id3v1_gs_setField_year(pTagDst, i32);
	/* genre */
	i32 = st_id3v1_gs_getField_genre(pTagSrc);
	if (res == ST_ERR_SUCC &&
			i32 >= ST_ID3_TFLD_GENRE_MIN_IV1 &&
			i32 <= ST_ID3_TFLD_GENRE_MAX_IV1)
		res = st_id3v1_gs_setField_genre(pTagDst, i32);

	/* tidy up */
	if (res == ST_ERR_SUCC)
		res = st_id3v1_tidyUpTag(pTagDst);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Tidy up the tag
 */
Tst_err
st_id3v1_tidyUpTag(Tst_id3v1_tag *pTag)
{
	Tst_id3v1_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v1_tag_intn*)pTag->pObInternal;

	st_id3v1_fnc_updAreFldsSet(pTagI);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Write ID3v1 Tag to stream
 *   An ID3v1 Tag belongs at the very end of a file.
 *   ID3v1 Extended fields won't be written because the Extended Tag
 *   is highly inofficial and not supported by most software.
 *
 * pTagI->pStrwr   MUST be set
 *        pFilen   MUST be set
 *        tbas     MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_id3v1_writeTagToStream(Tst_id3v1_tag *pTag)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res = ST_ERR_SUCC;
	Tst_binobj         tagBO;
	Tst_id3v1_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v1_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pStrwr == NULL)

	/* tidy up */
	res = st_id3v1_tidyUpTag(pTag);
	if (res != ST_ERR_SUCC)
		return res;

	if (! st_tagBas_gs_getHasFields(&pTagI->tbas)) {
		st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"won't write empty tag");
		return ST_ERR_ABRT;
	}

	/* init output binary object */
	st_binobj_stc_initBO(&tagBO);

	/* render Tag to buffer */
	res = st_id3v1_wr_rendTagToBO(pTagI, &tagBO);
	if (res != ST_ERR_SUCC) {
		st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"rendering tag failed");
		st_binobj_stc_freeBO(&tagBO);
		return res;
	}

	/* write buffer to stream */
	res = st_tagCFnc_writeTagBOtoStream(&pTagI->opts.basOpts, &pTagI->opts,
			cFNCN, st_id3v1_d_debCB, st_id3v1_d_errCB,
			pTagI->pFilen, &pTagI->tbas,
			pTagI->pStrwr, &tagBO);
	if (res != ST_ERR_SUCC)
		st_id3v1_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"writing tag to stream failed");

	st_binobj_stc_freeBO(&tagBO);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__concatStrFlds(Tst_id3v1_tag_intn *pTagI, const Tst_id3v1ext_frID idExt)
{
	Tst_err res;
	Tst_id3v1_fldData_intn *pFldIsrc = NULL,
	                       *pFldIdst = NULL;

	ST_ASSERTN_IARG(pTagI == NULL)

	switch (idExt) {
	case ST_ID3V1E_FID_EARTI:
		pFldIsrc = (Tst_id3v1_fldData_intn*)pTagI->fdExtArt.pObInternal;
		pFldIdst = (Tst_id3v1_fldData_intn*)pTagI->fdArt.pObInternal;
		break;
	case ST_ID3V1E_FID_ETITL:
		pFldIsrc = (Tst_id3v1_fldData_intn*)pTagI->fdExtTit.pObInternal;
		pFldIdst = (Tst_id3v1_fldData_intn*)pTagI->fdTit.pObInternal;
		break;
	case ST_ID3V1E_FID_EALBU:
		pFldIsrc = (Tst_id3v1_fldData_intn*)pTagI->fdExtAlb.pObInternal;
		pFldIdst = (Tst_id3v1_fldData_intn*)pTagI->fdAlb.pObInternal;
		break;
	default:
		ST_ASSERTN_IARG(idExt != ST_ID3V1E_FID_EARTI &&
				idExt != ST_ID3V1E_FID_ETITL && idExt != ST_ID3V1E_FID_EALBU)
		return ST_ERR_IARG;
	}
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldIsrc == NULL || pFldIdst == NULL)

	/* concatenate the strings */
	res = st_mtes_strConcat(&pFldIsrc->dataStr, &pFldIdst->dataStr);

	/* strip string */
	if (res == ST_ERR_SUCC)
		res = st_id3v1_fnc_stripStr(&pFldIdst->dataStr, NULL, NULL, NULL);

	/* */
	if (res == ST_ERR_SUCC)
		pFldIdst->isOK = ! st_mtes_isStrEmptyCh(&pFldIdst->dataStr);

	return res;
}

/******************************************************************************/
