/******************************************************************************/
/* tag_apev2_opts.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for APEv2 Tag options                                            */
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
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_OPTS_C
#include "tag_apev2-prot.h"
#undef SRC_TAG_APEV2_OPTS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_apev2_opts_setBasicOpts(Tst_apev2_tag *pTag,
		ST_OPTARG(const Tst_basOpts *pBOpts))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	if (pBOpts == NULL)
		ST_RSETSTC_BASOPTS(
				((Tst_apev2_tag_intn*)pTag->pObInternal)->opts.basOpts)
	else
		ST_COPYSTC_BASOPTS(*pBOpts,
				((Tst_apev2_tag_intn*)pTag->pObInternal)->opts.basOpts)
}

/*----------------------------------------------------------------------------*/

void
st_apev2_opts_setReadOnlyBasics(Tst_apev2_tag *pTag,
		const Tst_bool rdOnlyBasics)
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_apev2_tag_intn*)pTag->pObInternal)->opts.rdOnlyBasics = rdOnlyBasics;
}

/******************************************************************************/
