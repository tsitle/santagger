/******************************************************************************/
/* mf_cnvmrg-pp.h                 [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for converting/merging Tags                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 24.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MF_CNVMRG_PP_H
#define HEAD_MF_CNVMRG_PP_H

/*
// Own-Includes
*/
/** */
#include "src/includes/tag/tag_id3v1-defs.h"
#include "src/includes/tag/tag_id3v2-defs.h"
/** */
#include "mf-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_CNVMRG_C) || \
			defined(SRC_MF_EDIT_C)
	/** */
	Tst_err
	ast_mf_cm_convAndMerge(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);
	/** */
	Tst_err
	ast_mf_cm_mergeTagsOfAkind(const Tast_cln_a *pCmdln,
	                           Tast_mf_finfo *pMF,
	                           const Tst_bool mergeEmbVorbc,
	                           Tast_mf_editInfo *pEdInf);
	/** */
	Tst_err
	ast_mf_cm_mergeCmdlnIV1(Tast_cln_a *pCmdln,
	                        Tast_mf_finfo *pMF,
	                        Tast_mf_editInfo *pEdInf,
	                        const Tst_uint32 uidOfDestTag,
	                        Tst_uint32 *pNewUID,
	                        Tst_bool *pEdited);
	Tst_err
	ast_mf_cm_mergeCmdlnIV2(Tast_cln_a *pCmdln,
	                        Tast_mf_finfo *pMF,
	                        Tast_mf_editInfo *pEdInf,
	                        const Tst_uint32 uidOfDestTag,
	                        Tst_uint32 *pNewUID,
	                        Tst_bool *pEdited);
	Tst_err
	ast_mf_cm_mergeCmdlnAV2(Tast_cln_a *pCmdln,
	                        Tast_mf_finfo *pMF,
	                        Tast_mf_editInfo *pEdInf,
	                        const Tst_uint32 uidOfDestTag,
	                        Tst_uint32 *pNewUID,
	                        Tst_bool *pEdited);
	Tst_err
	ast_mf_cm_mergeCmdlnVOR(Tast_cln_a *pCmdln,
	                        Tast_mf_finfo *pMF,
	                        Tast_mf_editInfo *pEdInf,
	                        const Tst_uint32 uidOfDestTag,
	                        Tst_uint32 *pNewUID,
	                        Tst_bool *pEdited);
	/** */
	Tst_err
	ast_mf_cm_updRawTag(const Tast_cln_a *pCmdln,
	                    Tast_mf_finfo *pMF,
	                    Tast_mf_editInfo *pEdInf,
	                    Tst_tagMeta_mt *pMT);
	/** */
	Tst_err
	ast_mf_cm_convIv1ToX(const Tast_cln_a *pCmdln,
	                     Tast_mf_finfo *pMF,
	                     Tst_id3v1_tag *pTagSrc,
	                     const Tst_uint32 uidSrc,
	                     Tst_tagBasics const *pTBasSrc,
	                     const Tst_tagMeta_type mttpDest);
	Tst_err
	ast_mf_cm_convIv2ToX(const Tast_cln_a *pCmdln,
	                     Tast_mf_finfo *pMF,
	                     Tst_id3v2_tag *pTagSrc,
	                     const Tst_uint32 uidSrc,
	                     Tst_tagBasics const *pTBasSrc,
	                     const Tst_tagMeta_type mttpDest);
	Tst_err
	ast_mf_cm_convAv2ToX(const Tast_cln_a *pCmdln,
	                     Tast_mf_finfo *pMF,
	                     Tst_apev2_tag *pTagSrc,
	                     const Tst_uint32 uidSrc,
	                     Tst_tagBasics const *pTBasSrc,
	                     const Tst_tagMeta_type mttpDest);
	Tst_err
	ast_mf_cm_convVorToX(const Tast_cln_a *pCmdln,
	                     Tast_mf_finfo *pMF,
	                     Tst_vorbc_tag *pTagSrc,
	                     const Tst_uint32 uidSrc,
	                     Tst_tagBasics const *pTBasSrc,
	                     const Tst_tagMeta_type mttpDest,
	                     const Tst_uint32 bsIxSrc,
	                     const Tst_uint32 bsSIxSrc);
#endif  /* protected */


/*
// Functions (private)
*/
#ifdef SRC_MF_CNVMRG_C
	/** */
	static Tst_err
	AST_MF__cm_convYtoX(const Tast_cln_a *pCmdln,
	                    Tast_mf_finfo *pMF,
	                    const char *pFnc,
	                    void *pTagSrc,
	                    const Tst_uint32 uidSrc,
	                    Tst_tagBasics const *pTBasSrc,
	                    const Tst_tagMeta_type mttpSrc,
	                    Tst_tagMeta_type mttpDest,
	                    const Tst_uint32 bsIxSrc,
	                    const Tst_uint32 bsSIxSrc);
	/** */
	static Tst_err
	AST_MF__cm_convAllTagsToIV2(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);
	/** */
	static Tst_err
	AST_MF__cm_mergeTagsIV2_all(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF);
	/** */
	static Tst_err
	AST_MF__cm_mergeCmdln(Tast_cln_a *pCmdln,
	                      Tast_mf_finfo *pMF,
	                      Tast_mf_editInfo *pEdInf,
	                      const Tst_tagMeta_type mttpCur,
	                      const Tst_uint32 uidOfDestTag,
	                      Tst_uint32 *pNewUID,
	                      Tst_bool *pEdited);
	/** */
	static Tst_tagMeta_mt*
	AST_MF__cm_mergeCmdln_getTag(const Tast_cln_a *pCmdln,
	                             Tast_mf_finfo *pMF,
	                             Tast_mf_editInfo *pEdInf,
	                             const Tst_uint32 uidOfDestTag,
	                             const Tst_tagMeta_type mttpDest);
	/** */
	static Tst_err
	AST_MF__cm_mergeCmdln_getGenreCap(Tst_mtes_string *pGenreStr);
	/** */
	static Tst_err
	AST_MF__cm_mergeCmdln_remField(const Tast_cln_a *pCmdln,
	                               const Tst_tagMeta_type mttpRCur,
	                               void *pTag,
	                               const Tst_int32 frID,
	                               const Tst_bool remAll,
	                               const Tst_uint32 *pRemLst);
	static Tst_err
	AST_MF__cm_mergeCmdln_remPictOrGeob(const Tast_cln_a *pCmdln,
	                                    const Tst_tagMeta_type mttpRCur,
	                                    void *pTag,
	                                    const Tst_bool remPicOrGeo,
	                                    const Tst_bool remAll,
	                                    const Tst_uint32 *pRemLst,
	                                    Tst_bool *pEdited);
	static Tst_err
	AST_MF__cm_mergeCmdln_addOrModFld_file(const Tast_cln_a *pCmdln,
	                                       const Tst_str *pFnTag,
	                                       const Tst_tagMeta_type mttpRCur,
	                                       void *pTag,
	                                       const Tst_bool addOrMod,
	                                       Tast_cln_attFile *pAF,
	                                       Tst_bool *pEdited);
	/** */
	static Tst_err
	AST_MF__cm_updRawVorbTag(const Tast_cln_a *pCmdln,
	                         Tast_mf_finfo *pMF,
	                         Tst_vorbc_tag *pTag,
	                         const Tst_uint32 bsIx,
	                         const Tst_uint32 bsSIx);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_CNVMRG_PP_H */

/******************************************************************************/
