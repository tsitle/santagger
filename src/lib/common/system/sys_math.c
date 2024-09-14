/******************************************************************************/
/* sys_math.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for doing some math with 64bit integers                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_SYSMATH_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "src/includes/common/sys_math.h"

/*
// System-Includes
*/
#if (ST_SYSMATH_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** */
#define LOC_LT_(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
static const Tst_uint32 ST_SYSILOGTABLE256[256] = {
	/*Error*/0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	LOC_LT_(4), LOC_LT_(5), LOC_LT_(5),
	LOC_LT_(6), LOC_LT_(6), LOC_LT_(6), LOC_LT_(6),
	LOC_LT_(7), LOC_LT_(7), LOC_LT_(7), LOC_LT_(7),
	LOC_LT_(7), LOC_LT_(7), LOC_LT_(7), LOC_LT_(7)
};
#undef LOC_LT_

#if (CONFIG_ST_ALL_HAVE64BIT != 1)
/** */
/*static ST_INLINE_S
Tst_uint32 ST_SYSMATH__uint64_mult16bit(const Tst_uint32 factA,
                                        const Tst_uint32 factB);*/
#endif

/** */
static ST_INLINE_S Tst_uint32
ST_SYSMATH__strlen(const char *pStr, const Tst_uint32 strSz);
/** */
static ST_INLINE_S void
ST_SYSMATH__addNStrToNStr(Tst_str *pNumStr,
                          const Tst_uint32 numStrSz, const char *pAdd,
                          const Tst_uint32 addSz);
/** */
static ST_INLINE_S void
ST_SYSMATH__mulNStrBy2(Tst_str *pNumStr, const Tst_uint32 numStrSz);

/** */
#if (ST_SYSMATH_DEB_ == 1)
static void ST_SYSMATH__prf(char *pFmt, ...);
static void ST_SYSMATH__prE(char *pFmt, ...);
#endif

#if (CONFIG_ST_ALL_HAVE64BIT == 1)
#	define LOC_CP_I64_TO_UI64_(varUI, varSI)  { \
				(varUI).nativeU64 = (Tst_uint64_native)(varSI).nativeS64; }
#	define LOC_CP_UI64_TO_I64_(varSI, varUI)  { \
				(varSI).nativeS64 = (Tst_int64_native)(varUI).nativeU64; }
#	define LOC_CP_I64_(varDst, varSrc)  { \
				(varDst).nativeS64 = (varSrc).nativeS64; }
#	define LOC_CP_UI64_(varDst, varSrc)  { \
				(varDst).nativeU64 = (varSrc).nativeU64; }
#	define LOC_GETSIGN_(varSI)  \
				(((Tst_uint64_native)(varSI).nativeS64 >> 63) != 1)
#else
#	define LOC_CP_I64_TO_UI64_(varUI, varSI)  { \
				(varUI).loU = (varSI).loS; \
				(varUI).hiU = (varSI).hiS; }
#	define LOC_CP_UI64_TO_I64_(varSI, varUI)  { \
				(varSI).loS = (varUI).loU; \
				(varSI).hiS = (varUI).hiU; }
#	define LOC_CP_I64_(varDst, varSrc)  { \
				(varDst).hiS = (varSrc).hiS; \
				(varDst).loS = (varSrc).loS; }
#	define LOC_CP_UI64_(varDst, varSrc)  { \
				(varDst).hiU = (varSrc).hiU; \
				(varDst).loU = (varSrc).loU; }
#	define LOC_GETSIGN_(varSI)  (((varSI).hiS >> 31) != 1)
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set low- and high-bits of pVal to set32_lo/hi
 */
void
st_sysUInt64_setLoHi(Tst_uint64 *pVal,
		const Tst_uint32 set32_lo, const Tst_uint32 set32_hi)
{
	ST_ASSERTN_VOID(pVal == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeU64 = ((Tst_uint64_native)set32_lo) |
			((Tst_uint64_native)set32_hi << 32);
#	else
	pVal->hiU = set32_hi;
	pVal->loU = set32_lo;
#	endif
}

/*
 * Set pVal to set32
 */
void
st_sysUInt64_setUI32(Tst_uint64 *pVal, const Tst_uint32 set32)
{
	ST_ASSERTN_VOID(pVal == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeU64 = (Tst_uint64_native)set32;
#	else
	pVal->loU = set32;
	pVal->hiU = 0;
#	endif
}

/*
 * Set pVal to pSet64
 */
void
st_sysUInt64_setUI64(Tst_uint64 *pVal, const Tst_uint64 *pSet64)
{
	ST_ASSERTN_VOID(pVal == NULL || pSet64 == NULL || pVal == pSet64)

	LOC_CP_UI64_((*pVal), (*pSet64))
}

/*
 * Set pVal to the value of the numeric string in pNumStr
 */
Tst_bool
st_sysUInt64_setDecStr(Tst_uint64 *pVal, ST_OPTARG(const Tst_str *pNumStr))
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
#	define LOC_ADD_  { \
				old        = pVal->loU; \
				pVal->loU += olderLo; \
				if (pVal->loU < old) \
					++pVal->hiU; \
				pVal->hiU += olderHi; \
				}
#	define LOC_ADD_10X_  { LOC_ADD_ LOC_ADD_ LOC_ADD_ LOC_ADD_ \
				LOC_ADD_ LOC_ADD_ LOC_ADD_ LOC_ADD_ LOC_ADD_ }
	Tst_uint32    old,
	              olderLo,
	              olderHi;
#	endif
	Tst_uint32    x      = 0;
	Tst_bool      errFnd = ST_B_FALSE,
	              sgnFnd = ST_B_FALSE;
	Tst_str const *pCh   = pNumStr;

	ST_ASSERTN_BOOL(ST_B_FALSE, pVal == NULL)

	ST_SYSMATH_STC_RSETUI64(*pVal)
	if (pNumStr == NULL)
		return ST_B_TRUE;

	while (*pCh != 0) {
		if (*pCh == '+') {
			if (sgnFnd || x > 0) {
				errFnd = ST_B_TRUE;
				break;
			}
			sgnFnd = ST_B_TRUE;
			++pCh;
			++x;
			continue;
		} else if (*pCh == 0)
			break;
		else if (*pCh < '0' || *pCh > '9') {
			errFnd = ST_B_TRUE;
			break;
		}
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		pVal->nativeU64 *= 10;
		pVal->nativeU64 += (Tst_uint64_native)(*pCh - '0');
#		else
		olderLo = pVal->loU;
		olderHi = pVal->hiU;
		LOC_ADD_10X_
		old = (Tst_uint32)(*pCh - '0');
		pVal->loU += old;
		if (pVal->loU < old)
			++pVal->hiU;
#		endif
		++pCh;
		++x;
	}
	return (! errFnd);
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
#	undef LOC_ADD_
#	undef LOC_ADD_10X_
#	endif
}

/*----------------------------------------------------------------------------*/

/*
 * Convert pVal into 32bit unsigned int by simply cropping the high bits
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
Tst_uint32
st_sysUInt64_toUI32(const Tst_uint64 *pVal, ST_OPTARG(Tst_bool *pOFflag))
{
	ST_ASSERTN_NUM(0, pVal == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = ((pVal->nativeU64 >> 32) != 0);
#		endif
	return (Tst_uint32)pVal->nativeU64;
#	else
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pVal->hiU != 0);
#		endif
	return pVal->loU;
#	endif
}

/*
 * Convert 64bit unsigned int to 64bit signed int
 */
void
st_sysUInt64_toI64(const Tst_uint64 *pValUI, Tst_int64 *pValSI)
{
	ST_ASSERTN_VOID(pValSI == NULL || pValUI == NULL)

	LOC_CP_UI64_TO_I64_((*pValSI), (*pValUI))
}

/*
 * Convert pVal into hex-string
 *
 * pVal     MUST be set
 * pHexStr  MUST be != NULL and at least 19 bytes big
 */
void
st_sysUInt64_toHexStr(const Tst_uint64 *pVal,
		Tst_str *pHexStr, const Tst_uint32 numStrSz)
{
	ST_ASSERTN_VOID(pVal == NULL || pHexStr == NULL || numStrSz < 19)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	snprintf((char*)pHexStr, numStrSz, "0x%08x%08x",
			(Tst_uint32)(pVal->nativeU64 >> 32),
			(Tst_uint32)((pVal->nativeU64 << 32) >> 32));
#	else
	snprintf((char*)pHexStr, numStrSz, "0x%08x%08x", pVal->hiU, pVal->loU);
#	endif
}

/*
 * Convert pVal into numeric-string  (using the Horner-Scheme)
 *
 * pVal     MUST be set
 * pNumStr  MUST be != NULL and at least 21 bytes big
 */
void
st_sysUInt64_toDecStr(const Tst_uint64 *pVal,
		Tst_str *pNumStr, const Tst_uint32 numStrSz)
{
	Tst_uint32 x,
	           bit    = 0;
	Tst_bool   msbFnd = ST_B_FALSE,
	           is1st  = ST_B_TRUE;

	ST_ASSERTN_VOID(pVal == NULL || pNumStr == NULL || numStrSz < 21)

	for (bit = 0; bit < numStrSz; bit++)
		pNumStr[bit] = 0;
	for (x = 64; x > 0; x--) {
		if (
#			if (CONFIG_ST_ALL_HAVE64BIT == 1)
				((pVal->nativeU64 >> (x - 1)) & 1) == 1
#			else
				(x > 32 &&
					((pVal->hiU >> (x - 32 - 1)) & 1) == 1) ||
				(x <= 32 &&
					((pVal->loU >> (x - 1)) & 1) == 1)
#			endif
					) {
			msbFnd = ST_B_TRUE;
			bit    = 1;
		} else
			bit = 0;
		if (! msbFnd)
			continue;
		/* */
		if (is1st) {
			pNumStr[0] = '1';
			is1st      = ST_B_FALSE;
		} else if (bit == 1)
			ST_SYSMATH__addNStrToNStr(pNumStr, numStrSz, "1", 2);
		if (x > 1)
			ST_SYSMATH__mulNStrBy2(pNumStr, numStrSz);
	}
	if (! msbFnd)
		pNumStr[0] = '0';
}

