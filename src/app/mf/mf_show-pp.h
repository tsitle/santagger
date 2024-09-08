/******************************************************************************/
/* mf_show-pp.h                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Private/Protected functions etc. for displaying Tags and Audio/Video info  */
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

#ifndef HEAD_MF_SHOW_PP_H
#define HEAD_MF_SHOW_PP_H

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
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_SHOW_C)
	/** */
	Tst_err
	ast_mf_sw_showFileInfo(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_MF_SHOW_C
	/** */
	static void
	AST_MF__sw_tags(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_SHOW_PP_H */

/******************************************************************************/
