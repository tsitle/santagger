/******************************************************************************/
/* main_strrd.c             [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for the Stream Reader                                        */
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
#include "src/includes/common/streamrd.h"
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
#define RUN_TEST_0   1
#define RUN_TEST_1   1
#define RUN_TEST_2   1
#define RUN_TEST_3   1

#if (RUN_TEST_0 == 1)
Tst_bool TEST__0_cpFile(const char *pInFn);
#endif
#if (RUN_TEST_1 == 1)
Tst_bool TEST__1_cpFile(const char *pInFn);
#endif
#if (RUN_TEST_2 == 1)
Tst_bool TEST__2_cpFile(const char *pInFn);
#endif
#if (RUN_TEST_3 == 1)
Tst_bool TEST__3_cpFile(const char *pInFn);
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
		return 1;
	}

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

#	if (RUN_TEST_3 == 1)
	if (! TEST__3_cpFile(argv[1])) {
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
		Tst_sys_fstc *pFStcIn, Tst_sys_fstc *pFStcOut, Tst_streamrd *pSObj)
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
	sprintf(pOutFn, "%s-tstrrd%02d.out", pInFn, fncNr);
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
	st_streamrd_stc_initSObj(pSObj);
	res = st_streamrd_setInput_file(pSObj, pFStcIn);
	if (res != ST_ERR_SUCC) {
		bres = ST_B_FALSE;
		TEST__prf(pFnc, "Can't associate inp-file with SObj");
	}

	return bres;
}

