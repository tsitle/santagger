/******************************************************************************/
/* tag_apev2.h                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for APEv2 Tags                                                   */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_APEV2_H
#define LIBSANTAGGER_TAG_APEV2_H

/*
// Own-Includes
*/
#include "../common/binobj-defs.h"
#include "../common/string_mte-defs.h"
#include "../common/streamrd-defs.h"
#include "../common/streamwr-defs.h"
#include "tag_basics-defs.h"
#include "tag_apev2-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- tag_ape.c
*/
/** */
ST_EXPORT Tst_err
st_apev2_readTagFromStream(Tst_apev2_tag *pTag);

ST_EXPORT Tst_err
st_apev2_readTagFooterFromStream(Tst_apev2_tag *pTag,
                                 Tst_foffs *pRelativeOffsetOfHeader);

/** */
ST_EXPORT Tst_err
st_apev2_mergeTags(Tst_apev2_tag *pTagDst,
                   Tst_apev2_tag *pTagSrc,
                   const Tst_bool compareFlds);

/** */
ST_EXPORT Tst_err
st_apev2_disableFld_byID_all(Tst_apev2_tag *pTag,
                             const Tst_apev2_frID frID,
                             ST_OPTARG(const Tst_str *pFrIDstr));
ST_EXPORT Tst_err
st_apev2_disableFld_byID(Tst_apev2_tag *pTag,
                         const Tst_apev2_frID frID,
                         ST_OPTARG(const Tst_str *pFrIDstr),
                         const Tst_uint32 frNr);
ST_EXPORT Tst_err
st_apev2_disableFld_byFTP_all(Tst_apev2_tag *pTag,
                              const Tst_apev2_frTp frTp);
ST_EXPORT Tst_err
st_apev2_disableFld_byFTP(Tst_apev2_tag *pTag,
                          const Tst_apev2_frTp frTp,
                          const Tst_uint32 frNr);

/** */
ST_EXPORT Tst_err
st_apev2_tidyUpTag(Tst_apev2_tag *pTag);

/** */
ST_EXPORT Tst_err
st_apev2_writeTagToStream(Tst_apev2_tag *pTag);


/*
// Functions -- tag_ape_fldadd.c
*/
/** */
ST_EXPORT Tst_err
st_apev2_fldAdd_doesSimiliarFieldExist(Tst_apev2_tag *pTag,
                                       const Tst_bool tolerateDiffSLen,
                                       const Tst_apev2_fldData *pFld,
                                       Tst_bool *pHasSimiliarFld);
/** add/insert field */
/*** */
ST_EXPORT Tst_err
st_apev2_fldAdd(Tst_apev2_tag *pTag,
                const Tst_apev2_fldData *pFldAdd);
ST_EXPORT Tst_err
st_apev2_fldIns(Tst_apev2_tag *pTag,
                const Tst_uint32 insertPos,
                const Tst_apev2_fldData *pFldAdd);
/*** */
ST_EXPORT Tst_err
st_apev2_fldAdd_text(Tst_apev2_tag *pTag,
                     const Tst_apev2_frID frID,
                     ST_OPTARG(const Tst_str *pFrIDstr),
                     const Tst_mtes_string *pText);

ST_EXPORT Tst_err
st_apev2_fldIns_text(Tst_apev2_tag *pTag,
                     const Tst_apev2_frID frID,
                     ST_OPTARG(const Tst_str *pFrIDstr),
                     const Tst_uint32 insertPos,
                     const Tst_mtes_string *pText);
/*** */
ST_EXPORT Tst_err
st_apev2_fldAdd_textArr(Tst_apev2_tag *pTag,
                        const Tst_apev2_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        Tst_mtes_strArr *pTextArr);
ST_EXPORT Tst_err
st_apev2_fldIns_textArr(Tst_apev2_tag *pTag,
                        const Tst_apev2_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        const Tst_uint32 insertPos,
                        Tst_mtes_strArr *pTextArr);
/*** */
ST_EXPORT Tst_err
st_apev2_fldAdd_binary(Tst_apev2_tag *pTag,
                       const Tst_str *pFrIDstr,
                       Tst_binobj *pBinDat);
ST_EXPORT Tst_err
st_apev2_fldAdd_binary_rdFromFile(Tst_apev2_tag *pTag,
                                  const Tst_str *pFrIDstr,
                                  const Tst_mtes_string *pFilen);
