/******************************************************************************/
/* tag_id3v2_zfdeb.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for debugging ID3v2 Tag stuff                                */
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

/*
// Own-Includes
*/
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_ZFDEB_C
#include "tag_id3v2_zfdeb-prot.h"
/*** */
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_ZFDEB_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define LOC_ISVERB_DEB_  { \
			if (pOpts == NULL || \
					! st_tagFDeb_isVerb(((Tst_id3v2_opts*)pOpts)->basOpts.verb, mode) || \
					! ST_TFDEB_HASCBS_DEB_BD(((Tst_id3v2_opts*)pOpts)->basOpts)) \
				return; }
#define LOC_HAS_CB_ERR_  { \
			if (pOpts == NULL || \
					! ST_TFDEB_HASCBS_ERR_BD(((Tst_id3v2_opts*)pOpts)->basOpts)) \
				return; }

static const char*
ST_TIV2__fdeb_getIDs1(const Tst_byte tagVerMaj,
                      const Tst_id3v2_frID frID,
                      ST_OPTARG(const char *pFrIDstr));

static const char*
ST_TIV2__fdeb_getIDs2(const Tst_id3v2_fldProp_intn *pFldPrI);

static const char*
ST_TIV2__fdeb_getIDs3(const Tst_id3v2_fldData_intn *pFldI);
/** */
static void
ST_TIV2__fdeb_dbg(const Tst_bool stdOrErr,
                  const Tst_id3v2_opts *pOpts,
                  const int mode, const char *pFnc, const Tst_str *pFn,
                  const char *pIDstr, const Tst_uint32 fldNr,
                  const char *pFmt, va_list args);
/** */
static void
ST_TIV2__fdeb_dbgStr(const Tst_id3v2_opts *pOpts,
                     const int mode, const char *pFnc,
                     const char *pIDstr, const Tst_uint32 fldNr,
                     const Tst_bool withPreamble,
                     const char *pDesc, const Tst_mtes_string *pStr,
                     const char *pFmt, va_list args);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* same as st_id3v2_d_deb() */
void
st_id3v2_d_debCB(const void *pOpts, const int mode,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_TRUE, (const Tst_id3v2_opts*)pOpts,
			mode, pFnc, NULL, NULL, 0, pFmt, args);
	va_end(args);
}

