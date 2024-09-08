/******************************************************************************/
/* cln_psvt.c                     [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Sub functions for parsing special value types                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/sys_fnc.h>
#include <santag/common/string_mte.h>
#include <santag/tag/tag_id3v2.h>
#include <santag/tag/tag_apev2.h>
#include <santag/tag/tag_vorbc.h>
/** */
#define SRC_CLN_PSVT_C
#include "cln_psvt-pp.h"
/*** */
#include "cln_fnc-prot.h"
#include "cln_stcs-prot.h"
#undef SRC_CLN_PSVT_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), qsort() */
#include <string.h>      /* memcpy(), ... */
#include <ctype.h>       /* isalnum(), isdigit() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef struct {
	Tst_str const *pStr;
	Tst_str const *pCurCh;
	Tst_uint32    beg;
	Tst_uint32    end;
	/* */
	Tst_str const *pElem;
	Tst_uint32    elemLen;
} Tast_cln__psvt_gnle;

#define AST_CLN_RESET_GNLE(mac_gnle, mac_pStr)  { \
			(mac_gnle).pStr    = mac_pStr; \
			(mac_gnle).pCurCh  = mac_pStr; \
			(mac_gnle).beg     = 0; \
			(mac_gnle).end     = 0; \
			(mac_gnle).pElem   = NULL; \
			(mac_gnle).elemLen = 0; \
			}

static Tst_bool
AST_CLN__psvt_isLstNotEmpty(const char *pFnc, const char *pFmt,
                            Tast_cln_a *pCmdln, Tast_cln__psvt_gnle *pGNLE,
                            ST_OPTARG(Tst_uint32 *pCnt));
static Tst_bool
AST_CLN__psvt_getNextLstElem(Tast_cln__psvt_gnle *pGNLE);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse <taglist>, e.g. "id3v1,id3v2,ape,merg" or "none" or "all" or "auto"
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_taglist(Tast_cln_a *pCmdln, const Tst_str *pStrLst,
		const Tast_cln_t_tagList_pb(pAllwTags),
		const Tst_bool autoAllw, const Tst_bool nonAllw,
		Tast_cln_t_tagList_pb(pOutTags),
		Tst_bool *pAuto, Tst_bool *pNone)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrLst == NULL ||
			pAllwTags == NULL || pOutTags == NULL)

	return AST_CLN__psvt_parseTaglist(LOC_FNCN_, pCmdln,
			pStrLst,
			pAllwTags, autoAllw, nonAllw,
			pOutTags,
			pAuto, pNone);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <tagpair>, e.g. "all-iv1" or "iv1-iv2" or "iv1,iv2-av2"
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_tagpair(Tast_cln_a *pCmdln, const Tst_str *pStrPair,
		const Tast_cln_t_tagList_pb(pAllwTagsLeft),
		const Tast_cln_t_tagList_pb(pAllwTagsRight),
		Tast_cln_t_tagPair *pTP)
{
	const char *cFNCN = __FUNCTION__;
	Tst_bool   resB    = ST_B_TRUE;
	Tst_str    *pLeft  = NULL,
	           *pRight = NULL;
	Tst_uint32 slenT;
	Tast_cln_t_tagList_b(tmpOutTagsDst);

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrPair == NULL ||
			pAllwTagsLeft == NULL || pAllwTagsRight == NULL || pTP == NULL)

	/**ast_cln_prf("TP: '%s'\n", pStrPair);**/

	ast_cln_stc_rsetT_taglist(tmpOutTagsDst, ST_B_FALSE);

	/* */
	if (strchr((const char*)pStrPair, '-') == NULL) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"char '-' missing in <%s>, aborting", AST_CLN_SVT_FMT_TAGPAIR);
		return ST_B_FALSE;
	}

	/* split string */
	resB = AST_CLN__psvt_splitstr(pStrPair, '-', /*canBeEscaped:*/ST_B_FALSE,
			&pLeft, &pRight);

	/** handle left part */
	if (resB)
		slenT = st_sysStrlen(pLeft);
	if (resB && slenT == 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"left part of <%s> missing, aborting",
				AST_CLN_SVT_FMT_TAGPAIR);
		resB = ST_B_FALSE;
	} else if (resB) {
		/**ast_cln_prf(" left: '%s'\n", pLeft);**/
		resB = AST_CLN__psvt_parseTaglist(cFNCN, pCmdln, pLeft,
				pAllwTagsLeft,
				/*autoAllw:*/ST_B_FALSE, /*nonAllw:*/ST_B_FALSE,
				/*pOutTags:*/pTP->src,
				/*pAuto:*/NULL, /*pNone:*/NULL);
	}
	/** handle right part */
	if (resB)
		slenT = st_sysStrlen(pRight);
	if (resB && slenT == 0) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"right part of <%s> missing, aborting", AST_CLN_SVT_FMT_TAGPAIR);
		resB = ST_B_FALSE;
	} else if (resB) {
		char const *pCh = (char const*)pRight;

		/**ast_cln_prf(" right: '%s'\n", pRight);**/
		while (*pCh) {
			if (! isalnum((int)*pCh)) {
				pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
						"invalid char '%c' in right part of <%s>, aborting",
						*pCh, AST_CLN_SVT_FMT_TAGPAIR);
				resB = ST_B_FALSE;
				break;
			}
			++pCh;
		}
	}
	if (resB) {
		resB = AST_CLN__psvt_parseTagElem(cFNCN, pCmdln, pRight, slenT,
				pAllwTagsRight,
				/*autoAllw:*/ST_B_FALSE, /*nonAllw:*/ST_B_FALSE,
				/*allAllw:*/ST_B_FALSE,
				tmpOutTagsDst,
				/*pAuto:*/NULL, /*pNone:*/NULL);
	}
	if (resB) {
		Tst_uint32 x;

		for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
			if (tmpOutTagsDst[x]) {
				pTP->dst = (Tast_cln_t_tagListIx)x;
				break;
			}
	}

#	if 0
	/* remove tag 'dst' from 'src' */
	if (resB) {
		Tst_uint32 x;

		for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
			if (pTP->src[x] && (Tast_cln_t_tagListIx)x == pTP->dst)
				pTP->src[x] = ST_B_FALSE;
	}
	if (resB) {
		Tst_bool   haveany = ST_B_FALSE;
		Tst_uint32 x;

		/* check wether we still have any 'src' tag */
		for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
			if (pTP->src[x]) {
				haveany = ST_B_TRUE;
				break;
			}
		if (! haveany) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"source-tag==dest-tag in <%s>, nothing to convert, aborting",
					AST_CLN_SVT_FMT_TAGPAIR);
			resB = ST_B_FALSE;
		}
	}
