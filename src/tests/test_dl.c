/******************************************************************************/
/* test_dl.c                [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for Dynamic Lists                                            */
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
	#include <config.h>
#endif
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/dynlist.h"
#include "fncs_test_common.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), atoi() */
#include <string.h>       /* strcmp() */
#include <stdio.h>        /* sscanf() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define RUN_TEST_ANY  1
#if (RUN_TEST_ANY == 1)
	#define RUN_TEST_0   1
	#define RUN_TEST_1   1
	#define RUN_TEST_2   1
	#define RUN_TEST_3   1
	#define RUN_TEST_4   1
	#define RUN_TEST_5   1
	#define RUN_TEST_6   1
	#define RUN_TEST_7   1
	#define RUN_BENCH_1  1
#endif

typedef struct {
	Tst_bool   asc;    /* ascending or descending ? */
	Tst_uint32 comps;  /* comparisons count */
} TtestDl__sort;

typedef enum {
	TEST_DIR_ASC  = 0,  /* ascending */
	TEST_DIR_DSC  = 1,  /* descending */
	TEST_DIR_NONE = 2
} TtestDl__dir;

#if (RUN_TEST_0 == 1)
	Tst_bool TEST_DL__0(void);
#endif
#if (RUN_TEST_1 == 1)
	Tst_bool TEST_DL__1(void);
#endif
#if (RUN_TEST_2 == 1)
	Tst_bool TEST_DL__2(void);
#endif
#if (RUN_TEST_3 == 1)
	Tst_bool TEST_DL__3(void);
#endif
#if (RUN_TEST_4 == 1)
	Tst_bool TEST_DL__4(void);
#endif
#if (RUN_TEST_5 == 1)
	Tst_bool TEST_DL__5(void);
#endif
#if (RUN_TEST_6 == 1)
	Tst_bool TEST_DL__6(void);
#endif
#if (RUN_TEST_7 == 1)
	Tst_bool TEST_DL__7(void);
#endif

Tst_bool TEST_DL__createSortFile(const Tst_str *pPath, Tst_int32 tp, Tst_uint32 sz);
Tst_bool TEST_DL__printSortFile(const Tst_str *pFn, Tst_dynlist *pDL);
#if (RUN_BENCH_1 == 1)
	Tst_bool TEST_DL__bench1(void);
#endif
Tst_bool TEST_DL__bench2(const Tst_str *pFn, Tst_dynlist_sortAlgo algo, TtestDl__dir dir);

#define TEST__RSET_SORT(macS)  { (macS).asc = ST_B_TRUE; (macS).comps = 0; }

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
TEST_DL__prUsage(const char *pExe)
{
	Tst_str *pBN = NULL;

	st_sysFileBasename((const Tst_str*)pExe, &pBN);
	printf("Usage: %s -c PATH asc|dsc|rnd ELEMCNT\n"
			"\t--> create file for sorting and save it in PATH\n"
			"\t    either in ascending, descending or random order\n"
			"\t    and with ELEMCNT elements\n"
			"\t      (ELEMCNT can be written in decimal or hexadec.\n"
			"\t       format, e.g. 255 or hFF)\n\n", (char*)pBN);
	printf("       %s -p FILEN\n"
			"\t--> print elements from FILEN to stdout\n\n", (char*)pBN);
	printf("       %s -b FILEN [all|def|qs|ms|is [asc|dsc]]\n"
			"\t--> run benchmark using FILEN as input, the given algo\n"
			"\t    and the given sorting order\n\n", (char*)pBN);
	printf("       %s -s\n"
			"\t--> run standard tests\n\n", (char*)pBN);
	ST_DELPOINT(pBN)
}

