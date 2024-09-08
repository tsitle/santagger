/******************************************************************************/
/* sys_fnc.c                    [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* A few system functions                                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_SYSFNC_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "src/includes/common/sys_fnc.h"

/*
// System-Includes
*/
#if defined(WIN32)
#	include <windows.h>
#else
#	include <sys/types.h>   /* time_t */
#	include <time.h>        /* time(),nanosleep(),... */
#	include <sys/time.h>    /* timeval */
/*	include <sys/resource.h>*/
#endif
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */
#include <ctype.h>       /* tolower(), toupper() */
/*include <netinet/in.h>*/  /* ntohl() */
#if (ST_SYSFNC_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
const Tst_uint16 st_sysfnc_g_bigEndTest = 0x4321;
#endif  /* CONFIG_ST_ALL_DEBUG_ADD */

/*----------------------------------------------------------------------------*/

static Tst_bool
ST_SYSFNC__strcmpN(const Tst_bool caseSensitive,
                   const Tst_bool useMaxlen, const Tst_uint32 maxlen,
                   ST_OPTARG(const Tst_str *pStr1),
                   ST_OPTARG(const Tst_str *pStr2));
/** */
#if (ST_SYSFNC_DEB_ == 1)
static void ST_SYSFNC__prf(char *pFmt, ...);
static void ST_SYSFNC__prE(char *pFmt, ...);
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get string length (assuming string is ISO-encoded)
 *
 * pStrISO   may be NULL
 */
Tst_uint32
st_sysStrlen(ST_OPTARG(const Tst_str *pStrISO))
{
	Tst_str const *pC = pStrISO;

	if (pStrISO == NULL)
		return 0;

	while (*pC)
		++pC;
	return (pC - pStrISO);
}

/*----------------------------------------------------------------------------*/

/*
 * Copy pSrc to *ppDst, memory for *ppDst will be allocated
 */
