/******************************************************************************/
/* test_tfldmap.c           [Sandy Tagger | Testsuite]                   tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Test routines for mapping tag fields                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 14.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#include "src/includes/common/binobj.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_vorbc.h"
#include "src/includes/tag/tag_fldmap.h"
#include "fncs_test_common.h"

/*
// System-Includes
*/
#include <stdlib.h>       /* exit(), calloc(), getenv() */
#include <string.h>       /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define RUN_TEST_ALL  1
/* */
#define RUN_TEST_00  0
#define RUN_TEST_01  0
#define RUN_TEST_02  0
#define RUN_TEST_03  0
#define RUN_TEST_04  0
#define RUN_TEST_05  1
#define RUN_TEST_06  0
#define RUN_TEST_07  0
#define RUN_TEST_08  0
#define RUN_TEST_11  0
#define RUN_TEST_12  0
#define RUN_TEST_13  0

Tst_bool TEST_TFLDMAP__00_mapIV2_id(void);
Tst_bool TEST_TFLDMAP__01_mapIV2_idstr(void);
Tst_bool TEST_TFLDMAP__02_mapIV2_cust(void);
Tst_bool TEST_TFLDMAP__03_mapIV2_fromVor(void);
Tst_bool TEST_TFLDMAP__04_mapIV2_fromAv2(void);
Tst_bool TEST_TFLDMAP__05_mapIV2_fromIv1(void);
Tst_bool TEST_TFLDMAP__06_mapAV2_id(void);
Tst_bool TEST_TFLDMAP__07_mapAV2_cust(void);
Tst_bool TEST_TFLDMAP__08_mapAV2_fromIv2(void);
Tst_bool TEST_TFLDMAP__11_mapVOR_id(void);
Tst_bool TEST_TFLDMAP__12_mapVOR_cust(void);
Tst_bool TEST_TFLDMAP__13_mapVOR_fromIv2(void);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int
main(const int argc, const char *argv[])
{
	#if (RUN_TEST_ALL == 1 || RUN_TEST_00 == 1)
		if (! TEST_TFLDMAP__00_mapIV2_id()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_01 == 1)
		if (! TEST_TFLDMAP__01_mapIV2_idstr()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_02 == 1)
		if (! TEST_TFLDMAP__02_mapIV2_cust()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_03 == 1)
		if (! TEST_TFLDMAP__03_mapIV2_fromVor()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_04 == 1)
		if (! TEST_TFLDMAP__04_mapIV2_fromAv2()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_05 == 1)
		if (! TEST_TFLDMAP__05_mapIV2_fromIv1()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_06 == 1)
		if (! TEST_TFLDMAP__06_mapAV2_id()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_07 == 1)
		if (! TEST_TFLDMAP__07_mapAV2_cust()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_08 == 1)
		if (! TEST_TFLDMAP__08_mapAV2_fromIv2()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_11 == 1)
		if (! TEST_TFLDMAP__11_mapVOR_id()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_12 == 1)
		if (! TEST_TFLDMAP__12_mapVOR_cust()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	#if (RUN_TEST_ALL == 1 || RUN_TEST_13 == 1)
		if (! TEST_TFLDMAP__13_mapVOR_fromIv2()) {
			printf("! Test failed !\n");
			return 1;
		}
		printf("\n");
	#endif

	printf("TEST_TFLDMAP -- All tests passed :-)\n");
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set basics: Mode, FldNr, ID/ID-String
 */
static Tst_bool
TEST_TFLDMAP__map_setBasics(Tst_tfldMap_genTagFld *pGTF,
		const char *pTagN, const Tst_bool modeAoS, const Tst_int32 fnr,
		const Tst_bool useIDorIDStr, const Tst_int32 fid, const Tst_str *pFIDstr)
{
	Tst_err res;

	st_tagFldMap_stc_rsetGTF(pGTF);
	res = st_tagFldMap_gs_setMode(pGTF, /*addOrSet:*/modeAoS);
	if (res == ST_ERR_SUCC) {
		/* should fail: invalid tag type */
		res = st_tagFldMap_gs_setTagType(pGTF, (const Tst_str*)"bogus");
		if (res != ST_ERR_SUCC) {
			/* shouldn't fail */
			res = st_tagFldMap_gs_setTagType(pGTF, (const Tst_str*)pTagN);
		}
	}
	if (res == ST_ERR_SUCC) {
		if (useIDorIDStr) {
			res = st_tagFldMap_gs_setFldID(pGTF, fid);
		} else {
			res = st_tagFldMap_gs_setFldIDstr(pGTF, pFIDstr);
		}
	}
	if (res == ST_ERR_SUCC) {
		res = st_tagFldMap_gs_setFldNr(pGTF, fnr);
	}
	return (res == ST_ERR_SUCC);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
TEST_TFLDMAP__mapIV2_mapAndCheckBasics(const char *pFnc, Tst_tfldMap_genTagFld *pGTF,
		const Tst_bool expModeAoS, const Tst_int32 expFNr,
		const Tst_id3v2_frID expFID, const Tst_str *pExpFIDstr,
		const Tst_bool expIsOK,
		Tst_id3v2_fldData *pFData, Tst_id3v2_fldProp *pFProps)
{
	Tst_err   res;
	Tst_bool  isOK = ST_B_FALSE,
	          mode = ST_B_FALSE;
	Tst_int32 fnr  = -1;

	res = st_tagFldMap_mapToIV2(pGTF, pFData, &mode, &fnr, &isOK);
	if (res == ST_ERR_SUCC && ! isOK && expIsOK) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [isOK mismatch: exp T]");
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && mode != expModeAoS) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [mode mismatch: exp %c]",
				(expModeAoS ? 'A' : 'S'));
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && expModeAoS && fnr != -1) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [fnr mismatch: exp %d, got %d]",
				-1, fnr);
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && ! expModeAoS && fnr != expFNr) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [fnr mismatch: exp %d, got %d]",
				expFNr, fnr);
		res = ST_ERR_FAIL;
	} else if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(pFnc, "error: st_tagFldMap_mapToIV2() failed");
		res = ST_ERR_FAIL;
	}

	if (res == ST_ERR_SUCC && isOK) {
		res = st_id3v2_gs_getField_props(pFData, pFProps);
		if (res == ST_ERR_SUCC) {
			Tst_id3v2_frID fid;

			fid = st_id3v2_gs_getFieldProp_id(pFProps);
			if (expFID != -1 && fid != expFID) {
				TEST_FCOM__prf(pFnc, "error: couldn't map field "
						"[ID mismatch: exp %d, got %d]",
						(int)expFID, (int)fid);
				res = ST_ERR_FAIL;
			} else {
				Tst_str const *pIDstr;

				pIDstr = st_id3v2_gs_getFieldProp_idStr(pFProps);
				if (pIDstr == NULL) {
					TEST_FCOM__prf(pFnc, "error: couldn't map field [ID-String empty]");
					res = ST_ERR_FAIL;
				}
				if (res == ST_ERR_SUCC && pExpFIDstr != NULL &&
						strcasecmp((const char*)pIDstr, (const char*)pExpFIDstr) != 0) {
					TEST_FCOM__prf(pFnc, "error: couldn't map field "
							"[ID-String mismatch: '%s' != '%s']",
							pIDstr, pExpFIDstr);
					res = ST_ERR_FAIL;
				}
				if (res == ST_ERR_SUCC) {
					TEST_FCOM__prf(pFnc, " [ID=%3d,  IDstr='%s',  mode=%c,  fnr=%2d]",
							fid, (pIDstr == NULL ? "" : (char const*)pIDstr),
							(mode ? 'A' : 'S'), fnr);
				}
			}
		} else {
			TEST_FCOM__prf(pFnc, "error: st_id3v2_gs_getField_props() failed");
		}
	}
	return (res == ST_ERR_SUCC);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__00_mapIV2_id(void)
{
	const char *cFNCN = __func__;
	Tst_bool  bres;
	Tst_err   res;
	Tst_int32 fid;
	Tst_tfldMap_genTagFld gtf;
	Tst_id3v2_fldData     fdata;
	Tst_id3v2_fldProp     fprops;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping ID3v2 Tag Fields to ID3v2 Tag Fields by ID...");

	st_id3v2_stc_rsetFPr(&fprops);
	res  = st_id3v2_stc_initFDat(&fdata);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (fid = 0; fid <= (Tst_int32)ST_ID3V2_FID_NONE; fid++) {
		const Tst_int32 fnr = -1;

		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_ID3V2_TAG_NAME_SH, /*modeAoS:*/(fid % 2 == 0),
				fnr, /*useIDorIDStr:*/ST_B_TRUE, (Tst_id3v2_frID)fid, NULL);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapIV2_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_int(&gtf, 100);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_int() failed");
			break;
		}

		if (fid != (Tst_int32)ST_ID3V2_FID_NONE &&
				fid != (Tst_int32)ST_ID3V2_FID_234_CTXT &&
				fid != (Tst_int32)ST_ID3V2_FID_234_CURL) {
			/* shouldn't fail */
			bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
					/*expModeAoS:*/(fid % 2 == 0), fnr, (Tst_id3v2_frID)fid, NULL,
					/*expIsOK:*/ST_B_TRUE,
					&fdata, &fprops);
		} else if (fid == (Tst_int32)ST_ID3V2_FID_234_CTXT ||
					fid == (Tst_int32)ST_ID3V2_FID_234_CURL ||
					// ReSharper disable once CppDFAConstantConditions
					fid == (Tst_int32)ST_ID3V2_FID_NONE) {
			/* shouldn't fail, but get mapped to COMM */
			bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
					/*expModeAoS:*/(fid % 2 == 0), fnr, ST_ID3V2_FID_234_COMM, NULL,
					/*expIsOK:*/ST_B_FALSE,
					&fdata, &fprops);
		}
		if (! bres) {
			break;
		}
	}
	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_id3v2_stc_freeFDat(&fdata);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__01_mapIV2_idstr(void)
{
	const char *cFNCN = __func__;
	Tst_bool      bres;
	Tst_err       res;
	Tst_str const *pFIDstr;
	Tst_tfldMap_genTagFld gtf;
	Tst_id3v2_fldData     fdata;
	Tst_id3v2_fldProp     fprops,
	                      *pIteLast = NULL,
	                      iteCur;
	Tst_id3v2_frID        fid;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping ID3v2 Tag Fields to ID3v2 Tag Fields by ID-String...");

	st_id3v2_stc_rsetFPr(&iteCur);
	st_id3v2_stc_rsetFPr(&fprops);
	res  = st_id3v2_stc_initFDat(&fdata);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	while (st_id3v2_ite_nextTagFldProp(pIteLast, &iteCur) != NULL) {
		const Tst_int32 fnr = 1;

		pIteLast = &iteCur;
		fid      = st_id3v2_gs_getFieldProp_id(&iteCur);
		pFIDstr  = st_id3v2_gs_getFieldProp_idStr(&iteCur);
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_ID3V2_TAG_NAME_SH, /*modeAoS:*/(fid % 2 == 0),
				fnr, /*useIDorIDStr:*/ST_B_FALSE, ST_ID3V2_FID_NONE,
				pFIDstr);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__map_setBasics() failed");
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapIV2_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_pos(&gtf, 100, 200);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_pos() failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, pFIDstr,
				/*expIsOK:*/ST_B_TRUE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapIV2_mapAndCheckBasics() failed");
			break;
		}
	}
	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_id3v2_stc_freeFDat(&fdata);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__02_mapIV2_cust(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_int32  fnr = 1;
	Tst_uint32 x;
	char const *pFIDstrIn,
	           *pFIDstrOut;
	Tst_tfldMap_genTagFld gtf;
	Tst_id3v2_fldData     fdata;
	Tst_id3v2_fldProp     fprops;
	Tst_id3v2_frID        fid;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping ID3v2 Tag Fields to (custom) ID3v2 Tag Fields...");

	st_id3v2_stc_rsetFPr(&fprops);
	res  = st_id3v2_stc_initFDat(&fdata);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (x = 0; x <= 11; x++) {
		switch (x) {
			case 0:
				pFIDstrIn = "ASPI";  pFIDstrOut = "ASPI";  fid = ST_ID3V2_FID___4_ASPI; break;
			case 1:
				pFIDstrIn = "BUF";   pFIDstrOut = "RBUF";  fid = ST_ID3V2_FID_234_RBUF; break;
			case 2:
				pFIDstrIn = "CDM ";  pFIDstrOut = "CDM";   fid = ST_ID3V2_FID_2___CDMF; break;
			case 3:
				pFIDstrIn = "CM1 ";  pFIDstrOut = "CM1";   fid = ST_ID3V2_FID_2___COM1; break;
			case 4:
				pFIDstrIn = "CRM ";  pFIDstrOut = "CRM";   fid = ST_ID3V2_FID_2___CRM1; break;
			case 5:
				pFIDstrIn = "ENCR";  pFIDstrOut = "ENCR";  fid = ST_ID3V2_FID__34_ENCR; break;
			case 6:
				pFIDstrIn = "TSA ";  pFIDstrOut = "TSOA";  fid = ST_ID3V2_FID_234_TSOA; break;
			case 7:
				pFIDstrIn = "TSOA";  pFIDstrOut = "TSOA";  fid = ST_ID3V2_FID_234_TSOA; break;
			case 8:
				pFIDstrIn = "TAAA";  pFIDstrOut = "TAAA";  fid = ST_ID3V2_FID_234_CTXT; break;
			case 9:
				pFIDstrIn = "T000";  pFIDstrOut = "T000";  fid = ST_ID3V2_FID_234_CTXT; break;
			case 10:
				pFIDstrIn = "WAAA";  pFIDstrOut = "WAAA";  fid = ST_ID3V2_FID_234_CURL; break;
			default:
				pFIDstrIn = "WZZ0";  pFIDstrOut = "WZZ0";  fid = ST_ID3V2_FID_234_CURL;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_ID3V2_TAG_NAME_SH, /*modeAoS:*/(fid % 2 == 0),
				fnr, /*useIDorIDStr:*/ST_B_FALSE, ST_ID3V2_FID_NONE,
				(Tst_str const*)pFIDstrIn);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapIV2_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_pos(&gtf, 100, -1);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_pos() failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, (Tst_str const*)pFIDstrOut,
				/*expIsOK:*/ST_B_TRUE,
				&fdata, &fprops);
		if (! bres) {
			break;
		}
	}
	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_id3v2_stc_freeFDat(&fdata);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__03_mapIV2_fromVor(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_uint32 x;
	char const *pFIDstrInVOR,
	           *pFIDstrOutIV2;
	Tst_tfldMap_genTagFld gtf;
	Tst_id3v2_fldData     fdataIV2;
	Tst_id3v2_fldProp     fpropsIV2;
	Tst_id3v2_frID        fidIV2;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping Vorbis Tag Fields to ID3v2 Tag Fields...");

	st_id3v2_stc_rsetFPr(&fpropsIV2);
	res  = st_id3v2_stc_initFDat(&fdataIV2);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (x = 0; x <= 6; x++) {
		switch (x) {
			case 0:
				pFIDstrInVOR = "TITLE";        pFIDstrOutIV2 = "TIT2";  fidIV2 = ST_ID3V2_FID_234_TIT2; break;
			case 1:
				pFIDstrInVOR = "TRACKNUMBER";  pFIDstrOutIV2 = "TRCK";  fidIV2 = ST_ID3V2_FID_234_TRCK; break;
			case 2:
				pFIDstrInVOR = "TRACKTOTAL";   pFIDstrOutIV2 = "trck";  fidIV2 = ST_ID3V2_FID_234_TRCK; break;
			case 3:
				pFIDstrInVOR = "DISCNUMBER";   pFIDstrOutIV2 = "TpoS";  fidIV2 = ST_ID3V2_FID_234_TPOS; break;
			case 4:
				pFIDstrInVOR = "DISCTOTAL";    pFIDstrOutIV2 = "tPOs";  fidIV2 = ST_ID3V2_FID_234_TPOS; break;
			case 5:
				pFIDstrInVOR = "METADATA_BLOCK_PICTURE";  pFIDstrOutIV2 = "apic";  fidIV2 = ST_ID3V2_FID_234_APIC; break;
			default:
				pFIDstrInVOR = "My own ID";    pFIDstrOutIV2 = "COMM";  fidIV2 = ST_ID3V2_FID_234_COMM;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_VORBC_TAG_NAME_SH, /*modeAoS:*/ST_B_TRUE,
				-1, /*useIDorIDStr:*/ST_B_FALSE, -1,
				(Tst_str const*)pFIDstrInVOR);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, -1, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdataIV2, &fpropsIV2);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapIV2_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_pos(&gtf, 0, 200);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_pos() failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, fidIV2, (const Tst_str*)pFIDstrOutIV2,
				/*expIsOK:*/ST_B_TRUE,
				&fdataIV2, &fpropsIV2);
		if (! bres) {
			break;
		}
	}

	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_id3v2_stc_freeFDat(&fdataIV2);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__04_mapIV2_fromAv2(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_uint32 x;
	char const *pFIDstrInAV2,
	           *pFIDstrOutIV2;
	Tst_tfldMap_genTagFld gtf;
	Tst_id3v2_fldData     fdataIV2;
	Tst_id3v2_fldProp     fpropsIV2;
	Tst_id3v2_frID        fidIV2;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping APEv2 Tag Fields to ID3v2 Tag Fields...");

	st_id3v2_stc_rsetFPr(&fpropsIV2);
	res  = st_id3v2_stc_initFDat(&fdataIV2);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (x = 0; x <= 5; x++) {
		switch (x) {
			case 0:
				pFIDstrInAV2 = "TITLE";       pFIDstrOutIV2 = "TIT2";  fidIV2 = ST_ID3V2_FID_234_TIT2; break;
			case 1:
				pFIDstrInAV2 = "TRACK";       pFIDstrOutIV2 = "TRCK";  fidIV2 = ST_ID3V2_FID_234_TRCK; break;
			case 2:
				pFIDstrInAV2 = "TRACKTOTAL";  pFIDstrOutIV2 = "trck";  fidIV2 = ST_ID3V2_FID_234_TRCK; break;
			case 3:
				pFIDstrInAV2 = "DISC";        pFIDstrOutIV2 = "TpoS";  fidIV2 = ST_ID3V2_FID_234_TPOS; break;
			case 4:
				pFIDstrInAV2 = "DISCTOTAL";   pFIDstrOutIV2 = "tPOs";  fidIV2 = ST_ID3V2_FID_234_TPOS; break;
			default:
				pFIDstrInAV2 = "My own ID";   pFIDstrOutIV2 = "COMM";  fidIV2 = ST_ID3V2_FID_234_COMM;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_APEV2_TAG_NAME_SH, /*modeAoS:*/ST_B_TRUE,
				-1, /*useIDorIDStr:*/ST_B_FALSE, -1,
				(Tst_str const*)pFIDstrInAV2);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, -1, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdataIV2, &fpropsIV2);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapIV2_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_pos(&gtf, 0, 200);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_pos() failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, fidIV2, (const Tst_str*)pFIDstrOutIV2,
				/*expIsOK:*/ST_B_TRUE,
				&fdataIV2, &fpropsIV2);
		if (! bres) {
			break;
		}
	}

	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_id3v2_stc_freeFDat(&fdataIV2);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__05_mapIV2_fromIv1(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_uint32 x;
	Tst_int32  fidInIV1;
	char const *pFIDstrOutIV2;
	Tst_tfldMap_genTagFld gtf;
	Tst_id3v2_fldData     fdataIV2;
	Tst_id3v2_fldProp     fpropsIV2;
	Tst_id3v2_frID        fidIV2;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping ID3v1 Tag Fields to ID3v2 Tag Fields...");

	st_id3v2_stc_rsetFPr(&fpropsIV2);
	res  = st_id3v2_stc_initFDat(&fdataIV2);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (x = 0; x <= 6; x++) {
		switch (x) {
			case 0:
				fidInIV1 = ST_ID3V1_FID_ARTI;  pFIDstrOutIV2 = "TPE1";  fidIV2 = ST_ID3V2_FID_234_TPE1; break;
			case 1:
				fidInIV1 = ST_ID3V1_FID_ALBU;  pFIDstrOutIV2 = "TALB";  fidIV2 = ST_ID3V2_FID_234_TALB; break;
			case 2:
				fidInIV1 = ST_ID3V1_FID_TITL;  pFIDstrOutIV2 = "TIT2";  fidIV2 = ST_ID3V2_FID_234_TIT2; break;
			case 3:
				fidInIV1 = ST_ID3V1_FID_TRKN;  pFIDstrOutIV2 = "TRCK";  fidIV2 = ST_ID3V2_FID_234_TRCK; break;
			case 4:
				fidInIV1 = ST_ID3V1_FID_YEAR;  pFIDstrOutIV2 = "TYER";  fidIV2 = ST_ID3V2_FID_23__TYER; break;
			case 5:
				fidInIV1 = ST_ID3V1_FID_COMM;  pFIDstrOutIV2 = "COMM";  fidIV2 = ST_ID3V2_FID_234_COMM; break;
			default:
				fidInIV1 = ST_ID3V1_FID_GENR;  pFIDstrOutIV2 = "TCON";  fidIV2 = ST_ID3V2_FID_234_TCON;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_ID3V1_TAG_NAME_SH, /*modeAoS:*/ST_B_TRUE,
				-1, /*useIDorIDStr:*/ST_B_TRUE, fidInIV1, NULL);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, -1, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdataIV2, &fpropsIV2);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapIV2_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_pos(&gtf, 0, 200);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_pos() failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapIV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, fidIV2, (const Tst_str*)pFIDstrOutIV2,
				/*expIsOK:*/ST_B_TRUE,
				&fdataIV2, &fpropsIV2);
		if (! bres) {
			break;
		}
	}

	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_id3v2_stc_freeFDat(&fdataIV2);
	return bres;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
