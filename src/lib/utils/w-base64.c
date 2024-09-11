/******************************************************************************/
/* w-base64.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Base64 data en-/decoding                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 27.04.2010 (dd.mm.yyyy)                                     */
/*                                                                            */
/* This file contains wrapping functions for the Base64 stuff.                */
/* Those functions etc. that I wrote are marked with "by Tas".                */
/******************************************************************************/
/* Original header from base64.c follows:                                     */
/*
 * Copyright (c) 1996, 1998 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * Portions Copyright (c) 1995 by International Business Machines, Inc.
 *
 * International Business Machines, Inc. (hereinafter called IBM) grants
 * permission under its copyrights to use, copy, modify, and distribute this
 * Software with or without fee, provided that the above copyright notice and
 * all paragraphs of this notice appear in all copies, and that the name of IBM
 * not be used in connection with the marketing of any product incorporating
 * the Software or modifications thereof, without specific, written prior
 * permission.
 *
 * To the extent it has a right to do so, IBM grants an immunity from suit
 * under its patents, if any, for the use, sale or manufacture of products to
 * the extent that such products are used for performing Domain Name System
 * dynamic updates in TCP/IP networks by means of the Software.  No immunity is
 * granted for any product per se or for any other function of any product.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", AND IBM DISCLAIMS ALL WARRANTIES,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE, EVEN
 * IF IBM IS APPRISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */


/* (From RFC1521 and draft-ietf-dnssec-secext-03.txt)
   The following encoding technique is taken from RFC 1521 by Borenstein
   and Freed.  It is reproduced here in a slightly edited form for
   convenience.

   A 65-character subset of US-ASCII is used, enabling 6 bits to be
   represented per printable character. (The extra 65th character, "=",
   is used to signify a special processing function.)

   The encoding process represents 24-bit groups of input bits as output
   strings of 4 encoded characters. Proceeding from left to right, a
   24-bit input group is formed by concatenating 3 8-bit input groups.
   These 24 bits are then treated as 4 concatenated 6-bit groups, each
   of which is translated into a single digit in the base64 alphabet.

   Each 6-bit group is used as an index into an array of 64 printable
   characters. The character referenced by the index is placed in the
   output string.

                         Table 1: The Base64 Alphabet

      Value Encoding  Value Encoding  Value Encoding  Value Encoding
          0 A            17 R            34 i            51 z
          1 B            18 S            35 j            52 0
          2 C            19 T            36 k            53 1
          3 D            20 U            37 l            54 2
          4 E            21 V            38 m            55 3
          5 F            22 W            39 n            56 4
          6 G            23 X            40 o            57 5
          7 H            24 Y            41 p            58 6
          8 I            25 Z            42 q            59 7
          9 J            26 a            43 r            60 8
         10 K            27 b            44 s            61 9
         11 L            28 c            45 t            62 +
         12 M            29 d            46 u            63 /
         13 N            30 e            47 v
         14 O            31 f            48 w         (pad) =
         15 P            32 g            49 x
         16 Q            33 h            50 y

   Special processing is performed if fewer than 24 bits are available
   at the end of the data being encoded.  A full encoding quantum is
   always completed at the end of a quantity.  When fewer than 24 input
   bits are available in an input group, zero bits are added (on the
   right) to form an integral number of 6-bit groups.  Padding at the
   end of the data is performed using the '=' character.

   Since all base64 input is an integral number of octets, only the
         -------------------------------------------------
   following cases can arise:

       (1) the final quantum of encoding input is an integral
           multiple of 24 bits; here, the final unit of encoded
	   output will be an integral multiple of 4 characters
	   with no "=" padding,
       (2) the final quantum of encoding input is exactly 8 bits;
           here, the final unit of encoded output will be two
	   characters followed by two "=" padding characters, or
       (3) the final quantum of encoding input is exactly 16 bits;
           here, the final unit of encoded output will be three
	   characters followed by one "=" padding character.
   */
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
#define SRC_W_BASE64_C
#include "w-base64-prot.h"
#undef SRC_W_BASE64_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* strchr() */
#include <ctype.h>       /* isspace() */
#if (ST_W_BASE64_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static const char ST_BASE64_CSET[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char ST_BASE64_PAD_CHAR = '=';

