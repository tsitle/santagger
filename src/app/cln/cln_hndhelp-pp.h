/******************************************************************************/
/* cln_hndhelp-pp.h               [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for handling help pages                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CLN_HNDHELP_PP_H
#define HEAD_CLN_HNDHELP_PP_H

/*
// Own-Includes
*/
/** */
#include "src/includes/tag/tag_apev2-defs.h"
#include "src/includes/tag/tag_id3v2-defs.h"
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CLN_HNDHELP_C) || defined(SRC_CLN_HNDARG_C) || \
			defined(SRC_CLN_PARGS_C)
	/** help pages for commands */
	/*** */
	void
	ast_cln_hhlp_show_c_genre(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlp_show_c_genreValues(Tast_cln_a *pCmdln,
	                                const Tst_bool full);
	/*** */
	void
	ast_cln_hhlp_show_c_faddset(Tast_cln_a *pCmdln,
	                            const char *pOptCap,
	                            const Tst_bool addOrSet);
	void
	ast_cln_hhlp_show_c_faddsetValues(Tast_cln_a *pCmdln,
	                                  const char *pOptCap,
	                                  const Tst_bool addOrSet,
	                                  const Tast_cln_t_tagListIx whichHlp);
	/*** */
	void
	ast_cln_hhlp_show_c_dset(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlp_show_c_dsetValues(Tast_cln_a *pCmdln);
	/*** */
	void
	ast_cln_hhlp_show_c_conv(Tast_cln_a *pCmdln);

	/** help pages for options */
	void
	ast_cln_hhlp_show_o_verblev(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlp_show_o_picTp(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlp_show_o_picFm(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlp_show_o_ebs(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlp_show_o_tiv2(Tast_cln_a *pCmdln);

	/** help pages for special data types */
	/*** */
	void
	ast_cln_hhlp_show_t_taglist(Tast_cln_a *pCmdln,
	                            const char *pOptCap,
	                            const Tast_cln_t_tagList_pb(pAllwTags),
	                            const Tst_bool autoAllw,
	                            const Tst_bool nonAllw);
	/*** */
	void
	ast_cln_hhlp_show_t_numlist(Tast_cln_a *pCmdln,
	                            const char *pOptCap);
	/*** */
	void
	ast_cln_hhlp_show_t_pos(Tast_cln_a *pCmdln,
	                        const char *pOptCap,
	                        const char *pOptCapTot);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_CLN_HNDHELP_C
	/** */
	static void
	AST_CLN__hhlp_c_faddset_posvalsIV2(Tast_cln_a *pCmdln,
	                                   const char *pOptCap,
	                                   const Tst_bool fldOrDsc,
	                                   const Tst_bool addOrSet);
	static void
	AST_CLN__hhlp_c_faddset_posvalsAV2_or_VOR(Tast_cln_a *pCmdln,
	                                          const char *pOptCap,
	                                          const Tst_bool addOrSet,
	                                          const Tst_bool av2OrVor);
	/** */
	static void
	AST_CLN__hhlp_t_taglist_posvals(Tast_cln_a *pCmdln,
	                                const char *pHeadline,
	                                const Tast_cln_t_tagList_pb(pAllwTags),
	                                const Tst_bool autoAllw,
	                                const Tst_bool nonAllw,
	                                const Tst_bool allAllw);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CLN_HNDHELP_PP_H */

/******************************************************************************/
