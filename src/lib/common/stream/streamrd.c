/******************************************************************************/
/* streamrd.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for the Stream Reader                                            */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/common/streamrd.h"
/** */
#define SRC_STREAMRD_C
#include "streamrd-priv.h"
/*** */
#include "stream_crcs-prot.h"
#include "stream_stcs-prot.h"
#undef SRC_STREAMRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */
#if (ST_STREAMRD_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set input source to pBuffer
 *   The contents of this buffer won't be changed
 */
Tst_err
st_streamrd_setInput_buffer(Tst_streamrd *pSObj,
		const Tst_buf *pBuffer, const Tst_uint32 bufSize)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBuffer == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;

	st_streamrd_stc_rsetSObj(pSObj);
	st_stream_stc_rsetBuf(&pSOI->sbuf, ST_B_TRUE);
	/* */
	pSOI->sbuf.pBBeg = (Tst_buf*)pBuffer;
	pSOI->sbuf.pBCur = (Tst_buf*)pBuffer;
	pSOI->sbuf.pBEnd = pBuffer + bufSize;
	pSOI->sbuf.sz    = bufSize;
	/* just for completeness */
	pSOI->sbuf.pIntnBuf  = (Tst_buf*)pBuffer;
	pSOI->sbuf.intnBufSz = pSOI->sbuf.sz;
	/* */
	if (ST_STREAM_LOBYTES(pSOI) > 0)
		pSOI->sbuf.leftoverBits = 8;

	return ST_ERR_SUCC;
}

/*
 * Set input source to pFStc
 *   The contents of the associated file won't be changed
 */
Tst_err
st_streamrd_setInput_file(Tst_streamrd *pSObj, Tst_sys_fstc *pFStc)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(pSObj == NULL || pSObj->pObInternal == NULL ||
			! st_sysFStc_isOpen(pFStc))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;

	st_streamrd_stc_rsetSObj(pSObj);
	st_stream_stc_rsetBuf(&pSOI->sbuf, ST_B_TRUE);
	/* alloc dummy buffer */
	ST_CALLOC(pSOI->sbuf.pIntnBuf, Tst_buf*, 1, 1)
	if (pSOI->sbuf.pIntnBuf == NULL)
		return ST_ERR_OMEM;
	pSOI->sbuf.intnBufSz = 1;
	pSOI->sbuf.sz        = 1;
	/* */
	pSOI->sbuf.pBBeg     = pSOI->sbuf.pIntnBuf;
	pSOI->sbuf.pBCur     = pSOI->sbuf.pBBeg;
	pSOI->sbuf.pBEnd     = pSOI->sbuf.pBBeg;
	pSOI->sbuf.wasAllocd = ST_B_TRUE;
	/* */
	pSOI->sfil.pFStc = pFStc;
	pSOI->sfil.cpos  = st_sysFStc_getFPos(pFStc);
	pSOI->sfil.fsz   = st_sysFStc_getFileSz(pFStc, NULL);

	return ST_STREAMRD__fillBufferFromFile(pSOI, ST_STREAMRD_DEFBUFSZ);
}

/*----------------------------------------------------------------------------*/

/*
 * Get current input position
 */
Tst_foffs
st_streamrd_getCurPos(const Tst_streamrd *pSObj)
{
	Tst_uint32 lo;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_NUM(-1, ! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc != NULL) {
		lo = ST_STREAM_LOBYTES(pSOI);
		if (pSOI->sfil.cpos < (Tst_foffs)lo)
			return -1;
		return (pSOI->sfil.cpos - (Tst_foffs)lo);
	}
	return (pSOI->sbuf.pBCur >= pSOI->sbuf.pBBeg ?
			(Tst_foffs)(pSOI->sbuf.pBCur - pSOI->sbuf.pBBeg) : -1);
}

/*----------------------------------------------------------------------------*/

/*
 * Get input size (either the size of the input buffer or the file)
 */
Tst_fsize
st_streamrd_getInputSize(const Tst_streamrd *pSObj)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_NUM(0, ! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc == NULL)
		return (Tst_fsize)(pSOI->sbuf.pBEnd - pSOI->sbuf.pBBeg);
	if (pSOI->sfil.fsz < 1 || pSOI->sfil.cpos < 0)
		return 0;
	return pSOI->sfil.fsz;
}

/*
 * Returns the amount of remaining bytes in the current buffer/file
 *   including the current byte
 */
Tst_fsize
st_streamrd_getAmountRemainingBytes(const Tst_streamrd *pSObj)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_NUM(0, ! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc == NULL)
		return (Tst_fsize)ST_STREAM_LOBYTES(pSOI);
	if (pSOI->sfil.fsz < 1 || pSOI->sfil.cpos < 0 ||
			pSOI->sfil.fsz < (Tst_fsize)pSOI->sfil.cpos)
		return 0;
	return ((Tst_fsize)ST_STREAM_LOBYTES(pSOI) +
			(pSOI->sfil.fsz - (Tst_fsize)pSOI->sfil.cpos));
}

/*
 * Returns the amount of remaining bits in the current buffer/file
 *   including the current byte
 */
void
st_streamrd_getAmountRemainingBits(const Tst_streamrd *pSObj,
		Tst_fsize *pBytes, Tst_byte *pBitsMod8)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_VOID(! ST_STREAM_CHECK(pSObj) || pBytes == NULL ||
			pBitsMod8 == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	*pBytes    = st_streamrd_getAmountRemainingBytes(pSObj);
	*pBitsMod8 = (pSOI->sbuf.leftoverBits < 8 ?
			pSOI->sbuf.leftoverBits : 0);
	if (*pBitsMod8 > 0) {
		ST_ASSERTN_VOID(*pBytes == 0)

		--(*pBytes);
	}
}

/*
 * Has stream reached its end ?
 */
Tst_bool
st_streamrd_isEOF(const Tst_streamrd *pSObj)
{
	Tst_uint32 lob;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_BOOL(ST_B_TRUE, ! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	lob = ST_STREAM_LOBYTES(pSOI);
	if (pSOI->sfil.pFStc == NULL)
		return (lob < 1 || (lob == 1 && pSOI->sbuf.leftoverBits < 1));
	if (pSOI->sfil.fsz < 1 || pSOI->sfil.cpos < 0 ||
			pSOI->sfil.fsz < (Tst_fsize)pSOI->sfil.cpos)
		return ST_B_TRUE;
	lob += (pSOI->sfil.fsz - (Tst_fsize)pSOI->sfil.cpos);
	return (lob < 1 || (lob == 1 && pSOI->sbuf.leftoverBits < 1));
}

/*
 * Returns the amount of remaining bits in the current byte
 */
Tst_byte
st_streamrd_getAmountRemainingBitsInCurByte(const Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return ((Tst_streamrd_intn*)pSObj->pObInternal)->sbuf.leftoverBits;
}

/*
 * Get amount of read bits
 */
void
st_streamrd_getAmountBitsRead(const Tst_streamrd *pSObj,
		Tst_fsize *pBytes, Tst_byte *pBitsMod8)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBytes == NULL || pBitsMod8 == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	*pBytes    = pSOI->totBytesRd +
			((pSOI->totBitsRd_rem - (pSOI->totBitsRd_rem % 8)) / 8);
	*pBitsMod8 = (Tst_byte)(pSOI->totBitsRd_rem % 8);
}

/*
 * Get amount of read bytes, not counting partially read byte
 */
Tst_fsize
st_streamrd_getAmountBytesRead(const Tst_streamrd *pSObj)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	return pSOI->totBytesRd +
			((pSOI->totBitsRd_rem - (pSOI->totBitsRd_rem % 8)) / 8);
}

/*----------------------------------------------------------------------------*/

/*
 * Set start-point for CRC-8 computation
 */
