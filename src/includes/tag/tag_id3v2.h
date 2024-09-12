/******************************************************************************/
/* tag_id3v2.h                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for ID3v2 Tag handling                                           */
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

#ifndef HEAD_TAG_ID3V2_H
#define HEAD_TAG_ID3V2_H

/*
// Own-Includes
*/
#include "../common/binobj-defs.h"
#include "../common/streamrd-defs.h"
#include "../common/streamwr-defs.h"
#include "../common/string_mte-defs.h"
#include "../utils/utils_fmt-defs.h"
#include "tag_basics-defs.h"
#include "tag_id3v2-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- tag_id3v2.c
*/
/** */
ST_EXPORT Tst_err
st_id3v2_readTagFromStream(Tst_id3v2_tag *pTag);

/** */
ST_EXPORT Tst_err
st_id3v2_mergeTags(Tst_id3v2_tag *pTagMain,
                   Tst_id3v2_tag *pTagAdd,
                   const Tst_bool compareFlds);

/** */
ST_EXPORT Tst_err
st_id3v2_disableFld_byID_all(Tst_id3v2_tag *pTag,
                             const Tst_id3v2_frID frID,
                             ST_OPTARG(const Tst_str *pFrIDstr));
ST_EXPORT Tst_err
st_id3v2_disableFld_byID(Tst_id3v2_tag *pTag,
                         const Tst_id3v2_frID frID,
                         ST_OPTARG(const Tst_str *pFrIDstr),
                         const Tst_uint32 frNr);

/** */
ST_EXPORT Tst_err
st_id3v2_tidyUpTag(Tst_id3v2_tag *pTag, const Tst_bool discardFields);

/** */
ST_EXPORT Tst_err
st_id3v2_writeTagToStream(Tst_id3v2_tag *pTag);


/*
// Functions -- tag_id3v2_fldadd.c
*/
/** */
ST_EXPORT Tst_err
st_id3v2_fldAdd_doesSimiliarFieldExist(Tst_id3v2_tag *pTag,
                                       const Tst_bool tolerateDiffSLen,
                                       const Tst_id3v2_fldData *pFld,
                                       Tst_bool *pHasSimiliarFld);
/** add/insert field */
/*** */
ST_EXPORT Tst_err
st_id3v2_fldAdd(Tst_id3v2_tag *pTag,
                const Tst_id3v2_fldData *pFldAdd);
ST_EXPORT Tst_err
st_id3v2_fldIns(Tst_id3v2_tag *pTag,
                const Tst_uint32 insertPos,
                const Tst_id3v2_fldData *pFldAdd);
/*** */
ST_EXPORT Tst_err
st_id3v2_fldAdd_text(Tst_id3v2_tag *pTag,
                     const Tst_id3v2_frID frID,
                     ST_OPTARG(const Tst_str *pFrIDstr),
                     ST_OPTARG(const Tst_mtes_string *pDesc),
                     const Tst_mtes_string *pText);
ST_EXPORT Tst_err
st_id3v2_fldIns_text(Tst_id3v2_tag *pTag,
                     const Tst_id3v2_frID frID,
                     ST_OPTARG(const Tst_str *pFrIDstr),
                     const Tst_uint32 insertPos,
                     ST_OPTARG(const Tst_mtes_string *pDesc),
                     const Tst_mtes_string *pText);
/*** */
ST_EXPORT Tst_err
st_id3v2_fldAdd_textArr(Tst_id3v2_tag *pTag,
                        const Tst_id3v2_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        ST_OPTARG(const Tst_mtes_string *pDesc),
                        Tst_mtes_strArr *pTextArr);
ST_EXPORT Tst_err
st_id3v2_fldIns_textArr(Tst_id3v2_tag *pTag,
                        const Tst_id3v2_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        const Tst_uint32 insertPos,
                        ST_OPTARG(const Tst_mtes_string *pDesc),
                        Tst_mtes_strArr *pTextArr);
/*** */
ST_EXPORT Tst_err
st_id3v2_fldAdd_comm(Tst_id3v2_tag *pTag,
                     ST_OPTARG(const Tst_str *pLangShortISO),
                     ST_OPTARG(const Tst_mtes_string *pDesc),
                     Tst_mtes_strArr *pTextArr);
