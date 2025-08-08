/******************************************************************************/
/* test_mtes.c              [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for Multi-Encoding Strings                                   */
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
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "fncs_test_common.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), getenv() */
//#include <string.h>       /* memset() */
#include <stdarg.h>       /* va_list, ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool TEST_MTES__0_cmp(void);
Tst_bool TEST_MTES__1_sort(void);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	#define LOC_RUNTEST0  1
	#define LOC_RUNTEST1  1

	#if (LOC_RUNTEST0 == 1)
		if (! TEST_MTES__0_cmp()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (LOC_RUNTEST1 == 1)
		if (! TEST_MTES__1_sort()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	printf("TEST_MTES -- All tests passed :-)\n");
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
TEST_MTES__prfStr(const char *pFnc, const Tst_mtes_string *pStr,
		const char *pMsg, ...)
{
	Tst_mtes_string tmp;
	va_list args;

	st_mtes_stc_initStr(&tmp);
	st_mtes_convStr_c(pStr, &tmp, ST_MTES_TE_UTF8);
	printf("%s(): ", pFnc);
	va_start(args, pMsg);
	vprintf(pMsg, args);
	va_end(args);
	printf("  ' %s '  (te=%d)\n",
			(const char*)st_mtes_getDataPt(&tmp), st_mtes_getEncodingCur(pStr));
	st_mtes_stc_freeStr(&tmp);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests st_mtes_copyFromCharp_iso/utf8()
 *       st_mtes_strCompare()
 *       st_mtes_strlenBy()
 *       st_mtes_strlenCh()
 */
Tst_bool
TEST_MTES__0_cmp_sub(const char *pFncName,
		const Tst_mtes_textEnc enc1, const char *pCh1,
		const Tst_mtes_textEnc enc2, const char *pCh2,
		const Tst_uint32 len1by, const Tst_uint32 len1ch,
		const Tst_uint32 len2by, const Tst_uint32 len2ch,
		const Tst_int32 relat)
{
	Tst_err         res;
	Tst_int32       rel = 0;
	Tst_uint32      len;
	Tst_mtes_string str1,
	                str2;

	st_mtes_stc_initStr(&str1);
	if (enc1 == ST_MTES_TE_ISO) {
		res = st_mtes_copyFromCharp_iso((Tst_str*)pCh1, &str1);
	} else {
		res = st_mtes_copyFromCharp_utf8((Tst_str*)pCh1, &str1);
	}
	if (res == ST_ERR_SUCC) {
		st_mtes_stc_initStr(&str2);
		if (enc2 == ST_MTES_TE_ISO) {
			res = st_mtes_copyFromCharp_iso((Tst_str*)pCh2, &str2);
		} else {
			res = st_mtes_copyFromCharp_utf8((Tst_str*)pCh2, &str2);
		}
	}
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(pFncName, "ERROR");
		return ST_B_FALSE;
	}
	TEST_FCOM__prf(pFncName, "Compare: '%s'", pCh1);
	TEST_FCOM__prf(pFncName, "         '%s'", pCh2);
	res = st_mtes_strCompare(&str1, &str2, ST_B_TRUE, &rel);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(pFncName, "ERROR");
		return ST_B_FALSE;
	}
	if (relat != rel) {
		TEST_FCOM__prf(pFncName, "relat %d != %d", rel, relat);
		return ST_B_FALSE;
	}
	len = st_mtes_strlenBy(&str1);
	if (len != len1by) {
		TEST_FCOM__prf(pFncName, "len1by %u != %u", len, len1by);
		return ST_B_FALSE;
	}
	len = st_mtes_strlenCh(&str1);
	if (len != len1ch) {
		TEST_FCOM__prf(pFncName, "len1ch %u != %u", len, len1ch);
		return ST_B_FALSE;
	}
	len = st_mtes_strlenBy(&str2);
	if (len != len2by) {
		TEST_FCOM__prf(pFncName, "len2by %u != %u", len, len2by);
		return ST_B_FALSE;
	}
	len = st_mtes_strlenCh(&str2);
	if (len != len2ch) {
		TEST_FCOM__prf(pFncName, "len2ch %u != %u", len, len2ch);
		return ST_B_FALSE;
	}
	TEST_FCOM__prf(pFncName, "Result:  '%s'", pCh1);
	TEST_FCOM__prf(pFncName, "        %c'%s'",
			(relat == 0 ? '=' : (relat < 0 ? '<' : '>')), pCh2);
	TEST_FCOM__prf(pFncName, "Correct");

	st_mtes_stc_freeStr(&str1);
	st_mtes_stc_freeStr(&str2);
	return ST_B_TRUE;
}

Tst_bool
TEST_MTES__0_cmp(void)
{
	const char *cFNC = __func__;
	Tst_str chb1[10],
	        chb2[10];

	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "abcd",
			ST_MTES_TE_ISO, "abcde",
			4, 4, 5, 5, -1)) { return ST_B_FALSE; }
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "abcdd",
			ST_MTES_TE_ISO, "abcde",
			5, 5, 5, 5, -1)) { return ST_B_FALSE; }
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "abcdE",
			ST_MTES_TE_ISO, "abcdD",
			5, 5, 5, 5, 1)) { return ST_B_FALSE; }
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "abcdef",
			ST_MTES_TE_ISO, "abcd",
			6, 6, 4, 4, 1)) { return ST_B_FALSE; }
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "abcd @@@",
			ST_MTES_TE_ISO, "abcd @@@",
			8, 8, 8, 8, 0)) { return ST_B_FALSE; }
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "09_kkk",
			ST_MTES_TE_ISO, "09_Abc",
			6, 6, 6, 6, 1)) { return ST_B_FALSE; }
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "A",
			ST_MTES_TE_ISO, "b",
			1, 1, 1, 1, -1)) { return ST_B_FALSE; }
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_ISO, "AB c",
			ST_MTES_TE_ISO, "AB C",
			4, 4, 4, 4, 1)) { return ST_B_FALSE; }

	/* C3 84 C3 A4 │ C3 96 C3 B6 │ C3 BC C3 9C │ C3 B8 C3 98 */
	chb1[0] = 0xc3;
	chb1[1] = 0x84;
	chb1[2] = 0x00;
	chb2[0] = 0xc3;
	chb2[1] = 0xa4;
	chb2[2] = 0x00;
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_UTF8, (char*)chb1,
			ST_MTES_TE_UTF8, (char*)chb2,
			2, 1, 2, 1, -1)) { return ST_B_FALSE; }
	chb1[0] = 0xc3;
	chb1[1] = 0x96;
	chb1[2] = 'b';
	chb1[3] = 0xc3;
	chb1[4] = 0xbc;
	chb1[5] = 0x00;

	chb2[0] = 0xc3;
	chb2[1] = 0xb8;
	chb2[2] = 0x00;
	if (! TEST_MTES__0_cmp_sub(cFNC, ST_MTES_TE_UTF8, (char*)chb1,
			ST_MTES_TE_UTF8, (char*)chb2,
			5, 3, 2, 1, -1)) { return ST_B_FALSE; }
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

