/******************************************************************************/
/* mf_edit.c                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for editing Audio/Video files                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/streamrd.h"
#include "src/includes/common/streamwr.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/audvid/aud_mpeg1.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_meta.h"
#include "src/includes/tag/tag_id3v1.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_MF_EDIT_C
#include "mf_edit-pp.h"
/*** */
#include "mf_cnvmrg-pp.h"
#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#include "mf_stcs-pp.h"
#include "../cln/cln_stcs-prot.h"
#undef SRC_MF_EDIT_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), free(), ... */
#include <string.h>      /* memcpy(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
  (Horror) MPEG1 File: [IV2][IV2][AUDIO][IV1][APE][IV2][APE][IV1]
  (Horror) Flac/Ogg File: [IV2][AUDIO with multiple embedded VORBC][IV2]
  (Horror) Tag File: [APE][VORBC][APE][IV1][IV2][IV1][VORBC][IV2]

  Before writing any tag:
   - merge all IV2s --> one IV2 tag
     merge all APEs --> one APE tag
     ...
   - merge Tag-Fields from Cmdline with each of IV2, IV1, APE, VORBC, ...
  Writing tags:
   - MPEG1: we copy the audio data to a new file and at the same time
            we write the new tags
       e.g. [old IV2][old IV2][AUDIO][old IV1] --> [new IV2][AUDIO]
                                            or --> [new IV2][AUDIO][new IV1]
                                            or --> [AUDIO][new IV1]
     OGG and FLAC: we copy the audio data to a new file and at the same time
           we update the embedded vorbis tag if necessary/possible
       e.g. [old IV2][AUDIO with old embed. VORBC] --> [AUDIO with new embed. VORBC]
                                                or --> [AUDIO with old embed. VORBC]
   - all invalid or obsolete tags will be removed during this process.
     for example Flac shouldn't have any external tags, so if a IV2 tag
     was found in a Flac file it will be removed
*/

/*----------------------------------------------------------------------------*/

/*
 * Edits/Removes Tags
 *
 * pMF->fstc     MUST be set
 *      tagArr   MUST be set
 * pCmdln        MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
ast_mf_ed_editFile(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
#	define LOC_HAVETAG_SETWR_(mac_tlix)  { \
					hta[0]  = AST_MF__ed_haveTag(pMF, AST_CLN_T_TAGLIST_##mac_tlix); \
					hta[1] |= hta[0]; \
					edInf.tagEdLst[AST_CLN_T_TAGLIST_##mac_tlix] = hta[0]; \
				}
	const char *cFNCN = __FUNCTION__;
	Tst_err  res    = ST_ERR_SUCC;
	Tst_bool cont   = ST_B_TRUE,
	         hta[2] = {ST_B_FALSE, ST_B_FALSE};
	Tast_mf_editInfo edInf;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			! st_sysFStc_isWritable(&pMF->fstc))
	if (pCmdln->cmds.roMainCmds > 0 ||
			(pCmdln->cmds.rwTgRwrCmds == 0 &&
				pCmdln->cmds.rwTgEdtCmds == 0 &&
				pCmdln->cmds.rwTgRemCmds == 0 &&
				pCmdln->cmds.rwTgCnvCmds == 0))
		return ST_ERR_SUCC;

	if (pCmdln->opts.showStat)
		ast_mf_op_prMsg("*Editing tag(s)...");

	ast_mf_stc_initEditInfo(&edInf);

	/* set list of tags allowed to be written */
	/** set edInf.tagEdLst */
	ast_cln_stc_rsetT_taglist(edInf.tagEdLst, ST_B_FALSE);
	if (! pCmdln->opts.tagEdAuto) {
		Tst_uint32 x;

		for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
			if ((Tast_cln_t_tagListIx)x != AST_CLN_T_TAGLIST_MRG)
				edInf.tagEdLst[x] = pCmdln->opts.lstTagEd[x];
	}
	/** set edInf.allwLst and edInf.allwNonEmbTags */
	switch (pMF->ffmt) {
	case AST_MF_FFMT_MPG1:
		edInf.allwNonEmbTags = ST_B_TRUE;
		ast_cln_stc_rsetT_taglist(edInf.allwLst, ST_B_FALSE);
		edInf.allwLst[AST_CLN_T_TAGLIST_IV1] = ST_B_TRUE;
		edInf.allwLst[AST_CLN_T_TAGLIST_IV2] = ST_B_TRUE;
		edInf.allwLst[AST_CLN_T_TAGLIST_AV2] = ST_B_TRUE;
		/* */
		if (pCmdln->opts.tagEdAuto) {
			hta[1] = ST_B_FALSE;
			LOC_HAVETAG_SETWR_(IV1)
			LOC_HAVETAG_SETWR_(IV2)
			LOC_HAVETAG_SETWR_(AV2)
			if (! hta[1])
				edInf.tagEdLst[AST_CLN_T_TAGLIST_IV2] = ST_B_TRUE;
		}
		break;
	case AST_MF_FFMT_FLAC:
	case AST_MF_FFMT_OGG:
		edInf.allwNonEmbTags = ST_B_FALSE;
		ast_cln_stc_rsetT_taglist(edInf.allwLst, ST_B_FALSE);
		edInf.allwLst[AST_CLN_T_TAGLIST_VOR] = ST_B_TRUE;
		/* */
		if (pCmdln->opts.tagEdAuto)
			edInf.tagEdLst[AST_CLN_T_TAGLIST_VOR] = ST_B_TRUE;
		break;
	default:  /* all tag types allowed */
		edInf.allwNonEmbTags = ST_B_TRUE;
		ast_cln_stc_rsetT_taglist(edInf.allwLst, ST_B_TRUE);
		edInf.allwLst[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;
		/* */
		if (pCmdln->opts.tagEdAuto) {
			hta[1] = ST_B_FALSE;
			LOC_HAVETAG_SETWR_(IV1)
			LOC_HAVETAG_SETWR_(IV2)
			LOC_HAVETAG_SETWR_(AV2)
			LOC_HAVETAG_SETWR_(VOR)
			if (! hta[1])
				edInf.tagEdLst[AST_CLN_T_TAGLIST_IV2] = ST_B_TRUE;
		}
	}
	/** update edInf.tagEdLst */
	{
		Tst_uint32 x;

		for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
			if ((Tast_cln_t_tagListIx)x != AST_CLN_T_TAGLIST_MRG) {
				if (edInf.tagEdLst[x] && ! edInf.allwLst[x] &&
						! pCmdln->opts.quiet)
					ast_mf_op_prMsg("*  %s Tag not allowed in this file type",
							st_tagMeta_fnc_getTagDesc(
									ast_mf_fnc_mapTLIXtoMTTP(
											(Tast_cln_t_tagListIx)x))[0]);
				edInf.tagEdLst[x] = edInf.tagEdLst[x] && edInf.allwLst[x];
			}
	}

	/* none of the following commands change the input file */

	if (pCmdln->cmds.conv) {
		Tst_uint32 cnvTagsCnt = 0;

		/* convert tags if requested by cmdln */
		res = AST_MF__ed_cmdCnvTags(pCmdln, pMF, &edInf, &cnvTagsCnt);
		if (res != ST_ERR_SUCC)
			ast_mf_op_d_mfErr(pMF, cFNCN, "converting tags failed");
		else
			cont = (cnvTagsCnt > 0);
	}

	if (res == ST_ERR_SUCC && cont &&
			st_tagMeta_gs_getArr_amountTags(&pMF->tagArr) > 0) {
		/* convert/remove invalid tags by converting them into valid ones
		 *   if for example a Flac file has an ID3v2 tag then
		 *   the ID3v2 tag will be converted into a Vorbis tag  */
		res = AST_MF__ed_cnvremInvalidTags(pCmdln, pMF, &edInf);
		if (res != ST_ERR_SUCC)
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"converting/removing invalid tags failed");
	}

	if (res == ST_ERR_SUCC && cont && pCmdln->cmds.remTag) {
		Tst_uint32 remTagsCnt = 0;

		/* remove Tag(s) if requested by cmdln */
		res = AST_MF__ed_cmdRemTags(pCmdln, pMF, &edInf, &remTagsCnt);
		if (res != ST_ERR_SUCC)
			ast_mf_op_d_mfErr(pMF, cFNCN, "removing tags failed");
		else
			cont = (remTagsCnt > 0);
	}

	if (res == ST_ERR_SUCC && cont &&
			st_tagMeta_gs_getArr_amountTags(&pMF->tagArr) > 0) {
		/* merge Tags of some kind
		 *   if there are for example two IV2 tags in one file
		 *   then merge them into one tag  */
		res = ast_mf_cm_mergeTagsOfAkind(pCmdln, pMF, ST_B_FALSE, &edInf);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, cFNCN, "merging tags of a kind failed");
	}

	if (res == ST_ERR_SUCC && cont && pCmdln->cmds.rwTgEdtCmds > 0) {
		/* merge Tags with info from cmdline */
		res = AST_MF__ed_mergeTagsWithCmdln(pCmdln, pMF, &edInf);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"merging tags with data from cmdline failed");
	}

	if (res == ST_ERR_SUCC && cont) {
		/* set the main tag for each tag type, if not already set */
		AST_MF__ed_setMainTags(pCmdln, pMF, &edInf);
	}

	/* now we actually rewrite the input file */

	if (res == ST_ERR_SUCC && cont) {
		res = AST_MF__ed_rewriteFile(pCmdln, pMF, &edInf);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, cFNCN, "(re-)writing file failed");
	}

	ast_mf_stc_freeEditInfo(&edInf);
	return res;