void
SF__freeStcs(Tst_sys_fstc *pFStcIn, Tst_sys_fstc *pFStcOut,
		Tst_streamrd *pSObj, const Tst_bool delFile)
{
	st_streamrd_stc_freeSObj(pSObj);
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
 * Tests the Stream Reader
 */
Tst_bool
TEST__0_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__0_cpFile";
	Tst_bool     bres = ST_B_TRUE;
	Tst_err      res  = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamrd sobj;
	Tst_buf      ibuf[4096],
	             *pIB,
	             *pIBEnd;

	if (! SF__setupFiles(cFNCN, 0, pInFn,
			&fstcIn, &fstcOut, &sobj)) {
		return ST_B_FALSE;
	}

	/* */
	TEST__prf(cFNCN, "Copying file...");
	st_streamrd_startCRC8(&sobj);
	st_streamrd_startCRC16(&sobj);
	st_streamrd_startCRC32(&sobj);

	while (res == ST_ERR_SUCC && ! st_streamrd_isEOF(&sobj)) {
		pIB    = ibuf;
		pIBEnd = pIB + sizeof(ibuf);

		while (pIB < pIBEnd) {
			res = st_streamrd_rdByte(&sobj, 8, pIB);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "st_streamrd_rdByte() failed");
				}
				break;
			}
			++pIB;
		}
		if (! bres) {
			break;
		}
		/* */
		st_sysFStc_writeBuf(&fstcOut, ibuf, pIB - (Tst_buf*)&ibuf);
	}

	if (bres) {
		TEST__prf(cFNCN, " CRC8 : 0x%02x        %u",
				st_streamrd_getCRC8(&sobj), st_streamrd_getCRC8(&sobj));
		TEST__prf(cFNCN, " CRC16: 0x%04x      %u",
				st_streamrd_getCRC16(&sobj), st_streamrd_getCRC16(&sobj));
		TEST__prf(cFNCN, " CRC32: 0x%08x  %u",
				st_streamrd_getCRC32(&sobj), st_streamrd_getCRC32(&sobj));
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobj, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Reader
 */
Tst_bool
TEST__1_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__1_cpFile";
	Tst_bool     bres = ST_B_TRUE;
	Tst_err      res  = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamrd sobj;
	Tst_buf      buf[1024];
	Tst_uint32   cp        = sizeof(buf),
	             rdFromStr = 0;

	if (! SF__setupFiles(cFNCN, 1, pInFn,
			&fstcIn, &fstcOut, &sobj)) {
		return ST_B_FALSE;
	}

	/* */
	TEST__prf(cFNCN, "Copying file...");
	while (res == ST_ERR_SUCC) {
		if (cp > st_streamrd_getAmountRemainingBytes(&sobj)) {
			cp = st_streamrd_getAmountRemainingBytes(&sobj);
		}
		res = st_streamrd_rdAheadBuffer(&sobj, cp, buf, &rdFromStr);
		if (res != ST_ERR_SUCC || cp != rdFromStr) {
			if (res == ST_ERR_FEOF) {
				TEST__prf(cFNCN, "eof");
			} else {
				bres = ST_B_FALSE;
				TEST__prf(cFNCN, "ERROR #2");
			}
		} else {
			st_streamrd_rdSkipBytes(&sobj, cp, ST_B_TRUE);
			st_sysFStc_writeBuf(&fstcOut, buf, cp);
			if (cp < 1) {
				break;
			}
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobj, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Reader
 */
Tst_bool
TEST__2_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__2_cpFile";
	Tst_bool     bres = ST_B_TRUE;
	Tst_err      res  = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamrd sobj;
	Tst_buf      buf[1024];
	Tst_uint32   ui32;
	Tst_uint32   rb,
	             cp        = sizeof(buf),
	             copied    = 0,
	             rdFromStr = 0;
	Tst_byte     ui8       = 0;
	Tst_uint16   ui16      = 0;

	if (! SF__setupFiles(cFNCN, 2, pInFn,
			&fstcIn, &fstcOut, &sobj)) {
		return ST_B_FALSE;
	}

	/* */
	TEST__prf(cFNCN, "Copying file...");
	while (res == ST_ERR_SUCC) {
		rb = st_streamrd_getAmountRemainingBytes(&sobj);
		if (cp > rb) {
			cp = rb;
		}
		if (cp < 1) {
			break;
		}
		/* */
		if (rb >= 4 && rb % 19 == 0) {
			res = st_streamrd_rdUInt32(&sobj,
					ST_STREAMRD_IEND_IGN, 32, &ui32);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "UI32  from %u to %u",
						copied, copied + 4 - 1);
				ui32 = st_sysReverseByteOrder_UI32(ui32);
				st_sysFStc_writeBuf(&fstcOut, &ui32, 4);
				copied += 4;
			}
		} else if (rb >= 2 && rb % 17 == 0) {
			res = st_streamrd_rdUInt16(&sobj,
					ST_STREAMRD_IEND_IGN, 16, &ui16);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "UI16  from %u to %u",
						copied, copied + 2 - 1);
				ui16 = st_sysReverseByteOrder_UI16(ui16);
				st_sysFStc_writeBuf(&fstcOut, &ui16, 2);
				copied += 2;
			}
		} else if (rb >= 2 && rb % 13 == 0) {
			res = st_streamrd_rdByte(&sobj, 8, &ui8);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				st_sysFStc_writeBuf(&fstcOut, &ui8, 1);
			}
			res = st_streamrd_rdByte(&sobj, 8, &ui8);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "UI8x2  from %u to %u",
						copied, copied + 2 - 1);
				st_sysFStc_writeBuf(&fstcOut, &ui8, 1);
				copied += 2;
			}
		} else if (rb > 0 && rb % 11 == 0) {
			res = st_streamrd_rdByte(&sobj, 8, &ui8);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "UI8  from %u to %u",
						copied, copied + 1 - 1);
				st_sysFStc_writeBuf(&fstcOut, &ui8, 1);
				copied += 1;
			}
		} else {
			res = st_streamrd_rdAheadBuffer(&sobj, cp, buf, &rdFromStr);
			if (res != ST_ERR_SUCC || cp != rdFromStr) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "MCP  from %u to %u",
						copied, copied + cp - 1);
				st_streamrd_rdSkipBytes(&sobj, cp, ST_B_TRUE);
				st_sysFStc_writeBuf(&fstcOut, buf, cp);
				copied += cp;
			}
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobj, bres);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests the Stream Reader
 */