#if 0
/* Output buffer, by Tas */
typedef struct {
	union {
		Tst_buf *pBBuf;  /* actual buffer */
		Tst_str *pSBuf;
	} bs_buf;
	Tst_uint32 sz;     /* size of buffer */
	Tst_uint32 used;   /* used bytes of buffer */
	Tst_bool   isStr;  /* is Tst_str or Tst_buf buffer ? */
} Tst_base64__outBuf;
#endif

#if (ST_W_BASE64_DEB_ == 1)
/* Debugging, by Tas */
static void
ST_BASE64__prf(const char *pMsg, ...);
#endif

#if 0
/** resize buffer, by Tas */
static Tst_err
ST_BASE64__reszOutBuf(Tst_base64__outBuf *pOB, Tst_uint32 incSz);
/** encode from/to buffer */
static Tst_err
ST_BASE64__ntop(Tst_buf const *src, Tst_uint32 srclength,
                const Tst_bool withNULterm,
                Tst_base64__outBuf *pOB);
/** decode from/to buffer */
static Tst_err
ST_BASE64__pton(Tst_str const *src, Tst_base64__outBuf *pOB);
#endif

/** encode from stream reader to binobj, by Tas */
static Tst_err
ST_BASE64__ntop_binobj(Tst_streamrd *pStrrd,
                       const Tst_bool withNULterm,
                       Tst_binobj *pBinDatOut);
/** decode from stream reader to binobj, by Tas */
static Tst_err
ST_BASE64__pton_binobj(Tst_streamrd *pStrrd, Tst_binobj *pBinDatOut);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
/*
 * Do Base64 encoding, by Tas
 */
Tst_err
st_base64_encode(const Tst_buf *pInp, const Tst_uint32 inSz,
		const Tst_bool withNULterm,
		Tst_str **ppOutp, Tst_uint32 *pOutSz)
{
	Tst_err       res;
	Tst_buf const *pI = pInp;
	Tst_base64__outBuf obuf;

	ST_ASSERTN_IARG(pInp == NULL || ppOutp == NULL ||
			pOutSz == NULL)

	*pOutSz = 0;
	ST_DELPOINT(*ppOutp)
	/* */
	if (inSz < 1)
		return ST_ERR_SUCC;
	/* */
	obuf.sz           = 0;
	obuf.used         = 0;
	obuf.bs_buf.pSBuf = NULL;
	obuf.isStr        = ST_B_TRUE;
	/* */
	res = ST_BASE64__ntop(pI, inSz, withNULterm, &obuf);
	if (res != ST_ERR_SUCC)
		ST_DELPOINT(obuf.bs_buf.pSBuf)
	else {
		*pOutSz = obuf.used;
		*ppOutp = obuf.bs_buf.pSBuf;
	}
	return res;
}

/*
 * Do Base64 decoding, by Tas
 */
Tst_err
st_base64_decode(const Tst_str *pInp, const Tst_uint32 inSz,
		Tst_buf **ppOutp, Tst_uint32 *pOutSz)
{
	Tst_err       res;
	Tst_str const *pI = pInp;
	Tst_base64__outBuf obuf;

	ST_ASSERTN_IARG(pInp == NULL || ppOutp == NULL ||
			pOutSz == NULL)

	*pOutSz = 0;
	ST_DELPOINT(*ppOutp)
	/* */
	if (inSz < 2)
		return ST_ERR_SUCC;
	/* */
	obuf.sz           = 0;
	obuf.used         = 0;
	obuf.bs_buf.pBBuf = NULL;
	obuf.isStr        = ST_B_FALSE;
	/* */
	res = ST_BASE64__pton(pI, &obuf);
	if (res != ST_ERR_SUCC)
		ST_DELPOINT(obuf.bs_buf.pBBuf)
	else {
		*pOutSz = obuf.used;
		*ppOutp = obuf.bs_buf.pBBuf;
	}
	return res;
}
#endif

/*----------------------------------------------------------------------------*/

/*
 * Do Base64 encoding, by Tas
 *   this functions reads and writes to binary objects
 */