int
main(const int argc, const char *argv[])
{
	// ReSharper disable once CppTooWideScope
	const Tst_uint32 cELEMMAX = 0xffffff;
	TtestDl__dir           sdir  = TEST_DIR_NONE;
	Tst_dynlist_sortAlgo salgo = ST_DYNLIST_SA_NONE;

	if (argc == 5 && strcmp(argv[1], "-c") == 0) {
		int          tp;
		unsigned int sz;

		tp = (strcmp(argv[3], "asc") == 0 ? 0 :
				strcmp(argv[3], "dsc") == 0 ? 1 : 2);
		if (argv[4][0] != 0 && argv[4][0] == 'h') {
			if (st_sysStrlen2(argv[4]) < 2) {
				printf("Min ELEMCNT is 1\n");
				return 1;
			}
			if (st_sysStrlen2(argv[4]) > 7) {
				printf("Max ELEMCNT is h%X (%u)\n", cELEMMAX, cELEMMAX);
				return 1;
			}
			sz = strtoul(&argv[4][1], NULL, 16);
		} else {
			if (st_sysStrlen2(argv[4]) > 8) {
				printf("Max ELEMCNT is h%X (%u)\n", cELEMMAX, cELEMMAX);
				return 1;
			}
			sz = strtoul(argv[4], NULL, 10);
		}
		if (sz < 1) {
			printf("Min ELEMCNT is 1\n");
			return 1;
		}
		if (sz > cELEMMAX) {
			printf("Max ELEMCNT is h%X (%u)\n", cELEMMAX, cELEMMAX);
			return 1;
		}
		if (! TEST_DL__createSortFile((Tst_str*)argv[2], tp, (Tst_uint32)sz)) {
			return 1;
		}
		printf("\n");
		return 0;
	}
	if (argc == 3 && strcmp(argv[1], "-p") == 0) {
		if (! TEST_DL__printSortFile((Tst_str*)argv[2], NULL)) {
			return 1;
		}
		printf("\n");
		return 0;
	}
	if (argc >= 3 && strcmp(argv[1], "-b") == 0) {
		if (argc >= 4) {
			if (strcmp(argv[3], "def") == 0) {
				salgo = ST_DYNLIST_SA_DEF;
			} else if (strcmp(argv[3], "qs") == 0) {
				salgo = ST_DYNLIST_SA_QUICKSORT;
			} else if (strcmp(argv[3], "ms") == 0) {
				salgo = ST_DYNLIST_SA_MERGESORT;
			} else if (strcmp(argv[3], "is") == 0) {
				salgo = ST_DYNLIST_SA_INSSORT;
			} else {
				salgo = ST_DYNLIST_SA_NONE;
			}
			if (argc == 5) {
				if (strcmp(argv[4], "asc") == 0) {
					sdir = TEST_DIR_ASC;
				} else {
					sdir = TEST_DIR_DSC;
				}
			}
		}
		if (! TEST_DL__bench2((Tst_str*)argv[2], salgo, sdir)) {
			return 1;
		}
		printf("\n");
		return 0;
	}
	if (argc == 2 && strcmp(argv[1], "-s") == 0) {
		/* run standard tests */
	} else if (argc == 2 &&
			(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
		TEST_DL__prUsage(argv[0]);
		return 0;
	} else {
		TEST_DL__prUsage(argv[0]);
		return 1;
	}

	#if (RUN_TEST_0 == 1)
		if (! TEST_DL__0()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_1 == 1)
		if (! TEST_DL__1()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_2 == 1)
		if (! TEST_DL__2()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_3 == 1)
		if (! TEST_DL__3()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_4 == 1)
		if (! TEST_DL__4()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_5 == 1)
		if (! TEST_DL__5()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_6 == 1)
		if (! TEST_DL__6()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_7 == 1)
		if (! TEST_DL__7()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_BENCH_1 == 1)
		if (! TEST_DL__bench1()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	printf("TEST_DL -- All tests passed :-)\n");
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
TEST_DL__cbRsetElemUI32(void *pData)
{
	*(Tst_uint32*)pData = 0;
}

void
TEST_DL__cbFreeElemUI32(void *pData)
{
	*(Tst_uint32*)pData = 0;
}

Tst_int32
TEST_DL__cbCmpElemsUI32(const void *pData1, const void *pData2,
		void *pCustomOptions)
{
	if (pCustomOptions == NULL) {
		return 0;
	}
	/* for huge amounts of data to be sorted
	 * this would produce an overflow...  */
	++((TtestDl__sort*)pCustomOptions)->comps;

	if (((TtestDl__sort*)pCustomOptions)->asc) {
		return (*(Tst_uint32*)pData1 >= *(Tst_uint32*)pData2 ?
				(*(Tst_uint32*)pData1 > *(Tst_uint32*)pData2 ? 1 : 0) : -1);
	}
	return (*(Tst_uint32*)pData1 <= *(Tst_uint32*)pData2 ?
			(*(Tst_uint32*)pData1 < *(Tst_uint32*)pData2 ? 1 : 0) : -1);
}

void
TEST_DL__cbSortStatus(const Tst_uint32 valCur, const Tst_uint32 valMax)
{
	fprintf(stderr, "\r  sort prog %.2f%%  (%u / %u)",
			valMax > 0 ? ((double)valCur / (double)valMax) * 100.0 : 100.0,
			valCur, valMax);
	if (valCur == valMax || valMax == 0) {
		fprintf(stderr, "\n");
	}
}

Tst_err
TEST_DL__cbCloneInitElem(void *pElem)
{
	*(Tst_uint32*)pElem = 0;
	return ST_ERR_SUCC;
}

Tst_err
TEST_DL__cbCloneCopyElem(const void *pElemSrc, void *pElemDst)
{
	*(Tst_uint32*)pElemDst = *(const Tst_uint32*)pElemSrc;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_bool
TEST_DL__getIxAndDataUI32(const char *pFnc,
		const Tst_dynlist *pDL, Tst_uint32 **ppUI32, Tst_uint32 *pEIx)
{
	*ppUI32 = (Tst_uint32*)st_dynlist_ite_getCurrent(pDL);
	if (*ppUI32 == NULL) {
		TEST_FCOM__prf(pFnc, "st_dynlist_ite_getCurrent() failed");
		return ST_B_FALSE;
	}
	*pEIx = st_dynlist_ite_getCurrentIndex(pDL);
	if (*pEIx == 0) {
		TEST_FCOM__prf(pFnc, "st_dynlist_ite_getCurrentIndex() failed");
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

#if (RUN_TEST_0 == 1 || RUN_TEST_1 == 1 || RUN_TEST_2 == 1 || \
		RUN_TEST_3 == 1 || RUN_TEST_4 == 1 || RUN_TEST_5 == 1 || \
		RUN_TEST_6 == 1 || RUN_TEST_7 == 1 || \
		RUN_BENCH_1 == 1)
Tst_bool
TEST_DL__addDataUI32(const char *pFnc,
		Tst_dynlist *pDL, const Tst_uint32 val, Tst_uint32 **ppUI32)
{
	Tst_err res;

	ST_CALLOC(*ppUI32, Tst_uint32*, 1, sizeof(Tst_uint32))
	if (*ppUI32 == NULL) {
		TEST_FCOM__prf(pFnc, "out of mem #1");
		return ST_B_FALSE;
	}
	**ppUI32 = val;
	/* */
	res = st_dynlist_addElement(pDL, *ppUI32);
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_OMEM) {
			TEST_FCOM__prf(pFnc, "out of mem #2");
		} else {
			TEST_FCOM__prf(pFnc, "st_dynlist_addElement() failed");
		}
		ST_DELPOINT(*ppUI32)
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}
#endif

/*----------------------------------------------------------------------------*/

#if (RUN_TEST_0 == 1 || RUN_TEST_1 == 1 || RUN_TEST_2 == 1 || \
		RUN_TEST_3 == 1 || RUN_TEST_4 == 1 || RUN_TEST_5 == 1 || \
		RUN_TEST_6 == 1 || RUN_BENCH_1 == 1)
ST_INLINE_S Tst_bool
TEST_DL__insDataUI32(const char *pFnc,
		Tst_dynlist *pDL, const Tst_uint32 pos, const Tst_uint32 val,
		Tst_uint32 **ppUI32)
{
	Tst_err res;
	char msg[512];

	ST_CALLOC(*ppUI32, Tst_uint32*, 1, sizeof(Tst_uint32))
	if (*ppUI32 == NULL) {
		TEST_FCOM__prf(pFnc, "out of mem #1");
		return ST_B_FALSE;
	}
	**ppUI32 = val;
	/* */
	res = st_dynlist_insElement(pDL, *ppUI32, pos);
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_OMEM) {
			TEST_FCOM__prf(pFnc, "out of mem #2");
		} else {
			sprintf(msg, "st_dynlist_insElement() failed, pars:pos=%u,val=%u",
					pos, val);
			TEST_FCOM__prf(pFnc, msg);
		}
		ST_DELPOINT(*ppUI32)
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}
#endif

/*----------------------------------------------------------------------------*/

#if (RUN_TEST_0 == 1 || RUN_TEST_1 == 1 || RUN_TEST_2 == 1 || \
		RUN_TEST_3 == 1 || RUN_TEST_4 == 1 || RUN_TEST_5 == 1 || \
		RUN_TEST_6 == 1 || RUN_TEST_7 == 1 || \
		RUN_BENCH_1 == 1)
Tst_bool
TEST_DL__checkIxDataUI32(const char *pFnc,
		const Tst_uint32 ixIs, const Tst_uint32 ixSh,
		const Tst_uint32 valIs, const Tst_uint32 valSh)
{
	char msg[512];

	if (ixIs != ixSh) {
		sprintf(msg, " elemIx mismatch: %u != %u\n", ixIs, ixSh);
		TEST_FCOM__prf(pFnc, msg);
		return ST_B_FALSE;
	}
	if (valIs != valSh) {
		sprintf(msg, " value mismatch: %u != %u\n", valIs, valSh);
		TEST_FCOM__prf(pFnc, msg);
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests st_dynlist_stc_initDL()
 *       st_dynlist_addElement()
 *       st_dynlist_getElementCount()
 *       st_dynlist_ite_gotoFirst()
 *       st_dynlist_ite_getCurrent()
 *       st_dynlist_ite_getCurrentIndex()
 *       st_dynlist_ite_gotoNext()
 *       st_dynlist_stc_freeDL()
 */
#if (RUN_TEST_0 == 1)
Tst_bool
TEST_DL__0(void)
{
	const char       *cFNC  = __func__;
	const Tst_uint32 cELEMS = 20;
	Tst_bool    resB   = ST_B_TRUE;
	Tst_err     res;
	Tst_uint32  x,
	            cnt    = 0,
	            *pUI32 = NULL,
	            eix    = 0;
	Tst_dynlist dl;

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	TEST_FCOM__prf(cFNC, "Adding elements...");
	for (x = 0; x < cELEMS; x++) {
		resB = TEST_DL__addDataUI32(cFNC, &dl, x + 1, &pUI32);
		if (! resB) {
			break;
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Getting elements...");
		cnt = st_dynlist_getElementCount(&dl);
		if (cnt != cELEMS) {
			TEST_FCOM__prf(cFNC, "amount of elems wrong");
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		st_dynlist_ite_gotoFirst(&dl);
		x = 1;
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, x, *pUI32, x);
			if (! resB) {
				break;
			}
			++x;
		} while (st_dynlist_ite_gotoNext(&dl));
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_TEST_0 */

/*----------------------------------------------------------------------------*/

/*
 * Tests st_dynlist_stc_initDL()
 *       st_dynlist_addElement()
 *       st_dynlist_getElementCount()
 *       st_dynlist_ite_gotoLast()
 *       st_dynlist_ite_getCurrent()
 *       st_dynlist_ite_getCurrentIndex()
 *       st_dynlist_ite_gotoPrev()
 *       st_dynlist_stc_freeDL()
 */
#if (RUN_TEST_1 == 1)
Tst_bool
TEST_DL__1(void)
{
	const char       *cFNC  = __func__;
	const Tst_uint32 cELEMS = 20;
	Tst_bool    resB   = ST_B_TRUE;
	Tst_err     res;
	Tst_uint32  x,
	            cnt    = 0,
	            *pUI32 = NULL,
	            eix    = 0;
	Tst_dynlist dl;

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	TEST_FCOM__prf(cFNC, "Adding elements...");
	for (x = 0; x < cELEMS; x++) {
		resB = TEST_DL__addDataUI32(cFNC, &dl, x + 1, &pUI32);
		if (! resB) {
			break;
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Getting elements in reverse order...");
		cnt = st_dynlist_getElementCount(&dl);
		if (cnt != cELEMS) {
			TEST_FCOM__prf(cFNC, "amount of elems wrong");
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		x = cELEMS;
		st_dynlist_ite_gotoLast(&dl);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, x, *pUI32, x);
			if (! resB) {
				break;
			}
			if (x > 0) {
				--x;
			}
		} while (st_dynlist_ite_gotoPrev(&dl));
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_TEST_1 */

/*----------------------------------------------------------------------------*/

/*
 * Tests st_dynlist_stc_initDL()
 *       st_dynlist_addElement()
 *       st_dynlist_remElement()
 *       st_dynlist_getElementCount()
 *       st_dynlist_ite_gotoFirst()
 *       st_dynlist_ite_gotoLast()
 *       st_dynlist_ite_getCurrent()
 *       st_dynlist_ite_getCurrentIndex()
 *       st_dynlist_ite_gotoNext()
 *       st_dynlist_ite_gotoPrev()
 *       st_dynlist_stc_freeDL()
 */
#if (RUN_TEST_2 == 1)
Tst_bool
TEST_DL__2(void)
{
	const char       *cFNC  = __func__;
	const Tst_uint32 cELEMS = 20;
	Tst_bool    resB   = ST_B_TRUE;
	Tst_err     res;
	Tst_uint32  x,
	            cnt    = 0,
	            *pUI32 = NULL,
	            expV,
	            eix    = 0;
	Tst_dynlist dl;

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	TEST_FCOM__prf(cFNC, "Adding elements...");
	for (x = 0; x < cELEMS; x++) {
		resB = TEST_DL__addDataUI32(cFNC, &dl, x + 1, &pUI32);
		if (! resB) {
			break;
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Removing elements...");
		/* */
		/** */
		eix = 5;
		/** */
		if (! st_dynlist_remElement(&dl, eix)) {
			TEST_FCOM__prf(cFNC, "st_dynlist_remElement() failed");
			resB = ST_B_FALSE;
		}
		/* */
		if (resB) {
			/** */
			eix = 10;
			/** */
			if (! st_dynlist_remElement(&dl, eix)) {
				TEST_FCOM__prf(cFNC, "st_dynlist_remElement() failed");
				resB = ST_B_FALSE;
			}
		}
		/* */
		if (resB) {
			/** */
			st_dynlist_ite_gotoFirst(&dl);
			/** */
			if (! st_dynlist_remElement(&dl,
					st_dynlist_ite_getCurrentIndex(&dl))) {
				TEST_FCOM__prf(cFNC, "st_dynlist_remElement() failed");
				resB = ST_B_FALSE;
			}
		}
		/* */
		if (resB) {
			/** */
			st_dynlist_ite_gotoLast(&dl);
			/** */
			if (! st_dynlist_remElement(&dl,
					st_dynlist_ite_getCurrentIndex(&dl))) {
				TEST_FCOM__prf(cFNC, "st_dynlist_remElement() failed");
				resB = ST_B_FALSE;
			}
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Getting elements in reverse order...");
		cnt = st_dynlist_getElementCount(&dl);
		if (cnt + 4 != cELEMS) {
			TEST_FCOM__prf(cFNC, "amount of elems wrong");
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		x    = cELEMS - 1;
		expV = cELEMS;
		st_dynlist_ite_gotoLast(&dl);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
			if (expV == 20 || expV == 11 || expV == 5) {
				--expV;
			}
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, x, *pUI32, expV);
			if (! resB) {
				break;
			}
			if (x > 0) {
				--x;
			}
			if (expV > 0) {
				--expV;
			}
		} while (st_dynlist_ite_gotoPrev(&dl));
	}
	if (resB) {
		TEST_FCOM__prf(cFNC, "Getting elements...");
		x    = 1;
		expV = 1;
		st_dynlist_ite_gotoFirst(&dl);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
			if (expV == 1 || expV == 5 || expV == 11) {
				++expV;
			}
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, x, *pUI32, expV);
			if (! resB) {
				break;
			}
			x++;
			expV++;
		} while (st_dynlist_ite_gotoNext(&dl));
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_TEST_2 */

/*----------------------------------------------------------------------------*/

/*
 * Tests st_dynlist_stc_initDL()
 *       st_dynlist_stc_rsetDL()
 *       st_dynlist_addElement()
 *       st_dynlist_getElementCount()
 *       st_dynlist_isEmpty()
 *       st_dynlist_ite_gotoFirst()
 *       st_dynlist_ite_getCurrent()
 *       st_dynlist_ite_getCurrentIndex()
 *       st_dynlist_ite_gotoNext()
 *       st_dynlist_stc_freeDL()
 */
#if (RUN_TEST_3 == 1)
Tst_bool
TEST_DL__3(void)
{
	const char       *cFNC  = __func__;
	const Tst_uint32 cELEMS = 3;
	Tst_bool    resB   = ST_B_TRUE;
	Tst_err     res;
	Tst_uint32  x,
	            cnt    = 0,
	            *pUI32 = NULL,
	            eix    = 0;
	Tst_dynlist dl;

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	TEST_FCOM__prf(cFNC, "Adding elements...");
	for (x = 0; x < cELEMS; x++) {
		resB = TEST_DL__addDataUI32(cFNC, &dl, x + 1, &pUI32);
		if (! resB) {
			break;
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Getting elements...");
		cnt = st_dynlist_getElementCount(&dl);
		if (cnt != cELEMS) {
			TEST_FCOM__prf(cFNC, "amount of elems wrong");
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		st_dynlist_ite_gotoFirst(&dl);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
		} while (st_dynlist_ite_gotoNext(&dl));
	}

	/* */
	if (resB) {
		st_dynlist_ite_gotoLast(&dl);
		TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
		printf("  last:  ix %010u data: %010u\n", eix, *pUI32);
		resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS, *pUI32, cELEMS);

		if (resB) {
			st_dynlist_remElement(&dl,
					st_dynlist_ite_getCurrentIndex(&dl));
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (resB) {
				printf("  last:  ix %010u data: %010u\n", eix, *pUI32);
				resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS - 1,
						*pUI32, cELEMS - 1);
			}
		}

		if (resB) {
			st_dynlist_remElement(&dl,
					st_dynlist_ite_getCurrentIndex(&dl));
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (resB) {
				printf("  last:  ix %010u data: %010u\n", eix, *pUI32);
				resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS - 2,
						*pUI32, cELEMS - 2);
			}
		}

		if (resB) {
			printf("  removing last remaining elem...\n");
			st_dynlist_remElement(&dl,
					st_dynlist_ite_getCurrentIndex(&dl));
			printf("  expecting error:\n");
			resB = st_dynlist_isEmpty(&dl) &&
					! TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (resB) {
				printf("  last:  empty now, that's fine...\n");
			}
		}

		if (resB) {
			printf("  trying to remove last remaining elem again...\n");
			resB = ! st_dynlist_remElement(&dl,
					st_dynlist_ite_getCurrentIndex(&dl));
			if (! resB) {
				TEST_FCOM__prf(cFNC, "st_dynlist_remElement() failed");
			}
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Adding elements...");
		for (x = 0; x < cELEMS * 4; x++) {
			resB = TEST_DL__addDataUI32(cFNC, &dl, cELEMS + x + 1, &pUI32);
			if (! resB) {
				break;
			}
		}
	}
	if (resB) {
		TEST_FCOM__prf(cFNC, "Accessing elements...");
		printf("  expecting error:\n");
		resB = ! TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
		if (resB) {
			printf("  no current element set, that's fine...\n");
		}
	}

	if (resB) {
		st_dynlist_ite_gotoLast(&dl);
		resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
		if (resB) {
			printf("  last :  ix %010u data: %010u\n", eix, *pUI32);
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS * 4,
					*pUI32, cELEMS + cELEMS * 4);
		}
	}

	if (resB) {
		st_dynlist_ite_gotoFirst(&dl);
		resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
		if (resB) {
			printf("  first:  ix %010u data: %010u\n", eix, *pUI32);
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, 1,
					*pUI32, cELEMS + 1);
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Resetting st_dynlist...");
		st_dynlist_stc_rsetDL(&dl);

		TEST_FCOM__prf(cFNC, "Adding elements...");
		for (x = 0; x < cELEMS * 4; x++) {
			resB = TEST_DL__addDataUI32(cFNC, &dl, x + 1, &pUI32);
			if (! resB) {
				break;
			}
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Getting elements...");
		cnt = st_dynlist_getElementCount(&dl);
		if (cnt != cELEMS * 4) {
			TEST_FCOM__prf(cFNC, "amount of elems wrong");
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		x = 1;
		st_dynlist_ite_gotoFirst(&dl);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, x,
					*pUI32, x);
			if (! resB) {
				break;
			}
			++x;
		} while (st_dynlist_ite_gotoNext(&dl));
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_TEST_3 */

/*----------------------------------------------------------------------------*/

/*
 * Tests st_dynlist_stc_initDL()
 *       st_dynlist_addElement()
 *       st_dynlist_getElementCount()
 *       st_dynlist_isEmpty()
 *       st_dynlist_ite_gotoFirst()
 *       st_dynlist_ite_getCurrent()
 *       st_dynlist_ite_getCurrentIndex()
 *       st_dynlist_ite_gotoNext()
 *       st_dynlist_ite_goto()
 *       st_dynlist_stc_freeDL()
 */
#if (RUN_TEST_4 == 1)
Tst_bool
TEST_DL__4(void)
{
	const char       *cFNC  = __func__;
	const Tst_uint32 cELEMS = 0x3fffff;
	Tst_bool    resB   = ST_B_TRUE;
	Tst_err     res;
	Tst_uint32  x,
	            cnt    = 0,
	            *pUI32 = NULL,
	            eix    = 0;
	char        msg[512];
	double      timeB,
	            timeE;
	Tst_dynlist dl;

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	timeB = st_sysGetTime();
	sprintf(msg, "Adding %u elements (ca. %.2f MB RAM)...",
			cELEMS,
			(double)((cELEMS + 1024) *
					(sizeof(Tst_uint32) + (3 * sizeof(void*)))) /
				(1024.0 * 1024.0));
	TEST_FCOM__prf(cFNC, msg);
	for (x = 0; x < cELEMS; x++) {
		resB = TEST_DL__addDataUI32(cFNC, &dl, x + 1, &pUI32);
		if (! resB) {
			break;
		}
	}
	if (resB) {
		timeE = st_sysGetTime();
		printf("\t\t\ttime --> %.2fs\n\n", timeE - timeB);
	}

	/* */
	if (resB) {
		timeB = st_sysGetTime();
		TEST_FCOM__prf(cFNC, "Getting elements...");
		cnt = st_dynlist_getElementCount(&dl);
		if (cnt != cELEMS) {
			TEST_FCOM__prf(cFNC, "amount of elems wrong");
			resB = ST_B_FALSE;
		} else if (st_dynlist_isEmpty(&dl)) {
			TEST_FCOM__prf(cFNC, "st_dynlist_isEmpty() failed");
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		st_dynlist_ite_gotoFirst(&dl);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
		} while (st_dynlist_ite_gotoNext(&dl));
		if (resB) {
			timeE = st_sysGetTime();
			printf("\t\t\ttime --> %.2fs\n\n", timeE - timeB);
		}
	}
	/* */
	if (resB) {
		timeB = st_sysGetTime();
		TEST_FCOM__prf(cFNC, "Randomly accessing elements #1...");
		st_dynlist_ite_gotoFirst(&dl);

		eix = cELEMS / 2;
		if (! st_dynlist_ite_goto(&dl, eix)) {
			TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
			resB = ST_B_FALSE;
		} else {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (resB) {
				resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS / 2,
						*pUI32, cELEMS / 2);
			}
		}

		if (resB) {
			/* use cached element */
			eix += 11;
			if (! st_dynlist_ite_goto(&dl, eix)) {
				TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
				resB = ST_B_FALSE;
			} else {
				resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
				if (resB) {
					resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS / 2 + 11,
							*pUI32, cELEMS / 2 + 11);
				}
			}
		}

		if (resB) {
			/* use cached element */
			eix -= 12;
			if (! st_dynlist_ite_goto(&dl, eix)) {
				TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
				resB = ST_B_FALSE;
			} else {
				resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
				if (resB) {
					resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS / 2 - 1,
							*pUI32, cELEMS / 2 - 1);
				}
			}
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Randomly accessing elements #2...");
		st_dynlist_ite_gotoFirst(&dl);

		/* use last added element */
		eix = cELEMS - 20;
		if (! st_dynlist_ite_goto(&dl, eix)) {
			TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
			resB = ST_B_FALSE;
		} else {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (resB) {
				resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS - 20,
						*pUI32, cELEMS - 20);
			}
		}

		if (resB) {
			/* use last added element */
			eix += 11;
			if (! st_dynlist_ite_goto(&dl, eix)) {
				TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
				resB = ST_B_FALSE;
			} else {
				resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
				if (resB) {
					resB = TEST_DL__checkIxDataUI32(cFNC, eix, cELEMS - 20 + 11,
							*pUI32, cELEMS - 20 + 11);
				}
			}
		}
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Randomly accessing elements #3...");

		/* use first element */
		eix = 20;
		if (! st_dynlist_ite_goto(&dl, eix)) {
			TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
			resB = ST_B_FALSE;
		} else {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (resB) {
				resB = TEST_DL__checkIxDataUI32(cFNC, eix, 20,
						*pUI32, 20);
			}
		}

		if (resB) {
			/* use cached element */
			eix += 12;
			if (! st_dynlist_ite_goto(&dl, eix)) {
				TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
				resB = ST_B_FALSE;
			} else {
				resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
				if (resB) {
					resB = TEST_DL__checkIxDataUI32(cFNC, eix, 20 + 12,
							*pUI32, 20 + 12);
				}
			}
		}

		if (resB) {
			/* use cached element */
			eix -= 15;
			if (! st_dynlist_ite_goto(&dl, eix)) {
				TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
				resB = ST_B_FALSE;
			} else {
				resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
				if (resB) {
					resB = TEST_DL__checkIxDataUI32(cFNC, eix, 20 + 12 - 15,
							*pUI32, 20 + 12 - 15);
				}
			}
		}

		if (resB) {
			/* use first element */
			eix -= 11;
			if (! st_dynlist_ite_goto(&dl, eix)) {
				TEST_FCOM__prf(cFNC, "st_dynlist_ite_goto() failed");
				resB = ST_B_FALSE;
			} else {
				resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
				if (resB) {
					resB = TEST_DL__checkIxDataUI32(cFNC, eix, 20 + 12 - 15 - 11,
							*pUI32, 20 + 12 - 15 - 11);
				}
			}
		}

		if (resB) {
			timeE = st_sysGetTime();
			printf("\t\t\ttime --> %.2fs\n\n", timeE - timeB);
		}
	}
	/* */
	if (resB) {
		timeB = st_sysGetTime();
	}
	TEST_FCOM__prf(cFNC, "Freeing elements...");
	st_dynlist_stc_freeDL(&dl);
	if (resB) {
		timeE = st_sysGetTime();
		printf("\t\t\ttime --> %.2fs\n\n", timeE - timeB);

		TEST_FCOM__prf(cFNC, "OK");
	}
	return resB;
}
#endif  /* RUN_TEST_4 */

/*----------------------------------------------------------------------------*/

/*
 * Tests st_dynlist_stc_initDL()
 *       st_dynlist_insElement()
 *       st_dynlist_getElementCount()
 *       st_dynlist_ite_gotoFirst()
 *       st_dynlist_ite_getCurrent()
 *       st_dynlist_ite_getCurrentIndex()
 *       st_dynlist_ite_gotoNext()
 *       st_dynlist_stc_freeDL()
 */
#if (RUN_TEST_5 == 1)
Tst_bool
TEST_DL__5(void)
{
	const char       *cFNC  = __func__;
	const Tst_uint32 cELEMS = 6;
	Tst_bool    resB   = ST_B_TRUE;
	Tst_err     res;
	Tst_uint32  x,
	            cnt    = 0,
	            *pUI32 = NULL,
	            eix    = 0;
	Tst_dynlist dl;
	char        msg[512];

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	TEST_FCOM__prf(cFNC, "Inserting elements...");
	resB = TEST_DL__insDataUI32(cFNC, &dl, 33, 5, &pUI32);
	if (resB) {
		resB = TEST_DL__insDataUI32(cFNC, &dl, 1, 3, &pUI32);
	}
	if (resB) {
		resB = TEST_DL__insDataUI32(cFNC, &dl, 1, 2, &pUI32);
	}
	if (resB) {
		resB = TEST_DL__insDataUI32(cFNC, &dl, 3, 4, &pUI32);
	}
	if (resB) {
		resB = TEST_DL__insDataUI32(cFNC, &dl, 5, 6, &pUI32);
	}
	if (resB) {
		resB = TEST_DL__insDataUI32(cFNC, &dl, 1, 1, &pUI32);
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Getting elements...");
		cnt = st_dynlist_getElementCount(&dl);
		if (cnt != cELEMS) {
			sprintf(msg, "amount of elems wrong: %u != %u", cnt, cELEMS);
			TEST_FCOM__prf(cFNC, msg);
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		st_dynlist_ite_gotoFirst(&dl);
		x = 1;
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, &dl, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, x, *pUI32, x);
			if (! resB) {
				break;
			}
			++x;
		} while (st_dynlist_ite_gotoNext(&dl));
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_TEST_5 */

/*----------------------------------------------------------------------------*/

/*
 * Tests st_dynlist_stc_initDL()
 *       st_dynlist_stc_rsetDL()
 *       st_dynlist_addElement()
 *       st_dynlist_getElementCount()
 *       st_dynlist_clone()
 *       st_dynlist_sortElements()
 *       st_dynlist_ite_gotoFirst()
 *       st_dynlist_ite_getCurrent()
 *       st_dynlist_ite_getCurrentIndex()
 *       st_dynlist_ite_gotoNext()
 *       st_dynlist_stc_freeDL()
 */
#if (RUN_TEST_6 == 1)
Tst_bool
TEST_DL__6_sub2(Tst_dynlist *pDL, const Tst_uint32 elemCnt,
		const Tst_dynlist_sortAlgo sortAlgo, const Tst_bool ascOrDesc,
		const Tst_bool alwDup, const Tst_uint32 dup, const Tst_bool showUS)
{
	const char *cFNC = __func__;
	Tst_bool    resB   = ST_B_TRUE,
	            dupF   = ST_B_FALSE;
	Tst_uint32  x,
	            cnt    = 0,
	            *pUI32 = NULL,
	            eix    = 0;
	char        msg[512];
	TtestDl__sort custSO;

	printf("\n");
	/* */
	if (showUS) {
		TEST_FCOM__prf(cFNC, "Getting elements...");
		cnt = st_dynlist_getElementCount(pDL);
		if (cnt != elemCnt) {
			sprintf(msg, "amount of elems wrong: %u != %u", cnt, elemCnt);
			TEST_FCOM__prf(cFNC, msg);
			resB = ST_B_FALSE;
		}
		if (resB) {
			st_dynlist_ite_gotoFirst(pDL);
			do {
				resB = TEST_DL__getIxAndDataUI32(cFNC, pDL, &pUI32, &eix);
				if (! resB) {
					break;
				}
				printf("  ix %010u data: %010u\n", eix, *pUI32);
			} while (st_dynlist_ite_gotoNext(pDL));
		}
	}

	/* */
	if (resB) {
		TEST__RSET_SORT(custSO)
		custSO.asc = ascOrDesc;
		switch (sortAlgo) {
			case ST_DYNLIST_SA_DEF:
				TEST_FCOM__prf(cFNC, "sortAlgo: Default");
				break;
			case ST_DYNLIST_SA_INSSORT:
				TEST_FCOM__prf(cFNC, "sortAlgo: Insertionsort");
				break;
			case ST_DYNLIST_SA_QUICKSORT:
				TEST_FCOM__prf(cFNC, "sortAlgo: Quicksort");
				break;
			case ST_DYNLIST_SA_MERGESORT:
				TEST_FCOM__prf(cFNC, "sortAlgo: Mergesort");
				break;
			default:
				TEST_FCOM__prf(cFNC, "sortAlgo: unknown");
				break;
		}
		sprintf(msg, "order   : %s", ascOrDesc ? "asc" : "desc");
		TEST_FCOM__prf(cFNC, msg);
		TEST_FCOM__prf(cFNC, "Sorting elements...");
		resB = st_dynlist_sortElements(pDL, sortAlgo,
				TEST_DL__cbCmpElemsUI32, NULL, &custSO);
		if (! resB) {
			TEST_FCOM__prf(cFNC, "st_dynlist_sortElements() failed");
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Checking sorted elements...");
		cnt = st_dynlist_getElementCount(pDL);
		if (cnt != elemCnt) {
			sprintf(msg, "amount of elems wrong: %u != %u", cnt, elemCnt);
			TEST_FCOM__prf(cFNC, msg);
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		if (ascOrDesc) {
			x = 1;
		} else {
			x = elemCnt;
		}
		st_dynlist_ite_gotoFirst(pDL);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, pDL, &pUI32, &eix);
			if (! resB) {
				break;
			}
			printf("  ix %010u data: %010u\n", eix, *pUI32);
			if (alwDup && dup == *pUI32 && ! dupF) {
				dupF = ST_B_TRUE;
				continue;
			}
			resB = TEST_DL__checkIxDataUI32(cFNC, eix, eix, *pUI32, x);
			if (! resB) {
				break;
			}
			if (ascOrDesc) {
				++x;
			} else if (x > 0) {
				--x;
			}
		} while (st_dynlist_ite_gotoNext(pDL));
	}

	return resB;
}

Tst_bool
TEST_DL__6_sub(Tst_dynlist *pDL, const Tst_uint32 elemCnt,
		const Tst_bool ascOrDesc, const Tst_bool alwDup, const Tst_uint32 dup)
{
	const char *cFNC = __func__;
	Tst_bool    resB = ST_B_TRUE;
	Tst_err     res;
	int         sa;
	Tst_dynlist dupLst;

	res = st_dynlist_stc_initDL(&dupLst, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_OMEM) {
			TEST_FCOM__prf(cFNC, "out of mem");
		} else {
			TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		}
		return ST_B_FALSE;
	}
	for (sa = 0; sa < (int)ST_DYNLIST_SA_NONE; sa++) {
		TEST_FCOM__prf(cFNC, "cloning list from org to dup...");
		res = st_dynlist_clone(pDL, &dupLst,
				TEST_DL__cbCloneInitElem, TEST_DL__cbCloneCopyElem);
		if (res != ST_ERR_SUCC) {
			TEST_FCOM__prf(cFNC, "st_dynlist_clone() failed");
			resB = ST_B_FALSE;
			break;
		}
		if (! TEST_DL__6_sub2(&dupLst, elemCnt,
				(Tst_dynlist_sortAlgo)sa, ascOrDesc, alwDup, dup, sa == 0)) {
			resB = ST_B_FALSE;
			break;
		}
	}
	st_dynlist_stc_freeDL(&dupLst);
	return resB;
}

Tst_bool
TEST_DL__6(void)
{
	const char *cFNC = __func__;
	Tst_bool    resB;
	Tst_err     res;
	Tst_uint32  *pUI32 = NULL;
	Tst_dynlist dl;

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	TEST_FCOM__prf(cFNC, "#1 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
		resB = TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
	}
	/* */
	if (resB) {
		resB = TEST_DL__6_sub(&dl, 6, ST_B_TRUE, ST_B_FALSE, 0);
	}
	if (resB) {
		st_dynlist_stc_rsetDL(&dl);
		printf("\n");
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "#2 Adding elements...");
		resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
		if (resB) {
			TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
			resB = TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
		}
		/* */
		if (resB) {
			resB = TEST_DL__6_sub(&dl, 6, ST_B_FALSE, ST_B_FALSE, 0);
		}
		if (resB) {
			st_dynlist_stc_rsetDL(&dl);
			printf("\n");
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "#3 Adding elements...");
		resB = TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
		if (resB) {
			TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
			resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
		}
		/* */
		if (resB) {
			resB = TEST_DL__6_sub(&dl, 6, ST_B_TRUE, ST_B_FALSE, 0);
		}
		if (resB) {
			st_dynlist_stc_rsetDL(&dl);
			printf("\n");
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "#4 Adding elements...");
		resB = TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
		if (resB) {
			TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
			resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
		}
		/* */
		if (resB) {
			resB = TEST_DL__6_sub(&dl, 6, ST_B_TRUE, ST_B_FALSE, 0);
		}
		if (resB) {
			st_dynlist_stc_rsetDL(&dl);
			printf("\n");
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "#5 Adding elements...");
		resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
		/* */
		if (resB) {
			resB = TEST_DL__6_sub(&dl, 1, ST_B_TRUE, ST_B_FALSE, 0);
		}
		if (resB) {
			st_dynlist_stc_rsetDL(&dl);
			printf("\n");
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "#6 Adding elements...");
		resB = TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
		if (resB) {
			TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
			TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
			resB = TEST_DL__addDataUI32(cFNC, &dl, 7, &pUI32);
		}
		/* */
		if (resB) {
			resB = TEST_DL__6_sub(&dl, 8, ST_B_TRUE, ST_B_TRUE, 4);
		}
		if (resB) {
			st_dynlist_stc_rsetDL(&dl);
			printf("\n");
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_TEST_6 */

/*----------------------------------------------------------------------------*/

#if (RUN_TEST_7 == 1)
/*
 * Tests st_dynlist_moveElement()
 */
Tst_bool
TEST_DL__7_sub(Tst_dynlist *pDL, const Tst_uint32 elemCnt,
		const Tst_uint32 data[])
{
	const char *cFNC = __func__;
	Tst_bool   resB;
	Tst_uint32 *pUI32 = NULL,
	           eix    = 0,
	           x;

	resB = st_dynlist_ite_gotoFirst(pDL);
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_ite_gotoFirst() failed");
		return resB;
	}
	x = 1;
	do {
		resB = TEST_DL__getIxAndDataUI32(cFNC, pDL, &pUI32, &eix);
		if (! resB) {
			break;
		}
		printf("  ix %010u data: %010u\n", eix, *pUI32);
		resB = TEST_DL__checkIxDataUI32(cFNC, eix, x, *pUI32, data[x - 1]);
		if (! resB) {
			break;
		}
		++x;
	} while (st_dynlist_ite_gotoNext(pDL));
	if (resB && x != elemCnt + 1) {
		TEST_FCOM__prf(cFNC, "Element count doesn't match");
		resB = ST_B_FALSE;
	}

	if (resB) {
		TEST_FCOM__prf(cFNC, "Checking integrity...");
		resB = st_dynlist_check(pDL);
		if (! resB) {
			TEST_FCOM__prf(cFNC, "st_dynlist_check() failed");
		}
	}

	return resB;
}

Tst_bool
TEST_DL__7(void)
{
	const char *cFNC = __func__;
	Tst_bool    resB;
	Tst_err     res;
	Tst_uint32  *pUI32 = NULL,
	            data[6];
	Tst_dynlist dl;

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* */
	TEST_FCOM__prf(cFNC, "#1 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#1 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 1, 3);  /* 2 3 1 */
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 2;
		data[1] = 3;
		data[2] = 1;
		data[3] = 0;
		data[4] = 0;
		data[5] = 0;
		/* */
		resB = TEST_DL__7_sub(&dl, 3, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#2 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
		resB = TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}
	/* */
	TEST_FCOM__prf(cFNC, "#2 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 1, 2);  /* 2 1 3 4 5 6 */
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 3, 4);  /* 2 1 4 3 5 6 */
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 5, 6);  /* 2 1 4 3 6 5 */
	}
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 2;
		data[1] = 1;
		data[2] = 4;
		data[3] = 3;
		data[4] = 6;
		data[5] = 5;
		/* */
		resB = TEST_DL__7_sub(&dl, 6, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#3 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#3 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 1, 7);  /* 2 3 4 5 6 1 */
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 1, 7);  /* 3 4 5 6 1 2 */
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 2, 5);  /* 3 5 6 1 4 2 */
	}
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 3;
		data[1] = 5;
		data[2] = 6;
		data[3] = 1;
		data[4] = 4;
		data[5] = 2;
		/* */
		resB = TEST_DL__7_sub(&dl, 6, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#4 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#4 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 6, 1);  /* 6 1 2 3 4 5 */
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 3, 5);  /* 6 1 3 4 2 5 */
	}
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 6;
		data[1] = 1;
		data[2] = 3;
		data[3] = 4;
		data[4] = 2;
		data[5] = 5;
		/* */
		resB = TEST_DL__7_sub(&dl, 6, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#5 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#5 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 2, 7);  /* 1 3 4 5 6 2 */
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 6, 1);  /* 2 1 3 4 5 6 */
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 3, 100);  /* 2 1 4 5 6 3 */
	}
	if (resB) {
		resB = ! st_dynlist_moveElement(&dl, 7, 3);  /* 2 1 4 5 6 3 */
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 4, 5);  /* 2 1 4 6 5 3 */
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 5, 6);  /* 2 1 4 6 3 5 */
	}
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 2;
		data[1] = 1;
		data[2] = 4;
		data[3] = 6;
		data[4] = 3;
		data[5] = 5;
		/* */
		resB = TEST_DL__7_sub(&dl, 6, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#6 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#6 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 1, 1);  /* 1 2 */
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 2, 2);  /* 1 2 */
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 1, 2);  /* 2 1 */
	}
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 2;
		data[1] = 1;
		data[2] = 0;
		data[3] = 0;
		data[4] = 0;
		data[5] = 0;
		/* */
		resB = TEST_DL__7_sub(&dl, 2, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#7 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#7 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 3, 1);  /* 3 1 2 */
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 3;
		data[1] = 1;
		data[2] = 2;
		data[3] = 0;
		data[4] = 0;
		data[5] = 0;
		/* */
		resB = TEST_DL__7_sub(&dl, 3, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#8 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 5, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 6, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#8 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 2, 4);  /* 1 3 4 2 5 6 */
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 5, 2);  /* 1 5 3 4 2 6 */
	}
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 1;
		data[1] = 5;
		data[2] = 3;
		data[3] = 4;
		data[4] = 2;
		data[5] = 6;
		/* */
		resB = TEST_DL__7_sub(&dl, 6, data);
	}
	if (! resB) {
		st_dynlist_stc_freeDL(&dl);
		return resB;
	}

	/* */
	st_dynlist_stc_rsetDL(&dl);
	TEST_FCOM__prf(cFNC, "#9 Adding elements...");
	resB = TEST_DL__addDataUI32(cFNC, &dl, 1, &pUI32);
	if (resB) {
		TEST_DL__addDataUI32(cFNC, &dl, 2, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 3, &pUI32);
		TEST_DL__addDataUI32(cFNC, &dl, 4, &pUI32);
	}
	/* */
	TEST_FCOM__prf(cFNC, "#9 Moving elements...");
	resB = st_dynlist_moveElement(&dl, 3, 1);  /* 3 1 2 4 */
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 4, 1);  /* 4 3 1 2 */
	}
	if (! resB) {
		TEST_FCOM__prf(cFNC, "st_dynlist_moveElement() failed");
	}
	if (resB) {
		data[0] = 4;
		data[1] = 3;
		data[2] = 1;
		data[3] = 2;
		data[4] = 0;
		data[5] = 0;
		/* */
		resB = TEST_DL__7_sub(&dl, 4, data);
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 1, 3);  /* 3 1 4 2 */
	}
	if (resB) {
		resB = st_dynlist_moveElement(&dl, 1, 4);  /* 1 4 2 3 */
	}
	if (resB) {
		data[0] = 1;
		data[1] = 4;
		data[2] = 2;
		data[3] = 3;
		data[4] = 0;
		data[5] = 0;
		/* */
		resB = TEST_DL__7_sub(&dl, 4, data);
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_TEST_7 */

/*----------------------------------------------------------------------------*/

/*
 * Create file to sort
 */
Tst_bool
TEST_DL__createSortFile(const Tst_str *pPath,
		const Tst_int32 tp, const Tst_uint32 sz)
{
	Tst_bool     resB = ST_B_TRUE;
	Tst_sys_fstc fstc;
	Tst_str      fn[1024];
	Tst_uint32   x,
	             div,
	             *pUI32 = NULL;
	Tst_dynlist  dl;
	TtestDl__sort  custSO;

	st_sys_stc_initFStc(&fstc);
	st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);

	snprintf((char*)fn, sizeof(fn), "./%s/sort-%s-%08x.bin",
			(char*)pPath,
			(tp == 0 ? "asc" : (tp == 1 ? "dsc" : "rnd")), sz);

	printf("Creating sort file (tp=%s, sz=%u)  '%s'...\n",
			(tp == 0 ? "asc" : (tp == 1 ? "dsc" : "rnd")), sz, (char*)fn);

	if (st_sysFStc_setFilen(&fstc, fn) != ST_ERR_SUCC) {
		printf(" st_sysFStc_setFilen() failed\n");
		resB = ST_B_FALSE;
	} else if (st_sysFStc_openNew(&fstc) != ST_ERR_SUCC) {
		printf(" st_sysFStc_openNew() failed  "
				"(maybe file already exists ?)\n");
		resB = ST_B_FALSE;
	}
	if (resB) {
		printf("Generating random values...\n");
		for (x = 0; x < sz; x++) {
			if (x % 10000 == 0 || x + 1 == sz) {
				fprintf(stderr, "\r  gen prog %.2f%%",
						((double)x / (double)sz) * 100);
				if (x + 1 == sz) {
					fprintf(stderr, "\n");
				}
			}
			if (st_sysGetRand(1, 1000) % 2 == 0) {
				div = st_sysGetRand(1, st_sysGetRand(1, 100000));
			} else {
				div = 1;
			}
			/* */
			ST_CALLOC(pUI32, Tst_uint32*, 1, sizeof(Tst_uint32))
			if (pUI32 == NULL) {
				printf(" out of mem !\n");
				resB = ST_B_FALSE;
				break;
			}
			*pUI32 = st_sysGetRand(1, 0xffffffff / div);
			if (st_dynlist_addElement(&dl, pUI32) != ST_ERR_SUCC) {
				printf(" st_dynlist_addElement() failed\n");
				resB = ST_B_FALSE;
				ST_DELPOINT(pUI32)
				break;
			}
		}
	}
	if (resB && tp != 2) {
		printf("Sorting elements...\n");
		TEST__RSET_SORT(custSO)
		custSO.asc = (tp == 0);
		resB = st_dynlist_sortElements(&dl, ST_DYNLIST_SA_QUICKSORT,
				TEST_DL__cbCmpElemsUI32, TEST_DL__cbSortStatus, &custSO);
		if (! resB) {
			printf(" st_dynlist_sortElements() failed\n");
		}
	}
	if (resB) {
		printf("Writing elements to file...\n");
		st_dynlist_ite_gotoFirst(&dl);
		x = 0;
		do {
			if (x % 10000 == 0 || x + 1 == sz) {
				fprintf(stderr, "\r  wrt prog %.2f%%",
						((double)x / (double)sz) * 100);
				if (x + 1 == sz) {
					fprintf(stderr, "\n");
				}
			}
			pUI32 = (Tst_uint32*)st_dynlist_ite_getCurrent(&dl);
			if (pUI32 == NULL) {
				printf(" st_dynlist_ite_getCurrent() failed\n");
				resB = ST_B_FALSE;
				break;
			}
			if (st_sysFStc_writeBuf(&fstc,
					pUI32, sizeof(Tst_uint32)) != ST_ERR_SUCC) {
				printf(" st_sysFStc_writeBuf() failed\n");
				resB = ST_B_FALSE;
				break;
			}
			++x;
		} while (st_dynlist_ite_gotoNext(&dl));
	}
	st_sysFStc_close(&fstc);
	st_sys_stc_freeFStc(&fstc);
	printf("Freeing elements...\n");
	st_dynlist_stc_freeDL(&dl);
	if (resB) {
		printf("Done.\n");
	}
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Print file to sort
 */
Tst_bool
TEST_DL__printSortFile(const Tst_str *pFn, Tst_dynlist *pDL)
{
	Tst_bool     resB = ST_B_TRUE;
	Tst_sys_fstc fstc;
	Tst_uint32   x      = 1,
	             *pUI32 = NULL,
	             cpd    = 0;

	st_sys_stc_initFStc(&fstc);

	if (pDL == NULL) {
		printf("Printing sort file '%s'...\n", (char*)pFn);
	} else {
		printf("Reading sort file '%s'...\n", (char*)pFn);
	}

	if (st_sysFStc_setFilen(&fstc, pFn) != ST_ERR_SUCC) {
		printf(" st_sysFStc_setFilen() failed\n");
		resB = ST_B_FALSE;
	} else if (st_sysFStc_openExisting(&fstc, ST_B_TRUE, ST_B_FALSE) != ST_ERR_SUCC) {
		printf(" st_sysFStc_openExisting() failed  "
				"(maybe file doesn't exist ?)\n");
		resB = ST_B_FALSE;
	}
	if (resB) {
		if (pDL == NULL) {
			ST_CALLOC(pUI32, Tst_uint32*, 1, sizeof(Tst_uint32))
			*pUI32 = 0;
		}
		while (! st_sysFStc_isEOF(&fstc)) {
			if (pDL != NULL) {
				ST_CALLOC(pUI32, Tst_uint32*, 1, sizeof(Tst_uint32))
				if (pUI32 == NULL) {
					printf(" out of mem !\n");
					resB = ST_B_FALSE;
					break;
				}
				*pUI32 = 0;
			}
			if (st_sysFStc_readBuf(&fstc, sizeof(Tst_uint32),
					(Tst_buf*)pUI32, &cpd) != ST_ERR_SUCC) {
				printf(" st_sysFStc_readBuf() failed\n");
				resB = ST_B_FALSE;
				ST_DELPOINT(pUI32)
				break;
			}
			if (cpd != sizeof(Tst_uint32)) {
				if (cpd > 0)
					printf(" can't read further elements... (cpd=%u)\n", cpd);
				ST_DELPOINT(pUI32)
				break;
			}
			if (pDL == NULL) {
				printf("ix %010u: data %010u\n", x++, *pUI32);
			} else if (st_dynlist_addElement(pDL, pUI32) != ST_ERR_SUCC) {
				printf(" st_dynlist_addElement() failed\n");
				resB = ST_B_FALSE;
				ST_DELPOINT(pUI32)
				break;
			}
		}
		if (pDL == NULL) {
			ST_DELPOINT(pUI32)
		}
	}
	st_sysFStc_close(&fstc);
	st_sys_stc_freeFStc(&fstc);
	if (resB) {
		if (pDL == NULL) {
			printf("Done.\n");
		} else {
			printf("Done.  (%u elems read)\n", st_dynlist_getElementCount(pDL));
		}
	}
	return resB;
}

/*----------------------------------------------------------------------------*/

Tst_bool
TEST_DL__bench_sub2(Tst_dynlist *pDL, const Tst_uint32 elemCnt,
		const Tst_dynlist_sortAlgo sortAlgo, const Tst_bool ascOrDesc,
		const Tst_bool showUS, const Tst_bool showSO,
		const Tst_uint32 runC, const Tst_uint32 runT)
{
	const char *cFNC = __func__;
	Tst_bool    resB;
	Tst_uint32  cnt    = 0,
	            *pUI32 = NULL,
	            eix    = 0;
	char        msg[512];
	double      timeB,
	            timeE;
	TtestDl__sort custSO;

	/* */
	printf("\n");
	sprintf(msg, "run     : %u/%u", runC, runT);
	TEST_FCOM__prf(cFNC, msg);
	switch (sortAlgo) {
		case ST_DYNLIST_SA_DEF:
			TEST_FCOM__prf(cFNC, "sortAlgo: Default");
			break;
		case ST_DYNLIST_SA_INSSORT:
			TEST_FCOM__prf(cFNC, "sortAlgo: Insertionsort");
			break;
		case ST_DYNLIST_SA_QUICKSORT:
			TEST_FCOM__prf(cFNC, "sortAlgo: Quicksort");
			break;
		case ST_DYNLIST_SA_MERGESORT:
			TEST_FCOM__prf(cFNC, "sortAlgo: Mergesort");
			break;
		default:
			TEST_FCOM__prf(cFNC, "sortAlgo: unknown");
			break;
	}
	sprintf(msg, "order   : %s", ascOrDesc ? "asc" : "desc");
	TEST_FCOM__prf(cFNC, msg);
	/* */
	TEST_FCOM__prf(cFNC, "Getting unsorted elements...");
	cnt = st_dynlist_getElementCount(pDL);
	if (cnt != elemCnt) {
		sprintf(msg, "amount of elems wrong: %u != %u", cnt, elemCnt);
		TEST_FCOM__prf(cFNC, msg);
		resB = ST_B_FALSE;
	} else {
		st_dynlist_ite_gotoFirst(pDL);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, pDL, &pUI32, &eix);
			if (! resB) {
				break;
			}
			if (showUS &&
					(elemCnt < 10 || eix < 4 || eix > elemCnt - 3)) {
				printf("  ix %010u data: %010u\n", eix, *pUI32);
			}
		} while (st_dynlist_ite_gotoNext(pDL));
	}

	/* */
	if (resB) {
		TEST__RSET_SORT(custSO)
		custSO.asc = ascOrDesc;
		timeB = st_sysGetTime();
		TEST_FCOM__prf(cFNC, "Sorting elements...");
		resB = st_dynlist_sortElements(pDL, sortAlgo,
				TEST_DL__cbCmpElemsUI32, TEST_DL__cbSortStatus, &custSO);
		if (! resB) {
			TEST_FCOM__prf(cFNC, "st_dynlist_sortElements() failed");
		} else {
			timeE = st_sysGetTime();
			printf("\t\t\ttime --> %.2fs  (comps: %010u)\n\n",
					timeE - timeB, custSO.comps);
		}
	}

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Checking sorted elements...");
		cnt = st_dynlist_getElementCount(pDL);
		if (cnt != elemCnt) {
			sprintf(msg, "amount of elems wrong: %u != %u", cnt, elemCnt);
			TEST_FCOM__prf(cFNC, msg);
			resB = ST_B_FALSE;
		}
	}
	if (resB) {
		Tst_uint32 x;

		if (ascOrDesc) {
			x = 1;
		} else {
			x = 0xffffffff;
		}
		st_dynlist_ite_gotoFirst(pDL);
		do {
			resB = TEST_DL__getIxAndDataUI32(cFNC, pDL, &pUI32, &eix);
			if (! resB) {
				break;
			}
			if (showSO) {
				printf("  ix %010u data: %010u\n", eix, *pUI32);
			}
			resB = (ascOrDesc ? *pUI32 >= x : *pUI32 <= x);
			if (! resB) {
				sprintf(msg, "  sorting error: %u !%s %u",
						*pUI32, (ascOrDesc ? ">=" : "<="), x);
				TEST_FCOM__prf(cFNC, msg);
				/* */
				st_dynlist_ite_gotoFirst(pDL);
				do {
					TEST_DL__getIxAndDataUI32(cFNC, pDL, &pUI32, &eix);
					printf("  ix %010u data: %010u\n", eix, *pUI32);
				} while (st_dynlist_ite_gotoNext(pDL));
				break;
			}
			x = *pUI32;
		} while (st_dynlist_ite_gotoNext(pDL));
	}
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "Checking internal state of list...");
		resB = st_dynlist_check(pDL);
		if (! resB) {
			TEST_FCOM__prf(cFNC, "st_dynlist_check() failed");
		}
	}

	return resB;
}

