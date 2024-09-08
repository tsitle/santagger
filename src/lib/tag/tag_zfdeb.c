/******************************************************************************/
/* tag_zfdeb.c                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Sub functions for debugging Tags                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 02.04.2010 (dd.mm.yyyy)                                     */
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
/** */
#define SRC_TAG_ZFDEB_C
#include "tag_zfdeb-prot.h"
#undef SRC_TAG_ZFDEB_C
/** */
#if (ST_TAG_ZFDEB_DEB_ == 1)
#	include <santag/common/string_mte.h>
#endif

/*
// System-Includes
*/
#include <stdlib.h>      /* free(), ... */
#include <string.h>      /* memset(), ... */
#if (ST_TAG_ZFDEB_DEB_ == 1)
#	include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool
st_tagFDeb_isVerb(const Tst_int32 verb, const Tst_int32 mode)
{
	switch (mode) {
	case 0:  /* normal */
		if (! ST_ISVERB(verb, ST_VL_TAG))
			return ST_B_FALSE;
		break;
	case 1:  /* read value */
		if (! (ST_ISVERB(verb, ST_VL_TAG) ||
				ST_ISVERB(verb, ST_VL_TVREAD)))
			return ST_B_FALSE;
		break;
	case 2:  /* written value */
		if (! (ST_ISVERB(verb, ST_VL_TAG) ||
				ST_ISVERB(verb, ST_VL_TVWRIT)))
			return ST_B_FALSE;
		break;
	case 3:  /* set value */
		if (! (ST_ISVERB(verb, ST_VL_TAG) ||
				ST_ISVERB(verb, ST_VL_TVSET)))
			return ST_B_FALSE;
		break;
	default: /* invalid */
		return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

void
st_tagFDeb_dbg(const Tst_bool stdOrErr,
		const Tst_basOpts *pBasOpts,
		const Tst_int32 mode, const char *pFnc, const Tst_str *pFn,
		const char *pTagName, const char *pFieldStr,
		const char *pFmt, va_list args, const char *pDebStrMsg)
{
	char      buf[ST_DEBMSG_MAXSZ],
	          buf2[ST_DEBMSG_MAXSZ];
	Tst_int32 depth;
	Tst_bool  prFncN = ST_B_TRUE;

	ST_ASSERTN_VOID(pBasOpts == NULL)

	/* Fnc name */
	if (st_sysStrEmpty(pFnc))
		return;
	memset(buf2, 0, sizeof(buf2));
	depth = (strstr(pFnc, "__") != NULL ? 6 : 4) + 3;

	/* Message, Part I */
	if (stdOrErr && pTagName != NULL &&
			pBasOpts->cbsDbg.cbDbgFnc != NULL &&
			mode > 0 && ! ST_ISVERB(pBasOpts->verb, ST_VL_TAG)) {
		if (mode == 1 && ST_ISVERB(pBasOpts->verb, ST_VL_TVREAD)) {
			snprintf(buf, sizeof(buf), "\t%s: << ", pTagName);
		} else if (mode == 2 && ST_ISVERB(pBasOpts->verb, ST_VL_TVWRIT)) {
			snprintf(buf, sizeof(buf), "\t%s: >> ", pTagName);
		} else if (mode == 3 && ST_ISVERB(pBasOpts->verb, ST_VL_TVSET)) {
			snprintf(buf, sizeof(buf), "\t%s: == ", pTagName);
		} else
			buf[0] = 0;
		if (buf[0] != 0) {
			strcat(buf2, buf);
			prFncN = ST_B_FALSE;
			depth  = 0;
		}
	}

	/* Message, Part II, field ID & nr */
	if (pFieldStr != NULL) {
		snprintf(buf, sizeof(buf), "%s: ", pFieldStr);
		strcat(buf2, buf);
	}

	/* Message, Part III */
	if (pFmt != NULL) {
		vsnprintf(buf, sizeof(buf), pFmt, args);
		strcat(buf2, buf);
	}

	/* Message, Part IV, MTE String */
	if (pDebStrMsg != NULL && pDebStrMsg[0] != 0)
		strcat(buf2, pDebStrMsg);

	/* print message */
	if (buf2[0] != 0) {
		if (stdOrErr && pBasOpts->cbsDbg.cbDbgFnc != NULL)
			pBasOpts->cbsDbg.cbDbgFnc(depth,
					(prFncN ? pFnc : NULL), buf2);
		else if (! stdOrErr && pBasOpts->cbsDbg.cbErrFnc != NULL)
			pBasOpts->cbsDbg.cbErrFnc(depth, pFnc, pFn, buf2);
	}
}

void
st_tagFDeb_dbgStr(const Tst_basOpts *pBasOpts,
		const Tst_int32 mode, const char *pFnc,
		const char *pTagName, const char *pFieldStr,
		const Tst_bool withPreamble,
		const char *pDesc, const Tst_mtes_string *pStr,
		const char *pFmt, va_list args)
{
#	define LOC_PRMSG_  { \
			st_tagFDeb_dbg(ST_B_TRUE, pBasOpts, mode, pFnc, NULL, \
					pTagName, pFieldStr, pFmt, args, buf2); }
	Tst_err    res;
	Tst_bool   isok = ST_B_TRUE;
	char       buf[ST_DEBMSG_MAXSZ],
	           buf2[ST_DEBMSG_MAXSZ];
	Tst_uint32 slen,
	           strTchSz = 0;
	Tst_str    *pStrTch = NULL;
	Tst_mtes_string strT,
	                tmpPH;

	ST_ASSERTN_VOID(pBasOpts == NULL)

	memset(buf2, 0, sizeof(buf2));

	/* MTE String */
	if (pStr == NULL) {
		snprintf(buf2, sizeof(buf2), "pStr==NULL!");
		LOC_PRMSG_
		return;
	}
	if (withPreamble) {
		if (pDesc != NULL) {
			snprintf(buf, sizeof(buf), "%s ", pDesc);
			strcat(buf2, buf);
		}

		switch (st_mtes_getEncodingOrg(pStr)) {
		case ST_MTES_TE_ISO:
			snprintf(buf, sizeof(buf), "ISO  ");
			break;
		case ST_MTES_TE_UTF8:
			snprintf(buf, sizeof(buf), "U8   ");
			break;
		case ST_MTES_TE_UTF16LE:
		case ST_MTES_TE_UTF16BE:
			snprintf(buf, sizeof(buf), "U%s ",
					(st_mtes_getEncodingOrg(pStr) == ST_MTES_TE_UTF16LE ?
						"16L" : "16B"));
			break;
		default:
			snprintf(buf, sizeof(buf), "N/A  ");
			isok = ST_B_FALSE;
		}
		strcat(buf2, buf);
	}
	slen = st_mtes_strlenCh(pStr);
	snprintf(buf, sizeof(buf), "(sl %3u) ", slen);
	strcat(buf2, buf);

	if (! isok || slen < 1) {
		LOC_PRMSG_
		return;
	}

	/* shorten string */
	st_mtes_stc_initStr(&strT);
	res = st_mtes_strCpy(pStr, &strT);
	if (res == ST_ERR_SUCC && slen > 30) {
		st_mtes_stc_initStr(&tmpPH);
		res = st_mtes_setStrTo2(st_mtes_getEncodingCur(&strT), "[ .. ]", &tmpPH);
		if (res == ST_ERR_SUCC)
			res = st_mtes_shortenStr(&strT, 30, &tmpPH, /*endOrCenter:*/ST_B_FALSE);
		st_mtes_stc_freeStr(&tmpPH);
	}
	/* replace '\n' with "[\n]", ... */
	if (res == ST_ERR_SUCC)
		res = st_mtes_strReplace(&strT, (Tst_str*)"\r", (Tst_str*)"[\\r]", NULL);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strReplace(&strT, (Tst_str*)"\n", (Tst_str*)"[\\n]", NULL);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strReplace(&strT, (Tst_str*)"\"", (Tst_str*)"[\\\"]", NULL);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strReplace(&strT, (Tst_str*)"\t", (Tst_str*)"[\\t]", NULL);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strReplace(&strT, (Tst_str*)"%", (Tst_str*)"[\\%]", NULL);
	/* convert to UTF8 */
	if (res == ST_ERR_SUCC && st_mtes_getEncodingCur(&strT) != ST_MTES_TE_UTF8)
		res = st_mtes_convStrTo_utf8(&strT);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&strT);
		snprintf(buf, sizeof(buf), "-ERROR %d-", res);
		strcat(buf2, buf);
		LOC_PRMSG_
		return;
	}

	res = st_mtes_copyToCharp(&strT, &pStrTch, &strTchSz);
	if (res == ST_ERR_SUCC) {
		snprintf(buf, sizeof(buf), "\"%s\"",
				(pStrTch == NULL ? "" : (char*)pStrTch));
		strcat(buf2, buf);
		ST_DELPOINT(pStrTch)
	}
	st_mtes_stc_freeStr(&strT);
	LOC_PRMSG_
