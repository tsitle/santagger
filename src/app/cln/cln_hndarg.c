/******************************************************************************/
/* cln_hndarg.c                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for handling program parameters                                  */
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
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_CLN_HNDARG_C
#include "cln_hndarg-pp.h"
/*** */
#include "cln_af-prot.h"
#include "cln_hndhelp-pp.h"
#include "cln_psvt-pp.h"
#include "cln_fnc-prot.h"
#include "cln_setdefs-prot.h"
#include "cln_stcs-prot.h"
#undef SRC_CLN_HNDARG_C

/*
// System-Includes
*/
#include <stdlib.h>      /* free() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* --read | -r */
Tst_err
ast_cln_hnd_c_rd(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;

	if (pCmdln->cmds.roMainCmds > 0) {
		if (pCmdln->cmds.anlz) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"Notice: --read overwrites --ana !\n\n");
			pCmdln->cmds.anlz = ST_B_FALSE;
			--pCmdln->cmds.roMainCmds;
		}
		if (pCmdln->cmds.decAud) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"Notice: --read overwrites --dec !\n\n");
			pCmdln->cmds.decAud = ST_B_FALSE;
			--pCmdln->cmds.roMainCmds;
		}
	}

	if (! pCmdln->cmds.rd) {
		++pCmdln->cmds.roMainCmds;
		pCmdln->cmds.rd = ST_B_TRUE;
	}
	ast_cln_sd_setOverwr_cmds(pCmdln);
	return ST_ERR_SUCC;
}

/* --ana */
Tst_err
ast_cln_hnd_c_ana(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;

	if (pCmdln->cmds.rd) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: --read overwrites --ana !\n\n");
		return ST_ERR_SUCC;
	}

	if (pCmdln->cmds.decAud) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Notice: --ana overwrites --dec !\n\n");
		pCmdln->cmds.decAud = ST_B_FALSE;
		--pCmdln->cmds.roMainCmds;
	}

	if (! pCmdln->cmds.anlz) {
		++pCmdln->cmds.roMainCmds;
		pCmdln->cmds.anlz = ST_B_TRUE;
	}
	ast_cln_sd_setOverwr_cmds(pCmdln);
	return ST_ERR_SUCC;
}

