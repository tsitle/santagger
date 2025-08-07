/******************************************************************************/
/* test_binobj.c            [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for Binary Object                                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 24.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/streamrd.h"
#include "src/includes/common/binobj.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_file.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), getenv() */
//#include <string.h>       /* memset() */
#include <stdarg.h>       /* va_list, ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define RUN_TEST_0  1
#define RUN_TEST_1  1
#define RUN_TEST_2  1

Tst_uint32 TEST__getCRC32(const Tst_str *pFn);

Tst_bool TEST__0_rdBO(const char *pInFn, Tst_uint32 srcCRC32);
Tst_bool TEST__1_rdBO(const char *pInFn, Tst_uint32 srcCRC32);
Tst_bool TEST__2_rdBO(const char *pInFn, Tst_uint32 srcCRC32);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	Tst_uint32 srcCRC32;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
		return 1;
	}

	srcCRC32 = TEST__getCRC32((const Tst_str*)argv[1]);

#	if (RUN_TEST_0 == 1)
	if (! TEST__0_rdBO(argv[1], srcCRC32)) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");
#	endif

#	if (RUN_TEST_1 == 1)
	if (! TEST__1_rdBO(argv[1], srcCRC32)) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");
#	endif

#	if (RUN_TEST_2 == 1)
	if (! TEST__2_rdBO(argv[1], srcCRC32)) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");
