/******************************************************************************/
/* mc_skel_zrd-prot.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for reading Ogg-Skeleton                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MC_SKEL_ZRD_PROT_H
#define HEAD_MC_SKEL_ZRD_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/streamrd-defs.h"
/** */
#include "../cont_ogg/cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C)
	/** */
	Tst_err
	st_contOgg_skelrd_parseHPack(const Tst_contOgg_opts *pOpts,
	                             Tst_contOgg_substr_intn *pBSI,
	                             Tst_streamrd *pSObj,
	                             Tst_contOgg_cbStripStr cbStripStr);
	Tst_err
	st_contOgg_skelrd_parseBone(const Tst_contOgg_opts *pOpts,
	                            Tst_contOgg_substr_intn *pBSI,
	                            Tst_streamrd *pSObj);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_MC_SKEL_ZRD_PROT_H */

/******************************************************************************/