ST_EXPORT Tst_err
st_id3v2_fldIns_comm(Tst_id3v2_tag *pTag,
                     const Tst_uint32 insertPos,
                     ST_OPTARG(const Tst_str *pLangShortISO),
                     ST_OPTARG(const Tst_mtes_string *pDesc),
                     Tst_mtes_strArr *pTextArr);
/*** */
ST_EXPORT Tst_err
st_id3v2_fldAdd_geob(Tst_id3v2_tag *pTag,
                     ST_OPTARG(const Tst_mtes_string *pDesc),
                     ST_OPTARG(const Tst_str *pMimeISO),
                     ST_OPTARG(const Tst_mtes_string *pFilen),
                     Tst_binobj *pBinDat);
ST_EXPORT Tst_err
st_id3v2_fldAdd_geob_rdFromFile(Tst_id3v2_tag *pTag,
                                ST_OPTARG(const Tst_mtes_string *pDesc),
                                ST_OPTARG(const Tst_str *pMimeISO),
                                const Tst_mtes_string *pFilen);
/*** */
ST_EXPORT Tst_err
st_id3v2_fldAdd_apic(Tst_id3v2_tag *pTag,
                     ST_OPTARG(const Tst_mtes_string *pDesc),
                     const Tst_id3v2_picTp picTpE,
                     const Tst_utilsFmt_picFmt picFmtE,
                     Tst_binobj *pBinDat);
ST_EXPORT Tst_err
st_id3v2_fldAdd_apic_rdFromFile(Tst_id3v2_tag *pTag,
                                ST_OPTARG(const Tst_mtes_string *pDesc),
                                const Tst_id3v2_picTp picTpE,
                                Tst_utilsFmt_picFmt picFmtE,
                                const Tst_mtes_string *pFilen);

/** set field */
/*** */
/**** */
ST_EXPORT Tst_err
st_id3v2_fldSet_dataStr(Tst_id3v2_tag *pTag,
                        const Tst_id3v2_frID frID,
                        ST_OPTARG(const Tst_str *pFrIDstr),
                        const Tst_uint32 frNr,
                        const Tst_mtes_string *pText);
ST_EXPORT Tst_err
st_id3v2_fldSet_dataStr_int(Tst_id3v2_tag *pTag,
                            const Tst_id3v2_frID frID,
                            ST_OPTARG(const Tst_str *pFrIDstr),
                            const Tst_uint32 frNr,
                            const Tst_int32 num);
ST_EXPORT Tst_err
st_id3v2_fldSet_dataStr_pos(Tst_id3v2_tag *pTag,
                            const Tst_id3v2_frID frID,
                            ST_OPTARG(const Tst_str *pFrIDstr),
                            const Tst_uint32 frNr,
                            const Tst_int32 num,
                            const Tst_int32 numTot);
ST_EXPORT Tst_err
st_id3v2_fldSet_dataStrArr(Tst_id3v2_tag *pTag,
                           const Tst_id3v2_frID frID,
                           ST_OPTARG(const Tst_str *pFrIDstr),
                           const Tst_uint32 frNr,
                           Tst_mtes_strArr *pTextArr);
/**** only for ST_ID3V2_FTP_PIC/GEO */
ST_EXPORT Tst_err
st_id3v2_fldSet_dataBin(Tst_id3v2_tag *pTag,
                        const Tst_id3v2_frID frID,
                        const Tst_uint32 frNr,
                        Tst_binobj *pBinDat);
ST_EXPORT Tst_err
st_id3v2_fldSet_dataBin_rdFromFile(Tst_id3v2_tag *pTag,
                                   const Tst_id3v2_frID frID,
                                   const Tst_uint32 frNr,
                                   const Tst_mtes_string *pFilen);
/*** */
/**** only for ST_ID3V2_FTP_COM/GEO/PIC/SLT/UFI/WXX */
ST_EXPORT Tst_err
st_id3v2_fldSet_attrDesc(Tst_id3v2_tag *pTag,
                         const Tst_id3v2_frID frID,
                         const Tst_uint32 frNr,
                         const Tst_mtes_string *pDesc);
/**** only for ST_ID3V2_FTP_GEO */
ST_EXPORT Tst_err
st_id3v2_fldSet_attrMime(Tst_id3v2_tag *pTag,
                         const Tst_id3v2_frID frID,
                         const Tst_uint32 frNr,
                         const Tst_str *pMimeISO);
