/******************************************************************************/
/* cln_fnc.c                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Sub functions for parsing program options                                  */
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
/** */
#define SRC_CLN_FNC_C
#include "cln_fnc-prot.h"
/*** */
#include "cln_stcs-prot.h"
#undef SRC_CLN_FNC_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), qsort() */
#include <string.h>      /* memcpy(), ... */
#include <unistd.h>      /* getopt(), char(optarg),           */
                         /*   int(optind,opterr,optopt), ...  */
#ifndef _GNU_SOURCE
#	define DO_UNDEF_GNU_SRC  1
#	define _GNU_SOURCE
#else
#	define DO_UNDEF_GNU_SRC  0
#endif
#include <getopt.h>      /* getopt_long(), struct option, ...  */
#if (DO_UNDEF_GNU_SRC == 1)
#	undef _GNU_SOURCE
#endif
#undef DO_UNDEF_GNU_SRC
#if (AST_CLN_FNC_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
ast_cln_fnc_checkForAmbig(const Tst_str *pAppFn, const char *pFnc,
		const Tst_uint32 argc, const Tst_str **argv,
		Tast_cln_a *pCmdln)
{
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 x,
	           slen;
	Tst_bool   isAmb = ST_B_FALSE;

	for (x = 1; x < argc; x++) {
		slen = st_sysStrlen(argv[x]);
		if (slen >= 4 && st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "-ana")) {
			/* conflict: -a, --ana */
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '--ana', please use '--ana'");
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '-a na%s', please use '-a na%s'",
					(slen > 4 ? "*" : ""), (slen > 4 ? "*" : ""));
			isAmb = ST_B_TRUE;
		} else if (slen >= 4 && st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "-add")) {
			/* conflict: -a, --add-* */
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '--add-*', please use '--add-*'");
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '-a dd%s', please use '-a dd%s'",
					(slen > 4 ? "*" : ""), (slen > 4 ? "*" : ""));
			isAmb = ST_B_TRUE;
		} else if ((slen >= 4 && slen <= 6) &&
				st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--ad")) {
			/* conflict: --add-* */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 3 && slen <= 6) &&
				st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "--m")) {
			/* conflict: --mod-* */
			isAmb = ST_B_TRUE;
		} else if (slen >= 4 && st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "-all")) {
			/* conflict: -a, --allow-links */
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '--allow-links', please use '--allow-links'");
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '-a ll%s', please use '-a ll%s'",
					(slen > 4 ? "*" : ""), (slen > 4 ? "*" : ""));
			isAmb = ST_B_TRUE;
		} else if (slen >= 5 && st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "-conv")) {
			/* conflict: -c, --conv */
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '--conv', please use '--conv'");
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '-c onv%s', please use '-c onv%s'",
					(slen > 5 ? "*" : ""), (slen > 5 ? "*" : ""));
			isAmb = ST_B_TRUE;
		} else if (slen >= 3 && st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "-di")) {
			/* conflict: -d, --disc-*, --disp-*, --disallow-links */
			if (slen < 5 || st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "-disc"))
				pCmdln->cbErr(pAppFn, pFnc,
						"if you meant '--disc-*', please use '--disc-*'");
			if (slen < 5 || st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "-disp"))
				pCmdln->cbErr(pAppFn, pFnc,
						"if you meant '--disp-*', please use '--disp-*'");
			if (slen < 5 || st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "-disa"))
				pCmdln->cbErr(pAppFn, pFnc,
						"if you meant '--disallow-links', please use '--disallow-links'");
			isAmb = ST_B_TRUE;
		} else if (slen >= 3 && st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "-tr")) {
			/* conflict: -t, --track-* */
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '--track-*', please use '--track-*'");
			isAmb = ST_B_TRUE;
		} else if (slen >= 4 && st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "-bps")) {
			/* conflict: -b, --bps */
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '--bps', please use '--bps'");
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '-b ps%s', please use '-b ps%s'",
					(slen > 4 ? "*" : ""), (slen > 4 ? "*" : ""));
			isAmb = ST_B_TRUE;
		} else if (slen == 3 && st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "--b")) {
			/* conflict: --band, --bps */
			isAmb = ST_B_TRUE;
		} else if (slen >= 5 && st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "-tiv2")) {
			/* conflict: -t, --tiv2 */
			pCmdln->cbErr(pAppFn, pFnc,
					"if you meant '--tiv2', please use '--tiv2'");
			isAmb = ST_B_TRUE;
		} else if ((slen == 6 || slen == 7) &&
				st_sysStrcmpN2(ST_B_TRUE, 6, argv[x], "--disp")) {
			/* conflict: --disp-* */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 5 && slen <= 6) &&
				(st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "--pic") ||
					st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "--geo"))) {
			/* conflict: --pic-* / --geo-* */
			isAmb = ST_B_TRUE;
		} else if (slen == 7 &&
				st_sysStrcmpN2(ST_B_TRUE, 7, argv[x], "--pic-f")) {
			/* conflict: --pic-fm, --pic-fn */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 5 && slen <= 6) &&
				st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "--rem")) {
			/* conflict: --rem-* */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 3 && slen <= 10) &&
				st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "--f") &&
				(slen < 4 || ! st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--fa")) &&
				(slen < 4 || ! st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--fs"))) {
			/* conflict: --force-*, --fadd, --fset */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 3 && slen <= 8) &&
				st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "--t") &&
				(slen < 4 || ! st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--ta")) &&
				(slen < 4 || ! st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--ti"))) {
			/* conflict: --track-*, --tag-*, --tiv2 */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 4 && slen <= 6) &&
				st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--ta")) {
			/* conflict: --tag-* */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 3 && slen <= 7) &&
				st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "--d") &&
				(slen < 4 || ! st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--ds")) &&
				(slen < 4 || ! st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--de")) &&
				(slen < 6 || ! st_sysStrcmpN2(ST_B_TRUE, 6, argv[x], "--disa"))) {
			/* conflict: --disp-*, --disc-*, --dset, --dec */
			isAmb = ST_B_TRUE;
		} else if ((slen >= 3 && slen <= 7) &&
				(st_sysStrcmpN2(ST_B_TRUE, 3, argv[x], "--e") &&
					(slen < 4 || ! st_sysStrcmpN2(ST_B_TRUE, 4, argv[x], "--eb")) &&
					(slen < 5 || ! st_sysStrcmpN2(ST_B_TRUE, 5, argv[x], "--exa")))) {
			/* conflict: --extr-*, --ebs, --examples */
			isAmb = ST_B_TRUE;
		}
		if (isAmb) {
			pCmdln->cbErr(pAppFn, pFnc,
					"option '%s' is ambiguous", argv[x]);
			pCmdln->cbErr(pAppFn, pFnc,
					"try '%s --help' for more information", pAppFn);
			res = ST_ERR_ABRT;
			break;
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/* count array elements of AST_CLN_LONGOPTS */
void
ast_cln_fnc_countLongopts(const Tast_cln_longopts *pLOpts, Tst_uint32 *pCnt)
{
	*pCnt = 0;
	while (pLOpts[*pCnt].name != NULL)
		++(*pCnt);
}

/* set up long option cache */
void
ast_cln_fnc_setLongoptsCache(const Tast_cln_longopts *pLOpts,
		const Tst_uint32 cnt, Tast_cln_longoptCache *pLOCarr)
{
	Tst_uint32 x;

	for (x = 0; x < cnt; x++) {
		pLOCarr[x].pOptName = pLOpts[x].name;
		pLOCarr[x].slen     = -1;
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Copy string argument from optarg to ppStr
 */
Tst_err
ast_cln_fnc_cpyarg_str(const Tast_cln_a *pCmdln, const char *pCap,
		const Tst_uint32 maxlen, Tst_str **ppStr)
{
	const char *cFNCN = __FUNCTION__;
	Tst_uint32 len = st_sysStrlen2(optarg);

	ST_ASSERTN_NUM(ST_ERR_FAIL, pCmdln == NULL || pCap == NULL ||
			ppStr == NULL)

	if (len > maxlen) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Warning: value for %s too long (max. %u chars)\n\n",
				pCap, maxlen);
		len = maxlen;
	}
	ST_REALLOC(*ppStr, Tst_str*, len + 2, 1)
	if (*ppStr == NULL)
		return ST_ERR_OMEM;
	if (len > 0)
		strncpy((char*)*ppStr, optarg, len);
	(*ppStr)[len] = 0;
	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_OPTS))
		pCmdln->cbDbg(NULL, "\t< %s=\"%s\"", pCap, *ppStr);
	if (len > 0 && (*ppStr)[0] == '-') {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Warning: option argument begins with '-'\n\n");
	}
	return ST_ERR_SUCC;
}

