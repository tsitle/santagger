/******************************************************************************/
/* mf_stcs-pp.h                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for structures for 'main functions'            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MF_STCS_PP_H
#define HEAD_MF_STCS_PP_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "mf-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_STCS_C)
	/** */
	Tst_err ast_mf_stc_initMF(Tast_mf_finfo *pMF);
	void    ast_mf_stc_rsetMF(Tast_mf_finfo *pMF);
	void    ast_mf_stc_freeMF(Tast_mf_finfo *pMF);

	/** */
	void ast_mf_stc_setCBsDeb(Tst_callbacks_dbg *pCBs);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_MF_STCS_C) || defined(SRC_MF_CNVMRG_C) || \
			defined(SRC_MF_EDIT_C)
	/** */
	void     ast_mf_stc_initEditInfo(Tast_mf_editInfo *pEdInf);
	void     ast_mf_stc_rsetEditInfo(Tast_mf_editInfo *pEdInf);
#	define   ast_mf_stc_freeEditInfo  ast_mf_stc_rsetEditInfo
	Tst_err  ast_mf_stc_editInfo_addUIDtoObs(Tast_mf_editInfo *pEdInf,
	                                         const Tst_uint32 uid);
	Tst_bool ast_mf_stc_editInfo_isUIDinObs(const Tast_mf_editInfo *pEdInf,
	                                        const Tst_uint32 uid);
	Tst_err  ast_mf_stc_editInfo_addUIDtoUsd(Tast_mf_editInfo *pEdInf,
	                                         const Tst_uint32 uid);
	Tst_bool ast_mf_stc_editInfo_isUIDinUsd(const Tast_mf_editInfo *pEdInf,
	                                        const Tst_uint32 uid);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_MF_STCS_C
	/** */
	static void
	AST_MF__stc_rsetSetOptsData(Tast_mf_setOptsData *pSOD);
	/** */
	static Tst_err
	AST_MF__stc_editInfo_addUIDtoArr(Tast_mf_editInfo_uidArr *pUA,
	                                 const Tst_uint32 uid);
	static Tst_bool
	AST_MF__stc_editInfo_isUIDinArr(const Tast_mf_editInfo_uidArr *pUA,
	                                const Tst_uint32 uid);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_STCS_PP_H */

/******************************************************************************/
