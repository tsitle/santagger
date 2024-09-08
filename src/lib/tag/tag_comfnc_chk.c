/******************************************************************************/
/* tag_comfnc_chk.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Common functions for validating/parsing field data                         */
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
#include <santag/common/string_mte.h>
#include <santag/common/sys_fnc.h>
#include <santag/tag/tag_comfnc.h>
#include <santag/tag/tag_id3_helper.h>
/** */
#define SRC_TAG_COMFNC_CHK_C
#include "tag_comfnc_chk-pp.h"
/*** */
#include "tag_id3_helper-prot.h"
#undef SRC_TAG_COMFNC_CHK_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy(), ... */
#include <stdlib.h>      /* calloc(), ... */
#include <ctype.h>       /* isdigit() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Returns the Genre caption in pCapOut
 *
 * pGin     MUST be set
 * pCapOut  MUST be set
 * pGin     should contain a valid Genre-string
 *   (e.g. "(9)Metal" or "(9) Metal" or "9 Metal" or "9" or "Metal")
 *
 * returns: ERRCODE
 *          pCapOut will then contain the caption, e.g. "Metal"
 *            and be encoded as textEncOut
 */
Tst_err
st_tagCFnc_chk_getGenreCap(const Tst_mtes_string *pGin, Tst_mtes_string *pCapOut,
		const Tst_mtes_textEnc textEncOut)
{
	return ST_TCFNC__chk_getGenre(pGin, pCapOut, textEncOut, NULL, NULL);
}

Tst_err
st_tagCFnc_chk_getGenreNum(const Tst_mtes_string *pGin, Tst_int32 *pGNrOut)
{
	return ST_TCFNC__chk_getGenre(pGin, NULL, ST_MTES_TE_NONE, pGNrOut, NULL);
}

Tst_err
st_tagCFnc_chk_getGenreNum_pchar(const Tst_str *pGin, Tst_int32 *pGNrOut)
{
	Tst_err res;
	Tst_mtes_string tmps;

	ST_ASSERTN_IARG(pGin == NULL || pGNrOut == NULL)

	*pGNrOut = -1;
	/* */
	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;
	res = st_mtes_setStrTo(ST_MTES_TE_ISO, pGin, &tmps);
	if (res == ST_ERR_SUCC)
		res = st_tagCFnc_chk_getGenreNum(&tmps, pGNrOut);
	st_mtes_stc_freeStr(&tmps);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Returns timestamp in pTStamp
 *
 * pStr     MUST be set and should contain a valid Timestamp-string
 *            (e.g. "2021-09-20t08:05:69")
 * pTStamp  MUST be set
 * pIsOK    MUST be != NULL
 *
 * returns: ERRCODE
 */
Tst_err
st_tagCFnc_chk_getTStamp(const Tst_bool isTimestampStr, const Tst_bool isTimeStr,
		const Tst_bool isDateStr, const Tst_mtes_string *pStr,
		Tst_tagCFnc_tstamp *pTStamp, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_mtes_string tmps;

	ST_ASSERTN_IARG(pStr == NULL || pTStamp == NULL ||
			pIsOK == NULL)

	*pIsOK = ST_B_FALSE;
	st_tagCFnc_stc_rsetTSta(pTStamp);
	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;
	res = st_mtes_convStr_c(pStr, &tmps, ST_MTES_TE_ISO);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmps);
		return res;
	}
	if (isTimestampStr)
		*pIsOK = st_tagCFnc_chk_cTStamp(&tmps, pTStamp, NULL);
	else if (isTimeStr || isDateStr)
		*pIsOK = st_tagCFnc_chk_cTimeOrDate(&tmps, isTimeStr, pTStamp, NULL);
	if (! *pIsOK)
		st_tagCFnc_stc_rsetTSta(pTStamp);
	st_mtes_stc_freeStr(&tmps);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Get nums from PosString
 *   (e.g. "2/3" -> pNum=2, pNumTot=3)
 *
 * pStr      MUST be set
 * pNum      MUST be != NULL
 * pNumTot   MUST be != NULL
 */
