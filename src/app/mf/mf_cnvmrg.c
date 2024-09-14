/******************************************************************************/
/* mf_cnvmrg.c                    [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for converting/merging Tags                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 24.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_fldmap.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_conv.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_id3v1.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_meta.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_MF_CNVMRG_C
#include "mf_cnvmrg-pp.h"
/*** */
#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#include "mf_stcs-pp.h"
#include "../cln/cln_fnc-prot.h"
#undef SRC_MF_CNVMRG_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), free(), ... */
#include <string.h>      /* memcpy(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Convert and merge tags
 *   The converted tags will be removed from the tag array.
 *   The merged tags will remain in the tag array.
 *
 * returns: ERRCODE
 */
Tst_err
ast_mf_cm_convAndMerge(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	const char *cFNCN = __func__;
	Tst_err res;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			pMF->pFilen == NULL)

	if (! (pCmdln->cmds.rd || pCmdln->cmds.roTgExtCmds > 0) ||
			! AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_TAG) ||
			! (pCmdln->cmds.roTgExtCmds > 0 ||
				ast_cln_fnc_isAnySetInTaglist(pCmdln->opts.lstTagSw)))
		return ST_ERR_SUCC;

	/* convert tags to ID3v2 */
	res = AST_MF__cm_convAllTagsToIV2(pCmdln, pMF);
	if (res != ST_ERR_SUCC) {
		ast_mf_op_d_mfErr(pMF, cFNCN,
				"converting all tags to ID3v2 failed");
		return res;
	}

	/* merge all (converted) ID3v2 tags */
	if (pCmdln->opts.lstTagSw[AST_CLN_T_TAGLIST_MRG]) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, "merge all (converted) ID3v2 Tags...");
		res = AST_MF__cm_mergeTagsIV2_all(pCmdln, pMF);
		if (res != ST_ERR_SUCC)
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"merging all ID3v2 tags failed");
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Merge Tags of a kind
 *   The UIDs of the tags that are obsolete after merging
 *   will be stored in pEdInf->pObsoleteUIDs.
 *   The UIDs of the remaining 'main' tags will also be stored in pEdInf.
 *   The merged tags will remain in the tag array.
 */
Tst_err
ast_mf_cm_mergeTagsOfAkind(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const Tst_bool mergeEmbVorbc,
		Tast_mf_editInfo *pEdInf)
{
	const char             *cFNCN   = __func__;
	const Tst_tagMeta_type cMTTPS[] = {ST_MTAG_TTP_IV1, ST_MTAG_TTP_IV2,
	                                   ST_MTAG_TTP_AV2, ST_MTAG_TTP_VOR};
	const Tst_uint32       cMTTPS_N = sizeof(cMTTPS) / sizeof(Tst_tagMeta_type);
	Tst_err    res      = ST_ERR_SUCC;
	Tst_uint32 x,
	           cnt,
	           uidTmp   = 0,
	           uidFirst,
	           uidCur   = 0,
	           cntMrg   = 0;
	void       *pTagTmp = NULL,
	           *pTagFirst,
	           *pTagCur = NULL;
	char const *pTagD;
	Tst_tagMeta_mt        *pMT = NULL,
	                      *pMTfirst;
	Tst_tagBasics const   *pTBas;
	//Tast_cln_t_tagListIx  tlix;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			pMF->pFilen == NULL || pEdInf == NULL)

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN, "merge tags of a kind...");

	if (st_tagMeta_gs_getArr_amountTags(&pMF->tagArr) == 0)
		return ST_ERR_SUCC;

	for (x = 0; x < cMTTPS_N; x++) {
		pTagD = st_tagMeta_fnc_getTagDesc(cMTTPS[x])[0];
		pTBas = NULL;
		//tlix  = ast_mf_fnc_getSrcTLIXfromMTTP(cMTTPS[x]);
		pMT   = NULL;
		/* */
		cnt       = 0;
		pTagFirst = NULL;
		uidFirst  = 0;
		pMTfirst  = NULL;
		/* */
		while ((pMT = st_tagMeta_ite_nextTag_byType(
					&pMF->tagArr, cMTTPS[x], pMT)) != NULL) {
			pTBas = st_tagMeta_gs_getTag_tbas(pMT);
			if (pTBas == NULL || ! st_tagBas_gs_getHasFields(pTBas))
				continue;
			/* */
			uidTmp = st_tagMeta_gs_getTag_uid(pMT);
			if (ast_mf_stc_editInfo_isUIDinObs(pEdInf, uidTmp))
				continue;
			if (++cnt == 1) {
				uidFirst = uidTmp;
				pMTfirst = pMT;
			} else
				uidCur = uidTmp;
			/* */
			pTagCur = NULL;
			pTagTmp = st_tagMeta_gs_getRealTag_void(pMT);
			switch (cMTTPS[x]) {
			case ST_MTAG_TTP_IV1:
				if (cnt == 1)
					pEdInf->uidMainIV1 = uidFirst;
				break;
			case ST_MTAG_TTP_IV2:
				if (cnt == 1)
					pEdInf->uidMainIV2 = uidFirst;
				break;
			case ST_MTAG_TTP_AV2:
				if (cnt == 1)
					pEdInf->uidMainAV2 = uidFirst;
				break;
			case ST_MTAG_TTP_VOR:
				if (cnt == 2 && ! mergeEmbVorbc &&
						st_vorbc_gs_getTag_wasEmbedded(
								(const Tst_vorbc_tag*)pTagTmp)) {
					--cnt;
					continue;
				}
				if (cnt == 1)
					pEdInf->uidMainVOR = uidFirst;
				break;
			default:
				ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
				break;
			}
			if (cnt == 1)
				pTagFirst = pTagTmp;
			else
				pTagCur = pTagTmp;
			if (cnt < 2)
				continue;
			ST_ASSERTN_NUM(ST_ERR_FAIL, pTagFirst == NULL || pTagCur == NULL)
			/* */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN,
						"merging %s Tags (UID 0x%08x | 0x%08x)...",
						pTagD, uidFirst, uidCur);
			++cntMrg;
			switch (cMTTPS[x]) {
			case ST_MTAG_TTP_IV1:
				res = st_id3v1_mergeTags((Tst_id3v1_tag*)pTagFirst,
						(Tst_id3v1_tag*)pTagCur);
				break;
			case ST_MTAG_TTP_IV2:
				res = st_id3v2_mergeTags((Tst_id3v2_tag*)pTagFirst,
						(Tst_id3v2_tag*)pTagCur, ST_B_TRUE);
				break;
			case ST_MTAG_TTP_AV2:
				res = st_apev2_mergeTags((Tst_apev2_tag*)pTagFirst,
						(Tst_apev2_tag*)pTagCur, ST_B_TRUE);
				break;
			case ST_MTAG_TTP_VOR:
				res = st_vorbc_mergeTags((Tst_vorbc_tag*)pTagFirst,
						(Tst_vorbc_tag*)pTagCur, ST_B_TRUE);
				if (res == ST_ERR_SUCC)
					res = ast_mf_cm_updRawTag(pCmdln, pMF, pEdInf, pMTfirst);
				break;
			default:
				break;
			}
			/* add uidCur to obsolete UIDs */
			if (res == ST_ERR_SUCC)
				res = ast_mf_stc_editInfo_addUIDtoObs(pEdInf, uidCur);
			if (res != ST_ERR_SUCC)
				break;
		} /* end while */
	}

	if (res == ST_ERR_SUCC && pCmdln->opts.showStat)
		ast_mf_op_prMsg("*  merged %u tag%s",
				cntMrg, (cntMrg == 1 ? "" : "s"));

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Merge ID3v1 Tag values from the cmdline with those read from the file
 */
Tst_err
ast_mf_cm_mergeCmdlnIV1(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uidOfDestTag, Tst_uint32 *pNewUID,
		Tst_bool *pEdited)
{
	return AST_MF__cm_mergeCmdln(pCmdln, pMF, pEdInf, ST_MTAG_TTP_IV1,
			uidOfDestTag, pNewUID, pEdited);
}