Tst_bool
TEST_DL__bench_sub(Tst_dynlist *pDL, const Tst_uint32 elemCnt,
		const Tst_dynlist_sortAlgo sortAlgo, const TtestDl__dir dir,
		const Tst_bool showUS, const Tst_bool showSO)
{
	const char      *cFNC  = __func__;
	const Tst_uint32 cRUNS = 1;
	Tst_bool    resB = ST_B_TRUE;
	Tst_err     res  = ST_ERR_SUCC;
	int         sa,
	            ad,
	            x;
	Tst_dynlist dupLst,
	            *pUse;

	res = st_dynlist_stc_initDL(&dupLst, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_OMEM) {
			TEST_FCOM__prf(cFNC, "out of mem");
		} else {
			TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		}
		return ST_B_FALSE;
	}
	// ReSharper disable once CppDFAConstantConditions
	// ReSharper disable once CppRedundantBooleanExpressionArgument
	if (sortAlgo == ST_DYNLIST_SA_NONE || dir == TEST_DIR_NONE || cRUNS > 1) {
		pUse = &dupLst;
	} else {
		pUse = pDL;
	}
	for (sa = 0; sa < (int)ST_DYNLIST_SA_NONE; sa++) {
		if (sortAlgo != ST_DYNLIST_SA_NONE && (Tst_dynlist_sortAlgo)sa != sortAlgo) {
			continue;
		}
		for (ad = 0; ad < (int)TEST_DIR_NONE; ad++) {
			if (dir != TEST_DIR_NONE && (int)dir != ad) {
				continue;
			}
			for (x = 0; x < cRUNS; x++) {
				if (pUse == &dupLst) {
					TEST_FCOM__prf(cFNC, "cloning list from org to dup...");
					res = st_dynlist_clone(pDL, &dupLst,
							TEST_DL__cbCloneInitElem, TEST_DL__cbCloneCopyElem);
				}
				if (res != ST_ERR_SUCC) {
					TEST_FCOM__prf(cFNC, "st_dynlist_clone() failed");
					resB = ST_B_FALSE;
					break;
				}
				if (! TEST_DL__bench_sub2(pUse, elemCnt,
						(Tst_dynlist_sortAlgo)sa, ad == (int)TEST_DIR_ASC,
						showUS, showSO,
						x + 1, cRUNS)) {
					resB = ST_B_FALSE;
					break;
				}
			}
			if (! resB) {
				break;
			}
		}
		if (! resB) {
			break;
		}
	}
	st_dynlist_stc_freeDL(&dupLst);
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Benchmark sorting algos
 */