Tst_err
st_sysStrcpy(ST_OPTARG(const Tst_str *pSrc), Tst_str **ppDst)
{
	Tst_uint32 slen;

	ST_ASSERTN_IARG(ppDst == NULL)

	slen = st_sysStrlen(pSrc) + 1;
	ST_REALLOC(*ppDst, Tst_str*, slen, 1)
	if (*ppDst == NULL)
		return ST_ERR_OMEM;
	if (pSrc == NULL)
		(*ppDst)[0] = 0;
	else
		memcpy(*ppDst, pSrc, slen);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Convert pStr to uppercase
 */
void
st_sysStrToUpper(ST_OPTARG(Tst_str *pStr))
{
	Tst_str *pCh = pStr;

	if (pStr == NULL)
		return;
	while (*pCh) {
		*pCh = (Tst_str)toupper((int)*pCh);
		++pCh;
	}
}

/*
 * Convert pStr to lowercase
 */
void
st_sysStrToLower(ST_OPTARG(Tst_str *pStr))
{
	Tst_str *pCh = pStr;

	if (pStr == NULL)
		return;
	while (*pCh) {
		*pCh = (Tst_str)tolower((int)*pCh);
		++pCh;
	}
}

Tst_str
st_sysCharToUpper(Tst_str ch)
{
	return (Tst_str)toupper((int)ch);
}

Tst_str
st_sysCharToLower(Tst_str ch)
{
	return (Tst_str)tolower((int)ch);
}

/*----------------------------------------------------------------------------*/

/*
 * Compare pStr1 and pStr2, case-(in)sensitive
 *
 * returns: true if equal
 */
Tst_bool
st_sysStrcmp(const Tst_bool caseSensitive,
		ST_OPTARG(const Tst_str *pStr1), ST_OPTARG(const Tst_str *pStr2))
{
	return ST_SYSFNC__strcmpN(caseSensitive, ST_B_FALSE, 0, pStr1, pStr2);
}

Tst_bool
st_sysStrcmpN(const Tst_bool caseSensitive, const Tst_uint32 maxlen,
		ST_OPTARG(const Tst_str *pStr1), ST_OPTARG(const Tst_str *pStr2))
{
	return ST_SYSFNC__strcmpN(caseSensitive, ST_B_TRUE, maxlen, pStr1, pStr2);
}

/*----------------------------------------------------------------------------*/

/*
Tst_uint32 sysBig2LittleEnd(const Tlsize val, const Tst_uint32 valSz)
{
	if (valSz < 1 || valSz > 4 || val == 0)
		return 0;
	return (Tst_uint32)ntohl((Tlsize)(val << (32 - (8 * valSz))));
}

Tst_uint32 sysLittle2BigEnd(const Tlsize val, const Tst_uint32 valSz)
{
	if (valSz < 1 || valSz > 4 || val == 0)
		return 0;
	return (Tst_uint32)htonl((Tlsize)val << (32 - 8 * valSz));
}
*/

/*
 * Reverse Byte-Order for 'short' 2byte type
 */
Tst_uint16
st_sysReverseByteOrder_UI16(const Tst_uint16 val)
{
	Tst_uint16 rval;

	/* big2little or vice versa */
	/* ab --> ba */

	((Tst_buf*)&rval)[0] = ((Tst_buf*)&val)[1];  /* swap 0 and 1 */
	((Tst_buf*)&rval)[1] = ((Tst_buf*)&val)[0];  /* swap 1 and 0 */

	return rval;
}

/*
 * Reverse Byte-Order for 'int' 4byte type
 */
Tst_uint32
st_sysReverseByteOrder_UI32(const Tst_uint32 val)
{
	Tst_uint32 rval;
	Tst_buf    *pValP = (Tst_buf*)&val,
	           *pResP = (Tst_buf*)&rval;

	/* big2little or vice versa */
	/* abcd --> dcba */

	pResP[0] = pValP[3];  /* swap 0 and 3 */
	pResP[1] = pValP[2];  /* swap 1 and 2 */
	pResP[2] = pValP[1];  /* swap 2 and 1 */
	pResP[3] = pValP[0];  /* swap 3 and 0 */

	return rval;
}

/*
 * Reverse Byte-Order for 8byte buffer type
 *
 * pVal   MUST be != NULL
 * valSz  MUST be 8
 */
Tst_buf*
st_sysReverseByteOrder_LL(Tst_buf *pVal, const Tst_uint32 valSz)
{
	Tst_byte tmp;

	ST_ASSERTN_NULL(pVal == NULL || valSz != 8)  /* ret NULL */

	/* big2little or vice versa */
	/* abcdefgh --> hgfedcba */

	tmp     = pVal[7];  /* swap 0 and 7 */
	pVal[7] = pVal[0];
	pVal[0] = tmp;
	tmp     = pVal[6];  /* swap 1 and 6 */
	pVal[6] = pVal[1];
	pVal[1] = tmp;
	tmp     = pVal[5];  /* swap 2 and 5 */
	pVal[5] = pVal[2];
	pVal[2] = tmp;
	tmp     = pVal[4];  /* swap 3 and 4 */
	pVal[4] = pVal[3];
	pVal[3] = tmp;

	return pVal;
}

/*----------------------------------------------------------------------------*/

/*
 * Get system time
 */
double
st_sysGetTime()
{
#	ifdef WIN32  /* untested */
	LARGE_INTEGER t,
	              f;

	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double(t.QuadPart) / double(f.QuadPart));
#	else
#		if (HAVE_GETTIMEOFDAY == 1 && HAVE_STRUCT_TIMEVAL_TV_SEC == 1 && \
				HAVE_STRUCT_TIMEVAL_TV_USEC == 1)
	struct timeval  t;
	struct timezone tzp;

	gettimeofday(&t, &tzp);
	return (t.tv_sec + t.tv_usec * (1e-6));
#		else
	time_t timeV = 0;  /* time_t seems to be long int */

	time(&timeV);
	return (double)timeV;
#		endif
#	endif
}

/*----------------------------------------------------------------------------*/

/*
 * Puts current process at sleep
 */
void
st_sysSleepMS(const Tst_uint32 millisecs)
{
#	if (HAVE_STRUCT_TIMESPEC_TV_SEC == 1 && \
			HAVE_STRUCT_TIMESPEC_TV_NSEC == 1)
	struct timespec ts;

	if (millisecs > 0) {
		ts.tv_sec  = (time_t)((millisecs - millisecs % 1000) / 1000);
		ts.tv_nsec = (long)((millisecs - ts.tv_sec * 1000) * 1000 * 1000);
		nanosleep(&ts, NULL);
	}
#	endif
}

/*----------------------------------------------------------------------------*/

/*
 * Initialize Pseudo-Random-Number generator
 *   The PRNG is init'd with the current time,
 *   but you can use an additional seed
 */
void
st_sysInitRand(const Tst_uint32 optionalSeed)
{
	srand((Tst_uint32)time(NULL) | optionalSeed);
}

/*
 * Get Pseudo-Random-Number as UINT
 *
 * returns: value between min and max ( min <= x <= max ) as UINT
 */