/*----------------------------------------------------------------------------*/

/*
 * Compare pVal64_a and val32_b
 *
 * returns:  0 if pVal64_a ==val32_b
 *          -1 if pVal64_a < val32_b
 *           1 if pVal64_a > val32_b
 */
int
st_sysUInt64_cmpUI32(const Tst_uint64 *pVal64_a,
		const Tst_uint32 valCmp32)
{
	ST_ASSERTN_NUM(0, pVal64_a == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pVal64_a->nativeU64 > (Tst_uint64_native)valCmp32)
		return 1;  /* a > b */
	else if (pVal64_a->nativeU64 < (Tst_uint64_native)valCmp32)
		return -1;  /* a < b */
#	else
	if (pVal64_a->hiU != 0 || pVal64_a->loU > valCmp32)
		return 1;  /* a > b */
	else if (pVal64_a->loU < valCmp32)
		return -1;  /* a < b */
#	endif
	return 0;
}

/*
 * Compare pVal64_a and pVal64_b
 *
 * returns:  0 if pVal64_a ==pVal64_b
 *          -1 if pVal64_a < pVal64_b
 *           1 if pVal64_a > pVal64_b
 */
int
st_sysUInt64_cmpUI64(const Tst_uint64 *pVal64_a,
		const Tst_uint64 *pVal64_b)
{
	ST_ASSERTN_NUM(0, pVal64_a == NULL || pVal64_b == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pVal64_a->nativeU64 > pVal64_b->nativeU64)
		return 1;  /* a > b */
	else if (pVal64_a->nativeU64 < pVal64_b->nativeU64)
		return -1;  /* a < b */
#	else
	if (pVal64_a->hiU > pVal64_b->hiU ||
			(pVal64_a->hiU == pVal64_b->hiU && pVal64_a->loU > pVal64_b->loU))
		return 1;  /* a > b */
	else if (pVal64_a->hiU < pVal64_b->hiU ||
			(pVal64_a->hiU == pVal64_b->hiU && pVal64_a->loU < pVal64_b->loU))
		return -1;  /* a < b */
#	endif
	return 0;
}

int
st_sysUInt64_cmpLoHi(const Tst_uint64 *pVal64_a,
		const Tst_uint32 valCmp_lo, const Tst_uint32 valCmp_hi)
{
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	Tst_uint64_native valB;
#	endif

	ST_ASSERTN_NUM(0, pVal64_a == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	valB = ((Tst_uint64_native)valCmp_lo) |
			((Tst_uint64_native)valCmp_hi << 32);
	if (pVal64_a->nativeU64 > valB)
		return 1;  /* a > b */
	else if (pVal64_a->nativeU64 < valB)
		return -1;  /* a < b */
#	else
	if (pVal64_a->hiU > valCmp_hi ||
			(pVal64_a->hiU == valCmp_hi && pVal64_a->loU > valCmp_lo))
		return 1;  /* a > b */
	else if (pVal64_a->hiU < valCmp_hi ||
			(pVal64_a->hiU == valCmp_hi && pVal64_a->loU < valCmp_lo))
		return -1;  /* a < b */
#	endif
	return 0;
}

/*----------------------------------------------------------------------------*/

/*
 * Add add32 to pVal
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
void
st_sysUInt64_addUI32(Tst_uint64 *pVal, const Tst_uint32 add32,
		ST_OPTARG(Tst_bool *pOFflag))
{
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	Tst_uint64 old;
#	endif

	ST_ASSERTN_VOID(pVal == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	old.nativeU64    = pVal->nativeU64;
#		endif
	pVal->nativeU64 += add32;
	/* overflow check */
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pVal->nativeU64 < old.nativeU64);
#		endif
#	else
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	old.loU = pVal->loU;
	old.hiU = pVal->hiU;
#		endif
	/* */
	pVal->loU += add32;
	if (pVal->loU < add32)
		++pVal->hiU;
	/* overflow check */
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pVal->hiU < old.hiU ||
					(pVal->hiU == old.hiU && pVal->loU < old.loU));
#		endif
#	endif
}

/*
 * Add pAdd64 to pVal
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
void
st_sysUInt64_addUI64(Tst_uint64 *pVal, const Tst_uint64 *pAdd64,
		ST_OPTARG(Tst_bool *pOFflag))
{
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	Tst_uint64 old;
#	endif

	ST_ASSERTN_VOID(pVal == NULL || pAdd64 == NULL || pVal == pAdd64)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	old.nativeU64    = pVal->nativeU64;
#		endif
	pVal->nativeU64 += pAdd64->nativeU64;
	/* overflow check */
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pVal->nativeU64 < old.nativeU64);
#		endif
#	else
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	old.loU = pVal->loU;
	old.hiU = pVal->hiU;
#		endif
	/* */
	pVal->loU += pAdd64->loU;
	if (pVal->loU < pAdd64->loU)
		++pVal->hiU;
	pVal->hiU += pAdd64->hiU;
	/* overflow check */
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pVal->hiU < old.hiU ||
					(pVal->hiU == old.hiU && pVal->loU < old.loU));
#		endif
#	endif
}

/*
 * Subtract sub32 from pVal
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
void
st_sysUInt64_subUI32(Tst_uint64 *pVal, const Tst_uint32 sub32,
		ST_OPTARG(Tst_bool *pOFflag))
{
	ST_ASSERTN_VOID(pVal == NULL)

	/* overflow check */
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		*pOFflag = (pVal->nativeU64 < (Tst_uint64_native)sub32);
#		else
		*pOFflag = (pVal->hiU == 0 && pVal->loU < sub32);
#		endif
#	endif

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeU64 -= sub32;
#	else
	if (pVal->loU < sub32)
		--pVal->hiU;
	pVal->loU -= sub32;
#	endif
}

/*
 * Subtract pSub64 from pVal
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
void
st_sysUInt64_subUI64(Tst_uint64 *pVal, const Tst_uint64 *pSub64,
		ST_OPTARG(Tst_bool *pOFflag))
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_uint32 lo,
	           hi;
#	endif

	ST_ASSERTN_VOID(pVal == NULL || pSub64 == NULL || pVal == pSub64)

	/* overflow check */
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		*pOFflag = (pVal->nativeU64 < pSub64->nativeU64);
#		else
		*pOFflag = (pVal->hiU < pSub64->hiU ||
				(pVal->hiU == pSub64->hiU && pVal->loU < pSub64->loU));
#		endif
#	endif

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeU64 -= pSub64->nativeU64;
#	else
	/* b - c = b + (-c) = b + (~c + 1) */
	/** d=(~c + 1) */
	lo = ~pSub64->loU;
	hi = ~pSub64->hiU;
	if (lo == 0xffffffff) {
		lo = 0;
		++hi;
	} else
		++lo;
	/** e=(b + d) */
	lo += pVal->loU;
	if (lo < pVal->loU)
		++pVal->hiU;
	pVal->loU  = lo;
	pVal->hiU += hi;
#	endif
}

/*
 * Multiply fact32_a and fact32_b and store result in pRes
 *   Overflow is not possible
 *   (using Karatsuba algorithm)
 */
void
st_sysUInt64_multUI32s(Tst_uint64 *pRes,
		const Tst_uint32 fact32_a, const Tst_uint32 fact32_b)
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
#	define LOC_LO_(x)  (x & 0xffff)
#	define LOC_HI_(x)  ((x >> 16) /*& 0xffff*/)
#	define LOC_AHI_    words32[0]
#	define LOC_ALO_    words32[1]
#	define LOC_BHI_    words32[2]
#	define LOC_BLO_    words32[3]
#	define LOC_TX_     temps32[0]
#	define LOC_TY_     temps32[1]
#	define LOC_TZ0_    tz32[0]
#	define LOC_TZ1_    tz32[1]
#	define LOC_TZ2_    tz32[2]
	Tst_uint32 words32[4],
	           temps32[2],
	           tz32[3];
	Tst_uint64 tz1_64,
	           y_64;
#	endif

	ST_ASSERTN_VOID(pRes == NULL)

	/*if (fact32_a == 0 || fact32_b == 0) {
		ST_SYSMATH_STC_RSETUI64((*pRes))
		return;
	}*/

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pRes->nativeU64 = (Tst_uint64_native)fact32_a * (Tst_uint64_native)fact32_b;
#	else
	/*if (sysILog2_UI32(fact32_a) + sysILog2_UI32(fact32_b) < 31) {*/
	if (fact32_a < 0x10000 && fact32_b < 0x10000) {
		pRes->hiU = 0;
		pRes->loU = fact32_a * fact32_b;
		return;
	}
	/*
	 *  r := ((aH*2^16) + (aL)) * ((bH*2^16) + (bL))
	 *    := (aH*2^16) * (bH*2^16) +
	 *       (aH*2^16) * (bL)      + (aL) * (bH*2^16) +
	 *       (aL)      * (bL)
	 *    := (aH * bH * 2^32) +
	 *       ((aH * bL + aL * bH) * 2^16) +
	 *       (aL * bL)
	 *  [(aH * bL + aL * bH) = (aH + aL) * (bH + bL) -
	 *                         (aH * bH) -
	 *                         (aL * bL) ]
	 *  z2 := (aH * bH)
	 *  z0 := (aL * bL)
	 *  z1 := (aH + aL) * (bH + bL) - z2 - z0
	 *  r  := (z2 * 2^32) + (z1 * 2^16) + z0
	 *
	 *   tz2<<8 XXXXyyyy
	 *   tz1<<4     HHHHllll
	 *   tz0<<0         AAAAbbbb
	 */
	LOC_AHI_ = LOC_HI_(fact32_a);
	LOC_ALO_ = LOC_LO_(fact32_a);
	LOC_BHI_ = LOC_HI_(fact32_b);
	LOC_BLO_ = LOC_LO_(fact32_b);
	/* */
	LOC_TZ2_ = LOC_AHI_ * LOC_BHI_;
	LOC_TZ0_ = LOC_ALO_ * LOC_BLO_;
	/** tz1 := (aH + aL) * (bH + bL) - tz2 - tz0 */
	/*** */
	LOC_TX_ = LOC_AHI_ + LOC_ALO_;
	LOC_TY_ = LOC_BHI_ + LOC_BLO_;
