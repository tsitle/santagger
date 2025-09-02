/******************************************************************************/
/* tag_vorbc.h                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Vorbis Tags                                                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_VORBC_H
#define LIBSANTAGGER_TAG_VORBC_H

/*
// Own-Includes
*/
#include "../common/binobj-defs.h"
#include "../common/string_mte-defs.h"
#include "../common/streamrd-defs.h"
#include "../common/streamwr-defs.h"
#include "../utils/utils_fmt-defs.h"
#include "tag_vorbc-defs.h"
#include "tag_basics-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- tag_vorbc.c
*/
/** */
ST_EXPORT Tst_err
st_vorbc_readTagFromStream(Tst_vorbc_tag *pTag);

ST_EXPORT Tst_err
st_vorbc_readTagFromBinObj(Tst_vorbc_tag *pTag,
                           Tst_binobj *pBinDat);

/** */
ST_EXPORT Tst_err
st_vorbc_mergeTags(Tst_vorbc_tag *pTagDst,
                   Tst_vorbc_tag *pTagSrc,
                   const Tst_bool compareFlds);

/** */
ST_EXPORT Tst_err
st_vorbc_disableFld_all(Tst_vorbc_tag *pTag);

ST_EXPORT Tst_err
st_vorbc_disableFld_byID_all(Tst_vorbc_tag *pTag,
                             const Tst_vorbc_frID frID,
                             ST_OPTARG(const Tst_str *pFrIDstr));
ST_EXPORT Tst_err
st_vorbc_disableFld_byID(Tst_vorbc_tag *pTag,
                         const Tst_vorbc_frID frID,
                         ST_OPTARG(const Tst_str *pFrIDstr),
                         const Tst_uint32 frNr);

/** */
ST_EXPORT Tst_err
st_vorbc_tidyUpTag(Tst_vorbc_tag *pTag);

/** */
ST_EXPORT Tst_err
st_vorbc_updateRawTag(Tst_vorbc_tag *pTag);

ST_EXPORT Tst_err
st_vorbc_writeTagToStream(Tst_vorbc_tag *pTag);

ST_EXPORT Tst_err
st_vorbc_writeTagToBinObj(Tst_vorbc_tag *pTag,
                          Tst_binobj *pBinDat);


/*
// Functions -- tag_vorbc_fldadd.c
*/
/** */
ST_EXPORT Tst_err
st_vorbc_fldAdd_doesSimiliarFieldExist(Tst_vorbc_tag *pTag,
                                       const Tst_bool tolerateDiffSLen,
                                       const Tst_vorbc_fldData *pFld,
                                       Tst_bool *pHasSimiliarFld);
/** add/insert field */
/*** */
ST_EXPORT Tst_err
st_vorbc_fldAdd(Tst_vorbc_tag *pTag,
                const Tst_vorbc_fldData *pFldAdd);
ST_EXPORT Tst_err
st_vorbc_fldIns(Tst_vorbc_tag *pTag,
                const Tst_uint32 insertPos,
                const Tst_vorbc_fldData *pFldAdd);
/*** */
ST_EXPORT Tst_err
st_vorbc_fldAdd_text(Tst_vorbc_tag *pTag,
                     const Tst_vorbc_frID frID,
                     ST_OPTARG(const Tst_str *pFrIDstr),
                     const Tst_mtes_string *pText);
ST_EXPORT Tst_err
st_vorbc_fldIns_text(Tst_vorbc_tag *pTag,
                     const Tst_vorbc_frID frID,
                     ST_OPTARG(const Tst_str *pFrIDstr),
                     const Tst_uint32 insertPos,
                     const Tst_mtes_string *pText);
/*** */
ST_EXPORT Tst_err
st_vorbc_fldAdd_textArr(Tst_vorbc_tag *pTag,
                        const Tst_vorbc_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        Tst_mtes_strArr *pTextArr);
ST_EXPORT Tst_err
st_vorbc_fldIns_textArr(Tst_vorbc_tag *pTag,
                        const Tst_vorbc_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        const Tst_uint32 insertPos,
                        Tst_mtes_strArr *pTextArr);
