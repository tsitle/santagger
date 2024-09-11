/******************************************************************************/
/* mf_show_tiv2.c                 [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for displaying ID3v2 Tags                                        */
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
#include "src/includes/common/binobj.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_meta.h"
/** */
#define SRC_MF_SHOW_TIV2_C
#include "mf_show_tiv2-pp.h"
/*** */
#include "mf_show_av-pp.h"
#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#undef SRC_MF_SHOW_TIV2_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <ctype.h>       /* isprint() */
#include <stdlib.h>      /* calloc(), free(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Print one (converted) ID3v2 tag
 */
Tst_err
ast_mf_sw_showTagIV2(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		Tst_id3v2_tag *pTag,
		const Tst_tagMeta_type mtTypeOrg,
		const Tst_bool onlyHdLn,
		const Tast_cln_t_tagList_pb(pMergedLst))
{
	const char *cFNCN = __FUNCTION__;
	char const  **ppTagD,
	            *pTagDtmp;
	Tst_uint32  x,
	            delML     = 0;
	Tst_bool    printed   = ST_B_FALSE,
	            mrgLstOne = ST_B_FALSE,
	            is1Grp    = ST_B_FALSE;  /* for vorbis */
	Tst_id3v2_fldData *pItFld = NULL;
	Tst_id3v2_fldProp fldPr;
	Tast_mf__prnfld   optsPF;

	ST_ASSERTN_IARG(pCmdln == NULL || pTag == NULL)

	ppTagD = st_tagMeta_fnc_getTagDesc(mtTypeOrg);
	st_id3v2_stc_rsetFPr(&fldPr);

	AST_MF__sw_stc_rsetOPF(&optsPF,
			&pCmdln->opts.disp, pCmdln->opts.basOpts.verb);
	optsPF.prScd     = (mtTypeOrg != ST_MTAG_TTP_IV1);
	optsPF.hasTagPre = ST_B_TRUE;
	while ((pItFld = st_id3v2_ite_nextFld(pTag, pItFld)) != NULL)
		st_id3v2_gs_setField_wasDisplayed(pItFld, ST_B_FALSE);

	/* TAG NAME, VERSION and VALID FLD COUNT */
	ast_mf_op_prFInf("TAG ");
	ast_mf_op_prFInf("%s (%s", ppTagD[1], ppTagD[0]);
	switch (mtTypeOrg) {
	case ST_MTAG_TTP_IV1:
	case ST_MTAG_TTP_AV2:
		break;
	case ST_MTAG_TTP_VOR:
		if (AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_AUD) &&
				st_contOgg_gs_getStreamCount(&pMF->avOgg) > 1) {
			ast_mf_op_prFInf(", ");
			ast_mf_sw_printOGGstrNr(&pMF->avOgg, bsIx, bsSIx,
					/*prForAudioOrTag:*/ST_B_FALSE, &is1Grp, &delML);
		}
		break;
	case ST_MTAG_TTP_IV2:
		if (st_tagBas_gs_getHasTag(st_id3v2_gs_getTag_tbas(pTag))) {
			ast_mf_op_prFInf(".%u", st_id3v2_gs_getTag_majorVersion(pTag));
			if (st_id3v2_gs_getTag_minorVersion(pTag) > 0)
				ast_mf_op_prFInf(".%u", st_id3v2_gs_getTag_minorVersion(pTag));
		}
		break;
	case ST_MTAG_TTP_MRG:
		ast_mf_op_prFInf(" ");
		for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++) {
			if (! pMergedLst[x])
				continue;
			pTagDtmp = st_tagMeta_fnc_getTagDesc(
					ast_mf_fnc_mapTLIXtoMTTP((Tast_cln_t_tagListIx)x))[1];
			if (mrgLstOne)
				ast_mf_op_prFInf(",");
			ast_mf_op_prFInf("%s", pTagDtmp);
			mrgLstOne = ST_B_TRUE;
		}
		if (! mrgLstOne)
			ast_mf_op_prFInf("none");
		break;
	default:
		ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
		break;
	}
	x = st_id3v2_gs_getTag_amountValidFlds_all(pTag);
	if (st_tagBas_gs_getHasTag(st_id3v2_gs_getTag_tbas(pTag)))
		ast_mf_op_prFInf(": %u valid field%s", x, (x == 1 ? "" : "s"));
	else
		ast_mf_op_prFInf(": Tag not present");
	ast_mf_op_prFInf(")\n");
	if (onlyHdLn || ! st_tagBas_gs_getHasTag(st_id3v2_gs_getTag_tbas(pTag)))
		return ST_ERR_SUCC;

	/* tagger info */
	if (pCmdln->opts.disp.tagger)
		AST_MF__sw_taggerInfo(pCmdln->opts.disp.asISOorU8, pTag, ppTagD[1]);

	/* ARTIST */
	optsPF.frID = ST_ID3V2_FID_234_TPE1;
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Artist", &printed);

	/* ALBUM */
	optsPF.frID = ST_ID3V2_FID_234_TALB;
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Album", &printed);

	/* TITLE */
	optsPF.frID = ST_ID3V2_FID_234_TIT2;
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Title", &printed);

	/* TRACKNR */
	optsPF.frID = ST_ID3V2_FID_234_TRCK;
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Track", &printed);

	/* DISCNR */
	if (mtTypeOrg != ST_MTAG_TTP_IV1) {
		optsPF.frID = ST_ID3V2_FID_234_TPOS;
		AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Disc", &printed);
	}

	/* YEAR */
	optsPF.hasAltern = ST_B_TRUE;
	optsPF.frID      = ST_ID3V2_FID_23__TYER;  /* year */
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Year", &printed);
	if (! printed) {
		optsPF.prOnlyYr = ST_B_TRUE;
		optsPF.frID     = ST_ID3V2_FID___4_TDRL;  /* release time */
		AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Year", &printed);
		if (! printed) {
			optsPF.hasAltern = ST_B_FALSE;
			optsPF.frID      = ST_ID3V2_FID___4_TDRC;  /* recording time */
			AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Year", &printed);
		}
	}
	optsPF.prOnlyYr  = ST_B_FALSE;
	optsPF.hasAltern = ST_B_FALSE;

	/* GENRE */
	optsPF.frID = ST_ID3V2_FID_234_TCON;
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Genre", &printed);

	/* COMMENT */
	optsPF.frID = ST_ID3V2_FID_234_COMM;
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Comment", &printed);

	if (mtTypeOrg == ST_MTAG_TTP_IV1)
		return ST_ERR_SUCC;

	/* PICTURE */
	optsPF.frID = ST_ID3V2_FID_234_APIC;
	AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1], "Picture", &printed);

	if (optsPF.pOptsD->allFlds) {
		/* show remaining fields */
		optsPF.isExtra = ST_B_TRUE;
		pItFld         = NULL;
		while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
			if (st_id3v2_gs_getField_wasDisplayed(pItFld))
				continue;
			st_id3v2_gs_getField_props(pItFld, &fldPr);
			optsPF.frID     = st_id3v2_gs_getFieldProp_id(&fldPr);
			optsPF.pFrIDstr = st_id3v2_gs_getFieldProp_idStr(&fldPr);
			AST_MF__sw_printFld_sup(cFNCN, &optsPF, pTag, ppTagD[1],
					(const char*)st_id3v2_gs_getFieldProp_caption(&fldPr),
					&printed);
		}
	}

	if (! optsPF.pOptsD->allFlds) {
		Tst_uint32 udd = 0;

		/* count fields that have not been displayed yet */
		pItFld = NULL;
		while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL)
			if (! st_id3v2_gs_getField_wasDisplayed(pItFld))
				++udd;
		if (udd > 0) {
			ast_mf_op_prFInf("TAG %s (%u field%s not (fully) displayed)\n",
					ppTagD[1], udd, (udd == 1 ? "" : "s"));
		}
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tast_mf__prnfld */
static void
AST_MF__sw_stc_rsetOPF(Tast_mf__prnfld *pPF,
		const Tast_cln_optsDisp *pOptsDisp, const int verb)
{
	ST_ASSERTN_VOID(pPF == NULL || pOptsDisp == NULL)

	pPF->hasTagPre = ST_B_FALSE;
	pPF->hasAltern = ST_B_FALSE;
	pPF->prOnlyYr  = ST_B_FALSE;
	pPF->prScd     = ST_B_FALSE;
	pPF->isExtra   = ST_B_FALSE;
	pPF->frID      = ST_ID3V2_FID_NONE;
	pPF->pFrIDstr  = NULL;
	/* */
	pPF->pOptsD    = pOptsDisp;
	pPF->verb      = verb;
}

