/******************************************************************************/
/* ac_flac_zrda-pp.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for reading (Ogg-)Flac audio packets           */
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

#ifndef LIBSANTAGGER_AC_FLAC_ZRDA_PP_H
#define LIBSANTAGGER_AC_FLAC_ZRDA_PP_H

/*
// Own-Includes
*/
/** */
#include "ac_flac-prot.h"
#include "../cont_ogg/cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Types (protected)
*/
#if defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDA_C)
	/** */
	typedef struct {
		Tst_contOgg_substr_intn *pBSI;
		Tst_streamrd            *pSObj;
		/* */
		Tst_contOgg_flacAudFrame *pFra;
	} Tst_contOgg_codFlac_defParsA;
#endif  /* protected */


/*
// Types (private)
*/
#ifdef SRC_AC_FLAC_ZRDA_C
	/** */
	typedef struct {
		Tst_byte   sampleSz;
		Tst_byte   predOrder;
		Tst_byte   coeffPrec;
		Tst_int32  coeffArr[32];
		Tst_int32  quantLev;
		Tst_int32  *pResids;
		Tst_uint32 blockSz;
	} Tst_contOgg__codFlac_restoreLPC;
	/** */
	typedef struct {
		Tst_byte        predOrder;
		Tst_byte        coeffPrec;
		Tst_int32 const *pCoeffArr;
		Tst_int32       quantLev;
		Tst_int32 const *pResids;
		Tst_uint32      sampleCnt;
	} Tst_contOgg__codFlac_restoreLPC2;
#endif  /* private */


/*
// Functions (protected)
*/
#if defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDA_C)
	/** */
	Tst_bool
	st_contOgg_flacrda_searchNextFH(const Tst_contOgg_opts *pOpts,
	                                Tst_contOgg_codFlac_defParsA *pDPa);
	/** */
	Tst_err
	st_contOgg_flacrda_parseFH(const Tst_contOgg_opts *pOpts,
	                           Tst_contOgg_codFlac_defParsA *pDPa);
	/** */
	Tst_err
	st_contOgg_flacrda_parseSF(const Tst_contOgg_opts *pOpts,
	                           Tst_contOgg_codFlac_defParsA *pDPa);
	/** */
	void
	st_contOgg_flacrda_undoSpecChannEnc(const Tst_contOgg_opts *pOpts,
	                                    Tst_contOgg_codFlac_defParsA *pDPa);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_AC_FLAC_ZRDA_C
	/** */
	static Tst_err
	ST_CONTOGG__flacrda_parseSF_const(const Tst_contOgg_opts *pOpts,
	                                  Tst_contOgg_codFlac_defParsA *pDPa);
	static Tst_err
	ST_CONTOGG__flacrda_parseSF_fix(const Tst_contOgg_opts *pOpts,
	                                Tst_contOgg_codFlac_defParsA *pDPa);
	static Tst_err
	ST_CONTOGG__flacrda_parseSF_verbatim(const Tst_contOgg_opts *pOpts,
	                                     Tst_contOgg_codFlac_defParsA *pDPa);
	static Tst_err
	ST_CONTOGG__flacrda_parseSF_lpc(const Tst_contOgg_opts *pOpts,
	                                Tst_contOgg_codFlac_defParsA *pDPa);
	static Tst_err
	ST_CONTOGG__flacrda_parseSF_resid(const Tst_contOgg_opts *pOpts,
	                                  Tst_contOgg_codFlac_defParsA *pDPa);
	/** */
	static Tst_err
	ST_CONTOGG__flacrda_restoreFixed(const Tst_int32 *pResids,
	                                 const Tst_uint32 sampleCnt,
	                                 const Tst_uint32 predOrder,
	                                 Tst_int32 *pRawSamples);
	/** */
	static Tst_err
	ST_CONTOGG__flacrda_restoreLPC(const Tst_contOgg__codFlac_restoreLPC
	                                 *pLPC,
	                               Tst_int32 *pRawSamples);
	static Tst_err
	ST_CONTOGG__flacrda_restoreLPC_32bit(const Tst_contOgg__codFlac_restoreLPC2
	                                       *pLPC2,
	                                     Tst_int32 *pRawSamples2);
	static Tst_err
	ST_CONTOGG__flacrda_restoreLPC_64bit(const Tst_contOgg__codFlac_restoreLPC2
	                                       *pLPC2,
	                                     Tst_int32 *pRawSamples2);
#endif  /* private */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_AC_FLAC_ZRDA_PP_H */

/******************************************************************************/
