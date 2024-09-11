/******************************************************************************/
/* tag_vorbc_opts.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Vorbis Tag options                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_TAG_VORBC_OPTS_C
#include "tag_vorbc-prot.h"
#undef SRC_TAG_VORBC_OPTS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_vorbc_opts_setBasicOpts(Tst_vorbc_tag *pTag,
		ST_OPTARG(const Tst_basOpts *pBOpts))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	if (pBOpts == NULL)
		ST_RSETSTC_BASOPTS(
				((Tst_vorbc_tag_intn*)pTag->pObInternal)->opts.basOpts)
	else
		ST_COPYSTC_BASOPTS(*pBOpts,
				((Tst_vorbc_tag_intn*)pTag->pObInternal)->opts.basOpts)
}

/*----------------------------------------------------------------------------*/

void
st_vorbc_opts_setReadOnlyBasics(Tst_vorbc_tag *pTag,
		const Tst_bool rdOnlyBasics)
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_vorbc_tag_intn*)pTag->pObInternal)->opts.rdOnlyBasics = rdOnlyBasics;
}

/******************************************************************************/
