/******************************************************************************/
/* mf_read-pp.h                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for reading Audio/Video/Tag files              */
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

#ifndef HEAD_MF_READ_PP_H
#define HEAD_MF_READ_PP_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "mf-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_READ_C)
	/** */
	Tst_err ast_mf_rd_readFile(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);
#endif  /* protected */


/*
// Types (private)
*/
#ifdef SRC_MF_READ_C
	/** */
	typedef struct {
		Tst_bool   jumped;
		Tst_fsize  fsz;
		Tst_uint32 iv1TagSz;
	} Tast_mf__rd_jumpState;
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_MF_READ_C
	/** */
	static Tst_err
	AST_MF__rd_getNextMimeType(const char *pFnc,
	                           Tast_cln_a *pCmdln,
	                           Tast_mf_finfo *pMF,
	                           Tst_buf *pTmpBuf,
	                           const Tst_uint32 bufSz,
	                           Tst_bool *pEOF,
	                           Tst_utilsFmt_mimeTp *pMimeTp);
	/** */
	static Tst_err
	AST_MF__rd_jumpTo(Tast_cln_a *pCmdln,
	                  Tast_mf_finfo *pMF,
	                  Tast_mf__rd_jumpState *pJState);

	/** */
	static Tst_err
	AST_MF__rd_readTag(const Tast_cln_a *pCmdln,
	                   Tast_mf_finfo *pMF,
	                   const Tst_tagMeta_type tmTagType);

	/** */
	static Tst_err
	AST_MF__rd_readAV(Tast_cln_a *pCmdln,
	                  Tast_mf_finfo *pMF,
	                  const Tst_utilsFmt_mimeTp ufmtStrType);

	static Tst_err
	AST_MF__rd_readAV_mpeg1(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);

	static Tst_err
	AST_MF__rd_readAV_oggOrFlac(Tast_cln_a *pCmdln,
	                            Tast_mf_finfo *pMF,
	                            const Tst_utilsFmt_mimeTp ufmtStrType);

	static Tst_err
	AST_MF__rd_readAV_addVorbc(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);

	/** */
	static Tst_err
	AST_MF__rd_initPCMclient(Tast_cln_a *pCmdln,
	                         Tast_mf_finfo *pMF,
	                         const char *pFnc,
	                         Tst_contWav_obj **ppWavObj);
	static Tst_err
	AST_MF__rd_endPCMclient(Tast_cln_a *pCmdln,
	                        Tast_mf_finfo *pMF,
	                        const char *pFnc,
	                        Tst_contWav_obj **ppWavObj);
	static void
	AST_MF__rd_cbPCMnewOutpStr(const Tst_uint32 bsIx,
	                           const Tst_uint32 bsSIx,
	                           const Tst_str *pFilen,
	                           const Tst_bool pretending);

	/** */
	static void
	AST_MF__rd_printID3v2_errs(const int mode, Tst_id3v2_tag *pV2tag);
#endif  /* private */


/*
// Macros (private)
*/
#ifdef SRC_MF_READ_C
	/** */
#	define AST_MF__rd_stc_rsetJState(mac_jumpState)  { \
				(mac_jumpState).jumped   = ST_B_FALSE; \
				(mac_jumpState).fsz      = 0; \
				(mac_jumpState).iv1TagSz = 0; \
				}
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_READ_PP_H */

/******************************************************************************/