TEST_TFLDMAP__mapAV2_mapAndCheckBasics(const char *pFnc, Tst_tfldMap_genTagFld *pGTF,
		const Tst_bool expModeAoS, const Tst_int32 expFNr,
		const Tst_apev2_frID expFID, const Tst_str *pExpFIDstr,
		const Tst_bool expIsOK,
		Tst_apev2_fldData *pFData, Tst_apev2_fldProp *pFProps)
{
	Tst_err   res;
	Tst_bool  isOK = ST_B_FALSE,
	          mode = ST_B_FALSE;
	Tst_int32 fnr  = -1;

	res = st_tagFldMap_mapToAV2(pGTF, pFData, &mode, &fnr, &isOK);
	if (res == ST_ERR_SUCC && ! isOK && expIsOK) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [isOK mismatch: exp T]");
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && mode != expModeAoS) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [mode mismatch: exp %c]",
				(expModeAoS ? 'A' : 'S'));
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && expModeAoS && fnr != -1) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [fnr mismatch: exp %d, got %d]",
				-1, fnr);
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && ! expModeAoS && fnr != expFNr) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [fnr mismatch: exp %d, got %d]",
				expFNr, fnr);
		res = ST_ERR_FAIL;
	} else if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(pFnc, "error: st_tagFldMap_mapToAV2() failed");
		res = ST_ERR_FAIL;
	}

	if (res == ST_ERR_SUCC && isOK) {
		res = st_apev2_gs_getField_props(pFData, pFProps);
		if (res == ST_ERR_SUCC) {
			Tst_apev2_frID fid;

			fid = st_apev2_gs_getFieldProp_id(pFProps);
			if (expFID != -1 && fid != expFID) {
				TEST_FCOM__prf(pFnc, "error: couldn't map field "
						"[ID mismatch: exp %d, got %d]",
						(int)expFID, (int)fid);
				res = ST_ERR_FAIL;
			} else {
				Tst_str const *pIDstr;

				pIDstr = st_apev2_gs_getFieldProp_idStr(pFProps);
				if (pIDstr == NULL) {
					TEST_FCOM__prf(pFnc, "error: couldn't map field [ID-String empty]");
					res = ST_ERR_FAIL;
				}
				if (res == ST_ERR_SUCC && pExpFIDstr != NULL &&
						strcasecmp((const char*)pIDstr, (const char*)pExpFIDstr) != 0) {
					TEST_FCOM__prf(pFnc, "error: couldn't map field "
							"[ID-String mismatch: '%s' != '%s']",
							pIDstr, pExpFIDstr);
					res = ST_ERR_FAIL;
				}
				if (res == ST_ERR_SUCC) {
					TEST_FCOM__prf(pFnc, " [ID=%3d,  IDstr='%s',  mode=%c,  fnr=%2d]",
							fid, (pIDstr == NULL ? "" : (char const*)pIDstr),
							(mode ? 'A' : 'S'), fnr);
				}
			}
		} else {
			TEST_FCOM__prf(pFnc, "error: st_apev2_gs_getField_props() failed");
		}
	}
	return (res == ST_ERR_SUCC);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__06_mapAV2_id(void)
{
	const char *cFNCN = __func__;
	Tst_bool  bres;
	Tst_err   res;
	Tst_int32 fid;
	Tst_tfldMap_genTagFld gtf;
	Tst_apev2_fldData     fdata;
	Tst_apev2_fldProp     fprops;
	Tst_buf               buf[4] = {'T', 'A', 'S', 0};
	Tst_binobj            bindat;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping APEv2 Tag Fields to APEv2 Tag Fields by ID...");

	st_binobj_stc_initBO(&bindat);
	st_apev2_stc_rsetFPr(&fprops);
	res  = st_apev2_stc_initFDat(&fdata);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	st_binobj_appendData(&bindat, buf, sizeof(buf));

	for (fid = 0; fid <= (Tst_int32)ST_APEV2_FID_NONE; fid++) {
		const Tst_int32 fnr = -1;

		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_APEV2_TAG_NAME_SH, /*modeAoS:*/(fid % 2 == 0),
				fnr, /*useIDorIDStr:*/ST_B_TRUE, (Tst_apev2_frID)fid, NULL);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapAV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "notice: TEST_TFLDMAP__mapAV2_mapAndCheckBasics() failed on purpose");
			/*TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapAV2_mapAndCheckBasics() failed");
			break;*/
		}

		if (fid == ST_APEV2_FID_CBIN) {
			res = st_tagFldMap_gs_setDataBinary(&gtf, &bindat);
		} else {
			res  = st_tagFldMap_gs_setDataStr_int(&gtf, 100);
		}
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_int() failed");
			break;
		}

		if (fid != (Tst_int32)ST_APEV2_FID_NONE && fid != (Tst_int32)ST_APEV2_FID_CTXT) {
			/* shouldn't fail */
			bres = TEST_TFLDMAP__mapAV2_mapAndCheckBasics(cFNCN, &gtf,
					/*expModeAoS:*/(fid % 2 == 0), fnr, (Tst_apev2_frID)fid, NULL,
					/*expIsOK:*/ST_B_TRUE,
					&fdata, &fprops);
		} else {
			/* shouldn't fail, but get mapped to COMM */
			bres = TEST_TFLDMAP__mapAV2_mapAndCheckBasics(cFNCN, &gtf,
					/*expModeAoS:*/(fid % 2 == 0), fnr, ST_APEV2_FID_COMM, NULL,
					/*expIsOK:*/ST_B_FALSE,
					&fdata, &fprops);
		}
		if (! bres) {
			break;
		}
	}
	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_apev2_stc_freeFDat(&fdata);
	st_binobj_stc_freeBO(&bindat);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__07_mapAV2_cust(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_int32  fnr = 1;
	Tst_uint32 x;
	char const *pFIDstrIn,
	           *pFIDstrOut;
	Tst_buf    buf[4] = {'T', 'A', 'S', 0};
	Tst_tfldMap_genTagFld gtf;
	Tst_apev2_fldData     fdata;
	Tst_apev2_fldProp     fprops;
	Tst_apev2_frID        fid;
	Tst_binobj            bindat;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping APEv2 Tag Fields to (custom) APEv2 Tag Fields...");

	st_binobj_stc_initBO(&bindat);
	st_apev2_stc_rsetFPr(&fprops);
	res  = st_apev2_stc_initFDat(&fdata);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	st_binobj_appendData(&bindat, buf, sizeof(buf));

	for (x = 0; x <= 3; x++) {
		switch (x) {
			case 0:
				pFIDstrIn = "my own txt";  pFIDstrOut = "MY OWN TXT";  fid = ST_APEV2_FID_CTXT; break;
			case 1:
				pFIDstrIn = "MY OWN bin";  pFIDstrOut = "MY OWN BIN";  fid = ST_APEV2_FID_CBIN; break;
			case 2:
				pFIDstrIn = "DISC";        pFIDstrOut = "DISC";        fid = ST_APEV2_FID_DSCN; break;
			default:
				pFIDstrIn = "DISCTOTAL";   pFIDstrOut = "DISCTOTAL";   fid = ST_APEV2_FID_DSCT;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_APEV2_TAG_NAME_SH, /*modeAoS:*/(fid % 2 == 0),
				fnr, /*useIDorIDStr:*/ST_B_FALSE, ST_APEV2_FID_NONE,
				(Tst_str const*)pFIDstrIn);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapAV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "notice: TEST_TFLDMAP__mapAV2_mapAndCheckBasics() failed on purpose");
			/*TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapAV2_mapAndCheckBasics() failed");
			break;*/
		}

		if (fid == ST_APEV2_FID_CBIN) {
			res = st_tagFldMap_gs_setDataBinary(&gtf, &bindat);
		} else {
			res = st_tagFldMap_gs_setDataStr_pos(&gtf, 100, -1);
		}
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: setting field data failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapAV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, (Tst_str const*)pFIDstrOut,
				/*expIsOK:*/ST_B_TRUE,
				&fdata, &fprops);
		if (! bres) {
			break;
		}
	}
	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_apev2_stc_freeFDat(&fdata);
	st_binobj_stc_freeBO(&bindat);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__08_mapAV2_fromIv2(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_uint32 x;
	char const *pFIDstrInIV2,
	           *pFIDstrOutAV2;
	Tst_buf    buf[4] = {'T', 'A', 'S', 0};
	Tst_tfldMap_genTagFld gtf;
	Tst_apev2_fldData     fdataAV2;
	Tst_apev2_fldProp     fpropsAV2;
	Tst_apev2_frID        fidAV2;
	Tst_binobj            bindat;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping ID3v2 Tag Fields to APEv2 Tag Fields...");

	st_binobj_stc_initBO(&bindat);
	st_apev2_stc_rsetFPr(&fpropsAV2);
	res  = st_apev2_stc_initFDat(&fdataAV2);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	st_binobj_appendData(&bindat, buf, sizeof(buf));

	for (x = 0; x <= 5; x++) {
		switch (x) {
			case 0:
				pFIDstrInIV2 = "TIT2";  pFIDstrOutAV2 = "TITLE";    fidAV2 = ST_APEV2_FID_TITL; break;
			case 1:
				pFIDstrInIV2 = "TRCK";  pFIDstrOutAV2 = "TRACK";    fidAV2 = ST_APEV2_FID_TRKN; break;
			case 2:
				pFIDstrInIV2 = "TPOS";  pFIDstrOutAV2 = "DISC";     fidAV2 = ST_APEV2_FID_DSCN; break;
			case 3:
				pFIDstrInIV2 = "GEOB";  pFIDstrOutAV2 = "GEOB";     fidAV2 = ST_APEV2_FID_CBIN; break;
			case 4:
				pFIDstrInIV2 = "W000";  pFIDstrOutAV2 = "W000";     fidAV2 = ST_APEV2_FID_CTXT; break;
			default:
				pFIDstrInIV2 = "COMM";  pFIDstrOutAV2 = "COMMENT";  fidAV2 = ST_APEV2_FID_COMM;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_ID3V2_TAG_NAME_SH, /*modeAoS:*/ST_B_TRUE,
				-1, /*useIDorIDStr:*/ST_B_FALSE, -1,
				(Tst_str const*)pFIDstrInIV2);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapAV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, -1, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdataAV2, &fpropsAV2);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapAV2_mapAndCheckBasics() failed");
			break;
		}

		if (fidAV2 == ST_APEV2_FID_CBIN) {
			res = st_tagFldMap_gs_setDataBinary(&gtf, &bindat);
		} else {
			res = st_tagFldMap_gs_setDataStr_pos(&gtf, 100, -1);
		}
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: setting field data failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapAV2_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, fidAV2, (const Tst_str*)pFIDstrOutAV2,
				/*expIsOK:*/ST_B_TRUE,
				&fdataAV2, &fpropsAV2);
		if (! bres) {
			break;
		}
	}

	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_apev2_stc_freeFDat(&fdataAV2);
	st_binobj_stc_freeBO(&bindat);
	return bres;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
