/******************************************************************************/
/* w-md5.c                      [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for MD5 hashsum creation etc.                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2010 (dd.mm.yyyy)                                     */
/*                                                                            */
/* This file contains wrapping functions for the MD5 functions                */
/*   that libflac uses and that were written by                               */
/*     Colin Plumb (MD5) and Josh Coalson (Flac)                              */
/* Those functions etc. that I wrote are marked with "by Tas".                */
/******************************************************************************/
/* Original header from libflac follows:                                      */
/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 * Changed so as no longer to depend on Colin Plumb's `usual.h' header
 * definitions; now uses stuff from dpkg's config.h.
 *  - Ian Jackson <ijackson@nyx.cs.du.edu>.
 * Still in the public domain.
 *
 * Josh Coalson: made some changes to integrate with libFLAC.
 * Still in the public domain.
 */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#define SRC_W_MD5_C
#include "w-md5-prot.h"
#undef SRC_W_MD5_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** MD5 Context */
typedef struct {
	Tst_uint32 in[16];
	Tst_uint32 buf[4];
	Tst_uint32 bytes[2];
	Tst_buf    *pBufInternal;
	Tst_uint32 capacity;
} Tst_md5__context_intn;

static void
ST_MD5__transform(Tst_uint32 buf[4], Tst_uint32 const in[16]);

static void
ST_MD5__formatFlacInput(Tst_buf *pBuf,
                        const Tst_int32 **ppSignal,
                        const Tst_uint32 channels,
                        const Tst_uint32 samples,
                        const Tst_uint32 bytesPerSample);

#if (WORDS_BIGENDIAN == 1)
/*static void ST_MD5__byteSwap(Tst_uint32 *pBuf, Tst_uint32 words);*/
static void ST_MD5__byteSwapW4(Tst_uint32 *pBuf);
static void ST_MD5__byteSwapW14(Tst_uint32 *pBuf);
static void ST_MD5__byteSwapX16(Tst_uint32 *pBuf);
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_md5_context */
Tst_err
st_md5_stc_initCtx(Tst_md5_context *pCtx)
{
	ST_ASSERTN_IARG(pCtx == NULL)

	ST_CALLOC(pCtx->pObInternal, Tst_md5__context_intn*,
			1, sizeof(Tst_md5__context_intn))
	if (pCtx->pObInternal == NULL)
		return ST_ERR_OMEM;

	((Tst_md5__context_intn*)pCtx->pObInternal)->pBufInternal = NULL;
	st_md5_stc_rsetCtx(pCtx);
	return ST_ERR_SUCC;
}

void
st_md5_stc_rsetCtx(Tst_md5_context *pCtx)
{
	Tst_md5__context_intn *pCTXI;

	ST_ASSERTN_VOID(pCtx == NULL || pCtx->pObInternal == NULL)

	pCTXI = (Tst_md5__context_intn*)pCtx->pObInternal;
	/* */
	memset(pCTXI->in, 0, sizeof(pCTXI->in));
	/*
	 * From MD5:
	 * Start MD5 accumulation.  Set bit count to 0 and buffer to
	 * mysterious initialization constants
	 */
	pCTXI->buf[0]   = 0x67452301;
	pCTXI->buf[1]   = 0xefcdab89;
	pCTXI->buf[2]   = 0x98badcfe;
	pCTXI->buf[3]   = 0x10325476;
	pCTXI->bytes[0] = 0;
	pCTXI->bytes[1] = 0;
	ST_DELPOINT(pCTXI->pBufInternal)
	pCTXI->capacity = 0;
}