void
st_streamrd_startCRC8(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_startCRC8(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Pause or continue computation
 */
void
st_streamrd_pauseOrContinueCRC8(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_pauseOrContinueCRC8(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Update CRC with data in pBuf
 */
void
st_streamrd_updateCRC8(Tst_streamrd *pSObj,
		const Tst_buf *pBuf, const Tst_uint32 sz)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBuf == NULL)

	st_stream_updateCRC8(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs,
			pBuf, sz);
}

/*
 * Get current CRC-8 checksum
 */
Tst_byte
st_streamrd_getCRC8(const Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return st_stream_getCRC8(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Stop CRC-8 computation
 */
void
st_streamrd_stopCRC8(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_stopCRC8(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Set start-point for CRC-16 computation
 */
void
st_streamrd_startCRC16(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_startCRC16(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Pause or continue computation
 */
void
st_streamrd_pauseOrContinueCRC16(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_pauseOrContinueCRC16(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Update CRC with data in pBuf
 */
void
st_streamrd_updateCRC16(Tst_streamrd *pSObj,
		const Tst_buf *pBuf, const Tst_uint32 sz)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBuf == NULL)

	st_stream_updateCRC16(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs,
			pBuf, sz);
}

/*
 * Get current CRC-16 checksum
 */
Tst_uint16
st_streamrd_getCRC16(const Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return st_stream_getCRC16(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Stop CRC-16 computation
 */
void
st_streamrd_stopCRC16(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_stopCRC16(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Set start-point for CRC-32 computation
 */
void
st_streamrd_startCRC32(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_startCRC32(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Pause or continue computation
 */
void
st_streamrd_pauseOrContinueCRC32(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_pauseOrContinueCRC32(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Update CRC with data in pBuf
 */
void
st_streamrd_updateCRC32(Tst_streamrd *pSObj,
		const Tst_buf *pBuf, const Tst_uint32 sz)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBuf == NULL)

	st_stream_updateCRC32(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs,
			pBuf, sz);
}

/*
 * Get current CRC-32 checksum
 */
Tst_uint32
st_streamrd_getCRC32(const Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return st_stream_getCRC32(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Stop CRC-32 computation
 */
void
st_streamrd_stopCRC32(Tst_streamrd *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_stopCRC32(&((Tst_streamrd_intn*)pSObj->pObInternal)->crcs);
}

/*----------------------------------------------------------------------------*/

/*
 * Skip some bits
 *   and count them as read bits if countAsRead==ST_B_TRUE
 *   doesn't update CRCs
 */
Tst_err
st_streamrd_rdSkipBits(Tst_streamrd *pSObj,
		Tst_uint32 skip, const Tst_bool countAsRead)
{
	Tst_err    res  = ST_ERR_SUCC;
	Tst_byte   x    = 0;
	Tst_fsize  tbyr = 0;
	Tst_uint32 tbir = 0;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (! countAsRead) {
		tbyr = pSOI->totBytesRd;
		tbir = pSOI->totBitsRd_rem;
	}
	/* */
	while (skip > 0 && res == ST_ERR_SUCC) {
		res = st_streamrd_rdByte_noCRC(pSObj,
				(Tst_byte)(skip >= 8 ? 8 : skip), &x);
		if (skip >= 8)
			skip -= 8;
		else
			skip = 0;
	}
	/* */
	if (! countAsRead) {
		pSOI->totBytesRd    = tbyr;
		pSOI->totBitsRd_rem = tbir;
	}

	return res;
}

/*
 * Skip some bytes
 *   and count them as read bytes if countAsRead==ST_B_TRUE
 *   doesn't update CRCs
 */
Tst_err
st_streamrd_rdSkipBytes(Tst_streamrd *pSObj,
		Tst_uint32 skip, const Tst_bool countAsRead)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_byte   dec = 0;
	Tst_uint32 lob,
	           fskpd;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj))

	if (skip == 0)
		return ST_ERR_SUCC;

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sbuf.pBCur + skip >= pSOI->sbuf.pBEnd) {
		if (countAsRead) {
			/* add remaining bits to totBitsRd */
			if (pSOI->sbuf.leftoverBits > 0 &&
					pSOI->sbuf.leftoverBits < 8) {
				pSOI->totBitsRd_rem += pSOI->sbuf.leftoverBits;
				dec                  = 1;
			}
			lob = ST_STREAM_LOBYTES(pSOI);
			if (lob > 0) {
				pSOI->totBitsRd_rem += ((lob - dec) * 8);
				skip                -= lob;
			}
		}
		/* set buffer pointer to end-of-buffer */
		pSOI->sbuf.pBCur        = (Tst_buf*)pSOI->sbuf.pBEnd;
		pSOI->sbuf.leftoverBits = 0;
		if (pSOI->sfil.pFStc != NULL) {
			if (skip > 0) {
				/* skip some bytes in file */
				fskpd = st_sysFStc_skipBytes(pSOI->sfil.pFStc, skip);
				if (countAsRead)
					pSOI->totBitsRd_rem += (fskpd * 8);
				pSOI->sfil.cpos += (Tst_foffs)fskpd;
				/* */
				if (fskpd != skip)
					res = ST_ERR_FEOF;
			}
			/* refill buffer */
			if (res == ST_ERR_SUCC)
				res = ST_STREAMRD__fillBufferFromFile(pSOI, ST_STREAMRD_DEFBUFSZ);
		} else if (skip > 0)
			res = ST_ERR_FEOF;
	} else {
		if (countAsRead) {
			if (pSOI->sbuf.leftoverBits > 0 &&
					pSOI->sbuf.leftoverBits < 8) {
				pSOI->totBitsRd_rem += pSOI->sbuf.leftoverBits;
				dec                  = 1;
			}
			pSOI->totBitsRd_rem += (skip - dec) * 8;
		}
		pSOI->sbuf.pBCur       += skip;
		pSOI->sbuf.leftoverBits = 8;
	}
	/* */
	if (countAsRead && pSOI->totBitsRd_rem > 0xffff)
		ST_STREAMRD__INCREASE_READBITS(pSOI)

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Compare data read from SObj with pCmpWith
 *
 * returns: true if data matches
 */
Tst_bool
st_streamrd_rdMemcmpBytes(Tst_streamrd *pSObj,
		const Tst_uint32 nBytes, const void *pCmpWith)
{
	Tst_uint32 lob;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_BOOL(ST_B_FALSE,
			! ST_STREAM_CHECK(pSObj) || pCmpWith == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sbuf.leftoverBits != 0 && pSOI->sbuf.leftoverBits != 8)
		return ST_B_FALSE;

	lob = ST_STREAM_LOBYTES(pSOI);
	if (lob < nBytes) {
		if (pSOI->sfil.pFStc == NULL)
			return ST_B_FALSE;
		if (ST_STREAMRD__fillBufferFromFile(pSOI, nBytes) != ST_ERR_SUCC)
			return ST_B_FALSE;
		if (ST_STREAM_LOBYTES(pSOI) < nBytes)
			return ST_B_FALSE;
	}
	return (memcmp(pSOI->sbuf.pBCur, pCmpWith, nBytes) == 0);
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 */
/** updates CRCs */
Tst_err
st_streamrd_rdByte(Tst_streamrd *pSObj, const Tst_byte nBits,
		Tst_byte *pVal)
{
	Tst_byte rem;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 8)

	if (nBits == 0) {
		*pVal = 0;
		return ST_ERR_SUCC;
	}

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (nBits > pSOI->sbuf.leftoverBits) {
		/* case1 (nBits>loBits) */
		/*   n=8, lo=2       pB=xxxxxxAB  pV=AByyyyyy
		 *   n=7, lo=2       pB=xxxxxxAB  pV=0AByyyyy  */
		rem   = nBits - pSOI->sbuf.leftoverBits;
		*pVal = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) << rem;
		ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
		/*   n=8, rem=6      pB=CDEFGHIJ  pV=ABCDEFGH
		 *   n=7, rem=5      pB=CDEFGHIJ  pV=0ABCDEFG  */
		pSOI->sbuf.leftoverBits = 8 - rem;
		/* */
		*pVal |= ((*pSOI->sbuf.pBCur) >> pSOI->sbuf.leftoverBits);
	} else if (nBits == 8 && pSOI->sbuf.leftoverBits == 8) {
		/* case2 (nBits==loBits==8) */
		*pVal = *pSOI->sbuf.pBCur;
		ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	} else if (nBits < pSOI->sbuf.leftoverBits) {
		/* case3 (nBits<loBits) */
		*pVal = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) >>
				(pSOI->sbuf.leftoverBits - nBits);
		/* */
		pSOI->sbuf.leftoverBits -= nBits;
	} else {
		/* case4 ((nBits==loBits)!=8) */
		*pVal = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]);
		/* */
		ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	}
	/* */
	pSOI->totBitsRd_rem += nBits;
	if (pSOI->totBitsRd_rem > 0xffff)
		ST_STREAMRD__INCREASE_READBITS(pSOI)

	return ST_ERR_SUCC;
}

/** DOESN'T update CRCs */
Tst_err
st_streamrd_rdByte_noCRC(Tst_streamrd *pSObj, const Tst_byte nBits,
		Tst_byte *pVal)
{
	Tst_byte rem;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 8)

	if (nBits == 0) {
		*pVal = 0;
		return ST_ERR_SUCC;
	}

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (nBits > pSOI->sbuf.leftoverBits) {
		/* case1 (nBits>loBits) */
		/*   n=8, lo=2       pB=xxxxxxAB  pV=AByyyyyy
		 *   n=7, lo=2       pB=xxxxxxAB  pV=0AByyyyy  */
		rem   = nBits - pSOI->sbuf.leftoverBits;
		*pVal = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) << rem;
		ST_STREAMRD__NOCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
		/*   n=8, rem=6      pB=CDEFGHIJ  pV=ABCDEFGH
		 *   n=7, rem=5      pB=CDEFGHIJ  pV=0ABCDEFG  */
		pSOI->sbuf.leftoverBits = 8 - rem;
		/* */
		*pVal |= ((*pSOI->sbuf.pBCur) >> pSOI->sbuf.leftoverBits);
	} else if (nBits == 8 && pSOI->sbuf.leftoverBits == 8) {
		/* case2 (nBits==loBits==8) */
		*pVal = *pSOI->sbuf.pBCur;
		ST_STREAMRD__NOCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	} else if (nBits < pSOI->sbuf.leftoverBits) {
		/* case3 (nBits<loBits) */
		*pVal = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) >>
				(pSOI->sbuf.leftoverBits - nBits);
		/* */
		pSOI->sbuf.leftoverBits -= nBits;
	} else {
		/* case4 ((nBits==loBits)!=8) */
		*pVal = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]);
		/* */
		ST_STREAMRD__NOCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	}
	/* */
	pSOI->totBitsRd_rem += nBits;
	if (pSOI->totBitsRd_rem > 0xffff)
		ST_STREAMRD__INCREASE_READBITS(pSOI)

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* example input/output
 * Little-Endian input:
 *  rdByte[0]=0x01 rdByte[1]=0x00 rdByte[2]=0x00 rdByte[3]=0x04 ^= 67108865
 *  rdUInt32 --> ui32=0x01
 *               ui32<<8 | 0x00
 *               ui32<<8 | 0x00
 *               ui32<<8 | 0x04
 *             = on LE/BE: 0x01000004 ^= 16777220
 *
 * Big-Endian input:
 *  rdByte[0]=0x05 rdByte[1]=0x00 rdByte[2]=0x00 rdByte[3]=0x02 ^= 83886082
 *  rdUInt32 --> ui32=0x05
 *               ui32<<8 | 0x00
 *               ui32<<8 | 0x00
 *               ui32<<8 | 0x02
 *             = on LE/BE: 0x05000002 ^= 83886082
 */
/**printf("%s:%d> inp 0x%08x (0x%02x %02x %02x %02x)\n",
		__FILE__, __LINE__, ui32,
		((Tst_buf*)&ui32)[0], ((Tst_buf*)&ui32)[1],
		((Tst_buf*)&ui32)[2], ((Tst_buf*)&ui32)[3]);**/

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 */
Tst_err
st_streamrd_rdUInt16(Tst_streamrd *pSObj, const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_uint16 *pVal)
{
	Tst_err    res;
	Tst_uint32 ui32 = 0;

	ST_ASSERTN_IARG(pVal == NULL || nBits > 16 ||
			(inpEndian == ST_STREAMRD_IEND_LE && (nBits % 8) != 0))

	res = st_streamrd_rdUInt32(pSObj, ST_STREAMRD_IEND_IGN, nBits, &ui32);
	if (res == ST_ERR_SUCC) {
		*pVal = (Tst_uint16)ui32;
		if (inpEndian == ST_STREAMRD_IEND_LE) {
			/* ENDIAN: swap byte order */
			ST_STREAM_SWAP_BO_UI16(pVal)
			*pVal >>= (((16 - nBits) / 8) * 8);
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 */
Tst_err
st_streamrd_rdInt16(Tst_streamrd *pSObj, const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_int16 *pVal)
{
	Tst_err   res;
	Tst_int32 i32 = 0;

	ST_ASSERTN_IARG(pVal == NULL || nBits > 16 ||
			(inpEndian == ST_STREAMRD_IEND_LE && (nBits % 8) != 0))

	res = st_streamrd_rdInt32(pSObj, ST_STREAMRD_IEND_IGN, nBits, &i32);
	if (res == ST_ERR_SUCC) {
		*pVal = (Tst_int16)i32;
		if (inpEndian == ST_STREAMRD_IEND_LE) {
			/* ENDIAN: swap byte order */
			ST_STREAM_SWAP_BO_UI16(pVal)
			*pVal >>= (((16 - nBits) / 8) * 8);
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 */
Tst_err
st_streamrd_rdUInt32(Tst_streamrd *pSObj, const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_uint32 *pVal)
{
	Tst_byte byt,
	         xNBits = nBits;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 32 ||
			(inpEndian == ST_STREAMRD_IEND_LE && (nBits % 8) != 0))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	*pVal = 0;
	if (xNBits == 0)
		return ST_ERR_SUCC;
	byt = (*pSOI->sbuf.pBCur &
			ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]);

	pSOI->totBitsRd_rem += xNBits;
	do {
		if (xNBits <= pSOI->sbuf.leftoverBits) {
			*pVal <<= xNBits;
			*pVal  |= (byt >> (pSOI->sbuf.leftoverBits - xNBits));
			/* */
			pSOI->sbuf.leftoverBits -= xNBits;
			break;
		} else {
			*pVal <<= pSOI->sbuf.leftoverBits;
			*pVal  |= byt;
			xNBits -= pSOI->sbuf.leftoverBits;
			/* */
			ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			pSOI->sbuf.leftoverBits = 8;
			/* */
			byt = *pSOI->sbuf.pBCur;
		}
	} while (ST_B_TRUE);
	/* */
	if (pSOI->sbuf.leftoverBits == 0) {
		ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	}
	/* */
	if (pSOI->totBitsRd_rem > 0xffff)
		ST_STREAMRD__INCREASE_READBITS(pSOI)

	if (inpEndian == ST_STREAMRD_IEND_LE) {
		/* ENDIAN: swap byte order */
		ST_STREAM_SWAP_BO_UI32(pVal)
		*pVal >>= (((32 - nBits) / 8) * 8);
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 */
Tst_err
st_streamrd_rdInt32(Tst_streamrd *pSObj, const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_int32 *pVal)
{
	Tst_err res;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 32 ||
			(inpEndian == ST_STREAMRD_IEND_LE && (nBits % 8) != 0))

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (nBits > 8) {
		Tst_uint32 ui32 = 0;

		if (nBits - pSOI->sbuf.leftoverBits <= 8)
			res = ST_STREAMRD__rdUInt32_case2(pSOI, nBits, &ui32);
		else if (nBits - pSOI->sbuf.leftoverBits <= 16)
			res = ST_STREAMRD__rdUInt32_case3(pSOI, nBits, &ui32);
		else
			res = st_streamrd_rdUInt32(pSObj,
					ST_STREAMRD_IEND_IGN, nBits, &ui32);
		if (inpEndian == ST_STREAMRD_IEND_LE) {
			/* ENDIAN: swap byte order */
			ST_STREAM_SWAP_BO_UI32(&ui32)
			ui32 >>= (((32 - nBits) / 8) * 8);
		}
		/* sign-extend: */
		*pVal   = (Tst_int32)(ui32 << (32 - nBits));
		*pVal >>= (32 - nBits);
	} else if (nBits > 0) {
		Tst_byte byt = 0;

		res = st_streamrd_rdByte(pSObj, nBits, &byt);
		/* sign-extend: */
		*pVal   = (Tst_int32)(byt << (32 - nBits));
		*pVal >>= (32 - nBits);
	} else {
		*pVal = 0;
		res   = ST_ERR_SUCC;
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 */
Tst_err
st_streamrd_rdUInt64(Tst_streamrd *pSObj, const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_uint64 *pVal)
{
	Tst_byte byt,
	         xNBits = nBits;
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_byte usedBits = 0;
#	endif
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 64 ||
			(inpEndian == ST_STREAMRD_IEND_LE && (nBits % 8) != 0))

	ST_SYSMATH_STC_RSETUI64(*pVal)
	if (xNBits == 0)
		return ST_ERR_SUCC;

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	byt = (*pSOI->sbuf.pBCur &
			ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]);

	pSOI->totBitsRd_rem += xNBits;
	do {
		if (xNBits <= pSOI->sbuf.leftoverBits) {
#			if (CONFIG_ST_ALL_HAVE64BIT == 1)
			pVal->nativeU64 = (pVal->nativeU64 << xNBits) |
					(Tst_uint64_native)(byt >> (pSOI->sbuf.leftoverBits - xNBits));
#			else
			if (usedBits + xNBits > 32) {
				pVal->hiU = (pVal->hiU << xNBits) |
						(pVal->loU >> (32 - xNBits));
			}
			pVal->loU = (pVal->loU << xNBits) |
					(byt >> (pSOI->sbuf.leftoverBits - xNBits));
#			endif
			/* */
			pSOI->sbuf.leftoverBits -= xNBits;
			break;
		} else {
#			if (CONFIG_ST_ALL_HAVE64BIT == 1)
			pVal->nativeU64 = (pVal->nativeU64 << pSOI->sbuf.leftoverBits) | byt;
#			else
			if (usedBits + pSOI->sbuf.leftoverBits > 32) {
				/* xxYYYYYY, used=6, loB=3
				 *   --> 0000000Y YYYYYxxx  */
				pVal->hiU = (pVal->hiU << pSOI->sbuf.leftoverBits) |
						(pVal->loU >> (32 - pSOI->sbuf.leftoverBits));
			}
			pVal->loU = (pVal->loU << pSOI->sbuf.leftoverBits) | byt;
			/* */
			usedBits += pSOI->sbuf.leftoverBits;
#			endif
			xNBits -= pSOI->sbuf.leftoverBits;
			/* */
			ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			pSOI->sbuf.leftoverBits = 8;
			/* */
			byt = *pSOI->sbuf.pBCur;
		}
	} while (ST_B_TRUE);
	/* */
	if (pSOI->sbuf.leftoverBits == 0)
		ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	/* */
	if (pSOI->totBitsRd_rem > 0xffff)
		ST_STREAMRD__INCREASE_READBITS(pSOI)

	if (inpEndian == ST_STREAMRD_IEND_LE) {
		/* ENDIAN: swap byte order */
		st_sysUInt64_reverseByteOrder(pVal);
		if (((64 - nBits) / 8) * 8 > 0)
			st_sysUInt64_shiftRight(pVal, ((64 - nBits) / 8) * 8);
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 */
Tst_err
st_streamrd_rdInt64(Tst_streamrd *pSObj, const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_int64 *pVal)
{
	Tst_err    res;
	Tst_uint64 valUI;
	Tst_uint32 sh;

	ST_ASSERTN_IARG(inpEndian == ST_STREAMRD_IEND_LE && (nBits % 8) != 0)

	ST_SYSMATH_STC_RSETUI64(valUI)
	res = st_streamrd_rdUInt64(pSObj, ST_STREAMRD_IEND_IGN, nBits, &valUI);
	if (res != ST_ERR_SUCC)
		return res;

	sh = ((64 - nBits) / 8) * 8;
	if (inpEndian == ST_STREAMRD_IEND_LE) {
		/* ENDIAN: swap byte order */
		st_sysUInt64_reverseByteOrder(&valUI);
	} else if (sh > 0)
			st_sysUInt64_shiftLeft(&valUI, sh);

	st_sysUInt64_toI64(&valUI, pVal);
	if (sh > 0)
		st_sysSInt64_shiftRight(pVal, sh);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBytes bytes and store them in pBuf
 *
 * pSObj        MUST be set
 * pBuf         MUST be != NULL
 * pReadBytes   MUST be != NULL
 */
Tst_err
st_streamrd_rdBuffer(Tst_streamrd *pSObj, const Tst_uint32 nBytes,
		Tst_buf *pBuf, Tst_uint32 *pReadBytes)
{
	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj))

	return ST_STREAMRD__rdBuffer_pr(pSObj,
			nBytes, pBuf, pReadBytes, ST_B_TRUE);
}

Tst_err
st_streamrd_rdBuffer_noCRC(Tst_streamrd *pSObj, const Tst_uint32 nBytes,
		Tst_buf *pBuf, Tst_uint32 *pReadBytes)
{
	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj))

	return ST_STREAMRD__rdBuffer_pr(pSObj,
			nBytes, pBuf, pReadBytes, ST_B_FALSE);
}

/*----------------------------------------------------------------------------*/

/* why as macro ?
 * in flac files this macro is used many many million times
 *   via rdRiceSamples().
 * using st_streamrd_rdUnary() instead of this macro
 * results in a much longer runtime
 */
/** updates CRCs */
#define LOC_RDUNARY_MACRO_CRC_(mac_pSOI, mac_unaryByt, mac_unaryRes)  { \
		(mac_unaryRes) = 0; \
		(mac_unaryByt) = (*(mac_pSOI)->sbuf.pBCur & \
				ST_STREAM_LOBITS_MASK_8[(mac_pSOI)->sbuf.leftoverBits]) << \
				(8 - (mac_pSOI)->sbuf.leftoverBits); \
		\
		while ((mac_unaryByt) == 0) { \
			(mac_unaryRes) += (mac_pSOI)->sbuf.leftoverBits; \
			/* */ \
			ST_STREAMRD__UPDCRC_INCPOS_RET(mac_pSOI)  /* ret ST_ERR_FEOF */ \
			/* */ \
			(mac_pSOI)->sbuf.leftoverBits = 8; \
			/* */ \
			(mac_unaryByt) = *(mac_pSOI)->sbuf.pBCur; \
		} \
		/* */ \
		(mac_unaryByt)  = 8 - ST_STREAMRD_RDUNARY_MSB_TABLE[mac_unaryByt]; \
		(mac_unaryRes) += (mac_unaryByt); \
		/* */ \
		(mac_pSOI)->sbuf.leftoverBits -= (mac_unaryByt + 1); \
		/* */ \
		(mac_pSOI)->totBitsRd_rem += (mac_unaryRes + 1); \
		if ((mac_pSOI)->totBitsRd_rem > 0xffff) \
			ST_STREAMRD__INCREASE_READBITS(mac_pSOI) \
		/* */ \
		if ((mac_pSOI)->sbuf.leftoverBits == 0) \
			ST_STREAMRD__UPDCRC_INCPOS_NORET(mac_pSOI)  /* doesn't ret ST_ERR_FEOF */ \
		}
/** DOESN'T update CRCs */
#define LOC_RDUNARY_MACRO_NOCRC_(mac_pSOI, mac_unaryByt, mac_unaryRes)  { \
		(mac_unaryRes) = 0; \
		(mac_unaryByt) = (*(mac_pSOI)->sbuf.pBCur & \
				ST_STREAM_LOBITS_MASK_8[(mac_pSOI)->sbuf.leftoverBits]) << \
				(8 - (mac_pSOI)->sbuf.leftoverBits); \
		\
		while ((mac_unaryByt) == 0) { \
			(mac_unaryRes) += (mac_pSOI)->sbuf.leftoverBits; \
			/* */ \
			ST_STREAMRD__NOCRC_INCPOS_RET(mac_pSOI)  /* ret ST_ERR_FEOF */ \
			/* */ \
			(mac_pSOI)->sbuf.leftoverBits = 8; \
			/* */ \
			(mac_unaryByt) = *(mac_pSOI)->sbuf.pBCur; \
		} \
		/* */ \
		(mac_unaryByt)  = 8 - ST_STREAMRD_RDUNARY_MSB_TABLE[mac_unaryByt]; \
		(mac_unaryRes) += (mac_unaryByt); \
		/* */ \
		(mac_pSOI)->sbuf.leftoverBits -= (mac_unaryByt + 1); \
		/* */ \
		(mac_pSOI)->totBitsRd_rem += (mac_unaryRes + 1); \
		if ((mac_pSOI)->totBitsRd_rem > 0xffff) \
			ST_STREAMRD__INCREASE_READBITS(mac_pSOI) \
		/* */ \
		if ((mac_pSOI)->sbuf.leftoverBits == 0) \
			ST_STREAMRD__NOCRC_INCPOS_NORET(mac_pSOI)  /* doesn't ret ST_ERR_FEOF */ \
		}

/*
 * Read unary-encoded unsigned int, decode and store it in pVal
 *   (e.g. the decimal value 10 would be encoded as binary 00000000001,
 *         -------- " ------  0 ---------- " ------------- 1,
 *         read from left to right)
 */
Tst_err
st_streamrd_rdUnary(Tst_streamrd *pSObj, Tst_uint32 *pVal)
{
	Tst_byte byt;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->crcs.compCRC8 || pSOI->crcs.compCRC16 || pSOI->crcs.compCRC32) {
		LOC_RDUNARY_MACRO_CRC_(pSOI, byt, *pVal)
	} else {
		LOC_RDUNARY_MACRO_NOCRC_(pSOI, byt, *pVal)
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read NUL-terminated string with at most maxBytes bytes
 *   If maxBytes==0 then there is no limit
 */
Tst_err
st_streamrd_rdString(Tst_streamrd *pSObj, const Tst_uint32 maxBytes,
		Tst_str **ppStr)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 len    = 0,
	           allcd;
	Tst_str    ch     = 0;
	Tst_bool   isTerm = ST_B_FALSE;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || ppStr == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;

	if (pSOI->sbuf.leftoverBits != 0 && pSOI->sbuf.leftoverBits != 8)
		return ST_ERR_FAIL;

	allcd = 128;
	ST_REALLOC(*ppStr, Tst_str*, allcd, 1)
	if (*ppStr == NULL)
		return ST_ERR_OMEM;

	while (maxBytes == 0 || len < maxBytes) {
		res = st_streamrd_rdByte(pSObj, 8, (Tst_byte*)&ch);
		if (res != ST_ERR_SUCC)
			break;
		/* */
		if (len + 1 >= allcd) {
			allcd += 128;
			ST_REALLOC(*ppStr, Tst_str*, allcd, 1)
			if (*ppStr == NULL) {
				res = ST_ERR_OMEM;
				break;
			}
		}
		/* */
		(*ppStr)[len++] = ch;
		if (ch == 0) {
			isTerm = ST_B_TRUE;
			break;
		}
	}
	if (res == ST_ERR_SUCC && ! isTerm) {
		if (len + 1 >= allcd) {
			allcd += 8;
			ST_REALLOC(*ppStr, Tst_str*, allcd, 1)
			if (*ppStr == NULL)
				return ST_ERR_OMEM;
		}
		/* */
		(*ppStr)[len] = 0;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBits bits and store them in pVal
 *   The current buffer-position etc. won't be changed
 */
Tst_err
st_streamrd_rdAheadByte(Tst_streamrd *pSObj, const Tst_byte nBits,
		Tst_byte *pVal)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 8)

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* */
	res = st_streamrd_rdByte_noCRC(pSObj, nBits, pVal);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

Tst_err
st_streamrd_rdAheadUInt16(Tst_streamrd *pSObj,
		const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_uint16 *pVal)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 16)

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* */
	res = st_streamrd_rdUInt16(pSObj, inpEndian, nBits, pVal);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

Tst_err
st_streamrd_rdAheadInt16(Tst_streamrd *pSObj,
		const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_int16 *pVal)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 16)

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* */
	res = st_streamrd_rdInt16(pSObj, inpEndian, nBits, pVal);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

Tst_err
st_streamrd_rdAheadUInt32(Tst_streamrd *pSObj,
		const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_uint32 *pVal)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 32)

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* */
	res = st_streamrd_rdUInt32(pSObj, inpEndian, nBits, pVal);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

Tst_err
st_streamrd_rdAheadInt32(Tst_streamrd *pSObj,
		const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_int32 *pVal)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 32)

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* */
	res = st_streamrd_rdInt32(pSObj, inpEndian, nBits, pVal);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

Tst_err
st_streamrd_rdAheadUInt64(Tst_streamrd *pSObj,
		const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_uint64 *pVal)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 64)

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* */
	res = st_streamrd_rdUInt64(pSObj, inpEndian, nBits, pVal);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

Tst_err
st_streamrd_rdAheadInt64(Tst_streamrd *pSObj,
		const Tst_streamrd_endian inpEndian,
		const Tst_byte nBits, Tst_int64 *pVal)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL ||
			nBits > 64)

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* */
	res = st_streamrd_rdInt64(pSObj, inpEndian, nBits, pVal);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

Tst_err
st_streamrd_rdAheadBuffer(Tst_streamrd *pSObj, const Tst_uint32 nBytes,
		Tst_buf *pBuf, Tst_uint32 *pReadBytes)
{
	Tst_err res;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pBuf == NULL ||
			pReadBytes == NULL)

	if (((Tst_streamrd_intn*)pSObj->pObInternal)->sbuf.leftoverBits != 0 &&
			((Tst_streamrd_intn*)pSObj->pObInternal)->sbuf.leftoverBits != 8)
		return ST_ERR_FAIL;

	ST_STREAMRD__RDAHEAD_PREPARE((Tst_streamrd_intn*)pSObj->pObInternal)
	/* can't use ST_STREAMRD__rdBuffer_pr() here */
	res = ST_STREAMRD__rdBuffer_prSub((Tst_streamrd_intn*)pSObj->pObInternal,
			nBytes, pBuf, pReadBytes, ST_B_FALSE);
	/* */
	ST_STREAMRD__RDAHEAD_RESTORE((res == ST_ERR_SUCC),
			(Tst_streamrd_intn*)pSObj->pObInternal)
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Decode Flac's Rice-encoded Residual samples
 *
 * pResids  may be NULL, if != NULL decoded values will be stored in pResids
 */
Tst_err
st_streamrd_rdFlacRiceSamples(Tst_streamrd *pSObj,
		Tst_uint32 samplesTot, const Tst_byte ricePar, Tst_int32 *pResids)
{
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || ricePar > 32)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;
	/* */
	if (ricePar > 8) {
		if (pSOI->crcs.compCRC8 || pSOI->crcs.compCRC16 || pSOI->crcs.compCRC32)
			return ST_STREAMRD__rdRiceSamples_rpNZ_9_32_CRC(pSObj, pSOI,
					samplesTot, ricePar, pResids);
		else
			return ST_STREAMRD__rdRiceSamples_rpNZ_9_32_NOCRC(pSObj, pSOI,
					samplesTot, ricePar, pResids);
	}
	if (ricePar > 0) {
		if (pSOI->crcs.compCRC8 || pSOI->crcs.compCRC16 || pSOI->crcs.compCRC32)
			return ST_STREAMRD__rdRiceSamples_rpNZ_1_8_CRC(pSOI,
					samplesTot, ricePar, pResids);
		else
			return ST_STREAMRD__rdRiceSamples_rpNZ_1_8_NOCRC(pSOI,
					samplesTot, ricePar, pResids);
	}
	return ST_STREAMRD__rdRiceSamples_rpZ(pSOI, samplesTot, pResids);
}

/*----------------------------------------------------------------------------*/

/*
 * Reads a UTF8-encoded uint32 with up to 6 bytes of input
 *     and 31 bits of output
 *   This is used by Flac only
 */
Tst_err
st_streamrd_rdFlacUtf8Encoded_uint32(Tst_streamrd *pSObj,
		Tst_uint32 *pVal)
{
	Tst_err    res;
	Tst_uint32 v = 0,
	           i;
	Tst_byte   x = 0;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL)

	*pVal = 0;

	res = st_streamrd_rdByte(pSObj, 8, &x);
	if (res != ST_ERR_SUCC)
		return res;

	if (! (x & 0x80)) { /* 0xxxxxxx */
		v = x;
		i = 0;
	} else if (x & 0xC0 && !(x & 0x20)) { /* 110xxxxx */
		v = x & 0x1F;
		i = 1;
	} else if (x & 0xE0 && !(x & 0x10)) { /* 1110xxxx */
		v = x & 0x0F;
		i = 2;
	} else if (x & 0xF0 && !(x & 0x08)) { /* 11110xxx */
		v = x & 0x07;
		i = 3;
	} else if (x & 0xF8 && !(x & 0x04)) { /* 111110xx */
		v = x & 0x03;
		i = 4;
	} else if (x & 0xFC && !(x & 0x02)) { /* 1111110x */
		v = x & 0x01;
		i = 5;
	} else
		return ST_ERR_IDAT;
	for ( ; i > 0; i--) {
		res = st_streamrd_rdByte(pSObj, 8, &x);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (! (x & 0x80) || (x & 0x40)) /* 10xxxxxx */
			return ST_ERR_IDAT;
		v <<= 6;
		v  |= (x & 0x3F);
	}
	*pVal = v;
	return ST_ERR_SUCC;
}

