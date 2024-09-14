/******************************************************************************/
/* tag_comfnc-prot.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected common functions for Tags                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 04.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_COMFNC_PROT_H
#define HEAD_TAG_COMFNC_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
/** */
#include "src/includes/common/binobj-defs.h"
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/streamwr-defs.h"
#include "src/includes/common/string_mte-defs.h"
/** */
#include "tag_basics-prot.h"


ST_BEGIN_C_DECLS

/*
// Types (protected)
*/
#if defined(SRC_TAG_COMFNC_C) || \
			defined(SRC_TAG_ID3V1_C) || \
			defined(SRC_TAG_ID3V2_C) || \
				defined(SRC_TAG_ID3V2_FLDADD_C) || \
				defined(SRC_TAG_ID3V2_ZRD2_C) || \
				defined(SRC_TAG_ID3V2_ZWR_C) || \
			defined(SRC_TAG_APEV2_C) || \
				defined(SRC_TAG_APEV2_FLDADD_C) || \
				defined(SRC_TAG_APEV2_ZRD_C) || \
				defined(SRC_TAG_APEV2_ZWR_C) || \
			defined(SRC_TAG_VORBC_C) || \
				defined(SRC_TAG_VORBC_FLDADD_C) || \
				defined(SRC_TAG_VORBC_ZRD_C) || \
				defined(SRC_TAG_VORBC_ZWR_C)
	/** */
	typedef void (*Tst_tagCFnc_cbDbg)(
			const void *pOpts,
			const int mode,
			const char *pFnc,
			const char *pFmt, ...
		);
	typedef void (*Tst_tagCFnc_cbErr)(
			const void *pOpts,
			const char *pFnc,
			const Tst_str *pFn,
			const char *pFmt, ...
		);
#endif  /* protected */


/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_COMFNC_C) || \
			defined(SRC_TAG_ID3V1_C) || \
			defined(SRC_TAG_ID3V2_C) || \
				defined(SRC_TAG_ID3V2_FLDADD_C) || \
				defined(SRC_TAG_ID3V2_ZRD2_C) || \
				defined(SRC_TAG_ID3V2_ZWR_C) || \
			defined(SRC_TAG_APEV2_C) || \
				defined(SRC_TAG_APEV2_FLDADD_C) || \
				defined(SRC_TAG_APEV2_ZRD_C) || \
				defined(SRC_TAG_APEV2_ZWR_C) || \
			defined(SRC_TAG_VORBC_C) || \
				defined(SRC_TAG_VORBC_FLDADD_C) || \
				defined(SRC_TAG_VORBC_ZRD_C) || \
				defined(SRC_TAG_VORBC_ZWR_C)
	/** */
	Tst_err
	st_tagCFnc_readStr(
			const Tst_uint32 charWid,
			const Tst_mtes_textEnc textEnc,
			Tst_streamrd *pStrrd,
			const Tst_uint32 maxCopy,
			Tst_uint32 *pCopied,
			Tst_str **ppChar,
			Tst_uint32 *pPCharLen,
			ST_OPTARG(Tst_bool *pHadBOM)
		);
	/** */
	Tst_err
	st_tagCFnc_hasTripleDotEnding(
			const Tst_mtes_string *pStr,
			Tst_bool *pHasTD
		);
	/** */
	Tst_err
	st_tagCFnc_writeTagBOtoStream(
			const Tst_basOpts *pBasOpts,
			const void *pOpts,
			const char *pFnc,
			Tst_tagCFnc_cbDbg cbDbg,
			Tst_tagCFnc_cbErr cbErr,
			const Tst_str *pTagFilen,
			Tst_tagBasics *pTBas,
			ST_OPTARG(Tst_streamwr *pStrwr),
			Tst_binobj *pTagBO
		);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_TAG_COMFNC_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C)
	/** */
	Tst_bool
	st_tagCFnc_genreMerge(Tst_mtes_strArr *pSArr);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_COMFNC_PROT_H */

/******************************************************************************/
