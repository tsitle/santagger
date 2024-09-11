/******************************************************************************/
/* av_zfdeb.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for debugging Audio/Video stuff                              */
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

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_fnc.h"
/** */
#define SRC_AV_ZFDEB_C
#include "av_zfdeb-prot.h"
#undef SRC_AV_ZFDEB_C

/*
// System-Includes
*/
#include <string.h>      /* memset(),... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_audFDeb_dbg(const Tst_bool stdOrErr,
		const Tst_basOpts *pBasOpts,
		const char *pFnc, const Tst_str *pFn,
		const char *pFmt, va_list args)
{
	char      buf[ST_DEBMSG_MAXSZ];
	Tst_int32 depth;

	ST_ASSERTN_VOID(pBasOpts == NULL)

	/* Fnc name */
	ST_ASSERTN_VOID(st_sysStrEmpty(pFnc) || st_sysStrEmpty(pFmt))

	depth = (strstr(pFnc, "__") != NULL ? 6 : 4) + 3;

	/* Message */
	vsnprintf((char*)&buf, sizeof(buf), pFmt, args);
	if (buf[0] == 0x00)
		return;

	/* print Message */
	if (stdOrErr && pBasOpts->cbsDbg.cbDbgFnc != NULL)
		pBasOpts->cbsDbg.cbDbgFnc(depth, pFnc, (char*)&buf);
	else if (! stdOrErr && pBasOpts->cbsDbg.cbErrFnc != NULL)
		pBasOpts->cbsDbg.cbErrFnc(depth, pFnc, pFn, (char*)&buf);
}

/******************************************************************************/
