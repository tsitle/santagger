/******************************************************************************/
/* cont_ogg_zwr_cod-pp.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Private/Protected functions for writing OGG containers / Flac files        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CONT_OGG_ZWR_COD_PP_H
#define HEAD_CONT_OGG_ZWR_COD_PP_H

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
#if defined(SRC_CONT_OGG_ZWR_COD_C) || defined(SRC_CONT_OGG_ZWR_C) || \
			defined(SRC_CONT_OGG_C)
	/** */
	Tst_int32
	st_contOgg_wrcod_fndNextHd(Tst_contOgg_obj_intn *pAObjI,
	                           Tst_contOgg_substr_intn *pBSI,
	                           Tst_streamrd *pStrrd);
	/** */
	Tst_err
	st_contOgg_wrcod_appendVCmt(Tst_contOgg_obj_intn *pAObjI,
	                            Tst_contOgg_substream *pBS);
	/** */
	Tst_err
	st_contOgg_wrcod_flacHeaders(Tst_contOgg_obj_intn *pAObjI,
	                             Tst_contOgg_substream *pBS);
	/** */
	Tst_err
	st_contOgg_wrcod_copyData(const Tst_contOgg_opts *pOpts,
	                          Tst_streamrd *pStrrd,
	                          Tst_streamwr *pStrwr,
	                          const Tst_uint32 toCopy,
	                          Tst_uint32 *pCopied);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_CONT_OGG_ZWR_COD_C
	/** */
	static Tst_err
	ST_CONTOGG__wrcod_copyRVC(const Tst_contOgg_opts *pOpts,
	                          Tst_binobj *pBOsrc,
	                          Tst_binobj *pBOdst,
	                          Tst_streamwr *pStrwr,
	                          Tst_streamrd *pStrrdTmp,
	                          Tst_uint32 *pCopied);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_ZWR_COD_PP_H */

/******************************************************************************/
