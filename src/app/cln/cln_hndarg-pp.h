/******************************************************************************/
/* cln_hndarg-pp.h                [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for handling program parameters                */
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

#ifndef HEAD_CLN_HNDARG_PP_H
#define HEAD_CLN_HNDARG_PP_H

/*
// Own-Includes
*/
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CLN_HNDARG_C) || defined(SRC_CLN_PARGS_C)
	/** commands */
	/*** */
	Tst_err ast_cln_hnd_c_rd(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_ana(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_decAud(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_c_tfld_bnd(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_alb(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_sng(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_tnr(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_tnr_tot(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_dsc(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_dsc_tot(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_yea(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_com(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_gen(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_add_picOrGeo(Tast_cln_a *pCmdln,
	                                        const Tst_bool isPic);
	Tst_err ast_cln_hnd_c_tfld_mod_picOrGeo(Tast_cln_a *pCmdln,
	                                        const Tst_bool isPic);
	Tst_err ast_cln_hnd_c_tfld_rem_picOrGeo(Tast_cln_a *pCmdln,
	                                        const Tst_bool isPic);
	Tst_err ast_cln_hnd_c_tfld_fadd(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_fset(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_tfld_dset(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_c_rewr(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_c_rem_tag(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_c_extr_tag(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_c_extr_picOrGeo(Tast_cln_a *pCmdln,
	                                    const Tst_bool isPic);
	Tst_err ast_cln_hnd_c_extr_bd(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_c_conv(Tast_cln_a *pCmdln);

	/** options */
	/*** */
	Tst_err ast_cln_hnd_o_quiet(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_swstat(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_verblev(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_prt(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_o_qscan(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_o_allwlnks(Tast_cln_a *pCmdln,
	                               const Tst_bool allw);
	/*** */
	Tst_err ast_cln_hnd_o_disp_all(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_disp_det(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_disp_fstr(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_disp_isoOrU8(Tast_cln_a *pCmdln,
	                                   const Tst_bool isISO);
	/*** */
	Tst_err ast_cln_hnd_o_tag_swOrEd(Tast_cln_a *pCmdln,
	                                 const Tst_int32 which);
	/*** */
	Tst_err ast_cln_hnd_o_ebs(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_o_force_mpX(Tast_cln_a *pCmdln,
	                                const Tst_int32 lay);
	Tst_err ast_cln_hnd_o_force_avX(Tast_cln_a *pCmdln,
	                                const Tst_int32 av);
	/*** */
	Tst_err ast_cln_hnd_o_bps(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_o_tiv2(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_o_pic_tp(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_pic_fm(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_pic_de(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_geo_mi(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_geo_de(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_pic_fn(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_geo_fn(Tast_cln_a *pCmdln);
	/*** */
	Tst_err ast_cln_hnd_o_od(Tast_cln_a *pCmdln);
	Tst_err ast_cln_hnd_o_ow(Tast_cln_a *pCmdln);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_CLN_HNDARG_C
	/** */
	static Tst_err
	AST_CLN__hnd_readAndAddToPrio3List(Tast_cln_a *pCmdln,
	                                   const char *pOptCap,
	                                   const Tast_cln_t_tagListIx ttp,
	                                   const char *pFldName,
	                                   ST_OPTARG(const Tst_str *pSrcStr),
	                                   const int readStrOrIntOrBoth,
	                                   const Tst_int32 intMin,
	                                   const Tst_int32 intMax,
	                                   const Tst_int32 intNone);
	/** */
	static Tst_err
	AST_CLN__hnd_c_tfld_faddset(const char *pFnc,
	                            Tast_cln_a *pCmdln,
	                            const Tst_bool addOrSet);

	/** parse specific value types */
	/*** */
	static Tst_err
	AST_CLN__hnd_read_t_taglist(Tast_cln_a *pCmdln,
	                            const char *pOptCap,
	                            const Tast_cln_t_tagList_pb(pAllwTags),
	                            const Tst_bool autoAllw,
	                            const Tst_bool nonAllw,
	                            Tast_cln_t_tagList_pb(pOutTags),
	                            Tst_bool *pAuto,
	                            Tst_bool *pNone);
	/*** */
	static Tst_err
	AST_CLN__hnd_read_t_numlist(Tast_cln_a *pCmdln,
	                            const char *pOptCap,
	                            Tst_uint32 **ppILst,
	                            Tst_bool *pAll);
	/*** */
	static Tst_err
	AST_CLN__hnd_read_t_pos(Tast_cln_a *pCmdln,
	                        const char *pOptCap,
	                        const char *pOptCapTot,
	                        Tst_int32 *pPosNr,
	                        Tst_int32 *pPosTot);
	/*** */
	static Tst_err
	AST_CLN__hnd_read_t_fldaddset(Tast_cln_a *pCmdln,
	                              const char *pOptCap,
	                              const Tst_bool addOrSet,
	                              Tast_cln_t_fldDscAddSet *pFldAddSet,
	                              Tst_bool *pShowHelp,
	                              Tst_str *pWhichHelp);
	/*** */
	static Tst_err
	AST_CLN__hnd_read_t_dscset(Tast_cln_a *pCmdln,
	                           const char *pOptCap,
	                           Tast_cln_t_fldDscAddSet *pDscSet,
	                           Tst_bool *pShowHelpMain,
	                           Tst_bool *pShowHelpVals);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CLN_HNDARG_PP_H */

/******************************************************************************/
