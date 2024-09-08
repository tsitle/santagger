/******************************************************************************/
/* tag_vorbc_stcs-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected functions for Vorbis Tag structures                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_VORBC_STCS_PROT_H
#define HEAD_TAG_VORBC_STCS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_vorbc-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_VORBC_C) || defined(SRC_TAG_VORBC_STCS_C) || \
			defined(SRC_TAG_VORBC_ZRD_C) || \
			defined(SRC_TAG_CONV_C) || defined(SRC_TAG_META_STCS_C)
	/** */
	void st_vorbc_stc_rsetTag_prot(Tst_vorbc_tag *pTag,
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
#if defined(SRC_TAG_VORBC_C) || defined(SRC_TAG_VORBC_STCS_C) || \
			defined(SRC_TAG_VORBC_FNC_C) || defined(SRC_TAG_VORBC_ZRD_C) || \
			defined(SRC_TAG_VORBC_GS_C)
	/** */
	void    st_vorbc_stc_rsetFDat_onlyBufs(Tst_vorbc_fldData *pDat);
	/** */
	void    st_vorbc_stc_initFPr_i(Tst_vorbc_fldProp_intn *pFPrI);
	void    st_vorbc_stc_rsetFPr_i(Tst_vorbc_fldProp_intn *pFPrI);
#	define  st_vorbc_stc_freeFPr_i(pFPrI)  st_vorbc_stc_rsetFPr_i(pFPrI)
	Tst_err st_vorbc_stc_copyFPr_i(ST_OPTARG(const Tst_str *pSrcFKey),
	                               const Tst_vorbc_fldProp_intn *pSrcI,
	                               Tst_vorbc_fldProp_intn *pDstI);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_STCS_PROT_H */

/******************************************************************************/