Tst_err
st_base64_encode_binobj(Tst_binobj *pBinDatIn,
		const Tst_bool withNULterm, Tst_binobj *pBinDatOut)
{
	Tst_err    res;
	Tst_uint32 tsz;
	Tst_streamrd strrd;

	ST_ASSERTN_IARG(pBinDatIn == NULL || pBinDatOut == NULL)

	/* write some dummy data to output */
	tsz  = st_binobj_getDataSize(pBinDatIn);
	tsz += (tsz / 3);
	if (tsz < st_binobj_getThreshold(pBinDatOut)) {
		Tst_buf *pTmpB = NULL;

		ST_CALLOC(pTmpB, Tst_buf*, tsz, 1)
		if (pTmpB == NULL)
			return ST_ERR_OMEM;
		memset(pTmpB, 0, tsz);
		st_binobj_setData(pBinDatOut, pTmpB, tsz);
		ST_DELPOINT(pTmpB)
	}
	/* clear any previously written data
	 *   but leave internal buffer size of pBinDatOut untouched */
	st_binobj_stc_rsetData(pBinDatOut);
	/* */
	st_streamrd_stc_initSObj(&strrd);
	res = st_binobj_attachStreamrd(pBinDatIn, &strrd, 0);
	if (res != ST_ERR_SUCC) {
		st_streamrd_stc_freeSObj(&strrd);
		return res;
	}
	/* */
	res = ST_BASE64__ntop_binobj(&strrd, withNULterm, pBinDatOut);

	st_binobj_detachStreamrd(pBinDatIn);
	st_streamrd_stc_freeSObj(&strrd);
	return res;
}

/*
 * Do Base64 decoding, by Tas
 *   this functions reads and writes to binary objects
 */