/** set field */
/*** */
ST_EXPORT Tst_err
st_apev2_fldSet_dataStr(Tst_apev2_tag *pTag,
                        const Tst_apev2_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        const Tst_uint32 frNr,
                        const Tst_mtes_string *pText);
ST_EXPORT Tst_err
st_apev2_fldSet_dataStr_int(Tst_apev2_tag *pTag,
                            const Tst_apev2_frID frID,
                            ST_OPTARG(const Tst_str *pFrIDstr),
                            const Tst_uint32 frNr,
                            const Tst_int32 num);
ST_EXPORT Tst_err
st_apev2_fldSet_dataStrArr(Tst_apev2_tag *pTag,
                           const Tst_apev2_frID frID,
                           ST_OPTARG(const Tst_str *pFrIDstr),
                           const Tst_uint32 frNr,
                           Tst_mtes_strArr *pTextArr);
/*** */
/**** only for ST_APEV2_FTP_BDAT */
ST_EXPORT Tst_err
st_apev2_fldSet_dataBin(Tst_apev2_tag *pTag,
                        const Tst_uint32 frNr,
                        Tst_binobj *pBinDat);
ST_EXPORT Tst_err
st_apev2_fldSet_dataBin_rdFromFile(Tst_apev2_tag *pTag,
                                   const Tst_uint32 frNr,
                                   const Tst_mtes_string *pFilen);


/*
// Functions -- tag_apev2_fldget.c
*/
/** get field */
/*** */
ST_EXPORT Tst_apev2_fldData*
st_apev2_fldGet(Tst_apev2_tag *pTag,
                Tst_apev2_frID frID,
                ST_OPTARG(const Tst_str *pFrIDstr),
                const Tst_uint32 frNr);
/*** */
ST_EXPORT Tst_err
st_apev2_fldGet_dataStrArr(Tst_apev2_tag *pTag,
                           Tst_apev2_frID frID,
                           ST_OPTARG(const Tst_str *pFrIDstr),
                           const Tst_uint32 frNr,
                           Tst_mtes_strArr *pStrArr);


/*
// Functions -- tag_apev2_fnc.c
*/
/** */
ST_EXPORT Tst_bool
st_apev2_fnc_hasIDstrInvalidChars(ST_OPTARG(const Tst_str *pFIDstr),
                                  ST_OPTARG(Tst_str *pInvalidCharFound));
ST_EXPORT Tst_apev2_frID
st_apev2_fnc_getID_byIDstr(const Tst_str *pFIDstr,
                           const Tst_apev2_frTp frTyp);
ST_EXPORT Tst_err
st_apev2_fnc_getIDstr_byEnum(Tst_apev2_frID frID,
                             Tst_str **ppFIDstr);

/** */
ST_EXPORT Tst_err
st_apev2_fnc_getFieldDataTypes(const Tst_apev2_frID frID,
                               Tst_apev2_fldd_dattp *pDatTp);

ST_EXPORT Tst_err
st_apev2_fnc_getFieldStringTypes(const Tst_apev2_frID frID,
                                 Tst_apev2_fldd_strtp *pStrTp);


/*
// Functions -- tag_ape_gs.c
*/
/** Setter */
ST_EXPORT void
st_apev2_gs_setTag_strrd(Tst_apev2_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd));

ST_EXPORT void
st_apev2_gs_setTag_strwr(Tst_apev2_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr));

ST_EXPORT Tst_err
st_apev2_gs_setTag_filen(Tst_apev2_tag *pTag, const Tst_str *pFilen);

/** Getter */
/*** */
ST_EXPORT const Tst_tagBasics*
st_apev2_gs_getTag_tbas(const Tst_apev2_tag *pTag);

ST_EXPORT const Tst_str*
st_apev2_gs_getTag_filen(const Tst_apev2_tag *pTag);

ST_EXPORT Tst_uint32
st_apev2_gs_getTag_amountValidFlds(const Tst_apev2_tag *pTag);

ST_EXPORT Tst_uint32
st_apev2_gs_getTag_amountSkippedFields(const Tst_apev2_tag *pTag);

ST_EXPORT Tst_err
st_apev2_gs_getTag_taggerInfo(const Tst_apev2_tag *pTag,
                              Tst_mtes_string *pTI);

/*** */
ST_EXPORT Tst_uint32
st_apev2_gs_getField_nr(const Tst_apev2_fldData *pFld);

