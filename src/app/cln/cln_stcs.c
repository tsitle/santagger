/******************************************************************************/
/* cln_stcs.c                     [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for program option structures                                    */
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
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
/** */
#define SRC_CLN_STCS_C
#include "cln_stcs-prot.h"
#undef SRC_CLN_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* free() */
#include <string.h>      /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tast_cln_a */
void
ast_cln_stc_initCln(Tast_cln_a *pCmd)
{
	ST_ASSERTN_VOID(pCmd == NULL)

	pCmd->pPAlngoptcArr = NULL;
	/* */
	AST_CLN__stc_initOpts(pCmd);

	ast_cln_stc_rsetCln(pCmd);
}

void
ast_cln_stc_rsetCln(Tast_cln_a *pCmd)
{
	ST_ASSERTN_VOID(pCmd == NULL)

	/* pointer to app filename */
	pCmd->pAppFn = NULL;
	/* long option cache array */
	ST_DELPOINT(pCmd->pPAlngoptcArr)

	/* commands */
	ast_cln_stc_resetCmdsTg(pCmd,
			/*roMain:*/ST_B_TRUE, /*roExt:*/ST_B_TRUE,
			/*rwRwr:*/ST_B_TRUE, /*rwEdt:*/ST_B_TRUE,
			/*rwRem:*/ST_B_TRUE, /*rwCnv:*/ST_B_TRUE);

	/* options */
	AST_CLN__stc_rsetOpts(pCmd);

	/* callbacks */
	pCmd->cbMsg = NULL;
	pCmd->cbDbg = NULL;
	pCmd->cbErr = NULL;
}

void
ast_cln_stc_freeCln(Tast_cln_a *pCmd)
{
	ST_ASSERTN_VOID(pCmd == NULL)

	ast_cln_stc_rsetCln(pCmd);
	/* options */
	AST_CLN__stc_freeOpts(pCmd);
}

/*----------------------------------------------------------------------------*/

/* Tast_cln_attFile */
void
ast_cln_stc_initAF(Tast_cln_attFile *pAF)
{
	ST_ASSERTN_VOID(pAF == NULL)

	pAF->pFn   = NULL;
	pAF->pDesc = NULL;
	pAF->pMime = NULL;

	ast_cln_stc_rsetAF(pAF);
}

void
ast_cln_stc_rsetAF(Tast_cln_attFile *pAF)
{
	ST_ASSERTN_VOID(pAF == NULL)

	ST_DELPOINT(pAF->pFn)
	ST_DELPOINT(pAF->pDesc)
	ST_DELPOINT(pAF->pMime)
	pAF->picTp      = ST_ID3V2_PT_NONE;
	pAF->isPTPset   = ST_B_FALSE;
	pAF->picFm      = ST_UTILSFMT_PF_NONE;
	pAF->isPFMset   = ST_B_FALSE;
	pAF->modFNr     = 0;
	pAF->isPicOrGeo = ST_B_TRUE;
	pAF->addOrMod   = ST_B_TRUE;
	pAF->isOK       = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/* Tast_cln_attFileArr */
void
ast_cln_stc_initAFarr(Tast_cln_attFileArr *pAFarr)
{
	ST_ASSERTN_VOID(pAFarr == NULL)

	pAFarr->pArr = NULL;
	pAFarr->sz   = 0;
	pAFarr->cnt  = 0;
}

void
ast_cln_stc_rsetAFarr(Tast_cln_attFileArr *pAFarr)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pAFarr == NULL)

	for (x = 0; x < pAFarr->sz; x++)
		ast_cln_stc_rsetAF(&pAFarr->pArr[x]);
	ST_DELPOINT(pAFarr->pArr)
	pAFarr->cnt = 0;
	pAFarr->sz  = 0;
}

/* Resizes attach-file array */
Tst_err
ast_cln_stc_reszAFarr(Tast_cln_attFileArr *pAFarr,
		const Tst_uint32 newSz)
{
	Tst_uint32 x;

	ST_ASSERTN_IARG(pAFarr == NULL)

	if (newSz > pAFarr->sz) {
		ST_REALLOC(pAFarr->pArr, Tast_cln_attFile*,
				newSz, sizeof(Tast_cln_attFile))
		if (pAFarr->pArr == NULL)
			return ST_ERR_OMEM;
		for (x = pAFarr->sz; x < newSz; x++)
			ast_cln_stc_initAF(&pAFarr->pArr[x]);
		pAFarr->sz = newSz;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tast_cln_t_tagList_b */
void
ast_cln_stc_rsetT_taglist(Tast_cln_t_tagList_pb(pList), const Tst_bool setTo)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pList == NULL)

	for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
		pList[x] = setTo;
}

