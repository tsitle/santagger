/******************************************************************************/
/* tag_apev2_zfdeb.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Debugging functions for APEv2 Tags                                         */
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

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_fnc.h"
/** */
#define SRC_TAG_APEV2_ZFDEB_C
#include "tag_apev2_zfdeb-prot.h"
/*** */
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_APEV2_ZFDEB_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */
#if (ST_TAG_AV2_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define LOC_ISVERB_DEB_  { \
			if (pOpts == NULL || \
					! st_tagFDeb_isVerb( \
						((const Tst_apev2_opts*)pOpts)->basOpts.verb, mode) || \
					! ST_TFDEB_HASCBS_DEB_BD( \
						((const Tst_apev2_opts*)pOpts)->basOpts)) \
				return; }
#define LOC_HAS_CB_ERR_  { \
			if (pOpts == NULL || \
					! ST_TFDEB_HASCBS_ERR_BD( \
						((const Tst_apev2_opts*)pOpts)->basOpts)) \
				return; }

static void
ST_TAV2__d_deb_getFN(const Tst_apev2_fldData_intn *pFldI,
                     char *pFldNameOut, const Tst_uint32 bufSz);
static void
ST_TAV2__d_deb_sub(const Tst_bool stdOrErr,
                   const Tst_apev2_opts *pOpts,
                   const int mode,
                   const char *pFnc,
                   ST_OPTARG(const Tst_str *pFn),
                   ST_OPTARG(const Tst_apev2_fldData_intn *pFldI),
                   const char *pFmt, va_list args);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* same as st_apev2_d_deb() */
void
st_apev2_d_debCB(const void *pOpts, const int mode,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TAV2__d_deb_sub(ST_B_TRUE, (const Tst_apev2_opts*)pOpts,
			mode, pFnc, NULL,
			NULL, pFmt, args);
	va_end(args);
}

/* same as st_apev2_d_err() */
void
st_apev2_d_errCB(const void *pOpts, const char *pFnc,
		const Tst_str *pFn, const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TAV2__d_deb_sub(ST_B_FALSE, (const Tst_apev2_opts*)pOpts,
			0, pFnc, pFn, NULL, pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/

/*
 * Print debug info
 */
void
st_apev2_d_deb(const Tst_apev2_opts *pOpts, const int mode,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TAV2__d_deb_sub(ST_B_TRUE, pOpts, mode, pFnc, NULL,
			NULL, pFmt, args);
	va_end(args);
}

void
st_apev2_d_fdebStr(const Tst_apev2_opts *pOpts, const int mode,
		const char *pFnc, const Tst_apev2_fldData_intn *pFldI,
		const char *pDesc, const Tst_mtes_string *pStr,
		const char *pFmt, ...)
{
	char    fldname[64];
	va_list args;

	LOC_ISVERB_DEB_
	fldname[0] = 0;
	if (pFldI != NULL)
		ST_TAV2__d_deb_getFN(pFldI, fldname, sizeof(fldname));
	va_start(args, pFmt);
	st_tagFDeb_dbgStr(&pOpts->basOpts, mode, pFnc,
			"APEV2", (pFldI == NULL ? NULL : fldname),
			ST_B_TRUE, pDesc, pStr, pFmt, args);
	va_end(args);
}

void
st_apev2_d_fdeb(const Tst_apev2_opts *pOpts, const int mode,
		const char *pFnc, const Tst_apev2_fldData_intn *pFldI,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TAV2__d_deb_sub(ST_B_TRUE, pOpts, mode, pFnc, NULL,
			pFldI, pFmt, args);
	va_end(args);
}

void
st_apev2_d_err(const Tst_apev2_opts *pOpts, const char *pFnc,
		const Tst_str *pFn, const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TAV2__d_deb_sub(ST_B_FALSE, pOpts, 0, pFnc, pFn,
			NULL, pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_TAV2__d_deb_getFN(const Tst_apev2_fldData_intn *pFldI,
		char *pFldNameOut, const Tst_uint32 bufSz)
{
	Tst_uint32 x;
	char       tmpMsg[32];

	if (pFldI == NULL)
		return;
	x = st_sysStrlen2(pFldI->fldPrI.pVIDstr) + 2 + 1;
	if (x > 17 + 3) {
		tmpMsg[0] = '\'';
		strncpy(&tmpMsg[1], pFldI->fldPrI.pVIDstr, 17);
		strncpy(&tmpMsg[1 + 17], "[...]", 5);
		tmpMsg[1 + 17 + 5] = '\'';
	} else
		snprintf(tmpMsg, x, "'%s'", pFldI->fldPrI.pVIDstr);

	/*snprintf(*ppBuf, x, "%-*s #%02u", 11, pT, pFldI->fnr);*/
	snprintf(pFldNameOut, bufSz, "%s%-s #%02u",
			(pFldI->fldPrI.typ == ST_APEV2_FTP_BDAT ? "CBIN " : ""),
			tmpMsg, pFldI->fnr);
}

static void
ST_TAV2__d_deb_sub(const Tst_bool stdOrErr,
		const Tst_apev2_opts *pOpts,
		const int mode,
		const char *pFnc,
		ST_OPTARG(const Tst_str *pFn),
		ST_OPTARG(const Tst_apev2_fldData_intn *pFldI),
		const char *pFmt, va_list args)
{
	char fldname[64];

	fldname[0] = 0;
	if (pFldI != NULL)
		ST_TAV2__d_deb_getFN(pFldI, fldname, sizeof(fldname));
	st_tagFDeb_dbg(stdOrErr, &pOpts->basOpts, mode, pFnc, pFn,
			"APEV2", (pFldI == NULL ? NULL : fldname),
			pFmt, args, NULL);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_TAG_AV2_DEB_ == 1)
void st_apev2_prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

void st_apev2_prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}
#endif

/******************************************************************************/
