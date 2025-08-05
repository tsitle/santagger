/******************************************************************************/
/* tag_apev2_stcs-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for APEv2 Tag structures                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_APEV2_STCS_PROT_H
#define HEAD_TAG_APEV2_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "tag_apev2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_STCS_C) || \
			defined(SRC_TAG_CONV_C) || defined(SRC_TAG_META_STCS_C)
	/** */
	void st_apev2_stc_rsetTag_prot(Tst_apev2_tag *pTag,
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
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_STCS_C) || \
			defined(SRC_TAG_APEV2_GS_C) || defined(SRC_TAG_APEV2_FLDADD_C) || \
			defined(SRC_TAG_APEV2_FNC_C) || defined(SRC_TAG_APEV2_ZRD_C)
	/** */
	void    st_apev2_stc_rsetFDat_onlyBufs(Tst_apev2_fldData *pFld);
	/** */
	void    st_apev2_stc_initFPr_i(Tst_apev2_fldProp_intn *pFPrI);
	void    st_apev2_stc_rsetFPr_i(Tst_apev2_fldProp_intn *pFPrI);
#	define  st_apev2_stc_freeFPr_i(pFPrI)  st_apev2_stc_rsetFPr_i(pFPrI)
	Tst_err st_apev2_stc_copyFPr_i(ST_OPTARG(const Tst_str *pSrcFKey),
	                               const Tst_apev2_fldProp_intn *pSrcI,
	                               Tst_apev2_fldProp_intn *pDstI);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_APEV2_STCS_PROT_H */

/******************************************************************************/