/*----------------------------------------------------------------------------*/

/* Tast_cln_t_tagPair */
void
ast_cln_stc_rsetT_tagpair(Tast_cln_t_tagPair *pTP)
{
	ST_ASSERTN_VOID(pTP == NULL)

	ast_cln_stc_rsetT_taglist(pTP->src, ST_B_FALSE);
	pTP->dst = AST_CLN_T_TAGLIST_IV2;
}

/*----------------------------------------------------------------------------*/

/* Tast_cln_t_fldDscAddSet */
void
ast_cln_stc_initT_flddscaddset(Tast_cln_t_fldDscAddSet *pFP)
{
	ST_ASSERTN_VOID(pFP == NULL)

	pFP->pFldname  = NULL;
	pFP->pValueStr = NULL;

	ast_cln_stc_rsetT_flddscaddset(pFP);
}

void
ast_cln_stc_rsetT_flddscaddset(Tast_cln_t_fldDscAddSet *pFP)
{
	ST_ASSERTN_VOID(pFP == NULL)

	pFP->ttp = AST_CLN_T_TAGLIST_IV2;
	ST_DELPOINT(pFP->pFldname)
	pFP->fldnr = -1;
	ST_DELPOINT(pFP->pValueStr)
	pFP->valueInt      = -1;
	pFP->isIntStr      = ST_B_FALSE;
	pFP->fldDoAddOrSet = ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/* Tast_cln_t_tiv2opt */
void
ast_cln_stc_rsetT_tiv2opt(Tast_cln_t_tiv2opt *pTOpt)
{
	ST_ASSERTN_VOID(pTOpt == NULL)

	pTOpt->saveAsVers  = AST_CLN_DEF_TIV2_VAL_SAV;  /* v2.x.0 */
	pTOpt->allwCompr   = AST_CLN_DEF_TIV2_VAL_AWC;
	pTOpt->allwUnsynch = AST_CLN_DEF_TIV2_VAL_AWU;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
ast_cln_stc_resetCmdsTg(Tast_cln_a *pCmdln,
		const Tst_bool roMain, const Tst_bool roExt,
		const Tst_bool rwRwr, const Tst_bool rwEdt, const Tst_bool rwRem,
		const Tst_bool rwCnv)
{
	ST_ASSERTN_VOID(pCmdln == NULL)

	/* (read-only) main */
	if (roMain) {
		pCmdln->cmds.rd     = ST_B_FALSE;
		pCmdln->cmds.anlz   = ST_B_FALSE;
		pCmdln->cmds.decAud = ST_B_FALSE;
		/* */
		pCmdln->cmds.roMainCmds = 0;
	}

	/* (read-only) tag-extract */
	if (roExt) {
		pCmdln->cmds.extrTag  = ST_B_FALSE;
		pCmdln->cmds.extrPic  = ST_B_FALSE;
		pCmdln->cmds.extrGeo  = ST_B_FALSE;
		pCmdln->cmds.extrBinD = ST_B_FALSE;
		/* */
		pCmdln->cmds.roTgExtCmds = 0;
	}

	/* (read-write) tag-rewr */
	if (rwRwr) {
		pCmdln->cmds.rewrFile = ST_B_FALSE;
		/* */
		pCmdln->cmds.rwTgRwrCmds = 0;
	}
	/* (read-write) tag-edit */
	if (rwEdt) {
		pCmdln->cmds.rwTgEdtCmds = 0;
	}
	/* (read-write) tag-remove */
	if (rwRem) {
		pCmdln->cmds.remTag = ST_B_FALSE;
		/* */
		pCmdln->cmds.rwTgRemCmds = 0;
	}
	/* (read-write) tag-convert */
	if (rwCnv) {
		pCmdln->cmds.conv = ST_B_FALSE;
		/* */
		pCmdln->cmds.rwTgCnvCmds = 0;
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* callbacks */
static void
AST_CLN__stc_cbReset_t_flddscaddset(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	ast_cln_stc_rsetT_flddscaddset((Tast_cln_t_fldDscAddSet*)pElem);
}

static void
AST_CLN__stc_cbFree_t_flddscaddset(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	ast_cln_stc_freeT_flddscaddset((Tast_cln_t_fldDscAddSet*)pElem);
}

/*----------------------------------------------------------------------------*/

/* command line options */
static void
AST_CLN__stc_initOpts(Tast_cln_a *pCmd)
{
	ST_ASSERTN_VOID(pCmd == NULL)

	pCmd->opts.pLstExtrPic = NULL;
	pCmd->opts.pLstExtrGeo = NULL;
	/* */
	pCmd->opts.pOutpdir = NULL;
	/* */
	AST_CLN__stc_initCOTF(&pCmd->optsTagFlds);
	AST_CLN__stc_initCOAM(&pCmd->optsAvMisc);

	AST_CLN__stc_rsetOpts(pCmd);
}

static void
AST_CLN__stc_rsetOpts(Tast_cln_a *pCmd)
{
	ST_ASSERTN_VOID(pCmd == NULL)

	/* basic options */
	ST_RSETSTC_BASOPTS(pCmd->opts.basOpts)
	pCmd->opts.basOpts.pretWr = ST_B_FALSE;

	/* */
	pCmd->opts.quiet    = ST_B_FALSE;
	pCmd->opts.showStat = ST_B_FALSE;

	/* quick scan */
	pCmd->opts.quickScan = ST_B_FALSE;

	/* taglists show/read/write */
	ast_cln_stc_rsetT_taglist(pCmd->opts.lstTagSw, ST_B_FALSE);
	pCmd->opts.paLstTagSwSet = ST_B_FALSE;
	pCmd->opts.paTagSwNone   = ST_B_FALSE;
	ast_cln_stc_rsetT_taglist(pCmd->opts.lstTagEd, ST_B_FALSE);
	pCmd->opts.paLstTagEdSet = ST_B_FALSE;
	pCmd->opts.paTagEdNone   = ST_B_FALSE;
	pCmd->opts.tagEdAuto     = ST_B_FALSE;

	/* input */
	pCmd->opts.isInpISOorU8 = ST_B_FALSE;  /* def: UTF8 */

	/* display */
	AST_CLN__stc_rsetCOD(&pCmd->opts.disp);

	/* output dir etc. */
	ST_DELPOINT(pCmd->opts.pOutpdir)
	pCmd->opts.owExFiles = ST_B_FALSE;

	/* taglists remove/extract */
	ast_cln_stc_rsetT_taglist(pCmd->opts.lstRemTag, ST_B_FALSE);
	ast_cln_stc_rsetT_taglist(pCmd->opts.lstExtrTag, ST_B_FALSE);
	ST_DELPOINT(pCmd->opts.pLstExtrPic)
	pCmd->opts.extrPicAll = ST_B_FALSE;
	ST_DELPOINT(pCmd->opts.pLstExtrGeo)
	pCmd->opts.extrGeoAll = ST_B_FALSE;

	/* tagpair convert */
	ast_cln_stc_rsetT_tagpair(&pCmd->opts.convTagp);

	/* further options */
	AST_CLN__stc_rsetCOTF(&pCmd->optsTagFlds);
	ast_cln_stc_rsetT_tiv2opt(&pCmd->optsTagIV2);
	AST_CLN__stc_rsetCOAM(&pCmd->optsAvMisc);
}

static void
AST_CLN__stc_freeOpts(Tast_cln_a *pCmd)
{
	ST_ASSERTN_VOID(pCmd == NULL)

	AST_CLN__stc_rsetOpts(pCmd);
	/* further options */
	AST_CLN__stc_freeCOTF(&pCmd->optsTagFlds);
}

/*----------------------------------------------------------------------------*/

/* Tast_cln_optsTagFlds */
static void
AST_CLN__stc_initCOTF(Tast_cln_optsTagFlds *pCOTF)
{
	ST_ASSERTN_VOID(pCOTF == NULL)

	ast_cln_stc_initAFarr(&pCOTF->attFileArr);
	/* */
	pCOTF->pLstRemPic = NULL;
	pCOTF->pLstRemGeo = NULL;
	/* */
	st_dynlist_stc_initDL(&pCOTF->fsetListPrio3, sizeof(Tast_cln_t_fldDscAddSet),
		    AST_CLN__stc_cbReset_t_flddscaddset,
		    AST_CLN__stc_cbFree_t_flddscaddset);
	st_dynlist_stc_initDL(&pCOTF->faddsetListPrio2, sizeof(Tast_cln_t_fldDscAddSet),
		    AST_CLN__stc_cbReset_t_flddscaddset,
		    AST_CLN__stc_cbFree_t_flddscaddset);
	st_dynlist_stc_initDL(&pCOTF->dsetListPrio1, sizeof(Tast_cln_t_fldDscAddSet),
		    AST_CLN__stc_cbReset_t_flddscaddset,
		    AST_CLN__stc_cbFree_t_flddscaddset);

	AST_CLN__stc_rsetCOTF(pCOTF);
}

static void
AST_CLN__stc_rsetCOTF(Tast_cln_optsTagFlds *pCOTF)
{
	ST_ASSERTN_VOID(pCOTF == NULL)

	pCOTF->trk     = -1;
	pCOTF->trkTot  = -1;
	pCOTF->disc    = -1;
	pCOTF->discTot = -1;
	/* */
	ast_cln_stc_rsetAFarr(&pCOTF->attFileArr);
	/* */
	ST_DELPOINT(pCOTF->pLstRemPic)
	pCOTF->remPicAll = ST_B_FALSE;
	ST_DELPOINT(pCOTF->pLstRemGeo)
	pCOTF->remGeoAll = ST_B_FALSE;
	/* */
	st_dynlist_stc_rsetDL(&pCOTF->fsetListPrio3);
	st_dynlist_stc_rsetDL(&pCOTF->faddsetListPrio2);
	st_dynlist_stc_rsetDL(&pCOTF->dsetListPrio1);
}

static void
AST_CLN__stc_freeCOTF(Tast_cln_optsTagFlds *pCOTF)
{
	ST_ASSERTN_VOID(pCOTF == NULL)

	AST_CLN__stc_rsetCOTF(pCOTF);
	/* */
	st_dynlist_stc_freeDL(&pCOTF->fsetListPrio3);
	st_dynlist_stc_freeDL(&pCOTF->faddsetListPrio2);
	st_dynlist_stc_freeDL(&pCOTF->dsetListPrio1);
}

/*----------------------------------------------------------------------------*/

/* Tast_cln_optsAvMisc */
static void
AST_CLN__stc_initCOAM(Tast_cln_optsAvMisc *pCOAM)
{
	ST_ASSERTN_VOID(pCOAM == NULL)

	pCOAM->pEdBsArr = NULL;

	AST_CLN__stc_rsetCOAM(pCOAM);
}

static void
AST_CLN__stc_rsetCOAM(Tast_cln_optsAvMisc *pCOAM)
{
	ST_ASSERTN_VOID(pCOAM == NULL)

	/* MPEG-1 */
	pCOAM->mpeg1_forceMLay = ST_MPEG1_LAYER_NONE;
	pCOAM->mpeg1_forceMAv  = ST_MPEG1_AUDVERS_NONE;
	/* list of bitstreams to edit */
	ST_DELPOINT(pCOAM->pEdBsArr)
	pCOAM->edBsAll = st_sysStrcmp2(ST_B_TRUE, AST_CLN_DEF_EBS_STR, AST_CLN_ARG_ALL);
	/* decode audio */
	pCOAM->decAudFmt = ST_UTILSFMT_MTP_NONE;
	pCOAM->decAudBps = AST_CLN_DEF_BPS_VAL;

}

/*----------------------------------------------------------------------------*/

/* Tast_cln_optsDisp */
static void
AST_CLN__stc_rsetCOD(Tast_cln_optsDisp *pCOD)
{
	ST_ASSERTN_VOID(pCOD == NULL)

	pCOD->allFlds    = ST_B_FALSE;
	pCOD->fldDetails = ST_B_FALSE;
	pCOD->fullstr    = ST_B_FALSE;
	pCOD->asISOorU8  = ST_B_FALSE;  /* def: UTF8 */
	pCOD->tagger     = ST_B_TRUE;
}

/******************************************************************************/