/**** only for ST_ID3V2_FTP_PIC */
ST_EXPORT Tst_err
st_id3v2_fldSet_attrPicTp(Tst_id3v2_tag *pTag,
                          const Tst_id3v2_frID frID,
                          const Tst_uint32 frNr,
                          const Tst_id3v2_picTp picTpE);
ST_EXPORT Tst_err
st_id3v2_fldSet_attrPicFmt(Tst_id3v2_tag *pTag,
                           const Tst_id3v2_frID frID,
                           const Tst_uint32 frNr,
                           const Tst_utilsFmt_picFmt picFmtE);


/*
// Functions -- tag_id3v2_fldget.c
*/
/** get field */
/*** */
ST_EXPORT Tst_id3v2_fldData*
st_id3v2_fldGet(Tst_id3v2_tag *pTag,
                Tst_id3v2_frID frID,
                ST_OPTARG(const Tst_str *pFrIDstr),
                const Tst_uint32 frNr);
/*** */
ST_EXPORT Tst_err
st_id3v2_fldGet_dataStrArr(Tst_id3v2_tag *pTag,
                           Tst_id3v2_frID frID,
                           ST_OPTARG(const Tst_str *pFrIDstr),
                           const Tst_uint32 frNr,
                           Tst_mtes_strArr *pStrArr);
ST_EXPORT Tst_err
st_id3v2_fldGet_dataStr_pos(Tst_id3v2_tag *pTag,
                            Tst_id3v2_frID frID,
                            const Tst_uint32 frNr,
                            Tst_uint32 *pNum,
                            Tst_uint32 *pNumTot);


/*
// Functions -- tag_id3v2_fnc.c
*/
/** */
ST_EXPORT Tst_bool
st_id3v2_fnc_hasIDstrInvalidChars(ST_OPTARG(Tst_str *pFIDstr),
                                  const Tst_bool modInvChars,
                                  ST_OPTARG(Tst_str *pInvalidCharFound));

ST_EXPORT Tst_id3v2_frID
st_id3v2_fnc_getID_byIDstr(const Tst_str *pFIDstr);

ST_EXPORT Tst_err
st_id3v2_fnc_getIDstr_byEnum(Tst_id3v2_frID frID,
                             const Tst_uint32 tagVerMaj,
                             Tst_str **ppFIDstr);

/** */
ST_EXPORT Tst_err
st_id3v2_fnc_getFieldDataTypes(const Tst_id3v2_frID frID,
                               Tst_id3v2_fldd_dattp *pDatTp);

ST_EXPORT Tst_err
st_id3v2_fnc_getFieldStringTypes(const Tst_id3v2_frID frID,
                                 Tst_id3v2_fldd_strtp *pStrTp);

/** */
/*** */
ST_EXPORT Tst_id3v2_picTp
st_id3v2_fnc_getPicTp_enum_byBin(const Tst_byte bin);

ST_EXPORT Tst_id3v2_picTp
st_id3v2_fnc_getPicTp_enum_byCapShort(const Tst_str *pCapSh);

/*** */
ST_EXPORT Tst_byte
st_id3v2_fnc_getPicTp_bin_byEnum(const Tst_id3v2_picTp ptpE);

/*** */
ST_EXPORT const Tst_str*
st_id3v2_fnc_getPicTp_cap_byEnum(const Tst_id3v2_picTp ptpE);

/*** */
ST_EXPORT const Tst_str*
st_id3v2_fnc_getPicTp_capShort_byEnum(const Tst_id3v2_picTp ptpE);


/*
// Functions -- tag_id3v2_gs.c
*/
/** Setter */
/*** */
ST_EXPORT void
st_id3v2_gs_setTag_strrd(Tst_id3v2_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd));

ST_EXPORT void
st_id3v2_gs_setTag_strwr(Tst_id3v2_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr));

ST_EXPORT Tst_err
st_id3v2_gs_setTag_filen(Tst_id3v2_tag *pTag, const Tst_str *pFilen);

ST_EXPORT Tst_err
st_id3v2_gs_setTag_majorVersion(Tst_id3v2_tag *pTag,
                                const Tst_uint32 vers);
/*** */
ST_EXPORT void
st_id3v2_gs_setField_wasDisplayed(Tst_id3v2_fldData *pFld,
                                  const Tst_bool wasIt);

/** Getter */
/*** */
ST_EXPORT const Tst_tagBasics*
st_id3v2_gs_getTag_tbas(const Tst_id3v2_tag *pTag);