/*
 * Merge ID3v2 Tag values from the cmdline with those read from the file
 */
Tst_err
ast_mf_cm_mergeCmdlnIV2(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uidOfDestTag, Tst_uint32 *pNewUID,
		Tst_bool *pEdited)
{
	return AST_MF__cm_mergeCmdln(pCmdln, pMF, pEdInf, ST_MTAG_TTP_IV2,
			uidOfDestTag, pNewUID, pEdited);
}

/*
 * Merge APEv2 Tag values from the cmdline with those read from the file
 */
Tst_err
ast_mf_cm_mergeCmdlnAV2(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uidOfDestTag, Tst_uint32 *pNewUID,
		Tst_bool *pEdited)
{
	return AST_MF__cm_mergeCmdln(pCmdln, pMF, pEdInf, ST_MTAG_TTP_AV2,
			uidOfDestTag, pNewUID, pEdited);
}

/*
 * Merge Vorbis Tag values from the cmdline with those read from the file
 */
Tst_err
ast_mf_cm_mergeCmdlnVOR(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uidOfDestTag, Tst_uint32 *pNewUID,
		Tst_bool *pEdited)
{
	return AST_MF__cm_mergeCmdln(pCmdln, pMF, pEdInf, ST_MTAG_TTP_VOR,
			uidOfDestTag, pNewUID, pEdited);
}

/*----------------------------------------------------------------------------*/

/*
 * Update embedded tags
 */
Tst_err
ast_mf_cm_updRawTag(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf, Tst_tagMeta_mt *pMT)
{
	const char *cFNCN = __func__;
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 bsIx  = 0,
	           bsSIx = 0;
	void       *pTagM;
	Tst_tagMeta_type mttpCur;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			pEdInf == NULL || pMT == NULL)

	pTagM   = st_tagMeta_gs_getRealTag_void(pMT);
	mttpCur = st_tagMeta_gs_getTag_type(pMT);

	switch (mttpCur) {
	case ST_MTAG_TTP_VOR:
		pEdInf->modEmbVorTag = ST_B_TRUE;
		st_tagMeta_gs_getTag_bsIx(pMT, &bsIx, &bsSIx);
		res = AST_MF__cm_updRawVorbTag(pCmdln, pMF,
				(Tst_vorbc_tag*)pTagM, bsIx, bsSIx);
		if (res != ST_ERR_SUCC && res != ST_ERR_ABRT)
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"updating raw vorbis tag failed");
		break;
	default:
		break;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert an ID3v1 tag to any other tag type
 */
Tst_err
ast_mf_cm_convIv1ToX(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tst_id3v1_tag *pTagSrc, const Tst_uint32 uidSrc,
		Tst_tagBasics const *pTBasSrc,
		const Tst_tagMeta_type mttpDest)
{
#	define LOC_FNCN_  __func__

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			 pMF->pFilen == NULL || pTagSrc == NULL)

	return AST_MF__cm_convYtoX(pCmdln, pMF, LOC_FNCN_,
			pTagSrc, uidSrc, pTBasSrc,
			ST_MTAG_TTP_IV1, mttpDest,
			1, 0);
#	undef LOC_FNCN_
}

/*
 * Convert an ID3v2 tag to any other tag type
 */
Tst_err
ast_mf_cm_convIv2ToX(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tst_id3v2_tag *pTagSrc, const Tst_uint32 uidSrc,
		Tst_tagBasics const *pTBasSrc,
		const Tst_tagMeta_type mttpDest)
{
#	define LOC_FNCN_  __func__

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			 pMF->pFilen == NULL || pTagSrc == NULL)

	return AST_MF__cm_convYtoX(pCmdln, pMF, LOC_FNCN_,
			pTagSrc, uidSrc, pTBasSrc,
			ST_MTAG_TTP_IV2, mttpDest,
			1, 0);
#	undef LOC_FNCN_
}

/*
 * Convert an APEv2 tag to any other tag type
 */
Tst_err
ast_mf_cm_convAv2ToX(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tst_apev2_tag *pTagSrc, const Tst_uint32 uidSrc,
		Tst_tagBasics const *pTBasSrc,
		const Tst_tagMeta_type mttpDest)
{
#	define LOC_FNCN_  __func__

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			 pMF->pFilen == NULL || pTagSrc == NULL)

	return AST_MF__cm_convYtoX(pCmdln, pMF, LOC_FNCN_,
			pTagSrc, uidSrc, pTBasSrc,
			ST_MTAG_TTP_AV2, mttpDest,
			1, 0);
#	undef LOC_FNCN_
}

/*
 * Convert an Vorbis tag to any other tag type
 */
Tst_err
ast_mf_cm_convVorToX(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tst_vorbc_tag *pTagSrc, const Tst_uint32 uidSrc,
		Tst_tagBasics const *pTBasSrc,
		const Tst_tagMeta_type mttpDest,
		const Tst_uint32 bsIxSrc, const Tst_uint32 bsSIxSrc)
{
#	define LOC_FNCN_  __func__

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			 pMF->pFilen == NULL || pTagSrc == NULL)

	return AST_MF__cm_convYtoX(pCmdln, pMF, LOC_FNCN_,
			pTagSrc, uidSrc, pTBasSrc,
			ST_MTAG_TTP_VOR, mttpDest,
			bsIxSrc, bsSIxSrc);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Print debugging message for AST_MF__cm_convYtoX()
 */
static void
AST_MF__cm_convYtoX_dmsg(const char *pFnc,
		const Tst_tagMeta_type mttpSrc, const Tst_tagMeta_type mttpDst,
		const Tst_uint32 uidOrg, const Tst_uint32 uidNew,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		const Tst_vorbc_tag *pVorbc)
{
	if (mttpSrc != ST_MTAG_TTP_VOR ||
			(pVorbc != NULL && ! st_vorbc_gs_getTag_wasEmbedded(pVorbc)))
		ast_mf_op_d_deb(pFnc,
				"convert %s to %s Tag (UID 0x%08x --> 0x%08x)...",
				st_tagMeta_fnc_getTagDesc(mttpSrc)[0],
				st_tagMeta_fnc_getTagDesc(mttpDst)[0],
				uidOrg, uidNew);
	else if (mttpSrc == ST_MTAG_TTP_VOR)
		ast_mf_op_d_deb(pFnc,
				"convert %s to %s Tag (UID 0x%08x --> 0x%08x, bs #%u.%u)...",
				st_tagMeta_fnc_getTagDesc(mttpSrc)[0],
				st_tagMeta_fnc_getTagDesc(mttpDst)[0],
				uidOrg, uidNew, bsIx, bsSIx);
}

/*
 * Convert any tag type to any other tag type
 */
