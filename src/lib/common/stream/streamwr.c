/******************************************************************************/
/* streamwr.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for the Stream Writer                                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/streamwr.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_math.h"
/** */
#define SRC_STREAMWR_C
#include "streamwr-priv.h"
/*** */
#include "stream_crcs-prot.h"
#include "stream_stcs-prot.h"
#undef SRC_STREAMWR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */
#if (ST_STREAMWR_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set output destination to the internal buffer
 */
Tst_err
st_streamwr_setOutput_buffer(Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(pSObj == NULL || pSObj->pObInternal == NULL)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;

	st_streamwr_stc_rsetSObj(pSObj);
	/*st_stream_stc_rsetBuf(&pSOI->sbuf, ST_B_FALSE);*/

	/* alloc buffer */
	if (pSOI->sbuf.intnBufSz == 0) {
		pSOI->sbuf.intnBufSz = ST_STREAMWR_DEFBUFSZ;
		ST_CALLOC(pSOI->sbuf.pIntnBuf, Tst_buf*, pSOI->sbuf.intnBufSz, 1)
		if (pSOI->sbuf.pIntnBuf == NULL)
			return ST_ERR_OMEM;
		pSOI->sbuf.sz        = pSOI->sbuf.intnBufSz;
		pSOI->sbuf.wasAllocd = ST_B_TRUE;
	}
	memset(pSOI->sbuf.pIntnBuf, 0, pSOI->sbuf.intnBufSz);
	/* */
	pSOI->sbuf.pBBeg        = pSOI->sbuf.pIntnBuf;
	pSOI->sbuf.pBCur        = pSOI->sbuf.pBBeg;
	pSOI->sbuf.pBEnd        = pSOI->sbuf.pBBeg + pSOI->sbuf.sz;
	pSOI->sbuf.leftoverBits = 8;

	return ST_ERR_SUCC;
}

/*
 * Set output destination to pFStc
 */
Tst_err
st_streamwr_setOutput_file(Tst_streamwr *pSObj, Tst_sys_fstc *pFStc)
{
	Tst_err res;
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(pFStc == NULL || ! st_sysFStc_isWritable(pFStc))

	/* init common stuff (common to file and buffer input) */
	res = st_streamwr_setOutput_buffer(pSObj);
	if (res != ST_ERR_SUCC)
		return res;

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* init file specific stuff */
	pSOI->sfil.pFStc = pFStc;
	pSOI->sfil.cpos  = st_sysFStc_getFPos(pFStc);
	pSOI->sfil.fsz   = st_sysFStc_getFileSz(pFStc, NULL);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Get current position
 */
Tst_foffs
st_streamwr_getCurPos(const Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_NUM(-1, ! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc == NULL)
		return (pSOI->sbuf.pBCur >= pSOI->sbuf.pBBeg ?
					(Tst_foffs)(pSOI->sbuf.pBCur - pSOI->sbuf.pBBeg) :
					-1);

	return (pSOI->sbuf.pBCur >= pSOI->sbuf.pBBeg ?
				pSOI->sfil.cpos +
					(Tst_foffs)(pSOI->sbuf.pBCur - pSOI->sbuf.pBBeg) :
				-1);
}

/*
 * Get current size of the output buffer
 */
Tst_uint32
st_streamwr_getOutputBufSize(const Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_NUM(0, ! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc != NULL)
		return 0;
	return (pSOI->sbuf.pBCur - pSOI->sbuf.pBBeg) +
			(pSOI->sbuf.leftoverBits > 0 && pSOI->sbuf.leftoverBits < 8 ?
				1 : 0);
}

/*
 * Get current size of the output file
 */
Tst_fsize
st_streamwr_getOutputFileSize(const Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_NUM(0, ! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc == NULL)
		return 0;
	return pSOI->sfil.fsz +
			(pSOI->sbuf.pBCur - pSOI->sbuf.pBBeg) +
			(pSOI->sbuf.leftoverBits > 0 && pSOI->sbuf.leftoverBits < 8 ?
				1 : 0);
}

/*
 * Returns the amount of remaining bits in the current byte
 */
Tst_byte
st_streamwr_getAmountRemainingBitsInCurByte(const Tst_streamwr *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return ((Tst_streamwr_intn*)pSObj->pObInternal)->sbuf.leftoverBits;
}

/*
 * Get amount of written bits
 */
void
st_streamwr_getAmountBitsWritten(const Tst_streamwr *pSObj,
		Tst_fsize *pBytes, Tst_byte *pBitsMod8)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBytes == NULL || pBitsMod8 == NULL)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	*pBytes    = pSOI->totBytesWr +
			((pSOI->totBitsWr_rem - (pSOI->totBitsWr_rem % 8)) / 8);
	*pBitsMod8 = (Tst_byte)(pSOI->totBitsWr_rem % 8);
}

