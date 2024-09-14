/******************************************************************************/
/* tag_comfnc.c                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Common functions for Tags                                                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 04.12.2010 (dd.mm.yyyy)                                     */
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
#include "src/includes/common/streamwr.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_comfnc.h"
/** */
#define SRC_TAG_COMFNC_C
#include "tag_comfnc-prot.h"
/*** */
#include "tag_comfnc_chk-pp.h"
#include "tag_zfdeb-prot.h"
#undef SRC_TAG_COMFNC_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_tagCFnc_stc_rsetTSta(Tst_tagCFnc_tstamp *pTSt)
{
	ST_ASSERTN_VOID(pTSt == NULL)

	pTSt->year  = 0;
	pTSt->month = 0;
	pTSt->day   = 0;
	pTSt->hour  = 0;
	pTSt->min   = 0;
	pTSt->sec   = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * Merge IPLS multi-string into one single string
 *   The input array should contain something like this
 *     arr[0]:A, arr[1]:B, arr[2]:C, arr[3]:D, ...
 *   The output string will have the format "A:B; C:D; ..."
 *
 * returns: ERRCODE
 *          the merged string will reside within the first element of pSArr
 */
Tst_err
st_tagCFnc_iplsMerge(Tst_mtes_strArr *pSArr)
{
	const Tst_mtes_textEnc cTE_INT = ST_MTES_TE_UTF16LE;
	Tst_err    res;
	Tst_uint32 x,
	           ecnt;
	Tst_mtes_string tmpS1,
	                tmpS2,
	                tmpS3,
	                *pElemX;

	ST_ASSERTN_IARG(pSArr == NULL)

	ecnt = st_mtes_strArrGetElemCount(pSArr);
	if (ecnt < 2)
		return ST_ERR_SUCC;

	/* init some stuff */
	/** */
	res = st_mtes_stc_initStr(&tmpS1);
	if (res != ST_ERR_SUCC)
		return res;
	res = st_mtes_stc_initStr(&tmpS2);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmpS1);
		return res;
	}
	res = st_mtes_stc_initStr(&tmpS3);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tmpS1);
		st_mtes_stc_freeStr(&tmpS2);
		return res;
	}
	/** */
	res = st_mtes_setStrTo2(cTE_INT, ":", &tmpS1);
	if (res == ST_ERR_SUCC)
		res = st_mtes_setStrTo2(cTE_INT, "; ", &tmpS2);
	if (res == ST_ERR_SUCC)
		res = st_mtes_setStrTo2(cTE_INT, "-", &tmpS3);

	/**{
		char dbgmsg[128];

		snprintf(dbgmsg, sizeof(dbgmsg), "%s(): org:", __func__);
		st_tagFDeb_prStrArr(dbgmsg, pSArr);
	}**/

	/* do the merging */
	if (res == ST_ERR_SUCC) {
		Tst_bool doOneExtra;

		doOneExtra = (ecnt % 2 != 0);
		if (doOneExtra)
			++ecnt;
		if (st_mtes_isStrEmptyCh(st_mtes_strArrGetElem(pSArr, 1))) {
			/* set first elem to "-" if empty */
			st_mtes_strCpy(&tmpS3, st_mtes_strArrGetElem(pSArr, 1));
		}
		for (x = 2; x <= ecnt; x++) {
			if (x % 2 == 0) {
				/* elem1 = elem1 + ":" */
				res = st_mtes_strConcat(&tmpS1, st_mtes_strArrGetElem(pSArr, 1));
			} else {
				/* elem1 = elem1 + "; " */
				res = st_mtes_strConcat(&tmpS2, st_mtes_strArrGetElem(pSArr, 1));
			}
			if (res != ST_ERR_SUCC)
				break;
			/* */
			if (! doOneExtra || x < ecnt)
				pElemX = st_mtes_strArrGetElem(pSArr, x);
			else
				pElemX = NULL;
			if (pElemX != NULL && ! st_mtes_isStrEmptyCh(pElemX))
				st_tagCFnc_chk_cPeopleEntry(pElemX, NULL);
			if (pElemX == NULL || st_mtes_isStrEmptyCh(pElemX)) {
				/* elem1 = elem1 + '-' */
				res = st_mtes_strConcat(&tmpS3, st_mtes_strArrGetElem(pSArr, 1));
			} else {
				/* elem1 = elem1 + elemX */
				res = st_mtes_strConcat(pElemX, st_mtes_strArrGetElem(pSArr, 1));
			}
			if (res != ST_ERR_SUCC)
				break;
		}
	}

	st_mtes_stc_freeStr(&tmpS1);
	st_mtes_stc_freeStr(&tmpS2);
	st_mtes_stc_freeStr(&tmpS3);

	/* remove all but the first string from the array */
	if (res == ST_ERR_SUCC)
		for (x = ecnt; x > 1; x--)
			st_mtes_strArrRem(pSArr, x);
	/**{
		char dbgmsg[128];

		snprintf(dbgmsg, sizeof(dbgmsg), "%s(): merged:", __func__);
		st_tagFDeb_prStrArr(dbgmsg, pSArr);
	}**/

	return res;
}