static Tst_err
AST_MF__cm_convYtoX(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const char *pFnc,
		void *pTagSrc, const Tst_uint32 uidSrc,
		Tst_tagBasics const *pTBasSrc,
		const Tst_tagMeta_type mttpSrc, Tst_tagMeta_type mttpDest,
		const Tst_uint32 bsIxSrc, const Tst_uint32 bsSIxSrc)
{
#	define LOC_MAPTTP_(mac_mttp, mac_tcvttp)  \
					switch (mac_mttp) { \
					case ST_MTAG_TTP_IV1: mac_tcvttp = ST_TCNV_TTP_IV1; break; \
					case ST_MTAG_TTP_IV2: mac_tcvttp = ST_TCNV_TTP_IV2; break; \
					case ST_MTAG_TTP_AV2: mac_tcvttp = ST_TCNV_TTP_AV2; break; \
					case ST_MTAG_TTP_VOR: mac_tcvttp = ST_TCNV_TTP_VOR; break; \
					default: \
						ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */ \
						res = ST_ERR_FAIL; \
					}
	Tst_err res = ST_ERR_SUCC;
	void    *pTagDst;
	Tst_tagMeta_mt    *pNewMT   = NULL;
	Tst_tagConv_tagTp srcTCVttp = ST_TCNV_TTP_IV1,
	                  dstTCVttp = ST_TCNV_TTP_IV1;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL)

	if (pTagSrc == NULL ||
			pTBasSrc == NULL || ! st_tagBas_gs_getHasTag(pTBasSrc))
		return ST_ERR_SUCC;

	/* add new tag of type mttpDest to array */
	pNewMT = st_tagMeta_addNewToArr(&pMF->tagArr,
			mttpDest, bsIxSrc, bsSIxSrc);
	if (pNewMT == NULL)
		return ST_ERR_FAIL;

	/* print deb message */
	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		AST_MF__cm_convYtoX_dmsg(pFnc,
				mttpSrc, mttpDest,
				uidSrc, st_tagMeta_gs_getTag_uid(pNewMT),
				bsIxSrc, bsSIxSrc,
				(mttpSrc == ST_MTAG_TTP_VOR ? (Tst_vorbc_tag*)pTagSrc : NULL));

	/* map tag type constants */
	LOC_MAPTTP_(mttpSrc, srcTCVttp)
	LOC_MAPTTP_(mttpDest, dstTCVttp)

	/* begin converting */
	pTagDst = st_tagMeta_gs_getRealTag_void(pNewMT);
	res     = st_tagConv_XtoY(pCmdln->optsTagIV2.saveAsVers,
			srcTCVttp, pTagSrc, dstTCVttp, pTagDst);
	if (res != ST_ERR_SUCC)
		ast_mf_op_d_mfErr(pMF, pFnc,
				"converting tag failed");

	/* set original tag type and
	 *   copy tag basics from original tag to new one  */
	if (res == ST_ERR_SUCC && pNewMT != NULL && pTBasSrc != NULL) {
		st_tagMeta_gs_setTag_typeOrg(pNewMT, mttpSrc);
		res = st_tagMeta_gs_setTag_tbasOrg(pNewMT, pTBasSrc);
	}

	return res;
#	undef LOC_MAPTTP_
}

/*----------------------------------------------------------------------------*/

/*
 * Convert all non-ID3v2 Tags into ID3v2 Tags
 *   The old tags will be removed from the tag array
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MF__cm_convAllTagsToIV2(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
#	define LOC_DEBUG_IV1_  0
	const char             *cFNCN   = __func__;
	const Tst_tagMeta_type cMTTPS[] = {ST_MTAG_TTP_IV1, ST_MTAG_TTP_AV2,
	                                   ST_MTAG_TTP_VOR};
	const Tst_uint32       cMTTPS_N = sizeof(cMTTPS) / sizeof(Tst_tagMeta_type);
	Tst_err    res;
	Tst_uint32 x,
	           itUID,
	           itBsIx  = 0,
	           itBsSIx = 0;
	Tst_tagMeta_mt       *pItMT;
	Tst_tagBasics const  *pTBas;
	Tast_cln_t_tagListIx tlix;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL)

	if (pCmdln->cmds.anlz)
		return ST_ERR_SUCC;

	for (x = 0; x < cMTTPS_N; x++) {
		pItMT = NULL;
		itUID = 0;
		/* */
		tlix = ast_mf_fnc_getSrcTLIXfromMTTP(cMTTPS[x]);
		if (! (pCmdln->opts.lstTagSw[tlix] ||
					pCmdln->opts.lstTagSw[AST_CLN_T_TAGLIST_MRG]) &&
				pCmdln->cmds.roTgExtCmds == 0)
			continue;
		if (pCmdln->cmds.roTgExtCmds > 0 &&
				(cMTTPS[x] == ST_MTAG_TTP_IV1 ||
					! (pCmdln->cmds.extrPic || pCmdln->cmds.extrGeo ||
						pCmdln->cmds.extrBinD)))
			continue;
		/* */
		do {
			if (itUID != 0)
				pItMT = NULL;  /* begin with first valid tag again */
			pItMT = st_tagMeta_ite_nextTag_byType(&pMF->tagArr,
					cMTTPS[x], pItMT);
			if (pItMT == NULL) {
				itUID = 0;
				continue;
			}
			pTBas = st_tagMeta_gs_getTag_tbas(pItMT);
			if (pTBas == NULL || ! st_tagBas_gs_getHasTag(pTBas)) {
				itUID = 0;
				continue;
			}
			itUID = st_tagMeta_gs_getTag_uid(pItMT);

			/* convert tag into IV2 */
			st_tagMeta_gs_getTag_bsIx(pItMT, &itBsIx, &itBsSIx);
			res = AST_MF__cm_convYtoX(pCmdln, pMF, cFNCN,
	                    st_tagMeta_gs_getRealTag_void(pItMT),
	                    itUID, pTBas,
	                    cMTTPS[x], ST_MTAG_TTP_IV2,
	                    itBsIx, itBsSIx);
			if (res != ST_ERR_SUCC)
				return res;

			/* remove old tag */
#			if (LOC_DEBUG_IV1_ == 1)
			if (cMTTPS[x] == ST_MTAG_TTP_IV1)
				itUID = 0;  /* --> don't begin while-loop from beginning */
			else
#			endif
			{
				st_tagMeta_removeTagFromArr_byUID(&pMF->tagArr, itUID);
				/**ast_mf_op_prf(" AST_MF__cm_convAllTagsToIV2(): "
						"remove 0x%08x\n", itUID);**/
			}
		} while (pItMT != NULL);
	}

	return ST_ERR_SUCC;
#	undef LOC_DEBUG_IV1_
}

/*----------------------------------------------------------------------------*/

/*
 * Merge all Tags (converted Tags and regular ID3v2 Tags)
 *   The merged tags will remain in the tag array
 */
static Tst_err
AST_MF__cm_mergeTagsIV2_all(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_bool   is1st = ST_B_TRUE;
	char const *pTagD;
	Tst_uint32 mrgUID,
	           curUID;
	Tst_tagMeta_mt   *pMT = NULL;
	Tst_tagMeta_type mttpOrg;
	Tst_id3v2_tag    *pIv2M,
	                 *pIv2C;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL)

	pMT = st_tagMeta_addNewToArr(&pMF->tagArr, ST_MTAG_TTP_MRG, 1, 0);
	if (pMT == NULL)
		return ST_ERR_FAIL;
	pIv2M  = st_tagMeta_gs_getRealTag_iv2(pMT);
	mrgUID = st_tagMeta_gs_getTag_uid(pMT);
	pMT    = NULL;

	while ((pMT = st_tagMeta_ite_nextTag_byType(
				&pMF->tagArr, ST_MTAG_TTP_IV2, pMT)) != NULL) {
		curUID = st_tagMeta_gs_getTag_uid(pMT);
		if (mrgUID == curUID)
			continue;
		mttpOrg = st_tagMeta_gs_getTag_typeOrg(pMT);
		pTagD   = st_tagMeta_fnc_getTagDesc(mttpOrg)[0];
		/* */
		pIv2C = st_tagMeta_gs_getRealTag_iv2(pMT);
		if (pIv2C == NULL ||
				! st_tagBas_gs_getHasFields(st_id3v2_gs_getTag_tbas(pIv2C)))
			continue;
		/* add all fields from (converted) ID3v2 Tag */
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN,
					"adding %s%s Tag... (UID 0x%08x | 0x%08x)",
					(mttpOrg != ST_MTAG_TTP_IV2 ? "converted " : ""),
					pTagD, mrgUID, curUID);
		if (! st_tagBas_gs_getHasTag(st_id3v2_gs_getTag_tbas(pIv2M))) {
			st_id3v2_gs_setTag_majorVersion(pIv2M,
					st_id3v2_gs_getTag_majorVersion(pIv2C));
		}
		res = st_id3v2_mergeTags(pIv2M, pIv2C, ! is1st);
		if (res != ST_ERR_SUCC)
			return res;
		is1st = ST_B_FALSE;
	}

	/* if the master tag is empty, remove it */
	if (! st_tagBas_gs_getHasTag(st_id3v2_gs_getTag_tbas(pIv2M))) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN,
					"removing empty Merged Tag... (UID 0x%08x)",
					mrgUID);
		st_tagMeta_removeTagFromArr_byUID(&pMF->tagArr, mrgUID);
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Set or add field values or attributes
 */