/*
 * Copy integer argument from optarg to pInt
 *
 * numMin/Max/None : min./max./'none' value for pInt
 * pCap            : name of option/command/...
 * pOArg           : will be used instead of optarg if != NULL
 *
 * returns: ERRCODE
 */
Tst_err
ast_cln_fnc_cpyarg_i32(const Tast_cln_a *pCmdln, const char *pCap,
		const Tst_int32 numMin, const Tst_int32 numMax, const Tst_int32 numNone,
		Tst_int32 *pInt)
{
	const char *cFNCN = __FUNCTION__;
	Tst_int32  lres  = numMin - 1;
	char const *pOA  = optarg;
	Tst_bool   isNum = ST_B_TRUE;

	ST_ASSERTN_NUM(ST_ERR_FAIL, pCmdln == NULL || pCap == NULL ||
			numMin == ST_TINT32_MIN || pInt == NULL)

	if (st_sysStrEmpty(optarg)) {
		/* this may happen for example with '--track-tot ""' */
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"argument for %s is empty, aborting", pCap);
		return ST_ERR_ABRT;
	}
	if (st_sysStrlen2(optarg) >= 10) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"argument for %s is too long (max. 9 chars), aborting", pCap);
		return ST_ERR_ABRT;
	}

	if (optarg[0] == '-') {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"Warning: argument begins with '-'\n\n");
	}
	while (*pOA) {
		if (*pOA < ST_CSET_ISO_DIG0 || *pOA > ST_CSET_ISO_DIG9) {
			isNum = ST_B_FALSE;
			break;
		}
		++pOA;
	}
	if (! isNum) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"argument for %s isn't a number, aborting", pCap);
		return ST_ERR_ABRT;
	}
	lres = (Tst_int32)strtol(optarg, (char**)NULL, 10);

	if (lres < numMin && lres != numNone) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"min. for %s is %d, aborting", pCap, numMin);
		return ST_ERR_ABRT;
	}
	if (lres > numMax && lres != numNone) {
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"max. for %s is %d, aborting", pCap, numMax);
		return ST_ERR_ABRT;
	}
	*pInt = lres;
	if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_OPTS))
		pCmdln->cbDbg(NULL, "\t< %s=%d", pCap, *pInt);
	return ST_ERR_SUCC;
}