ST_EXPORT Tst_err
st_apev2_gs_getField_props(const Tst_apev2_fldData *pFld,
                           Tst_apev2_fldProp *pFldPr);

ST_EXPORT Tst_err
st_apev2_gs_getFieldData_stringArr(const Tst_apev2_fldData *pFld,
                                   Tst_mtes_strArr *pStrArr);

ST_EXPORT Tst_uint32
st_apev2_gs_getFieldData_stringArr_elemCount(const Tst_apev2_fldData *pFld);

ST_EXPORT Tst_err
st_apev2_gs_getFieldData_stringArr_elem(const Tst_apev2_fldData *pFld,
                                        const Tst_uint32 ix,
                                        Tst_mtes_string *pStr);

ST_EXPORT Tst_uint32
st_apev2_gs_getFieldData_geoBinSz(const Tst_apev2_fldData *pFld);

ST_EXPORT Tst_err
st_apev2_gs_getFieldData_geoBin(const Tst_apev2_fldData *pFld,
                                Tst_binobj *pBinDat);

ST_EXPORT Tst_err
st_apev2_gs_getFieldData_geoBinToFile(const Tst_apev2_fldData *pFld,
                                      const Tst_str *pOutpFilen);

/*** */
ST_EXPORT Tst_apev2_frTp
st_apev2_gs_getFieldProp_typ(const Tst_apev2_fldProp *pFldPr);

ST_EXPORT Tst_apev2_frID
st_apev2_gs_getFieldProp_id(const Tst_apev2_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_apev2_gs_getFieldProp_idStr(const Tst_apev2_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_apev2_gs_getFieldProp_caption(const Tst_apev2_fldProp *pFldPr);

/** Iterator */
/*** for existing fields in a tag */
ST_EXPORT Tst_apev2_fldData*
st_apev2_ite_nextFld(const Tst_apev2_tag *pTag,
                     ST_OPTARG(const Tst_apev2_fldData *pFldLast));

ST_EXPORT Tst_apev2_fldData*
st_apev2_ite_nextValidFld(const Tst_apev2_tag *pTag,
                          ST_OPTARG(const Tst_apev2_fldData *pFldLast));

ST_EXPORT Tst_apev2_fldData*
st_apev2_ite_nextValidFld_byID(const Tst_apev2_tag *pTag,
                               const Tst_apev2_frID frID,
                               ST_OPTARG(const Tst_str *pFrIDstr),
                               ST_OPTARG(const Tst_apev2_fldData *pFldLast));

/*** for tag field properties list */
ST_EXPORT Tst_apev2_fldProp*
st_apev2_ite_nextTagFldProp(ST_OPTARG(Tst_apev2_fldProp *pFldPrLast),
                            Tst_apev2_fldProp *pFldPrNext);


/*
// Functions -- tag_ape_opts.c
*/
/** Options: Setter */
/*** */
ST_EXPORT void
st_apev2_opts_setBasicOpts(Tst_apev2_tag *pTag,
                           ST_OPTARG(const Tst_basOpts *pBOpts));
/*** */
ST_EXPORT void
st_apev2_opts_setReadOnlyBasics(Tst_apev2_tag *pTag,
                                const Tst_bool rdOnlyBasics);


/*
// Functions -- tag_ape_stcs.c
*/
/** Structs */
/*** */
ST_EXPORT Tst_err
st_apev2_stc_initTag(Tst_apev2_tag *pTag, const Tst_str *pFilen);

ST_EXPORT void
st_apev2_stc_rsetTag(Tst_apev2_tag *pTag);

ST_EXPORT void
st_apev2_stc_freeTag(Tst_apev2_tag *pTag);

/*** */
ST_EXPORT Tst_err
st_apev2_stc_initFDat(Tst_apev2_fldData *pFld);

ST_EXPORT void
st_apev2_stc_rsetFDat(Tst_apev2_fldData *pFld);

ST_EXPORT void
st_apev2_stc_freeFDat(Tst_apev2_fldData *pFld);

/*** */
ST_EXPORT void
st_apev2_stc_rsetFPr(Tst_apev2_fldProp *pFPr);

/*** */
ST_EXPORT void
st_apev2_stc_rsetDatTp(Tst_apev2_fldd_dattp *pDatTp);

ST_EXPORT void
st_apev2_stc_rsetStrTp(Tst_apev2_fldd_strtp *pStrTp);


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_APEV2_H */

/******************************************************************************/