/*
 * Get amount of written bytes, not counting partially written byte
 */
Tst_fsize
st_streamwr_getAmountBytesWritten(const Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	return pSOI->totBytesWr +
			((pSOI->totBitsWr_rem - (pSOI->totBitsWr_rem % 8)) / 8);
}

/*----------------------------------------------------------------------------*/

/*
 * Get current output buffer
 */
const Tst_buf*
st_streamwr_getOutputBuffer(const Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_NULL(pSObj == NULL || pSObj->pObInternal == NULL)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc != NULL)
		return NULL;
	return pSOI->sbuf.pBBeg;
}

/*----------------------------------------------------------------------------*/

/*
 * If output is a file: write all remaining data to output
 * If output is a buffer: reset buffer
 */
Tst_err
st_streamwr_flush(Tst_streamwr *pSObj)
{
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj))

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sfil.pFStc == NULL) {
		pSOI->sbuf.pBCur        = pSOI->sbuf.pBBeg;
		pSOI->sbuf.leftoverBits = 8;
		memset(pSOI->sbuf.pBBeg, 0, pSOI->sbuf.pBEnd - pSOI->sbuf.pBBeg);
	} else if (pSOI->sbuf.pBCur != NULL &&
			(pSOI->sbuf.pBCur > pSOI->sbuf.pBBeg ||
				pSOI->sbuf.leftoverBits < 8)) {
		return ST_STREAMWR__flushOutpFile(pSOI);
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Set start-point for CRC-8 computation
 */
void
st_streamwr_startCRC8(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_startCRC8(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Pause or continue computation
 */
void
st_streamwr_pauseOrContinueCRC8(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_pauseOrContinueCRC8(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Update CRC with data in pBuf
 */
void
st_streamwr_updateCRC8(Tst_streamwr *pSObj,
		const Tst_buf *pBuf, const Tst_uint32 sz)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBuf == NULL)

	st_stream_updateCRC8(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs,
			pBuf, sz);
}

/*
 * Get current CRC-8 checksum
 */
Tst_byte
st_streamwr_getCRC8(const Tst_streamwr *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return st_stream_getCRC8(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Stop CRC-8 computation
 */
void
st_streamwr_stopCRC8(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_stopCRC8(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Set start-point for CRC-16 computation
 */
void
st_streamwr_startCRC16(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_startCRC16(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Pause or continue computation
 */
void
st_streamwr_pauseOrContinueCRC16(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_pauseOrContinueCRC16(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Update CRC with data in pBuf
 */
void
st_streamwr_updateCRC16(Tst_streamwr *pSObj,
		const Tst_buf *pBuf, const Tst_uint32 sz)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBuf == NULL)

	st_stream_updateCRC16(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs,
			pBuf, sz);
}

/*
 * Get current CRC-16 checksum
 */
Tst_uint16
st_streamwr_getCRC16(const Tst_streamwr *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return st_stream_getCRC16(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Stop CRC-16 computation
 */
void
st_streamwr_stopCRC16(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_stopCRC16(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Set start-point for CRC-32 computation
 */
void
st_streamwr_startCRC32(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_startCRC32(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Pause or continue computation
 */
void
st_streamwr_pauseOrContinueCRC32(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_pauseOrContinueCRC32(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Update CRC with data in pBuf
 */
void
st_streamwr_updateCRC32(Tst_streamwr *pSObj,
		const Tst_buf *pBuf, const Tst_uint32 sz)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL ||
			pBuf == NULL)

	st_stream_updateCRC32(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs,
			pBuf, sz);
}

/*
 * Get current CRC-32 checksum
 */
Tst_uint32
st_streamwr_getCRC32(const Tst_streamwr *pSObj)
{
	ST_ASSERTN_NUM(0, pSObj == NULL || pSObj->pObInternal == NULL)

	return st_stream_getCRC32(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*
 * Stop CRC-32 computation
 */
void
st_streamwr_stopCRC32(Tst_streamwr *pSObj)
{
	ST_ASSERTN_VOID(pSObj == NULL || pSObj->pObInternal == NULL)

	st_stream_stopCRC32(&((Tst_streamwr_intn*)pSObj->pObInternal)->crcs);
}

/*----------------------------------------------------------------------------*/

/*
 * Write nBits bits from val to output stream
 */
/** updates CRCs */
Tst_err
st_streamwr_wrByte(Tst_streamwr *pSObj,
		const Tst_byte nBits, Tst_byte val)
{
	Tst_err  res;
	Tst_byte rem;
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || nBits > 8)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (nBits > pSOI->sbuf.leftoverBits) {
		/* case1 (nBits>loBits) */
		val &= ST_STREAM_LOBITS_MASK_8[nBits];
		/* *pBCur=xxxxx000, val=0abcdefg, nBits=7, loB=3
		 *   *pBCur |= 00000abc
		 *   ++pBCur
		 *   *pBCur = defg0000  */
		rem                = nBits - pSOI->sbuf.leftoverBits;
		*pSOI->sbuf.pBCur |= (val >> rem);
		ST_STREAMWR__UPDCRC_INCPOS_RET(res, pSOI)
		*pSOI->sbuf.pBCur = (val << (8 - rem));
		/* */
		pSOI->sbuf.leftoverBits = 8 - rem;
	} else if (nBits == 8 && pSOI->sbuf.leftoverBits == 8) {
		/* case2 (nBits==loBits==8) */
		*pSOI->sbuf.pBCur = val;
		ST_STREAMWR__UPDCRC_INCPOS_RET(res, pSOI)
	} else if (nBits < pSOI->sbuf.leftoverBits) {
		/* case3 (nBits<loBits) */
		val &= ST_STREAM_LOBITS_MASK_8[nBits];
		/* *pBCur=xx000000, val=000abcde, nBits=5, loB=6
		 *   *pBCur |= 00abcde0  */
		*pSOI->sbuf.pBCur |= (val << (pSOI->sbuf.leftoverBits - nBits));
		/* */
		pSOI->sbuf.leftoverBits -= nBits;
	} else {
		/* case4 ((nBits==loBits)!=8) */
		*pSOI->sbuf.pBCur |= (val & ST_STREAM_LOBITS_MASK_8[nBits]);
		ST_STREAMWR__UPDCRC_INCPOS_RET(res, pSOI)
		/* */
		pSOI->sbuf.leftoverBits = 8;
	}

	/* */
	pSOI->totBitsWr_rem += nBits;
	if (pSOI->totBitsWr_rem > 0xffff)
		ST_STREAMWR__INCREASE_WRITBITS(pSOI)

	return ST_ERR_SUCC;
}

/** DOESN'T update CRCs */
Tst_err
st_streamwr_wrByte_noCRC(Tst_streamwr *pSObj,
		const Tst_byte nBits, Tst_byte val)
{
	Tst_err  res;
	Tst_byte rem;
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || nBits > 8)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (nBits > pSOI->sbuf.leftoverBits) {
		/* case1 (nBits>loBits) */
		val &= ST_STREAM_LOBITS_MASK_8[nBits];
		/* *pBCur=xxxxx000, val=0abcdefg, nBits=7, loB=3
		 *   *pBCur |= 00000abc
		 *   ++pBCur
		 *   *pBCur = defg0000  */
		rem                = nBits - pSOI->sbuf.leftoverBits;
		*pSOI->sbuf.pBCur |= (val >> rem);
		ST_STREAMWR__NOCRC_INCPOS_RET(res, pSOI)
		*pSOI->sbuf.pBCur = (val << (8 - rem));
		/* */
		pSOI->sbuf.leftoverBits = 8 - rem;
	} else if (nBits == 8 && pSOI->sbuf.leftoverBits == 8) {
		/* case2 (nBits==loBits==8) */
		*pSOI->sbuf.pBCur = val;
		ST_STREAMWR__NOCRC_INCPOS_RET(res, pSOI)
	} else if (nBits < pSOI->sbuf.leftoverBits) {
		/* case3 (nBits<loBits) */
		val &= ST_STREAM_LOBITS_MASK_8[nBits];
		/* *pBCur=xx000000, val=000abcde, nBits=5, loB=6
		 *   *pBCur |= 00abcde0  */
		*pSOI->sbuf.pBCur |= (val << (pSOI->sbuf.leftoverBits - nBits));
		/* */
		pSOI->sbuf.leftoverBits -= nBits;
	} else {
		/* case4 ((nBits==loBits)!=8) */
		*pSOI->sbuf.pBCur |= (val & ST_STREAM_LOBITS_MASK_8[nBits]);
		ST_STREAMWR__NOCRC_INCPOS_RET(res, pSOI)
		/* */
		pSOI->sbuf.leftoverBits = 8;
	}

	/* */
	pSOI->totBitsWr_rem += nBits;
	if (pSOI->totBitsWr_rem > 0xffff)
		ST_STREAMWR__INCREASE_WRITBITS(pSOI)

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Write nBits bits from val to output stream
 */
Tst_err
st_streamwr_wrUInt16(Tst_streamwr *pSObj, const Tst_streamwr_endian outpEndian,
		const Tst_byte nBits, const Tst_uint16 val)
{
	ST_ASSERTN_IARG(nBits > 16)

	return st_streamwr_wrUInt32(pSObj, outpEndian, nBits, (Tst_uint32)val);
}

/*
 * Write nBits bits from val to output stream
 */
Tst_err
st_streamwr_wrInt16(Tst_streamwr *pSObj, const Tst_streamwr_endian outpEndian,
		const Tst_byte nBits, const Tst_int16 val)
{
	ST_ASSERTN_IARG(nBits > 16)

	return st_streamwr_wrUInt32(pSObj, outpEndian, nBits, (Tst_uint32)val);
}

/*----------------------------------------------------------------------------*/

/*
 * Write nBits bits from val to output stream
 */
Tst_err
st_streamwr_wrUInt32(Tst_streamwr *pSObj, const Tst_streamwr_endian outpEndian,
		Tst_byte nBits, Tst_uint32 val)
{
	Tst_err res = ST_ERR_SUCC;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || nBits > 32 ||
			(outpEndian == ST_STREAMWR_OEND_LE && (nBits % 8) != 0))

	if (outpEndian == ST_STREAMWR_OEND_LE) {
		/* ENDIAN: swap byte order, LE-->BE */
		ST_STREAM_SWAP_BO_UI32(&val)
		val >>= (((32 - nBits) / 8) * 8);
	}

	if (nBits > 24) {
		res   = st_streamwr_wrByte(pSObj, nBits - 24, (val >> 24) & 0xff);
		nBits = 24;
	}
	if (nBits > 16 && res == ST_ERR_SUCC) {
		res   = st_streamwr_wrByte(pSObj, nBits - 16, (val >> 16) & 0xff);
		nBits = 16;
	}
	if (nBits > 8 && res == ST_ERR_SUCC) {
		res   = st_streamwr_wrByte(pSObj, nBits - 8, (val >> 8) & 0xff);
		nBits = 8;
	}
	if (nBits > 0 && res == ST_ERR_SUCC)
		res = st_streamwr_wrByte(pSObj, nBits, val & 0xff);
	return res;
}

/*
 * Write nBits bits from val to output stream
 */
Tst_err
st_streamwr_wrInt32(Tst_streamwr *pSObj, const Tst_streamwr_endian outpEndian,
		const Tst_byte nBits, const Tst_int32 val)
{
	return st_streamwr_wrUInt32(pSObj, outpEndian, nBits, (Tst_uint32)val);
}

/*----------------------------------------------------------------------------*/

/*
 * Write nBits bits from val to output stream
 */
Tst_err
st_streamwr_wrUInt64(Tst_streamwr *pSObj, const Tst_streamwr_endian outpEndian,
		Tst_byte nBits, const Tst_uint64 *pVal)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint64 wrVal;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || nBits > 64 || pVal == NULL ||
			(outpEndian == ST_STREAMWR_OEND_LE && (nBits % 8) != 0))

	st_sysUInt64_setUI64(&wrVal, pVal);
	if (outpEndian == ST_STREAMWR_OEND_LE) {
		/* ENDIAN: swap byte order, LE-->BE */
		st_sysUInt64_reverseByteOrder(&wrVal);
		if (((64 - nBits) / 8) * 8 > 0)
			st_sysUInt64_shiftRight(&wrVal, ((64 - nBits) / 8) * 8);
	}

	if (nBits > 56) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits - 56, (wrVal.nativeU64 >> 56) & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits - 56, (wrVal.hiU >> 24) & 0xff);
#		endif
		nBits = 56;
	}
	if (nBits > 48) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits - 48, (wrVal.nativeU64 >> 48) & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits - 48, (wrVal.hiU >> 16) & 0xff);
#		endif
		nBits = 48;
	}
	if (nBits > 40) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits - 40, (wrVal.nativeU64 >> 40) & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits - 40, (wrVal.hiU >> 8) & 0xff);
#		endif
		nBits = 40;
	}
	if (nBits > 32) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits - 32, (wrVal.nativeU64 >> 32) & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits - 32, wrVal.hiU & 0xff);
#		endif
		nBits = 32;
	}

	if (nBits > 24) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits - 24, (wrVal.nativeU64 >> 24) & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits - 24, (wrVal.loU >> 24) & 0xff);
#		endif
		nBits = 24;
	}
	if (nBits > 16 && res == ST_ERR_SUCC) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits - 16, (wrVal.nativeU64 >> 16) & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits - 16, (wrVal.loU >> 16) & 0xff);
#		endif
		nBits = 16;
	}
	if (nBits > 8 && res == ST_ERR_SUCC) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits - 8, (wrVal.nativeU64 >> 8) & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits - 8, (wrVal.loU >> 8) & 0xff);
#		endif
		nBits = 8;
	}
	if (nBits > 0 && res == ST_ERR_SUCC) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		res = st_streamwr_wrByte(pSObj,
				nBits, wrVal.nativeU64 & 0xff);