/**
 * Split IPLS string up into multi-string
 *   The input string should have the format
 *     "A:B; C:D; ..." or "A:B;C:D;..." or "A:B| C:D| ..." or "A:B|C:D|..."
 *     and reside within the first element of pSArr
 *   The output array will contain something like this
 *     arr[0]:A, arr[1]:B, arr[2]:C, arr[3]:D, ...
 *
 * returns: ERRCODE
 *          pSArr will contain the individual entries
 */
Tst_err
st_tagCFnc_iplsSplit(Tst_mtes_strArr *pSArr)
{
	const Tst_str          *cNEEDLE1A = (const Tst_str*)";",
	                       *cNEEDLE1B = (const Tst_str*)"|",
	                       *cNEEDLE2  = (const Tst_str*)":";
	const Tst_mtes_textEnc cTE_INT    = ST_MTES_TE_UTF16LE;
	Tst_err res;
	Tst_mtes_strArr  newArr;
	Tst_mtes_string  *pSrcElem,
	                 tmpSrc,
	                 remStr,
	                 elemP,
	                 elemPl,
	                 elemPr,
	                 emptyS;
	Tst_mtes_textEnc teOrg = ST_MTES_TE_ISO;

	ST_ASSERTN_IARG(pSArr == NULL)

	if (st_mtes_strArrGetElemCount(pSArr) == 0)
		return ST_ERR_SUCC;

	res = st_mtes_stc_initSA(&newArr);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&tmpSrc);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&remStr);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&elemP);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&elemPl);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&elemPr);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&emptyS);
	if (res != ST_ERR_SUCC)
		return res;

	pSrcElem = st_mtes_strArrGetElem(pSArr, 1);
	teOrg    = st_mtes_getEncodingCur(pSrcElem);
	if (teOrg != cTE_INT) {
		res      = st_mtes_convStr_c(pSrcElem, &tmpSrc, cTE_INT);
		pSrcElem = &tmpSrc;
	}
	if (res == ST_ERR_SUCC)
		res = st_mtes_setStrTo2(cTE_INT, "-", &emptyS);
	/* pSrcElem is now encoded as cTE_INT */

	if (res == ST_ERR_SUCC) {
		Tst_bool   hasSa,
		           hasSb;
		Tst_uint32 offsChA,
		           offsChB;

		/**{
			char dbgmsg[128];

			snprintf(dbgmsg, sizeof(dbgmsg), "%s(): org:", __func__);
			st_tagFDeb_prStr(dbgmsg, pSrcElem);
		}**/
		do {
			/* get next element-pair */
			/** search for ";" or "|" */
			/**st_tagFDeb_prStr("pSrcElem= ", pSrcElem);**/
			if (st_mtes_isStrEmptyBy(pSrcElem))
				break;
			offsChA = 0;
			offsChB = 0;
			hasSa   = st_mtes_strPos(pSrcElem, cNEEDLE1A, &offsChA);
			hasSb   = st_mtes_strPos(pSrcElem, cNEEDLE1B, &offsChB);
			if (hasSa && hasSb) {
				if (offsChA <= offsChB)
					hasSb = ST_B_FALSE;
				else
					hasSa = ST_B_FALSE;
			}
			/** get element-pair (str="E1:E2;E3:E4" --> elemP="E1:E2",remStr="E3:E4") */
			if (hasSa || hasSb) {
				res = st_mtes_strSplit(pSrcElem, (hasSa ? cNEEDLE1A : cNEEDLE1B),
							&elemP, &remStr);
			} else {
				st_mtes_stc_rsetStr(&remStr, ST_B_TRUE, ST_B_TRUE);
				res = st_mtes_strCpy(pSrcElem, &elemP);
			}
			if (res != ST_ERR_SUCC)
				break;
			/**st_tagFDeb_prStr(" elemP= ", &elemP);
			st_tagFDeb_prStr(" remStr= ", &remStr);**/

			/* split up element-pair */
			/** search for ":" */
			if (! st_mtes_isStrEmptyBy(&elemP)) {
				Tst_bool tmpIsStrEmpty[2];
				int      relat[2] = {0, 0};

				hasSa = st_mtes_strPos(&elemP, cNEEDLE2, NULL);
				/* split (elemP="E1:E2" --> elemPl="E1",elemPr="E2") */
				if (hasSa) {
					res = st_mtes_strSplit(&elemP, cNEEDLE2, &elemPl, &elemPr);
					/**st_tagFDeb_prStr(" split elemPl= ", &elemPl);
					st_tagFDeb_prStr(" split elemPr= ", &elemPr);**/
				} else {
					st_mtes_stc_rsetStr(&elemPr, ST_B_TRUE, ST_B_TRUE);
					res = st_mtes_strCpy(&elemP, &elemPl);
				}
				if (res != ST_ERR_SUCC)
					continue;  /* --> end while() */

				/* check/strip elemPl and elemPr */
				st_tagCFnc_chk_cPeopleEntry(&elemPl, NULL);
				st_tagCFnc_chk_cPeopleEntry(&elemPr, NULL);

				/* set elemPl/elemPr to "-" if empty */
				tmpIsStrEmpty[0] = st_mtes_isStrEmptyCh(&elemPl);
				tmpIsStrEmpty[1] = st_mtes_isStrEmptyCh(&elemPr);
				if (! tmpIsStrEmpty[0]) {
					res = st_mtes_strCompare(&elemPl, &emptyS, ST_B_TRUE, &relat[0]);
					/* */
					tmpIsStrEmpty[0] = (relat[0] == 0);
				}
				if (res == ST_ERR_SUCC && ! tmpIsStrEmpty[1]) {
					res = st_mtes_strCompare(&elemPr, &emptyS, ST_B_TRUE, &relat[1]);
					/* */
					tmpIsStrEmpty[1] = (relat[1] == 0);
				}
				if (res != ST_ERR_SUCC)
					continue;  /* --> end while() */
				if (! (tmpIsStrEmpty[0] && tmpIsStrEmpty[1])) {
					if (tmpIsStrEmpty[0] && relat[0] != 0)
						st_mtes_strCpy(&emptyS, &elemPl);
					else if (tmpIsStrEmpty[1] && relat[1] != 0)
						st_mtes_strCpy(&emptyS, &elemPr);

					/* convert elemPl and elemPr back to teOrg */
					if (teOrg != cTE_INT)
						res = st_mtes_convStr(&elemPl, teOrg);
					if (res == ST_ERR_SUCC && teOrg != cTE_INT)
						res = st_mtes_convStr(&elemPr, teOrg);
					/* add elemPl and elemPr to array */
					if (res == ST_ERR_SUCC) {
						/**st_tagFDeb_prStr("   adding: elemPl= ", &elemPl);**/
						res = st_mtes_strArrAdd(&newArr, &elemPl);
					}
					if (res == ST_ERR_SUCC) {
						/**st_tagFDeb_prStr("   adding: elemPr= ", &elemPr);**/
						res = st_mtes_strArrAdd(&newArr, &elemPr);
					}
					if (res != ST_ERR_SUCC)
						continue;  /* --> end while() */
				}
			}

			/* copy remainer to tmpSrc */
			res      = st_mtes_strCpy(&remStr, &tmpSrc);
			pSrcElem = &tmpSrc;
		} while (res == ST_ERR_SUCC);

		/**{
			char dbgmsg[128];

			snprintf(dbgmsg, sizeof(dbgmsg), "%s(): splitted:", __func__);
			st_tagFDeb_prStrArr(dbgmsg, &newArr);
		}**/
	}

	if (res == ST_ERR_SUCC)
		res = st_mtes_strArrCpy(&newArr, pSArr);

	st_mtes_stc_freeSA(&newArr);
	st_mtes_stc_freeStr(&tmpSrc);
	st_mtes_stc_freeStr(&remStr);
	st_mtes_stc_freeStr(&elemP);
	st_mtes_stc_freeStr(&elemPl);
	st_mtes_stc_freeStr(&elemPr);
	st_mtes_stc_freeStr(&emptyS);
	return res;
}

