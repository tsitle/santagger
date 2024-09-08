/******************************************************************************/
/* mf_outp.c                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for printing messages                                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
/** */
#define SRC_MF_OUTP_C
#include "mf_outp-pp.h"
#undef SRC_MF_OUTP_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), free(), ... */
#include <string.h>      /* strstr(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** are colors usable on stdout/err ? */
Tst_bool   ast_g_mf_op_colorUsable[] = {ST_B_FALSE, ST_B_FALSE};
/** have colors been used on stdout/err ? */
Tst_bool   ast_g_mf_op_colorUsed[] = {ST_B_FALSE, ST_B_FALSE};
/** length of one line on terminal */
Tst_uint32 ast_g_mf_op_termLnLen = 80;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* this code is based on libutf8 */
Tst_bool
ast_mf_op_isLocaleUTF8(const char *pLocale)
{
	const char* cp       = pLocale;
	const char* encoding = NULL;

	ST_ASSERTN_BOOL(ST_B_FALSE, pLocale == NULL)

	for (; *cp != '\0' && *cp != '@' && *cp != '+' && *cp != ','; cp++) {
		if (*cp == '.') {
			encoding = ++cp;
			for (; *cp != '\0' && *cp != '@' && *cp != '+' && *cp != ','; cp++)
				;
			if ((cp - encoding == 5 &&
						st_sysStrcmpN2(ST_B_TRUE, 5, encoding, "UTF-8")) ||
					(cp - encoding == 4 &&
						st_sysStrcmpN2(ST_B_TRUE, 4, encoding, "utf8")))
				return ST_B_TRUE;
			break;
		}
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Callbacks for debug/error messages
 *   --> AST_MF__op_prMsg()
 */
void
ast_mf_op_cb_dbgFnc(const int depth, ST_OPTARG(const char* pFnc),
		const char* pMsg)
{
	Tast_mf__msgDepth depE;

	/* pFnc == NULL allowed */
	ST_ASSERTN_VOID(pMsg == NULL || pMsg[0] == 0x00)

	switch (depth) {
	case 0:  depE = AST_MF_MSGDEPTH_0; break;
	case 2:  depE = AST_MF_MSGDEPTH_2; break;
	case 4:  depE = AST_MF_MSGDEPTH_4; break;
	default: depE = AST_MF_MSGDEPTH_6;
	}
	AST_MF__op_prMsg(NULL, stdout, NULL,
			depE, AST_MF_MSGTP_DBG,
			pFnc, ST_B_FALSE, pMsg);
}

void
ast_mf_op_cb_errFnc(const int depth, ST_OPTARG(const char* pFnc),
		ST_OPTARG(const Tst_str* pFn),
		const char* pMsg)
{
	/* pFnc == NULL allowed */
	ST_ASSERTN_VOID(pMsg == NULL || pMsg[0] == 0x00)

	AST_MF__op_prMsg(NULL, stderr, pFn,
			AST_MF_MSGDEPTH_0, AST_MF_MSGTP_ERR,
			pFnc, ST_B_FALSE, pMsg);
}

/*
 * Callback for analizing-mode messages
 *   --> ast_mf_op_prMsg()
 */
void
ast_mf_op_cb_anaMsg(const char *pFmt, ...)
{
	char    buf[ST_DEBMSG_MAXSZ * 2];
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	vsnprintf((char*)buf, sizeof(buf), pFmt, args);
	va_end(args);
	ast_mf_op_prMsg(" * %s", (char*)buf);
}

/*
 * Callbacks for messages from ast_cln_*
 *   --> AST_MF__op_prMsgArgs()
 */
void
ast_mf_op_cb_clnDbg(ST_OPTARG(const char* pFnc), const char* pFmt, ...)
{
	va_list args;

	/* pFnc == NULL allowed */
	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stdout, NULL,
			AST_MF_MSGDEPTH_2, AST_MF_MSGTP_DBG,
			pFnc, ST_B_FALSE, pFmt, &args);
	va_end(args);
}

void
ast_mf_op_cb_clnErr(const Tst_str* pAppFn, const char* pFnc,
		const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pAppFn == NULL || pAppFn[0] == 0x00 ||
				pFnc == NULL || pFnc[0] == 0x00 ||
				pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(pAppFn, stderr, NULL,
			AST_MF_MSGDEPTH_2, AST_MF_MSGTP_ERR,
			pFnc, ST_B_FALSE, pFmt, &args);
	va_end(args);
}

void
ast_mf_op_cb_clnMsg(const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stdout, NULL,
			AST_MF_MSGDEPTH_0, AST_MF_MSGTP_NOR,
			NULL, ST_B_TRUE, pFmt, &args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/

/*
 * Print messages from main.c
 *   --> AST_MF__op_prMsgArgs()
 */
void
ast_mf_op_d_mainDeb(const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stdout, NULL,
			AST_MF_MSGDEPTH_2, AST_MF_MSGTP_DBG,
			"main", ST_B_FALSE, pFmt, &args);
	va_end(args);
}

void
ast_mf_op_d_mainErrApp(ST_OPTARG(const Tst_str* pAppFn), const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(pAppFn, stderr, NULL,
			AST_MF_MSGDEPTH_0, AST_MF_MSGTP_ERR,
			"main", ST_B_FALSE, pFmt, &args);
	va_end(args);
}

void
ast_mf_op_d_mainErrFile(ST_OPTARG(const Tst_str* pFn), const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stderr, pFn,
			AST_MF_MSGDEPTH_0, AST_MF_MSGTP_ERR,
			"main", ST_B_FALSE, pFmt, &args);
	va_end(args);
}

