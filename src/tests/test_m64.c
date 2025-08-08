/******************************************************************************/
/* test_m64.c               [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for 64-bit math functions                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 15.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"
#include "fncs_test_common.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), getenv() */
#include <string.h>       /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool TEST_M64__run(void);
Tst_bool TEST_M64__00_cmpUI(void);
Tst_bool TEST_M64__00_cmpSI(void);
Tst_bool TEST_M64__01_addUI(void);
Tst_bool TEST_M64__01_addSI(void);
Tst_bool TEST_M64__02_subUI(void);
Tst_bool TEST_M64__02_subSI(void);
Tst_bool TEST_M64__03_mulUI(void);
Tst_bool TEST_M64__03_mulSI(void);
Tst_bool TEST_M64__04_divUI(void);
Tst_bool TEST_M64__04_divSI(void);
Tst_bool TEST_M64__05_log(void);
Tst_bool TEST_M64__06_addUI(void);
Tst_bool TEST_M64__06_addSI(void);
Tst_bool TEST_M64__07_subUI(void);
Tst_bool TEST_M64__07_subSI(void);
Tst_bool TEST_M64__08_mulUI(void);
Tst_bool TEST_M64__08_mulSI(void);
Tst_bool TEST_M64__09_divUI(void);
Tst_bool TEST_M64__09_divSI(void);
Tst_bool TEST_M64__10_shiftUI(void);
Tst_bool TEST_M64__10_shiftSI(void);
Tst_bool TEST_M64__11_restUI(void);
Tst_bool TEST_M64__11_restSI(void);
Tst_bool TEST_M64__bench(void);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	if (! TEST_M64__run()) {
		printf("! Test failed !\n");
		return 1;
	}

	printf("TEST_M64 -- All tests passed :-)\n");
	return 0;
}

Tst_bool
TEST_M64__run(void)
{
	#define LOC_RUNBENCH    0  /* bench */
	#define LOC_RUNTESTALL  1
	#define LOC_RUNTEST0     0  /* comp */
	#define LOC_RUNTEST1     0  /* add */
	#define LOC_RUNTEST2     0  /* sub */
	#define LOC_RUNTEST3     0  /* mult */
	#define LOC_RUNTEST4     0  /* div */
	#define LOC_RUNTEST5     0  /* log */
	#define LOC_RUNTEST6     0  /* add */
	#define LOC_RUNTEST7     0  /* sub */
	#define LOC_RUNTEST8     0  /* mult */
	#define LOC_RUNTEST9     0  /* div */
	#define LOC_RUNTEST10    0  /* shift */
	#define LOC_RUNTEST11    0  /* rest */

	#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
		if (ST_SYSFNC_ISBIGEND) {
			printf("System is Big-Endian\n");
			#if (WORDS_BIGENDIAN != 1)
				printf("  but app was compiled for Little-Endian\n");
			#endif
		} else {
			printf("System is Little-Endian\n");
			#if (WORDS_BIGENDIAN == 1)
				printf("  but app was compiled for Big-Endian\n");
			#endif
		}
	#endif

	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		printf("System supports 64bit\n");
	#else
		printf("System doesn't support 64bit\n");
	#endif
	printf("\n");

	#if (LOC_RUNTEST0 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__00_cmpUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__00_cmpSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST1 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__01_addUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__01_addSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST2 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__02_subUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__02_subSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST3 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__03_mulUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__03_mulSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST4 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__04_divUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__04_divSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST5 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__05_log()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST6 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__06_addUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__06_addSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST7 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__07_subUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__07_subSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST8 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__08_mulUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__08_mulSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST9 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__09_divUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__09_divSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST10 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__10_shiftUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__10_shiftSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNTEST11 == 1 || LOC_RUNTESTALL == 1)
		if (! TEST_M64__11_restUI()) { return ST_B_FALSE; }
		printf("\n");
		if (! TEST_M64__11_restSI()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	#if (LOC_RUNBENCH == 1)
		if (! TEST_M64__bench()) { return ST_B_FALSE; }
		printf("\n");
	#endif

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
TEST_M64__prfStr(const char *pFnc, const char *pMsg, const char *pStr)
{
	printf("%s(): %s '%s'\n", pFnc, pMsg, pStr);
}

void
TEST_M64__prf32UI(const char *pFnc, const char *pMsg, const Tst_uint32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("         0x%08x\n", v32);
}

void
TEST_M64__prf32UI_hexAdec(const char *pFnc, const char *pMsg,
		const Tst_uint32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("%u    (0x%08x)\n", v32, v32);
}

void
TEST_M64__prf32SI(const char *pFnc, const char *pMsg, const Tst_int32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("         0x%08x\n", v32);
}

void
TEST_M64__prf32UI_dec(const char *pFnc, const char *pMsg, const Tst_uint32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("%u\n", v32);
}

void
TEST_M64__prf32SI_dec(const char *pFnc, const char *pMsg,
		const Tst_int32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("%d\n", v32);
}

void
TEST_M64__prf32SI_hexAdec(const char *pFnc, const char *pMsg,
		const Tst_int32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("%d    (0x%08x)\n", v32, v32);
}

void
TEST_M64__prf64UI(const char *pFnc, const char *pMsg, const Tst_uint64 *pV64)
{
	/*char ui64str[19];

	ui64str[0] = 0;
	st_sysUInt64_toHexStr(pV64, ui64str);
	printf("%s(): %s %s\n", pFnc, pMsg, ui64str);*/

	printf("%s(): %s ", pFnc, pMsg);
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		printf("0x%08x %08x\n",
				(Tst_uint32)(pV64->nativeU64 >> 32),
				(Tst_uint32)((pV64->nativeU64 << 32) >> 32));
	#else
		printf("0x%08x %08x\n", pV64->hiU, pV64->loU);
	#endif
}

void
TEST_M64__prf64UIstr(const char *pFnc, const char *pMsg,
		const Tst_uint64 *pV64)
{
	Tst_str numStr[50];

	st_sysUInt64_toDecStr(pV64, numStr, sizeof(numStr));
	printf("%s(): %s %s\n", pFnc, pMsg, (const char*)numStr);
}

void
TEST_M64__prf64UI_hexAdec(const char *pFnc, const char *pMsg,
		const Tst_uint64 *pV64)
{
	Tst_str numStr[50];

	st_sysUInt64_toDecStr(pV64, numStr, sizeof(numStr));
	printf("%s(): %s %s    (", pFnc, pMsg, (const char*)numStr);
#	if (CONFIG_ST_ALL_HAVE64BIT == 1)
	printf("0x%08x %08x)\n",
			(Tst_uint32)(pV64->nativeU64 >> 32),
			(Tst_uint32)((pV64->nativeU64 << 32) >> 32));
#	else
	printf("0x%08x %08x)\n", pV64->hiU, pV64->loU);
#	endif
}

void
TEST_M64__prf64SI(const char *pFnc, const char *pMsg, const Tst_int64 *pV64)
{
	printf("%s(): %s ", pFnc, pMsg);
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		printf("0x%08x %08x\n",
				(Tst_uint32)(pV64->nativeS64 >> 32),
				(Tst_uint32)((pV64->nativeS64 << 32) >> 32));
	#else
		printf("0x%08x %08x\n", pV64->hiS, pV64->loS);
	#endif
}

void
TEST_M64__prf64SIstr(const char *pFnc, const char *pMsg,
		const Tst_int64 *pV64)
{
	Tst_str numStr[50];

	st_sysSInt64_toDecStr(pV64, numStr, sizeof(numStr));
	printf("%s(): %s %s\n", pFnc, pMsg, (const char*)numStr);
}

void
TEST_M64__prf64SI_hexAdec(const char *pFnc, const char *pMsg,
		const Tst_int64 *pV64)
{
	Tst_str numStr[50];

	st_sysSInt64_toDecStr(pV64, numStr, sizeof(numStr));
	printf("%s(): %s %s    (", pFnc, pMsg, (const char*)numStr);
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		printf("0x%08x %08x)\n",
				(Tst_uint32)(pV64->nativeS64 >> 32),
				(Tst_uint32)((pV64->nativeS64 << 32) >> 32));
	#else
		printf("0x%08x %08x)\n", pV64->hiS, pV64->loS);
	#endif
}

void
TEST_M64__prf64UI_sh(const char *pFnc, const Tst_int32 shift,
		const Tst_bool toLeft, const Tst_uint64 *pV64)
{
	if (toLeft) {
		printf("%s(): << %3d = ", pFnc, shift);
	} else {
		printf("%s(): >> %3d = ", pFnc, shift);
	}
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		printf("0x%08x %08x\n",
				(Tst_uint32)(pV64->nativeU64 >> 32),
				(Tst_uint32)((pV64->nativeU64 << 32) >> 32));
	#else
		printf("0x%08x %08x\n", pV64->hiU, pV64->loU);
	#endif
}

void
TEST_M64__prf64SI_sh(const char *pFnc, const Tst_int32 shift,
		const Tst_bool toLeft, const Tst_int64 *pV64)
{
	if (toLeft) {
		printf("%s(): << %3d = ", pFnc, shift);
	} else {
		printf("%s(): >> %3d = ", pFnc, shift);
	}
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		printf("0x%08x %08x\n",
				(Tst_uint32)(pV64->nativeS64 >> 32),
				(Tst_uint32)((pV64->nativeS64 << 32) >> 32));
	#else
		printf("0x%08x %08x\n", pV64->hiS, pV64->loS);
	#endif
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_cmpUI32()
 *       st_sysUInt64_cmpUI64()
 *       st_sysUInt64_cmpLoHi()
 */
Tst_bool
TEST_M64__00_cmpUI(void)
{
	const char *cFNC = __func__;
	#define LOC_COMPARE_(cond)  { \
				if (cond) TEST_FCOM__prf(cFNC, "Correct"); \
				else { TEST_FCOM__prf(cFNC, "! Wrong !"); return ST_B_FALSE; } \
			}
	Tst_uint64 val64,
	           valCmp64;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)

	/* */
	st_sysUInt64_setLoHi(&val64, 0, 0);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 1) < 0)
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 0) == 0)

	/* */
	st_sysUInt64_setUI32(&val64, 0);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 1) < 0)
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 0) == 0)

	/* */
	st_sysUInt64_setLoHi(&val64, 1, 0);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 2) < 0)
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 0) > 0)
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 1) == 0)

	/* */
	st_sysUInt64_setUI32(&val64, 1);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 2) < 0)
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 0) > 0)
	LOC_COMPARE_(st_sysUInt64_cmpUI32(&val64, 1) == 0)

	/* */
	st_sysUInt64_setLoHi(&val64, 2, 50);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 2, 49);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) > 0)
	st_sysUInt64_setLoHi(&valCmp64, 2, 51);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) < 0)
	st_sysUInt64_setLoHi(&valCmp64, 2, 50);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) == 0)
	st_sysUInt64_setLoHi(&valCmp64, 1, 50);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) > 0)
	st_sysUInt64_setLoHi(&valCmp64, 3, 50);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) < 0)

	/* */
	st_sysUInt64_setLoHi(&val64, 2, 0);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 2, 1);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) < 0)
	st_sysUInt64_setLoHi(&valCmp64, 2, 0);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) == 0)

	/* */
	st_sysUInt64_setLoHi(&val64, 0, 2);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0, 1);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) > 0)
	st_sysUInt64_setLoHi(&valCmp64, 0, 3);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) < 0)
	st_sysUInt64_setLoHi(&valCmp64, 0, 2);
	LOC_COMPARE_(st_sysUInt64_cmpUI64(&val64, &valCmp64) == 0)

	/* */
	st_sysUInt64_setLoHi(&val64, 0x1234, 0x5678);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysUInt64_cmpLoHi(&val64, 0x1234, 0x5678) == 0)
	LOC_COMPARE_(st_sysUInt64_cmpLoHi(&val64, 0x1233, 0x5678) > 0)
	LOC_COMPARE_(st_sysUInt64_cmpLoHi(&val64, 0x1235, 0x5678) < 0)
	LOC_COMPARE_(st_sysUInt64_cmpLoHi(&val64, 0x1234, 0) > 0)
	LOC_COMPARE_(st_sysUInt64_cmpLoHi(&val64, 0, 0x5678) > 0)

	/* */
	st_sysUInt64_setLoHi(&val64, 0xabcd, 0x5678);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysUInt64_cmpLoHi(&val64, 0xabcd, 0x5678) == 0)

	return ST_B_TRUE;
	#undef LOC_COMPARE_
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_cmpI32()
 *       st_sysSInt64_cmpI64()
 *       st_sysSInt64_cmpLoHi()
 */