#	undef LOC_HAVETAG_SETWR_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
AST_MF__ed_haveTag(Tast_mf_finfo *pMF, const Tast_cln_t_tagListIx tlix)
{
	Tst_tagMeta_mt      *pMT = NULL;
	Tst_tagMeta_type    mttp;
	Tst_tagBasics const *pTBas;

	mttp = ast_mf_fnc_mapTLIXtoMTTP(tlix);
	while ((pMT = st_tagMeta_ite_nextTag_byTypeOrg(
				&pMF->tagArr, mttp, pMT)) != NULL) {
		pTBas = st_tagMeta_gs_getTag_tbas(pMT);
		if (pTBas != NULL && st_tagBas_gs_getHasTag(pTBas))
			return ST_B_TRUE;
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Remove all tags that are listed in pCmdln->opts.lstRemTag
 *   The tags that have to be removed will be added to pEdInf->pObsoleteUIDs.
 *   Exmaple: file: [IV2][AUDIO][APE][APE][IV1]
 *            pCmdln->opts.lstRemTag: erase APE and IV1
 *              ---> [IV2][AUDIO]
*/
static Tst_err
AST_MF__ed_cmdRemTags(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf, Tst_uint32 *pRemTagsCnt)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 x,
	           uid;
	Tst_bool   clearVorTag;
	void       *pRTag;
	Tst_tagMeta_mt      *pMT;
	Tst_tagMeta_type    mttpRem;
	Tst_tagBasics const *pTBas;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			pEdInf == NULL || pRemTagsCnt == NULL)

	*pRemTagsCnt = 0;

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN, "cmd remove tags...");

	if (st_sysFStc_getFileSz(&pMF->fstc, NULL) == 0) {
		if (! pCmdln->opts.quiet)
			ast_mf_op_prMsg("*  no Tag(s) to remove...");
		return ST_ERR_SUCC;
	}

	for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++) {
		if (! pCmdln->opts.lstRemTag[x])
			continue;
		mttpRem = ast_mf_fnc_mapTLIXtoMTTP((Tast_cln_t_tagListIx)x);
		pMT     = NULL;
		/* */
		while ((pMT = st_tagMeta_ite_nextTag_byTypeOrg(
					&pMF->tagArr, mttpRem, pMT)) != NULL) {
			pTBas = st_tagMeta_gs_getTag_tbas(pMT);
			if (pTBas == NULL || ! st_tagBas_gs_getHasTag(pTBas))
				continue;
			uid = st_tagMeta_gs_getTag_uid(pMT);
			if (ast_mf_stc_editInfo_isUIDinObs(pEdInf, uid))
				continue;
			/* */
			clearVorTag = ST_B_FALSE;
			pRTag       = st_tagMeta_gs_getRealTag_void(pMT);
			if (mttpRem == ST_MTAG_TTP_VOR) {
				if (st_vorbc_gs_getTag_wasEmbedded((const Tst_vorbc_tag*)pRTag)) {
					Tst_uint32 bsIx  = 0,
					           bsSIx = 0;

					st_tagMeta_gs_getTag_bsIx(pMT, &bsIx, &bsSIx);
					if (! (pCmdln->optsAvMisc.edBsAll ||
							ast_mf_fnc_isBsInList(pCmdln->optsAvMisc.pEdBsArr,
									bsIx, bsSIx)))
						continue;
					clearVorTag = ST_B_TRUE;
				}
			}
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN,
						"removing %s Tag (UID 0x%08x)",
						st_tagMeta_fnc_getTagDesc(mttpRem)[0], uid);
			++(*pRemTagsCnt);
			if (clearVorTag) {
				res = st_vorbc_disableFld_all((Tst_vorbc_tag*)pRTag);
				if (res == ST_ERR_SUCC)
					res = ast_mf_cm_updRawTag(pCmdln, pMF, pEdInf, pMT);
			} else
				res = ast_mf_stc_editInfo_addUIDtoObs(pEdInf, uid);
			if (res != ST_ERR_SUCC)
				break;
		}
	}

	if (res == ST_ERR_SUCC && ! pCmdln->opts.quiet)
		ast_mf_op_prMsg("*  removed %u tag%s",
				*pRemTagsCnt, (*pRemTagsCnt == 1 ? "" : "s"));

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Merge Tag(s) of type mttp with info from commandline
 */
