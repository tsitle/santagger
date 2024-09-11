/******************************************************************************/
/* tag_vorbc_ztidy-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for tidying up Vorbis Tags                             */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 26.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_VORBC_ZTIDY_PROT_H
#define HEAD_TAG_VORBC_ZTIDY_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_vorbc-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
/** */
#if defined(SRC_TAG_VORBC_ZTIDY_C) || defined(SRC_TAG_VORBC_C)
	/** */
	Tst_err
	st_vorbc_tidy_mapMergePosFlds(const char *pFnc,
	                              Tst_vorbc_tag *pTag);
	/** */
	Tst_err
	st_vorbc_tidy_remDups(const char *pFnc,
	                      Tst_vorbc_tag *pTag,
	                      Tst_vorbc_fldData *pFld,
	                      Tst_bool *pDiscardedOne);
	/** */
	Tst_err
	st_vorbc_tidy_mergFields(const char *pFnc,
	                         Tst_vorbc_tag *pTag,
	                         Tst_vorbc_fldData *pFld);
	Tst_err
	st_vorbc_tidy_mergMultStr(const char *pFnc,
	                          const Tst_vorbc_opts *pOpts,
	                          Tst_vorbc_fldData *pFld);
	/** */
	Tst_err
	st_vorbc_tidy_chkAllFields(const char *pFnc,
	                           Tst_vorbc_tag *pTag);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_ZTIDY_PROT_H */

/******************************************************************************/
