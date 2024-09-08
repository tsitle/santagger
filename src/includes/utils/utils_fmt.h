/******************************************************************************/
/* utils_fmt.h                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for determining file formats etc.                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_UTILS_FMT_H
#define HEAD_UTILS_FMT_H

/*
// Own-Includes
*/
#include "src/includes/common/binobj-defs.h"
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/utils/utils_fmt-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions
*/
/** Picture Format */
/*** */
ST_EXPORT Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byFExt(const Tst_str *pFExt);

ST_EXPORT Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byFn(const Tst_str *pFilen);

ST_EXPORT Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byMime(const Tst_str *pMime);

ST_EXPORT Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byBuffer(const Tst_buf *pBuf,
                                    const Tst_uint32 sz);

ST_EXPORT Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byBObj(Tst_binobj *pBinDat);

/*** */
ST_EXPORT const Tst_str*
st_utilsFmt_getPicFmt_fext_byEnum(const Tst_utilsFmt_picFmt pfmE);

/*** */
ST_EXPORT const Tst_str*
st_utilsFmt_getPicFmt_mime_byEnum(const Tst_utilsFmt_picFmt pfmE);

/** MIME */
/*** */
ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byFExt(const Tst_str *pFExt);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byFn(const Tst_str *pFilen);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byMime(const Tst_str *pMime);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream(Tst_streamrd *pSObj);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream_pic(Tst_streamrd *pSObj);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream_audvid(Tst_streamrd *pSObj);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream_tag(Tst_streamrd *pSObj);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer(const Tst_buf *pBuf,
                                  const Tst_uint32 sz);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer_pic(const Tst_buf *pBuf,
                                      const Tst_uint32 sz);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer_audvid(const Tst_buf *pBuf,
                                         const Tst_uint32 sz);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer_tag(const Tst_buf *pBuf,
                                      const Tst_uint32 sz);

ST_EXPORT Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBObj(Tst_binobj *pBinDat);

/*** */
ST_EXPORT const Tst_str*
st_utilsFmt_getMime_defMime_byFExt(const Tst_str *pFExt);

ST_EXPORT const Tst_str*
st_utilsFmt_getMime_defMime_byFn(const Tst_str *pFilen);

ST_EXPORT const Tst_str*
st_utilsFmt_getMime_defMime_byEnum(const Tst_utilsFmt_mimeTp mtp);

/*** */
ST_EXPORT const Tst_str*
st_utilsFmt_getMime_defFExt_byFExt(const Tst_str *pFExt);

ST_EXPORT const Tst_str*
st_utilsFmt_getMime_defFExt_byFn(const Tst_str *pFilen);

ST_EXPORT const Tst_str*
st_utilsFmt_getMime_defFExt_byMime(const Tst_str *pMime);

ST_EXPORT const Tst_str*
st_utilsFmt_getMime_defFExt_byEnum(const Tst_utilsFmt_mimeTp mtp);


ST_END_C_DECLS

#endif  /* HEAD_UTILS_FMT_H */

/******************************************************************************/
