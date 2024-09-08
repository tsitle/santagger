/******************************************************************************/
/* sys_math.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
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

#ifndef HEAD_SYS_MATH_H
#define HEAD_SYS_MATH_H

/*
// Own-Includes
*/
#include "src/includes/common/sys_math-defs.h"


ST_BEGIN_C_DECLS

/* If overflow checks are not enabled in sys_math-defs.h
 * then the optional argument pOFflag in all of the
 * following functions will be of no use            */

/*
// Functions - Unsigned Int
*/
/** */
ST_EXPORT void
st_sysUInt64_setLoHi(Tst_uint64 *pVal,
                     const Tst_uint32 set32_lo,
                     const Tst_uint32 set32_hi);
ST_EXPORT void
st_sysUInt64_setUI32(Tst_uint64 *pVal,
                     const Tst_uint32 set32);
ST_EXPORT void
st_sysUInt64_setUI64(Tst_uint64 *pVal,
                     const Tst_uint64 *pSet64);
ST_EXPORT Tst_bool
st_sysUInt64_setDecStr(Tst_uint64 *pVal,
                       ST_OPTARG(const Tst_str *pNumStr));

/** */
ST_EXPORT Tst_uint32
st_sysUInt64_toUI32(const Tst_uint64 *pVal,
                    ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysUInt64_toI64(const Tst_uint64 *pValUI,
                   Tst_int64 *pValSI);
ST_EXPORT void
st_sysUInt64_toHexStr(const Tst_uint64 *pVal,
                      Tst_str *pHexStr,
                      const Tst_uint32 numStrSz);
ST_EXPORT void
st_sysUInt64_toDecStr(const Tst_uint64 *pVal,
                      Tst_str *pNumStr,
                      const Tst_uint32 numStrSz);

/** */
ST_EXPORT int
st_sysUInt64_cmpUI32(const Tst_uint64 *pVal64_a,
                     const Tst_uint32 valCmp32);
ST_EXPORT int
st_sysUInt64_cmpLoHi(const Tst_uint64 *pVal64_a,
                     const Tst_uint32 valCmp_lo,
                     const Tst_uint32 valCmp_hi);
ST_EXPORT int
st_sysUInt64_cmpUI64(const Tst_uint64 *pVal64_a,
                     const Tst_uint64 *pVal64_b);

/** */
ST_EXPORT void
st_sysUInt64_addUI32(Tst_uint64 *pVal,
                     const Tst_uint32 add32,
                     ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysUInt64_addUI64(Tst_uint64 *pVal,
                     const Tst_uint64 *pAdd64,
                     ST_OPTARG(Tst_bool *pOFflag));
/** */
ST_EXPORT void
st_sysUInt64_subUI32(Tst_uint64 *pVal,
                     const Tst_uint32 sub32,
                     ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysUInt64_subUI64(Tst_uint64 *pVal,
                     const Tst_uint64 *pSub64,
                     ST_OPTARG(Tst_bool *pOFflag));
/** */
ST_EXPORT void
st_sysUInt64_multUI32s(Tst_uint64 *pRes,
                       const Tst_uint32 fact32_a,
                       const Tst_uint32 fact32_b);
ST_EXPORT void
st_sysUInt64_multUI32(Tst_uint64 *pVal,
                      const Tst_uint32 val32_b,
                      ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysUInt64_multUI64s(Tst_uint64 *pRes,
                       const Tst_uint64 *pFact_a,
                       const Tst_uint64 *pFact_b,
                       ST_OPTARG(Tst_bool *pOFflag));
/** */
ST_EXPORT void
st_sysUInt64_divUI32(Tst_uint64 *pVal,
                     const Tst_uint32 div32);
ST_EXPORT void
st_sysUInt64_divUI32_rem(Tst_uint64 *pVal,
                         const Tst_uint32 div32,
                         ST_OPTARG(Tst_uint32 *pRemainder32));
ST_EXPORT void
st_sysUInt64_divUI64(Tst_uint64 *pVal,
                     const Tst_uint64 *pDiv64);
ST_EXPORT void
st_sysUInt64_divUI64_rem(Tst_uint64 *pVal,
                         const Tst_uint64 *pDiv64,
                         ST_OPTARG(Tst_uint64 *pRemainder64));
/** */
ST_EXPORT void
st_sysUInt64_modUI32(const Tst_uint64 *pDividend,
                     const Tst_uint32 divisor32,
                     Tst_uint32 *pRemainder32);
ST_EXPORT void
st_sysUInt64_modUI64(const Tst_uint64 *pDividend,
                     const Tst_uint64 *pDivisor,
                     Tst_uint64 *pRemainder);

/** */
ST_EXPORT void
st_sysUInt64_shiftRight(Tst_uint64 *pVal,
                        const Tst_int32 sh2r);
ST_EXPORT void
st_sysUInt64_shiftLeft(Tst_uint64 *pVal,
                       const Tst_int32 sh2l);

/** */
ST_EXPORT Tst_uint32
sysILog2_UI8(const Tst_byte val);

ST_EXPORT Tst_uint32
sysILog2_UI32(const Tst_uint32 val);

ST_EXPORT Tst_uint32
sysILog2_UI64(const Tst_uint64 *pVal);

/** */
ST_EXPORT void
st_sysUInt64_reverseByteOrder(Tst_uint64 *pVal);


/*
// Functions - Signed Int
*/
/** */
ST_EXPORT Tst_bool
st_sysSInt64_getSign(const Tst_int64 *pVal);

ST_EXPORT void
st_sysSInt64_setSign(Tst_int64 *pVal, const Tst_bool sgn);

ST_EXPORT void
st_sysSInt64_setLoHi(Tst_int64 *pVal,
                     const Tst_uint32 set32_lo,
                     const Tst_uint32 set32_hi);

ST_EXPORT void
st_sysSInt64_setI32(Tst_int64 *pVal, const Tst_int32 setI32);

ST_EXPORT void
st_sysSInt64_setI64(Tst_int64 *pVal, const Tst_int64 *pSet64);

ST_EXPORT Tst_bool
st_sysSInt64_setDecStr(Tst_int64 *pVal,
                       ST_OPTARG(const Tst_str *pNumStr));

/** */
ST_EXPORT Tst_int32
st_sysSInt64_toI32(const Tst_int64 *pVal, ST_OPTARG(Tst_bool *pOFflag));

ST_EXPORT void
st_sysSInt64_toUI64(const Tst_int64 *pValSI, Tst_uint64 *pValUI);

ST_EXPORT void
st_sysSInt64_toHexStr(const Tst_int64 *pVal,
                      Tst_str *pHexStr,
                      const Tst_uint32 numStrSz);

ST_EXPORT void
st_sysSInt64_toDecStr(const Tst_int64 *pVal,
                      Tst_str *pNumStr,
                      const Tst_uint32 numStrSz);

/** */
ST_EXPORT int
st_sysSInt64_cmpI32(const Tst_int64 *pVal64_a,
                    const Tst_int32 valCmp32);
ST_EXPORT int
st_sysSInt64_cmpLoHi(const Tst_int64 *pVal64_a,
                     const Tst_uint32 valCmp_lo,
                     const Tst_uint32 valCmp_hi);
ST_EXPORT int
st_sysSInt64_cmpI64(const Tst_int64 *pVal64_a,
                    const Tst_int64 *pVal64_b);

/** */
ST_EXPORT void
st_sysSInt64_addUI32(Tst_int64 *pVal,
                     const Tst_uint32 add32,
                     ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysSInt64_addI32(Tst_int64 *pVal,
                    const Tst_int32 add32,
                    ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysSInt64_addI64(Tst_int64 *pVal,
                    const Tst_int64 *pAdd64,
                    ST_OPTARG(Tst_bool *pOFflag));
/** */
ST_EXPORT void
st_sysSInt64_subI32(Tst_int64 *pVal,
                    const Tst_int32 sub32,
                    ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysSInt64_subI64(Tst_int64 *pVal,
                    const Tst_int64 *pSub64,
                    ST_OPTARG(Tst_bool *pOFflag));
/** */
ST_EXPORT void
st_sysSInt64_multI32s(Tst_int64 *pRes,
                      const Tst_int32 fact32_a,
                      const Tst_int32 fact32_b);
ST_EXPORT void
st_sysSInt64_multI32(Tst_int64 *pVal,
                     const Tst_int32 val32_b,
                     ST_OPTARG(Tst_bool *pOFflag));
ST_EXPORT void
st_sysSInt64_multI64s(Tst_int64 *pRes,
                      const Tst_int64 *pFact_a,
                      const Tst_int64 *pFact_b,
                      ST_OPTARG(Tst_bool *pOFflag));
/** */
ST_EXPORT void
st_sysSInt64_divI32(Tst_int64 *pVal, const Tst_int32 div32);

ST_EXPORT void
st_sysSInt64_divI32_rem(Tst_int64 *pVal,
                        const Tst_int32 div32,
                        ST_OPTARG(Tst_int32 *pRemainder32));
ST_EXPORT void
st_sysSInt64_divI64(Tst_int64 *pVal, const Tst_int64 *pDiv64);

ST_EXPORT void
st_sysSInt64_divI64_rem(Tst_int64 *pVal,
                        const Tst_int64 *pDiv64,
                        ST_OPTARG(Tst_int64 *pRemainder64));
/** */
ST_EXPORT void
st_sysSInt64_shiftRight(Tst_int64 *pVal, const Tst_int32 sh2r);

ST_EXPORT void
st_sysSInt64_shiftLeft(Tst_int64 *pVal, const Tst_int32 sh2l);

/** */
ST_EXPORT void
st_sysSInt64_reverseByteOrder(Tst_int64 *pVal);


/*
// Macros
*/
/** Tst_int64 + Tst_uint64 */
#if (CONFIG_ST_ALL_HAVE64BIT == 1)
#	define ST_SYSMATH_STC_RSETSI64(mac_int64Bits)  { \
			(mac_int64Bits).nativeS64 = 0; \
		}
#	define ST_SYSMATH_STC_RSETUI64(mac_uint64Bits)  { \
			(mac_uint64Bits).nativeU64 = 0; \
		}
#else
#	define ST_SYSMATH_STC_RSETSI64(mac_int64Bits)  { \
			(mac_int64Bits).loS = 0; \
			(mac_int64Bits).hiS = 0; \
		}
#	define ST_SYSMATH_STC_RSETUI64(mac_uint64Bits)  { \
			(mac_uint64Bits).loU = 0; \
			(mac_uint64Bits).hiU = 0; \
		}
#endif


ST_END_C_DECLS

#endif  /* HEAD_SYS_MATH_H */

/******************************************************************************/
