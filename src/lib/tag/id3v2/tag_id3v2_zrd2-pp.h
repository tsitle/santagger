/******************************************************************************/
/* tag_id3v2_zrd2-pp.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for reading ID3v2 Tags                                       */
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

#ifndef HEAD_TAG_ID3V2_ZRD2_PP_H
#define HEAD_TAG_ID3V2_ZRD2_PP_H

/*
// Own-Includes
*/
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZRD2_C)
	/** */
	Tst_err st_id3v2_rd2_readFldData(Tst_id3v2_tag *pTag,
	                                 Tst_id3v2_fldData_intn *pFldI,
	                                 Tst_streamrd *pStrrdFld,
	                                 Tst_uint32 frSize);
#endif  /* protected */


/*
// Types (private)
*/
#ifdef SRC_TAG_ID3V2_ZRD2_C
	/** */
	typedef struct {
		Tst_streamrd *pStrrdFld;
		Tst_uint32   frSize;
	} Tst_iv2__readBuf;
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_TAG_ID3V2_ZRD2_C
	/** */
	static Tst_err
	ST_TIV2__rd2_fd_decTE(const Tst_id3v2_opts *pOpts,
	                      const char *pFnc,
	                      Tst_iv2__readBuf *pRB,
	                      Tst_id3v2_fldData_intn *pFldI,
	                      Tst_mtes_textEnc *pTE);
	static Tst_err
	ST_TIV2__rd2_fd_decLang(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_iv2__readBuf *pRB,
	                        Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rd2_fd_decBinDat(const Tst_id3v2_opts *pOpts,
	                          const char *pFnc,
	                          Tst_iv2__readBuf *pRB,
	                          Tst_id3v2_fldData_intn *pFldI,
	                          const Tst_uint32 uncBufSz,
	                          const Tst_mtes_textEnc sltTE);
	static Tst_err
	ST_TIV2__rd2_fd_decUVal(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_iv2__readBuf *pRB,
	                        Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rd2_fd_decBVal(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_iv2__readBuf *pRB,
	                        Tst_id3v2_fldData_intn *pFldI);
	/** */
	static Tst_err
	ST_TIV2__rd2_fd_decPic1(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_iv2__readBuf *pRB,
	                        Tst_id3v2_fldData_intn *pFldI,
	                        const Tst_byte verMaj);
	static Tst_err
	ST_TIV2__rd2_fd_decGeo1(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_iv2__readBuf *pRB,
	                        Tst_id3v2_fldData_intn *pFldI,
	                        const Tst_mtes_textEnc te);
	static Tst_err
	ST_TIV2__rd2_fd_decCdm1(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_iv2__readBuf *pRB,
	                        Tst_id3v2_fldData_intn *pFldI,
	                        Tst_uint32 *pUncBufSz);
	static Tst_err
	ST_TIV2__rd2_fd_decSlt1(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_iv2__readBuf *pRB,
	                        Tst_id3v2_fldData_intn *pFldI);
	/** */
	static Tst_err
	ST_TIV2__rd2_fd_cdm_decmpr(const Tst_id3v2_opts *pOpts,
	                           const char *pFnc,
	                           Tst_iv2__readBuf *pRB,
	                           Tst_id3v2_fldData_intn *pFldI,
	                           const Tst_uint32 uncBufSz);
	static Tst_err
	ST_TIV2__rd2_fd_slt_b2t(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        Tst_id3v2_fldData_intn *pFldI,
	                        const Tst_mtes_textEnc te);
	static Tst_err
	ST_TIV2__rd2_fd_UfiMci_b2t(const Tst_id3v2_opts *pOpts,
	                           const char *pFnc,
	                           Tst_id3v2_fldData_intn *pFldI,
	                           const Tst_mtes_textEnc te,
	                           const Tst_buf *pBuf,
	                           const Tst_uint32 bufSz);
	/** */
	static Tst_err
	ST_TIV2__rd2_fd_decMultStr(const Tst_id3v2_opts *pOpts,
	                           const char *pFnc,
	                           Tst_iv2__readBuf *pRB,
	                           Tst_id3v2_fldData_intn *pFldI,
	                           Tst_id3v2_fldd_strtp *pStrTp,
	                           const Tst_mtes_textEnc textEnc);
	/** */
	static Tst_err
	ST_TIV2__rd2_fd_copyStr(const Tst_id3v2_opts *pOpts,
	                        const char *pFnc,
	                        const Tst_bool newLnAllowed,
	                        const Tst_mtes_textEnc textEnc,
	                        Tst_iv2__readBuf *pRB,
	                        const Tst_uint32 maxlen,
	                        Tst_mtes_string *pMStr,
	                        Tst_uint32 *pCopied,
	                        Tst_id3v2_fldData_intn *pFldI);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_ZRD2_PP_H */

/******************************************************************************/
