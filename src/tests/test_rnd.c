/******************************************************************************/
/* main_rnd.c               [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for generating Pseudo-Random-Numbers                         */
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

/*
// System-Includes
*/
//#include <stdlib.h>       /* exit(), calloc(), getenv() */
//#include <string.h>       /* memset() */
#include <stdarg.h>       /* va_list, ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void TEST__prf(const char *pFnc, const char *pMsg, ...);
void TEST__prf32(const char *pFnc, const char *pMsg, Tst_uint32 v32);
void TEST__prf32dec(const char *pFnc, const char *pMsg, Tst_uint32 v32);
/* */
Tst_bool TEST__0_genInt(void);
Tst_bool TEST__1_genDbl(void);
/** */
/**void sysTestRand(void);
void sysTestRandDbl(void);**/

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	if (! TEST__0_genInt()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST__1_genDbl()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	printf("All tests passed :-)\n");
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
TEST__prf(const char *pFnc, const char *pMsg, ...)
{
	char    msgBuf[1024];
	va_list args;

	va_start(args, pMsg);

	vsnprintf(msgBuf, sizeof(msgBuf), pMsg, args);
	printf("%s(): %s\n", pFnc, msgBuf);
	va_end(args);
}

void
TEST__prf32(const char *pFnc, const char *pMsg, const Tst_uint32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("0x%08x\n", v32);
}

void
TEST__prf32dec(const char *pFnc, const char *pMsg, const Tst_uint32 v32)
{
	printf("%s(): %s ", pFnc, pMsg);
	printf("%u\n", v32);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysGetRand()
 */
Tst_bool
TEST__0_genInt(void)
{
	const char *cFNCN = __func__;
	const Tst_uint32 cLOWER = 100;
	const Tst_uint32 cUPPER = 120;
	Tst_uint32 x;
	Tst_uint32 val;

	st_sysInitRand(0);

	for (x = 0; x < (cUPPER - cLOWER) * 10; x++) {
		val = st_sysGetRand(cLOWER, cUPPER);
		if (val < cLOWER || val > cUPPER) {
			TEST__prf(cFNCN, "error: val %u not in range %u .. %u", val, cLOWER, cUPPER);
			return ST_B_FALSE;
		}
	}

	TEST__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysGetRand()
 */
Tst_bool
TEST__1_genDbl(void)
{
	const char *cFNCN = __func__;
	const double cLOWER = 100.0f;
	const double cUPPER = 120.0f;
	Tst_uint32 x;
	double val;

	st_sysInitRand(0);

	for (x = 0; x < (Tst_uint32)(cUPPER - cLOWER) * 10; x++) {
		val = st_sysGetRandDbl(cLOWER, cUPPER);
		if (val < cLOWER || val > cUPPER) {
			TEST__prf(cFNCN, "error: val %f not in range %f .. %f", val, cLOWER, cUPPER);
			return ST_B_FALSE;
		}
	}

	TEST__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


/**
void
__sysTestRand_initVals(Tst_uint32 *pArr, const Tst_uint32 so)
{
	int x;

	__sysf_prf(" initV\n");
	for (x = 0; x < so; x++)
		pArr[x] = 0;
}

void
__sysTestRand_printZeroVals(Tst_uint32 *pArr, const Tst_uint32 so,
		const Tst_uint32 min, const Tst_uint32 max)
{
	const int MAXWC = 50;
	int x,
	    wc = 0;

	__sysf_prf(" print\n");
	if (max >= so)
		__sysf_prf("   !!! max >= so\n");
	else
		for (x = min; x <= max; x++)
			if (pArr[x] == 0) {
				__sysf_prf("  %5u : %u\n", x, pArr[x]);
				wc++;
				if (wc >= MAXWC) {
					__sysf_prf("   stopping here\n");
					exit(1);
				}
			}
}

void
__sysTestRand_getRndVals(Tst_uint32 *pArr, const Tst_uint32 so,
		const Tst_uint32 min, const Tst_uint32 max,
		const Tst_uint32 runs)
{
	int        x;
	Tst_uint32 val;

	__sysTestRand_initVals(pArr, so);
	__sysf_prf(" start %u - %u\n", min, max);
	if (max >= so)
		__sysf_prf("   !!! max >= so\n");
	else {
		for (x = 0; x < runs; x++) {
			val = st_sysGetRand(min, max);
			if ((min > 0 && val < min) || val > max)
				__sysf_prf("   --> val %u out of range\n", val);
			else
				pArr[val]++;
		}
	}
}

void
__sysTestRand_getRndValsDbl(const double min, const double max,
		const Tst_uint32 runs)
{
	int    x;
	double val;

	__sysf_prf(" start %f - %f\n", min, max);
	for (x = 0; x < runs; x++) {
		val = st_sysGetRandDbl(min, max);
		** __sysf_prf("   %f\n", val); **
		if (val == min || val == max)
			__sysf_prf("   at border %f\n", val);
	}
}

void
sysTestRand(void)
{
	int        x;
	Tst_uint32 vals[10000],
	           so = sizeof(vals) / sizeof(Tst_uint32),
	           val,
	           min,
	           max;
	const int RUNS = so * 10;

	min = 0;
	max = so - 1;
	__sysf_prf(" initR #0\n");
	st_sysInitRand(0);
	__sysTestRand_getRndVals((Tst_uint32*)&vals, so, min, max, RUNS);
	__sysTestRand_printZeroVals((Tst_uint32*)&vals, so, min, max);

	min = 1;
	max = so - 1;
	__sysf_prf(" initR #1\n");
	st_sysInitRand(1);
	__sysTestRand_getRndVals((Tst_uint32*)&vals, so, min, max, RUNS);
	__sysTestRand_printZeroVals((Tst_uint32*)&vals, so, min, max);

	min = 2;
	max = so - 1;
	__sysf_prf(" initR #2\n");
	st_sysInitRand(2);
	__sysTestRand_getRndVals((Tst_uint32*)&vals, so, min, max, RUNS);
	__sysTestRand_printZeroVals((Tst_uint32*)&vals, so, min, max);

	min = 2;
	max = (so - 1) - 1;
	__sysf_prf(" initR #3\n");
	st_sysInitRand(3);
	__sysTestRand_getRndVals((Tst_uint32*)&vals, so, min, max, RUNS);
	__sysTestRand_printZeroVals((Tst_uint32*)&vals, so, min, max);

	min = 0;
	max = so / 10;
	__sysf_prf(" initR #4\n");
	st_sysInitRand(4);
	__sysTestRand_getRndVals((Tst_uint32*)&vals, so, min, max, RUNS);
	__sysTestRand_printZeroVals((Tst_uint32*)&vals, so, min, max);

	min = 0;
	max = 1;
	__sysf_prf(" initR #5\n");
	st_sysInitRand(5);
	__sysTestRand_getRndVals((Tst_uint32*)&vals, so, min, max, RUNS);
	for (x = min; x <= max; x++)
		__sysf_prf("  __ %5u : %5u __\n", x, vals[x]);

	min = UINT_MAX / 2;
	max = UINT_MAX - 1;
	__sysf_prf(" initR #6\n");
	st_sysInitRand(6);
	__sysf_prf(" start %u - %u\n", min, max);
	for (x = 0; x < RUNS; x++) {
		val = st_sysGetRand(min, max);
		if ((min > 0 && val < min) || val > max)
			__sysf_prf("   --> val %u out of range\n", val);
	}

	__sysf_prf("Done.\n");
	exit(0);
}

void
sysTestRandDbl(void)
{
	const int    RUNS = 40;
	const double MAXV = 1e5;
	double min,
	       max;

	min = 0.0;
	max = MAXV - 1.0;
	__sysf_prf(" initR #0\n");
	st_sysInitRand(0);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	min = 1.0;
	max = MAXV - 1.0;
	__sysf_prf(" initR #1\n");
	st_sysInitRand(1);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	min = 2.0;
	max = MAXV - 1.0;
	__sysf_prf(" initR #2\n");
	st_sysInitRand(2);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	min = 2.0;
	max = (MAXV - 1.0) - 1.0;
	__sysf_prf(" initR #3\n");
	st_sysInitRand(3);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	min = 0.0;
	max = MAXV / 10.0;
	__sysf_prf(" initR #4\n");
	st_sysInitRand(4);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	min = 0.0;
	max = 1.0;
	__sysf_prf(" initR #4\n");
	st_sysInitRand(4);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	min = -1.0;
	max = 1.0;
	__sysf_prf(" initR #4\n");
	st_sysInitRand(4);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	min = -1000.0;
	max = -51.0;
	__sysf_prf(" initR #4\n");
	st_sysInitRand(4);
	__sysTestRand_getRndValsDbl(min, max, RUNS);

	__sysf_prf("Done.\n");
	exit(0);
}**/

/******************************************************************************/
