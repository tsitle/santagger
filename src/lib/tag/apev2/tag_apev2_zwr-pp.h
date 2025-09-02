/******************************************************************************/
/* tag_apev2_zwr-pp.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for writing APEv2 Tags                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_APEV2_ZWR_PP_H
#define LIBSANTAGGER_TAG_APEV2_ZWR_PP_H

/*
// Own-Includes
*/
/** */
#include "tag_apev2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
/** */
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_ZWR_C)
	/** */
	Tst_err
	st_apev2_wr_rendTagToBO(Tst_apev2_tag *pTag,
	                        Tst_tagBasics *pTBasNew,
	                        Tst_binobj *pTagBO);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_APEV2_ZWR_C
	/** */
	static Tst_err
	ST_TAV2__rend_headOrFoot(const Tst_bool hdOrFoot,
	                         Tst_apev2_tag_intn *pTagI,
	                         const Tst_uint32 numFlds,
	                         Tst_tagBasics *pTBasNew,
	                         Tst_binobj *pTagBO);
	static Tst_err
	ST_TAV2__rend_allFlds(Tst_apev2_tag *pTag,
	                      Tst_tagBasics *pTBasNew,
	                      Tst_binobj *pTagBO);
	static Tst_err
	ST_TAV2__rend_fld(Tst_apev2_tag_intn *pTagI,
	                  Tst_tagBasics *pTBasNew,
	                  Tst_apev2_fldData_intn *pFldI,
	                  Tst_binobj *pTagBO);
	static Tst_err
	ST_TAV2__rend_fldHead(Tst_apev2_fldData_intn *pFldI,
	                      const Tst_bool isFinal,
	                      const Tst_uint32 frBeg,
	                      const Tst_uint32 frSz,
	                      Tst_uint32 *pFrHdSz,
	                      Tst_binobj *pTagBO);
	static Tst_err
	ST_TAV2__rend_fldData(Tst_apev2_opts *pOpts,
	                      Tst_apev2_fldData_intn *pFldI,
	                      Tst_binobj *pTagBO);
#endif  /* private */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_APEV2_ZWR_PP_H */

/******************************************************************************/