#	undef LOC_PRMSG_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_TAG_ZFDEB_DEB_ == 1)
void st_tagFDeb_prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

void st_tagFDeb_prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}

void st_tagFDeb_prBits(const Tst_buf *pSrc, const Tst_byte sz)
{
	Tst_byte x = 0;

	while (++x <= sz) {
		printf("byte %u: b", x);
		printf("%d", (pSrc[x - 1] & 0x80) == 0x80 ? 1 : 0);
		printf("%d", (pSrc[x - 1] & 0x40) == 0x40 ? 1 : 0);
		printf("%d", (pSrc[x - 1] & 0x20) == 0x20 ? 1 : 0);
		printf("%d", (pSrc[x - 1] & 0x10) == 0x10 ? 1 : 0);
		printf("%d", (pSrc[x - 1] & 0x08) == 0x08 ? 1 : 0);
		printf("%d", (pSrc[x - 1] & 0x04) == 0x04 ? 1 : 0);
		printf("%d", (pSrc[x - 1] & 0x02) == 0x02 ? 1 : 0);
		printf("%d", (pSrc[x - 1] & 0x01) == 0x01 ? 1 : 0);
		printf(" (d%u)\n", (Tst_byte)pSrc[x - 1]);
	}
	printf("\n");
}

void st_tagFDeb_prBuf(const Tst_buf *pBuf, const Tst_uint32 sz)
{
	Tst_uint32 x,
	           w = 0;

	printf("\t");
	for (x = 0; x < sz; x++) {
		printf("%02x", pBuf[x]);
		if (++w >= 25) {
			printf("\n\t");
			w = 0;
		}
	}
	if (w > 0)
		printf("\n");
}