/*
 * Reads a UTF8-encoded uint64 with up to 7 bytes of input
 *     and 36 bits of output
 *   This is used by Flac only
 */
Tst_err
st_streamrd_rdFlacUtf8Encoded_uint64(Tst_streamrd *pSObj,
		Tst_uint64 *pVal)
{
	Tst_err    res;
	Tst_byte   x      = 0;
	Tst_uint32 i      = 0,
	           tmp32  = 0;
	#if (CONFIG_ST_ALL_HAVE64BIT != 1)
		Tst_bool full36 = ST_B_FALSE;
	#endif

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pVal == NULL)

	ST_SYSMATH_STC_RSETUI64((*pVal))

	res = st_streamrd_rdByte(pSObj, 8, &x);
	if (res != ST_ERR_SUCC)
		return res;

	if (! (x & 0x80)) { /* 0xxxxxxx */
		tmp32 = x;  /* 8 bit */
		i     = 0;
	} else if (x & 0xC0 && !(x & 0x20)) { /* 110xxxxx */
		tmp32 = x & 0x1F;  /* 5 bit */
		i     = 1;
	} else if (x & 0xE0 && !(x & 0x10)) { /* 1110xxxx */
		tmp32 = x & 0x0F;  /* 4 bit */
		i     = 2;
	} else if (x & 0xF0 && !(x & 0x08)) { /* 11110xxx */
		tmp32 = x & 0x07;  /* 3 bit */
		i     = 3;
	} else if (x & 0xF8 && !(x & 0x04)) { /* 111110xx */
		tmp32 = x & 0x03;  /* 2 bit */
		i     = 4;
	} else if (x & 0xFC && !(x & 0x02)) { /* 1111110x */
		tmp32 = x & 0x01;  /* 1 bit */
		i     = 5;
	} else if (x & 0xFE && !(x & 0x01)) { /* 11111110 */
		tmp32 = 0;  /* 0 bit */
		i     = 6;
		/* */
		#if (CONFIG_ST_ALL_HAVE64BIT != 1)
			full36 = ST_B_TRUE;
		#endif
	} else
		return ST_ERR_IDAT;

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeU64 = (Tst_uint64_native)tmp32;
#	else
	pVal->loU = tmp32;