Tst_bool
TEST__3_cpFile(const char *pInFn)
{
	const char *cFNCN = "TEST__3_cpFile";
	Tst_bool     bres = ST_B_TRUE;
	Tst_err      res  = ST_ERR_SUCC;
	Tst_sys_fstc fstcIn,
	             fstcOut;
	Tst_streamrd sobj;
	Tst_buf      buf[1024];
	Tst_uint32   ui32;
	Tst_uint32   rb,
	             cp        = sizeof(buf),
	             copied    = 0,
	             rdFromStr = 0;
	Tst_byte     ui8       = 0;
	Tst_uint16   ui16      = 0;
	Tst_uint32   rnd;

	if (! SF__setupFiles(cFNCN, 3, pInFn,
			&fstcIn, &fstcOut, &sobj)) {
		return ST_B_FALSE;
	}

	/* */
	st_sysInitRand(0);
	TEST__prf(cFNCN, "Copying file...");
	while (res == ST_ERR_SUCC) {
		rnd = st_sysGetRand(1, 100);
		rb  = st_streamrd_getAmountRemainingBytes(&sobj);
		if (cp > rb) {
			cp = rb;
		}
		if (cp < 1) {
			break;
		}
		/* */
		if (rb >= 4 && rnd % 19 == 0) {
			res = st_streamrd_rdUInt32(&sobj,
					ST_STREAMRD_IEND_IGN, 32, &ui32);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "UI32  from %u to %u",
						copied, copied + 4 - 1);
				ui32 = st_sysReverseByteOrder_UI32(ui32);
				st_sysFStc_writeBuf(&fstcOut, &ui32, 4);
				copied += 4;
			}
		} else if (rb >= 2 && rnd % 17 == 0) {
			res = st_streamrd_rdUInt16(&sobj,
					ST_STREAMRD_IEND_IGN, 16, &ui16);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "UI16  from %u to %u",
						copied, copied + 2 - 1);
				ui16 = st_sysReverseByteOrder_UI16(ui16);
				st_sysFStc_writeBuf(&fstcOut, &ui16, 2);
				copied += 2;
			}
		} else if (rb >= 1 && rnd % 13 == 0) {
			res = st_streamrd_rdAheadByte(&sobj, 8, &ui8);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "Ahead UI8  from %u to %u",
						copied, copied + 1 - 1);
			}
		} else if (rb >= 2 && rnd % 11 == 0) {
			res = st_streamrd_rdAheadUInt16(&sobj,
					ST_STREAMRD_IEND_IGN, 16, &ui16);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "Ahead UI16  from %u to %u",
						copied, copied + 2 - 1);
			}
		} else if (rb > 0 && rnd % 9 == 0) {
			res = st_streamrd_rdByte(&sobj, 8, &ui8);
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "UI8  from %u to %u",
						copied, copied + 1 - 1);
				st_sysFStc_writeBuf(&fstcOut, &ui8, 1);
				copied += 1;
			}
		} else {
			res = st_streamrd_rdAheadBuffer(&sobj, cp, buf, &rdFromStr);
			if (res != ST_ERR_SUCC || cp != rdFromStr) {
				if (res == ST_ERR_FEOF) {
					TEST__prf(cFNCN, "eof");
				} else {
					bres = ST_B_FALSE;
					TEST__prf(cFNCN, "ERROR #2");
				}
			} else {
				TEST__prf(cFNCN, "MCP  from %u to %u",
						copied, copied + cp - 1);
				st_streamrd_rdSkipBytes(&sobj, cp, ST_B_TRUE);
				st_sysFStc_writeBuf(&fstcOut, buf, cp);
				copied += cp;
			}
		}
	}

	if (bres) {
		TEST__prf(cFNCN, "Done.");
	}

	/* */
	SF__freeStcs(&fstcIn, &fstcOut, &sobj, bres);
	return bres;
}

/******************************************************************************/