Tst_bool
TEST_M64__00_cmpSI(void)
{
	const char *cFNC = __func__;
	#define LOC_COMPARE_(cond)  { \
				if (cond) TEST_FCOM__prf(cFNC, "Correct"); \
				else { TEST_FCOM__prf(cFNC, "! Wrong !"); return ST_B_FALSE; } \
			}
	Tst_int64 val64;

	ST_SYSMATH_STC_RSETSI64(val64)

	/* */
	st_sysSInt64_setLoHi(&val64, 0, 0);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 1) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 0) == 0)

	/* */
	st_sysSInt64_setI32(&val64, 0);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 1) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 0) == 0)

	/* */
	st_sysSInt64_setLoHi(&val64, 2, 0);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 1) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 3) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 2) == 0)

	/* */
	st_sysSInt64_setI32(&val64, 2);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 1) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 3) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 2) == 0)

	/* */
	st_sysSInt64_setI32(&val64, -1000);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1001) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -999) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 5) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1000) == 0)

	/* */
	st_sysSInt64_setI32(&val64, -1001);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1001) == 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -999) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 5) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -1000) < 0)

	/* */
	st_sysSInt64_setI32(&val64, 321);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, -150009) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 153) > 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 555) < 0)
	LOC_COMPARE_(st_sysSInt64_cmpI32(&val64, 321) == 0)

	return ST_B_TRUE;
	#undef LOC_COMPARE_
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_addUI32()
 *       st_sysUInt64_cmpUI32()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__01_addUI(void)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           valCmp64;
	int        cmp;
	Tst_bool   of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)

	/* */
	st_sysUInt64_setLoHi(&val64, 2000000008, 0);
	TEST_M64__prf64UI(cFNC, "Set 2000000008: ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 1999999999, NULL);
	TEST_M64__prf64UI(cFNC, "Add 1999999999: ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 189999999, NULL);
	TEST_M64__prf64UI(cFNC, "Add  189999999: ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 104967289, &of);
	TEST_M64__prf64UI(cFNC, "Add  104967289: ", &val64);
	/* */
	cmp = st_sysUInt64_cmpUI32(&val64, 0xffffffff);
	if (cmp == 0 && ! of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	st_sysUInt64_addUI32(&val64, 1, NULL);
	TEST_M64__prf64UI(cFNC, "Add          1: ", &val64);
	/* */
	cmp = st_sysUInt64_cmpUI32(&val64, 0xffffffff);
	if (cmp > 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	st_sysUInt64_addUI32(&val64, 1, NULL);
	TEST_M64__prf64UI(cFNC, "Add          1: ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 0xfffffffe, NULL);
	TEST_M64__prf64UI(cFNC, "Add 0xfffffffe: ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 1, &of);
	TEST_M64__prf64UI(cFNC, "Add          1: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0, 2);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0 && ! of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setLoHi(&val64, 0xfffffffe, 0xffffffff);
	TEST_M64__prf64UI(cFNC, "Set           : ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 1, &of);
	TEST_M64__prf64UI(cFNC, "Add          1: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0xffffffff, 0xffffffff);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
#	if (SYSMATH_OVERFLOW_CHECK_ != 1)
	of = ST_B_FALSE;
#	endif
	if (cmp == 0 && ! of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	st_sysUInt64_addUI32(&val64, 1, &of);
	TEST_M64__prf64UI(cFNC, "Add          1: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0, 0);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
#	if (SYSMATH_OVERFLOW_CHECK_ != 1)
	of = ST_B_TRUE;
#	endif
	if (cmp == 0 && of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setLoHi(&val64, 2000000000, 0);
	TEST_M64__prf64UI(cFNC, "Set 2000000000: ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 2000000000, NULL);
	TEST_M64__prf64UI(cFNC, "Add 2000000000: ", &val64);
	/* */
	st_sysUInt64_addUI32(&val64, 311160000, NULL);
	TEST_M64__prf64UI(cFNC, "Add  311160000: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0x00f714c0, 1);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_addUI32()
 *       st_sysUInt64_cmpUI32()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__01_addSI_sub(const Tst_int32 set, const Tst_int32 add,
		const Tst_int32 res, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          valCmp64;
	int       cmp;
	Tst_bool  of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)

	/* */
	st_sysSInt64_setI32(&val64, set);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	st_sysSInt64_addI32(&val64, add, &of);
	TEST_M64__prf32SI_dec(cFNC, "Add : ", add);
	/* */
	st_sysSInt64_setI32(&valCmp64, res);
	cmp = (st_sysSInt64_toI32(&val64, NULL) ==
				st_sysSInt64_toI32(&valCmp64, NULL) ? 0 : -1);
	TEST_M64__prf32SI_dec(cFNC, "    = ",
			st_sysSInt64_toI32(&val64, NULL));
	TEST_M64__prf64SI_hexAdec(cFNC, "      ", &val64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cmp == 0 && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		if (cmp != 0) {
			TEST_M64__prf32SI_dec(cFNC, "   != ",
					st_sysSInt64_toI32(&valCmp64, NULL));
			TEST_M64__prf64SI_hexAdec(cFNC, "      ", &valCmp64);
		}
		if (of != ofExp) {
			TEST_FCOM__prf(cFNC, "  overflow flag didn't match");
		}
		return ST_B_FALSE;
	}

	return ST_B_TRUE;
}

Tst_bool
TEST_M64__01_addSI(void)
{
	if (! TEST_M64__01_addSI_sub(0, 0, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__01_addSI_sub(0, 1, 1, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__01_addSI_sub(1, 0, 1, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__01_addSI_sub(1, 2, 3, ST_B_FALSE)) { return ST_B_FALSE; }

	if (! TEST_M64__01_addSI_sub(0, -1, -1, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__01_addSI_sub(-1, 0, -1, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__01_addSI_sub(1, -1, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__01_addSI_sub(-1, 1, 0, ST_B_FALSE)) { return ST_B_FALSE; }

	if (! TEST_M64__01_addSI_sub(-1, -1, -2, ST_B_FALSE)) { return ST_B_FALSE; }

	if (! TEST_M64__01_addSI_sub(-10, -34, -44, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__01_addSI_sub((Tst_int32)0x80000000, (Tst_int32)0x80000000, 0, ST_B_FALSE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_subUI32()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__02_subUI(void)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           valCmp64;
	int        cmp;
	Tst_bool   of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	/* */
	st_sysUInt64_setLoHi(&val64, 0x00f714c0, 1);
	TEST_M64__prf64UI(cFNC, "Set           : ", &val64);
	/* */
	st_sysUInt64_subUI32(&val64, 311160000, NULL);
	TEST_M64__prf64UI(cFNC, "Sub  311160000: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0xee6b2800, 0);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	st_sysUInt64_subUI32(&val64, 2000000000, NULL);
	TEST_M64__prf64UI(cFNC, "Sub 2000000000: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 2000000000, 0);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	st_sysUInt64_subUI32(&val64, 2000000000, NULL);
	TEST_M64__prf64UI(cFNC, "Sub 2000000000: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0, 0);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setLoHi(&val64, 1, 1);
	TEST_M64__prf64UI(cFNC, "Set           : ", &val64);
	/* */
	st_sysUInt64_subUI32(&val64, 1, NULL);
	TEST_M64__prf64UI(cFNC, "Sub          1: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0, 1);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	st_sysUInt64_subUI32(&val64, 0xffffffff, NULL);
	TEST_M64__prf64UI(cFNC, "Sub 0xffffffff: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 1, 0);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	st_sysUInt64_subUI32(&val64, 2, &of);
	TEST_M64__prf64UI(cFNC, "Sub          2: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0xffffffff, 0xffffffff);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ST_B_TRUE;
	#endif
	if (cmp == 0 && of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setLoHi(&val64, 0, 35623);
	TEST_M64__prf64UI(cFNC, "Set           : ", &val64);
	/* */
	st_sysUInt64_subUI32(&val64, 12345, &of);
	TEST_M64__prf64UI(cFNC, "Sub      12345: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0xffffcfc7, 0x8b26);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ST_B_FALSE;
	#endif
	if (cmp == 0 && ! of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setLoHi(&val64, 3, 0);
	TEST_M64__prf64UI(cFNC, "Set           : ", &val64);
	/* */
	st_sysUInt64_subUI32(&val64, 12345, &of);
	TEST_M64__prf64UI(cFNC, "Sub      12345: ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, 0xffffcfca, 0xffffffff);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ST_B_TRUE;
	#endif
	if (cmp == 0 && of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_subI32()
 *       st_sysSInt64_cmpI64()
 */
Tst_bool
TEST_M64__02_subSI_sub(const Tst_int32 set, const Tst_int32 sub,
		const Tst_int32 res)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          valCmp64;
	int       cmp;
	Tst_bool  of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	/* */
	st_sysSInt64_setI32(&val64, set);
	TEST_M64__prf64SI(cFNC, "Set  : ", &val64);
	/* */
	st_sysSInt64_subI32(&val64, sub, &of);
	TEST_M64__prf32SI_dec(cFNC, "Sub  : ", sub);
	TEST_M64__prf64SI(cFNC, "     = ", &val64);
	TEST_M64__prf32SI_dec(cFNC, "     = ",
			st_sysSInt64_toI32(&val64, NULL));
	/* */
	st_sysSInt64_setI32(&valCmp64, res);
	cmp = st_sysSInt64_cmpI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ST_B_FALSE;
	#endif
	if (cmp == 0 && ! of) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prf64SI(cFNC, "     != ", &valCmp64);
		TEST_M64__prf32SI_dec(cFNC, "      = ",
				st_sysSInt64_toI32(&valCmp64, NULL));
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__02_subSI(void)
{
	if (! TEST_M64__02_subSI_sub(1, 1, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__02_subSI_sub(1, 0, 1)) { return ST_B_FALSE; }
	if (! TEST_M64__02_subSI_sub(789, 109, 680)) { return ST_B_FALSE; }
	if (! TEST_M64__02_subSI_sub(1, 2, -1)) { return ST_B_FALSE; }
	if (! TEST_M64__02_subSI_sub(3, 5, -2)) { return ST_B_FALSE; }

	if (! TEST_M64__02_subSI_sub(3, -5, 8)) { return ST_B_FALSE; }
	if (! TEST_M64__02_subSI_sub(-3, 5, -8)) { return ST_B_FALSE; }

	if (! TEST_M64__02_subSI_sub(-3, -3, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__02_subSI_sub(-624, -625, 1)) { return ST_B_FALSE; }
	if (! TEST_M64__02_subSI_sub(-624, -623, -1)) { return ST_B_FALSE; }
	/* */
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_multUI32s()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__03_mulUI_sub(const Tst_uint32 factA, const Tst_uint32 factB,
		const Tst_uint32 resLo, const Tst_uint32 resHi)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           valCmp64;
	int        cmp;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	/* */
	st_sysUInt64_multUI32s(&val64, factA, factB);
	TEST_M64__prf32UI_dec(cFNC, "Mul  : ", factA);
	TEST_M64__prf32UI_dec(cFNC, "     * ", factB);
	TEST_M64__prf64UI(cFNC, "     = ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prf64UI(cFNC, "    != ", &valCmp64);
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__03_mulUI(void)
{
	if (! TEST_M64__03_mulUI_sub(12345, 74553,
				0x36db87b1, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulUI_sub(12345, 0,
				0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulUI_sub(123456, 123456,
				0x8C751000, 3)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulUI_sub(1234567, 1234567,
				0xdec1fb31, 0x162)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulUI_sub(0xffffffff, 0x0fffffff,
				0xf0000001, 0xffffffe)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulUI_sub(0xffffffff, 0xffffffff,
				0x00000001, 0xfffffffe)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_multUI32s()
 *       st_sysSInt64_cmpUI64()
 */
Tst_bool
TEST_M64__03_mulSI_sub(const Tst_int32 factA, const Tst_int32 factB,
		const Tst_uint32 resLo, const Tst_uint32 resHi)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          valCmp64;
	int       cmp;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	/* */
	st_sysSInt64_multI32s(&val64, factA, factB);
	TEST_M64__prf32SI_dec(cFNC, "Mul  : ", factA);
	TEST_M64__prf32SI_dec(cFNC, "     * ", factB);
	TEST_M64__prf64SI(cFNC, "     = ", &val64);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysSInt64_cmpI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__03_mulSI(void)
{
	if (! TEST_M64__03_mulSI_sub(0, 0, 0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(0, 1, 0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(1, 0, 0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(0, -1, 0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(-1, 0, 0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(-13, 0, 0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(0, -86, 0, 0)) { return ST_B_FALSE; }

	if (! TEST_M64__03_mulSI_sub(12345, 74553,
				0x36db87b1, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(12345, 0,
				0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(123456, 123456,
				0x8C751000, 3)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(1234567, 1234567,
				0xdec1fb31, 0x162)) { return ST_B_FALSE; }

	if (! TEST_M64__03_mulSI_sub(1000, -33,
				0xffff7f18, 0xffffffff)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(-1000, 33,
				0xffff7f18, 0xffffffff)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(-1000, -33,
				33000, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(-1, 1,
				0xffffffff, 0xffffffff)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(1, -1,
				0xffffffff, 0xffffffff)) { return ST_B_FALSE; }
	if (! TEST_M64__03_mulSI_sub(-1, -1,
				1, 0)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_divUI32()
 *       st_sysUInt64_divUI32_rem()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__04_divUI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 div,
		const Tst_uint32 resLo, const Tst_uint32 resHi, const Tst_uint32 resRem)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           valCmp64;
	Tst_uint32 rem = 0;
	int        cmp;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64UI_hexAdec(cFNC, "Set1 : ", &val64);
	/* */
	TEST_M64__prf32UI_hexAdec(cFNC, "Div1 / ", div);
	st_sysUInt64_divUI32(&val64, div);
	TEST_M64__prf64UI_hexAdec(cFNC, "     = ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prf64UI_hexAdec(cFNC, "res != ", &valCmp64);
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	/* */
	TEST_FCOM__prf(cFNC, "Div2 / ");
	st_sysUInt64_divUI32_rem(&val64, div, &rem);
	TEST_M64__prf64UI_hexAdec(cFNC, "     = ", &val64);
	TEST_M64__prf32UI_hexAdec(cFNC,    "     + ", rem);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0 && resRem == rem) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		if (cmp != 0) {
			TEST_M64__prf64UI_hexAdec(cFNC, "res != ", &valCmp64);
		}
		if (resRem != rem) {
			TEST_M64__prf32UI_hexAdec(cFNC, "rem != ", resRem);
		}
		return ST_B_FALSE;
	}

	return ST_B_TRUE;
}

Tst_bool
TEST_M64__04_divUI(void)
{
	if (! TEST_M64__04_divUI_sub(0x00000000, 0x00000000,
			0x00000000,
			0x00000000, 0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x00000002, 0x00000000,
			0x00000000,
			0x00000000, 0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x00000002, 0x00000000,
			0x00000002,
			0x00000001, 0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x00000000, 0x00000002,
			0x00000002,
			0x00000000, 0x00000001, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x00000002, 0x00000002,
			0x00000002,
			0x00000001, 0x00000001, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x00000003, 0x00000000,
			0x00000002,
			0x00000001, 0x00000000, 0x00000001)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x0000000f, 0x00000000,
			0x00000004,
			0x00000003, 0x00000000, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0xf1abcd56, 0x00000000,
			0x00005634,
			0x0002cdb2, 0x00000000, 0x0000392e)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x00000064, 0x00000000,
			0x00000063,
			0x00000001, 0x00000000, 0x00000001)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x00000064, 0x00000000,
			0x00000065,
			0x00000000, 0x00000000, 0x00000064)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0x4000435e, 0x0000a453,
			0x00007775,
			0x602760d0, 0x00000001, 0x0000544e)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0xfd6c8c1f, 0xf7b3e4a2,
			0x84217319,
			0xdfeb06eb, 0x00000001, 0x7b124e2c)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divUI_sub(0xfd6c8c1f, 0xf7b3e4a2,
			0x0000567a,
			0xbe6be243, 0x0002dd48, 0x00003631)) { return ST_B_FALSE; }
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_divI32()
 *       st_sysSInt64_divI32_rem()
 *       st_sysSInt64_cmpUI64()
 */
Tst_bool
TEST_M64__04_divSI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_int32 div,
		const Tst_uint32 resLo, const Tst_uint32 resHi,
		const Tst_int32 resRem)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          valCmp64;
	Tst_int32 rem = 0;
	int       cmp;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set1 : ", &val64);
	/* */
	TEST_M64__prf32SI_hexAdec(cFNC, "Div1 / ", div);
	st_sysSInt64_divI32(&val64, div);
	TEST_M64__prf64SI_hexAdec(cFNC, "     = ", &val64);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysSInt64_cmpI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prf64SI_hexAdec(cFNC, "res != ", &valCmp64);
		return ST_B_FALSE;
	}

	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	/* */
	TEST_FCOM__prf(cFNC, "Div2 / ");
	st_sysSInt64_divI32_rem(&val64, div, &rem);
	TEST_M64__prf64SI_hexAdec(cFNC, "     = ", &val64);
	TEST_M64__prf32SI_hexAdec(cFNC,    "     + ", rem);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysSInt64_cmpI64(&val64, &valCmp64);
	if (cmp == 0 && resRem == rem) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		if (cmp != 0) {
			TEST_M64__prf64SI_hexAdec(cFNC, "res != ", &valCmp64);
		}
		if (resRem != rem) {
			TEST_M64__prf32SI_hexAdec(cFNC, "rem != ", resRem);
		}
		return ST_B_FALSE;
	}

	return ST_B_TRUE;
}

Tst_bool
TEST_M64__04_divSI(void)
{
	if (! TEST_M64__04_divSI_sub(0x00000000, 0x00000000,
			0x00000000,
			0x00000000, 0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(0x00000002, 0x00000000,
			0x00000000,
			0x00000000, 0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(0x00000002, 0x00000000,
			0x00000002,
			0x00000001, 0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(0x00000000, 0x00000002,
			0x00000002,
			0x00000000, 0x00000001, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(0x00000002, 0x00000002,
			0x00000002,
			0x00000001, 0x00000001, 0x00000000)) { return ST_B_FALSE; }

	if (! TEST_M64__04_divSI_sub(0xfffff002, 0xffffffff,  /* -4094 / 87 */
			87,
			0xffffffd1, 0xffffffff, -5)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(4094, 0,  /* 4094 / -87 */
			-87,
			0xffffffd1, 0xffffffff, 0x00000005)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(0xfffff002, 0xffffffff,  /* -4094 / -87 */
			-87,
			47, 0, -5)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(4094, 0,  /* 4094 / 87 */
			87,
			47, 0, 5)) { return ST_B_FALSE; }

	if (! TEST_M64__04_divSI_sub(0xffffffff, 0x7fffffff,
			0x7fffffff,
			0x00000002, 0x00000001, 1)) { return ST_B_FALSE; }
	if (! TEST_M64__04_divSI_sub(0x00000000, 0x80000000,
			0x7fffffff,
			0xfffffffe, 0xfffffffe, -2)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       sysILog_UI32()
 *       sysILog_UI64()
 */
Tst_bool
TEST_M64__05_log_sub(const Tst_uint32 val32, const Tst_uint32 logRes)
{
	const char *cFNC = __func__;
	Tst_uint32 log;

	TEST_M64__prf32UI(cFNC, "Set : ", val32);
	/* */
	log = sysILog2_UI32(val32);
	TEST_M64__prf32UI_dec(cFNC, "Log = ", log);
	if (log == logRes) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__05_log_sub2(const Tst_uint32 lo, const Tst_uint32 hi,
		const Tst_uint32 logRes)
{
	const char *cFNC = __func__;
	Tst_uint64 val64;
	Tst_uint32 log;

	ST_SYSMATH_STC_RSETUI64(val64)
	st_sysUInt64_setLoHi(&val64, lo, hi);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	log = sysILog2_UI64(&val64);
	TEST_M64__prf32UI_dec(cFNC, "Log = ", log);
	if (log == logRes) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__05_log(void)
{
	/* */
	if (! TEST_M64__05_log_sub(0, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(1, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(2, 1)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(3, 1)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(4, 2)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(5, 2)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(6, 2)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(7, 2)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(8, 3)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(15, 3)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(16, 4)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(31, 4)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(32, 5)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(33, 5)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(63, 5)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(64, 6)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(65, 6)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(78, 6)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(111, 6)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(127, 6)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(128, 7)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(129, 7)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(133, 7)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(190, 7)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(254, 7)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(255, 7)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(256, 8)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(257, 8)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(510, 8)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(511, 8)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(512, 9)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(513, 9)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(1023, 9)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(1024, 10)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(1025, 10)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x000007ffU, 10)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00000800U, 11)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x0000080fU, 11)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00000920U, 11)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00000fffU, 11)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00001000U, 12)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00001010U, 12)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00001fffU, 12)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00010f0fU, 16)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x0001eeeeU, 16)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x0003abcdU, 17)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00070000U, 18)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x000f0000U, 19)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00100100U, 20)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00200000U, 21)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00500000U, 22)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00700000U, 22)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x00900000U, 23)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x009573abU, 23)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x01111111U, 24)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x01ffffffU, 24)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x02222222U, 25)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x03aefb40U, 25)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x04000000U, 26)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x0f000000U, 27)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x10000000U, 28)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x18435234U, 28)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x28435234U, 29)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x38435234U, 29)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x48435234U, 30)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x58435234U, 30)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x78435234U, 30)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x88435234U, 31)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0x98435234U, 31)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub(0xb547455aU, 31)) { return ST_B_FALSE; }

	/* */
	if (! TEST_M64__05_log_sub2(0x00000000U, 0x00000000U, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x00000001U, 0x00000000U, 0)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x00000002U, 0x00000000U, 1)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x00000003U, 0x00000000U, 1)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0xffffffffU, 0x00000000U, 31)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x00000000U, 0x00000001U, 32)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x00000001U, 0x00000001U, 32)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x0034ab00U, 0x00000002U, 33)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x10203041U, 0x00000003U, 33)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x00000000U, 0x00000004U, 34)) { return ST_B_FALSE; }
	if (! TEST_M64__05_log_sub2(0x80000000U, 0x00000004U, 34)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_addUI64()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__06_addUI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 addLo, const Tst_uint32 addHi,
		const Tst_uint32 resLo, const Tst_uint32 resHi, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           add64,
	           valCmp64;
	int        cmp;
	Tst_bool   of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(add64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	st_sysUInt64_setLoHi(&add64, addLo, addHi);
	TEST_M64__prf64UI(cFNC, "Add + ", &add64);
	st_sysUInt64_addUI64(&val64, &add64, &of);
	TEST_M64__prf64UI(cFNC, "    = ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cmp == 0 && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__06_addUI(void)
{
	if (! TEST_M64__06_addUI_sub(0x01234567, 0x89abcdef,
			0x01234567, 0x89abcdef,
			0x02468ace, 0x13579bde, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addUI_sub(0x01234567, 0x89abcdef,
			0x00000000, 0x00000001,
			0x01234567, 0x89abcdf0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addUI_sub(0xffffffff, 0xefffffff,
			0x00000001, 0x00000000,
			0x00000000, 0xf0000000, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addUI_sub(0x0150e512, 0x109fad32,
			0xff314351, 0x04400000,
			0x00822863, 0x14dfad33, ST_B_FALSE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_addSI64()
 *       st_sysSInt64_cmpSI64()
 */
Tst_bool
TEST_M64__06_addSI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 addLo, const Tst_uint32 addHi,
		const Tst_uint32 resLo, const Tst_uint32 resHi, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          add64,
	          valCmp64;
	int       cmp;
	Tst_bool  of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(add64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	st_sysSInt64_setLoHi(&add64, addLo, addHi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Add + ", &add64);
	st_sysSInt64_addI64(&val64, &add64, &of);
	TEST_M64__prf64SI_hexAdec(cFNC, "    = ", &val64);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysSInt64_cmpI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cmp == 0 && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__06_addSI(void)
{
	if (! TEST_M64__06_addSI_sub(0x01234567, 0x89abcdef,
			0x01234567, 0x89abcdef,
			0x02468ace, 0x13579bde, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0x01234567, 0x89abcdef,
			0x00000000, 0x00000001,
			0x01234567, 0x89abcdf0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0xffffffff, 0xefffffff,
			0x00000001, 0x00000000,
			0x00000000, 0xf0000000, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0x0150e512, 0x109fad32,
			0xff314351, 0x04400000,
			0x00822863, 0x14dfad33, ST_B_FALSE)) { return ST_B_FALSE; }

	if (! TEST_M64__06_addSI_sub(0x0150e512, 0x809fad32,
			0xff314351, 0x04400000,
			0x00822863, 0x84dfad33, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0xffffffff, 0x7fffffff,
			0x00000001, 0x00000000,
			0x00000000, 0x80000000, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0x00000000, 0x80000000,
			0xffffffff, 0xffffffff,
			0xffffffff, 0x7fffffff, ST_B_TRUE)) { return ST_B_FALSE; }

	if (! TEST_M64__06_addSI_sub(0x01430000, 0xff042030,  /* -a + +b */
			0xa6443b00, 0x0023faff,
			0xa7873b00, 0xff281b2f, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0x11430043, 0x3a89213a,  /* +a + -b */
			0xa6443b00, 0x8023faff,
			0xb7873b43, 0xbaad1c39, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0x95e2bccc, 0xa627814b,  /* -a + -b */
			0xa6443b00, 0x8023faff,
			0x3c26f7cc, 0x264b7c4b, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__06_addSI_sub(0x95e2bccc, 0xa627814b,  /* -a + -b */
			0xa6443b00, 0xf0001111,
			0x3c26f7cc, 0x9627925d, ST_B_FALSE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_subUI64()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__07_subUI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 subLo, const Tst_uint32 subHi,
		const Tst_uint32 resLo, const Tst_uint32 resHi, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           sub64,
	           valCmp64;
	int        cmp;
	Tst_bool   of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(sub64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64UI(cFNC, "Set : ", &val64);
	/* */
	st_sysUInt64_setLoHi(&sub64, subLo, subHi);
	TEST_M64__prf64UI(cFNC, "Sub - ", &sub64);
	st_sysUInt64_subUI64(&val64, &sub64, &of);
	TEST_M64__prf64UI(cFNC, "    = ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cmp == 0 && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__07_subUI(void)
{
	if (! TEST_M64__07_subUI_sub(0x01234567, 0x89abcdef,
			0x01234568, 0x89abcdef,
			0xffffffff, 0xffffffff, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x01234567, 0x89abcdef,
			0x92efacde, 0x89abcdef,
			0x6e339889, 0xffffffff, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x01234567, 0x89abcdef,
			0x92efacde, 0xbabcdef0,
			0x6e339889, 0xceeeeefe, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x01234567, 0x89abcdef,
			0x01234567, 0x89abcdef,
			0, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x01234567, 0x89abcdef,
			0x01234566, 0x89abcdef,
			1, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x01234567, 0x89abcdef,
			0x01234566, 0x00000000,
			0x00000001, 0x89abcdef, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x01234567, 0x89abcdef,
			0x01234567, 0x00000000,
			0x00000000, 0x89abcdef, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x00a5f088, 0x00534edf,
			0x00a5f089, 0x00000000,
			0xffffffff, 0x00534ede, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x00a5f088, 0x00534ed0,
			0x02a5f087, 0x00400000,
			0xfe000001, 0x00134ecf, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x00000000, 0x00534ed0,
			0x00000001, 0x00534ecf,
			0xffffffff, 0x00000000, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x00351abc, 0,
			0x00051abc, 0,
			0x00300000, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subUI_sub(0x00351abc, 0,
			0x00051abd, 0,
			0x002fffff, 0, ST_B_FALSE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_subI64()
 *       st_sysSInt64_cmpI64()
 */
Tst_bool
TEST_M64__07_subSI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 subLo, const Tst_uint32 subHi,
		const Tst_uint32 resLo, const Tst_uint32 resHi, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          sub64,
	          valCmp64;
	int       cmp;
	Tst_bool  of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(sub64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set : ", &val64);
	/* */
	st_sysSInt64_setLoHi(&sub64, subLo, subHi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Sub - ", &sub64);
	st_sysSInt64_subI64(&val64, &sub64, &of);
	TEST_M64__prf64SI_hexAdec(cFNC, "    = ", &val64);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysSInt64_cmpI64(&val64, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cmp == 0 && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		if (cmp != 0) {
			TEST_M64__prf64SI_hexAdec(cFNC, "   != ", &valCmp64);
		}
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__07_subSI(void)
{
	if (! TEST_M64__07_subSI_sub(0x01234567, 0x89abcdef,
			0x01234568, 0x89abcdef,
			0xffffffff, 0xffffffff, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x01234567, 0x89abcdef,
			0x92efacde, 0x89abcdef,
			0x6e339889, 0xffffffff, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x01234567, 0x89abcdef,
			0x92efacde, 0xbabcdef0,
			0x6e339889, 0xceeeeefe, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x01234567, 0x89abcdef,
			0x01234567, 0x89abcdef,
			0, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x01234567, 0x89abcdef,
			0x01234566, 0x89abcdef,
			1, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x01234567, 0x89abcdef,
			0x01234566, 0x00000000,
			0x00000001, 0x89abcdef, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x01234567, 0x89abcdef,
			0x01234567, 0x00000000,
			0x00000000, 0x89abcdef, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00a5f088, 0x00534edf,
			0x00a5f089, 0x00000000,
			0xffffffff, 0x00534ede, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00a5f088, 0x00534ed0,
			0x02a5f087, 0x00400000,
			0xfe000001, 0x00134ecf, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00000000, 0x00534ed0,
			0x00000001, 0x00534ecf,
			0xffffffff, 0x00000000, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00351abc, 0,
			0x00051abc, 0,
			0x00300000, 0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00351abc, 0,
			0x00051abd, 0,
			0x002fffff, 0, ST_B_FALSE)) { return ST_B_FALSE; }

	if (! TEST_M64__07_subSI_sub(0x00351abc, 0x35670001,  /* +a - -b */
			0x00051abd, 0x80010000,
			0x002fffff, 0xb5660001, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00351abc, 0x35670001,  /* +a - -b */
			0x33333333, 0xfaaaaaaa,
			0xcd01e789, 0x3abc5556, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00351abc, 0xc5670001,  /* -a - +b */
			0x44455444, 0x70102030,
			0xbbefc678, 0x5556dfd0, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00351abc, 0xc5670001,  /* -a - +b */
			0x44455444, 0x00002030,
			0xbbefc678, 0xc566dfd0, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__07_subSI_sub(0x00351abc, 0xc5670001,  /* -a - -b */
			0x44455444, 0xa0002030,
			0xbbefc678, 0x2566dfd0, ST_B_FALSE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_multUI64s()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__08_mulUI_sub(const Tst_uint32 mulAlo, const Tst_uint32 mulAhi,
		const Tst_uint32 mulBlo, const Tst_uint32 mulBhi,
		const Tst_uint32 resLo, const Tst_uint32 resHi, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_uint64 val64a,
	           val64b,
	           val64r,
	           valCmp64;
	int        cmp;
	Tst_bool   of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETUI64(val64a)
	ST_SYSMATH_STC_RSETUI64(val64b)
	ST_SYSMATH_STC_RSETUI64(val64r)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	/* */
	st_sysUInt64_setLoHi(&val64a, mulAlo, mulAhi);
	TEST_M64__prf64UI(cFNC, "Mul : ", &val64a);
	/* */
	st_sysUInt64_setLoHi(&val64b, mulBlo, mulBhi);
	TEST_M64__prf64UI(cFNC, "    * ", &val64b);
	st_sysUInt64_multUI64s(&val64r, &val64a, &val64b, &of);
	TEST_M64__prf64UI(cFNC, "    = ", &val64r);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysUInt64_cmpUI64(&val64r, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cmp == 0 && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__08_mulUI(void)
{
	if (! TEST_M64__08_mulUI_sub(0x00000000, 0x00000001,
			0x00000000, 0x00000001,
			0, 0, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulUI_sub(0x00000000, 0x00000002,
			0x80000000, 0x00000000,
			0, 0, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulUI_sub(0x00000000, 0x00000001,
			0x00005000, 0x00000000,
			0x00000000, 0x00005000, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulUI_sub(0x00000000, 0x00000001,
			0x80000000, 0x00000000,
			0x00000000, 0x80000000, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulUI_sub(0x54534343, 0x00000000,
			0xffff0000, 0x00000000,
			0xbcbd0000, 0x5452eeef, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulUI_sub(0x04534343, 0x00000000,
			0xf0000001, 0x0000003f,
			0xd4534343, 0x148b9c8b, ST_B_TRUE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_multI64s()
 *       st_sysSInt64_cmpI64()
 */
Tst_bool
TEST_M64__08_mulSI_sub(const Tst_uint32 mulAlo, const Tst_uint32 mulAhi,
		const Tst_uint32 mulBlo, const Tst_uint32 mulBhi,
		const Tst_uint32 resLo, const Tst_uint32 resHi, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_int64 val64a,
	          val64b,
	          val64r,
	          valCmp64;
	int       cmp;
	Tst_bool  of = ST_B_FALSE;

	ST_SYSMATH_STC_RSETSI64(val64a)
	ST_SYSMATH_STC_RSETSI64(val64b)
	ST_SYSMATH_STC_RSETSI64(val64r)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	/* */
	st_sysSInt64_setLoHi(&val64a, mulAlo, mulAhi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Mul : ", &val64a);
	/* */
	st_sysSInt64_setLoHi(&val64b, mulBlo, mulBhi);
	TEST_M64__prf64SI_hexAdec(cFNC, "    * ", &val64b);
	st_sysSInt64_multI64s(&val64r, &val64a, &val64b, &of);
	TEST_M64__prf64SI_hexAdec(cFNC, "    = ", &val64r);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysSInt64_cmpI64(&val64r, &valCmp64);
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cmp == 0 && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__08_mulSI(void)
{
	if (! TEST_M64__08_mulSI_sub(0x00000000, 0x00000001,
			0x00000000, 0x00000001,
			0, 0, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x00000000, 0x00000002,
			0x80000000, 0x00000000,
			0, 0, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x00000000, 0x00000001,
			0x00005000, 0x00000000,
			0x00000000, 0x00005000, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x00000000, 0x00000001,
			0x80000000, 0x00000000,
			0x00000000, 0x80000000, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x54534343, 0x00000000,
			0xffff0000, 0x00000000,
			0xbcbd0000, 0x5452eeef, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x04534343, 0x00000000,
			0xf0000001, 0x0000003f,
			0xd4534343, 0x148b9c8b, ST_B_TRUE)) { return ST_B_FALSE; }

	if (! TEST_M64__08_mulSI_sub(0x04534343, 0x20405000,  /* +a * -b */
			0xfffff001, 0xffffffff,
			0x2fe0ecbd, 0xe4bfb045, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x04534343, 0x00005030,  /* +a * -b */
			0xfffff001, 0xffffffff,
			0xd01f1343, 0xfafd4fea, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x04534343, 0xffffff30,  /* -a * +b */
			0x00453451, 0x00000000,
			0x6910e433, 0xc7c6a983, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x04534343, 0xffffff30,  /* -a * +b */
			0x10453451, 0x00000000,
			0x9910e433, 0xc80bddb7, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x04534343, 0xffffff30,  /* -a * -b */
			0xff453451, 0xffffffff,
			0x2610e433, 0x97c25640, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__08_mulSI_sub(0x04534343, 0xffffff30,  /* -a * -b */
			0xfffa3aa1, 0xffffffff,
			0x02087b23, 0x04b0443a, ST_B_FALSE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_divUI64()
 *       st_sysUInt64_divUI64_rem()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__09_divUI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 divLo, const Tst_uint32 divHi,
		const Tst_uint32 resLo, const Tst_uint32 resHi,
		const Tst_uint32 resRemLo, const Tst_uint32 resRemHi)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           div64,
	           rem64,
	           valCmp64,
	           valRem64;
	int        cmp1,
	           cmp2;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(div64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	ST_SYSMATH_STC_RSETUI64(rem64)
	ST_SYSMATH_STC_RSETUI64(valRem64)
	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64UI(cFNC, "Set1 : ", &val64);
	/* */
	st_sysUInt64_setLoHi(&div64, divLo, divHi);
	TEST_M64__prf64UI(cFNC, "Div1 / ", &div64);
	st_sysUInt64_divUI64(&val64, &div64);
	TEST_M64__prf64UI(cFNC, "     = ", &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp1 = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp1 == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prf64UI(cFNC, "res != ", &valCmp64);
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	/* */
	st_sysUInt64_setLoHi(&div64, divLo, divHi);
	TEST_FCOM__prf(cFNC, "Div2 / ");
	st_sysUInt64_divUI64_rem(&val64, &div64, &rem64);
	TEST_M64__prf64UI(cFNC, "     = ", &val64);
	TEST_M64__prf64UI(cFNC, "     + ", &rem64);
	/* */
	st_sysUInt64_setLoHi(&valRem64, resRemLo, resRemHi);
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp1 = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	cmp2 = st_sysUInt64_cmpUI64(&rem64, &valRem64);
	if (cmp1 == 0 && cmp2 == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		if (cmp1 != 0) {
			TEST_M64__prf64UI(cFNC, "res != ", &valCmp64);
		}
		if (cmp2 != 0) {
			TEST_M64__prf64UI(cFNC, "rem != ", &valRem64);
		}
		return ST_B_FALSE;
	}

	return ST_B_TRUE;
}

Tst_bool
TEST_M64__09_divUI(void)
{
	if (! TEST_M64__09_divUI_sub(0x00000000, 0x00000000,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x00000000, 0x00000002,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x00000000, 0x00000000,
			0x00000000, 0x00000002,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x10444477, 0xf9900000,
			0x33000000, 0xd0101402,
			0x00000001, 0x00000000,
			0xdd444477, 0x297febfd)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x10444477, 0xf9900000,
			0x33000000, 0x0e101402,
			0x00000011, 0x00000000,
			0xad444477, 0x0a7eabda)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x10444477, 0xf9900000,
			0x00918273, 0x0d1b983f,
			0x00000013, 0x00000000,
			0x057795ee, 0x0083b353)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x00000000, 0xfedcba98,
			0x00918273, 0x00001234,
			0x000e0042, 0x00000000,
			0xb8325e5a, 0x0000013a)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x00000000, 0xfedcba98,
			0x00000002, 0x00000004,
			0x3fb72ea5, 0x00000000,
			0x8091a2b6, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x00000000, 0xfedcba98,
			0x00005fe6, 0x00000000,
			0x34ae4487, 0x0002a85a,
			0x000055b6, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x03450000, 0x00000000,
			0x00005fe6, 0x00000000,
			0x000008ba, 0x00000000,
			0x000022e4, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x03450000, 0x00000000,
			0x04005fe6, 0x00000000,
			0x00000000, 0x00000000,
			0x03450000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x00000000, 0x0000eb60,
			0x00000000, 0x0000ec00,
			0x00000000, 0x00000000,
			0x00000000, 0x0000eb60)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divUI_sub(0x00000000, 0x0000eb60,
			0x00000000, 0x0000ec00,
			0x00000000, 0x00000000,
			0x00000000, 0x0000eb60)) { return ST_B_FALSE; }
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_divI64()
 *       st_sysSInt64_divI64_rem()
 *       st_sysSInt64_cmpI64()
 */
Tst_bool
TEST_M64__09_divSI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 divLo, const Tst_uint32 divHi,
		const Tst_uint32 resLo, const Tst_uint32 resHi,
		const Tst_uint32 resRemLo, const Tst_uint32 resRemHi)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          div64,
	          rem64,
	          valCmp64,
	          valRem64;
	int       cmp1,
	          cmp2;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(div64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	ST_SYSMATH_STC_RSETSI64(rem64)
	ST_SYSMATH_STC_RSETSI64(valRem64)
	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Set1 : ", &val64);
	/* */
	st_sysSInt64_setLoHi(&div64, divLo, divHi);
	TEST_M64__prf64SI_hexAdec(cFNC, "Div1 / ", &div64);
	st_sysSInt64_divI64(&val64, &div64);
	TEST_M64__prf64SI_hexAdec(cFNC, "     = ", &val64);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp1 = st_sysSInt64_cmpI64(&val64, &valCmp64);
	if (cmp1 == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prf64SI_hexAdec(cFNC, "res != ", &valCmp64);
		return ST_B_FALSE;
	}

	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	/* */
	st_sysSInt64_setLoHi(&div64, divLo, divHi);
	TEST_FCOM__prf(cFNC, "Div2 / ");
	st_sysSInt64_divI64_rem(&val64, &div64, &rem64);
	TEST_M64__prf64SI_hexAdec(cFNC, "     = ", &val64);
	TEST_M64__prf64SI_hexAdec(cFNC, "     + ", &rem64);
	/* */
	st_sysSInt64_setLoHi(&valRem64, resRemLo, resRemHi);
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp1 = st_sysSInt64_cmpI64(&val64, &valCmp64);
	cmp2 = st_sysSInt64_cmpI64(&rem64, &valRem64);
	if (cmp1 == 0 && cmp2 == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		if (cmp1 != 0) {
			TEST_M64__prf64SI_hexAdec(cFNC, "res != ", &valCmp64);
		}
		if (cmp2 != 0) {
			TEST_M64__prf64SI_hexAdec(cFNC, "rem != ", &valRem64);
		}
		return ST_B_FALSE;
	}

	return ST_B_TRUE;
}

Tst_bool
TEST_M64__09_divSI(void)
{
	if (! TEST_M64__09_divSI_sub(0x00000000, 0x00000000,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x00000000, 0x00000002,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x00000000, 0x00000000,
			0x00000000, 0x00000002,
			0x00000000, 0x00000000,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x10444477, 0xf9900000,
			0x33000000, 0xd0101402,
			0x00000000, 0x00000000,
			0x10444477, 0xf9900000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x10444477, 0xf9900000,
			0x33000000, 0x0e101402,
			0x0000000, 0x00000000,
			0x10444477, 0xf9900000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x10444477, 0xf9900000,
			0x00918273, 0x0d1b983f,
			0x00000000, 0x00000000,
			0x10444477, 0xf9900000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x00000000, 0xfedcba98,
			0x00918273, 0x00001234,
			0xfffff000, 0xffffffff,
			0x18273000, 0xfffffaa1)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x03450000, 0x00000000,
			0x00005fe6, 0x00000000,
			0x000008ba, 0x00000000,
			0x000022e4, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x03450000, 0x00000000,
			0x04005fe6, 0x00000000,
			0x00000000, 0x00000000,
			0x03450000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x00000000, 0x0000eb60,
			0x00000000, 0x0000ec00,
			0x00000000, 0x00000000,
			0x00000000, 0x0000eb60)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x00000000, 0x0000eb60,
			0x00000000, 0x0000ec00,
			0x00000000, 0x00000000,
			0x00000000, 0x0000eb60)) { return ST_B_FALSE; }

	if (! TEST_M64__09_divSI_sub(0x00140000, 0x7030eb60,  /* +a / -b */
			0x00234200, 0xffffff30,
			0xff75eb16, 0xffffffff,
			0x75765400, 0x00000044)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x00140000, 0x8030eb60,  /* -a / +b */
			0x00234200, 0x0fffff30,
			0xfffffff9, 0xffffffff,
			0x010ace00, 0xf030e5b0)) { return ST_B_FALSE; }
	if (! TEST_M64__09_divSI_sub(0x00140000, 0x8030eb60,  /* -a / -b */
			0x11111111, 0xf3455130,
			0x0000000a, 0x00000000,
			0x55695556, 0xff7bbf7f)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_shiftRight()
 *       st_sysUInt64_shiftLeft()
 *       st_sysUInt64_cmpUI64()
 */
Tst_bool
TEST_M64__10_shiftUI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_int32 shift, const Tst_bool toLeft,
		const Tst_uint32 resLo, const Tst_uint32 resHi)
{
	const char *cFNC = __func__;
	Tst_uint64 val64,
	           valCmp64;
	int        cmp;

	ST_SYSMATH_STC_RSETUI64(val64)
	ST_SYSMATH_STC_RSETUI64(valCmp64)
	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64UI(cFNC, "Set    :", &val64);
	/* */
	if (toLeft) {
		st_sysUInt64_shiftLeft(&val64, shift);
	} else {
		st_sysUInt64_shiftRight(&val64, shift);
	}
	TEST_M64__prf64UI_sh(cFNC, shift, toLeft, &val64);
	/* */
	st_sysUInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysUInt64_cmpUI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__10_shiftUI(void)
{
	const char *cFNC = __func__;

	TEST_FCOM__prf(cFNC, "\n\tto left");
	if (! TEST_M64__10_shiftUI_sub(0x00000000, 0x00000000,
			0, ST_B_TRUE,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000001, 0x00000000,
			0, ST_B_TRUE,
			0x00000001, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000001, 0x00000000,
			31, ST_B_TRUE,
			0x80000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000001, 0x00000000,
			32, ST_B_TRUE,
			0x00000000, 0x00000001)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto right");
	if (! TEST_M64__10_shiftUI_sub(0x00000000, 0x00000000,
			0, ST_B_FALSE,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000001, 0x00000000,
			0, ST_B_FALSE,
			0x00000001, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000000, 0x80000000,
			31, ST_B_FALSE,
			0x00000000, 0x00000001)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000000, 0x80000000,
			32, ST_B_FALSE,
			0x80000000, 0x00000000)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto left");
	if (! TEST_M64__10_shiftUI_sub(0xf0000000, 0x00000000,
			2, ST_B_TRUE,
			0xc0000000, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xf0000001, 0x00000000,
			2, ST_B_TRUE,
			0xc0000004, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xf0000001, 0x00000001,
			2, ST_B_TRUE,
			0xc0000004, 0x00000007)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xf0000001, 0x30000001,
			2, ST_B_TRUE,
			0xc0000004, 0xc0000007)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00ff00ff, 0x00000000,
			16, ST_B_TRUE,
			0x00ff0000, 0x000000ff)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto left (right)");
	if (! TEST_M64__10_shiftUI_sub(0xf0000000, 0x00000000,
			-2, ST_B_TRUE,
			0x3c000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000000, 0x0000000f,
			-2, ST_B_TRUE,
			0xc0000000, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xabcdefab, 0x12345678,
			-16, ST_B_TRUE,
			0x5678abcd, 0x00001234)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xabcdefab, 0x12345678,
			-17, ST_B_TRUE,
			0x2b3c55e6, 0x0000091a)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xabcdefab, 0x12345678,
			-31, ST_B_TRUE,
			0x2468acf1, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xabcdefab, 0x12345678,
			-32, ST_B_TRUE,
			0x12345678, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xabcdefab, 0x12345678,
			-33, ST_B_TRUE,
			0x091A2B3C, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x96969696, 0xa5a5a5a5,
			-1, ST_B_TRUE,
			0xCB4B4B4B, 0x52D2D2D2)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0xabcdefab, 0x82345678,
			-63, ST_B_TRUE,
			0x00000001, 0x00000000)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto right (left)");
	if (! TEST_M64__10_shiftUI_sub(0x80604020, 0x99999999,
			-8, ST_B_FALSE,
			0x60402000, 0x99999980)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x80604020, 0x99999999,
			-32, ST_B_FALSE,
			0x00000000, 0x80604020)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x80604020, 0x99999999,
			-33, ST_B_FALSE,
			0x00000000, 0x00C08040)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x96969696, 0xa5a5a5a5,
			-1, ST_B_FALSE,
			0x2D2D2D2C, 0x4B4B4B4B)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftUI_sub(0x00000001, 0x99999999,
			-63, ST_B_FALSE,
			0x00000000, 0x80000000)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_shiftRight()
 *       st_sysSInt64_shiftLeft()
 *       st_sysSInt64_cmpI64()
 */
Tst_bool
TEST_M64__10_shiftSI_sub(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_int32 shift, const Tst_bool toLeft,
		const Tst_uint32 resLo, const Tst_uint32 resHi)
{
	const char *cFNC = __func__;
	Tst_int64 val64,
	          valCmp64;
	int       cmp;

	ST_SYSMATH_STC_RSETSI64(val64)
	ST_SYSMATH_STC_RSETSI64(valCmp64)
	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64SI(cFNC, "Set    :", &val64);
	/* */
	if (toLeft) {
		st_sysSInt64_shiftLeft(&val64, shift);
	} else {
		st_sysSInt64_shiftRight(&val64, shift);
	}
	TEST_M64__prf64SI_sh(cFNC, shift, toLeft, &val64);
	/* */
	st_sysSInt64_setLoHi(&valCmp64, resLo, resHi);
	cmp = st_sysSInt64_cmpI64(&val64, &valCmp64);
	if (cmp == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__10_shiftSI(void)
{
	const char *cFNC = __func__;

	TEST_FCOM__prf(cFNC, "\n\tto left");
	if (! TEST_M64__10_shiftSI_sub(0x00000000, 0x00000000,
			0, ST_B_TRUE,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000001, 0x00000000,
			0, ST_B_TRUE,
			0x00000001, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000001, 0x00000000,
			31, ST_B_TRUE,
			0x80000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000001, 0x00000000,
			32, ST_B_TRUE,
			0x00000000, 0x00000001)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto right");
	if (! TEST_M64__10_shiftSI_sub(0x00000000, 0x00000000,
			0, ST_B_FALSE,
			0x00000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000001, 0x00000000,
			0, ST_B_FALSE,
			0x00000001, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000000, 0x80000000,
			31, ST_B_FALSE,
			0x00000000, 0xffffffff)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000000, 0x80000000,
			32, ST_B_FALSE,
			0x80000000, 0xffffffff)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto left");
	if (! TEST_M64__10_shiftSI_sub(0xf0000000, 0x00000000,
			2, ST_B_TRUE,
			0xc0000000, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xf0000001, 0x00000000,
			2, ST_B_TRUE,
			0xc0000004, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xf0000001, 0x00000001,
			2, ST_B_TRUE,
			0xc0000004, 0x00000007)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xf0000001, 0x30000001,
			2, ST_B_TRUE,
			0xc0000004, 0xc0000007)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00ff00ff, 0x00000000,
			16, ST_B_TRUE,
			0x00ff0000, 0x000000ff)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto left (right)");
	if (! TEST_M64__10_shiftSI_sub(0xf0000000, 0x00000000,
			-2, ST_B_TRUE,
			0x3c000000, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000000, 0x0000000f,
			-2, ST_B_TRUE,
			0xc0000000, 0x00000003)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xabcdefab, 0x12345678,
			-16, ST_B_TRUE,
			0x5678abcd, 0x00001234)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xabcdefab, 0x12345678,
			-17, ST_B_TRUE,
			0x2b3c55e6, 0x0000091a)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xabcdefab, 0x12345678,
			-31, ST_B_TRUE,
			0x2468acf1, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xabcdefab, 0x12345678,
			-32, ST_B_TRUE,
			0x12345678, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xabcdefab, 0x12345678,
			-33, ST_B_TRUE,
			0x091A2B3C, 0x00000000)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x96969696, 0xa5a5a5a5,
			-1, ST_B_TRUE,
			0xCB4B4B4B, 0xd2D2D2D2)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xabcdefab, 0x82345678,
			-63, ST_B_TRUE,
			0xffffffff, 0xffffffff)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0xabcdefab, 0x82345678,
			-5, ST_B_TRUE,
			0xc55e6f7d, 0xfc11a2b3)) { return ST_B_FALSE; }

	TEST_FCOM__prf(cFNC, "\n\tto right (left)");
	if (! TEST_M64__10_shiftSI_sub(0x80604020, 0x99999999,
			-8, ST_B_FALSE,
			0x60402000, 0x99999980)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x80604020, 0x99999999,
			-32, ST_B_FALSE,
			0x00000000, 0x80604020)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x80604020, 0x99999999,
			-33, ST_B_FALSE,
			0x00000000, 0x00C08040)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x96969696, 0xa5a5a5a5,
			-1, ST_B_FALSE,
			0x2D2D2D2C, 0x4B4B4B4B)) { return ST_B_FALSE; }
	if (! TEST_M64__10_shiftSI_sub(0x00000001, 0x99999999,
			-63, ST_B_FALSE,
			0x00000000, 0x80000000)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysUInt64_setLoHi()
 *       st_sysUInt64_setUI64()
 *       st_sysUInt64_cmpUI64()
 *       st_sysUInt64_toUI32()
 *       st_sysUInt64_toDecStr()
 *       st_sysUInt64_setDecStr()
 */
Tst_bool
TEST_M64__11_restUI_sub1(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 resC)
{
	const char *cFNC = __func__;
	Tst_uint64 val64;
	Tst_uint32 cnv = 0;
	Tst_bool   of  = ST_B_FALSE;

	ST_SYSMATH_STC_RSETUI64(val64)
	/* */
	st_sysUInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64UI(cFNC, "Set  : ", &val64);
	/* */
	cnv = st_sysUInt64_toUI32(&val64, &of);
	TEST_M64__prf32UI(cFNC, "Conv : ", cnv);
	/* */
	if (cnv == resC) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restUI_sub2(const Tst_uint32 setLo, const Tst_uint32 setHi)
{
	const char *cFNC = __func__;
	Tst_uint64 val64a,
	           val64b;

	ST_SYSMATH_STC_RSETUI64(val64a)
	ST_SYSMATH_STC_RSETUI64(val64b)
	/* */
	st_sysUInt64_setLoHi(&val64a, setLo, setHi);
	TEST_M64__prf64UI(cFNC, "Set  : ", &val64a);
	/* */
	st_sysUInt64_setUI64(&val64b, &val64a);
	TEST_M64__prf64UI(cFNC, "     : ", &val64b);
	/* */
	if (st_sysUInt64_cmpUI64(&val64a, &val64b) == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	/* */
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restUI_sub3(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const char *pStr)
{
	const char *cFNC = __func__;
	Tst_uint64 val64a,
	           val64c;
	Tst_str    nstr[50];

	ST_SYSMATH_STC_RSETUI64(val64a)
	ST_SYSMATH_STC_RSETUI64(val64c)
	/* */
	st_sysUInt64_setLoHi(&val64a, setLo, setHi);
	TEST_M64__prf64UI(cFNC, "Set LoHi : ", &val64a);
	/* */
	st_sysUInt64_toDecStr(&val64a, (Tst_str*)nstr, sizeof(nstr));
	TEST_M64__prfStr(cFNC, "         = ", (char*)nstr);
	/* */
	if (strstr(pStr, "+") != NULL || strcmp(pStr, (char*)nstr) == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	/* */
	st_sysUInt64_setDecStr(&val64a, (const Tst_str*)pStr);
	TEST_M64__prfStr(cFNC, "Set DStr : ", pStr);
	/* */
	TEST_M64__prf64UI(cFNC, "         = ", &val64a);
	/* */
	st_sysUInt64_setLoHi(&val64c, setLo, setHi);
	if (st_sysUInt64_cmpUI64(&val64a, &val64c) == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restUI_sub4(const char *pStr, const Tst_bool resExp)
{
	const char *cFNC = __func__;
	Tst_uint64 val64a;

	ST_SYSMATH_STC_RSETUI64(val64a)

	TEST_M64__prfStr(cFNC, "Set DStr : ", pStr);
	if (resExp == st_sysUInt64_setDecStr(&val64a, (const Tst_str*)pStr)) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restUI(void)
{
	if (! TEST_M64__11_restUI_sub1(0x00000001, 0x00000000,
			0x00000001)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub1(0xffffffff, 0x00000000,
			0xffffffff)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub1(0x00000000, 0x00000001,
			0x00000000)) { return ST_B_FALSE; }

	if (! TEST_M64__11_restUI_sub2(0x00000001, 0x00000001)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub2(0x10000001, 0x10000001)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub2(0xa3b3c3d3, 0x95959595)) { return ST_B_FALSE; }

	if (! TEST_M64__11_restUI_sub3(0, 0, "0")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0, 0, "+")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(1, 0, "1")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(2, 0, "2")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(2, 0, "+2")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(10, 0, "10")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(100, 0, "100")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(101, 0, "101")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(33, 0, "33")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(32, 0, "32")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(789, 0, "789")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(788, 0, "788")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(342789, 0, "342789")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(34278925, 0, "34278925")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(34278924, 0, "34278924")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0xffffffff, 0x00000000, "4294967295"))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0x00000000, 0x00000001, "4294967296"))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0x77654321, 0x00000001, "6298092321"))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0x7ffeecab, 0x00000001, "6442380459"))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0x87654321, 0x00000001, "6566527777"))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0x87654321, 0x00000010, "70991037217"))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0x87654321, 0xabcdef87, "12379814316082545441"))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub3(0xffffffff, 0xffffffff, "18446744073709551615"))
		{ return ST_B_FALSE; }

	if (! TEST_M64__11_restUI_sub4("+132", ST_B_TRUE))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub4("-132", ST_B_FALSE))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub4("", ST_B_TRUE))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub4("+", ST_B_TRUE))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub4("++132", ST_B_FALSE))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub4(" 132", ST_B_FALSE))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub4("+ 132", ST_B_FALSE))
		{ return ST_B_FALSE; }
	if (! TEST_M64__11_restUI_sub4("+1 32", ST_B_FALSE))
		{ return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysSInt64_setLoHi()
 *       st_sysSInt64_setI32()
 *       st_sysSInt64_toI32()
 *       st_sysSInt64_toDecStr()
 *       st_sysSInt64_setDecStr()
 */
Tst_bool
TEST_M64__11_restSI_sub1(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const Tst_uint32 resC, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_int64 val64;
	Tst_int32 cnv = 0;
	Tst_bool  of  = ST_B_FALSE;

	ST_SYSMATH_STC_RSETSI64(val64)
	/* */
	st_sysSInt64_setLoHi(&val64, setLo, setHi);
	TEST_M64__prf64SI(cFNC, "Set  : ", &val64);
	/* */
	cnv = st_sysSInt64_toI32(&val64, &of);
	TEST_M64__prf32SI(cFNC, "Conv : ", cnv);
	TEST_M64__prf32SI_dec(cFNC, "     : ", cnv);
	/* */
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cnv == (Tst_int32)resC && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restSI_sub2(const Tst_int32 set32,
		const Tst_uint32 resC, const Tst_bool ofExp)
{
	const char *cFNC = __func__;
	Tst_int64 val64;
	Tst_int32 cnv = 0;
	Tst_bool  of  = ST_B_FALSE;

	ST_SYSMATH_STC_RSETSI64(val64)
	/* */
	st_sysSInt64_setI32(&val64, set32);
	TEST_M64__prf64SI(cFNC, "Set  : ", &val64);
	/* */
	cnv = st_sysSInt64_toI32(&val64, &of);
	TEST_M64__prf32SI(cFNC, "Conv : ", cnv);
	TEST_M64__prf32SI_dec(cFNC, "     : ", cnv);
	/* */
	#if (SYSMATH_OVERFLOW_CHECK_ != 1)
		of = ofExp;
	#endif
	if (cnv == (Tst_int32)resC && of == ofExp) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}

	/* */
	st_sysSInt64_setSign(&val64, ! st_sysSInt64_getSign(&val64));
	TEST_M64__prf64SI(cFNC, "Sign : ", &val64);
	/* */
	cnv = st_sysSInt64_toI32(&val64, NULL);
	TEST_M64__prf32SI(cFNC, "Conv : ", cnv);
	TEST_M64__prf32SI_dec(cFNC, "     : ", cnv);
	/* */
	if (cnv == (Tst_int32)resC * -1) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restSI_sub3(const Tst_uint32 setLo, const Tst_uint32 setHi,
		const char *pStr)
{
	const char *cFNC = __func__;
	Tst_int64 val64a,
	          val64c;
	Tst_str   nstr[50];

	ST_SYSMATH_STC_RSETSI64(val64a)
	ST_SYSMATH_STC_RSETSI64(val64c)
	/* */
	st_sysSInt64_setLoHi(&val64a, setLo, setHi);
	TEST_M64__prf64SI(cFNC, "Set LoHi : ", &val64a);
	/* */
	st_sysSInt64_toDecStr(&val64a, (Tst_str*)nstr, sizeof(nstr));
	TEST_M64__prfStr(cFNC, "         = ", (char*)nstr);
	/* */
	if (strcmp(pStr, (char*)nstr) == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prfStr(cFNC, "        != ", pStr);
		return ST_B_FALSE;
	}

	/* */
	st_sysSInt64_setDecStr(&val64a, (const Tst_str*)pStr);
	TEST_M64__prfStr(cFNC, "Set DStr : ", pStr);
	/* */
	TEST_M64__prf64SI(cFNC, "         = ", &val64a);
	/* */
	st_sysSInt64_setLoHi(&val64c, setLo, setHi);
	if (st_sysSInt64_cmpI64(&val64a, &val64c) == 0) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		TEST_M64__prf64SI_hexAdec(cFNC, "        != ", &val64c);
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restSI_sub4(const char *pStr, const Tst_bool resExp)
{
	const char *cFNC = __func__;
	Tst_int64 val64a;

	ST_SYSMATH_STC_RSETSI64(val64a)

	TEST_M64__prfStr(cFNC, "Set DStr : ", pStr);
	if (resExp == st_sysSInt64_setDecStr(&val64a, (const Tst_str*)pStr)) {
		TEST_FCOM__prf(cFNC, "Correct");
	} else {
		TEST_FCOM__prf(cFNC, "! Wrong !");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

Tst_bool
TEST_M64__11_restSI(void)
{
	if (! TEST_M64__11_restSI_sub1(0x00000001, 0x00000000,
			0x00000001, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub1(0x80808080, 0x00000002,
			0x80808080, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub1(0xffffffff, 0xffffffff,
			0xffffffff, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub1(0x00000001, 0x80000000,
			0x00000001, ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub1(0x71234567, 0xabcdedcb,
			0x71234567, ST_B_TRUE)) { return ST_B_FALSE; }

	if (! TEST_M64__11_restSI_sub2(-100000000, 0xfa0a1f00, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub2(7452051, 0x0071b593, ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub2(32, 0x00000020, ST_B_FALSE)) { return ST_B_FALSE; }

	if (! TEST_M64__11_restSI_sub3(0, 0, "0")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(1, 0, "1")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(2, 0, "2")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(33, 0, "33")) { return ST_B_FALSE; }

	if (! TEST_M64__11_restSI_sub3(-1, -1, "-1")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(0xFDF10851, 0xFFFFFFFF, "-34535343")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(0xE023A34E, 0xFFFFFFFF, "-534535346")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(0xFDF10851, 0xF302341b, "-936128477710448559")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(0, 0x80000000, "-9223372036854775808")) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub3(0xffffffff, 0x7fffffff, "9223372036854775807")) { return ST_B_FALSE; }

	if (! TEST_M64__11_restSI_sub4("+132", ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("-132", ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("", ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("+", ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("-", ST_B_TRUE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("++132", ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("-+132", ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4(" 132", ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("+ 132", ST_B_FALSE)) { return ST_B_FALSE; }
	if (! TEST_M64__11_restSI_sub4("+1 32", ST_B_FALSE)) { return ST_B_FALSE; }

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

Tst_bool
TEST_M64__bench(void)
{
	#define LOC_SO_RNDS  4096
	#define LOC_TEST_ADD  1
	#define LOC_TEST_SUB  1
	#define LOC_TEST_MUL  1
	#define LOC_TEST_DIV  1
	#define LOC_TEST_DIV_N1  1
	#define LOC_TEST_DIV_N2  1
	#define LOC_TEST_DIV_N3  1
	#define LOC_TEST_DIV_N4  1  /* div with checking */

	const char *cFNC = __func__;
	const Tst_uint32 cRUNS   = 0xfffffff,
	                 cRUNS_D = 0xffffff;
	Tst_uint32 x;
	#if (LOC_TEST_MUL == 1 || LOC_TEST_DIV == 1)
		Tst_uint32 y,
		           yStart;
	#endif
	#if (LOC_TEST_DIV == 1)
		#if (LOC_TEST_DIV_N1 == 1 || LOC_TEST_DIV_N2 == 1)
			Tst_uint32 rem = 0;
		#endif
		#if (LOC_TEST_DIV_N3 == 1 || LOC_TEST_DIV_N4 == 1)
			Tst_uint64 rem64;
		#endif
		#if (LOC_TEST_DIV_N4 == 1)
			Tst_uint64 orgVal64,
			           mulVal64;
		#endif
	#endif
	Tst_uint64 val64a,
	           val64b;
	Tst_uint32 rnds[LOC_SO_RNDS];
	double     timeB,
	           timeE;

	TEST_FCOM__prf(cFNC, "Init...");
	ST_SYSMATH_STC_RSETUI64(val64a)
	ST_SYSMATH_STC_RSETUI64(val64b)
	for (x = 0; x < LOC_SO_RNDS; x++) {
		rnds[x] = st_sysGetRand(0, 0xfffffff);
	}
	printf("\n");

	#if (LOC_TEST_ADD == 1)
		/* */
		/** emulated:  OF 2.1s  nOF 1.5s
		 ** native  :     2.2s      1.3s */
		TEST_FCOM__prf(cFNC, "Start ADD#1...");
		timeB = st_sysGetTime();
		printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS);
		/* */
		for (x = 0; x < cRUNS; x++) {
			st_sysUInt64_addUI32(&val64a, x, NULL);
		}
		/* */
		timeE = st_sysGetTime();
		TEST_M64__prf64UI(cFNC, "Res: ", &val64a);
		TEST_FCOM__prf(cFNC, "Done.");
		printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);

		/* */
		/** emulated:  OF 2.4s  nOF 1.6s
		 ** native  :     2.3s      1.4s */
		TEST_FCOM__prf(cFNC, "Start ADD#2...");
		timeB = st_sysGetTime();
		printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS);
		/* */
		st_sysUInt64_setLoHi(&val64a, 0, 0);
		for (x = 0; x < cRUNS; x++) {
			st_sysUInt64_addUI32(&val64a, rnds[x % LOC_SO_RNDS], NULL);
		}
		/* */
		timeE = st_sysGetTime();
		TEST_FCOM__prf(cFNC, "Done.");
		printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);
	#endif

	#if (LOC_TEST_SUB == 1)
		/* */
		/** emulated:  OF 2.1s  nOF 1.5s
		 ** native  :     2.5s      1.4s */
		TEST_FCOM__prf(cFNC, "Start SUB#1...");
		timeB = st_sysGetTime();
		printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS);
		/* */
		st_sysUInt64_setLoHi(&val64a, 0xffffffff, 0xffffffff);
		for (x = 0; x < cRUNS; x++) {
			st_sysUInt64_subUI32(&val64a, x, NULL);
		}
		/* */
		timeE = st_sysGetTime();
		TEST_M64__prf64UI(cFNC, "Res: ", &val64a);
		TEST_FCOM__prf(cFNC, "Done.");
		printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);

		/* */
		/** emulated:  OF 2.5s  nOF 1.6s
		 ** native  :     2.6s      1.5s  */
		TEST_FCOM__prf(cFNC, "Start SUB#2...");
		timeB = st_sysGetTime();
		printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS);
		/* */
		st_sysUInt64_setLoHi(&val64a, 0xffffffff, 0xffffffff);
		for (x = 0; x < cRUNS; x++) {
			st_sysUInt64_subUI32(&val64a, rnds[x % LOC_SO_RNDS], NULL);
		}
		/* */
		timeE = st_sysGetTime();
		TEST_FCOM__prf(cFNC, "Done.");
		printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);
	#endif

	#if (LOC_TEST_MUL == 1)
		/* */
		/** emulated: OF 4.8s  nOF 4.8s
		 ** native  :    1.7s      1.7s */
		TEST_FCOM__prf(cFNC, "Start MUL#1...");
		timeB = st_sysGetTime();
		printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS);
		/* */
		st_sysUInt64_setLoHi(&val64a, 0, 0);
		yStart = cRUNS + cRUNS / 2 - 13;
		y      = yStart;
		for (x = 1; x <= cRUNS; x++) {
			st_sysUInt64_multUI32s(&val64a, x, y);
			if (--y == 0) {
				y = yStart;
			}
		}
		/* */
		timeE = st_sysGetTime();
		TEST_M64__prf64UI(cFNC, "Res: ", &val64a);
		TEST_FCOM__prf(cFNC, "Done.");
		printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);

		/* */
		/** emulated: OF 4.8s  nOF 4.8s
		 ** native  :    1.8s      1.8s  */
		TEST_FCOM__prf(cFNC, "Start MUL#2...");
		timeB = st_sysGetTime();
		printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS);
		/* */
		st_sysUInt64_setLoHi(&val64a, 0, 0);
		for (x = 1; x <= cRUNS; x++) {
			st_sysUInt64_multUI32s(&val64a, x, rnds[x % LOC_SO_RNDS]);
		}
		/* */
		timeE = st_sysGetTime();
		TEST_FCOM__prf(cFNC, "Done.");
		printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);
	#endif

	#if (LOC_TEST_DIV == 1)
		#if (LOC_TEST_DIV_N1 == 1)
			/* */
			/** emulated:  OF 2.9s  nOF 2.9s
			 ** native  :     0.8s      0.8s */
			TEST_FCOM__prf(cFNC, "Start DIV#1...");
			timeB = st_sysGetTime();
			printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS_D);
			/* */
			st_sysUInt64_setLoHi(&val64a, 0xffffffff, 0xffffffff);
			yStart = cRUNS + cRUNS / 2 - 13;
			y      = yStart;
			for (x = 1; x <= cRUNS_D; x++) {
				st_sysUInt64_divUI32_rem(&val64a, y, NULL);
				if (y % 4 == 0) {
					st_sysUInt64_setLoHi(&val64a, 0xffffffff, 0xffffffff);
				}
				if (--y == 0) {
					y = yStart;
				}
			}
			/* */
			timeE = st_sysGetTime();
			TEST_M64__prf64UI(cFNC, "Res: ", &val64a);
			TEST_FCOM__prf(cFNC, "Done.");
			printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);
		#endif

		#if (LOC_TEST_DIV_N2 == 1)
			/* */
			/** emulated:  OF 2.9s  nOF 2.9s
			 ** native  :     1.3s      1.4s  */
			TEST_FCOM__prf(cFNC, "Start DIV#2...");
			timeB = st_sysGetTime();
			printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS_D);
			/* */
			st_sysUInt64_setLoHi(&val64a, 0xffffffff, 0xffffffff);
			yStart = cRUNS + cRUNS / 2 - 13;
			y      = yStart;
			for (x = 1; x <= cRUNS_D; x++) {
				st_sysUInt64_divUI32_rem(&val64a, y, &rem);
				if (y % 4 == 0) {
					st_sysUInt64_setLoHi(&val64a, 0xffffffff, 0xffffffff);
				}
				if (--y == 0) {
					y = yStart;
				}
			}
			/* */
			timeE = st_sysGetTime();
			TEST_FCOM__prf(cFNC, "Done.");
			printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);
		#endif

		#if (LOC_TEST_DIV_N3 == 1)
			/* */
			/** emulated:  OF 3.8s  nOF 3.6s
			 ** native  :     2.0s      2.0s    */
			TEST_FCOM__prf(cFNC, "Start DIV#3...");
			timeB = st_sysGetTime();
			printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS_D);
			/* */
			ST_SYSMATH_STC_RSETUI64(rem64)
			yStart = cRUNS + cRUNS / 2 - 13;
			y      = yStart;
			for (x = 1; x <= cRUNS_D; x++) {
				st_sysUInt64_setLoHi(&val64a, x, rnds[x % LOC_SO_RNDS] + y);
				st_sysUInt64_setLoHi(&val64b, rnds[y % LOC_SO_RNDS], x);
				st_sysUInt64_divUI64_rem(&val64a, &val64b, &rem64);
				/*if (st_sysUInt64_cmpLoHi(&val64a, 0, 0) == 0) {
					TEST_M64__prf64UI(cFNC, "a  ", &val64a);
					TEST_M64__prf64UI(cFNC, "b  ", &val64b);
				}*/
				if (--y == 0) {
					y = yStart;
				}
			}
			/* */
			timeE = st_sysGetTime();
			TEST_FCOM__prf(cFNC, "Done.");
			printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);
		#endif

		#if (LOC_TEST_DIV_N4 == 1)
			/* */
			/** emulated: OF 9.3s  nOF 8.4s
			 ** native  :    5.3s      5.0s  */
			TEST_FCOM__prf(cFNC, "Start DIV#4...");
			timeB = st_sysGetTime();
			printf("\ttime %.2f  (runs %u)\n", timeB, cRUNS_D * 2);
			/* */
			ST_SYSMATH_STC_RSETUI64(rem64)
			ST_SYSMATH_STC_RSETUI64(orgVal64)
			ST_SYSMATH_STC_RSETUI64(mulVal64)
			yStart = cRUNS + cRUNS / 2 - 13;
			y      = yStart;
			for (x = 1; x <= cRUNS_D * 2; x++) {
				st_sysUInt64_setLoHi(&val64a, x, rnds[x % LOC_SO_RNDS] + y);
				st_sysUInt64_setUI64(&orgVal64, &val64a);
				st_sysUInt64_setLoHi(&val64b, rnds[y % LOC_SO_RNDS], x);
				st_sysUInt64_divUI64_rem(&val64a, &val64b, &rem64);
				/* */
				st_sysUInt64_multUI64s(&mulVal64, &val64a, &val64b, NULL);
				st_sysUInt64_addUI64(&mulVal64, &rem64, NULL);
				if (st_sysUInt64_cmpUI64(&mulVal64, &orgVal64) != 0) {
					TEST_FCOM__prf(cFNC, "Div or Mul failed");
					break;
				}
				if (--y == 0) {
					y = yStart;
				}
			}
			/* */
			timeE = st_sysGetTime();
			TEST_FCOM__prf(cFNC, "Done.");
			printf("\ttime %.2f  --> %.2fs\n\n", timeE, timeE - timeB);
		#endif
	#endif

	/* */
	return ST_B_TRUE;

	#undef LOC_SO_RNDS
	#undef LOC_TEST_ADD
	#undef LOC_TEST_SUB
	#undef LOC_TEST_MUL
	#undef LOC_TEST_DIV
	#undef LOC_TEST_DIV_N1
	#undef LOC_TEST_DIV_N2
	#undef LOC_TEST_DIV_N3
	#undef LOC_TEST_DIV_N4
}

/******************************************************************************/
