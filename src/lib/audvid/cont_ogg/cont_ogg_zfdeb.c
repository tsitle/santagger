/******************************************************************************/
/* cont_ogg_zfdeb.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for OGG debugging                                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_math.h"
/** */
#define SRC_CONT_OGG_ZFDEB_C
#include "cont_ogg_zfdeb-prot.h"
/*** */
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_OGG_ZFDEB_C

/*
// System-Includes
*/
#if (ST_CONTOGG_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_CONTOGG__d_debOrAna(const Tst_contOgg_opts *pOpts,
                       const Tst_bool isSrcOggOrFlac,
                       const Tst_bool isNormOrWarn,
                       const char *pFnc,
                       const char *pFmt, va_list *pArgs);
static void
ST_CONTOGG__d_debOrAnaBS1(const Tst_contOgg_opts *pOpts,
                          const Tst_contOgg_substr_intn *pBSI,
                          const Tst_bool isNormOrWarn,
                          const char *pFnc,
                          const char *pFmt, va_list *pArgs);
static void
ST_CONTOGG__d_debOrAnaBS2(const Tst_contOgg_opts *pOpts,
                          const Tst_contOgg_substr_intn *pBSI,
                          const Tst_contOgg_fhd *pFHd,
                          const Tst_bool isNormOrWarn,
                          const char *pFnc,
                          const char *pFmt, va_list *pArgs);
static void
ST_CONTOGG__d_debOrAnaBS3(const Tst_contOgg_opts *pOpts,
                          const Tst_contOgg_substr_intn *pBSI,
                          const Tst_bool isNormOrWarn,
                          const char *pFnc,
                          const char *pFmt, va_list *pArgs);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_contOgg_d_deb(const Tst_contOgg_opts *pOpts,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts))

	va_start(args, pFmt);
	st_audFDeb_dbg(ST_B_TRUE, &pOpts->basOpts,
			pFnc, NULL, pFmt, args);
	va_end(args);
}

void
st_contOgg_d_debBS1(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;
	char    msg1[ST_DEBMSG_MAXSZ],
	        msg2[ST_DEBMSG_MAXSZ];

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts) ||
			pBSI == NULL)

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	va_end(args);
	snprintf(msg1, sizeof(msg1), "bs #%u.%u: %s",
			pBSI->grpIx, pBSI->grpSIx, msg2);
	st_contOgg_d_deb(pOpts, pFnc, msg1);
}

void
st_contOgg_d_debBS2(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const Tst_contOgg_fhd *pFHd,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;
	char    msg1[4096 * 2],
	        msg2[2048 * 2];

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts) ||
			pBSI == NULL || pFHd == NULL)

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	va_end(args);
	snprintf(msg1, sizeof(msg1), "bs #%u.%u ofra #%u: %s",
			pBSI->grpIx, pBSI->grpSIx, pFHd->realNr, msg2);
	st_contOgg_d_deb(pOpts, pFnc, msg1);
}

void
st_contOgg_d_debBS3(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;
	char    msg1[4096 * 2],
	        msg2[2048 * 2];

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts) ||
			pBSI == NULL || pBSI->pFHdBeg == NULL)

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	va_end(args);
	snprintf(msg1, sizeof(msg1), "bs #%u.%u pack #%u ofrB #%u: %s",
			pBSI->grpIx, pBSI->grpSIx, pBSI->packCur.nr,
			pBSI->pFHdBeg->realNr, msg2);
	st_contOgg_d_deb(pOpts, pFnc, msg1);
}

void
st_contOgg_d_debBS3d(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list    args;
	char       msg1[4096 * 2],
	           msg2[2048 * 2];
	Tst_uint32 frames;

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts) ||
			pBSI == NULL || pBSI->pFHdBeg == NULL)

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	va_end(args);
	switch (pBSI->bsType) {
	/*case ST_CONTOGG_BSTP_VORB:
	case ST_CONTOGG_BSTP_THEO:
	case ST_CONTOGG_BSTP_SPEE:*/
	case ST_CONTOGG_BSTP_FLAC:
		frames = (pBSI->pFlac ? pBSI->pFlac->aFramesCnt : 0); break;
	default:
		frames = 0;
	}
	snprintf(msg1, sizeof(msg1),
			"bs #%u.%u pack #%u ofrB #%u cfr #%u:\n\t\t%s",
			pBSI->grpIx, pBSI->grpSIx, pBSI->packCur.nr,
			pBSI->pFHdBeg->realNr, frames, msg2);
	st_contOgg_d_deb(pOpts, pFnc, msg1);
}