static Tst_err
AST_MF__cm_mergeCmdln_useList(Tast_cln_a *pCmdln,
		const Tst_tagMeta_type mttpCur, void *pTagM,
		Tst_dynlist *pFldDscList, const Tst_bool isFldOrDsc,
		Tst_bool *pEdited)
{
#	define LOC_MAP_TLIX_TO_TN(mac_tlix)  \
				switch (mac_tlix) { \
				case AST_CLN_T_TAGLIST_IV1: pTagNameSh = ST_ID3V1_TAG_NAME_SH; break; \
				case AST_CLN_T_TAGLIST_IV2: pTagNameSh = ST_ID3V2_TAG_NAME_SH; break; \
				case AST_CLN_T_TAGLIST_AV2: pTagNameSh = ST_APEV2_TAG_NAME_SH; break; \
				case AST_CLN_T_TAGLIST_VOR: pTagNameSh = ST_VORBC_TAG_NAME_SH; break; \
				default: pTagNameSh = NULL; \
				}
	Tst_err    res          = ST_ERR_SUCC;
	char const *pTagNameSh;
	Tst_bool   modeAddOrSet = ST_B_TRUE,
	           isOK         = ST_B_FALSE;
	Tst_int32  fnrInt       = -1;
	Tst_uint32 fnrUI32      = 0;
	Tst_str    *pFrIDstr    = NULL;
	Tast_cln_t_fldDscAddSet *pFP;
	Tst_tfldMap_genTagFld   gtf;
	Tst_mtes_string         tmpStr;
	Tst_id3v1_frID          frIDiv1 = ST_ID3V1_FID_NONE;
	Tst_id3v2_frID          frIDiv2 = ST_ID3V2_FID_NONE;
	Tst_apev2_frID          frIDav2 = ST_APEV2_FID_NONE;
	Tst_vorbc_frID          frIDvor = ST_VORBC_FID_NONE;

	if (! st_dynlist_ite_gotoFirst(pFldDscList))
		return ST_ERR_SUCC;

	/* init some vars */
	st_tagFldMap_stc_initGTF(&gtf);
	st_mtes_stc_initStr(&tmpStr);

	/* */
	do {
		pFP = (Tast_cln_t_fldDscAddSet*)st_dynlist_ite_getCurrent(pFldDscList);
		/* */
		LOC_MAP_TLIX_TO_TN(pFP->ttp)
		ST_ASSERTN_NUM(ST_ERR_FAIL, pTagNameSh == NULL)  /* cause exit() here */
		st_tagFldMap_stc_rsetGTF(&gtf);
		/**ast_mf_op_prf("-- %d|%s:%s(.%d) = '%s'|%d\n",
				pFP->ttp, pTagNameSh, pFP->pFldname, pFP->fldnr,
				pFP->pValueStr, pFP->valueInt);**/
		/* set Generic Tag Field */
		if (isFldOrDsc)
			st_tagFldMap_gs_setMode(&gtf, pFP->fldDoAddOrSet);
		else
			st_tagFldMap_gs_setMode(&gtf, /*addOrSet:*/ST_B_FALSE);
		res = st_tagFldMap_gs_setTagType(&gtf, (const Tst_str*)pTagNameSh);
		if (res == ST_ERR_SUCC)
			res = st_tagFldMap_gs_setFldIDstr(&gtf, pFP->pFldname);
		if (res == ST_ERR_SUCC)
			st_tagFldMap_gs_setFldNr(&gtf, pFP->fldnr);
		if (res != ST_ERR_SUCC)
			continue;  /* --> end while() */

		/* copy the string value to tmpStr */
		res = st_mtes_copyFromCharp_isoOrUtf8(pFP->pValueStr,
					pCmdln->opts.isInpISOorU8, &tmpStr);
		if (res != ST_ERR_SUCC)
			continue;  /* --> end while() */

		/* get field ID and ID-String from Generic Tag Field */
		switch (mttpCur) {
		case ST_MTAG_TTP_IV1:
			res = st_tagFldMap_mapToIV1_onlyID(&gtf,
					&frIDiv1, &pFrIDstr, &modeAddOrSet, &fnrInt, &isOK);
			break;
		case ST_MTAG_TTP_IV2:
			res = st_tagFldMap_mapToIV2_onlyID(&gtf,
					&frIDiv2, &pFrIDstr, &modeAddOrSet, &fnrInt, &isOK);
			break;
		case ST_MTAG_TTP_AV2:
			res = st_tagFldMap_mapToAV2_onlyID(&gtf,
					&frIDav2, &pFrIDstr, &modeAddOrSet, &fnrInt, &isOK);
			break;
		case ST_MTAG_TTP_VOR:
			res = st_tagFldMap_mapToVOR_onlyID(&gtf,
					&frIDvor, &pFrIDstr, &modeAddOrSet, &fnrInt, &isOK);
			break;
		default:
			ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
			res = ST_ERR_FAIL;
		}
		if (res != ST_ERR_SUCC || ! isOK)
			continue;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pFrIDstr == NULL)
		fnrUI32 = (fnrInt > 0 ? (Tst_uint32)fnrInt : 0);
		/**ast_mf_op_prf("--> '%s'.%u\n", pFrIDstr, fnrUI32);**/

		/* add/set field to/in tag */
		switch (mttpCur) {
		case ST_MTAG_TTP_IV1:
			if (isFldOrDsc) {
				switch (frIDiv1) {
				case ST_ID3V1_FID_TITL:
				case ST_ID3V1_FID_ALBU:
				case ST_ID3V1_FID_ARTI:
				case ST_ID3V1_FID_COMM:
					res = st_id3v1_fldSet_dataStr((Tst_id3v1_tag*)pTagM,
							frIDiv1, &tmpStr);
					/* */
					*pEdited = ST_B_TRUE;
					break;
				case ST_ID3V1_FID_TRKN:
				case ST_ID3V1_FID_GENR:
				case ST_ID3V1_FID_YEAR:
					if (pFP->isIntStr) {
						res = st_id3v1_fldSet_dataInt((Tst_id3v1_tag*)pTagM,
								frIDiv1, pFP->valueInt);
						/* */
						*pEdited = ST_B_TRUE;
					}
					break;
				default:
					break;
				}
			}
			break;
		case ST_MTAG_TTP_IV2:
			if (isFldOrDsc) {
				if (modeAddOrSet)
					res = st_id3v2_fldAdd_text((Tst_id3v2_tag*)pTagM,
							frIDiv2, pFrIDstr, /*pDesc:*/NULL, &tmpStr);
				else
					res = st_id3v2_fldSet_dataStr((Tst_id3v2_tag*)pTagM,
							frIDiv2, pFrIDstr, fnrUI32, &tmpStr);
			} else {
				res = st_id3v2_fldSet_attrDesc((Tst_id3v2_tag*)pTagM,
						frIDiv2, fnrUI32, &tmpStr);
			}
			/* */
			*pEdited = ST_B_TRUE;
			break;
		case ST_MTAG_TTP_AV2:
			if (isFldOrDsc && frIDav2 == ST_APEV2_FID_GENR)
				res = AST_MF__cm_mergeCmdln_getGenreCap(&tmpStr);
			if (isFldOrDsc && res == ST_ERR_SUCC) {
				if (modeAddOrSet)
					res = st_apev2_fldAdd_text((Tst_apev2_tag*)pTagM,
							frIDav2, pFrIDstr, &tmpStr);
				else
					res = st_apev2_fldSet_dataStr((Tst_apev2_tag*)pTagM,
							frIDav2, pFrIDstr, fnrUI32, &tmpStr);
				/* */
				*pEdited = ST_B_TRUE;
			}
			break;
		case ST_MTAG_TTP_VOR:
			if (isFldOrDsc && frIDvor == ST_VORBC_FID_GENR)
				res = AST_MF__cm_mergeCmdln_getGenreCap(&tmpStr);
			if (isFldOrDsc && res == ST_ERR_SUCC) {
				if (modeAddOrSet)
					res = st_vorbc_fldAdd_text((Tst_vorbc_tag*)pTagM,
							frIDvor, pFrIDstr, &tmpStr);
				else
					res = st_vorbc_fldSet_dataStr((Tst_vorbc_tag*)pTagM,
							frIDvor, pFrIDstr, fnrUI32, &tmpStr);
			} else if (res == ST_ERR_SUCC) {
				res = st_vorbc_fldSet_attrDesc((Tst_vorbc_tag*)pTagM,
						frIDvor, fnrUI32, &tmpStr);
			}
			/* */
			*pEdited = ST_B_TRUE;
			break;
		default:
			break;
		}
	} while (res == ST_ERR_SUCC && st_dynlist_ite_gotoNext(pFldDscList));

	/* free some vars */
	st_tagFldMap_stc_freeGTF(&gtf);
	st_mtes_stc_freeStr(&tmpStr);
	ST_DELPOINT(pFrIDstr)

	return res;
