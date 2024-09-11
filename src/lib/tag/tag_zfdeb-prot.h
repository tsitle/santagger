/******************************************************************************/
/* tag_zfdeb-prot.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for debugging Tags                                 */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 02.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ZFDEB_PROT_H
#define HEAD_TAG_ZFDEB_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_TAG_ZFDEB_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "src/includes/common/string_mte-defs.h"

/*
// System-Includes
*/
#include <stdarg.h>      /* va_list, ... */


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_ZFDEB_C) || \
			defined(SRC_TAG_ID3V2_ZFDEB_C) || \
			defined(SRC_TAG_ID3V1_ZFDEB_C) || \
			defined(SRC_TAG_APEV2_ZFDEB_C) || \
			defined(SRC_TAG_VORBC_ZFDEB_C) || \
			defined(SRC_TAG_CONV_C) || \
			defined(SRC_TAG_COMFNC_C)
	/** */
	Tst_bool
	st_tagFDeb_isVerb(const Tst_int32 verb, const Tst_int32 mode);
	/** */
	void
	st_tagFDeb_dbg(const Tst_bool stdOrErr,
	               const Tst_basOpts *pBasOpts,
	               const Tst_int32 mode,
	               const char *pFnc,
	               const Tst_str *pFn,
	               const char *pTagName,
	               const char *pFieldStr,
	               const char *pFmt,
	               va_list args,
	               const char *pDebStrMsg);
	void
	st_tagFDeb_dbgStr(const Tst_basOpts *pBasOpts,
	                  const Tst_int32 mode,
	                  const char *pFnc,
	                  const char *pTagName,
	                  const char *pFieldStr,
	                  const Tst_bool withPreamble,
	                  const char *pDesc,
	                  const Tst_mtes_string *pStr,
	                  const char *pFmt,
	                  va_list args);
#endif  /* protected */


/*
// Macros (protected)
*/
#if defined(SRC_TAG_ZFDEB_C) || \
			defined(SRC_TAG_ID3V1_ZFDEB_C) || \
				defined(SRC_TAG_ID3V1_C) || \
				defined(SRC_TAG_ID3V1_GS_C) || \
				defined(SRC_TAG_ID3V1_FLDADD_C) || \
				defined(SRC_TAG_ID3V1_ZRD_C) || \
				defined(SRC_TAG_ID3V1_ZWR_C) || \
			defined(SRC_TAG_ID3V2_ZFDEB_C) || \
				defined(SRC_TAG_ID3V2_C) || \
				defined(SRC_TAG_ID3V2_FLDADD_C) || \
				defined(SRC_TAG_ID3V2_FLDGET_C) || \
				defined(SRC_TAG_ID3V2_ZCHK_C) || \
				defined(SRC_TAG_ID3V2_ZTIDY_C) || \
				defined(SRC_TAG_ID3V2_ZRD_C) || \
				defined(SRC_TAG_ID3V2_ZRD2_C) || \
				defined(SRC_TAG_ID3V2_ZWR_C) || \
			defined(SRC_TAG_APEV2_ZFDEB_C) || \
				defined(SRC_TAG_APEV2_C) || \
				defined(SRC_TAG_APEV2_FLDADD_C) || \
				defined(SRC_TAG_APEV2_FLDGETC) || \
				defined(SRC_TAG_APEV2_ZTIDY_C) || \
				defined(SRC_TAG_APEV2_ZRD_C) || \
				defined(SRC_TAG_APEV2_ZWR_C) || \
			defined(SRC_TAG_VORBC_ZFDEB_C) || \
				defined(SRC_TAG_VORBC_C) || \
				defined(SRC_TAG_VORBC_FLDADD_C) || \
				defined(SRC_TAG_VORBC_FLDGETC) || \
				defined(SRC_TAG_VORBC_ZTIDY_C) || \
				defined(SRC_TAG_VORBC_ZRD_C) || \
				defined(SRC_TAG_VORBC_ZWR_C) || \
			defined(SRC_TAG_CONV_C) || \
			defined(SRC_TAG_COMFNC_C)

	/** */
#	define ST_TFDEB_ISVERBTAG_BP(mac_pBOpts)  \
				(mac_pBOpts != NULL && ST_TFDEB_ISVERBTAG_BD(*mac_pBOpts))
#	define ST_TFDEB_ISVERBTAG_BD(mac_bopts)  \
				((mac_bopts).verb != 0 && \
					(ST_ISVERB((mac_bopts).verb, ST_VL_TAG) || \
						ST_ISVERB((mac_bopts).verb, ST_VL_TVREAD) || \
						ST_ISVERB((mac_bopts).verb, ST_VL_TVSET) || \
						ST_ISVERB((mac_bopts).verb, ST_VL_TVWRIT)))
	/** */
#	define ST_TFDEB_HASCBS_DEB_BD(mac_bopts)  \
				((mac_bopts).cbsDbg.cbDbgFnc != NULL)
#	define ST_TFDEB_HASCBS_ERR_BD(mac_bopts)  \
				((mac_bopts).cbsDbg.cbErrFnc != NULL)
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_TAG_ZFDEB_C) || \
			defined(SRC_TAG_ID3V2_C) || \
				defined(SRC_TAG_ID3V2_FNC_C) || \
				defined(SRC_TAG_ID3V2_FLDADD_C) || \
				defined(SRC_TAG_ID3V2_ZCHK_C) || \
				defined(SRC_TAG_ID3V2_ZFDEB_C) || \
				defined(SRC_TAG_ID3V2_ZRD_C) || \
				defined(SRC_TAG_ID3V2_ZRD2_C) || \
				defined(SRC_TAG_ID3V2_ZSYNC_C) || \
				defined(SRC_TAG_ID3V2_ZTIDY_C) || \
				defined(SRC_TAG_ID3V2_ZWR_C) || \
			defined(SRC_TAG_COMFNC_C)
	/** */
#	if (ST_TAG_ZFDEB_DEB_ == 1)
	void st_tagFDeb_prf(const char *pFmt, ...);
	void st_tagFDeb_prE(const char *pFmt, ...);
	void st_tagFDeb_prBits(const Tst_buf *pSrc, const Tst_byte sz);
	void st_tagFDeb_prBuf(const Tst_buf *pBuf, const Tst_uint32 sz);
	void st_tagFDeb_prStr(const char *pMsg, const Tst_mtes_string *pStr);
	void st_tagFDeb_prStrArr(const char *pMsg, const Tst_mtes_strArr *pSArr);
#	endif
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ZFDEB_PROT_H */

/******************************************************************************/