ST_EXPORT const Tst_str*
st_id3v2_gs_getTag_filen(const Tst_id3v2_tag *pTag);

ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_majorVersion(const Tst_id3v2_tag *pTag);

ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_minorVersion(const Tst_id3v2_tag *pTag);

ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_amountFlds(const Tst_id3v2_tag *pTag);

ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_amountValidFlds_byID(Tst_id3v2_tag *pTag,
                                        const Tst_id3v2_frID frID,
                                        ST_OPTARG(const Tst_str *pFrIDstr));

ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_amountValidFlds_all(Tst_id3v2_tag *pTag);

ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_amountSkippedFields(Tst_id3v2_tag *pTag);

/*** */
ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_tagErrorCount(const Tst_id3v2_tag *pTag);

ST_EXPORT Tst_err
st_id3v2_gs_getTag_tagErrors(const Tst_id3v2_tag *pTag,
                             Tst_id3v2_tagErrs *pTgErrors);

ST_EXPORT Tst_uint32
st_id3v2_gs_getTag_fldsErrorCount(const Tst_id3v2_tag *pTag);

ST_EXPORT Tst_err
st_id3v2_gs_getTag_taggerInfo(const Tst_id3v2_tag *pTag,
                              Tst_mtes_string *pTI);

/*** */
/**** */
ST_EXPORT Tst_bool
st_id3v2_gs_getField_isOK(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_bool
st_id3v2_gs_getField_wasDisplayed(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_uint32
st_id3v2_gs_getField_nr(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_err
st_id3v2_gs_getField_props(const Tst_id3v2_fldData *pFld,
                           Tst_id3v2_fldProp *pFldPr);

ST_EXPORT Tst_uint32
st_id3v2_gs_getField_errorCount(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_err
st_id3v2_gs_getField_errors(const Tst_id3v2_fldData *pFld,
                            Tst_id3v2_fldErrs *pFldErrors);

/**** */
ST_EXPORT Tst_uint32
st_id3v2_gs_getFieldData_binSz(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_err
st_id3v2_gs_getFieldData_bin(const Tst_id3v2_fldData *pFld,
                             Tst_binobj *pBinDat);

ST_EXPORT Tst_err
st_id3v2_gs_getFieldData_binToFile(const Tst_id3v2_fldData *pFld,
                                   const Tst_str *pOutpFilen);

ST_EXPORT Tst_uint32
st_id3v2_gs_getFieldData_uval(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_byte
st_id3v2_gs_getFieldData_bval(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_err
st_id3v2_gs_getFieldData_stringArr(const Tst_id3v2_fldData *pFld,
                                   Tst_mtes_strArr *pStrArr);

ST_EXPORT Tst_uint32
st_id3v2_gs_getFieldData_stringArr_elemCount(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_err
st_id3v2_gs_getFieldData_stringArr_elem(const Tst_id3v2_fldData *pFld,
                                        const Tst_uint32 ix,
                                        Tst_mtes_string *pStr);

/**** */
ST_EXPORT Tst_id3v2_picTp
st_id3v2_gs_getFieldAttr_picType(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_utilsFmt_picFmt
st_id3v2_gs_getFieldAttr_picFmt(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_id3v2_sltTSF
st_id3v2_gs_getFieldAttr_syltTSF(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_id3v2_sltCTp
st_id3v2_gs_getFieldAttr_syltContTp(const Tst_id3v2_fldData *pFld);

ST_EXPORT const Tst_str*
st_id3v2_gs_getFieldAttr_geobMime(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_err
st_id3v2_gs_getFieldAttr_geobFilen(const Tst_id3v2_fldData *pFld,
                                   Tst_mtes_string *pFn);

ST_EXPORT Tst_bool
st_id3v2_gs_getFieldAttr_geobFilen_isEmpty(const Tst_id3v2_fldData *pFld);

ST_EXPORT const Tst_str*
st_id3v2_gs_getFieldAttr_lang(const Tst_id3v2_fldData *pFld);

ST_EXPORT Tst_err
st_id3v2_gs_getFieldAttr_desc(const Tst_id3v2_fldData *pFld,
                              Tst_mtes_string *pSCD);

ST_EXPORT Tst_bool
st_id3v2_gs_getFieldAttr_desc_isEmpty(const Tst_id3v2_fldData *pFld);

/*** */
ST_EXPORT Tst_id3v2_frTp
st_id3v2_gs_getFieldProp_typ(const Tst_id3v2_fldProp *pFldPr);

ST_EXPORT Tst_id3v2_frID
st_id3v2_gs_getFieldProp_id(const Tst_id3v2_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_id3v2_gs_getFieldProp_idStr(const Tst_id3v2_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_id3v2_gs_getFieldProp_idStr_v02(const Tst_id3v2_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_id3v2_gs_getFieldProp_idStr_v34(const Tst_id3v2_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_id3v2_gs_getFieldProp_caption(const Tst_id3v2_fldProp *pFldPr);

ST_EXPORT Tst_uint32
st_id3v2_gs_getFieldProp_verFl(const Tst_id3v2_fldProp *pFldPr);

/** Iterators */
/*** for existing fields in a tag */
ST_EXPORT Tst_id3v2_fldData*
st_id3v2_ite_nextFld(const Tst_id3v2_tag *pTag,
                     ST_OPTARG(const Tst_id3v2_fldData *pFldLast));

ST_EXPORT Tst_id3v2_fldData*
st_id3v2_ite_nextValidFld(const Tst_id3v2_tag *pTag,
                          ST_OPTARG(const Tst_id3v2_fldData *pFldLast));

ST_EXPORT Tst_id3v2_fldData*
st_id3v2_ite_nextValidFld_byID(const Tst_id3v2_tag *pTag,
                               const Tst_id3v2_frID frID,
                               ST_OPTARG(const Tst_str *pFrIDstr),
                               ST_OPTARG(const Tst_id3v2_fldData *pFldLast));

/*** for tag field properties list */
ST_EXPORT Tst_id3v2_fldProp*
st_id3v2_ite_nextTagFldProp(ST_OPTARG(Tst_id3v2_fldProp *pFldPrLast),
                            Tst_id3v2_fldProp *pFldPrNext);


/*
// Functions -- tag_id3v2_opts.c
*/
/** Options: Setter */
/*** */
ST_EXPORT void
st_id3v2_opts_setBasicOpts(Tst_id3v2_tag *pTag,
                           ST_OPTARG(const Tst_basOpts *pBOpts));
/*** */
ST_EXPORT void
st_id3v2_opts_setReadOnlyBasics(Tst_id3v2_tag *pTag,
                                const Tst_bool rdOnlyBasics);
ST_EXPORT void
st_id3v2_opts_setDoUnsyncing(Tst_id3v2_tag *pTag,
                             const Tst_bool doUnsync);
ST_EXPORT void
st_id3v2_opts_setCompressBigFrames(Tst_id3v2_tag *pTag,
                                   const Tst_bool doCompr);
/*** */
ST_EXPORT void
st_id3v2_opts_setCB_cbZLib(Tst_id3v2_tag *pTag,
                           ST_OPTARG(Tst_id3v2_cbZLib_decompress cbZLibDecmpr),
                           ST_OPTARG(Tst_id3v2_cbZLib_compress cbZLibCmpr));


/*
// Functions -- tag_id3v2_stcs.c
*/
/** Structs */
/*** */
ST_EXPORT Tst_err
st_id3v2_stc_initTag(Tst_id3v2_tag *pTag, const Tst_str *pFilen);

ST_EXPORT void
st_id3v2_stc_rsetTag(Tst_id3v2_tag *pTag);

ST_EXPORT void
st_id3v2_stc_freeTag(Tst_id3v2_tag *pTag);

/*** */
ST_EXPORT Tst_err
st_id3v2_stc_initFDat(Tst_id3v2_fldData *pDat);

ST_EXPORT void
st_id3v2_stc_rsetFDat(Tst_id3v2_fldData *pDat);

ST_EXPORT void
st_id3v2_stc_freeFDat(Tst_id3v2_fldData *pDat);

/*** */
ST_EXPORT void
st_id3v2_stc_rsetFPr(Tst_id3v2_fldProp *pFldPr);

/*** */
ST_EXPORT void
st_id3v2_stc_rsetDatTp(Tst_id3v2_fldd_dattp *pDatTp);

ST_EXPORT void
st_id3v2_stc_rsetStrTp(Tst_id3v2_fldd_strtp *pStrTp);

/*** */
ST_EXPORT void
st_id3v2_stc_rsetTagErrs(Tst_id3v2_tagErrs *pErr);

ST_EXPORT void
st_id3v2_stc_rsetFldErrs(Tst_id3v2_fldErrs *pErr);


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_H */

/******************************************************************************/
