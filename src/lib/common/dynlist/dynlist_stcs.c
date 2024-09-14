/******************************************************************************/
/* dynlist_stcs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Dynamic List structures                                      */
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

/*
// Own-Includes
*/
/** */
#include "src/includes/common/dynlist.h"
/** */
#define SRC_DYNLIST_STCS_C
#include "dynlist_stcs-prot.h"
#undef SRC_DYNLIST_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define ST_DYNLIST__RSET_EIX(macEIX)  { (macEIX).pE = NULL; (macEIX).ix = 0; }

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_dynlist */
Tst_err
st_dynlist_stc_initDL(Tst_dynlist *pDL, const Tst_uint32 sizeOfElem,
		Tst_dynlist_cbResetElem cbResetElem, Tst_dynlist_cbFreeElem cbFreeElem)
{
	Tst_dynlist_intn *pDLI = NULL;

	ST_ASSERTN_IARG(pDL == NULL || sizeOfElem == 0 ||
			cbResetElem == NULL || cbFreeElem == NULL)

	ST_CALLOC(pDLI, Tst_dynlist_intn*, 1, sizeof(Tst_dynlist_intn))
	if (pDLI == NULL) {
		pDL->pObInternal = NULL;
		return ST_ERR_OMEM;
	}
	pDL->pObInternal = pDLI;

	/* */
	pDLI->pStart     = NULL;
	pDLI->sz         = 0;
	pDLI->cnt        = 0;
	pDLI->soElem     = sizeOfElem;
	pDLI->cbRsetElem = cbResetElem;
	pDLI->cbFreeElem = cbFreeElem;

	st_dynlist_stc_rsetDL(pDL);

	return ST_ERR_SUCC;
}

void
st_dynlist_stc_rsetDL(Tst_dynlist *pDL)
{
	Tst_dynlist_elem *pE,
	                 *pN;
	Tst_dynlist_intn *pDLI = NULL;

	ST_ASSERTN_VOID(pDL == NULL || pDL->pObInternal == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	if (pDLI->pStart != NULL) {
		pE = pDLI->pStart;
		while (pE != NULL) {
			pN = pE->pNext;
			st_dynlist_stc_freeElem(pE, pDLI->cbFreeElem);
			ST_DELPOINT(pE)
			pE = pN;
		}
		pDLI->pStart = NULL;
		pDLI->sz     = 0;
		pDLI->cnt    = 0;
	}
	pDLI->pEnd = NULL;
	ST_DYNLIST__RSET_EIX(pDLI->lastAdded)
	ST_DYNLIST__RSET_EIX(pDLI->iter)
	ST_DYNLIST__RSET_EIX(pDLI->cacheElem)
}

void
st_dynlist_stc_freeDL(Tst_dynlist *pDL)
{
	ST_ASSERTN_VOID(pDL == NULL)

	if (pDL->pObInternal != NULL) {
		st_dynlist_stc_rsetDL(pDL);
		/* */
		ST_DELPOINT(pDL->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_dynlist_elem */
ST_INLINE_S void
st_dynlist_stc_freeElem(Tst_dynlist_elem *pElem,
		Tst_dynlist_cbFreeElem cbFreeElement)
{
	ST_ASSERTN_VOID(pElem == NULL)

	if (pElem->pData != NULL) {
		ST_ASSERTN_VOID(cbFreeElement == NULL)

		cbFreeElement(pElem->pData);
		ST_DELPOINT(pElem->pData)
	}
	/*DYNLIST__RSET_ELEM_NOCB(pElem)*/
}

/******************************************************************************/