#if 0
	if (LOC_TX_ < 0x10000 && LOC_TY_ < 0x10000) {
		LOC_TZ1_ = ST_SYSMATH__uint64_mult16bit(LOC_TX_, LOC_TY_) -
				LOC_TZ2_ - LOC_TZ0_;
		/* */
		LOC_TY_   = LOC_TZ1_ + LOC_HI_(LOC_TZ0_);
		pRes->loU = (LOC_LO_(LOC_TY_) << 16) | LOC_LO_(LOC_TZ0_);
		pRes->hiU = LOC_TZ2_ + LOC_HI_(LOC_TY_);
		return;
	}
#endif
	/*** */
	/**if (sysILog2_UI32(LOC_TX_) + sysILog2_UI32(LOC_TY_) > 30) {**/
	if (LOC_TX_ > 0xffff || LOC_TY_ > 0xffff) {
		ST_SYSMATH_STC_RSETUI64(tz1_64)
		st_sysUInt64_multUI32s(&tz1_64, LOC_TX_, LOC_TY_);
		st_sysUInt64_subUI32(&tz1_64, LOC_TZ2_, NULL);
		st_sysUInt64_subUI32(&tz1_64, LOC_TZ0_, NULL);
		/* y := tz1 + LOC_HI_(tz0) */
		y_64.loU = tz1_64.loU;
		y_64.hiU = tz1_64.hiU;
		st_sysUInt64_addUI32(&y_64, LOC_HI_(LOC_TZ0_), NULL);
		/* */
		pRes->loU = (LOC_LO_(y_64.loU) << 16) | LOC_LO_(LOC_TZ0_);
		pRes->hiU = LOC_TZ2_ + LOC_HI_(y_64.loU) + (LOC_LO_(y_64.hiU) << 16);
		return;
	}
	/*** */
	LOC_TZ1_ = (LOC_TX_ * LOC_TY_) - LOC_TZ2_ - LOC_TZ0_;
	/* */
	LOC_TY_   = LOC_TZ1_ + LOC_HI_(LOC_TZ0_);
	pRes->loU = (LOC_LO_(LOC_TY_) << 16) | LOC_LO_(LOC_TZ0_);
	pRes->hiU = LOC_TZ2_ + LOC_HI_(LOC_TY_);
#	undef LOC_LO_
#	undef LOC_HI_
#	undef LOC_AHI_
#	undef LOC_ALO_
#	undef LOC_BHI_
#	undef LOC_BLO_
#	undef LOC_TX_
#	undef LOC_TY_
#	undef LOC_TZ0_
#	undef LOC_TZ1_
#	undef LOC_TZ2_
#	endif
}

/*
 * Multiply pVal and val32_b and store result in pVal
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
void
st_sysUInt64_multUI32(Tst_uint64 *pVal, const Tst_uint32 val32_b,
		ST_OPTARG(Tst_bool *pOFflag))
{
	Tst_uint64 tmp_a,
	           tmp_b;

	ST_ASSERTN_VOID(pVal == NULL)

	LOC_CP_UI64_(tmp_a, *pVal)
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	tmp_b.nativeU64 = (Tst_uint64_native)val32_b;
#	else
	tmp_b.loU = val32_b;
	tmp_b.hiU = 0;
#	endif
	st_sysUInt64_multUI64s(pVal, &tmp_a, &tmp_b, pOFflag);
}

/*
 * Multiply fact32_a and fact32_b and store result in pRes
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
void
st_sysUInt64_multUI64s(Tst_uint64 *pRes,
		const Tst_uint64 *pFact_a, const Tst_uint64 *pFact_b,
		ST_OPTARG(Tst_bool *pOFflag))
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	Tst_uint32 oldHi;
#		endif
	Tst_uint64 tmp;
#	endif

	ST_ASSERTN_VOID(pRes == NULL || pFact_a == NULL || pFact_b == NULL ||
			pRes == pFact_a || pRes == pFact_b)

	ST_SYSMATH_STC_RSETUI64((*pRes))
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pFact_a->nativeU64 == 0 || pFact_b->nativeU64 == 0) {
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
		if (pOFflag != NULL)
			*pOFflag = ST_B_FALSE;
#		endif
		return;
	}
	pRes->nativeU64 = pFact_a->nativeU64 * pFact_b->nativeU64;
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pRes->nativeU64 / pFact_a->nativeU64 != pFact_b->nativeU64);
#		endif
#	else
	if ((pFact_a->hiU == 0 && pFact_a->loU == 0) ||
			(pFact_b->hiU == 0 && pFact_b->loU == 0)) {
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
		if (pOFflag != NULL)
			*pOFflag = ST_B_FALSE;
#		endif
		return;
	}
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = ST_B_FALSE;
#		endif
	/* overflow check */
	/*if (sysILog2_UI64(pFact_a) + sysILog2_UI64(pFact_b) > 63) {
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
		if (pOFflag != NULL)
			*pOFflag = ST_B_TRUE;
#		endif
		return;
	}*/
	/*
	 *  r = ((aH*2^32) + (aL)) * ((bH*2^32) + (bL))
	 *    = (aH * bH * 2^64) +
	 *      ((aH * bL + aL * bH) * 2^32) +
	 *      (aL * bL)
	 *  --> aH*bH must be 0 --> aH or bH must be 0
	 *      this is ensured by
	 *        ILog2(pFact_a) + ILog2(pFact_b) > 63 --> res=0, return
	 *  --> in ((aH * bL + aL * bH) * 2^32)
	 *      either aH*bL or aL*bH is 0
	 *  let aH*bL be != 0, e.g. aH=0xffffffff, bL=0xffffffff
	 *    (then aH!=0, bH=0, bL!=0)
	 *    if ILog2(aH)+ILog2(bL)>31 then the result needs 33bits
	 *      and we get a carry of 1
	 *
	 *  --> r = ((aH * bL + aL * bH) * 2^32) +
	 *          (aL * bL)
	 */
	ST_SYSMATH_STC_RSETUI64(tmp)
	st_sysUInt64_multUI32s(pRes, pFact_a->loU, pFact_b->loU);
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	oldHi = pRes->hiU;
#		endif
	if (pFact_b->hiU == 0) {
		st_sysUInt64_multUI32s(&tmp, pFact_a->hiU, pFact_b->loU);
	} else {
		st_sysUInt64_multUI32s(&tmp, pFact_a->loU, pFact_b->hiU);
	}
	pRes->hiU += tmp.loU;
	/* overflow check */
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pRes->hiU < oldHi || tmp.hiU != 0) ||
				(sysILog2_UI64(pFact_a) + sysILog2_UI64(pFact_b) > 63);
#		endif
#	endif
}

/*
 * Divide pVal by div32
 */
void
st_sysUInt64_divUI32(Tst_uint64 *pVal, const Tst_uint32 div32)
{
	ST_ASSERTN_VOID(pVal == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (div32 == 0) {
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}
	pVal->nativeU64 = pVal->nativeU64 / (Tst_uint64_native)div32;
#	else
	st_sysUInt64_divUI32_rem(pVal, div32, NULL);
#	endif
}

/*
 * Divide pVal by div32 and store remainder in pRemainder
 *
 * pRemainder  may be NULL
 */
#define LOC_DIV32_REM_DOSUB_(dsTmpLo, dsTmpHi, dsVal, dsSub)  { \
			if ((dsVal).hiU == 0) { \
				(dsVal).loU -= (dsSub).loU; \
			} else { \
				dsTmpLo = (~(dsSub).loU); \
				dsTmpHi = (~(dsSub).hiU); \
				if (dsTmpLo == 0xffffffff) { \
					dsTmpLo = 0; \
					++dsTmpHi; \
				} else \
					++dsTmpLo; \
				dsTmpLo += (dsVal).loU; \
				if (dsTmpLo < (dsVal).loU) \
					++(dsVal).hiU; \
				(dsVal).loU  = dsTmpLo; \
				(dsVal).hiU += dsTmpHi; \
			} \
		}
void
st_sysUInt64_divUI32_rem(Tst_uint64 *pVal, const Tst_uint32 div32,
		ST_OPTARG(Tst_uint32 *pRemainder32))
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_uint64 b,
	           d,
	           tmp,
	           rem,
	           tres;
	Tst_uint32 subTemps[2];
#	endif

	ST_ASSERTN_VOID(pVal == NULL)

	if (div32 == 0) {
		if (pRemainder32 != NULL)
			*pRemainder32 = 0;
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pRemainder32 != NULL)
		*pRemainder32 = (Tst_uint32)(pVal->nativeU64 % (Tst_uint64_native)div32);
	pVal->nativeU64 = pVal->nativeU64 / (Tst_uint64_native)div32;
