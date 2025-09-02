/******************************************************************************/
/* aud_mpeg1_zrd-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for MPEG-1 Frameheader and VBR Headers                 */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 30.12.2009 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_AUD_MPEG1_ZRD_PROT_H
#define LIBSANTAGGER_AUD_MPEG1_ZRD_PROT_H

/*
// Own-Includes
*/
/** */
#include "aud_mpeg1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_ZRD_C)
	/** */
	Tst_err st_mpeg1_rd_findAndReadNextFrHd(Tst_mpeg1_obj_intn *pMOI,
	                                        Tst_mpeg1_farnfh *pFar);
	Tst_err st_mpeg1_rd_checkFrameType(Tst_mpeg1_obj_intn *pMOI,
	                                   Tst_mpeg1_fhd *pMFH);
	/** */
	void st_mpeg1_rd_computePlaytime(Tst_mpeg1_obj_intn *pMOI);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_AUD_MPEG1_ZRD_PROT_H */

/******************************************************************************/
