/******************************************************************************/
/* tag_id3v2_zfdeb-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for debugging ID3v2 Tags                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_ZFDEB_PROT_H
#define HEAD_TAG_ID3V2_ZFDEB_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V2_ZFDEB_C) || defined(SRC_TAG_ID3V2_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C) || defined(SRC_TAG_ID3V2_FNC_C) || \
			defined(SRC_TAG_ID3V2_FLDADD_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZRD2_C) || \
			defined(SRC_TAG_ID3V2_ZWR_C) || defined(SRC_TAG_ID3V2_ZTIDY_C)
	/** */
	void st_id3v2_d_debCB(const void *pOpts,
	                      const int mode,
	                      const char *pFnc,
	                      const char *pFmt, ...);
	void st_id3v2_d_errCB(const void *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const char *pFmt, ...);
	/** */
	void st_id3v2_d_deb(const Tst_id3v2_opts *pOpts,
	                    const int mode,
	                    const char *pFnc,
	                    const char *pFmt, ...);
	void st_id3v2_d_fdeb1(const Tst_id3v2_opts *pOpts,
	                      const int mode,
	                      const char *pFnc,
	                      const Tst_byte tagVerMaj,
	                      const Tst_id3v2_frID frID,
	                      ST_OPTARG(const char *pFrIDstr),
	                      const char *pFmt, ...);
	void st_id3v2_d_fdeb2(const Tst_id3v2_opts *pOpts,
	                      const int mode,
	                      const char *pFnc,
	                      const Tst_id3v2_fldProp_intn *pFldPrI,
	                      const char *pFmt, ...);
	void st_id3v2_d_fdeb3(const Tst_id3v2_opts *pOpts,
	                      const int mode,
	                      const char *pFnc,
	                      const Tst_id3v2_fldData_intn *pFldI,
	                      const char *pFmt, ...);
	/** */
	void st_id3v2_d_debStr(const Tst_id3v2_opts *pOpts,
	                       const int mode,
	                       const char *pFnc,
	                       const Tst_bool withPreamble,
	                       const char *pDesc,
	                       const Tst_mtes_string *pStr,
	                       const char *pFmt, ...);
	void st_id3v2_d_fdeb1Str(const Tst_id3v2_opts *pOpts,
	                         const int mode,
	                         const char *pFnc,
	                         const Tst_byte tagVerMaj,
	                         const Tst_id3v2_frID frID,
	                         ST_OPTARG(const char *pFrIDstr),
	                         const Tst_bool withPreamble,
	                         const char *pDesc,
	                         const Tst_mtes_string *pStr,
	                         const char *pFmt, ...);
	void st_id3v2_d_fdeb2Str(const Tst_id3v2_opts *pOpts,
	                         const int mode,
	                         const char *pFnc,
	                         const Tst_id3v2_fldProp_intn *pFldPrI,
	                         const Tst_bool withPreamble,
	                         const char *pDesc,
	                         const Tst_mtes_string *pStr,
	                         const char *pFmt, ...);
	void st_id3v2_d_fdeb3Str(const Tst_id3v2_opts *pOpts,
	                         const int mode,
	                         const char *pFnc,
	                         const Tst_id3v2_fldData_intn *pFldI,
	                         const Tst_bool withPreamble,
	                         const char *pDesc,
	                         const Tst_mtes_string *pStr,
	                         const char *pFmt, ...);
	/** */
	void st_id3v2_d_err(const Tst_id3v2_opts *pOpts,
	                    const char *pFnc,
	                    const Tst_str *pFn,
	                    const char *pFmt, ...);
	void st_id3v2_d_ferr1(const Tst_id3v2_opts *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const Tst_byte tagVerMaj,
	                      const Tst_id3v2_frID frID,
	                      ST_OPTARG(const char *pFrIDstr),
	                      const char *pFmt, ...);
	void st_id3v2_d_ferr2(const Tst_id3v2_opts *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const Tst_id3v2_fldProp_intn *pFldPrI,
	                      const char *pFmt, ...);
	void st_id3v2_d_ferr3(const Tst_id3v2_opts *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const Tst_id3v2_fldData_intn *pFldI,
	                      const char *pFmt, ...);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_ZFDEB_PROT_H */

/******************************************************************************/