static Tst_err
AST_MF__ed_mergeTagsWithCmdln_sub(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const char *pFnc, const Tst_tagMeta_type mttp,
		Tast_mf_editInfo *pEdInf, Tst_bool *pEdited, Tst_uint32 *pCntEd)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 newUID = 0;
	char const *pTagD = NULL;

	*pEdited = ST_B_FALSE;

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
		pTagD = st_tagMeta_fnc_getTagDesc(mttp)[0];
		ast_mf_op_d_deb(pFnc,
				"merging %s Tag: cmdline&file", pTagD);
	}

	switch (mttp) {
	case ST_MTAG_TTP_IV1:
		res = ast_mf_cm_mergeCmdlnIV1(pCmdln, pMF, pEdInf,
				pEdInf->uidMainIV1, &newUID, pEdited);
		if (res != ST_ERR_SUCC)
			ast_mf_op_d_mfErr(pMF, pFnc,
					"merging cmdln with ID3v1 failed");
		else {
			if (*pEdited)
				++(*pCntEd);
			pEdInf->uidMainIV1 = newUID;
		}
		break;
	case ST_MTAG_TTP_IV2:
		res = ast_mf_cm_mergeCmdlnIV2(pCmdln, pMF, pEdInf,
				pEdInf->uidMainIV2, &newUID, pEdited);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, pFnc,
					"merging cmdln with ID3v2 failed");
		else {
			if (*pEdited)
				++(*pCntEd);
			pEdInf->uidMainIV2 = newUID;
		}
		break;
	case ST_MTAG_TTP_AV2:
		res = ast_mf_cm_mergeCmdlnAV2(pCmdln, pMF, pEdInf,
				pEdInf->uidMainAV2, &newUID, pEdited);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, pFnc,
					"merging cmdln with APEv2 failed");
		else {
			if (*pEdited)
				++(*pCntEd);
			pEdInf->uidMainAV2 = newUID;
		}
		break;
	case ST_MTAG_TTP_VOR:
		if (AST_MF__ed_haveTag(pMF, AST_CLN_T_TAGLIST_VOR)) {
			Tst_uint32 uid,
			           bsIx  = 0,
			           bsSIx = 0;
			Tst_tagMeta_mt *pMT = NULL;

			/* it might be that we need to do merging
			 *   with several tags here...   */
			while ((pMT = st_tagMeta_ite_nextTag_byType(
					&pMF->tagArr, ST_MTAG_TTP_VOR, pMT)) != NULL) {
				uid = st_tagMeta_gs_getTag_uid(pMT);
				if (ast_mf_stc_editInfo_isUIDinObs(pEdInf, uid))
					continue;
				st_tagMeta_gs_getTag_bsIx(pMT, &bsIx, &bsSIx);
				if (! (pCmdln->optsAvMisc.edBsAll ||
						ast_mf_fnc_isBsInList(pCmdln->optsAvMisc.pEdBsArr,
								bsIx, bsSIx)))
					continue;
				/* */
				res = ast_mf_cm_mergeCmdlnVOR(pCmdln, pMF, pEdInf,
						uid, &newUID, pEdited);
				if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
					ast_mf_op_d_mfErr(pMF, pFnc,
							"merging cmdln with Vorbis failed");
				else {
					if (pEdInf->uidMainVOR == 0)
						pEdInf->uidMainVOR = newUID;
					if (*pEdited)
						++(*pCntEd);
				}
			}
		} else if (pCmdln->optsAvMisc.edBsAll ||
				ast_mf_fnc_isBsInList(pCmdln->optsAvMisc.pEdBsArr, 1, 0)) {
			/* we only do merging creating a new tag here */
			res = ast_mf_cm_mergeCmdlnVOR(pCmdln, pMF, pEdInf,
					0, &newUID, pEdited);
			if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
				ast_mf_op_d_mfErr(pMF, pFnc,
						"merging cmdln with Vorbis failed");
			else {
				if (pEdInf->uidMainVOR == 0)
					pEdInf->uidMainVOR = newUID;
				if (*pEdited)
					++(*pCntEd);
			}
		}
		break;
	default:
		ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
		res = ST_ERR_FAIL;
	}

	if (res == ST_ERR_SUCC && newUID != 0 &&
			ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(pFnc,
				"main %s Tag has UID 0x%08x", pTagD, newUID);
	return res;
}

/*
 * Merge Tag(s) with info from commandline
 */
static Tst_err
AST_MF__ed_mergeTagsWithCmdln(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf)
{
	const char             *cFNCN   = __FUNCTION__;
	const Tst_tagMeta_type cMTTPS[] = {ST_MTAG_TTP_IV1, ST_MTAG_TTP_AV2,
	                                   ST_MTAG_TTP_IV2, ST_MTAG_TTP_VOR};
	const Tst_uint32       cMTTPS_N = sizeof(cMTTPS) / sizeof(Tst_tagMeta_type);
	Tst_err    res   = ST_ERR_SUCC;
	Tst_bool   edOne = ST_B_FALSE;
	Tst_uint32 cntEd = 0,
	           x;
	Tast_cln_t_tagListIx tlix;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL || pEdInf == NULL)

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN, "merge tags with cmdline...");

	for (x = 0; x < cMTTPS_N; x++) {
		tlix = ast_mf_fnc_getSrcTLIXfromMTTP(cMTTPS[x]);
		if (! pEdInf->tagEdLst[tlix])
			continue;
		res = AST_MF__ed_mergeTagsWithCmdln_sub(pCmdln, pMF, cFNCN,
				cMTTPS[x], pEdInf, &edOne, &cntEd);
		if (res != ST_ERR_SUCC)
			break;
	}

	if (res == ST_ERR_SUCC && ! pCmdln->opts.quiet)
		ast_mf_op_prMsg("*  edited %u tag%s",
				cntEd, (cntEd == 1 ? "" : "s"));

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert/Remove all tags of the type mttpSrc
 *   The tags that have to be removed will be added to pEdInf->pObsoleteUIDs
 */
