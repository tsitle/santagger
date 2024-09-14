/******************************************************************************/
/* cont_wav_zfdeb.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for WAV debugging                                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 09.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#define SRC_CONT_WAV_ZFDEB_C
#include "cont_wav_zfdeb-prot.h"
/*** */
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_WAV_ZFDEB_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_contWav_d_deb(const Tst_contWav_opts *pOpts,
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
st_contWav_d_err(const Tst_contWav_opts *pOpts,
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

/******************************************************************************/