#		else
		res = st_streamwr_wrByte(pSObj,
				nBits, wrVal.loU & 0xff);
#		endif
	}
	return res;
}

/*
 * Write nBits bits from val to output stream
 */
Tst_err
st_streamwr_wrInt64(Tst_streamwr *pSObj, const Tst_streamwr_endian outpEndian,
		const Tst_byte nBits, const Tst_int64 *pVal)
{
	Tst_uint64 valUI;

	ST_ASSERTN_IARG(pVal == NULL)

	st_sysSInt64_toUI64(pVal, &valUI);
	return st_streamwr_wrUInt64(pSObj, outpEndian, nBits, &valUI);
}

/*----------------------------------------------------------------------------*/

/*
 * Write bufSz bytes from pBuf to output stream
 */
/** updates CRCs */
Tst_err
st_streamwr_wrBuffer(Tst_streamwr *pSObj,
		const Tst_uint32 bufSz, const Tst_buf *pBuf)
{
	return ST_STREAMWR__wrBuffer_pr(pSObj, bufSz, pBuf, ST_B_TRUE);
}

/** DOESN'T update CRCs */
Tst_err
st_streamwr_wrBuffer_noCRC(Tst_streamwr *pSObj,
		const Tst_uint32 bufSz, const Tst_buf *pBuf)
{
	return ST_STREAMWR__wrBuffer_pr(pSObj, bufSz, pBuf, ST_B_FALSE);
}