/* Tast_mf__prnfldsub */
static void
AST_MF__sw_stc_rsetOPFS(Tast_mf__prnfldsub *pPFS,
		const Tast_cln_optsDisp *pOptsDisp)
{
	ST_ASSERTN_VOID(pPFS == NULL)

	st_id3v2_stc_rsetStrTp(&pPFS->strTp);
	pPFS->rwoTp       = ST_ID3V2_TFLD_RWO_TP_NONE;
	pPFS->six         = 0;
	pPFS->stot        = 0;
	pPFS->fnr         = 0;
	pPFS->ftot        = 0;
	pPFS->preambelLen = 0;
	pPFS->printPreamb = ST_B_FALSE;
	pPFS->dispFStrOW  = ST_B_FALSE;
	pPFS->prOnlyYr    = ST_B_FALSE;
	pPFS->isNumeroid  = ST_B_FALSE;
	pPFS->pendingNL   = ST_B_TRUE;
	/* */
	pPFS->pOptsD      = pOptsDisp;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_MF__sw_taggerInfo(const Tst_bool dispAsISOorU8,
		Tst_id3v2_tag *pTag, const char *pTagName)
{
	Tst_err res;
	Tst_str *pTaInCh = NULL;
	Tst_mtes_string taIn;

	st_mtes_stc_initStr(&taIn);
	res = st_id3v2_gs_getTag_taggerInfo(pTag, &taIn);
	if (res != ST_ERR_SUCC)
		st_sysStrcpy2("-ERROR-", &pTaInCh);
	else if (! st_mtes_isStrEmptyBy(&taIn)) {
		if (dispAsISOorU8)
			res = st_mtes_copyToCharp_iso(&taIn, &pTaInCh);
		else
			res = st_mtes_copyToCharp_utf8(&taIn, &pTaInCh);
		if (res != ST_ERR_SUCC)
			st_sysStrcpy2("-ERROR-", &pTaInCh);
	}
	st_mtes_stc_freeStr(&taIn);

	if (! st_sysStrEmpty(pTaInCh)) {
		char msg[256];

		snprintf(msg, sizeof(msg),
				"TAG %s%s", (pTagName ? pTagName : ""), (pTagName ? " " : ""));

		ast_mf_op_prFInf("%s(TaggerInfo: %s)\n",
				msg, (pTaInCh != NULL ? (char*)pTaInCh : ""));
	}

	ST_DELPOINT(pTaInCh)
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Master functions for printing all tag fields of one kind
 *   All fields with the ID pOptsPF->frID will be printed
 */
static void
AST_MF__sw_printFld_sup(const char *pFnc, const Tast_mf__prnfld *pOptsPF,
		Tst_id3v2_tag *pTag, const char *pTagName, const char *pFldName,
		Tst_bool *pPrinted)
{
	char       msg[256],
	           fldnX[16];
	Tst_uint32 cntPrintedChars,
	           slen;

	*pPrinted = ST_B_FALSE;
	if (st_id3v2_gs_getTag_amountValidFlds_byID(pTag,
				pOptsPF->frID, pOptsPF->pFrIDstr) < 1)
		return;
	snprintf(msg, sizeof(msg),
			"TAG %s%s", (pTagName ? pTagName : ""), (pTagName ? " " : ""));
	fldnX[0] = 0;
	if (pOptsPF->frID == ST_ID3V2_FID_234_CTXT || pOptsPF->frID == ST_ID3V2_FID_234_CURL)
		snprintf(fldnX, sizeof(fldnX), " (%s)",
				(pOptsPF->pFrIDstr != NULL ? (char*)pOptsPF->pFrIDstr : "----"));
	ast_mf_op_prFInf("%s%s%s: ", msg, pFldName, fldnX);
	slen            = st_sysStrlen2(pFldName);
	cntPrintedChars = st_sysStrlen2(msg) + slen + 2;
	if (slen < 7 &&
			(! (pOptsPF->pOptsD->allFlds || pOptsPF->pOptsD->fullstr ||
					pOptsPF->pOptsD->fldDetails))) {
		ast_mf_op_prFInf("%*s", 7 - slen, "");
		cntPrintedChars += (7 - slen);
	}

	AST_MF__sw_printFld_sub(pFnc, pTag, pOptsPF, cntPrintedChars, pPrinted);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Print field nr, string nr and data type
 */
static void
AST_MF__sw_prFldNrsAndDT(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_bool isDatTp_fn, const Tst_bool isDatTp_mim,
		ST_OPTARG(Tst_uint32 *pDelML))
{
	Tst_uint32 x = 0;
	Tst_bool   havePendNL;

	if (pOptsPFS->printPreamb) {
		for (x = 0; x < pOptsPFS->preambelLen; x++)
			ast_mf_op_prFInf(" ");
		pOptsPFS->pendingNL = ST_B_TRUE;
	}
	if (pOptsPFS->pOptsD->fldDetails) {
		ST_ASSERTN_VOID(pDelML == NULL)

		if (pOptsPFS->ftot > 1 || pOptsPFS->stot > 1) {
			ast_mf_op_prFInf("%02u%s", pOptsPFS->fnr,
					(pOptsPFS->stot > 1 &&
						pOptsPFS->rwoTp != ST_ID3V2_TFLD_RWO_TP_SCD ? "." : ""));
			pOptsPFS->pendingNL = ST_B_TRUE;
			/* */
			*pDelML += 2 + (pOptsPFS->stot > 1 &&
					pOptsPFS->rwoTp != ST_ID3V2_TFLD_RWO_TP_SCD ? 1 : 0);
		}
		if (pOptsPFS->stot > 1 && pOptsPFS->rwoTp != ST_ID3V2_TFLD_RWO_TP_SCD) {
			ast_mf_op_prFInf("%02u: ", pOptsPFS->six);
			pOptsPFS->pendingNL = ST_B_TRUE;
			/* */
			*pDelML += 4;
		} else if (pOptsPFS->ftot > 1 ||
				(pOptsPFS->rwoTp == ST_ID3V2_TFLD_RWO_TP_SCD && pOptsPFS->stot > 1)) {
			ast_mf_op_prFInf(": ");
			pOptsPFS->pendingNL = ST_B_TRUE;
			/* */
			*pDelML += 2;
		}
	} else
		return;

	*pDelML   += 5;
	havePendNL = ST_B_TRUE;
	switch (pOptsPFS->rwoTp) {
	case ST_ID3V2_TFLD_RWO_TP_SCD: ast_mf_op_prFInf("Desc "); break;
	case ST_ID3V2_TFLD_RWO_TP_TEXT:
		if (pOptsPFS->strTp.hasTStampStr)
			ast_mf_op_prFInf("TStS ");
		else if (pOptsPFS->strTp.hasPosStr)
			ast_mf_op_prFInf("PosS ");
		else if (pOptsPFS->strTp.hasDateStr)
			ast_mf_op_prFInf("DatS ");
		else if (pOptsPFS->strTp.hasTimeStr)
			ast_mf_op_prFInf("TimS ");
		else if (pOptsPFS->isNumeroid)
			ast_mf_op_prFInf("NumS ");
		else
			ast_mf_op_prFInf("Text ");
		break;
	case ST_ID3V2_TFLD_RWO_TP_BDAT: ast_mf_op_prFInf("Data "); break;
	case ST_ID3V2_TFLD_RWO_TP_PICT:
		if (isDatTp_mim)
			ast_mf_op_prFInf("Mime ");
		else
			ast_mf_op_prFInf("Pict ");
		break;
	case ST_ID3V2_TFLD_RWO_TP_GEOB:
		if (isDatTp_fn)
			ast_mf_op_prFInf("File ");
		else if (isDatTp_mim)
			ast_mf_op_prFInf("Mime ");
		else
			ast_mf_op_prFInf("Geob ");
		break;
	case ST_ID3V2_TFLD_RWO_TP_UVAL:
		if (pOptsPFS->frID == ST_ID3V2_FID_234_POPM)
			ast_mf_op_prFInf("PCnt ");
		else if (pOptsPFS->frID == ST_ID3V2_FID_234_PCNT)
			ast_mf_op_prFInf("PCnt ");
		else
			ast_mf_op_prFInf("Uval ");
		break;
	case ST_ID3V2_TFLD_RWO_TP_BVAL:
		if (pOptsPFS->frID == ST_ID3V2_FID_234_POPM)
			ast_mf_op_prFInf("Rati ");
		else
			ast_mf_op_prFInf("Byte ");
		break;
	case ST_ID3V2_TFLD_RWO_TP_SYLT: ast_mf_op_prFInf("Sylt "); break;
	default:  /* print nothing */
		havePendNL = ST_B_FALSE;
		*pDelML   -= 5;
	}
	pOptsPFS->pendingNL = havePendNL;
}

/*
 * Print unsigned int value
 *
 * returns: true on success
 */
static Tst_bool
AST_MF__sw_prFldUVal(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_id3v2_fldData *pFld)
{
	Tst_uint32 delML = 0;

	AST_MF__sw_prFldNrsAndDT(pOptsPFS, ST_B_FALSE, ST_B_FALSE, &delML);
	ast_mf_op_prFInf("%u\n", st_id3v2_gs_getFieldData_uval(pFld));
	pOptsPFS->pendingNL = ST_B_FALSE;
	return ST_B_TRUE;
}

/*
 * Print byte value
 *
 * returns: true on success
 */
static Tst_bool
AST_MF__sw_prFldBVal(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_id3v2_fldData *pFld)
{
	Tst_uint32 delML = 0;

	AST_MF__sw_prFldNrsAndDT(pOptsPFS, ST_B_FALSE, ST_B_FALSE, &delML);
	ast_mf_op_prFInf("%u\n", st_id3v2_gs_getFieldData_bval(pFld));
	pOptsPFS->pendingNL = ST_B_FALSE;
	return ST_B_TRUE;
}

/*
 * Print synced lyrics info
 *
 * returns: true on success
 */
static Tst_bool
AST_MF__sw_prFldSylt(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_id3v2_fldData *pFld)
{
	Tst_uint32 delML = 0;

	AST_MF__sw_prFldNrsAndDT(pOptsPFS, ST_B_FALSE, ST_B_FALSE, &delML);
	ast_mf_op_prFInf("time stamp format=");
	switch (st_id3v2_gs_getFieldAttr_syltTSF(pFld)) {
	case ST_ID3V2_SLTTSF_MPEG:  ast_mf_op_prFInf("MPEG frames based"); break;
	case ST_ID3V2_SLTTSF_MILLI: ast_mf_op_prFInf("Millisecs based"); break;
	default: ast_mf_op_prFInf("Unknown");
	}
	ast_mf_op_prFInf(", content type=");
	switch (st_id3v2_gs_getFieldAttr_syltContTp(pFld)) {
	case ST_ID3V2_SLTCTP_OTHER:  ast_mf_op_prFInf("Other"); break;
	case ST_ID3V2_SLTCTP_LYR:    ast_mf_op_prFInf("Lyrics"); break;
	case ST_ID3V2_SLTCTP_TRANS:  ast_mf_op_prFInf("Text transcription"); break;
	case ST_ID3V2_SLTCTP_MOVE:   ast_mf_op_prFInf("Movement/Part name"); break;
	case ST_ID3V2_SLTCTP_EVENT:  ast_mf_op_prFInf("Events"); break;
	case ST_ID3V2_SLTCTP_CHORD:  ast_mf_op_prFInf("Chord"); break;
	case ST_ID3V2_SLTCTP_TRIV:   ast_mf_op_prFInf("Trivia/'Pop up'"); break;
	case ST_ID3V2_SLTCTP_URLWP:  ast_mf_op_prFInf("URLs to webpages"); break;
	case ST_ID3V2_SLTCTP_URLIMG: ast_mf_op_prFInf("URLs to images"); break;
	default: ast_mf_op_prFInf("Unknown");
	}
	ast_mf_op_prFInf("\n");
	pOptsPFS->pendingNL = ST_B_FALSE;
	return ST_B_TRUE;
}

/*
 * Print binary data
 *
 * returns: true on success
 */
static Tst_bool
AST_MF__sw_prFldBinData(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_id3v2_fldData *pFld, const Tst_bool prData)
{
	const Tst_uint32 cMAXDATASIZE = 4096;
	Tst_uint32 delML  = 0,
	           maxlen,
	           prx    = 0,
	           prxa   = 0,
	           x      = 0,
	           y,
	           binSz;
	Tst_buf    ch,
	           *pBinD = NULL;
	Tst_bool   oneLn  = ST_B_FALSE;
	Tst_binobj binDat;

	AST_MF__sw_prFldNrsAndDT(pOptsPFS, ST_B_FALSE, ST_B_FALSE, &delML);

	binSz = st_id3v2_gs_getFieldData_binSz(pFld);
	if (! prData || ! pOptsPFS->dispFStrOW || binSz > cMAXDATASIZE)
		ast_mf_op_prFInf("not displayed [");
	else
		ast_mf_op_prFInf("[follows, ");

	ast_mf_op_prFInf("%u byte%s", binSz, (binSz == 1 ? "" : "s"));
	if (binSz >= 1024 * 1024)
		ast_mf_op_prFInf(", %.2f MB", (double)binSz / (double)(1024 * 1024));
	else if (binSz > 1024)
		ast_mf_op_prFInf(", %.2f KB", (double)binSz / (double)(1024));
	ast_mf_op_prFInf("]\n");
	pOptsPFS->pendingNL = ST_B_FALSE;

	if (! prData || ! pOptsPFS->dispFStrOW || binSz > cMAXDATASIZE)
		return ST_B_TRUE;

	/* */
	st_binobj_stc_initBO(&binDat);
	if (st_id3v2_gs_getFieldData_bin(pFld, &binDat) != ST_ERR_SUCC) {
		st_binobj_stc_freeBO(&binDat);
		return ST_B_FALSE;
	}
	binSz = 0;
	if (st_binobj_copyIntoBuffer(&binDat, &pBinD, &binSz) != ST_ERR_SUCC) {
		st_binobj_stc_freeBO(&binDat);
		return ST_B_FALSE;
	}

	/* */
	ast_mf_op_prFInf("[ hex | asc\n");
	maxlen = ast_g_mf_op_termLnLen;  /* maxlen of hexa and char per line */
	while (x < binSz) {
		while ((prx + prx / 3) + (3 + 2) < maxlen &&
				x < binSz) {
			ast_mf_op_prFInf("%02x ", pBinD[x]);
			++x;
			prx += 3;
		}
		if (x >= binSz)
			break;
		ast_mf_op_prFInf("| ");
		for (y = 0; y < prx / 3; y++) {
			ch = pBinD[x - prx / 3 + y];
			ast_mf_op_prFInf("%c", (isprint((int)ch) ? (char)ch : '.'));
		}
		ast_mf_op_prFInf("\n");
		prx   = 0;
		oneLn = ST_B_TRUE;
	}
	if (prx > 0) {
		while (oneLn && (prx + prx / 3) + (3 + 2) < maxlen) {
			ast_mf_op_prFInf("   ");
			prx  += 3;
			prxa += 3;
		}
		ast_mf_op_prFInf("| ");
		prx -= prxa;
		for (y = 0; y < prx / 3; y++) {
			if (x - prx / 3 + y >= binSz)
				break;
			ch = pBinD[x - prx / 3 + y];
			if (ch < ST_CSET_ISO_PRNSMA || ch > ST_CSET_ISO_PRNBIG)
				ast_mf_op_prFInf(".");
			else
				ast_mf_op_prFInf("%c", (char)ch);
		}
		ast_mf_op_prFInf("\n");
	}
	ast_mf_op_prFInf("]\n");
	pOptsPFS->pendingNL = ST_B_FALSE;

	ST_DELPOINT(pBinD)
	st_binobj_stc_freeBO(&binDat);
	return ST_B_TRUE;
}

/*
 * Shorten string for output
 *   pSLen must contain strlen of pTmpS in chars
 *   delML must contain the amount of already printed chars
 *           in the current line
 */
static Tst_err
AST_MF__sw_prFldStr_shorten(Tst_uint32 delML, Tst_mtes_string *pTmpS,
		Tst_uint32 *pSLen)
{
	const char *cDOTDOT = " [..]";
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 maxlen = ast_g_mf_op_termLnLen;
	Tst_mtes_string tmpE;

	if (delML > 0)
		maxlen -= (maxlen > delML ? delML : maxlen);
	if (maxlen == 0)
		maxlen = 3 + st_sysStrlen2(cDOTDOT);
	if (*pSLen <= maxlen)
		return ST_ERR_SUCC;
	/* */
	st_mtes_stc_initStr(&tmpE);
	res = st_mtes_setStrTo2(st_mtes_getEncodingCur(pTmpS), cDOTDOT, &tmpE);
	if (res == ST_ERR_SUCC)
		res = st_mtes_shortenStr(pTmpS, maxlen, &tmpE, /*endOrCenter:*/ST_B_TRUE);
	if (res == ST_ERR_SUCC)
		*pSLen = maxlen;
	st_mtes_stc_freeStr(&tmpE);
	return res;
}

/*
 * Print string
 */
static Tst_err
AST_MF__sw_prFldStr(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_mtes_string *pStr, const Tst_str* pLang,
		const Tst_id3v2_fldd_strtp *pTpStr, const Tst_uint32 year,
		Tst_uint32 delMaxlISO)
{
	Tst_err       res;
	Tst_uint32    slenU16ch,
	              slenU16by;
	Tst_str const *pS = NULL;
	Tst_bool      withQMarks;
	Tst_mtes_string tmps;

	if (pTpStr != NULL && pTpStr->hasTStampStr && pOptsPFS->prOnlyYr) {
		ast_mf_op_prFInf("%u\n", year);
		pOptsPFS->pendingNL = ST_B_FALSE;
		return ST_ERR_SUCC;
	}

	withQMarks = ! pOptsPFS->dispFStrOW &&
			(pTpStr == NULL || ! pOptsPFS->isNumeroid);

	if (withQMarks)
		delMaxlISO += 2;

	st_mtes_stc_initStr(&tmps);
	/* I use UTF16 here, because it has a fixed char-width,
	 *   unlike UTF8 which uses 1 to 6 bytes for one char  */
	res = st_mtes_convStr_c(pStr, &tmps, ST_MTES_TE_UTF16LE);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmps);
		return res;
	}
	slenU16ch = st_mtes_strlenCh(&tmps);
	if (slenU16ch > 0 && pOptsPFS->rwoTp != ST_ID3V2_TFLD_RWO_TP_SCD &&
			! pOptsPFS->dispFStrOW) {
		/* replace '\n' with "[\n]", ... */
		st_mtes_strReplace(&tmps, (Tst_str*)"\n", (Tst_str*)"[\\n]", NULL);
		st_mtes_strReplace(&tmps, (Tst_str*)"\"", (Tst_str*)"[\\\"]", NULL);
		slenU16ch = st_mtes_strlenCh(&tmps);
	}
	if (slenU16ch > 0) {
		/* important to avoid buffer-overflows in printf() */
		st_mtes_strReplace(&tmps, (Tst_str*)"%", (Tst_str*)"%%", NULL);
		slenU16ch = st_mtes_strlenCh(&tmps);
	}

	if (withQMarks)
		ast_mf_op_prFInf("\"");

	if (! pOptsPFS->dispFStrOW) {
		res = AST_MF__sw_prFldStr_shorten(delMaxlISO, &tmps, &slenU16ch);
		if (res != ST_ERR_SUCC) {
			st_mtes_stc_freeStr(&tmps);
			return res;
		}
	}

	if (slenU16ch > 0) {
		if (! pOptsPFS->pOptsD->asISOorU8) {
			if (st_mtes_getEncodingCur(&tmps) != ST_MTES_TE_UTF8) {
				/* Modern Linux-Terminals/Shells support UTF8
				 *   and UTF8 can handle all Unicode chars,
				 *   which are a lot more than ISO-8859-1 Latin has,
				 *   and it's compatible with the first 127 ISO chars */
				res = st_mtes_convStr(&tmps, ST_MTES_TE_UTF8);
				if (res != ST_ERR_SUCC) {
					st_mtes_stc_freeStr(&tmps);
					return res;
				}
			}
			pS = st_mtes_getDataPt(&tmps);
		} else {
			if (st_mtes_getEncodingCur(&tmps) != ST_MTES_TE_ISO) {
				res = st_mtes_convStr(&tmps, ST_MTES_TE_ISO);
				if (res != ST_ERR_SUCC) {
					st_mtes_stc_freeStr(&tmps);
					return res;
				}
			}
			pS = st_mtes_getDataPt(&tmps);
		}
	}
	if (pOptsPFS->dispFStrOW) {
		slenU16by = st_mtes_strlenBy(&tmps);
		ast_mf_op_prFInf("[follows, ");
		ast_mf_op_prFInf("%u char%s", slenU16ch, (slenU16ch == 1 ? "" : "s"));
		if (slenU16ch != slenU16by)
			ast_mf_op_prFInf("/%u byte%s", slenU16by, (slenU16by == 1 ? "" : "s"));
		if (pLang && ! st_sysStrcmp2(ST_B_TRUE, pLang, "xxx"))
			ast_mf_op_prFInf(", lang '%s'", (char*)pLang);
		ast_mf_op_prFInf("]\n");
		ast_mf_op_prFInf("<...\n");
	}
	if (pS != NULL)
		ast_mf_op_prFInf((const char*)pS);

	if (withQMarks)
		ast_mf_op_prFInf("\"");
	else if (pOptsPFS->dispFStrOW)
		ast_mf_op_prFInf("\n...>");
	ast_mf_op_prFInf("\n");
	pOptsPFS->pendingNL = ST_B_FALSE;
	st_mtes_stc_freeStr(&tmps);
	return ST_ERR_SUCC;
}

/*
 * Print picture info
 *
 * returns: true on success
 */
static Tst_bool
AST_MF__sw_prFldPic(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_id3v2_fldData *pFld)
{
	Tst_uint32    delML = 0;
	Tst_str const *pCap;

	AST_MF__sw_prFldNrsAndDT(pOptsPFS, ST_B_FALSE, ST_B_FALSE, &delML);

	ast_mf_op_prFInf("type=");
	pCap = st_id3v2_fnc_getPicTp_cap_byEnum(
			st_id3v2_gs_getFieldAttr_picType(pFld));
	if (pCap == NULL)
		ast_mf_op_prFInf("UNKNOWN");
	else
		ast_mf_op_prFInf("'%s'", pCap);
	ast_mf_op_prFInf(", format=");
	pCap = st_utilsFmt_getPicFmt_fext_byEnum(
			st_id3v2_gs_getFieldAttr_picFmt(pFld));
	if (pCap == NULL)
		 ast_mf_op_prFInf("xxx");
	else
		ast_mf_op_prFInf("%s", pCap);
	ast_mf_op_prFInf("\n");
	pOptsPFS->pendingNL = ST_B_FALSE;
	return ST_B_TRUE;
}

/*
 * Print GEOB info
 *
 * returns: true on success
 */
static Tst_err
AST_MF__sw_prFldGeo(Tast_mf__prnfldsub *pOptsPFS,
		const Tst_id3v2_fldData *pFld)
{
	Tst_err    res;
	Tst_uint32 delML = 0;
	Tst_mtes_string tmps;

	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;
	res = st_id3v2_gs_getFieldAttr_geobFilen(pFld, &tmps);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmps);
		return res;
	}

	/* print filename */
	if (! st_mtes_isStrEmptyCh(&tmps)) {
		AST_MF__sw_prFldNrsAndDT(pOptsPFS, ST_B_TRUE, ST_B_FALSE, &delML);
		AST_MF__sw_prFldStr(pOptsPFS, &tmps, NULL, NULL, 0, delML);
	}
	/* print mime-type */
	delML = 0;
	res   = st_mtes_setStrTo(ST_MTES_TE_ISO,
			st_id3v2_gs_getFieldAttr_geobMime(pFld), &tmps);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmps);
		return res;
	}
	AST_MF__sw_prFldNrsAndDT(pOptsPFS, ST_B_FALSE, ST_B_TRUE, &delML);
	AST_MF__sw_prFldStr(pOptsPFS, &tmps, NULL, NULL, 0, delML);
	st_mtes_stc_freeStr(&tmps);
	return ST_ERR_SUCC;
}

