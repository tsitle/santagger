/******************************************************************************/
/* tag_id3v2_opts.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
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
/** */
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_OPTS_C
#include "tag_id3v2-prot.h"
#undef SRC_TAG_ID3V2_OPTS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_id3v2_opts_setBasicOpts(Tst_id3v2_tag *pTag,
		ST_OPTARG(const Tst_basOpts *pBOpts))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	if (pBOpts == NULL)
		ST_RSETSTC_BASOPTS(
				((Tst_id3v2_tag_intn*)pTag->pObInternal)->opts.basOpts)
	else
		ST_COPYSTC_BASOPTS(*pBOpts,
				((Tst_id3v2_tag_intn*)pTag->pObInternal)->opts.basOpts)
}

/*----------------------------------------------------------------------------*/

void
st_id3v2_opts_setReadOnlyBasics(Tst_id3v2_tag *pTag,
		const Tst_bool rdOnlyBasics)
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v2_tag_intn*)pTag->pObInternal)->opts.rdOnlyBasics = rdOnlyBasics;
}

void
st_id3v2_opts_setDoUnsyncing(Tst_id3v2_tag *pTag, const Tst_bool doUnsync)
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v2_tag_intn*)pTag->pObInternal)->opts.unsync = doUnsync;
}

void
st_id3v2_opts_setCompressBigFrames(Tst_id3v2_tag *pTag, const Tst_bool doCompr)
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v2_tag_intn*)pTag->pObInternal)->opts.compress = doCompr;
}

void
st_id3v2_opts_setCB_cbZLib(Tst_id3v2_tag *pTag,
		ST_OPTARG(Tst_id3v2_cbZLib_decompress cbZLibDecmpr),
		ST_OPTARG(Tst_id3v2_cbZLib_compress cbZLibCmpr))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v2_tag_intn*)pTag->pObInternal)->opts.cbZLibDecmpr = cbZLibDecmpr;
	((Tst_id3v2_tag_intn*)pTag->pObInternal)->opts.cbZLibCmpr   = cbZLibCmpr;
}

/******************************************************************************/
