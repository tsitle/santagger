/******************************************************************************/
/* main_strwr.c             [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for the Stream Writer                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/streamwr.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), getenv() */
#include <string.h>       /* memset() */
#include <stdarg.h>       /* va_list, ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define DEL_OUTP_FILES  1
#define RUN_TEST_0  1
#define RUN_TEST_1  1
#define RUN_TEST_2  1

Tst_bool TEST__0_cpFile(const char *pInFn);
Tst_bool TEST__1_cpFile(const char *pInFn);
Tst_bool TEST__2_cpFile(const char *pInFn);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
		return 1;
	}

	st_sysInitRand(0);

#	if (RUN_TEST_0 == 1)
	if (! TEST__0_cpFile(argv[1])) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");
#	endif

#	if (RUN_TEST_1 == 1)
	if (! TEST__1_cpFile(argv[1])) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");
#	endif

#	if (RUN_TEST_2 == 1)
	if (! TEST__2_cpFile(argv[1])) {
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
/*
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
*/

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool
SF__setupFiles(const char *pFnc, const int fncNr, const char *pInFn,
		Tst_sys_fstc *pFStcIn, Tst_sys_fstc *pFStcOut, Tst_streamwr *pSObj)
{
	Tst_bool bres    = ST_B_TRUE;
	Tst_err  res;
	char     *pOutFn = NULL;

	st_sys_stc_initFStc(pFStcIn);
	st_sys_stc_initFStc(pFStcOut);

	st_sysFStc_setFilen(pFStcIn, (Tst_str*)pInFn);
	res = st_sysFStc_openExisting(pFStcIn, ST_B_TRUE, ST_B_FALSE);
	if (res != ST_ERR_SUCC) {
		TEST__prf(pFnc, "Can't open file '%s'",
				st_sysFStc_getFilen(pFStcIn));
		st_sys_stc_freeFStc(pFStcIn);
		st_sys_stc_freeFStc(pFStcOut);
		return ST_B_FALSE;
	}
	ST_CALLOC(pOutFn, char*, strlen(pInFn) + 100, 1)
	sprintf(pOutFn, "%s-tstrwr%02d.out", pInFn, fncNr);
	if (st_sysDoesFileExist((const Tst_str*)pOutFn)) {
		st_sysUnlinkFile((const Tst_str*)pOutFn);
	}
	st_sysFStc_setFilen(pFStcOut, (Tst_str*)pOutFn);
	ST_DELPOINT(pOutFn)
	res = st_sysFStc_openNew(pFStcOut);
	if (res != ST_ERR_SUCC) {
		TEST__prf(pFnc, "Can't create file '%s'",
				st_sysFStc_getFilen(pFStcOut));
		st_sysFStc_close(pFStcIn);
		st_sys_stc_freeFStc(pFStcIn);
		st_sys_stc_freeFStc(pFStcOut);
		return ST_B_FALSE;
	}

	/* */
	st_streamwr_stc_initSObj(pSObj);
	res = st_streamwr_setOutput_file(pSObj, pFStcOut);
	if (res != ST_ERR_SUCC) {
		bres = ST_B_FALSE;
		TEST__prf(pFnc, "Can't associate outp-file with SObj");
	}

	return bres;
}

void
SF__freeStcs(Tst_sys_fstc *pFStcIn, Tst_sys_fstc *pFStcOut,
		Tst_streamwr *pSObj, const Tst_bool delFile)
{
	st_streamwr_stc_freeSObj(pSObj);
	st_sysFStc_close(pFStcIn);
	st_sysFStc_close(pFStcOut);
#	if (DEL_OUTP_FILES == 1)
	if (delFile) {
		st_sysUnlinkFile(st_sysFStc_getFilen(pFStcOut));
	}
#	endif
	st_sys_stc_freeFStc(pFStcIn);
	st_sys_stc_freeFStc(pFStcOut);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Writer
 */
Tst_bool
TEST__0_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__0_cpFile";
	Tst_bool     bres   = ST_B_TRUE;
	Tst_err      res    = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamwr sobj;
	Tst_uint32   cpd     = 0;
	Tst_buf      ibuf[16 * 1024],
	             *pIB,
	             *pIBEnd;

	if (! SF__setupFiles(cFNCN, 0, pInFn,
			&fstcIn, &fstcOut, &sobj)) {
		return ST_B_FALSE;
	}

	/* */
	TEST__prf(cFNCN, "Copying file...");
	st_streamwr_startCRC8(&sobj);
	st_streamwr_startCRC16(&sobj);
	st_streamwr_startCRC32(&sobj);

	while (res == ST_ERR_SUCC && bres) {
		res = st_sysFStc_readBuf(&fstcIn, sizeof(ibuf),
				ibuf, &cpd);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_sysFStc_readBuf() failed");
			break;
		}
		if (cpd == 0) {
			break;
		}
		pIB    = ibuf;
		pIBEnd = pIB + cpd;

		do {
			res = st_streamwr_wrByte(&sobj, 8, *pIB);
			if (res != ST_ERR_SUCC) {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamwr_wrByte() failed");
				break;
			}
			++pIB;
		} while (pIB < pIBEnd);
	}

	if (bres) {
		TEST__prf(cFNCN, " CRC8 : 0x%02x        %u",
				st_streamwr_getCRC8(&sobj), st_streamwr_getCRC8(&sobj));
		TEST__prf(cFNCN, " CRC16: 0x%04x      %u",
				st_streamwr_getCRC16(&sobj), st_streamwr_getCRC16(&sobj));
		TEST__prf(cFNCN, " CRC32: 0x%08x  %u",
				st_streamwr_getCRC32(&sobj), st_streamwr_getCRC32(&sobj));
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobj, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Writer
 */
Tst_bool
TEST__1_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__1_cpFile";
	Tst_bool     bres   = ST_B_TRUE;
	Tst_err      res    = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamwr sobj;
	Tst_uint32   cpd     = 0,
	             rnd;
	Tst_buf      ibuf[16 * 1024],
	             *pIB,
	             *pIBEnd;
	/**Tst_byte  byt;**/

	if (! SF__setupFiles(cFNCN, 1, pInFn,
			&fstcIn, &fstcOut, &sobj)) {
		return ST_B_FALSE;
	}

	/* */
	TEST__prf(cFNCN, "Copying file...");
	st_streamwr_startCRC8(&sobj);
	st_streamwr_startCRC16(&sobj);
	st_streamwr_startCRC32(&sobj);

	while (res == ST_ERR_SUCC && bres) {
		res = st_sysFStc_readBuf(&fstcIn, sizeof(ibuf),
				ibuf, &cpd);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_sysFStc_readBuf() failed");
			break;
		}
		if (cpd == 0) {
			break;
		}
		pIB    = ibuf;
		pIBEnd = pIB + cpd;

		do {
			while ((rnd = st_sysGetRand(0, 0xffffff) % 8) == 0) {
				// keep going until [rnd] % 8 != 0
			}
			/**byt   = (*pIB << rnd);
			byt >>= rnd;
			printf(" val 0x%02x  %ub: 0x%02x | %ub: 0x%02x\n",
					*pIB, rnd, *pIB >> (8 - rnd),
					8 - rnd, byt);**/
			res = st_streamwr_wrByte(&sobj, rnd, *pIB >> (8 - rnd));
			if (res != ST_ERR_SUCC) {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamwr_wrByte() failed");
				break;
			}
			res = st_streamwr_wrByte(&sobj, 8 - rnd, *pIB);
			if (res != ST_ERR_SUCC) {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamwr_wrByte() failed");
				break;
			}
			++pIB;
		} while (pIB < pIBEnd);
	}

	if (bres) {
		TEST__prf(cFNCN, " CRC8 : 0x%02x        %u",
				st_streamwr_getCRC8(&sobj), st_streamwr_getCRC8(&sobj));
		TEST__prf(cFNCN, " CRC16: 0x%04x      %u",
				st_streamwr_getCRC16(&sobj), st_streamwr_getCRC16(&sobj));
		TEST__prf(cFNCN, " CRC32: 0x%08x  %u",
				st_streamwr_getCRC32(&sobj), st_streamwr_getCRC32(&sobj));
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobj, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Writer
 */
Tst_bool
TEST__2_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__2_cpFile";
	Tst_bool     bres     = ST_B_TRUE;
	Tst_err      res      = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamwr sobj[3];
	Tst_uint32   cpd      = 0,
	             crc32[3] = {0, 0, 0};
	Tst_buf      ibuf[16 * 1024];

	/* sobj[0] uses an output file
	 * sobj[1] uses an output buffer
	 * sobj[2] is only used for CRC computation  */
	if (! SF__setupFiles(cFNCN, 2, pInFn,
			&fstcIn, &fstcOut, &sobj[0])) {
		return ST_B_FALSE;
	}
	st_streamwr_stc_initSObj(&sobj[1]);
	st_streamwr_stc_initSObj(&sobj[2]);
	res = st_streamwr_setOutput_buffer(&sobj[1]);
	if (res != ST_ERR_SUCC) {
		bres = ST_B_FALSE;
		TEST__prf(cFNCN, "st_streamwr_setOutput_buffer() failed");
	}

	/* */
	if (bres) {
		TEST__prf(cFNCN, "Copying file...");
		st_streamwr_startCRC32(&sobj[0]);
		st_streamwr_startCRC32(&sobj[1]);
		st_streamwr_startCRC32(&sobj[2]);
	}

	while (res == ST_ERR_SUCC && bres) {
		res = st_sysFStc_readBuf(&fstcIn, sizeof(ibuf),
				ibuf, &cpd);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_sysFStc_readBuf() failed");
			break;
		}
		if (cpd == 0) {
			break;
		}

		st_streamwr_updateCRC32(&sobj[2], ibuf, cpd);

		res = st_streamwr_wrBuffer(&sobj[0], cpd, ibuf);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_streamwr_wrBuffer() failed");
			break;
		}

		res = st_streamwr_wrBuffer(&sobj[1], cpd, ibuf);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_streamwr_wrBuffer() failed");
			break;
		}
	}

	if (bres) {
		crc32[0] = st_streamwr_getCRC32(&sobj[0]);
		crc32[1] = st_streamwr_getCRC32(&sobj[1]);
		crc32[2] = st_streamwr_getCRC32(&sobj[2]);
		TEST__prf(cFNCN, " CRC32: "
				"read 0x%08x  writeF 0x%08x  writeB 0x%08x",
				crc32[2], crc32[0], crc32[1]);
		if (crc32[2] != crc32[0] || crc32[2] != crc32[1]) {
			TEST__prf(cFNCN, "CRC32 doesn't match !");
			bres = ST_B_FALSE;
		} else {
			TEST__prf(cFNCN, "Done.");
		}
	}

	/* */
	st_streamwr_stc_freeSObj(&sobj[1]);
	st_streamwr_stc_freeSObj(&sobj[2]);
	SF__freeStcs(&fstcIn, &fstcOut, &sobj[0], bres);
	return bres;
}

/******************************************************************************/
