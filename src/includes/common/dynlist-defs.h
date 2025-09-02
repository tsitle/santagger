/******************************************************************************/
/* dynlist-defs.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for Dynamic Lists                                              */
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

#ifndef LIBSANTAGGER_DYNLIST_DEFS_H
#define LIBSANTAGGER_DYNLIST_DEFS_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** max amount of elements in a dynamic list */
ST_EXPORT extern const Tst_uint32 ST_DYNLIST_MAXELEMS;

/** sorting algos */
typedef enum {
	ST_DYNLIST_SA_DEF       = 0,  /* Default: let sorting algo decide */
	ST_DYNLIST_SA_QUICKSORT = 1,  /* Quicksort */
	ST_DYNLIST_SA_MERGESORT = 2,  /* Mergesort */
	ST_DYNLIST_SA_INSSORT   = 3,  /* Insertionsort */
	ST_DYNLIST_SA_NONE      = 4
} Tst_dynlist_sortAlgo;


/*
// Types
*/
/** callbacks */
typedef void      (*Tst_dynlist_cbResetElem)(void *pElem);
typedef void      (*Tst_dynlist_cbFreeElem)(void *pElem);
typedef Tst_int32 (*Tst_dynlist_cbCompareElems)(const void *pElem1,
                                                const void *pElem2,
                                                ST_OPTARG(void *pCustomCmpOpts));
typedef void    (*Tst_dynlist_cbSortStatus)(const Tst_uint32 valCur,
                                            const Tst_uint32 valMax);
typedef Tst_err (*Tst_dynlist_cbCloneInitElem)(void *pElem);
typedef Tst_err (*Tst_dynlist_cbCloneCopyElem)(const void *pElemSrc,
                                               void *pElemDst);

/** */
typedef struct {
	void *pObInternal;  /* pointer to Tst_dynlist_intn */
} Tst_dynlist;


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_DYNLIST_DEFS_H */

/******************************************************************************/
