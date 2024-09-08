/******************************************************************************/
/* tag_id3v1_stcs-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected functions for ID3v1 (Extended) Tag structures                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V1_STCS_PROT_H
#define HEAD_TAG_ID3V1_STCS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/streamrd-defs.h>
#include <santag/tag/tag_id3v1-defs.h>
/** */
#include "tag_id3v1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V1_C) || defined(SRC_TAG_ID3V1_STCS_C) || \
			defined(SRC_TAG_ID3V1_GS_C) || \
			defined(SRC_TAG_CONV_C) || defined(SRC_TAG_META_STCS_C)
	/** */
	void st_id3v1_stc_rsetTag_prot(Tst_id3v1_tag *pTag,
	                               const Tst_bool resetOpts,
	                               const Tst_bool resetStrrd,
	                               Tst_streamrd *pStrrd,
	                               const Tst_bool resetStrwr,
	                               Tst_streamwr *pStrwr,
	                               const Tst_bool resetFilen,
	                               const Tst_str *pFilen);
	/** */
	void
	st_id3v1_stc_rsetFDat_onlyBufs(Tst_id3v1_fldData *pDat);
	/** */
	Tst_err st_id3v1_stc_initFPr_i(Tst_id3v1_fldProp_intn *pFldPrI);
	void    st_id3v1_stc_rsetFPr_i(Tst_id3v1_fldProp_intn *pFldPrI);
	void    st_id3v1_stc_freeFPr_i(Tst_id3v1_fldProp_intn *pFldPrI);
	Tst_err st_id3v1_stc_copyFPr_i(const Tst_id3v1_fldProp_intn *pSrcI,
	                               Tst_id3v1_fldProp_intn *pDstI);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V1_STCS_PROT_H */

/******************************************************************************/
