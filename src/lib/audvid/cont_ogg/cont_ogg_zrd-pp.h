/******************************************************************************/
/* cont_ogg_zrd-pp.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for reading OGG containers / Flac files        */
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

#ifndef HEAD_CONT_OGG_ZRD_PP_H
#define HEAD_CONT_OGG_ZRD_PP_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_C) || defined(SRC_CONT_OGG_ZRD_C)
	/** */
	void
	st_contOgg_rd_showUsedChecksums(const Tst_contOgg_opts *pOpts,
	                                const Tst_bool isSrcOggOrFlac);
	/** */
	Tst_err
	st_contOgg_rd_rdPacket(Tst_contOgg_obj_intn *pAObjI,
	                       Tst_contOgg_substr_intn **ppBSI,
	                       Tst_contOgg_fhd **ppFHd);
	/** */
	Tst_err
	st_contOgg_rd_rdFrameHd(Tst_contOgg_obj_intn *pAObjI,
	                        Tst_contOgg_fhd *pFHd);
	Tst_err
	st_contOgg_rd_rdFrameDat(Tst_contOgg_obj_intn *pAObjI,
	                         Tst_contOgg_substr_intn *pBSI,
	                         Tst_contOgg_fhd *pFHd,
	                         Tst_uint32 frDatSz,
	                         const Tst_bool appendOrAuto);
	/** */
	Tst_contOgg_substream*
	st_contOgg_rd_getBS(Tst_contOgg_obj_intn *pAObjI,
	                    const Tst_uint32 bsSerial);
	/** */
	void
	st_contOgg_rd_finish_pcmClient_andDecoder(Tst_contOgg_obj_intn *pAObjI);
	/** */
	void
	st_contOgg_rd_afterReading(const char *pFnc,
	                           Tst_contOgg_obj_intn *pAObjI);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_CONT_OGG_ZRD_C
	/** */
	static void
	ST_CONTOGG__rd_printStreamType(const Tst_contOgg_opts *pOpts,
	                               const Tst_contOgg_substr_intn *pBSI,
	                               const char *pFnc);
	/** */
	static void
	ST_CONTOGG__rd_computePlaytimeAndBitrate(const Tst_contOgg_opts *pOpts,
	                                         const Tst_uint64 *pSamples,
	                                         const Tst_uint32 sampleRate,
	                                         const Tst_fsize dataSize,
	                                         Tst_av_playtime *pPTime,
	                                         Tst_uint32 *pBitrateAVG);
	/** */
	static Tst_err
	ST_CONTOGG__rd_addFHdToStream(Tst_contOgg_obj_intn *pAObjI,
	                              Tst_contOgg_fhd *pFHd,
	                              Tst_contOgg_substr_intn **ppBSI);
	/** */
	static void
	ST_CONTOGG__rd_setLastBStoGrouped(Tst_contOgg_substreamArr *pSA,
	                                  const Tst_uint32 grpIx);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_ZRD_PP_H */

/******************************************************************************/
