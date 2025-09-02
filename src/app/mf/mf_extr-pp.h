/******************************************************************************/
/* mf_extr-pp.h                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for extracting data from Tags etc.             */
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

#ifndef APPSANTAGGER_MF_EXTR_PP_H
#define APPSANTAGGER_MF_EXTR_PP_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj-defs.h"
/** */
#include "mf-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_EXTR_C)
	/** */
	Tst_err ast_mf_extr_extrData(const Tast_cln_a *pCmdln,
	                             Tast_mf_finfo *pMF);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_MF_EXTR_C
	/** */
	static Tst_err
	AST_MF__extr_extrTag(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);

	/** */
	static Tst_err
	AST_MF__extr_extrOther(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);

	/** */
	static Tst_err
	AST_MF__extr_extrTag_sub(const Tast_cln_a *pCmdln,
	                         Tst_sys_fstc *pFStcIn,
	                         const char *pTDesc,
	                         const char *pFnExt,
	                         const Tst_foffs offs,
	                         const Tst_uint32 binSize,
	                         Tst_vorbc_tag *pVor,
	                         Tst_uint32 vorBsIx,
	                         Tst_uint32 vorBsSIx);
	/** */
	static Tst_err
	AST_MF__extr_extrOther_sub(const Tast_cln_a *pCmdln,
	                           const Tst_str *pFnIn,
	                           const char *pTDesc,
	                           const char *pDDesc,
	                           const Tst_bool extrAll,
	                           ST_OPTARG(const Tst_uint32 *pIxArr),
	                           const Tst_id3v2_frID fldID,
	                           const Tst_id3v2_frTp fldType,
	                           Tst_id3v2_tag *pTag,
	                           const Tst_uint32 tagNr);
#endif  /* private */


ST_END_C_DECLS

#endif  /* APPSANTAGGER_MF_EXTR_PP_H */

/******************************************************************************/
