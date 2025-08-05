/******************************************************************************/
/* ac_vorb_stcs-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for OGG-Vorbis structures                              */
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

#ifndef HEAD_AC_VORB_STCS_PROT_H
#define HEAD_AC_VORB_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "ac_vorb-prot.h"
#include "../cont_ogg/cont_ogg-common-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_STCS_C) || defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_AC_VORB_C) || defined(SRC_AC_VORB_STCS_C) || \
			defined(SRC_AC_VORB_ZRD_C)
	/** */
	Tst_err st_contOgg_stc_initVorb(Tst_contOgg_vorbis *pAud);
	void    st_contOgg_stc_rsetVorb(Tst_contOgg_vorbis *pAud);
	void    st_contOgg_stc_freeVorb(Tst_contOgg_vorbis *pAud);
	/** */
#	define st_contOgg_stc_initVorbIdent(pID)  st_contOgg_stc_rsetVorbIdent(pID)
	void   st_contOgg_stc_rsetVorbIdent(Tst_contOgg_vorbIdent *pID);
#	define st_contOgg_stc_freeVorbIdent(pID)  st_contOgg_stc_rsetVorbIdent(pID)
	/** */
#	define st_contOgg_stc_initVorbSetup(pSet)  st_contOgg_stc_rsetVorbSetup(pSet)
	void   st_contOgg_stc_rsetVorbSetup(Tst_contOgg_vorbSetup *pSet);
#	define st_contOgg_stc_freeVorbSetup(pSet)  st_contOgg_stc_rsetVorbSetup(pSet)

	/** */
	void st_contOgg_stc_rsetVorbOpts(Tst_contOgg_vorbOpts *pVO);

	/** */
	Tst_err st_contOgg_stc_initVorbDec(Tst_contOgg_vorbDecObj *pDec);
	void    st_contOgg_stc_rsetVorbDec(Tst_contOgg_vorbDecObj *pDec);
	void    st_contOgg_stc_freeVorbDec(Tst_contOgg_vorbDecObj *pDec);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AC_VORB_STCS_PROT_H */

/******************************************************************************/
