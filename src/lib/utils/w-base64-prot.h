/******************************************************************************/
/* w-base64-prot.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for Base64 data en-/decoding                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 27.04.2010 (dd.mm.yyyy)                                     */
/*                                                                            */
/* This file contains wrapping functions for the Base64 stuff.                */
/* Those functions etc. that I wrote are marked with "by Tas".                */
/******************************************************************************/

#ifndef LIBSANTAGGER_W_BASE64_PROT_H
#define LIBSANTAGGER_W_BASE64_PROT_H

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_W_BASE64_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "src/includes/common/binobj-defs.h"


ST_BEGIN_C_DECLS

/*******************************************************************************
 * Functions (protected)
 ******************************************************************************/

#if defined(SRC_W_BASE64_C) || \
			defined(SRC_TAG_VORBC_ZRD_C) || defined(SRC_TAG_VORBC_ZWR_C) || \
			defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C)
	/** */
#	if 0
	Tst_err
	st_base64_encode(const Tst_buf *pInp,
	                 const Tst_uint32 inSz,
	                 const Tst_bool withNULterm,
	                 Tst_str **ppOutp,
	                 Tst_uint32 *pOutSz);
	Tst_err
	st_base64_decode(const Tst_str *pInp,
	                 const Tst_uint32 inSz,
	                 Tst_buf **ppOutp,
	                 Tst_uint32 *pOutSz);
#	endif
	/** */
	Tst_err
	st_base64_encode_binobj(Tst_binobj *pBinDatIn,
	                        const Tst_bool withNULterm,
	                        Tst_binobj *pBinDatOut);
	Tst_err
	st_base64_decode_binobj(Tst_binobj *pBinDatIn,
	                        Tst_binobj *pBinDatOut);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_W_BASE64_PROT_H */

/******************************************************************************/
