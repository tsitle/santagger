/******************************************************************************/
/* test_sysfile.c           [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for system functions related to files                        */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_file.h"
#include "fncs_test_common.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), getenv() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool TEST_SYSFILE__0_base(void);
Tst_bool TEST_SYSFILE__1_base(void);
Tst_bool TEST_SYSFILE__2_concat(void);
Tst_bool TEST_SYSFILE__3_concat(void);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	if (! TEST_SYSFILE__0_base()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFILE__1_base()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFILE__2_concat()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	if (! TEST_SYSFILE__3_concat()) {
		printf("! Test failed !\n");
		return 1;
	}
	printf("\n");

	printf("TEST_SYSFILE -- All tests passed :-)\n");
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysFileBasename()
 *       st_sysDirname()
 */
Tst_bool
TEST_SYSFILE__0_base(void)
{
	const char *cFNCN = __func__;
	#if defined(_WIN32) || defined (__CYGWIN__)
		const Tst_str *cDEL = (const Tst_str*)"\\";
		const Tst_str *cDN = (const Tst_str*)"albu\\quer\\que";
		const Tst_str *cFN = (const Tst_str*)"santa.fe";
	#else
		const Tst_str *cDEL = (const Tst_str*)"/";
		const Tst_str *cDN = (const Tst_str*)"albu/quer/que";
		const Tst_str *cFN = (const Tst_str*)"santa.fe";
	#endif
	Tst_bool resB;
	Tst_str *pTemp = NULL;
	Tst_str *pFull = NULL;

	st_sysStrcpy(cDN, &pFull);
	st_sysStrapp(cDEL, &pFull);
	st_sysStrapp(cFN, &pFull);

	resB = st_sysFileBasename(pFull, &pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysFileBasename() failed");
		return ST_B_FALSE;
	}

	resB = st_sysStrcmp(ST_B_TRUE, pTemp, cFN);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: base filename does not match: is '%s' != exp '%s'", pTemp, cFN);
		return ST_B_FALSE;
	}

	//
	resB = st_sysDirname(pFull, &pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysDirname() failed");
		return ST_B_FALSE;
	}

	resB = st_sysStrcmp(ST_B_TRUE, pTemp, cDN);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: dirname does not match: is '%s' != exp '%s'", pTemp, cDN);
		return ST_B_FALSE;
	}

	TEST_FCOM__prf(cFNCN, "OK");

	ST_DELPOINT(pFull)
	ST_DELPOINT(pTemp)

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysFileBasename()
 *       st_sysDirname()
 */
Tst_bool
TEST_SYSFILE__1_base(void)
{
	const char *cFNCN = __func__;
	const Tst_str *cFN = (const Tst_str*)"santa.fe";
	Tst_bool resB;
	Tst_str *pTemp = NULL;
	Tst_str *pFull = NULL;

	st_sysStrapp(cFN, &pFull);

	resB = st_sysFileBasename(pFull, &pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysFileBasename() failed");
		return ST_B_FALSE;
	}

	resB = st_sysStrcmp(ST_B_TRUE, pTemp, cFN);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: base filename does not match: is '%s' != exp '%s'", pTemp, cFN);
		return ST_B_FALSE;
	}

	//
	resB = st_sysDirname(pFull, &pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysDirname() failed");
		return ST_B_FALSE;
	}

	resB = st_sysStrcmp(ST_B_TRUE, pTemp, (const Tst_str*)".");
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: dirname does not match: is '%s' != exp '%s'", pTemp, (const Tst_str*)".");
		return ST_B_FALSE;
	}

	TEST_FCOM__prf(cFNCN, "OK");

	ST_DELPOINT(pFull)
	ST_DELPOINT(pTemp)

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysConcatDirAndFilen()
 */
Tst_bool
TEST_SYSFILE__2_concat(void)
{
	const char *cFNCN = __func__;
	#if defined(_WIN32) || defined (__CYGWIN__)
		const Tst_str *cDEL = (const Tst_str*)"\\";
	#else
		const Tst_str *cDEL = (const Tst_str*)"/";
	#endif
	const Tst_str *cDN = (const Tst_str*)"abra";
	const Tst_str *cFN = (const Tst_str*)"santa.fe";
	Tst_bool resB;
	Tst_str *pTemp = NULL;
	Tst_str *pFull = NULL;

	st_sysStrapp(cDN, &pFull);
	st_sysStrapp(cDEL, &pFull);
	st_sysStrapp(cFN, &pFull);

	resB = st_sysConcatDirAndFilen(cDN, cFN, &pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysConcatDirAndFilen() failed");
		return ST_B_FALSE;
	}

	resB = st_sysStrcmp(ST_B_TRUE, pTemp, pFull);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: path does not match: is '%s' != exp '%s'", pTemp, pFull);
		return ST_B_FALSE;
	}

	TEST_FCOM__prf(cFNCN, "OK");

	ST_DELPOINT(pFull)
	ST_DELPOINT(pTemp)

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/**
 * Tests st_sysConcatDirAndFilen()
 */
Tst_bool
TEST_SYSFILE__3_concat(void)
{
	const char *cFNCN = __func__;
	#if defined(_WIN32) || defined (__CYGWIN__)
		const Tst_str *cDEL = (const Tst_str*)"\\";
	#else
		const Tst_str *cDEL = (const Tst_str*)"/";
	#endif
	const Tst_str *cDN1 = (const Tst_str*)"abra";
	const Tst_str *cDN2 = (const Tst_str*)"hadabra";
	const Tst_str *cFN = (const Tst_str*)"santa.fe";
	Tst_bool resB;
	Tst_str *pInpDn = NULL;
	Tst_str *pInpFn = NULL;
	Tst_str *pTemp = NULL;
	Tst_str *pFull = NULL;

	st_sysStrapp(cDN1, &pInpDn);
	st_sysStrapp(cDEL, &pInpDn);
	st_sysStrapp(cDN2, &pInpDn);

	st_sysStrapp(cDN1, &pInpFn);
	st_sysStrapp(cDEL, &pInpFn);
	st_sysStrapp(cDN2, &pInpFn);
	st_sysStrapp(cDEL, &pInpFn);
	st_sysStrapp(cFN, &pInpFn);

	st_sysStrapp(cDN1, &pFull);
	st_sysStrapp(cDEL, &pFull);
	st_sysStrapp(cDN2, &pFull);
	st_sysStrapp(cDEL, &pFull);
	st_sysStrapp(cFN, &pFull);

	resB = st_sysConcatDirAndFilen(pInpDn, pInpFn, &pTemp);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: st_sysConcatDirAndFilen() failed");
		return ST_B_FALSE;
	}

	resB = st_sysStrcmp(ST_B_TRUE, pTemp, pFull);
	if (! resB) {
		TEST_FCOM__prf(cFNCN, "error: path does not match: is '%s' != exp '%s'", pTemp, pFull);
		return ST_B_FALSE;
	}

	TEST_FCOM__prf(cFNCN, "OK");

	ST_DELPOINT(pFull)
	ST_DELPOINT(pTemp)

	return ST_B_TRUE;
}

/******************************************************************************/
