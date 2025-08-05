/******************************************************************************/
/* cont_ogg_zrd_cod-pp.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for OGG/Flac codec handling                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CONT_OGG_ZRD_COD_PP_H
#define HEAD_CONT_OGG_ZRD_COD_PP_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/streamrd-defs.h"
/** */
#include "cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_ZRD_COD_C)
	/** for OGG packets */
	Tst_err
	st_contOgg_rdcod_parsePackOgg(const Tst_contOgg_opts *pOpts,
	                              Tst_contOgg_substr_intn *pBSI);

	/** for Flac packets */
	Tst_err
	st_contOgg_rdcod_parsePackFlac(const Tst_contOgg_opts *pOpts,
	                               Tst_contOgg_substr_intn *pBSI,
	                               Tst_streamrd *pSObj);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C)
	/** for (OGG-)Flac streams */
	Tst_err
	st_contOgg_rdcod_finishFlac(const Tst_contOgg_opts *pOpts,
	                            Tst_contOgg_substr_intn *pBSI);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_CONT_OGG_ZRD_COD_C
	/** for OGG or Flac packets */
	static Tst_err
	ST_CONTOGG__rdcod_parsePack(const Tst_contOgg_opts *pOpts,
	                            Tst_contOgg_substr_intn *pBSI,
	                            Tst_streamrd *pSObj);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_codecHead(const Tst_contOgg_opts *pOpts,
	                               Tst_contOgg_substr_intn *pBSI,
	                               Tst_streamrd *pSObj,
	                               Tst_byte *pPackTp,
	                               Tst_bool *pFound);
	static Tst_err
	ST_CONTOGG__rdcod_pP_findNextCodecHd(const char *pFnc,
	                                     const Tst_contOgg_opts *pOpts,
	                                     Tst_contOgg_substr_intn *pBSI,
	                                     Tst_streamrd *pSObj,
	                                     Tst_byte *pPackTp,
	                                     Tst_bool *pFoundCodHd2);
	static Tst_err
	ST_CONTOGG__rdcod_pP_sub(const Tst_contOgg_opts *pOpts,
	                         Tst_contOgg_substr_intn *pBSI,
	                         Tst_streamrd *pSObj,
	                         const Tst_byte packTp,
	                         const Tst_bool foundCodHd);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_skeleton(const Tst_contOgg_opts *pOpts,
	                              Tst_contOgg_substr_intn *pBSI,
	                              Tst_byte packTp,
	                              Tst_streamrd *pSObj);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_vorbis(const Tst_contOgg_opts *pOpts,
	                            Tst_contOgg_substr_intn *pBSI,
	                            Tst_byte packTp,
	                            Tst_streamrd *pSObj,
	                            const Tst_bool foundCodHd);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_flac(const Tst_contOgg_opts *pOpts,
	                          Tst_contOgg_substr_intn *pBSI,
	                          Tst_byte packTp,
	                          Tst_streamrd *pSObj);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_speex(const Tst_contOgg_opts *pOpts,
	                           Tst_contOgg_substr_intn *pBSI,
	                           Tst_byte packTp,
	                           Tst_streamrd *pSObj);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_celt(const Tst_contOgg_opts *pOpts,
	                          Tst_contOgg_substr_intn *pBSI,
	                          Tst_byte packTp,
	                          Tst_streamrd *pSObj);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_theora(const Tst_contOgg_opts *pOpts,
	                            Tst_contOgg_substr_intn *pBSI,
	                            Tst_byte packTp,
	                            Tst_streamrd *pSObj,
	                            const Tst_bool foundCodHd);

	/** */
	static Tst_err
	ST_CONTOGG__rdcod_pP_dirac(const Tst_contOgg_opts *pOpts,
	                           Tst_contOgg_substr_intn *pBSI,
	                           Tst_byte packTp,
	                           Tst_streamrd *pSObj,
	                           const Tst_bool foundCodHd);

	/** */
	static void
	ST_CONTOGG__rdcod_stripStr(Tst_str *pStr, const Tst_uint32 len);
	/** */
	static Tst_err
	ST_CONTOGG__rdcod_readPartialPack(Tst_contOgg_packet *pCurPack,
	                                  Tst_uint32 *pSegsX,
	                                  Tst_uint32 *pOutBytes,
	                                  const Tst_bool copyData,
	                                  Tst_streamrd *pSObjIn,
	                                  const Tst_buf *pCodecHeadBuf,
	                                  const Tst_uint32 codHdSz,
	                                  Tst_buf **ppBuf,
	                                  Tst_uint32 *pBufSz,
	                                  Tst_uint32 *pBufUsd);
	static Tst_err
	ST_CONTOGG__rdcod_getBogusVCmt(const Tst_buf *pCodecHeadBuf,
	                               const Tst_uint32 codHdSz,
	                               const Tst_bool withFramingBit,
	                               Tst_buf **ppBuf,
	                               Tst_uint32 *pBufSz,
	                               Tst_uint32 *pBufUsd);
	/** */
	static Tst_err
	ST_CONTOGG__rdcod_initPCMclient(const Tst_contOgg_opts *pOpts,
	                                const char *pFnc,
	                                Tst_contOgg_substr_intn *pBSI,
	                                void **ppPCMstrHnd);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_ZRD_COD_PP_H */

/******************************************************************************/
