/******************************************************************************/
/* mf_outp-pp.h                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for printing messages                          */
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

#ifndef APPSANTAGGER_MF_OUTP_PP_H
#define APPSANTAGGER_MF_OUTP_PP_H

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define AST_MF_OUTP_DEB_  1  /* enable additional debugging stuff ? */
#endif
/** */
#include "mf-prot.h"

/*
// System-Includes
*/
#include <stdarg.h>      /* va_list, ... */


ST_BEGIN_C_DECLS

/*
// Global variables (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_OUTP_C) || \
			defined(SRC_MF_SHOW_C) || defined(SRC_MF_SHOW_AV_C) || \
			defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_TIV2_C)
	/** these are implemented in mf_outp.c: */
	/*** are colors usable on stdout/err ? */
	extern Tst_bool   ast_g_mf_op_colorUsable[];
	/*** have colors been used on stdout/err ? */
	extern Tst_bool   ast_g_mf_op_colorUsed[];
	/*** length of one line on terminal */
	extern Tst_uint32 ast_g_mf_op_termLnLen;
#endif  /* protected */


/*
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_OUTP_C) || \
			defined(SRC_MF_CNVMRG_C) || \
			defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C) || \
			defined(SRC_MF_EXTR_C) || \
			defined(SRC_MF_EDIT_C) || \
			defined(SRC_MF_SHOW_C) || defined(SRC_MF_SHOW_AV_C) || \
			defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_TIV2_C) || \
			defined(SRC_MF_STCS_C)
	/** */
	Tst_bool
	ast_mf_op_isLocaleUTF8(const char *pLocale);

	/** callbacks for debug/error messages with function name */
	void
	ast_mf_op_cb_dbgFnc(const int depth,
	                    ST_OPTARG(const char* pFnc),
	                    const char* pMsg);
	void
	ast_mf_op_cb_errFnc(const int depth,
	                    ST_OPTARG(const char* pFnc),
	                    ST_OPTARG(const Tst_str* pFn),
	                    const char* pMsg);

	/** callback for analizing-mode messages */
	void
	ast_mf_op_cb_anaMsg(const char *pFmt, ...);

	/** callbacks for messages from ast_cln_* */
	void
	ast_mf_op_cb_clnDbg(ST_OPTARG(const char* pFnc),
	                    const char* pFmt,
	                    ...);

	void
	ast_mf_op_cb_clnErr(const Tst_str* pAppFn,
	                    const char* pFnc,
	                    const char* pFmt, ...);
	void
	ast_mf_op_cb_clnMsg(const char* pFmt, ...);

	/** fnc's for messages from main.c */
	void
	ast_mf_op_d_mainDeb(const char* pFmt, ...);

	void
	ast_mf_op_d_mainErrApp(ST_OPTARG(const Tst_str* pAppFn),
	                       const char* pFmt,
	                       ...);

	void
	ast_mf_op_d_mainErrFile(ST_OPTARG(const Tst_str* pFn),
	                        const char* pFmt,
	                        ...);

	/** fnc's for all other debugging/error messages */
	void
	ast_mf_op_d_deb(ST_OPTARG(const char* pFnc),
	                const char* pFmt,
	                ...);

	void
	ast_mf_op_d_mfErr(const Tast_mf_finfo *pMF,
	                  ST_OPTARG(const char* pFnc),
	                  const char* pFmt,
	                  ...);

	void
	ast_mf_op_d_tagErr(ST_OPTARG(const Tst_str *pFn),
	                   ST_OPTARG(const char* pFnc),
	                   const char* pFmt,
	                   ...);
#	define ast_mf_op_d_fileErr  ast_mf_op_d_tagErr

	/** fnc's for normal/error messages */
	void
	ast_mf_op_prMsg(const char* pFmt, ...);

	void
	ast_mf_op_prErr(ST_OPTARG(const char* pFnc),
	                const char* pFmt,
	                ...);

	/** fnc's for file info (Tags/Audio/Video) messages */
	void
	ast_mf_op_prFInf(const char* pFmt, ...);

	/** */
#	if (AST_MF_OUTP_DEB_ == 1)
	void ast_mf_op_prf(const char *pFmt, ...);
	void ast_mf_op_prE(const char *pFmt, ...);
	void ast_mf_op_prBytes(const Tst_buf *pB, const Tst_uint32 sz);
#	endif
#endif  /* protected */


/*
// Constants (private)
*/
#ifdef SRC_MF_OUTP_C
	/** */
	typedef enum {
		AST_MF_COL_FG_BLACK  = 30,  /* black */
		AST_MF_COL_FG_RED    = 31,  /* red */
		AST_MF_COL_FG_GREEN  = 32,  /* green */
		AST_MF_COL_FG_BROWN  = 33,  /* brown */
		AST_MF_COL_FG_BLUE   = 34,  /* blue */
		AST_MF_COL_FG_PURPLE = 35,  /* magenta (purple) */
		AST_MF_COL_FG_CYAN   = 36,  /* cyan (light blue) */
		AST_MF_COL_FG_GRAY   = 37,  /* gray */
		AST_MF_COL_FG_NONE   = 0
	} Tast_mf__colorFg;
	/** */
	typedef enum {
		AST_MF_COL_BG_BLACK  = 40,  /* black */
		AST_MF_COL_BG_RED    = 41,  /* red */
		AST_MF_COL_BG_GREEN  = 42,  /* green */
		AST_MF_COL_BG_BROWN  = 43,  /* brown */
		AST_MF_COL_BG_BLUE   = 44,  /* blue */
		AST_MF_COL_BG_PURPLE = 45,  /* magenta */
		AST_MF_COL_BG_CYAN   = 46,  /* cyan */
		AST_MF_COL_BG_WHITE  = 47,  /* white */
		AST_MF_COL_BG_NONE   = 0
	} Tast_mf__colorBg;
	/** */
	typedef enum {
		AST_MF_MSGDEPTH_0 = 0,
		AST_MF_MSGDEPTH_2 = 2,
		AST_MF_MSGDEPTH_4 = 4,
		AST_MF_MSGDEPTH_6 = 6
	} Tast_mf__msgDepth;
	/** */
	typedef enum {
		AST_MF_MSGTP_NOR  = 0,
		AST_MF_MSGTP_DBG  = 1,
		AST_MF_MSGTP_ERR  = 2,
		AST_MF_MSGTP_FINF = 3
	} Tast_mf__msgType;
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_MF_OUTP_C
	/** */
	static void
	AST_MF__op_prMsg(ST_OPTARG(const Tst_str *pAppFn),
	                 FILE *pOutpFile,
	                 ST_OPTARG(const Tst_str *pFn),
	                 const Tast_mf__msgDepth depthE,
	                 const Tast_mf__msgType msgTpE,
	                 ST_OPTARG(const char *pFnc),
	                 const Tst_bool prNonAsc,
	                 ST_OPTARG(const char *pMsg));
	static void
	AST_MF__op_prMsgArgs(ST_OPTARG(const Tst_str *pAppFn),
	                     FILE *pOutpFile,
	                     ST_OPTARG(const Tst_str *pFn),
	                     const Tast_mf__msgDepth depthE,
	                     const Tast_mf__msgType msgTpE,
	                     ST_OPTARG(const char *pFnc),
	                     const Tst_bool prNonAsc,
	                     ST_OPTARG(const char *pFmt),
	                     ST_OPTARG(va_list *pArgs));
#endif  /* private */


ST_END_C_DECLS

#endif  /* APPSANTAGGER_MF_OUTP_PP_H */

/******************************************************************************/
