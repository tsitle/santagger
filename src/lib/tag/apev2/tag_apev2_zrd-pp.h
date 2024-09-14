/******************************************************************************/
/* tag_apev2_zrd-pp.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for reading APEv2 Tags                         */
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

#ifndef HEAD_TAG_APEV2_ZRD_PP_H
#define HEAD_TAG_APEV2_ZRD_PP_H

/*
// Own-Includes
*/
/** */
#include "tag_apev2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_ZRD_C)
	/** */
	Tst_err st_apev2_rd_readHdOrFoot(Tst_apev2_tag_intn *pTagI,
	                                 const Tst_bool isFooterOrHeader,
	                                 Tst_bool *pIsOK,
	                                 Tst_uint32 *pTagSz,
	                                 Tst_uint32 *pNumFlds);
	/** */
	Tst_err st_apev2_rd_readTagFlds(Tst_apev2_tag *pTag,
	                                Tst_uint32 numFlds,
	                                Tst_uint32 *pRemSz);
	/** */
	void st_apev2_rd_updateTagSize(Tst_apev2_tag_intn *pTagI,
	                               const char *pFnc,
	                               const Tst_bool useOffsets,
	                               Tst_uint32 newSz);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_APEV2_ZRD_C
	/** */
	static Tst_err
	ST_TAV2__rd_readHdOrFoot_parse(const Tst_apev2_opts *pOpts,
	                               const Tst_str *pFn,
	                               const Tst_buf *pBuf,
	                               const Tst_bool isFooterOrHeader,
	                               Tst_uint32 *pSize,
	                               Tst_uint32 *pFlags,
	                               Tst_uint32 *pNumFlds);
	/** */
	static Tst_err
	ST_TAV2__rd_readTagFldHd(const Tst_apev2_opts *pOpts,
	                         const Tst_uint32 fnr,
	                         Tst_streamrd *pStrrd,
	                         Tst_uint32 *pFldSz,
	                         Tst_bool *pBinOrUTF8,
	                         Tst_str **ppFldKey,
	                         Tst_uint32 *pRemSz);
	/** */
	static Tst_err
	ST_TAV2__rd_readFldData(Tst_apev2_tag_intn *pTagI,
	                        Tst_apev2_fldData_intn *pFldI,
	                        const Tst_uint32 fldSz,
	                        Tst_buf **ppFBuf,
	                        Tst_uint32 *pFBufSz);
	/** */
	static Tst_err
	ST_TAV2__rd_copyBinData(Tst_apev2_tag_intn *pTagI,
	                        Tst_apev2_fldData_intn *pFldI,
	                        const Tst_uint32 fldSz);
	/** */
	static Tst_err
	ST_TAV2__rd_copyMultStrU8(Tst_apev2_tag_intn *pTagI,
	                          Tst_apev2_fldData_intn *pFldI,
	                          const Tst_uint32 fldSz);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_APEV2_ZRD_PP_H */

/******************************************************************************/