#	undef LOC_MAP_TLIX_TO_TN
}

/*
 * Set field value for field of type Pos, like Tracknr. or Discnr.
 */
static Tst_err
AST_MF__cm_mergeCmdln_allOfTypePos_one(Tast_cln_a *pCmdln,
		const Tst_tagMeta_type mttpCur, void *pTagM,
		const Tst_int32 frID_nr, const Tst_int32 frID_tot,
		const Tst_int32 valNr, const Tst_int32 valTot,
		Tst_bool *pEdited)
{
	Tst_err res = ST_ERR_SUCC;

	switch (mttpCur) {
	case ST_MTAG_TTP_IV1:
		if (valNr >= 0)
			res = st_id3v1_fldSet_dataInt((Tst_id3v1_tag*)pTagM,
					(Tst_id3v1_frID)frID_nr, valNr);
		break;
	case ST_MTAG_TTP_IV2:
		res = st_id3v2_fldSet_dataStr_pos((Tst_id3v2_tag*)pTagM,
				(Tst_id3v2_frID)frID_nr, NULL, 0, valNr, valTot);
		break;
	case ST_MTAG_TTP_AV2:
		if (valNr >= 0)
			res = st_apev2_fldSet_dataStr_int((Tst_apev2_tag*)pTagM,
					(Tst_apev2_frID)frID_nr, NULL, 0, valNr);
		if (res == ST_ERR_SUCC && valTot >= 0)
			res = st_apev2_fldSet_dataStr_int((Tst_apev2_tag*)pTagM,
					(Tst_apev2_frID)frID_tot, NULL, 0, valTot);
		break;
	case ST_MTAG_TTP_VOR:
		if (valNr >= 0)
			res = st_vorbc_fldSet_dataStr_int((Tst_vorbc_tag*)pTagM,
					(Tst_vorbc_frID)frID_nr, NULL, 0, valNr);
		if (res == ST_ERR_SUCC && valTot >= 0)
			res = st_vorbc_fldSet_dataStr_int((Tst_vorbc_tag*)pTagM,
					(Tst_vorbc_frID)frID_tot, NULL, 0, valTot);
		break;
	default:
		res = ST_ERR_IARG;
	}

	return res;
}

/*
 * Set all field values for field of type Pos, like Tracknr. or Discnr.
 */
static Tst_err
AST_MF__cm_mergeCmdln_allOfTypePos(Tast_cln_a *pCmdln,
		const Tst_tagMeta_type mttpCur, void *pTagM,
		Tst_bool *pEdited)
{
#	define LOC_GET_DEST_ID_(mac_dstID, mac_idIV1, mac_idIV2, mac_idAV2, mac_idVOR)  \
				switch (mttpCur) { \
				case ST_MTAG_TTP_IV1: mac_dstID = (Tst_int32)mac_idIV1; break; \
				case ST_MTAG_TTP_IV2: mac_dstID = (Tst_int32)mac_idIV2; break; \
				case ST_MTAG_TTP_AV2: mac_dstID = (Tst_int32)mac_idAV2; break; \
				case ST_MTAG_TTP_VOR: mac_dstID = (Tst_int32)mac_idVOR; break; \
				default: break; \
				}
	Tst_err   res     = ST_ERR_SUCC;
	Tst_int32 fid_nr  = 0,
	          fid_tot = 0;

	if (pCmdln->optsTagFlds.trk >= 0 || pCmdln->optsTagFlds.trkTot >= 0) {
		/* set tracknr. */
		LOC_GET_DEST_ID_(fid_nr,
				ST_ID3V1_FID_TRKN, ST_ID3V2_FID_234_TRCK,
				ST_APEV2_FID_TRKN, ST_VORBC_FID_TRKN)
		LOC_GET_DEST_ID_(fid_tot,
				fid_nr,            fid_nr,
				ST_APEV2_FID_TRKT, ST_VORBC_FID_TRKT)
		if (res == ST_ERR_SUCC)
			res = AST_MF__cm_mergeCmdln_allOfTypePos_one(pCmdln, mttpCur, pTagM,
					fid_nr, fid_tot,
					pCmdln->optsTagFlds.trk, pCmdln->optsTagFlds.trkTot,
					pEdited);
	}
	if (res == ST_ERR_SUCC &&
			(pCmdln->optsTagFlds.disc >= 0 || pCmdln->optsTagFlds.discTot >= 0) &&
			mttpCur != ST_MTAG_TTP_IV1) {
		/* set discnr. */
		LOC_GET_DEST_ID_(fid_nr,
				0,                 ST_ID3V2_FID_234_TPOS,
				ST_APEV2_FID_DSCN, ST_VORBC_FID_DSCN)
		LOC_GET_DEST_ID_(fid_tot,
				0,                 fid_nr,
				ST_APEV2_FID_DSCT, ST_VORBC_FID_DSCT)
		res = AST_MF__cm_mergeCmdln_allOfTypePos_one(pCmdln, mttpCur, pTagM,
				fid_nr, fid_tot,
				pCmdln->optsTagFlds.disc, pCmdln->optsTagFlds.discTot,
				pEdited);
	}

	return res;
#	undef LOC_GET_DEST_ID_
}

/*
 * Merge (non-IV1) Tag with Cmdln
 */
