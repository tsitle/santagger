/******************************************************************************/
/* vc_drc_theo_stcs-prot.h      [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for OGG-Dirac / OGG-Theora structures                  */
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

#ifndef HEAD_VC_DRC_THEO_STCS_PROT_H
#define HEAD_VC_DRC_THEO_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "vc_drc_theo-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_STCS_C) || defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C)
	/** */
	/*** */
	Tst_err st_contOgg_stc_initTheo(Tst_contOgg_theora *pVid);
	void    st_contOgg_stc_rsetTheo(Tst_contOgg_theora *pVid);
	void    st_contOgg_stc_freeTheo(Tst_contOgg_theora *pVid);
	/*** */
#	define st_contOgg_stc_initTheoIdent(pID)  st_contOgg_stc_rsetTheoIdent(pID)
	void   st_contOgg_stc_rsetTheoIdent(Tst_contOgg_theoIdent *pID);
#	define st_contOgg_stc_freeTheoIdent(pID)  st_contOgg_stc_rsetTheoIdent(pID)

	/** */
	/*** */
	Tst_err st_contOgg_stc_initDira(Tst_contOgg_dirac *pVid);
	void    st_contOgg_stc_rsetDira(Tst_contOgg_dirac *pVid);
#	define  st_contOgg_stc_freeDira(pVid)  st_contOgg_stc_rsetDira(pVid)
	/*** */
#	define  st_contOgg_stc_initDiraIdent(pID)  st_contOgg_stc_rsetDiraIdent(pID)
	void    st_contOgg_stc_rsetDiraIdent(Tst_contOgg_diraIdent *pID);
#	define  st_contOgg_stc_freeDiraIdent(pID)  st_contOgg_stc_rsetDiraIdent(pID)
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_VC_DRC_THEO_STCS_PROT_H */

/******************************************************************************/