/**
 * Convert an ID3v2 Picture Type to a Vorbis Picture Type
 *
 * @param srcPicTp Source picture type
 * @return Converted picture type
 */
Tst_vorbc_picTp
st_tagCFnc_convId3v2ToVorbc_picTp(const Tst_id3v2_picTp srcPicTp) {
	switch (srcPicTp) {
		case ST_ID3V2_PT_OTHER:
			return ST_VORBC_PT_OTHER;
		case ST_ID3V2_PT_3232:
			return ST_VORBC_PT_3232;
		case ST_ID3V2_PT_OFICON:
			return ST_VORBC_PT_OFICON;
		case ST_ID3V2_PT_COVFRO:
			return ST_VORBC_PT_COVFRO;
		case ST_ID3V2_PT_COVBAC:
			return ST_VORBC_PT_COVBAC;
		case ST_ID3V2_PT_LEAFL:
			return ST_VORBC_PT_LEAFL;
		case ST_ID3V2_PT_MEDIA:
			return ST_VORBC_PT_MEDIA;
		case ST_ID3V2_PT_LEADART:
			return ST_VORBC_PT_LEADART;
		case ST_ID3V2_PT_ART:
			return ST_VORBC_PT_ART;
		case ST_ID3V2_PT_COND:
			return ST_VORBC_PT_COND;
		case ST_ID3V2_PT_BAND:
			return ST_VORBC_PT_BAND;
		case ST_ID3V2_PT_COMP:
			return ST_VORBC_PT_COMP;
		case ST_ID3V2_PT_LYRIC:
			return ST_VORBC_PT_LYRIC;
		case ST_ID3V2_PT_RECLOC:
			return ST_VORBC_PT_RECLOC;
		case ST_ID3V2_PT_DURREC:
			return ST_VORBC_PT_DURREC;
		case ST_ID3V2_PT_DURPER:
			return ST_VORBC_PT_DURPER;
		case ST_ID3V2_PT_SCRCAP:
			return ST_VORBC_PT_SCRCAP;
		case ST_ID3V2_PT_FLASH:
			return ST_VORBC_PT_FLASH;
		case ST_ID3V2_PT_ILLUS:
			return ST_VORBC_PT_ILLUS;
		case ST_ID3V2_PT_BLOGO:
			return ST_VORBC_PT_BLOGO;
		case ST_ID3V2_PT_PLOGO:
			return ST_VORBC_PT_PLOGO;
		default:  // ST_ID3V2_PT_NONE
			return ST_VORBC_PT_NONE;
	}
}