#if (RUN_BENCH_1 == 1)
Tst_bool
TEST_DL__bench1(void)
{
	#define LOC_RUN_1  1
	#define LOC_RUN_2  1
	const char *cFNC = __func__;
	#if (LOC_RUN_1 == 1)
		const Tst_uint32 cELEMS1 = 0x00000fff;
	#endif
	#if (LOC_RUN_2 == 1)
		const Tst_uint32 cELEMS2 = 0x000fffff;
	#endif
	Tst_bool    resB = ST_B_TRUE;
	Tst_err     res;
	Tst_uint32  x,
	            *pUI32 = NULL,
	            div;
	Tst_dynlist dl;
	char        msg[512];

	res = st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}

	/* all algos */
	#if (LOC_RUN_1 == 1)
		sprintf(msg, "#1 Adding/Inserting %u elements (ca. %.2f MB RAM)...",
				cELEMS1,
				(double)((cELEMS1 + 1024) *
						(sizeof(Tst_uint32) + (3 * sizeof(void*)))) /
					(1024.0 * 1024.0));
		TEST_FCOM__prf(cFNC, msg);
		for (x = 0; x < cELEMS1; x++) {
			if (x % 10000 == 0 || x + 1 == cELEMS1) {
				fprintf(stderr, "\r  add  prog %.2f%%",
						((double)x / (double)cELEMS1) * 100);
				if (x + 1 == cELEMS1) {
					fprintf(stderr, "\n");
				}
			}
			if (st_sysGetRand(1, 1000) % 2 == 0) {
				div = st_sysGetRand(1, st_sysGetRand(1, 100000));
			} else {
				div = 1;
			}
			if (st_dynlist_isEmpty(&dl) || st_sysGetRand(1, 1000) % 2 == 0) {
				/* add */
				resB = TEST_DL__addDataUI32(cFNC, &dl,
						st_sysGetRand(1, 0xffffffff / div), &pUI32);
			} else {
				/* insert */
				resB = TEST_DL__insDataUI32(cFNC, &dl,
						st_sysGetRand(1, st_dynlist_getElementCount(&dl)),
						st_sysGetRand(1, 0xffffffff / div), &pUI32);
			}
			if (! resB) {
				break;
			}
		}
		if (resB) {
			TEST_FCOM__prf(cFNC, "using all algos");
			resB = TEST_DL__bench_sub(&dl, cELEMS1,
					ST_DYNLIST_SA_NONE, TEST_DIR_NONE, ST_B_FALSE, ST_B_FALSE);
		}
	#endif

	/* only quick-/mergesort */
	#if (LOC_RUN_2 == 1)
		if (resB) {
			printf("\n");
			st_dynlist_stc_rsetDL(&dl);
			sprintf(msg, "#2 Inserting %u elements (ca. %.2f MB RAM)...",
					cELEMS2,
					(double)((cELEMS2 + 1024) *
							(sizeof(Tst_uint32) + (3 * sizeof(void*)))) /
						(1024.0 * 1024.0));
			TEST_FCOM__prf(cFNC, msg);
			for (x = 0; x < cELEMS2; x++) {
				if (x % 10000 == 0 || x + 1 == cELEMS2) {
					fprintf(stderr, "\r  add  prog %.2f%%",
							((double)x / (double)cELEMS2) * 100);
					if (x + 1 == cELEMS2) {
						fprintf(stderr, "\n");
					}
				}
				div = st_sysGetRand(1, st_sysGetRand(1, 100000));
				/* insert */
				resB = TEST_DL__insDataUI32(cFNC, &dl,
						1, st_sysGetRand(1, 0xffffffff / div), &pUI32);
				if (! resB) {
					break;
				}
			}
			if (resB) {
				TEST_FCOM__prf(cFNC, "using only quicksort");
				resB = TEST_DL__bench_sub(&dl, cELEMS2,
						ST_DYNLIST_SA_QUICKSORT, TEST_DIR_NONE, ST_B_FALSE, ST_B_FALSE);
				TEST_FCOM__prf(cFNC, "using only mergesort");
				resB = TEST_DL__bench_sub(&dl, cELEMS2,
						ST_DYNLIST_SA_MERGESORT, TEST_DIR_NONE, ST_B_FALSE, ST_B_FALSE);
			}
		}
	#endif

	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}
#endif  /* RUN_BENCH_1 */

/*----------------------------------------------------------------------------*/

/*
 * Benchmark sorting algos
 */
Tst_bool
TEST_DL__bench2(const Tst_str *pFn, const Tst_dynlist_sortAlgo algo,
		const TtestDl__dir dir)
{
	const char *cFNC = __func__;
	Tst_bool    resB;
	Tst_dynlist dl;

	TEST_FCOM__prf(cFNC, "Starting...");
	if (st_dynlist_stc_initDL(&dl, sizeof(Tst_uint32),
			TEST_DL__cbRsetElemUI32, TEST_DL__cbFreeElemUI32) != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "st_dynlist_stc_initDL() failed");
		return ST_B_FALSE;
	}
	if (! TEST_DL__printSortFile(pFn, &dl)) {
		return ST_B_FALSE;
	}

	resB = TEST_DL__bench_sub(&dl, st_dynlist_getElementCount(&dl),
			algo, dir, ST_B_FALSE, ST_B_FALSE);
	/* */
	if (resB) {
		TEST_FCOM__prf(cFNC, "OK");
	}
	st_dynlist_stc_freeDL(&dl);
	return resB;
}

/******************************************************************************/