#	else
	/* init vars */
	rem.loU = pVal->loU;
	rem.hiU = pVal->hiU;
	b.loU   = div32;
	b.hiU   = 0;
	d.loU   = 1;
	d.hiU   = 0;
	ST_SYSMATH_STC_RSETUI64(tres)
	/* */
	if (pVal->hiU >= div32) {
		tres.hiU = pVal->hiU / div32;
		/* rem -= (uint64_t)(high * base) << 32 */
		tmp.hiU = tres.hiU * div32;
		tmp.loU = 0;
		LOC_DIV32_REM_DOSUB_(subTemps[0], subTemps[1], rem, tmp)
	}
	/* while b>0 && b<rem */
	if ((b.loU != 0 || b.hiU != 0) && (rem.loU != 0 || rem.hiU != 0))
		while (b.hiU < rem.hiU || (b.hiU == rem.hiU && b.loU < rem.loU)) {
			/* b = b + b ^=  b<<1 */
			b.hiU   = (b.loU >> 31) | (b.hiU << 1);
			b.loU <<= 1;
			/* d = d + d ^=  d<<1 */
			d.hiU   = (d.loU >> 31) | (d.hiU << 1);
			d.loU <<= 1;
		}
	/* */
	do {
		if (rem.hiU > b.hiU || (rem.hiU == b.hiU && rem.loU >= b.loU)) {
			/* rem >= b */

			/* rem -= b */
			LOC_DIV32_REM_DOSUB_(subTemps[0], subTemps[1], rem, b)
			/* tres += d */
			if (d.loU > 0) {
				tres.loU += d.loU;
				if (tres.loU < d.loU)
					++tres.hiU;
			}
			if (d.hiU > 0)
				tres.hiU += d.hiU;
		}
		/* */
		b.loU   = (b.hiU << 31) | (b.loU >> 1);
		b.hiU >>= 1;
		/* */
		if (d.hiU == 0) {
			d.loU >>= 1;
			if (d.loU == 0)
				break;
		} else {
			d.loU   = (d.hiU << 31) | (d.loU >> 1);
			d.hiU >>= 1;
		}
	} while (ST_B_TRUE);
	/* */
	if (pRemainder32 != NULL)
		*pRemainder32 = rem.loU;
	pVal->loU = tres.loU;
	pVal->hiU = tres.hiU;
#	endif
}
#undef LOC_DIV32_REM_DOSUB_

/*
 * Divide pVal by pDiv64
 */
void
st_sysUInt64_divUI64(Tst_uint64 *pVal, const Tst_uint64 *pDiv64)
{
	ST_ASSERTN_VOID(pVal == NULL || pDiv64 == NULL || pVal == pDiv64)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pDiv64->nativeU64 == 0) {
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}
	pVal->nativeU64 = pVal->nativeU64 / pDiv64->nativeU64;
#	else
	st_sysUInt64_divUI64_rem(pVal, pDiv64, NULL);
#	endif
}

/*
 * Divide pVal by pDiv64 and store remainder in pRemainder64
 *
 * pRemainder64  may be NULL
 */
void
st_sysUInt64_divUI64_rem(Tst_uint64 *pVal, const Tst_uint64 *pDiv64,
		ST_OPTARG(Tst_uint64 *pRemainder64))
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_uint64 newDivor,
	           orgDivi,
	           tmp;
	Tst_int32  ilog;
#	endif

	ST_ASSERTN_VOID(pVal == NULL || pDiv64 == NULL || pVal == pDiv64)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pDiv64->nativeU64 == 0) {
		if (pRemainder64 != NULL)
			ST_SYSMATH_STC_RSETUI64((*pRemainder64))
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}
	if (pRemainder64 != NULL)
		pRemainder64->nativeU64 = pVal->nativeU64 % pDiv64->nativeU64;
	pVal->nativeU64 = pVal->nativeU64 / pDiv64->nativeU64;
#	else
	if (pRemainder64 != NULL)
		ST_SYSMATH_STC_RSETUI64((*pRemainder64))
	if (pVal->hiU == 0) {  /* pVal=0.?, pDiv64=?.? */
		if (pDiv64->hiU != 0) {  /* pVal=0.?, pDiv64=1.?  --> pVal<pDiv64 */
			/**printf("64/64 case 1\n");**/
			if (pRemainder64 != NULL)
				pRemainder64->loU = pVal->loU;
			ST_SYSMATH_STC_RSETUI64((*pVal))
		} else {
			if (pDiv64->loU == 0) {  /* pVal=0.?, pDiv64=0.0 */
				/**printf("64/64 case 2\n");**/
				pVal->loU = 0;
			} else {                /* pVal=0.?, pDiv64=0.1 */
				/**printf("64/64 case 3\n");**/
				if (pRemainder64 != NULL)
					pRemainder64->loU = pVal->loU % pDiv64->loU;
				pVal->loU /= pDiv64->loU;
			}
		}
	} else {  /* pVal=1.?, pDiv64=?.? */
		if (pVal->hiU < pDiv64->hiU ||
				(pVal->hiU == pDiv64->hiU && pVal->loU < pDiv64->loU)) {
			/* pVal < pDiv64 */
			/**printf("64/64 case 4\n");**/
			if (pRemainder64 != NULL) {
				pRemainder64->loU = pVal->loU;
				pRemainder64->hiU = pVal->hiU;
			}
			ST_SYSMATH_STC_RSETUI64((*pVal))
			return;
		}
		/* pVal >= pDiv64 */

		newDivor.loU = pDiv64->loU;
		newDivor.hiU = pDiv64->hiU;
		orgDivi.loU  = pVal->loU;
		orgDivi.hiU  = pVal->hiU;

		/* shift dividend and/or divisor to right if necessary */
		if (newDivor.hiU != 0) {         /* pVal=1.?, pDiv64=1.? */
			ilog = sysILog2_UI32(newDivor.hiU) + 1;
			st_sysUInt64_shiftRight(pVal, ilog);
			st_sysUInt64_shiftRight(&newDivor, ilog);
		} else if (newDivor.loU == 0) {  /* pVal=1.?, pDiv64=0.0 */
			/**printf("64/64 case 5\n");**/
			ST_SYSMATH_STC_RSETUI64((*pVal))
			return;
		}

		/* */
		if (pVal->hiU == 0) {  /* pVal=0.?, newDivor=0.? */
			/**printf("64/64 case 6\n");**/
			pVal->loU /= newDivor.loU;
			/* get real remainder */
			if (pRemainder64 != NULL) {
				ST_SYSMATH_STC_RSETUI64(tmp)
				st_sysUInt64_multUI64s(&tmp, pVal, pDiv64, NULL);
				pRemainder64->loU = orgDivi.loU;
				pRemainder64->hiU = orgDivi.hiU;
				st_sysUInt64_subUI64(pRemainder64, &tmp, NULL);
			}
		} else {
			/**printf("64/64 case 7\n");**/
			st_sysUInt64_divUI32(pVal, newDivor.loU);
			/* check result and get real remainder */
			ST_SYSMATH_STC_RSETUI64(tmp)
			st_sysUInt64_multUI64s(&tmp, pVal, pDiv64, NULL);
			ilog = st_sysUInt64_cmpUI64(&tmp, &orgDivi);
			if (ilog > 0) {
				/**printf("64/64 case 8\n");**/
				st_sysUInt64_subUI32(pVal, 1, NULL);
				st_sysUInt64_multUI64s(&tmp, pVal, pDiv64, NULL);
				if (pRemainder64 != NULL) {
					pRemainder64->loU = orgDivi.loU;
					pRemainder64->hiU = orgDivi.hiU;
					st_sysUInt64_subUI64(pRemainder64, &tmp, NULL);
				}
			} else if (pRemainder64 != NULL && ilog < 0) {
				/**printf("64/64 case 9\n");**/
				pRemainder64->loU = orgDivi.loU;
				pRemainder64->hiU = orgDivi.hiU;
				st_sysUInt64_subUI64(pRemainder64, &tmp, NULL);
			}
		}
	}
#	endif
}

/*----------------------------------------------------------------------------*/

void
st_sysUInt64_modUI32(const Tst_uint64 *pDividend,
		const Tst_uint32 divisor32, Tst_uint32 *pRemainder32)
{
	Tst_uint64 divi;

	ST_ASSERTN_VOID(pDividend == NULL || pRemainder32 == NULL)

	LOC_CP_UI64_(divi, (*pDividend))
	st_sysUInt64_divUI32_rem(&divi, divisor32, pRemainder32);
}

void
st_sysUInt64_modUI64(const Tst_uint64 *pDividend,
		const Tst_uint64 *pDivisor, Tst_uint64 *pRemainder)
{
	Tst_uint64 divi;

	ST_ASSERTN_VOID(pDividend == NULL || pDivisor == NULL || pRemainder == NULL)

	LOC_CP_UI64_(divi, (*pDividend))
	st_sysUInt64_divUI64_rem(&divi, pDivisor, pRemainder);
}

/*----------------------------------------------------------------------------*/

/*
 * Shift pVal sh2r-bits to right
 */
void
st_sysUInt64_shiftRight(Tst_uint64 *pVal, const Tst_int32 sh2r)
{
	ST_ASSERTN_VOID(pVal == NULL)
	/*if (sh2r > 63) {
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}*/

	if (sh2r < 0)
		st_sysUInt64_shiftLeft(pVal, -sh2r);
	else if (sh2r > 0) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		pVal->nativeU64 >>= sh2r;
#		else
		/* hi ABCDEFGH lo JKLMNPQR >> 3
		 * --> lo=(hi FGH00000 | lo 000JKLMN)=FGHJKLMN
		 *     hi=000ABCDE
		 * = hi 000ABCDE lo FGHJKLMN  */
		if (sh2r < 32) {
			if (pVal->hiU == 0)
				pVal->loU >>= sh2r;
			else {
				pVal->loU   = (pVal->hiU << (32 - sh2r)) | (pVal->loU >> sh2r);
				pVal->hiU >>= sh2r;
			}
		} else {
			if (pVal->hiU == 0)
				pVal->loU = 0;
			else {
				pVal->loU = (pVal->hiU >> (32 + sh2r - 64));
				pVal->hiU = 0;
			}
		}
#		endif
	}
}

/*
 * Shift pVal sh2l-bits to left
 */
