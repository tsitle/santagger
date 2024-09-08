/******************************************************************************/
/* tag_meta.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for Meta Tags (can contain any tag type)                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_META_H
#define HEAD_TAG_META_H

/*
// Own-Includes
*/
#include <santag/common/streamrd-defs.h>
#include <santag/common/streamwr-defs.h>
#include <santag/tag/tag_basics-defs.h>
#include <santag/tag/tag_meta-defs.h>


ST_BEGIN_C_DECLS

/*
// Functions -- tag_meta.c
*/
/** */
ST_EXPORT Tst_tagMeta_mt*
st_tagMeta_addNewToArr(Tst_tagMeta_mtArr *pMA,
                       const Tst_tagMeta_type mtType,
                       const Tst_uint32 bsIx,
                       const Tst_uint32 bsSIx);

ST_EXPORT Tst_err
st_tagMeta_removeTagFromArr_byUID(Tst_tagMeta_mtArr *pMA,
                                  const Tst_uint32 uid);


/*
// Functions -- tag_meta_fnc.c
*/
/** */
ST_EXPORT const char**
st_tagMeta_fnc_getTagDesc(const Tst_tagMeta_type mttp);


/*
// Functions -- tag_meta_gs.c
*/
/** Setter */
/*** */
ST_EXPORT void
st_tagMeta_gs_setArr_strrdForAllTags(Tst_tagMeta_mtArr *pMA,
                                     ST_OPTARG(Tst_streamrd *pStrrd));
ST_EXPORT void
st_tagMeta_gs_setArr_strwrForAllTags(Tst_tagMeta_mtArr *pMA,
                                     ST_OPTARG(Tst_streamwr *pStrwr));
ST_EXPORT Tst_err
st_tagMeta_gs_setArr_filenForAllTags(Tst_tagMeta_mtArr *pMA,
                                     const Tst_str *pFilen);
/*** */
ST_EXPORT void
st_tagMeta_gs_setArr_cbForSettingOptions(Tst_tagMeta_mtArr *pMA,
                                         ST_OPTARG(Tst_tagMeta_cbSetOpts_iv1
                                           cbSetOptsIV1),
                                         ST_OPTARG(void *pSetOptsDataIV1),
                                         ST_OPTARG(Tst_tagMeta_cbSetOpts_iv2
                                           cbSetOptsIV2),
                                         ST_OPTARG(void *pSetOptsDataIV2),
                                         ST_OPTARG(Tst_tagMeta_cbSetOpts_av2
                                           cbSetOptsAV2),
                                         ST_OPTARG(void *pSetOptsDataAV2),
                                         ST_OPTARG(Tst_tagMeta_cbSetOpts_vor
                                           cbSetOptsVOR),
                                         ST_OPTARG(void *pSetOptsDataVOR));
/*** */
ST_EXPORT void
st_tagMeta_gs_setTag_strrd(Tst_tagMeta_mt *pMT, ST_OPTARG(Tst_streamrd *pStrrd));

ST_EXPORT void
st_tagMeta_gs_setTag_strwr(Tst_tagMeta_mt *pMT, ST_OPTARG(Tst_streamwr *pStrwr));

ST_EXPORT Tst_err
st_tagMeta_gs_setTag_filen(Tst_tagMeta_mt *pMT, const Tst_str *pFilen);

ST_EXPORT Tst_err
st_tagMeta_gs_setTag_tbasOrg(Tst_tagMeta_mt *pMT,
                             const Tst_tagBasics *pTBasOrg);

ST_EXPORT void
st_tagMeta_gs_setTag_typeOrg(Tst_tagMeta_mt *pMT,
                             const Tst_tagMeta_type mtType);

/** Getter */
/*** */
ST_EXPORT const Tst_str*
st_tagMeta_gs_getArr_filen(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_bool
st_tagMeta_gs_getArr_isAnyTagPresent(const Tst_tagMeta_mtArr *pMA);

ST_EXPORT Tst_uint32
st_tagMeta_gs_getArr_amountTags(const Tst_tagMeta_mtArr *pMA);

ST_EXPORT Tst_uint32
st_tagMeta_gs_getArr_amountPresentTags(const Tst_tagMeta_mtArr *pMA);

/*** */
ST_EXPORT Tst_tagMeta_mt*
st_tagMeta_gs_getArr_tag_byUID(const Tst_tagMeta_mtArr *pMA,
                               const Tst_uint32 uid);
ST_EXPORT Tst_tagMeta_mt*
st_tagMeta_gs_getArr_tag_byBSIx(const Tst_tagMeta_mtArr *pMA,
                                const Tst_uint32 bsIx,
                                const Tst_uint32 bsSIx);

/*** */
ST_EXPORT const Tst_tagBasics*
st_tagMeta_gs_getTag_tbas(const Tst_tagMeta_mt *pMT);

ST_EXPORT const Tst_tagBasics*
st_tagMeta_gs_getTag_tbasOrg(const Tst_tagMeta_mt *pMT);

ST_EXPORT const Tst_str*
st_tagMeta_gs_getTag_filen(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_bool
st_tagMeta_gs_getTag_isSet(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_uint32
st_tagMeta_gs_getTag_uid(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_tagMeta_type
st_tagMeta_gs_getTag_type(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_tagMeta_type
st_tagMeta_gs_getTag_typeOrg(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_bool
st_tagMeta_gs_getTag_bsIx(const Tst_tagMeta_mt *pMT,
                          Tst_uint32 *pBsIx,
                          Tst_uint32 *pBsSIx);

/** */
ST_EXPORT void*
st_tagMeta_gs_getRealTag_void(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_id3v1_tag*
st_tagMeta_gs_getRealTag_iv1(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_id3v2_tag*
st_tagMeta_gs_getRealTag_iv2(const Tst_tagMeta_mt *pMT);

#define st_tagMeta_gs_getRealTag_mrg  st_tagMeta_gs_getRealTag_iv2

ST_EXPORT Tst_apev2_tag*
st_tagMeta_gs_getRealTag_av2(const Tst_tagMeta_mt *pMT);

ST_EXPORT Tst_vorbc_tag*
st_tagMeta_gs_getRealTag_vor(const Tst_tagMeta_mt *pMT);

/** Iterator */
ST_EXPORT Tst_tagMeta_mt*
st_tagMeta_ite_nextTag_any(const Tst_tagMeta_mtArr *pMA,
                           ST_OPTARG(const Tst_tagMeta_mt *pLast));
ST_EXPORT Tst_tagMeta_mt*
st_tagMeta_ite_nextTag_byType(const Tst_tagMeta_mtArr *pMA,
                              Tst_tagMeta_type mtType,
                              ST_OPTARG(const Tst_tagMeta_mt *pLast));
ST_EXPORT Tst_tagMeta_mt*
st_tagMeta_ite_nextTag_byTypeOrg(const Tst_tagMeta_mtArr *pMA,
                                 Tst_tagMeta_type mtTypeOrg,
                                 ST_OPTARG(const Tst_tagMeta_mt *pLast));


/*
// Functions -- tag_meta_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_tagMeta_stc_initTagArr(Tst_tagMeta_mtArr *pMA,
                          const Tst_str *pFilen);
ST_EXPORT void
st_tagMeta_stc_rsetTagArr(Tst_tagMeta_mtArr *pMA);

ST_EXPORT void
st_tagMeta_stc_freeTagArr(Tst_tagMeta_mtArr *pMA);


ST_END_C_DECLS

#endif  /* HEAD_TAG_META_H */

/******************************************************************************/
