/******************************************************************************/
/* tag_id3v1_zfdeb.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for debugging ID3v1 (Extended) Tags                              */
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

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/tag/tag_id3_helper-defs.h"
/** */
#define SRC_TAG_ID3V1_ZFDEB_C
#include "tag_id3v1_zfdeb-prot.h"
/*** */
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V1_ZFDEB_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define LOC_ISVERB_DEB_  { \
			if (pOpts == NULL || \
					! st_tagFDeb_isVerb(((Tst_id3v1_opts*)pOpts)->basOpts.verb, mode) || \
					! ST_TFDEB_HASCBS_DEB_BD(((Tst_id3v1_opts*)pOpts)->basOpts)) \
				return; }
#define LOC_HAS_CB_ERR_  { \
			if (pOpts == NULL || \
					! ST_TFDEB_HASCBS_ERR_BD(((Tst_id3v1_opts*)pOpts)->basOpts)) \
				return; }

static void
ST_TIV1__d_getFldName(char buf[], const Tst_uint32 soBuf,
                      const Tst_id3v1_fldProp_intn *pFldPrI);
static void
ST_TIV1__d_deb_sub(const Tst_bool stdOrErr,
                   const Tst_id3v1_opts *pOpts, const int mode,
                   const char *pFnc, const Tst_str *pFn,
                   const Tst_id3v1_fldProp_intn *pFldPrI,
                   const char *pFmt, va_list args);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* same as st_id3v1_d_deb() */
void
st_id3v1_d_debCB(const void *pOpts, const Tst_int32 mode,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV1__d_deb_sub(ST_B_TRUE, (const Tst_id3v1_opts*)pOpts,
			mode, pFnc, NULL, NULL, pFmt, args);
	va_end(args);
}

/* same as st_id3v1_d_err() */
void
st_id3v1_d_errCB(const void *pOpts, const char *pFnc,
		const Tst_str *pFn, const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TIV1__d_deb_sub(ST_B_FALSE, (const Tst_id3v1_opts*)pOpts,
			0, pFnc, pFn, NULL, pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/

/*
 * Print debug info
 */
void
st_id3v1_d_deb(const Tst_id3v1_opts *pOpts, const int mode,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV1__d_deb_sub(ST_B_TRUE, pOpts, mode, pFnc, NULL, NULL, pFmt, args);
	va_end(args);
}

void
st_id3v1_d_fdeb(const Tst_id3v1_opts *pOpts, const int mode,
		const char *pFnc, const Tst_id3v1_fldProp_intn *pFldPrI,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV1__d_deb_sub(ST_B_TRUE, pOpts, mode, pFnc, NULL, pFldPrI, pFmt, args);
	va_end(args);
}

void
st_id3v1_d_fdebStr(const Tst_id3v1_opts *pOpts, const int mode,
		const char *pFnc, const Tst_id3v1_fldProp_intn *pFldPrI,
		const Tst_mtes_string *pStr,
		const char *pFmt, ...)
{
	va_list args;
	char    buf[64];

	buf[0] = 0;
	if (pFldPrI != NULL)
		ST_TIV1__d_getFldName(buf,  sizeof(buf), pFldPrI);

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	st_tagFDeb_dbgStr(&pOpts->basOpts, mode, pFnc,
			"ID3V1", (pFldPrI != NULL ? buf : NULL),
			ST_B_TRUE, NULL, pStr, pFmt, args);
	va_end(args);
}

void
st_id3v1_d_err(const Tst_id3v1_opts *pOpts, const char *pFnc,
		const Tst_str *pFn, const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TIV1__d_deb_sub(ST_B_FALSE, pOpts, 0, pFnc, pFn, NULL, pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_TIV1__d_getFldName(char buf[], const Tst_uint32 soBuf,
		const Tst_id3v1_fldProp_intn *pFldPrI)
{
	if (pFldPrI != NULL &&
			(pFldPrI->id != ST_ID3V1_FID_NONE ||
				pFldPrI->idExt != ST_ID3V1E_FID_NONE) &&
			pFldPrI->pCIDstr != NULL) {
		snprintf(buf, soBuf, "%s", pFldPrI->pCIDstr);
	}
}

static void
ST_TIV1__d_deb_sub(const Tst_bool stdOrErr,
		const Tst_id3v1_opts *pOpts, const int mode,
		const char *pFnc, const Tst_str *pFn,
		const Tst_id3v1_fldProp_intn *pFldPrI,
		const char *pFmt, va_list args)
{
	char buf[64];

	buf[0] = 0;
	if (pFldPrI != NULL)
		ST_TIV1__d_getFldName(buf,  sizeof(buf), pFldPrI);
	/* */
	st_tagFDeb_dbg(stdOrErr, &pOpts->basOpts, mode, pFnc, pFn,
			"ID3V1", (pFldPrI != NULL ? buf : NULL),
			pFmt, args, NULL);
}

/******************************************************************************/