/**
 * Convert a Vorbis Picture Type to an ID3v2 Picture Type
 *
 * @param srcPicTp Source picture type
 * @return Converted picture type
 */
Tst_id3v2_picTp
st_tagCFnc_convVorbcToId3v2_picTp(const Tst_vorbc_picTp srcPicTp) {
	switch (srcPicTp) {
		case ST_VORBC_PT_OTHER:
			return ST_ID3V2_PT_OTHER;
		case ST_VORBC_PT_3232:
			return ST_ID3V2_PT_3232;
		case ST_VORBC_PT_OFICON:
			return ST_ID3V2_PT_OFICON;
		case ST_VORBC_PT_COVFRO:
			return ST_ID3V2_PT_COVFRO;
		case ST_VORBC_PT_COVBAC:
			return ST_ID3V2_PT_COVBAC;
		case ST_VORBC_PT_LEAFL:
			return ST_ID3V2_PT_LEAFL;
		case ST_VORBC_PT_MEDIA:
			return ST_ID3V2_PT_MEDIA;
		case ST_VORBC_PT_LEADART:
			return ST_ID3V2_PT_LEADART;
		case ST_VORBC_PT_ART:
			return ST_ID3V2_PT_ART;
		case ST_VORBC_PT_COND:
			return ST_ID3V2_PT_COND;
		case ST_VORBC_PT_BAND:
			return ST_ID3V2_PT_BAND;
		case ST_VORBC_PT_COMP:
			return ST_ID3V2_PT_COMP;
		case ST_VORBC_PT_LYRIC:
			return ST_ID3V2_PT_LYRIC;
		case ST_VORBC_PT_RECLOC:
			return ST_ID3V2_PT_RECLOC;
		case ST_VORBC_PT_DURREC:
			return ST_ID3V2_PT_DURREC;
		case ST_VORBC_PT_DURPER:
			return ST_ID3V2_PT_DURPER;
		case ST_VORBC_PT_SCRCAP:
			return ST_ID3V2_PT_SCRCAP;
		case ST_VORBC_PT_FLASH:
			return ST_ID3V2_PT_FLASH;
		case ST_VORBC_PT_ILLUS:
			return ST_ID3V2_PT_ILLUS;
		case ST_VORBC_PT_BLOGO:
			return ST_ID3V2_PT_BLOGO;
		case ST_VORBC_PT_PLOGO:
			return ST_ID3V2_PT_PLOGO;
		default:  // ST_VORBC_PT_NONE
			return ST_ID3V2_PT_NONE;
	}
}