#	endif

	for( ; i > 0; i--) {
		res = st_streamrd_rdByte(pSObj, 8, &x);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (! (x & 0x80) || (x & 0x40))  /* 10xxxxxx */
			return ST_ERR_IDAT;
		/* */
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		pVal->nativeU64 = (pVal->nativeU64 << 6) | (x & 0x3F);  /* +6 bit */
#		else
		if (full36 && i == 1) {
			/* 30 bits used so far */
			pVal->hiU = pVal->loU >> 26;
		}
		pVal->loU <<= 6;
		pVal->loU  |= (x & 0x3F);  /* +6 bit */
#		endif
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* to generate bitmasks...
static void ST_STREAMRD__generateMasks32()
{
	Tst_uint32 x, cur, mask;

	for (cur = 0; cur < 33; cur++) {
		mask = 0;
		for (x = 0; x < cur; x++)
			mask |= (1 << x);
		ST_STREAMRD__prf("0x%08x,", mask);
		if (cur == 0 || cur % 4 == 0)
			ST_STREAMRD__prf("\n");
		else
			ST_STREAMRD__prf(" ");
	}
	ST_STREAMRD__prf("\n");
}

static void ST_STREAMRD__generateMasks64()
{
	Tst_uint32 x, cur, hi, lo;

	for (cur = 0; cur < 65; cur++) {
		lo = 0;
		hi = 0;
		for (x = 0; x < cur; x++)
			if (x < 32)
				lo |= (1 << x);
			else
				hi |= (1 << (x - 32));
		ST_STREAMRD__prf("0x%08x%08xULL,", hi, lo);
		if (cur == 0 || cur % 2 == 0)
			ST_STREAMRD__prf("\n");
		else
			ST_STREAMRD__prf(" ");
	}
	ST_STREAMRD__prf("\n");
}
*/

/*----------------------------------------------------------------------------*/

static Tst_err
ST_STREAMRD__fillBufferFromFile(Tst_streamrd_intn *pSOI,
		Tst_uint32 amount)
{
	Tst_err    res;
	Tst_byte   loBits;
	Tst_uint32 loByts,
	           cpd     = 0,
	           restor,
	           bcrOffs = 0;
	Tst_buf    *pBCR   = NULL;

	loByts = ST_STREAM_LOBYTES(pSOI);
	if (loByts >= amount)
		return ST_ERR_SUCC;

	if (pSOI->sbuf.pBCurRestore != NULL &&
			pSOI->sbuf.pBCurRestore < pSOI->sbuf.pBEnd) {
		/* translate pBCurRestore to relative offset */
		pBCR    = pSOI->sbuf.pBCurRestore;
		bcrOffs = (Tst_uint32)(pSOI->sbuf.pBEnd - pBCR);
	}

	restor = loByts + 8;
	if (restor > ST_STREAM_BUFSIZE(pSOI))
		restor = ST_STREAM_BUFSIZE(pSOI);
	if (amount > loByts)
		amount -= loByts;
	amount += (1024 - ((amount + restor) % 1024));

	/* append new data to remaining bytes */
	/** */
	loBits = pSOI->sbuf.leftoverBits;
	/** */
	res = st_stream_stc_reszIntnBuf(&pSOI->sbuf, amount + restor, restor);
	if (res != ST_ERR_SUCC)
		return res;
	/** */
	if (pSOI->sfil.cpos >= 0 && (Tst_fsize)pSOI->sfil.cpos < pSOI->sfil.fsz) {
		/* read new bytes from file */
		res = st_sysFStc_readBuf(pSOI->sfil.pFStc, amount,
				&pSOI->sbuf.pBBeg[restor], &cpd);
		if (res != ST_ERR_SUCC)
			return res;
		pSOI->sfil.cpos = st_sysFStc_getFPos(pSOI->sfil.pFStc);
	}
	/* */
	pSOI->sbuf.pBEnd = pSOI->sbuf.pBBeg + restor + cpd;
	pSOI->sbuf.pBCur = (Tst_buf*)(pSOI->sbuf.pBEnd - cpd - loByts);
	/* */
	if (loBits == 0 && cpd > 0)
		pSOI->sbuf.leftoverBits = 8;
	else
		pSOI->sbuf.leftoverBits = loBits;
	/* */
	if (pBCR != NULL) {
		/* update pBCurRestore */
		pSOI->sbuf.pBCurRestore = (Tst_buf*)(pSOI->sbuf.pBEnd - cpd - bcrOffs);
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

static Tst_bool
ST_STREAMRD__fillBuf_n_setBCur(Tst_streamrd_intn *pSOI,
		const Tst_uint32 inc)
{
	Tst_bool hadOneLO = (ST_STREAM_LOBYTES(pSOI) > 0);

	if (ST_STREAMRD__fillBufferFromFile(pSOI,
			(inc < ST_STREAMRD_DEFBUFSZ ?
				ST_STREAMRD_DEFBUFSZ : inc)) != ST_ERR_SUCC) {
		pSOI->sbuf.pBCur        = (Tst_buf*)pSOI->sbuf.pBEnd;
		pSOI->sbuf.leftoverBits = 0;
		return ST_B_FALSE;
	}
	if (ST_STREAM_LOBYTES(pSOI) < inc) {
		pSOI->sbuf.pBCur        = (Tst_buf*)pSOI->sbuf.pBEnd;
		pSOI->sbuf.leftoverBits = 0;
		return ST_B_FALSE;
	}

	if (hadOneLO) {
		++pSOI->sbuf.pBCur;
		if (pSOI->sbuf.pBCur >= pSOI->sbuf.pBEnd) {
			pSOI->sbuf.leftoverBits = 0;
			return ST_B_FALSE;
		}
	}
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Read nBytes from input stream to pBuf
 *   If the stream ended before nBytes could be read
 *   *pReadBytes will be set to the actual amount of read bytes
 */
static Tst_err
ST_STREAMRD__rdBuffer_pr(Tst_streamrd *pSObj,
		const Tst_uint32 nBytes,
		Tst_buf *pBuf, Tst_uint32 *pReadBytes,
		const Tst_bool withCRCs)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 toRd   = ST_STREAMRD_DEFBUFSZ,
	           rdOnce = 0;
	Tst_streamrd_intn *pSOI;

	ST_ASSERTN_IARG(pBuf == NULL || pReadBytes == NULL)

	pSOI = (Tst_streamrd_intn*)pSObj->pObInternal;

	if (pSOI->sbuf.leftoverBits != 0 && pSOI->sbuf.leftoverBits != 8)
		return ST_ERR_FAIL;

	*pReadBytes = 0;
	while (*pReadBytes < nBytes) {
		if (*pReadBytes + toRd > nBytes)
			toRd = nBytes - *pReadBytes;
		res = ST_STREAMRD__rdBuffer_prSub(pSOI, toRd,
				&pBuf[*pReadBytes], &rdOnce, withCRCs);
		if (res == ST_ERR_SUCC)
			res = st_streamrd_rdSkipBytes(pSObj, rdOnce, ST_B_TRUE);
		*pReadBytes += rdOnce;
		if (res != ST_ERR_SUCC || toRd != rdOnce)
			break;
	}
	return res;
}

static Tst_err
ST_STREAMRD__rdBuffer_prSub(Tst_streamrd_intn *pSOI,
		const Tst_uint32 nBytes,
		Tst_buf *pBuf, Tst_uint32 *pReadBytes,
		const Tst_bool withCRCs)
{
	Tst_uint32 lob;

	*pReadBytes = nBytes;
	/* */
	lob = ST_STREAM_LOBYTES(pSOI);
	if (lob < nBytes) {
		Tst_err res;

		if (pSOI->sfil.pFStc != NULL) {
			res = ST_STREAMRD__fillBufferFromFile(pSOI, nBytes);
			if (res != ST_ERR_SUCC)
				return res;
			lob = ST_STREAM_LOBYTES(pSOI);
		}
		if (lob == 0)
			return ST_ERR_FEOF;
		/* */
		if (lob < nBytes)
			*pReadBytes = lob;
	}

	if (withCRCs) {
		Tst_buf *pOB,
		        *pIB,
		        *pIBEnd;

		/* */
		pOB    = pBuf;
		pIB    = pSOI->sbuf.pBCur;
		pIBEnd = pIB + *pReadBytes;
		if (pSOI->crcs.compCRC8) {
			while (pIB < pIBEnd)
				ST_STREAM_UPDATE_CRC8(pSOI->crcs, *pIB++)
			pIB = pSOI->sbuf.pBCur;
		}
		if (pSOI->crcs.compCRC16) {
			while (pIB < pIBEnd)
				ST_STREAM_UPDATE_CRC16(pSOI->crcs, *pIB++)
			pIB = pSOI->sbuf.pBCur;
		}
		if (pSOI->crcs.compCRC32) {
			while (pIB < pIBEnd)
				ST_STREAM_UPDATE_CRC32(pSOI->crcs, *pIB++)
			pIB = pSOI->sbuf.pBCur;
		}
		while (pIB < pIBEnd)
			*pOB++ = *pIB++;
	} else {
		memcpy(pBuf, pSOI->sbuf.pBCur, *pReadBytes);
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* optimized for:
 *   n>=leftoverBits && n-leftoverBits<=8
 *      --> value spans 2 bytes at most */
static Tst_err
ST_STREAMRD__rdUInt32_case2(Tst_streamrd_intn *pSOI,
		Tst_byte nBits, Tst_uint32 *pVal)
{
	ST_ASSERTN_IARG(nBits < pSOI->sbuf.leftoverBits ||
			nBits - pSOI->sbuf.leftoverBits > 8)

	pSOI->totBitsRd_rem += nBits;
	if (pSOI->totBitsRd_rem > 0xffff)
		ST_STREAMRD__INCREASE_READBITS(pSOI)
	/* */
	*pVal = (*pSOI->sbuf.pBCur &
			ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) <<
			(nBits - pSOI->sbuf.leftoverBits);
	/* */
	nBits -= pSOI->sbuf.leftoverBits;

	ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
	*pVal |= (*pSOI->sbuf.pBCur >> (8 - nBits));

	if (nBits == 8) {
		ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	} else
		pSOI->sbuf.leftoverBits = 8 - nBits;

	return ST_ERR_SUCC;
}

/* optimized for:
 *   n>=leftoverBits && n-leftoverBits<=16
 *     --> value spans 3 bytes at most */
static Tst_err
ST_STREAMRD__rdUInt32_case3(Tst_streamrd_intn *pSOI,
		Tst_byte nBits, Tst_uint32 *pVal)
{
	ST_ASSERTN_IARG(nBits < pSOI->sbuf.leftoverBits ||
			nBits - pSOI->sbuf.leftoverBits > 16)

	pSOI->totBitsRd_rem += nBits;
	if (pSOI->totBitsRd_rem > 0xffff)
		ST_STREAMRD__INCREASE_READBITS(pSOI)
	/* */
	*pVal = (*pSOI->sbuf.pBCur &
			ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) <<
			(nBits - pSOI->sbuf.leftoverBits);
	/* */
	nBits -= pSOI->sbuf.leftoverBits;

	ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
	*pVal |= (*pSOI->sbuf.pBCur << (nBits - 8));
	ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
	*pVal |= (*pSOI->sbuf.pBCur >> (16 - nBits));

	if (nBits == 16) {
		ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
	} else
		pSOI->sbuf.leftoverBits = 16 - nBits;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* optimized for: ricePar == 0 */
static Tst_err
ST_STREAMRD__rdRiceSamples_rpZ(Tst_streamrd_intn *pSOI,
		Tst_uint32 samplesTot, Tst_int32 *pResids)
{
	register Tst_byte   byt  = 0;
	register Tst_uint32 uval = 0;  /* unary value */

	while (samplesTot > 0) {
		/* read unary part */
		LOC_RDUNARY_MACRO_CRC_(pSOI, byt, uval)

		/* decode value */
		if (pResids != NULL)
			*pResids++ = (Tst_int32)(uval >> 1) ^ -(Tst_int32)(uval & 1);

		--samplesTot;
	}
	return ST_ERR_SUCC;
}

/* optimized for: 0 < ricePar <= 8
 *  updates CRCs */
static Tst_err
ST_STREAMRD__rdRiceSamples_rpNZ_1_8_CRC(Tst_streamrd_intn *pSOI,
		Tst_uint32 samplesTot, const Tst_byte ricePar, Tst_int32 *pResids)
{
	register Tst_byte   byt  = 0;
	Tst_byte            rem;
	register Tst_uint32 uval = 0;  /* unary value */

	while (samplesTot > 0) {
		/* read unary part */
		LOC_RDUNARY_MACRO_CRC_(pSOI, byt, uval)

		/* read binary part */

		if (ricePar > pSOI->sbuf.leftoverBits) {
			/* from st_streamrd_rdByte() case1  */
			/** */
			rem = ricePar - pSOI->sbuf.leftoverBits;
			byt = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) << rem;
			ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			pSOI->sbuf.leftoverBits = 8 - rem;
			/** */
			byt |= ((*pSOI->sbuf.pBCur) >> pSOI->sbuf.leftoverBits);
		} else if (ricePar == 8 && pSOI->sbuf.leftoverBits == 8) {
			/* from st_streamrd_rdByte() case2  */
			byt = *pSOI->sbuf.pBCur;
			ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
		} else if (ricePar < pSOI->sbuf.leftoverBits) {
			/* from st_streamrd_rdByte() case3  */
			byt = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) >>
					(pSOI->sbuf.leftoverBits - ricePar);
			pSOI->sbuf.leftoverBits -= ricePar;
		} else {
			/* from st_streamrd_rdByte() case4  */
			/** */
			byt = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]);
			/** */
			ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
		}
		pSOI->totBitsRd_rem += ricePar;
		if (pSOI->totBitsRd_rem > 0xffff)
			ST_STREAMRD__INCREASE_READBITS(pSOI)

		/* compose the value */
		if (pResids != NULL) {
			uval        = (uval << ricePar) | byt;
			*pResids++  = (Tst_int32)(uval >> 1) ^ -(Tst_int32)(uval & 1);
		}

		--samplesTot;
	}

	return ST_ERR_SUCC;
}

/* optimized for: 0 < ricePar <= 8
 *  DOESN'T update CRCs */
static Tst_err
ST_STREAMRD__rdRiceSamples_rpNZ_1_8_NOCRC(Tst_streamrd_intn *pSOI,
		Tst_uint32 samplesTot, const Tst_byte ricePar, Tst_int32 *pResids)
{
	register Tst_byte   byt  = 0;
	Tst_byte            rem;
	register Tst_uint32 uval = 0;  /* unary value */

	while (samplesTot > 0) {
		/* read unary part */
		LOC_RDUNARY_MACRO_NOCRC_(pSOI, byt, uval)

		/* read binary part */

		if (ricePar > pSOI->sbuf.leftoverBits) {
			/* from st_streamrd_rdByte() case1  */
			/** */
			rem = ricePar - pSOI->sbuf.leftoverBits;
			byt = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) << rem;
			ST_STREAMRD__NOCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			pSOI->sbuf.leftoverBits = 8 - rem;
			/** */
			byt |= ((*pSOI->sbuf.pBCur) >> pSOI->sbuf.leftoverBits);
		} else if (ricePar == 8 && pSOI->sbuf.leftoverBits == 8) {
			/* from st_streamrd_rdByte() case2  */
			byt = *pSOI->sbuf.pBCur;
			ST_STREAMRD__NOCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
		} else if (ricePar < pSOI->sbuf.leftoverBits) {
			/* from st_streamrd_rdByte() case3  */
			byt = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) >>
					(pSOI->sbuf.leftoverBits - ricePar);
			pSOI->sbuf.leftoverBits -= ricePar;
		} else {
			/* from st_streamrd_rdByte() case4  */
			/** */
			byt = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]);
			/** */
			ST_STREAMRD__NOCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
		}
		pSOI->totBitsRd_rem += ricePar;
		if (pSOI->totBitsRd_rem > 0xffff)
			ST_STREAMRD__INCREASE_READBITS(pSOI)

		/* compose the value */
		if (pResids != NULL) {
			uval        = (uval << ricePar) | byt;
			*pResids++  = (Tst_int32)(uval >> 1) ^ -(Tst_int32)(uval & 1);
		}

		--samplesTot;
	}

	return ST_ERR_SUCC;
}