static Tst_err
AST_MF__ed_cnvremInvalidTags_sub(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const char *pFnc, const char *pDbgAddText,
		Tast_mf_editInfo *pEdInf,
		const Tst_tagMeta_type mttpSrc, const Tst_tagMeta_type mttpDest,
		const Tst_bool doOnlyCnv, Tst_uint32 *pCntCnvRem)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 uid;
	void       *pTagTmp;
	char const *pTagD = NULL;
	Tst_bool   wasConv;
	Tst_tagMeta_mt      *pMT = NULL;
	Tst_tagBasics const *pTBas;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL || pEdInf == NULL ||
			mttpSrc == ST_MTAG_TTP_NONE || mttpDest == ST_MTAG_TTP_MRG ||
			pCntCnvRem == NULL)

	while ((pMT = st_tagMeta_ite_nextTag_byType(
				&pMF->tagArr, mttpSrc, pMT)) != NULL) {
		pTBas = st_tagMeta_gs_getTag_tbas(pMT);
		if (pTBas == NULL || ! st_tagBas_gs_getHasTag(pTBas))
			continue;
		uid = st_tagMeta_gs_getTag_uid(pMT);
		if (ast_mf_stc_editInfo_isUIDinObs(pEdInf, uid))
			continue;
		pTagD = st_tagMeta_fnc_getTagDesc(mttpSrc)[0];
		/**ast_mf_op_prf("%s(): "
				"found possible %s tag: UID 0x%08x\n", pFnc, pTagD, uid);**/
		/* */
		if (mttpSrc == ST_MTAG_TTP_IV2 && mttpSrc == mttpDest && doOnlyCnv) {
			Tst_id3v2_tag *pTagIV2 = st_tagMeta_gs_getRealTag_iv2(pMT);

			if (st_id3v2_gs_getTag_majorVersion(pTagIV2) !=
					pCmdln->optsTagIV2.saveAsVers) {
				if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
					ast_mf_op_d_deb(pFnc,
							"%s%s Tag version will be converted (UID 0x%08x)",
							pDbgAddText, pTagD, uid);
				++(*pCntCnvRem);
			}
			continue;
		}
		if (mttpSrc == mttpDest && doOnlyCnv)
			continue;
		/* */
		wasConv = ST_B_FALSE;
		if (mttpSrc != mttpDest && mttpSrc != ST_MTAG_TTP_MRG &&
				mttpDest != ST_MTAG_TTP_NONE) {
			/**if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(pFnc,
						"converting %s%s Tag (UID 0x%08x)",
						pDbgAddText, pTagD, uid);**/
			pTagTmp = st_tagMeta_gs_getRealTag_void(pMT);
			switch (mttpSrc) {
			case ST_MTAG_TTP_IV1:
				res = ast_mf_cm_convIv1ToX(pCmdln, pMF,
						(Tst_id3v1_tag*)pTagTmp, uid, pTBas, mttpDest);
				break;
			case ST_MTAG_TTP_IV2:
				res = ast_mf_cm_convIv2ToX(pCmdln, pMF,
						(Tst_id3v2_tag*)pTagTmp, uid, pTBas, mttpDest);
				break;
			case ST_MTAG_TTP_AV2:
				res = ast_mf_cm_convAv2ToX(pCmdln, pMF,
						(Tst_apev2_tag*)pTagTmp, uid, pTBas, mttpDest);
				break;
			case ST_MTAG_TTP_VOR:
				if (st_vorbc_gs_getTag_wasEmbedded((const Tst_vorbc_tag*)pTagTmp))
					continue;
				res = ast_mf_cm_convVorToX(pCmdln, pMF,
						(Tst_vorbc_tag*)pTagTmp, uid, pTBas, mttpDest, 1, 0);
				break;
			default:
				ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
				break;
			}
			wasConv = ST_B_TRUE;
		}
		if (res == ST_ERR_SUCC) {
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				if (wasConv)
					ast_mf_op_d_deb(pFnc,
							"%s%s Tag has been converted (UID 0x%08x)",
							pDbgAddText, pTagD, uid);
				else
					ast_mf_op_d_deb(pFnc,
							"removing %s%s Tag (UID 0x%08x)",
							pDbgAddText, pTagD, uid);
			}
			++(*pCntCnvRem);
			res = ast_mf_stc_editInfo_addUIDtoObs(pEdInf, uid);
		}
		if (res != ST_ERR_SUCC)
			break;
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert all tags that are listed in pCmdln->opts.convTagp.src
 *   The tags that have been converted will be added to pEdInf->pObsoleteUIDs.
 *   Exmaple: file: [IV2][AUDIO][APE][APE][IV1]
 *            pCmdln->opts.convTagp: convert all to IV2
 *              ---> [IV2][AUDIO]
*/
static Tst_err
AST_MF__ed_cmdCnvTags(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf, Tst_uint32 *pCnvTagsCnt)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 x;
	Tst_tagMeta_type mttpDest;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			pEdInf == NULL || pCnvTagsCnt == NULL)

	*pCnvTagsCnt = 0;

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN, "cmd convert tags...");

	if (st_sysFStc_getFileSz(&pMF->fstc, NULL) == 0) {
		if (! pCmdln->opts.quiet)
			ast_mf_op_prMsg("*  no Tag(s) to convert...");
		return ST_ERR_SUCC;
	}

	mttpDest = ast_mf_fnc_mapTLIXtoMTTP(pCmdln->opts.convTagp.dst);
	for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++) {
		if (! pCmdln->opts.convTagp.src[x])
			continue;
		res = AST_MF__ed_cnvremInvalidTags_sub(pCmdln, pMF, cFNCN, "", pEdInf,
				ast_mf_fnc_mapTLIXtoMTTP((Tast_cln_t_tagListIx)x), mttpDest,
				ST_B_TRUE, pCnvTagsCnt);
		if (res != ST_ERR_SUCC)
			break;
	}

	if (res == ST_ERR_SUCC && ! pCmdln->opts.quiet)
		ast_mf_op_prMsg("*  converted %u tag%s",
				*pCnvTagsCnt, (*pCnvTagsCnt == 1 ? "" : "s"));

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert/Remove invalid tags by converting them into valid ones
 *   If for example a Flac file has an ID3v2 tag then
 *   the ID3v2 tag will be converted into a Vorbis tag.
 *   The tags that have to be removed will be added to pEdInf->pObsoleteUIDs
 */
static Tst_err
AST_MF__ed_cnvremInvalidTags(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res       = ST_ERR_SUCC;
	Tst_uint32 x,
	           cntCnvRem = 0;
	Tst_tagMeta_type mttpDest = ST_MTAG_TTP_NONE;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL || pEdInf == NULL)

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN, "convert/remove invalid tags...");

	switch (pMF->ffmt) {
	case AST_MF_FFMT_MPG1:
		if (pEdInf->tagEdLst[AST_CLN_T_TAGLIST_IV2])
			mttpDest = ST_MTAG_TTP_IV2;
		else if (pEdInf->tagEdLst[AST_CLN_T_TAGLIST_AV2])
			mttpDest = ST_MTAG_TTP_AV2;
		else if (pEdInf->tagEdLst[AST_CLN_T_TAGLIST_IV1])
			mttpDest = ST_MTAG_TTP_IV1;
		break;
	case AST_MF_FFMT_FLAC:
	case AST_MF_FFMT_OGG:
		mttpDest = ST_MTAG_TTP_VOR;
		break;
	default:  /* all tag types allowed (remove only MRG)*/
		break;
	}

	for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++) {
		if (pEdInf->allwLst[x])
			continue;
		res = AST_MF__ed_cnvremInvalidTags_sub(pCmdln, pMF, cFNCN, "invalid ",
				pEdInf,
				ast_mf_fnc_mapTLIXtoMTTP((Tast_cln_t_tagListIx)x), mttpDest,
				ST_B_FALSE, &cntCnvRem);
		if (res != ST_ERR_SUCC)
			break;
	}

	if (res == ST_ERR_SUCC && pCmdln->opts.showStat)
		ast_mf_op_prMsg("*  converted/removed %u invalid tag%s",
				cntCnvRem, (cntCnvRem == 1 ? "" : "s"));

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Return the UID of the first valid tag of the type mttpCur
 */
