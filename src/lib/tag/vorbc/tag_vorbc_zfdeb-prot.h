/******************************************************************************/
/* tag_vorbc_zfdeb-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected debugging functions for Vorbis Tags                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_VORBC_ZFDEB_PROT_H
#define LIBSANTAGGER_TAG_VORBC_ZFDEB_PROT_H

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_TAG_VORBC_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "tag_vorbc-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_VORBC_ZFDEB_C) || defined(SRC_TAG_VORBC_C) || \
			defined(SRC_TAG_VORBC_FLDADD_C) || \
			defined(SRC_TAG_VORBC_ZTIDY_C) || \
			defined(SRC_TAG_VORBC_ZRD_C) || defined(SRC_TAG_VORBC_ZWR_C)
	/** */
	void st_vorbc_d_debCB(const void *pOpts,
	                      const Tst_int32 mode,
	                      const char *pFnc,
	                      const char *pFmt, ...);
	void st_vorbc_d_errCB(const void *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const char *pFmt, ...);
	/** */
	void st_vorbc_d_deb(const Tst_vorbc_opts *pOpts,
	                    const Tst_int32 mode,
	                    const char *pFnc,
	                    const char *pFmt, ...);
	void st_vorbc_d_fdebStr(const Tst_vorbc_opts *pOpts,
	                        const Tst_int32 mode,
	                        const char *pFnc,
	                        const Tst_vorbc_fldData_intn *pFldI,
	                        const char *pDesc,
	                        const Tst_mtes_string *pStr,
	                        const char *pFmt, ...);
	void st_vorbc_d_fdeb(const Tst_vorbc_opts *pOpts,
	                     const Tst_int32 mode,
	                     const char *pFnc,
	                     const Tst_vorbc_fldData_intn *pFldI,
	                     const char *pFmt, ...);
	void st_vorbc_d_err(const Tst_vorbc_opts *pOpts,
	                    const char *pFnc,
	                    const Tst_str *pFn,
	                    const char *pFmt, ...);

	/** */
#	if (ST_TAG_VORBC_DEB_ == 1)
	void st_vorbc_prf(const char *pFmt, ...);
	void st_vorbc_prE(const char *pFmt, ...);
#	endif
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_VORBC_ZFDEB_PROT_H */

/******************************************************************************/
