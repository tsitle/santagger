/******************************************************************************/
/* main_stream.c            [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for the Stream Reader / Writer                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/streamrd.h"
#include "src/includes/common/streamwr.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), getenv() */
#include <string.h>       /* memset() */
#include <stdarg.h>       /* va_list, ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define DEL_OUTP_FILES  1
#define RUN_TEST_I0  1
#define RUN_TEST_I1  1
#define RUN_TEST_I2  1
#define RUN_TEST_I3  1

Tst_bool TEST__I0_cpFile(const char *pInFn);
Tst_bool TEST__I1_cpFile(const char *pInFn);
Tst_bool TEST__I2_cpFile(const char *pInFn);
Tst_bool TEST__I3_varVals();
Tst_bool TEST__S0_endian();
Tst_bool TEST__S1_endian();

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
printUsage(const char *pApp)
{
	fprintf(stderr, "Usage: %s -i inputfile\n"
			"\t--> run test with inputfile\n", pApp);
	fprintf(stderr, "       %s -s\n"
			"\t--> run standard tests\n\n", pApp);
	exit(1);
}

int
main(const int argc, const char *argv[])
{
	Tst_byte x;
	Tst_bool argI = ST_B_FALSE,
	         argS = ST_B_FALSE;

	argI = (argc == 3 && strcmp(argv[1], "-i") == 0);
	argS = (! argI && argc == 2 && strcmp(argv[1], "-s") == 0);
	if (! (argI || argS)) {
		printUsage(argv[0]);
	}

	st_sysInitRand(0);

	if (argI) {  /* run tests with inputfile */
#		if (RUN_TEST_I0 == 1)
		for (x = 0; x < 3; x++) {
			printf("Run #%u/%u\n", x + 1, 3);
			if (! TEST__I0_cpFile(argv[2])) {
				printf("! Test failed !\n");
				return 1;
			}
			printf("\n");
		}
#		endif

#		if (RUN_TEST_I1 == 1)
		for (x = 0; x < 3; x++) {
			printf("Run #%u/%u\n", x + 1, 3);
			if (! TEST__I1_cpFile(argv[2])) {
				printf("! Test failed !\n");
				return 1;
			}
			printf("\n");
		}
#		endif

#		if (RUN_TEST_I2 == 1)
		for (x = 0; x < 3; x++) {
			printf("Run #%u/%u\n", x + 1, 3);
			if (! TEST__I2_cpFile(argv[2])) {
				printf("! Test failed !\n");
				return 1;
			}
			printf("\n");
		}
#		endif

#		if (RUN_TEST_I3 == 1)
		for (x = 0; x < 3; x++) {
			printf("Run #%u/%u\n", x + 1, 3);
			if (! TEST__I3_varVals()) {
				printf("! Test failed !\n");
				return 1;
			}
			printf("\n");
		}
#		endif
	} else if (argS) {  /* run standard tests */
		if (! TEST__S0_endian()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");

		if (! TEST__S1_endian()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	}

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

void
TEST__prBuf(const char *pFnc, const char *pMsg,
		const Tst_buf *pBuf, const Tst_uint32 sz)
{
	Tst_uint32 x,
	           y;

	printf("%s(): %s:\n  0x", pFnc, pMsg);
	for (x = 0; x < sz; x++) {
		for (y = x; y < x + 10 && y < sz; y++) {
			printf("%02x ", pBuf[y]);
		}
		x = y - 1;
		if (x + 1 < sz) {
			printf("\n  0x");
		}
	}
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
		Tst_sys_fstc *pFStcIn, Tst_sys_fstc *pFStcOut,
		Tst_streamrd *pSObjR, Tst_streamwr *pSObjW)
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
	sprintf(pOutFn, "%s-tstream%02d.out", pInFn, fncNr);
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
	st_streamrd_stc_initSObj(pSObjR);
	res = st_streamrd_setInput_file(pSObjR, pFStcIn);
	if (res != ST_ERR_SUCC) {
		bres = ST_B_FALSE;
		TEST__prf(pFnc, "Can't associate inp-file with SObj");
	}
	st_streamwr_stc_initSObj(pSObjW);
	res = st_streamwr_setOutput_file(pSObjW, pFStcOut);
	if (res != ST_ERR_SUCC) {
		bres = ST_B_FALSE;
		TEST__prf(pFnc, "Can't associate outp-file with SObj");
	}

	return bres;
}

void
SF__freeStcs(Tst_sys_fstc *pFStcIn, Tst_sys_fstc *pFStcOut,
		Tst_streamrd *pSObjR, Tst_streamwr *pSObjW,
		const Tst_bool delFile)
{
	st_streamrd_stc_freeSObj(pSObjR);
	st_streamwr_stc_freeSObj(pSObjW);
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
 * Tests the Stream Reader and Writer
 */
Tst_bool
TEST__I0_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__I0_cpFile";
	Tst_bool     bres   = ST_B_TRUE;
	Tst_err      res    = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamrd sobjR;
	Tst_streamwr sobjW;
	Tst_byte     byt = 0,
	             crc8[2];
	Tst_uint32   rnd,
	             crc32[2];
	Tst_uint16   crc16[2];

	if (! SF__setupFiles(cFNCN, 0, pInFn,
			&fstcIn, &fstcOut, &sobjR, &sobjW)) {
		return ST_B_FALSE;
	}

	/* */
	TEST__prf(cFNCN, "Copying file...");
	st_streamrd_startCRC8(&sobjR);
	st_streamrd_startCRC16(&sobjR);
	st_streamrd_startCRC32(&sobjR);
	st_streamwr_startCRC8(&sobjW);
	st_streamwr_startCRC16(&sobjW);
	st_streamwr_startCRC32(&sobjW);

	while (res == ST_ERR_SUCC && bres) {
		while ((rnd = st_sysGetRand(0, 0xffffff) % 8) == 0) {
			// keep going until [rnd] % 8 != 0
		}

		if (st_streamrd_getAmountRemainingBytes(&sobjR) <= 1 &&
				rnd > st_streamrd_getAmountRemainingBitsInCurByte(&sobjR)) {
			rnd = st_streamrd_getAmountRemainingBitsInCurByte(&sobjR);
			if (rnd == 0) {
				TEST__prf(cFNCN, "eof [1]");
				break;
			}
		}
		/**TEST__prf(cFNCN, " rd bits %u", rnd);**/
		res = st_streamrd_rdByte(&sobjR, rnd, &byt);
		if (res != ST_ERR_SUCC) {
			if (res == ST_ERR_FEOF) {
				TEST__prf(cFNCN, "eof [2]");
			} else {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamrd_rdByte() failed");
			}
			break;
		}

		res = st_streamwr_wrByte(&sobjW, rnd, byt);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_streamwr_wrByte() failed");
			break;
		}
	}

	if (bres) {
		crc8[0]  = st_streamrd_getCRC8(&sobjR);
		crc8[1]  = st_streamwr_getCRC8(&sobjW);
		crc16[0] = st_streamrd_getCRC16(&sobjR);
		crc16[1] = st_streamwr_getCRC16(&sobjW);
		crc32[0] = st_streamrd_getCRC32(&sobjR);
		crc32[1] = st_streamwr_getCRC32(&sobjW);
		TEST__prf(cFNCN, " CRC8 : read 0x%02x       write 0x%02x",
				crc8[0], crc8[1]);
		if (crc8[0] != crc8[1]) {
			TEST__prf(cFNCN, "CRC8 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, " CRC16: read 0x%04x     write 0x%04x",
				crc16[0], crc16[1]);
		if (crc16[0] != crc16[1]) {
			TEST__prf(cFNCN, "CRC16 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, " CRC32: read 0x%08x write 0x%08x",
				crc32[0], crc32[1]);
		if (crc32[0] != crc32[1]) {
			TEST__prf(cFNCN, "CRC32 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobjR, &sobjW, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Reader and Writer
 */
Tst_bool
TEST__I1_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__I1_cpFile";
	Tst_bool      bres   = ST_B_TRUE;
	Tst_err       res    = ST_ERR_SUCC;
	Tst_sys_fstc  fstcIn,
	              fstcOut;
	Tst_streamrd  sobjR;
	Tst_streamwr  sobjWF,
	              sobjWB;
	Tst_byte      byt = 0,
	              crc8[3];
	Tst_uint32    rnd,
	              crc32[3],
	              outpBSz,
	              bitsRd = 0;
	Tst_uint16    crc16[3];
	Tst_buf const *pOutpBuf;
	Tst_foffs     ioPos[2];

	if (! SF__setupFiles(cFNCN, 1, pInFn,
			&fstcIn, &fstcOut, &sobjR, &sobjWF)) {
		return ST_B_FALSE;
	}
	st_streamwr_stc_initSObj(&sobjWB);
	res = st_streamwr_setOutput_buffer(&sobjWB);
	if (res != ST_ERR_SUCC) {
		bres = ST_B_FALSE;
		TEST__prf(cFNCN, "st_streamwr_setOutput_buffer() failed");
	}

	/* */
	if (bres) {
		TEST__prf(cFNCN, "Copying file...");
		st_streamrd_startCRC8(&sobjR);
		st_streamrd_startCRC16(&sobjR);
		st_streamrd_startCRC32(&sobjR);
		st_streamwr_startCRC8(&sobjWF);
		st_streamwr_startCRC16(&sobjWF);
		st_streamwr_startCRC32(&sobjWF);
		st_streamwr_startCRC8(&sobjWB);
		st_streamwr_startCRC16(&sobjWB);
		st_streamwr_startCRC32(&sobjWB);
	}

	while (res == ST_ERR_SUCC && bres) {
		while ((rnd = st_sysGetRand(0, 0xffffff) % 8) == 0) {
			// keep going until [rnd] % 8 != 0
		}

		if (st_streamrd_getAmountRemainingBytes(&sobjR) <= 1 &&
				rnd > st_streamrd_getAmountRemainingBitsInCurByte(&sobjR)) {
			rnd = st_streamrd_getAmountRemainingBitsInCurByte(&sobjR);
			if (rnd == 0) {
				TEST__prf(cFNCN, "eof [1]");
				break;
			}
		}
		/**TEST__prf(cFNCN, " rd bits %u", rnd);**/
		res = st_streamrd_rdByte(&sobjR, rnd, &byt);
		if (res != ST_ERR_SUCC) {
			if (res == ST_ERR_FEOF) {
				TEST__prf(cFNCN, "eof [2]");
			} else {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamrd_rdByte() failed");
			}
			break;
		}
		bitsRd  += rnd;
		ioPos[0] = st_streamrd_getCurPos(&sobjR);

		res = st_streamwr_wrByte(&sobjWB, rnd, byt);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_streamwr_wrByte() failed");
			break;
		}

		/* */
		outpBSz = st_streamwr_getOutputBufSize(&sobjWB);
		if (outpBSz >= 1024 * 5 &&
				(st_streamwr_getAmountRemainingBitsInCurByte(&sobjWB) == 0 ||
					st_streamwr_getAmountRemainingBitsInCurByte(&sobjWB) == 8)) {
			pOutpBuf = st_streamwr_getOutputBuffer(&sobjWB);
			if (pOutpBuf == NULL) {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamwr_getOutputBuffer() failed");
				break;
			}
			res = st_streamwr_wrBuffer(&sobjWF, outpBSz, pOutpBuf);
			if (res != ST_ERR_SUCC) {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamwr_wrBuffer() failed");
				break;
			}
			st_streamwr_flush(&sobjWB);
			bitsRd   = 0;
			ioPos[1] = st_streamwr_getCurPos(&sobjWF);
			/* */
			if (ioPos[0] != ioPos[1]) {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "file positions mismatch");
				break;
			}
		}
	}

	/* */
	outpBSz = st_streamwr_getOutputBufSize(&sobjWB);
	if (outpBSz > 0) {
		if (st_streamwr_getAmountRemainingBitsInCurByte(&sobjWB) != 0 &&
				st_streamwr_getAmountRemainingBitsInCurByte(&sobjWB) != 8) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "some bits were left behind --> failure");
		} else {
			pOutpBuf = st_streamwr_getOutputBuffer(&sobjWB);
			if (pOutpBuf == NULL) {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "st_streamwr_getOutputBuffer() failed");
			} else {
				res = st_streamwr_wrBuffer(&sobjWF, outpBSz, pOutpBuf);
				if (res != ST_ERR_SUCC) {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "st_streamwr_wrBuffer() failed");
				} else {
					st_streamwr_flush(&sobjWB);
				}
			}
		}
	}

	if (bres) {
		crc8[0]  = st_streamrd_getCRC8(&sobjR);
		crc8[1]  = st_streamwr_getCRC8(&sobjWF);
		crc8[2]  = st_streamwr_getCRC8(&sobjWB);
		crc16[0] = st_streamrd_getCRC16(&sobjR);
		crc16[1] = st_streamwr_getCRC16(&sobjWF);
		crc16[2] = st_streamwr_getCRC16(&sobjWB);
		crc32[0] = st_streamrd_getCRC32(&sobjR);
		crc32[1] = st_streamwr_getCRC32(&sobjWF);
		crc32[2] = st_streamwr_getCRC32(&sobjWB);
		TEST__prf(cFNCN, " CRC8 : "
				"read 0x%02x       writeF 0x%02x       writeB 0x%02x",
				crc8[0], crc8[1], crc8[2]);
		if (crc8[0] != crc8[1] || crc8[0] != crc8[2]) {
			TEST__prf(cFNCN, "CRC8 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, " CRC16: "
				"read 0x%04x     writeF 0x%04x     writeB 0x%04x",
				crc16[0], crc16[1], crc16[2]);
		if (crc16[0] != crc16[1] || crc16[0] != crc16[2]) {
			TEST__prf(cFNCN, "CRC16 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, " CRC32: "
				"read 0x%08x writeF 0x%08x writeB 0x%08x",
				crc32[0], crc32[1], crc32[2]);
		if (crc32[0] != crc32[1] || crc32[0] != crc32[2]) {
			TEST__prf(cFNCN, "CRC32 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	st_streamwr_stc_freeSObj(&sobjWB);
	SF__freeStcs(&fstcIn, &fstcOut, &sobjR, &sobjWF, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Reader and Writer
 */
Tst_bool
TEST__I2_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__I2_cpFile";
	Tst_bool      bres = ST_B_TRUE;
	Tst_err       res  = ST_ERR_SUCC;
	Tst_sys_fstc  fstcIn,
	              fstcOut;
	Tst_streamrd  sobjR;
	Tst_streamwr  sobjWF;
	Tst_uint32    rnd,
	              rndT,
	              crc32[2],
	              rwUI32    = 0;
	Tst_int32     rwI32     = 0;
	Tst_uint16    rwUI16    = 0;
	Tst_int16     rwI16     = 0;
	Tst_uint64    rwUI64;
	Tst_int64     rwI64;
	Tst_foffs     ioPos[2];
	Tst_fsize     ofsz      = 0,
	              ofszBits  = 0,
	              abw       = 0,
	              abr       = 0,
	              amReBy    = 0;
	Tst_byte      bitsM8[2] = {0, 0},
	              rwUI8     = 0,
	              amReBi    = 0;

	if (! SF__setupFiles(cFNCN, 2, pInFn,
			&fstcIn, &fstcOut, &sobjR, &sobjWF)) {
		return ST_B_FALSE;
	}

	ST_SYSMATH_STC_RSETSI64(rwI64)
	ST_SYSMATH_STC_RSETUI64(rwUI64)
	/* */
	if (bres) {
		TEST__prf(cFNCN, "Copying file...");
		st_streamrd_startCRC32(&sobjR);
		st_streamwr_startCRC32(&sobjWF);
	}

	while (res == ST_ERR_SUCC && bres) {
		while ((rnd = st_sysGetRand(0, 0xffffff) % 64) == 0) {
			// keep going until [rnd] % 64 != 0
		}

		st_streamrd_getAmountRemainingBits(&sobjR, &amReBy, &amReBi);
		if (amReBy < 8) {
			amReBi += (amReBy * 8);
			if (rnd > amReBi) {
				rnd = amReBi;
				/**TEST__prf(cFNCN, "  rnd<--%u\n", rnd);**/
			}
			if (rnd == 0) {
				TEST__prf(cFNCN, "eof [1]");
				break;
			}
		}

		rndT = st_sysGetRand(0, 0xffffff) % 7;
		switch (rndT) {
		case 0:
			rnd = rnd % 8;
			/**TEST__prf(cFNCN, "  rd %2ubits BYTE", rnd);**/
			res = st_streamrd_rdByte(&sobjR, rnd, &rwUI8);
			break;
		case 1:
			rnd = rnd % 16;
			/**TEST__prf(cFNCN, "  rd %2ubits USHORT", rnd);**/
			res = st_streamrd_rdUInt16(&sobjR,
					ST_STREAMRD_IEND_IGN, rnd, &rwUI16);
			break;
		case 2:
			rnd = rnd % 16;
			/**TEST__prf(cFNCN, "  rd %2ubits SHORT", rnd);**/
			res = st_streamrd_rdInt16(&sobjR,
					ST_STREAMRD_IEND_IGN, rnd, &rwI16);
			break;
		case 3:
			rnd = rnd % 32;
			/**TEST__prf(cFNCN, "  rd %2ubits UINT32", rnd);**/
			res = st_streamrd_rdUInt32(&sobjR,
					ST_STREAMRD_IEND_IGN, rnd, &rwUI32);
			break;
		case 4:
			rnd = rnd % 32;
			/**TEST__prf(cFNCN, "  rd %2ubits INT32", rnd);**/
			res = st_streamrd_rdInt32(&sobjR,
					ST_STREAMRD_IEND_IGN, rnd, &rwI32);
			break;
		case 5:
			/**TEST__prf(cFNCN, "  rd %2ubits UINT64", rnd);**/
			res = st_streamrd_rdUInt64(&sobjR,
					ST_STREAMRD_IEND_IGN, rnd, &rwUI64);
			break;
		default:
			/**TEST__prf(cFNCN, "  rd %2ubits INT64", rnd);**/
			res = st_streamrd_rdInt64(&sobjR,
					ST_STREAMRD_IEND_IGN, rnd, &rwI64);
		}
		if (res != ST_ERR_SUCC) {
			if (res == ST_ERR_FEOF) {
				TEST__prf(cFNCN, "eof [2]");
			} else {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "reading value of type %u failed",
						rndT);
			}
			break;
		}
		ioPos[0] = st_streamrd_getCurPos(&sobjR);

		switch (rndT) {
			case 0:
				res = st_streamwr_wrByte(&sobjWF, rnd, rwUI8);
				break;
			case 1:
				res = st_streamwr_wrUInt16(&sobjWF, ST_STREAMWR_OEND_IGN, rnd, rwUI16);
				break;
			case 2:
				res = st_streamwr_wrInt16(&sobjWF, ST_STREAMWR_OEND_IGN, rnd, rwI16);
				break;
			case 3:
				res = st_streamwr_wrUInt32(&sobjWF, ST_STREAMWR_OEND_IGN, rnd, rwUI32);
				break;
			case 4:
				res = st_streamwr_wrInt32(&sobjWF, ST_STREAMWR_OEND_IGN, rnd, rwI32);
				break;
			case 5:
				res = st_streamwr_wrUInt64(&sobjWF, ST_STREAMWR_OEND_IGN, rnd, &rwUI64);
				break;
			default:
				res = st_streamwr_wrInt64(&sobjWF, ST_STREAMWR_OEND_IGN, rnd, &rwI64);
		}
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "writing value of type %u failed",
					rndT);
			break;
		}

		/* */
		ioPos[1]  = st_streamwr_getCurPos(&sobjWF);
		ofszBits += rnd;
		ofsz      = (ofszBits + 7) / 8;
		/* */
		if (ioPos[0] != ioPos[1]) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "file positions mismatch");
			break;
		}
		if (ofsz != st_streamwr_getOutputFileSize(&sobjWF)) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "output size mismatch");
			break;
		}
		/* */
		st_streamrd_getAmountBitsRead(&sobjR, &abr, &bitsM8[0]);
		st_streamwr_getAmountBitsWritten(&sobjWF, &abw, &bitsM8[1]);
		if (bitsM8[0] != bitsM8[1]) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "amount of read/written bits mismatch");
			break;
		}
		if (abr != abw) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "amount of read/written bytes mismatch");
			break;
		}
		if (abw != st_streamwr_getAmountBytesWritten(&sobjWF)) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "abw != st_streamwr_getAmountBytesWritten()");
			break;
		}
	}

	if (bres) {
		crc32[0] = st_streamrd_getCRC32(&sobjR);
		crc32[1] = st_streamwr_getCRC32(&sobjWF);
		TEST__prf(cFNCN, " CRC32: "
				"read 0x%08x writeF 0x%08x",
				crc32[0], crc32[1]);
		if (crc32[0] != crc32[1]) {
			TEST__prf(cFNCN, "CRC32 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobjR, &sobjWF, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Reader and Writer
 */
void
TEST__I3_varVals_genStr(Tst_str **ppStr, const Tst_uint32 maxLen)
{
	Tst_uint32 len = (maxLen == 0 ? st_sysGetRand(0, 1024) : maxLen),
	           x;

	ST_CALLOC(*ppStr, Tst_str*, len + 1, 1)
	if (*ppStr == NULL) {
		return;
	}
	(*ppStr)[len] = 0;
	for (x = 0; x < len; x++) {
		(*ppStr)[x] = 'A' + st_sysGetRand(0, 25);
	}
}

Tst_bool
TEST__I3_varVals()
{
#	define LOC_CHKWRFNC_(mac_fnc)  { \
				if (res != ST_ERR_SUCC) { \
					TEST__prf(cFNCN, "st_streamwr_wr%s() failed", mac_fnc); \
					bres = ST_B_FALSE; \
				} }
#	define LOC_CHKRDFNC_(mac_fnc)  { \
				if (res != ST_ERR_SUCC) { \
					TEST__prf(cFNCN, "st_streamrd_rd%s() failed", mac_fnc); \
					bres = ST_B_FALSE; \
				} }
	const char *cFNCN = "TEST__I3_varVals";
	Tst_bool      bres        = ST_B_TRUE;
	Tst_err       res         = ST_ERR_SUCC;
	Tst_streamrd  sobjR;
	Tst_streamwr  sobjWB;
	Tst_uint32    crc32[2]    = {0, 0},
	              wrUnary[4]  = {0, 0, 0, 0},
	              rdUnary[4]  = {0, 0, 0, 0},
	              outpBSz;
	Tst_buf const *pOutpBuf   = NULL;
	Tst_byte      skpdBits[2] = {0, 0},
	              rdUI8       = 0;
	Tst_str       *pWrStr[3]  = {NULL, NULL, NULL},
	              *pRdStr[3]  = {NULL, NULL, NULL};

	st_streamrd_stc_initSObj(&sobjR);
	st_streamwr_stc_initSObj(&sobjWB);
	res = st_streamwr_setOutput_buffer(&sobjWB);
	if (res != ST_ERR_SUCC) {
		bres = ST_B_FALSE;
		TEST__prf(cFNCN, "st_streamwr_setOutput_buffer() failed");
	}

	/* */
	if (bres) {
		TEST__prf(cFNCN, "Writing and reading generated data...");
		st_streamwr_startCRC32(&sobjWB);
	}

	/* Write values */
	wrUnary[0] = st_sysGetRand(0, 1024);
	res        = st_streamwr_wrUnary(&sobjWB, wrUnary[0]);
	LOC_CHKWRFNC_("Unary")
	if (bres) {
		wrUnary[1] = st_sysGetRand(0, 1024);
		res        = st_streamwr_wrUnary(&sobjWB, wrUnary[1]);
		LOC_CHKWRFNC_("Unary")
		skpdBits[0] = st_streamwr_getAmountRemainingBitsInCurByte(&sobjWB);
		if (skpdBits[0] == 8) {
			skpdBits[0] = 0;
		}
	}
	if (bres && skpdBits[0] > 0) {
		res = st_streamwr_wrByte(&sobjWB, skpdBits[0], 0);
		LOC_CHKWRFNC_("Byte")
	}
	if (bres) {
		TEST__I3_varVals_genStr(&pWrStr[0], 0);
		if (pWrStr[0] == NULL) {
			return ST_B_FALSE;
		}
		res = st_streamwr_wrString(&sobjWB, 0, pWrStr[0]);
		LOC_CHKWRFNC_("String")
	}
	if (bres) {
		wrUnary[2] = st_sysGetRand(0, 1024);
		res        = st_streamwr_wrUnary(&sobjWB, wrUnary[2]);
		LOC_CHKWRFNC_("Unary")
		skpdBits[1] = st_streamwr_getAmountRemainingBitsInCurByte(&sobjWB);
		if (skpdBits[1] == 8) {
			skpdBits[1] = 0;
		}
	}
	if (bres && skpdBits[1] > 0) {
		res = st_streamwr_wrByte(&sobjWB, skpdBits[1], 0);
		LOC_CHKWRFNC_("Byte")
	}
	if (bres) {
		TEST__I3_varVals_genStr(&pWrStr[1], 0);
		if (pWrStr[1] == NULL) {
			return ST_B_FALSE;
		}
		res = st_streamwr_wrString(&sobjWB, 0, pWrStr[1]);
		LOC_CHKWRFNC_("String")
	}
	if (bres) {
		TEST__I3_varVals_genStr(&pWrStr[2], 10);
		if (pWrStr[2] == NULL) {
			return ST_B_FALSE;
		}
		res = st_streamwr_wrString(&sobjWB, 7, pWrStr[2]);
		LOC_CHKWRFNC_("String")
		pWrStr[2][7] = 0;
	}
	if (bres) {
		wrUnary[3] = st_sysGetRand(0, 1024);
		res        = st_streamwr_wrUnary(&sobjWB, wrUnary[3]);
		LOC_CHKWRFNC_("Unary")
	}

	/* */
	outpBSz = st_streamwr_getOutputBufSize(&sobjWB);
	if (outpBSz > 0)
		pOutpBuf = st_streamwr_getOutputBuffer(&sobjWB);
	if (pOutpBuf == NULL) {
		TEST__prf(cFNCN, "st_streamwr_getOutputBuffer() failed");
		bres = ST_B_FALSE;
	}
	if (bres) {
		res = st_streamrd_setInput_buffer(&sobjR, pOutpBuf, outpBSz);
		if (res != ST_ERR_SUCC) {
			bres = ST_B_FALSE;
			TEST__prf(cFNCN, "st_streamrd_setInput_buffer() failed");
		} else {
			st_streamrd_startCRC32(&sobjR);
		}
	}

	/* Read values */
	if (bres) {
		res = st_streamrd_rdUnary(&sobjR, &rdUnary[0]);
		LOC_CHKRDFNC_("Unary")
		if (bres && wrUnary[0] != rdUnary[0]) {
			TEST__prf(cFNCN, "invalid value read [1]: 0x%08x != 0x%08x",
					wrUnary[0], rdUnary[0]);
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		res = st_streamrd_rdUnary(&sobjR, &rdUnary[1]);
		LOC_CHKRDFNC_("Unary")
		if (bres && wrUnary[1] != rdUnary[1]) {
			TEST__prf(cFNCN, "invalid value read [2]: 0x%08x != 0x%08x",
					wrUnary[1], rdUnary[1]);
			bres = ST_B_FALSE;
		}
	}
	if (bres && skpdBits[0] > 0) {
		res = st_streamrd_rdByte(&sobjR, skpdBits[0], &rdUI8);
		LOC_CHKRDFNC_("Byte")
		if (bres && rdUI8 != 0) {
			TEST__prf(cFNCN, "invalid value read [3]: 0x%02x != 0x%02x",
					rdUI8, 0);
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		res = st_streamrd_rdString(&sobjR, 0, &pRdStr[0]);
		LOC_CHKRDFNC_("String")
		if (bres && strcmp((char*)pWrStr[0], (char*)pRdStr[0]) != 0) {
			TEST__prf(cFNCN, "invalid value read [4]: '%s' != '%s'",
					(char*)pWrStr[0], (char*)pRdStr[0]);
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		res = st_streamrd_rdUnary(&sobjR, &rdUnary[2]);
		LOC_CHKRDFNC_("Unary")
		if (bres && wrUnary[2] != rdUnary[2]) {
			TEST__prf(cFNCN, "invalid value read [5]: 0x%08x != 0x%08x",
					wrUnary[2], rdUnary[2]);
			bres = ST_B_FALSE;
		}
	}
	if (bres && skpdBits[1] > 0) {
		res = st_streamrd_rdByte(&sobjR, skpdBits[1], &rdUI8);
		LOC_CHKRDFNC_("Byte")
		if (bres && rdUI8 != 0) {
			TEST__prf(cFNCN, "invalid value read [6]: 0x%02x != 0x%02x",
					rdUI8, 0);
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		res = st_streamrd_rdString(&sobjR, 0, &pRdStr[1]);
		LOC_CHKRDFNC_("String")
		if (bres && strcmp((char*)pWrStr[1], (char*)pRdStr[1]) != 0) {
			TEST__prf(cFNCN, "invalid value read [7]: '%s' != '%s'",
					(char*)pWrStr[1], (char*)pRdStr[1]);
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		res = st_streamrd_rdString(&sobjR, 7, &pRdStr[2]);
		LOC_CHKRDFNC_("String")
		if (bres && strcmp((char*)pWrStr[2], (char*)pRdStr[2]) != 0) {
			TEST__prf(cFNCN, "invalid value read [8]: '%s' != '%s'",
					(char*)pWrStr[2], (char*)pRdStr[2]);
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		res = st_streamrd_rdUnary(&sobjR, &rdUnary[3]);
		LOC_CHKRDFNC_("Unary")
		if (bres && wrUnary[3] != rdUnary[3]) {
			TEST__prf(cFNCN, "invalid value read [9]: 0x%08x != 0x%08x",
					wrUnary[3], rdUnary[3]);
			bres = ST_B_FALSE;
		}
	}

	/* */
	if (bres) {
		crc32[0] = st_streamwr_getCRC32(&sobjWB);
		crc32[1] = st_streamrd_getCRC32(&sobjR);
		TEST__prf(cFNCN, " CRC32: "
				"writeB 0x%08x read 0x%08x",
				crc32[0], crc32[1]);
		if (crc32[0] != crc32[1]) {
			TEST__prf(cFNCN, "CRC32 doesn't match !");
			bres = ST_B_FALSE;
		}
	}
	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	st_streamwr_stc_freeSObj(&sobjWB);
	st_streamrd_stc_freeSObj(&sobjR);
	ST_DELPOINT(pWrStr[0])
	ST_DELPOINT(pWrStr[1])
	ST_DELPOINT(pWrStr[2])
	ST_DELPOINT(pRdStr[0])
	ST_DELPOINT(pRdStr[1])
	ST_DELPOINT(pRdStr[2])
	return bres;
#	undef LOC_CHKWRFNC_
#	undef LOC_CHKRDFNC_
}

/*----------------------------------------------------------------------------*/

#define LOC_DTP_BYTE_   0
#define LOC_DTP_UI16_   1
#define LOC_DTP_UI32_   2
#define LOC_DTP_UI64_   3
#define LOC_DTP_SI16_   4
#define LOC_DTP_SI32_   5
#define LOC_DTP_SI64_   6
#define LOC_DTP_UNARY_  7

typedef struct {
	int        datTp;
	Tst_byte   nBits;
	Tst_byte   byt;
	Tst_uint16 ui16;
	Tst_uint32 ui32;
	Tst_uint64 ui64;
	Tst_int16  si16;
	Tst_int32  si32;
	Tst_int64  si64;
	Tst_uint32 unar;
} Tloc_ioData;

#define LOC_RSET_IODATA_(mac_iod)  { \
			(mac_iod).datTp = LOC_DTP_BYTE_; \
			(mac_iod).nBits = 0; \
			(mac_iod).byt   = 0; \
			(mac_iod).ui16  = 0; \
			(mac_iod).ui32  = 0; \
			ST_SYSMATH_STC_RSETUI64((mac_iod).ui64) \
			(mac_iod).si16 = 0; \
			(mac_iod).si32 = 0; \
			ST_SYSMATH_STC_RSETSI64((mac_iod).si64) \
			(mac_iod).unar = 0; \
			}

Tst_bool
TEST__SX_endian_subRd(const char *pFnc, Tst_streamrd *pStrrd,
		const Tloc_ioData *pExpc, const Tst_bool isInpLEorBE)
{
	Tst_bool    bres = ST_B_TRUE;
	Tst_err     res  = ST_ERR_SUCC;
	Tst_str     hex[2][50];
	Tloc_ioData         rdInp;
	Tst_streamrd_endian inpEnd = (isInpLEorBE ? ST_STREAMRD_IEND_LE : ST_STREAMRD_IEND_BE);

	hex[0][0] = 0;
	hex[1][0] = 0;
	LOC_RSET_IODATA_(rdInp)
	rdInp.datTp = pExpc->datTp;
	rdInp.nBits = pExpc->nBits;

	/*
	 * where ST_STREAMRD_IEND_IGN is used:
	 *   if isInpLEorBE then ENDIAN: LE-->!HOST
	 *                  else ENDIAN: BE-->HOST
	 * where ST_STREAMRD_IEND_LE or ST_STREAMRD_IEND_BE is used:
	 *   ENDIAN: LE-->HOST or BE-->HOST
	*/

	switch (pExpc->datTp) {
		case LOC_DTP_BYTE_:
			res = st_streamrd_rdByte(pStrrd, rdInp.nBits, &rdInp.byt);
			if (res == ST_ERR_SUCC && rdInp.byt != pExpc->byt) {
				TEST__prf(pFnc, "! rd byte#1: 0x%02x != 0x%02x\n", rdInp.byt, pExpc->byt);
				bres = ST_B_FALSE;
			}
			break;
		case LOC_DTP_UI16_:
			res = st_streamrd_rdAheadUInt16(pStrrd,
					ST_STREAMRD_IEND_IGN, rdInp.nBits, &rdInp.ui16);
			if (res == ST_ERR_SUCC) {
				if (isInpLEorBE) {
					rdInp.ui16 <<= (((16 - rdInp.nBits) / 8) * 8);
					rdInp.ui16   = st_sysReverseByteOrder_UI16(rdInp.ui16);  /* ENDIAN: swap */
				}
				if (rdInp.ui16 != pExpc->ui16) {
					TEST__prf(pFnc, "! rd ui16#1: 0x%04x != 0x%04x\n", rdInp.ui16, pExpc->ui16);
					bres = ST_B_FALSE;
				} else {
					res = st_streamrd_rdUInt16(pStrrd, inpEnd, rdInp.nBits, &rdInp.ui16);
					if (res == ST_ERR_SUCC && rdInp.ui16 != pExpc->ui16) {
						TEST__prf(pFnc, "! rd ui16#2: 0x%04x != 0x%04x\n", rdInp.ui16, pExpc->ui16);
						bres = ST_B_FALSE;
					}
				}
			}
			break;
		case LOC_DTP_UI32_:
			res = st_streamrd_rdAheadUInt32(pStrrd,
					ST_STREAMRD_IEND_IGN, rdInp.nBits, &rdInp.ui32);
			if (res == ST_ERR_SUCC) {
				if (isInpLEorBE) {
					rdInp.ui32 <<= (((32 - rdInp.nBits) / 8) * 8);
					rdInp.ui32   = st_sysReverseByteOrder_UI32(rdInp.ui32);  /* ENDIAN: swap */
				}
				if (rdInp.ui32 != pExpc->ui32) {
					TEST__prf(pFnc, "! rd ui32#1: 0x%08x != 0x%08x\n", rdInp.ui32, pExpc->ui32);
					bres = ST_B_FALSE;
				} else {
					res = st_streamrd_rdUInt32(pStrrd, inpEnd, rdInp.nBits, &rdInp.ui32);
					if (res == ST_ERR_SUCC && rdInp.ui32 != pExpc->ui32) {
						TEST__prf(pFnc, "! rd ui32#2: 0x%08x != 0x%08x\n", rdInp.ui32, pExpc->ui32);
						bres = ST_B_FALSE;
					}
				}
			}
			break;
		case LOC_DTP_UI64_:
			res = st_streamrd_rdAheadUInt64(pStrrd,
					ST_STREAMRD_IEND_IGN, rdInp.nBits, &rdInp.ui64);
			if (res == ST_ERR_SUCC) {
				if (isInpLEorBE) {
					st_sysUInt64_shiftLeft(&rdInp.ui64, ((64 - rdInp.nBits) / 8) * 8);
					st_sysUInt64_reverseByteOrder(&rdInp.ui64);  /* ENDIAN: swap */
				}
				if (st_sysUInt64_cmpUI64(&rdInp.ui64, &pExpc->ui64) != 0) {
					st_sysUInt64_toHexStr(&rdInp.ui64, hex[0], sizeof(hex[0]));
					st_sysUInt64_toHexStr(&pExpc->ui64, hex[1], sizeof(hex[1]));
					TEST__prf(pFnc, "! rd ui64#1: %s != %s\n", hex[0], hex[1]);
					bres = ST_B_FALSE;
				} else {
					res = st_streamrd_rdUInt64(pStrrd, inpEnd, rdInp.nBits, &rdInp.ui64);
					if (res == ST_ERR_SUCC &&
							st_sysUInt64_cmpUI64(&rdInp.ui64, &pExpc->ui64) != 0) {
						st_sysUInt64_toHexStr(&rdInp.ui64, hex[0], sizeof(hex[0]));
						st_sysUInt64_toHexStr(&pExpc->ui64, hex[1], sizeof(hex[1]));
						TEST__prf(pFnc, "! rd ui64#2: %s != %s\n", hex[0], hex[1]);
						bres = ST_B_FALSE;
					}
				}
			}
			break;
		case LOC_DTP_SI16_:
			res = st_streamrd_rdAheadInt16(pStrrd,
					ST_STREAMRD_IEND_IGN, rdInp.nBits, &rdInp.si16);
			if (res == ST_ERR_SUCC) {
				if (isInpLEorBE) {
					rdInp.si16 =
							(Tst_int16)st_sysReverseByteOrder_UI16((Tst_uint16)rdInp.si16);  /* ENDIAN: swap */
					rdInp.si16 = (Tst_int16)(rdInp.si16 >> (((16 - rdInp.nBits) / 8) * 8));
				}
				if (rdInp.si16 != pExpc->si16) {
					TEST__prf(pFnc, "! rd si16#1: 0x%04x != 0x%04x\n", rdInp.si16, pExpc->si16);
					bres = ST_B_FALSE;
				} else {
					res = st_streamrd_rdInt16(pStrrd, inpEnd, rdInp.nBits, &rdInp.si16);
					if (res == ST_ERR_SUCC && rdInp.si16 != pExpc->si16) {
						TEST__prf(pFnc, "! rd si16#2: 0x%04x != 0x%04x\n", rdInp.si16, pExpc->si16);
						bres = ST_B_FALSE;
					}
				}
			}
			break;
		case LOC_DTP_SI32_:
			res = st_streamrd_rdAheadInt32(pStrrd,
					ST_STREAMRD_IEND_IGN, rdInp.nBits, &rdInp.si32);
			if (res == ST_ERR_SUCC) {
				if (isInpLEorBE) {
					rdInp.si32   =
							(Tst_int32)st_sysReverseByteOrder_UI32((Tst_uint32)rdInp.si32);  /* ENDIAN: swap */
					rdInp.si32 >>= (((32 - rdInp.nBits) / 8) * 8);
				}
				if (rdInp.si32 != pExpc->si32) {
					TEST__prf(pFnc, "! rd si32#1: 0x%08x != 0x%08x\n", rdInp.si32, pExpc->si32);
					bres = ST_B_FALSE;
				} else {
					res = st_streamrd_rdInt32(pStrrd, inpEnd, rdInp.nBits, &rdInp.si32);
					if (res == ST_ERR_SUCC && rdInp.si32 != pExpc->si32) {
						TEST__prf(pFnc, "! rd si32#2: 0x%08x != 0x%08x\n", rdInp.si32, pExpc->si32);
						bres = ST_B_FALSE;
					}
				}
			}
			break;
		case LOC_DTP_SI64_:
			res = st_streamrd_rdAheadInt64(pStrrd,
					ST_STREAMRD_IEND_IGN, rdInp.nBits, &rdInp.si64);
			if (res == ST_ERR_SUCC) {
				if (isInpLEorBE) {
					st_sysSInt64_reverseByteOrder(&rdInp.si64);  /* ENDIAN: swap */
					st_sysSInt64_shiftRight(&rdInp.si64, ((64 - rdInp.nBits) / 8) * 8);
				}
				if (st_sysSInt64_cmpI64(&rdInp.si64, &pExpc->si64) != 0) {
					st_sysSInt64_toHexStr(&rdInp.si64, hex[0], sizeof(hex[0]));
					st_sysSInt64_toHexStr(&pExpc->si64, hex[1], sizeof(hex[1]));
					TEST__prf(pFnc, "! rd si64#1: %s != %s\n", hex[0], hex[1]);
					bres = ST_B_FALSE;
				} else {
					res = st_streamrd_rdInt64(pStrrd, inpEnd, rdInp.nBits, &rdInp.si64);
					if (res == ST_ERR_SUCC &&
							st_sysSInt64_cmpI64(&rdInp.si64, &pExpc->si64) != 0) {
						st_sysSInt64_toHexStr(&rdInp.si64, hex[0], sizeof(hex[0]));
						st_sysSInt64_toHexStr(&pExpc->si64, hex[1], sizeof(hex[1]));
						TEST__prf(pFnc, "! rd si64#2: %s != %s\n", hex[0], hex[1]);
						bres = ST_B_FALSE;
					}
				}
			}
			break;
		case LOC_DTP_UNARY_:
			res = st_streamrd_rdUnary(pStrrd, &rdInp.unar);
			if (res == ST_ERR_SUCC && rdInp.unar != pExpc->unar) {
				TEST__prf(pFnc, "! rd unary#1: 0x%08x != 0x%08x\n", rdInp.unar, pExpc->unar);
				bres = ST_B_FALSE;
			}
			break;
		default:
			TEST__prf(pFnc, "unknown data type");
			bres = ST_B_FALSE;
	}

	if (bres && res != ST_ERR_SUCC) {
		TEST__prf(pFnc, "! error reading from stream: res %d", (int)res);
		bres = ST_B_FALSE;
	}

	return bres;
}

Tst_bool
TEST__SX_endian_subWr(const char *pFnc, Tst_streamwr *pStrwr,
		const Tloc_ioData *pToWr, const Tst_bool wrOutpLEorBE)
{
	Tst_bool bres = ST_B_TRUE;
	Tst_err  res  = ST_ERR_SUCC;
	Tst_streamwr_endian outEnd = (wrOutpLEorBE ? ST_STREAMWR_OEND_LE : ST_STREAMWR_OEND_BE);

	switch (pToWr->datTp) {
		case LOC_DTP_BYTE_:
			res = st_streamwr_wrByte(pStrwr, pToWr->nBits, pToWr->byt);
			break;
		case LOC_DTP_UI16_:
			res = st_streamwr_wrUInt16(pStrwr, outEnd, pToWr->nBits, pToWr->ui16);
			break;
		case LOC_DTP_UI32_:
			res = st_streamwr_wrUInt32(pStrwr, outEnd, pToWr->nBits, pToWr->ui32);
			break;
		case LOC_DTP_UI64_:
			res = st_streamwr_wrUInt64(pStrwr, outEnd, pToWr->nBits, &pToWr->ui64);
			break;
		case LOC_DTP_SI16_:
			res = st_streamwr_wrInt16(pStrwr, outEnd, pToWr->nBits, pToWr->si16);
			break;
		case LOC_DTP_SI32_:
			res = st_streamwr_wrInt32(pStrwr, outEnd, pToWr->nBits, pToWr->si32);
			break;
		case LOC_DTP_SI64_:
			res = st_streamwr_wrInt64(pStrwr, outEnd, pToWr->nBits, &pToWr->si64);
			break;
		case LOC_DTP_UNARY_:
			res = st_streamwr_wrUnary(pStrwr, pToWr->unar);
			break;
		default:
			TEST__prf(pFnc, "unknown data type");
			bres = ST_B_FALSE;
	}

	if (bres && res != ST_ERR_SUCC) {
		TEST__prf(pFnc, "! error writing to stream: res %d", (int)res);
		bres = ST_B_FALSE;
	}

	return bres;
}

/*
 * Tests the Stream Reader
 */
Tst_bool
TEST__S0_endian()
{
	const char *cFNCN = "TEST__S0_endian";
	Tst_bool   bres = ST_B_TRUE;
	Tst_buf    bufLE[50],
	           bufBE[50];
	Tst_uint32 ui32a[2],
	           tmpUI32,
	           usdLE = 0,
	           usdBE = 0;
	Tst_uint16 ui16a[2],
	           tmpUI16;
	Tloc_ioData   expc;
	Tst_streamrd  sobjR;

	ui32a[0] = 511;  /*0x000001ff*/
	ui16a[0] = 511;  /*0x01ff*/
	ui32a[1] = 11259375;  /*0x00abcdef*/
	ui16a[1] = 52;  /*0x0034*/

	/* write Little-Endian buffer */
	/** */
	tmpUI32 = ui32a[0];
#	if (WORDS_BIGENDIAN == 1)
	tmpUI32 = st_sysReverseByteOrder_UI32(tmpUI32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(bufLE, &tmpUI32, 4);
	usdLE += 4;
	/** */
	tmpUI16 = ui16a[0];
#	if (WORDS_BIGENDIAN == 1)
	tmpUI16 = st_sysReverseByteOrder_UI16(tmpUI16);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(&bufLE[4], &tmpUI16, 2);
	usdLE += 2;
	/** */
	tmpUI32 = ui32a[1];
#	if (WORDS_BIGENDIAN == 1)
	tmpUI32 = st_sysReverseByteOrder_UI32(tmpUI32);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(&bufLE[4 + 2], &tmpUI32, 3);
	usdLE += 3;
	/** */
	tmpUI16 = ui16a[1];
#	if (WORDS_BIGENDIAN == 1)
	tmpUI16 = st_sysReverseByteOrder_UI16(tmpUI16);  /* ENDIAN: BE-->LE */
#	endif
	memcpy(&bufLE[4 + 2 + 3], &tmpUI16, 1);
	usdLE += 1;
	/** */
	TEST__prBuf(cFNCN, "bufLE", bufLE, usdLE);

	/* write Big-Endian buffer */
	/** */
	tmpUI32 = ui32a[0];
#	if (WORDS_BIGENDIAN != 1)
	tmpUI32 = st_sysReverseByteOrder_UI32(tmpUI32);  /* ENDIAN: LE-->BE */
#	endif
	memcpy(bufBE, &tmpUI32, 4);
	usdBE += 4;
	/** */
	tmpUI16 = ui16a[0];
#	if (WORDS_BIGENDIAN != 1)
	tmpUI16 = st_sysReverseByteOrder_UI16(tmpUI16);  /* ENDIAN: LE-->BE */
#	endif
	memcpy(&bufBE[4], &tmpUI16, 2);
	usdBE += 2;
	/** */
	tmpUI32 = ui32a[1];
#	if (WORDS_BIGENDIAN != 1)
	tmpUI32 <<= 8;
	tmpUI32   = st_sysReverseByteOrder_UI32(tmpUI32);  /* ENDIAN: LE-->BE */
#	else
	tmpUI32 <<= 8;
#	endif
	memcpy(&bufBE[4 + 2], &tmpUI32, 3);
	usdBE += 3;
	/** */
	tmpUI16 = ui16a[1];
#	if (WORDS_BIGENDIAN != 1)
	tmpUI16   = st_sysReverseByteOrder_UI16(tmpUI16);  /* ENDIAN: LE-->BE */
	tmpUI16 >>= 8;
#	else
	tmpUI16 <<= 8;
#	endif
	memcpy(&bufBE[4 + 2 + 3], &tmpUI16, 1);
	usdBE += 1;
	/** */
	TEST__prBuf(cFNCN, "bufBE", bufBE, usdBE);

	/* */
	st_streamrd_stc_initSObj(&sobjR);

	/* read Little-Endian buffer */
	/** */
	LOC_RSET_IODATA_(expc)
	/** */
	TEST__prf(cFNCN, "reading LE-buffer...");
	st_streamrd_setInput_buffer(&sobjR, bufLE, usdLE);
	/** */
	expc.datTp = LOC_DTP_UI32_;
	expc.nBits = 32;
	expc.ui32  = ui32a[0];
	bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*LE:*/ST_B_TRUE);
	/** */
	if (bres) {
		expc.datTp = LOC_DTP_UI16_;
		expc.nBits = 16;
		expc.ui16  = ui16a[0];
		bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*LE:*/ST_B_TRUE);
	}
	/** */
	if (bres) {
		expc.datTp = LOC_DTP_UI32_;
		expc.nBits = 24;  /* % 8 == 0 */
		expc.ui32  = ui32a[1];
		bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*LE:*/ST_B_TRUE);
	}
	/** */
	if (bres) {
		expc.datTp = LOC_DTP_UI16_;
		expc.nBits = 8;  /* % 8 == 0 */
		expc.ui16  = ui16a[1];
		bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*LE:*/ST_B_TRUE);
	}

	/* read Big-Endian buffer */
	/** */
	LOC_RSET_IODATA_(expc)
	/** */
	if (bres) {
		TEST__prf(cFNCN, "reading BE-buffer...");
		st_streamrd_setInput_buffer(&sobjR, bufBE, usdBE);
	}
	/** */
	if (bres) {
		expc.datTp = LOC_DTP_UI32_;
		expc.nBits = 32;
		expc.ui32  = ui32a[0];
		bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*BE:*/ST_B_FALSE);
	}
	/** */
	if (bres) {
		expc.datTp = LOC_DTP_UI16_;
		expc.nBits = 16;
		expc.ui16  = ui16a[0];
		bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*BE:*/ST_B_FALSE);
	}
	/** */
	if (bres) {
		expc.datTp = LOC_DTP_UI32_;
		expc.nBits = 24;  /* % 8 == 0 */
		expc.ui32  = ui32a[1];
		bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*BE:*/ST_B_FALSE);
	}
	/** */
	if (bres) {
		expc.datTp = LOC_DTP_UI16_;
		expc.nBits = 8;  /* % 8 == 0 */
		expc.ui16  = ui16a[1];
		bres       = TEST__SX_endian_subRd(cFNCN, &sobjR, &expc, /*BE:*/ST_B_FALSE);
	}

	st_streamrd_stc_freeSObj(&sobjR);
	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Reader and Writer
 */
Tst_bool
TEST__S1_endian_sub_wrVals(const char *pFnc,
		const Tst_uint32 ecnt, const Tloc_ioData *pIODarr,
		Tst_streamwr *pStrwr,
		Tst_buf **ppBufLE, Tst_buf **ppBufBE,
		Tst_uint32 *pBSzLE, Tst_uint32 *pBSzBE)
{
	Tst_bool   bres = ST_B_TRUE;
	Tst_uint32 x;

	if (ppBufLE != NULL) {
		*pBSzLE = 0;
	}
	if (ppBufBE != NULL) {
		*pBSzBE = 0;
	}

	/* write Little-Endian buffer */
	if (ppBufLE != NULL) {
		/** */
		TEST__prf(pFnc, "writing LE-buffer...");
		st_streamwr_setOutput_buffer(pStrwr);
		/** */
		for (x = 0; x < ecnt; x++) {
			bres = TEST__SX_endian_subWr(pFnc, pStrwr,
					&pIODarr[x], /*LE:*/ST_B_TRUE);
			if (! bres) {
				break;
			}
		}
		/** copy LE buffer to pBufLE */
		if (bres) {
			*pBSzLE = st_streamwr_getOutputBufSize(pStrwr);
			ST_REALLOC(*ppBufLE, Tst_buf*, *pBSzLE, 1)
			if (*ppBufLE == NULL) {
				bres = ST_B_FALSE;
			} else if (*pBSzLE > 0) {
				memcpy(*ppBufLE, st_streamwr_getOutputBuffer(pStrwr), *pBSzLE);
			}
			TEST__prBuf(pFnc, "bufLE", *ppBufLE, *pBSzLE);
		}
	}

	/* write Big-Endian buffer */
	if (ppBufBE != NULL) {
		/** */
		if (bres) {
			TEST__prf(pFnc, "writing BE-buffer...");
			st_streamwr_setOutput_buffer(pStrwr);
			/** */
			for (x = 0; x < ecnt; x++) {
				bres = TEST__SX_endian_subWr(pFnc, pStrwr,
						&pIODarr[x], /*BE:*/ST_B_FALSE);
				if (! bres) {
					break;
				}
			}
		}
		/** copy BE buffer to *ppBufBE */
		if (bres) {
			*pBSzBE = st_streamwr_getOutputBufSize(pStrwr);
			ST_REALLOC(*ppBufBE, Tst_buf*, *pBSzBE, 1)
			if (*ppBufBE == NULL) {
				bres = ST_B_FALSE;
			} else if (*pBSzBE > 0) {
				memcpy(*ppBufBE, st_streamwr_getOutputBuffer(pStrwr), *pBSzBE);
			}
			TEST__prBuf(pFnc, "bufBE", *ppBufBE, *pBSzBE);
		}
	}
	return bres;
}

Tst_bool
TEST__S1_endian_sub_rdVals(const char *pFnc,
		const Tst_uint32 ecnt, const Tloc_ioData *pExpcArr,
		Tst_streamrd *pStrrd,
		const Tst_buf *pBufLE, const Tst_buf *pBufBE,
		const Tst_uint32 bszLE, const Tst_uint32 bszBE)
{
	Tst_bool   bres = ST_B_TRUE;
	Tst_uint32 x;

	/* read Little-Endian buffer */
	if (pBufLE != NULL) {
		TEST__prf(pFnc, "reading LE-buffer...");
		st_streamrd_setInput_buffer(pStrrd, pBufLE, bszLE);
		/** */
		for (x = 0; x < ecnt; x++) {
			bres = TEST__SX_endian_subRd(pFnc, pStrrd,
					&pExpcArr[x], /*LE:*/ST_B_TRUE);
			if (! bres) {
				break;
			}
		}
	}

	/* read Big-Endian buffer */
	if (bres && pBufBE != NULL) {
		TEST__prf(pFnc, "reading BE-buffer...");
		st_streamrd_setInput_buffer(pStrrd, pBufBE, bszBE);
		/** */
		for (x = 0; x < ecnt; x++) {
			bres = TEST__SX_endian_subRd(pFnc, pStrrd,
					&pExpcArr[x], /*BE:*/ST_B_FALSE);
			if (! bres) {
				break;
			}
		}
	}
	return bres;
}

Tst_bool
TEST__S1_endian()
{
#	define LOC_ELEMS_  8
	const char *cFNCN = "TEST__S1_endian";
	Tst_bool   bres    = ST_B_TRUE;
	Tst_buf    *pBufLE = NULL,
	           *pBufBE = NULL;
	Tst_uint32 usdLE   = 0,
	           usdBE   = 0;
	Tloc_ioData  expc[LOC_ELEMS_];
	Tst_streamrd sobjR;
	Tst_streamwr sobjW;

	/* */
	st_streamwr_stc_initSObj(&sobjW);
	st_streamrd_stc_initSObj(&sobjR);

	/* --------------------- */
	/* #A: prepare test data */
	/** */
	LOC_RSET_IODATA_(expc[0])
	expc[0].datTp = LOC_DTP_BYTE_;
	expc[0].nBits = 8;
	expc[0].byt   = 0x96;  /* b10010110 */
	/** */
	LOC_RSET_IODATA_(expc[1])
	expc[1].datTp = LOC_DTP_UI16_;
	expc[1].nBits = 16;
	expc[1].ui16  = 0xabcd;
	/** */
	LOC_RSET_IODATA_(expc[2])
	expc[2].datTp = LOC_DTP_UI32_;
	expc[2].nBits = 32;
	expc[2].ui32  = 0xabcd4321;
	/** */
	LOC_RSET_IODATA_(expc[3])
	expc[3].datTp = LOC_DTP_UI64_;
	expc[3].nBits = 64;
	st_sysUInt64_setLoHi(&expc[3].ui64, /*lo:*/0x08824655, /*hi:*/0x34101630);
	/** */
	LOC_RSET_IODATA_(expc[4])
	expc[4].datTp = LOC_DTP_SI16_;
	expc[4].nBits = 16;
	expc[4].si16  = (Tst_int16)0xabcd;
	/** */
	LOC_RSET_IODATA_(expc[5])
	expc[5].datTp = LOC_DTP_SI32_;
	expc[5].nBits = 32;
	expc[5].si32  = (Tst_int32)0xabcd4321;
	/** */
	LOC_RSET_IODATA_(expc[6])
	expc[6].datTp = LOC_DTP_SI64_;
	expc[6].nBits = 64;
	st_sysSInt64_setLoHi(&expc[6].si64, /*lo:*/0x08824655, /*hi:*/0x34101630);
	/** */
	LOC_RSET_IODATA_(expc[7])
	expc[7].datTp = LOC_DTP_UNARY_;
	expc[7].unar  = 33;

	/* #A: write/read */
	/** */
	TEST__prf(cFNCN, "test #A");
	/** write buffers */
	bres = TEST__S1_endian_sub_wrVals(cFNCN, LOC_ELEMS_, expc,
			&sobjW, &pBufLE, &pBufBE, &usdLE, &usdBE);
	/** read buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_rdVals(cFNCN, LOC_ELEMS_, expc,
				&sobjR, pBufLE, pBufBE, usdLE, usdBE);
	}

	/* --------------------- */
	/* #B: prepare test data */
	/** */
	LOC_RSET_IODATA_(expc[0])
	expc[0].datTp = LOC_DTP_BYTE_;
	expc[0].nBits = 8;
	expc[0].byt   = 0x69;
	/** */
	LOC_RSET_IODATA_(expc[1])
	expc[1].datTp = LOC_DTP_UI16_;
	expc[1].nBits = 8;  /* % 8 == 0 */
	expc[1].ui16  = 0xab;
	/** */
	LOC_RSET_IODATA_(expc[2])
	expc[2].datTp = LOC_DTP_UI32_;
	expc[2].nBits = 24;  /* % 8 == 0 */
	expc[2].ui32  = 0xcd4321;
	/** */
	LOC_RSET_IODATA_(expc[3])
	expc[3].datTp = LOC_DTP_UI64_;
	expc[3].nBits = 56;  /* % 8 == 0 */
	st_sysUInt64_setLoHi(&expc[3].ui64, /*lo:*/0x08824655, /*hi:*/0x101630);
	/** */
	LOC_RSET_IODATA_(expc[4])
	expc[4].datTp = LOC_DTP_SI16_;
	expc[4].nBits = 8;  /* % 8 == 0 */
	expc[4].si16  = (Tst_int16)0xffef;
	/** */
	LOC_RSET_IODATA_(expc[5])
	expc[5].datTp = LOC_DTP_SI32_;
	expc[5].nBits = 24;  /* % 8 == 0 */
	expc[5].si32  = (Tst_int32)0xffd5c4b3;
	/** */
	LOC_RSET_IODATA_(expc[6])
	expc[6].datTp = LOC_DTP_SI64_;
	expc[6].nBits = 56;  /* % 8 == 0 */
	st_sysSInt64_setLoHi(&expc[6].si64, /*lo:*/0x08824655, /*hi:*/0xff901630);
	/** */
	LOC_RSET_IODATA_(expc[7])
	expc[7].datTp = LOC_DTP_UNARY_;
	expc[7].unar  = 1;

	/* #B: write/read */
	/** */
	if (bres) {
		printf("\n");
		TEST__prf(cFNCN, "test #B");
	}
	/** write buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_wrVals(cFNCN, LOC_ELEMS_, expc,
				&sobjW, &pBufLE, &pBufBE, &usdLE, &usdBE);
	}
	/** read buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_rdVals(cFNCN, LOC_ELEMS_, expc,
				&sobjR, pBufLE, pBufBE, usdLE, usdBE);
	}

	/* --------------------- */
	/* #C: prepare test data */
	/** */
	LOC_RSET_IODATA_(expc[0])
	expc[0].datTp = LOC_DTP_BYTE_;
	expc[0].nBits = 8;
	expc[0].byt   = 0x00;
	/** */
	LOC_RSET_IODATA_(expc[1])
	expc[1].datTp = LOC_DTP_UI16_;
	expc[1].nBits = 8;  /* % 8 == 0 */
	expc[1].ui16  = 0xff;
	/** */
	LOC_RSET_IODATA_(expc[2])
	expc[2].datTp = LOC_DTP_UI32_;
	expc[2].nBits = 16;  /* % 8 == 0 */
	expc[2].ui32  = 0x4321;
	/** */
	LOC_RSET_IODATA_(expc[3])
	expc[3].datTp = LOC_DTP_UI64_;
	expc[3].nBits = 48;  /* % 8 == 0 */
	st_sysUInt64_setLoHi(&expc[3].ui64, /*lo:*/0x08824655, /*hi:*/0x1630);
	/** */
	LOC_RSET_IODATA_(expc[4])
	expc[4].datTp = LOC_DTP_SI16_;
	expc[4].nBits = 8;  /* % 8 == 0 */
	expc[4].si16  = -19;
	/** */
	LOC_RSET_IODATA_(expc[5])
	expc[5].datTp = LOC_DTP_SI32_;
	expc[5].nBits = 24;  /* % 8 == 0 */
	expc[5].si32  = -18000;
	/** */
	LOC_RSET_IODATA_(expc[6])
	expc[6].datTp = LOC_DTP_SI64_;
	expc[6].nBits = 48;  /* % 8 == 0 */
	st_sysSInt64_setI32(&expc[6].si64, -100000000);
	/** */
	LOC_RSET_IODATA_(expc[7])
	expc[7].datTp = LOC_DTP_UNARY_;
	expc[7].unar  = 2;

	/* #C: write/read */
	/** */
	if (bres) {
		printf("\n");
		TEST__prf(cFNCN, "test #C");
	}
	/** write buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_wrVals(cFNCN, LOC_ELEMS_, expc,
				&sobjW, &pBufLE, &pBufBE, &usdLE, &usdBE);
	}
	/** read buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_rdVals(cFNCN, LOC_ELEMS_, expc,
				&sobjR, pBufLE, pBufBE, usdLE, usdBE);
	}

	/* --------------------- */
	/* #D: prepare test data */
	/** */
	LOC_RSET_IODATA_(expc[0])
	expc[0].datTp = LOC_DTP_BYTE_;
	expc[0].nBits = 7;
	expc[0].byt   = 0x16;  /* b00010110 --> b0010110 */
	/** */
	LOC_RSET_IODATA_(expc[1])
	expc[1].datTp = LOC_DTP_UI16_;
	expc[1].nBits = 15;
	expc[1].ui16  = 0x2bcd;  /* b0010 1011 1100 1101 --> b010101111001101 */
	/** */
	LOC_RSET_IODATA_(expc[2])
	expc[2].datTp = LOC_DTP_UI32_;
	expc[2].nBits = 31;
	expc[2].ui32  = 0x2bcd4321;  /* --> b010 1011 1100 1101 0100 0011 0010 0001 */
	/** */
	LOC_RSET_IODATA_(expc[3])
	expc[3].datTp = LOC_DTP_UI64_;
	expc[3].nBits = 63;
	st_sysUInt64_setLoHi(&expc[3].ui64, /*lo:*/0x08824655, /*hi:*/0x34101630);
	/** */
	LOC_RSET_IODATA_(expc[4])
	expc[4].datTp = LOC_DTP_SI16_;
	expc[4].nBits = 15;
	expc[4].si16  = (Tst_int16)0x2bcd;
	/** */
	LOC_RSET_IODATA_(expc[5])
	expc[5].datTp = LOC_DTP_SI32_;
	expc[5].nBits = 31;
	expc[5].si32  = (Tst_int32)0x2bcd4321;
	/** */
	LOC_RSET_IODATA_(expc[6])
	expc[6].datTp = LOC_DTP_UNARY_;
	expc[6].unar  = 5;
	/** */
	LOC_RSET_IODATA_(expc[7])
	expc[7].datTp = LOC_DTP_SI64_;
	expc[7].nBits = 63;
	st_sysSInt64_setLoHi(&expc[7].si64, /*lo:*/0x08824655, /*hi:*/0x34101630);

	/* #D: write/read */
	/** */
	if (bres) {
		printf("\n");
		TEST__prf(cFNCN, "test #D");
	}
	/** write buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_wrVals(cFNCN, LOC_ELEMS_, expc,
				&sobjW, NULL, &pBufBE, NULL, &usdBE);
	}
	/** read buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_rdVals(cFNCN, LOC_ELEMS_, expc,
				&sobjR, NULL, pBufBE, 0, usdBE);
	}

	/* --------------------- */
	/* #E: prepare test data */
	/** */
	LOC_RSET_IODATA_(expc[0])
	expc[0].datTp = LOC_DTP_BYTE_;
	expc[0].nBits = 5;
	expc[0].byt   = 0x16;
	/** */
	LOC_RSET_IODATA_(expc[1])
	expc[1].datTp = LOC_DTP_UI16_;
	expc[1].nBits = 13;
	expc[1].ui16  = 0x0bcd;
	/** */
	LOC_RSET_IODATA_(expc[2])
	expc[2].datTp = LOC_DTP_UI32_;
	expc[2].nBits = 5;
	expc[2].ui32  = 0x04;
	/** */
	LOC_RSET_IODATA_(expc[3])
	expc[3].datTp = LOC_DTP_UI64_;
	expc[3].nBits = 30;
	st_sysUInt64_setLoHi(&expc[3].ui64, /*lo:*/0x08724653, /*hi:*/0x0);
	/** */
	LOC_RSET_IODATA_(expc[4])
	expc[4].datTp = LOC_DTP_SI16_;
	expc[4].nBits = 11;
	expc[4].si16  = -11;
	/** */
	LOC_RSET_IODATA_(expc[5])
	expc[5].datTp = LOC_DTP_SI32_;
	expc[5].nBits = 12;
	expc[5].si32  = -12;
	/** */
	LOC_RSET_IODATA_(expc[6])
	expc[6].datTp = LOC_DTP_UNARY_;
	expc[6].unar  = 85;
	/** */
	LOC_RSET_IODATA_(expc[7])
	expc[7].datTp = LOC_DTP_SI64_;
	expc[7].nBits = 34;
	st_sysSInt64_setLoHi(&expc[7].si64, /*lo:*/0xfedcba98, /*hi:*/0x02);

	/* #E: write/read */
	/** */
	if (bres) {
		printf("\n");
		TEST__prf(cFNCN, "test #E");
	}
	/** write buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_wrVals(cFNCN, LOC_ELEMS_, expc,
				&sobjW, NULL, &pBufBE, NULL, &usdBE);
	}
	/** read buffers */
	if (bres) {
		bres = TEST__S1_endian_sub_rdVals(cFNCN, LOC_ELEMS_, expc,
				&sobjR, NULL, pBufBE, 0, usdBE);
	}

	/* */
	st_streamwr_stc_freeSObj(&sobjW);
	st_streamrd_stc_freeSObj(&sobjR);
	ST_DELPOINT(pBufLE)
	ST_DELPOINT(pBufBE)
	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}
	return bres;
#	undef LOC_ELEMS_
}

/******************************************************************************/
