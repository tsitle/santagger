/******************************************************************************/
/* tag_vorbc_zwr-pp.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for writing Vorbis Tags                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_VORBC_ZWR_PP_H
#define HEAD_TAG_VORBC_ZWR_PP_H

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
#if defined(SRC_TAG_VORBC_C) || defined(SRC_TAG_VORBC_ZWR_C)
	/** */
	Tst_err
	st_vorbc_wr_rendTagToBO(Tst_vorbc_tag *pTag,
	                        Tst_tagBasics *pTBasNew,
	                        Tst_binobj *pTagBO);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_VORBC_ZWR_C
	/** */
	static Tst_err
	ST_TVORBC__rend_head(Tst_vorbc_tag_intn *pTagI,
	                     const Tst_uint32 numFlds,
	                     Tst_tagBasics *pTBasNew,
	                     const Tst_bool isFinal,
	                     Tst_uint32 *pVendLen,
	                     Tst_binobj *pTagBO);
	static Tst_err
	ST_TVORBC__rend_allFlds(Tst_vorbc_tag *pTag,
	                        Tst_tagBasics *pTBasNew,
	                        Tst_binobj *pTagBO);
	static Tst_err
	ST_TVORBC__rend_fld(Tst_vorbc_tag_intn *pTagI,
	                    Tst_tagBasics *pTBasNew,
	                    Tst_vorbc_fldData_intn *pFldI,
	                    Tst_binobj *pTagBO);
	static Tst_err
	ST_TVORBC__rend_fldHead(Tst_vorbc_fldData_intn *pFldI,
	                        const Tst_bool isFinal,
	                        const Tst_uint32 frBeg,
	                        const Tst_uint32 frSz,
	                        Tst_uint32 *pFrHdSz,
	                        Tst_binobj *pTagBO);
	static Tst_err
	ST_TVORBC__rend_fldData(Tst_vorbc_opts *pOpts,
	                        Tst_vorbc_fldData_intn *pFldI,
	                        Tst_binobj *pTagBO);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_ZWR_PP_H */

/******************************************************************************/