/* optimized for: 8 < ricePar <= 32
 *  updates CRCs */
static Tst_err
ST_STREAMRD__rdRiceSamples_rpNZ_9_32_CRC(Tst_streamrd *pSObj,
		Tst_streamrd_intn *pSOI,
		Tst_uint32 samplesTot, const Tst_byte ricePar, Tst_int32 *pResids)
{
	Tst_err             res;
	register Tst_byte   byt  = 0;
	Tst_byte            tmpN;
	register Tst_uint32 uval = 0;  /* unary value */
	Tst_uint32          ui32 = 0;

	while (samplesTot > 0) {
		/* read unary part */
		LOC_RDUNARY_MACRO_CRC_(pSOI, byt, uval)

		/* read binary part */

		if (ricePar - pSOI->sbuf.leftoverBits <= 8) {
			/* from ST_STREAMRD__rdUInt32_case2()  */
			pSOI->totBitsRd_rem += ricePar;
			if (pSOI->totBitsRd_rem > 0xffff)
				ST_STREAMRD__INCREASE_READBITS(pSOI)
			/** */
			tmpN  = ricePar;
			ui32  = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) <<
					(tmpN - pSOI->sbuf.leftoverBits);
			tmpN -= pSOI->sbuf.leftoverBits;
			/** */
			ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			ui32 |= (*pSOI->sbuf.pBCur >> (8 - tmpN));
			/** */
			if (tmpN == 8) {
				ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
			} else
				pSOI->sbuf.leftoverBits = 8 - tmpN;
		} else if (ricePar - pSOI->sbuf.leftoverBits <= 16) {
			/* from ST_STREAMRD__rdUInt32_case3()  */
			pSOI->totBitsRd_rem += ricePar;
			if (pSOI->totBitsRd_rem > 0xffff)
				ST_STREAMRD__INCREASE_READBITS(pSOI)
			/** */
			tmpN = ricePar;
			ui32 = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) <<
					(tmpN - pSOI->sbuf.leftoverBits);
			/** */
			tmpN -= pSOI->sbuf.leftoverBits;
			/** */
			ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			ui32 |= (*pSOI->sbuf.pBCur << (tmpN - 8));
			ST_STREAMRD__UPDCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			ui32 |= (*pSOI->sbuf.pBCur >> (16 - tmpN));
			/** */
			if (tmpN == 16) {
				ST_STREAMRD__UPDCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
			} else
				pSOI->sbuf.leftoverBits = 16 - tmpN;
		} else {
			res = st_streamrd_rdUInt32(pSObj,
					ST_STREAMRD_IEND_BE, ricePar, &ui32);  /* ENDIAN: BE-->HOST */
			if (res != ST_ERR_SUCC)
				return res;
		}

		/* compose the value */
		if (pResids != NULL) {
			uval       = (uval << ricePar) | ui32;
			*pResids++ = (Tst_int32)(uval >> 1) ^ -(Tst_int32)(uval & 1);
		}

		--samplesTot;
	}

	return ST_ERR_SUCC;
}

