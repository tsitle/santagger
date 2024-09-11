/******************************************************************************/
/* mf_show_tiv2-pp.h              [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for displaying ID3v2 Tags                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MF_SHOW_TIV2_PP_H
#define HEAD_MF_SHOW_TIV2_PP_H

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
#if defined(SRC_MF_SHOW_TIV2_C) || defined(SRC_MF_SHOW_C)
	/** */
	Tst_err
	ast_mf_sw_showTagIV2(const Tast_cln_a *pCmdln,
	                     Tast_mf_finfo *pMF,
	                     const Tst_uint32 bsIx,
	                     const Tst_uint32 bsSIx,
	                     Tst_id3v2_tag *pTag,
	                     const Tst_tagMeta_type mtTypeOrg,
	                     const Tst_bool onlyHdLn,
	                     const Tast_cln_t_tagList_pb(pMergedLst));
#endif  /* protected */


/*
// Types (private)
*/
#ifdef SRC_MF_SHOW_TIV2_C
	/** */
	typedef struct {
		Tst_uint32 six;          /* string index */
		Tst_uint32 stot;         /* amount of strings in array */
		Tst_uint32 fnr;          /* field nr */
		Tst_uint32 ftot;         /* amount of fields with same ID */
		Tst_uint32 preambelLen;  /* length of preambel */
		Tst_bool   printPreamb;  /* print preambel */
		Tst_bool   dispFStrOW;   /* display full strings ? */
		Tst_bool   prOnlyYr;     /* print only year of timestamp ? */
		Tst_bool   isNumeroid;   /* is some sort of numerical string ? */
		Tst_bool   pendingNL;    /* do we have a pending newline ? */
		/* */
		Tst_id3v2_frID           frID;       /* ID of fields to display */
		Tst_str const            *pFrIDstr;  /* ID-String of ... */
		Tst_id3v2_fldd_strtp     strTp;      /* type of string to display */
		Tst_id3v2_fldd_rword_typ rwoTp;      /* type of data to display */
		/* */
		Tast_cln_optsDisp const *pOptsD;  /* display options */
	} Tast_mf__prnfldsub;
	/** */
	typedef struct {
		Tst_bool       hasTagPre;  /* has Tag-Name in preambel ? */
		Tst_bool       hasAltern;  /* exists an alternative field ? */
		Tst_bool       prOnlyYr;   /* print only year of timestamp ? */
		Tst_bool       prScd;      /* print short cont. description ? */
		Tst_bool       isExtra;    /* is field additional to def. fields ? */
		Tst_id3v2_frID frID;       /* ID of fields to display */
		Tst_str const  *pFrIDstr;  /* ID-String of ... */
		/* */
		Tast_cln_optsDisp const *pOptsD;  /* display options */
		Tst_int32               verb;
	} Tast_mf__prnfld;
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_MF_SHOW_TIV2_C
	/** */
	static void
	AST_MF__sw_stc_rsetOPF(Tast_mf__prnfld *pPF,
	                       const Tast_cln_optsDisp *pOptsDisp,
	                       const int verb);
	static void
	AST_MF__sw_stc_rsetOPFS(Tast_mf__prnfldsub *pPFS,
	                        const Tast_cln_optsDisp *pOptsDisp);
	/** */
	static void
	AST_MF__sw_taggerInfo(const Tst_bool dispAsISOorU8,
	                      Tst_id3v2_tag *pTag,
	                      const char *pTagName);
	/** */
	static void
	AST_MF__sw_printFld_sup(const char *pFnc,
	                        const Tast_mf__prnfld *pOptsPF,
	                        Tst_id3v2_tag *pTag,
	                        const char *pTagName,
	                        const char *pFldName,
	                        Tst_bool *pPrinted);
	static Tst_err
	AST_MF__sw_printFld_sub(const char *pFnc,
	                        Tst_id3v2_tag *pTag,
	                        const Tast_mf__prnfld *pOptsPF,
	                        const Tst_uint32 cntPrintedChars,
	                        Tst_bool *pFound);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_SHOW_TIV2_PP_H */

/******************************************************************************/
