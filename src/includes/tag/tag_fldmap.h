/******************************************************************************/
/* tag_fldmap.h                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for mapping Tag Fields                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_FLDMAP_H
#define LIBSANTAGGER_TAG_FLDMAP_H

/*
// Own-Includes
*/
#include "../common/binobj-defs.h"
#include "../common/string_mte-defs.h"
#include "tag_fldmap-defs.h"
#include "tag_apev2-defs.h"
#include "tag_id3v1-defs.h"
#include "tag_id3v2-defs.h"
#include "tag_vorbc-defs.h"
#include "../utils/utils_fmt-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions
*/
/** Field Mappings */
ST_EXPORT Tst_err
st_tagFldMap_mapToIV1(Tst_tfldMap_genTagFld *pFld,
                      Tst_id3v1_fldData *pOut,
                      Tst_bool *pModeAddOrSet,
                      Tst_int32 *pFNr,
                      Tst_bool *pIsOK);
ST_EXPORT Tst_err
st_tagFldMap_mapToIV2(Tst_tfldMap_genTagFld *pFld,
                      Tst_id3v2_fldData *pOut,
                      Tst_bool *pModeAddOrSet,
                      Tst_int32 *pFNr,
                      Tst_bool *pIsOK);
ST_EXPORT Tst_err
st_tagFldMap_mapToAV2(Tst_tfldMap_genTagFld *pFld,
                      Tst_apev2_fldData *pOut,
                      Tst_bool *pModeAddOrSet,
                      Tst_int32 *pFNr,
                      Tst_bool *pIsOK);
ST_EXPORT Tst_err
st_tagFldMap_mapToVOR(Tst_tfldMap_genTagFld *pFld,
                      Tst_vorbc_fldData *pOut,
                      Tst_bool *pModeAddOrSet,
                      Tst_int32 *pFNr,
                      Tst_bool *pIsOK);

/** Field ID Mappings */
ST_EXPORT Tst_err
st_tagFldMap_mapToIV1_onlyID(Tst_tfldMap_genTagFld *pFld,
                             Tst_id3v1_frID *pIDout,
                             Tst_str **ppIDstrOut,
                             Tst_bool *pModeAddOrSet,
                             Tst_int32 *pFNr,
                             Tst_bool *pIsOK);
ST_EXPORT Tst_err
st_tagFldMap_mapToIV2_onlyID(Tst_tfldMap_genTagFld *pFld,
                             Tst_id3v2_frID *pIDout,
                             Tst_str **ppIDstrOut,
                             Tst_bool *pModeAddOrSet,
                             Tst_int32 *pFNr,
                             Tst_bool *pIsOK);
ST_EXPORT Tst_err
st_tagFldMap_mapToAV2_onlyID(Tst_tfldMap_genTagFld *pFld,
                             Tst_apev2_frID *pIDout,
                             Tst_str **ppIDstrOut,
                             Tst_bool *pModeAddOrSet,
                             Tst_int32 *pFNr,
                             Tst_bool *pIsOK);
ST_EXPORT Tst_err
st_tagFldMap_mapToVOR_onlyID(Tst_tfldMap_genTagFld *pFld,
                             Tst_vorbc_frID *pIDout,
                             Tst_str **ppIDstrOut,
                             Tst_bool *pModeAddOrSet,
                             Tst_int32 *pFNr,
                             Tst_bool *pIsOK);


/*
// Functions -- tag_fldmap_gs.c
*/
/** Setter */
/*** */
ST_EXPORT Tst_err
st_tagFldMap_gs_setMode(Tst_tfldMap_genTagFld *pFld,
                        const Tst_bool addOrSet);

ST_EXPORT Tst_err
st_tagFldMap_gs_setTagType(Tst_tfldMap_genTagFld *pFld,
                           const Tst_str *pTagNameShort);

ST_EXPORT Tst_err
st_tagFldMap_gs_setFldID(Tst_tfldMap_genTagFld *pFld,
                         const Tst_int32 fldID);

