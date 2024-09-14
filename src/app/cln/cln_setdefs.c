/******************************************************************************/
/* cln_setdefs.c                  [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for setting default program options                          */
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
#include "src/includes/common/sys_fnc.h"
/** */
#define SRC_CLN_SETDEFS_C
#include "cln_setdefs-prot.h"
/*** */
#include "cln_stcs-prot.h"
#undef SRC_CLN_SETDEFS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_CLN__sd_setDefs_tagSw(Tast_cln_a *pCmdln, Tast_cln_t_tagList_pb(pDefLst));
static void
AST_CLN__sd_setDefs_tagEd(Tast_cln_a *pCmdln, Tast_cln_t_tagList_pb(pDefLst));

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set defaults for pCmdln->opts.lstTagSw and
 *                  pCmdln->opts.lstTagEd
 */
void
ast_cln_sd_setDefs_tag_swOrWr(Tast_cln_a *pCmdln)
{
	Tst_bool resTagSw = ST_B_TRUE,
	         resTagEd = ST_B_TRUE;
	Tast_cln_t_tagList_b(defLst);

	if ((pCmdln->cmds.rwTgRemCmds > 0 && pCmdln->cmds.remTag) ||
			(pCmdln->cmds.roMainCmds > 0 && pCmdln->cmds.decAud)) {
		/* reset --tag-sw/wr */
		ast_cln_stc_rsetT_taglist(defLst, ST_B_FALSE);
		AST_CLN__sd_setDefs_tagSw(pCmdln, defLst);  /* none */
		AST_CLN__sd_setDefs_tagEd(pCmdln, defLst);  /* none */
		/* */
		resTagSw = ST_B_FALSE;
		resTagEd = ST_B_FALSE;
	} else if (pCmdln->cmds.roTgExtCmds > 0) {
		/* reset --tag-sw/wr */
		ast_cln_stc_rsetT_taglist(defLst, ST_B_FALSE);
		AST_CLN__sd_setDefs_tagSw(pCmdln, defLst);  /* none */
		AST_CLN__sd_setDefs_tagEd(pCmdln, defLst);  /* none */
		/* */
		resTagSw = ST_B_FALSE;
		resTagEd = ST_B_FALSE;
	} else if (pCmdln->cmds.rwTgRwrCmds > 0) {
		/* reset --tag-sw/wr */
		ast_cln_stc_rsetT_taglist(defLst, ST_B_FALSE);
		AST_CLN__sd_setDefs_tagSw(pCmdln, defLst);  /* none */
		if (! pCmdln->opts.paLstTagEdSet) {
			pCmdln->opts.tagEdAuto = ST_B_TRUE;
			AST_CLN__sd_setDefs_tagEd(pCmdln, defLst);  /* none */
		}
		/* */
		resTagSw = ST_B_FALSE;
		resTagEd = ST_B_FALSE;
	} else if (pCmdln->cmds.rwTgEdtCmds > 0) {
		/* reset --tag-sw/wr */
		ast_cln_stc_rsetT_taglist(defLst, ST_B_FALSE);
		AST_CLN__sd_setDefs_tagSw(pCmdln, defLst);  /* none */
		if (! pCmdln->opts.paLstTagEdSet) {
			pCmdln->opts.tagEdAuto = ST_B_TRUE;
			AST_CLN__sd_setDefs_tagEd(pCmdln, defLst);  /* none */
		}
		/* */
		resTagSw = ST_B_FALSE;
		resTagEd = ST_B_FALSE;
	} else if (pCmdln->cmds.rwTgCnvCmds > 0) {
		/* reset --tag-sw/wr */
		ast_cln_stc_rsetT_taglist(defLst, ST_B_FALSE);
		AST_CLN__sd_setDefs_tagSw(pCmdln, defLst);  /* none */
		pCmdln->opts.tagEdAuto = ST_B_FALSE;
		AST_CLN__sd_setDefs_tagEd(pCmdln, defLst);  /* none */
		pCmdln->opts.lstTagEd[pCmdln->opts.convTagp.dst] = ST_B_TRUE;
		/* */
		resTagSw = ST_B_FALSE;
		resTagEd = ST_B_FALSE;
	}

	/* */
	if (resTagSw && ! pCmdln->opts.paLstTagSwSet) {
		/* set defaults for --tag-sw */
		ast_cln_stc_rsetT_taglist(defLst, ST_B_FALSE);
		defLst[AST_CLN_DEF_TAG_SW_VAL] = ST_B_TRUE;
		AST_CLN__sd_setDefs_tagSw(pCmdln, defLst);  /* only MRG */
	}
	if (resTagEd && ! pCmdln->opts.paLstTagEdSet) {
		/* set defaults for --tag-wr */
		ast_cln_stc_rsetT_taglist(defLst, ST_B_TRUE);
		AST_CLN__sd_setDefs_tagEd(pCmdln, defLst);  /* all but AV2 */
		if (st_sysStrcmp2(ST_B_TRUE, AST_CLN_DEF_TAG_WR_STR, AST_CLN_ARG_AUTO))
			pCmdln->opts.tagEdAuto = ST_B_TRUE;
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Overwrite pCmdln->opts.paLstTagSwSet and
 *           pCmdln->opts.paLstTagEdSet
 * if necessary
 */
void
ast_cln_sd_setOverwr_tag_swOrWr(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __func__;

	if (pCmdln->cmds.roTgExtCmds > 0 ||
			pCmdln->cmds.rwTgRwrCmds > 0 ||
			pCmdln->cmds.rwTgRemCmds > 0 ||
			pCmdln->cmds.rwTgCnvCmds > 0) {
		if (pCmdln->opts.paLstTagEdSet) {
			pCmdln->opts.paLstTagEdSet = ST_B_FALSE;
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"Notice: tag-extr/rewr/cnv/rem-commands overwrite --tag-wr !\n\n");
		}
	}

	if (pCmdln->cmds.rwTgRwrCmds > 0 ||
			pCmdln->cmds.rwTgEdtCmds > 0 ||
			pCmdln->cmds.rwTgRemCmds > 0 ||
			pCmdln->cmds.rwTgCnvCmds > 0) {
		if (pCmdln->opts.paLstTagSwSet) {
			pCmdln->opts.paLstTagSwSet = ST_B_FALSE;
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"Notice: tag-rewr/edit/cnv/rem-commands overwrite --tag-sw !\n\n");
		}
	}

	if (pCmdln->cmds.anlz || pCmdln->cmds.decAud) {
		if (pCmdln->opts.paLstTagEdSet) {
			pCmdln->opts.paLstTagEdSet = ST_B_FALSE;
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"Notice: --ana/dec overwrite --tag-wr !\n\n");
		}
	}
}

