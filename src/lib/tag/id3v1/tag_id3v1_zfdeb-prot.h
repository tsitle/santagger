/******************************************************************************/
/* tag_id3v1_zfdeb-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for debugging ID3v1 (Extended) Tags                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V1_ZFDEB_PROT_H
#define HEAD_TAG_ID3V1_ZFDEB_PROT_H

/*
// Own-Includes
*/
/** */
#include "tag_id3v1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V1_C) || defined(SRC_TAG_ID3V1_GS_C) || \
			defined(SRC_TAG_ID3V1_FLDADD_C) || \
			defined(SRC_TAG_ID3V1_ZRD_C) || defined(SRC_TAG_ID3V1_ZWR_C) || \
			defined(SRC_TAG_ID3V1_ZFDEB_C)
	/** */
	void st_id3v1_d_debCB(const void *pOpts,
	                      const Tst_int32 mode,
	                      const char *pFnc,
	                      const char *pFmt, ...);
	void st_id3v1_d_errCB(const void *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const char *pFmt, ...);
	/** */
	void st_id3v1_d_deb(const Tst_id3v1_opts *pOpts,
	                    const Tst_int32 mode,
	                    const char *pFnc,
	                    const char *pFmt, ...);
	void st_id3v1_d_fdeb(const Tst_id3v1_opts *pOpts,
	                     const Tst_int32 mode,
	                     const char *pFnc,
	                     const Tst_id3v1_fldProp_intn *pFldPrI,
	                     const char *pFmt, ...);
	void st_id3v1_d_fdebStr(const Tst_id3v1_opts *pOpts,
	                        const Tst_int32 mode,
	                        const char *pFnc,
	                        const Tst_id3v1_fldProp_intn *pFldPrI,
	                        const Tst_mtes_string *pStr,
	                        const char *pFmt, ...);
	void st_id3v1_d_err(const Tst_id3v1_opts *pOpts,
	                    const char *pFnc,
	                    const Tst_str *pFn,
	                    const char *pFmt, ...);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V1_ZFDEB_PROT_H */

/******************************************************************************/
