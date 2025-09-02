/******************************************************************************/
/* ac_spx_clt_stcs-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for Ogg-Speex and Ogg-Celt structures                  */
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

#ifndef LIBSANTAGGER_AC_SPX_CLT_STCS_PROT_H
#define LIBSANTAGGER_AC_SPX_CLT_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "ac_spx_clt-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_AC_SPX_CLT_STCS_C) || \
			defined(SRC_CONT_OGG_STCS_C) || defined(SRC_CONT_OGG_ZRD_COD_C)
	/** Speex */
	/*** */
	Tst_err st_contOgg_stc_initSpee(Tst_contOgg_speex *pAud);
	void    st_contOgg_stc_rsetSpee(Tst_contOgg_speex *pAud);
	void    st_contOgg_stc_freeSpee(Tst_contOgg_speex *pAud);
	/*** */
	void    st_contOgg_stc_initSpeeIdent(Tst_contOgg_speeIdent *pID);
	void    st_contOgg_stc_rsetSpeeIdent(Tst_contOgg_speeIdent *pID);
#	define  st_contOgg_stc_freeSpeeIdent(pID)  st_contOgg_stc_rsetSpeeIdent(pID)

	/** Celt */
	/*** */
	Tst_err st_contOgg_stc_initCelt(Tst_contOgg_celt *pAud);
	void    st_contOgg_stc_rsetCelt(Tst_contOgg_celt *pAud);
	void    st_contOgg_stc_freeCelt(Tst_contOgg_celt *pAud);
	/*** */
	void   st_contOgg_stc_initCeltIdent(Tst_contOgg_celtIdent *pID);
	void   st_contOgg_stc_rsetCeltIdent(Tst_contOgg_celtIdent *pID);
#	define st_contOgg_stc_freeCeltIdent(pID)  st_contOgg_stc_rsetCeltIdent(pID)
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_AC_SPX_CLT_STCS_PROT_H */

/******************************************************************************/