static Tst_err
AST_MF__cm_mergeCmdln(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf,
		const Tst_tagMeta_type mttpCur,
		const Tst_uint32 uidOfDestTag, Tst_uint32 *pNewUID,
		Tst_bool *pEdited)
{
#	define LOC_TAG_SUPPORTS_AF_(mac_supp)  { \
				switch (mttpCur) { \
				case ST_MTAG_TTP_AV2: \
					if (pAF->isPicOrGeo) { \
						if (! pCmdln->opts.quiet) \
							ast_mf_op_prMsg("(APEv2 Tags don't really support pictures, " \
									"skipping file)"); \
						mac_supp = ST_B_FALSE; \
					} \
					break; \
				case ST_MTAG_TTP_VOR: \
					if (! pAF->isPicOrGeo) { \
						if (! pCmdln->opts.quiet) \
							ast_mf_op_prMsg("(Vorbis Tags don't support arbitrary binary data, " \
									"skipping file)"); \
						mac_supp = ST_B_FALSE; \
					} \
					break; \
				default: \
					break; \
				} \
				}
	const char *cFNCN = __func__;
	Tst_err       res    = ST_ERR_SUCC;
	Tst_uint32    x;
	Tst_str const *pFnTag;
	void          *pTagM = NULL;
	Tst_tagMeta_mt      *pMT;
	Tst_tagBasics const *pTBasM;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			 pMF->pFilen == NULL || pEdInf == NULL ||
			 pNewUID == NULL || pEdited == NULL)

	*pEdited = ST_B_FALSE;

	/* get tag to merge cmdln with */
	pMT = AST_MF__cm_mergeCmdln_getTag(pCmdln, pMF, pEdInf,
			uidOfDestTag, mttpCur);
	if (pMT == NULL)
		return ST_ERR_FAIL;
	*pNewUID = st_tagMeta_gs_getTag_uid(pMT);
	pFnTag   = st_tagMeta_gs_getTag_filen(pMT);
	pTBasM   = st_tagMeta_gs_getTag_tbas(pMT);
	pTagM    = st_tagMeta_gs_getRealTag_void(pMT);

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN,
				"starting to merge %s Tag with cmdline... (UID 0x%08x)",
				st_tagMeta_fnc_getTagDesc(mttpCur)[0], *pNewUID);

	switch (mttpCur) {
	case ST_MTAG_TTP_IV1:
	case ST_MTAG_TTP_AV2:
	case ST_MTAG_TTP_VOR:
		break;
	case ST_MTAG_TTP_IV2:
		if (! st_tagBas_gs_getHasTag(pTBasM))
			st_id3v2_gs_setTag_majorVersion((Tst_id3v2_tag*)pTagM,
					pCmdln->optsTagIV2.saveAsVers);
		break;
	default:
		ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
		return ST_ERR_FAIL;
	}

	/* do the merging */
	/** using dsetListPrio1 */
	res = AST_MF__cm_mergeCmdln_useList(pCmdln, mttpCur, pTagM,
			&pCmdln->optsTagFlds.dsetListPrio1, /*isFldOrDsc:*/ST_B_FALSE,
			pEdited);
	if (res != ST_ERR_SUCC)
		return res;
    /** using faddsetListPrio2 (first set and then add) */
	res = AST_MF__cm_mergeCmdln_useList(pCmdln, mttpCur, pTagM,
			&pCmdln->optsTagFlds.faddsetListPrio2, /*isFldOrDsc:*/ST_B_TRUE,
			pEdited);
	if (res != ST_ERR_SUCC)
		return res;
	/** using fsetListPrio3 */
	res = AST_MF__cm_mergeCmdln_useList(pCmdln, mttpCur, pTagM,
			&pCmdln->optsTagFlds.fsetListPrio3, /*isFldOrDsc:*/ST_B_TRUE,
			pEdited);
	if (res != ST_ERR_SUCC)
		return res;

	/** priority 4, Tracknr. / Discnr. */
	res = AST_MF__cm_mergeCmdln_allOfTypePos(pCmdln, mttpCur, pTagM, pEdited);
	if (res != ST_ERR_SUCC)
		return res;

	/** priority 4, Picture / GEOB (binary fields) */
	/*** modify */
	if (pCmdln->optsTagFlds.attFileArr.cnt > 0) {
		Tst_bool modAF;
		Tast_cln_attFileArr *pAFarr;
		Tast_cln_attFile    *pAF;

		pAFarr = &pCmdln->optsTagFlds.attFileArr;
		for (x = 0; x < pAFarr->cnt; x++) {
			pAF = (Tast_cln_attFile*)&pAFarr->pArr[x];
			if (! pAF->isOK || pAF->addOrMod)
				continue;
			/* */
			modAF = ST_B_TRUE;
			LOC_TAG_SUPPORTS_AF_(modAF)
			if (! modAF)
				continue;
			res = AST_MF__cm_mergeCmdln_addOrModFld_file(pCmdln, pFnTag, mttpCur,
					pTagM, /*addOrMod:*/ST_B_FALSE, pAF, pEdited);
			if (res != ST_ERR_SUCC)
				return res;
		}
	}
	/*** remove */
	if (pCmdln->optsTagFlds.pLstRemPic &&
			(mttpCur == ST_MTAG_TTP_IV2 ||
				mttpCur == ST_MTAG_TTP_VOR) &&
			st_tagBas_gs_getHasTag(pTBasM)) {
		res = AST_MF__cm_mergeCmdln_remPictOrGeob(pCmdln, mttpCur,
				pTagM, /*remPicOrGeo:*/ST_B_TRUE,
				pCmdln->optsTagFlds.remPicAll, pCmdln->optsTagFlds.pLstRemPic,
				pEdited);
		if (res != ST_ERR_SUCC)
			return res;
	}
	if (pCmdln->optsTagFlds.pLstRemGeo &&
			(mttpCur == ST_MTAG_TTP_IV2 ||
				mttpCur == ST_MTAG_TTP_AV2) &&
			st_tagBas_gs_getHasTag(pTBasM)) {
		res = AST_MF__cm_mergeCmdln_remPictOrGeob(pCmdln, mttpCur,
				pTagM, /*remPicOrGeo:*/ST_B_FALSE,
				pCmdln->optsTagFlds.remGeoAll, pCmdln->optsTagFlds.pLstRemGeo,
				pEdited);
		if (res != ST_ERR_SUCC)
			return res;
	}
	/*** add */
	if (pCmdln->optsTagFlds.attFileArr.cnt > 0) {
		Tst_bool addAF;
		Tast_cln_attFileArr *pAFarr;
		Tast_cln_attFile    *pAF;

		pAFarr = &pCmdln->optsTagFlds.attFileArr;
		for (x = 0; x < pAFarr->cnt; x++) {
			pAF = (Tast_cln_attFile*)&pAFarr->pArr[x];
			if (! pAF->isOK || ! pAF->addOrMod)
				continue;
			/* */
			addAF = ST_B_TRUE;
			LOC_TAG_SUPPORTS_AF_(addAF)
			if (! addAF)
				continue;
			res = AST_MF__cm_mergeCmdln_addOrModFld_file(pCmdln, pFnTag, mttpCur,
					pTagM, /*addOrMod:*/ST_B_TRUE, pAF, pEdited);
			if (res != ST_ERR_SUCC)
				return res;
		}
	}

	/* tidy up */
	if (res == ST_ERR_SUCC) {
		switch (mttpCur) {
		case ST_MTAG_TTP_IV1:
			res = st_id3v1_tidyUpTag((Tst_id3v1_tag*)pTagM);
			break;
		case ST_MTAG_TTP_IV2:
			res = st_id3v2_tidyUpTag((Tst_id3v2_tag*)pTagM, ST_B_TRUE);
			break;
		case ST_MTAG_TTP_AV2:
			res = st_apev2_tidyUpTag((Tst_apev2_tag*)pTagM);
			break;
		case ST_MTAG_TTP_VOR:
			res = st_vorbc_tidyUpTag((Tst_vorbc_tag*)pTagM);
			break;
		default:
			break;
		}
	}

	if (res == ST_ERR_SUCC && ! st_tagBas_gs_getHasFields(pTBasM)) {
		if (st_tagBas_gs_getHasTag(pTBasM))
			*pEdited = ST_B_TRUE;
		else {
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN,
						"removing empty %stag... (UID 0x%08x)",
						(st_tagBas_gs_getHasTag(pTBasM) ? "" : "new "),
						*pNewUID);
			st_tagMeta_removeTagFromArr_byUID(&pMF->tagArr, *pNewUID);
		}
		*pNewUID = 0;
	}

	/* update raw vorbis tag */
	if (res == ST_ERR_SUCC && *pEdited &&
			mttpCur == ST_MTAG_TTP_VOR && pNewUID != 0) {
		if (st_vorbc_gs_getTag_wasEmbedded((Tst_vorbc_tag*)pTagM))
			res = ast_mf_cm_updRawTag(pCmdln, pMF, pEdInf, pMT);
	}

	return res;
#	undef LOC_TAG_SUPPORTS_AF_
}

/*----------------------------------------------------------------------------*/

/*
 * Get tag to merge cmdln with
 */
