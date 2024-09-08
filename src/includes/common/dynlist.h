/******************************************************************************/
/* dynlist.h                    [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for Dynamic Lists                                                */
/*                                                                            */
/* The advantage of this Dynamic List is that pointers                        */
/*   to its elements are stable even if some elements have been               */
/*   added, removed, moved, ...                                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 09.08.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_DYNLIST_H
#define HEAD_DYNLIST_H

/*
// Own-Includes
*/
#include "src/includes/common/dynlist-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- dynlist.c
*/
/** */
ST_EXPORT Tst_uint32
st_dynlist_getElementCount(const Tst_dynlist *pDL);

ST_EXPORT Tst_bool
st_dynlist_isEmpty(const Tst_dynlist *pDL);

/** */
/*** */
ST_EXPORT Tst_bool
st_dynlist_ite_goto(Tst_dynlist *pDL, const Tst_uint32 index);

ST_EXPORT Tst_bool
st_dynlist_ite_gotoFirst(Tst_dynlist *pDL);

ST_EXPORT Tst_bool
st_dynlist_ite_gotoLast(Tst_dynlist *pDL);

ST_EXPORT Tst_bool
st_dynlist_ite_gotoNext(Tst_dynlist *pDL);

ST_EXPORT Tst_bool
st_dynlist_ite_gotoPrev(Tst_dynlist *pDL);
/*** */
ST_EXPORT Tst_uint32
st_dynlist_ite_getCurrentIndex(const Tst_dynlist *pDL);

ST_EXPORT void*
st_dynlist_ite_getCurrent(const Tst_dynlist *pDL);

/** */
ST_EXPORT Tst_err
st_dynlist_addElement(Tst_dynlist *pDL, void *pData);

ST_EXPORT Tst_err
st_dynlist_insElement(Tst_dynlist *pDL,
                      void *pData,
                      const Tst_uint32 insertAsIndex);

ST_EXPORT Tst_bool
st_dynlist_remElement(Tst_dynlist *pDL, const Tst_uint32 index);

ST_EXPORT void*
st_dynlist_getElement(Tst_dynlist *pDL, const Tst_uint32 index);

ST_EXPORT Tst_bool
st_dynlist_moveElement(Tst_dynlist *pDL,
                       const Tst_uint32 fromIndex,
                       const Tst_uint32 toIndex);

/** */
ST_EXPORT Tst_err
st_dynlist_clone(Tst_dynlist *pSrc, Tst_dynlist *pDst,
                 Tst_dynlist_cbCloneInitElem cbCloneInitElem,
                 Tst_dynlist_cbCloneCopyElem cbCloneCopyElem);

/** */
ST_EXPORT Tst_bool
st_dynlist_sortElements(Tst_dynlist *pDL,
                        Tst_dynlist_sortAlgo sortAlgo,
                        Tst_dynlist_cbCompareElems cbCmpElems,
                        ST_OPTARG(Tst_dynlist_cbSortStatus cbStatus),
                        ST_OPTARG(void *pCustomCmpOpts));

/** */
ST_EXPORT Tst_bool
st_dynlist_check(const Tst_dynlist *pDL);


/*
// Functions -- dynlist_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_dynlist_stc_initDL(Tst_dynlist *pDL,
                      const Tst_uint32 sizeOfElem,
                      Tst_dynlist_cbResetElem cbResetElem,
                      Tst_dynlist_cbFreeElem cbFreeElem);

ST_EXPORT void
st_dynlist_stc_rsetDL(Tst_dynlist *pDL);

ST_EXPORT void
st_dynlist_stc_freeDL(Tst_dynlist *pDL);


ST_END_C_DECLS

#endif  /* HEAD_DYNLIST_H */

/******************************************************************************/