/*** */
ST_EXPORT Tst_err
st_vorbc_fldAdd_pict(Tst_vorbc_tag *pTag,
                     ST_OPTARG(const Tst_mtes_string *pDesc),
                     const Tst_vorbc_picTp picTpE,
                     const Tst_utilsFmt_picFmt picFmtE,
                     Tst_binobj *pBinDat);
ST_EXPORT Tst_err
st_vorbc_fldAdd_pict_rdFromFile(Tst_vorbc_tag *pTag,
                                ST_OPTARG(const Tst_mtes_string *pDesc),
                                const Tst_vorbc_picTp picTpE,
                                Tst_utilsFmt_picFmt picFmtE,
                                const Tst_mtes_string *pFilen);

/** set field */
/*** */
/**** */
ST_EXPORT Tst_err
st_vorbc_fldSet_dataStr(Tst_vorbc_tag *pTag,
                        const Tst_vorbc_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        const Tst_uint32 frNr,
                        const Tst_mtes_string *pText);
ST_EXPORT Tst_err
st_vorbc_fldSet_dataStr_int(Tst_vorbc_tag *pTag,
                            const Tst_vorbc_frID frID,
                            ST_OPTARG(const Tst_str *pFrIDstr),
                            const Tst_uint32 frNr,
                            const Tst_int32 num);
ST_EXPORT Tst_err
st_vorbc_fldSet_dataStrArr(Tst_vorbc_tag *pTag,
                           const Tst_vorbc_frID frID,
                           ST_OPTARG(const Tst_str *pFrIDstr),
                           const Tst_uint32 frNr,
                           Tst_mtes_strArr *pTextArr);
/**** only for ST_VORBC_FTP_PICT */
ST_EXPORT Tst_err
st_vorbc_fldSet_dataBin(Tst_vorbc_tag *pTag,
                        const Tst_vorbc_frID frID,
                        const Tst_uint32 frNr,
                        Tst_binobj *pBinDat);
ST_EXPORT Tst_err
st_vorbc_fldSet_dataBin_rdFromFile(Tst_vorbc_tag *pTag,
                                   const Tst_vorbc_frID frID,
                                   const Tst_uint32 frNr,
                                   const Tst_mtes_string *pFilen);
/*** */
/**** only for ST_VORBC_FTP_PICT */
ST_EXPORT Tst_err
st_vorbc_fldSet_attrDesc(Tst_vorbc_tag *pTag,
                         const Tst_vorbc_frID frID,
                         const Tst_uint32 frNr,
                         const Tst_mtes_string *pDesc);
ST_EXPORT Tst_err
st_vorbc_fldSet_attrPicTp(Tst_vorbc_tag *pTag,
                          const Tst_vorbc_frID frID,
                          const Tst_uint32 frNr,
                          const Tst_vorbc_picTp picTpE);
ST_EXPORT Tst_err
st_vorbc_fldSet_attrPicFmt(Tst_vorbc_tag *pTag,
                           const Tst_vorbc_frID frID,
                           const Tst_uint32 frNr,
                           const Tst_utilsFmt_picFmt picFmtE);


/*
// Functions -- tag_vorbc_fldget.c
*/
/** get field */
/*** */
ST_EXPORT Tst_vorbc_fldData*
st_vorbc_fldGet(Tst_vorbc_tag *pTag,
                Tst_vorbc_frID frID,
                ST_OPTARG(const Tst_str *pFrIDstr),
                const Tst_uint32 frNr);
/*** */
ST_EXPORT Tst_err
st_vorbc_fldGet_dataStrArr(Tst_vorbc_tag *pTag,
                           Tst_vorbc_frID frID,
                           ST_OPTARG(const Tst_str *pFrIDstr),
                           const Tst_uint32 frNr,
                           Tst_mtes_strArr *pStrArr);


/*
// Functions -- tag_vorbc_fnc.c
*/
/** */
ST_EXPORT Tst_bool
st_vorbc_fnc_hasIDstrInvalidChars(ST_OPTARG(const Tst_str *pFIDstr),
                                  ST_OPTARG(Tst_str *pInvalidCharFound));