void
st_contOgg_d_debBS3d_flacSF(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_flacSubFr *pSFr,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;
	char    msg1[4096 * 2],
	        msg2[2048 * 2];

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts))

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	va_end(args);
	if (pSFr != NULL)
		snprintf(msg1, sizeof(msg1),
				"cfr #%u cha #%u:\n\t\t%s",
				pSFr->frameNr, pSFr->channel, msg2);
	else
		snprintf(msg1, sizeof(msg1),
				"%s", msg2);
	st_contOgg_d_deb(pOpts, pFnc, msg1);
}

void
st_contOgg_d_debBS4(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;
	char    msg1[4096 * 2],
	        msg2[2048 * 2];

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_DEB_BD(pOpts->basOpts) ||
			pBSI == NULL)

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	va_end(args);
	snprintf(msg1, sizeof(msg1), "bs #%u.%u pack #%u: %s",
			pBSI->grpIx, pBSI->grpSIx, pBSI->packCur.nr,
			msg2);
	st_contOgg_d_deb(pOpts, pFnc, msg1);
}

void
st_contOgg_d_err(const Tst_contOgg_opts *pOpts,
		const char *pFnc, const Tst_str *pFn, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_ERR_BD(pOpts->basOpts))

	va_start(args, pFmt);
	st_audFDeb_dbg(ST_B_FALSE, &pOpts->basOpts,
			pFnc, pFn, pFmt, args);
	va_end(args);
}

void
st_contOgg_d_errBS2(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const Tst_contOgg_fhd *pFHd, const char *pFnc,
		const Tst_str *pFn, const char *pFmt, ...)
{
	va_list args;
	char    msg1[4096 * 2],
	        msg2[2048 * 2];

	ST_ASSERTN_VOID(pOpts == NULL ||
			! ST_AVFDEB_HASCBS_ERR_BD(pOpts->basOpts) ||
			pBSI == NULL || pFHd == NULL)

	va_start(args, pFmt);
	vsnprintf(msg2, sizeof(msg2), pFmt, args);
	va_end(args);
	snprintf(msg1, sizeof(msg1), "bs #%u.%u ofra #%u: %s",
			pBSI->grpIx, pBSI->grpSIx, pFHd->realNr, msg2);
	st_contOgg_d_err(pOpts, pFnc, pFn, msg1);
}

/*----------------------------------------------------------------------------*/

void
st_contOgg_d_debOrAna(const Tst_contOgg_opts *pOpts,
		const Tst_bool isSrcOggOrFlac,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAna(pOpts, isSrcOggOrFlac,
			ST_B_TRUE, pFnc, pFmt, &args);
	va_end(args);
}

void
st_contOgg_d_debOrAna_warn(const Tst_contOgg_opts *pOpts,
		const Tst_bool isSrcOggOrFlac,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAna(pOpts, isSrcOggOrFlac,
			ST_B_FALSE, pFnc, pFmt, &args);
	va_end(args);
}

void
st_contOgg_d_debOrAnaBS1(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL || pBSI == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAnaBS1(pOpts, pBSI,
			ST_B_TRUE, pFnc, pFmt, &args);
	va_end(args);
}

void
st_contOgg_d_debOrAnaBS1warn(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL || pBSI == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAnaBS1(pOpts, pBSI,
			ST_B_FALSE, pFnc, pFmt, &args);
	va_end(args);
}

void
st_contOgg_d_debOrAnaBS2(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const Tst_contOgg_fhd *pFHd,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL || pBSI == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAnaBS2(pOpts, pBSI, pFHd,
			ST_B_TRUE, pFnc, pFmt, &args);
	va_end(args);
}

void
st_contOgg_d_debOrAnaBS2warn(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const Tst_contOgg_fhd *pFHd,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL || pBSI == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAnaBS2(pOpts, pBSI, pFHd,
			ST_B_FALSE, pFnc, pFmt, &args);
	va_end(args);
}

void
st_contOgg_d_debOrAnaBS3(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL || pBSI == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAnaBS3(pOpts, pBSI,
			ST_B_TRUE, pFnc, pFmt, &args);
	va_end(args);
}