/* optimized for: 8 < ricePar <= 32
 *  DOESN'T update CRCs */
static Tst_err
ST_STREAMRD__rdRiceSamples_rpNZ_9_32_NOCRC(Tst_streamrd *pSObj,
		Tst_streamrd_intn *pSOI,
		Tst_uint32 samplesTot, const Tst_byte ricePar, Tst_int32 *pResids)
{
	Tst_err             res;
	register Tst_byte   byt  = 0;
	Tst_byte            tmpN;
	register Tst_uint32 uval = 0;  /* unary value */
	Tst_uint32          ui32 = 0;

	while (samplesTot > 0) {
		/* read unary part */
		LOC_RDUNARY_MACRO_NOCRC_(pSOI, byt, uval)

		/* read binary part */

		if (ricePar - pSOI->sbuf.leftoverBits <= 8) {
			/* from ST_STREAMRD__rdUInt32_case2()  */
			pSOI->totBitsRd_rem += ricePar;
			if (pSOI->totBitsRd_rem > 0xffff)
				ST_STREAMRD__INCREASE_READBITS(pSOI)
			/** */
			tmpN  = ricePar;
			ui32  = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) <<
					(tmpN - pSOI->sbuf.leftoverBits);
			tmpN -= pSOI->sbuf.leftoverBits;
			/** */
			ST_STREAMRD__NOCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			ui32 |= (*pSOI->sbuf.pBCur >> (8 - tmpN));
			/** */
			if (tmpN == 8) {
				ST_STREAMRD__NOCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
			} else
				pSOI->sbuf.leftoverBits = 8 - tmpN;
		} else if (ricePar - pSOI->sbuf.leftoverBits <= 16) {
			/* from ST_STREAMRD__rdUInt32_case3()  */
			pSOI->totBitsRd_rem += ricePar;
			if (pSOI->totBitsRd_rem > 0xffff)
				ST_STREAMRD__INCREASE_READBITS(pSOI)
			/** */
			tmpN = ricePar;
			ui32 = (*pSOI->sbuf.pBCur &
					ST_STREAM_LOBITS_MASK_8[pSOI->sbuf.leftoverBits]) <<
					(tmpN - pSOI->sbuf.leftoverBits);
			/** */
			tmpN -= pSOI->sbuf.leftoverBits;
			/** */
			ST_STREAMRD__NOCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			ui32 |= (*pSOI->sbuf.pBCur << (tmpN - 8));
			ST_STREAMRD__NOCRC_INCPOS_RET(pSOI)  /* ret ST_ERR_FEOF */
			ui32 |= (*pSOI->sbuf.pBCur >> (16 - tmpN));
			/** */
			if (tmpN == 16) {
				ST_STREAMRD__NOCRC_INCPOS_NORET(pSOI)  /* doesn't ret ST_ERR_FEOF */
			} else
				pSOI->sbuf.leftoverBits = 16 - tmpN;
		} else {
			res = st_streamrd_rdUInt32(pSObj,
					ST_STREAMRD_IEND_BE, ricePar, &ui32);  /* ENDIAN: BE-->HOST */
			if (res != ST_ERR_SUCC)
				return res;
		}

		/* compose the value */
		if (pResids != NULL) {
			uval       = (uval << ricePar) | ui32;
			*pResids++ = (Tst_int32)(uval >> 1) ^ -(Tst_int32)(uval & 1);
		}

		--samplesTot;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_STREAMRD_DEB_ == 1)
static void ST_STREAMRD__prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

static void ST_STREAMRD__prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}

static void ST_STREAMRD__prBitsInByte(const Tst_byte byt)
{
	Tst_byte x;

	for (x = 0; x < 8; x++)
		ST_STREAMRD__prf("%d", (byt >> (8 - 1 - x)) & 0x01);
}
#endif

/******************************************************************************/