/*
 * Overwrite pCmdln->cmds.roTgExtCmds,
 *           pCmdln->cmds.rwTgRwrCmds,
 *           pCmdln->cmds.rwTgEdtCmds,
 *           pCmdln->cmds.rwTgRemCmds and
 *           pCmdln->cmds.rwTgCnvCmds
 * if necessary
 */
void
ast_cln_sd_setOverwr_cmds(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __func__;

	if (pCmdln->cmds.roMainCmds > 0 && pCmdln->cmds.roTgExtCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: readonly-commands overwrite tag-extr-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_TRUE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.roMainCmds > 0 && pCmdln->cmds.rwTgRwrCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: readonly-commands overwrite tag-rewr-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_TRUE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.roMainCmds > 0 && pCmdln->cmds.rwTgEdtCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: readonly-commands overwrite tag-edit-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_TRUE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.roMainCmds > 0 && pCmdln->cmds.rwTgRemCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: readonly-commands overwrite tag-rem-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_TRUE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.roMainCmds > 0 && pCmdln->cmds.rwTgCnvCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: readonly-commands overwrite tag-conv-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_TRUE);
	}

	if (pCmdln->cmds.roTgExtCmds > 0 && pCmdln->cmds.rwTgRwrCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-extr-commands overwrite tag-rewr-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_TRUE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.roTgExtCmds > 0 && pCmdln->cmds.rwTgEdtCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-extr-commands overwrite tag-edit-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_TRUE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.roTgExtCmds > 0 && pCmdln->cmds.rwTgRemCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-extr-commands overwrite tag-rem-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_TRUE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.roTgExtCmds > 0 && pCmdln->cmds.rwTgCnvCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-extr-commands overwrite tag-conv-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_TRUE);
	}

	if (pCmdln->cmds.rwTgRemCmds > 0 && pCmdln->cmds.rwTgRwrCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-rewr-commands overwrite tag-rem-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_TRUE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.rwTgRemCmds > 0 && pCmdln->cmds.rwTgEdtCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-edit-commands overwrite tag-rem-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_TRUE, /*rwCnv:*/ST_B_FALSE);
	}
	if (pCmdln->cmds.rwTgRemCmds > 0 && pCmdln->cmds.rwTgCnvCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-conv-commands overwrite tag-rem-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_TRUE, /*rwCnv:*/ST_B_FALSE);
	}

	if (pCmdln->cmds.rwTgRwrCmds > 0 && pCmdln->cmds.rwTgCnvCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-rewr-commands overwrite tag-conv-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_TRUE);
	}
	if (pCmdln->cmds.rwTgEdtCmds > 0 && pCmdln->cmds.rwTgCnvCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-edit-commands overwrite tag-conv-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_FALSE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_TRUE);
	}

	if (pCmdln->cmds.rwTgRwrCmds > 0 && pCmdln->cmds.rwTgEdtCmds > 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: tag-rewr-commands overwrite tag-edit-commands !\n\n");
		ast_cln_stc_resetCmdsTg(pCmdln,
				/*roMain:*/ST_B_FALSE, /*roExt:*/ST_B_FALSE,
				/*rwRwr:*/ST_B_FALSE, /*rwEdt:*/ST_B_TRUE,
				/*rwRem:*/ST_B_FALSE, /*rwCnv:*/ST_B_FALSE);
	}

	/* */
	ast_cln_sd_setOverwr_tag_swOrWr(pCmdln);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_CLN__sd_setDefs_tagSw(Tast_cln_a *pCmdln, Tast_cln_t_tagList_pb(pDefLst))
{
	Tst_uint32 x;
	Tst_bool   oneSet = ST_B_FALSE;

	for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++) {
		if (pDefLst[x])
			oneSet = ST_B_TRUE;
		pCmdln->opts.lstTagSw[x] = pDefLst[x];
	}
	/* */
	pCmdln->opts.paLstTagSwSet = ST_B_TRUE;
	pCmdln->opts.paTagSwNone   = ! oneSet;
}

static void
AST_CLN__sd_setDefs_tagEd(Tast_cln_a *pCmdln, Tast_cln_t_tagList_pb(pDefLst))
{
	Tst_uint32 x;
	Tst_bool   oneSet = ST_B_FALSE;

	for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++) {
		if (x == (Tst_uint32)AST_CLN_T_TAGLIST_MRG)
			continue;
		if (pDefLst[x])
			oneSet = ST_B_TRUE;
		pCmdln->opts.lstTagEd[x] = pDefLst[x];
	}
	pCmdln->opts.lstTagEd[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;
	/* */
	pCmdln->opts.paLstTagEdSet = ST_B_TRUE;
	pCmdln->opts.paTagEdNone   = ! oneSet;
}

/******************************************************************************/
