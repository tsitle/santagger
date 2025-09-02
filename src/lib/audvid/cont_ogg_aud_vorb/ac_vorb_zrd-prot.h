/******************************************************************************/
/* ac_vorb_zrd-prot.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for reading Ogg-Vorbis                                 */
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

#ifndef LIBSANTAGGER_AC_VORB_ZRD_PROT_H
#define LIBSANTAGGER_AC_VORB_ZRD_PROT_H

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
#if defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C)
	/** */
	Tst_err
	st_contOgg_vorbrd_initDecoder(const Tst_contOgg_opts *pOpts,
	                              Tst_contOgg_substr_intn *pBSI);
	void
	st_contOgg_vorbrd_endDecoder(const Tst_contOgg_opts *pOpts,
	                             Tst_contOgg_substr_intn *pBSI);
	/** */
	Tst_err
	st_contOgg_vorbrd_feedDecoderHeader(const Tst_contOgg_opts *pOpts,
	                                    Tst_contOgg_substr_intn *pBSI);
	Tst_err
	st_contOgg_vorbrd_feedDecoderAudio(const Tst_contOgg_opts *pOpts,
	                                   Tst_contOgg_substr_intn *pBSI);
	/** */
	Tst_err
	st_contOgg_vorbrd_parseHPack(const Tst_contOgg_opts *pOpts,
	                             Tst_contOgg_substr_intn *pBSI,
	                             Tst_byte packTp,
	                             Tst_streamrd *pSObj,
	                             Tst_bool *pStopThen);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_AC_VORB_ZRD_PROT_H */

/******************************************************************************/
