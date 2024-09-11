/******************************************************************************/
/* dynlist-priv.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private definitions for Dynamic Lists                                      */
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

#ifndef HEAD_DYNLIST_PRIV_H
#define HEAD_DYNLIST_PRIV_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "dynlist-prot.h"


ST_BEGIN_C_DECLS

/*
// Types (private)
*/
#ifdef SRC_DYNLIST_C
	/** for Quick/Mergesort */
	typedef struct {
		Tst_dynlist_intn           *pDLI;
		Tst_dynlist_cbCompareElems cbCmpElems;
		Tst_dynlist_cbSortStatus   cbStatus;
		void                       *pCustomCmpOpts;
		Tst_uint32                 recLev;
	} Tst_dynlist__qms;
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_DYNLIST_C
	/** */
	static Tst_dynlist_elem*
	ST_DYNLIST__goto(Tst_dynlist_intn *pDLI,
	                 const Tst_uint32 dstIx);

	/** */
	static void
	ST_DYNLIST__sortInsertionSort(Tst_dynlist_intn *pDLI,
	                              Tst_dynlist_cbCompareElems cbCmpElems,
	                              Tst_dynlist_cbSortStatus cbStatus,
	                              void *pCustomCmpOpts);
	static void
	ST_DYNLIST__sortQuickSort(Tst_dynlist__qms *pQS,
	                          const Tst_uint32 eCnt,
	                          Tst_dynlist_eix *pLeft,
	                          Tst_dynlist_eix *pRight);
	static void
	ST_DYNLIST__sortMergeSort(Tst_dynlist__qms *pMS,
	                          const Tst_uint32 eCnt,
	                          Tst_dynlist_eix *pL,
	                          Tst_dynlist_eix *pR);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_DYNLIST_PRIV_H */

/******************************************************************************/