/*
 * Print all other debugging/error messages
 *   --> AST_MF__op_prMsgArgs()
 */
void
ast_mf_op_d_deb(ST_OPTARG(const char* pFnc), const char* pFmt, ...)
{
	va_list           args;
	Tast_mf__msgDepth depE;

	/* pFnc == NULL allowed */
	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	if (! st_sysStrEmpty(pFnc))
		depE = (strstr(pFnc, "__") != NULL ? AST_MF_MSGDEPTH_6 : AST_MF_MSGDEPTH_4);
	else
		depE = AST_MF_MSGDEPTH_0;
	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stdout, NULL,
			depE, AST_MF_MSGTP_DBG,
			pFnc, ST_B_FALSE, pFmt, &args);
	va_end(args);
}

void
ast_mf_op_d_mfErr(const Tast_mf_finfo *pMF, ST_OPTARG(const char* pFnc),
		const char* pFmt, ...)
{
	va_list           args;
	Tast_mf__msgDepth depE;

	/* pFnc == NULL allowed */
	ST_ASSERTN_VOID(pMF == NULL || pFmt == NULL || pFmt[0] == 0x00)

	if (! st_sysStrEmpty(pFnc))
		depE = (strstr(pFnc, "__") != NULL ? AST_MF_MSGDEPTH_6 : AST_MF_MSGDEPTH_4);
	else
		depE = AST_MF_MSGDEPTH_0;
	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stderr, st_sysFStc_getFilen(&pMF->fstc),
			depE, AST_MF_MSGTP_ERR,
			pFnc, ST_B_FALSE, pFmt, &args);
	va_end(args);
}

void
ast_mf_op_d_tagErr(ST_OPTARG(const Tst_str *pFn), ST_OPTARG(const char* pFnc),
		const char* pFmt, ...)
{
	va_list           args;
	Tast_mf__msgDepth depE;

	/* pFnc == NULL allowed */
	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	if (! st_sysStrEmpty(pFnc))
		depE = (strstr(pFnc, "__") != NULL ? AST_MF_MSGDEPTH_6 : AST_MF_MSGDEPTH_4);
	else
		depE = AST_MF_MSGDEPTH_0;
	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stderr, pFn,
			depE, AST_MF_MSGTP_ERR,
			pFnc, ST_B_FALSE, pFmt, &args);
	va_end(args);
}

/*
 * Print normal messages
 *   --> AST_MF__op_prMsgArgs()
 */
void
ast_mf_op_prMsg(const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stdout, NULL,
			AST_MF_MSGDEPTH_0, AST_MF_MSGTP_NOR,
			NULL, ST_B_TRUE, pFmt, &args);
	va_end(args);
}

/*
 * Print error messages
 *   --> AST_MF__op_prMsgArgs()
 */
void
ast_mf_op_prErr(ST_OPTARG(const char* pFnc), const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stderr, NULL,
			AST_MF_MSGDEPTH_4, AST_MF_MSGTP_ERR,
			NULL, ST_B_TRUE, pFmt, &args);
	va_end(args);
}