Tst_err
st_base64_decode_binobj(Tst_binobj *pBinDatIn, Tst_binobj *pBinDatOut)
{
	Tst_err    res;
	Tst_uint32 tsz;
	Tst_streamrd strrd;

	ST_ASSERTN_IARG(pBinDatIn == NULL || pBinDatOut == NULL)

	/* write some dummy data to output */
	tsz = st_binobj_getDataSize(pBinDatIn);
	if (tsz < st_binobj_getThreshold(pBinDatOut)) {
		Tst_buf *pTmpB = NULL;

		ST_CALLOC(pTmpB, Tst_buf*, tsz, 1)
		if (pTmpB == NULL)
			return ST_ERR_OMEM;
		memset(pTmpB, 0, tsz);
		st_binobj_setData(pBinDatOut, pTmpB, tsz);
		ST_DELPOINT(pTmpB)
	}
	/* clear any previously written data
	 *   but leave internal buffer size of pBinDatOut untouched */
	st_binobj_stc_rsetData(pBinDatOut);
	/* */
	st_streamrd_stc_initSObj(&strrd);
	res = st_binobj_attachStreamrd(pBinDatIn, &strrd, 0);
	if (res != ST_ERR_SUCC) {
		st_streamrd_stc_freeSObj(&strrd);
		return res;
	}
	/* */
	res = ST_BASE64__pton_binobj(&strrd, pBinDatOut);

	st_binobj_detachStreamrd(pBinDatIn);
	st_streamrd_stc_freeSObj(&strrd);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if 0
/* resize buffer, by Tas */
static Tst_err
ST_BASE64__reszOutBuf(Tst_base64__outBuf *pOB, Tst_uint32 incSz)
{
	if (incSz < 1024)
		incSz = 1024;
	if (pOB->isStr) {
		ST_REALLOC(pOB->bs_buf.pSBuf, Tst_str*, pOB->sz + incSz, 1)
		if (pOB->bs_buf.pSBuf == NULL)
			return ST_ERR_OMEM;
	} else {
		ST_REALLOC(pOB->bs_buf.pBBuf, Tst_buf*, pOB->sz + incSz, 1)
		if (pOB->bs_buf.pBBuf == NULL)
			return ST_ERR_OMEM;
	}
	pOB->sz += incSz;
	/**ST_BASE64__prf("b64: bufSz %u for %s\n",
			pOB->sz, (pOB->isStr ? "Tst_str" : "Tst_buf"));**/
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* slightly modified by Tas */
static Tst_err
ST_BASE64__ntop(Tst_buf const *src, Tst_uint32 srclength,
		const Tst_bool withNULterm,
		Tst_base64__outBuf *pOB)
{
	Tst_err    res;
	/*Tst_uint32 datalength = 0;*/
	Tst_buf    input[3];
	Tst_buf    output[4];
	Tst_uint32 i;

	while (2 < srclength) {
		input[0]   = *src++;
		input[1]   = *src++;
		input[2]   = *src++;
		srclength -= 3;

		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
		output[3] = input[2] & 0x3f;
#		if (ST_W_BASE64_DEB_ == 1)
		ST_ASSERTN_FAIL(output[0] >= 64)
		ST_ASSERTN_FAIL(output[1] >= 64)
		ST_ASSERTN_FAIL(output[2] >= 64)
		ST_ASSERTN_FAIL(output[3] >= 64)
#		endif

		/*if (datalength + 4 > targsize)
			return (-1);
		target[datalength++] = ST_BASE64_CSET[output[0]];
		target[datalength++] = ST_BASE64_CSET[output[1]];
		target[datalength++] = ST_BASE64_CSET[output[2]];
		target[datalength++] = ST_BASE64_CSET[output[3]];*/
		if (pOB->used + 4 >= pOB->sz) {
			res = ST_BASE64__reszOutBuf(pOB, srclength / 2);
			if (res != ST_ERR_SUCC)
				return res;
		}
		pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_CSET[output[0]];
		pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_CSET[output[1]];
		pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_CSET[output[2]];
		pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_CSET[output[3]];
	}

	/* Now we worry about padding. */
	if (0 != srclength) {
		ST_ASSERTN_NUM(ST_ERR_FAIL, srclength > sizeof(input))

		/* Get what's left. */
		input[0] = input[1] = input[2] = '\0';
		for (i = 0; i < srclength; i++)
			input[i] = *src++;

		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
#		if (ST_W_BASE64_DEB_ == 1)
		ST_ASSERTN_FAIL(output[0] >= 64)
		ST_ASSERTN_FAIL(output[1] >= 64)
		ST_ASSERTN_FAIL(output[2] >= 64)
#		endif

		/*if (datalength + 4 > targsize)
			return (-1);
		target[datalength++] = ST_BASE64_CSET[output[0]];
		target[datalength++] = ST_BASE64_CSET[output[1]];
		if (srclength == 1)
			target[datalength++] = ST_BASE64_PAD_CHAR;
		else
			target[datalength++] = ST_BASE64_CSET[output[2]];
		target[datalength++] = ST_BASE64_PAD_CHAR;*/
		if (pOB->used + 4 >= pOB->sz) {
			res = ST_BASE64__reszOutBuf(pOB, srclength / 2);
			if (res != ST_ERR_SUCC)
				return res;
		}
		pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_CSET[output[0]];
		pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_CSET[output[1]];
		if (srclength == 1)
			pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_PAD_CHAR;
		else
			pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_CSET[output[2]];
		pOB->bs_buf.pSBuf[pOB->used++] = ST_BASE64_PAD_CHAR;
	}
	/*if (datalength >= targsize)
		return (-1);
	target[datalength] = '\0';*/	/* Returned value doesn't count \0. */
	if (withNULterm) {
		if (pOB->used + 1 >= pOB->sz) {
			res = ST_BASE64__reszOutBuf(pOB, 2);
			if (res != ST_ERR_SUCC)
				return res;
		}
		pOB->bs_buf.pSBuf[pOB->used++] = '\0';
	}
	/*return (datalength);*/
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* skips all whitespace anywhere.
   converts characters, four at a time, starting at (or after)
   src from base - 64 numbers into three 8 bit bytes in the target area.
 */

/* slightly modified by Tas */
static Tst_err
ST_BASE64__pton(Tst_str const *src, Tst_base64__outBuf *pOB)
{
	Tst_err    res;
	int        /*tarindex = 0,*/
	           state = 0,
	           ch;
	char const *pPos;

	while ((ch = (char)*src++) != '\0') {
		if (isspace((unsigned char)ch))   /* Skip whitespace anywhere. */
			continue;

		if (ch == ST_BASE64_PAD_CHAR)
			break;

		pPos = strchr(ST_BASE64_CSET, ch);
		if (pPos == 0) {  /* A non-base64 character. */
			/**ST_BASE64__prf("inv char\n");**/
			return ST_ERR_IDAT;
		}

		switch (state) {
		case 0:
			/*if (target) {
				if ((Tst_uint32)tarindex >= targsize)
					return (-1);
				target[tarindex] = (pPos - ST_BASE64_CSET) << 2;
			}*/
			if (pOB->used + 1 >= pOB->sz) {
				res = ST_BASE64__reszOutBuf(pOB, pOB->sz / 2);
				if (res != ST_ERR_SUCC)
					return res;
			}
			pOB->bs_buf.pBBuf[pOB->used] = (pPos - ST_BASE64_CSET) << 2;
			state = 1;
			break;
		case 1:
			/*if (target) {
				if ((Tst_uint32)tarindex + 1 >= targsize)
					return (-1);
				target[tarindex]   |=  (pPos - ST_BASE64_CSET) >> 4;
				target[tarindex+1]  = ((pPos - ST_BASE64_CSET) & 0x0f) << 4;
			}
			tarindex++;*/
			if (pOB->used + 2 >= pOB->sz) {
				res = ST_BASE64__reszOutBuf(pOB, pOB->sz / 2);
				if (res != ST_ERR_SUCC)
					return res;
			}
			pOB->bs_buf.pBBuf[pOB->used]    |=  (pPos - ST_BASE64_CSET) >> 4;
			pOB->bs_buf.pBBuf[pOB->used + 1] = ((pPos - ST_BASE64_CSET) & 0x0f) << 4;
			pOB->used++;
			state = 2;
			break;
		case 2:
			/*if (target) {
				if ((Tst_uint32)tarindex + 1 >= targsize)
					return (-1);
				target[tarindex]   |=  (pPos - ST_BASE64_CSET) >> 2;
				target[tarindex+1]  = ((pPos - ST_BASE64_CSET) & 0x03) << 6;
			}
			tarindex++;*/
			if (pOB->used + 2 >= pOB->sz) {
				res = ST_BASE64__reszOutBuf(pOB, pOB->sz / 2);
				if (res != ST_ERR_SUCC)
					return res;
			}
			pOB->bs_buf.pBBuf[pOB->used]  |=  (pPos - ST_BASE64_CSET) >> 2;
			pOB->bs_buf.pBBuf[pOB->used+1] = ((pPos - ST_BASE64_CSET) & 0x03) << 6;
			pOB->used++;
			state = 3;
			break;
		case 3:
			/*if (target) {
				if ((Tst_uint32)tarindex >= targsize)
					return (-1);
				target[tarindex] |= (pPos - ST_BASE64_CSET);
			}
			tarindex++;*/
			if (pOB->used + 1 >= pOB->sz) {
				res = ST_BASE64__reszOutBuf(pOB, pOB->sz / 2);
				if (res != ST_ERR_SUCC)
					return res;
			}
			pOB->bs_buf.pBBuf[pOB->used] |= (pPos - ST_BASE64_CSET);
			pOB->used++;
			state = 0;
			break;
		default:
			ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
			break;
		}
	}

	/*
	 * We are done decoding Base-64 chars.  Let's see if we ended
	 * on a byte boundary, and/or with errorneous trailing characters.
	 */

	if (ch == ST_BASE64_PAD_CHAR) {		/* We got a pad char. */
		ch = (char)*src++;		/* Skip it, get next. */
		switch (state) {
		case 0:		/* Invalid = in first position */
		case 1:		/* Invalid = in second position */
			/**ST_BASE64__prf("inv pad char st0/1\n");**/
			/*return ST_ERR_IDAT;*/

			/* I tolerate this error here... */
			break;
		case 2:		/* Valid, means one byte of info */
			/* Skip any number of spaces. */
			for (; ch != '\0'; ch = *src++)
				if (! isspace((unsigned char)ch))
					break;
			/* Make sure there is another trailing = sign. */
			if (ch != ST_BASE64_PAD_CHAR) {
				/**ST_BASE64__prf("inv pad char st2\n");**/
				return ST_ERR_IDAT;
			}
			ch = *src++;		/* Skip the = */
			/* Fall through to "single trailing =" case. */
			/* FALLTHROUGH */

		case 3:		/* Valid, means two bytes of info */
			/*
			 * We know this char is an =.  Is there anything but
			 * whitespace after it?
			 */
			for (; ch != '\0'; ch = (char)*src++)
				if (! isspace((unsigned char)ch)) {
					/**ST_BASE64__prf("inv char st3\n");**/
					return ST_ERR_IDAT;
				}

			/*
			 * Now make sure for cases 2 and 3 that the "extra"
			 * bits that slopped past the last full byte were
			 * zeros.  If we don't check them, they become a
			 * subliminal channel.
			 */
			/*if (target && target[tarindex] != 0)
				return (-1);*/
			if (pOB->bs_buf.pBBuf != NULL && pOB->bs_buf.pBBuf[pOB->used] != 0) {
				/**ST_BASE64__prf("inv char != 0\n");**/
				return ST_ERR_IDAT;
			}
		}
	} else {
		/*
		 * We ended by seeing the end of the string.  Make sure we
		 * have no partial bytes lying around.
		 */
		if (state != 0) {
			/**ST_BASE64__prf("inv state\n");**/
			return ST_ERR_IDAT;
		}
	}

	/*return (tarindex);*/
	return ST_ERR_SUCC;
}
#endif

/*----------------------------------------------------------------------------*/

/** encode binobj, by Tas
 **   this is the same code as in ST_BASE64__ntop()
 **   the only difference is that we use pStrrd for reading the input
 **   and pBinDatOut for writing the output  */
static Tst_err
ST_BASE64__ntop_binobj(Tst_streamrd *pStrrd,
		const Tst_bool withNULterm, Tst_binobj *pBinDatOut)
{
	Tst_err    res;
	Tst_buf    inpBuf[1024 * 3],
	           outpBuf[4096],
	           outpPadd[5],
	           *pIB,
	           *pIBend,
	           *pOB;
	Tst_uint32 rdFromStr = 0;

	while (! st_streamrd_isEOF(pStrrd)) {
		st_streamrd_rdAheadBuffer(pStrrd, sizeof(inpBuf), inpBuf, &rdFromStr);
		rdFromStr -= (rdFromStr % 3);
		if (rdFromStr == 0)
			break;
		st_streamrd_rdSkipBytes(pStrrd, rdFromStr, ST_B_TRUE);
		pIB    = inpBuf;
		pIBend = pIB + rdFromStr;
		pOB    = outpBuf;

		do {
			pOB[0] = pIB[0] >> 2;
			pOB[1] = ((pIB[0] & 0x03) << 4) + (pIB[1] >> 4);
			pOB[2] = ((pIB[1] & 0x0f) << 2) + (pIB[2] >> 6);
			pOB[3] = pIB[2] & 0x3f;

			pOB[0] = ST_BASE64_CSET[pOB[0]];
			pOB[1] = ST_BASE64_CSET[pOB[1]];
			pOB[2] = ST_BASE64_CSET[pOB[2]];
			pOB[3] = ST_BASE64_CSET[pOB[3]];

			pIB += 3;
			pOB += 4;
		} while (pIB < pIBend);

		res = st_binobj_appendData(pBinDatOut,
				outpBuf, (Tst_uint32)(pOB - outpBuf));
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* Now we worry about padding */
	pOB = outpPadd;
	if (! st_streamrd_isEOF(pStrrd)) {
		/* Get what's left */
		inpBuf[0] = inpBuf[1] = inpBuf[2] = '\0';
		st_streamrd_rdBuffer_noCRC(pStrrd, 3, inpBuf, &rdFromStr);

		if (rdFromStr > 0) {
			outpBuf[0] = inpBuf[0] >> 2;
			outpBuf[1] = ((inpBuf[0] & 0x03) << 4) + (inpBuf[1] >> 4);
			outpBuf[2] = ((inpBuf[1] & 0x0f) << 2) + (inpBuf[2] >> 6);

			*pOB++ = ST_BASE64_CSET[outpBuf[0]];
			*pOB++ = ST_BASE64_CSET[outpBuf[1]];
			if (rdFromStr == 1)
				*pOB++ = ST_BASE64_PAD_CHAR;
			else
				*pOB++ = ST_BASE64_CSET[outpBuf[2]];
			*pOB++ = ST_BASE64_PAD_CHAR;
		}
	}

	if (withNULterm)
		*pOB++ = '\0';

	return st_binobj_appendData(pBinDatOut,
			outpPadd, (Tst_uint32)(pOB - outpPadd));
}

/*----------------------------------------------------------------------------*/

/** decode binobj, by Tas
 **   this is the same code as in ST_BASE64__pton()
 **   the only difference is that we use pStrrd for reading the input
 **   and pBinDatOut for writing the output  */
static Tst_err
ST_BASE64__pton_binobj(Tst_streamrd *pStrrd, Tst_binobj *pBinDatOut)
{
	Tst_err    res;
	Tst_buf    inpBuf[4096],
	           outpBuf[4096],
	           *pIB    = NULL,
	           *pOB    = outpBuf,
	           *pOBend = pOB + sizeof(outpBuf);
	Tst_uint32 avail   = 0,
	           used;
	int        state   = 0,
	           ch;
	char const *pPos;

	memset(outpBuf, 0, sizeof(outpBuf));

	while (! st_streamrd_isEOF(pStrrd) || avail > 0) {
		if (avail == 0) {
			st_streamrd_rdBuffer_noCRC(pStrrd, sizeof(inpBuf),
					inpBuf, &avail);
			if (avail == 0)
				break;
			pIB = inpBuf;
		}
		ch = (int)*pIB++;
		--avail;
		if (ch == '\0' || ch == ST_BASE64_PAD_CHAR)
			break;

		if (isspace((unsigned char)ch))   /* Skip whitespace anywhere */
			continue;

		pPos = strchr(ST_BASE64_CSET, ch);
		if (pPos == NULL)  /* A non-base64 character */
			return ST_ERR_IDAT;

		switch (state) {
		case 0:
			*pOB  = (pPos - ST_BASE64_CSET) << 2;
			state = 1;
			break;
		case 1:
			*pOB      |= (pPos - ST_BASE64_CSET) >> 4;
			*(pOB + 1) = ((pPos - ST_BASE64_CSET) & 0x0f) << 4;
			++pOB;
			state = 2;
			break;
		case 2:
			*pOB      |=  (pPos - ST_BASE64_CSET) >> 2;
			*(pOB + 1) = ((pPos - ST_BASE64_CSET) & 0x03) << 6;
			++pOB;
			state = 3;
			break;
		case 3:
			*pOB |= (pPos - ST_BASE64_CSET);
			++pOB;
			state = 0;
			break;
		default:  /* invalid state */
			ST_ASSERTN_NUM(ST_ERR_FAIL, 0 != 1)  /* cause exit() here */
			break;
		}

		if (pOB + 16 > pOBend) {
			/* flush complete bytes */
			used = (Tst_uint32)(pOB - outpBuf);
			res  = st_binobj_appendData(pBinDatOut, outpBuf, used);
			if (res != ST_ERR_SUCC)
				return res;
			/* */
			memmove(outpBuf, &outpBuf[used], sizeof(outpBuf) - used);
			pOB = outpBuf;
		}
	}

	if (pOB > outpBuf) {
		/* flush remaining complete bytes */
		used = (Tst_uint32)(pOB - outpBuf);
		res  = st_binobj_appendData(pBinDatOut, outpBuf, used);
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* we skip the entire padding and
	 *   error checking part here for simplicity  */

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_W_BASE64_DEB_ == 1)
/* Debugging, by Tas */
static void ST_BASE64__prf(const char *pMsg, ...)
{
	va_list args;

	va_start(args, pMsg);
	vprintf(pMsg, args);
	va_end(args);
}
#endif

/******************************************************************************/
