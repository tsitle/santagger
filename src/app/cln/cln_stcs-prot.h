/******************************************************************************/
/* cln_stcs-prot.h                [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for program option structures                          */
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

#ifndef HEAD_CLN_STCS_PROT_H
#define HEAD_CLN_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_CLN_STCS_C) || defined(SRC_MAIN_C)
	/** */
	void   ast_cln_stc_initCln(Tast_cln_a *pCmd);
	void   ast_cln_stc_rsetCln(Tast_cln_a *pCmd);
	void   ast_cln_stc_freeCln(Tast_cln_a *pCmd);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_CLN_STCS_C) || \
			defined(SRC_CLN_AF_C) || defined(SRC_CLN_FNC_C)
	/** */
	void   ast_cln_stc_initAF(Tast_cln_attFile *pAF);
	void   ast_cln_stc_rsetAF(Tast_cln_attFile *pAF);
#	define ast_cln_stc_freeAF  ast_cln_stc_rsetAF

	/** */
	void    ast_cln_stc_initAFarr(Tast_cln_attFileArr *pAFarr);
	void    ast_cln_stc_rsetAFarr(Tast_cln_attFileArr *pAFarr);
#	define  ast_cln_stc_freeAFarr  ast_cln_stc_rsetAFarr
	Tst_err ast_cln_stc_reszAFarr(Tast_cln_attFileArr *pAFarr,
	                              const Tst_uint32 newSz);
#endif  /* protected */


/*
// Functions (protected), Part III
*/
#if defined(SRC_CLN_STCS_C) || \
			defined(SRC_CLN_HNDARG_C) || defined(SRC_CLN_HNDHELP_C) || \
			defined(SRC_CLN_PSVT_C) || defined(SRC_CLN_SETDEFS_C) || \
			defined(SRC_MF_STCS_C) || defined(SRC_MF_EDIT_C) || \
			defined(SRC_MF_SHOW_C)
	/** special value types */
	/*** */
	void   ast_cln_stc_rsetT_taglist(Tast_cln_t_tagList_pb(pList),
	                                 const Tst_bool setTo);
	/*** */
	void   ast_cln_stc_rsetT_tagpair(Tast_cln_t_tagPair *pTP);
	/*** */
	void   ast_cln_stc_initT_flddscaddset(Tast_cln_t_fldDscAddSet *pFP);
	void   ast_cln_stc_rsetT_flddscaddset(Tast_cln_t_fldDscAddSet *pFP);
#	define ast_cln_stc_freeT_flddscaddset  ast_cln_stc_rsetT_flddscaddset
	/*** */
	void    ast_cln_stc_rsetT_tiv2opt(Tast_cln_t_tiv2opt *pTOpt);
#endif  /* protected */


/*
// Functions (protected), Part IV
*/
#if defined(SRC_CLN_STCS_C) || defined(SRC_CLN_HNDARG_C) || \
			defined(SRC_CLN_SETDEFS_C)
	/** */
	void ast_cln_stc_resetCmdsTg(Tast_cln_a *pCmdln,
	                             const Tst_bool roMain,
	                             const Tst_bool roExt,
	                             const Tst_bool rwRwr,
	                             const Tst_bool rwEdt,
	                             const Tst_bool rwRem,
	                             const Tst_bool rwCnv);
#endif  /* protected */


/*
// Functions (private)
*/
#if defined(SRC_CLN_STCS_C)
	/** */
	static void
	AST_CLN__stc_initOpts(Tast_cln_a *pCmd);
	static void
	AST_CLN__stc_rsetOpts(Tast_cln_a *pCmd);
	static void
	AST_CLN__stc_freeOpts(Tast_cln_a *pCmd);

	/** */
	static void
	AST_CLN__stc_initCOTF(Tast_cln_optsTagFlds *pCOTF);
	static void
	AST_CLN__stc_rsetCOTF(Tast_cln_optsTagFlds *pCOTF);
	static void
	AST_CLN__stc_freeCOTF(Tast_cln_optsTagFlds *pCOTF);

	/** */
	static void
	AST_CLN__stc_initCOAM(Tast_cln_optsAvMisc *pCOAM);
	static void
	AST_CLN__stc_rsetCOAM(Tast_cln_optsAvMisc *pCOAM);
#	define AST_CLN__stc_freeCOAM  AST_CLN__stc_rsetCOAM

	/** */
	static void
	AST_CLN__stc_rsetCOD(Tast_cln_optsDisp *pCOD);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CLN_STCS_PROT_H */

/******************************************************************************/
