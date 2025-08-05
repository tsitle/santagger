/******************************************************************************/
/* mf_show_av-pp.h                [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for displaying Audio/Video info                */
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

#ifndef HEAD_MF_SHOW_AV_PP_H
#define HEAD_MF_SHOW_AV_PP_H

/*
// Own-Includes
*/
/** */
#include "mf-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_MF_SHOW_AV_C) || defined(SRC_MF_SHOW_C)
	/** */
	Tst_err
	ast_mf_sw_showMPEG1stream(const Tast_cln_a *pCmdln,
	                          const Tst_bool printVBRhd,
	                          const Tst_mpeg1_obj *pMO);
	/** */
	Tst_err
	ast_mf_sw_showOGGstreams(const Tast_cln_a *pCmdln,
	                         const Tst_contOgg_obj *pOgg);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_MF_SHOW_AV_C) || defined(SRC_MF_SHOW_TIV2_C)
	/** */
	void
	ast_mf_sw_printOGGstrNr(const Tst_contOgg_obj *pAObj,
	                        const Tst_uint32 bsIx,
	                        const Tst_uint32 bsSIx,
	                        const Tst_bool prForAudioOrTag,
	                        Tst_bool *pIs1Grp,
	                        Tst_uint32 *pDelML);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_MF_SHOW_AV_C
	/** */
	static Tst_err
	AST_MF__sw_shortPCh(const Tst_str *pPChar,
	                    Tst_str **ppShort,
	                    const Tst_uint32 delML);

	/** */
	static void
	AST_MF__sw_printPlaytime(const Tst_av_playtime *pPTime,
	                         const Tst_bool isCirca);

#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_SHOW_AV_PP_H */

/******************************************************************************/
