/******************************************************************************/
/* tag_id3v2_zrd-pp.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading ID3v2 Tags                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_ZRD_PP_H
#define HEAD_TAG_ID3V2_ZRD_PP_H

/*
// Own-Includes
*/
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_ID3V2_ZRD_C)
	/** */
	Tst_err st_id3v2_rd_readHdOrFoot(Tst_id3v2_tag *pTag,
	                                 const Tst_bool isFooterOrHeader,
	                                 Tst_bool *pIsOK,
	                                 Tst_uint32 *pEhdNfraSz,
	                                 Tst_uint32 *pTotRd);
	Tst_err st_id3v2_rd_readEHdAndFlds(Tst_id3v2_tag *pTag,
	                                   const Tst_uint32 allrRead);
	/** */
	void st_id3v2_rd_updateTagSize(Tst_id3v2_tag_intn *pTagI,
	                               const char *pFnc,
	                               const Tst_bool useOffsets,
	                               Tst_uint32 newSz);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_ID3V2_ZRD_C
	/** */
	static Tst_err
	ST_TIV2__rd_extHd(Tst_id3v2_tag_intn *pTagI,
	                  Tst_streamrd *pStrrdTag,
	                  Tst_uint32 *pRemSz);
	static Tst_err
	ST_TIV2__rd_allFlds(Tst_id3v2_tag *pTag,
	                    Tst_streamrd *pStrrdTag,
	                    Tst_uint32 *pRemSz);
	/** */
	static Tst_err
	ST_TIV2__rd_fld(Tst_id3v2_tag *pTag,
	                Tst_streamrd *pStrrdTag,
	                const Tst_uint32 frSize,
	                const Tst_str *pIDstr,
	                const Tst_uint32 flags,
	                Tst_bool *pSkipField);
	/** */
	static Tst_err
	ST_TIV2__rd_fld_sub(Tst_id3v2_tag *pTag,
	                    Tst_streamrd *pStrrdTag,
	                    Tst_uint32 frSize,
	                    const Tst_str *pFrIDstr,
	                    const Tst_uint32 flags,
	                    Tst_bool *pSkipField);
	/** */
	static void
	ST_TIV2__rd_fld_flagsParse(Tst_id3v2_tag_intn *pTagI,
	                           Tst_id3v2_fldData_intn *pFldI,
	                           const Tst_uint32 flags,
	                           Tst_id3v2_frhdFlags *pFlagsFHd);
	static Tst_err
	ST_TIV2__rd_fld_flagsHandle(Tst_id3v2_tag_intn *pTagI,
	                            Tst_id3v2_fldData_intn *pFldI,
	                            Tst_id3v2_frhdFlags *pFlagsFHd,
	                            Tst_streamrd **ppStrrdFld,
	                            Tst_uint32 *pFrSize,
	                            Tst_streamrd *pStrrdFldBO,
	                            Tst_binobj *pBinDatFld);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_ZRD_PP_H */

/******************************************************************************/