#	endif

	ST_DELPOINT(pLeft)
	ST_DELPOINT(pRight)
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <numlist>, e.g. "1,2,3" or "all"
 *   (0 is not allowed since it means 'end of list')
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_numlist(Tast_cln_a *pCmdln,
		const Tst_str *pStrLst, Tst_uint32 **ppArr,
		Tst_bool *pAll)
{
	const char *cFNCN = __FUNCTION__;
	Tst_uint32 arrSz = 0,
	           fnd   = 0,
	           x,
	           y,
	           *pNr,
	           rangeEnd;
	Tast_cln__psvt_gnle gnle;

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrLst == NULL ||
			ppArr == NULL || pAll == NULL)

	AST_CLN_RESET_GNLE(gnle, pStrLst)

	/* count elements and reserve some mem */
	if (! AST_CLN__psvt_isLstNotEmpty(cFNCN, AST_CLN_SVT_FMT_NUMLIST,
				pCmdln, &gnle, &arrSz))
		return ST_B_FALSE;
	++arrSz;
	ST_REALLOC(*ppArr, Tst_uint32*, arrSz, sizeof(Tst_uint32))
	if (*ppArr == NULL)
		return ST_B_FALSE;
	memset(*ppArr, 0, arrSz * sizeof(Tst_uint32));

	/* parse elements */
	pNr = &(*ppArr)[0];
	while (AST_CLN__psvt_getNextLstElem(&gnle)) {
		rangeEnd = 0;
		if (! AST_CLN__psvt_parseNumlistElem(pCmdln,
					gnle.pElem, gnle.elemLen,
					pNr, &rangeEnd, pAll))
			return ST_B_FALSE;
		if (rangeEnd > 0 && *pNr != rangeEnd) {
			Tst_uint32 tmp,
			           startVal;

			if (rangeEnd < *pNr) {
				tmp      = *pNr;
				*pNr     = rangeEnd;
				rangeEnd = tmp;
			}
			startVal = *pNr;
			tmp      = rangeEnd - startVal + 1;  /* tmp ^= length of range */
			if (fnd + tmp + 1 > arrSz) {
				ST_REALLOC(*ppArr, Tst_uint32*, fnd + tmp + 1, sizeof(Tst_uint32))
				if (*ppArr == NULL)
					return ST_B_FALSE;
				memset(&(*ppArr)[arrSz], 0,
						((fnd + tmp + 1) - arrSz) * sizeof(Tst_uint32));
				arrSz = fnd + tmp + 1;
			}
			for (x = 0; x < tmp; x++)
				(*ppArr)[fnd++] = startVal++;
			pNr = &(*ppArr)[fnd];
		} else {
			++fnd;
			if (fnd + 1 > arrSz) {
				ST_REALLOC(*ppArr, Tst_uint32*, fnd + 1, sizeof(Tst_uint32))
				if (*ppArr == NULL)
					return ST_B_FALSE;
				memset(&(*ppArr)[arrSz], 0,
						((fnd + 1) - arrSz) * sizeof(Tst_uint32));
				arrSz = fnd + 1;
				pNr   = &(*ppArr)[fnd];
			} else
				++pNr;
		}
		/**ast_cln_prf(" numlist: arrSz %3u, fnd %3u\n", arrSz, fnd);**/
	}

	/* sort and strip array */
	/**for (x = 0; x < fnd + 1; x++)
		ast_cln_prf(" numlist unsorted: ix %3u   fnd %u\n", (*ppArr)[x], fnd);**/
	qsort(*ppArr, fnd,
			sizeof(Tst_uint32), &AST_CLN__psvt_compNumlistElems);
	for (x = 0; x < fnd; x++) {
		if ((*ppArr)[x] == 0)
			break;
		y = x + 1;
		while (y < fnd) {
			if ((*ppArr)[x] == (*ppArr)[y]) {
				memmove(&(*ppArr)[y], &(*ppArr)[y + 1],
						(fnd - y - 1) * sizeof(Tst_uint32));
				(*ppArr)[fnd - 1] = 0;
				continue;
			} else
				break;
			++y;
		}
	}
	/**for (x = 0; x < fnd + 1; x++)
		ast_cln_prf(" numlist sorted: ix %3u   fnd %u\n", (*ppArr)[x], fnd);**/
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <vlevlist>, e.g. "gen,opts,8" or "none" or "all"
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_vlevlist(Tast_cln_a *pCmdln, const Tst_str *pStrLst,
		Tst_int32 *pVerb, Tst_bool *pNon)
{
	const char *cFNCN = __FUNCTION__;
	Tst_int32 vlE;
	Tast_cln__psvt_gnle gnle;

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrLst == NULL ||
			pVerb == NULL || pNon == NULL)

	AST_CLN_RESET_GNLE(gnle, pStrLst)

	/* check list */
	if (! AST_CLN__psvt_isLstNotEmpty(cFNCN, AST_CLN_SVT_FMT_VLEVLIST,
				pCmdln, &gnle, NULL))
		return ST_B_FALSE;
	/* parse elements */
	while (AST_CLN__psvt_getNextLstElem(&gnle)) {
		vlE = 0;
		if (! AST_CLN__psvt_parseVlevlistElem(pCmdln,
					gnle.pElem, gnle.elemLen, &vlE, pNon))
			return ST_B_FALSE;
		/**ast_cln_prf(" vlev: %d (none=%d)\n", vlE, (pNon ? (int)*pNon : 0));**/
		*pVerb = *pVerb | vlE;
	}
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <bslist>, e.g. "all" or "1" or "1.2" or "1,2.3" or "1.0,3,4.1"
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_bslist(Tast_cln_a *pCmdln, const Tst_str *pStrLst,
		Tast_cln_t_ebs **ppEBSarr,
		Tst_bool *pAll)
{
	const char *cFNCN = __FUNCTION__;
	Tst_uint32 fnd = 0;
	Tast_cln__psvt_gnle gnle;
	Tast_cln_t_ebs      *pCElem;

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrLst == NULL ||
			ppEBSarr == NULL || pAll == NULL)

	AST_CLN_RESET_GNLE(gnle, pStrLst)

	/* count elements and reserve some mem */
	if (! AST_CLN__psvt_isLstNotEmpty(cFNCN, AST_CLN_SVT_FMT_BSLIST,
				pCmdln, &gnle, &fnd))
		return ST_B_FALSE;
	ST_REALLOC(*ppEBSarr, Tast_cln_t_ebs*,
			fnd + 1, sizeof(Tast_cln_t_ebs))
	if (*ppEBSarr == NULL)
		return ST_B_FALSE;
	memset(*ppEBSarr, 0, (fnd + 1) * sizeof(Tst_uint32));
	/*fnd = 0;*/

	/* parse elements */
	pCElem = &(*ppEBSarr)[0];
	while (AST_CLN__psvt_getNextLstElem(&gnle)) {
		if (! AST_CLN__psvt_parseBslistElem(pCmdln,
					gnle.pElem, gnle.elemLen,
					pAll, pCElem))
			return ST_B_FALSE;
		/* */
		/*++fnd;*/
		++pCElem;
	}

	/* add End-Of-List element '0.0' */
	pCElem->ix  = 0;
	pCElem->six = 0;

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <pos>, e.g. "1" or "1/" or "1/2" or "/2"
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_pos(Tast_cln_a *pCmdln, const Tst_str *pStrPos,
		Tst_int32 *pPosNr, Tst_int32 *pPosTot)
{
	const char *cFNCN = __FUNCTION__;
	Tst_bool   resB     = ST_B_TRUE,
	           hasLeft  = ST_B_FALSE,
	           hasRight = ST_B_FALSE;
	Tst_str    *pLeft  = NULL,
	           *pRight = NULL;
	Tst_uint32 slenT   = 0;

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrPos == NULL ||
			pPosNr == NULL || pPosTot == NULL)

	/* split string */
	resB = AST_CLN__psvt_splitstr(pStrPos, '/', /*canBeEscaped:*/ST_B_FALSE,
			&pLeft, &pRight);

	/** handle left part */
	if (resB)
		slenT = st_sysStrlen(pLeft);
	if (resB && slenT >= 10) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"left part of <%s> is too long (max. 9 digits), aborting",
				AST_CLN_SVT_FMT_POS);
		resB = ST_B_FALSE;
	} else if (resB && slenT > 0) {
		/**ast_cln_prf(" left: '%s'\n", pLeft);**/
		resB    = AST_CLN__psvt_str2int(cFNCN, pCmdln, AST_CLN_SVT_FMT_POS,
				pLeft, pPosNr);
		hasLeft = resB;
		/**if (resB) ast_cln_prf(" left: %d\n", *pPosNr);**/
	}
	/** handle right part */
	if (resB)
		slenT = st_sysStrlen(pRight);
	if (resB && slenT >= 10) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"right part of <%s> is too long (max. 9 digits), aborting",
				AST_CLN_SVT_FMT_POS);
		resB = ST_B_FALSE;
	} else if (resB && slenT > 0) {
		/**ast_cln_prf(" right: '%s'\n", pRight);**/
		resB     = AST_CLN__psvt_str2int(cFNCN, pCmdln, AST_CLN_SVT_FMT_POS,
				pRight, pPosTot);
		hasRight = resB;
		/**if (resB) ast_cln_prf(" right: %d\n", *pPosTot);**/
	}

	if (resB && ! (hasLeft || hasRight)) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"<%s> is empty, aborting", AST_CLN_SVT_FMT_POS);
		resB = ST_B_FALSE;
	}

	ST_DELPOINT(pLeft)
	ST_DELPOINT(pRight)
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <fldadd>/<fldset>,
 *   e.g. "id3v2:TBPM=300" or "TBPM=300" or "COMM=Screw you"
 *        or "COMM=" or "COMM.2="
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_fldaddset(Tast_cln_a *pCmdln, const Tst_str *pStrPair,
		const Tst_bool addOrSet, Tast_cln_t_fldDscAddSet *pFP)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrPair == NULL ||
			pFP == NULL)

	return AST_CLN__psvt_flddscaddset(LOC_FNCN_,
			(addOrSet ? AST_CLN_SVT_FMT_FLDADD : AST_CLN_SVT_FMT_FLDSET),
			pCmdln, pStrPair,
			/*canHaveTag:*/ST_B_TRUE, /*canHaveFldnr:*/! addOrSet,
			/*mustHaveValue:*/addOrSet, pFP);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <dscset> (special form of <fldset>),
 *  e.g. "COMM.1=Hallow" or "GEOB=" or "APIC=All Band Members"
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_dscset(Tast_cln_a *pCmdln, const Tst_str *pStrPair,
		Tast_cln_t_fldDscAddSet *pDP)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrPair == NULL ||
			pDP == NULL)

	return AST_CLN__psvt_flddscaddset(LOC_FNCN_, AST_CLN_SVT_FMT_DSCSET,
			pCmdln, pStrPair,
			/*canHaveTag:*/ST_B_FALSE, /*canHaveFldnr:*/ST_B_TRUE,
			/*mustHaveValue:*/ST_B_FALSE, pDP);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <tiv2optlist>, e.g. "v3" or "cmpr,v3"
 *
 * returns: true on success
 */
