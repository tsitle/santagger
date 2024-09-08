/******************************************************************************/
/* mf_show.c                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for displaying Tags and Audio/Video info                         */
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
// Own-Includes, Part I
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/audvid/aud_mpeg1.h>
#include <santag/audvid/cont_ogg.h>
#include <santag/tag/tag_basics.h>
#include <santag/tag/tag_id3v2-defs.h>
#include <santag/tag/tag_meta.h>
/** */
#define SRC_MF_SHOW_C
#include "mf_show-pp.h"
/*** */
#include "mf_show_av-pp.h"
#include "mf_show_tiv1-pp.h"
#include "mf_show_tiv2-pp.h"
#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#include "../cln/cln_stcs-prot.h"
#undef SRC_MF_SHOW_C

/*
// Own-Includes, Part II
*/
#if (AST_MF_SW_DEBUG_IV1 == 1)
#	include <santag/tag/tag_id3v1-defs.h>
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Print Tag and Audio info to stdout
 *
 * pCmdln  MUST be set
 * pMF     MUST be set
 */
Tst_err
ast_mf_sw_showFileInfo(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL)
	if (! pCmdln->cmds.rd || pMF->contTypes == (Tst_int32)AST_MF_CTP_NONE)
		return ST_ERR_SUCC;

	/* output audio/video/... info */
	if (pMF->contTypes != (Tst_int32)AST_MF_CTP_NONE &&
			pMF->contTypes != (Tst_int32)AST_MF_CTP_TAG) {
		if (pMF->ffmt == AST_MF_FFMT_MPG1 || pMF->ffmt == AST_MF_FFMT_NONE) {
			/* output MPEG-1 stream Info */
			if (st_mpeg1_gs_getStr_hasMPEGstream(&pMF->audMpg1) &&
					(pCmdln->cmds.rd ||
						ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_AUD))) {
				if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
					ast_mf_op_d_deb(cFNCN, "show MPEG-1 info...");
				res = ast_mf_sw_showMPEG1stream(pCmdln,
						pCmdln->cmds.rd, &pMF->audMpg1);
				if (res != ST_ERR_SUCC) {
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"showing MPEG-1 stream info failed");
					return res;
				}
			} else if (! st_mpeg1_gs_getStr_hasMPEGstream(&pMF->audMpg1))
				ast_mf_op_prFInf("AUD mpg (no MPEG-1 stream found)\n");
		}
		if (pMF->ffmt == AST_MF_FFMT_OGG || pMF->ffmt == AST_MF_FFMT_FLAC ||
				pMF->ffmt == AST_MF_FFMT_NONE) {
			/* output OGG/FLAC streams' Info */
			if ((st_contOgg_gs_getHasOggStream(&pMF->avOgg) ||
						st_contOgg_gs_getHasFlacStream(&pMF->avOgg)) &&
					(pCmdln->cmds.rd ||
						ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_AUD))) {
				if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
					ast_mf_op_d_deb(cFNCN, "show OGG/FLAC info...");
				res = ast_mf_sw_showOGGstreams(pCmdln, &pMF->avOgg);
				if (res != ST_ERR_SUCC) {
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"showing Ogg stream info failed");
					return res;
				}
			} else {
				if ((pMF->ffmt == AST_MF_FFMT_FLAC ||
							pMF->ffmt == AST_MF_FFMT_NONE) &&
						! st_contOgg_gs_getHasFlacStream(&pMF->avOgg))
					ast_mf_op_prFInf("AUD flc (no FLAC streams found)\n");
				if ((pMF->ffmt == AST_MF_FFMT_OGG ||
							pMF->ffmt == AST_MF_FFMT_NONE) &&
						! st_contOgg_gs_getHasOggStream(&pMF->avOgg))
					ast_mf_op_prFInf("AUD ogg (no OGG streams found)\n");
			}
		}
	} else
		ast_mf_op_prFInf("AUD all (no Audio frames found)\n");

	/* output tags */
	if (st_tagMeta_gs_getArr_isAnyTagPresent(&pMF->tagArr)) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, "show Tags...");
		AST_MF__sw_tags(pCmdln, pMF);
	} else
		ast_mf_op_prFInf("TAG all (no Tags found)\n");
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
AST_MF__sw_isInTagInArray(const Tst_tagMeta_mtArr *pMA,
		const Tst_tagMeta_type mttpOrg)
{
	Tst_tagMeta_mt const *pMT = NULL;
	Tst_tagBasics const  *pTBas;

	if (mttpOrg != ST_MTAG_TTP_NONE)
		while ((pMT = st_tagMeta_ite_nextTag_byTypeOrg(pMA,
					mttpOrg, pMT)) != NULL) {
			pTBas = st_tagMeta_gs_getTag_tbas(pMT);
			if (pTBas != NULL && st_tagBas_gs_getHasTag(pTBas) &&
					st_tagMeta_gs_getTag_type(pMT) != ST_MTAG_TTP_MRG)
				return ST_B_TRUE;
		}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Print Tags
 *   Only (converted) ID3v2 tags are printed
 */
static void
AST_MF__sw_tags(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
#	define LOC_PRDIV_  for (xyz = 0; xyz < 7; xyz++) \
				ast_mf_op_prFInf("-%s", (xyz+1 == 7 ? "\n" : ""));
	const char             *cFNCN   = __FUNCTION__;
	const Tst_tagMeta_type cMTTPS[] = {ST_MTAG_TTP_IV1, ST_MTAG_TTP_AV2,
	                                   ST_MTAG_TTP_IV2, ST_MTAG_TTP_VOR,
	                                   ST_MTAG_TTP_MRG};
	const Tst_uint32       cMTTPS_N = sizeof(cMTTPS) / sizeof(Tst_tagMeta_type);
	Tst_uint32 x       = 0,
	           xyz,
	           bsIx    = 0,
	           bsSIx   = 0;
	Tst_bool   shwdOne = ST_B_FALSE;
	Tast_cln_t_tagList_b(mrgLst);
	Tst_tagMeta_mt const *pMT;
	Tst_tagMeta_type     mttpCur;
	Tst_id3v2_tag        *pIv2C;
	Tast_cln_t_tagListIx tlix;
#	if (AST_MF_SW_DEBUG_IV1 == 1)
	Tst_id3v1_tag        *pIv1;
#	endif

	ast_cln_stc_rsetT_taglist(mrgLst, ST_B_FALSE);

	/* for debugging purposes: */
#	if (AST_MF_SW_DEBUG_IV1 == 1)
	if (pCmdln->opts.lstTagSw[AST_CLN_T_TAGLIST_IV1] &&
			AST_MF__sw_isInTagInArray(&pMF->tagArr, ST_MTAG_TTP_IV1)) {
		pMT = NULL;
		while ((pMT = st_tagMeta_ite_nextTag_byType(
					&pMF->tagArr, ST_MTAG_TTP_IV1, pMT)) != NULL) {
			pIv1 = st_tagMeta_gs_getRealTag_iv1(pMT);
			if (pIv1 == NULL)
				continue;
			if (shwdOne)
				LOC_PRDIV_
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "showing Tag (UID 0x%08x)",
						st_tagMeta_gs_getTag_uid(pMT));
			ast_mf_sw_showTagIV1(pCmdln, pIv1);
			shwdOne = ST_B_TRUE;
		}
	}
#	endif

	/* get list of merged tags */
	for (x = 0; x < cMTTPS_N; x++) {
		tlix         = ast_mf_fnc_getSrcTLIXfromMTTP(cMTTPS[x]);
		mrgLst[tlix] = (tlix != AST_CLN_T_TAGLIST_MRG &&
				AST_MF__sw_isInTagInArray(&pMF->tagArr, cMTTPS[x]));
	}
	/* show tags */
	for (x = 0; x < cMTTPS_N; x++) {
		tlix = ast_mf_fnc_getSrcTLIXfromMTTP(cMTTPS[x]);
		if (! pCmdln->opts.lstTagSw[tlix])
			continue;
		pMT = NULL;
		while ((pMT = st_tagMeta_ite_nextTag_any(&pMF->tagArr, pMT)) != NULL) {
			if (st_tagMeta_gs_getTag_typeOrg(pMT) != cMTTPS[x])
				continue;
			mttpCur = st_tagMeta_gs_getTag_type(pMT);
			if (mttpCur != ST_MTAG_TTP_IV2 && mttpCur != ST_MTAG_TTP_MRG)
				continue;
			/* */
			pIv2C = st_tagMeta_gs_getRealTag_iv2(pMT);
			if (pIv2C == NULL)
				continue;
			if (shwdOne)
				LOC_PRDIV_
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "showing Tag (UID 0x%08x)",
						st_tagMeta_gs_getTag_uid(pMT));
			st_tagMeta_gs_getTag_bsIx(pMT, &bsIx, &bsSIx);
			ast_mf_sw_showTagIV2(pCmdln, pMF,
					bsIx, bsSIx,
					pIv2C, cMTTPS[x],
					ST_B_FALSE, mrgLst);
			shwdOne = ST_B_TRUE;
		}
	}

	if (! shwdOne)
		ast_mf_op_prFInf("TAG all (no Tags to show)\n");
#	undef LOC_PRDIV_
}

/******************************************************************************/