void
st_md5_stc_freeCtx(Tst_md5_context *pCtx)
{
	ST_ASSERTN_VOID(pCtx == NULL)

	if (pCtx->pObInternal != NULL) {
		st_md5_stc_rsetCtx(pCtx);
		ST_DELPOINT(pCtx->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void
st_md5_update(Tst_md5_context *pContext,
		Tst_buf const *pBuf, Tst_uint32 len)
{
	Tst_uint32 t;
	Tst_md5__context_intn *pCTXI;

	ST_ASSERTN_VOID(pContext == NULL || pContext->pObInternal == NULL ||
			pBuf == NULL)

	pCTXI = (Tst_md5__context_intn*)pContext->pObInternal;

	/* Update byte count */
	t = pCTXI->bytes[0];
	if ((pCTXI->bytes[0] = t + len) < t)
		pCTXI->bytes[1]++;	/* Carry from low to high */

	t = 64 - (t & 0x3f);	/* Space available in pCTXI->in (at least 1) */
	if (t > len) {
		memcpy((Tst_byte*)pCTXI->in + 64 - t, pBuf, len);
		return;
	}
	/* First chunk is an odd size */
	memcpy((Tst_byte*)pCTXI->in + 64 - t, pBuf, t);
#	if (WORDS_BIGENDIAN == 1)
	ST_MD5__byteSwapX16(pCTXI->in);
#	endif
	ST_MD5__transform(pCTXI->buf, pCTXI->in);
	pBuf += t;
	len  -= t;

	/* Process data in 64-byte chunks */
	while (len >= 64) {
		memcpy(pCTXI->in, pBuf, 64);
#		if (WORDS_BIGENDIAN == 1)
		ST_MD5__byteSwapX16(pCTXI->in);
#		endif
		ST_MD5__transform(pCTXI->buf, pCTXI->in);
		pBuf += 64;
		len  -= 64;
	}

	/* Handle any remaining bytes of data. */
	memcpy(pCTXI->in, pBuf, len);
}

/*----------------------------------------------------------------------------*/

/*
 * From libflac:
 * Convert the incoming audio signal to a byte stream and st_md5_update it
 */
Tst_err
st_md5_updateFlac(Tst_md5_context *pContext,
		const Tst_int32 **ppSamples, const Tst_uint32 channels,
		const Tst_uint32 samplesPerChannel,
		const Tst_uint32 bytesPerSample)
{
	const Tst_uint32 bytes_needed = channels *
			samplesPerChannel * bytesPerSample;
	Tst_md5__context_intn *pCTXI;

	ST_ASSERTN_IARG(pContext == NULL || pContext->pObInternal == NULL ||
			ppSamples == NULL)

	pCTXI = (Tst_md5__context_intn*)pContext->pObInternal;
	/* */
	if (pCTXI->capacity < bytes_needed) {
		ST_REALLOC(pCTXI->pBufInternal, Tst_buf*,
				bytes_needed, 1)
		if (pCTXI->pBufInternal == NULL)
			return ST_ERR_OMEM;
		pCTXI->capacity = bytes_needed;
	}

	ST_MD5__formatFlacInput(pCTXI->pBufInternal, ppSamples,
			channels, samplesPerChannel, bytesPerSample);

	st_md5_update(pContext, pCTXI->pBufInternal, bytes_needed);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void
st_md5_final(Tst_buf digest[16], Tst_md5_context *pContext)
{
	Tst_int32 count;  /* bytes in pContext->in */
	Tst_byte  *p;
	Tst_md5__context_intn *pCTXI;

	ST_ASSERTN_VOID(pContext == NULL || pContext->pObInternal == NULL)

	pCTXI = (Tst_md5__context_intn*)pContext->pObInternal;

	count = pCTXI->bytes[0] & 0x3f;
	p     = (Tst_byte*)pCTXI->in + count;

	/* Set the first char of padding to 0x80.  There is always room. */
	*p++ = 0x80;

	/* Bytes of padding needed to make 56 bytes (-8..55) */
	count = 56 - 1 - count;

	if (count < 0) {	/* Padding forces an extra block */
		memset(p, 0, count + 8);
#		if (WORDS_BIGENDIAN == 1)
		ST_MD5__byteSwapX16(pCTXI->in);
#		endif
		ST_MD5__transform(pCTXI->buf, pCTXI->in);
		p     = (Tst_byte*)pCTXI->in;
		count = 56;
	}
	memset(p, 0, count);
#	if (WORDS_BIGENDIAN == 1)
	ST_MD5__byteSwapW14(pCTXI->in);
#	endif

	/* Append length in bits and transform */
	pCTXI->in[14] = pCTXI->bytes[0] << 3;
	pCTXI->in[15] = pCTXI->bytes[1] << 3 | pCTXI->bytes[0] >> 29;
	ST_MD5__transform(pCTXI->buf, pCTXI->in);

#	if (WORDS_BIGENDIAN == 1)
	ST_MD5__byteSwapW4(pCTXI->buf);
#	endif
	memcpy(digest, pCTXI->buf, 16);

	st_md5_stc_rsetCtx(pContext);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* The four core functions - F1 is optimized somewhat */
/* #define LOC_F1_(x, y, z) (x & y | ~x & z) */
#define LOC_F1_(x, y, z) (z ^ (x & (y ^ z)))
#define LOC_F2_(x, y, z) LOC_F1_(z, x, y)
#define LOC_F3_(x, y, z) (x ^ y ^ z)
#define LOC_F4_(x, y, z) (y ^ (x | ~z))
/* This is the central step in the MD5 algorithm. */
#define LOC_MD5STEP_(f,w,x,y,z,in,s) \
	 (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void
ST_MD5__transform(Tst_uint32 buf[4], Tst_uint32 const in[16])
{
	register Tst_uint32 a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	LOC_MD5STEP_(LOC_F1_, a, b, c, d, in[0] + 0xd76aa478, 7);
	LOC_MD5STEP_(LOC_F1_, d, a, b, c, in[1] + 0xe8c7b756, 12);
	LOC_MD5STEP_(LOC_F1_, c, d, a, b, in[2] + 0x242070db, 17);
	LOC_MD5STEP_(LOC_F1_, b, c, d, a, in[3] + 0xc1bdceee, 22);
	LOC_MD5STEP_(LOC_F1_, a, b, c, d, in[4] + 0xf57c0faf, 7);
	LOC_MD5STEP_(LOC_F1_, d, a, b, c, in[5] + 0x4787c62a, 12);
	LOC_MD5STEP_(LOC_F1_, c, d, a, b, in[6] + 0xa8304613, 17);
	LOC_MD5STEP_(LOC_F1_, b, c, d, a, in[7] + 0xfd469501, 22);
	LOC_MD5STEP_(LOC_F1_, a, b, c, d, in[8] + 0x698098d8, 7);
	LOC_MD5STEP_(LOC_F1_, d, a, b, c, in[9] + 0x8b44f7af, 12);
	LOC_MD5STEP_(LOC_F1_, c, d, a, b, in[10] + 0xffff5bb1, 17);
	LOC_MD5STEP_(LOC_F1_, b, c, d, a, in[11] + 0x895cd7be, 22);
	LOC_MD5STEP_(LOC_F1_, a, b, c, d, in[12] + 0x6b901122, 7);
	LOC_MD5STEP_(LOC_F1_, d, a, b, c, in[13] + 0xfd987193, 12);
	LOC_MD5STEP_(LOC_F1_, c, d, a, b, in[14] + 0xa679438e, 17);
	LOC_MD5STEP_(LOC_F1_, b, c, d, a, in[15] + 0x49b40821, 22);

	LOC_MD5STEP_(LOC_F2_, a, b, c, d, in[1] + 0xf61e2562, 5);
	LOC_MD5STEP_(LOC_F2_, d, a, b, c, in[6] + 0xc040b340, 9);
	LOC_MD5STEP_(LOC_F2_, c, d, a, b, in[11] + 0x265e5a51, 14);
	LOC_MD5STEP_(LOC_F2_, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	LOC_MD5STEP_(LOC_F2_, a, b, c, d, in[5] + 0xd62f105d, 5);
	LOC_MD5STEP_(LOC_F2_, d, a, b, c, in[10] + 0x02441453, 9);
	LOC_MD5STEP_(LOC_F2_, c, d, a, b, in[15] + 0xd8a1e681, 14);
	LOC_MD5STEP_(LOC_F2_, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	LOC_MD5STEP_(LOC_F2_, a, b, c, d, in[9] + 0x21e1cde6, 5);
	LOC_MD5STEP_(LOC_F2_, d, a, b, c, in[14] + 0xc33707d6, 9);
	LOC_MD5STEP_(LOC_F2_, c, d, a, b, in[3] + 0xf4d50d87, 14);
	LOC_MD5STEP_(LOC_F2_, b, c, d, a, in[8] + 0x455a14ed, 20);
	LOC_MD5STEP_(LOC_F2_, a, b, c, d, in[13] + 0xa9e3e905, 5);
	LOC_MD5STEP_(LOC_F2_, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	LOC_MD5STEP_(LOC_F2_, c, d, a, b, in[7] + 0x676f02d9, 14);
	LOC_MD5STEP_(LOC_F2_, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	LOC_MD5STEP_(LOC_F3_, a, b, c, d, in[5] + 0xfffa3942, 4);
	LOC_MD5STEP_(LOC_F3_, d, a, b, c, in[8] + 0x8771f681, 11);
	LOC_MD5STEP_(LOC_F3_, c, d, a, b, in[11] + 0x6d9d6122, 16);
	LOC_MD5STEP_(LOC_F3_, b, c, d, a, in[14] + 0xfde5380c, 23);
	LOC_MD5STEP_(LOC_F3_, a, b, c, d, in[1] + 0xa4beea44, 4);
	LOC_MD5STEP_(LOC_F3_, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	LOC_MD5STEP_(LOC_F3_, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	LOC_MD5STEP_(LOC_F3_, b, c, d, a, in[10] + 0xbebfbc70, 23);
	LOC_MD5STEP_(LOC_F3_, a, b, c, d, in[13] + 0x289b7ec6, 4);
	LOC_MD5STEP_(LOC_F3_, d, a, b, c, in[0] + 0xeaa127fa, 11);
	LOC_MD5STEP_(LOC_F3_, c, d, a, b, in[3] + 0xd4ef3085, 16);
	LOC_MD5STEP_(LOC_F3_, b, c, d, a, in[6] + 0x04881d05, 23);
	LOC_MD5STEP_(LOC_F3_, a, b, c, d, in[9] + 0xd9d4d039, 4);
	LOC_MD5STEP_(LOC_F3_, d, a, b, c, in[12] + 0xe6db99e5, 11);
	LOC_MD5STEP_(LOC_F3_, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	LOC_MD5STEP_(LOC_F3_, b, c, d, a, in[2] + 0xc4ac5665, 23);

	LOC_MD5STEP_(LOC_F4_, a, b, c, d, in[0] + 0xf4292244, 6);
	LOC_MD5STEP_(LOC_F4_, d, a, b, c, in[7] + 0x432aff97, 10);
	LOC_MD5STEP_(LOC_F4_, c, d, a, b, in[14] + 0xab9423a7, 15);
	LOC_MD5STEP_(LOC_F4_, b, c, d, a, in[5] + 0xfc93a039, 21);
	LOC_MD5STEP_(LOC_F4_, a, b, c, d, in[12] + 0x655b59c3, 6);
	LOC_MD5STEP_(LOC_F4_, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	LOC_MD5STEP_(LOC_F4_, c, d, a, b, in[10] + 0xffeff47d, 15);
	LOC_MD5STEP_(LOC_F4_, b, c, d, a, in[1] + 0x85845dd1, 21);
	LOC_MD5STEP_(LOC_F4_, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	LOC_MD5STEP_(LOC_F4_, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	LOC_MD5STEP_(LOC_F4_, c, d, a, b, in[6] + 0xa3014314, 15);
	LOC_MD5STEP_(LOC_F4_, b, c, d, a, in[13] + 0x4e0811a1, 21);
	LOC_MD5STEP_(LOC_F4_, a, b, c, d, in[4] + 0xf7537e82, 6);
	LOC_MD5STEP_(LOC_F4_, d, a, b, c, in[11] + 0xbd3af235, 10);
	LOC_MD5STEP_(LOC_F4_, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	LOC_MD5STEP_(LOC_F4_, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}
#undef LOC_F1_
#undef LOC_F2_
#undef LOC_F3_
#undef LOC_F4_
#undef LOC_MD5STEP_


#if (WORDS_BIGENDIAN == 1)

/*
 * Swapping 32bit value:
 *  AAbbCCdd
 *            bbCCdd00 &--> bb00dd00
 *            00AAbbCC &--> 00AA00CC
 *                     |--> bbAAddCC
 *            bbAAddCC >>-- 0000bbAA
 *            bbAAddCC <<-- ddCC0000
 *                     |--> ddCCbbAA
 *  ddCCbbAA
 */
#define LOC_SWAP_UI32_  /* from libflac -- macrodized by Tas */ \
			x       = *pBuf; \
			x       = ((x << 8) & 0xff00ff00) | ((x >> 8) & 0x00ff00ff); \
			*pBuf++ = (x >> 16) | (x << 16);

/*
 * Reverse byteorder of 32bit 'words'
 */
static void
ST_MD5__byteSwapW4(Tst_uint32 *pBuf)
{
	register Tst_uint32 x;

	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
}

static void
ST_MD5__byteSwapW14(Tst_uint32 *pBuf)
{
	register Tst_uint32 x;

	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
	LOC_SWAP_UI32_  LOC_SWAP_UI32_
}

/*
 * From libflac:
 * Reverse byteorder of 32bit 'words'
 */
/*static void
ST_MD5__byteSwap(Tst_uint32 *pBuf, Tst_uint32 words)
{
	register Tst_uint32 x;

	do {
		x       = *pBuf;
		x       = ((x << 8) & 0xff00ff00) | ((x >> 8) & 0x00ff00ff);
		*pBuf++ = (x >> 16) | (x << 16);
	} while (--words);
}*/

static void
ST_MD5__byteSwapX16(Tst_uint32 *pBuf)
{
	register Tst_uint32 x;

	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
	LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_  LOC_SWAP_UI32_
}
#endif

/*
 * From libflac:
 * Convert the incoming audio signal to a byte stream
 */
static void
ST_MD5__formatFlacInput(Tst_buf *pBuf,
		const Tst_int32 **ppSignal,
		const Tst_uint32 channels, const Tst_uint32 samples,
		const Tst_uint32 bytesPerSample)
{
	Tst_uint32 channel,
	           sample;
	register Tst_int32 i32;
	register Tst_byte  *pBufP = pBuf;

#	if (WORDS_BIGENDIAN != 1)
	if (channels == 2 && bytesPerSample == 2) {
		Tst_int16 *pBufP16 = ((Tst_int16*)pBufP) + 1;

		memcpy(pBufP, ppSignal[0], sizeof(Tst_int32) * samples);
		for (sample = 0; sample < samples; sample++, pBufP16 += 2)
			*pBufP16 = (Tst_int16)ppSignal[1][sample];
	} else if (channels == 1 && bytesPerSample == 2) {
		Tst_int16 *pBufP16 = (Tst_int16*)pBufP;

		for (sample = 0; sample < samples; sample++)
			*pBufP16++ = (Tst_int16)ppSignal[0][sample];
	}
	else
#	endif
	if (bytesPerSample == 2) {
		if (channels == 2) {
			for (sample = 0; sample < samples; sample++) {
				i32      = ppSignal[0][sample];
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32;
				i32      = ppSignal[1][sample];
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32;
			}
		} else if (channels == 1) {
			for (sample = 0; sample < samples; sample++) {
				i32      = ppSignal[0][sample];
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32;
			}
		} else {
			for (sample = 0; sample < samples; sample++) {
				for (channel = 0; channel < channels; channel++) {
					i32      = ppSignal[channel][sample];
					*pBufP++ = (Tst_byte)i32; i32 >>= 8;
					*pBufP++ = (Tst_byte)i32;
				}
			}
		}
	} else if (bytesPerSample == 3) {
		if (channels == 2) {
			for (sample = 0; sample < samples; sample++) {
				i32      = ppSignal[0][sample];
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32;
				i32      = ppSignal[1][sample];
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32;
			}
		} else if (channels == 1) {
			for (sample = 0; sample < samples; sample++) {
				i32      = ppSignal[0][sample];
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32;
			}
		} else {
			for (sample = 0; sample < samples; sample++) {
				for (channel = 0; channel < channels; channel++) {
					i32      = ppSignal[channel][sample];
					*pBufP++ = (Tst_byte)i32; i32 >>= 8;
					*pBufP++ = (Tst_byte)i32; i32 >>= 8;
					*pBufP++ = (Tst_byte)i32;
				}
			}
		}
	} else if (bytesPerSample == 1) {
		if (channels == 2) {
			for (sample = 0; sample < samples; sample++) {
				i32      = ppSignal[0][sample];
				*pBufP++ = (Tst_byte)i32;
				i32      = ppSignal[1][sample];
				*pBufP++ = (Tst_byte)i32;
			}
		} else if (channels == 1) {
			for (sample = 0; sample < samples; sample++) {
				i32      = ppSignal[0][sample];
				*pBufP++ = (Tst_byte)i32;
			}
		} else {
			for (sample = 0; sample < samples; sample++) {
				for (channel = 0; channel < channels; channel++) {
					i32      = ppSignal[channel][sample];
					*pBufP++ = (Tst_byte)i32;
				}
			}
		}
	} else {  /* bytesPerSample == 4 */
		for (sample = 0; sample < samples; sample++) {
			for (channel = 0; channel < channels; channel++) {
				i32      = ppSignal[channel][sample];
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32; i32 >>= 8;
				*pBufP++ = (Tst_byte)i32;
			}
		}
	}
}

/******************************************************************************/
