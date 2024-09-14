/******************************************************************************/
/* tag_id3v2_zchk.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for validating ID3v2 Tag field data                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_ZCHK_C
#include "tag_id3v2_zchk-pp.h"
/*** */
#include "tag_id3v2-prot.h"
#include "tag_id3v2_stcs-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_comfnc_chk-pp.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_ZCHK_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy(), ... */
#include <stdlib.h>      /* calloc(), ... */
#include <ctype.h>       /* isdigit() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Run checks on one data field
 *
 * pFld  MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_id3v2_chk_checkFld(ST_OPTARG(const Tst_id3v2_opts *pOpts),
		const Tst_bool verbShowMappedStr,
		Tst_id3v2_fldData *pFld)
{
	const char *cFNCN = __func__;
	Tst_err  res;
	Tst_bool isOK = ST_B_FALSE;
	Tst_id3v2_fldProp_intn *pFldPrI;
	Tst_id3v2_fldErrs      *pFErrs;
	Tst_id3v2_fldData_intn *pFldI = NULL;

	/* ______________________________________________________________ */
	/* COMM,USLT:     te lang                      scd  text          */
	/* TXXX,WXXX:     te                           scd  text          */
	/* APIC:          te [PIC:imgfor|APIC:mime] pt desc      BINdata  */
	/* CM1,T000-TZZZ: te                                string(s)     */
	/* W000-WZZZ:                                       url           */
	/* UFID,PRIV:                                  own       BINdata  */
	/* IPLS:          te                                strpairs      */
	/* MCDI:                                                 BINdata  */
	/* GEO[B]:        te mime filen                scd       BINdata  */
	/* ______________________________________________________________ */
	/* CDM:     comprMeth uncomprSz comprBINdata                      */
	/* [P]CNT:  4bytes [byte ...]                                     */
	/* POP[M]:  email rating 4bytes [byte ...]                        */
	/* S[Y]LT:  te lang timestampfmt conttp contdesc BINdata          */
	/* ______________________________________________________________ */

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

	pFldPrI = &pFldI->fldPrI;
	pFErrs  = &pFldI->errs;

	pFldI->isOK = ST_B_FALSE;  /* ignore this field */

	/* Part I - Text/Bin */
	res = ST_TIV2__chk_chkFld_partI(pOpts, cFNCN,
			pFldI, pFldPrI, pFErrs, &isOK);
	if (res != ST_ERR_SUCC || ! isOK)
		return res;

	/* Part II - Scd/Filen/Mime/Lang/... */
	res = ST_TIV2__chk_chkFld_partII(pOpts, cFNCN,
			pFldI, pFldPrI, pFErrs, &isOK);
	if (res != ST_ERR_SUCC || ! isOK)
		return res;

	/* Part III - string-array, string formats */
	res = ST_TIV2__chk_chkFld_partIII(pOpts, cFNCN,
			verbShowMappedStr, pFldI, pFldPrI, pFErrs, &isOK);
	if (res != ST_ERR_SUCC || ! isOK)
		return res;

	/* Part IV - Set Owner/eMail if empty */
	res = ST_TIV2__chk_chkFld_partIV(pOpts, cFNCN,
			pFldI, pFldPrI, pFErrs);
	if (res != ST_ERR_SUCC)
		return res;

	/* */
	pFldI->isOK = ST_B_TRUE;  /* we love this field */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Run checks - Part I
 *   Text/BinData: String min/maxlen, Bin min/maxlen, strip strings
 *   ST_ID3V2_FTP_CNT/POP
 */
static Tst_err
ST_TIV2__chk_chkFld_partI(ST_OPTARG(const Tst_id3v2_opts *pOpts),
		const char *pFnc,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_id3v2_fldProp_intn *pFldPrI,
		Tst_id3v2_fldErrs *pFErrs, Tst_bool *pIsOK)
{
	Tst_err    res     = ST_ERR_SUCC;
	Tst_bool   cnv2ISO = ST_B_FALSE,  /* convert to ISO string ? */
	           dummy   = ST_B_TRUE,
	           modded  = ST_B_FALSE,
	           hadIvCh = ST_B_FALSE;
	Tst_uint32 strMinl = 0, /* string minlen */
	           binMinl = 0, /* binData minlen */
	           mscMin  = 0, /* multi-string min count */
	           slen,
	           valstrcnt,
	           x;
	Tst_mtes_strRestr    restr;
	Tst_mtes_string      *pStr;
	Tst_id3v2_fldd_strtp strTp;
	Tst_id3v2_fldd_dattp datTp;

	ST_ASSERTN_IARG(pFldI == NULL || pFldPrI == NULL ||
			pFErrs == NULL || pIsOK == NULL)

	*pIsOK = ST_B_FALSE;
	st_id3v2_stc_rsetStrTp(&strTp);
	st_id3v2_stc_rsetDatTp(&datTp);

	st_id3v2_fnc_getFieldStringTypes(pFldPrI->id, &strTp);
	st_id3v2_fnc_getFieldDataTypes(pFldPrI->id, &datTp);

	/* general restrictions */
	if (datTp.hasText) {
		if (strTp.hasMappedStr)
			mscMin = 1;
		strMinl = 1;
	}
	if (datTp.hasBDat)
		binMinl = 1;

	/* more specific restrictions */
	restr.hasR    = ST_B_TRUE;  /* we have restrictions */
	restr.ruleAlw = ST_B_TRUE;  /* mode: allow */
	st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
	restr.strMaxl = ST_ID3V2_MAX_STRLEN_M * 1024 * 1024;
	if (datTp.hasText) {
		restr.disNewln    = ST_B_TRUE;   /* disallow '\n' */
		restr.disTab      = ST_B_TRUE;   /* disallow tab '\t' */
		restr.disNotPrint = ST_B_TRUE;   /* disallow not-printable chars */
		if (strTp.hasNumStr) {
			restr.ruleAlw = ST_B_FALSE;  /* mode: disallow */
			st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
			restr.disDig  = ST_B_FALSE;  /* allow digits */
			restr.strMaxl = 10;
			cnv2ISO       = ST_B_TRUE;
		} else if (strTp.hasGenreStr) {
			restr.disBracks = ST_B_TRUE;  /* disallow brackets '<>[]{}' */
			cnv2ISO         = ST_B_TRUE;
		} else if (strTp.hasPosStr) {
			restr.ruleAlw  = ST_B_FALSE;  /* mode: disallow */
			st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
			restr.disSlash = ST_B_FALSE;  /* allow slashes */
			restr.disDig   = ST_B_FALSE;  /* allow digits */
			restr.strMaxl  = 20;
			cnv2ISO        = ST_B_TRUE;
		} else if (strTp.hasYearStr || strTp.hasDateStr ||
				strTp.hasTimeStr) {
			restr.ruleAlw = ST_B_FALSE;  /* mode: disallow */
			st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
			restr.disDig  = ST_B_FALSE;  /* allow digits */
			restr.strMaxl = 4;
			strMinl       = 4;
			cnv2ISO       = ST_B_TRUE;
		} else if (strTp.hasTStampStr) {
			restr.strMaxl = 19;
			strMinl       = 4;
			cnv2ISO       = ST_B_TRUE;
		} else if (strTp.hasUrlStr) {
			/* nothing to do */
		} else if (strTp.hasSimpleStr || strTp.hasMappedStr) {
			restr.disNotPrint = ST_B_FALSE;  /* allow not-printable chars */
		} else if (strTp.hasFullStr) {
			restr.disNewln    = ST_B_FALSE;  /* allow '\n' */
			restr.disTab      = ST_B_FALSE;  /* allow '\t' */
			restr.disNotPrint = ST_B_FALSE;  /* allow not-printable chars */
		}
	}
	switch (pFldPrI->id) {
	case ST_ID3V2_FID_234_TBPM:  /* BPM */
		restr.strMaxl = 8;
		break;
	case ST_ID3V2_FID_234_TCMP:  /* iTunes Comp */
		restr.strMaxl = 1;
		break;
	case ST_ID3V2_FID_234_TSRC:  /* ISRC */
		restr.strMaxl = 12;
		break;
	case ST_ID3V2_FID_234_UFID:
		restr.hasR = ST_B_FALSE;  /* we don't have restrictions */
		/*binMaxl = 64;*/
		break;
	default:
		if (! dummy) dummy = ST_B_TRUE;  /* do nothing */
	}

	/* apply restrictions */
	/** part I */
	if (datTp.hasText) {
		/* Text */
		/** */
		if (mscMin > 0 && st_mtes_strArrGetElemCount(&pFldI->dataSArr) < mscMin) {
			st_mtes_stc_rsetSA(&pFldI->dataSArr);
			return ST_ERR_SUCC;  /* ignore this field */
		}
		/** */
		valstrcnt = 0;
		for (x = 1; x <= st_mtes_strArrGetElemCount(&pFldI->dataSArr); x++) {
			pStr = st_mtes_strArrGetElem(&pFldI->dataSArr, x);
			/* */
			if (cnv2ISO && st_mtes_getEncodingCur(pStr) != ST_MTES_TE_ISO) {
				res = st_mtes_convStrTo_iso(pStr);
				if (res != ST_ERR_SUCC)
					return res;
			}
			/* */
			slen = st_mtes_strlenCh(pStr);
			if (slen > 0 && restr.hasR) {
				res = st_mtes_stripStr(pStr, &restr, &hadIvCh, &modded);
				if (res != ST_ERR_SUCC)
					return res;
				/* set error flag and update counter */
				if (hadIvCh && ! pFErrs->frHasInvChars) {
					pFErrs->frHasInvChars = ST_B_TRUE;
					++pFldI->frErrCnt;
				}
				/* */
				if (modded) {
					slen = st_mtes_strlenCh(pStr);
					if (pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
						st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
								ST_B_TRUE, "ctx", pStr, "");
				}
			}
			/* */
			if (strMinl > 0 && slen < strMinl) {
				/* set error flag and update counter */
				if (! pFErrs->frHasInvChars) {
					pFErrs->frHasInvChars = ST_B_TRUE;
					++pFldI->frErrCnt;
				}
				/* */
				if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) == 1)
					return ST_ERR_SUCC;  /* ignore this field */
				continue;
			}
			/* */
			++valstrcnt;
		} /* end for */
		if (mscMin > 0 && valstrcnt < mscMin &&
				st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0) {
			st_mtes_stc_rsetSA(&pFldI->dataSArr);
			return ST_ERR_SUCC;  /* ignore this field */
		}
	}
	/** */
	if (datTp.hasBDat) {
		/* Binary data */
		/** */
		if (binMinl > 0 && st_binobj_getDataSize(&pFldI->dataBinDat) < binMinl) {
			/* set error flag and update counter */
			if (! pFErrs->frHasInvData) {
				pFErrs->frHasInvData = ST_B_TRUE;
				++pFldI->frErrCnt;
			}
			/* */
			return ST_ERR_SUCC;  /* ignore this field */
		}
		/** check for all-nulls */
		if (ST_TIV2__chksub_isDataAllNulls(&pFldI->dataBinDat)) {
			if (pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
				st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
						"bindata all NULL, ignoring field...");
			return ST_ERR_SUCC;  /* ignore this field */
		}
	}
	/** */
	switch (pFldPrI->typ) {
	case ST_ID3V2_FTP_CNT:       /* [P]CNT */
		if (pFldI->dataUVal == 0)
			return ST_ERR_SUCC;  /* ignore this field */
		break;
	case ST_ID3V2_FTP_POP:       /* POP[M] */
		if (pFldI->dataBVal == 0 && st_mtes_isStrEmptyBy(&pFldI->attrDesc) &&
				pFldI->dataBVal == 0 && pFldI->dataUVal == 0)
			return ST_ERR_SUCC;  /* ignore this field */
		break;
	default:
		if (dummy) dummy = ST_B_FALSE;  /* do nothing */
	}

	*pIsOK = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Run checks - Part II
 *   Scd/Filen/Mime/Lang
 *   Picture
 *   Synced lyrics
 */
