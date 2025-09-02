/******************************************************************************/
/* tag_apev2_zfdeb-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected debugging functions for APEv2 Tags                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_APEV2_ZFDEB_PROT_H
#define LIBSANTAGGER_TAG_APEV2_ZFDEB_PROT_H

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_TAG_AV2_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "tag_apev2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_ZFDEB_C) || \
			defined(SRC_TAG_APEV2_FLDADD_C) || \
			defined(SRC_TAG_APEV2_ZTIDY_C) || \
			defined(SRC_TAG_APEV2_ZRD_C) || defined(SRC_TAG_APEV2_ZWR_C)
	/** */
	void st_apev2_d_debCB(const void *pOpts,
	                      const int mode,
	                      const char *pFnc,
	                      const char *pFmt, ...);
	void st_apev2_d_errCB(const void *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const char *pFmt, ...);
	/** */
	void st_apev2_d_deb(const Tst_apev2_opts *pOpts,
	                    const int mode,
	                    const char *pFnc,
	                    const char *pFmt, ...);
	void st_apev2_d_fdebStr(const Tst_apev2_opts *pOpts,
	                        const int mode,
	                        const char *pFnc,
	                        const Tst_apev2_fldData_intn *pFldI,
	                        const char *pDesc,
	                        const Tst_mtes_string *pStr,
	                        const char *pFmt, ...);
	void st_apev2_d_fdeb(const Tst_apev2_opts *pOpts,
	                     const int mode,
	                     const char *pFnc,
	                     const Tst_apev2_fldData_intn *pFldI,
	                     const char *pFmt, ...);
	void st_apev2_d_err(const Tst_apev2_opts *pOpts,
	                    const char *pFnc,
	                    const Tst_str *pFn,
	                    const char *pFmt, ...);

	/** */
#	if (ST_TAG_AV2_DEB_ == 1)
	void st_apev2_prf(const char *pFmt, ...);
	void st_apev2_prE(const char *pFmt, ...);
#	endif
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_APEV2_ZFDEB_PROT_H */

/******************************************************************************/
