/******************************************************************************/
/* aud_mpeg1_zrd2-pp.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Sub functions for MPEG-1 Frameheader and VBR Headers                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AUD_MPEG1_ZRD2_PP_H
#define HEAD_AUD_MPEG1_ZRD2_PP_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "aud_mpeg1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_AUD_MPEG1_ZRD_C) || defined(SRC_AUD_MPEG1_ZRD2_C)
	/** */
	Tst_err
	st_mpeg1_rd2_readFrHd(Tst_mpeg1_obj_intn *pMOI,
	                      Tst_mpeg1_farnfh *pFar);
	/** */
	Tst_int32
	st_mpeg1_rd2_getSideinfoSize(const Tst_mpeg1_fhd *pMFH);
	/** */
	ST_INLINE_H Tst_uint32
	st_mpeg1_rd2_getSamplesPerFrame(const Tst_mpeg1_layer lay,
	                                const Tst_mpeg1_audioVers vers);
	/** */
	Tst_err
	st_mpeg1_rd2_readVBRheadXing(const Tst_buf *pBuf,
	                             const Tst_uint32 bufSz,
	                             Tst_mpeg1_vbr *pVBRhd);
	Tst_err
	st_mpeg1_rd2_readVBRheadFraun(const Tst_buf *pBuf,
	                              const Tst_uint32 bufSz,
	                              Tst_mpeg1_vbr *pVBRhd);
	/** */
	const Tst_buf*
	st_mpeg1_rd2_memfind(const Tst_buf *pHay,
	                     const Tst_uint32 haySz,
	                     const Tst_buf *pNeedle,
	                     const Tst_uint32 needleSz);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_AUD_MPEG1_ZRD2_C
	/** */
	static void
	ST_MPEG1__rd2_getFrameLength(Tst_mpeg1_fhd *pMFH);
	/** */
	static Tst_err
	ST_MPEG1__rd2_calcSRFI(const Tst_byte byt,
	                       const Tst_mpeg1_audioVers avers,
	                       Tst_uint32 *pSFreq);
	static Tst_err
	ST_MPEG1__rd2_calcBRI(const Tst_byte byt,
	                      const Tst_mpeg1_audioVers avers,
	                      const Tst_mpeg1_layer lay,
	                      Tst_uint32 *pBRate);
	/** */
	static Tst_err
	ST_MPEG1__rd2_calcBRI_v1_l1(const Tst_byte byt, Tst_uint32 *pBRate);

	static Tst_err
	ST_MPEG1__rd2_calcBRI_v1_l2(const Tst_byte byt, Tst_uint32 *pBRate);

	static Tst_err
	ST_MPEG1__rd2_calcBRI_v1_l3(const Tst_byte byt, Tst_uint32 *pBRate);

	static Tst_err
	ST_MPEG1__rd2_calcBRI_v2_25_l1(const Tst_byte byt, Tst_uint32 *pBRate);

	static Tst_err
	ST_MPEG1__rd2_calcBRI_v2_25_l2_3(const Tst_byte byt, Tst_uint32 *pBRate);
	/** */
	static ST_INLINE_H Tst_int32
	ST_MPEG1__rd2_floor(const double val);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_AUD_MPEG1_ZRD2_PP_H */

/******************************************************************************/