/* same as st_id3v2_d_err() */
void
st_id3v2_d_errCB(const void *pOpts, const char *pFnc,
		const Tst_str *pFn,const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_FALSE, (const Tst_id3v2_opts*)pOpts,
			0, pFnc, pFn,
			NULL, 0, pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/

void
st_id3v2_d_deb(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_TRUE, pOpts, mode, pFnc, NULL, NULL, 0, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_fdeb1(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const Tst_byte tagVerMaj,
		const Tst_id3v2_frID frID, ST_OPTARG(const char *pFrIDstr),
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_TRUE, pOpts, mode, pFnc, NULL,
			ST_TIV2__fdeb_getIDs1(tagVerMaj, frID, pFrIDstr), 0, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_fdeb2(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const Tst_id3v2_fldProp_intn *pFldPrI,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_TRUE, pOpts, mode, pFnc, NULL,
			ST_TIV2__fdeb_getIDs2(pFldPrI), 0, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_fdeb3(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const Tst_id3v2_fldData_intn *pFldI,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_TRUE, pOpts, mode, pFnc, NULL,
			ST_TIV2__fdeb_getIDs3(pFldI), (pFldI != NULL ? pFldI->fnr : 0),
			pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/

void
st_id3v2_d_debStr(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const Tst_bool withPreamble,
		const char *pDesc, const Tst_mtes_string *pStr,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbgStr(pOpts, mode, pFnc, NULL, 0,
			withPreamble, pDesc, pStr, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_fdeb1Str(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const Tst_byte tagVerMaj,
		const Tst_id3v2_frID frID, ST_OPTARG(const char *pFrIDstr),
		const Tst_bool withPreamble,
		const char *pDesc, const Tst_mtes_string *pStr,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbgStr(pOpts, mode, pFnc,
			ST_TIV2__fdeb_getIDs1(tagVerMaj, frID, pFrIDstr), 0,
			withPreamble, pDesc, pStr, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_fdeb2Str(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const Tst_id3v2_fldProp_intn *pFldPrI,
		const Tst_bool withPreamble,
		const char *pDesc, const Tst_mtes_string *pStr,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbgStr(pOpts, mode, pFnc,
			ST_TIV2__fdeb_getIDs2(pFldPrI), 0,
			withPreamble, pDesc, pStr, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_fdeb3Str(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const Tst_id3v2_fldData_intn *pFldI,
		const Tst_bool withPreamble,
		const char *pDesc, const Tst_mtes_string *pStr,
		const char *pFmt, ...)
{
	va_list args;

	LOC_ISVERB_DEB_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbgStr(pOpts, mode, pFnc,
			ST_TIV2__fdeb_getIDs3(pFldI), (pFldI != NULL ? pFldI->fnr : 0),
			withPreamble, pDesc, pStr, pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/

void
st_id3v2_d_err(const Tst_id3v2_opts *pOpts,
		const char *pFnc, const Tst_str *pFn,
		const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_FALSE, pOpts, 0, pFnc, pFn,
			NULL, 0, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_ferr1(const Tst_id3v2_opts *pOpts,
		const char *pFnc, const Tst_str *pFn,
		const Tst_byte tagVerMaj,
		const Tst_id3v2_frID frID, ST_OPTARG(const char *pFrIDstr),
		const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_FALSE, pOpts, 0, pFnc, pFn,
			ST_TIV2__fdeb_getIDs1(tagVerMaj, frID, pFrIDstr), 0, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_ferr2(const Tst_id3v2_opts *pOpts,
		const char *pFnc, const Tst_str *pFn,
		const Tst_id3v2_fldProp_intn *pFldPrI,
		const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_FALSE, pOpts, 0, pFnc, pFn,
			ST_TIV2__fdeb_getIDs2(pFldPrI), 0, pFmt, args);
	va_end(args);
}

void
st_id3v2_d_ferr3(const Tst_id3v2_opts *pOpts,
		const char *pFnc, const Tst_str *pFn,
		const Tst_id3v2_fldData_intn *pFldI,
		const char *pFmt, ...)
{
	va_list args;

	LOC_HAS_CB_ERR_
	va_start(args, pFmt);
	ST_TIV2__fdeb_dbg(ST_B_FALSE, pOpts, 0, pFnc, pFn,
			ST_TIV2__fdeb_getIDs3(pFldI), (pFldI != NULL ? pFldI->fnr : 0),
			pFmt, args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static const char*
ST_TIV2__fdeb_getIDs1(const Tst_byte tagVerMaj,
		const Tst_id3v2_frID frID, ST_OPTARG(const char *pFrIDstr))
{
	const char *cID2  = "---",
	           *cID34 = "----";
	Tst_uint32 ix   = 0,
	           slen = st_sysStrlen2(pFrIDstr);
	char const *pIDs;
	Tst_id3v2_fldProp_intn const *pFldPrI;

	if (pFrIDstr != NULL && (slen == 3 || slen == 4))
		return pFrIDstr;

	if (! st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS, frID, &ix))
		return (tagVerMaj == 2 ? cID2 : cID34);
	pFldPrI = (Tst_id3v2_fldProp_intn*)&ST_ID3V2_FLDPROPS[ix];
	pIDs    = (tagVerMaj == 2 ? pFldPrI->pCID02str : pFldPrI->pCID34str);
	if (pIDs == NULL || pIDs[0] == ' ' || pIDs[0] == '-')
		pIDs = (tagVerMaj == 2 ? pFldPrI->pCID34str : pFldPrI->pCID02str);
	if (pIDs == NULL || pIDs[0] == ' ' || pIDs[0] == '-')
		pIDs = (tagVerMaj == 2 ? cID2 : cID34);
	return pIDs;
}

static const char*
ST_TIV2__fdeb_getIDs2(const Tst_id3v2_fldProp_intn *pFldPrI)
{
	const char *cID34 = "----";
	char const *pIDs;

	if (pFldPrI == NULL)
		return cID34;
	pIDs = pFldPrI->pIDstr;
	if (pIDs == NULL || pIDs[0] == ' ' || pIDs[0] == '-')
		pIDs = pFldPrI->vid34str;
	if (pIDs[0] == 0x00 || pIDs[0] == ' ' || pIDs[0] == '-')
		pIDs = pFldPrI->vid02str;
	if (pIDs[0] == 0x00 || pIDs[0] == ' ' || pIDs[0] == '-')
		pIDs = pFldPrI->pCID34str;
	if (pIDs == NULL || pIDs[0] == ' ' || pIDs[0] == '-')
		pIDs = pFldPrI->pCID02str;
	if (pIDs == NULL || pIDs[0] == ' ' || pIDs[0] == '-')
		pIDs = cID34;
	return pIDs;
}

static const char*
ST_TIV2__fdeb_getIDs3(const Tst_id3v2_fldData_intn *pFldI)
{
	const char *cID34 = "----";

	if (pFldI == NULL)
		return cID34;
	return ST_TIV2__fdeb_getIDs2(&pFldI->fldPrI);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_TIV2__fdeb_dbg(const Tst_bool stdOrErr,
		const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc, const Tst_str *pFn,
		const char *pIDstr, const Tst_uint32 fldNr,
		const char *pFmt, va_list args)
{
	char buf[64];

	buf[0] = 0;
	if (pIDstr != NULL) {
		if (fldNr == 0)
			snprintf(buf, sizeof(buf), "%-4s", pIDstr);
		else
			snprintf(buf, sizeof(buf), "%-4s #%02u", pIDstr, fldNr);
	}

	st_tagFDeb_dbg(stdOrErr, &pOpts->basOpts, mode, pFnc, pFn,
			"ID3V2", (pIDstr != NULL ? buf : NULL),
			pFmt, args, NULL);
}

static void
ST_TIV2__fdeb_dbgStr(const Tst_id3v2_opts *pOpts,
		const int mode, const char *pFnc,
		const char *pIDstr, const Tst_uint32 fldNr,
		const Tst_bool withPreamble,
		const char *pDesc, const Tst_mtes_string *pStr,
		const char *pFmt, va_list args)
{
	char buf[64];

	buf[0] = 0;
	if (pIDstr != NULL) {
		if (fldNr == 0)
			snprintf(buf, sizeof(buf), "%-4s", pIDstr);
		else
			snprintf(buf, sizeof(buf), "%-4s #%02u", pIDstr, fldNr);
	}

	st_tagFDeb_dbgStr(&pOpts->basOpts, mode, pFnc,
			"ID3V2", (pIDstr != NULL ? buf : NULL),
			withPreamble, pDesc, pStr, pFmt, args);
}

/******************************************************************************/
