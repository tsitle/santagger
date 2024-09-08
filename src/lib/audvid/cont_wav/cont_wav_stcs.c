/******************************************************************************/
/* cont_wav_stcs.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for WAV container structs                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.07.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/audvid/cont_wav.h"
/** */
#define SRC_CONT_WAV_STCS_C
#include "cont_wav_stcs-prot.h"
#undef SRC_CONT_WAV_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), free(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void ST_CONTWAV__stc_cbRsetElem(void *pElem);
static void ST_CONTWAV__stc_cbFreeElem(void *pElem);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contWav_obj */
Tst_err
st_contWav_stc_initWObj(Tst_contWav_obj *pWObj)
{
	Tst_err res;
	Tst_contWav_obj_intn *pWObjI;

	ST_ASSERTN_IARG(pWObj == NULL)

	ST_CALLOC(pWObjI, Tst_contWav_obj_intn*,
			1, sizeof(Tst_contWav_obj_intn))
	if (pWObjI == NULL)
		return ST_ERR_OMEM;
	pWObj->pObInternal = pWObjI;

	res = st_dynlist_stc_initDL(&pWObjI->list, sizeof(Tst_contWav_handle),
			ST_CONTWAV__stc_cbRsetElem, ST_CONTWAV__stc_cbFreeElem);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	st_contWav_stc_initOpts(&pWObjI->opts);

	st_contWav_stc_rsetWObj(pWObj);
	return ST_ERR_SUCC;
}

void
st_contWav_stc_rsetWObj(Tst_contWav_obj *pWObj)
{
	Tst_contWav_obj_intn *pWObjI;

	ST_ASSERTN_VOID(pWObj == NULL || pWObj->pObInternal == NULL)

	pWObjI = (Tst_contWav_obj_intn*)pWObj->pObInternal;

	st_dynlist_stc_rsetDL(&pWObjI->list);
	pWObjI->pCacheElem = NULL;
	pWObjI->cacheBsIx  = 0;
	pWObjI->cacheBsSIx = 0;
	/* */
	st_contWav_stc_rsetOpts(&pWObjI->opts);
	/* */
	pWObjI->uniqueIxLast = 1;
}

