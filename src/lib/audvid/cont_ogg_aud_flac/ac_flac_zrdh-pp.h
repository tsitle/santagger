/******************************************************************************/
/* ac_flac_zrdh-pp.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for reading (Ogg-)Flac header packets          */
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

#ifndef HEAD_AC_FLAC_ZRDH_PP_H
#define HEAD_AC_FLAC_ZRDH_PP_H

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
#if defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_CONT_OGG_ZWR_COD_C)
	/** */
	typedef struct {
		Tst_contOgg_substr_intn *pBSI;
		Tst_streamrd            *pSObj;
	} Tst_contOgg_codFlac_defParsH;
#endif  /* protected */


/*
// Functions (protected)
*/
#if defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_CONT_OGG_ZWR_COD_C)
	/** */
	Tst_err
	st_contOgg_flacrdh_parseH_MD(const Tst_contOgg_opts *pOpts,
	                             Tst_contOgg_codFlac_defParsH *pDPh);
	/** */
	Tst_err
	st_contOgg_flacrdh_parseBH(Tst_streamrd *pSObj,
	                            Tst_contOgg_flacBlockType *pBlockType,
	                            Tst_bool *pIsLast,
	                            Tst_uint32 *pMDsz);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_AC_FLAC_ZRDH_C
	/** */
	static Tst_err
	ST_CONTOGG__flacrdh_parseStrInf(const Tst_contOgg_opts *pOpts,
	                                Tst_contOgg_codFlac_defParsH *pDPh,
	                                const Tst_uint32 mdSz);
	static Tst_err
	ST_CONTOGG__flacrdh_parseComm(const Tst_contOgg_opts *pOpts,
	                              Tst_contOgg_codFlac_defParsH *pDPh,
	                              const Tst_uint32 mdSz);
	static Tst_err
	ST_CONTOGG__flacrdh_parsePict(const Tst_contOgg_opts *pOpts,
	                              Tst_contOgg_codFlac_defParsH *pDPh,
	                              const Tst_uint32 mdSz);
	static Tst_err
	ST_CONTOGG__flacrdh_parseApp(const Tst_contOgg_opts *pOpts,
	                             Tst_contOgg_codFlac_defParsH *pDPh,
	                             Tst_uint32 mdSz);
	static Tst_err
	ST_CONTOGG__flacrdh_parseSeek(const Tst_contOgg_opts *pOpts,
	                              Tst_contOgg_codFlac_defParsH *pDPh,
	                              Tst_uint32 mdSz);
	static Tst_err
	ST_CONTOGG__flacrdh_parseCue(const Tst_contOgg_opts *pOpts,
	                             Tst_contOgg_codFlac_defParsH *pDPh,
	                             const Tst_uint32 mdSz);
	static Tst_err
	ST_CONTOGG__flacrdh_parseCue_track(const Tst_contOgg_opts *pOpts,
	                                   Tst_contOgg_codFlac_defParsH *pDPh,
	                                   const Tst_byte curTrkNr,
	                                   const Tst_bool isCDDA);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_AC_FLAC_ZRDH_PP_H */

/******************************************************************************/
