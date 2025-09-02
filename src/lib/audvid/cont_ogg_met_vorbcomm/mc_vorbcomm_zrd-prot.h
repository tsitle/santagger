/******************************************************************************/
/* mc_vorbcomm_zrd-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for reading Vorbis Comments                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_MC_VORBCOMM_ZRD_PROT_H
#define LIBSANTAGGER_MC_VORBCOMM_ZRD_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj-defs.h"
/** */
#include "../cont_ogg/cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_ZRD_C)
	/** */
	Tst_err
	st_contOgg_vorbComm_parse(Tst_contOgg_substr_intn *pBSI,
	                          Tst_streamrd *pSObj,
	                          Tst_contOgg_vorbCommentRaw *pVCR,
	                          const Tst_bool hasFramingBit,
	                          const Tst_bool hasMaxSz,
	                          Tst_uint32 maxSz,
	                          const Tst_int32 srcCmtBegPos);
	Tst_err
	st_contOgg_vorbComm_addKey(Tst_contOgg_substr_intn *pBSI,
	                           Tst_contOgg_vorbCommentRaw *pVCR,
	                           const char *pKey,
	                           Tst_binobj *pData);
	/** */
	Tst_err
	st_contOgg_vorbComm_parseBasics(Tst_contOgg_substr_intn *pBSI,
	                                Tst_contOgg_vorbCommentRaw *pVCR);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_MC_VORBCOMM_ZRD_PROT_H */

/******************************************************************************/