/*----------------------------------------------------------------------------*/

/**
 * Get a string containing the name of the shared library and its version
 *
 * @param pTagger Output string (e.g. 'libsantag x.y.z' or 'libsantag: x.y.z')
 * @param withColon Add a colon after the library name?
 * @return Error code
 */
Tst_err
st_tagCFnc_getTaggerStr(Tst_mtes_string *pTagger, Tst_bool withColon)
{
	char myVend[128];

	ST_ASSERTN_IARG(pTagger == NULL)

	snprintf(myVend, sizeof(myVend),
			"%s%s %s", ST_LIBSANTAG_NAME, withColon ? ":" : "", ST_LIBSANTAG_VERS_STRING);
	return st_mtes_copyFromCharp_iso((const Tst_str*)myVend, pTagger);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * Read a string from the pStrrd
 *
 * pStrrd      MUST be set
 * pCopied     MUST be != NULL
 * ppChar      MUST be != NULL
 * pPCharLen   MUST be != NULL
 * pHadBOM     may be NULL if charWid != 2
 */
Tst_err
st_tagCFnc_readStr(const Tst_uint32 charWid,
		const Tst_mtes_textEnc textEnc,
		Tst_streamrd *pStrrd,
		const Tst_uint32 maxCopy, Tst_uint32 *pCopied,
		Tst_str **ppChar, Tst_uint32 *pPCharLen,
		ST_OPTARG(Tst_bool *pHadBOM))
{
	Tst_bool   isTerm    = ST_B_FALSE;
	Tst_buf    tmpBuf[2048];
	Tst_uint32 slenOne,
	           sszOne,
	           rdFromStr = 0,
	           toCp      = sizeof(tmpBuf),
	           dstSize   = 0,
	           x,
	           plus      = (charWid == 1 ? 0 : 2);

	ST_ASSERTN_IARG((charWid == 2 && pHadBOM == NULL) ||
			pStrrd == NULL || pCopied == NULL || ppChar == NULL ||
			pPCharLen == NULL)

	*pPCharLen = 0;
	*pCopied   = 0;
	if (charWid == 2)
		*pHadBOM = ST_B_FALSE;

	/* copy some bytes from pStrrd to *ppChar */
	while (*pCopied < maxCopy && ! st_streamrd_isEOF(pStrrd)) {
		if (*pCopied + toCp > maxCopy) {
			toCp = maxCopy - *pCopied;
			if ((toCp % charWid) != 0)
				--toCp;
		}
		st_streamrd_rdAheadBuffer(pStrrd, toCp, tmpBuf, &rdFromStr);
		if (charWid == 2 && rdFromStr > 0 && (rdFromStr % charWid) != 0)
			--rdFromStr;
		if (rdFromStr == 0)
			break;
		/* */
		slenOne = rdFromStr;
		if (charWid == 2) {
			if (*pPCharLen == 0 && ! *pHadBOM && rdFromStr >= 2 &&
					((tmpBuf[0] == ST_UTF16_BOM_BE[0] &&
							tmpBuf[1] == ST_UTF16_BOM_BE[1]) ||
						(tmpBuf[0] == ST_UTF16_BOM_LE[0] &&
							tmpBuf[1] == ST_UTF16_BOM_LE[1]))) {
				*pHadBOM = ST_B_TRUE;
				plus     = 0;
			}
		}
		for (x = 0; x + (charWid - 1) < rdFromStr; x += charWid) {
			if (tmpBuf[x] == 0 && (charWid == 1 || tmpBuf[x + 1] == 0)) {
				isTerm  = ST_B_TRUE;
				slenOne = x;
				break;
			}
		}
		sszOne = slenOne + (isTerm ? charWid : 0);
		/* */
		if (*pCopied + sszOne + 2 * charWid > dstSize) {
			ST_REALLOC(*ppChar, Tst_str*, *pCopied + sszOne + 2 * charWid, 1)
			if (*ppChar == NULL)
				return ST_ERR_OMEM;
			dstSize = *pCopied + sszOne + 2 * charWid;
		}
		if (slenOne > 0) {
			*pPCharLen += slenOne;
			memcpy(&(*ppChar)[*pCopied + plus], tmpBuf, slenOne);
		}
		/* */
		st_streamrd_rdSkipBytes(pStrrd, sszOne, ST_B_TRUE);
		*pCopied += sszOne;
		/* */
		if (isTerm || rdFromStr != sizeof(tmpBuf))
			break;
	}

	/* */
	if (dstSize == 0) {
		ST_REALLOC(*ppChar, Tst_str*, 2 * charWid, 1)
		if (*ppChar == NULL)
			return ST_ERR_OMEM;
		*pPCharLen = 0;
		if (charWid == 2)
			*pHadBOM = ST_B_FALSE;
	}
	/* */
	if (charWid == 2 && ! *pHadBOM) {
		*pPCharLen += charWid;
		/* insert BOM */
		if (textEnc == ST_MTES_TE_UTF16BE)
			memcpy(*ppChar, &ST_UTF16_BOM_BE, charWid);
		else
			memcpy(*ppChar, &ST_UTF16_BOM_LE, charWid);
	}
	/* set NUL-term */
	(*ppChar)[*pPCharLen] = 0;
	if (charWid == 2)
		(*ppChar)[*pPCharLen + 1] = 0;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/**
 * Does the string in pStr end with three dots ?
 */
Tst_err
st_tagCFnc_hasTripleDotEnding(const Tst_mtes_string *pStr, Tst_bool *pHasTD)
{
	const char *cDOTDOT = "...";
	Tst_err    res;
	Tst_uint32 len;
	Tst_str    *pTmpU8 = NULL;

	*pHasTD = ST_B_FALSE;
	res     = st_mtes_copyToCharp_utf8(pStr, &pTmpU8);
	if (res == ST_ERR_SUCC) {
		len = st_sysStrlen(pTmpU8);
		if (len >= 3 && memcmp(&pTmpU8[len - 3], cDOTDOT, 3) == 0)
			*pHasTD = ST_B_TRUE;
	}
	ST_DELPOINT(pTmpU8)
	return res;
}

/*----------------------------------------------------------------------------*/

/**
 * Write Tag Buffer to stream
 *
 * @param pBasOpts    MUST be set
 * @param pOpts       MUST be set
 * @param cbDbg       MUST be != NULL
 * @param cbErr       MUST be != NULL
 * @param pTagFilen   MUST be != NULL
 * @param pTBas       MUST be set
 * @param pStrwr      may be NULL if pBasOpts->pretWr==TRUE
 * @param pTagBO      MUST be set
 * @return Error code
 */
Tst_err
st_tagCFnc_writeTagBOtoStream(const Tst_basOpts *pBasOpts, const void *pOpts,
		const char *pFnc,
		Tst_tagCFnc_cbDbg cbDbg, Tst_tagCFnc_cbErr cbErr,
		const Tst_str *pTagFilen, Tst_tagBasics *pTBas,
		ST_OPTARG(Tst_streamwr *pStrwr), Tst_binobj *pTagBO)
{
	Tst_err    res       = ST_ERR_SUCC;
	Tst_foffs  offs;
	Tst_buf    buf[4096];
	Tst_uint32 nsz,
	           cpTot     = 0,
	           toCp      = sizeof(buf),
	           rdFromStr = 0;
	Tst_streamrd strrd;

	ST_ASSERTN_IARG(pBasOpts == NULL || pOpts == NULL ||
			pFnc == NULL || pTagFilen == NULL || pTBas == NULL ||
			cbDbg == NULL || cbErr == NULL ||
			(pStrwr == NULL && ! pBasOpts->pretWr) || pTagBO == NULL)

	offs = (pBasOpts->pretWr ? 0 : st_streamwr_getCurPos(pStrwr));
	nsz  = st_binobj_getDataSize(pTagBO);

	if (nsz == 0) {
		cbErr(pOpts, pFnc, pTagFilen,
				"won't write empty tag");
		return ST_ERR_ABRT;
	}

	if (ST_TFDEB_ISVERBTAG_BP(pBasOpts)) {
		cbDbg(pOpts, 0, pFnc,
				"writing to stream%s",
				(pBasOpts->pretWr ? "  (pretending)" : ""));
		if (pBasOpts->pretWr)
			cbDbg(pOpts, 0, pFnc,
					"write tag: "
					"%u bytes, %s  (pretending)",
					nsz,
					(st_tagBas_gs_getHasTag(pTBas) ? "ow" : "nw"));
		else
			cbDbg(pOpts, 0, pFnc,
					"write tag: o "ST_TFOFFS_PRF_0X" ("ST_TFOFFS_PRF_LD"), "
					"%u bytes, %s",
					(Tst_foffs)offs, (Tst_foffs)offs,
					nsz,
					(st_tagBas_gs_getHasTag(pTBas) ? "ow" : "nw"));
	}

	st_streamrd_stc_initSObj(&strrd);
	res = st_binobj_attachStreamrd(pTagBO, &strrd, 0);
	if (res != ST_ERR_SUCC)
		return res;

	/* write binary data to stream */
	while (cpTot < nsz) {
		if (cpTot + toCp > nsz)
			toCp = nsz - cpTot;
		res = st_streamrd_rdBuffer_noCRC(&strrd, toCp, buf, &rdFromStr);
		if (res == ST_ERR_SUCC && toCp != rdFromStr)
			res = ST_ERR_FCRD;
		if (res == ST_ERR_SUCC && ! pBasOpts->pretWr) {
			res = st_streamwr_wrBuffer_noCRC(pStrwr, rdFromStr, buf);
			if (res != ST_ERR_SUCC)
				cbErr(pOpts, pFnc, pTagFilen,
						"can't write to stream");
		}
		if (res != ST_ERR_SUCC)
			break;
		cpTot += rdFromStr;
	}

	/* update tag info */
	if (res == ST_ERR_SUCC) {
		st_tagBas_gs_setOffset(pTBas, offs);
		st_tagBas_gs_setSize(pTBas, nsz);
		st_tagBas_gs_setHasTag(pTBas, ST_B_TRUE);
	}

	st_binobj_detachStreamrd(pTagBO);
	st_streamrd_stc_freeSObj(&strrd);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * Merge Genre-strings to one string
 *
 * @return True on success (if value is OK)
 */
Tst_bool
st_tagCFnc_genreMerge(Tst_mtes_strArr *pSArr)
{
	Tst_bool   resB   = ST_B_TRUE,
	           mod    = ST_B_FALSE;
	Tst_str    *pRstr = NULL;
	Tst_uint32 x,
	           len    = 0,
	           pos    = 0;
	Tst_int32  gnrTmp = -1;
	Tst_mtes_strArr sArrTmp;
	Tst_mtes_string *pStr,
	                tmpr,
	                cap;

	ST_ASSERTN_BOOL(ST_B_FALSE, pSArr == NULL)

	if (st_mtes_stc_initSA(&sArrTmp) != ST_ERR_SUCC ||
			st_mtes_stc_initStr(&cap) != ST_ERR_SUCC)
		return ST_B_FALSE;

	for (x = 1; x <= st_mtes_strArrGetElemCount(pSArr); x++) {
		pStr = st_mtes_strArrGetElem(pSArr, x);
		if (st_mtes_isStrEmptyCh(pStr) || st_mtes_getEncodingCur(pStr) != ST_MTES_TE_ISO) {
			resB = ST_B_FALSE;
			break;
		}
		if (! st_tagCFnc_chk_cGenre_sub(pStr, &cap, &gnrTmp, &pRstr, &mod))
			continue;
		len += st_mtes_strlenCh(&cap) + 2;  /* "xxx, " */
		if (st_mtes_strArrAdd(&sArrTmp, &cap) != ST_ERR_SUCC) {
			resB = ST_B_FALSE;
			break;
		}
	}
	st_mtes_stc_rsetSA(pSArr);
	/* */
	resB = resB && (st_mtes_strArrGetElemCount(&sArrTmp) > 0);
	if (resB) {
		ST_REALLOC(pRstr, Tst_str*, len + 1, 1)
		if (pRstr == NULL)
			resB = ST_B_FALSE;
	}
	if (resB) {
		memset(pRstr, 0, len + 1);
		for (x = 1; x <= st_mtes_strArrGetElemCount(&sArrTmp); x++) {
			pStr = st_mtes_strArrGetElem(&sArrTmp, x);
			len  = st_mtes_strlenCh(pStr);
			memcpy(&pRstr[pos], st_mtes_getDataPt(pStr), len);
			pos += len;
			if (x + 1 <= st_mtes_strArrGetElemCount(&sArrTmp)) {
				strcpy((char*)&pRstr[pos], ", ");
				pos += 2;
			}
		}
		/* */
		if (st_mtes_stc_initStr(&tmpr) != ST_ERR_SUCC)
			resB = ST_B_FALSE;
		else {
			if (st_mtes_setStrTo(ST_MTES_TE_ISO, pRstr, &tmpr) != ST_ERR_SUCC)
				resB = ST_B_FALSE;
			if (st_mtes_strArrAdd(pSArr, &tmpr) != ST_ERR_SUCC)
				resB = ST_B_FALSE;
			st_mtes_stc_freeStr(&tmpr);
		}
	}
	/* */
	ST_DELPOINT(pRstr)
	st_mtes_stc_freeStr(&cap);
	st_mtes_stc_freeSA(&sArrTmp);
	return resB;
}

/******************************************************************************/