ST_EXPORT Tst_err
st_tagFldMap_gs_setFldIDstr(Tst_tfldMap_genTagFld *pFld,
                            const Tst_str *pFldIDstr);

ST_EXPORT Tst_err
st_tagFldMap_gs_setFldNr(Tst_tfldMap_genTagFld *pFld,
                         const Tst_int32 fldNr);

/*** */
ST_EXPORT Tst_err
st_tagFldMap_gs_setDataStr(Tst_tfldMap_genTagFld *pFld,
                           const Tst_mtes_string *pValue);

ST_EXPORT Tst_err
st_tagFldMap_gs_setDataStr_int(Tst_tfldMap_genTagFld *pFld,
                               const Tst_int32 value);

ST_EXPORT Tst_err
st_tagFldMap_gs_setDataStr_pos(Tst_tfldMap_genTagFld *pFld,
                               const Tst_int32 valueNr,
                               const Tst_int32 valueTot);

ST_EXPORT Tst_err
st_tagFldMap_gs_setDataStr_sarr(Tst_tfldMap_genTagFld *pFld,
                                Tst_mtes_strArr *pSArr);

ST_EXPORT Tst_err
st_tagFldMap_gs_setDataBinary(Tst_tfldMap_genTagFld *pFld,
                              Tst_binobj *pBinDat);

ST_EXPORT Tst_err
st_tagFldMap_gs_setDataUVal(Tst_tfldMap_genTagFld *pFld,
                            const Tst_uint32 uval);

ST_EXPORT Tst_err
st_tagFldMap_gs_setDataBVal(Tst_tfldMap_genTagFld *pFld,
                            const Tst_byte bval);

/*** */
ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrDesc(Tst_tfldMap_genTagFld *pFld,
                            const Tst_mtes_string *pDesc);

ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrLang(Tst_tfldMap_genTagFld *pFld,
                            const Tst_str *pLangISO);

ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrGEOBfilen(Tst_tfldMap_genTagFld *pFld,
                                 const Tst_mtes_string *pFilen);

ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrGEOBmime(Tst_tfldMap_genTagFld *pFld,
                                const Tst_str *pMimeISO);

ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrPICTtype(Tst_tfldMap_genTagFld *pFld,
                                const Tst_id3v2_picTp picTp);

ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrPICTformat(Tst_tfldMap_genTagFld *pFld,
                                  const Tst_utilsFmt_picFmt picFmt);

ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrSYLTtsf(Tst_tfldMap_genTagFld *pFld,
                               const Tst_id3v2_sltTSF tsf);

ST_EXPORT Tst_err
st_tagFldMap_gs_setAttrSYLTctp(Tst_tfldMap_genTagFld *pFld,
                               const Tst_id3v2_sltCTp ctp);

/** Getter */
/*** */
ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasData_text(const Tst_tfldMap_genTagFld *pFld);

ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasData_bin(const Tst_tfldMap_genTagFld *pFld);

ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasData_uval(const Tst_tfldMap_genTagFld *pFld);

ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasData_bval(const Tst_tfldMap_genTagFld *pFld);

/*** */
ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasAttr_desc(const Tst_tfldMap_genTagFld *pFld);

ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasAttr_lang(const Tst_tfldMap_genTagFld *pFld);

ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasAttr_pict(const Tst_tfldMap_genTagFld *pFld);

ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasAttr_geob(const Tst_tfldMap_genTagFld *pFld);

ST_EXPORT Tst_bool
st_tagFldMap_gs_getHasAttr_sylt(const Tst_tfldMap_genTagFld *pFld);


/*
// Functions -- tag_fldmap_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_tagFldMap_stc_initGTF(Tst_tfldMap_genTagFld *pGTF);

ST_EXPORT void
st_tagFldMap_stc_rsetGTF(Tst_tfldMap_genTagFld *pGTF);

ST_EXPORT void
st_tagFldMap_stc_freeGTF(Tst_tfldMap_genTagFld *pGTF);


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_FLDMAP_H */

/******************************************************************************/
