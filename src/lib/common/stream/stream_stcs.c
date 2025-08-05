/******************************************************************************/
/* stream_stcs.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Stream Reader / Writer structures                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/streamrd.h"
#include "src/includes/common/streamwr.h"
/** */
#define SRC_STREAM_STCS_C
#include "stream_stcs-prot.h"
#undef SRC_STREAM_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* free() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_streamrd */
Tst_err
st_streamrd_stc_initSObj(Tst_streamrd *pSObj)
{
	ST_ASSERTN_IARG(pSObj == NULL)

	ST_CALLOC(pSObj->pObInternal, Tst_streamrd_intn*,
			1, sizeof(Tst_streamrd_intn))
	if (pSObj->pObInternal == NULL)
		return ST_ERR_OMEM;

	st_stream_stc_initBuf(&((Tst_streamrd_intn*)pSObj->pObInternal)->sbuf);
	/* */
	st_streamrd_stc_rsetSObj(pSObj);
	return ST_ERR_SUCC;
}

void
st_streamrd_stc_rsetSObj(Tst_streamrd *pSObj)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;

	st_stream_stc_rsetBuf(&pSOI->sbuf, ST_B_FALSE);
	st_stream_stc_rsetFil(&pSOI->sfil);
	/* */
	st_stream_stc_rsetCRCs(&pSOI->crcs);
	/* */
	pSOI->totBytesRd    = 0;
	pSOI->totBitsRd_rem = 0;
}

void
st_streamrd_stc_freeSObj(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL)

	if (pSObj->pObInternal != NULL) {
		st_streamrd_stc_rsetSObj(pSObj);
		/* */
		st_stream_stc_freeBuf(
				&((Tst_streamrd_intn*)pSObj->pObInternal)->sbuf);
		/* */
		ST_DELPOINT(pSObj->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_streamwr */
Tst_err
st_streamwr_stc_initSObj(Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(pSObj == NULL)

	ST_CALLOC(pSOI, Tst_streamwr_intn*,
			1, sizeof(Tst_streamwr_intn))
	if (pSOI == NULL)
		return ST_ERR_OMEM;
	pSObj->pObInternal = pSOI;

	pSOI->sfil.pFStc = NULL;
	st_stream_stc_initBuf(&pSOI->sbuf);
	/* */
	st_streamwr_stc_rsetSObj(pSObj);
	return ST_ERR_SUCC;
}

void
st_streamwr_stc_rsetSObj(Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;

	if (pSOI->sfil.pFStc != NULL)
		st_streamwr_flush(pSObj);
	/* */
	st_stream_stc_rsetBuf(&pSOI->sbuf, ST_B_FALSE);
	st_stream_stc_rsetFil(&pSOI->sfil);
	/* */
	st_stream_stc_rsetCRCs(&pSOI->crcs);
	/* */
	pSOI->totBytesWr    = 0;
	pSOI->totBitsWr_rem = 0;
}

void
st_streamwr_stc_freeSObj(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL)

	if (pSObj->pObInternal != NULL) {
		st_streamwr_stc_rsetSObj(pSObj);
		/* */
		st_stream_stc_freeBuf(
				&((Tst_streamwr_intn*)pSObj->pObInternal)->sbuf);
		/* */
		ST_DELPOINT(pSObj->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_stream_buf */
void
st_stream_stc_initBuf(Tst_stream_buf *pSBuf)
{
	ST_ASSERTN_VOID(pSBuf == NULL)

	pSBuf->pBBeg     = NULL;
	pSBuf->wasAllocd = ST_B_FALSE;
	pSBuf->sz        = 0;
	/* */
	pSBuf->pIntnBuf  = NULL;
	pSBuf->intnBufSz = 0;
	/* */
	st_stream_stc_rsetBuf(pSBuf, ST_B_FALSE);
}

void
st_stream_stc_rsetBuf(Tst_stream_buf *pSBuf, const Tst_bool freeBufs)
{
	ST_ASSERTN_VOID(pSBuf == NULL)

	if (pSBuf->wasAllocd) {
		if (freeBufs) {
			ST_DELPOINT(pSBuf->pIntnBuf)
			pSBuf->sz        = 0;
			pSBuf->wasAllocd = ST_B_FALSE;
			/* */
			pSBuf->intnBufSz = 0;
		}
	} else {
		pSBuf->pBBeg     = NULL;
		pSBuf->sz        = 0;
		pSBuf->wasAllocd = ST_B_FALSE;
		/* */
		pSBuf->intnBufSz = 0;
	}
	pSBuf->pBCur        = NULL;
	pSBuf->pBEnd        = NULL;
	pSBuf->leftoverBits = 0;
	/* */
	pSBuf->pBCurRestore = NULL;
}

Tst_err
st_stream_stc_reszIntnBuf(Tst_stream_buf *pSBuf,
		const Tst_uint32 newSz, const Tst_uint32 restor)
{
	Tst_uint32 oldUsed,
	           oldCur;

	ST_ASSERTN_IARG(pSBuf == NULL)

	if (newSz > pSBuf->intnBufSz) {
		oldUsed = (pSBuf->pBEnd != NULL ? pSBuf->pBEnd - pSBuf->pBBeg : 0);
		oldCur  = (pSBuf->pBCur != NULL ? pSBuf->pBCur - pSBuf->pBBeg : 0);
		/* */
		ST_REALLOC(pSBuf->pIntnBuf, Tst_buf*, newSz, 1)
		if (pSBuf->pIntnBuf == NULL)
			return ST_ERR_OMEM;
		memset(&pSBuf->pIntnBuf[pSBuf->intnBufSz], 0,
				newSz - pSBuf->intnBufSz);
		pSBuf->pBEnd     = pSBuf->pIntnBuf + oldUsed;
		pSBuf->pBCur     = pSBuf->pIntnBuf + oldCur;
		pSBuf->intnBufSz = newSz;
		pSBuf->wasAllocd = ST_B_TRUE;
	}

	if (restor > 0)
		memmove(pSBuf->pIntnBuf, pSBuf->pBEnd - restor, restor);

	pSBuf->pBBeg = pSBuf->pIntnBuf;
	pSBuf->sz    = pSBuf->intnBufSz;

	/* these will be set by the calling function:
	pSBuf->pBCur        = NULL;
	pSBuf->pBEnd        = NULL;
	pSBuf->leftoverBits = 0;  */

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* Tst_stream_file */
void
st_stream_stc_rsetFil(Tst_stream_file *pSFil)
{
	ST_ASSERTN_VOID(pSFil == NULL)

	pSFil->fsz   = 0;
	pSFil->cpos  = -1;
	pSFil->pFStc = NULL;
}

/*----------------------------------------------------------------------------*/

/* Tst_stream_crcs */
void
st_stream_stc_rsetCRCs(Tst_stream_crcs *pCRCs)
{
	ST_ASSERTN_VOID(pCRCs == NULL)

	pCRCs->compCRC8  = ST_B_FALSE;
	pCRCs->pausCRC8  = ST_B_FALSE;
	pCRCs->compCRC16 = ST_B_FALSE;
	pCRCs->pausCRC16 = ST_B_FALSE;
	pCRCs->compCRC32 = ST_B_FALSE;
	pCRCs->pausCRC32 = ST_B_FALSE;
	/* */
	pCRCs->crc8  = 0;
	pCRCs->crc16 = 0;
	pCRCs->crc32 = 0;
}

/******************************************************************************/