Tst_bool
ast_cln_psvt_tiv2optlist(Tast_cln_a *pCmdln, const Tst_str *pStrLst,
		Tast_cln_t_tiv2opt *pTOpts)
{
#	define LOC_FNCN_  __FUNCTION__
	Tast_cln__psvt_gnle gnle;

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrLst == NULL ||
			pTOpts == NULL)

	AST_CLN_RESET_GNLE(gnle, pStrLst)

	/* check list */
	if (! AST_CLN__psvt_isLstNotEmpty(LOC_FNCN_, AST_CLN_SVT_FMT_TIV2OPTLIST,
				pCmdln, &gnle, NULL))
		return ST_B_FALSE;
	/* parse elements */
	while (AST_CLN__psvt_getNextLstElem(&gnle)) {
		if (! AST_CLN__psvt_parseTiv2optlistElem(pCmdln,
					gnle.pElem, gnle.elemLen, pTOpts))
			return ST_B_FALSE;
	}

	return ST_B_TRUE;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse <taglist>, e.g. "id3v1,id3v2,ape,merg" or "none" or "all" or "auto"
 *
 * returns: true on success
 */
static Tst_bool
AST_CLN__psvt_parseTaglist(const char *pFnc, Tast_cln_a *pCmdln,
		const Tst_str *pStrLst,
		const Tast_cln_t_tagList_pb(pAllwTags),
		const Tst_bool autoAllw, const Tst_bool nonAllw,
		Tast_cln_t_tagList_pb(pOutTags),
		ST_OPTARG(Tst_bool *pAuto), ST_OPTARG(Tst_bool *pNone))
{
	Tast_cln__psvt_gnle gnle;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFnc == NULL || pCmdln == NULL ||
			pStrLst == NULL || pAllwTags == NULL || pOutTags == NULL)

	AST_CLN_RESET_GNLE(gnle, pStrLst)

	/* check list */
	if (! AST_CLN__psvt_isLstNotEmpty(pFnc, AST_CLN_SVT_FMT_TAGLIST,
				pCmdln, &gnle, NULL))
		return ST_B_FALSE;
	/* parse elements */
	while (AST_CLN__psvt_getNextLstElem(&gnle)) {
		if (! AST_CLN__psvt_parseTagElem(pFnc, pCmdln,
					gnle.pElem, gnle.elemLen,
					pAllwTags,
					autoAllw, nonAllw, /*allAllw:*/ST_B_TRUE,
					pOutTags,
					pAuto, pNone))
			return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

/*
 * Parse <taglist> element and set pArr, e.g. "id3v1"
 *
 * returns: true if element is OK
 */
static Tst_bool
AST_CLN__psvt_parseTagElem(const char *pFnc, Tast_cln_a *pCmdln,
		const Tst_str *pStrElem, const Tst_uint32 len,
		const Tast_cln_t_tagList_pb(pAllwTags),
		const Tst_bool autoAllw, const Tst_bool nonAllw,
		const Tst_bool allAllw,
		Tast_cln_t_tagList_pb(pOutTags),
		ST_OPTARG(Tst_bool *pAuto), ST_OPTARG(Tst_bool *pNone))
{
#	define LOC_FNDSTRMATCH_(mac_tlix)  { \
				if (pAllwTags[mac_tlix]) { \
					isOK               = ST_B_TRUE; \
					pOutTags[mac_tlix] = (pNone == NULL || ! *pNone) && \
							(pAuto == NULL || ! *pAuto); \
				} else \
					nAllw = ST_B_TRUE; \
				}
	char       *pT   = NULL;
	Tst_bool   isOK  = ST_B_FALSE,
	           nAllw = ST_B_FALSE;
	Tst_uint32 x;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFnc == NULL || pCmdln == NULL ||
			pStrElem == NULL || pAllwTags == NULL || pOutTags == NULL)

	ST_CALLOC(pT, char*, len + 1, 1)
	if (pT == NULL)
		return ST_B_FALSE;
	memcpy(pT, pStrElem, len);
	pT[len] = 0;
	/* */
	if (ast_cln_fnc_strcmp2xN(pT, len,
			AST_CLN_ARG_TIV1, AST_CLN_ARG_TIV1SH)) {
		LOC_FNDSTRMATCH_(AST_CLN_T_TAGLIST_IV1)
	} else if (ast_cln_fnc_strcmp2xN(pT, len,
			AST_CLN_ARG_TIV2, AST_CLN_ARG_TIV2SH)) {
		LOC_FNDSTRMATCH_(AST_CLN_T_TAGLIST_IV2)
	} else if (ast_cln_fnc_strcmp2xN(pT, len,
			AST_CLN_ARG_TAV2, AST_CLN_ARG_TAV2SH)) {
		LOC_FNDSTRMATCH_(AST_CLN_T_TAGLIST_AV2)
	} else if (ast_cln_fnc_strcmp2xN(pT, len,
			AST_CLN_ARG_TVOR, AST_CLN_ARG_TVORSH)) {
		LOC_FNDSTRMATCH_(AST_CLN_T_TAGLIST_VOR)
	} else if (ast_cln_fnc_strcmp2xN(pT, len,
			AST_CLN_ARG_TMRG, AST_CLN_ARG_TMRGSH)) {
		LOC_FNDSTRMATCH_(AST_CLN_T_TAGLIST_MRG)
	} else if (ast_cln_fnc_strcmp2xN(pT, len,
				AST_CLN_ARG_AUTO, AST_CLN_ARG_AUTOSH)) {
		if (autoAllw && pAuto != NULL) {
			ast_cln_stc_rsetT_taglist(pOutTags, ST_B_FALSE);
			/* */
			*pAuto = ST_B_TRUE;
			isOK   = ST_B_TRUE;
		} else
			nAllw = ST_B_TRUE;
	} else if (ast_cln_fnc_strcmp2xN(pT, len,
				AST_CLN_ARG_ALL, AST_CLN_ARG_ALLSH)) {
		if (allAllw) {
			for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
				pOutTags[x] = pAllwTags[x] && (pNone == NULL || ! *pNone);
			isOK = ST_B_TRUE;
		} else
			nAllw = ST_B_TRUE;
	} else if (ast_cln_fnc_strcmp2xN(pT, len,
			AST_CLN_ARG_NONE, AST_CLN_ARG_NONESH)) {
		if (nonAllw && pNone != NULL) {
			ast_cln_stc_rsetT_taglist(pOutTags, ST_B_FALSE);
			/* */
			*pNone = ST_B_TRUE;
			isOK   = ST_B_TRUE;
		} else
			nAllw = ST_B_TRUE;
	}
	/* */
	if (nAllw)
		pCmdln->cbErr(pCmdln->pAppFn, pFnc,
				"<%s> '%s' is not allowed, aborting",
				AST_CLN_SVT_FMT_TAG, pT);
	else if (! isOK)
		pCmdln->cbErr(pCmdln->pAppFn, pFnc,
				"unknown <%s> '%s', aborting", AST_CLN_SVT_FMT_TAG, pT);

	ST_DELPOINT(pT)
	return isOK;
