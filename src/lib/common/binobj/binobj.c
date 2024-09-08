/******************************************************************************/
/* binobj.c                     [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for Binary Object                                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_file.h"
/** */
#define SRC_BINOBJ_C
#include "binobj-prot.h"
#undef SRC_BINOBJ_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const Tst_uint32 ST_BINOBJ_DEF_THRESHOLD = 1024 * 1024;  /* 1 MB */

/*----------------------------------------------------------------------------*/

static Tst_err
ST_BINOBJ__createAndOpenFile(Tst_binobj_intn *pBOI);

static Tst_err
ST_BINOBJ__copyFromAndToFStc(Tst_sys_fstc *pFStcIn,
                             Tst_binobj_intn *pBOIdst,
                             const Tst_fsize fsz,
                             const Tst_bool createNewFile);

static Tst_err
ST_BINOBJ__appendData_streamrd_pr(Tst_binobj *pBOdst,
                                  Tst_streamrd *pStrrd,
                                  const Tst_uint32 size,
                                  const Tst_bool withCRCs);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set threshold at which to write to a temp. file instead of a buffer
 */
void
st_binobj_setThreshold(Tst_binobj *pBO, const Tst_uint32 thres)
{
	ST_ASSERTN_VOID(pBO == NULL || pBO->pObInternal == NULL)

	((Tst_binobj_intn*)pBO->pObInternal)->threshold = thres;
}

/*
 * Set default amount of additional bytes when reallocating
 */
void
st_binobj_setDefaultReallcSize(Tst_binobj *pBO, Tst_uint32 size)
{
	ST_ASSERTN_VOID(pBO == NULL || pBO->pObInternal == NULL)

	if (size < 1024)
		size = 1024;
	((Tst_binobj_intn*)pBO->pObInternal)->reallcSz = size;
}

/*----------------------------------------------------------------------------*/

/*
 * Get threshold
 */
Tst_uint32
st_binobj_getThreshold(const Tst_binobj *pBO)
{
	ST_ASSERTN_NUM(0, pBO == NULL || pBO->pObInternal == NULL)

	return ((Tst_binobj_intn*)pBO->pObInternal)->threshold;
}

/*
 * Get default amount of additional bytes when reallocating
 */
Tst_uint32
st_binobj_getDefaultReallcSize(const Tst_binobj *pBO)
{
	ST_ASSERTN_NUM(0, pBO == NULL || pBO->pObInternal == NULL)

	return ((Tst_binobj_intn*)pBO->pObInternal)->reallcSz;
}

/*
 * Get size of data in binary object
 */