Tst_err
st_tagCFnc_chk_getPosStringNums(const Tst_mtes_string *pStr,
		Tst_uint32 *pNum, Tst_uint32 *pNumTot)
{
	Tst_err res;
	Tst_mtes_string   tmps;
	Tst_mtes_strRestr restr;

	ST_ASSERTN_IARG(pStr == NULL || pNum == NULL || pNumTot == NULL)

	*pNum    = 0;
	*pNumTot = 0;
	/* */
	res = st_mtes_stc_initStr(&tmps);
	if (res != ST_ERR_SUCC)
		return res;
	restr.hasR     = ST_B_TRUE;   /* we have restrictions */
	restr.ruleAlw  = ST_B_FALSE;  /* mode: disallow */
	st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
	restr.disSlash = ST_B_FALSE;  /* allow slashes */
	restr.disDig   = ST_B_FALSE;  /* allow digits */
	restr.strMaxl  = 20;
	/* */
	res = st_mtes_convStrTo_iso_c(pStr, &tmps);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stripStr(&tmps, &restr, NULL, NULL);
	if (res == ST_ERR_SUCC &&
			! st_tagCFnc_chk_cTrack(&tmps, pNum, pNumTot, NULL)) {
		*pNum    = 0;
		*pNumTot = 0;
	}
	st_mtes_stc_freeStr(&tmps);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Check and correct one Genre-string
 *
 * pStr is ISO string, 1 <= len
 *
 * returns: true on success (if value is OK)
 */
Tst_bool
st_tagCFnc_chk_cGenre(Tst_mtes_string *pStr, ST_OPTARG(Tst_bool *pModded))
{
	Tst_bool  resB;
	Tst_str   *pRstr = NULL;
	Tst_int32 gnrTmp = -1;
	Tst_mtes_string cap;

	if (st_mtes_stc_initStr(&cap) != ST_ERR_SUCC)
		return ST_B_FALSE;

	resB = st_tagCFnc_chk_cGenre_sub(pStr, &cap, &gnrTmp, &pRstr, pModded);
	if (! resB)
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
	if (resB && ((pModded != NULL && *pModded) ||
			(pRstr == NULL || st_mtes_getDataPt(pStr) == NULL ||
				! st_sysStrcmp2(ST_B_TRUE, pRstr, st_mtes_getDataPt(pStr))))) {
		if (st_mtes_setStrTo(ST_MTES_TE_ISO, pRstr, pStr) != ST_ERR_SUCC)
			resB = ST_B_FALSE;
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
	}
	st_mtes_stc_freeStr(&cap);
	ST_DELPOINT(pRstr)
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * pInp might be something like "(199 )Crucifix"
 * pCap would then become "Crucifix",
 *   ppRstr "199 Crucifix" and pGenNr 199
 *
 * returns: true on success (if value is OK)
 */
Tst_bool
st_tagCFnc_chk_cGenre_sub(const Tst_mtes_string *pInp,
		Tst_mtes_string *pCap, Tst_int32 *pGenNr,
		Tst_str **ppRstr, ST_OPTARG(Tst_bool *pModded))
{
	Tst_bool      resB    = ST_B_TRUE;
	Tst_str const *pISO   = st_mtes_getDataPt(pInp);
	Tst_int32     gnrFRS  = -1,     /* genre nr. for ppRstr */
	              begN    = -1,     /* offset begin num */
	              endN    = -1,     /*  -"-   end   -"- */
	              begT    = -1,     /* offset begin cap */
	              endT    = -1,     /*  -"-   end   -"- */
	              state   = 0,      /* current parsing state */
	              ix      = -1;     /* index of genre in genre-array */
	Tst_str       *pTstr  = NULL,   /* temporary caption string */
	              *pNstr  = NULL;   /* temporary */
	char const    *pGAcap = NULL,   /* caption from array */
	              *pGAsid = NULL;   /* special genre ID ('RX' or 'CR') */
	Tst_byte      gaNr    = 255;    /* genre nr. from array */
	Tst_uint32    slen    = st_mtes_strlenCh(pInp),
	              lenT    = 0,
	              lenR    = 0;

	st_mtes_stc_rsetStr(pCap, ST_B_TRUE, ST_B_TRUE);
	ST_DELPOINT((*ppRstr))
	if (pModded != NULL)
		*pModded = ST_B_FALSE;
	*pGenNr = -1;
	if (pISO == NULL)
		return ST_B_FALSE;

	/* "(9)Metal", "(9)", "9", "Metal", "9 Metal", "( 9 ) Metal", ... */
	/* parse string */
	while (*pISO) {
		switch (state) {
		case 0:  /* init */
			/**st_id3v2_prf("state %d, '%c'\n", state, *pISO);**/
			if (*pISO == ST_CSET_ISO_PARTH1) {
				begN  = 1;
				endN  = 1;
				state = 1;  /* in parenth. */
			} else if (isdigit((int)*pISO)) {
				begN  = 0;
				endN  = 0;
				state = 5;  /* number start */
			} else if (*pISO == ST_CSET_ISO_PARTH2) {
				begT  = 1;
				endT  = 1;
				state = 10;  /* text start */
			} else {
				begT  = 0;
				endT  = 0;
				state = 10;  /* text start */
			}
			break;
		case 1:  /* in parenth. */
			/**st_id3v2_prf("state %d, '%c'\n", state, *pISO);**/
			if (*pISO == ST_CSET_ISO_PARTH2) {
				begT  = endN + 1;
				endT  = begT;
				begN  = -1;
				endN  = -1;
				state = 7;  /* after par: text start */
			} else if (isdigit((int)*pISO)) {
				endN++;
				state = 2;  /* in par: num start */
			} else {
				begN++;
				endN++;
			}
			break;
		case 2:  /* in par: num start */
			/**st_id3v2_prf("state %d, '%c'\n", state, *pISO);**/
			if (*pISO == ST_CSET_ISO_PARTH2 || ! isdigit((int)*pISO)) {
				begT = endN + 1;
				endT = begT;
				endN--;
				if (*pISO == ST_CSET_ISO_PARTH2)
					state = 7;  /* after par: text start */
				else {
					state = 3;  /* in par: after num */
					if (*pISO != ST_CSET_ISO_SPA) {
						begN = -1;
						endN = -1;
					}
				}
			} else
				endN++;
			break;
		case 3:  /* in par: after num */
			/**st_id3v2_prf("state %d, '%c'\n", state, *pISO);**/
			begT++;
			endT++;
			if (*pISO == ST_CSET_ISO_PARTH2) {
				state = 8;  /* after par: text start */
			} else if (*pISO != ST_CSET_ISO_SPA) {
				begN = -1;
				endN = -1;
			}
			break;
		case 5:  /* num start */
			/**st_id3v2_prf("state %d, '%c'\n", state, *pISO);**/
			if (! isdigit((int)*pISO)) {
				begT = endN + 1;
				endT = begT;
				if (*pISO == ST_CSET_ISO_SPA) {
					begT++;
					endT++;
				} else if (*pISO != ST_CSET_ISO_SPA && ! isdigit((int)*pISO)) {
					begN = -1;
					endN = -1;
				}
				state = 8;  /* after num: text start */
			} else
				endN++;
			break;
		case 7:  /* after par: text start */
		case 8:  /* after num: text start */
			/**st_id3v2_prf("state %d, '%c'\n", state, *pISO);**/
			if (*pISO == ST_CSET_ISO_SPA)
				begT++;
			else
				state = 10;  /* text start */
			endT++;
			break;
		default:  /* text start */
			/**st_id3v2_prf("state %d, '%c'\n", state, *pISO);**/
			endT++;
		}
		pISO++;
	}
	/**st_id3v2_prf("n %d-%d '%c-%c',  t %d-%d '%c-%c'  \"%s\"\n", begN, endN,
			(begN>=0&&begN<slen?pInp->pData[begN]:'%'),
			(endN>=0&&endN<slen?pInp->pData[endN]:'%'), begT, endT,
			(begT>=0&&begT<slen?pInp->pData[begT]:'%'),
			(endT>=0&&endT<slen?pInp->pData[endT]:'%'),
			pInp->pData);**/
	if (begN > endN) begN = -1; else {
		if (begN >= (Tst_int32)slen) begN = -1;
		else if (endN >= (Tst_int32)slen) endN = slen - 1; }
	if (begT > endT) begT = -1; else {
		if (begT >= (Tst_int32)slen) begT = -1;
		else if (endT >= (Tst_int32)slen) endT = slen - 1; }

	/* convert genre-number from string to int */
	if (begN >= 0 && endN >= begN && (slen = endN - begN + 1) < 8) {
		ST_CALLOC(pNstr, Tst_str*, slen + 1, 1)
		if (pNstr == NULL)
			return ST_B_FALSE;
		memcpy(pNstr, &st_mtes_getDataPt(pInp)[begN], slen);
		pNstr[slen] = 0;
		*pGenNr     = (Tst_uint32)atoi((char*)pNstr);
		ST_DELPOINT(pNstr)
		if (*pGenNr > ST_ID3_TFLD_GENRE_MAX_IV2)
			*pGenNr = -1;
		/* get default caption for pGenNr */
		if (*pGenNr >= 0 && *pGenNr <= ST_ID3_GENRE_DEFMAXNR && *pGenNr != 255 &&
				st_id3_genreExists_int((Tst_byte)*pGenNr, &ix)) {
			/* use ONLY these vals, forget about the parsed caption */
			pGAcap  = ST_ID3_GENRES[ix].pCap;
			gaNr    = (Tst_byte)*pGenNr;
			lenT    = st_sysStrlen2(pGAcap);
			lenR    = 1 + 3 + 1 + lenT;  /* '(xxx)YYYY' */
			begT    = -1;
			gnrFRS  = -1;
		} else if (*pGenNr >= 0 && *pGenNr <= ST_ID3_GENRE_DEFMAXNR) {
			gnrFRS = -1;
			lenR   = 0;  /* '' */
		} else {
			gnrFRS = *pGenNr;
			lenR   = slen;  /* 'xxx...xxx' */
			if (*pGenNr == 255)
				*pGenNr = -1;
		}
	}
	/* copy caption-substring */
	if (begT >= 0 && endT >= begT) {
		slen = endT - begT + 1;
		ST_CALLOC(pTstr, Tst_str*, slen + 1, 1)
		if (pTstr == NULL) {
			ST_DELPOINT(pNstr)
			return ST_B_FALSE;
		}
		memcpy(pTstr, &st_mtes_getDataPt(pInp)[begT], slen);
		pTstr[slen] = 0;
		lenT        = slen;  /* 'YYYY' */
		/* get default number for caption */
		if (st_id3_genreExists_str(pTstr, &ix)) {
			/* use ONLY these vals, forget about the parsed gnrFRS */
			gaNr    = ST_ID3_GENRES[ix].nr;
			pGAcap  = ST_ID3_GENRES[ix].pCap;
			lenT    = st_sysStrlen2(pGAcap);
			lenR    = 1 + 3 + 1 + lenT;  /* '(xxx)YYYY' */
			gnrFRS  = -1;
			*pGenNr = gaNr;
		} else if (st_id3_isSpecGenre(pTstr, &ix)) {
			/* use ONLY these vals, forget about the parsed gnrFRS */
			pGAsid  = ST_ID3_GENRES_SPEC[ix].pIDstr;
			pGAcap  = ST_ID3_GENRES_SPEC[ix].pCap;
			lenR    = st_sysStrlen2(pGAsid) + 1 + st_sysStrlen2(pGAcap);  /* 'ZZ YYYY' */
			lenT    = lenR;
			gnrFRS  = -1;
			*pGenNr = -1;
		} else {
			if (lenR > 0)
				lenR += 2 + lenT;  /* 'xxx YYYY' */
			else
				lenR = lenT;  /* 'YYYY' */
			pTstr[0] = st_sysCharToUpper(pTstr[0]);
			/** */
		}
		if (*pGenNr == 255)
			*pGenNr = -1;
	}

	/* create new genre string */
	ST_REALLOC(*ppRstr, Tst_str*, lenR + 40, 1)
	if (*ppRstr == NULL) {
		ST_DELPOINT(pTstr)
		return ST_B_FALSE;
	}
	memset(*ppRstr, 0, lenR + 40);
	if (lenR > 0) {
		if (gnrFRS >= 0) {
			/* non-default genre */
			if (lenT > 0) {  /* 'xxx YYYY' */
				sprintf((char*)*ppRstr, "(%u)%s", (Tst_uint32)gnrFRS, pTstr);
				if (st_mtes_setStrTo(ST_MTES_TE_ISO, pTstr, pCap) != ST_ERR_SUCC)
					resB = ST_B_FALSE;
			} else {
				sprintf((char*)*ppRstr, "Unknown/%d", gnrFRS);
				if (st_mtes_setStrTo(ST_MTES_TE_ISO, *ppRstr, pCap) != ST_ERR_SUCC)
					resB = ST_B_FALSE;
				else
					sprintf((char*)*ppRstr, "(%u)Unknown/%d",
							(Tst_uint32)gnrFRS, gnrFRS);
			}
		} else if (lenT > 0 && pGAcap == NULL) {
			/* only caption */
			sprintf((char*)*ppRstr, "%s", pTstr);
			if (st_mtes_setStrTo(ST_MTES_TE_ISO, pTstr, pCap) != ST_ERR_SUCC)
				resB = ST_B_FALSE;
		} else if (gaNr < 255 && pGAcap != NULL) {
			/* default genre */
			sprintf((char*)*ppRstr, "(%u)%s", gaNr, pGAcap);
			if (st_mtes_setStrTo2(ST_MTES_TE_ISO, pGAcap, pCap) != ST_ERR_SUCC)
				resB = ST_B_FALSE;
		} else if (pGAsid != NULL && pGAcap != NULL) {
			/* default special genre */
			sprintf((char*)*ppRstr, "%s %s", pGAsid, pGAcap);
			if (st_mtes_setStrTo2(ST_MTES_TE_ISO, pGAcap, pCap) != ST_ERR_SUCC)
				resB = ST_B_FALSE;
		} else
			lenR = 0;
	}
	if (lenR == 0) {
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		resB = ST_B_FALSE;
	}
	ST_DELPOINT(pTstr)
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Check and correct Tracknr/Posnr
 *
 * pStr is ISO string, 1 <= len <= 20
 *
 * returns: true on success (if value is OK)
 */
Tst_bool
st_tagCFnc_chk_cTrack(Tst_mtes_string *pStr,
		ST_OPTARG(Tst_uint32 *pNum), ST_OPTARG(Tst_uint32 *pNumTot),
		ST_OPTARG(Tst_bool *pModded))
{
	Tst_bool      resB  = ST_B_TRUE,
	              chg   = ST_B_FALSE,
	              chgd  = ST_B_FALSE;
	Tst_str const *pISO = st_mtes_getDataPt(pStr);
	Tst_uint32    vl    = 0,
	              vr    = 0;
	Tst_str       vstr1[40 + 1],
	              vstr2[40 + 1],
	              *pVs  = (Tst_str*)&vstr1;

	ST_ASSERTN_BOOL(ST_B_FALSE, pStr == NULL)

	if (pNum != NULL)
		*pNum = 0;
	if (pNumTot != NULL)
		*pNumTot = 0;
	if (pModded != NULL)
		*pModded = ST_B_FALSE;
	if (pISO == NULL)
		return ST_B_FALSE;

	/* "9", "/", "/0", "/01", "/20", "0/", "20/", "20/30", "1//2", "1/2/2"... */
	memset(&vstr1, 0, 40);
	memset(&vstr2, 0, 40);
	while (*pISO) {
		if (isdigit((int)*pISO)) {
			if (chg) {
				*pVs = 0;
				pVs  = (Tst_str*)&vstr2;
				chg  = ST_B_FALSE;
				chgd = ST_B_TRUE;
			}
			*pVs++ = *pISO;
		} else {
			if (chgd)
				break;
			chg = ST_B_TRUE;
		}
		++pISO;
	}
	*pVs = 0;
	if (st_sysStrlen(vstr1) < 8)
		vl = (Tst_uint32)atoi((char*)&vstr1);
	if (st_sysStrlen(vstr2) < 8)
		vr = (Tst_uint32)atoi((char*)&vstr2);
	if (vl > ST_ID3_TFLD_TRACK_MAX_IV2)
		vl = 0;
	if (vr > ST_ID3_TFLD_TRACK_MAX_IV2)
		vr = 0;
	if (vl > vr)
		vr = 0;
	if (pNum != NULL)
		*pNum = vl;
	if (pNumTot != NULL)
		*pNumTot = vr;
	/* */
	if (vl < 1 && vr < 1) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		resB = ST_B_FALSE;
	} else {
		if (vr < 1)
			sprintf((char*)vstr1, "%u", vl);
		else
			sprintf((char*)vstr1, "%u/%u", vl, vr);
		if (! st_sysStrcmp2(ST_B_TRUE, vstr1, st_mtes_getDataPt(pStr))) {
			if (st_mtes_setStrTo(ST_MTES_TE_ISO, vstr1, pStr) != ST_ERR_SUCC)
				resB = ST_B_FALSE;
			if (pModded != NULL)
				*pModded = ST_B_TRUE;
		}
	}

	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Check and correct Time or Date
 *
 * pStr is ISO string, len == 4, time: HHMM (e.g. "2359")
 *                               date: DDMM (e.g. "3101")
 *
 * returns: true on success (if value is OK)
 */
Tst_bool
st_tagCFnc_chk_cTimeOrDate(Tst_mtes_string *pStr, const Tst_bool isTime,
		ST_OPTARG(Tst_tagCFnc_tstamp *pTStamp), ST_OPTARG(Tst_bool *pModded))
{
	Tst_bool   inv = ST_B_FALSE;
	Tst_uint32 val1,
	           val2;
	Tst_str    valS[3];
	Tst_tagCFnc_tstamp tst,
	                   *pTS  = &tst;

	ST_ASSERTN_BOOL(ST_B_FALSE, pStr == NULL)

	if (pTStamp != NULL)
		pTS = pTStamp;
	st_tagCFnc_stc_rsetTSta(pTS);
	if (pModded != NULL)
		*pModded = ST_B_FALSE;
	if (st_mtes_getDataPt(pStr) == NULL)
		return ST_B_FALSE;

	valS[2] = 0;
	memcpy(valS, st_mtes_getDataPt(pStr), 2);
	sscanf((char*)valS, "%u", &val1);
	memcpy(valS, &st_mtes_getDataPt(pStr)[2], 2);
	sscanf((char*)valS, "%u", &val2);
	if (isTime) {
		inv = (val1 > 23 || val2 > 59 || (val1 == 0 && val2 == 0));
		if (! inv) {
			pTS->hour = val1;
			pTS->min  = val2;
		}
	} else {
		inv = (val1 == 0 || val1 > 31 ||
				val2 == 0 || val2 > 12);
		if (! inv) {
			pTS->day   = val1;
			pTS->month = val2;
		}
	}

	if (inv) {
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		st_mtes_stc_rsetStr(pStr, ST_B_TRUE, ST_B_TRUE);
	}

	return (! inv);
}

/*----------------------------------------------------------------------------*/

/*
 * Check and correct Timestamp
 *
 * pStr is ISO string, 4 <= len <= 19
 *
 * returns: true on success (if value is OK)
 *          if pTStamp is != NULL then it will be set
 */
Tst_bool
st_tagCFnc_chk_cTStamp(Tst_mtes_string *pStr,
		ST_OPTARG(Tst_tagCFnc_tstamp *pTStamp), ST_OPTARG(Tst_bool *pModded))
{
	Tst_bool      resB  = ST_B_TRUE;
	Tst_str const *pS   = st_mtes_getDataPt(pStr);
	Tst_str       tmps[5],
	              tmpo[20];
	int           state = 0;
	Tst_uint32    cb    = 0,
	              ce    = 0,
	              ab[6] = {0, 0, 0, 0, 0, 0},
	              ae[6] = {0, 0, 0, 0, 0, 0},
	              ax    = 0,
	              tmpi,
	              leno  = 0;
	Tst_bool      cont  = ST_B_TRUE;
	Tst_tagCFnc_tstamp tst,
	                   *pTS  = &tst;

	ST_ASSERTN_BOOL(ST_B_FALSE, pStr == NULL)

	if (pTStamp != NULL)
		pTS = pTStamp;
	st_tagCFnc_stc_rsetTSta(pTS);
	if (pModded != NULL)
		*pModded = ST_B_FALSE;
	if (pS == NULL)
		return ST_B_FALSE;

	/* "2002-01-17T21:22:01" */
	while (*pS && cont) {
		switch (state) {
		case 0:  /* start */
			if (isdigit((int)*pS))
				state = 1;  /* year begun */
			else
				cont = ST_B_FALSE;
			break;
		case 1:  /* year, 2nd digit */
		case 2:  /* year, 3rd digit */
		case 3:  /* year, 4th digit */
			if (isdigit((int)*pS)) {
				++state;
				++ce;
			} else
				cont = ST_B_FALSE;
			break;
		case 4:  /* '-' */
			ab[ax]   = cb;
			ae[ax++] = ce;
			if (*pS == '-') {
				++state;
				++ce;
				cb = ce + 1;
			} else
				cont = ST_B_FALSE;
			break;
		case 5:  /* month, 1st digit */
		case 6:  /* month, 2nd digit */
		case 8:  /* day, 1st digit */
		case 9:  /* day, 2nd digit */
			if (isdigit((int)*pS)) {
				++state;
				++ce;
			} else
				cont = ST_B_FALSE;
			break;
		case 7:  /* '-' */
			ab[ax]   = cb;
			ae[ax++] = ce;
			if (*pS == '-') {
				++state;
				++ce;
				cb = ce + 1;
			} else
				cont = ST_B_FALSE;
			break;
		case 10:  /* 'T' or ' ' */
			ab[ax]   = cb;
			ae[ax++] = ce;
			if (*pS == 'T' || *pS == 't' || *pS == ' ') {
				++state;
				++ce;
				cb = ce + 1;
			} else
				cont = ST_B_FALSE;
			break;
		case 11:  /* hour, 1st digit */
		case 12:  /* hour, 2nd digit */
		case 14:  /* min, 1st digit */
		case 15:  /* min, 2nd digit */
		case 17:  /* sec, 1st digit */
		case 18:  /* sec, 2nd digit */
			if (isdigit((int)*pS)) {
				++state;
				++ce;
			} else
				cont = ST_B_FALSE;
			break;
		case 13:  /* ':' */
		case 16:  /* ':' */
			ab[ax]   = cb;
			ae[ax++] = ce;
			if (*pS == ':') {
				++state;
				++ce;
				cb = ce + 1;
			} else
				cont = ST_B_FALSE;
			break;
		}
		/* */
		++pS;
	}
	/* */
	if (! cont || state == 0 || state == 1 || state == 2 ||
			state == 3 || state == 5 || state == 6 || state == 8 ||
			state == 9 || state == 11 || state == 12 || state == 14 ||
			state == 15 || state == 17 || state == 18) {
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
	} else {
		ab[ax] = cb;
		ae[ax] = ce;
	}
	/* */
	memset(tmpo, 0, 20);
	for (ax = 0; ax < 6; ax++) {
		if (ae[ax] == 0)
			break;
		memset(tmps, 0, 5);
		memcpy(tmps, &st_mtes_getDataPt(pStr)[ab[ax]], ae[ax] - ab[ax] + 1);
		tmpi = (Tst_uint32)atoi((char*)tmps);
		switch (ax) {
		case 0:
			if (tmpi >= ST_ID3_TFLD_YEAR_MIN && tmpi <= ST_ID3_TFLD_YEAR_MAX) {
				pTS->year = tmpi;
				memcpy(tmpo, &st_mtes_getDataPt(pStr)[ab[ax]], 4);
				leno += 4;
			} else
				resB = ST_B_FALSE;
			break;
		case 1:
			if (tmpi > 0 && tmpi <= 12) {
				pTS->month   = (Tst_byte)tmpi;
				tmpo[leno++] = '-';
				memcpy(&tmpo[leno], &st_mtes_getDataPt(pStr)[ab[ax]], 2);
				leno += 2;
			}
			break;
		case 2:
			if (tmpi > 0 && tmpi <= 31) {
				pTS->day     = (Tst_byte)tmpi;
				tmpo[leno++] = '-';
				memcpy(&tmpo[leno], &st_mtes_getDataPt(pStr)[ab[ax]], 2);
				leno += 2;
			}
			break;
		case 3:
			if (/*tmpi >= 0 &&*/ tmpi < 24) {
				pTS->hour    = (Tst_byte)tmpi;
				tmpo[leno++] = 'T';
				memcpy(&tmpo[leno], &st_mtes_getDataPt(pStr)[ab[ax]], 2);
				leno += 2;
			}
			break;
		case 4:
			if (/*tmpi >= 0 &&*/ tmpi < 60) {
				pTS->min     = (Tst_byte)tmpi;
				tmpo[leno++] = ':';
				memcpy(&tmpo[leno], &st_mtes_getDataPt(pStr)[ab[ax]], 2);
				leno += 2;
			}
			break;
		default:
			if (/*tmpi >= 0 &&*/ tmpi < 60) {
				pTS->sec     = (Tst_byte)tmpi;
				tmpo[leno++] = ':';
				memcpy(&tmpo[leno], &st_mtes_getDataPt(pStr)[ab[ax]], 2);
				leno += 2;
			}
		}
		/* */
		if (! resB)
			break;
	}
	/* */
	resB = resB && (leno >= 4);
	if (resB) {
		++leno;
		if (st_mtes_getStringSize(pStr) != leno) {
			if (pModded != NULL)
				*pModded = ST_B_TRUE;
		}
		if (st_mtes_copyFromCharp(tmpo, leno, ST_MTES_TE_ISO, pStr) != ST_ERR_SUCC)
			return ST_B_FALSE;
	}

	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Check people mapping
 *
 * returns: true on success (if value-pairs are OK)
 */
Tst_bool
st_tagCFnc_chk_cPeopleArr(Tst_mtes_strArr *pPeopleArr,
		ST_OPTARG(Tst_bool *pModded))
{
#	define LOC_SETTO_(pStr)  {resF = st_mtes_setStrTo2(te, "-", pStr); \
			if (resF != ST_ERR_SUCC) {st_mtes_stc_freeStr(&tmpstr); return ST_B_FALSE;}}
	Tst_err    resF;
	Tst_uint32 x,
	           pcnt = 0;
	Tst_bool   chgd = ST_B_FALSE,
	           chgdIE,
	           invmOK,
	           inveOK;
	Tst_mtes_textEnc te;
	Tst_mtes_string  tmpstr,
	                 *pInvm,
	                 *pInve;

	if (pModded != NULL)
		*pModded = ST_B_FALSE;

	resF = st_mtes_stc_initStr(&tmpstr);
	if (resF != ST_ERR_SUCC)
		return ST_B_FALSE;

	/* split up single string into array ? */
	if (st_mtes_strArrGetElemCount(pPeopleArr) == 1) {
		resF = st_tagCFnc_iplsSplit(pPeopleArr);
		if (resF != ST_ERR_SUCC)
			return ST_B_FALSE;
		chgd = (st_mtes_strArrGetElemCount(pPeopleArr) != 1);
	}

	/* check string pairs */
	x = 1;
	while (x <= st_mtes_strArrGetElemCount(pPeopleArr)) {
		/* 'involvement' */
		pInvm  = st_mtes_strArrGetElem(pPeopleArr, x);
		te     = st_mtes_getEncodingCur(pInvm);
		/**st_id3v2_prStr("invm= ", pInvm);**/
		chgdIE = ST_B_FALSE;
		invmOK = st_tagCFnc_chk_cPeopleEntry(pInvm, &chgdIE);
		if (pModded != NULL && chgdIE)
			*pModded = ST_B_TRUE;
		if (! invmOK || st_mtes_isStrEmptyCh(pInvm)) {
			if (x + 1 <= st_mtes_strArrGetElemCount(pPeopleArr)) {
				/* if other string follows
				 *  shouldn't have been empty */
				invmOK = ST_B_FALSE;
			} else {  /* if this is the last string */
				chgd = ST_B_TRUE;
				if ((resF = st_mtes_strArrRem(pPeopleArr, x)) != ST_ERR_SUCC) {
					st_mtes_stc_freeStr(&tmpstr);
					return ST_B_FALSE;
				}
				break;
			}
		}
		/* 'involvee' */
		inveOK = ST_B_FALSE;
		if (x + 1 <= st_mtes_strArrGetElemCount(pPeopleArr)) {
			pInve  = st_mtes_strArrGetElem(pPeopleArr, x + 1);
			/**st_id3v2_prStr("inve= ", pInve);**/
			chgdIE = ST_B_FALSE;
			inveOK = st_tagCFnc_chk_cPeopleEntry(pInve, &chgdIE);
			if (pModded != NULL && chgdIE)
				*pModded = ST_B_TRUE;
			if (inveOK && st_mtes_isStrEmptyCh(pInve)) {
				/* shouldn't have been empty */
				inveOK = ST_B_FALSE;
			}
		} else
			pInve = &tmpstr;
		if (! invmOK && inveOK) {
			/* set 'involvement' to "-" */
			LOC_SETTO_(pInvm)
			chgd = ST_B_TRUE;
		} else if (invmOK && ! inveOK) {
			/* set 'involvee' to "-" */
			LOC_SETTO_(pInve)
			chgd = ST_B_TRUE;
			if (x + 1 > st_mtes_strArrGetElemCount(pPeopleArr)) {
				resF = st_mtes_strArrAdd(pPeopleArr, pInve);
				if (resF != ST_ERR_SUCC) {
					st_mtes_stc_freeStr(&tmpstr);
					return ST_B_FALSE;
				}
			}
		} else if (! (invmOK || inveOK)) {
			/* remove pair */
			chgd = ST_B_TRUE;
			resF = st_mtes_strArrRem(pPeopleArr, x);
			if (resF == ST_ERR_SUCC)
				resF = st_mtes_strArrRem(pPeopleArr, x);
			if (resF != ST_ERR_SUCC) {
				st_mtes_stc_freeStr(&tmpstr);
				return ST_B_FALSE;
			}
			continue;  /* x remains the same */
		}
		++pcnt;
		/* */
		x += 2;
	}
	/* */
	if (pModded != NULL && chgd)
		*pModded = ST_B_TRUE;
	/* */
	st_mtes_stc_freeStr(&tmpstr);

	return (pcnt > 0);
#	undef LOC_SETTO_
}

/*----------------------------------------------------------------------------*/

/*
 * Check and correct IPLS entry (Involvement or Involvee)
 *
 * returns: true on success (if value is OK)
 */
Tst_bool
st_tagCFnc_chk_cPeopleEntry(Tst_mtes_string *pEntry,
		ST_OPTARG(Tst_bool *pModded))
{
	Tst_bool resB,
	         modStrip = ST_B_FALSE,
	         modRep   = ST_B_FALSE;
	Tst_mtes_strRestr restr;

	ST_ASSERTN_BOOL(ST_B_FALSE, pEntry == NULL)

	if (pModded != NULL)
		*pModded = ST_B_FALSE;

	if (st_mtes_strReplace(pEntry,
			(const Tst_str*)";", (const Tst_str*)" ", &modRep) != ST_ERR_SUCC)
		return ST_B_FALSE;
	if (pModded != NULL && modRep)
		*pModded = ST_B_TRUE;
	if (st_mtes_strReplace(pEntry,
			(const Tst_str*)"|", (const Tst_str*)" ", &modRep) != ST_ERR_SUCC)
		return ST_B_FALSE;
	if (pModded != NULL && modRep)
		*pModded = ST_B_TRUE;
	if (st_mtes_strReplace(pEntry,
			(const Tst_str*)":", (const Tst_str*)" ", &modRep) != ST_ERR_SUCC)
		return ST_B_FALSE;
	if (pModded != NULL && modRep)
		*pModded = ST_B_TRUE;

	restr.hasR     = ST_B_TRUE;
	restr.ruleAlw  = ST_B_TRUE;
	st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
	restr.disNewln = ST_B_TRUE;  /* disallow '\n' */
	restr.disTab   = ST_B_TRUE;  /* disallow '\t' */

	if (st_mtes_stripStr(pEntry, &restr, NULL, &modStrip) != ST_ERR_SUCC)
		return ST_B_FALSE;
	if (pModded != NULL && modStrip)
		*pModded = ST_B_TRUE;

	resB = ! st_mtes_isStrEmptyBy(pEntry);
	if (resB) {
		Tst_mtes_string emptyS;

		st_mtes_stc_initStr(&emptyS);
		if (st_mtes_setStrTo(ST_MTES_TE_UTF16LE,
					(const Tst_str*)"-", &emptyS) != ST_ERR_SUCC)
			resB = ST_B_FALSE;
		else
			resB = ! st_mtes_strEqual(pEntry, &emptyS, ST_B_TRUE);
		st_mtes_stc_freeStr(&emptyS);
	}
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Check and correct Beats per Minute
 *
 * pStr is ISO string, 1 <= len <= 8
 *
 * returns: true on success (if value is OK)
 */
Tst_bool
st_tagCFnc_chk_cBPM(Tst_mtes_string *pStr, ST_OPTARG(Tst_bool *pModded))
{
	Tst_bool  resB = ST_B_TRUE;
	Tst_int32 bpm  = 0;
	Tst_str   bpmstr[40];

	ST_ASSERTN_BOOL(ST_B_FALSE, pStr == NULL)

	if (pModded != NULL)
		*pModded = ST_B_FALSE;

	bpm = st_mtes_toInt32(pStr);
	if (bpm < 1) {
		st_mtes_stc_rsetStr(pStr, ST_B_FALSE, ST_B_FALSE);
		if (pModded != NULL)
			*pModded = ST_B_TRUE;
		resB = ST_B_FALSE;
	} else {
		sprintf((char*)bpmstr, "%u", (Tst_uint32)bpm);
		if (! st_sysStrcmp2(ST_B_TRUE, bpmstr, st_mtes_getDataPt(pStr))) {
			if (st_mtes_setStrTo(ST_MTES_TE_ISO, bpmstr, pStr) != ST_ERR_SUCC)
				resB = ST_B_FALSE;
			if (pModded != NULL)
				*pModded = ST_B_TRUE;
		}
	}
	return resB;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get either Genre-Caption or -Number
 *
 * pGin      MUST be set
 * pCapOut   may be NULL
 * pGNrOut   may be NULL
 *
 * returns: ERRCODE
 *          if pCapOut is != NULL then it will contain
 *            the Genre-Caption and be encoded as textEncOut
 *          if pGNrOut is != NULL then it will contain the Genre-Number
 */
static Tst_err
ST_TCFNC__chk_getGenre(const Tst_mtes_string *pGin, Tst_mtes_string *pCapOut,
		const Tst_mtes_textEnc textEncOut, Tst_int32 *pGNrOut,
		ST_OPTARG(Tst_bool *pModded))
{
	Tst_err   res    = ST_ERR_SUCC;
	Tst_str   *pRstr = NULL;
	Tst_int32 gnrTmp = -1,
	          *pGNr  = &gnrTmp;
	Tst_mtes_string       tmpI,
	                      cOutTmp,
	                      *pCO = &cOutTmp;
	Tst_mtes_string const *pI  = pGin;

	ST_ASSERTN_IARG(pGin == NULL &&
			! (pCapOut != NULL || pGNrOut != NULL))

	if (pModded != NULL)
		*pModded = ST_B_FALSE;
	if (pCapOut != NULL) {
		pCO = pCapOut;
		st_mtes_stc_rsetStr(pCapOut, ST_B_TRUE, ST_B_TRUE);
	} else {
		res = st_mtes_stc_initStr(&cOutTmp);
		if (res != ST_ERR_SUCC)
			return res;
		*pGNrOut = -1;
		pGNr     = pGNrOut;
	}

	res = st_mtes_stc_initStr(&tmpI);
	if (res != ST_ERR_SUCC) {
		if (pCapOut == NULL)
			st_mtes_stc_freeStr(&cOutTmp);
		return res;
	}

	if (st_mtes_getEncodingCur(pGin) != ST_MTES_TE_ISO) {
		res = st_mtes_convStr_c(pGin, &tmpI, ST_MTES_TE_ISO);
		if (res != ST_ERR_SUCC) {
			st_mtes_stc_freeStr(&tmpI);
			if (pCapOut == NULL)
				st_mtes_stc_freeStr(&cOutTmp);
			return ST_ERR_FAIL;
		}
		pI = &tmpI;
	}
	if (! st_tagCFnc_chk_cGenre_sub(pI, pCO, pGNr, &pRstr, pModded)) {
		ST_DELPOINT(pRstr)
		st_mtes_stc_freeStr(&tmpI);
		if (pCapOut == NULL)
			st_mtes_stc_freeStr(&cOutTmp);
		return ST_ERR_FAIL;
	}
	ST_DELPOINT(pRstr)
	st_mtes_stc_freeStr(&tmpI);
	if (pCapOut == NULL)
		st_mtes_stc_freeStr(&cOutTmp);
	else if (st_mtes_getEncodingCur(pCapOut) != textEncOut)
		res = st_mtes_convStr(pCapOut, textEncOut);
	return res;
}

/******************************************************************************/