#	undef LOC_FNDSTRMATCH_
}

/*----------------------------------------------------------------------------*/

/* for qsort() */
static int
AST_CLN__psvt_compNumlistElems(const void *pIx1, const void *pIx2)
{
	if (pIx1 == NULL || pIx2 == NULL)
		return 1;
	if (*(Tst_uint32*)pIx1 == 0)
		return 1;
	if (*(Tst_uint32*)pIx1 < *(Tst_uint32*)pIx2)
		return -1;
	if (*(Tst_uint32*)pIx1 > *(Tst_uint32*)pIx2)
		return 1;
	return 0;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <numlist> element and set pNr and pRangeEnd,
 *   e.g. "34" or "3-5" or "all"
 *
 * returns: true if element is OK
 */
static Tst_bool
AST_CLN__psvt_parseNum(const char *pFnc, Tast_cln_a *pCmdln,
		const Tst_str *pStrNum, const Tst_uint32 len, Tst_uint32 *pNr)
{
	Tst_bool  resB;
	Tst_int32 tmpI32 = -1;

	resB = AST_CLN__psvt_str2int(pFnc, pCmdln, AST_CLN_SVT_FMT_NUM,
			pStrNum, &tmpI32);
	if (resB) {
		if (tmpI32 <= 0) {
			pCmdln->cbErr(pCmdln->pAppFn, pFnc,
					"invalid <%s> %d in <%s>, aborting",
					AST_CLN_SVT_FMT_NUM, tmpI32, AST_CLN_SVT_FMT_NUMLIST);
			resB = ST_B_FALSE;
		} else {
			*pNr = (Tst_uint32)tmpI32;
			/**ast_cln_prf("  have num %u\n", *pNr);**/
		}
	}
	return resB;
}

static Tst_bool
AST_CLN__psvt_parseNumlistElem(Tast_cln_a *pCmdln,
		const Tst_str *pStrElem, const Tst_uint32 len,
		Tst_uint32 *pNr, Tst_uint32 *pRangeEnd, Tst_bool *pAll)
{
	const char *cFNCN = __FUNCTION__;
	char       *pT     = NULL;
	Tst_str    *pLeft  = NULL,
	           *pRight = NULL;
	Tst_bool   isOK    = ST_B_FALSE;
	Tst_uint32 lenL,
	           lenR;

	*pRangeEnd = 0;

	ST_CALLOC(pT, char*, len + 1, 1)
	if (pT == NULL)
		return ST_B_FALSE;
	memcpy(pT, pStrElem, len);
	pT[len] = 0;
	/* */
	if (len > 3 + 1 + 3)
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"element '%s' in <%s> too long (max. 7 chars), "
				"aborting", pT, AST_CLN_SVT_FMT_NUMLIST);
	else if (AST_CLN__psvt_splitstr((const Tst_str*)pT, '-',
				/*canBeEscaped:*/ST_B_FALSE, &pLeft, &pRight)) {
		lenL = st_sysStrlen(pLeft);
		lenR = st_sysStrlen(pRight);

		if (lenR == 0 &&
				ast_cln_fnc_strcmp2xN((const char*)pLeft, lenL,
						AST_CLN_ARG_ALL, AST_CLN_ARG_ALLSH)) {
			*pAll = ST_B_TRUE;
			isOK  = ST_B_TRUE;
		} else if (lenL == 0) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"left side of <%s> is empty, aborting",
					AST_CLN_SVT_FMT_NUMRANGE);
		} else {
			isOK = AST_CLN__psvt_parseNum(cFNCN, pCmdln,
					pLeft, lenL, pNr);
			if (isOK && lenR > 0)
				isOK = AST_CLN__psvt_parseNum(cFNCN, pCmdln,
						pRight, lenR, pRangeEnd);
		}
	}
	ST_DELPOINT(pT)
	ST_DELPOINT(pLeft)
	ST_DELPOINT(pRight)
	return isOK;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <vlevlist> element
 *
 * returns: true if element is OK
 */
