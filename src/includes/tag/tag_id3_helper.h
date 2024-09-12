/******************************************************************************/
/* tag_id3_helper.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Helper functions for ID3 Tags                                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3_HELPER_H
#define HEAD_TAG_ID3_HELPER_H

/*
// Own-Includes
*/
#include "tag_id3_helper-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions
*/
/** ID3v1 & ID3v2 */
ST_EXPORT Tst_bool
st_id3_genreExists_int(const Tst_byte gennr, ST_OPTARG(Tst_int32 *pIx));

ST_EXPORT Tst_bool
st_id3_genreExists_str(const Tst_str *pCap, ST_OPTARG(Tst_int32 *pIx));

ST_EXPORT const char*
st_id3_getGenreCap(const Tst_int32 ix);

ST_EXPORT Tst_bool
st_id3_isStandardGenre(const Tst_int32 ix);

/** ID3v2 */
ST_EXPORT Tst_bool
st_id3_isSpecGenre(const Tst_str *pIDandorCap, ST_OPTARG(Tst_int32 *pIx));


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3_HELPER_H */

/******************************************************************************/
