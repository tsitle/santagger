/******************************************************************************/
/* tag_id3v2_zwr-pp.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for writing ID3v2 Tags                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 05.03.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_ZWR_PP_H
#define HEAD_TAG_ID3V2_ZWR_PP_H

/*
// Own-Includes
*/
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

#define ST_ID3V2_RENDER_EXTHEAD  0  /* render ext. header ? */


/*
// Functions (protected)
*/
/** */
#if defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_ID3V2_ZWR_C)
	/** */
	Tst_err
	st_id3v2_wr_rendTagToBO(Tst_id3v2_tag *pTag,
	                        Tst_tagBasics *pTBasNew,
	                        Tst_binobj **ppTagBO);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_TAG_ID3V2_ZWR_C
	/** */
	static Tst_err
	ST_TIV2__rend_headOrFoot(const Tst_bool hdOrFoot,
	                         const Tst_id3v2_tag_intn *pTagI,
	                         Tst_tagBasics *pTBasNew,
	                         const Tst_id3v2_thdFlags *pTHdFlagsNew,
	                         Tst_binobj *pOutp);
#	if (ST_ID3V2_RENDER_EXTHEAD == 1)
	static Tst_err
	ST_TIV2__rend_extHead(const Tst_id3v2_tag_intn *pTagI,
	                      Tst_tagBasics *pTBasNew,
	                      const Tst_bool isFinal,
	                      const Tst_uint32 paddLen,
	                      Tst_binobj *pOutp);
#	endif

	/** */
	static Tst_err
	ST_TIV2__rend_addPadd(const Tst_id3v2_opts *pOpts,
	                      Tst_tagBasics *pTBasNew,
	                      Tst_binobj *pOutp,
	                      const Tst_bool willFooterBeAdded,
	                      Tst_uint32 *pPaddLen);

	/** */
	static Tst_err
	ST_TIV2__rend_allFlds(Tst_id3v2_tag *pTag,
	                      Tst_tagBasics *pTBasNew,
	                      Tst_id3v2_thdFlags *pTHdFlagsNew,
	                      Tst_binobj *pOutp);
	static Tst_err
	ST_TIV2__rend_fld(const Tst_id3v2_tag_intn *pTagI,
	                  Tst_tagBasics *pTBasNew,
	                  Tst_id3v2_thdFlags *pTHdFlagsNew,
	                  Tst_id3v2_fldData_intn *pFldI,
	                  Tst_binobj **ppOutp);

	/** */
	static Tst_err
	ST_TIV2__rend_fldHead(const Tst_id3v2_opts *pOpts,
	                      const Tst_byte verMaj,
	                      Tst_id3v2_fldData_intn *pFldI,
	                      const Tst_id3v2_frhdFlags *pFrHdFlags,
	                      Tst_binobj *pOutp,
	                      const Tst_uint32 frSize);
	static void
	ST_TIV2__rend_fldFlags(const Tst_id3v2_opts *pOpts,
	                       const Tst_byte verMaj,
	                       const Tst_id3v2_frhdFlags *pFrHdFlags,
	                       Tst_uint32 *pFlagBytes);
	static Tst_err
	ST_TIV2__rend_fldData(const Tst_id3v2_opts *pOpts,
	                      const Tst_byte verMaj,
	                      Tst_id3v2_fldData_intn *pFldI,
	                      Tst_id3v2_frhdFlags *pFrHdFlags,
	                      Tst_binobj *pOutp);

	/** */
	static Tst_err
	ST_TIV2__rend_fd_corrTEs(const Tst_byte verMaj,
	                         Tst_id3v2_fldData_intn *pFldI,
	                         Tst_mtes_textEnc *pTE);
	static Tst_err
	ST_TIV2__rend_fd_corrTEs_equalize(const Tst_byte verMaj,
	                                  Tst_mtes_string *pScd,
	                                  Tst_mtes_strArr *pSArr,
	                                  Tst_mtes_string *pFn,
	                                  Tst_mtes_textEnc *pTE);
	static Tst_err
	ST_TIV2__rend_fd_corrTEs_sylt(Tst_id3v2_fldData_intn *pFldI,
	                              const Tst_mtes_textEnc teSrc,
	                              const Tst_mtes_textEnc teDst);

	/** */
	static Tst_err
	ST_TIV2__rend_fd_encTE(const Tst_id3v2_opts *pOpts,
	                       Tst_binobj *pOutp,
	                       const Tst_mtes_textEnc te);
	static Tst_err
	ST_TIV2__rend_fd_encLang(const Tst_id3v2_opts *pOpts,
	                         const char *pFnc,
	                         Tst_binobj *pOutp,
	                         const Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rend_fd_encBinDat(const Tst_id3v2_opts *pOpts,
	                           const char *pFnc,
	                           Tst_binobj *pOutp,
	                           Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rend_fd_encUVal(const Tst_id3v2_opts *pOpts,
	                         const char *pFnc,
	                         Tst_binobj *pOutp,
	                         const Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rend_fd_encBVal(const Tst_id3v2_opts *pOpts,
	                         const char *pFnc,
	                         Tst_binobj *pOutp,
	                         const Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rend_fd_encPic1(const Tst_id3v2_opts *pOpts,
	                         const char *pFnc,
	                         const Tst_byte verMaj,
	                         Tst_binobj *pOutp,
	                         const Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rend_fd_encPic1_im(const Tst_id3v2_opts *pOpts,
	                            const Tst_byte verMaj,
	                            Tst_binobj *pOutp,
	                            const char *pIF,
	                            const char *pMim);
	static Tst_err
	ST_TIV2__rend_fd_encGeo1(const Tst_id3v2_opts *pOpts,
	                         const char *pFnc,
	                         Tst_binobj *pOutp,
	                         const Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rend_fd_encSlt1(const Tst_id3v2_opts *pOpts,
	                         const char *pFnc,
	                         Tst_binobj *pOutp,
	                         const Tst_id3v2_fldData_intn *pFldI);
	static Tst_err
	ST_TIV2__rend_fd_encStr(Tst_binobj *pOutp,
	                        const Tst_mtes_string *pStr,
	                        const Tst_bool withTerm);
	static Tst_err
	ST_TIV2__rend_fd_encMultStr(const Tst_id3v2_opts *pOpts,
	                            const char *pFnc,
	                            Tst_binobj *pOutp,
	                            const Tst_mtes_strArr *pStrArr,
	                            const Tst_id3v2_fldData_intn *pFldI,
	                            const Tst_bool isTaggInf);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_ZWR_PP_H */

/******************************************************************************/