static Tst_bool
AST_CLN__psvt_parseVlevlistElem(Tast_cln_a *pCmdln,
		const Tst_str *pStrElem, const Tst_uint32 len, Tst_int32 *pVL,
		Tst_bool *pNon)
{
	const char *cFNCN = __FUNCTION__;
	char       *pT   = NULL;
	Tst_bool   isOK  = ST_B_FALSE,
	           isNum = ST_B_TRUE;
	Tst_uint32 num;
	Tast_cln_t_verblevList_capDesc const *pCapDesc = AST_CLN_VERBLIST_CAPDESC,
	                                     *pCDfnd   = pCapDesc;

	for (num = 0; num < len; num++)
		if (! isdigit((int)pStrElem[num])) {
			isNum = ST_B_FALSE;
			break;
		}

	ST_CALLOC(pT, char*, len + 1, 1)
	if (pT == NULL)
		return ST_B_FALSE;
	memcpy(pT, pStrElem, len);
	pT[len] = 0;

	if (len > 4) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"<%s> element '%s' too long, aborting",
				AST_CLN_SVT_FMT_VLEVLIST, pT);
		ST_DELPOINT(pT)
		return ST_B_FALSE;
	}
	/* */
	num = 0;
	if (isNum)
		sscanf((char*)pT, "%u", &num);
	while (pCapDesc->lev >= 0 && pCapDesc->pCap != NULL)
		if ((isNum && pCapDesc->lev == (Tst_int32)(pCapDesc->lev & num)) ||
				(! isNum &&
					ast_cln_fnc_strcmp2xN(pT, len,
							pCapDesc->pCap, pCapDesc->pCapSh))) {
			isOK   = ST_B_TRUE;
			pCDfnd = pCapDesc;
			break;
		} else
			++pCapDesc;
	if (! isOK && isNum && num <= 0xff) {
		isOK = ST_B_TRUE;
		*pVL = num;
	}
	if (isOK) {
		if (st_sysStrcmp2(ST_B_TRUE, pCDfnd->pCap, AST_CLN_ARG_ALL)) {
			*pVL  = 0x7fffffff;
			*pNon = ST_B_FALSE;
		} else if (st_sysStrcmp2(ST_B_TRUE, pCDfnd->pCap, AST_CLN_ARG_NONE))
			*pNon = ST_B_TRUE;
		else if (! *pNon) {
			if (isNum)
				*pVL = num;
			else
				*pVL = pCapDesc->lev;
		}
	} else
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"invalid <%s> element '%s', aborting",
				AST_CLN_SVT_FMT_VLEVLIST, pT);
	ST_DELPOINT(pT)
	return isOK;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <bslist> element
 *
 * returns: true if element is OK
 */
static Tst_bool
AST_CLN__psvt_parseBslistElem(Tast_cln_a *pCmdln,
		const Tst_str *pStrElem, const Tst_uint32 len,
		Tst_bool *pAll, Tast_cln_t_ebs *pEBS)
{
	const char *cFNCN = __FUNCTION__;
	char     *pT  = NULL;
	Tst_bool isOK = ST_B_FALSE;

	pEBS->ix  = 0;
	pEBS->six = 0;

	ST_CALLOC(pT, char*, len + 1, 1)
	if (pT == NULL)
		return ST_B_FALSE;
	memcpy(pT, pStrElem, len);
	pT[len] = 0;
	/* */
	if (ast_cln_fnc_strcmp2xN(pT, len, AST_CLN_ARG_ALL, AST_CLN_ARG_ALLSH)) {
		*pAll = ST_B_TRUE;
		isOK  = ST_B_TRUE;
	} else if (len > 3) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"element '%s' in <%s> too long (max. 3 chars), "
				"aborting", pT, AST_CLN_SVT_FMT_BSLIST);
	} else {
		if (! isdigit((int)pT[0]) || pT[0] == '0' ||
				len == 2 ||
				(len == 3 &&
					(pT[1] != '.' || ! isdigit((int)pT[2]))))
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"invalid element '%s' in <%s>, aborting",
					pT, AST_CLN_SVT_FMT_BSLIST);
		else {
			pEBS->ix = (Tst_uint32)(pT[0] - '0');
			if (len == 3)
				pEBS->six = (Tst_uint32)(pT[2] - '0');
			isOK = ST_B_TRUE;
		}
	}

	ST_DELPOINT(pT)
	return isOK;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <fldadd>/<fldset>/<dscset>,
 *   e.g. "id3v2:TBPM=300" or "TBPM=300" or "COMM=Screw you"
 *        or "COMM=" or "COMM.2="
 */