TEST_TFLDMAP__mapVOR_mapAndCheckBasics(const char *pFnc, Tst_tfldMap_genTagFld *pGTF,
		const Tst_bool expModeAoS, const Tst_int32 expFNr,
		const Tst_vorbc_frID expFID, const Tst_str *pExpFIDstr,
		const Tst_bool expIsOK,
		Tst_vorbc_fldData *pFData, Tst_vorbc_fldProp *pFProps)
{
	Tst_err   res;
	Tst_bool  isOK = ST_B_FALSE,
	          mode = ST_B_FALSE;
	Tst_int32 fnr  = -1;
	Tst_vorbc_frID fid;

	res = st_tagFldMap_mapToVOR(pGTF, pFData, &mode, &fnr, &isOK);
	if (res == ST_ERR_SUCC && ! isOK && expIsOK) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [isOK mismatch: exp T]");
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && mode != expModeAoS) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [mode mismatch: exp %c]",
				(expModeAoS ? 'A' : 'S'));
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && expModeAoS && fnr != -1) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [fnr mismatch: exp %d, got %d]",
				-1, fnr);
		res = ST_ERR_FAIL;
	} else if (res == ST_ERR_SUCC && isOK && ! expModeAoS && fnr != expFNr) {
		TEST_FCOM__prf(pFnc, "error: couldn't map field [fnr mismatch: exp %d, got %d]",
				expFNr, fnr);
		res = ST_ERR_FAIL;
	} else if (res != ST_ERR_SUCC) {
		TEST_FCOM__prf(pFnc, "error: st_tagFldMap_mapToVOR() failed");
		res = ST_ERR_FAIL;
	}

	if (res == ST_ERR_SUCC && isOK) {
		res = st_vorbc_gs_getField_props(pFData, pFProps);
		if (res == ST_ERR_SUCC) {
			Tst_str const *pIDstr;

			fid = st_vorbc_gs_getFieldProp_id(pFProps);
			if (expFID != -1 && fid != expFID) {
				TEST_FCOM__prf(pFnc, "error: couldn't map field "
						"[ID mismatch: exp %d, got %d]",
						(int)expFID, (int)fid);
				res = ST_ERR_FAIL;
			} else {
				pIDstr = st_vorbc_gs_getFieldProp_idStr(pFProps);
				if (pIDstr == NULL) {
					TEST_FCOM__prf(pFnc, "error: couldn't map field [ID-String empty]");
					res = ST_ERR_FAIL;
				}
				if (res == ST_ERR_SUCC && pExpFIDstr != NULL &&
						strcasecmp((const char*)pIDstr, (const char*)pExpFIDstr) != 0) {
					TEST_FCOM__prf(pFnc, "error: couldn't map field "
							"[ID-String mismatch: '%s' != '%s']",
							pIDstr, pExpFIDstr);
					res = ST_ERR_FAIL;
				}
				if (res == ST_ERR_SUCC) {
					TEST_FCOM__prf(pFnc, " [ID=%3d,  IDstr='%s',  mode=%c,  fnr=%2d]",
							fid, (pIDstr == NULL ? "" : (char const*)pIDstr),
							(mode ? 'A' : 'S'), fnr);
				}
			}
		} else {
			TEST_FCOM__prf(pFnc, "error: st_vorbc_gs_getField_props() failed");
		}
	}
	return (res == ST_ERR_SUCC);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__11_mapVOR_id(void)
{
	const char *cFNCN = __func__;
	Tst_bool  bres;
	Tst_err   res;
	Tst_int32 fnr = -1,
	          fid;
	Tst_tfldMap_genTagFld gtf;
	Tst_vorbc_fldData     fdata;
	Tst_vorbc_fldProp     fprops;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping Vorbis Tag Fields to Vorbis Tag Fields by ID...");

	st_vorbc_stc_rsetFPr(&fprops);
	res  = st_vorbc_stc_initFDat(&fdata);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (fid = 0; fid <= (Tst_int32)ST_VORBC_FID_NONE; fid++) {
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_VORBC_TAG_NAME_SH, /*modeAoS:*/(fid % 2 == 0),
				fnr, /*useIDorIDStr:*/ST_B_TRUE, (Tst_vorbc_frID)fid, NULL);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapVOR_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapVOR_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_int(&gtf, 100);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: st_tagFldMap_gs_setDataStr_int() failed");
			break;
		}

		if (fid != (Tst_int32)ST_VORBC_FID_NONE &&
				fid != (Tst_int32)ST_VORBC_FID_CTXT) {
			/* shouldn't fail */
			bres = TEST_TFLDMAP__mapVOR_mapAndCheckBasics(cFNCN, &gtf,
					/*expModeAoS:*/(fid % 2 == 0), fnr, (Tst_vorbc_frID)fid, NULL,
					/*expIsOK:*/ST_B_TRUE,
					&fdata, &fprops);
		} else if (fid == (Tst_int32)ST_VORBC_FID_CTXT ||
					// ReSharper disable once CppDFAConstantConditions
					fid == (Tst_int32)ST_VORBC_FID_NONE) {
			/* shouldn't fail, but get mapped to COMM */
			bres = TEST_TFLDMAP__mapVOR_mapAndCheckBasics(cFNCN, &gtf,
					/*expModeAoS:*/(fid % 2 == 0), fnr, ST_VORBC_FID_DESC, NULL,
					/*expIsOK:*/ST_B_FALSE,
					&fdata, &fprops);
		}
		if (! bres) {
			break;
		}
	}
	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_vorbc_stc_freeFDat(&fdata);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__12_mapVOR_cust(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_uint32 x;
	char const *pFIDstrIn,
	           *pFIDstrOut;
	Tst_tfldMap_genTagFld gtf;
	Tst_vorbc_fldData     fdata;
	Tst_vorbc_fldProp     fprops;
	Tst_vorbc_frID        fid;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping Vorbis Tag Fields to (custom) Vorbis Tag Fields...");

	st_vorbc_stc_rsetFPr(&fprops);
	res  = st_vorbc_stc_initFDat(&fdata);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (x = 0; x <= 2; x++) {
		const Tst_int32 fnr = 1;

		switch (x) {
			case 0:
				pFIDstrIn = "my own txt";  pFIDstrOut = "MY OWN TXT";  fid = ST_VORBC_FID_CTXT; break;
			case 1:
				pFIDstrIn = "DISCNUMBER";  pFIDstrOut = "DISCnumber";  fid = ST_VORBC_FID_DSCN; break;
			default:
				pFIDstrIn = "DISCTOTAL";   pFIDstrOut = "DISCTOTAL";   fid = ST_VORBC_FID_DSCT;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_VORBC_TAG_NAME_SH, /*modeAoS:*/(fid % 2 == 0),
				fnr, /*useIDorIDStr:*/ST_B_FALSE, ST_VORBC_FID_NONE,
				(Tst_str const*)pFIDstrIn);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapVOR_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdata, &fprops);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapVOR_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_pos(&gtf, 100, -1);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: setting field data failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapVOR_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/(fid % 2 == 0), fnr, fid, (Tst_str const*)pFIDstrOut,
				/*expIsOK:*/ST_B_TRUE,
				&fdata, &fprops);
		if (! bres) {
			break;
		}
	}
	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_vorbc_stc_freeFDat(&fdata);
	return bres;
}

