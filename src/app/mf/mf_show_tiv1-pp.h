/******************************************************************************/
/* mf_show_tiv1-pp.h              [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for displaying ID3v1 Tags                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef APPSANTAGGER_MF_SHOW_TIV1_PP_H
#define APPSANTAGGER_MF_SHOW_TIV1_PP_H

/*
// Own-Includes
*/
/** */
#include "mf-prot.h"

#define AST_MF_SW_DEBUG_IV1  0  /* set to 1 to use ast_mf_sw_showTagIV1() */


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_C)
	/** */
#	if (AST_MF_SW_DEBUG_IV1 == 1)
	Tst_err
	ast_mf_sw_showTagIV1(const Tast_cln_a *pCmdln, const Tst_id3v1_tag *pTag);
#	endif
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_MF_SHOW_TIV1_C
	/** */
#	if (AST_MF_SW_DEBUG_IV1 == 1)
	static void
	AST_MF__sw_sTIV1_prStr_simple(const Tst_mtes_string *pStr,
	                              const Tst_bool wNln,
	                              const Tst_uint32 defwid,
	                              const Tst_bool dispAsISOorU8);
	static void
	AST_MF__sw_sTIV1_prGen(const Tst_byte gen);
#	endif
#endif  /* private */


ST_END_C_DECLS

#endif  /* APPSANTAGGER_MF_SHOW_TIV1_PP_H */

/******************************************************************************/