/*
 * Print file info (Tags/Audio/Video) messages
 *   --> AST_MF__op_prMsgArgs()
 */
void
ast_mf_op_prFInf(const char* pFmt, ...)
{
	va_list args;

	ST_ASSERTN_VOID(pFmt == NULL || pFmt[0] == 0x00)

	va_start(args, pFmt);
	AST_MF__op_prMsgArgs(NULL, stdout, NULL,
			AST_MF_MSGDEPTH_0, AST_MF_MSGTP_FINF,
			NULL, ST_B_TRUE, pFmt, &args);
	va_end(args);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_MF__op_prepColor(FILE *pFile)
{
	int x = 0;

	if (pFile == stderr)
		x = 1;
	else if (pFile != stdout)
		return;
	if (! ast_g_mf_op_colorUsable[x])
		return;
	if (ast_g_mf_op_colorUsed[x]) {
		/* restore def color */
		fwrite("\033[m", sizeof("\033[m") - 1, 1, pFile);
		fflush(pFile);
	}
	ast_g_mf_op_colorUsed[x] = ST_B_TRUE;
	fwrite("\033[0m", sizeof("\033[0m") - 1, 1, pFile);  /* rset */
}

static void
AST_MF__op_startColor(FILE *pFile,
		const Tast_mf__colorFg colFg, const Tast_mf__colorBg colBg)
{
	int x = 0;

	if (pFile == stderr)
		x = 1;
	else if (pFile != stdout)
		return;
	if (! ast_g_mf_op_colorUsable[x] ||
			(colFg == AST_MF_COL_FG_NONE && colBg == AST_MF_COL_BG_NONE))
		return;

	fwrite("\033[", sizeof("\033[") - 1, 1, pFile);
	if (colFg != AST_MF_COL_FG_NONE && colBg != AST_MF_COL_BG_NONE)
		fprintf(pFile, "%d;%dm", colBg, colFg);
	else if (colFg != AST_MF_COL_FG_NONE)
		fprintf(pFile, "%dm", colFg);
	else
		fprintf(pFile, "%dm", colBg);
}

static void
AST_MF__op_endColor(FILE *pFile)
{
	int x = 0;

	if (pFile == stderr)
		x = 1;
	else if (pFile != stdout)
		return;
	if (! (ast_g_mf_op_colorUsable[x] && ast_g_mf_op_colorUsed[x]))
		return;
	fwrite("\033[0m", sizeof("\033[0m") - 1, 1, pFile);  /* rset */
	/*fwrite("\033[K", sizeof("\033[K") - 1, 1, pFile);*/  /* clear to eol */
}

/*----------------------------------------------------------------------------*/

/*
 * Print messages
 *   --> AST_MF__op_prMsg()
 */
static void
AST_MF__op_prMsg(ST_OPTARG(const Tst_str* pAppFn), FILE *pOutpFile,
		ST_OPTARG(const Tst_str* pFn),
		const Tast_mf__msgDepth depthE, const Tast_mf__msgType msgTpE,
		ST_OPTARG(const char* pFnc), const Tst_bool prNonAsc,
		ST_OPTARG(const char* pMsg))
{
	AST_MF__op_prMsgArgs(pAppFn, pOutpFile, pFn,
			depthE, msgTpE, pFnc, prNonAsc, pMsg, NULL);
}

/*
 * Print messages
 *   --> fprintf()
 */
static void
AST_MF__op_prMsgArgs(ST_OPTARG(const Tst_str* pAppFn), FILE *pOutpFile,
		ST_OPTARG(const Tst_str* pFn),
		const Tast_mf__msgDepth depthE, const Tast_mf__msgType msgTpE,
		ST_OPTARG(const char* pFnc), const Tst_bool prNonAsc,
		ST_OPTARG(const char* pFmt), ST_OPTARG(va_list *pArgs))
{
	char const *pC;
	char       *pMBuf = NULL;
	int        depthI;
	Tst_uint32 mbufSz = 0,
	           vsnSz  = 0;
	Tst_bool   colUsa;
	va_list    argsBckp;

	ST_ASSERTN_VOID(pOutpFile == NULL)

	colUsa = (msgTpE != AST_MF_MSGTP_NOR &&
				(pOutpFile == stdout || pOutpFile == stderr));
	depthI = (int)depthE;

	if (pAppFn != NULL)
		fprintf(pOutpFile, "%s: ", (! st_sysStrEmpty(pAppFn) ? (char*)pAppFn : "--"));
	if (pFn != NULL)
		fprintf(pOutpFile, "[%s] ", (! st_sysStrEmpty(pFn) ? (char*)pFn : "--"));
	if (pAppFn == NULL && pFn == NULL && depthI > 0)
		fprintf(pOutpFile, "%*s", depthI, " ");
	if (pFnc != NULL) {
		if (colUsa) {
			AST_MF__op_prepColor(pOutpFile);
			AST_MF__op_startColor(pOutpFile, AST_MF_COL_FG_PURPLE, AST_MF_COL_BG_NONE);
		}
		/* */
		fprintf(pOutpFile, "%s()", pFnc);
		/* */
		if (colUsa)
			AST_MF__op_endColor(pOutpFile);
		/* */
		fprintf(pOutpFile, ": ");
	}
	if (pFmt && pFmt[0] != 0x00) {
		if (pArgs != NULL) {
			va_copy(argsBckp, *pArgs);
			mbufSz = 64;
			ST_CALLOC(pMBuf, char*, mbufSz, 1)
			if (pMBuf == NULL) {
				fprintf(pOutpFile, "\nOUT OF MEM (mbufSz %u, vsnSz %u)\n",
						mbufSz, vsnSz);
				va_end(argsBckp);
				return;
			}
			vsnSz = vsnprintf(pMBuf, mbufSz, pFmt, *pArgs);
			if (vsnSz >= mbufSz) {
				mbufSz = vsnSz + 2;
				ST_REALLOC(pMBuf, char*, mbufSz, 1)
				if (pMBuf == NULL) {
					fprintf(pOutpFile, "\nOUT OF MEM (mbufSz %u, vsnSz %u)\n",
							mbufSz, vsnSz);
					va_end(argsBckp);
					return;
				}
				vsnprintf(pMBuf, mbufSz, pFmt, argsBckp);
			}
			pC = pMBuf;
			va_end(argsBckp);
		} else
			pC = pFmt;
		/* */
		while (*pC)
			if (*pC == ST_CSET_ISO_SPA || *pC == ST_CSET_ISO_TAB)
				fputc(*pC++, pOutpFile);
			else
				break;
		/* */
		if (colUsa) {
			AST_MF__op_prepColor(pOutpFile);
			AST_MF__op_startColor(pOutpFile,
					(msgTpE == AST_MF_MSGTP_DBG ? AST_MF_COL_FG_GREEN :
						msgTpE == AST_MF_MSGTP_ERR ? AST_MF_COL_FG_RED :
							AST_MF_COL_FG_CYAN),
					AST_MF_COL_BG_NONE);
		}
		/* */
		while (*pC) {
			if (*pC == ST_CSET_ISO_LF && *(char*)(pC + 1) == 0) {
				if (msgTpE == AST_MF_MSGTP_FINF && colUsa) {
					AST_MF__op_endColor(pOutpFile);
					colUsa = ST_B_FALSE;
				} else
					break;
			}
			if (*pC == ST_CSET_ISO_TAB || *pC == ST_CSET_ISO_LF || prNonAsc ||
					(*pC >= ST_CSET_ISO_PRNSMA && *pC <= ST_CSET_ISO_PRNBIG))
				fputc(*pC, pOutpFile);
			else
				fprintf(pOutpFile, "[0x%02x]", (unsigned char)*pC);
			++pC;
		}
		/* */
		ST_DELPOINT(pMBuf)
		if (colUsa)
			AST_MF__op_endColor(pOutpFile);
	}
	/* */
	if (msgTpE != AST_MF_MSGTP_FINF)
		fputc('\n', pOutpFile);
	fflush(pOutpFile);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* for debugging */
#if (AST_MF_OUTP_DEB_ == 1)
void ast_mf_op_prf(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

void ast_mf_op_prE(const char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}

void ast_mf_op_prBytes(const Tst_buf *pB, const Tst_uint32 sz)
{
	Tst_uint32 x;

	printf("\n0x");
	for (x = 0; x < sz; x++)
		printf("%02x ", pB[x]);
	printf("\n");
}
#endif

/******************************************************************************/
