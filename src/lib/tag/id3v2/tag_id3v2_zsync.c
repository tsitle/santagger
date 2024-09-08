/******************************************************************************/
/* tag_id3v2_zsync.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Sub functions for Unsynchronization & Co.                                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
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
/** */
#define SRC_TAG_ID3V2_ZSYNC_C
#include "tag_id3v2_zsync-prot.h"
/*** */
#include "tag_id3v2-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_ZSYNC_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy(), ... */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__sync_reverseUnsync(ST_OPTARG(Tst_bool *pFalseSyncsFnd),
                            Tst_buf *pBuf, const Tst_uint32 bufSz,
                            Tst_uint32 *pDelta);
static Tst_err
ST_TIV2__sync_doUnsync(const Tst_buf *pBufIn, const Tst_uint32 bufSzIn,
                       Tst_buf **ppBuf, Tst_uint32 *pBufSz,
                       Tst_uint32 *pDelta);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Convert SafeSync-Interger to unsigned int
 *   Range for a SafeSync-Int is 0 <= x < 256MB
 *   cvInt must be in BigEndian-order
 */
Tst_uint32
st_id3v2_sync_syncsafe2uint(Tst_uint32 cvInt)
{
	if (cvInt == 0)
		return 0;

	/**st_id3v2_prf("%s:%d> syncsafe2uint inp 0x%08x (0x%02x %02x %02x %02x)\n",
			__FILE__, __LINE__, cvInt,
			((Tst_buf*)&cvInt)[0], ((Tst_buf*)&cvInt)[1],
			((Tst_buf*)&cvInt)[2], ((Tst_buf*)&cvInt)[3]);**/

	cvInt = ((cvInt & 0x0000007f) |        /* bits  6-0 */
			((cvInt & 0x00007f00) >> 1) |  /* bits 13-0 */
			((cvInt & 0x007f0000) >> 2) |  /* bits 20-0 */
			((cvInt & 0x7f000000) >> 3));  /* bits 27-0 */

	/**st_id3v2_prf("%s:%d> syncsafe2uint out 0x%08x (0x%02x %02x %02x %02x)\n",
			__FILE__, __LINE__, cvInt,
			((Tst_buf*)&cvInt)[0], ((Tst_buf*)&cvInt)[1],
			((Tst_buf*)&cvInt)[2], ((Tst_buf*)&cvInt)[3]);**/
	return cvInt;
}