static Tst_tagMeta_mt*
AST_MF__cm_mergeCmdln_getTag(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf_editInfo *pEdInf,
		const Tst_uint32 uidOfDestTag, const Tst_tagMeta_type mttpDest)
{
	Tst_tagMeta_mt      *pMT = NULL;
	Tst_tagBasics const *pTBas;

	ST_ASSERTN_NULL(pCmdln == NULL || pMF == NULL || pEdInf == NULL)

	if (uidOfDestTag == 0) {
		while ((pMT = st_tagMeta_ite_nextTag_byType(
					&pMF->tagArr, mttpDest, pMT)) != NULL) {
			pTBas = st_tagMeta_gs_getTag_tbas(pMT);
			if (pTBas == NULL || ! st_tagBas_gs_getHasTag(pTBas) ||
					ast_mf_stc_editInfo_isUIDinObs(pEdInf,
							st_tagMeta_gs_getTag_uid(pMT)))
				continue;
			break;
		}
	} else if (! ast_mf_stc_editInfo_isUIDinObs(pEdInf, uidOfDestTag)) {
		pMT = st_tagMeta_gs_getArr_tag_byUID(&pMF->tagArr, uidOfDestTag);
		if (pMT != NULL) {
			pTBas = st_tagMeta_gs_getTag_tbas(pMT);
			if (pTBas == NULL || ! st_tagBas_gs_getHasTag(pTBas))
				pMT = NULL;
		}
	}
	/* */
	if (pMT == NULL) {
		/* create new tag */
		pMT = st_tagMeta_addNewToArr(&pMF->tagArr, mttpDest, 1, 0);
	}
	return pMT;
}

/*----------------------------------------------------------------------------*/

static Tst_err
AST_MF__cm_mergeCmdln_getGenreCap(Tst_mtes_string *pGenreStr)
{
	Tst_err res;
	Tst_mtes_string capOut;

	if (st_mtes_strlenCh(pGenreStr) == 0)
		return ST_ERR_SUCC;

	res = st_mtes_stc_initStr(&capOut);
	if (res != ST_ERR_SUCC)
		return res;

	/* convert "(9)Metal" to "Metal" */
	res = st_tagCFnc_chk_getGenreCap(pGenreStr, &capOut, ST_MTES_TE_UTF8);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strCpy(&capOut, pGenreStr);

	st_mtes_stc_freeStr(&capOut);
	return res;
}

/*----------------------------------------------------------------------------*/

static Tst_err
AST_MF__cm_mergeCmdln_remField(const Tast_cln_a *pCmdln,
		const Tst_tagMeta_type mttpRCur,
		void *pTag, const Tst_int32 frID,
		const Tst_bool remAll, const Tst_uint32 *pRemLst)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 x   = 0;

	if (remAll) {
		switch (mttpRCur) {
		case ST_MTAG_TTP_IV2:
			res = st_id3v2_disableFld_byID_all((Tst_id3v2_tag*)pTag,
					(const Tst_id3v2_frID)frID, NULL);
			break;
		case ST_MTAG_TTP_AV2:
			res = st_apev2_disableFld_byID_all((Tst_apev2_tag*)pTag,
					(const Tst_apev2_frID)frID, NULL);
			break;
		case ST_MTAG_TTP_VOR:
			res = st_vorbc_disableFld_byID_all((Tst_vorbc_tag*)pTag,
					(const Tst_vorbc_frID)frID, NULL);
			break;
		default:
			break;
		}
	} else {
		while (pRemLst[x] != 0) {
			switch (mttpRCur) {
			case ST_MTAG_TTP_IV2:
				res = st_id3v2_disableFld_byID((Tst_id3v2_tag*)pTag,
						(const Tst_id3v2_frID)frID, NULL, pRemLst[x]);
				break;
			case ST_MTAG_TTP_AV2:
				res = st_apev2_disableFld_byID((Tst_apev2_tag*)pTag,
						(const Tst_apev2_frID)frID, NULL, pRemLst[x]);
				break;
			case ST_MTAG_TTP_VOR:
				res = st_vorbc_disableFld_byID((Tst_vorbc_tag*)pTag,
						(const Tst_vorbc_frID)frID, NULL, pRemLst[x]);
				break;
			default:
				break;
			}
			if (res != ST_ERR_SUCC)
				break;
			++x;
		}
	}
	return res;
}

static Tst_err
AST_MF__cm_mergeCmdln_remPictOrGeob(const Tast_cln_a *pCmdln,
		const Tst_tagMeta_type mttpRCur,
		void *pTag, const Tst_bool remPicOrGeo,
		const Tst_bool remAll, const Tst_uint32 *pRemLst,
		Tst_bool *pEdited)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 x;
	Tst_int32  dstID  = -1,
	           dstFTP = -1;

	switch (mttpRCur) {
	case ST_MTAG_TTP_IV2:
		dstID = (Tst_int32)(remPicOrGeo ? ST_ID3V2_FID_234_APIC : ST_ID3V2_FID_234_GEOB);
		break;
	case ST_MTAG_TTP_AV2:
		if (remPicOrGeo)
			return ST_ERR_SUCC;
		dstFTP = (Tst_int32)ST_APEV2_FTP_BDAT;
		break;
	case ST_MTAG_TTP_VOR:
		if (! remPicOrGeo)
			return ST_ERR_SUCC;
		dstID = (Tst_int32)ST_VORBC_FID_MBPI;
		break;
	default:
		break;
	}

	switch (mttpRCur) {
	case ST_MTAG_TTP_IV2:
	case ST_MTAG_TTP_VOR:
		if (dstID >= 0) {
			res = AST_MF__cm_mergeCmdln_remField(pCmdln, mttpRCur,
					pTag, dstID, remAll, pRemLst);
			/* */
			*pEdited = (res == ST_ERR_SUCC);
		}
		break;
	case ST_MTAG_TTP_AV2:
		if (dstFTP >= 0) {
			if (remAll) {
				res = st_apev2_disableFld_byFTP_all((Tst_apev2_tag*)pTag,
						(const Tst_apev2_frTp)dstFTP);
				/* */
				*pEdited = (res == ST_ERR_SUCC);
			} else {
				x = 0;
				while (pRemLst[x] != 0) {
					res = st_apev2_disableFld_byFTP((Tst_apev2_tag*)pTag,
							(const Tst_apev2_frTp)dstFTP, pRemLst[x]);
					if (res != ST_ERR_SUCC)
						break;
					++x;
					/* */
					*pEdited = ST_B_TRUE;
				}
			}
		}
		break;
	default:
		break;
	}

	return res;
}

