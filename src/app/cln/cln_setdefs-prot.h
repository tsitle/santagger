/******************************************************************************/
/* cln_setdefs-prot.h             [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for setting default program options                */
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

#ifndef HEAD_CLN_SETDEFS_PROT_H
#define HEAD_CLN_SETDEFS_PROT_H

/*
// Own-Includes
*/
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CLN_SETDEFS_C) || defined(SRC_CLN_HNDARG_C) || \
			defined(SRC_CLN_PARGS_C)
	/** */
	void ast_cln_sd_setDefs_tag_swOrWr(Tast_cln_a *pCmdln);
	/** */
	void ast_cln_sd_setOverwr_tag_swOrWr(Tast_cln_a *pCmdln);
	void ast_cln_sd_setOverwr_cmds(Tast_cln_a *pCmdln);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_CLN_SETDEFS_PROT_H */

/******************************************************************************/
