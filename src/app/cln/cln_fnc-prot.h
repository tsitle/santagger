/******************************************************************************/
/* cln_fnc-prot.h                 [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions etc. for parsing program options                   */
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

#ifndef APPSANTAGGER_CLN_FNC_PROT_H
#define APPSANTAGGER_CLN_FNC_PROT_H

/*
// Own-Includes
*/
#if (LIBSANTAGGER_CFG_EXTRA_DEBUG == 1)
#	define AST_CLN_FNC_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_CLN_FNC_C) || defined(SRC_CLN_PARGS_C)
	/** */
	Tst_err
	ast_cln_fnc_checkForAmbig(const Tst_str *pAppFn,
	                          const char *pFnc,
	                          const Tst_uint32 argc,
	                          const Tst_str **argv,
	                          Tast_cln_a *pCmdln);
	/** */
	void
	ast_cln_fnc_countLongopts(const Tast_cln_longopts *pLOpts,
	                          Tst_uint32 *pCnt);
	void
	ast_cln_fnc_setLongoptsCache(const Tast_cln_longopts *pLOpts,
	                             const Tst_uint32 cnt,
	                             Tast_cln_longoptCache *pLOCarr);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_CLN_FNC_C) || defined(SRC_CLN_PARGS_C) || \
			defined(SRC_CLN_HNDARG_C)
	/** */
	Tst_err
	ast_cln_fnc_cpyarg_str(const Tast_cln_a *pCmdln,
	                       const char *pCap,
	                       const Tst_uint32 maxlen,
	                       Tst_str **ppStr);
	Tst_err
	ast_cln_fnc_cpyarg_i32(const Tast_cln_a *pCmdln,
	                       const char *pCap,
	                       const Tst_int32 numMin,
	                       const Tst_int32 numMax,
	                       const Tst_int32 numNone,
	                       Tst_int32 *pInt);
	Tst_err
	ast_cln_fnc_cpyarg_i32OrStr(const Tast_cln_a *pCmdln,
	                            const char *pCap,
	                            const Tst_int32 numMin,
	                            const Tst_int32 numMax,
	                            const Tst_int32 numNone,
	                            const Tst_int32 strMaxlen,
	                            Tst_int32 *pInt,
	                            Tst_str **ppStr);
#endif  /* protected */


/*
// Functions (protected), Part III
*/
#if defined(SRC_CLN_FNC_C) || defined(SRC_CLN_PSVT_C) || \
			defined(SRC_CLN_HNDARG_C) || defined(SRC_CLN_HNDHELP_C) || \
			defined(SRC_CLN_HNDHELP_FNC_C) || defined(SRC_CLN_PARGS_C)
	/** */
	Tst_bool
	ast_cln_fnc_strcmp2x(ST_OPTARG(const char *pArg),
	                     ST_OPTARG(const char *pOpt1),
	                     ST_OPTARG(const char *pOpt2));
	Tst_bool
	ast_cln_fnc_strcmp2xN(ST_OPTARG(const char *pArg),
	                      const Tst_uint32 lenArg,
	                      ST_OPTARG(const char *pOpt1),
	                      ST_OPTARG(const char *pOpt2));

	/** */
#	if (AST_CLN_FNC_DEB_ == 1)
	void ast_cln_prf(char *pFmt, ...);
#	endif
#endif  /* protected */


/*
// Functions (protected), Part IV
*/
#if defined(SRC_CLN_FNC_C) || defined(SRC_MAIN_C) || \
			defined(SRC_MF_CNVMRG_C)
	/** */
	Tst_bool
	ast_cln_fnc_isAnySetInTaglist(const Tast_cln_t_tagList_pb(pList));
#endif  /* protected */


ST_END_C_DECLS

#endif  /* APPSANTAGGER_CLN_FNC_PROT_H */

/******************************************************************************/
