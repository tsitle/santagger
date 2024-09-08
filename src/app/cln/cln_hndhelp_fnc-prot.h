/******************************************************************************/
/* cln_hndhelp_fnc-prot.h         [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected sub functions for handling help pages                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CLN_HNDHELP_FNC_PROT_H
#define HEAD_CLN_HNDHELP_FNC_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/tag/tag_apev2-defs.h"
#include "src/includes/tag/tag_id3v2-defs.h"
#include "src/includes/tag/tag_vorbc-defs.h"
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_CLN_HNDHELP_FNC_C) || defined(SRC_CLN_HNDHELP_C)
	/** */
	typedef enum {
		AST_CLN_HHLP_FMT_FADD_FLDNAME = 1,
		AST_CLN_HHLP_FMT_FADD_STR     = 2,
		AST_CLN_HHLP_FMT_FSET_FLDNAME = 4,
		AST_CLN_HHLP_FMT_FSET_STR     = 8,
		AST_CLN_HHLP_FMT_DSET_FLDNAME = 16,
		AST_CLN_HHLP_FMT_DSET_STR     = 32
	} Tast_cln_hhlp_fmt_flddscaddset;
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_CLN_HNDHELP_FNC_C) || defined(SRC_CLN_HNDHELP_C)
	/** */
	typedef struct {
		char           *pID02;
		char           *pID34;
		char           *pCap;
		Tst_id3v2_frTp ftp;
		char           *pFTPstr;
		Tst_uint32     verFl;
	} Tast_cln_pvalIV2;

	typedef struct {
		char           *pID;
		char           *pCap;
		Tst_apev2_frTp ftpAV2;
		Tst_vorbc_frTp ftpVOR;
		char           *pFTPstr;
	} Tast_cln_pvalAV2_or_VOR;
#endif  /* protected */


/*
// Functions (protected)
*/
#if defined(SRC_CLN_HNDHELP_FNC_C) || defined(SRC_CLN_HNDHELP_C)
	/** callbacks */
	void
	ast_cln_hhlpfnc_cbRsetElemIV2(void *pElem);
	void
	ast_cln_hhlpfnc_cbRsetElemAV2_or_VOR(void *pElem);

	/** special value type formats */
	/*** */
	void
	ast_cln_hhlpfnc_fmt_tag(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_tagpair(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_taglist(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_vlev(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_vlevlist(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_bs(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_bslist(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_numrange(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_numlist(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_pos(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_fldname(Tast_cln_a *pCmdln);
	void
	ast_cln_hhlpfnc_fmt_tiv2optlist(Tast_cln_a *pCmdln);
	/*** <fldadd>/<fldset>/<dscset> */
	void
	ast_cln_hhlpfnc_fmt_flddscaddset(Tast_cln_a *pCmdln,
	                                 const int fmt,
	                                 const Tst_bool isTagOptional,
	                                 ST_OPTARG(const char *pTagLong),
	                                 ST_OPTARG(const char *pTagShort));

	/** */
	const char*
	ast_cln_hhlpfnc_c_faddset_posvalsIV2_ftp(const Tst_id3v2_frTp ftp);
	Tst_bool
	ast_cln_hhlpfnc_c_faddset_posvalsIV2_genlist(const Tst_bool fldOrDsc,
	                                             const Tst_bool addOrSet,
	                                             Tst_dynlist *pPVlist);
	/** */
	const char*
	ast_cln_hhlpfnc_c_faddset_posvalsAV2_ftp(const Tst_apev2_frTp ftp);
	const char*
	ast_cln_hhlpfnc_c_faddset_posvalsVOR_ftp(const Tst_vorbc_frTp ftp);
	Tst_bool
	ast_cln_hhlpfnc_c_faddset_posvalsAV2_or_VOR_genlist(Tst_dynlist *pPVlist,
	                                                    const Tst_bool av2OrVor,
	                                                    Tst_uint32 *pFldnMaxlen);

	/** */
	void
	ast_cln_hhlpfnc_c_genre_part(Tast_cln_a *pCmdln);

	/** */
	const char*
	ast_cln_hhlpfnc_o_verblev_getCap(const Tst_int32 lev,
	                                 const Tst_bool getSh);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_CLN_HNDHELP_FNC_PROT_H */

/******************************************************************************/