static Tst_uint32
AST_MF__ed_setMainTags_sub(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const char *pFnc,
		Tast_mf_editInfo *pEdInf,
		const Tst_tagMeta_type mttpCur)
{
	Tst_uint32 uid = 0;
	Tst_tagMeta_mt      *pMT = NULL;
	Tst_tagBasics const *pTBas;

	while ((pMT = st_tagMeta_ite_nextTag_byType(
				&pMF->tagArr, mttpCur, pMT)) != NULL) {
		uid   = 0;
		pTBas = st_tagMeta_gs_getTag_tbas(pMT);
		if (pTBas == NULL || ! st_tagBas_gs_getHasFields(pTBas) ||
				st_tagMeta_gs_getTag_type(pMT) == ST_MTAG_TTP_MRG)
			continue;
		uid = st_tagMeta_gs_getTag_uid(pMT);
		if (ast_mf_stc_editInfo_isUIDinObs(pEdInf, uid)) {
			uid = 0;
			continue;
		}
		break;
	}

	if (uid != 0 && ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(pFnc,
				"setting main %s Tag to UID 0x%08x",
				st_tagMeta_fnc_getTagDesc(mttpCur)[0], uid);
	return uid;
}

/*
 * Set the main tag for each tag type, if not already set
 */
static void
AST_MF__ed_setMainTags(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf)
{
	const char *cFNCN = __FUNCTION__;

	ST_ASSERTN_VOID(pCmdln == NULL || pMF == NULL || pEdInf == NULL)

	if (pEdInf->uidMainIV1 == 0 &&
			pEdInf->allwLst[AST_CLN_T_TAGLIST_IV1])
		pEdInf->uidMainIV1 = AST_MF__ed_setMainTags_sub(pCmdln, pMF, cFNCN,
				pEdInf, ST_MTAG_TTP_IV1);
	if (pEdInf->uidMainIV2 == 0 &&
			pEdInf->allwLst[AST_CLN_T_TAGLIST_IV2])
		pEdInf->uidMainIV2 = AST_MF__ed_setMainTags_sub(pCmdln, pMF, cFNCN,
				pEdInf, ST_MTAG_TTP_IV2);
	if (pEdInf->uidMainAV2 == 0 &&
			pEdInf->allwLst[AST_CLN_T_TAGLIST_AV2])
		pEdInf->uidMainAV2 = AST_MF__ed_setMainTags_sub(pCmdln, pMF, cFNCN,
				pEdInf, ST_MTAG_TTP_AV2);
	if (pEdInf->uidMainVOR == 0 &&
			pEdInf->allwLst[AST_CLN_T_TAGLIST_VOR])
		pEdInf->uidMainVOR = AST_MF__ed_setMainTags_sub(pCmdln, pMF, cFNCN,
				pEdInf, ST_MTAG_TTP_VOR);
}

/*----------------------------------------------------------------------------*/

/*
 * Check if there are any tags that match some criteria
 *   and issue debug messages depending on the function params
 */
static Tst_bool
AST_MF__ed_checkTags(const char *pFnc,
		const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf,
		Tst_tagMeta_mt **ppMT, Tst_tagBasics **ppTBas,
		const Tst_foffs compareOffs,
		const Tst_bool cmdSkipOrWrite)
{
	Tst_bool   resB = ST_B_FALSE;
	Tst_uint32 uid;
	Tst_tagMeta_type mttpCur = ST_MTAG_TTP_NONE,
	                 mttpOrg = ST_MTAG_TTP_NONE;

	*ppMT   = NULL;
	*ppTBas = NULL;

	while ((*ppMT = st_tagMeta_ite_nextTag_any(&pMF->tagArr, *ppMT)) != NULL) {
		uid = st_tagMeta_gs_getTag_uid(*ppMT);
		if (ast_mf_stc_editInfo_isUIDinUsd(pEdInf, uid) ||
				st_tagMeta_gs_getTag_type(*ppMT) == ST_MTAG_TTP_MRG)
			continue;
		/* */
		mttpCur = st_tagMeta_gs_getTag_type(*ppMT);
		mttpOrg = st_tagMeta_gs_getTag_typeOrg(*ppMT);
		*ppTBas = (Tst_tagBasics*)st_tagMeta_gs_getTag_tbas(*ppMT);
		if (cmdSkipOrWrite) {
			/* mode: skip */

			if (*ppTBas == NULL ||
					(mttpOrg == ST_MTAG_TTP_VOR &&
						st_vorbc_gs_getTag_wasEmbedded(
							st_tagMeta_gs_getRealTag_vor(*ppMT))))
				continue;
			/* the tag must exist in the input file
			 *   at the current offset to be skipped...  */
			if (st_tagBas_gs_getHasTag(*ppTBas) &&
					compareOffs >= 0 &&
					compareOffs == st_tagBas_gs_getOffset(*ppTBas)) {
				resB = ST_B_TRUE;
				break;
			}
		} else if (pEdInf->allwNonEmbTags) {
			/* mode: write */

			/* ignore empty or obsolete tags */
			if (*ppTBas == NULL || ! st_tagBas_gs_getHasTag(*ppTBas) ||
					! st_tagBas_gs_getHasFields(*ppTBas) ||
					ast_mf_stc_editInfo_isUIDinObs(pEdInf, uid))
				continue;
			/* if the current tag is the main tag of the current tag type
			 *   and we're at the correct position in the output stream
			 *   we accept the current tag to be written  */
			switch (mttpCur) {
			case ST_MTAG_TTP_IV1:
				if ((! pEdInf->hasAudio || pEdInf->wroteAudio) &&
						pEdInf->wroteIV2 && pEdInf->wroteAV2 &&
						uid == pEdInf->uidMainIV1) {
					resB             = ST_B_TRUE;
					pEdInf->wroteIV1 = ST_B_TRUE;
				}
				break;
			case ST_MTAG_TTP_IV2:
				if (! (pEdInf->hasAudio && pEdInf->wroteAudio) &&
						uid == pEdInf->uidMainIV2) {
					resB             = ST_B_TRUE;
					pEdInf->wroteIV2 = ST_B_TRUE;
				}
				break;
			case ST_MTAG_TTP_AV2:
				if ((! pEdInf->hasAudio || pEdInf->wroteAudio) &&
						pEdInf->wroteIV2 &&
						uid == pEdInf->uidMainAV2) {
					resB             = ST_B_TRUE;
					pEdInf->wroteAV2 = ST_B_TRUE;
				}
				break;
			case ST_MTAG_TTP_VOR:
				if (st_vorbc_gs_getTag_wasEmbedded(
							st_tagMeta_gs_getRealTag_vor(*ppMT)))
					continue;
				if ((! pEdInf->hasAudio || pEdInf->wroteAudio) &&
						pEdInf->wroteIV2 &&
						uid == pEdInf->uidMainVOR) {
					resB             = ST_B_TRUE;
					pEdInf->wroteVOR = ST_B_TRUE;
				}
				break;
			default:
				ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
				break;
			}
			if (resB)
				break;
		}
	}
	if (resB) {
		if (cmdSkipOrWrite &&
				ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(pFnc,
					"skipping %s Tag in input (UID 0x%08x)",
					st_tagMeta_fnc_getTagDesc(mttpOrg)[0],
					uid);
		if (! cmdSkipOrWrite)
			ast_mf_stc_editInfo_addUIDtoUsd(pEdInf, uid);
	} else {
		*ppMT   = NULL;
		*ppTBas = NULL;
	}
	return resB;
}

/*
 * Copy some data from pStrrd to pStrwr
 */
static Tst_err
AST_MF__ed_copyData(const Tst_bool pretWr,
		Tst_streamrd *pStrrd, Tst_streamwr *pStrwr,
		const Tst_fsize copySz, Tst_fsize *pCopied)
{
	Tst_err    res       = ST_ERR_SUCC;
	Tst_buf    buf[4096 * 4];
	Tst_uint32 rdFromStr = 0,
	           toCp      = sizeof(buf);

	*pCopied = 0;
	while (*pCopied < copySz) {
		if (*pCopied + (Tst_fsize)toCp > copySz)
			toCp = (Tst_uint32)(copySz - *pCopied);
		if (! pretWr) {
			res = st_streamrd_rdBuffer_noCRC(pStrrd, toCp, buf, &rdFromStr);
			if (res == ST_ERR_SUCC)
				res = st_streamwr_wrBuffer_noCRC(pStrwr, rdFromStr, buf);
			if (res != ST_ERR_SUCC)
				break;
			*pCopied += (Tst_fsize)rdFromStr;
			if (rdFromStr != toCp)
				break;
		} else {
			st_streamrd_rdSkipBytes(pStrrd, toCp, ST_B_TRUE);
			*pCopied += (Tst_fsize)toCp;
		}
	}
	return res;
}

/*
 * (Re-)Write tag
 */
static Tst_err
AST_MF__ed_writeTag(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tst_tagBasics const *pTBas, Tst_tagMeta_mt const *pMT,
		Tst_streamwr *pStrwr, Tst_fsize *pWrittenBytes)
{
#	define LOC_RETIFEMPTY_  { \
					if (! st_tagBas_gs_getHasFields(pTBas)) { \
						if (pCmdln->opts.showStat) \
							ast_mf_op_prMsg("*  tag is empty, ignoring it (UID 0x%08x)", uid); \
						return ST_ERR_SUCC; \
					} }
	const char *cFNCN = __FUNCTION__;
	Tst_err    res = ST_ERR_SUCC;
	void       *pTagTmp;
	Tst_bool   hasTag;
	Tst_uint32 uid;
	char const *pTagD;
	Tst_tagMeta_type mttpCur,
	                 mttpOrg;

	*pWrittenBytes = 0;

	uid = st_tagMeta_gs_getTag_uid(pMT);
	/* ignore tag if it's empty */
	LOC_RETIFEMPTY_

	hasTag  = st_tagBas_gs_getHasTag(pTBas);
	mttpCur = st_tagMeta_gs_getTag_type(pMT);
	mttpOrg = st_tagMeta_gs_getTag_typeOrg(pMT);
	pTagD   = st_tagMeta_fnc_getTagDesc(mttpCur)[0];
	pTagTmp = st_tagMeta_gs_getRealTag_void(pMT);

	if (hasTag && mttpCur == ST_MTAG_TTP_VOR) {
		if (st_vorbc_gs_getTag_wasEmbedded((const Tst_vorbc_tag*)pTagTmp)) {
			/* this needs to be embedded into an audio/video stream */
			return ST_ERR_SUCC;
		}
	}

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
		if (! hasTag)
			ast_mf_op_d_deb(cFNCN,
					"(%s Tag not present, creating it)", pTagD);
		ast_mf_op_d_deb(cFNCN, "writing %s Tag (UID 0x%08x)",
				pTagD, uid);
	}

	switch (mttpCur) {
	case ST_MTAG_TTP_IV1:
		/* write ID3v1 Tag */
		res = st_id3v1_writeTagToStream((Tst_id3v1_tag*)pTagTmp);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"writing ID3v1 tag to stream failed");
		break;
	case ST_MTAG_TTP_IV2:
		/* write ID3v2 Tag */
		if (! hasTag ||
				(pCmdln->cmds.conv &&
					pCmdln->opts.convTagp.dst == AST_CLN_T_TAGLIST_IV2)) {
			st_id3v2_gs_setTag_majorVersion((Tst_id3v2_tag*)pTagTmp,
					pCmdln->optsTagIV2.saveAsVers);
			/* */
			res = st_id3v2_tidyUpTag((Tst_id3v2_tag*)pTagTmp, ST_B_TRUE);
			if (res != ST_ERR_SUCC)
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"tidying up ID3v2 tag failed");
			else
				LOC_RETIFEMPTY_
		}
		if (res == ST_ERR_SUCC) {
			res = st_id3v2_writeTagToStream((Tst_id3v2_tag*)pTagTmp);
			if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"writing ID3v2 tag to stream failed");
		}
		break;
	case ST_MTAG_TTP_AV2:
		/* write APEv2 Tag */
		res = st_apev2_writeTagToStream((Tst_apev2_tag*)pTagTmp);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"writing APEv2 tag to stream failed");
		break;
	case ST_MTAG_TTP_VOR:
		/* write Vorbis Tag */
		res = st_vorbc_writeTagToStream((Tst_vorbc_tag*)pTagTmp);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"writing Vorbis tag to stream failed");
		break;
	default:
		ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
		res = ST_ERR_FAIL;
	}

	if (res == ST_ERR_SUCC) {
		*pWrittenBytes = (Tst_fsize)st_tagBas_gs_getSize(pTBas);
		if (! pCmdln->opts.quiet)
			ast_mf_op_prMsg("*  %s%s Tag successfully written%s",
					(mttpCur == mttpOrg ? "" : "(converted) "),
					pTagD, (pCmdln->opts.basOpts.pretWr ? "  (pretended)" : ""));
	}
	return res;
