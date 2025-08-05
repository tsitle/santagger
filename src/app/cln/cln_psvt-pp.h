/******************************************************************************/
/* cln_psvt-pp.h                  [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected sub functions for parsing special value types            */
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

#ifndef HEAD_CLN_PSVT_PP_H
#define HEAD_CLN_PSVT_PP_H

/*
// Own-Includes
*/
/** */
#include "cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CLN_PSVT_C) || defined(SRC_CLN_HNDARG_C)
	/** */
	Tst_bool
	ast_cln_psvt_taglist(Tast_cln_a *pCmdln,
	                     const Tst_str *pStrLst,
	                     const Tast_cln_t_tagList_pb(pAllwTags),
	                     const Tst_bool autoAllw,
	                     const Tst_bool nonAllw,
	                     Tast_cln_t_tagList_pb(pOutTags),
	                     Tst_bool *pAuto,
	                     Tst_bool *pNone);
	Tst_bool
	ast_cln_psvt_tagpair(Tast_cln_a *pCmdln,
	                     const Tst_str *pStrPair,
	                     const Tast_cln_t_tagList_pb(pAllwTagsLeft),
	                     const Tast_cln_t_tagList_pb(pAllwTagsRight),
	                     Tast_cln_t_tagPair *pTP);
	Tst_bool
	ast_cln_psvt_numlist(Tast_cln_a *pCmdln,
	                     const Tst_str *pStrLst,
	                     Tst_uint32 **ppArr,
	                     Tst_bool *pAll);
	Tst_bool
	ast_cln_psvt_vlevlist(Tast_cln_a *pCmdln,
	                      const Tst_str *pStrLst,
	                      Tst_int32 *pVerb,
	                      Tst_bool *pNone);
	Tst_bool
	ast_cln_psvt_bslist(Tast_cln_a *pCmdln,
	                    const Tst_str *pStrLst,
	                    Tast_cln_t_ebs **ppEBSarr,
	                    Tst_bool *pAll);
	Tst_bool
	ast_cln_psvt_pos(Tast_cln_a *pCmdln,
	                 const Tst_str *pStrPos,
	                 Tst_int32 *pPosNr,
	                 Tst_int32 *pPosTot);
	Tst_bool
	ast_cln_psvt_fldaddset(Tast_cln_a *pCmdln,
	                       const Tst_str *pStrPair,
	                       const Tst_bool addOrSet,
	                       Tast_cln_t_fldDscAddSet *pFP);
	Tst_bool
	ast_cln_psvt_dscset(Tast_cln_a *pCmdln,
	                    const Tst_str *pStrPair,
	                    Tast_cln_t_fldDscAddSet *pDP);
	Tst_bool
	ast_cln_psvt_tiv2optlist(Tast_cln_a *pCmdln,
	                         const Tst_str *pStrLst,
	                         Tast_cln_t_tiv2opt *pTOpts);
#endif  /* protected */


/*
// Functions (private)
*/
#if defined(SRC_CLN_PSVT_C)
	/** */
	static Tst_bool
	AST_CLN__psvt_parseTaglist(const char *pFnc,
	                           Tast_cln_a *pCmdln,
	                           const Tst_str *pStrLst,
	                           const Tast_cln_t_tagList_pb(pAllwTags),
	                           const Tst_bool autoAllw,
	                           const Tst_bool nonAllw,
	                           Tast_cln_t_tagList_pb(pOutTags),
	                           ST_OPTARG(Tst_bool *pAuto),
	                           ST_OPTARG(Tst_bool *pNone));
	static Tst_bool
	AST_CLN__psvt_parseTagElem(const char *pFnc,
	                           Tast_cln_a *pCmdln,
	                           const Tst_str *pStrElem,
	                           const Tst_uint32 len,
	                           const Tast_cln_t_tagList_pb(pAllwTags),
	                           const Tst_bool autoAllw,
	                           const Tst_bool nonAllw,
	                           const Tst_bool allAllw,
	                           Tast_cln_t_tagList_pb(pOutTags),
	                           ST_OPTARG(Tst_bool *pAuto),
	                           ST_OPTARG(Tst_bool *pNone));
	/** */
	static int
	AST_CLN__psvt_compNumlistElems(const void *pIx1, const void *pIx2);
	static Tst_bool
	AST_CLN__psvt_parseNumlistElem(Tast_cln_a *pCmdln,
	                               const Tst_str *pStrElem,
	                               const Tst_uint32 len,
	                               Tst_uint32 *pNr,
	                               Tst_uint32 *pRangeEnd,
	                               Tst_bool *pAll);
	/** */
	static Tst_bool
	AST_CLN__psvt_parseVlevlistElem(Tast_cln_a *pCmdln,
	                                const Tst_str *pStrElem,
	                                const Tst_uint32 len,
	                                Tst_int32 *pVL,
	                                Tst_bool *pNon);
	/** */
	static Tst_bool
	AST_CLN__psvt_parseBslistElem(Tast_cln_a *pCmdln,
	                              const Tst_str *pStrElem,
	                              const Tst_uint32 len,
	                              Tst_bool *pAll,
	                              Tast_cln_t_ebs *pEBS);
	/** */
	static Tst_bool
	AST_CLN__psvt_flddscaddset(const char *pFnc,
	                           const char *pFmt,
	                           Tast_cln_a *pCmdln,
	                           const Tst_str *pStrPair,
	                           const Tst_bool canHaveTag,
	                           const Tst_bool canHaveFldnr,
	                           const Tst_bool mustHaveValue,
	                           Tast_cln_t_fldDscAddSet *pFP);
	static Tst_bool
	AST_CLN__psvt_parseTiv2optlistElem(Tast_cln_a *pCmdln,
	                                   const Tst_str *pStrElem,
	                                   const Tst_uint32 len,
	                                   Tast_cln_t_tiv2opt *pTOpts);

	/** */
	static Tst_bool
	AST_CLN__psvt_str2int(ST_OPTARG(const char *pFnc),
	                      ST_OPTARG(Tast_cln_a *pCmdln),
	                      ST_OPTARG(const char *pDatatype),
	                      const Tst_str *pStr,
	                      Tst_int32 *pInt);
	static Tst_bool
	AST_CLN__psvt_hasSeparator(const Tst_str *pStr,
	                           const char separator,
	                           const Tst_bool canBeEscaped);
	static Tst_bool
	AST_CLN__psvt_splitstr(const Tst_str *pStr,
	                       const char separator,
	                       const Tst_bool canBeEscaped,
	                       Tst_str **ppLeft,
	                       Tst_str **ppRight);
	static Tst_bool
	AST_CLN__psvt_unescape(Tst_str **ppStr,
	                       const char separator);
	static Tst_bool
	AST_CLN__psvt_copystr(const Tst_str *pSrc,
	                      Tst_int32 maxlen,
	                      Tst_str **ppDst);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CLN_PSVT_PP_H */

/******************************************************************************/