/* --dec wav|aiff */
Tst_err
ast_cln_hnd_c_decAud(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pFmt = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_CMD_DECAUD_LO,
			AST_CLN_MAX_ARGLEN, &pFmt);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pFmt)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no output format given, aborting");
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		if (st_sysStrcmp2(ST_B_TRUE, pFmt, AST_CLN_ARG_DECAUD_WAV)) {
			pCmdln->optsAvMisc.decAudFmt = ST_UTILSFMT_MTP_AUDWAV;
		} else if (st_sysStrcmp2(ST_B_TRUE, pFmt, AST_CLN_ARG_DECAUD_AIFF)) {
			pCmdln->optsAvMisc.decAudFmt = ST_UTILSFMT_MTP_AUDAIFF;
		} else {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"invalid output format given, aborting");
			res = ST_ERR_ABRT;
		}
	}
	ST_DELPOINT(pFmt)

	if (res == ST_ERR_SUCC) {
		if (pCmdln->cmds.rd) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"Notice: --read overwrites --dec !\n\n");
		} else if (pCmdln->cmds.anlz) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"Notice: --ana overwrites --dec !\n\n");
		} else {
			if (! pCmdln->cmds.decAud) {
				pCmdln->cmds.decAud = ST_B_TRUE;
				++pCmdln->cmds.roMainCmds;
			}
			ast_cln_sd_setOverwr_cmds(pCmdln);
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* [--band | -b] <str> */
Tst_err
ast_cln_hnd_c_tfld_bnd(Tast_cln_a *pCmdln)
{
	return AST_CLN__hnd_readAndAddToPrio3List(pCmdln, AST_CLN_CMD_BND_LO,
			AST_CLN_T_TAGLIST_IV2, "TPE1", /*pSrcStr:*/NULL,
			/*readStrOrIntOrBoth:*/1,
			/*intMin:*/0, /*intMax:*/0, /*intNone:*/0);
}

/* [--album | -a] <str> */
Tst_err
ast_cln_hnd_c_tfld_alb(Tast_cln_a *pCmdln)
{
	return AST_CLN__hnd_readAndAddToPrio3List(pCmdln, AST_CLN_CMD_ALB_LO,
			AST_CLN_T_TAGLIST_IV2, "TALB", /*pSrcStr:*/NULL,
			/*readStrOrIntOrBoth:*/1,
			/*intMin:*/0, /*intMax:*/0, /*intNone:*/0);
}

/* [--song | -s] <str> */
Tst_err
ast_cln_hnd_c_tfld_sng(Tast_cln_a *pCmdln)
{
	return AST_CLN__hnd_readAndAddToPrio3List(pCmdln, AST_CLN_CMD_SNG_LO,
			AST_CLN_T_TAGLIST_IV2, "TIT2", /*pSrcStr:*/NULL,
			/*readStrOrIntOrBoth:*/1,
			/*intMin:*/0, /*intMax:*/0, /*intNone:*/0);
}

/* [--track-nr | -t] <pos> */
Tst_err
ast_cln_hnd_c_tfld_tnr(Tast_cln_a *pCmdln)
{
	Tst_err res;

	/* tracknr==0 means there is no tracknr */
	res = AST_CLN__hnd_read_t_pos(pCmdln,
			AST_CLN_CMD_TRK_LO, AST_CLN_CMD_TRK_TOT_LO,
			&pCmdln->optsTagFlds.trk, &pCmdln->optsTagFlds.trkTot);
	if (res == ST_ERR_SUCC) {
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* --track-tot=<num> */
Tst_err
ast_cln_hnd_c_tfld_tnr_tot(Tast_cln_a *pCmdln)
{
	/* tracktot==0 means there is no tracktot */
	Tst_err res = ast_cln_fnc_cpyarg_i32(pCmdln,
			AST_CLN_CMD_TRK_TOT_LO,
			/*min:*/1, /*max:*/ST_ID3_TFLD_TRACK_MAX_IV2, /*none:*/0,
			&pCmdln->optsTagFlds.trkTot);

	if (res == ST_ERR_SUCC) {
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* [--disc-nr | -d] <pos> */
Tst_err
ast_cln_hnd_c_tfld_dsc(Tast_cln_a *pCmdln)
{
	Tst_err res;

	/* discnr==0 means there is no discnr */
	res = AST_CLN__hnd_read_t_pos(pCmdln,
			AST_CLN_CMD_DSC_LO, AST_CLN_CMD_DSC_TOT_LO,
			&pCmdln->optsTagFlds.disc, &pCmdln->optsTagFlds.discTot);
	if (res == ST_ERR_SUCC) {
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* --disc-tot=<num> */
Tst_err
ast_cln_hnd_c_tfld_dsc_tot(Tast_cln_a *pCmdln)
{
	/* disctot==0 means there is no disctot */
	Tst_err res = ast_cln_fnc_cpyarg_i32(pCmdln,
			AST_CLN_CMD_DSC_TOT_LO,
			/*min:*/1, /*max:*/ST_ID3_TFLD_TRACK_MAX_IV2, /*none:*/0,
			&pCmdln->optsTagFlds.discTot);

	if (res == ST_ERR_SUCC) {
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* [--year | -y] <num> */
Tst_err
ast_cln_hnd_c_tfld_yea(Tast_cln_a *pCmdln)
{
	/* year==0 means there is no year */
	return AST_CLN__hnd_readAndAddToPrio3List(pCmdln, AST_CLN_CMD_YEA_LO,
			AST_CLN_T_TAGLIST_IV2, "TYER", /*pSrcStr:*/NULL,
			/*readStrOrIntOrBoth:*/2,
			/*intMin:*/ST_ID3_TFLD_YEAR_MIN,
			/*intMax:*/ST_ID3_TFLD_YEAR_MAX, /*intNone:*/0);
}

/* [--comment | -c] <str> */
Tst_err
ast_cln_hnd_c_tfld_com(Tast_cln_a *pCmdln)
{
	return AST_CLN__hnd_readAndAddToPrio3List(pCmdln, AST_CLN_CMD_COM_LO,
			AST_CLN_T_TAGLIST_IV2, "COMM", /*pSrcStr:*/NULL,
			/*readStrOrIntOrBoth:*/1,
			/*intMin:*/0, /*intMax:*/0, /*intNone:*/0);
}

/* [--genre | -g] <num>|<str> */
Tst_err
ast_cln_hnd_c_tfld_gen(Tast_cln_a *pCmdln)
{
	Tst_err   res;
	Tst_str   *pSrcStr = NULL;
	Tst_int32 ival     = -1;

	/* genreNr==255 or genreStr=="" means there is no genre */
	res = ast_cln_fnc_cpyarg_i32OrStr(pCmdln, AST_CLN_CMD_GEN_LO,
			/*min:*/0, /*max:*/ST_ID3_TFLD_GENRE_MAX_IV1, /*none:*/255,
			AST_CLN_MAX_ARGLEN,
			&ival, &pSrcStr);
	if (res == ST_ERR_SUCC) {
		/* show help ? */
		if (pSrcStr != NULL &&
				(ast_cln_fnc_strcmp2x((const char*)pSrcStr,
							AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH))) {
			ast_cln_hhlp_show_c_genre(pCmdln);
			res = ST_ERR_QUIT;
		} else if (pSrcStr != NULL &&
				st_sysStrcmp2(ST_B_TRUE, pSrcStr, AST_CLN_ARG_HLPVALS)) {
			ast_cln_hhlp_show_c_genreValues(pCmdln, ST_B_TRUE);
			res = ST_ERR_QUIT;
		}
	}

	if (res == ST_ERR_SUCC && pSrcStr == NULL) {
		/* convert int back to string */
		ST_CALLOC(pSrcStr, Tst_str*, 32, 1)
		if (pSrcStr == NULL)
			res = ST_ERR_OMEM;
		else
			snprintf((char*)pSrcStr, 32, "%d", ival);
	}
	if (res == ST_ERR_SUCC) {
		res = AST_CLN__hnd_readAndAddToPrio3List(pCmdln, AST_CLN_CMD_GEN_LO,
				AST_CLN_T_TAGLIST_IV2, "TCON", pSrcStr,
				/*readStrOrIntOrBoth:*/3,
				/*intMin:*/0,
				/*intMax:*/ST_ID3_TFLD_GENRE_MAX_IV1, /*intNone:*/255);
	}

	ST_DELPOINT(pSrcStr)
	return res;
}

/* --add-pic/geo <filen> */
Tst_err
ast_cln_hnd_c_tfld_add_picOrGeo(Tast_cln_a *pCmdln, const Tst_bool isPic)
{
	const char *cFNCN = __FUNCTION__;
	Tst_str *pFn = NULL;
	Tst_err res;

	res = ast_cln_fnc_cpyarg_str(pCmdln,
			(isPic ? AST_CLN_CMD_ADD_PIC_LO : AST_CLN_CMD_ADD_GEO_LO),
			AST_CLN_MAX_ARGLEN, &pFn);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pFn)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no filename given, aborting");
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		res = ast_cln_af_addAF(pCmdln, isPic, /*addOrMod:*/ST_B_TRUE,
					pFn, /*modFNr:*/0);
	}
	if (res == ST_ERR_SUCC) {
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	ST_DELPOINT(pFn)
	return res;
}

/* --mod-pic/geo <num> */
Tst_err
ast_cln_hnd_c_tfld_mod_picOrGeo(Tast_cln_a *pCmdln, const Tst_bool isPic)
{
	Tst_err   res;
	Tst_int32 modFNr = 0;

	res = ast_cln_fnc_cpyarg_i32(pCmdln,
			(isPic ? AST_CLN_CMD_MOD_PIC_LO : AST_CLN_CMD_MOD_GEO_LO),
			/*min:*/1, /*max:*/9999, /*none:*/0,
			&modFNr);
	if (modFNr < 0)
		modFNr = 0;
	if (res == ST_ERR_SUCC) {
		res = ast_cln_af_addAF(pCmdln, isPic, /*addOrMod:*/ST_B_FALSE,
					/*pFilen:*/NULL, /*modFNr:*/modFNr);
	}
	if (res == ST_ERR_SUCC) {
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* --rem-pic/geo <numlist> */
Tst_err
ast_cln_hnd_c_tfld_rem_picOrGeo(Tast_cln_a *pCmdln, const Tst_bool isPic)
{
	Tst_err res;

	res = AST_CLN__hnd_read_t_numlist(pCmdln,
			(isPic ? AST_CLN_CMD_REM_PIC_LO : AST_CLN_CMD_REM_GEO_LO),
			(Tst_uint32**)(isPic ? &pCmdln->optsTagFlds.pLstRemPic :
				&pCmdln->optsTagFlds.pLstRemGeo),
			(isPic ? &pCmdln->optsTagFlds.remPicAll :
				&pCmdln->optsTagFlds.remGeoAll));
	/* */
	if (res == ST_ERR_SUCC) {
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* --fadd=<fldadd> */
Tst_err
ast_cln_hnd_c_tfld_fadd(Tast_cln_a *pCmdln)
{
#	define LOC_FNCN_  __FUNCTION__

	return AST_CLN__hnd_c_tfld_faddset(LOC_FNCN_, pCmdln, ST_B_TRUE);
#	undef LOC_FNCN_
}

/* --fset=<fldset> */
Tst_err
ast_cln_hnd_c_tfld_fset(Tast_cln_a *pCmdln)
{
#	define LOC_FNCN_  __FUNCTION__

	return AST_CLN__hnd_c_tfld_faddset(LOC_FNCN_, pCmdln, ST_B_FALSE);
#	undef LOC_FNCN_
}

/* --dset=<dscset> */
Tst_err
ast_cln_hnd_c_tfld_dset(Tast_cln_a *pCmdln)
{
	Tst_err  res    = ST_ERR_SUCC;
	Tst_bool swHlpM = ST_B_FALSE,
	         swHlpV = ST_B_FALSE;
	Tast_cln_t_fldDscAddSet *pDP = NULL;

	ST_CALLOC(pDP, Tast_cln_t_fldDscAddSet*,
			1, sizeof(Tast_cln_t_fldDscAddSet))
	if (pDP == NULL)
		return ST_ERR_OMEM;
	ast_cln_stc_initT_flddscaddset(pDP);
	pDP->ttp           = AST_CLN_T_TAGLIST_IV2;
	pDP->fldDoAddOrSet = ST_B_FALSE;

	if (res == ST_ERR_SUCC)
		res = AST_CLN__hnd_read_t_dscset(pCmdln, AST_CLN_CMD_DSET_LO,
				pDP, &swHlpM, &swHlpV);
	if (res == ST_ERR_SUCC) {
		if (swHlpM) {
			ast_cln_hhlp_show_c_dset(pCmdln);
			res = ST_ERR_QUIT;
		} else if (swHlpV) {
			ast_cln_hhlp_show_c_dsetValues(pCmdln);
			res = ST_ERR_QUIT;
		} else {
			st_dynlist_addElement(&pCmdln->optsTagFlds.dsetListPrio1, pDP);
			/* */
			++pCmdln->cmds.rwTgEdtCmds;
			ast_cln_sd_setOverwr_cmds(pCmdln);
		}
	}
	if (res != ST_ERR_SUCC) {
		ast_cln_stc_freeT_flddscaddset(pDP);
		ST_DELPOINT(pDP)
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/* --rewr */
Tst_err
ast_cln_hnd_c_rewr(Tast_cln_a *pCmdln)
{
	if (! pCmdln->cmds.rewrFile) {
		++pCmdln->cmds.rwTgRwrCmds;
		pCmdln->cmds.rewrFile = ST_B_TRUE;
	}
	ast_cln_sd_setOverwr_cmds(pCmdln);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* --rem-tag <taglist> */
Tst_err
ast_cln_hnd_c_rem_tag(Tast_cln_a *pCmdln)
{
	Tst_err res;
	Tast_cln_t_tagList_b(allwTags);

	ast_cln_stc_rsetT_taglist(allwTags, ST_B_TRUE);
	allwTags[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;

	res = AST_CLN__hnd_read_t_taglist(pCmdln, AST_CLN_CMD_REM_TAG_LO,
			allwTags, /*auto:*/ST_B_FALSE, /*none:*/ST_B_FALSE,
			pCmdln->opts.lstRemTag, /*pAuto:*/NULL, /*pNone:*/NULL);
	if (res == ST_ERR_SUCC) {
		if (! pCmdln->cmds.remTag) {
			pCmdln->cmds.remTag = ST_B_TRUE;
			++pCmdln->cmds.rwTgRemCmds;
		}
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* --extr-tag <taglist> */
Tst_err
ast_cln_hnd_c_extr_tag(Tast_cln_a *pCmdln)
{
	Tst_err res;
	Tast_cln_t_tagList_b(allwTags);

	ast_cln_stc_rsetT_taglist(allwTags, ST_B_TRUE);
	allwTags[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;

	res = AST_CLN__hnd_read_t_taglist(pCmdln, AST_CLN_CMD_EXTR_TAG_LO,
			allwTags, /*auto:*/ST_B_FALSE, /*none:*/ST_B_FALSE,
			pCmdln->opts.lstExtrTag, /*pAuto:*/NULL, /*pNone:*/NULL);
	if (res == ST_ERR_SUCC) {
		if (! pCmdln->cmds.extrTag) {
			pCmdln->cmds.extrTag = ST_B_TRUE;
			++pCmdln->cmds.roTgExtCmds;
		}
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* --extr-pic/geo <numlist> */
Tst_err
ast_cln_hnd_c_extr_picOrGeo(Tast_cln_a *pCmdln, const Tst_bool isPic)
{
	Tst_err res;

	res = AST_CLN__hnd_read_t_numlist(pCmdln,
			(isPic ? AST_CLN_CMD_EXTR_PIC_LO : AST_CLN_CMD_EXTR_GEO_LO),
			(Tst_uint32**)(isPic ? &pCmdln->opts.pLstExtrPic :
				&pCmdln->opts.pLstExtrGeo),
			(isPic ? &pCmdln->opts.extrPicAll : &pCmdln->opts.extrGeoAll));

	if (res == ST_ERR_SUCC) {
		if (isPic) {
			if (! pCmdln->cmds.extrPic) {
				pCmdln->cmds.extrPic = ST_B_TRUE;
				++pCmdln->cmds.roTgExtCmds;
			}
		} else {
			if (! pCmdln->cmds.extrGeo) {
				pCmdln->cmds.extrGeo = ST_B_TRUE;
				++pCmdln->cmds.roTgExtCmds;
			}
		}
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	return res;
}

/* --extr-bd */
Tst_err
ast_cln_hnd_c_extr_bd(Tast_cln_a *pCmdln)
{
	if (! pCmdln->cmds.extrBinD) {
		pCmdln->cmds.extrBinD = ST_B_TRUE;
		++pCmdln->cmds.roTgExtCmds;
	}
	ast_cln_sd_setOverwr_cmds(pCmdln);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* --conv <tagpair> */
Tst_err
ast_cln_hnd_c_conv(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pStr = NULL;
	Tast_cln_t_tagList_b(allwTagsLeft);
	Tast_cln_t_tagList_b(allwTagsRight);

	ast_cln_stc_rsetT_taglist(allwTagsLeft, ST_B_TRUE);
	ast_cln_stc_rsetT_taglist(allwTagsRight, ST_B_TRUE);
	allwTagsLeft[AST_CLN_T_TAGLIST_MRG]  = ST_B_FALSE;
	/*allwTagsRight[AST_CLN_T_TAGLIST_IV1] = ST_B_FALSE;*/
	allwTagsRight[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_CMD_CONV_LO, 512, &pStr);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pStr)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting", AST_CLN_SVT_FMT_TAGPAIR);
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		if (ast_cln_fnc_strcmp2x((const char*)pStr,
					AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
			ast_cln_hhlp_show_c_conv(pCmdln);
			res = ST_ERR_QUIT;
		} else if (! ast_cln_psvt_tagpair(pCmdln, pStr,
					allwTagsLeft, allwTagsRight, &pCmdln->opts.convTagp)) {
			res = ST_ERR_ABRT;
		} else {
			if (! pCmdln->cmds.conv) {
				pCmdln->cmds.conv = ST_B_TRUE;
				++pCmdln->cmds.rwTgCnvCmds;
			}
			ast_cln_sd_setOverwr_cmds(pCmdln);
		}
	}

	ST_DELPOINT(pStr)
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* -q */
Tst_err
ast_cln_hnd_o_quiet(Tast_cln_a *pCmdln)
{
	pCmdln->opts.quiet        = ST_B_TRUE;
	pCmdln->opts.showStat     = ST_B_FALSE;
	pCmdln->opts.basOpts.verb = 0;
	return ST_ERR_SUCC;
}

/* -v */
Tst_err
ast_cln_hnd_o_swstat(Tast_cln_a *pCmdln)
{
	if (pCmdln->opts.quiet)
		pCmdln->opts.showStat = ST_B_FALSE;
	else
		pCmdln->opts.showStat = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/* --vl <vlevlist> */
Tst_err
ast_cln_hnd_o_verblev(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err   res   = ST_ERR_SUCC;
	Tst_int32 tmp   = 0;
	Tst_str   *pLst = NULL;
	Tst_bool  none  = ST_B_FALSE;

	res = ast_cln_fnc_cpyarg_i32OrStr(pCmdln, "verbose",
			/*min:*/0x01, /*max:*/0xff, /*none:*/0,
			AST_CLN_MAX_ARGLEN, &tmp, &pLst);
	if (res == ST_ERR_SUCC && pCmdln->opts.quiet) {
		tmp = 0;  /* ^= none */
		ST_DELPOINT(pLst)
	}
	if (res == ST_ERR_SUCC) {
		if (tmp > 0 && pLst == NULL)
			pCmdln->opts.basOpts.verb = pCmdln->opts.basOpts.verb | tmp;
		else if (tmp == 0 && pLst == NULL)
			none = ST_B_TRUE;
		else if (! st_sysStrEmpty(pLst)) {
			if (ast_cln_fnc_strcmp2x((const char*)pLst,
						AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
				ast_cln_hhlp_show_o_verblev(pCmdln);
				res = ST_ERR_QUIT;
			} else if (! ast_cln_psvt_vlevlist(pCmdln, pLst, &tmp, &none))
				res = ST_ERR_ABRT;
		} else {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"no <%s> given, aborting", AST_CLN_SVT_FMT_VLEVLIST);
			res = ST_ERR_ABRT;
		}
	}
	if (res == ST_ERR_SUCC) {
		if (tmp > 0 || none) {
			if (none)
				pCmdln->opts.basOpts.verb = 0;
			else {
				pCmdln->opts.basOpts.verb = pCmdln->opts.basOpts.verb | tmp;
				if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_TAG))
					pCmdln->opts.basOpts.verb = (pCmdln->opts.basOpts.verb &
							~(ST_VL_TVREAD | ST_VL_TVSET | ST_VL_TVWRIT));
			}
		}
	}
	ST_DELPOINT(pLst)
	return res;
}

/* --pretend | -p */
Tst_err
ast_cln_hnd_o_prt(Tast_cln_a *pCmdln)
{
	pCmdln->opts.basOpts.pretWr = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* --qscan */
Tst_err
ast_cln_hnd_o_qscan(Tast_cln_a *pCmdln)
{
	pCmdln->opts.quickScan = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* --allow-links | --disallow-links */
Tst_err
ast_cln_hnd_o_allwlnks(Tast_cln_a *pCmdln, const Tst_bool allw)
{
	pCmdln->opts.basOpts.allowLnkInpF = allw;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* --disp-all */
Tst_err
ast_cln_hnd_o_disp_all(Tast_cln_a *pCmdln)
{
	pCmdln->opts.disp.allFlds = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/* --disp-det */
Tst_err
ast_cln_hnd_o_disp_det(Tast_cln_a *pCmdln)
{
	pCmdln->opts.disp.fldDetails = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/* --disp-fs */
Tst_err
ast_cln_hnd_o_disp_fstr(Tast_cln_a *pCmdln)
{
	pCmdln->opts.disp.fullstr = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/* --disp-iso/u8 */
Tst_err
ast_cln_hnd_o_disp_isoOrU8(Tast_cln_a *pCmdln, const Tst_bool isISO)
{
	pCmdln->opts.disp.asISOorU8 = isISO;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* --tag-sw/ed <taglist> */
Tst_err
ast_cln_hnd_o_tag_swOrEd(Tast_cln_a *pCmdln, const Tst_int32 which)
{
	Tst_err  res;
	Tst_bool *pLTSet;
	Tast_cln_t_tagList_b(allwTags);

	ast_cln_stc_rsetT_taglist(allwTags, ST_B_TRUE);
	allwTags[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;

	switch (which) {
	case 0:  /* show */
		pLTSet = &pCmdln->opts.paLstTagSwSet;
		/* */
		allwTags[AST_CLN_T_TAGLIST_MRG] = ST_B_TRUE;
		/* */
		res = AST_CLN__hnd_read_t_taglist(pCmdln, AST_CLN_OPT_TAG_SW_LO,
				allwTags, /*auto:*/ST_B_FALSE, /*none:*/ST_B_TRUE,
				pCmdln->opts.lstTagSw,
				/*pAuto:*/NULL,
				/*pNone:*/&pCmdln->opts.paTagSwNone);
		break;
	default:  /* edit/write */
		pLTSet = &pCmdln->opts.paLstTagEdSet;
		/* */
		res = AST_CLN__hnd_read_t_taglist(pCmdln, AST_CLN_OPT_TAG_ED_LO,
				allwTags, /*auto:*/ST_B_TRUE, /*none:*/ST_B_TRUE,
				pCmdln->opts.lstTagEd,
				/*pAuto:*/&pCmdln->opts.tagEdAuto,
				/*pNone:*/&pCmdln->opts.paTagEdNone);
	}
	/* */
	if (res == ST_ERR_SUCC) {
		*pLTSet = ST_B_TRUE;
		/* */
		ast_cln_sd_setOverwr_tag_swOrWr(pCmdln);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/* --ebs <bslist> */
Tst_err
ast_cln_hnd_o_ebs(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pLst = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_EBS_LO,
			AST_CLN_MAX_ARGLEN, &pLst);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pLst)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting", AST_CLN_SVT_FMT_BSLIST);
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		if (ast_cln_fnc_strcmp2x((const char*)pLst,
					AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
			ast_cln_hhlp_show_o_ebs(pCmdln);
			res = ST_ERR_QUIT;
		} else {
			pCmdln->optsAvMisc.edBsAll = ST_B_FALSE;
			if (! ast_cln_psvt_bslist(pCmdln, pLst,
						&pCmdln->optsAvMisc.pEdBsArr,
						&pCmdln->optsAvMisc.edBsAll))
				res = ST_ERR_ABRT;
		}
	}
	ST_DELPOINT(pLst)
	return res;
}

/*----------------------------------------------------------------------------*/

/* --force-mpX */
Tst_err
ast_cln_hnd_o_force_mpX(Tast_cln_a *pCmdln, const Tst_int32 lay)
{
	switch (lay) {
	case 1: pCmdln->optsAvMisc.mpeg1_forceMLay = ST_MPEG1_LAYER_1; break;
	case 2: pCmdln->optsAvMisc.mpeg1_forceMLay = ST_MPEG1_LAYER_2; break;
	case 3: pCmdln->optsAvMisc.mpeg1_forceMLay = ST_MPEG1_LAYER_3; break;
	default:
		return ST_ERR_IARG;
	}
	return ST_ERR_SUCC;
}

/* --force-avX */
Tst_err
ast_cln_hnd_o_force_avX(Tast_cln_a *pCmdln, const Tst_int32 av)
{
	switch (av) {
	case  1: pCmdln->optsAvMisc.mpeg1_forceMAv = ST_MPEG1_AUDVERS_1; break;
	case  2: pCmdln->optsAvMisc.mpeg1_forceMAv = ST_MPEG1_AUDVERS_2; break;
	case 25: pCmdln->optsAvMisc.mpeg1_forceMAv = ST_MPEG1_AUDVERS_25; break;
	default:
		return ST_ERR_IARG;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* --bps 0|16|24|32 */
Tst_err
ast_cln_hnd_o_bps(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err   res  = ST_ERR_SUCC;
	Tst_int32 tmpI = -1;

	res = ast_cln_fnc_cpyarg_i32(pCmdln, AST_CLN_OPT_BPS_LO,
			/*min:*/0, /*max:*/32, /*none:*/-1, &tmpI);

	if (res == ST_ERR_SUCC) {
		switch (tmpI) {
		case 0:
		case 16:
		case 24:
		case 32:
			pCmdln->optsAvMisc.decAudBps = (Tst_uint32)tmpI;
			break;
		default:
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"invalid sample size (bitsPerSample) given, aborting");
			res = ST_ERR_ABRT;
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/* --tiv2 <tiv2optlist> */
Tst_err
ast_cln_hnd_o_tiv2(Tast_cln_a *pCmdln)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pLst = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_TIV2_LO, 100, &pLst);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pLst)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting", AST_CLN_SVT_FMT_TIV2OPTLIST);
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		if (ast_cln_fnc_strcmp2x((const char*)pLst,
					AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
			ast_cln_hhlp_show_o_tiv2(pCmdln);
			res = ST_ERR_QUIT;
		} else {
			if (! ast_cln_psvt_tiv2optlist(pCmdln, pLst, &pCmdln->optsTagIV2))
				res = ST_ERR_ABRT;
		}
	}
	ST_DELPOINT(pLst)
	return res;
}

/*----------------------------------------------------------------------------*/

/* --pic-tp <num>|<pictp> */
Tst_err
ast_cln_hnd_o_pic_tp(Tast_cln_a *pCmdln)
{
	Tst_err   res;
	Tst_int32 tmpI   = -1;
	Tst_str   *pTmpS = NULL;
	Tst_id3v2_picTp ptpE = ST_ID3V2_PT_NONE;

	res = ast_cln_fnc_cpyarg_i32OrStr(pCmdln, AST_CLN_OPT_PIC_TP_LO,
			/*min:*/0, /*max:*/ST_ID3V2_PT_NONE, /*none:*/ST_ID3V2_PT_NONE,
			AST_CLN_MAX_ARGLEN, &tmpI, &pTmpS);
	if (res == ST_ERR_SUCC) {
		if (! st_sysStrEmpty(pTmpS)) {
			if (ast_cln_fnc_strcmp2x((const char*)pTmpS,
						AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
				ast_cln_hhlp_show_o_picTp(pCmdln);
				res = ST_ERR_QUIT;
			} else
				ptpE = st_id3v2_fnc_getPicTp_enum_byCapShort(pTmpS);
		} else if (tmpI >= 0)
			ptpE = (Tst_id3v2_picTp)tmpI;
	}
	if (res == ST_ERR_SUCC && ptpE != ST_ID3V2_PT_NONE)
		res = ast_cln_af_addAttrToAF(pCmdln, ST_B_TRUE, /*desc:*/NULL,
				ptpE, ST_UTILSFMT_PF_NONE, /*mime:*/NULL, /*filen:*/NULL);
	ST_DELPOINT(pTmpS)
	return res;
}

/* --pic-fm <num>|<picfm> */
Tst_err
ast_cln_hnd_o_pic_fm(Tast_cln_a *pCmdln)
{
	Tst_err   res;
	Tst_int32 tmpI   = -1;
	Tst_str   *pTmpS = NULL;
	Tst_utilsFmt_picFmt pfmE = ST_UTILSFMT_PF_NONE;

	res = ast_cln_fnc_cpyarg_i32OrStr(pCmdln, AST_CLN_OPT_PIC_FM_LO,
			/*min:*/0, /*max:*/ST_UTILSFMT_PF_NONE, /*none:*/ST_UTILSFMT_PF_NONE,
			AST_CLN_MAX_ARGLEN, &tmpI, &pTmpS);
	if (res == ST_ERR_SUCC) {
		if (! st_sysStrEmpty(pTmpS)) {
			if (ast_cln_fnc_strcmp2x((const char*)pTmpS,
						AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
				ast_cln_hhlp_show_o_picFm(pCmdln);
				res = ST_ERR_QUIT;
			} else
				pfmE = st_utilsFmt_getPicFmt_enum_byFExt(pTmpS);
		} else if (tmpI >= 0)
			pfmE = (Tst_utilsFmt_picFmt)tmpI;
	}
	if (res == ST_ERR_SUCC && pfmE != ST_UTILSFMT_PF_NONE)
		res = ast_cln_af_addAttrToAF(pCmdln, ST_B_TRUE, /*desc:*/NULL,
				ST_ID3V2_PT_NONE, pfmE, /*mime:*/NULL, /*filen:*/NULL);
	ST_DELPOINT(pTmpS)
	return res;
}

/* --pic-de <str> */
Tst_err
ast_cln_hnd_o_pic_de(Tast_cln_a *pCmdln)
{
	Tst_err res;
	Tst_str *pTmpS = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_PIC_DE_LO,
			AST_CLN_MAX_ARGLEN, &pTmpS);
	if (res == ST_ERR_SUCC)
		res = ast_cln_af_addAttrToAF(pCmdln, ST_B_TRUE, /*desc:*/pTmpS,
				ST_ID3V2_PT_NONE, ST_UTILSFMT_PF_NONE,
				/*mime:*/NULL, /*filen:*/NULL);
	ST_DELPOINT(pTmpS)
	return res;
}

/* --pic-fn <filen> */
Tst_err
ast_cln_hnd_o_pic_fn(Tast_cln_a *pCmdln)
{
	Tst_err res;
	Tst_str *pTmpS = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_PIC_FN_LO,
			AST_CLN_MAX_ARGLEN, &pTmpS);
	if (res == ST_ERR_SUCC)
		res = ast_cln_af_addAttrToAF(pCmdln, ST_B_TRUE, /*desc:*/NULL,
				ST_ID3V2_PT_NONE, ST_UTILSFMT_PF_NONE,
				/*mime:*/NULL, /*filen:*/pTmpS);
	ST_DELPOINT(pTmpS)
	return res;
}

/* --geo-mi <mime> */
Tst_err
ast_cln_hnd_o_geo_mi(Tast_cln_a *pCmdln)
{
	Tst_err res;
	Tst_str *pTmpS = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_GEO_MI_LO,
			AST_CLN_MAX_ARGLEN, &pTmpS);
	if (res == ST_ERR_SUCC)
		res = ast_cln_af_addAttrToAF(pCmdln, ST_B_FALSE, /*desc:*/NULL,
				ST_ID3V2_PT_NONE, ST_UTILSFMT_PF_NONE,
				/*mime:*/pTmpS, /*filen:*/NULL);
	ST_DELPOINT(pTmpS)
	return res;
}

/* --geo-de <str> */
Tst_err
ast_cln_hnd_o_geo_de(Tast_cln_a *pCmdln)
{
	Tst_err res;
	Tst_str *pTmpS = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_GEO_DE_LO,
			AST_CLN_MAX_ARGLEN, &pTmpS);
	if (res == ST_ERR_SUCC)
		res = ast_cln_af_addAttrToAF(pCmdln, ST_B_FALSE, /*desc:*/pTmpS,
				ST_ID3V2_PT_NONE, ST_UTILSFMT_PF_NONE,
				/*mime:*/NULL, /*filen:*/NULL);
	ST_DELPOINT(pTmpS)
	return res;
}

/* --geo-fn <filen> */
Tst_err
ast_cln_hnd_o_geo_fn(Tast_cln_a *pCmdln)
{
	Tst_err res;
	Tst_str *pTmpS = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_GEO_FN_LO,
			AST_CLN_MAX_ARGLEN, &pTmpS);
	if (res == ST_ERR_SUCC)
		res = ast_cln_af_addAttrToAF(pCmdln, ST_B_FALSE, /*desc:*/NULL,
				ST_ID3V2_PT_NONE, ST_UTILSFMT_PF_NONE,
				/*mime:*/NULL, /*filen:*/pTmpS);
	ST_DELPOINT(pTmpS)
	return res;
}

/*----------------------------------------------------------------------------*/

/* --outdir <dirn> */
Tst_err
ast_cln_hnd_o_outdir(Tast_cln_a *pCmdln)
{
	Tst_err res;

	res = ast_cln_fnc_cpyarg_str(pCmdln, AST_CLN_OPT_OUTDIR_LO,
			AST_CLN_MAX_ARGLEN, &pCmdln->opts.pOutpdir);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
AST_CLN__hnd_readAndAddToPrio3List(Tast_cln_a *pCmdln, const char *pOptCap,
		const Tast_cln_t_tagListIx ttp, const char *pFldName,
		ST_OPTARG(const Tst_str *pSrcStr),
		const int readStrOrIntOrBoth,
		const Tst_int32 intMin, const Tst_int32 intMax, const Tst_int32 intNone)
{
	Tst_err res = ST_ERR_SUCC;
	Tast_cln_t_fldDscAddSet *pFP = NULL;

	/* prepare pFP */
	/** */
	ST_CALLOC(pFP, Tast_cln_t_fldDscAddSet*,
			1, sizeof(Tast_cln_t_fldDscAddSet))
	if (pFP == NULL)
		return ST_ERR_OMEM;
	ast_cln_stc_initT_flddscaddset(pFP);
	pFP->ttp           = ttp;
	pFP->fldDoAddOrSet = ST_B_FALSE;
	/** */
	res = st_sysStrcpy((const Tst_str*)pFldName, &pFP->pFldname);

	/* read or copy the value */
	if (res == ST_ERR_SUCC && pSrcStr == NULL) {
		switch (readStrOrIntOrBoth) {
		case 1:
			res = ast_cln_fnc_cpyarg_str(pCmdln, pOptCap,
					AST_CLN_MAX_ARGLEN, &pFP->pValueStr);
			break;
		case 2:
			res = ast_cln_fnc_cpyarg_i32(pCmdln, pOptCap,
					/*min:*/intMin, /*max:*/intMax, /*none:*/intNone,
					&pFP->valueInt);
			break;
		case 3:
			res = ast_cln_fnc_cpyarg_i32OrStr(pCmdln, pOptCap,
					/*min:*/intMin, /*max:*/intMax, /*none:*/intNone,
					AST_CLN_MAX_ARGLEN,
					&pFP->valueInt, &pFP->pValueStr);
			break;
		default:
			res = ST_ERR_IARG;
		}
	} else if (res == ST_ERR_SUCC) {
		/* pSrcStr has already been checked */
		res = st_sysStrcpy(pSrcStr, &pFP->pValueStr);
	}

	/* convert string to int or vice-versa */
	if (res == ST_ERR_SUCC) {
		if (pFP->pValueStr != NULL) {
			Tst_uint32 slen = st_sysStrlen(pFP->pValueStr);

			if (slen > 0 && slen < 10) {
				Tst_str const *pVSch = pFP->pValueStr;
				Tst_bool      isNum  = ST_B_TRUE;

				while (*pVSch) {
					if (*pVSch < ST_CSET_ISO_DIG0 || *pVSch > ST_CSET_ISO_DIG9) {
						isNum = ST_B_FALSE;
						break;
					}
					++pVSch;
				}
				if (isNum) {
					pFP->valueInt = (Tst_int32)strtol((const char*)pFP->pValueStr,
								(char**)NULL, 10);
					if ((readStrOrIntOrBoth == 2 || readStrOrIntOrBoth == 3) &&
							pFP->valueInt == intNone) {
						res           = st_sysStrcpy2("", &pFP->pValueStr);
						pFP->valueInt = -1;
					} else
						pFP->isIntStr = ST_B_TRUE;
				}
			}
		} else if (pFP->valueInt >= 0) {
			if ((readStrOrIntOrBoth == 2 || readStrOrIntOrBoth == 3) &&
					pFP->valueInt == intNone) {
				res           = st_sysStrcpy2("", &pFP->pValueStr);
				pFP->valueInt = -1;
			} else {
				ST_REALLOC(pFP->pValueStr, Tst_str*, 32, 1)
				if (pFP->pValueStr == NULL)
					res = ST_ERR_OMEM;
				else {
					snprintf((char*)pFP->pValueStr, 32, "%d", pFP->valueInt);
					pFP->isIntStr = ST_B_TRUE;
				}
			}
		}
	}

	/* add pFP to the list */
	if (res == ST_ERR_SUCC) {
		st_dynlist_addElement(&pCmdln->optsTagFlds.fsetListPrio3, pFP);
		/* */
		++pCmdln->cmds.rwTgEdtCmds;
		ast_cln_sd_setOverwr_cmds(pCmdln);
	}
	if (res != ST_ERR_SUCC) {
		ast_cln_stc_freeT_flddscaddset(pFP);
		ST_DELPOINT(pFP)
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/* --fadd/fset=<fldadd>/<fldset> */
static Tst_err
AST_CLN__hnd_c_tfld_faddset(const char *pFnc, Tast_cln_a *pCmdln,
		const Tst_bool addOrSet)
{
	Tst_err    res          = ST_ERR_SUCC;
	char const *pOptCap;
	Tst_bool   swHlp        = ST_B_FALSE;
	Tst_str    whichHelp[4] = {0, 0, 0, 0};
	Tast_cln_t_fldDscAddSet *pFP = NULL;

	pOptCap = (addOrSet ? AST_CLN_CMD_FADD_LO : AST_CLN_CMD_FSET_LO);

	ST_CALLOC(pFP, Tast_cln_t_fldDscAddSet*,
			1, sizeof(Tast_cln_t_fldDscAddSet))
	if (pFP == NULL)
		return ST_ERR_OMEM;
	ast_cln_stc_initT_flddscaddset(pFP);
	pFP->ttp           = AST_CLN_T_TAGLIST_IV2;
	pFP->fldDoAddOrSet = addOrSet;

	if (res == ST_ERR_SUCC)
		res = AST_CLN__hnd_read_t_fldaddset(pCmdln, pOptCap,
				addOrSet, pFP, &swHlp, whichHelp);
	if (res == ST_ERR_SUCC) {
		if (swHlp && whichHelp[0] == 'm' && whichHelp[1] == 'a') {
			ast_cln_hhlp_show_c_faddset(pCmdln, pOptCap, addOrSet);
			res = ST_ERR_QUIT;
		} else if (swHlp) {
			Tst_bool   whIsOK = ST_B_FALSE;
			Tst_uint32 lenWH  = st_sysStrlen(whichHelp);
			Tast_cln_t_tagListIx whTLIX = AST_CLN_T_TAGLIST_IV2;

			if (lenWH == 3) {
				whIsOK = st_sysStrcmpN2(ST_B_FALSE, 3, whichHelp, "IV2");
				if (whIsOK)
					whTLIX = AST_CLN_T_TAGLIST_IV2;
				else {
					whIsOK = st_sysStrcmpN2(ST_B_FALSE, 3, whichHelp, "AV2");
					if (whIsOK)
						whTLIX = AST_CLN_T_TAGLIST_AV2;
				}
				if (! whIsOK) {
					whIsOK = st_sysStrcmpN2(ST_B_FALSE, 3, whichHelp, "VOR");
					if (whIsOK)
						whTLIX = AST_CLN_T_TAGLIST_VOR;
				}
			}
			if (! whIsOK) {
				pCmdln->cbErr(pCmdln->pAppFn, pFnc,
						"unknown help page requested");
				res = ST_ERR_ABRT;
			} else {
				ast_cln_hhlp_show_c_faddsetValues(pCmdln, pOptCap,
						addOrSet, whTLIX);
				res = ST_ERR_QUIT;
			}
		} else {
			st_dynlist_addElement(&pCmdln->optsTagFlds.faddsetListPrio2, pFP);
			/* */
			++pCmdln->cmds.rwTgEdtCmds;
			ast_cln_sd_setOverwr_cmds(pCmdln);
		}
	}
	if (res != ST_ERR_SUCC) {
		ast_cln_stc_freeT_flddscaddset(pFP);
		ST_DELPOINT(pFP)
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* read <taglist> */
static Tst_err
AST_CLN__hnd_read_t_taglist(Tast_cln_a *pCmdln, const char *pOptCap,
		const Tast_cln_t_tagList_pb(pAllwTags),
		const Tst_bool autoAllw, const Tst_bool nonAllw,
		Tast_cln_t_tagList_pb(pOutTags),
		Tst_bool *pAuto, Tst_bool *pNone)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pLst = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, pOptCap, 256, &pLst);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pLst)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting", AST_CLN_SVT_FMT_TAGLIST);
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		if (ast_cln_fnc_strcmp2x((const char*)pLst,
					AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
			ast_cln_hhlp_show_t_taglist(pCmdln, pOptCap,
					pAllwTags, autoAllw, nonAllw);
			res = ST_ERR_QUIT;
		} else if (! ast_cln_psvt_taglist(pCmdln, pLst,
					pAllwTags, autoAllw, nonAllw,
					pOutTags, pAuto, pNone))
			res = ST_ERR_ABRT;
	}
	/* */
	ST_DELPOINT(pLst)
	return res;
}

/*----------------------------------------------------------------------------*/

/* read <numlist> */
static Tst_err
AST_CLN__hnd_read_t_numlist(Tast_cln_a *pCmdln, const char *pOptCap,
	Tst_uint32 **ppILst, Tst_bool *pAll)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pLst = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, pOptCap, 1000 * 3 + 999, &pLst);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pLst)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting", AST_CLN_SVT_FMT_NUMLIST);
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		if (ast_cln_fnc_strcmp2x((const char*)pLst,
					AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
			ast_cln_hhlp_show_t_numlist(pCmdln, pOptCap);
			res = ST_ERR_QUIT;
		} else if (! ast_cln_psvt_numlist(pCmdln, pLst, ppILst, pAll))
			res = ST_ERR_ABRT;
	}
	/* */
	ST_DELPOINT(pLst)
	return res;
}

/*----------------------------------------------------------------------------*/

/* read <pos> */
static Tst_err
AST_CLN__hnd_read_t_pos(Tast_cln_a *pCmdln, const char *pOptCap,
		const char *pOptCapTot,
		Tst_int32 *pPosNr, Tst_int32 *pPosTot)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pStr = NULL;

	/* empty nr means there is no number */
	res = ast_cln_fnc_cpyarg_str(pCmdln, pOptCap, 10 * 2 + 1, &pStr);
	if (res == ST_ERR_SUCC && pStr == NULL) {
		/* this case should never occur */
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting", AST_CLN_SVT_FMT_POS);
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		if (st_sysStrEmpty(pStr))
			*pPosNr = 0;
		else if (ast_cln_fnc_strcmp2x((const char*)pStr,
					AST_CLN_ARG_HLP, AST_CLN_ARG_HLPSH)) {
			ast_cln_hhlp_show_t_pos(pCmdln, pOptCap, pOptCapTot);
			res = ST_ERR_QUIT;
		} else if (! ast_cln_psvt_pos(pCmdln, pStr, pPosNr, pPosTot))
			res = ST_ERR_ABRT;
	}
	/* */
	ST_DELPOINT(pStr)
	return res;
}

/*----------------------------------------------------------------------------*/

/* read <fldadd>/<fldset> */
static Tst_err
AST_CLN__hnd_read_t_fldaddset(Tast_cln_a *pCmdln, const char *pOptCap,
		const Tst_bool addOrSet,
		Tast_cln_t_fldDscAddSet *pFldAddSet,
		Tst_bool *pShowHelp, Tst_str *pWhichHelp)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pStr = NULL;

	pWhichHelp[0] = 0;

	res = ast_cln_fnc_cpyarg_str(pCmdln, pOptCap, AST_CLN_MAX_ARGLEN, &pStr);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pStr)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting",
				(addOrSet ? AST_CLN_SVT_FMT_FLDADD : AST_CLN_SVT_FMT_FLDSET));
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		Tst_uint32 lenArgHML = st_sysStrlen2(AST_CLN_ARG_HLP),
		           lenArgHMS = st_sysStrlen2(AST_CLN_ARG_HLPSH),
		           lenArgHV  = st_sysStrlen2(AST_CLN_ARG_HLPVALS),
		           lenStr    = st_sysStrlen(pStr);

		if ((lenStr == lenArgHML &&
					st_sysStrcmp2(ST_B_TRUE, pStr, AST_CLN_ARG_HLP)) ||
				(lenStr == lenArgHMS &&
					st_sysStrcmp2(ST_B_TRUE, pStr, AST_CLN_ARG_HLPSH))) {
			/* help | h */
			*pShowHelp    = ST_B_TRUE;
			pWhichHelp[0] = 'm';
			pWhichHelp[1] = 'a';
		} else if (lenStr >= lenArgHV &&
				st_sysStrcmpN2(ST_B_FALSE, lenArgHV, pStr, AST_CLN_ARG_HLPVALS)) {
			if (lenStr == lenArgHV + 3 &&
					st_sysStrcmpN2(ST_B_FALSE, lenArgHV, pStr, AST_CLN_ARG_HLPVALS)) {
				/* helpvalsIV2 | helpvalsAV2 | helpvalsVOR */
				*pShowHelp    = ST_B_TRUE;
				pWhichHelp[0] = pStr[lenArgHV];
				pWhichHelp[1] = pStr[lenArgHV + 1];
				pWhichHelp[2] = pStr[lenArgHV + 2];
			} else {
				/* helpvals* */
				*pShowHelp = ST_B_TRUE;
			}
		} else if (! ast_cln_psvt_fldaddset(pCmdln, pStr, addOrSet, pFldAddSet))
			res = ST_ERR_ABRT;
	}
	/* */
	ST_DELPOINT(pStr)
	return res;
}

/*----------------------------------------------------------------------------*/

/* read <dscset> */
static Tst_err
AST_CLN__hnd_read_t_dscset(Tast_cln_a *pCmdln, const char *pOptCap,
		Tast_cln_t_fldDscAddSet *pDscSet,
		Tst_bool *pShowHelpMain, Tst_bool *pShowHelpVals)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_str *pStr = NULL;

	res = ast_cln_fnc_cpyarg_str(pCmdln, pOptCap, AST_CLN_MAX_ARGLEN, &pStr);
	if (res == ST_ERR_SUCC && st_sysStrEmpty(pStr)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"no <%s> given, aborting", AST_CLN_SVT_FMT_DSCSET);
		res = ST_ERR_ABRT;
	}
	if (res == ST_ERR_SUCC) {
		Tst_uint32 lenArgHML = st_sysStrlen2(AST_CLN_ARG_HLP),
		           lenArgHMS = st_sysStrlen2(AST_CLN_ARG_HLPSH),
		           lenArgHV  = st_sysStrlen2(AST_CLN_ARG_HLPVALS),
		           lenStr    = st_sysStrlen(pStr);

		if ((lenStr == lenArgHML &&
					st_sysStrcmp2(ST_B_TRUE, pStr, AST_CLN_ARG_HLP)) ||
				(lenStr == lenArgHMS &&
					st_sysStrcmp2(ST_B_TRUE, pStr, AST_CLN_ARG_HLPSH))) {
			/* help | h */
			*pShowHelpMain = ST_B_TRUE;
		} else if ((lenStr >= lenArgHV && lenStr <= lenArgHV + 3) &&
				st_sysStrcmpN2(ST_B_FALSE, lenArgHV, pStr, AST_CLN_ARG_HLPVALS)) {
			/* helpvals */
			if ((lenStr > lenArgHV && lenStr < lenArgHV + 3) ||
					(lenStr == lenArgHV + 3 &&
						((const char)pStr[lenArgHV] != 'I' ||
							(const char)pStr[lenArgHV + 1] != 'V' ||
							(const char)pStr[lenArgHV + 2] != '2'))) {
				pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
						"unknown help page requested");
				res = ST_ERR_ABRT;
			} else
				*pShowHelpVals = ST_B_TRUE;
		} else if (! ast_cln_psvt_dscset(pCmdln, pStr, pDscSet))
			res = ST_ERR_ABRT;
	}
	/* */
	ST_DELPOINT(pStr)
	return res;
}

/******************************************************************************/
