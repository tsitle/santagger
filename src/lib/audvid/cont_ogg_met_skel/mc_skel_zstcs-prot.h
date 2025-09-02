/******************************************************************************/
/* mc_skel_zstcs-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for OGG-Skeleton structures                            */
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

#ifndef LIBSANTAGGER_MC_SKEL_ZSTCS_PROT_H
#define LIBSANTAGGER_MC_SKEL_ZSTCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "mc_skel-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_STCS_C) || defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C)
	/** */
	void   st_contOgg_stc_initSkel(Tst_contOgg_skeleton *pSk);
	void   st_contOgg_stc_rsetSkel(Tst_contOgg_skeleton *pSk);
#	define st_contOgg_stc_freeSkel(pSk)  st_contOgg_stc_rsetSkel(pSk)
	/** */
	void st_contOgg_stc_rsetSkelIdent(Tst_contOgg_skelIdent *pID);
	/** */
	void   st_contOgg_stc_initSkelBone(Tst_contOgg_skelBone *pSkB);
	void   st_contOgg_stc_rsetSkelBone(Tst_contOgg_skelBone *pSkB);
#	define st_contOgg_stc_freeSkelBone(pSkB)  st_contOgg_stc_rsetSkelBone(pSkB)
	/** */
	void    st_contOgg_stc_initSkelBoneArr(Tst_contOgg_skelBoneArr *pBA);
	void    st_contOgg_stc_rsetSkelBoneArr(Tst_contOgg_skelBoneArr *pBA);
	Tst_err st_contOgg_stc_reszSkelBoneArr(Tst_contOgg_skelBoneArr *pBA,
	                                       const Tst_uint32 newSz);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_MC_SKEL_ZSTCS_PROT_H */

/******************************************************************************/
