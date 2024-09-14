/******************************************************************************/
/* aud_mpeg1_zfdeb-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected debugging functions for MPEG-1 streams                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AUD_MPEG1_ZFDEB_PROT_H
#define HEAD_AUD_MPEG1_ZFDEB_PROT_H

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_AUD_MPEG1_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "aud_mpeg1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_ZFDEB_C) || \
			defined(SRC_AUD_MPEG1_ZRD_C) || defined(SRC_AUD_MPEG1_ZRD2_C)
	/** debug/error */
	void st_mpeg1_d_deb(const Tst_mpeg1_opts *pOpts,
	                    const char *pFnc,
	                    const char *pFmt, ...);
	void st_mpeg1_d_debOrAna(const Tst_mpeg1_opts *pOpts,
	                         const Tst_bool isNormOrWarn,
	                         const char *pFnc,
	                         const char *pFmt, ...);
	void st_mpeg1_d_err(const Tst_mpeg1_opts *pOpts,
	                    const char *pFnc,
	                    const Tst_str *pFn,
	                    const char *pFmt, ...);

	/** */
#	if (ST_AUD_MPEG1_DEB_ == 1)
	void st_mpeg1_prf(const char *pFmt, ...);
	void st_mpeg1_prE(const char *pFmt, ...);
	void st_mpeg1_prBuf(const Tst_buf *pBuf, const Tst_uint32 sz);
#	endif
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AUD_MPEG1_ZFDEB_PROT_H */

/******************************************************************************/