Tst_uint32
st_sysGetRand(const Tst_uint32 min, const Tst_uint32 max)
{
	Tst_uint32 y,
	           r;

	ST_ASSERTN_NUM(0, min > max)  /* ret 0 */

	if (max == 0xffffffff) {
		if (min == 0)
			r = max;
		else
			r = (max - min) + 1;
	} else
		r = (max + 1) - min;
	y = (Tst_uint32)(r * ((double)rand() / (double)RAND_MAX));
	return (min + y);
}

/*Tst_uint32 st_sysGetRandOLD(const Tst_uint32 min, const Tst_uint32 max)
{
	double     x;
	Tst_uint32 y;

	if (min > max)
		return 0;
	* return (min +
			(Tst_uint32)((double)((Tlsize)max + 1 - min * rand()) /
				(RAND_MAX + (double)min))); *
	x = (double)((Tlsize)max - min + 1);
	y = (Tst_uint32)((x * (double)rand()) / (double)(RAND_MAX + min));
	return min + y;
}*/

/*
 * Get Pseudo-Random-Number as DOUBLE
 *
 * returns: value between min and max ( min <= x <= max ) as DOUBLE
 */
double
st_sysGetRandDbl(const double min, const double max)
{
	double y,
	       r;

	ST_ASSERTN_NUM(0.0, min > max)  /* ret 0.0 */

	/* j = 1 + (int) (10.0 * (rand() / (RAND_MAX + 1.0))) */

	/* --> 0 <= y <= |max-min| */
	r = (max + 1.0) - min;
	y = r * ((double)rand() / (double)RAND_MAX);
	/* --> min <= y <= max */
	y += min;
	if (y > max) {  /* because of rounding errors */
		/**ST_SYSFNC__prE("st_sysGetRandDbl(): y > max (%f > %f) lev1\n",
				y, max);**/
		y = min + ((y - max) / r);
		if (y > max) {  /* well, so be it */
			/**ST_SYSFNC__prE("st_sysGetRandDbl(): y > max (%f > %f) lev2\n",
					y, max);**/
			y = max;
		} else
			if (y < min) {  /* because of rounding errors */
				/**ST_SYSFNC__prE("st_sysGetRandDbl(): y < min (%f < %f) lev2\n",
						y, min);**/
				y = min;
			}
	} else
		if (y < min) {  /* because of rounding errors */
			/**ST_SYSFNC__prE("st_sysGetRandDbl(): y < min (%f < %f) lev1\n",
					y, min);**/
			y = min;
		}
	return y;
}

/*----------------------------------------------------------------------------*/

/*
 * Search for pRepl in pBuf
 *
 * pBuf      MUST be set
 * pSearch   MUST be set
 * pOffs     may be NULL
 *
 * returns: true if found
 */
Tst_bool
st_sysBufSearch(const Tst_buf *pBuf, const Tst_uint32 bSz,
        const Tst_buf *pSearch, const Tst_uint32 sSz,
        ST_OPTARG(Tst_uint32 *pOffs))
{
	Tst_uint32 y,
	           z;
	Tst_bool   fnd = ST_B_FALSE;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBuf == NULL || pSearch == NULL)

	if (pOffs != NULL)
		*pOffs = 0;
	if (bSz >= sSz && sSz > 0) {
		for (y = 0; y < bSz; y++) {
			if (y + sSz > bSz)
				break;
			z   = y;
			fnd = ST_B_TRUE;
			while (fnd && y + sSz > z) {
				if (pBuf[z] != pSearch[z - y])
					fnd = ST_B_FALSE;
				else
					++z;
			}
			if (fnd) {
				if (pOffs != NULL)
					*pOffs = y;
				break;
			}
		}
	}
	return fnd;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
ST_SYSFNC__strcmpN(const Tst_bool caseSensitive,
		const Tst_bool useMaxlen, const Tst_uint32 maxlen,
		ST_OPTARG(const Tst_str *pStr1), ST_OPTARG(const Tst_str *pStr2))
{
	Tst_bool      resB = ST_B_TRUE;
	Tst_str const *pS1 = pStr1,
	              *pS2 = pStr2;
	Tst_uint32    x    = 0;

	if (pStr1 == NULL || pStr2 == NULL)
		return ST_B_FALSE;

	while ((! useMaxlen || x < maxlen) && *pS1) {
		if (*pS2 == 0x00 ||
				(caseSensitive && *pS1 != *pS2) ||
				(! caseSensitive && tolower((int)*pS1) != tolower((int)*pS2))) {
			resB = ST_B_FALSE;
			break;
		}
		++pS1;
		++pS2;
		++x;
	}

	return (resB && (useMaxlen || (*pS1 == 0 && *pS2 == 0)));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_SYSFNC_DEB_ == 1)
static void ST_SYSFNC__prf(char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

static void ST_SYSFNC__prE(char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}
#endif

/******************************************************************************/