void
st_contWav_stc_freeWObj(Tst_contWav_obj *pWObj)
{
	Tst_contWav_obj_intn *pWObjI;

	ST_ASSERTN_VOID(pWObj == NULL)

	if (pWObj->pObInternal != NULL) {
		st_contWav_stc_rsetWObj(pWObj);
		/* */
		pWObjI = (Tst_contWav_obj_intn*)pWObj->pObInternal;
		st_dynlist_stc_freeDL(&pWObjI->list);
		/* */
		st_contWav_stc_freeOpts(&pWObjI->opts);
		/* */
		ST_DELPOINT(pWObj->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_contWav_handle */
Tst_err
st_contWav_stc_initElem(Tst_contWav_handle *pElem, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_contWav_handle_intn *pEI;

	ST_ASSERTN_IARG(pElem == NULL || pFilen == NULL)

	ST_CALLOC(pEI, Tst_contWav_handle_intn*,
			1, sizeof(Tst_contWav_handle_intn))
	if (pEI == NULL)
		return ST_ERR_OMEM;
	pElem->pObInternal = pEI;

	pEI->pFilen = NULL;
	res         = st_sysStrcpy(pFilen, &pEI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	st_sys_stc_initFStc(&pEI->fstcOut);
	st_contWav_stc_initDBuf(&pEI->dbuf);

	st_contWav_stc_rsetElem(pElem);
	return ST_ERR_SUCC;
}

void
st_contWav_stc_rsetElem(Tst_contWav_handle *pElem)
{
	Tst_contWav_handle_intn *pEI;

	ST_ASSERTN_VOID(pElem == NULL || pElem->pObInternal == NULL)

	pEI = (Tst_contWav_handle_intn*)pElem->pObInternal;

	st_sys_stc_rsetFStc(&pEI->fstcOut, ST_B_TRUE);
	/* */
	pEI->bsIx  = 0;
	pEI->bsSIx = 0;
	/* header info */
	/** */
	pEI->hdInfo.hdWritten = ST_B_FALSE;
	/** */
	ST_SYSMATH_STC_RSETUI64(pEI->hdInfo.totSampleCnt)
	pEI->hdInfo.sampleRate       = 0;
	pEI->hdInfo.bitsPerSample    = 0;
	pEI->hdInfo.channMask        = 0;
	pEI->hdInfo.channels         = 0;
	pEI->hdInfo.isExtensibleWFmt = ST_B_FALSE;
	/* */
	st_contWav_stc_rsetDBuf(&pEI->dbuf);
	/* */
	pEI->uniqueIx = 0;
	/* */
	pEI->pOpts = NULL;
}

void
st_contWav_stc_freeElem(Tst_contWav_handle *pElem)
{
	Tst_contWav_handle_intn *pEI;

	ST_ASSERTN_VOID(pElem == NULL)

	if (pElem->pObInternal != NULL) {
		st_contWav_stc_rsetElem(pElem);
		/* */
		pEI = (Tst_contWav_handle_intn*)pElem->pObInternal;

		ST_DELPOINT(pEI->pFilen)
		st_sys_stc_freeFStc(&pEI->fstcOut);
		st_contWav_stc_freeDBuf(&pEI->dbuf);
		ST_DELPOINT(pElem->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contWav_opts */
void
st_contWav_stc_initOpts(Tst_contWav_opts *pOpts)
{

	pOpts->pOutpdir = NULL;

	st_contWav_stc_rsetOpts(pOpts);
}

void
st_contWav_stc_rsetOpts(Tst_contWav_opts *pOpts)
{
	ST_ASSERTN_VOID(pOpts == NULL)

	ST_RSETSTC_BASOPTS(pOpts->basOpts)
	/* */
#	if (WORDS_BIGENDIAN != 1)
	pOpts->isSystemBE = ST_B_FALSE;
#	else
	pOpts->isSystemBE = ST_B_TRUE;
#	endif
	pOpts->wrFmtRIFForAIFF = ST_B_TRUE;
	/* */
	ST_DELPOINT(pOpts->pOutpdir)
	/* */
	pOpts->cbNewOutpStr = NULL;
}

/*----------------------------------------------------------------------------*/

/* Tst_contWav_destBuffer */
void
st_contWav_stc_initDBuf(Tst_contWav_destBuffer *pDBuf)
{
	ST_ASSERTN_VOID(pDBuf == NULL)

	pDBuf->pDstBuf    = NULL;
	pDBuf->ppSrcBufSh = NULL;
	/* */
	st_contWav_stc_rsetDBuf(pDBuf);
}

void
st_contWav_stc_rsetDBuf(Tst_contWav_destBuffer *pDBuf)
{
	ST_ASSERTN_VOID(pDBuf == NULL)

	ST_DELPOINT(pDBuf->pDstBuf)
	ST_DELPOINT(pDBuf->ppSrcBufSh)
	pDBuf->dstBufSz   = 0;
	pDBuf->srcBufShSz = 0;
	/* */
	pDBuf->pU8buf   = NULL;
	pDBuf->pS16buf  = NULL;
	pDBuf->pU16buf  = NULL;
	pDBuf->pS32buf  = NULL;
	pDBuf->pU32buf  = NULL;
	/* */
	pDBuf->bytesToWrite = 0;
}

Tst_err
st_contWav_stc_reszDBuf_dstBuf(Tst_contWav_destBuffer *pDBuf,
		const Tst_uint32 newSz)
{
	ST_ASSERTN_IARG(pDBuf == NULL)

	if (newSz > pDBuf->dstBufSz) {
		ST_REALLOC(pDBuf->pDstBuf, Tst_int8*, newSz,
				sizeof(Tst_int32) /* !!! this is with intention */)
		if (pDBuf->pDstBuf == NULL)
			return ST_ERR_OMEM;
		pDBuf->dstBufSz = newSz;
	}
	pDBuf->pU8buf  = (Tst_uint8*)pDBuf->pDstBuf;
	pDBuf->pS16buf = (Tst_int16*)pDBuf->pDstBuf;
	pDBuf->pU16buf = (Tst_uint16*)pDBuf->pDstBuf;
	pDBuf->pS32buf = (Tst_int32*)pDBuf->pDstBuf;
	pDBuf->pU32buf = (Tst_uint32*)pDBuf->pDstBuf;
	return ST_ERR_SUCC;
}

Tst_err
st_contWav_stc_reszDBuf_srcBufSh(Tst_contWav_destBuffer *pDBuf,
		const Tst_uint32 newSz)
{
	ST_ASSERTN_IARG(pDBuf == NULL)

	if (newSz > pDBuf->srcBufShSz) {
		ST_REALLOC(pDBuf->ppSrcBufSh, Tst_int32**, newSz, sizeof(Tst_int32))
		if (pDBuf->ppSrcBufSh == NULL)
			return ST_ERR_OMEM;
		pDBuf->srcBufShSz = newSz;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_CONTWAV__stc_cbRsetElem(void *pElem)
{
	st_contWav_stc_rsetElem((Tst_contWav_handle*)pElem);
}

static void
ST_CONTWAV__stc_cbFreeElem(void *pElem)
{
	st_contWav_stc_freeElem((Tst_contWav_handle*)pElem);
}

/******************************************************************************/