/*----------------------------------------------------------------------------*/

/*
 * Tests ...
 */
Tst_bool
TEST_TFLDMAP__13_mapVOR_fromIv2(void)
{
	const char *cFNCN = __func__;
	Tst_bool   bres;
	Tst_err    res;
	Tst_uint32 x;
	char const *pFIDstrInIV2,
	           *pFIDstrOutVOR;
	Tst_tfldMap_genTagFld gtf;
	Tst_vorbc_fldData     fdataVOR;
	Tst_vorbc_fldProp     fpropsVOR;
	Tst_vorbc_frID        fidVOR;

	/* */
	TEST_FCOM__prf(cFNCN, "Mapping ID3v2 Tag Fields to Vorbis Tag Fields...");

	st_vorbc_stc_rsetFPr(&fpropsVOR);
	res  = st_vorbc_stc_initFDat(&fdataVOR);
	bres = (res == ST_ERR_SUCC);
	if (bres) {
		res  = st_tagFldMap_stc_initGTF(&gtf);
		bres = (res == ST_ERR_SUCC);
	}
	if (! bres) {
		TEST_FCOM__prf(cFNCN, "error: init structs failed");
		return ST_B_FALSE;
	}

	for (x = 0; x <= 4; x++) {
		switch (x) {
			case 0:
				pFIDstrInIV2 = "TIt2";  pFIDstrOutVOR = "TITLE";        fidVOR = ST_VORBC_FID_TITL; break;
			case 1:
				pFIDstrInIV2 = "TRCk";  pFIDstrOutVOR = "TRACKNUMber";  fidVOR = ST_VORBC_FID_TRKN; break;
			case 2:
				pFIDstrInIV2 = "tpos";  pFIDstrOutVOR = "DISCnumBER";   fidVOR = ST_VORBC_FID_DSCN; break;
			case 3:
				pFIDstrInIV2 = "w000";  pFIDstrOutVOR = "W000";         fidVOR = ST_VORBC_FID_CTXT; break;
			default:
				pFIDstrInIV2 = "CoMM";  pFIDstrOutVOR = "description";  fidVOR = ST_VORBC_FID_DESC;
		}
		/* set basics: Mode, FldNr, ID */
		bres = TEST_TFLDMAP__map_setBasics(&gtf, ST_ID3V2_TAG_NAME_SH, /*modeAoS:*/ST_B_TRUE,
				-1, /*useIDorIDStr:*/ST_B_FALSE, -1,
				(Tst_str const*)pFIDstrInIV2);
		if (! bres) {
			break;
		}

		/* should fail: no data/attributes set */
		bres = TEST_TFLDMAP__mapVOR_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, -1, NULL,
				/*expIsOK:*/ST_B_FALSE,
				&fdataVOR, &fpropsVOR);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: TEST_TFLDMAP__mapVOR_mapAndCheckBasics() failed");
			break;
		}

		res  = st_tagFldMap_gs_setDataStr_pos(&gtf, 100, -1);
		bres = (res == ST_ERR_SUCC);
		if (! bres) {
			TEST_FCOM__prf(cFNCN, "error: setting field data failed");
			break;
		}

		/* shouldn't fail */
		bres = TEST_TFLDMAP__mapVOR_mapAndCheckBasics(cFNCN, &gtf,
				/*expModeAoS:*/ST_B_TRUE, -1, fidVOR, (const Tst_str*)pFIDstrOutVOR,
				/*expIsOK:*/ST_B_TRUE,
				&fdataVOR, &fpropsVOR);
		if (! bres) {
			break;
		}
	}

	if (bres) {
		TEST_FCOM__prf(cFNCN, "Done.");
	}

	st_tagFldMap_stc_freeGTF(&gtf);
	st_vorbc_stc_freeFDat(&fdataVOR);
	return bres;
}

/******************************************************************************/
