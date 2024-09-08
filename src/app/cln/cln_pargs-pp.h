/******************************************************************************/
/* cln_pargs-pp.h                 [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Private/Protected functions for parsing program options                    */
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

#ifndef HEAD_CLN_PARGS_PP_H
#define HEAD_CLN_PARGS_PP_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (private)
*/
#ifdef SRC_CLN_PARGS_C
	/** short options
	 **   options that require an argument are followed by a ':'  */
	static const char *AST_CLN_SHORTOPTS = "b:a:s:t:d:y:c:g:hvrpq";

	/** long options */
	static const Tast_cln_longopts AST_CLN_LONGOPTS[] = {
			{AST_CLN_CMD_RD_LO,        no_argument,       NULL, AST_CLN_CMD_RD_SO},
			{AST_CLN_CMD_ANA_LO,       no_argument,       NULL, 0},
			{AST_CLN_CMD_DECAUD_LO,    required_argument, NULL, 0},
			/** */
			{AST_CLN_CMD_EXTR_TAG_LO,  required_argument, NULL, 0},
			{AST_CLN_CMD_EXTR_PIC_LO,  required_argument, NULL, 0},
			{AST_CLN_CMD_EXTR_GEO_LO,  required_argument, NULL, 0},
			{AST_CLN_CMD_EXTR_BD_LO,   no_argument,       NULL, 0},
			/** */
			{AST_CLN_CMD_BND_LO,       required_argument, NULL, AST_CLN_CMD_BND_SO},
			{AST_CLN_CMD_ALB_LO,       required_argument, NULL, AST_CLN_CMD_ALB_SO},
			{AST_CLN_CMD_SNG_LO,       required_argument, NULL, AST_CLN_CMD_SNG_SO},
			{AST_CLN_CMD_TRK_LO,       required_argument, NULL, AST_CLN_CMD_TRK_SO},
			{AST_CLN_CMD_TRK_TOT_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_DSC_LO,       required_argument, NULL, AST_CLN_CMD_DSC_SO},
			{AST_CLN_CMD_DSC_TOT_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_YEA_LO,       required_argument, NULL, AST_CLN_CMD_YEA_SO},
			{AST_CLN_CMD_COM_LO,       required_argument, NULL, AST_CLN_CMD_COM_SO},
			{AST_CLN_CMD_GEN_LO,       required_argument, NULL, AST_CLN_CMD_GEN_SO},
			{AST_CLN_CMD_ADD_PIC_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_ADD_GEO_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_MOD_PIC_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_MOD_GEO_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_REM_PIC_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_REM_GEO_LO,   required_argument, NULL, 0},
			{AST_CLN_CMD_FADD_LO,      required_argument, NULL, 0},
			{AST_CLN_CMD_FSET_LO,      required_argument, NULL, 0},
			{AST_CLN_CMD_DSET_LO,      required_argument, NULL, 0},
			/** */
			{AST_CLN_CMD_REWR_LO,      no_argument,       NULL, 0},
			/** */
			{AST_CLN_CMD_CONV_LO,      required_argument, NULL, 0},
			/** */
			{AST_CLN_CMD_REM_TAG_LO,   required_argument, NULL, 0},
			/** */
			{AST_CLN_CMD_HLP_LO,       no_argument,       NULL, AST_CLN_CMD_HLP_SO},
			{AST_CLN_CMD_VER_LO,       no_argument,       NULL, 0},
			{AST_CLN_CMD_INF_LO,       no_argument,       NULL, 0},
			{AST_CLN_CMD_LIC_LO,       no_argument,       NULL, 0},
			{AST_CLN_CMD_EXA_LO,       no_argument,       NULL, 0},
			/** */
			{AST_CLN_OPT_VERBLEV_LO,   required_argument, NULL, 0},
			{AST_CLN_OPT_PRT_LO,       no_argument,       NULL, AST_CLN_OPT_PRT_SO},
			{AST_CLN_OPT_QSCAN_LO,     no_argument,       NULL, 0},
			{AST_CLN_OPT_ALLWLNKS_LO,     no_argument,    NULL, 0},
			{AST_CLN_OPT_DISALLWLNKS_LO,  no_argument,    NULL, 0},
			{AST_CLN_OPT_EBS_LO,       required_argument, NULL, 0},
			{AST_CLN_OPT_DISP_ALL_LO,  no_argument,       NULL, 0},
			{AST_CLN_OPT_DISP_DET_LO,  no_argument,       NULL, 0},
			{AST_CLN_OPT_DISP_FS_LO,   no_argument,       NULL, 0},
			{AST_CLN_OPT_DISP_ISO_LO,  no_argument,       NULL, 0},
			{AST_CLN_OPT_DISP_U8_LO,   no_argument,       NULL, 0},
			{AST_CLN_OPT_TAG_SW_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_TAG_ED_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_FRC_MP1_LO,   no_argument,       NULL, 0},
			{AST_CLN_OPT_FRC_MP2_LO,   no_argument,       NULL, 0},
			{AST_CLN_OPT_FRC_MP3_LO,   no_argument,       NULL, 0},
			{AST_CLN_OPT_FRC_AV1_LO,   no_argument,       NULL, 0},
			{AST_CLN_OPT_FRC_AV2_LO,   no_argument,       NULL, 0},
			{AST_CLN_OPT_FRC_AV25_LO,  no_argument,       NULL, 0},
			{AST_CLN_OPT_TIV2_LO,      required_argument, NULL, 0},
			{AST_CLN_OPT_BPS_LO,       required_argument, NULL, 0},
			{AST_CLN_OPT_PIC_TP_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_PIC_FM_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_PIC_DE_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_PIC_FN_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_GEO_DE_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_GEO_MI_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_GEO_FN_LO,    required_argument, NULL, 0},
			/** */
			{AST_CLN_OPT_OUTDIR_LO,    required_argument, NULL, 0},
			{AST_CLN_OPT_OWEXF_LO,     no_argument,       NULL, 0},
			/** */
			{NULL, 0, NULL, 0} };

	/** */
	const Tast_cln_t_verblevList_capDesc AST_CLN_VERBLIST_CAPDESC[] = {
			{ST_VL_GEN,    "gen", NULL, "general info"},
			{ST_VL_OPTS,   "opt", NULL, "cmdline options"},
			{ST_VL_TVREAD, "trd", NULL, "read tag field values"},
			{ST_VL_TVSET,  "tst", NULL, "set tag field vals"},
			{ST_VL_TVWRIT, "twr", NULL, "written tag field vals"},
			{ST_VL_TAG,    "tag", NULL, "tag info (overwrites trd/tst/twr)"},
			{ST_VL_AUD,    "aud", NULL, "audio info"},
			{ST_VL_AUDDET, "dau", NULL, "detailed audio info"},
			/* */
			{255, AST_CLN_ARG_ALL,  AST_CLN_ARG_ALLSH,  "enable all levels"},
			{0,   AST_CLN_ARG_NONE, AST_CLN_ARG_NONESH, "disable all levels"},
			{-1, NULL, NULL, NULL} };

	/** */
	const Tast_cln_t_tagList_capDesc AST_CLN_TAGLIST_CAPDESC[] = {
			{AST_CLN_T_TAGLIST_IV1,
				AST_CLN_ARG_TIV1, AST_CLN_ARG_TIV1SH, "ID3v1 Tag"},
			{AST_CLN_T_TAGLIST_IV2,
				AST_CLN_ARG_TIV2, AST_CLN_ARG_TIV2SH, "ID3v2 Tag"},
			{AST_CLN_T_TAGLIST_AV2,
				AST_CLN_ARG_TAV2, AST_CLN_ARG_TAV2SH, "APEv2 Tag"},
			{AST_CLN_T_TAGLIST_VOR,
				AST_CLN_ARG_TVOR, AST_CLN_ARG_TVORSH, "Vorbis Tag"},
			{AST_CLN_T_TAGLIST_MRG,
				AST_CLN_ARG_TMRG, AST_CLN_ARG_TMRGSH, "Merged Tags"},
			/* */
			{-1,
				AST_CLN_ARG_ALL,  AST_CLN_ARG_ALLSH,  "all Tags"},
			{-1,
				AST_CLN_ARG_AUTO, AST_CLN_ARG_AUTOSH, "auto select"},
			{-1,
				AST_CLN_ARG_NONE, AST_CLN_ARG_NONESH, "no Tag(s)"},
			{-1, NULL, NULL, NULL} };

	/** */
	const Tast_cln_t_tiv2optList_capDesc AST_CLN_TIV2OPTLIST_CAPDESC[] = {
			{AST_CLN_ARG_TIV2O_CMPR,   "allow compression"},
			{AST_CLN_ARG_TIV2O_NOCMPR, "disallow compression (recommended)"},
			{AST_CLN_ARG_TIV2O_UNSY,   "allow unsynchronization"},
			{AST_CLN_ARG_TIV2O_NOUNSY, "disallow unsynchronization (recommended)"},
			{AST_CLN_ARG_TIV2O_V2,     "save as version v2.2"},
			{AST_CLN_ARG_TIV2O_V3,     "save as version v2.3 (recommended)"},
			{AST_CLN_ARG_TIV2O_V4,     "save as version v2.4"},
			{NULL, NULL} };