ST_EXPORT Tst_vorbc_frID
st_vorbc_fnc_getID_byIDstr(const Tst_str *pFIDstr);

ST_EXPORT Tst_err
st_vorbc_fnc_getIDstr_byEnum(Tst_vorbc_frID frID,
                             Tst_str **ppFIDstr);

/** */
ST_EXPORT Tst_err
st_vorbc_fnc_getFieldDataTypes(const Tst_vorbc_frID frID,
                               Tst_vorbc_fldd_dattp *pDatTp);

ST_EXPORT Tst_err
st_vorbc_fnc_getFieldStringTypes(const Tst_vorbc_frID frID,
                                 Tst_vorbc_fldd_strtp *pStrTp);


/*
// Functions -- tag_vorbc_gs.c
*/
/** Setter */
ST_EXPORT void
st_vorbc_gs_setTag_strrd(Tst_vorbc_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd));

ST_EXPORT void
st_vorbc_gs_setTag_strwr(Tst_vorbc_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr));

ST_EXPORT Tst_err
st_vorbc_gs_setTag_filen(Tst_vorbc_tag *pTag, const Tst_str *pFilen);

ST_EXPORT void
st_vorbc_gs_setTag_wasEmbedded(Tst_vorbc_tag *pTag, const Tst_bool yesOrNo);

/** Getter */
/*** */
ST_EXPORT Tst_uint32
st_vorbc_gs_getRawTagSize(const Tst_vorbc_tag *pTag);

ST_EXPORT Tst_err
st_vorbc_gs_getRawTag(const Tst_vorbc_tag *pTag,
                      Tst_binobj *pRawTag);

ST_EXPORT Tst_binobj*
st_vorbc_gs_getRawTagPointer(const Tst_vorbc_tag *pTag);

ST_EXPORT Tst_err
st_vorbc_gs_getRawTagIntoFile(const Tst_vorbc_tag *pTag,
                              const Tst_str *pOutpFilen);

/*** */
ST_EXPORT const Tst_tagBasics*
st_vorbc_gs_getTag_tbas(const Tst_vorbc_tag *pTag);

ST_EXPORT const Tst_str*
st_vorbc_gs_getTag_filen(const Tst_vorbc_tag *pTag);

ST_EXPORT Tst_bool
st_vorbc_gs_getTag_wasEmbedded(const Tst_vorbc_tag *pTag);

ST_EXPORT Tst_uint32
st_vorbc_gs_getTag_amountValidFlds(const Tst_vorbc_tag *pTag);

ST_EXPORT Tst_uint32
st_vorbc_gs_getTag_amountSkippedFields(const Tst_vorbc_tag *pTag);

ST_EXPORT Tst_err
st_vorbc_gs_getTag_taggerInfo(const Tst_vorbc_tag *pTag,
                              Tst_mtes_string *pTI);

/*** */
ST_EXPORT Tst_uint32
st_vorbc_gs_getField_nr(const Tst_vorbc_fldData *pFld);

ST_EXPORT Tst_err
st_vorbc_gs_getField_props(const Tst_vorbc_fldData *pFld,
                           Tst_vorbc_fldProp *pFldPr);

ST_EXPORT Tst_err
st_vorbc_gs_getFieldData_stringArr(const Tst_vorbc_fldData *pFld,
                                   Tst_mtes_strArr *pStrArr);

ST_EXPORT Tst_uint32
st_vorbc_gs_getFieldData_stringArr_elemCount(const Tst_vorbc_fldData *pFld);

ST_EXPORT Tst_err
st_vorbc_gs_getFieldData_stringArr_elem(const Tst_vorbc_fldData *pFld,
                                        const Tst_uint32 ix,
                                        Tst_mtes_string *pStr);

ST_EXPORT Tst_uint32
st_vorbc_gs_getFieldData_picBinSz(const Tst_vorbc_fldData *pFld);

ST_EXPORT Tst_err
st_vorbc_gs_getFieldData_picBin(const Tst_vorbc_fldData *pFld,
                                Tst_binobj *pBinDat);