#	endif

	printf("All tests passed :-)\n");
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
TEST__prf(const char *pFnc, const char *pMsg, ...)
{
	va_list args;

	printf("%s(): ", pFnc);
	va_start(args, pMsg);
	vprintf(pMsg, args);
	va_end(args);
	printf("\n");
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_uint32
TEST__getCRC32(const Tst_str *pFn)
{
	const char *cFNCN = "TEST__getCRC32";
	Tst_buf      buf[1024 * 16];
	Tst_uint32   rdBytes = 0,
	             crc32;
	Tst_sys_fstc fstcIn;
	Tst_streamrd strrd;

	TEST__prf(cFNCN, "Getting src CRC32 of '%s'...", pFn);

	st_sys_stc_initFStc(&fstcIn);
	st_sysFStc_setFilen(&fstcIn, pFn);

	if (st_sysFStc_openExisting(&fstcIn, ST_B_TRUE, ST_B_FALSE) != ST_ERR_SUCC) {
		TEST__prf(cFNCN, "can't open file '%s'",
				pFn);
		st_sys_stc_freeFStc(&fstcIn);
		return 0;
	}

	st_streamrd_stc_initSObj(&strrd);
	st_streamrd_setInput_file(&strrd, &fstcIn);
	st_streamrd_startCRC32(&strrd);
	while (! st_streamrd_isEOF(&strrd)) {
		st_streamrd_rdBuffer(&strrd, sizeof(buf), buf, &rdBytes);
	}
	crc32 = st_streamrd_getCRC32(&strrd);

	/* */
	st_streamrd_stc_freeSObj(&strrd);
	st_sys_stc_freeFStc(&fstcIn);
	return crc32;
}

/*----------------------------------------------------------------------------*/

Tst_uint32
TEST__getCRC32bo(const char *pFnc, Tst_binobj *pBO)
{
	Tst_err      res     = ST_ERR_SUCC;
	Tst_buf      buf[4096];
	Tst_uint32   rdBytes = 0,
	             crc32   = 0;
	Tst_streamrd strrd;

	st_streamrd_stc_initSObj(&strrd);

	TEST__prf(pFnc, "Reading BO with Strrd...");

	res = st_binobj_attachStreamrd(pBO, &strrd, 0);
	if (res != ST_ERR_SUCC) {
		TEST__prf(pFnc, "st_binobj_attachStreamrd() failed");
	} else {
		st_streamrd_startCRC32(&strrd);
		while (! st_streamrd_isEOF(&strrd)) {
			st_streamrd_rdBuffer(&strrd, sizeof(buf), buf, &rdBytes);
		}
		crc32 = st_streamrd_getCRC32(&strrd);
	}
	st_binobj_detachStreamrd(pBO);

	st_streamrd_stc_freeSObj(&strrd);
	return crc32;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read binary object from file and store it in a file
 */
Tst_bool
TEST__0_rdBO(const char *pInFn, const Tst_uint32 srcCRC32)
{
	const char *cFNCN = "TEST__0_rdBO";
	Tst_bool   bres = ST_B_TRUE;
	Tst_err    res  = ST_ERR_SUCC;
	Tst_uint32 crc32,
	           thres;
	Tst_binobj bo,
	           boCopy;

	st_binobj_stc_initBO(&bo);
	st_binobj_stc_initBO(&boCopy);
	/* */
	TEST__prf(cFNCN, "Reading BinObj from file --> file...");

	thres = 0;
	st_binobj_setThreshold(&bo, thres);
	if (thres != st_binobj_getThreshold(&bo)) {
		TEST__prf(cFNCN, "threshold mismatch");
		bres = ST_B_FALSE;
	}

	if (bres) {
		res = st_binobj_setData_file(&bo, (const Tst_str*)pInFn, ST_B_FALSE);
		if (res != ST_ERR_SUCC) {
			TEST__prf(cFNCN, "st_binobj_setData_file() failed");
			bres = ST_B_FALSE;
		}
	}

	if (bres &&
			st_sysGetFileSz_byFn((const Tst_str*)pInFn, NULL) != (Tst_fsize)st_binobj_getDataSize(&bo)) {
		TEST__prf(cFNCN, "dataSize mismatch");
		bres = ST_B_FALSE;
	}

	if (bres) {
		crc32 = TEST__getCRC32bo(cFNCN, &bo);

		TEST__prf(cFNCN, " CRC32: src 0x%08x | bo   0x%08x",
				srcCRC32, crc32);
		if (srcCRC32 != crc32) {
			TEST__prf(cFNCN, " CRC32 mismatch");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Copying BinObj...");
		res = st_binobj_copy(&bo, &boCopy);
		if (res != ST_ERR_SUCC) {
			TEST__prf(cFNCN, "st_binobj_copy() failed");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		crc32 = TEST__getCRC32bo(cFNCN, &boCopy);

		TEST__prf(cFNCN, " CRC32: src 0x%08x | boCp 0x%08x",
				srcCRC32, crc32);
		if (srcCRC32 != crc32) {
			TEST__prf(cFNCN, " CRC32 mismatch");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}
	/* */
	st_binobj_stc_freeBO(&bo);
	st_binobj_stc_freeBO(&boCopy);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Read binary object from file and store it in a buffer
 */
Tst_bool
TEST__1_rdBO(const char *pInFn, const Tst_uint32 srcCRC32)
{
	const char *cFNCN = "TEST__1_rdBO";
	Tst_bool   bres = ST_B_TRUE;
	Tst_err    res  = ST_ERR_SUCC;
	Tst_uint32 crc32,
	           thres;
	Tst_binobj bo,
	           boCopy;

	st_binobj_stc_initBO(&bo);
	st_binobj_stc_initBO(&boCopy);
	/* */
	TEST__prf(cFNCN, "Reading BinObj from file --> buffer...");

	thres = (Tst_uint32)st_sysGetFileSz_byFn((const Tst_str*)pInFn, NULL) + 1;
	st_binobj_setThreshold(&bo, thres);
	if (thres != st_binobj_getThreshold(&bo)) {
		TEST__prf(cFNCN, "threshold mismatch");
		bres = ST_B_FALSE;
	}

	if (bres) {
		res = st_binobj_setData_file(&bo, (const Tst_str*)pInFn, ST_B_FALSE);
		if (res != ST_ERR_SUCC) {
			TEST__prf(cFNCN, "st_binobj_setData_file() failed");
			bres = ST_B_FALSE;
		}
	}

	if (bres &&
			st_sysGetFileSz_byFn((const Tst_str*)pInFn, NULL) != (Tst_fsize)st_binobj_getDataSize(&bo)) {
		TEST__prf(cFNCN, "dataSize mismatch");
		bres = ST_B_FALSE;
	}

	if (bres) {
		crc32 = TEST__getCRC32bo(cFNCN, &bo);

		TEST__prf(cFNCN, " CRC32: src 0x%08x | bo   0x%08x",
				srcCRC32, crc32);
		if (srcCRC32 != crc32) {
			TEST__prf(cFNCN, " CRC32 mismatch");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Copying BinObj...");
		res = st_binobj_copy(&bo, &boCopy);
		if (res != ST_ERR_SUCC) {
			TEST__prf(cFNCN, "st_binobj_copy() failed");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		crc32 = TEST__getCRC32bo(cFNCN, &boCopy);

		TEST__prf(cFNCN, " CRC32: src 0x%08x | boCp 0x%08x",
				srcCRC32, crc32);
		if (srcCRC32 != crc32) {
			TEST__prf(cFNCN, " CRC32 mismatch");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}
	/* */
	st_binobj_stc_freeBO(&bo);
	st_binobj_stc_freeBO(&boCopy);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Read binary object from file and store it in a buffer
 */
Tst_bool
TEST__2_rdBO(const char *pInFn, const Tst_uint32 srcCRC32)
{
	const char *cFNCN = "TEST__2_rdBO";
	Tst_bool     bres    = ST_B_TRUE;
	Tst_err      res     = ST_ERR_SUCC;
	Tst_buf      buf[1024 * 16];
	Tst_uint32   rdBytes = 0,
	             crc32,
	             thres;
	Tst_sys_fstc fstcIn;
	Tst_binobj   bo,
	             boCopy;

	st_binobj_stc_initBO(&bo);
	st_binobj_stc_initBO(&boCopy);
	st_sys_stc_initFStc(&fstcIn);
	/* */
	TEST__prf(cFNCN, "Reading BinObj from file --> buffer/file...");

	thres = (Tst_uint32)sizeof(buf) * 4;
	st_binobj_setThreshold(&bo, thres);
	if (thres != st_binobj_getThreshold(&bo)) {
		TEST__prf(cFNCN, "threshold mismatch");
		bres = ST_B_FALSE;
	}

	if (bres) {
		st_sysFStc_setFilen(&fstcIn, (const Tst_str*)pInFn);
		res = st_sysFStc_openExisting(&fstcIn, ST_B_TRUE, ST_B_FALSE);
		if (res != ST_ERR_SUCC) {
			TEST__prf(cFNCN, "can't open file '%s'",
					pInFn);
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		while (! st_sysFStc_isEOF(&fstcIn)) {
			st_sysFStc_readBuf(&fstcIn, (Tst_uint32)sizeof(buf), buf, &rdBytes);
			res = st_binobj_appendData(&bo, buf, rdBytes);
			if (res != ST_ERR_SUCC) {
				TEST__prf(cFNCN, "st_binobj_appendData() failed");
				bres = ST_B_FALSE;
				break;
			}
		}
	}

	if (bres &&
			st_sysGetFileSz_byFn((const Tst_str*)pInFn, NULL) != (Tst_fsize)st_binobj_getDataSize(&bo)) {
		TEST__prf(cFNCN, "dataSize mismatch");
		bres = ST_B_FALSE;
	}

	if (bres) {
		crc32 = TEST__getCRC32bo(cFNCN, &bo);

		TEST__prf(cFNCN, " CRC32: src 0x%08x | bo   0x%08x",
				srcCRC32, crc32);
		if (srcCRC32 != crc32) {
			TEST__prf(cFNCN, " CRC32 mismatch");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Copying BinObj...");
		res = st_binobj_copy(&bo, &boCopy);
		if (res != ST_ERR_SUCC) {
			TEST__prf(cFNCN, "st_binobj_copy() failed");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		crc32 = TEST__getCRC32bo(cFNCN, &boCopy);

		TEST__prf(cFNCN, " CRC32: src 0x%08x | boCp 0x%08x",
				srcCRC32, crc32);
		if (srcCRC32 != crc32) {
			TEST__prf(cFNCN, " CRC32 mismatch");
			bres = ST_B_FALSE;
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}
	/* */
	st_binobj_stc_freeBO(&bo);
	st_binobj_stc_freeBO(&boCopy);
	st_sys_stc_freeFStc(&fstcIn);
	return bres;
}

/******************************************************************************/
