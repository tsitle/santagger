/******************************************************************************/
/* aud_mpeg1_stcs-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for MPEG-1 structures                                  */
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

#ifndef HEAD_AUD_MPEG1_STCS_PROT_H
#define HEAD_AUD_MPEG1_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "aud_mpeg1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_STCS_C)
	/** */
	void st_mpeg1_stc_rsetMO_prot(Tst_mpeg1_obj *pMO,
	                              const Tst_bool resetOpts,
	                              const Tst_bool resetStrrd,
	                              Tst_streamrd *pStrrd,
	                              const Tst_bool resetFilen,
	                              const Tst_str *pFilen);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_STCS_C) || \
			defined(SRC_AUD_MPEG1_ZRD_C) || defined(SRC_AUD_MPEG1_ZRD2_C)
	/** */
	void st_mpeg1_stc_rsetOpts(Tst_mpeg1_opts *pOpts);

	/** */
#	define st_mpeg1_stc_initFHd(pFH)  st_mpeg1_stc_rsetFHd(pFH, ST_B_TRUE)
	void   st_mpeg1_stc_rsetFHd(Tst_mpeg1_fhd *pFH, const Tst_bool resetFNr);
#	define st_mpeg1_stc_freeFHd(pFH)  st_mpeg1_stc_rsetFHd(pFH, ST_B_TRUE)
	void   st_mpeg1_stc_copyFHd(const Tst_mpeg1_fhd *pSrc, Tst_mpeg1_fhd *pDst);
	/** */
	void   st_mpeg1_stc_initVI(Tst_mpeg1_vbr *pVBR);
	void   st_mpeg1_stc_rsetVI(Tst_mpeg1_vbr *pVBR);
#	define st_mpeg1_stc_freeVI(pVBR)  st_mpeg1_stc_rsetVI(pVBR)
	/** */
	void   st_mpeg1_stc_initFar(Tst_mpeg1_farnfh *pFar);
	void   st_mpeg1_stc_rsetFar(Tst_mpeg1_farnfh *pFar,
	                            const Tst_bool resetOnlyOutp);
#	define st_mpeg1_stc_freeFar(pFar)  st_mpeg1_stc_rsetFar(pFar, ST_B_FALSE)
	/** */
	void st_mpeg1_stc_rsetFrms(Tst_mpeg1_frames *pFrms);

	/** */
	Tst_err st_mpeg1_stc_initDec(Tst_mpeg1_decoder *pDec);
	void    st_mpeg1_stc_rsetDec(Tst_mpeg1_decoder *pDec);
	void    st_mpeg1_stc_freeDec(Tst_mpeg1_decoder *pDec);
	Tst_err st_mpeg1_stc_reszDecBufInp(Tst_mpeg1_decoder *pDec,
	                                   const Tst_uint32 newSz);
#endif  /* protected */


/*
// Functions (protected), Part III
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_STCS_C)
	/** */
	Tst_err st_mpeg1_stc_initBrCounta(Tst_uint32 ***pppBrCnt);
	void    st_mpeg1_stc_incBrCounta(Tst_uint32 **ppBrCnt, const Tst_uint32 br);
	void    st_mpeg1_stc_freeBrCounta(Tst_uint32 ***pppBrCnt);
#endif  /* protected */


/*
// Macros (protected)
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_STCS_C) || \
			defined(SRC_AUD_MPEG1_ZRD_C) || defined(SRC_AUD_MPEG1_ZRD2_C)
	/** */
#	define ST_MPEG1_STC_RSETFHD_BUT_NOT_FRAMENR(mac_pFH)  { \
				(mac_pFH)->audioVers  = ST_MPEG1_AUDVERS_NONE; \
				(mac_pFH)->layer      = ST_MPEG1_LAYER_NONE; \
				(mac_pFH)->bitrate    = 0; \
				(mac_pFH)->samplerate = 0; \
				(mac_pFH)->paddLen    = 0; \
				(mac_pFH)->hasCRC     = ST_B_FALSE; \
				(mac_pFH)->channMode  = ST_MPEG1_CMODE_NONE; \
				/* */ \
				(mac_pFH)->isOffsOK = ST_B_FALSE; \
				(mac_pFH)->hasVBRhd = ST_B_FALSE; \
				(mac_pFH)->frameSz = 0; \
				/* */ \
				memset((mac_pFH)->rawFHd, 0, sizeof((mac_pFH)->rawFHd)); \
			}
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AUD_MPEG1_STCS_PROT_H */

/******************************************************************************/