Tst_uint32
st_binobj_getDataSize(const Tst_binobj *pBO)
{
	ST_ASSERTN_NUM(0, pBO == NULL || pBO->pObInternal == NULL)

	return ((Tst_binobj_intn*)pBO->pObInternal)->dataSz;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set binary object's data by copying the content of pBin
 */
Tst_err
st_binobj_setData(Tst_binobj *pBOdst,
		const Tst_buf *pBin, const Tst_uint32 binSz)
{
	Tst_err res;
	Tst_binobj_intn *pBOIdst;

	ST_ASSERTN_IARG(pBOdst == NULL || pBOdst->pObInternal == NULL ||
			pBin == NULL)

	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	st_binobj_stc_rsetData(pBOdst);
	pBOIdst->pSObjR = NULL;
	/* */
	if (binSz < pBOIdst->threshold) {
		if (binSz > pBOIdst->dataBufSz) {
			ST_REALLOC(pBOIdst->pDataBuf, Tst_buf*, binSz, 1)
			if (pBOIdst->pDataBuf == NULL)
				return ST_ERR_OMEM;
			pBOIdst->dataBufSz = binSz;
		}
		memcpy(pBOIdst->pDataBuf, pBin, binSz);
		pBOIdst->isBufOrFile = ST_B_TRUE;
	} else {
		/* free buffer */
		pBOIdst->dataBufSz = 0;
		ST_DELPOINT(pBOIdst->pDataBuf)

		/* create and open temp. file */
		res = ST_BINOBJ__createAndOpenFile(pBOIdst);
		if (res != ST_ERR_SUCC)
			return res;

		/* write data to file */
		res = st_sysFStc_writeBuf(pBOIdst->pDataFStc, pBin, binSz);
		if (res != ST_ERR_SUCC)
			return res;

		pBOIdst->isBufOrFile = ST_B_FALSE;
	}
	pBOIdst->dataSz = binSz;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Set binary object's data by copying the file pFilen
 */
Tst_err
st_binobj_setData_file(Tst_binobj *pBOdst, const Tst_str *pInpFilen,
		const Tst_bool allwLinkAsInput)
{
	Tst_err      res;
	Tst_fsize    fsz;
	Tst_bool     isTooBig = ST_B_FALSE;
	Tst_sys_fstc fstcIn;
	Tst_uint32   cpdUI    = 0;
	Tst_binobj_intn *pBOIdst;

	ST_ASSERTN_IARG(pBOdst == NULL || pBOdst->pObInternal == NULL ||
			pInpFilen == NULL)

	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	st_binobj_stc_rsetData(pBOdst);
	pBOIdst->pSObjR = NULL;
	/* */
	fsz = st_sysGetFileSz_byFn(pInpFilen, &isTooBig);
	if (isTooBig)
		return ST_ERR_FAIL;

	/* open file */
	st_sys_stc_initFStc(&fstcIn);
	res = st_sysFStc_setFilen(&fstcIn, pInpFilen);
	if (res != ST_ERR_SUCC) {
		st_sys_stc_freeFStc(&fstcIn);
		return res;
	}
	res = st_sysFStc_openExisting(&fstcIn, allwLinkAsInput, ST_B_FALSE);
	if (res != ST_ERR_SUCC) {
		st_sys_stc_freeFStc(&fstcIn);
		return res;
	}

	if (fsz < (Tst_fsize)pBOIdst->threshold) {
		if ((Tst_uint32)fsz > pBOIdst->dataBufSz) {
			ST_REALLOC(pBOIdst->pDataBuf, Tst_buf*, (Tst_uint32)fsz, 1)
			if (pBOIdst->pDataBuf == NULL) {
				st_sys_stc_freeFStc(&fstcIn);
				return ST_ERR_OMEM;
			}
			pBOIdst->dataBufSz = (Tst_uint32)fsz;
		}
		res = st_sysFStc_readBuf(&fstcIn,
				(Tst_uint32)fsz, pBOIdst->pDataBuf, &cpdUI);
		if (res == ST_ERR_SUCC) {
			pBOIdst->dataSz      = cpdUI;
			pBOIdst->isBufOrFile = ST_B_TRUE;
		}
	} else {
		res = ST_BINOBJ__copyFromAndToFStc(&fstcIn, pBOIdst, fsz, ST_B_TRUE);
	}

	st_sys_stc_freeFStc(&fstcIn);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Set binary object's data by copying size-bytes from pStrrd
 */
Tst_err
st_binobj_setData_streamrd(Tst_binobj *pBOdst,
		Tst_streamrd *pStrrd, const Tst_uint32 size,
		const Tst_bool withCRCs)
{
	Tst_binobj_intn *pBOIdst;

	ST_ASSERTN_IARG(pBOdst == NULL || pBOdst->pObInternal == NULL ||
			pStrrd == NULL)

	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	st_binobj_stc_rsetData(pBOdst);
	pBOIdst->pSObjR = NULL;

	return ST_BINOBJ__appendData_streamrd_pr(pBOdst,
			pStrrd, size, withCRCs);
}

/*----------------------------------------------------------------------------*/

/*
 * Copy pBOsrc's content to pBOdst
 */
Tst_err
st_binobj_copy(Tst_binobj *pBOsrc, Tst_binobj *pBOdst)
{
	Tst_err   res;
	Tst_foffs offsSrc;
	Tst_binobj_intn *pBOIsrc,
	                *pBOIdst;

	ST_ASSERTN_IARG(pBOsrc == NULL || pBOsrc->pObInternal == NULL ||
			pBOdst == NULL || pBOdst->pObInternal == NULL)

	pBOIsrc = (Tst_binobj_intn*)pBOsrc->pObInternal;
	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	pBOIdst->threshold = pBOIsrc->threshold;
	pBOIdst->reallcSz  = pBOIsrc->reallcSz;

	if (pBOIsrc->isBufOrFile) {
		if (pBOIsrc->dataSz == 0) {
			st_binobj_stc_rsetData(pBOdst);
			return ST_ERR_SUCC;
		}
		/* copy from/to buffer */
		return st_binobj_setData(pBOdst,
				pBOIsrc->pDataBuf, pBOIsrc->dataSz);
	}

	/* copy from/to file */
	if (pBOIsrc->pDataFStc == NULL)
		return ST_ERR_FAIL;
	st_sysFStc_flush(pBOIsrc->pDataFStc);
	offsSrc = st_sysFStc_getFPos(pBOIsrc->pDataFStc);
	st_sysFStc_setFPos(pBOIsrc->pDataFStc, 0, ST_SYS_SETFPOS_BEG);

	st_binobj_stc_rsetData(pBOdst);

	res = ST_BINOBJ__copyFromAndToFStc(pBOIsrc->pDataFStc, pBOIdst,
			(Tst_fsize)pBOIsrc->dataSz, ST_B_TRUE);

	if (offsSrc != st_sysFStc_getFPos(pBOIsrc->pDataFStc))
		st_sysFStc_setFPos(pBOIsrc->pDataFStc, offsSrc, ST_SYS_SETFPOS_BEG);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy pBOsrc's content to *ppBuffer
 */
Tst_err
st_binobj_copyIntoBuffer(Tst_binobj *pBOsrc,
		Tst_buf **ppBuffer, Tst_uint32 *pBufSz)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_foffs  offsSrc;
	Tst_uint32 toCp,
	           cpTot = 0,
	           cpd;
	Tst_binobj_intn *pBOIsrc;

	ST_ASSERTN_IARG(pBOsrc == NULL || pBOsrc->pObInternal == NULL ||
			ppBuffer == NULL || pBufSz == NULL)

	pBOIsrc = (Tst_binobj_intn*)pBOsrc->pObInternal;
	/* */
	if (pBOIsrc->dataSz != *pBufSz || *ppBuffer == NULL) {
		ST_REALLOC(*ppBuffer, Tst_buf*,
				(pBOIsrc->dataSz > 0 ? pBOIsrc->dataSz : 1), 1)
		if (*ppBuffer == NULL)
			return ST_ERR_OMEM;
		*pBufSz = pBOIsrc->dataSz;
	}
	/* */
	if (pBOIsrc->isBufOrFile) {
		if (pBOIsrc->dataSz > 0)
			memcpy(*ppBuffer, pBOIsrc->pDataBuf, pBOIsrc->dataSz);
	} else {
		st_sysFStc_flush(pBOIsrc->pDataFStc);
		offsSrc = st_sysFStc_getFPos(pBOIsrc->pDataFStc);
		st_sysFStc_setFPos(pBOIsrc->pDataFStc, 0, ST_SYS_SETFPOS_BEG);
		/* */
		toCp = 4096;
		while (! st_sysFStc_isEOF(pBOIsrc->pDataFStc) && cpTot < pBOIsrc->dataSz) {
			if (cpTot + toCp > pBOIsrc->dataSz)
				toCp = pBOIsrc->dataSz - cpTot;
			res = st_sysFStc_readBuf(pBOIsrc->pDataFStc, toCp,
					&(*ppBuffer)[cpTot], &cpd);
			if (res != ST_ERR_SUCC)
				break;
			cpTot += cpd;
			if (cpd != toCp)
				break;
		}
		/* */
		if (offsSrc != st_sysFStc_getFPos(pBOIsrc->pDataFStc))
			st_sysFStc_setFPos(pBOIsrc->pDataFStc, offsSrc, ST_SYS_SETFPOS_BEG);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy pBOsrc's content to pOutpFilen
 */
Tst_err
st_binobj_copyIntoFile(Tst_binobj *pBOsrc, const Tst_str *pOutpFilen)
{
	Tst_err      res = ST_ERR_SUCC;
	Tst_sys_fstc fstcOut;
	Tst_binobj_intn *pBOIsrc;

	ST_ASSERTN_IARG(pBOsrc == NULL || pBOsrc->pObInternal == NULL ||
			pOutpFilen == NULL)

	pBOIsrc = (Tst_binobj_intn*)pBOsrc->pObInternal;
	/* */
	st_sys_stc_initFStc(&fstcOut);
	res = st_sysFStc_setFilen(&fstcOut, pOutpFilen);
	if (res != ST_ERR_SUCC) {
		st_sys_stc_freeFStc(&fstcOut);
		return res;
	}
	if (res == ST_ERR_SUCC)
		res = st_sysFStc_openNew(&fstcOut);
	/* */
	if (res == ST_ERR_SUCC && pBOIsrc->isBufOrFile) {
		Tst_buf    buf[4096];
		Tst_uint32 toWr  = sizeof(buf),
		           wrTot = 0;

		while (wrTot < pBOIsrc->dataSz) {
			if (wrTot + toWr > pBOIsrc->dataSz)
				toWr = pBOIsrc->dataSz - wrTot;
			res = st_sysFStc_writeBuf(&fstcOut,
					&pBOIsrc->pDataBuf[wrTot], toWr);
			if (res != ST_ERR_SUCC)
				break;
			wrTot += toWr;
		}
	} else if (res == ST_ERR_SUCC) {
		Tst_fsize fsz   = (Tst_fsize)pBOIsrc->dataSz,
		          cpdFS = 0;
		Tst_foffs offsSrc;

		st_sysFStc_flush(pBOIsrc->pDataFStc);
		offsSrc = st_sysFStc_getFPos(pBOIsrc->pDataFStc);
		st_sysFStc_setFPos(pBOIsrc->pDataFStc, 0, ST_SYS_SETFPOS_BEG);
		/* */
		res = st_sysFStc_copyFromAndToFile(ST_B_FALSE,
				pBOIsrc->pDataFStc, &fstcOut, fsz, &cpdFS);
		if (res != ST_ERR_SUCC || fsz != cpdFS) {
			if (res == ST_ERR_SUCC)
				res = ST_ERR_FCRD;
		}
		/* */
		if (offsSrc != st_sysFStc_getFPos(pBOIsrc->pDataFStc))
			st_sysFStc_setFPos(pBOIsrc->pDataFStc, offsSrc, ST_SYS_SETFPOS_BEG);
	}

	st_sys_stc_freeFStc(&fstcOut);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Append the data from pBin to the content of pBOdst
 */
Tst_err
st_binobj_appendData(Tst_binobj *pBOdst,
		const Tst_buf *pBin, const Tst_uint32 binSz)
{
	Tst_err res;
	Tst_binobj_intn *pBOIdst;

	ST_ASSERTN_IARG(pBOdst == NULL || pBOdst->pObInternal == NULL ||
			pBin == NULL)

	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	pBOIdst->pSObjR = NULL;
	/* */
	if (pBOIdst->isBufOrFile) {
		if (pBOIdst->dataSz + binSz < pBOIdst->threshold) {
			/* append pBin to pDataBuf */
			if (pBOIdst->dataSz + binSz > pBOIdst->dataBufSz) {
				Tst_uint32 tmpNewBSz = pBOIdst->dataSz + binSz;

				tmpNewBSz += (pBOIdst->reallcSz - (tmpNewBSz % pBOIdst->reallcSz));
				ST_REALLOC(pBOIdst->pDataBuf, Tst_buf*, tmpNewBSz, 1)
				if (pBOIdst->pDataBuf == NULL)
					return ST_ERR_OMEM;
				pBOIdst->dataBufSz = tmpNewBSz;
			}
			memcpy(&pBOIdst->pDataBuf[pBOIdst->dataSz], pBin, binSz);
			pBOIdst->dataSz += binSz;
			return ST_ERR_SUCC;
		}

		/* create and open temp. file */
		res = ST_BINOBJ__createAndOpenFile(pBOIdst);
		if (res != ST_ERR_SUCC)
			return res;

		/* write pDataBuf to new file */
		res = st_sysFStc_writeBuf(pBOIdst->pDataFStc,
				pBOIdst->pDataBuf, pBOIdst->dataSz);
		if (res != ST_ERR_SUCC)
			return res;

		/* free pDataBuf */
		pBOIdst->dataBufSz = 0;
		ST_DELPOINT(pBOIdst->pDataBuf)
	}

	/* append pBin to file */
	pBOIdst->isBufOrFile = ST_B_FALSE;
	pBOIdst->dataSz     += binSz;
	st_sysFStc_setFPos(pBOIdst->pDataFStc, 0, ST_SYS_SETFPOS_END);
	return st_sysFStc_writeBuf(pBOIdst->pDataFStc, pBin, binSz);
}

/*----------------------------------------------------------------------------*/

/*
 * Append the data from pBOsrc to the content of pBOdst
 */
Tst_err
st_binobj_appendData_binobj(Tst_binobj *pBOdst, Tst_binobj *pBOsrc)
{
	Tst_err   res;
	Tst_foffs offsSrc;
	Tst_binobj_intn *pBOIsrc,
	                *pBOIdst;

	ST_ASSERTN_IARG(pBOsrc == NULL || pBOsrc->pObInternal == NULL ||
			pBOdst == NULL || pBOdst->pObInternal == NULL)

	pBOIsrc = (Tst_binobj_intn*)pBOsrc->pObInternal;
	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	pBOIdst->pSObjR = NULL;
	/* */
	if (pBOIsrc->isBufOrFile) {
		/* append data from buffer to buffer/file */
		if (pBOIsrc->dataSz > 0)
			return st_binobj_appendData(pBOdst,
					pBOIsrc->pDataBuf, pBOIsrc->dataSz);
		return ST_ERR_SUCC;
	}

	/* append data from file to buffer/file */
	st_sysFStc_flush(pBOIsrc->pDataFStc);
	offsSrc = st_sysFStc_getFPos(pBOIsrc->pDataFStc);
	st_sysFStc_setFPos(pBOIsrc->pDataFStc, 0, ST_SYS_SETFPOS_BEG);
	/* */
	if (pBOIdst->isBufOrFile) {
		/* append data from file to buffer */
		Tst_buf    buf[4096];
		Tst_uint32 toCp  = sizeof(buf),
		           cpTot = 0,
		           cpd;

		while (! st_sysFStc_isEOF(pBOIsrc->pDataFStc) &&
				cpTot < pBOIsrc->dataSz) {
			if (cpTot + toCp > pBOIsrc->dataSz)
				toCp = pBOIsrc->dataSz - cpTot;
			res = st_sysFStc_readBuf(pBOIsrc->pDataFStc, toCp,
					buf, &cpd);
			if (res == ST_ERR_SUCC)
				res = st_binobj_appendData(pBOdst, buf, cpd);
			if (res != ST_ERR_SUCC)
				break;
			cpTot += cpd;
			if (cpd != toCp)
				break;
		}
	} else {
		/* append data from file to file */
		res = ST_BINOBJ__copyFromAndToFStc(pBOIsrc->pDataFStc, pBOIdst,
				(Tst_fsize)pBOIsrc->dataSz, ST_B_FALSE);
	}
	/* */
	if (offsSrc != st_sysFStc_getFPos(pBOIsrc->pDataFStc))
		st_sysFStc_setFPos(pBOIsrc->pDataFStc, offsSrc, ST_SYS_SETFPOS_BEG);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Append size-bytes from pStrrd to the content of pBOdst
 */
Tst_err
st_binobj_appendData_streamrd(Tst_binobj *pBOdst, Tst_streamrd *pStrrd,
		const Tst_uint32 size, const Tst_bool withCRCs)
{
	Tst_binobj_intn *pBOIdst;

	ST_ASSERTN_IARG(pBOdst == NULL || pBOdst->pObInternal == NULL ||
			pStrrd == NULL)

	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	pBOIdst->pSObjR = NULL;

	return ST_BINOBJ__appendData_streamrd_pr(pBOdst,
			pStrrd, size, withCRCs);
}

/*----------------------------------------------------------------------------*/

/*
 * Replace data in binary object
 */
Tst_err
st_binobj_replaceData(Tst_binobj *pBOdst, Tst_foffs startPos,
		const Tst_buf *pBin, const Tst_uint32 binSz)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 toCp,
	           cpd;
	Tst_binobj_intn *pBOIdst;

	ST_ASSERTN_IARG(pBOdst == NULL || pBOdst->pObInternal == NULL ||
			pBin == NULL)

	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;
	/* */
	pBOIdst->pSObjR = NULL;

	if (pBOIdst->dataSz == 0)
		return ST_ERR_FAIL;
	if (startPos < 0)
		startPos = 0;
	else if (startPos > (Tst_foffs)pBOIdst->dataSz)
		startPos = (Tst_foffs)pBOIdst->dataSz;

	toCp = binSz;
	if ((Tst_uint32)startPos + toCp > pBOIdst->dataSz)
		toCp = pBOIdst->dataSz - (Tst_uint32)startPos;

	if (toCp > 0) {
		if (pBOIdst->isBufOrFile)
			memcpy(&pBOIdst->pDataBuf[(Tst_uint32)startPos], pBin, toCp);
		else {
			st_sysFStc_setFPos(pBOIdst->pDataFStc, startPos, ST_SYS_SETFPOS_BEG);
			res = st_sysFStc_writeBuf(pBOIdst->pDataFStc, pBin, toCp);
		}
	}

	cpd  = toCp;
	toCp = binSz - cpd;
	if (res == ST_ERR_SUCC && toCp > 0)
		res = st_binobj_appendData(pBOdst, &pBin[cpd], toCp);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Attach a stream reader to the binary object for being able to
 *   read its content
 */
Tst_err
st_binobj_attachStreamrd(Tst_binobj *pBO, Tst_streamrd *pSObjR,
		Tst_foffs startPos)
{
	Tst_binobj_intn *pBOI;

	ST_ASSERTN_IARG(pBO == NULL || pBO->pObInternal == NULL)

	pBOI = (Tst_binobj_intn*)pBO->pObInternal;
	/* */
	if (pBOI->dataSz == 0 || pBOI->pSObjR != NULL)
		return ST_ERR_FAIL;
	if (startPos < 0)
		startPos = 0;
	else if (startPos > (Tst_foffs)pBOI->dataSz)
		startPos = (Tst_foffs)pBOI->dataSz;
	/* */
	pBOI->pSObjR = pSObjR;
	if (pBOI->isBufOrFile)
		return st_streamrd_setInput_buffer(pSObjR,
				(startPos < (Tst_foffs)pBOI->dataSz ?
					&pBOI->pDataBuf[(Tst_uint32)startPos] :
					pBOI->pDataBuf),
				pBOI->dataSz - (Tst_uint32)startPos);

	st_sysFStc_flush(pBOI->pDataFStc);
	if (startPos < (Tst_foffs)pBOI->dataSz)
		st_sysFStc_setFPos(pBOI->pDataFStc, startPos, ST_SYS_SETFPOS_BEG);
	else
		st_sysFStc_setFPos(pBOI->pDataFStc, 0, ST_SYS_SETFPOS_END);
	return st_streamrd_setInput_file(pSObjR, pBOI->pDataFStc);
}

/*
 * Detach the previously attached stream reader
 */
void
st_binobj_detachStreamrd(Tst_binobj *pBO)
{
	ST_ASSERTN_VOID(pBO == NULL || pBO->pObInternal == NULL)

	((Tst_binobj_intn*)pBO->pObInternal)->pSObjR = NULL;
}

/*----------------------------------------------------------------------------*/

/*
 * Are pBO1's and pBO2's contents equal ?
 *   Neither to pBO1 nor to pBO2 must a stream reader have been attached
 */
Tst_bool
st_binobj_areEqual(Tst_binobj *pBO1, Tst_binobj *pBO2)
{
	Tst_err    res;
	Tst_bool   resB       = ST_B_TRUE;
	Tst_buf    buf1[1024],
	           buf2[1024];
	Tst_uint32 rdFromStr1 = 0,
	           rdFromStr2 = 0;
	Tst_binobj_intn *pBOI1,
	                *pBOI2;
	Tst_streamrd    strrd1,
	                strrd2;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBO1 == NULL || pBO1->pObInternal == NULL ||
			pBO2 == NULL || pBO2->pObInternal == NULL)

	pBOI1 = (Tst_binobj_intn*)pBO1->pObInternal;
	pBOI2 = (Tst_binobj_intn*)pBO2->pObInternal;
	/* can't compare if a stream reader is already attached */
	if (pBOI1->pSObjR != NULL || pBOI2->pSObjR != NULL)
		return ST_B_FALSE;
	/* */
	if (pBOI1->dataSz != pBOI2->dataSz)
		return ST_B_FALSE;
	/* */
	st_streamrd_stc_initSObj(&strrd1);
	st_streamrd_stc_initSObj(&strrd2);
	res = st_binobj_attachStreamrd(pBO1, &strrd1, 0);
	if (res == ST_ERR_SUCC)
		res = st_binobj_attachStreamrd(pBO2, &strrd2, 0);
	if (res != ST_ERR_SUCC) {
		st_binobj_detachStreamrd(pBO1);
		st_binobj_detachStreamrd(pBO2);
		return ST_B_FALSE;
	}
	/* */
	if (st_streamrd_getAmountRemainingBytes(&strrd1) !=
			st_streamrd_getAmountRemainingBytes(&strrd2))
		resB = ST_B_FALSE;
	while (resB && ! st_streamrd_isEOF(&strrd1)) {
		st_streamrd_rdBuffer_noCRC(&strrd1, sizeof(buf1), buf1, &rdFromStr1);
		st_streamrd_rdBuffer_noCRC(&strrd2, sizeof(buf2), buf2, &rdFromStr2);
		if (rdFromStr1 != rdFromStr2) {
			resB = ST_B_FALSE;
			continue;
		}
		if (rdFromStr1 == 0)
			break;
		resB = (memcmp(buf1, buf2, rdFromStr1) == 0);
	}
	/* */
	st_binobj_detachStreamrd(pBO1);
	st_binobj_detachStreamrd(pBO2);
	st_streamrd_stc_freeSObj(&strrd1);
	st_streamrd_stc_freeSObj(&strrd2);
	return resB;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_BINOBJ__createAndOpenFile(Tst_binobj_intn *pBOI)
{
	Tst_err res;

	/* create filename */
	ST_REALLOC(pBOI->pDataTmpFn, Tst_str*, 1024, 1)
	if (pBOI->pDataTmpFn == NULL)
		return ST_ERR_OMEM;
	if (! st_sysGetTmpFilename(pBOI->pDataTmpFn, 1024, ST_B_FALSE))
		return ST_ERR_FAIL;

	/* create and open file */
	ST_REALLOC(pBOI->pDataFStc, Tst_sys_fstc*, 1, sizeof(Tst_sys_fstc))
	if (pBOI->pDataFStc == NULL)
		return ST_ERR_OMEM;
	st_sys_stc_initFStc(pBOI->pDataFStc);
	res = st_sysFStc_setFilen(pBOI->pDataFStc, pBOI->pDataTmpFn);
	if (res != ST_ERR_SUCC)
		return res;
	return st_sysFStc_openNew(pBOI->pDataFStc);
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_BINOBJ__copyFromAndToFStc(Tst_sys_fstc *pFStcIn,
		Tst_binobj_intn *pBOIdst, const Tst_fsize fsz,
		const Tst_bool createNewFile)
{
	Tst_err   res;
	Tst_fsize cpdFS = 0;

	if (createNewFile) {
		/* free buffer */
		pBOIdst->dataBufSz = 0;
		ST_DELPOINT(pBOIdst->pDataBuf)

		/* create and open temp. file */
		res = ST_BINOBJ__createAndOpenFile(pBOIdst);
		if (res != ST_ERR_SUCC)
			return res;
	} else if (pBOIdst->dataSz > 0)
		st_sysFStc_setFPos(pBOIdst->pDataFStc, 0, ST_SYS_SETFPOS_END);

	/* copy file */
	res = st_sysFStc_copyFromAndToFile(ST_B_FALSE,
			pFStcIn, pBOIdst->pDataFStc, fsz, &cpdFS);
	if (res != ST_ERR_SUCC || fsz != cpdFS) {
		if (res == ST_ERR_SUCC)
			res = ST_ERR_FCRD;
		return res;
	}

	pBOIdst->dataSz     += (Tst_uint32)cpdFS;
	pBOIdst->isBufOrFile = ST_B_FALSE;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy size-bytes from pStrrd to pBOdst
 */
static Tst_err
ST_BINOBJ__appendData_streamrd_pr(Tst_binobj *pBOdst,
		Tst_streamrd *pStrrd, const Tst_uint32 size,
		const Tst_bool withCRCs)
{
	Tst_err    res       = ST_ERR_SUCC;
	Tst_buf    buf[4096];
	Tst_uint32 toRd,
	           rdTot     = 0,
	           rdFromStr = 0;
	Tst_binobj_intn *pBOIdst;

	ST_ASSERTN_IARG(pBOdst == NULL || pBOdst->pObInternal == NULL ||
			pStrrd == NULL)

	pBOIdst = (Tst_binobj_intn*)pBOdst->pObInternal;

	/* read data from pStrrd and append it to pBOdst */
	toRd = sizeof(buf);
	while (rdTot < size) {
		if (rdTot + toRd > size)
			toRd = size - rdTot;
		if (withCRCs)
			res = st_streamrd_rdBuffer(pStrrd,
					toRd, buf, &rdFromStr);
		else
			res = st_streamrd_rdBuffer_noCRC(pStrrd,
					toRd, buf, &rdFromStr);
		if (res != ST_ERR_SUCC && res != ST_ERR_FEOF)
			break;
		/* */
		res = st_binobj_appendData(pBOdst, buf, rdFromStr);
		if (res != ST_ERR_SUCC)
			break;
		rdTot += rdFromStr;
		if (rdFromStr != toRd)
			break;
	}

	return res;
}

/******************************************************************************/