ST_EXPORT Tst_err
st_vorbc_gs_getFieldData_picBinToFile(const Tst_vorbc_fldData *pFld,
                                      const Tst_str *pOutpFilen);

ST_EXPORT const Tst_str*
st_vorbc_gs_getFieldAttr_picMime(const Tst_vorbc_fldData *pFld);

ST_EXPORT Tst_err
st_vorbc_gs_getFieldAttr_picDesc(const Tst_vorbc_fldData *pFld,
                                  Tst_mtes_string *pDesc);

ST_EXPORT Tst_vorbc_picTp
st_vorbc_gs_getFieldAttr_picType(const Tst_vorbc_fldData *pFld);

/*** */
ST_EXPORT Tst_vorbc_frTp
st_vorbc_gs_getFieldProp_typ(const Tst_vorbc_fldProp *pFldPr);

ST_EXPORT Tst_vorbc_frID
st_vorbc_gs_getFieldProp_id(const Tst_vorbc_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_vorbc_gs_getFieldProp_idStr(const Tst_vorbc_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_vorbc_gs_getFieldProp_caption(const Tst_vorbc_fldProp *pFldPr);

/** Iterator */
ST_EXPORT Tst_vorbc_fldData*
st_vorbc_ite_nextFld(const Tst_vorbc_tag *pTag,
                     ST_OPTARG(const Tst_vorbc_fldData *pFldLast));

ST_EXPORT Tst_vorbc_fldData*
st_vorbc_ite_nextValidFld(const Tst_vorbc_tag *pTag,
                          ST_OPTARG(const Tst_vorbc_fldData *pFldLast));

ST_EXPORT Tst_vorbc_fldData*
st_vorbc_ite_nextValidFld_byID(const Tst_vorbc_tag *pTag,
                               const Tst_vorbc_frID frID,
                               ST_OPTARG(const Tst_str *pFrIDstr),
                               ST_OPTARG(const Tst_vorbc_fldData *pFldLast));

/*** for tag field properties list */
ST_EXPORT Tst_vorbc_fldProp*
st_vorbc_ite_nextTagFldProp(ST_OPTARG(Tst_vorbc_fldProp *pFldPrLast),
                            Tst_vorbc_fldProp *pFldPrNext);


/*
// Functions -- tag_vorbc_opts.c
*/
/** Options: Setter */
/*** */
ST_EXPORT void
st_vorbc_opts_setBasicOpts(Tst_vorbc_tag *pTag,
                           ST_OPTARG(const Tst_basOpts *pBOpts));
/*** */
ST_EXPORT void
st_vorbc_opts_setReadOnlyBasics(Tst_vorbc_tag *pTag,
                                const Tst_bool rdOnlyBasics);


/*
// Functions -- tag_vorbc_stcs.c
*/
/** Structs */
/*** */
ST_EXPORT Tst_err
st_vorbc_stc_initTag(Tst_vorbc_tag *pTag, const Tst_str *pFilen);

ST_EXPORT void
st_vorbc_stc_rsetTag(Tst_vorbc_tag *pTag);

ST_EXPORT void
st_vorbc_stc_freeTag(Tst_vorbc_tag *pTag);

/*** */
ST_EXPORT Tst_err
st_vorbc_stc_initFDat(Tst_vorbc_fldData *pDat);

ST_EXPORT void
st_vorbc_stc_rsetFDat(Tst_vorbc_fldData *pDat);

ST_EXPORT void
st_vorbc_stc_freeFDat(Tst_vorbc_fldData *pDat);

/*** */
ST_EXPORT void
st_vorbc_stc_rsetFPr(Tst_vorbc_fldProp *pFPr);

/*** */
ST_EXPORT void
st_vorbc_stc_rsetDatTp(Tst_vorbc_fldd_dattp *pDatTp);

ST_EXPORT void
st_vorbc_stc_rsetStrTp(Tst_vorbc_fldd_strtp *pStrTp);


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_VORBC_H */

/******************************************************************************/