void
st_sysUInt64_shiftLeft(Tst_uint64 *pVal, const Tst_int32 sh2l)
{
	ST_ASSERTN_VOID(pVal == NULL)
	/*if (sh2l > 63) {
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}*/

	if (sh2l < 0)
		st_sysUInt64_shiftRight(pVal, -sh2l);
	else if (sh2l > 0) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		pVal->nativeU64 <<= sh2l;
#		else
		/* hi ABCDEFGH lo JKLMNPQR << 5
		 * --> hi=(hi FGH00000 | lo 000JKLMN)=FGHJKLMN
		 *     lo=PQR00000
		 * = hi FGHJKLMN lo PQR00000  */
		if (sh2l < 32) {
			if (pVal->loU == 0)
				pVal->hiU <<= sh2l;
			else {
				pVal->hiU   = (pVal->loU >> (32 - sh2l)) | (pVal->hiU << sh2l);
				pVal->loU <<= sh2l;
			}
		} else {
			if (pVal->loU == 0)
				pVal->hiU = 0;
			else {
				pVal->hiU = (pVal->loU << (32 + sh2l - 64));
				pVal->loU = 0;
			}
		}
#		endif
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Return log2(val)
 *
 * log2( 0) = Error
 * log2( 1) = 0   log2( 2) = 1   log2( 3) = 1   log2( 4) = 2
 * log2( 5) = 2   log2( 6) = 2   log2( 7) = 2   log2( 8) = 3
 * log2( 9) = 3   log2(10) = 3   log2(11) = 3   log2(12) = 3
 * log2(13) = 3   log2(14) = 3   log2(15) = 3   log2(16) = 4
 * log2(17) = 4   log2(18) = 4   ...
 */
Tst_uint32
sysILog2_UI8(const Tst_byte val)
{
	if (val < 1)
		return 0;

	return ST_SYSILOGTABLE256[val];
}

Tst_uint32
sysILog2_UI32(const Tst_uint32 val)
{
	register Tst_uint32 t,
	                    tt;

	if (val < 1)
		return 0;

	if ((tt = val >> 16) != 0)
		return ((t = tt >> 8) ? 24 + ST_SYSILOGTABLE256[t] :
				16 + ST_SYSILOGTABLE256[tt]);
	return ((t = val >> 8) ?
			8 + ST_SYSILOGTABLE256[t] :
			ST_SYSILOGTABLE256[val]);
	/* above algo is ~7 times faster than
	 *   Tst_uint32 l = 0;
	 *   if (val < 1) return 0;
	 *   while (val >>= 1) ++l;
	 *   return l;
	*/
}

Tst_uint32
sysILog2_UI64(const Tst_uint64 *pVal)
{
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	Tst_uint32 tmp;
#	endif

	ST_ASSERTN_NUM(0, pVal == NULL)  /* ret 0 */
	if (st_sysUInt64_cmpUI32(pVal, 1) < 0)
		return 0;

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	/* high */
	tmp = (pVal->nativeU64 >> 32);
	if (tmp > 0)
		return (sysILog2_UI32(tmp) + 32);
	/* low */
	tmp = (pVal->nativeU64 << 32) >> 32;
	return sysILog2_UI32(tmp);
#	else
	if (pVal->hiU > 0)
		return (sysILog2_UI32(pVal->hiU) + 32);
	return sysILog2_UI32(pVal->loU);
#	endif
}

/*----------------------------------------------------------------------------*/

/*
 * Reverse Byte-Order
 *
 * pVal   MUST be != NULL
 */
void
st_sysUInt64_reverseByteOrder(Tst_uint64 *pVal)
{
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	Tst_byte tmp;
	Tst_buf  *pValBuf;
#	else
	Tst_uint32 rvalLo = 0,
	           rvalHi = 0;
	Tst_buf    *pValBufLo,
	           *pValBufHi;
#	endif

	ST_ASSERTN_VOID(pVal == NULL)

	/* big2little or vice versa */
	/* abcdefgh --> hgfedcba */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pValBuf = (Tst_buf*)&pVal->nativeU64;
	/* */
	tmp        = pValBuf[7];  /* swap 0 and 7 */
	pValBuf[7] = pValBuf[0];
	pValBuf[0] = tmp;
	tmp        = pValBuf[6];  /* swap 1 and 6 */
	pValBuf[6] = pValBuf[1];
	pValBuf[1] = tmp;
	tmp        = pValBuf[5];  /* swap 2 and 5 */
	pValBuf[5] = pValBuf[2];
	pValBuf[2] = tmp;
	tmp        = pValBuf[4];  /* swap 3 and 4 */
	pValBuf[4] = pValBuf[3];
	pValBuf[3] = tmp;
#	else
	pValBufLo = (Tst_buf*)&pVal->loU;
	pValBufHi = (Tst_buf*)&rvalHi;
	/* */
	pValBufHi[0] = pValBufLo[3];  /* swap 0 and 3 */
	pValBufHi[1] = pValBufLo[2];  /* swap 1 and 2 */
	pValBufHi[2] = pValBufLo[1];  /* swap 2 and 1 */
	pValBufHi[3] = pValBufLo[0];  /* swap 3 and 0 */

	pValBufHi = (Tst_buf*)&pVal->hiU;
	pValBufLo = (Tst_buf*)&rvalLo;
	/* */
	pValBufLo[0] = pValBufHi[3];  /* swap 0 and 3 */
	pValBufLo[1] = pValBufHi[2];  /* swap 1 and 2 */
	pValBufLo[2] = pValBufHi[1];  /* swap 2 and 1 */
	pValBufLo[3] = pValBufHi[0];  /* swap 3 and 0 */

	pVal->loU = rvalLo;
	pVal->hiU = rvalHi;
#	endif
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool
st_sysSInt64_getSign(const Tst_int64 *pVal)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pVal == NULL)

	return LOC_GETSIGN_(*pVal);
}

void
st_sysSInt64_setSign(Tst_int64 *pVal, const Tst_bool sgn)
{
	ST_ASSERTN_VOID(pVal == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pVal->nativeS64 != 0 && LOC_GETSIGN_(*pVal) != sgn)
		pVal->nativeS64 *= (-1);
#	else
	if ((pVal->loS != 0 || pVal->hiS != 0) &&
			LOC_GETSIGN_(*pVal) != sgn) {
		/* 45 * -1 = ~45 + 1 */
		pVal->loS = ~pVal->loS;
		pVal->hiS = ~pVal->hiS;
		/* add 1 to pVal */
		pVal->loS += 1;
		if (pVal->loS == 0)
			++pVal->hiS;
	}
#	endif
}