Tst_uint32
st_id3v2_sync_uint2syncsafe(Tst_uint32 cvInt)
{
	Tst_buf buf[4];

	if (cvInt == 0 || cvInt > ST_ID3V2_MAX_SAFEINT) {
		/**if (cvInt > ST_ID3V2_MAX_SAFEINT)
			st_id3v2_prf("! cvInt %u byt, %.2f mb, 0x%08x\n", cvInt,
					(float)cvInt / 1024.0 / 1024.0, cvInt);**/
		return 0;
	}

	/* buf is big-endian */
	buf[0] = (cvInt >> 21) & 0x7f;
	buf[1] = (cvInt >> 14) & 0x7f;
	buf[2] = (cvInt >>  7) & 0x7f;
	buf[3] = (cvInt >>  0) & 0x7f;
	memcpy(&cvInt, &buf, 4);

	/**st_id3v2_prf("%s:%d> uint2syncsafe out BE 0x%08x (0x%02x %02x %02x %02x)\n",
			__FILE__, __LINE__, cvInt,
			((Tst_buf*)&cvInt)[0], ((Tst_buf*)&cvInt)[1],
			((Tst_buf*)&cvInt)[2], ((Tst_buf*)&cvInt)[3]);**/

	return cvInt;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Reverse Unsynchronization
 *   read input from pStrrd and write output to pDataOut
 *
 * pFalseSyncsFnd  may be NULL
 * pStrrd          MUST be set
 * pDataOut        MUST be set
 * pDelta          MUST be != NULL
 *
 * returns: ERRCODE
 *          pDelta will be set to the amount of removed bytes
 */
Tst_err
st_id3v2_sync_reverseUnsync_stream2bo(ST_OPTARG(Tst_bool *pFalseSyncsFnd),
		Tst_streamrd *pStrrd, const Tst_uint32 dataLen,
		Tst_binobj *pDataOut, Tst_uint32 *pDelta)
{
	Tst_err    res       = ST_ERR_SUCC;
	Tst_buf    buf[2048 + 1];
	Tst_uint32 toRd      = sizeof(buf) - 1,
	           rdTot     = 0,
	           rdFromStr = 0,
	           deltaOne  = 0;
	Tst_bool   fsfndOne  = ST_B_FALSE;

	ST_ASSERTN_IARG(pStrrd == NULL || pDelta == NULL || pDataOut == NULL)

	if (pFalseSyncsFnd != NULL)
		*pFalseSyncsFnd = ST_B_FALSE;
	*pDelta = 0;
	st_binobj_stc_rsetData(pDataOut);

	while (rdTot < dataLen && ! st_streamrd_isEOF(pStrrd)) {
		if (rdTot + toRd > dataLen)
			toRd = dataLen - rdTot;
		res = st_streamrd_rdBuffer_noCRC(pStrrd, toRd, buf, &rdFromStr);
		if (res != ST_ERR_SUCC)
			break;
		rdTot += rdFromStr;
		/* */
		if (rdFromStr > 0 && rdTot + 1 <= dataLen &&
				buf[rdFromStr - 1] == 0xff &&
				! st_streamrd_isEOF(pStrrd)) {
			st_streamrd_rdByte_noCRC(pStrrd, 8, &buf[rdFromStr]);
			++rdFromStr;
			++rdTot;
		}
		/* */
		res = ST_TIV2__sync_reverseUnsync(
				(pFalseSyncsFnd != NULL ? &fsfndOne : NULL),
				buf, rdFromStr,
				&deltaOne);
		if (res != ST_ERR_SUCC)
			break;
		if (pFalseSyncsFnd != NULL)
			*pFalseSyncsFnd |= fsfndOne;
		*pDelta += deltaOne;
		/* */
		ST_ASSERTN_NUM(ST_ERR_FAIL, rdFromStr <= deltaOne)
		res = st_binobj_appendData(pDataOut, buf, rdFromStr - deltaOne);
		if (res != ST_ERR_SUCC)
			break;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Do Unsynchronization
 *   read input from pDataIn and write to pDataOut
 *
 * pDataIn    MUST be set
 * pDataOut   MUST be set
 * pDelta     MUST be set
 *
 * returns: ERRCODE
 *          pDelta will be set to the amount of added bytes
 */
Tst_err
st_id3v2_sync_doUnsync_bo2bo(Tst_binobj *pDataIn, Tst_binobj *pDataOut,
		Tst_uint32 *pDelta)
{
	Tst_err    res       = ST_ERR_SUCC;
	Tst_buf    bufIn[2048 + 1],
	           *pBufOut  = NULL;
	Tst_uint32 toRd      = sizeof(bufIn) - 1,
	           rdTot     = 0,
	           rdFromStr = 0,
	           inpLen,
	           deltaOne  = 0,
	           bufOutSz  = 0;
	Tst_streamrd strrd;

	ST_ASSERTN_IARG(pDataIn == NULL || pDataOut == NULL || pDelta == NULL)

	*pDelta = 0;
	st_binobj_stc_rsetData(pDataOut);

	st_streamrd_stc_initSObj(&strrd);
	res = st_binobj_attachStreamrd(pDataIn, &strrd, 0);
	if (res != ST_ERR_SUCC) {
		st_streamrd_stc_freeSObj(&strrd);
		return res;
	}
	inpLen = st_binobj_getDataSize(pDataIn);

	while (! st_streamrd_isEOF(&strrd)) {
		if (rdTot + toRd > inpLen)
			toRd = inpLen - rdTot;
		res = st_streamrd_rdBuffer_noCRC(&strrd, toRd, bufIn, &rdFromStr);
		if (res != ST_ERR_SUCC)
			break;
		rdTot += rdFromStr;
		/* */
		if (rdFromStr  > 0 && rdTot + 1 <= inpLen &&
				bufIn[rdFromStr - 1] == 0xff &&
				! st_streamrd_isEOF(&strrd)) {
			st_streamrd_rdByte_noCRC(&strrd, 8, &bufIn[rdFromStr]);
			++rdFromStr;
			++rdTot;
		}
		/* */
		res = ST_TIV2__sync_doUnsync(bufIn, rdFromStr,
				&pBufOut, &bufOutSz,
				&deltaOne);
		if (res != ST_ERR_SUCC)
			break;
		*pDelta += deltaOne;
		/* */
		ST_ASSERTN_NUM(ST_ERR_FAIL, rdFromStr + deltaOne > bufOutSz)
		res = st_binobj_appendData(pDataOut, pBufOut, rdFromStr + deltaOne);
		if (res != ST_ERR_SUCC)
			break;
	}

	ST_DELPOINT(pBufOut)
	st_binobj_detachStreamrd(pDataIn);
	st_streamrd_stc_freeSObj(&strrd);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Check whether pBuf contains false sync bytes
 *
 * pBuf     MUST be != NULL
 * pFSBcnt  may be NULL
 *
 * returns: true if pBuf contains false sync bytes
 */
Tst_bool
st_id3v2_sync_hasFalseSync(const Tst_bool fastOrThourough,
		Tst_buf *pBuf, const Tst_uint32 bufSz, Tst_uint32 *pFSBcnt)
{
	Tst_uint32 x,
	           cnt = 0;
	Tst_byte   c1,
	           c2;

	if (pBuf == NULL || bufSz == 0)
		return ST_B_FALSE;
	if (memchr(pBuf, 0xff, bufSz) == NULL)
		return ST_B_FALSE;

	for (x = 0; x + 1 < bufSz; x++) {
		c1 = pBuf[x];
		c2 = pBuf[x + 1];
		if (c1 == 0xff && (c2 & 0xe0) == 0xe0) {
			++cnt;
			/**st_id3v2_prf("    hasFalseSync: "
					"false sync bytes 0x%02x%02x  (pos %u)\n",
					c1, c2, x);**/
			if (fastOrThourough)
				break;
			/*++x;  no, we want them all*/
		} else if (c2 != 0xff)
			++x;  /* to speed things up */
	}
	if (pBuf[bufSz - 1] == 0xff) {
		++cnt;
		/**st_id3v2_prf("    hasFalseSync: "
				"false sync byte 0x%02x at buffer end  (pos %u)\n",
				pBuf[bufSz - 1], x);**/
	}

	/**if (cnt > 0) {
		if (fastOrThourough)
			st_id3v2_prf("    hasFalseSync: false sync bytes found\n");
		else
			st_id3v2_prf("    hasFalseSync: %u false sync bytes\n", cnt);
	}**/
	if (pFSBcnt != NULL)
		*pFSBcnt = cnt;
	return (cnt > 0);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Reverse Unsynchronization
 *
 * pFalseSyncsFnd  may be NULL
 * pBuf            MUST be != NULL
 * pDelta          MUST be != NULL
 *
 * returns: ERRCODE
 *          pBuf will be modified
 *          pDelta will be set to the amount of removed bytes
 */
static Tst_err
ST_TIV2__sync_reverseUnsync(ST_OPTARG(Tst_bool *pFalseSyncsFnd),
		Tst_buf *pBuf, const Tst_uint32 bufSz, Tst_uint32 *pDelta)
{
	Tst_uint32 xI   = 0,
	           xO,
	           cntE = 0;
	/**Tst_uint32 cntU = 0;**/
	Tst_byte   c1,
	           c2;

	ST_ASSERTN_IARG(pBuf == NULL || pDelta == NULL)

	if (pFalseSyncsFnd != NULL)
		*pFalseSyncsFnd = ST_B_FALSE;
	*pDelta = 0;
	if (bufSz < 2)
		return ST_ERR_SUCC;

	if (memchr(pBuf, 0xff, bufSz) == NULL)
		return ST_ERR_SUCC;

	for (xO = 0; xI < bufSz && xO < bufSz; xO++) {
		c1       = pBuf[xI];
		pBuf[xO] = c1;
		if (xI + 1 < bufSz)
			c2 = pBuf[xI + 1];
		else {
			if (c1 == 0xff) {
				++cntE;
				/**st_id3v2_prf("    reverseUnsync: "
						"false sync byte 0x%02x at bufEnd found (posI %u)\n",
						c1, xI);**/
			}
			continue;  /* --> end of for() */
		}
		/* */
		if (c1 == 0xff && c2 == 0x00) {
			/* 0xff 0x00 0x?? --> 0xff 0x?? */
			/**++cntU;**/
			/**if (xI + 2 < bufSz)
				st_id3v2_prf("    reverseUnsync: "
						"unsync bytes 0x%02x%02x%02x found "
						"(posI [%u-%u], posO [%u-%u])\n",
						c1, c2, pBuf[xI + 2], xI, xI + 2, xO, xO + 1);
			else
				st_id3v2_prf("    reverseUnsync: "
						"unsync bytes 0x%02x%02x at bufEnd found "
						" (posI [%u-%u], posO [%u-%u])\n",
						c1, c2, xI, xI + 1, xO, xO);**/
			++xI;
			++(*pDelta);
		} else if (c1 == 0xff && (c2 & 0xe0) == 0xe0) {
			/* 11111111 111xxxxx !!! */
			++cntE;
			/**st_id3v2_prf("    reverseUnsync: "
					"false sync bytes 0x%02x%02x found (posI %u)\n",
					c1, c2, xI);**/
		}
		++xI;
	}

	if (cntE > 0 && pFalseSyncsFnd != NULL)
		*pFalseSyncsFnd = ST_B_FALSE;
	/**if (cntU > 0)
		st_id3v2_prf("    reverseUnsync: "
				"reversed unsync of %u bytes\n", cntU);**/
	return ST_ERR_SUCC;
}

/*
 * Do Unsynchronization
 *
 * pBufIn     MUST be != NULL
 * ppBufOut   MUST be != NULL
 * pBufSzOut  MUST be != NULL
 * pDelta     MUST be != NULL
 *
 * returns: ERRCODE
 *          pDelta will be set to the amount of added bytes
 */
static Tst_err
ST_TIV2__sync_doUnsync(const Tst_buf *pBufIn, const Tst_uint32 bufSzIn,
		Tst_buf **ppBufOut, Tst_uint32 *pBufSzOut, Tst_uint32 *pDelta)
{
	const Tst_uint32 cDELBSZ = 4096;
	Tst_uint32 xI,
	           xO     = 0,
	           remDSz = 0;
	/**Tst_uint32 cntF   = 0,
	           cntZ   = 0;**/
	Tst_byte   c1,
	           c2;
	/**Tst_bool isLast = ST_B_FALSE;**/

	ST_ASSERTN_IARG(pBufIn == NULL || ppBufOut == NULL ||
			pBufSzOut == NULL || pDelta == NULL)

	*pDelta = 0;
	if (bufSzIn == 0)
		return ST_ERR_SUCC;

	remDSz = *pBufSzOut;

	if (memchr(pBufIn, 0xff, bufSzIn) == NULL) {
		if (bufSzIn > *pBufSzOut) {
			ST_REALLOC(*ppBufOut, Tst_buf*, bufSzIn, 1)
			if (*ppBufOut == NULL)
				return ST_ERR_OMEM;
			*pBufSzOut = bufSzIn;
		}
		memcpy(*ppBufOut, pBufIn, bufSzIn);
		return ST_ERR_SUCC;
	}

	for (xI = 0; xI < bufSzIn; xI++) {
		c1 = pBufIn[xI];
		if (xI + 1 < bufSzIn)
			c2 = pBufIn[xI + 1];
		else {
			c2     = 0x00;
			/**isLast = ST_B_TRUE;**/
		}
		if (remDSz < 5) {
			ST_REALLOC(*ppBufOut, Tst_buf*, *pBufSzOut + cDELBSZ, 1)
			if (*ppBufOut == NULL)
				return ST_ERR_OMEM;
			memset(&(*ppBufOut)[*pBufSzOut], 0, cDELBSZ);
			remDSz     += cDELBSZ;
			*pBufSzOut += cDELBSZ;
		}
		/* */
		(*ppBufOut)[xO++] = c1;
		if (c1 == 0xff && (c2 & 0xe0) == 0xe0) {
			/* 11111111 111xxxxx --> 11111111 00000000 111xxxxx */
			/**++cntF;**/
			/**st_id3v2_prf("    doUnsync: "
					"unsyncing false bytes 0x%02x%02x  (posI %u, posO %u)\n",
					c1, c2, xI, xO - 1);**/
			(*ppBufOut)[xO++] = 0x00;
			--remDSz;
			++(*pDelta);
		} else if (c1 == 0xff && c2 == 0x00) {
			/* 0xff 0x00 0x?? --> 0xff 0x00 0x00 0x?? */
			/**++cntZ;**/
			/**if (isLast)
				st_id3v2_prf("    doUnsync: "
						"unsyncing ambig bytes 0x%02x%02x %s (posI %u, posO %u)\n",
						c1, c2, (isLast ? "at bufEnd" : ""), xI, xO - 1);**/
			(*ppBufOut)[xO++] = 0x00;
			--remDSz;
			++(*pDelta);
		}
		/* */
		--remDSz;
	}

	/**if (cntF > 0 || cntZ > 0)
		st_id3v2_prf("    doUnsync: "
				"unsynced %u false sync and %u 0xff00 bytes (total %u)\n",
				cntF, cntZ, cntF + cntZ);**/
	return ST_ERR_SUCC;
}

/******************************************************************************/