void st_tagFDeb_prStr_(const Tst_mtes_string *pStr)
{
	Tst_str    *pStrTch  = NULL;
	Tst_uint32 strTchSz  = 0;
	Tst_mtes_string strT;

	st_mtes_stc_initStr(&strT);
	st_mtes_strCpy(pStr, &strT);
	/* convert to UTF8 */
	if (st_mtes_getEncodingCur(&strT) != ST_MTES_TE_UTF8 &&
			st_mtes_getEncodingCur(&strT) != ST_MTES_TE_ISO)
		st_mtes_convStrTo_utf8(&strT);
	st_mtes_copyToCharp(&strT, &pStrTch, &strTchSz);

	printf("'%s'\n", pStrTch);

	ST_DELPOINT(pStrTch)
	st_mtes_stc_freeStr(&strT);
}

void st_tagFDeb_prStr(const char *pMsg, const Tst_mtes_string *pStr)
{
	if (pStr != NULL) {
		printf("%s [STR] ", (pMsg ? pMsg : ""));
		st_tagFDeb_prStr_(pStr);
	} else
		printf("%s [STR]=NULL\n", (pMsg ? pMsg : ""));
}

void st_tagFDeb_prStrArr(const char *pMsg, const Tst_mtes_strArr *pSArr)
{
	if (pSArr != NULL) {
		Tst_uint32 x,
		           ecnt;
		Tst_mtes_string *pS;

		printf("%s [ARRAY]\n", (pMsg ? pMsg : ""));
		ecnt = st_mtes_strArrGetElemCount(pSArr);
		for (x = 1; x <= ecnt; x++) {
			pS = st_mtes_strArrGetElem(pSArr, x);
			printf("  ");
			st_tagFDeb_prStr_(pS);
		}
	} else
		printf("%s [ARRAY]=NULL\n", (pMsg ? pMsg : ""));
}
#endif

/******************************************************************************/
