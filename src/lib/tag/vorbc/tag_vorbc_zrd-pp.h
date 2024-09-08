/******************************************************************************/
/* tag_vorbc_zrd-pp.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Private/Protected functions for reading Vorbis Tags                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_VORBC_ZRD_PP_H
#define HEAD_TAG_VORBC_ZRD_PP_H

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
#if defined(SRC_TAG_VORBC_C) || defined(SRC_TAG_VORBC_ZRD_C)
	/** */
	Tst_err
	st_vorbc_rd_readHeader(Tst_streamrd *pSObjP,
	                       Tst_vorbc_tag_intn *pTagI,
	                       Tst_bool *pIsOK,
	                       Tst_uint32 *pNumFlds,
	                       Tst_uint32 *pHeadSz);
	/** */
	Tst_err
	st_vorbc_rd_readFlds(Tst_streamrd *pSObjP,
	                     Tst_vorbc_tag *pTag,
	                     const Tst_uint32 headSz,
	                     Tst_uint32 numFlds);
	/** */
	void
	st_vorbc_rd_updateTagSize(Tst_vorbc_tag_intn *pTagI,
	                          const char *pFnc,
	                          const Tst_bool useOffsets,
	                          Tst_uint32 newSz);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_VORBC_ZRD_C
	/** */
	static Tst_err
	ST_TVORBC__rd_readTagFldHd(const Tst_vorbc_opts *pOpts,
	                           const Tst_uint32 fnr,
	                           Tst_streamrd *pStrrdTag,
	                           Tst_uint32 *pFldSz,
	                           Tst_str **ppFldKey,
	                           Tst_uint32 *pFldKeySz,
	                           Tst_uint32 *pRemSz);
	/** */
	static Tst_bool
	ST_TVORBC__rd_shallAppend(Tst_vorbc_flds *pFlds,
	                          Tst_vorbc_fldData **ppFld,
	                          Tst_vorbc_fldData_intn **ppFldI);
	/** */
	static Tst_err
	ST_TVORBC__rd_copyStrU8(const Tst_vorbc_opts *pOpts,
	                        const char *pFnc,
	                        Tst_streamrd *pStrrdTag,
	                        const Tst_uint32 len,
	                        Tst_vorbc_fldData_intn *pFldI);
	/** */
	static Tst_err
	ST_TVORBC__rd_copyPict(Tst_vorbc_tag_intn *pTagI,
	                       const char *pFnc,
	                       Tst_streamrd *pStrrdTag,
	                       const Tst_uint32 len,
	                       Tst_vorbc_fldData_intn *pFldI);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_ZRD_PP_H */

/******************************************************************************/
