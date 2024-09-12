/******************************************************************************/
/* tag_comfnc.h                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Common functions for Tags                                                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 04.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_COMFNC_H
#define HEAD_TAG_COMFNC_H

/*
// Own-Includes
*/
#include "../common/string_mte-defs.h"
#include "tag_comfnc-defs.h"
#include "tag_id3v2-defs.h"
#include "tag_vorbc-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- tag_comfnc.c
*/
/** */
ST_EXPORT Tst_err
st_tagCFnc_iplsMerge(Tst_mtes_strArr *pSArr);

ST_EXPORT Tst_err
st_tagCFnc_iplsSplit(Tst_mtes_strArr *pSArr);

ST_EXPORT Tst_vorbc_picTp
st_tagCFnc_convId3v2ToVorbc_picTp(Tst_id3v2_picTp srcPicTp);

ST_EXPORT Tst_id3v2_picTp
st_tagCFnc_convVorbcToId3v2_picTp(Tst_vorbc_picTp srcPicTp);

/** Structs */
ST_EXPORT void
st_tagCFnc_stc_rsetTSta(Tst_tagCFnc_tstamp *pTSt);


/*
// Functions -- tag_comfnc_chk.c
*/
/** */
ST_EXPORT Tst_err
st_tagCFnc_chk_getGenreCap(const Tst_mtes_string *pGin,
                           Tst_mtes_string *pCapOut,
                           const Tst_mtes_textEnc textEncOut);

ST_EXPORT Tst_err
st_tagCFnc_chk_getGenreNum(const Tst_mtes_string *pGin, Tst_int32 *pGNrOut);

ST_EXPORT Tst_err
st_tagCFnc_chk_getGenreNum_pchar(const Tst_str *pGin, Tst_int32 *pGNrOut);

ST_EXPORT Tst_err
st_tagCFnc_chk_getTStamp(const Tst_bool isTimestampStr,
                         const Tst_bool isTimeStr,
                         const Tst_bool isDateStr,
                         const Tst_mtes_string *pStr,
                         Tst_tagCFnc_tstamp *pTStamp,
                         Tst_bool *pIsOK);

ST_EXPORT Tst_err
st_tagCFnc_chk_getPosStringNums(const Tst_mtes_string *pStr,
                                Tst_uint32 *pNum,
                                Tst_uint32 *pNumTot);


ST_END_C_DECLS

#endif  /* HEAD_TAG_COMFNC_H */

/******************************************************************************/