static Tst_err
ST_TIV2__chk_chkFld_partII(ST_OPTARG(const Tst_id3v2_opts *pOpts),
		const char *pFnc,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_id3v2_fldProp_intn *pFldPrI,
		Tst_id3v2_fldErrs *pFErrs, Tst_bool *pIsOK)
{
	Tst_err    res     = ST_ERR_SUCC;
	Tst_bool   modded  = ST_B_FALSE,
	           hadIvCh = ST_B_FALSE;
	Tst_uint32 x;
	Tst_mtes_strRestr    restr;
	Tst_mtes_string      *pStr,
	                     tmps;
	Tst_id3v2_fldd_dattp datTp;

	ST_ASSERTN_IARG(pFldI == NULL || pFldPrI == NULL ||
			pFErrs == NULL || pIsOK == NULL)

	*pIsOK = ST_B_FALSE;
	/* */
	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;
	st_id3v2_stc_rsetDatTp(&datTp);
	st_id3v2_fnc_getFieldDataTypes(pFldPrI->id, &datTp);
	st_mtes_stc_rsetRestr(&restr, ST_B_FALSE);
	restr.strMaxl = ST_ID3V2_MAX_STRLEN_M * 1024 * 1024;

	/* Short descr. / Filename / MIME type */
	if (datTp.hasScd || datTp.hasFn || datTp.hasMime) {
		for (x = 0; x < 3; x++) {
			if (x == 0 && ! datTp.hasScd)
				continue;
			if (x == 1 && ! datTp.hasFn)
				continue;
			if (x == 2 && ! datTp.hasMime)
				continue;
			/** */
			restr.hasR        = ST_B_TRUE;
			restr.ruleAlw     = ST_B_TRUE;
			st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
			restr.disNewln    = ST_B_TRUE;  /* disallow '\n' */
			restr.disSpa      = (x == 2);   /* disallow ' ' */
			restr.disTab      = ST_B_TRUE;  /* disallow '\t' */
			restr.disNotPrint = (x == 2);   /* disallow not-printable chars */
			restr.strMaxl     = ST_ID3V2_MAX_STRLEN_M * 1024 * 1024;
			/** */
			switch (x) {
			case 0: pStr = &pFldI->attrDesc; break;
			case 1: pStr = &pFldI->attrGeoFn; break;
			default:
				if (pFldI->pAttrGeoMime &&
						st_sysStrcmp2(ST_B_TRUE, pFldI->pAttrGeoMime, "binary"))
					st_mtes_stc_rsetStr(&tmps, ST_B_TRUE, ST_B_TRUE);
				else {
					res = st_mtes_setStrTo(ST_MTES_TE_ISO,
							pFldI->pAttrGeoMime, &tmps);
					if (res != ST_ERR_SUCC) {
						st_mtes_stc_freeStr(&tmps);
						return res;
					}
				}
				pStr = &tmps;
			}
			if (st_mtes_getStringSize(pStr) > 1) {
				res = st_mtes_stripStr(pStr, &restr, &hadIvCh, &modded);
				if (res != ST_ERR_SUCC) {
					st_mtes_stc_freeStr(&tmps);
					return res;
				}
				/* set error flag and update counter */
				if (hadIvCh && ! pFErrs->frHasInvChars) {
					pFErrs->frHasInvChars = ST_B_TRUE;
					++pFldI->frErrCnt;
				}
			} else
				modded = ST_B_FALSE;
			/** */
			if (x == 2 && st_mtes_getStringSize(pStr) == 0) {
				/* set MIME to default type */
				res = st_mtes_setStrTo2(ST_MTES_TE_ISO, ST_ID3V2_MIMEDEF, &tmps);
				if (res != ST_ERR_SUCC) {
					st_mtes_stc_freeStr(&tmps);
					return res;
				}
				modded = ST_B_TRUE;
			}
			/** */
			if (modded) {
				/* set error flag and update counter */
				if (! pFErrs->frHasInvChars) {
					pFErrs->frHasInvChars = ST_B_TRUE;
					++pFldI->frErrCnt;
				}
				/* */
				if (x == 2) {
					res = st_mtes_copyToCharp(&tmps, &pFldI->pAttrGeoMime, NULL);
					if (res != ST_ERR_SUCC) {
						st_mtes_stc_freeStr(&tmps);
						return res;
					}
				}
				/* */
				if (pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
							ST_B_TRUE,
							(x == 0 ? "csc" : (x == 1 ? "cfn" : "cmi")),
							pStr, "");
			}

		}
	}
	st_mtes_stc_freeStr(&tmps);

	/* language */
	if (datTp.hasLang) {
		modded = ST_B_FALSE;
		ST_TIV2__chksub_cLang((Tst_str*)pFldI->attrLang, &modded);
		if (modded && ! pFldI->errs.frHasInvLng) {
			pFldI->errs.frHasInvLng = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}

	/* picture */
	if (datTp.hasPict) {
		modded = ST_B_FALSE;
		if (! ST_TIV2__chksub_cPict(pFldI, &modded))
			return ST_ERR_SUCC;
		if (modded && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"mim=\"%s\"/fmt='%s',  ptp='%s'",
					st_utilsFmt_getPicFmt_mime_byEnum(pFldI->attrPicFmt),
					st_utilsFmt_getPicFmt_fext_byEnum(pFldI->attrPicFmt),
					st_id3v2_fnc_getPicTp_capShort_byEnum(pFldI->attrPicTp));
	}

	/* synced lyrics */
	if (datTp.hasSylt) {
		modded = ST_B_FALSE;
		ST_TIV2__chksub_cSylt(pFldI, &modded);
		if (modded && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
					"tsf=%d,  ctp=%d", pFldI->attrSltTSF, pFldI->attrSltCTP);
	}

	*pIsOK = ST_B_TRUE;  /* we love this field */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Run checks - Part III
 *   specific string formats (BPM,Genre,Track/Pos,Time/Date,Timestamp)
 *   strip string-array
 *   mapped strings (IPLS)
 */
static Tst_err
ST_TIV2__chk_chkFld_partIII(ST_OPTARG(const Tst_id3v2_opts *pOpts),
		const char *pFnc,
		const Tst_bool verbShowMappedStr,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_id3v2_fldProp_intn *pFldPrI,
		Tst_id3v2_fldErrs *pFErrs, Tst_bool *pIsOK)
{
	Tst_err    res;
	Tst_bool   modded,
	           moddedP = ST_B_FALSE,
	           oneOK;
	Tst_uint32 x;
	Tst_mtes_string      *pStr;
	Tst_id3v2_fldd_strtp strTp;
	Tst_id3v2_fldd_dattp datTp;

	ST_ASSERTN_IARG(pFldI == NULL || pFldPrI == NULL ||
			pFErrs == NULL || pIsOK == NULL)

	*pIsOK = ST_B_TRUE;

	st_id3v2_stc_rsetStrTp(&strTp);
	st_id3v2_stc_rsetDatTp(&datTp);
	st_id3v2_fnc_getFieldStringTypes(pFldPrI->id, &strTp);
	st_id3v2_fnc_getFieldDataTypes(pFldPrI->id, &datTp);

	if (datTp.hasText && ! strTp.hasMappedStr) {
		oneOK = ST_B_FALSE;
		for (x = 1; x <= st_mtes_strArrGetElemCount(&pFldI->dataSArr); x++) {
			pStr = st_mtes_strArrGetElem(&pFldI->dataSArr, x);
			/* */
			modded = ST_B_FALSE;
			/* */
			if (pFldPrI->id == ST_ID3V2_FID_234_TBPM) {
				if (st_tagCFnc_chk_cBPM(pStr, &modded))
					oneOK = ST_B_TRUE;
				if (modded && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
							ST_B_TRUE, "cbp", pStr, "");
			} else if (strTp.hasGenreStr) {
				if (st_tagCFnc_chk_cGenre(pStr, &modded))
					oneOK = ST_B_TRUE;
				if (modded && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
							ST_B_TRUE, "cge", pStr, "");
			} else if (strTp.hasPosStr) {
				if (st_tagCFnc_chk_cTrack(pStr, NULL, NULL, &modded))
					oneOK = ST_B_TRUE;
				if (modded && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
							ST_B_TRUE, "ctn", pStr, "");
			} else if (strTp.hasTimeStr || strTp.hasDateStr) {
				if (st_tagCFnc_chk_cTimeOrDate(pStr, strTp.hasTimeStr, NULL, &modded))
					oneOK = ST_B_TRUE;
				if (modded && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
								ST_B_TRUE, "ctd", pStr, "");
			} else if (strTp.hasYearStr || strTp.hasTStampStr) {
				if (st_tagCFnc_chk_cTStamp(pStr, NULL, &modded))
					oneOK = ST_B_TRUE;
				if (modded && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
								ST_B_TRUE, "cts", pStr, "");
			} else {
				oneOK = ST_B_TRUE;
				break;
			}
			/* */
			if (modded) {
				/* set error flag and update counter */
				if (! pFldI->errs.frHasInvChars) {
					pFldI->errs.frHasInvChars = ST_B_TRUE;
					++pFldI->frErrCnt;
				}
			}
		}
		/** */
		*pIsOK = oneOK;
	} else if (datTp.hasText && strTp.hasMappedStr) {
		*pIsOK = st_tagCFnc_chk_cPeopleArr(&pFldI->dataSArr, &moddedP);
		if (moddedP && ! pFldI->errs.frHasInvData) {
			pFldI->errs.frHasInvData = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}
	if (! *pIsOK)
		return ST_ERR_SUCC;

	if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) == 0)
		return ST_ERR_SUCC;

	/* strip array */
	/**st_id3v2_prStrArr("before strip:", &pFldI->dataSArr);**/
	modded = ST_B_FALSE;
	res    = st_mtes_strArrStrip(&pFldI->dataSArr, strTp.hasMappedStr, &modded);
	if (res != ST_ERR_SUCC)
		return res;
	/**st_id3v2_prStrArr("after strip:", &pFldI->dataSArr);**/
	*pIsOK = (st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0);
	if (! *pIsOK)
		return ST_ERR_SUCC;
	moddedP = modded || moddedP;

	/* output modified people mapping */
	if (moddedP && strTp.hasMappedStr && verbShowMappedStr &&
			pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		ST_TIV2__chksub_cPeople_print(pOpts, pFnc, pFldI, 3);

	/* */
	if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) <= 1)
		return ST_ERR_SUCC;
	if (strTp.hasGenreStr) {
		/* merge genre-strings */
		*pIsOK = st_tagCFnc_genreMerge(&pFldI->dataSArr);
		if (*pIsOK && pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
					ST_B_TRUE, "cge",
					st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Run checks - Part IV
 *   Set Owner/eMail if empty
 */
static Tst_err
ST_TIV2__chk_chkFld_partIV(ST_OPTARG(const Tst_id3v2_opts *pOpts),
		const char *pFnc,
		Tst_id3v2_fldData_intn *pFldI,
		Tst_id3v2_fldProp_intn *pFldPrI,
		Tst_id3v2_fldErrs *pFErrs)
{
	Tst_err  res    = ST_ERR_SUCC;
	//Tst_bool modded = ST_B_FALSE;

	ST_ASSERTN_IARG(pFldI == NULL || pFldPrI == NULL || pFErrs == NULL)

	switch (pFldI->fldPrI.id) {
	case ST_ID3V2_FID_234_AENC:
	case ST_ID3V2_FID_234_UFID:
	case ST_ID3V2_FID__34_PRIV:
	case ST_ID3V2_FID_234_POPM:
		if (st_mtes_isStrEmptyBy(&pFldI->attrDesc)) {
			res = st_mtes_setStrTo2(ST_MTES_TE_ISO,
					(pFldI->fldPrI.id != ST_ID3V2_FID_234_POPM ?
						"No One" : "no@one.com"), &pFldI->attrDesc);
			if (res == ST_ERR_SUCC) {
				//modded = ST_B_TRUE;
				if (pOpts != NULL && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
					st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
							ST_B_TRUE, "csc", &pFldI->attrDesc, "");
			}
		}
		break;
	default:
		//modded = ST_B_FALSE;  /* do nothing */
		break;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Check and correct 3 byte language identifier
 *
 * pLang has 4 bytes
 *
 * returns: true on success (if value is OK)
 */
static Tst_bool
ST_TIV2__chksub_cLang(Tst_str *pLang, ST_OPTARG(Tst_bool *pModded))
{
	Tst_uint32 slen,
	           x;
	Tst_str    uch;

	if (pModded != NULL)
		*pModded = ST_B_FALSE;

	slen = st_sysStrlen(pLang);
	if (slen < 2 || slen > 3) {
		/* reset 'language' entirely */
		pLang[0] = 'x';
		pLang[1] = 'x';
		pLang[2] = 'x';
		/* */
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		return ST_B_TRUE;
	}

	/* convert to lower case */
	st_sysStrToLower(pLang);
	/* */
	if (st_sysStrcmp2(ST_B_TRUE, pLang, "de ") ||
			st_sysStrcmp2(ST_B_TRUE, pLang, "deu")) {
		pLang[0] = 'g';
		pLang[1] = 'e';
		pLang[2] = 'r';
		/* */
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		return ST_B_TRUE;
	}
	if (st_sysStrcmpN2(ST_B_TRUE, 2, pLang, "en")) {
		if (pLang[2] != 'u' && pLang[2] != 'g') {
			pLang[2] = 'g';
			/* */
			if (pModded != NULL)
				*pModded = ST_B_TRUE;
		}
		return ST_B_TRUE;
	}
	/* elimate invalid chars */
	for (x = 0; x < slen; x++) {
		uch = pLang[x];
		/* only a-z and A-Z are allowed */
		if (! ((uch >= ST_CSET_ISO_ACAP && uch <= ST_CSET_ISO_ZCAP) ||
				(uch >= ST_CSET_ISO_ASMA && uch <= ST_CSET_ISO_ZSMA))) {
			/* clear 'language' entirely */
			pLang[0] = 'x';
			pLang[1] = 'x';
			pLang[2] = 'x';
			/* */
			if (pModded != NULL)
				*pModded = ST_B_TRUE;
			break;
		}
	}
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Check and correct picture stuff
 *
 * returns: true on success (if value is OK)
 */
static Tst_bool
ST_TIV2__chksub_cPict(Tst_id3v2_fldData_intn *pFldI,
		ST_OPTARG(Tst_bool *pModded))
{
	if (pModded != NULL)
		*pModded = ST_B_FALSE;

	/* picture type */
	if (st_id3v2_fnc_getPicTp_cap_byEnum(pFldI->attrPicTp) == NULL) {
		pFldI->attrPicTp = ST_ID3V2_PT_OTHER;
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		/* */
		if (! pFldI->errs.frHasInvPT) {
			pFldI->errs.frHasInvPT = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}
	/* picture format/fileext/mime-type */
	if (st_utilsFmt_getPicFmt_fext_byEnum(pFldI->attrPicFmt) == NULL) {
		pFldI->attrPicFmt = ST_UTILSFMT_PF_JPG;
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		/* */
		if (! pFldI->errs.frHasInvPF) {
			pFldI->errs.frHasInvPF = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}

	return ST_B_TRUE;
}

static void
ST_TIV2__chksub_cSylt(Tst_id3v2_fldData_intn *pFldI,
		ST_OPTARG(Tst_bool *pModded))
{
	if (pModded != NULL)
		*pModded = ST_B_FALSE;

	switch (pFldI->attrSltTSF) {
	case ST_ID3V2_SLTTSF_MPEG:
	case ST_ID3V2_SLTTSF_MILLI:
		break;
	default:
		pFldI->attrSltTSF = ST_ID3V2_SLTTSF_MPEG;
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		/* */
		if (! pFldI->errs.frHasInvSLTtsf) {
			pFldI->errs.frHasInvSLTtsf = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}
	switch (pFldI->attrSltCTP) {
	case ST_ID3V2_SLTCTP_OTHER:  case ST_ID3V2_SLTCTP_LYR:
	case ST_ID3V2_SLTCTP_TRANS:  case ST_ID3V2_SLTCTP_MOVE:
	case ST_ID3V2_SLTCTP_EVENT:  case ST_ID3V2_SLTCTP_CHORD:
	case ST_ID3V2_SLTCTP_TRIV:   case ST_ID3V2_SLTCTP_URLWP:
	case ST_ID3V2_SLTCTP_URLIMG:
		break;
	default:
		pFldI->attrSltCTP = ST_ID3V2_SLTCTP_OTHER;
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		/* */
		if (! pFldI->errs.frHasInvSLTctp) {
			pFldI->errs.frHasInvSLTctp = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
	}
}

/*----------------------------------------------------------------------------*/

static void
ST_TIV2__chksub_cPeople_print(ST_OPTARG(const Tst_id3v2_opts *pOpts),
		const char *pFnc,
		Tst_id3v2_fldData_intn *pFldI, const int modlev)
{
	Tst_uint32 x;

	if (pOpts == NULL || ! ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		return;
	for (x = 1; x + 1 <= st_mtes_strArrGetElemCount(&pFldI->dataSArr); x += 2) {
		st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
				ST_B_TRUE, (modlev < 3 ? "map" : "cmp"),
				st_mtes_strArrGetElem(&pFldI->dataSArr, x), "");
		st_id3v2_d_fdeb3(pOpts, 0, pFnc, pFldI,
				"  -->");
		st_id3v2_d_fdeb3Str(pOpts, 0, pFnc, pFldI,
				ST_B_TRUE, "   ",
				st_mtes_strArrGetElem(&pFldI->dataSArr, x + 1), "");
	}
}

/*----------------------------------------------------------------------------*/

static Tst_bool
ST_TIV2__chksub_isDataAllNulls(Tst_binobj *pBinDat)
{
	Tst_buf    buf1[2048],
	           buf2[2048];
	Tst_bool   isAN      = ST_B_TRUE;
	Tst_uint32 rdFromStr = 0;
	Tst_streamrd strrdBO;

	st_streamrd_stc_initSObj(&strrdBO);
	if (st_binobj_attachStreamrd(pBinDat, &strrdBO, 0) != ST_ERR_SUCC)
		return ST_B_FALSE;

	memset(buf2, 0, sizeof(buf2));
	while (! st_streamrd_isEOF(&strrdBO)) {
		st_streamrd_rdBuffer_noCRC(&strrdBO, sizeof(buf1), buf1, &rdFromStr);
		if (memcmp(buf1, buf2, rdFromStr) != 0) {
			isAN = ST_B_FALSE;
			break;
		}
		if (sizeof(buf1) != rdFromStr)
			break;
	}

	st_binobj_detachStreamrd(pBinDat);
	st_streamrd_stc_freeSObj(&strrdBO);
	return isAN;
}

/******************************************************************************/
