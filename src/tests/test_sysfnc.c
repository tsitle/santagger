/******************************************************************************/
/* test_sysfnc.c            [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for system functions                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.08.2025 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#include "src/includes/common/sys_fnc.h"
#include "fncs_test_common.h"

/*
// System-Includes
*/
#include <stdlib.h> /* exit(), calloc(), getenv() */
#include <string.h>

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool TEST_SYSFNC__0_rndInt(void);
Tst_bool TEST_SYSFNC__1_rndDbl(void);
Tst_bool TEST_SYSFNC__2_time(void);
Tst_bool TEST_SYSFNC__3_byteOrder(void);
Tst_bool TEST_SYSFNC__4_bufSearch(void);
Tst_bool TEST_SYSFNC__5_strings(void);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	if (! TEST_SYSFNC__0_rndInt()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFNC__1_rndDbl()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFNC__2_time()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFNC__3_byteOrder()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFNC__4_bufSearch()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFNC__5_strings()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	printf("TEST_SYSFNC -- All tests passed :-)\n");
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysGetRand()
 */
Tst_bool
TEST_SYSFNC__0_rndInt(void)
{
	const char *cFNCN = __func__;
	const Tst_uint32 cLOWER = 100;
	const Tst_uint32 cUPPER = 120;
	Tst_uint32 x;
	Tst_uint32 val;

	for (x = 0; x < (cUPPER - cLOWER) * 5; x++) {
		val = st_sysGetRand(cLOWER, cUPPER);
		if (val < cLOWER || val > cUPPER) {
			TEST_FCOM__prf(cFNCN, "error: val %u not in range %u .. %u", val, cLOWER, cUPPER);
			return ST_B_FALSE;
		}
		TEST_FCOM__prf(cFNCN, "- val %u", val);
	}

	TEST_FCOM__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests st_sysGetRand()
 */
Tst_bool
TEST_SYSFNC__1_rndDbl(void)
{
	const char *cFNCN = __func__;
	const double cLOWER = 100.0f;
	const double cUPPER = 120.0f;
	Tst_uint32 x;
	double val;

	for (x = 0; x < (Tst_uint32)(cUPPER - cLOWER) * 5; x++) {
		val = st_sysGetRandDbl(cLOWER, cUPPER);
		if (val < cLOWER || val > cUPPER) {
			TEST_FCOM__prf(cFNCN, "error: val %f not in range %f .. %f", val, cLOWER, cUPPER);
			return ST_B_FALSE;
		}
		TEST_FCOM__prf(cFNCN, "- val %f", val);
	}

	TEST_FCOM__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysGetTime()
 *       st_sysSleepMS()
 */
Tst_bool
TEST_SYSFNC__2_time(void)
{
	const char *cFNCN = __func__;
	double time1;
	double time2;

	//
	time1 = st_sysGetTime();
	st_sysSleepMS(500);
	time2 = st_sysGetTime();

	if (time2 - time1 < 0.49 || time2 - time1 > 0.6) {
		TEST_FCOM__prf(cFNCN, "error: time delta mismatch (is=%.f, exp=0.5)", time2 - time1);
		return ST_B_FALSE;
	}
	TEST_FCOM__prf(cFNCN, "time delta = %.2f", time2 - time1);

	//
	time1 = st_sysGetTime();
	st_sysSleepMS(750);
	time2 = st_sysGetTime();

	if (time2 - time1 < 0.749 || time2 - time1 > 0.8) {
		TEST_FCOM__prf(cFNCN, "error: time delta mismatch (is=%.f, exp=0.75)", time2 - time1);
		return ST_B_FALSE;
	}
	TEST_FCOM__prf(cFNCN, "time delta = %.2f", time2 - time1);

	//
	TEST_FCOM__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysReverseByteOrder_UI16()
 *       st_sysReverseByteOrder_UI32()
 *       st_sysReverseByteOrder_LL()
 */
Tst_bool
TEST_SYSFNC__3_byteOrder(void)
{
	const char *cFNCN = __func__;
	const Tst_uint16 cINP_UI16_S = 0x1234;
	const Tst_uint16 cINP_UI16_T = 0x3412;
	const Tst_uint32 cINP_UI32_S = 0x12345678;
	const Tst_uint32 cINP_UI32_T = 0x78563412;
	const Tst_buf cINP_UI64_S[8] = {0x12, 0x34, 0x56, 0x78, 0xA1, 0xB2, 0xC3, 0xD4};
	const Tst_buf cINP_UI64_T[8] = {0xD4, 0xC3, 0xB2, 0xA1, 0x78, 0x56, 0x34, 0x12};
	Tst_uint16 testUI16;
	Tst_uint32 testUI32;
	Tst_buf testUI64Buf[8];
	Tst_buf cmpUI64Buf[8];

	//
	testUI16 = st_sysReverseByteOrder_UI16(cINP_UI16_S);
	if (testUI16 != cINP_UI16_T) {
		TEST_FCOM__prf(cFNCN, "error: st_sysReverseByteOrder_UI16() failed (is=0x%04X, exp=0x%04X)", testUI16, cINP_UI16_T);
		return ST_B_FALSE;
	}

	//
	testUI32 = st_sysReverseByteOrder_UI32(cINP_UI32_S);
	if (testUI32 != cINP_UI32_T) {
		TEST_FCOM__prf(cFNCN, "error: st_sysReverseByteOrder_UI32() failed (is=0x%08X, exp=0x%08X)", testUI32, cINP_UI32_T);
		return ST_B_FALSE;
	}

	//
	memcpy(testUI64Buf, cINP_UI64_S, sizeof(testUI64Buf));
	memcpy(cmpUI64Buf, cINP_UI64_T, sizeof(cmpUI64Buf));
	st_sysReverseByteOrder_LL(testUI64Buf, sizeof(testUI64Buf));
	if (memcmp(testUI64Buf, cmpUI64Buf, sizeof(testUI64Buf)) != 0) {
		TEST_FCOM__prf(cFNCN, "error: st_sysReverseByteOrder_LL() failed");
		return ST_B_FALSE;
	}

	//
	TEST_FCOM__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysBufSearch()
 */
Tst_bool
TEST_SYSFNC__4_bufSearch(void)
{
	const char *cFNCN = __func__;
	const Tst_buf cINP_BIG[8] = {0x12, 0x34, 0x56, 0x78, 0xA1, 0xB2, 0xC3, 0xD4};
	const Tst_buf cINP_SUB1[1] = {0x78};
	const Tst_buf cINP_SUB2[3] = {0x34, 0x56, 0x78};
	const Tst_buf cINP_SUB3[3] = {0x11, 0x11, 0x11};
	const Tst_buf cINP_SUB4[8] = {0x12, 0x34, 0x56, 0x78, 0xA1, 0xB2, 0xC3, 0xD4};
	Tst_bool fnd;
	Tst_uint32 offs = 0;

	//
	fnd = st_sysBufSearch(cINP_BIG, sizeof(cINP_BIG), cINP_SUB1, sizeof(cINP_SUB1), &offs);
	if (! fnd || offs != 3) {
		TEST_FCOM__prf(cFNCN, "error: st_sysBufSearch() failed (sub1)");
		return ST_B_FALSE;
	}

	//
	fnd = st_sysBufSearch(cINP_BIG, sizeof(cINP_BIG), cINP_SUB2, sizeof(cINP_SUB2), &offs);
	if (! fnd || offs != 1) {
		TEST_FCOM__prf(cFNCN, "error: st_sysBufSearch() failed (sub2)");
		return ST_B_FALSE;
	}

	// expect fail
	fnd = st_sysBufSearch(cINP_BIG, sizeof(cINP_BIG), cINP_SUB3, sizeof(cINP_SUB3), &offs);
	if (fnd) {
		TEST_FCOM__prf(cFNCN, "error: st_sysBufSearch() failed (sub3)");
		return ST_B_FALSE;
	}

	//
	fnd = st_sysBufSearch(cINP_BIG, sizeof(cINP_BIG), cINP_SUB4, sizeof(cINP_SUB4), &offs);
	if (! fnd || offs != 0) {
		TEST_FCOM__prf(cFNCN, "error: st_sysBufSearch() failed (sub4)");
		return ST_B_FALSE;
	}

	//
	TEST_FCOM__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysStrlen()
 *       st_sysStrcpy()
 *       st_sysStrapp()
 *       st_sysStrToUpper()
 *       st_sysStrToLower()
 *       st_sysStrcmp()
 *       st_sysStrcmpN()
 */
Tst_bool
TEST_SYSFNC__5_strings(void)
{
	const char *cFNCN = __func__;
	Tst_uint32 resI;
	Tst_err resE;
	Tst_bool resB;
	Tst_str* pTemp = NULL;

	//
	resI = st_sysStrlen((const Tst_str*)"abcd");
	if (resI != 4) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrlen() failed (t1)");
		return ST_B_FALSE;
	}

	//
	resI = st_sysStrlen(NULL);
	if (resI != 0) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrlen() failed (t2)");
		return ST_B_FALSE;
	}

	//
	resE = st_sysStrcpy((const Tst_str*)"abcd", &pTemp);
	if (resE != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcpy() failed (t3-a)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmp(ST_B_TRUE, (const Tst_str*)"abcd", pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmp() failed (t3-b)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmp(ST_B_FALSE, (const Tst_str*)"aBcD", pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmp() failed (t3-c)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmp(ST_B_FALSE, (const Tst_str*)"aBcDe", pTemp);
	if (resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmp() failed (t3-d)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmpN(ST_B_FALSE, 2, (const Tst_str*)"aB", pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmpN() failed (t3-e)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmpN(ST_B_FALSE, 2, pTemp, (const Tst_str*)"aB");
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmpN() failed (t3-f)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmpN(ST_B_FALSE, st_sysStrlen(pTemp), pTemp, (const Tst_str*)"aBcDeF");
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmpN() failed (t3-g)");
		return ST_B_FALSE;
	}

	//
	resE = st_sysStrcpy(NULL, &pTemp);
	if (resE != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcpy() failed (t4-a)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmp(ST_B_TRUE, (const Tst_str*)"", pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmp() failed (t4-b)");
		return ST_B_FALSE;
	}
	resI = st_sysStrlen(pTemp);
	if (resI != 0) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrlen() failed (t4-c)");
		return ST_B_FALSE;
	}

	//
	resE = st_sysStrcpy((const Tst_str*)"12345678", &pTemp);
	if (resE != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcpy() failed (t5-a)");
		return ST_B_FALSE;
	}
	resE = st_sysStrapp((const Tst_str*)"12345678", &pTemp);
	if (resE != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrapp() failed (t5-b)");
		return ST_B_FALSE;
	}
	resE = st_sysStrapp(NULL, &pTemp);
	if (resE != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrapp() failed (t5-c)");
		return ST_B_FALSE;
	}
	resB = st_sysStrcmp(ST_B_TRUE, (const Tst_str*)"1234567812345678", pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmp() failed (t5-d)");
		return ST_B_FALSE;
	}

	// expect fail
	resB = st_sysStrcmp(ST_B_TRUE, NULL, NULL);
	if (resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcmp() failed (t6)");
		return ST_B_FALSE;
	}

	//
	resE = st_sysStrcpy((const Tst_str*)"abcdEFGH", &pTemp);
	if (resE != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrcpy() failed (t7-a)");
		return ST_B_FALSE;
	}
	st_sysStrToUpper(pTemp);
	if (! st_sysStrcmp(ST_B_TRUE, (const Tst_str*)"ABCDEFGH", pTemp)) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrToUpper() failed (t7-b)");
		return ST_B_FALSE;
	}
	st_sysStrToLower(pTemp);
	if (! st_sysStrcmp(ST_B_TRUE, (const Tst_str*)"abcdefgh", pTemp)) {
		TEST_FCOM__prf(cFNCN, "error: st_sysStrToUpper() failed (t7-c)");
		return ST_B_FALSE;
	}

	//
	TEST_FCOM__prf(cFNCN, "OK");

	return ST_B_TRUE;
}

/******************************************************************************/