#endif  /* private */


/*
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_CLN_PARGS_C)
	/** */
	Tst_err
	ast_cln_pa_parseArgs(const Tst_str *pAppFn,
	                     const Tst_uint32 argc,
	                     const Tst_str **argv,
	                     Tast_cln_a *pCmdln,
	                     Tst_uint32 *pParbeg);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_CLN_PARGS_C
	/** */
	static Tst_err
	AST_CLN__pa_printUsage(const Tst_str *pAppFn,
	                       const Tast_cln_cbMsg cbMsg);
	/** handle special commands */
	/*** */
	static Tst_err
	AST_CLN__pa_sc_help(const Tst_str *pAppFn,
	                    const Tast_cln_cbMsg cbMsg,
	                    const Tst_bool isInpISOorU8);
	static void
	AST_CLN__pa_sc_help_prOne(const Tast_cln_cbMsg cbMsg,
	                          const char opt1,
	                          const char *pOpt2,
	                          const char *pAddOpt,
	                          const char *pVals,
	                          const char *pDesc,
	                          const Tst_bool onlyDiv);
	/*** */
	static Tst_err
	AST_CLN__pa_sc_info(const Tast_cln_cbMsg cbMsg, const Tst_str* pAppFn);
	/*** */
	static Tst_err
	AST_CLN__pa_sc_version(const Tast_cln_cbMsg cbMsg);
	/*** */
	static Tst_err
	AST_CLN__pa_sc_license(const Tast_cln_cbMsg cbMsg);
	/*** */
	static Tst_err
	AST_CLN__pa_sc_examples(const Tast_cln_cbMsg cbMsg, const Tst_str* pAppFn);

	/** */
	static Tst_err
	AST_CLN__pa_hndArg(const Tst_int32 ch,
	                   const Tst_int32 oix,
	                   const Tst_uint32 cntarr,
	                   Tast_cln_a *pCmdln);

	static Tst_err
	AST_CLN__pa_hndArgLong(const Tst_int32 oix, Tast_cln_a *pCmdln);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CLN_PARGS_PP_H */

/******************************************************************************/
