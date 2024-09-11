/******************************************************************************/
/* tag_id3v2_stcs-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for ID3v2 Tag structures                           */
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

#ifndef HEAD_TAG_ID3V2_STCS_PROT_H
#define HEAD_TAG_ID3V2_STCS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_ID3V2_STCS_C) || \
			defined(SRC_TAG_CONV_C) || defined(SRC_TAG_META_STCS_C)
	/** */
	void st_id3v2_stc_rsetTag_prot(Tst_id3v2_tag *pTag,
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
#if defined(SRC_TAG_ID3V2_STCS_C) || defined(SRC_TAG_ID3V2_C) || \
			defined(SRC_TAG_ID3V2_GS_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C) || defined(SRC_TAG_ID3V2_FNC_C) || \
			defined(SRC_TAG_ID3V2_FLDADD_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZTIDY_C) || \
			defined(SRC_TAG_ID3V2_ZWR_C)
	/** */
	void st_id3v2_stc_rsetFDat_onlyBufs(Tst_id3v2_fldData *pDat);

	/** */
	Tst_err st_id3v2_stc_initFPr_i(Tst_id3v2_fldProp_intn *pFldPrI);
	void    st_id3v2_stc_rsetFPr_i(Tst_id3v2_fldProp_intn *pFldPrI);
	void    st_id3v2_stc_freeFPr_i(Tst_id3v2_fldProp_intn *pFldPrI);
	Tst_err st_id3v2_stc_copyFPr_i(const Tst_uint32 srcTagVers,
	                               ST_OPTARG(const Tst_str *pSrcFrIDstr),
	                               const Tst_id3v2_fldProp_intn *pSrcI,
	                               Tst_id3v2_fldProp_intn *pDstI);

	/** */
	void st_id3v2_stc_rsetDatOrdArr(Tst_id3v2_fldd_rwordPArr pArr,
	                                const Tst_uint32 sz);
	void st_id3v2_stc_copyDatTp(const Tst_id3v2_fldd_dattp *pSrc,
	                            Tst_id3v2_fldd_dattp *pDst);
	void st_id3v2_stc_copyStrTp(const Tst_id3v2_fldd_strtp *pSrc,
	                            Tst_id3v2_fldd_strtp *pDst);

	/** */
	void st_id3v2_stc_rsetTHdF(Tst_id3v2_thdFlags *pFl);
	void st_id3v2_stc_rsetFHdF(Tst_id3v2_frhdFlags *pFl);

	/** */
	Tst_err st_id3v2_stc_initFDatOutpArr(Tst_id3v2_fieldsOutp *pFldsO,
	                                     const Tst_uint32 newSz);
	void    st_id3v2_stc_freeFDatOutpArr(Tst_id3v2_fieldsOutp *pFldsO);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_STCS_PROT_H */

/******************************************************************************/