void
st_sysSInt64_setLoHi(Tst_int64 *pVal,
		const Tst_uint32 set32_lo, const Tst_uint32 set32_hi)
{
	ST_ASSERTN_VOID(pVal == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeS64 = (Tst_int64_native)((Tst_uint64_native)set32_lo |
					((Tst_uint64_native)set32_hi << 32));
#	else
	pVal->loS = set32_lo;
	pVal->hiS = set32_hi;
#	endif
}

void
st_sysSInt64_setI32(Tst_int64 *pVal, const Tst_int32 setI32)
{
	ST_ASSERTN_VOID(pVal == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeS64 = (Tst_int64_native)setI32;
#	else
	pVal->loS = (Tst_uint32)setI32;
	pVal->hiS = 0;
	if (setI32 < 0)
		pVal->hiS = (Tst_uint32)-1;
#	endif
}

void
st_sysSInt64_setI64(Tst_int64 *pVal, const Tst_int64 *pSet64)
{
	ST_ASSERTN_VOID(pVal == NULL || pSet64 == NULL)

	LOC_CP_I64_((*pVal), (*pSet64))
}

Tst_bool
st_sysSInt64_setDecStr(Tst_int64 *pVal, ST_OPTARG(const Tst_str *pNumStr))
{
	Tst_uint32    x      = 0;
	Tst_uint64    temp;
	Tst_bool      sgn    = ST_B_TRUE,
	              sgnFnd = ST_B_FALSE,
	              errFnd = ST_B_FALSE;
	Tst_str const *pCh   = pNumStr;

	ST_ASSERTN_BOOL(ST_B_FALSE, pVal == NULL)

	if (pNumStr == NULL) {
		ST_SYSMATH_STC_RSETSI64(*pVal)
		return ST_B_TRUE;
	}

	while (*pCh != 0) {
		if (*pCh == '+') {
			if (sgnFnd || x > 0)
				errFnd = ST_B_TRUE;
			sgn    = ST_B_TRUE;
			sgnFnd = ST_B_TRUE;
		} else if (*pCh == '-') {
			if (sgnFnd || x > 0)
				errFnd = ST_B_TRUE;
			sgn    = ST_B_FALSE;
			sgnFnd = ST_B_TRUE;
		} else
			break;
		++pCh;
		++x;
	}

	if (errFnd)
		ST_SYSMATH_STC_RSETSI64(*pVal)
	else {
		ST_SYSMATH_STC_RSETUI64(temp)
		errFnd = ! st_sysUInt64_setDecStr(&temp, pNumStr + (sgnFnd ? 1 : 0));
		LOC_CP_UI64_TO_I64_((*pVal), temp)
		if (! errFnd && ! sgn)
			st_sysSInt64_setSign(pVal, ST_B_FALSE);
	}
	return (! errFnd);
}

/*----------------------------------------------------------------------------*/

/*
 * Convert pVal into 32bit signed int by simply cropping the high bits
 *   If pOFflag is != NULL it will be set to ST_B_TRUE on overflow
 */
Tst_int32
st_sysSInt64_toI32(const Tst_int64 *pVal, ST_OPTARG(Tst_bool *pOFflag))
{
	ST_ASSERTN_NUM(0, pVal == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (((pVal->nativeS64 *
					(pVal->nativeS64 < 0 ? -1 : 1)) >> 32) != 0);
#		endif
	return (Tst_int32)pVal->nativeS64;
#	else
#		if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		*pOFflag = (pVal->hiS != 0xffffffff && pVal->hiS != 0);
#		endif
	return (Tst_int32)pVal->loS;
#	endif
}

/*
 * Convert 64bit signed int to 64bit unsigned int
 */
void
st_sysSInt64_toUI64(const Tst_int64 *pValSI, Tst_uint64 *pValUI)
{
	ST_ASSERTN_VOID(pValSI == NULL || pValUI == NULL)

	LOC_CP_I64_TO_UI64_((*pValUI), (*pValSI))
}

/*
 * Convert pVal into hex-string
 *
 * pVal     MUST be set
 * pHexStr  MUST be != NULL and at least 19 bytes big
 */
void
st_sysSInt64_toHexStr(const Tst_int64 *pVal,
		Tst_str *pHexStr, const Tst_uint32 numStrSz)
{
	Tst_uint64 temp;

	ST_ASSERTN_VOID(pVal == NULL || pHexStr == NULL)

	LOC_CP_I64_TO_UI64_(temp, (*pVal))
	st_sysUInt64_toHexStr(&temp, pHexStr, numStrSz);
}

/*
 * Convert pVal into numeric-string
 *
 * pVal     MUST be set
 * pNumStr  MUST be != NULL and at least 22 bytes big
 */
void
st_sysSInt64_toDecStr(const Tst_int64 *pVal,
		Tst_str *pNumStr, const Tst_uint32 numStrSz)
{
	Tst_int64  tempSI;
	Tst_uint64 tempUI;

	ST_ASSERTN_VOID(pVal == NULL || pNumStr == NULL || numStrSz < 22)

	if (! LOC_GETSIGN_(*pVal)) {
		LOC_CP_I64_(tempSI, (*pVal))
		st_sysSInt64_setSign(&tempSI, ST_B_TRUE);
		LOC_CP_I64_TO_UI64_(tempUI, tempSI)
		st_sysUInt64_toDecStr(&tempUI, &pNumStr[1], numStrSz - 1);
		pNumStr[0] = '-';
	} else {
		LOC_CP_I64_TO_UI64_(tempUI, (*pVal))
		st_sysUInt64_toDecStr(&tempUI, pNumStr, numStrSz);
	}
}

/*----------------------------------------------------------------------------*/

int
st_sysSInt64_cmpI32(const Tst_int64 *pVal64_a,
		const Tst_int32 valCmp32)
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_int64 temp_b;
#	endif

	ST_ASSERTN_NUM(0, pVal64_a == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pVal64_a->nativeS64 > (Tst_int64_native)valCmp32)
		return 1;
	if (pVal64_a->nativeS64 < (Tst_int64_native)valCmp32)
		return -1;
	return 0;
#	else
	ST_SYSMATH_STC_RSETSI64(temp_b)
	st_sysSInt64_setI32(&temp_b, valCmp32);
	return st_sysSInt64_cmpI64(pVal64_a, &temp_b);
#	endif
}

int
st_sysSInt64_cmpLoHi(const Tst_int64 *pVal64_a,
		const Tst_uint32 valCmp_lo, const Tst_uint32 valCmp_hi)
{
	Tst_int64 temp_b;

	ST_ASSERTN_NUM(0, pVal64_a == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	temp_b.nativeS64 = (Tst_int64_native)((Tst_uint64_native)valCmp_lo |
				((Tst_uint64_native)valCmp_hi << 32));
	if (pVal64_a->nativeS64 == temp_b.nativeS64)
		return 0;
	if (pVal64_a->nativeS64 < temp_b.nativeS64)
		return -1;
	return 1;
#	else
	temp_b.loS = valCmp_lo;
	temp_b.hiS = valCmp_hi;
	return st_sysSInt64_cmpI64(pVal64_a, &temp_b);
#	endif
}

int
st_sysSInt64_cmpI64(const Tst_int64 *pVal64_a,
		const Tst_int64 *pVal64_b)
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_bool sgn_a,
	         sgn_b;
#	endif

	ST_ASSERTN_NUM(0, pVal64_a == NULL || pVal64_b == NULL)  /* ret 0 */

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pVal64_a->nativeS64 > pVal64_b->nativeS64)
		return 1;
	if (pVal64_a->nativeS64 < pVal64_b->nativeS64)
		return -1;
#	else
	sgn_a = LOC_GETSIGN_(*pVal64_a);
	sgn_b = LOC_GETSIGN_(*pVal64_b);
	if (sgn_a && ! sgn_b)
		return 1;
	if (! sgn_a && sgn_b)
		return -1;
	if (pVal64_a->hiS > pVal64_b->hiS ||
			(pVal64_a->hiS == pVal64_b->hiS && pVal64_a->loS > pVal64_b->loS))
		return 1;  /* a > b */
	else if (pVal64_a->hiS < pVal64_b->hiS ||
			(pVal64_a->hiS == pVal64_b->hiS && pVal64_a->loS < pVal64_b->loS))
		return -1;  /* a < b */
#	endif
	return 0;
}

/*----------------------------------------------------------------------------*/

void
st_sysSInt64_addI32(Tst_int64 *pVal, const Tst_int32 add32,
		ST_OPTARG(Tst_bool *pOFflag))
{
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	Tst_bool sgn_a = ST_B_TRUE;
#	endif

	ST_ASSERTN_VOID(pVal == NULL)

#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL)
		sgn_a = LOC_GETSIGN_(*pVal);
#	endif
	/* */
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeS64 += (Tst_int64_native)add32;
#	else
	pVal->loS += (Tst_uint32)add32;
	if (pVal->loS < (Tst_uint32)add32)
		++pVal->hiS;
	pVal->hiS += (add32 >= 0 ? 0 : 0xffffffff);
#	endif
	/* overflow check */
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL) {
		/* max=+9,min=-10, 7 + 3 = -1 or -9 + -2 = 10 --> overflow
		 *                 0 + -10 = -10 or -10 + 9 = -1 --> no overfl. */
		if (sgn_a && add32 >= 0)
			*pOFflag = ! LOC_GETSIGN_(*pVal);
		else if (! (sgn_a || add32 >= 0))
			*pOFflag = LOC_GETSIGN_(*pVal);
	}
#	endif
}

void
st_sysSInt64_addI64(Tst_int64 *pVal, const Tst_int64 *pAdd64,
		ST_OPTARG(Tst_bool *pOFflag))
{
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	Tst_bool sgn_a = ST_B_TRUE,
	         sgn_b = ST_B_TRUE;
#	endif

	ST_ASSERTN_VOID(pVal == NULL || pAdd64 == NULL)

#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL) {
		sgn_a = LOC_GETSIGN_(*pVal);
		sgn_b = LOC_GETSIGN_(*pAdd64);
	}
#	endif
	/* */
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pVal->nativeS64 += pAdd64->nativeS64;
#	else
	pVal->loS += pAdd64->loS;
	if (pVal->loS < pAdd64->loS)
		++pVal->hiS;
	pVal->hiS += pAdd64->hiS;
#	endif
	/* overflow check */
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL) {
		/* max=+9,min=-10, 7 + 3 = -1 --> overflow
		 *                 -9 + -2 = 10 --> overflow
		 *                 0 + -10 = -10 --> no overfl.
		 *                 -10 + 9 = -1 --> no overfl.  */
		if (sgn_a && sgn_b)
			*pOFflag = ! LOC_GETSIGN_(*pVal);
		else if (! (sgn_a || sgn_b))
			*pOFflag = LOC_GETSIGN_(*pVal);
	}
#	endif
}

/*----------------------------------------------------------------------------*/

void
st_sysSInt64_subI32(Tst_int64 *pVal, const Tst_int32 sub32,
		ST_OPTARG(Tst_bool *pOFflag))
{
	Tst_int64 temp;

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	temp.nativeS64 = (Tst_int64_native)sub32;
#	else
	ST_SYSMATH_STC_RSETSI64(temp)
	st_sysSInt64_setI32(&temp, sub32);
#	endif
	st_sysSInt64_subI64(pVal, &temp, pOFflag);
}

void
st_sysSInt64_subI64(Tst_int64 *pVal, const Tst_int64 *pSub64,
		ST_OPTARG(Tst_bool *pOFflag))
{
	Tst_uint64 temp_aUI,
	           temp_bUI;
	Tst_int64  temp_aSI,
	           temp_bSI;
	Tst_uint64 *pTemp_aUI = &temp_aUI;
	Tst_bool   sgn_a,
	           sgn_b,
	           chgSgn   = ST_B_FALSE;

	ST_ASSERTN_VOID(pVal == NULL || pSub64 == NULL)

	sgn_a = LOC_GETSIGN_(*pVal);
	sgn_b = LOC_GETSIGN_(*pSub64);
	/* */
	if (sgn_a && sgn_b) {
		LOC_CP_I64_TO_UI64_(temp_aUI, (*pVal))
		LOC_CP_I64_TO_UI64_(temp_bUI, (*pSub64))
		/* +x - +y = x - y = ?z */
		if (st_sysUInt64_cmpUI64(&temp_aUI, &temp_bUI) >= 0) {
			/* x - y = +z */
			st_sysUInt64_subUI64(&temp_aUI, &temp_bUI, pOFflag);
		} else {
			/* x - y = -z
			 *
			 * 3 - 4 = -(4 - 3) = -1  */
			pTemp_aUI = &temp_bUI;
			chgSgn    = ST_B_TRUE;
			st_sysUInt64_subUI64(&temp_bUI, &temp_aUI, pOFflag);
		}
		/* */
		LOC_CP_UI64_TO_I64_((*pVal), (*pTemp_aUI))
	} else if (! (sgn_a || sgn_b)) {
		LOC_CP_I64_(temp_aSI, (*pVal))
		LOC_CP_I64_(temp_bSI, (*pSub64))
		st_sysSInt64_setSign(&temp_aSI, ST_B_TRUE);
		st_sysSInt64_setSign(&temp_bSI, ST_B_TRUE);

		LOC_CP_I64_TO_UI64_(temp_aUI, temp_aSI)
		LOC_CP_I64_TO_UI64_(temp_bUI, temp_bSI)

		/* -x - -y = -x + y = y - x = ?z */
		if (st_sysUInt64_cmpUI64(&temp_bUI, &temp_aUI) >= 0) {
			/* y - x = +z
			 *
			 * 4 - 4 = 0
			 * 4 - 3 = 1  */
			pTemp_aUI = &temp_bUI;
			st_sysUInt64_subUI64(&temp_bUI, &temp_aUI, pOFflag);
		} else {
			/* y - x = -z
			 *
			 * 3 - 4 = -(4 - 3) = -1  */
			chgSgn  = ST_B_TRUE;
			st_sysUInt64_subUI64(&temp_aUI, &temp_bUI, pOFflag);
		}
		/* */
		LOC_CP_UI64_TO_I64_((*pVal), (*pTemp_aUI))
	} else {
		LOC_CP_I64_(temp_aSI, (*pVal))
		LOC_CP_I64_(temp_bSI, (*pSub64))
		if (sgn_a) {
			/* +x - -y = x + y = +z */
			st_sysSInt64_setSign(&temp_bSI, ST_B_TRUE);
		} else {
			/* -x - +y = -x - y = -(x + y) = -z */
			st_sysSInt64_setSign(&temp_aSI, ST_B_TRUE);
			chgSgn = ST_B_TRUE;
		}
		st_sysSInt64_addI64(&temp_aSI, &temp_bSI, pOFflag);
		LOC_CP_I64_((*pVal), temp_aSI)
	}
	/* */
	if (chgSgn)
		st_sysSInt64_setSign(pVal, ! LOC_GETSIGN_(*pVal));
}

