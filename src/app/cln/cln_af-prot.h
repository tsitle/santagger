/******************************************************************************/
/* cln_af-prot.h                  [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for attached files                                 */
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

#ifndef HEAD_CLN_AF_PROT_H
#define HEAD_CLN_AF_PROT_H

/*
// Own-Includes
*/
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CLN_AF_C) || defined(SRC_CLN_HNDARG_C) || \
			defined(SRC_CLN_PARGS_C)
	/** */
	Tst_err
	ast_cln_af_addAF(Tast_cln_a *pCmdln,
	                 const Tst_bool isPicOrGeo,
	                 const Tst_bool addOrMod,
	                 ST_OPTARG(const Tst_str *pFn),
	                 const Tst_uint32 modFNr);

	Tst_err
	ast_cln_af_addAttrToAF(Tast_cln_a *pCmdln,
	                       const Tst_bool isPicOrGeo,
	                       ST_OPTARG(const Tst_str *pDesc),
	                       const Tst_id3v2_picTp picTpE,
	                       const Tst_utilsFmt_picFmt picFmtE,
	                       ST_OPTARG(const Tst_str *pMime),
	                       ST_OPTARG(const Tst_str *pModFilen));

	void
	ast_cln_af_prAFarr(const Tast_cln_a *pCmdln);

	Tst_err
	ast_cln_af_checkAFarr(Tast_cln_a *pCmdln);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_CLN_AF_PROT_H */

/******************************************************************************/
