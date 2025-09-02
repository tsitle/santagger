/******************************************************************************/
/* mf_edit-pp.h                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions etc. for editing Audio/Video files             */
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

#ifndef APPSANTAGGER_MF_EDIT_PP_H
#define APPSANTAGGER_MF_EDIT_PP_H

/*
// Own-Includes
*/
/** */
#include "mf-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_EDIT_C)
	/** */
	Tst_err
	ast_mf_ed_editFile(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_MF_EDIT_C
	/** */
	static Tst_bool
	AST_MF__ed_haveTag(Tast_mf_finfo *pMF,
	                   const Tast_cln_t_tagListIx tlix);
	/** */
	static Tst_err
	AST_MF__ed_cmdRemTags(const Tast_cln_a *pCmdln,
	                      Tast_mf_finfo *pMF,
	                      Tast_mf_editInfo *pEdInf,
	                      Tst_uint32 *pRemTagsCnt);
	/** */
	static Tst_err
	AST_MF__ed_mergeTagsWithCmdln(Tast_cln_a *pCmdln,
	                              Tast_mf_finfo *pMF,
	                              Tast_mf_editInfo *pEdInf);
	/** */
	static Tst_err
	AST_MF__ed_cmdCnvTags(const Tast_cln_a *pCmdln,
	                      Tast_mf_finfo *pMF,
	                      Tast_mf_editInfo *pEdInf,
	                      Tst_uint32 *pCnvTagsCnt);
	/** */
	static Tst_err
	AST_MF__ed_cnvremInvalidTags(const Tast_cln_a *pCmdln,
	                             Tast_mf_finfo *pMF,
	                             Tast_mf_editInfo *pEdInf);
	/** */
	static void
	AST_MF__ed_setMainTags(const Tast_cln_a *pCmdln,
	                       Tast_mf_finfo *pMF,
	                       Tast_mf_editInfo *pEdInf);
	/** */
	static Tst_err
	AST_MF__ed_rewriteFile(const Tast_cln_a *pCmdln,
	                       Tast_mf_finfo *pMF,
	                       Tast_mf_editInfo *pEdInf);
#endif  /* private */


ST_END_C_DECLS

#endif  /* APPSANTAGGER_MF_EDIT_PP_H */

/******************************************************************************/