#	undef LOC_RETIFEMPTY_
}

/*
 * Rewrite file
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MF__ed_rewriteFile(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res    = ST_ERR_SUCC;
	Tst_fsize  fszI,
	           fszN   = 0,
	           wrb    = 0,
	           toCp,
	           cpd    = 0,
	           skdByt = 0;
	Tst_foffs  curOffs;
	Tst_str    tmpFn[1024 + 1];
	Tst_bool   fndOne = ST_B_FALSE,
	           doCopy,
	           hasMPEG1,
	           hasOGG,
	           hasFLAC;
	Tst_tagBasics  *pTBas;
	Tst_tagMeta_mt *pMT;
	Tst_sys_fstc   fstcOut;
	Tst_streamrd   strrd;
	Tst_streamwr   strwr;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL || pEdInf == NULL)

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN, "rewrite file%s...",
				(pCmdln->opts.basOpts.pretWr ? " (pretend)" : ""));

	fszI = st_sysFStc_getFileSz(&pMF->fstc, NULL);

	/* */
	if (! st_sysFStc_setFPos(&pMF->fstc, 0, ST_SYS_SETFPOS_BEG)) {
		ast_mf_op_d_mfErr(pMF, cFNCN,
				"can't seek in input file '%s'",
				st_sysFStc_getFilen(&pMF->fstc));
		return ST_ERR_FAIL;
	}

	/* open output stream */
	/** */
	if (! st_sysGetTmpFilename(tmpFn, sizeof(tmpFn), ST_B_TRUE)) {
		ast_mf_op_d_mfErr(pMF, cFNCN,
				"can't create temp filename");
		return ST_ERR_FAIL;
	}
	/** */
	st_sys_stc_initFStc(&fstcOut);
	if (! pCmdln->opts.basOpts.pretWr) {
		st_sysFStc_setFilen(&fstcOut, tmpFn);
		res = st_sysFStc_openNew(&fstcOut);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"can't open tmp output-file '%s'", tmpFn);
			st_sys_stc_freeFStc(&fstcOut);
			return res;
		}
	}
	/** */
	st_streamwr_stc_initSObj(&strwr);
	if (! pCmdln->opts.basOpts.pretWr) {
		res = st_streamwr_setOutput_file(&strwr, &fstcOut);
		if (res != ST_ERR_SUCC) {
			st_streamwr_stc_freeSObj(&strwr);
			st_sys_stc_freeFStc(&fstcOut);
			st_sysUnlinkFile(tmpFn);
			return res;
		}
	}
	st_tagMeta_gs_setArr_strwrForAllTags(&pMF->tagArr, &strwr);
	st_contOgg_gs_setStrwr(&pMF->avOgg, &strwr);

	/* open input stream */
	st_streamrd_stc_initSObj(&strrd);
	res = st_streamrd_setInput_file(&strrd, &pMF->fstc);
	if (res != ST_ERR_SUCC) {
		st_tagMeta_gs_setArr_strwrForAllTags(&pMF->tagArr, NULL);
		st_contOgg_gs_setStrwr(&pMF->avOgg, NULL);
		st_streamrd_stc_freeSObj(&strrd);
		st_streamwr_stc_freeSObj(&strwr);
		st_sys_stc_freeFStc(&fstcOut);
		if (! pCmdln->opts.basOpts.pretWr)
			st_sysUnlinkFile(tmpFn);
		return res;
	}
	st_contOgg_gs_setStrrd(&pMF->avOgg, &strrd);

	/* */
	if (pEdInf->uidMainIV1 == 0) {
		pEdInf->wroteIV1 = ST_B_TRUE;
		/**ast_mf_op_prf(" no main IV1\n");**/
	}
	if (pEdInf->uidMainIV2 == 0) {
		pEdInf->wroteIV2 = ST_B_TRUE;
		/**ast_mf_op_prf(" no main IV2\n");**/
	}
	if (pEdInf->uidMainAV2 == 0) {
		pEdInf->wroteAV2 = ST_B_TRUE;
		/**ast_mf_op_prf(" no main AV2\n");**/
	}
	if (pEdInf->uidMainVOR == 0) {
		pEdInf->wroteVOR = ST_B_TRUE;
		/**ast_mf_op_prf(" no main VOR\n");**/
	}
	hasMPEG1 = st_mpeg1_gs_getStr_hasMPEGstream(&pMF->audMpg1);
	hasOGG   = st_contOgg_gs_getHasOggStream(&pMF->avOgg);
	hasFLAC  = st_contOgg_gs_getHasFlacStream(&pMF->avOgg);
	if (hasMPEG1 || hasOGG || hasFLAC) {
		pEdInf->hasAudio = ST_B_TRUE;
	} else {
		pEdInf->hasAudio = ST_B_FALSE;
		/**ast_mf_op_prf(" no audio/video\n");**/
	}
	pEdInf->wroteAudio = ! pEdInf->hasAudio;

	/* (re-)write some tags and rewrite or copy audio  */
	if (pCmdln->opts.showStat)
		ast_mf_op_prMsg("*(Re-)Writing file...");
	while ((curOffs = st_streamrd_getCurPos(&strrd)) < (Tst_foffs)fszI) {
		fndOne = ST_B_FALSE;
		pTBas  = NULL;
		pMT    = NULL;
		/* check whether we have a tag to skip */
		fndOne = AST_MF__ed_checkTags(cFNCN, pCmdln, pMF,
				pEdInf, &pMT, &pTBas, curOffs,
				/*cmdSkipOrWrite:*/ST_B_TRUE);
		if (fndOne) {
			/* skip tag in input file */
			toCp = (Tst_fsize)st_tagBas_gs_getSize(pTBas);
			if (toCp > 0)
				st_streamrd_rdSkipBytes(&strrd, (Tst_uint32)toCp, ST_B_TRUE);
			/**ast_mf_op_prf(" skipped tag\n");**/
			continue;
		} else if (pEdInf->wroteAudio)
			break;
		/* check whether we have a tag to (re-)write */
		fndOne = AST_MF__ed_checkTags(cFNCN, pCmdln, pMF,
				pEdInf, &pMT, &pTBas, -1,
				/*cmdSkipOrWrite:*/ST_B_FALSE);
		if (fndOne) {
			if (skdByt > 0) {
				if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
					ast_mf_op_d_deb(cFNCN,
							"skipped "ST_TFSIZE_PRF_LU" byte%s of input",
							(Tst_fsize)skdByt, (skdByt == 1 ? "" : "s"));
				skdByt = 0;
			}
			/* (re-)write tag */
			/**ast_mf_op_prf(" write tag\n");**/
			res = AST_MF__ed_writeTag(pCmdln, pMF, pTBas, pMT,
					&strwr, &wrb);
			if (res != ST_ERR_SUCC) {
				if (res != ST_ERR_ABRT)
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"writing tag failed");
				break;
			}
			fszN += wrb;
		} else {
			toCp   = 0;
			doCopy = ST_B_FALSE;
			/**if (skdByt == 0) ast_mf_op_prf(" audVid ?\n");**/
			/* check whether we have audio/video to copy */
			if (pEdInf->hasAudio && ! pEdInf->wroteAudio) {
				/**ast_mf_op_prf(" audVid still ? \n");**/
				if (hasMPEG1 &&
						curOffs == st_mpeg1_gs_getStr_startOffset(&pMF->audMpg1)) {
					toCp = st_mpeg1_gs_getStr_size(&pMF->audMpg1);
					if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
						ast_mf_op_d_deb(cFNCN,
								"copying MPEG1 stream");
					pEdInf->wroteAudio = ST_B_TRUE;
					doCopy             = ST_B_TRUE;
					/**ast_mf_op_prf(" audVid - MPG1\n");**/
				} else if ((hasOGG || hasFLAC) &&
						curOffs == st_contOgg_gs_getStartOffset(&pMF->avOgg)) {
					if (pEdInf->modEmbVorTag ||
							st_contOgg_opts_getForceRewriteComments(&pMF->avOgg)) {
						if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
							ast_mf_op_d_deb(cFNCN,
									"rewriting %s stream",
									(hasOGG ? "OGG" : "Flac"));
						if (hasOGG)
							res = st_contOgg_writeStreams(&pMF->avOgg);
						else
							res = st_contOgg_writeFlacStream(&pMF->avOgg);
						if (res != ST_ERR_SUCC) {
							if (res != ST_ERR_ABRT)
								ast_mf_op_d_mfErr(pMF, cFNCN,
										"writing %s failed",
										(hasOGG ? "OGG" : "Flac"));
							break;
						}
						if (! pCmdln->opts.quiet)
							ast_mf_op_prMsg("*  %s stream successfully rewritten%s",
									(hasOGG ? "OGG" : "Flac"),
									(pEdInf->modEmbVorTag ?
										" - with modified Vorbis Tag" : ""));
						fszN += st_contOgg_gs_getSize(&pMF->avOgg);
					} else {
						toCp = st_contOgg_gs_getSize(&pMF->avOgg);
						if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
							ast_mf_op_d_deb(cFNCN,
									"copying %s stream",
									(hasOGG ? "OGG" : "Flac"));
						doCopy = ST_B_TRUE;
					}
					pEdInf->wroteAudio = ST_B_TRUE;
					/**ast_mf_op_prf(" audVid - OGG/Flac\n");**/
				}
				/* */
				if (doCopy && toCp > 0 &&
						ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
					if (pCmdln->opts.basOpts.pretWr)
						ast_mf_op_d_deb(cFNCN,
								"audio/video stream: sz "ST_TFSIZE_PRF_LU,
								toCp);
					else
						ast_mf_op_d_deb(cFNCN,
								"audio/video stream: "
								"o "ST_TFOFFS_PRF_0X", sz "ST_TFSIZE_PRF_LU,
								curOffs, toCp);
				}
			}
			/* copy data */
			if (doCopy && toCp > 0) {
				if (skdByt > 0) {
					if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
						ast_mf_op_d_deb(cFNCN,
								"skipped "ST_TFSIZE_PRF_LU" byte%s of input",
								(Tst_fsize)skdByt, (skdByt == 1 ? "" : "s"));
					skdByt = 0;
				}
				/* */
				/**ast_mf_op_prf(" copy audVid\n");**/
				res = AST_MF__ed_copyData(pCmdln->opts.basOpts.pretWr,
						&strrd, &strwr, toCp, &cpd);
				if (res != ST_ERR_SUCC) {
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"copying data failed");
					break;
				}
				fszN += cpd;
			} else {
				/* skip one byte */
				st_streamrd_rdSkipBytes(&strrd, 1, ST_B_TRUE);
				++skdByt;
				if (skdByt % 1024 == 0 && pCmdln->opts.showStat)
					ast_mf_op_prMsg("   (skipped "ST_TFSIZE_PRF_LU" bytes)",
							(Tst_fsize)skdByt);
			}
		}
	}
	/* */
	if (skdByt > 0) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN,
					"skipped "ST_TFSIZE_PRF_LU" byte%s of input",
					(Tst_fsize)skdByt, (skdByt == 1 ? "" : "s"));
		skdByt = 0;
	}
	/* write only new tags */
	do {
		if (! pEdInf->allwNonEmbTags)
			break;
		fndOne = ST_B_FALSE;
		pTBas  = NULL;
		pMT    = NULL;
		/* check whether we have a tag to (re-)write */
		fndOne = AST_MF__ed_checkTags(cFNCN, pCmdln, pMF,
				pEdInf, &pMT, &pTBas, -1,
				/*cmdSkipOrWrite:*/ST_B_FALSE);
		if (! fndOne)
			continue;
		/* (re-)write tag */
		res = AST_MF__ed_writeTag(pCmdln, pMF, pTBas, pMT,
				&strwr, &wrb);
		if (res != ST_ERR_SUCC) {
			if (res != ST_ERR_ABRT)
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"writing tag failed");
			break;
		}
		fszN += wrb;
	} while (fndOne);

	/* close all in- and output */
	st_tagMeta_gs_setArr_strwrForAllTags(&pMF->tagArr, NULL);
	st_streamrd_stc_freeSObj(&strrd);
	st_streamwr_stc_freeSObj(&strwr);
	st_sys_stc_freeFStc(&fstcOut);
	if (res == ST_ERR_SUCC && ! pCmdln->opts.basOpts.pretWr)
		st_sysFStc_close(&pMF->fstc);

	/* */
	if (res == ST_ERR_SUCC) {
		if (! pCmdln->opts.basOpts.pretWr) {
			Tst_str const *pInpFilen = st_sysFStc_getFilen(&pMF->fstc);

			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN,
						"replacing old file with new one");
			if (! st_sysUnlinkFile(pInpFilen)) {
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"can't remove file '%s'", pInpFilen);
				res = ST_ERR_FAIL;
			} else {
				if (! st_sysRenameFile(tmpFn, pInpFilen)) {
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"can't rename temp. file '%s' to '%s'",
							tmpFn, pInpFilen);
					res = ST_ERR_FAIL;
				}
			}
		}
	}

	if (res == ST_ERR_SUCC && pCmdln->opts.showStat)
		ast_mf_op_prMsg("*  filesize after rewriting file%s: "
				ST_TFSIZE_PRF_LU" bytes  (d "ST_TFOFFS_PRF_PLD" bytes)",
				(pCmdln->opts.basOpts.pretWr ? " would be" : ""),
				(Tst_fsize)fszN,
				(Tst_foffs)fszN - (Tst_foffs)fszI);

	if (res == ST_ERR_SUCC && ! pCmdln->opts.basOpts.pretWr) {
		/* open freshly created file */
		res = st_sysFStc_openExisting(&pMF->fstc,
				pCmdln->opts.basOpts.allowLnkInpF, ST_B_TRUE);
	} else {
		if (! pCmdln->opts.basOpts.pretWr)
			st_sysUnlinkFile(tmpFn);
	}

	return res;
}

/******************************************************************************/