/*----------------------------------------------------------------------------*/

void
st_sysSInt64_multI32s(Tst_int64 *pRes,
		const Tst_int32 fact32_a, const Tst_int32 fact32_b)
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_uint64 temp_rUI;
#	endif
	register Tst_int32 temp_aSI = fact32_a,
	                   temp_bSI = fact32_b;
	register Tst_bool  sgn_a    = (fact32_a >= 0),
	                   sgn_b    = (fact32_b >= 0);

	ST_ASSERTN_VOID(pRes == NULL)

	if (! sgn_a)
		temp_aSI *= -1;
	if (! sgn_b)
		temp_bSI *= -1;

	/* */
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	pRes->nativeS64 = (Tst_int64_native)temp_aSI * (Tst_int64_native)temp_bSI;
#	else
	if (temp_aSI < 0x10000 && temp_bSI < 0x10000) {
		pRes->hiS = 0;
		pRes->loS = (Tst_uint32)temp_aSI * (Tst_uint32)temp_bSI;
	} else {
		ST_SYSMATH_STC_RSETUI64(temp_rUI)
		st_sysUInt64_multUI32s(&temp_rUI, (Tst_uint32)temp_aSI, (Tst_uint32)temp_bSI);
		LOC_CP_UI64_TO_I64_((*pRes), temp_rUI)
	}
#	endif
	/* */
	if ((sgn_a && ! sgn_b) || (! sgn_a && sgn_b))
		st_sysSInt64_setSign(pRes, ST_B_FALSE);
}

void
st_sysSInt64_multI32(Tst_int64 *pVal, const Tst_int32 val32_b,
		ST_OPTARG(Tst_bool *pOFflag))
{
	Tst_int64 tmp_a,
	          tmp_b;

	ST_ASSERTN_VOID(pVal == NULL)

	LOC_CP_I64_(tmp_a, *pVal)
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	tmp_b.nativeS64 = (Tst_int64_native)val32_b;
#	else
	tmp_b.loS = (Tst_uint32)val32_b;
	tmp_b.hiS = (val32_b >= 0 ? 0 : (Tst_uint32)-1);
#	endif
	st_sysSInt64_multI64s(pVal, &tmp_a, &tmp_b, pOFflag);
}

void
st_sysSInt64_multI64s(Tst_int64 *pRes,
		const Tst_int64 *pFact_a, const Tst_int64 *pFact_b,
		ST_OPTARG(Tst_bool *pOFflag))
{
	Tst_int64  temp_aSI,
	           temp_bSI;
	Tst_uint64 temp_aUI,
	           temp_bUI,
	           temp_rUI;
	Tst_bool   sgn_a,
	           sgn_b,
	           sgn_r;

	ST_ASSERTN_VOID(pRes == NULL || pFact_a == NULL || pFact_b == NULL)

	sgn_a = LOC_GETSIGN_(*pFact_a);
	sgn_b = LOC_GETSIGN_(*pFact_b);
	LOC_CP_I64_(temp_aSI, (*pFact_a))
	LOC_CP_I64_(temp_bSI, (*pFact_b))
	if (! sgn_a)
		st_sysSInt64_setSign(&temp_aSI, ST_B_TRUE);
	if (! sgn_b)
		st_sysSInt64_setSign(&temp_bSI, ST_B_TRUE);

	LOC_CP_I64_TO_UI64_(temp_aUI, temp_aSI)
	LOC_CP_I64_TO_UI64_(temp_bUI, temp_bSI)
	ST_SYSMATH_STC_RSETUI64(temp_rUI)
	/* */
	st_sysUInt64_multUI64s(&temp_rUI, &temp_aUI, &temp_bUI, pOFflag);
	/* */
	LOC_CP_UI64_TO_I64_((*pRes), temp_rUI)
	/* */
	sgn_r = (! sgn_a || sgn_b) && (sgn_a || ! sgn_b);
	if (! sgn_r)
		st_sysSInt64_setSign(pRes, ST_B_FALSE);
	/* 2nd overflow check */
#	if (ST_SYSMATH_OVERFLOW_CHECK_ == 1)
	if (pOFflag != NULL && sgn_r != LOC_GETSIGN_(*pRes))
		*pOFflag = ST_B_TRUE;
#	endif
}

/*----------------------------------------------------------------------------*/

void
st_sysSInt64_divI32(Tst_int64 *pVal, const Tst_int32 div32)
{
	st_sysSInt64_divI32_rem(pVal, div32, NULL);
}

void
st_sysSInt64_divI32_rem(Tst_int64 *pVal, const Tst_int32 div32,
		ST_OPTARG(Tst_int32 *pRemainder32))
{
	Tst_int64  temp_vSI;
	Tst_uint64 temp_vUI;
	Tst_uint32 temp_dUI,
	           temp_rUI = 0,
	           *pTR_UI  = NULL;
	Tst_bool   sgn_v,
	           sgn_d;

	ST_ASSERTN_VOID(pVal == NULL)

	sgn_v = LOC_GETSIGN_(*pVal);
	sgn_d = (div32 >= 0);
	LOC_CP_I64_(temp_vSI, (*pVal))
	/* make pVal and div32 positive */
	if (! sgn_v)
		st_sysSInt64_setSign(&temp_vSI, ST_B_TRUE);
	if (! sgn_d)
		temp_dUI = (Tst_uint32)(div32 * -1);
	else
		temp_dUI = (Tst_uint32)div32;

	/* convert signed to unsigned */
	LOC_CP_I64_TO_UI64_(temp_vUI, temp_vSI)
	if (pRemainder32 != NULL)
		pTR_UI = &temp_rUI;
	/* divide unsigned ints */
	st_sysUInt64_divUI32_rem(&temp_vUI, temp_dUI, pTR_UI);
	/* */
	LOC_CP_UI64_TO_I64_((*pVal), temp_vUI)
	if (pRemainder32 != NULL) {
		*pRemainder32 = (Tst_int32)*pTR_UI;
		if ((sgn_v && *pRemainder32 < 0) || (! sgn_v && *pRemainder32 > 0))
			*pRemainder32 *= -1;
	}
	if ((sgn_v || sgn_d) && (! sgn_d || ! sgn_v))
		st_sysSInt64_setSign(pVal, ST_B_FALSE);
}

void
st_sysSInt64_divI64(Tst_int64 *pVal, const Tst_int64 *pDiv64)
{
	st_sysSInt64_divI64_rem(pVal, pDiv64, NULL);
}

void
st_sysSInt64_divI64_rem(Tst_int64 *pVal, const Tst_int64 *pDiv64,
		ST_OPTARG(Tst_int64 *pRemainder64))
{
	Tst_int64  temp_vSI,
	           temp_dSI;
	Tst_uint64 temp_vUI,
	           temp_dUI,
	           temp_rUI;
	Tst_uint64 *pTR_UI = NULL;
	Tst_bool   sgn_v,
	           sgn_d;

	ST_ASSERTN_VOID(pVal == NULL || pDiv64 == NULL)

	sgn_v = LOC_GETSIGN_(*pVal);
	sgn_d = LOC_GETSIGN_(*pDiv64);
	LOC_CP_I64_(temp_vSI, (*pVal))
	LOC_CP_I64_(temp_dSI, (*pDiv64))
	/* make pVal and pDiv64 positive */
	if (! sgn_v)
		st_sysSInt64_setSign(&temp_vSI, ST_B_TRUE);
	if (! sgn_d)
		st_sysSInt64_setSign(&temp_dSI, ST_B_TRUE);
	/* convert signed to unsigned */
	LOC_CP_I64_TO_UI64_(temp_vUI, temp_vSI)
	LOC_CP_I64_TO_UI64_(temp_dUI, temp_dSI)
	if (pRemainder64 != NULL) {
		ST_SYSMATH_STC_RSETUI64(temp_rUI)
		pTR_UI = &temp_rUI;
	}
	/* divide unsigned ints */
	st_sysUInt64_divUI64_rem(&temp_vUI, &temp_dUI, pTR_UI);
	/* */
	LOC_CP_UI64_TO_I64_((*pVal), temp_vUI)
	if (pRemainder64 != NULL) {
		LOC_CP_UI64_TO_I64_((*pRemainder64), (*pTR_UI))
		st_sysSInt64_setSign(pRemainder64, sgn_v);
	}
	if ((sgn_v || sgn_d) && (! sgn_d || ! sgn_v))
		st_sysSInt64_setSign(pVal, ST_B_FALSE);
}

/*----------------------------------------------------------------------------*/

/*
 * Shift pVal sh2r-bits to right
 */