static Tst_bool
AST_CLN__psvt_flddscaddset(const char *pFnc, const char *pFmt,
		Tast_cln_a *pCmdln,
		const Tst_str *pStrPair,
		const Tst_bool canHaveTag, const Tst_bool canHaveFldnr,
		const Tst_bool mustHaveValue,
		Tast_cln_t_fldDscAddSet *pFP)
{
	Tst_bool   resB;
	Tst_str    *pLeft      = NULL,
	           *pLeftTag   = NULL,
	           *pLeftFNaNr = NULL,
	           *pLeftFNr   = NULL;
	Tst_uint32 slen        = 0;

	ST_ASSERTN_BOOL(ST_B_FALSE, pCmdln == NULL || pStrPair == NULL ||
			pFP == NULL)

	if (strchr((const char*)pStrPair, '=') == NULL) {
		pCmdln->cbErr(pCmdln->pAppFn, pFnc,
				"char '=' missing in <%s>, aborting", pFmt);
		return ST_B_FALSE;
	}

	resB = AST_CLN__psvt_splitstr(pStrPair, '=', /*canBeEscaped:*/ST_B_FALSE,
			&pLeft, &pFP->pValueStr);
	if (resB && mustHaveValue && st_sysStrEmpty(pFP->pValueStr)) {
		pCmdln->cbErr(pCmdln->pAppFn, pFnc,
				"this <%s> needs a value, aborting", pFmt);
		resB = ST_B_FALSE;
	}
	if (resB) {
		Tst_bool hasSepa;
		Tst_str  **ppNext;

		/**ast_cln_prf(" left : '%s'\n", pLeft);**/
		/**ast_cln_prf(" value: '%s'\n", pFP->pValueStr);**/

		/* get "<tag>" and "<fldname>.<num>" */
		hasSepa = AST_CLN__psvt_hasSeparator(pLeft, ':', /*canBeEscaped:*/ST_B_TRUE);
		if (! canHaveTag && hasSepa) {
			pCmdln->cbErr(pCmdln->pAppFn, pFnc,
					"<%s> not allowed in this <%s>, aborting",
					AST_CLN_SVT_FMT_TAG, pFmt);
			resB = ST_B_FALSE;
		}
		if (resB && hasSepa)
			resB = AST_CLN__psvt_splitstr(pLeft, ':', /*canBeEscaped:*/ST_B_TRUE,
					&pLeftTag, &pLeftFNaNr);
		else if (resB && ! hasSepa)
			resB = AST_CLN__psvt_unescape(&pLeft, ':');
		ppNext = (hasSepa ? &pLeftFNaNr : &pLeft);
		/* get "<fldname>" and "<num>" */
		if (resB)
			hasSepa = AST_CLN__psvt_hasSeparator(*ppNext, '.',
					/*canBeEscaped:*/ST_B_TRUE);
		if (resB && ! canHaveFldnr && hasSepa) {
			pCmdln->cbErr(pCmdln->pAppFn, pFnc,
					"field nr. not allowed in this <%s>, aborting", pFmt);
			resB = ST_B_FALSE;
		}
		if (resB && hasSepa)
			resB = AST_CLN__psvt_splitstr(*ppNext, '.', /*canBeEscaped:*/ST_B_TRUE,
					&pFP->pFldname, &pLeftFNr);
		else if (resB && ! hasSepa) {
			resB = AST_CLN__psvt_copystr(*ppNext, -1, &pFP->pFldname);
			if (resB)
				resB = AST_CLN__psvt_unescape(&pFP->pFldname, '.');
		}

	}
	/* parse <tag> in pLeftTag */
	if (resB)
		slen = st_sysStrlen(pLeftTag);
	if (resB && slen > 0) {
		Tast_cln_t_tagList_b(allwTags);
		Tast_cln_t_tagList_b(tmpOutTagsDst);

		/**ast_cln_prf(" tag  : '%s'\n", pLeftTag);**/
		ast_cln_stc_rsetT_taglist(allwTags, ST_B_TRUE);
		allwTags[AST_CLN_T_TAGLIST_IV1] = ST_B_FALSE;
		allwTags[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;
		ast_cln_stc_rsetT_taglist(tmpOutTagsDst, ST_B_FALSE);

		resB = AST_CLN__psvt_parseTagElem(pFnc, pCmdln, pLeftTag, slen,
				allwTags,
				/*autoAllw:*/ST_B_FALSE, /*nonAllw:*/ST_B_FALSE,
				/*allAllw:*/ST_B_FALSE,
				tmpOutTagsDst,
				/*pAuto:*/NULL, /*pNone:*/NULL);
		if (resB) {
			Tst_uint32 x;

			for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
				if (tmpOutTagsDst[x]) {
					pFP->ttp = (Tast_cln_t_tagListIx)x;
					break;
				}
		}
	}
	/* parse field name in pFP->pFldname */
	if (resB)
		slen = st_sysStrlen(pFP->pFldname);
	if (resB && slen < 3) {
		pCmdln->cbErr(pCmdln->pAppFn, pFnc,
				"field name too short (min. 3 chars) in <%s>, aborting", pFmt);
		resB = ST_B_FALSE;
	} else if (resB) {
		char *pCh = (char*)pFP->pFldname;

		/**ast_cln_prf(" fname: '%s'\n", pFP->pFldname);**/
		st_sysStrToUpper(pFP->pFldname);
		while (*pCh) {
			if (! (*pCh >= 0x20 && *pCh <= 0x7d) || *pCh == '=') {
				pCmdln->cbErr(pCmdln->pAppFn, pFnc,
						"invalid char in field name in <%s> (inv ch=0x%02x)",
						pFmt, *pCh);
				pFP->pFldname[0] = 0;
				resB             = ST_B_FALSE;
				break;
			}
			++pCh;
		}
		/**ast_cln_prf(" -->fname: '%s'\n", pFP->pFldname);**/
	}
	/* parse field nr. in pLeftFNr */
	if (resB)
		slen = st_sysStrlen(pLeftFNr);
	if (resB && slen > 3) {
		pCmdln->cbErr(pCmdln->pAppFn, pFnc,
				"field nr. too long (max. 3 digits) in <%s>, aborting", pFmt);
		resB = ST_B_FALSE;
	} else if (resB && slen > 0) {
		/**ast_cln_prf(" fnr  : '%s'\n", pLeftFNr);**/
		resB = AST_CLN__psvt_str2int(pFnc, pCmdln, pFmt, pLeftFNr, &pFP->fldnr);
		/**ast_cln_prf(" -->fnr  : %d\n", pFP->fldnr);**/
	}
	/* convert field value in pFP->pValueStr into an integer, if possible */
	if (resB)
		pFP->isIntStr = AST_CLN__psvt_str2int(NULL, NULL, NULL,
				pFP->pValueStr, &pFP->valueInt);

	ST_DELPOINT(pLeft)
	ST_DELPOINT(pLeftTag)
	ST_DELPOINT(pLeftFNaNr)
	ST_DELPOINT(pLeftFNr)

	/* check field name */
	if (resB) {
		Tst_str    invch  = 0x00;
		char const *pTagN = NULL;

		switch (pFP->ttp) {
		case AST_CLN_T_TAGLIST_IV2:
			pTagN = ST_ID3V2_TAG_NAME;
			resB  = ! st_id3v2_fnc_hasIDstrInvalidChars(pFP->pFldname,
						/*mod:*/ST_B_FALSE, &invch);
			break;
		case AST_CLN_T_TAGLIST_AV2:
			pTagN = ST_APEV2_TAG_NAME;
			resB  = ! st_apev2_fnc_hasIDstrInvalidChars(pFP->pFldname, &invch);
			break;
		case AST_CLN_T_TAGLIST_VOR:
			pTagN = ST_VORBC_TAG_NAME;
			resB  = ! st_vorbc_fnc_hasIDstrInvalidChars(pFP->pFldname, &invch);
			break;
		default:
			break;
		}
		if (! resB) {
			if (invch == ' ' && pFP->ttp == AST_CLN_T_TAGLIST_IV2)
				pCmdln->cbErr(pCmdln->pAppFn, pFnc,
						"%s tag allows whitespaces in field names only "
						"as last char (for %s.3/4)",
						pTagN, pTagN);
			else
				pCmdln->cbErr(pCmdln->pAppFn, pFnc,
						"%s tag doesn't allow the char '%c' in field names",
						pTagN, (char)invch);
			pFP->pFldname[0] = 0;
			resB             = ST_B_FALSE;
		}
	}

	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse <tiv2optlist> element
 *
 * returns: true if element is OK
 */
static Tst_bool
AST_CLN__psvt_parseTiv2optlistElem(Tast_cln_a *pCmdln,
		const Tst_str *pStrElem, const Tst_uint32 len,
		Tast_cln_t_tiv2opt *pTOpts)
{
	const char *cFNCN = __FUNCTION__;
	Tst_bool   isOK = ST_B_FALSE;
	char       *pT  = NULL;
	Tast_cln_t_tiv2optList_capDesc const *pCapDesc = AST_CLN_TIV2OPTLIST_CAPDESC;

	ST_CALLOC(pT, char*, len + 1, 1)
	if (pT == NULL)
		return ST_B_FALSE;
	memcpy(pT, pStrElem, len);
	pT[len] = 0;

	/* */
	while (pCapDesc->pCap != NULL && pCapDesc->pDesc != NULL) {
		if (! ast_cln_fnc_strcmp2xN(pT, len, pCapDesc->pCap, NULL)) {
			++pCapDesc;
			continue;
		}
		/* */
		if (st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_TIV2O_CMPR))
			pTOpts->allwCompr = ST_B_TRUE;
		else if (st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_TIV2O_NOCMPR))
			pTOpts->allwCompr = ST_B_FALSE;
		else if (st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_TIV2O_UNSY))
			pTOpts->allwUnsynch = ST_B_TRUE;
		else if (st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_TIV2O_NOUNSY))
			pTOpts->allwUnsynch = ST_B_FALSE;
		else if (st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_TIV2O_V2))
			pTOpts->saveAsVers = 2;
		else if (st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_TIV2O_V3))
			pTOpts->saveAsVers = 3;
		else if (st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_TIV2O_V4))
			pTOpts->saveAsVers = 4;
		isOK = ST_B_TRUE;
		break;
	}

	if (! isOK)
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"invalid element '%s' in <%s>, aborting",
				pT, AST_CLN_SVT_FMT_TIV2OPTLIST);
	/**else
		ast_cln_prf(" tiv2opt: cmpr=%d, unsy=%d, v=%u\n",
				(int)pTOpts->allwCompr, (int)pTOpts->allwUnsynch, pTOpts->saveAsVers);**/
	ST_DELPOINT(pT)
	return isOK;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
