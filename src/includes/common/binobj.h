/******************************************************************************/
/* binobj.h                     [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Binary Objects                                               */
/*                                                                            */
/* Binary Objects can contain arbitrary data,                                 */
/*   the trick is that the data internally gets either stored                 */
/*   in a buffer or a temporary file, depending of the data's size            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_BINOBJ_H
#define HEAD_BINOBJ_H

/*
// Own-Includes
*/
#include "binobj-defs.h"
#include "streamrd-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- binobj.c
*/
/** Set threshold at which data will be written to a temp. file
 **   instead of a buffer */
ST_EXPORT void
st_binobj_setThreshold(Tst_binobj *pBO, const Tst_uint32 thres);

ST_EXPORT void
st_binobj_setDefaultReallcSize(Tst_binobj *pBO, Tst_uint32 size);

/** */
ST_EXPORT Tst_uint32
st_binobj_getThreshold(const Tst_binobj *pBO);

ST_EXPORT Tst_uint32
st_binobj_getDefaultReallcSize(const Tst_binobj *pBO);

ST_EXPORT Tst_uint32
st_binobj_getDataSize(const Tst_binobj *pBO);

/** */
ST_EXPORT Tst_err
st_binobj_setData(Tst_binobj *pBOdst,
                  const Tst_buf *pBin,
                  const Tst_uint32 binSz);

ST_EXPORT Tst_err
st_binobj_setData_file(Tst_binobj *pBOdst,
                       const Tst_str *pInpFilen,
                       const Tst_bool allwLinkAsInput);

ST_EXPORT Tst_err
st_binobj_setData_streamrd(Tst_binobj *pBOdst,
                           Tst_streamrd *pStrrd,
                           const Tst_uint32 size,
                           const Tst_bool withCRCs);

/** */
ST_EXPORT Tst_err
st_binobj_appendData(Tst_binobj *pBOdst,
                     const Tst_buf *pBin,
                     const Tst_uint32 binSz);

ST_EXPORT Tst_err
st_binobj_appendData_binobj(Tst_binobj *pBOdst,
                            Tst_binobj *pBOsrc);

ST_EXPORT Tst_err
st_binobj_appendData_streamrd(Tst_binobj *pBOdst,
                              Tst_streamrd *pStrrd,
                              const Tst_uint32 size,
                              const Tst_bool withCRCs);

/** */
ST_EXPORT Tst_err
st_binobj_replaceData(Tst_binobj *pBOdst,
                      Tst_foffs startPos,
                      const Tst_buf *pBin,
                      const Tst_uint32 binSz);

/** */
ST_EXPORT Tst_err
st_binobj_copy(Tst_binobj *pBOsrc, Tst_binobj *pBOdst);

ST_EXPORT Tst_err
st_binobj_copyIntoBuffer(Tst_binobj *pBOsrc,
                         Tst_buf **ppBuffer,
                         Tst_uint32 *pBufSz);

ST_EXPORT Tst_err
st_binobj_copyIntoFile(Tst_binobj *pBOsrc, const Tst_str *pOutpFilen);

/** */
ST_EXPORT Tst_err
st_binobj_attachStreamrd(Tst_binobj *pBO,
                         Tst_streamrd *pSObjR,
                         Tst_foffs startPos);

ST_EXPORT void
st_binobj_detachStreamrd(Tst_binobj *pBO);

/** */
ST_EXPORT Tst_bool
st_binobj_areEqual(Tst_binobj *pBO1, Tst_binobj *pBO2);


/*
// Functions -- binobj_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_binobj_stc_initBO(Tst_binobj *pBO);

ST_EXPORT void
st_binobj_stc_rsetBO(Tst_binobj *pBO);

ST_EXPORT void
st_binobj_stc_freeBO(Tst_binobj *pBO);

ST_EXPORT void
st_binobj_stc_rsetData(Tst_binobj *pBO);


ST_END_C_DECLS

#endif  /* HEAD_BINOBJ_H */

/******************************************************************************/
