/******************************************************************************/
/* dynlist.c                    [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Dynamic Lists                                                */
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
#define SRC_DYNLIST_C
#include "dynlist-priv.h"
/*** */
#include "dynlist_stcs-prot.h"
#undef SRC_DYNLIST_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define ST_DYNLIST_ADDSZ_DEF  512

const Tst_uint32 ST_DYNLIST_MAXELEMS = 0xffffff00 - ST_DYNLIST_ADDSZ_DEF;

#define ST_DYNLIST__RSET_ELEM_NOCB(pMacElem)  { \
			(pMacElem)->pNext = NULL; (pMacElem)->pPrev = NULL; }
#define ST_DYNLIST__RSET_ELEM_WCB(pMacElem, macCBrsetElem)  { \
			if ((pMacElem)->pData != NULL) \
				macCBrsetElem((pMacElem)->pData); \
			ST_DYNLIST__RSET_ELEM_NOCB(pMacElem) \
			}
#define ST_DYNLIST__INIT_ELEM(pMacElem)  { \
			(pMacElem)->pData = NULL; ST_DYNLIST__RSET_ELEM_NOCB(pMacElem); }

#define ST_DYNLIST__CMP(macIx1, macIx2)  \
			(macIx1 >= macIx2 ? (macIx1 > macIx2 ? 1 : 0) : -1)

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_uint32
st_dynlist_getElementCount(const Tst_dynlist *pDL)
{
	ST_ASSERTN_NUM(0, pDL == NULL || pDL->pObInternal == NULL)

	return ((Tst_dynlist_intn*)pDL->pObInternal)->cnt;
}