void
st_contOgg_d_debOrAnaBS3warn(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const char *pFnc, const char *pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pOpts == NULL || pBSI == NULL)

	va_start(args, pFmt);
	ST_CONTOGG__d_debOrAnaBS3(pOpts, pBSI,
			ST_B_FALSE, pFnc, pFmt, &args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_CONTOGG__d_debOrAna(const Tst_contOgg_opts *pOpts,
		const Tst_bool isSrcOggOrFlac,
		const Tst_bool isNormOrWarn, const char *pFnc,
		const char *pFmt, va_list *pArgs)
{
	char msg1[4096 * 2],
	     msg2[2048 * 2];

	vsnprintf(msg2, sizeof(msg2), pFmt, *pArgs);
	if (pOpts->anlz && ST_AVFDEB_HASCB_ANAL_P(pOpts)) {
		if (isSrcOggOrFlac)
			snprintf(msg1, sizeof(msg1), "AUD: %sogg: %s",
					(isNormOrWarn ? "" : "! "), msg2);
		else
			snprintf(msg1, sizeof(msg1), "AUD: %sflac: %s",
					(isNormOrWarn ? "" : "! "), msg2);
		pOpts->cbAnaMsg(msg1);
	} else if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_deb(pOpts, pFnc, msg2);
}

static void
ST_CONTOGG__d_debOrAnaBS1(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const Tst_bool isNormOrWarn, const char *pFnc,
		const char *pFmt, va_list *pArgs)
{
	char msg1[4096 * 2],
	     msg2[2048 * 2];

	vsnprintf(msg2, sizeof(msg2), pFmt, *pArgs);
	if (pOpts->anlz && ST_AVFDEB_HASCB_ANAL_P(pOpts)) {
		if (pBSI->isSrcOggOrFlac)
			snprintf(msg1, sizeof(msg1), "AUD: %sogg: bs #%u.%u: %s",
					(isNormOrWarn ? "" : "! "),
					pBSI->grpIx, pBSI->grpSIx, msg2);
		else
			snprintf(msg1, sizeof(msg1), "AUD: %sflac: %s",
					(isNormOrWarn ? "" : "! "), msg2);
		pOpts->cbAnaMsg(msg1);
	} else if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS1(pOpts, pBSI, pFnc, msg2);
}

static void
ST_CONTOGG__d_debOrAnaBS2(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const Tst_contOgg_fhd *pFHd, const Tst_bool isNormOrWarn,
		const char *pFnc, const char *pFmt, va_list *pArgs)
{
	char msg1[4096 * 2],
	     msg2[2048 * 2];

	vsnprintf(msg2, sizeof(msg2), pFmt, *pArgs);
	if (pOpts->anlz && ST_AVFDEB_HASCB_ANAL_P(pOpts)) {
		if (pBSI->isSrcOggOrFlac)
			snprintf(msg1, sizeof(msg1), "AUD: %sogg: bs #%u.%u ofra #%u: %s",
					(isNormOrWarn ? "" : "! "),
					pBSI->grpIx, pBSI->grpSIx, pFHd->realNr, msg2);
		else
			snprintf(msg1, sizeof(msg1), "AUD: %sflac: %s",
					(isNormOrWarn ? "" : "! "), msg2);
		pOpts->cbAnaMsg(msg1);
	} else if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS2(pOpts, pBSI, pFHd, pFnc, msg2);
}

static void
ST_CONTOGG__d_debOrAnaBS3(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI,
		const Tst_bool isNormOrWarn, const char *pFnc,
		const char *pFmt, va_list *pArgs)
{
	char msg1[4096 * 2],
	     msg2[2048 * 2];

	vsnprintf(msg2, sizeof(msg2), pFmt, *pArgs);
	if (pOpts->anlz && ST_AVFDEB_HASCB_ANAL_P(pOpts)) {
		if (pBSI->isSrcOggOrFlac)
			snprintf(msg1, sizeof(msg1), "AUD: %sogg: bs #%u.%u: %s",
					(isNormOrWarn ? "" : "! "),
					pBSI->grpIx, pBSI->grpSIx, msg2);
		else
			snprintf(msg1, sizeof(msg1), "AUD: %sflac: %s",
					(isNormOrWarn ? "" : "! "), msg2);
		pOpts->cbAnaMsg(msg1);
	} else if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, pFnc, msg2);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_CONTOGG_DEB_ == 1)
void st_contOgg_prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

void st_contOgg_prf64(const char *pMsg, const Tst_uint64 *pUI64)
{
	Tst_str ui64hex[50],
	        ui64dec[50];

	ui64hex[0] = 0;
	ui64dec[0] = 0;
	st_sysUInt64_toHexStr(pUI64, ui64hex, sizeof(ui64hex));
	st_sysUInt64_toDecStr(pUI64, ui64dec, sizeof(ui64dec));
	st_contOgg_prf("%s %s  (%s)\n", pMsg, ui64dec, ui64hex);
}

void st_contOgg_prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}

void st_contOgg_prBitsInByte(const Tst_byte byt)
{
	Tst_byte x;

	for (x = 0; x < 8; x++)
		st_contOgg_prf("%d", (byt >> (8 - 1 - x)) & 0x01);
}

void st_contOgg_prBuf(const Tst_buf *pBuf, const Tst_uint32 sz)
{
	Tst_uint32 x;

	for (x = 0; x < sz; x++)
		st_contOgg_prf("%02x", pBuf[x]);
}
#endif

/******************************************************************************/