/*
 * Print all fields with the ID frID
 */
static Tst_err
AST_MF__sw_printFld_sub(const char *pFnc,
		Tst_id3v2_tag *pTag, const Tast_mf__prnfld *pOptsPF,
		const Tst_uint32 cntPrintedChars, Tst_bool *pFound)
{
	const Tst_uint32 cPREAMBLEN_SHOR = 3;  /* "   " */
	Tst_err    res;
	Tst_uint32 y,
	           preambLenNw = 0,
	           preambLenEx = cntPrintedChars,
	           tmpYr       = 0,
	           delMaxlISO  = 0;
	Tst_bool   is1stFld    = ST_B_TRUE,
	           is1stData,
	           isMultiln   = ST_B_FALSE,
	           hasFldScd   = ST_B_FALSE,
	           hasFldBDat  = ST_B_FALSE,
	           hasFldPic   = ST_B_FALSE,
	           hasFldGeo   = ST_B_FALSE,
	           //prOneStr    = ST_B_FALSE,
	           isTSok      = ST_B_FALSE;
	Tst_mtes_string      *pStr,
	                     tmps1,
	                     tmps2;
	Tst_id3v2_fldData    *pItFld = NULL;
	Tst_id3v2_fldProp    fldPr;
	Tst_id3v2_fldd_dattp datTp;
	Tast_mf__prnfldsub   optsPFS;

	*pFound = ST_B_FALSE;
	AST_MF__sw_stc_rsetOPFS(&optsPFS, pOptsPF->pOptsD);
	optsPFS.prOnlyYr = pOptsPF->prOnlyYr;
	optsPFS.frID     = pOptsPF->frID;
	optsPFS.pFrIDstr = pOptsPF->pFrIDstr;
	st_id3v2_stc_rsetDatTp(&datTp);
	st_id3v2_stc_rsetFPr(&fldPr);

	/* count fields with same frID */
	while ((pItFld = st_id3v2_ite_nextValidFld_byID(pTag,
				pOptsPF->frID, pOptsPF->pFrIDstr, pItFld)) != NULL) {
		if (! st_id3v2_gs_getField_wasDisplayed(pItFld)) {
			*pFound = ST_B_TRUE;
			++optsPFS.ftot;
		}
	}

	isMultiln = pOptsPF->pOptsD->fldDetails || pOptsPF->pOptsD->fullstr;
	if (! pOptsPF->hasTagPre)
		preambLenEx -= 3;

	if (! *pFound) {
		/* if no field with pOptsPF->frID was found, say \n */
		if (! pOptsPF->hasAltern) {
			if (pOptsPF->pOptsD->fldDetails || pOptsPF->pOptsD->fullstr) {
				ast_mf_op_prFInf("\n%*sVoid ----", cPREAMBLEN_SHOR, "");
				optsPFS.pendingNL = ST_B_TRUE;
			}
			if (optsPFS.pendingNL) {
				ast_mf_op_prFInf("\n");
				optsPFS.pendingNL = ST_B_FALSE;
			}
		}
		return ST_ERR_SUCC;
	}

	res = st_mtes_stc_initStr(&tmps1);
	if (res != ST_ERR_SUCC)
		return res;
	res = st_mtes_stc_initStr(&tmps2);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmps1);
		return res;
	}

	/* does any field have a SCD ? */
	if (pOptsPF->prScd && pOptsPF->pOptsD->fldDetails) {
		pItFld = NULL;
		while ((pItFld = st_id3v2_ite_nextValidFld_byID(pTag,
					pOptsPF->frID, pOptsPF->pFrIDstr, pItFld)) != NULL) {
			if (! st_id3v2_gs_getField_wasDisplayed(pItFld) &&
					! st_id3v2_gs_getFieldAttr_desc_isEmpty(pItFld)) {
				isMultiln = ST_B_TRUE;
				break;
			}
		}
	}

	/* output all fields with pOptsPF->frID */
	pItFld = NULL;
	while ((pItFld = st_id3v2_ite_nextValidFld_byID(pTag,
				pOptsPF->frID, pOptsPF->pFrIDstr, pItFld)) != NULL) {
		if (st_id3v2_gs_getField_wasDisplayed(pItFld))
			continue;
		st_id3v2_gs_setField_wasDisplayed(pItFld, ! pOptsPF->prOnlyYr);
		st_id3v2_fnc_getFieldStringTypes(pOptsPF->frID, &optsPFS.strTp);
		st_id3v2_fnc_getFieldDataTypes(pOptsPF->frID, &datTp);

		/* */
		optsPFS.isNumeroid = optsPFS.strTp.hasNumStr ||
				optsPFS.strTp.hasYearStr ||
				optsPFS.strTp.hasDateStr || optsPFS.strTp.hasTimeStr ||
				optsPFS.strTp.hasTStampStr || optsPFS.strTp.hasPosStr;
		optsPFS.dispFStrOW = pOptsPF->pOptsD->fullstr /*&&
				! optsPFS.isNumeroid*/;
		optsPFS.stot  = st_id3v2_gs_getFieldData_stringArr_elemCount(pItFld);
		optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_NONE;
		optsPFS.fnr   = st_id3v2_gs_getField_nr(pItFld);

		/* */
		hasFldScd  = pOptsPF->prScd && datTp.hasScd &&
				(! st_id3v2_gs_getFieldAttr_desc_isEmpty(pItFld)) &&
				pOptsPF->pOptsD->fldDetails;
		hasFldPic  = datTp.hasPict && pOptsPF->pOptsD->fldDetails;
		hasFldGeo  = datTp.hasGeob && pOptsPF->pOptsD->fldDetails;
		hasFldBDat = datTp.hasBDat && pOptsPF->pOptsD->fldDetails;
		isMultiln  = isMultiln ||
				((hasFldBDat || hasFldScd || hasFldPic || hasFldGeo ||
					optsPFS.ftot > 1 || optsPFS.stot > 1 ||
					(optsPFS.stot == 1 && optsPFS.dispFStrOW)) &&
					(pOptsPF->pOptsD->fldDetails || optsPFS.dispFStrOW));
		if (isMultiln) {
			preambLenEx = 0;
			preambLenNw = cPREAMBLEN_SHOR;
			if (is1stFld) {
				ast_mf_op_prFInf("\n");
				optsPFS.pendingNL = ST_B_FALSE;
			}
		}
		/* */
		optsPFS.preambelLen = preambLenNw;
		optsPFS.printPreamb = ST_B_TRUE;
		/* */
		is1stData = ST_B_TRUE;

		/* handle some special cases */
		if (! pOptsPF->pOptsD->fldDetails) {
			st_id3v2_gs_getField_props(pItFld, &fldPr);
			switch (st_id3v2_gs_getFieldProp_typ(&fldPr)) {
			case ST_ID3V2_FTP_SLT:      /* S[Y]LT */
			case ST_ID3V2_FTP_IPL:      /* IPL[S] */
			case ST_ID3V2_FTP_CDM:      /* CDM */
			case ST_ID3V2_FTP_CNT:      /* [P]CNT */
			case ST_ID3V2_FTP_POP:      /* POP[M] */
			case ST_ID3V2_FTP_CPY:      /* copy data */
				AST_MF__sw_prFldNrsAndDT(&optsPFS, ST_B_FALSE, ST_B_FALSE, NULL);
				ast_mf_op_prFInf("[misc data]\n");
				optsPFS.pendingNL = ST_B_FALSE;
				continue;
			default:
				break;
			}
		}

		/* print ShortContDesc */
		if (hasFldScd) {
			res = st_id3v2_gs_getFieldAttr_desc(pItFld, &tmps1);
			if (res != ST_ERR_SUCC) {
				st_mtes_stc_freeStr(&tmps1);
				st_mtes_stc_freeStr(&tmps2);
				return res;
			}
			optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_SCD;
			optsPFS.six   = 1;
			delMaxlISO    = (pOptsPF->pOptsD->fldDetails || optsPFS.dispFStrOW ?
						optsPFS.preambelLen : cntPrintedChars);
			AST_MF__sw_prFldNrsAndDT(&optsPFS, ST_B_FALSE, ST_B_FALSE, &delMaxlISO);
			AST_MF__sw_prFldStr(&optsPFS, &tmps1, NULL, NULL, 0, delMaxlISO);
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
		}
		/* print picture info */
		if (datTp.hasPict && (is1stData || pOptsPF->pOptsD->fldDetails)) {
			if (! pOptsPF->pOptsD->fldDetails) {
				if (pOptsPF->pOptsD->allFlds) {
					AST_MF__sw_prFldNrsAndDT(&optsPFS, ST_B_FALSE, ST_B_FALSE, NULL);
					ast_mf_op_prFInf("[binary data]\n");
					optsPFS.pendingNL = ST_B_FALSE;
					st_id3v2_gs_setField_wasDisplayed(pItFld, ST_B_TRUE);
				} else {
					ast_mf_op_prFInf("%u picture%s\n", optsPFS.ftot,
							(optsPFS.ftot == 1 ? "" : "s"));
					optsPFS.pendingNL = ST_B_FALSE;
					st_id3v2_gs_setField_wasDisplayed(pItFld, optsPFS.ftot == 1);
				}
				break;
			} else {
				optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_PICT;
				optsPFS.six   = 1;
				AST_MF__sw_prFldPic(&optsPFS, pItFld);
			}
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
		}
		/* print geob info */
		if (hasFldGeo && (is1stData || pOptsPF->pOptsD->fldDetails)) {
			optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_GEOB;
			optsPFS.six   = 1;
			AST_MF__sw_prFldGeo(&optsPFS, pItFld);
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
		}
		/* print Binary data */
		if (datTp.hasBDat && (is1stData || pOptsPF->pOptsD->fldDetails)) {
			if (! pOptsPF->pOptsD->fldDetails) {
				AST_MF__sw_prFldNrsAndDT(&optsPFS, ST_B_FALSE, ST_B_FALSE, NULL);
				ast_mf_op_prFInf("[binary data]\n");
				optsPFS.pendingNL = ST_B_FALSE;
				st_id3v2_gs_setField_wasDisplayed(pItFld, ST_B_TRUE);
				break;
			} else {
				optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_BDAT;
				optsPFS.six   = 1;
				AST_MF__sw_prFldBinData(&optsPFS, pItFld, ! (hasFldPic || hasFldGeo));
			}
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
		}
		/* print UInt value */
		if (datTp.hasUVal && (is1stData || pOptsPF->pOptsD->fldDetails)) {
			optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_UVAL;
			optsPFS.six   = 1;
			AST_MF__sw_prFldUVal(&optsPFS, pItFld);
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
		}
		/* print byte value */
		if (datTp.hasBVal && (is1stData || pOptsPF->pOptsD->fldDetails)) {
			optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_BVAL;
			optsPFS.six   = 1;
			AST_MF__sw_prFldBVal(&optsPFS, pItFld);
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
		}
		/* print synced lyrics info */
		if (datTp.hasSylt && (is1stData || pOptsPF->pOptsD->fldDetails)) {
			optsPFS.rwoTp = ST_ID3V2_TFLD_RWO_TP_SYLT;
			optsPFS.six   = 1;
			AST_MF__sw_prFldSylt(&optsPFS, pItFld);
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
		}

		/* print string array */
		/** */
		if (! datTp.hasText && datTp.hasCTxt && optsPFS.stot > 0) {
			res = st_id3v2_gs_getFieldData_stringArr_elem(pItFld, 1, &tmps1);
			if (res != ST_ERR_SUCC) {
				st_mtes_stc_freeStr(&tmps1);
				st_mtes_stc_freeStr(&tmps2);
				return res;
			}
		} else
			st_mtes_stc_rsetStr(&tmps1, ST_B_TRUE, ST_B_TRUE);
		if (! (datTp.hasText ||
					(datTp.hasCTxt &&
						optsPFS.stot > 0 && ! st_mtes_isStrEmptyBy(&tmps1)))) {
			if (pOptsPF->prOnlyYr)
				st_id3v2_gs_setField_wasDisplayed(pItFld, ST_B_TRUE);
			if (optsPFS.pendingNL ||
					(optsPFS.dispFStrOW && (optsPFS.fnr < optsPFS.ftot))) {
				ast_mf_op_prFInf("\n");
				optsPFS.pendingNL = ST_B_FALSE;
			}
			continue;
		}
		/** */
		//prOneStr = ST_B_FALSE;
		for (y = 1; y <= optsPFS.stot; y++) {
			res = st_id3v2_gs_getFieldData_stringArr_elem(pItFld, y, &tmps1);
			if (res != ST_ERR_SUCC) {
				st_mtes_stc_freeStr(&tmps1);
				st_mtes_stc_freeStr(&tmps2);
				return res;
			}
			pStr = &tmps1;
			/* */
			if (optsPFS.strTp.hasGenreStr) {
				if (st_tagCFnc_chk_getGenreCap(pStr,
						&tmps2, ST_MTES_TE_UTF8) != ST_ERR_SUCC) {
					ast_mf_op_d_tagErr(st_id3v2_gs_getTag_filen(pTag), pFnc,
							"getting genre caption failed");
					continue;
				}
				pStr = &tmps2;
			} else if (optsPFS.strTp.hasTStampStr) {
				Tst_tagCFnc_tstamp tmpTS;

				st_tagCFnc_stc_rsetTSta(&tmpTS);
				res = st_tagCFnc_chk_getTStamp(ST_B_TRUE, ST_B_FALSE, ST_B_FALSE,
						pStr, &tmpTS, &isTSok);
				if (res != ST_ERR_SUCC) {
					ast_mf_op_d_tagErr(st_id3v2_gs_getTag_filen(pTag), pFnc,
							"getting timestamp failed");
					continue;
				}
				if (! isTSok) {
					ast_mf_op_d_tagErr(st_id3v2_gs_getTag_filen(pTag), pFnc,
							"timestamp invalid");
					continue;
				}
				tmpYr = tmpTS.year;
				if (pOptsPF->prOnlyYr && tmpTS.month < 1)
					st_id3v2_gs_setField_wasDisplayed(pItFld, ST_B_TRUE);
			}

			/* */
			optsPFS.rwoTp       = (isMultiln ? ST_ID3V2_TFLD_RWO_TP_TEXT :
					ST_ID3V2_TFLD_RWO_TP_NONE);
			optsPFS.six         = y;
			optsPFS.preambelLen = (isMultiln ? preambLenNw : preambLenEx);
			optsPFS.printPreamb = isMultiln;
			delMaxlISO          = (pOptsPF->pOptsD->fldDetails || optsPFS.dispFStrOW ?
						optsPFS.preambelLen : cntPrintedChars);
			AST_MF__sw_prFldNrsAndDT(&optsPFS, ST_B_FALSE, ST_B_FALSE, &delMaxlISO);
			if (AST_MF__sw_prFldStr(&optsPFS, pStr,
					(datTp.hasLang ?
						st_id3v2_gs_getFieldAttr_lang(pItFld) : NULL),
					&optsPFS.strTp, tmpYr, delMaxlISO) != ST_ERR_SUCC) {
				ast_mf_op_d_tagErr(st_id3v2_gs_getTag_filen(pTag), pFnc,
						"printing field string failed");
			}
			is1stFld  = ST_B_FALSE;
			is1stData = ST_B_FALSE;
			//prOneStr  = ST_B_TRUE;

			/* */
			if (! pOptsPF->pOptsD->fldDetails)
				break;
			if (optsPFS.pendingNL ||
					(optsPFS.dispFStrOW &&
						(optsPFS.six < optsPFS.stot ||
							(optsPFS.fnr < optsPFS.ftot)))) {
				ast_mf_op_prFInf("\n");
				optsPFS.pendingNL = ST_B_FALSE;
			}
		}

		/* */
		if (! pOptsPF->pOptsD->fldDetails) {
			if (optsPFS.stot > 1)
				st_id3v2_gs_setField_wasDisplayed(pItFld, ST_B_FALSE);
			break;
		}
	}
	st_mtes_stc_freeStr(&tmps1);
	st_mtes_stc_freeStr(&tmps2);

	if (optsPFS.pendingNL || optsPFS.dispFStrOW)
		ast_mf_op_prFInf("\n");
	return ST_ERR_SUCC;
}

/******************************************************************************/