AST_CLN__psvt_str2int(ST_OPTARG(const char *pFnc), ST_OPTARG(Tast_cln_a *pCmdln),
		ST_OPTARG(const char *pDatatype), const Tst_str *pStr, Tst_int32 *pInt)
{
	char const *pCh = (char const*)pStr;
	Tst_uint32 len  = 0;

	while (*pCh) {
		++len;
		if (len > 9 || ! isdigit((int)*pCh)) {
			if (pCmdln != NULL && pFnc != NULL && pDatatype != NULL) {
				if (len > 9)
					pCmdln->cbErr(pCmdln->pAppFn, pFnc,
							"<%s> too long (max. 9 digits) in <%s>, aborting",
							AST_CLN_SVT_FMT_NUM, pDatatype);
				else
					pCmdln->cbErr(pCmdln->pAppFn, pFnc,
							"invalid char '%c' in <%s>, aborting",
							*pCh, pDatatype);
			}
			return ST_B_FALSE;
		}
		++pCh;
	}
	sscanf((const char*)pStr, "%d", pInt);
	return ST_B_TRUE;
}

static Tst_bool
AST_CLN__psvt_hasSeparator(const Tst_str *pStr, const char separator,
		const Tst_bool canBeEscaped)
{
	Tst_str const *pC,
	              *pPrev;

	pC = (Tst_str const*)strchr((const char*)pStr, separator);
	if (pC == NULL)
		return ST_B_FALSE;
	if (! canBeEscaped || pC == pStr)
		return ST_B_TRUE;

	/* check for escaped separator */
	pPrev = (Tst_str const*)(pC - 1);
	while (*pC != 0 && *pPrev == (const Tst_str)'\\') {
		/**ast_cln_prf(" --HS: str='%s', escaped '%c'--\n", pStr, separator);**/
		do {
			++pC;
			++pPrev;
		} while (*pC != 0 && *pC != (const Tst_str)separator);
	}
	return (*pC == (const Tst_str)separator);
}