Tst_bool
TEST_MTES__1_sort(void)
{
	const char *cFNC = __func__;
	const Tst_uint32 cLEN         = 10,
	                 cCNT         = 50;
	const Tst_bool   showUnsorted = ST_B_FALSE,
	                 showSorted   = ST_B_TRUE,
	                 useRestrCS   = ST_B_FALSE,
	                 ascOrDesc    = ST_B_TRUE,
	                 casesens     = ST_B_TRUE;
	const Tst_str cCHARSET[] = "123";
	Tst_err       res;
	Tst_uint32    x,
	              y;
	double        timeB;
	Tst_str const *pCS = NULL;
	Tst_mtes_string  str;
	Tst_mtes_strArr  sArr;
	Tst_mtes_textEnc te;

	st_mtes_stc_initSA(&sArr);
	TEST_FCOM__prf(cFNC, "Generate strings...  (cnt=%u, len=%u)",
			cCNT, cLEN);
	st_mtes_stc_initStr(&str);

	if (useRestrCS) {
		// ReSharper disable once CppDFAUnreachableCode
		pCS = cCHARSET;
	}

	for (x = 0; x < cCNT; x++) {
		for (y = 0; y <= 3; y++) {
			switch (y) {
				case 0: te = ST_MTES_TE_ISO; break;
				case 1: te = ST_MTES_TE_UTF8; break;
				case 2: te = ST_MTES_TE_UTF16LE; break;
				default: te = ST_MTES_TE_UTF16BE;
			}
			if (! st_mtes_generateString(&str, te, cLEN, pCS)) {
				TEST_FCOM__prf(cFNC, "ERROR");
				break;
			}
			if (showUnsorted) {
				// ReSharper disable once CppDFAUnreachableCode
				TEST_MTES__prfStr(cFNC, &str, "gen");
			}
			if (st_mtes_strlenCh(&str) != cLEN) {
				TEST_FCOM__prf(cFNC, "ERROR");
				break;
			}
			res = st_mtes_strArrAdd(&sArr, &str);
			if (res != ST_ERR_SUCC) {
				TEST_FCOM__prf(cFNC, "ERROR");
				break;
			}
		}
	}
	st_mtes_stc_freeStr(&str);

	TEST_FCOM__prf(cFNC, "Sort strings...");
	timeB = st_sysGetTime();
	res = st_mtes_strArrSort(&sArr, ascOrDesc, casesens);
	if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(cFNC, "ERROR");
		st_mtes_stc_freeSA(&sArr);
		return ST_B_FALSE;
	}
	TEST_FCOM__prf(cFNC, "time: %.2f secs", st_sysGetTime() - timeB);
	if (showSorted) {
		for (x = 1; x <= cCNT; x++) {
			TEST_MTES__prfStr(cFNC, st_mtes_strArrGetElem(&sArr, x),
					"srt [%02x%02x]",
					st_mtes_getDataPt(st_mtes_strArrGetElem(&sArr, x))[0],
					st_mtes_getDataPt(st_mtes_strArrGetElem(&sArr, x))[1]);
		}
	}

	st_mtes_stc_freeSA(&sArr);
	return ST_B_TRUE;
}

/******************************************************************************/