Tst_bool
st_dynlist_isEmpty(const Tst_dynlist *pDL)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pDL == NULL || pDL->pObInternal == NULL)

	return (((Tst_dynlist_intn*)pDL->pObInternal)->cnt == 0);
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_dynlist_ite_goto(Tst_dynlist *pDL, const Tst_uint32 index)
{
	Tst_dynlist_elem *pE;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	pE = ST_DYNLIST__goto((Tst_dynlist_intn*)pDL->pObInternal, index);
	if (pE != NULL && pE->pData != NULL) {
		((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE = pE;
		((Tst_dynlist_intn*)pDL->pObInternal)->iter.ix = index;
		return ST_B_TRUE;
	}
	return ST_B_FALSE;
}

Tst_bool
st_dynlist_ite_gotoFirst(Tst_dynlist *pDL)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE =
			((Tst_dynlist_intn*)pDL->pObInternal)->pStart;
	if (((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE != NULL &&
			((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE->pData != NULL) {
		((Tst_dynlist_intn*)pDL->pObInternal)->iter.ix = 1;
		return ST_B_TRUE;
	}
	((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE = NULL;
	((Tst_dynlist_intn*)pDL->pObInternal)->iter.ix = 0;
	return ST_B_FALSE;
}

Tst_bool
st_dynlist_ite_gotoLast(Tst_dynlist *pDL)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE =
			((Tst_dynlist_intn*)pDL->pObInternal)->lastAdded.pE;
	if (((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE != NULL &&
			((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE->pData != NULL) {
		((Tst_dynlist_intn*)pDL->pObInternal)->iter.ix =
				((Tst_dynlist_intn*)pDL->pObInternal)->lastAdded.ix;
		return ST_B_TRUE;
	}
	((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE = NULL;
	((Tst_dynlist_intn*)pDL->pObInternal)->iter.ix = 0;
	return ST_B_FALSE;
}

Tst_bool
st_dynlist_ite_gotoNext(Tst_dynlist *pDL)
{
	Tst_dynlist_intn *pDLI;
	Tst_dynlist_elem *pE;
	Tst_uint32       ixN;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	if (pDLI->iter.pE != NULL) {
		if (pDLI->iter.pE == pDLI->lastAdded.pE) {
			pE  = NULL;
			ixN = 0;
		} else {
			pE  = pDLI->iter.pE->pNext;
			ixN = pDLI->iter.ix + 1;
		}
	} else {
		pE  = pDLI->pStart;
		ixN = 1;
	}

	if (pE != NULL && pE->pData != NULL) {
		/* next element found */
		pDLI->cacheElem.pE = pE;
		pDLI->cacheElem.ix = ixN;
		pDLI->iter.pE      = pE;
		pDLI->iter.ix      = ixN;
		return ST_B_TRUE;
	}
	/* no next elem found */
	pDLI->iter.pE = pDLI->lastAdded.pE;
	pDLI->iter.ix = pDLI->lastAdded.ix;
	return ST_B_FALSE;
}

Tst_bool
st_dynlist_ite_gotoPrev(Tst_dynlist *pDL)
{
	Tst_dynlist_intn *pDLI;
	Tst_dynlist_elem *pE;
	Tst_uint32       ixN;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	if (pDLI->iter.pE != NULL) {
		if (pDLI->iter.pE == pDLI->pStart) {
			pE  = NULL;
			ixN = 0;
		} else {
			pE  = pDLI->iter.pE->pPrev;
			ixN = pDLI->iter.ix - 1;
		}
	} else {
		pE  = pDLI->lastAdded.pE;
		ixN = pDLI->lastAdded.ix;
	}

	if (pE != NULL && pE->pData != NULL) {
		/* previous element found */
		pDLI->cacheElem.pE = pE;
		pDLI->cacheElem.ix = ixN;
		pDLI->iter.pE      = pE;
		pDLI->iter.ix      = ixN;
		return ST_B_TRUE;
	}
	/* no previous elem found */
	pDLI->iter.pE = pDLI->pStart;
	pDLI->iter.ix = 1;
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_dynlist_ite_getCurrentIndex(const Tst_dynlist *pDL)
{
	ST_ASSERTN_NUM(0, pDL == NULL || pDL->pObInternal == NULL)

	if (((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE != NULL)
		return ((Tst_dynlist_intn*)pDL->pObInternal)->iter.ix;
	return 0;
}

/*----------------------------------------------------------------------------*/

void*
st_dynlist_ite_getCurrent(const Tst_dynlist *pDL)
{
	ST_ASSERTN_NULL(pDL == NULL || pDL->pObInternal == NULL)

	return (((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE != NULL ?
			((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE->pData : NULL);
}

/*----------------------------------------------------------------------------*/

#define LOC_ALLOC_ELEMS_(pMacDLI, pMacE, macTmp, macX)  { \
		if ((pMacDLI)->cnt == (pMacDLI)->sz) { \
			/* alloc some more list elements */ \
			if ((pMacDLI)->pStart == NULL) { \
				ST_CALLOC(pMacE, Tst_dynlist_elem*, 1, sizeof(Tst_dynlist_elem)) \
				if ((pMacE) == NULL) \
					return ST_ERR_OMEM; \
				ST_DYNLIST__INIT_ELEM((pMacE)) \
				\
				(pMacDLI)->pStart = (pMacE); \
				(pMacDLI)->pEnd   = (pMacE); \
				(pMacDLI)->sz     = 1; \
			} else { \
				if ((pMacDLI)->sz < 10) \
					macTmp = 10 - (pMacDLI)->sz; \
				else if ((pMacDLI)->sz < 1000) \
					macTmp = (pMacDLI)->sz / 2; \
				else \
					macTmp = ST_DYNLIST_ADDSZ_DEF; \
				(pMacDLI)->sz += macTmp; \
				\
				for (macX = 0; macX < macTmp; macX++) { \
					ST_CALLOC(pMacE, Tst_dynlist_elem*, 1, sizeof(Tst_dynlist_elem)) \
					if ((pMacE) == NULL) \
						return ST_ERR_OMEM; \
					ST_DYNLIST__INIT_ELEM(pMacE) \
					/* */ \
					(pMacDLI)->pEnd->pNext = (pMacE); \
					(pMacE)->pPrev         = (pMacDLI)->pEnd; \
					(pMacDLI)->pEnd        = (pMacE); \
				} \
			} \
		} }

Tst_err
st_dynlist_addElement(Tst_dynlist *pDL, void *pData)
{
	Tst_uint32       tmp,
	                 x;
	Tst_dynlist_intn *pDLI;
	Tst_dynlist_elem *pE = NULL;

	ST_ASSERTN_IARG(pDL == NULL || pDL->pObInternal == NULL || pData == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	if (pDLI->cnt >= ST_DYNLIST_MAXELEMS)
		return ST_ERR_DLLR;  /* limit reached */

	LOC_ALLOC_ELEMS_(pDLI, pE, tmp, x)

	if (pDLI->lastAdded.pE != NULL)
		pE = pDLI->lastAdded.pE->pNext;
	else
		pE = pDLI->pStart;
	pE->pData          = pData;
	pDLI->lastAdded.pE = pE;
	++pDLI->lastAdded.ix;
	++pDLI->cnt;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_dynlist_insElement(Tst_dynlist *pDL, void *pData,
		const Tst_uint32 insertAsIndex)
{
	Tst_uint32       tmp,
	                 x;
	Tst_dynlist_intn *pDLI;
	Tst_dynlist_elem *pE = NULL,
	                 *pN;

	ST_ASSERTN_IARG(pDL == NULL || pDL->pObInternal == NULL || pData == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	if (insertAsIndex > pDLI->cnt || pDLI->cnt == 0)
		return st_dynlist_addElement(pDL, pData);

	if (pDLI->cnt >= ST_DYNLIST_MAXELEMS)
		return ST_ERR_DLLR;  /* limit reached */

	LOC_ALLOC_ELEMS_(pDLI, pE, tmp, x)

	pE = ST_DYNLIST__goto(pDLI, insertAsIndex);
	if (pE == NULL)
		return ST_ERR_FAIL;
	/* */
	/** */
	pN                = pDLI->pEnd;
	pDLI->pEnd        = pDLI->pEnd->pPrev;
	pDLI->pEnd->pNext = NULL;
	if (pE == pDLI->pStart)
		pDLI->pStart = pN;
	/** */
	pN->pPrev = pE->pPrev;
	pN->pNext = pE;
	pN->pData = pData;
	/** */
	if (pE->pPrev != NULL)
		pE->pPrev->pNext = pN;
	pE->pPrev = pN;

	/* update lastAdded, cacheElem and iter */
	++pDLI->lastAdded.ix;
	if (pDLI->cacheElem.ix == insertAsIndex)
		pDLI->cacheElem.pE = pN;
	else if (pDLI->cacheElem.ix > insertAsIndex)
		++pDLI->cacheElem.ix;
	if (pDLI->iter.ix == insertAsIndex)
		pDLI->iter.pE = pN;
	else if (pDLI->iter.ix > insertAsIndex)
		++pDLI->iter.ix;

	++pDLI->cnt;
	return ST_ERR_SUCC;
}

#undef LOC_ALLOC_ELEMS_

/*----------------------------------------------------------------------------*/

Tst_bool
st_dynlist_remElement(Tst_dynlist *pDL, const Tst_uint32 index)
{
	Tst_dynlist_intn *pDLI;
	Tst_dynlist_elem *pE;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	pE = ST_DYNLIST__goto(pDLI, index);
	if (pE == NULL)
		return ST_B_FALSE;
	/* */
	if (pE->pPrev != NULL)
		pE->pPrev->pNext = pE->pNext;
	if (pE->pNext != NULL)
		pE->pNext->pPrev = pE->pPrev;
	/* update pStart, pEnd, lastAdded, cacheElem and iter */
	if (pE == pDLI->lastAdded.pE) {
		pDLI->lastAdded.pE = pDLI->lastAdded.pE->pPrev;
		--pDLI->lastAdded.ix;
	}
	if (pE == pDLI->pStart)
		pDLI->pStart = pE->pNext;
	if (pE == pDLI->pEnd) {
		pDLI->pEnd = pDLI->pEnd->pPrev;
		if (pDLI->pEnd != NULL)
			pDLI->pEnd->pNext = NULL;
	}
	if (pE == pDLI->iter.pE) {
		pDLI->iter.pE = pDLI->iter.pE->pPrev;
		--pDLI->iter.ix;
	}
	if (pE == pDLI->cacheElem.pE) {
		if (pDLI->cacheElem.pE->pPrev != NULL) {
			pDLI->cacheElem.pE = pDLI->cacheElem.pE->pPrev;
			--pDLI->cacheElem.ix;
		} else if (pDLI->cacheElem.pE->pNext != NULL) {
			pDLI->cacheElem.pE = pDLI->cacheElem.pE->pNext;
			++pDLI->cacheElem.ix;
		} else {
			pDLI->cacheElem.pE = NULL;
			pDLI->cacheElem.ix = 0;
		}
	}

	/* */
	st_dynlist_stc_freeElem(pE, pDLI->cbFreeElem);
	ST_DELPOINT(pE)
	--pDLI->cnt;
	--pDLI->sz;
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

void*
st_dynlist_getElement(Tst_dynlist *pDL, const Tst_uint32 index)
{
	ST_ASSERTN_NULL(pDL == NULL || pDL->pObInternal == NULL)

	if (! st_dynlist_ite_goto(pDL, index))
		return NULL;
	return (((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE != NULL ?
			((Tst_dynlist_intn*)pDL->pObInternal)->iter.pE->pData : NULL);
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_dynlist_moveElement(Tst_dynlist *pDL, const Tst_uint32 fromIndex,
		const Tst_uint32 toIndex)
{
	Tst_dynlist_elem *pEfrom,
	                 *pEto;
	Tst_dynlist_intn *pDLI;
	void             *pTmpD;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	if (toIndex == 0)
		return ST_B_FALSE;
	if (pDLI->cnt == 1 || fromIndex == toIndex)
		return ST_B_TRUE;
	if (! st_dynlist_ite_goto(pDL, fromIndex))
		return ST_B_FALSE;
	pEfrom = pDLI->iter.pE;

	if (toIndex >= pDLI->cnt)
		pEto = pDLI->lastAdded.pE;
	else {
		if (! st_dynlist_ite_goto(pDL, toIndex))
			return ST_B_FALSE;
		pEto = pDLI->iter.pE;
	}

	if (fromIndex + 1 == toIndex || toIndex + 1 == fromIndex ||
			(fromIndex == pDLI->cnt && toIndex > pDLI->cnt)) {
		/* simply swap data of knots */
		pTmpD         = pEfrom->pData;
		pEfrom->pData = pEto->pData;
		pEto->pData   = pTmpD;
	} else {
		/* update pStart, pEnd, lastAdded */
		if (fromIndex == 1)
			pDLI->pStart = pEfrom->pNext;
		else {
			if (toIndex == 1)
				pDLI->pStart = pEfrom;
			if (fromIndex == pDLI->cnt) {
				if (pDLI->cnt == pDLI->sz)
					pDLI->pEnd = pEfrom->pPrev;
				pDLI->lastAdded.pE = pEfrom->pPrev;
			}
		}
		if (toIndex >= pDLI->cnt) {
			if (pDLI->cnt == pDLI->sz)
				pDLI->pEnd = pEfrom;
			pDLI->lastAdded.pE = pEfrom;
		}
		/* unlink pEfrom */
		if (pEfrom->pPrev != NULL)
			pEfrom->pPrev->pNext = pEfrom->pNext;
		if (pEfrom->pNext != NULL)
			pEfrom->pNext->pPrev = pEfrom->pPrev;

		/* move pEfrom ... */
		if (toIndex > fromIndex) {
			/* a F c d T g ---> a c d T F g */
			pEfrom->pPrev = pEto;
			pEfrom->pNext = pEto->pNext;
			if (pEfrom->pNext != NULL)
				pEfrom->pNext->pPrev = pEfrom;
			pEto->pNext = pEfrom;
		} else {
			/* a T c d F g ---> a F T c d g */
			pEfrom->pNext = pEto;
			pEfrom->pPrev = pEto->pPrev;
			if (pEfrom->pPrev != NULL)
				pEfrom->pPrev->pNext = pEfrom;
			pEto->pPrev = pEfrom;
		}
	}

	/* update cacheElem and iter */
	pDLI->cacheElem.pE = NULL;
	pDLI->cacheElem.ix = 0;
	pDLI->iter.pE      = NULL;
	pDLI->iter.ix      = 0;

	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Clone list
 */
Tst_err
st_dynlist_clone(Tst_dynlist *pSrc, Tst_dynlist *pDst,
		Tst_dynlist_cbCloneInitElem cbCloneInitElem,
		Tst_dynlist_cbCloneCopyElem cbCloneCopyElem)
{
	Tst_err          res = ST_ERR_SUCC;
	Tst_dynlist_intn *pDLIsrc,
	                 *pDLIdst;
	Tst_dynlist_elem *pEsrc,
	                 *pEdst;
	void             *pD = NULL;

	ST_ASSERTN_IARG(pSrc == NULL || pSrc->pObInternal == NULL ||
			pDst == NULL || pDst->pObInternal == NULL ||
			cbCloneInitElem == NULL || cbCloneCopyElem == NULL)

	pDLIsrc = (Tst_dynlist_intn*)pSrc->pObInternal;
	pDLIdst = (Tst_dynlist_intn*)pDst->pObInternal;

	if (pDLIsrc->soElem != pDLIdst->soElem || pDLIsrc->cnt != pDLIdst->cnt ||
			pDLIdst->cbRsetElem != pDLIsrc->cbRsetElem ||
			pDLIdst->cbFreeElem != pDLIsrc->cbFreeElem) {
		/* reset dest list */
		st_dynlist_stc_rsetDL(pDst);
		pDLIdst->soElem     = pDLIsrc->soElem;
		pDLIdst->cbRsetElem = pDLIsrc->cbRsetElem;
		pDLIdst->cbFreeElem = pDLIsrc->cbFreeElem;
	}

	if (pDLIsrc->cnt > 0 && pDLIsrc->cnt == pDLIdst->cnt) {
		/* simply override data in dest list */
		pEsrc = pDLIsrc->pStart;
		pEdst = pDLIdst->pStart;
		while (pEsrc != NULL && pEsrc->pData != NULL) {
			if (pEdst->pData != NULL)
				pDLIsrc->cbFreeElem(pEdst->pData);
			/* */
			res = cbCloneInitElem(pEdst->pData);
			if (res == ST_ERR_SUCC)
				res = cbCloneCopyElem(pEsrc->pData, pEdst->pData);
			if (res != ST_ERR_SUCC)
				break;
			pEsrc = pEsrc->pNext;
			pEdst = pEdst->pNext;
		}
	} else {
		/* add all elements from source list to dest list */
		pEsrc = pDLIsrc->pStart;
		while (pEsrc != NULL && pEsrc->pData != NULL) {
			ST_CALLOC(pD, void*, 1, pDLIsrc->soElem)
			if (pD == NULL) {
				res = ST_ERR_OMEM;
				break;
			}
			res = cbCloneInitElem(pD);
			if (res == ST_ERR_SUCC)
				res = cbCloneCopyElem(pEsrc->pData, pD);
			/* */
			if (res == ST_ERR_SUCC)
				res = st_dynlist_addElement(pDst, pD);
			if (res != ST_ERR_SUCC) {
				ST_DELPOINT(pD)
				break;
			}
			pEsrc = pEsrc->pNext;
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Sort elements in list
 *   pCustomCmpOpts will be passed to cbCmpElems and may contain
 *     custom options for sorting or whatever
 *     (e.g. whether to sort in ascending/descending order)
 *
 * pDL             MUST be set
 * cbCmpElems      MUST be != NULL
 * cbStatus        may be NULL
 * pCustomCmpOpts  may be NULL
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_dynlist_sortElements(Tst_dynlist *pDL, Tst_dynlist_sortAlgo sortAlgo,
		Tst_dynlist_cbCompareElems cbCmpElems,
		ST_OPTARG(Tst_dynlist_cbSortStatus cbStatus),
		ST_OPTARG(void *pCustomCmpOpts))
{
	Tst_bool          resB = ST_B_TRUE;
	Tst_dynlist_intn  *pDLI;
	Tst_dynlist__qms  qms;
	Tst_dynlist_eix   sL,
	                  sR;
	Tst_uint32        eCnt = 0;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL ||
			cbCmpElems == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	if (pDLI->pStart == NULL || pDLI->pStart->pNext == NULL) {
		if (cbStatus != NULL)
			cbStatus(pDLI->cnt, pDLI->cnt);  /* 100% */
		return ST_B_TRUE;
	}

	eCnt = pDLI->cnt;
	if (sortAlgo == ST_DYNLIST_SA_DEF) {
		if (pDLI->cnt < 50)
			sortAlgo = ST_DYNLIST_SA_INSSORT;
		else
			sortAlgo = ST_DYNLIST_SA_MERGESORT;
	}
	if (cbStatus != NULL)
		cbStatus(0, eCnt);  /* 0% */
	switch (sortAlgo) {
	case ST_DYNLIST_SA_INSSORT:
		ST_DYNLIST__sortInsertionSort(pDLI,
				cbCmpElems, cbStatus, pCustomCmpOpts);
		break;
	case ST_DYNLIST_SA_QUICKSORT:
		qms.pDLI           = pDLI;
		qms.cbCmpElems     = cbCmpElems;
		qms.cbStatus       = cbStatus;
		qms.pCustomCmpOpts = pCustomCmpOpts;
		qms.recLev         = 0;
		/* */
		sL.pE = pDLI->pStart;
		sL.ix = 1;
		sR.pE = pDLI->lastAdded.pE;
		sR.ix = pDLI->lastAdded.ix;
		/* */
		ST_DYNLIST__sortQuickSort(&qms, eCnt, &sL, &sR);
		break;
	case ST_DYNLIST_SA_MERGESORT:
		qms.pDLI           = pDLI;
		qms.cbCmpElems     = cbCmpElems;
		qms.cbStatus       = cbStatus;
		qms.pCustomCmpOpts = pCustomCmpOpts;
		qms.recLev         = 0;
		/* */
		sL.pE = pDLI->pStart;
		sL.ix = 1;
		/* */
		pDLI->iter.pE      = NULL;
		pDLI->iter.ix      = 0;
		pDLI->cacheElem.pE = NULL;
		pDLI->cacheElem.ix = 0;
		/* */
		ST_DYNLIST__sortMergeSort(&qms, eCnt, &sL, &pDLI->lastAdded);
		/* update pDLI->pStart and pDLI->pEnd */
		pDLI->pStart = sL.pE;
		if (pDLI->cnt == pDLI->sz)
			pDLI->pEnd = pDLI->lastAdded.pE;
		break;
	default:
		resB = ST_B_FALSE;
	}
	if (resB && cbStatus != NULL)
		cbStatus(eCnt, eCnt);  /* 100% */
	return resB;
}

/*----------------------------------------------------------------------------*/

/*
 * Check internal state of list
 */
Tst_bool
st_dynlist_check(const Tst_dynlist *pDL)
{
	Tst_bool         resB        = ST_B_TRUE,
	                 ecntCorrect = ST_B_FALSE;
	Tst_uint32       cix         = 1;
	Tst_dynlist_intn *pDLI;
	Tst_dynlist_elem *pE;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDL == NULL || pDL->pObInternal == NULL)

	pDLI = (Tst_dynlist_intn*)pDL->pObInternal;

	/* check basics */
	if (pDLI->cnt == 0) {
		if (pDLI->pStart != NULL || pDLI->pEnd != NULL ||
				pDLI->lastAdded.pE != NULL)
			return ST_B_FALSE;
		return ST_B_TRUE;
	}
	if (pDLI->pStart == NULL || pDLI->pEnd == NULL ||
			pDLI->lastAdded.pE == NULL)
		return ST_B_FALSE;
	if (pDLI->cnt > pDLI->sz)
		return ST_B_FALSE;
	if (pDLI->lastAdded.ix != pDLI->cnt ||
			(pDLI->lastAdded.pE == pDLI->pEnd && pDLI->cnt != pDLI->sz))
		return ST_B_FALSE;
	if (pDLI->pStart->pPrev != NULL || pDLI->pEnd->pNext != NULL)
		return ST_B_FALSE;

	/* */
	pE = pDLI->pStart;
	while (pE != NULL) {
		if (pE == pE->pNext) {
			resB = ST_B_FALSE;
			break;
		}
		if (pE->pNext == NULL && pE != pDLI->pEnd) {
			resB = ST_B_FALSE;
			break;
		}
		if (pE != pDLI->pEnd && pE != pE->pNext->pPrev) {
			resB = ST_B_FALSE;
			break;
		}
		if (cix == pDLI->cnt) {
			if (pE != pDLI->lastAdded.pE) {
				resB = ST_B_FALSE;
				break;
			}
			ecntCorrect = ST_B_TRUE;
		} else if (pE == pDLI->lastAdded.pE) {
			resB = ST_B_FALSE;
			break;
		}
		if (cix == pDLI->sz) {
			if (pE != pDLI->pEnd) {
				resB = ST_B_FALSE;
				break;
			}
		} else if (pE == pDLI->pEnd) {
			resB = ST_B_FALSE;
			break;
		}
		if ((cix <= pDLI->cnt && pE->pData == NULL) ||
				(cix > pDLI->cnt && pE->pData != NULL)) {
			resB = ST_B_FALSE;
			break;
		}
		pE = pE->pNext;
		++cix;
	}
	if ((resB && cix != pDLI->sz + 1) || ! ecntCorrect)
		resB = ST_B_FALSE;
	return resB;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static LIBSANTAGGER_KWFNC_INLINE_S Tst_dynlist_elem*
ST_DYNLIST__getStartKnot(Tst_dynlist_intn *pDLI,
		const Tst_uint32 dstIx, Tst_int32 *pDirection, Tst_uint32 *pCurIx)
{
#	define LOC_GETDIST_(macDist, macBig, macSma)  {macDist = macBig - macSma;}
	Tst_uint32 distCE,
	           distLA,
	           distSK;
	Tst_int32  cmpCE,
	           cmpLA,
	           cmpSK;

	if (pDLI->cacheElem.pE != NULL) {
		cmpCE = ST_DYNLIST__CMP(dstIx, pDLI->cacheElem.ix);
		switch (cmpCE) {
		case 1:
			LOC_GETDIST_(distCE, dstIx, pDLI->cacheElem.ix)
			break;
		case -1:
			LOC_GETDIST_(distCE, pDLI->cacheElem.ix, dstIx)
			break;
		default:
			distCE = 0;
		}
	} else {
		distCE = ST_DYNLIST_MAXELEMS;
		cmpCE  = 0;
	}
	/* */
	if (distCE > 10 && pDLI->lastAdded.pE != NULL) {
		cmpLA = ST_DYNLIST__CMP(dstIx, pDLI->lastAdded.ix);
		switch (cmpLA) {
		case 1:
			LOC_GETDIST_(distLA, dstIx, pDLI->lastAdded.ix)
			break;
		case -1:
			LOC_GETDIST_(distLA, pDLI->lastAdded.ix, dstIx)
			break;
		default:
			distLA = 0;
		}
	} else {
		distLA = ST_DYNLIST_MAXELEMS;
		cmpLA  = -1;
	}
	/* */
	if (distCE > 10 && distLA > 10 && pDLI->pStart != NULL) {
		cmpSK = ST_DYNLIST__CMP(dstIx, 1);
		switch (cmpSK) {
		case 1:
			LOC_GETDIST_(distSK, dstIx, 1)
			break;
		case -1:
			LOC_GETDIST_(distSK, 1, dstIx)
			break;
		default:
			distSK = 0;
		}
	} else {
		distSK = ST_DYNLIST_MAXELEMS;
		cmpSK  = 1;
	}

	/* */
	if (ST_DYNLIST__CMP(distCE, distLA) <= 0) {
		/* distCE <= distLA */
		if (ST_DYNLIST__CMP(distCE, distSK) <= 0) {
			/* distCE <= distLA <= distSK */
			*pCurIx     = pDLI->cacheElem.ix;
			*pDirection = cmpCE;
			return pDLI->cacheElem.pE;
		} else {
			/* distSK <= distLA < distCE */
			*pCurIx     = 1;
			*pDirection = cmpSK;
			return pDLI->pStart;
		}
	} else {
		/* distCE > distLA */
		if (ST_DYNLIST__CMP(distLA, distSK) <= 0) {
			/* distLA <= distSK < distCE */
			*pCurIx     = pDLI->lastAdded.ix;
			*pDirection = cmpLA;
			return pDLI->lastAdded.pE;
		} else {
			/* distSK < distLA < distCE */
			*pCurIx     = 1;
			*pDirection = cmpSK;
			return pDLI->pStart;
		}
	}
#	undef LOC_GETDIST_
}

static Tst_dynlist_elem*
ST_DYNLIST__goto(Tst_dynlist_intn *pDLI,
		const Tst_uint32 dstIx)
{
	Tst_dynlist_elem *pE;
	Tst_int32        cmp = 0;
	Tst_uint32       cix = 0;

	pE = ST_DYNLIST__getStartKnot(pDLI, dstIx, &cmp, &cix);
	if (cmp >= 0) {
		/* go to right side */
		while (pE != NULL) {
			if (ST_DYNLIST__CMP(cix, dstIx) == 0) {
				pDLI->cacheElem.pE = pE;
				pDLI->cacheElem.ix = cix;
				return pE;
			}
			++cix;
			pE = pE->pNext;
		}
	} else {
		/* go to left side */
		while (pE != NULL) {
			if (ST_DYNLIST__CMP(cix, dstIx) == 0) {
				pDLI->cacheElem.pE = pE;
				pDLI->cacheElem.ix = cix;
				return pE;
			}
			--cix;
			pE = pE->pPrev;
		}
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/

static void
ST_DYNLIST__sortInsertionSort(Tst_dynlist_intn *pDLI,
		Tst_dynlist_cbCompareElems cbCmpElems, Tst_dynlist_cbSortStatus cbStatus,
		void *pCustomCmpOpts)
{
	Tst_dynlist_elem *pEc,
	                 *pEp;
	void             *pD;
	Tst_uint32       curIx,
	                 maxIx;

	pEc = pDLI->pStart->pNext;
	/* */
	if (cbStatus == NULL) {
		/** w/o status */
		while (pEc != NULL && pEc->pData != NULL) {
			pD  = pEc->pData;
			pEp = pEc;
			while (pEp->pPrev != NULL) {
				if (cbCmpElems(pEp->pPrev->pData, pD, pCustomCmpOpts) <= 0)
					break;
				pEp->pData = pEp->pPrev->pData;
				pEp        = pEp->pPrev;
			}
			pEp->pData = pD;
			pEc        = pEc->pNext;
		}
	} else {
		/** w status */
		curIx = 2;
		maxIx = pDLI->cnt;
		while (pEc != NULL && pEc->pData != NULL) {
			if (curIx < maxIx && curIx > 0 && curIx % 1000 == 0)
				cbStatus(curIx, maxIx);
			pD  = pEc->pData;
			pEp = pEc;
			while (pEp->pPrev != NULL) {
				if (cbCmpElems(pEp->pPrev->pData, pD, pCustomCmpOpts) <= 0)
					break;
				pEp->pData = pEp->pPrev->pData;
				pEp        = pEp->pPrev;
			}
			pEp->pData = pD;
			pEc        = pEc->pNext;
			++curIx;
		}
	}
}

static LIBSANTAGGER_KWFNC_INLINE_S void
ST_DYNLIST__sortInsertionSort_4QMS(
		Tst_dynlist_cbCompareElems cbCmpElems, void *pCustomCmpOpts,
		Tst_dynlist_elem *pLeft, Tst_dynlist_elem *pRight)
{
	Tst_dynlist_elem *pEc,
	                 *pEp;
	void             *pD;

	pEc = pLeft->pNext;
	if (pEc != NULL)
		while (pEc != pRight->pNext) {
			pD  = pEc->pData;
			pEp = pEc;
			while (pEp->pPrev != NULL && pEp != pLeft) {
				if (cbCmpElems(pEp->pPrev->pData, pD, pCustomCmpOpts) <= 0)
					break;
				pEp->pData = pEp->pPrev->pData;
				pEp        = pEp->pPrev;
			}
			pEp->pData = pD;
			pEc        = pEc->pNext;
		}
}

/*----------------------------------------------------------------------------*/

static LIBSANTAGGER_KWFNC_INLINE_S void
ST_DYNLIST__sortQuickSort_pivot(
		Tst_dynlist_cbCompareElems cbCmpElems, void *pCustomCmpOpts,
		Tst_dynlist_eix *pLeft, Tst_dynlist_eix *pMiddle,
		Tst_dynlist_eix *pRight,
		void *pT)
{
#	define LOC_MOT_THRESHOLD_  15
#	define LOC_R_AND_X_(pMacX)  { \
					pT                 = pRight->pE->pData; \
					pRight->pE->pData  = (pMacX)->pE->pData; \
					(pMacX)->pE->pData = pT; \
				}
#	define LOC_DEB_(macCS, pMacL, pMacM, pMacR)  { \
					/*printf("\t\t%s\t\tL: %010u,   M: %010u,  R: %010u\n", \
							macCS, *(Tst_uint32*)pMacL->pE->pData, \
							*(Tst_uint32*)pMacM->pE->pData, \
							*(Tst_uint32*)pMacR->pE->pData);*/ \
				}
	if (pRight->ix - pLeft->ix > LOC_MOT_THRESHOLD_) {
		pMiddle->pE = pLeft->pE;
		pMiddle->ix = pLeft->ix;
		while (pMiddle->ix < pLeft->ix + (pRight->ix - pLeft->ix) / 2 &&
				pMiddle->ix + 1 < pRight->ix) {
			pMiddle->pE = pMiddle->pE->pNext;
			++pMiddle->ix;
		}
		/* get 'median-of-three' */
		if (cbCmpElems(pLeft->pE->pData,
				pMiddle->pE->pData, pCustomCmpOpts) >= 0) {
			/* middle <= left */
			if (cbCmpElems(pLeft->pE->pData,
					pRight->pE->pData, pCustomCmpOpts) >= 0) {
				/* middle <= left && left >= right */
				if (cbCmpElems(pMiddle->pE->pData,
						pRight->pE->pData, pCustomCmpOpts) >= 0) {
					/* middle <= left && left >= right && middle >= right
					 * right <= middle <= left */
					LOC_DEB_("CASE 1 (RML*)", pRight, pMiddle, pLeft)
					LOC_R_AND_X_(pMiddle)
				} else {
					/* middle <= left && left >= right && middle < right
					 * middle < right <= left */
					LOC_DEB_("CASE 2 (MRL*)", pMiddle, pRight, pLeft)
				}
			} else {
				/* middle <= left && left < right */
				if (cbCmpElems(pMiddle->pE->pData,
						pRight->pE->pData, pCustomCmpOpts) >= 0) {
					/* middle <= left && left < right && middle >= right
					 * --> middle == left < right;  l=5, m=5, r=6 */
					LOC_DEB_("CASE 3 (LMR )", pLeft, pMiddle, pRight)
					LOC_R_AND_X_(pMiddle)
				} else {
					/* middle <= left && left < right && middle < right
					 * middle <= left < right  */
					LOC_DEB_("CASE 4 (MLR*)", pMiddle, pLeft, pRight)
					LOC_R_AND_X_(pLeft)
				}
			}
		} else {
			/* middle > left */
			if (cbCmpElems(pLeft->pE->pData,
					pRight->pE->pData, pCustomCmpOpts) >= 0) {
				/* middle > left && left >= right */
				/** case 5 (RLM) */
					/* middle > left && left >= right && middle >= right
					 * right <= left < middle */
					LOC_DEB_("CASE 5 (RLM*)", pRight, pLeft, pMiddle)
					LOC_R_AND_X_(pLeft)
				/** case 6 (---) */
					/* middle > left && left >= right && middle < right
					 * impossible; m=5, l=4, r=4 --> m>l,l>=r,m!<r */
			} else {
				/* middle > left && left < right */
				if (cbCmpElems(pMiddle->pE->pData,
						pRight->pE->pData, pCustomCmpOpts) >= 0) {
					/* middle > left && left < right && middle >= right
					 * left < right <= middle */
					LOC_DEB_("CASE 7 (LRM*)", pLeft, pRight, pMiddle)
				} else {
					/* middle > left && left < right && middle < right
					 * left < middle < right */
					LOC_DEB_("CASE 8 (LMR*)", pLeft, pMiddle, pRight)
					LOC_R_AND_X_(pMiddle)
				}
			}
		}
	}
#	undef LOC_MOT_THRESHOLD_
#	undef LOC_R_AND_X_
#	undef LOC_DEB_
}

static void
ST_DYNLIST__sortQuickSort_partition(
		Tst_dynlist_cbCompareElems cbCmpElems, void *pCustomCmpOpts,
		Tst_dynlist_eix *pLeft, Tst_dynlist_eix *pMiddle,
		Tst_dynlist_eix *pRight)
{
	Tst_dynlist_eix eixI,
	                eixJ;
	void            *pT = NULL;

	eixI.pE = pLeft->pE;
	eixI.ix = pLeft->ix;
	eixJ.pE = pRight->pE->pPrev;  /* start left from pivot element */
	eixJ.ix = pRight->ix - 1;
	ST_DYNLIST__sortQuickSort_pivot(cbCmpElems, pCustomCmpOpts,
			pLeft, pMiddle, pRight, pT);
	/* pivot element is pRight->pE->pData */

	do {
		/* search from left for element that is greater than pivot */
		while (cbCmpElems(eixI.pE->pData, pRight->pE->pData,
					pCustomCmpOpts) <= 0 &&
				eixI.ix < pRight->ix) {
			eixI.pE = eixI.pE->pNext;
			++eixI.ix;
		}
		/* search from right for element that is smaller than pivot */
		while (cbCmpElems(eixJ.pE->pData, pRight->pE->pData,
					pCustomCmpOpts) >= 0 &&
				eixJ.ix > pLeft->ix) {
			eixJ.pE = eixJ.pE->pPrev;
			--eixJ.ix;
		}
		if (eixI.ix < eixJ.ix) {
			pT             = eixI.pE->pData;
			eixI.pE->pData = eixJ.pE->pData;
			eixJ.pE->pData = pT;
		}
	} while (eixI.ix < eixJ.ix);

	/* swap pivot (pRight->pE->pData) to final destination (eixI.pE->pData) */
	if (cbCmpElems(eixI.pE->pData, pRight->pE->pData,
			pCustomCmpOpts) > 0) {
		pT                = eixI.pE->pData;
		eixI.pE->pData    = pRight->pE->pData;
		pRight->pE->pData = pT;
	}
	/* */
	pMiddle->pE = eixI.pE;
	pMiddle->ix = eixI.ix;
}

static void
ST_DYNLIST__sortQuickSort(Tst_dynlist__qms *pQS, const Tst_uint32 eCnt,
		Tst_dynlist_eix *pLeft, Tst_dynlist_eix *pRight)
{
#	define LOC_IS_THRESHOLD_  25
	Tst_dynlist_eix tmiddle,
	                tleft,
	                tright;

	++pQS->recLev;

	/* recursive/iterative version */
	/** */
	tmiddle.pE = NULL;
	tmiddle.ix = 0;
	tleft.pE   = pLeft->pE;
	tleft.ix   = pLeft->ix;
	tright.pE  = pRight->pE;
	tright.ix  = pRight->ix;
	/** */
	while (tleft.ix < tright.ix) {
		/* */
		ST_DYNLIST__sortQuickSort_partition(
				pQS->cbCmpElems, pQS->pCustomCmpOpts,
				&tleft, &tmiddle, &tright);
		/* */
		if (pQS->recLev == 1 && pQS->cbStatus != NULL &&
				tright.ix > tleft.ix)
			pQS->cbStatus(eCnt - (tright.ix - tleft.ix), eCnt);
		/* */
		if (tright.ix - tmiddle.ix > tmiddle.ix - tleft.ix) {
			if (tmiddle.pE->pPrev != NULL) {
				tmiddle.pE = tmiddle.pE->pPrev;
				--tmiddle.ix;
				if (tmiddle.ix > tleft.ix) {
					if (tmiddle.ix - tleft.ix < LOC_IS_THRESHOLD_) {
						ST_DYNLIST__sortInsertionSort_4QMS(
								pQS->cbCmpElems, pQS->pCustomCmpOpts,
								tleft.pE, tmiddle.pE);
					} else
						ST_DYNLIST__sortQuickSort(pQS, eCnt, &tleft, &tmiddle);
				}
				tleft.pE = tmiddle.pE->pNext->pNext;
				tleft.ix = tmiddle.ix + 2;
			} else {
				tleft.pE = tmiddle.pE->pNext;
				tleft.ix = tmiddle.ix + 1;
			}
		} else {
			if (tmiddle.pE->pNext != NULL) {
				tmiddle.pE = tmiddle.pE->pNext;
				++tmiddle.ix;
				if (tmiddle.ix < tright.ix) {
					if (tright.ix - tmiddle.ix < LOC_IS_THRESHOLD_) {
						ST_DYNLIST__sortInsertionSort_4QMS(
								pQS->cbCmpElems, pQS->pCustomCmpOpts,
								tmiddle.pE, tright.pE);
					} else
						ST_DYNLIST__sortQuickSort(pQS, eCnt, &tmiddle, &tright);
				}
				tright.pE = tmiddle.pE->pPrev->pPrev;
				tright.ix = tmiddle.ix - 2;
			} else {
				tright.pE = tmiddle.pE->pPrev;
				tright.ix = tmiddle.ix - 1;
			}
		}
	}

	--pQS->recLev;
#	undef LOC_IS_THRESHOLD_
}

/*----------------------------------------------------------------------------*/

static void
ST_DYNLIST__sortMergeSort_merge(Tst_dynlist__qms *pMS,
		Tst_dynlist_eix *pLL, Tst_dynlist_eix *pLR,
		Tst_dynlist_eix *pRL, Tst_dynlist_eix *pRR)
{
	Tst_dynlist_eix  xr,
	                 xe;
	Tst_dynlist_elem *pEXRN,
	                 *pELRP,
	                 *pERLP,
	                 *pERRP;
	Tst_uint32       lrIx;

	xe.pE = pLL->pE;
	xe.ix = pLL->ix;
	xr.pE = pRL->pE;
	xr.ix = pRL->ix;
	lrIx  = pLR->ix;
	while (xe.ix <= lrIx && xr.ix <= pRR->ix) {
		if (pMS->cbCmpElems(xe.pE->pData, xr.pE->pData,
				pMS->pCustomCmpOpts) <= 0) {
			xe.pE = xe.pE->pNext;
			++xe.ix;
		} else {
			pEXRN = xr.pE->pNext;
			pELRP = pLR->pE->pPrev;
			pERLP = pRL->pE->pPrev;
			pERRP = pRR->pE->pPrev;
			/* */
			/** xr->prev <-> xr <-> xr->next
			 ** ---> xr->prev <-> xr->next     */
			xr.pE->pPrev->pNext = xr.pE->pNext;
			if (xr.pE->pNext != NULL)
				xr.pE->pNext->pPrev = xr.pE->pPrev;
			/** move to pos of xe */
			/** xe->prev <-> xe <-> xe->next
			 ** ---> xe->prev <-> xr <-> xe <-> xe->next  */
			xr.pE->pPrev = xe.pE->pPrev;
			if (xr.pE->pPrev != NULL)
				xr.pE->pPrev->pNext = xr.pE;
			xr.pE->pNext = xe.pE;
			xe.pE->pPrev = xr.pE;

			/* update pLL, pLR, pRL and pRR */
			if (xe.ix == pLL->ix) {
				pLL->pE = xr.pE;
			}
			if (xe.ix <= pLR->ix && pLL->ix != pLR->ix) {
				if (xe.ix == pLR->ix) {
					pLR->pE = xr.pE;
				} else if (xe.ix + 1 == pLR->ix) {
					pLR->pE = xe.pE;
				} else {
					pLR->pE = pELRP;
				}
			}
			if (xe.ix <= pRL->ix) {
				if (xe.ix == pRL->ix) {
					pRL->pE = xr.pE;
				} else if (xe.ix + 1 == pRL->ix) {
					pRL->pE = xe.pE;
				} else {  /* ! */
					if (xr.ix + 1 != pRL->ix) {
						pRL->pE = pERLP;
					}
				}
			}
			if (xe.ix + 1 == pRR->ix) {
				pRR->pE = xe.pE;
			} else if (xr.ix == pRR->ix) {
				pRR->pE = pERRP;
			}

			/* */
			xr.pE = pEXRN;
			++xr.ix;
			++xe.ix;
			++lrIx;
		}
	}
}

static void
ST_DYNLIST__sortMergeSort(Tst_dynlist__qms *pMS, const Tst_uint32 eCnt,
		Tst_dynlist_eix *pL, Tst_dynlist_eix *pR)
{
#	define LOC_IS_THRESHOLD_  15
	Tst_dynlist_eix midL,
	                midR;
	Tst_uint32      dst;

	++pMS->recLev;

	if (pR->ix - pL->ix < LOC_IS_THRESHOLD_) {
		ST_DYNLIST__sortInsertionSort_4QMS(
				pMS->cbCmpElems, pMS->pCustomCmpOpts,
				pL->pE, pR->pE);
	} else {
		/* find middle element */
		midL.pE = pL->pE;
		midL.ix = pL->ix;
		dst     = pL->ix + (pR->ix - pL->ix) / 2;
		if (dst + 1 >= pR->ix)
			dst = pR->ix - 1;
		while (midL.ix < dst) {
			midL.pE = midL.pE->pNext;
			++midL.ix;
		}
		midR.pE = midL.pE->pNext;
		midR.ix = midL.ix + 1;
		/* sort left half */
		if (midL.ix != pL->ix)
			ST_DYNLIST__sortMergeSort(pMS, eCnt, pL, &midL);
		/* */
		if (pMS->recLev == 1 && pMS->cbStatus != NULL)
			pMS->cbStatus(eCnt / 3, eCnt);  /* 33% */
		/* sort right half */
		if (pR->ix != midR.ix)
			ST_DYNLIST__sortMergeSort(pMS, eCnt, &midR, pR);
		/* */
		if (pMS->recLev == 1 && pMS->cbStatus != NULL)
			pMS->cbStatus((eCnt / 3) * 2, eCnt);  /* 66% */
		/* merge left and right half */
		ST_DYNLIST__sortMergeSort_merge(pMS, pL, &midL, &midR, pR);
	}

	--pMS->recLev;
#	undef LOC_SORT2_
#	undef LOC_IS_THRESHOLD_
}

/******************************************************************************/
