/******************************************************************************/
/* w-md5-prot.h                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for MD5 hashsum creation etc.                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2010 (dd.mm.yyyy)                                     */
/*                                                                            */
/* This file contains wrapping functions for the MD5 functions                */
/*   that libflac uses and that were written by                               */
/*     Colin Plumb (MD5) and Josh Coalson (Flac)                              */
/* Those functions etc. that I wrote are marked with "by Tas".                */
/******************************************************************************/

#ifndef HEAD_W_MD5_PROT_H
#define HEAD_W_MD5_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*******************************************************************************
 * Types (protected)
 ******************************************************************************/

#if defined(SRC_W_MD5_C) || \
			defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_STCS_C) || \
			defined(SRC_CONT_OGG_OPTS_C) || defined(SRC_CONT_OGG_GS_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_FLAC_ZRDA_C) || \
			defined(SRC_AC_FLAC_STCS_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || \
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** MD5 Context */
	typedef struct {
		void *pObInternal;  /* pointer to Tst_md5__context_intn */
	} Tst_md5_context;
#endif  /* protected */


/*******************************************************************************
 * Functions (protected)
 ******************************************************************************/

#if defined(SRC_W_MD5_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_STCS_C)
	/** structs */
	Tst_err st_md5_stc_initCtx(Tst_md5_context *pCtx);
	void    st_md5_stc_rsetCtx(Tst_md5_context *pCtx);
	void    st_md5_stc_freeCtx(Tst_md5_context *pCtx);

	/** */
	void st_md5_update(Tst_md5_context *pContext,
	                   Tst_buf const *pBuf,
	                   Tst_uint32 len);
	void st_md5_final(Tst_buf digest[16], Tst_md5_context *pContext);
#endif  /* protected */

#if defined(SRC_W_MD5_C) || defined(SRC_AC_FLAC_ZRD_C)
	/** */
	Tst_err st_md5_updateFlac(Tst_md5_context *pContext,
	                          const Tst_int32 **ppSamples,
	                          const Tst_uint32 channels,
	                          const Tst_uint32 samplesPerChannel,
	                          const Tst_uint32 bytesPerSample);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_W_MD5_PROT_H */

/******************************************************************************/
