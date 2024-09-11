/******************************************************************************/
/* tag_id3v1.h                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for ID3v1 (Extended) Tag handling                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V1_H
#define HEAD_TAG_ID3V1_H

/*
// Own-Includes
*/
#include "src/includes/common/string_mte-defs.h"
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/streamwr-defs.h"
#include "src/includes/tag/tag_basics-defs.h"
#include "src/includes/tag/tag_id3v1-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- tag_id3v1.c
*/
/** */
ST_EXPORT Tst_err
st_id3v1_readTagFromStream(Tst_id3v1_tag *pTag);

/** */
ST_EXPORT Tst_err
st_id3v1_mergeTags(Tst_id3v1_tag *pTagDst, Tst_id3v1_tag *pTagSrc);

/** */
ST_EXPORT Tst_err
st_id3v1_tidyUpTag(Tst_id3v1_tag *pTag);

/** */
ST_EXPORT Tst_err
st_id3v1_writeTagToStream(Tst_id3v1_tag *pTag);


/*
// Functions -- tag_id3v1_fldadd.c
*/
/** */
ST_EXPORT Tst_err
st_id3v1_fldAdd(Tst_id3v1_tag *pTag,
                const Tst_id3v1_fldData *pFldAdd);

/** */
ST_EXPORT Tst_err
st_id3v1_fldSet_dataStr(Tst_id3v1_tag *pTag,
                        const Tst_id3v1_frID frID,
                        const Tst_mtes_string *pText);
ST_EXPORT Tst_err
st_id3v1_fldSet_dataInt(Tst_id3v1_tag *pTag,
                        const Tst_id3v1_frID frID,
                        const Tst_int32 ival);


/*
// Functions -- tag_id3v1_fnc.c
*/
/** */
ST_EXPORT Tst_bool
st_id3v1_fnc_hasIDstrInvalidChars(ST_OPTARG(const Tst_str *pFIDstr),
                                  ST_OPTARG(Tst_str *pInvalidCharFound));

ST_EXPORT Tst_id3v1_frID
st_id3v1_fnc_getID_byIDstr(const Tst_str *pFIDstr);

ST_EXPORT Tst_err
st_id3v1_fnc_getIDstr_byEnum(const Tst_id3v1_frID frID,
                             Tst_str **ppFIDstr);


/*
// Functions -- tag_id3v1_gs.c
*/
/** Setter */
/*** */
ST_EXPORT void
st_id3v1_gs_setTag_strrd(Tst_id3v1_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd));

ST_EXPORT void
st_id3v1_gs_setTag_strwr(Tst_id3v1_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr));

ST_EXPORT Tst_err
st_id3v1_gs_setTag_filen(Tst_id3v1_tag *pTag, const Tst_str *pFilen);

/*** */
ST_EXPORT Tst_err
st_id3v1_gs_setField_artist(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_id3v1_gs_setField_album(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_id3v1_gs_setField_title(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_id3v1_gs_setField_comment(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr);

ST_EXPORT Tst_err
st_id3v1_gs_setField_tracknr(Tst_id3v1_tag *pTag, Tst_int32 tnr);

ST_EXPORT Tst_err
st_id3v1_gs_setField_genre(Tst_id3v1_tag *pTag, Tst_int32 gen);

ST_EXPORT Tst_err
st_id3v1_gs_setField_year(Tst_id3v1_tag *pTag, Tst_int32 year);

/** Getter */
/*** */
ST_EXPORT const Tst_tagBasics*
st_id3v1_gs_getTag_tbas(const Tst_id3v1_tag *pTag);

ST_EXPORT const Tst_str*
st_id3v1_gs_getTag_filen(const Tst_id3v1_tag *pTag);

ST_EXPORT Tst_bool
st_id3v1_gs_getTag_hasErrors(const Tst_id3v1_tag *pTag);

/*** */
ST_EXPORT Tst_err
st_id3v1_gs_getField_artist(const Tst_id3v1_tag *pTag,
                            Tst_mtes_string *pArtist);

ST_EXPORT Tst_err
st_id3v1_gs_getField_album(const Tst_id3v1_tag *pTag,
                           Tst_mtes_string *pAlbum);

ST_EXPORT Tst_err
st_id3v1_gs_getField_title(const Tst_id3v1_tag *pTag,
                           Tst_mtes_string *pTitle);

ST_EXPORT Tst_err
st_id3v1_gs_getField_comment(const Tst_id3v1_tag *pTag,
                             Tst_mtes_string *pComm);

ST_EXPORT Tst_int32
st_id3v1_gs_getField_tracknr(const Tst_id3v1_tag *pTag);

ST_EXPORT Tst_int32
st_id3v1_gs_getField_year(const Tst_id3v1_tag *pTag);

ST_EXPORT Tst_int32
st_id3v1_gs_getField_genre(const Tst_id3v1_tag *pTag);

/*** */
ST_EXPORT Tst_id3v1_frTp
st_id3v1_gs_getFieldProp_typ(const Tst_id3v1_fldProp *pFldPr);

ST_EXPORT Tst_id3v1_frID
st_id3v1_gs_getFieldProp_id(const Tst_id3v1_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_id3v1_gs_getFieldProp_idStr(const Tst_id3v1_fldProp *pFldPr);

ST_EXPORT const Tst_str*
st_id3v1_gs_getFieldProp_caption(const Tst_id3v1_fldProp *pFldPr);

/** Iterators */
/*** for tag field properties list */
ST_EXPORT Tst_id3v1_fldProp*
st_id3v1_ite_nextTagFldProp(ST_OPTARG(Tst_id3v1_fldProp *pFldPrLast),
                            Tst_id3v1_fldProp *pFldPrNext);


/*
// Functions -- tag_id3v1_opts.c
*/
/** Options: Setter */
/*** */
ST_EXPORT void
st_id3v1_opts_setBasicOpts(Tst_id3v1_tag *pTag,
                           ST_OPTARG(const Tst_basOpts *pBOpts));
/*** */
ST_EXPORT void
st_id3v1_opts_setReadOnlyBasics(Tst_id3v1_tag *pTag,
                                const Tst_bool rdOnlyBasics);


/*
// Functions -- tag_id3v1_stcs.c
*/
/** Structs */
/*** */
ST_EXPORT Tst_err
st_id3v1_stc_initTag(Tst_id3v1_tag *pTag, const Tst_str *pFilen);

ST_EXPORT void
st_id3v1_stc_rsetTag(Tst_id3v1_tag *pTag);

ST_EXPORT void
st_id3v1_stc_freeTag(Tst_id3v1_tag *pTag);

/*** */
ST_EXPORT Tst_err
st_id3v1_stc_initFDat(Tst_id3v1_fldData *pDat);

ST_EXPORT void
st_id3v1_stc_rsetFDat(Tst_id3v1_fldData *pDat);

ST_EXPORT void
st_id3v1_stc_freeFDat(Tst_id3v1_fldData *pDat);

/*** */
ST_EXPORT void
st_id3v1_stc_rsetFPr(Tst_id3v1_fldProp *pFldPr);


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V1_H */

/******************************************************************************/
