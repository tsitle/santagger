/******************************************************************************/
/* cont_ogg_stcs-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for OGG structures                                     */
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

#ifndef HEAD_CONT_OGG_STCS_PROT_H
#define HEAD_CONT_OGG_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/audvid/cont_ogg-defs.h"
/** */
#include "cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_CONT_OGG_C) || defined(SRC_CONT_OGG_STCS_C)
	/** */
	void st_contOgg_stc_rsetAObj_prot(Tst_contOgg_obj *pAObj,
	                                  const Tst_bool resetOpts,
	                                  const Tst_bool resetStrrd,
	                                  Tst_streamrd *pStrrd,
	                                  const Tst_bool resetStrwr,
	                                  Tst_streamwr *pStrwr,
	                                  const Tst_bool resetFilen,
	                                  const Tst_str *pFilen);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_CONT_OGG_STCS_C) || defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || \
			defined(SRC_AC_FLAC_ZRD_C)
	/** */
	Tst_err st_contOgg_stc_initBS(Tst_contOgg_substream *pBS,
	                              const Tst_str *pFilen,
	                              Tst_contOgg_opts *pOpts);
	void    st_contOgg_stc_rsetBS(Tst_contOgg_substream *pBS);
	void    st_contOgg_stc_rsetBS(Tst_contOgg_substream *pBS);

	/** */
	void st_contOgg_stc_rsetOpts(Tst_contOgg_opts *pOpts);

	/** */
	/*** */
	void st_contOgg_stc_rsetFHd(Tst_contOgg_fhd *pFH);
	void st_contOgg_stc_copyFHd(const Tst_contOgg_fhd *pFHsrc,
	                            Tst_contOgg_fhd *pFHdst);
	/*** */
	Tst_err st_contOgg_stc_initFHdArr(Tst_contOgg_frameHdArr *pFA);
	void    st_contOgg_stc_rsetFHdArr(Tst_contOgg_frameHdArr *pFA);
	void    st_contOgg_stc_freeFHdArr(Tst_contOgg_frameHdArr *pFA);
	/*** */
	Tst_err st_contOgg_stc_reszFHdArr(Tst_contOgg_frameHdArr *pFA,
	                                  const Tst_uint32 newSz);
	/*** */
	Tst_contOgg_fhd*
	st_contOgg_stc_getFHdArrElem_byIX(Tst_contOgg_frameHdArr *pFA,
	                                  const Tst_uint32 ix);
	Tst_contOgg_fhd*
	st_contOgg_stc_getFHdArrElem_bySNandFNR(Tst_contOgg_frameHdArr *pFA,
	                                        const Tst_uint32 bsSerial,
	                                        const Tst_uint32 fnr);
	/*** */
	Tst_err st_contOgg_stc_deactFHdArrPrevElems(Tst_contOgg_frameHdArr *pFA,
	                                            const Tst_uint32 bsSerial,
	                                            const Tst_uint32 fnrBeg);

	/** */
	/*** */
	Tst_err st_contOgg_stc_initBsArr(Tst_contOgg_substreamArr *pSA,
	                                 Tst_contOgg_opts *pOpts);
	void    st_contOgg_stc_rsetBsArr(Tst_contOgg_substreamArr *pSA);
#	define  st_contOgg_stc_freeBsArr(pSA)  st_contOgg_stc_rsetBsArr(pSA)
	/*** */
	Tst_err st_contOgg_stc_reszBsArr(const Tst_str *pFilen,
	                                 Tst_contOgg_substreamArr *pSA,
	                                 const Tst_uint32 newSz);
	/*** */
	void    st_contOgg_stc_rsetBsArrPackData(Tst_contOgg_substreamArr *pSA);

	/** */
	void st_contOgg_stc_initPack(Tst_contOgg_packet *pPck);
	void st_contOgg_stc_rsetPack(Tst_contOgg_packet *pPck);
	void st_contOgg_stc_rsetPack_dataOnly(Tst_contOgg_packet *pPck);
	void st_contOgg_stc_freePack(Tst_contOgg_packet *pPck);

	/** */
	Tst_err st_contOgg_stc_reszBuf(Tst_buf **ppBuf,
	                               Tst_uint32 *pBufSz,
	                               const Tst_uint32 newSz);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_STCS_PROT_H */

/******************************************************************************/