/*
 * Copy integer/string argument from optarg to pInt/ppStr
 */
Tst_err
ast_cln_fnc_cpyarg_i32OrStr(const Tast_cln_a *pCmdln, const char *pCap,
		const Tst_int32 numMin, const Tst_int32 numMax, const Tst_int32 numNone,
		const Tst_int32 strMaxlen, Tst_int32 *pInt, Tst_str **ppStr)
{
	Tst_bool isNum = ST_B_FALSE;

	ST_ASSERTN_NUM(ST_ERR_FAIL, pCmdln == NULL || pCap == NULL ||
			pInt == NULL || ppStr == NULL)

	if (st_sysStrEmpty(optarg)) {
		*pInt = numNone;  /* ==none */
		ST_DELPOINT(*ppStr)
		return ST_ERR_SUCC;
	}
	/* */
	if (st_sysStrlen2(optarg) < 10) {
		char const *pOA = optarg;

		isNum = ST_B_TRUE;
		while (*pOA) {
			if (*pOA < ST_CSET_ISO_DIG0 || *pOA > ST_CSET_ISO_DIG9) {
				isNum = ST_B_FALSE;
				break;
			}
			++pOA;
		}
	}
	/* */
	if (isNum) {
		ST_DELPOINT(*ppStr)
		return ast_cln_fnc_cpyarg_i32(pCmdln, pCap,
				numMin, numMax, numNone, pInt);
	}
	return ast_cln_fnc_cpyarg_str(pCmdln, pCap, strMaxlen, ppStr);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Compare pArg with pOpt1 and pOpt2
 *
 * returns: true if pArg matches pOpt1 or pOpt2
 */
Tst_bool
ast_cln_fnc_strcmp2x(ST_OPTARG(const char *pArg),
		ST_OPTARG(const char *pOpt1), ST_OPTARG(const char *pOpt2))
{
	return (pArg != NULL &&
			((pOpt1 != NULL && st_sysStrcmp2(ST_B_TRUE, pArg, pOpt1)) ||
				(pOpt2 != NULL && st_sysStrcmp2(ST_B_TRUE, pArg, pOpt2))));
}

/*
 * Compare pArg with pOpt1 and pOpt2, but only first lenArg chars of pArg
 *
 * returns: true if pArg matches pOpt1 or pOpt2
 */
Tst_bool
ast_cln_fnc_strcmp2xN(ST_OPTARG(const char *pArg), const Tst_uint32 lenArg,
		ST_OPTARG(const char *pOpt1), ST_OPTARG(const char *pOpt2))
{
	return (pArg != NULL &&
			lenArg > 0 && lenArg <= st_sysStrlen2(pArg) &&
			((pOpt1 != NULL && lenArg == st_sysStrlen2(pOpt1) &&
				st_sysStrcmpN2(ST_B_TRUE, lenArg, pArg, pOpt1)) ||
			(pOpt2 != NULL && lenArg == st_sysStrlen2(pOpt2) &&
				st_sysStrcmpN2(ST_B_TRUE, lenArg, pArg, pOpt2))));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool
ast_cln_fnc_isAnySetInTaglist(Tast_cln_t_tagList_pb(pList))
{
	Tst_uint32 x;

	ST_ASSERTN_BOOL(ST_B_FALSE, pList == NULL)

	for (x = 0; x < AST_CLN_T_TAGLIST_CNT; x++)
		if (pList[x])
			return ST_B_TRUE;
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (AST_CLN_FNC_DEB_ == 1)
void ast_cln_prf(char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}
#endif

/******************************************************************************/