void
st_sysSInt64_shiftRight(Tst_int64 *pVal, const Tst_int32 sh2r)
{
	ST_ASSERTN_VOID(pVal == NULL)
	/*if (sh2r > 63) {
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}*/

	if (sh2r < 0)
		st_sysSInt64_shiftLeft(pVal, -sh2r);
	else if (sh2r > 0) {
#		if (CONFIG_ST_ALL_HAVE64BIT == 1)
		pVal->nativeS64 >>= sh2r;
#		else
		if (sh2r < 32) {
			if (LOC_GETSIGN_(*pVal)) {
				pVal->loS   = (pVal->hiS << (32 - sh2r)) |
						(pVal->loS >> sh2r);
				pVal->hiS >>= sh2r;
			} else {
				pVal->loS = (pVal->hiS << (32 - sh2r)) |
						(pVal->loS >> sh2r);
				pVal->hiS = (Tst_uint32)((Tst_int32)pVal->hiS >> sh2r);
			}
		} else {
			if (LOC_GETSIGN_(*pVal)) {
				pVal->loS = (pVal->hiS >> (32 + sh2r - 64));
				pVal->hiS = 0;
			} else {
				pVal->loS = (Tst_uint32)((Tst_int32)pVal->hiS >> (32 + sh2r - 64));
				pVal->hiS = 0xffffffff;
			}
		}
#		endif
	}
}

/*
 * Shift pVal sh2l-bits to left
 */
void
st_sysSInt64_shiftLeft(Tst_int64 *pVal, const Tst_int32 sh2l)
{
	Tst_uint64 temp;

	ST_ASSERTN_VOID(pVal == NULL)

	if (sh2l < 0)
		st_sysSInt64_shiftRight(pVal, -sh2l);
	else {
		LOC_CP_I64_TO_UI64_(temp, (*pVal))
		st_sysUInt64_shiftLeft(&temp, sh2l);
		LOC_CP_UI64_TO_I64_((*pVal), temp)
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Reverse Byte-Order
 *
 * pVal   MUST be != NULL
 */
void
st_sysSInt64_reverseByteOrder(Tst_int64 *pVal)
{
	Tst_uint64 temp;

	ST_ASSERTN_VOID(pVal == NULL)

	LOC_CP_I64_TO_UI64_(temp, (*pVal))
	st_sysUInt64_reverseByteOrder(&temp);
	LOC_CP_UI64_TO_I64_((*pVal), temp)
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (CONFIG_ST_ALL_HAVE64BIT != 1)
#if 0
static ST_INLINE_S Tst_uint32
ST_SYSMATH__uint64_mult16bit(const Tst_uint32 factA, const Tst_uint32 factB)
{
#	define LOC_LO_(x)  (x & 0x00ff)
#	define LOC_HI_(x)  ((x & 0xff00) >> 8)
#	define LOC_AHI_    words16[0]
#	define LOC_ALO_    words16[1]
#	define LOC_BHI_    words16[2]
#	define LOC_BLO_    words16[3]
#	define LOC_TX_     temps16[0]
#	define LOC_TY_     temps16[1]
#	define LOC_TZ0_    tz16[0]
#	define LOC_TZ1_    tz16[1]
#	define LOC_TZ2_    tz16[2]
	Tst_uint32 words16[4],
	           temps16[2],
	           tz16[3];

	if (factA == 0 || factB == 0)
		return 0;
	LOC_AHI_ = LOC_HI_(factA);
	LOC_ALO_ = LOC_LO_(factA);
	LOC_BHI_ = LOC_HI_(factB);
	LOC_BLO_ = LOC_LO_(factB);
	/* */
	/** */
	LOC_TZ2_ = LOC_AHI_ * LOC_BHI_;
	LOC_TZ0_ = LOC_ALO_ * LOC_BLO_;
	/** tz1 = (aH + aL) * (bH + bL) - tz2 - tz0 */
	LOC_TX_   = LOC_AHI_ + LOC_ALO_;
	LOC_TY_   = LOC_BHI_ + LOC_BLO_;
	LOC_TZ1_  = LOC_TX_ * LOC_TY_ - LOC_TZ2_ - LOC_TZ0_;
	/* */
	LOC_TY_ = LOC_TZ1_ + LOC_HI_(LOC_TZ0_ );
	return ((LOC_LO_(LOC_TY_) << 8) | LOC_LO_(LOC_TZ0_) |
			((LOC_TZ2_ + LOC_HI_(LOC_TY_)) << 16));
#	undef LOC_LO_
#	undef LOC_HI_
#	undef LOC_AHI_
#	undef LOC_ALO_
#	undef LOC_BHI_
#	undef LOC_BLO_
#	undef LOC_TX_
#	undef LOC_TY_
#	undef LOC_TZ0_
#	undef LOC_TZ1_
#	undef LOC_TZ2_
}
#endif  /* if 0 */
#endif  /* CONFIG_ST_ALL_HAVE64BIT != 1 */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef THESE_ARE_ALTERNATIVE_VERSIONS
static void
ST_SYSMATH__uint64_divUI64__(Tst_uint64 *pVal, const Tst_uint64 *pDiv64)
{
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
	Tst_uint64 quot64,
	           qbit64,
	           div64;
#	endif

	ST_ASSERTN_VOID(pVal == NULL || pDiv64 == NULL)

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	if (pDiv64->nativeU64 == 0) {
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}
	pVal->nativeU64 = pVal->nativeU64 / pDiv64->nativeU64;
#	else
	if ((pDiv64->hiU == 0 && pDiv64->loU == 0) ||
			(pVal->hiU == 0 && pVal->loU == 0)) {
		ST_SYSMATH_STC_RSETUI64((*pVal))
		return;
	}

	if (pVal->hiU == 0 && pDiv64->hiU == 0) {
		pVal->loU /= pDiv64->loU;
	} else {
		ST_SYSMATH_STC_RSETUI64(quot64)
		qbit64.loU = 1;
		qbit64.hiU = 0;
		div64.loU  = pDiv64->loU;
		div64.hiU  = pDiv64->hiU;
		while (div64.hiU < 0x80000000) {
			st_sysUInt64_shiftLeft(&div64, 1);
			st_sysUInt64_shiftLeft(&qbit64, 1);
		}
		while (qbit64.hiU != 0 || qbit64.loU != 0) {
			/* qbit64 != 0 */
			if (div64.hiU < pVal->hiU ||
					(div64.hiU == pVal->hiU && div64.loU <= pVal->loU)) {
				/* div64 <= pVal */
				if (pVal->hiU == 0)
					pVal->loU -= div64.loU;
				else
					st_sysUInt64_subUI64(pVal, &div64);
				st_sysUInt64_addUI64(&quot64, &qbit64);
			}
			if (div64.hiU == 0)
				div64.loU >>= 1;
			else
				st_sysUInt64_shiftRight(&div64, 1);
			if (qbit64.hiU == 0)
				qbit64.loU >>= 1;
			else
				st_sysUInt64_shiftRight(&qbit64, 1);
		}
		/* remainder is now in pVal and the result in quot64 */

		pVal->hiU = quot64.hiU;
		pVal->loU = quot64.loU;
	}
#	endif
}

static void
ST_SYSMATH__uint64_divUI32__(Tst_uint64 *pVal, const Tst_uint32 div32)
{
	Tst_uint64 div64;

#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	div64.nativeU64 = (Tst_uint64_native)div32;
#	else
	div64.loU = div32;
	div64.hiU = 0;
#	endif
	ST_SYSMATH__uint64_divUI64__(pVal, &div64);
}
#endif  /* alternative versions */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static ST_INLINE_S Tst_uint32
ST_SYSMATH__strlen(const char *pStr, const Tst_uint32 strSz)
{
	char const *pS = pStr;
	Tst_uint32 x   = 0;

	while (*pS && x++ < strSz)
		++pS;
	return (pS - pStr);
}

static ST_INLINE_S void
ST_SYSMATH__addNStrToNStr(Tst_str *pNumStr,
		const Tst_uint32 numStrSz, const char *pAdd, const Tst_uint32 addSz)
{
	Tst_uint32 lenA  = ST_SYSMATH__strlen((char*)pNumStr, numStrSz),
	           lenB  = ST_SYSMATH__strlen(pAdd, addSz),
	           lenM  = (lenA > lenB ? lenA : lenB),
	           x,
	           xA,
	           xB,
	           vA,
	           vB,
	           r,
	           carry = 0;
	char       resStr[64];

	/**ST_SYSMATH__prf("add%*s%s\n", (lenM + 1) - lenA, " ", pNumStr);
	ST_SYSMATH__prf("   %*s%s\n", (lenM + 1) - lenB, " ", pAdd);**/
	for (x = 0; x < sizeof(resStr); x++)
		resStr[x] = 0;
	xA = lenA;
	xB = lenB;
	for (x = lenM; x > 0; x--) {
		/* */
		vA = 0;
		if (xA > 0)
			vA = (Tst_uint32)(pNumStr[xA - 1] - '0');
		vB = 0;
		if (xB > 0)
			vB = (Tst_uint32)(pAdd[xB - 1] - '0');
		r = (vA + vB + carry) % 10;
		/**ST_SYSMATH__prf("    [%u] %u + %u + c%u = %u\n",
				x - 1, vA, vB, carry, r);**/
		carry = (vA + vB + carry - r) / 10;
		/**if (carry > 0)
			ST_SYSMATH__prf("    [%u]   c%u\n", x - 1, carry);**/
		resStr[x - 1] = '0' + (char)r;
		/* */
		if (xA > 0)
			--xA;
		if (xB > 0)
			--xB;
	}
	if (carry != 0) {
		for (x = lenM + 1; x > 1; x--)
			resStr[x - 1] = resStr[x - 2];
		resStr[0] = '1';
	}
	for (x = 0; x < (carry != 0 ? lenM + 1 : lenM) &&
			x < numStrSz && x < sizeof(resStr); x++)
		pNumStr[x] = (Tst_str)resStr[x];
}

static ST_INLINE_S void
ST_SYSMATH__mulNStrBy2(Tst_str *pNumStr, const Tst_uint32 numStrSz)
{
	/**ST_SYSMATH__prf("mul 2\n");**/
	ST_SYSMATH__addNStrToNStr(pNumStr, numStrSz, (char*)pNumStr, numStrSz);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_SYSMATH_DEB_ == 1)
static void ST_SYSMATH__prf(char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

static void ST_SYSMATH__prE(char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}
#endif

/******************************************************************************/