/*----------------------------------------------------------------------------*/

/*
 * Write unary-encoded unsigned int
 *   (e.g. the decimal value 10 would be encoded as binary 00000000001,
 *         -------- " ------  0 ---------- " ------------- 1,
 *         read from left to right)
 */
Tst_err
st_streamwr_wrUnary(Tst_streamwr *pSObj, Tst_uint32 val)
{
	Tst_err res = ST_ERR_SUCC;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj))

	while (val > 7 && res == ST_ERR_SUCC) {
		res  = st_streamwr_wrByte(pSObj, 8, 0);
		val -= 8;
	}
	if (res == ST_ERR_SUCC)
		return st_streamwr_wrByte(pSObj, val + 1, 1);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Write NUL-terminated string with at most maxBytes bytes
 *   If maxBytes==0 then there is no limit
 */
Tst_err
st_streamwr_wrString(Tst_streamwr *pSObj,
		const Tst_uint32 maxBytes, const Tst_str *pStr)
{
	Tst_err       res = ST_ERR_SUCC;
	Tst_str const *pS = pStr;
	Tst_uint32    wr  = 0;
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pStr == NULL)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;

	if (pSOI->sbuf.leftoverBits != 0 && pSOI->sbuf.leftoverBits != 8)
		return ST_ERR_FAIL;

	while ((maxBytes == 0 || wr < maxBytes) && *pS && res == ST_ERR_SUCC) {
		res = st_streamwr_wrByte(pSObj, 8, *pS++);
		++wr;
	}

	if ((maxBytes == 0 || wr < maxBytes) && res == ST_ERR_SUCC)
		return st_streamwr_wrByte(pSObj, 8, 0);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Flush data in internal buffer to output file
 */