static Tst_err
AST_MF__cm_mergeCmdln_addOrModFld_file(const Tast_cln_a *pCmdln,
		const Tst_str *pFnTag, const Tst_tagMeta_type mttpRCur,
		void *pTag,
		const Tst_bool addOrMod, Tast_cln_attFile *pAF,
		Tst_bool *pEdited)
{
	const char *cFNCN = __func__;
	Tst_err       res   = ST_ERR_SUCC;
	Tst_str const *pMim = NULL;
	Tst_mtes_string tmpFn,
	                tmpDesc,
	                tmpMime,
	                *pDsc = NULL;

	ST_ASSERTN_IARG(pAF == NULL || pAF->addOrMod != addOrMod)

	if (pAF->pFn != NULL && ! st_sysDoesFileExist(pAF->pFn)) {
		ast_mf_op_d_tagErr(pFnTag, cFNCN,
				"file \"%s\" doesn't exist", pAF->pFn);
		return ST_ERR_FAIL;
	}

	st_mtes_stc_initStr(&tmpFn);
	st_mtes_stc_initStr(&tmpDesc);
	st_mtes_stc_initStr(&tmpMime);

	/* get filename/description/mime */
	/** get filename --> tmpFn */
	if (pAF->pFn != NULL)
		res = st_mtes_copyFromCharp_isoOrUtf8(pAF->pFn,
				pCmdln->opts.isInpISOorU8, &tmpFn);
	/** get description --> pDsc */
	if (res == ST_ERR_SUCC && pAF->pDesc != NULL) {
		res = st_mtes_copyFromCharp_isoOrUtf8(pAF->pDesc,
				pCmdln->opts.isInpISOorU8, &tmpDesc);
		if (res == ST_ERR_SUCC)
			pDsc = &tmpDesc;
	}
	/** get mime --> pMim */
	if (res == ST_ERR_SUCC && pAF->pMime != NULL) {
		res = st_mtes_copyFromCharp_isoOrUtf8(pAF->pMime,
				pCmdln->opts.isInpISOorU8, &tmpMime);
		if (res == ST_ERR_SUCC &&
				st_mtes_getEncodingCur(&tmpMime) != ST_MTES_TE_ISO)
			res = st_mtes_convStr(&tmpMime, ST_MTES_TE_ISO);
		if (res == ST_ERR_SUCC)
			pMim = st_mtes_getDataPt(&tmpMime);
	}

	/* add file to tag or modify file in tag */
	if (res == ST_ERR_SUCC) {
		if (pAF->isPicOrGeo) {
			switch (mttpRCur) {
			case ST_MTAG_TTP_IV2:
				if (addOrMod) {
					res = st_id3v2_fldAdd_apic_rdFromFile((Tst_id3v2_tag*)pTag,
							pDsc, pAF->picTp, pAF->picFm, &tmpFn);
					/* */
					*pEdited = (res == ST_ERR_SUCC);
				} else {
					if (pAF->pFn != NULL && pAF->modFNr > 0) {
						/* replace file */
						res = st_id3v2_fldSet_dataBin_rdFromFile((Tst_id3v2_tag*)pTag,
								ST_ID3V2_FID_234_APIC, pAF->modFNr, &tmpFn);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->pDesc != NULL && pDsc != NULL) {
						/* replace description */
						res = st_id3v2_fldSet_attrDesc((Tst_id3v2_tag*)pTag,
								ST_ID3V2_FID_234_APIC, pAF->modFNr, pDsc);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->isPTPset) {
						/* replace picture type */
						res = st_id3v2_fldSet_attrPicTp((Tst_id3v2_tag*)pTag,
								ST_ID3V2_FID_234_APIC, pAF->modFNr, pAF->picTp);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->isPFMset) {
						/* replace picture format */
						res = st_id3v2_fldSet_attrPicFmt((Tst_id3v2_tag*)pTag,
								ST_ID3V2_FID_234_APIC, pAF->modFNr, pAF->picFm);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
				}
				break;
			case ST_MTAG_TTP_VOR:
				if (addOrMod) {
					res = st_vorbc_fldAdd_pict_rdFromFile((Tst_vorbc_tag*)pTag,
							pDsc, (Tst_vorbc_picTp)pAF->picTp, pAF->picFm, &tmpFn);
					/* */
					*pEdited = (res == ST_ERR_SUCC);
				} else {
					if (pAF->pFn != NULL && pAF->modFNr > 0) {
						/* replace file */
						res = st_vorbc_fldSet_dataBin_rdFromFile((Tst_vorbc_tag*)pTag,
								ST_VORBC_FID_MBPI, pAF->modFNr, &tmpFn);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->pDesc != NULL && pDsc != NULL) {
						/* replace description */
						res = st_vorbc_fldSet_attrDesc((Tst_vorbc_tag*)pTag,
								ST_VORBC_FID_MBPI, pAF->modFNr, pDsc);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->isPTPset) {
						/* replace picture type */
						res = st_vorbc_fldSet_attrPicTp(
								(Tst_vorbc_tag*)pTag,
								ST_VORBC_FID_MBPI,
								pAF->modFNr,
								st_tagCFnc_convId3v2ToVorbc_picTp(pAF->picTp)
							);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->isPFMset) {
						/* replace picture format */
						res = st_vorbc_fldSet_attrPicFmt((Tst_vorbc_tag*)pTag,
								ST_VORBC_FID_MBPI, pAF->modFNr, pAF->picFm);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
				}
				break;
			default:
				break;
			}
		} else {
			switch (mttpRCur) {
			case ST_MTAG_TTP_IV2:
				if (addOrMod) {
					res = st_id3v2_fldAdd_geob_rdFromFile((Tst_id3v2_tag*)pTag,
							pDsc, pMim, &tmpFn);
					/* */
					*pEdited = (res == ST_ERR_SUCC);
				} else {
					if (pAF->pFn != NULL && pAF->modFNr > 0) {
						/* replace file */
						res = st_id3v2_fldSet_dataBin_rdFromFile((Tst_id3v2_tag*)pTag,
								ST_ID3V2_FID_234_GEOB, pAF->modFNr, &tmpFn);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->pDesc != NULL && pDsc != NULL) {
						/* replace description */
						res = st_id3v2_fldSet_attrDesc((Tst_id3v2_tag*)pTag,
								ST_ID3V2_FID_234_GEOB, pAF->modFNr, pDsc);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
					if (pAF->pMime != NULL && pMim != NULL) {
						/* replace mime-string */
						res = st_id3v2_fldSet_attrMime((Tst_id3v2_tag*)pTag,
								ST_ID3V2_FID_234_GEOB, pAF->modFNr, pMim);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
				}
				break;
			case ST_MTAG_TTP_AV2:
				if (addOrMod) {
					res = st_apev2_fldAdd_binary_rdFromFile((Tst_apev2_tag*)pTag,
							(const Tst_str*)"GEOB", &tmpFn);
					/* */
					*pEdited = (res == ST_ERR_SUCC);
				} else {
					if (pAF->pFn != NULL && pAF->modFNr > 0) {
						/* replace file */
						res = st_apev2_fldSet_dataBin_rdFromFile((Tst_apev2_tag*)pTag,
								pAF->modFNr, &tmpFn);
						/* */
						*pEdited = (res == ST_ERR_SUCC);
					}
				}
				break;
			default:
				break;
			}
		}
		if (res != ST_ERR_SUCC) {
			switch (res) {
			case ST_ERR_PIFM:
			case ST_ERR_PITP:
				ast_mf_op_d_tagErr(pFnTag, cFNCN,
						"invalid picture %s found ('%s'), aborting",
						(res == ST_ERR_PIFM ? "format" : "type"), pAF->pFn);
				res = ST_ERR_ABRT;
				break;
			case ST_ERR_ABRT:
				break;
			default:
				ast_mf_op_d_tagErr(pFnTag, cFNCN,
						"adding file to tag failed");
			}
		}
	}

	st_mtes_stc_freeStr(&tmpFn);
	st_mtes_stc_freeStr(&tmpDesc);
	st_mtes_stc_freeStr(&tmpMime);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Update the raw vorbis tag as well in the tag itself as in
 *   the OGG stream the tag came from
 */
static Tst_err
AST_MF__cm_updRawVorbTag(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tst_vorbc_tag *pTag,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx)
{
	const char *cFNCN = __func__;
	Tst_err res;
	Tst_contOgg_substream *pBS;

	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
		ast_mf_op_d_deb(cFNCN,
				"updating raw Vorbis tag... (bs#%u.%u)",
				bsIx, bsSIx);
	res = st_vorbc_updateRawTag(pTag);
	if (res != ST_ERR_SUCC)
		return res;

	if (st_vorbc_gs_getTag_wasEmbedded(pTag)) {
		pBS = st_contOgg_gs_getStream_byGrpIx(&pMF->avOgg, bsIx, bsSIx);
		if (pBS == NULL) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"couldn't find associated OGG stream");
			res = ST_ERR_FAIL;
		} else {
			res = st_contOgg_vorbComm_gs_setRawCommentData(pBS,
					st_vorbc_gs_getRawTagPointer(pTag));
			if (res != ST_ERR_SUCC)
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"couldn't replace raw tag in OGG stream");
		}
	}

	return res;
}

/******************************************************************************/
