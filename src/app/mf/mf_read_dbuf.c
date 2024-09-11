/******************************************************************************/
/* mf_read_dbuf.c                 [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for audio decoding buffers                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#define SRC_MF_READ_DBUF_C
#include "mf_read_dbuf-prot.h"
#undef SRC_MF_READ_DBUF_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), random() */
#include <string.h>      /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (HAVE_LIBMPG123 == 1) || \
			(HAVE_LIBMAD == 1) || \
			(HAVE_LIBVORBIS == 1)

/* Tast_mf_rddec_dbuf */
void
ast_mf_rddec_stc_initDB(Tast_mf_rddec_dbuf *pDBuf)
{
	ST_ASSERTN_VOID(pDBuf == NULL)

	pDBuf->ppBufOutI32 = NULL;
	pDBuf->channels    = 0;
	pDBuf->pBufTmp     = NULL;
	/* */
	ast_mf_rddec_stc_rsetDB(pDBuf);
}

void
ast_mf_rddec_stc_rsetDB(Tast_mf_rddec_dbuf *pDBuf)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pDBuf == NULL)

	pDBuf->sampleCnt    = 0;
	pDBuf->sampleCntRes = 0;
	if (pDBuf->ppBufOutI32 != NULL) {
		for (x = 0; x < pDBuf->channels; x++)
			ST_DELPOINT(pDBuf->ppBufOutI32[x])
		ST_DELPOINT(pDBuf->ppBufOutI32)
	}
	/* */
	pDBuf->channels     = 0;
	pDBuf->sampleR      = 0;
	pDBuf->bps          = 0;
	pDBuf->isOutpFmtSet = ST_B_FALSE;
	/* */
	ST_DELPOINT(pDBuf->pBufTmp)
	pDBuf->bufTmpSz = 0;
}

Tst_err
ast_mf_rddec_stc_reszDecBufOutI32(Tast_mf_rddec_dbuf *pDBuf,
		const Tst_uint32 newSampleCnt)
{
	Tst_uint32 x;

	ST_ASSERTN_IARG(pDBuf == NULL)

	if (pDBuf->ppBufOutI32 == NULL) {
		ST_CALLOC(pDBuf->ppBufOutI32, Tst_int32**,
				pDBuf->channels, sizeof(Tst_int32*))
		if (pDBuf->ppBufOutI32 == NULL)
			return ST_ERR_OMEM;
		for (x = 0; x < pDBuf->channels; x++)
			pDBuf->ppBufOutI32[x] = NULL;
	}
	if (newSampleCnt > pDBuf->sampleCntRes) {
		for (x = 0; x < pDBuf->channels; x++) {
			ST_REALLOC(pDBuf->ppBufOutI32[x], Tst_int32*,
					newSampleCnt, sizeof(Tst_int32))
			if (pDBuf->ppBufOutI32[x] == NULL)
				return ST_ERR_OMEM;
			memset(&pDBuf->ppBufOutI32[x][pDBuf->sampleCntRes], 0,
					newSampleCnt - pDBuf->sampleCntRes);
		}
		pDBuf->sampleCntRes = newSampleCnt;
	}
	return ST_ERR_SUCC;
}

#endif  /* libmpg123 or libmad or libvorbis */

/******************************************************************************/