static Tst_err
ST_STREAMWR__flushOutpFile(Tst_streamwr_intn *pSOI)
{
	Tst_err    res;
	Tst_uint32 osz;

	osz = pSOI->sbuf.pBCur - pSOI->sbuf.pBBeg;
	if (osz > 0) {
		res = st_sysFStc_writeBuf(pSOI->sfil.pFStc,
				pSOI->sbuf.pBBeg, osz);
		if (res != ST_ERR_SUCC)
			return res;

		pSOI->sfil.cpos += osz;
		pSOI->sfil.fsz  += osz;
	}

	pSOI->sbuf.pBCur = pSOI->sbuf.pBBeg;
	memset(pSOI->sbuf.pBBeg, 0, pSOI->sbuf.pBEnd - pSOI->sbuf.pBBeg);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_STREAMWR__wrBuffer_pr(Tst_streamwr *pSObj,
		const Tst_uint32 bufSz, const Tst_buf *pBuf,
		const Tst_bool withCRCs)
{
	Tst_err       res;
	Tst_buf const *pIB    = pBuf,
	              *pIBEnd = pBuf + bufSz;
	Tst_streamwr_intn *pSOI;

	ST_ASSERTN_IARG(! ST_STREAM_CHECK(pSObj) || pBuf == NULL)

	pSOI = (Tst_streamwr_intn*)pSObj->pObInternal;
	/* */
	if (pSOI->sbuf.leftoverBits != 0 && pSOI->sbuf.leftoverBits != 8)
		return ST_ERR_FAIL;
	/* */
	if (withCRCs) {
		if (pSOI->crcs.compCRC8) {
			while (pIB < pIBEnd)
				ST_STREAM_UPDATE_CRC8(pSOI->crcs, *pIB++)
			pIB = pBuf;
		}
		if (pSOI->crcs.compCRC16) {
			while (pIB < pIBEnd)
				ST_STREAM_UPDATE_CRC16(pSOI->crcs, *pIB++)
			pIB = pBuf;
		}
		if (pSOI->crcs.compCRC32) {
			while (pIB < pIBEnd)
				ST_STREAM_UPDATE_CRC32(pSOI->crcs, *pIB++)
			pIB = pBuf;
		}
	}
	/* */
	if (pSOI->sfil.pFStc == NULL) {
		Tst_uint32 nbsz = (pSOI->sbuf.pBCur - pSOI->sbuf.pBBeg) + bufSz;

		if (nbsz > pSOI->sbuf.intnBufSz) {
			nbsz += (nbsz % ST_STREAMWR_DEFBUFSZ);
			/* */
			res = st_stream_stc_reszIntnBuf(&pSOI->sbuf, nbsz, 0);
			if (res != ST_ERR_SUCC)
				return res;
		}
		memcpy(pSOI->sbuf.pBCur, pBuf, bufSz);
		pSOI->sbuf.pBCur += bufSz;
	} else {
		Tst_uint32 toWr;

		while (pIB < pIBEnd) {
			if (pSOI->sbuf.pBCur + 1 >= pSOI->sbuf.pBEnd) {
				res = ST_STREAMWR__flushOutpFile(pSOI);
				if (res != ST_ERR_SUCC)
					return res;
			}
			toWr = pSOI->sbuf.pBEnd - pSOI->sbuf.pBCur;
			if (toWr > (Tst_uint32)(pIBEnd - pIB))
				toWr = pIBEnd - pIB;
			memcpy(pSOI->sbuf.pBCur, pIB, toWr);
			pIB              += toWr;
			pSOI->sbuf.pBCur += toWr;
		}
		if (pSOI->sbuf.pBCur + 1 >= pSOI->sbuf.pBEnd) {
			res = ST_STREAMWR__flushOutpFile(pSOI);
			if (res != ST_ERR_SUCC)
				return res;
		}
	}
	/* */
	pSOI->totBitsWr_rem += (bufSz * 8);
	if (pSOI->totBitsWr_rem > 0xffff)
		ST_STREAMWR__INCREASE_WRITBITS(pSOI)

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_STREAMWR_DEB_ == 1)
static void ST_STREAMWR__prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

static void ST_STREAMWR__prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}

static void ST_STREAMWR__prBitsInByte(const Tst_byte byt)
{
	Tst_byte x;

	for (x = 0; x < 8; x++)
		ST_STREAMWR__prf("%d", (byt >> (8 - 1 - x)) & 0x01);
}
#endif

/******************************************************************************/