static Tst_bool
AST_CLN__psvt_splitstr(const Tst_str *pStr, const char separator,
		const Tst_bool canBeEscaped,
		Tst_str **ppLeft, Tst_str **ppRight)
{
	char const *pChSplit;
	Tst_uint32 slen;
	Tst_int32  cplen;
	Tst_bool   isEscaped = ST_B_FALSE;

	ST_ASSERTN_BOOL(ST_B_FALSE, pStr == NULL || ppLeft == NULL ||
			ppRight == NULL)

	slen     = st_sysStrlen(pStr);
	pChSplit = strchr((const char*)pStr, separator);
	if (pChSplit == NULL)
		pChSplit = (char const*)(pStr + slen);
	/* length of left part */
	cplen = (Tst_int32)(pChSplit - (const char*)pStr);
	/* check for escaped separator */
	if (canBeEscaped)
		while (cplen > 0 && cplen < slen && pStr[cplen - 1] == (const Tst_str)'\\') {
			/**ast_cln_prf(" --SS: str='%s', escaped '%c'--\n", pStr, separator);**/
			isEscaped = ST_B_TRUE;
			do {
				++pChSplit;
				++cplen;
			} while (cplen < slen && pStr[cplen] != (const Tst_str)separator);
		}

	/* copy left part */
	if (! AST_CLN__psvt_copystr(pStr, cplen, ppLeft))
		return ST_B_FALSE;
	/* unescape */
	if (canBeEscaped && isEscaped && ! AST_CLN__psvt_unescape(ppLeft, separator))
		return ST_B_FALSE;

	/* copy right part */
	if (cplen >= 0 && slen >= (Tst_uint32)cplen)
		cplen = (Tst_int32)slen - cplen;
	else
		cplen = 0;
	if (! AST_CLN__psvt_copystr((const Tst_str*)(pChSplit + 1), cplen, ppRight))
		return ST_B_FALSE;
	/* unescape */
	if (canBeEscaped && cplen > 1 && ! AST_CLN__psvt_unescape(ppRight, separator))
		return ST_B_FALSE;

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

static Tst_bool
AST_CLN__psvt_unescape(Tst_str **ppStr, const char separator)
{
	Tst_str    repl[3],
	           rwith[2];
	Tst_buf    sbuf[2];
	Tst_uint32 slen;
	Tst_mtes_string tmpS;

	sbuf[0] = '\\';
	sbuf[1] = (Tst_buf)separator;
	slen    = st_sysStrlen(*ppStr);
	if (slen < 2 || ! st_sysBufSearch((const Tst_buf*)*ppStr, slen,
				sbuf, sizeof(sbuf), NULL))
		return ST_B_TRUE;

	/**ast_cln_prf(" --UE: str='%s', escaped '%c'--\n", *ppStr, separator);**/

	if (st_mtes_stc_initStr(&tmpS) != ST_ERR_SUCC)
		return ST_B_FALSE;
	if (st_mtes_copyFromCharp_utf8(*ppStr, &tmpS) != ST_ERR_SUCC)
		return ST_B_FALSE;
	repl[0] = (Tst_str)'\\';
	repl[1] = (Tst_str)separator;
	repl[2] = 0;
	rwith[0] = (Tst_str)separator;
	rwith[1] = 0;
	if (st_mtes_strReplace(&tmpS, repl, rwith, NULL) != ST_ERR_SUCC)
		return ST_B_FALSE;
	if (st_mtes_copyToCharp_utf8(&tmpS, ppStr) != ST_ERR_SUCC)
		return ST_B_FALSE;
	st_mtes_stc_freeStr(&tmpS);
	/**ast_cln_prf(" --UE: --> str='%s'--\n", *ppStr);**/
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/* maxlen<0 means copy entire string */
static Tst_bool
AST_CLN__psvt_copystr(const Tst_str *pSrc, Tst_int32 maxlen,
		Tst_str **ppDst)
{
	Tst_uint32 slen;

	slen = st_sysStrlen(pSrc);
	if (maxlen < 0)
		maxlen = st_sysStrlen(pSrc);
	else if (slen < (Tst_uint32)maxlen)
		maxlen = (Tst_int32)slen;
	ST_REALLOC(*ppDst, Tst_str*, (Tst_uint32)maxlen + 1, 1)
	if (*ppDst == NULL)
		return ST_B_FALSE;
	if (maxlen > 0)
		strncpy((char*)*ppDst, (const char*)pSrc, (Tst_uint32)maxlen);
	(*ppDst)[(Tst_uint32)maxlen] = 0;
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Check if list is not empty
 */
static Tst_bool
AST_CLN__psvt_isLstNotEmpty(const char *pFnc, const char *pFmt,
		Tast_cln_a *pCmdln, Tast_cln__psvt_gnle *pGNLE,
		ST_OPTARG(Tst_uint32 *pCnt))
{
	Tst_str const *pOrgCurCh = pGNLE->pCurCh;
	Tst_uint32    orgBeg     = pGNLE->beg,
	              orgEnd     = pGNLE->end;
	Tst_bool      fnd        = ST_B_FALSE;

	/**ast_cln_prf("LST: '%s'\n", pGNLE->pStr);**/
	if (pCnt != NULL)
		*pCnt = 0;
	while (AST_CLN__psvt_getNextLstElem(pGNLE)) {
		fnd = ST_B_TRUE;
		if (pCnt != NULL)
			++(*pCnt);
	}
	if (! fnd)
		pCmdln->cbErr(pCmdln->pAppFn, pFnc,
				"empty <%s> given, aborting", pFmt);

	pGNLE->pCurCh  = pOrgCurCh;
	pGNLE->beg     = orgBeg;
	pGNLE->end     = orgEnd;
	pGNLE->pElem   = NULL;
	pGNLE->elemLen = 0;

	return fnd;
}

/*
 * Get next element from list
 *   --> pGNLE->pElem and pGNLE->elemLen
 */
static Tst_bool
AST_CLN__psvt_getNextLstElem(Tast_cln__psvt_gnle *pGNLE)
{
	Tst_bool fnd = ST_B_FALSE;

	ST_ASSERTN_BOOL(ST_B_FALSE, pGNLE == NULL)

	pGNLE->pElem   = NULL;
	pGNLE->elemLen = 0;
	if (pGNLE->pCurCh == NULL || *pGNLE->pCurCh == 0)
		return ST_B_FALSE;
	while (*pGNLE->pCurCh) {
		if (*pGNLE->pCurCh == ',') {
			if (pGNLE->end > pGNLE->beg) {
				pGNLE->pElem   = (Tst_str const*)&pGNLE->pStr[pGNLE->beg];
				pGNLE->elemLen = pGNLE->end - pGNLE->beg;
				fnd            = ST_B_TRUE;
			}
			pGNLE->beg = pGNLE->end + 1;
		}
		++pGNLE->pCurCh;
		++pGNLE->end;
		if (fnd)
			return ST_B_TRUE;
	}
	if (pGNLE->beg < pGNLE->end) {
		pGNLE->pElem   = (Tst_str const*)&pGNLE->pStr[pGNLE->beg];
		pGNLE->elemLen = pGNLE->end - pGNLE->beg;
		return ST_B_TRUE;
	}
	return ST_B_FALSE;
}

/******************************************************************************/
