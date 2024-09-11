/******************************************************************************/
/* mc_vorbcomm_gs-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected getter/setter functions for Vorbis Comments                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 24.01.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MC_VORBCOMM_GS_PROT_H
#define HEAD_MC_VORBCOMM_GS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/audvid/cont_ogg-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_CONT_OGG_ZWR_C)
	Tst_bool
	st_contOgg_vorbComm_gs_getRC_wasModified(const Tst_contOgg_vorbCommentRaw *pVCR);

	Tst_uint32
	st_contOgg_vorbComm_gs_getRC_srcPackNr(const Tst_contOgg_vorbCommentRaw *pVCR);

	Tst_int32
	st_contOgg_vorbComm_gs_getRC_srcCmtBegPos(const Tst_contOgg_vorbCommentRaw *pVCR);

	Tst_int32
	st_contOgg_vorbComm_gs_getRC_srcCmtEndPos(const Tst_contOgg_vorbCommentRaw *pVCR);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_MC_VORBCOMM_GS_PROT_H */

/******************************************************************************/
