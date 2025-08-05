/******************************************************************************/
/* tag_basics.c                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Tag basics                                                   */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#define SRC_TAG_BASICS_C
#include "tag_basics-prot.h"
#undef SRC_TAG_BASICS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** basic Tag info */
typedef struct {
	Tst_bool   hasTag;      /* is the tag present ? */
	Tst_bool   areFldsSet;  /* does the tag contain at least one valid field ? */
	Tst_foffs  offs;        /* offset of the tag in the stream */
	Tst_uint32 sz;          /* size of the tag */
} Tst_tagBasics__intn;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_tagBasics */
Tst_err
st_tagBas_stc_initTB(Tst_tagBasics *pTB)
{
	ST_ASSERTN_IARG(pTB == NULL)

	ST_CALLOC(pTB->pObInternal, Tst_tagBasics__intn*,
			1, sizeof(Tst_tagBasics__intn))
	if (pTB->pObInternal == NULL)
		return ST_ERR_OMEM;
	st_tagBas_stc_rsetTB(pTB);
	return ST_ERR_SUCC;
}

void
st_tagBas_stc_rsetTB(Tst_tagBasics *pTB)
{
	Tst_tagBasics__intn *pTBI;

	ST_ASSERTN_VOID(pTB == NULL || pTB->pObInternal == NULL)

	pTBI = (Tst_tagBasics__intn*)pTB->pObInternal;
	/* */
	pTBI->hasTag     = ST_B_FALSE;
	pTBI->areFldsSet = ST_B_FALSE;
	pTBI->offs       = -1;
	pTBI->sz         = 0;
}

void
st_tagBas_stc_freeTB(Tst_tagBasics *pTB)
{
	ST_ASSERTN_VOID(pTB == NULL)

	if (pTB->pObInternal != NULL) {
		st_tagBas_stc_rsetTB(pTB);
		ST_DELPOINT(pTB->pObInternal)
	}
}

Tst_err
st_tagBas_stc_copyTB(const Tst_tagBasics *pTBsrc,
		Tst_tagBasics *pTBdst)
{
	Tst_tagBasics__intn       *pTBIdst;
	Tst_tagBasics__intn const *pTBIsrc;

	ST_ASSERTN_IARG(pTBsrc == NULL || pTBsrc->pObInternal == NULL ||
			pTBdst == NULL || pTBdst->pObInternal == NULL)

	pTBIsrc = (Tst_tagBasics__intn*)pTBsrc->pObInternal;
	pTBIdst = (Tst_tagBasics__intn*)pTBdst->pObInternal;

	pTBIdst->hasTag     = pTBIsrc->hasTag;
	pTBIdst->areFldsSet = pTBIsrc->areFldsSet;
	pTBIdst->offs       = pTBIsrc->offs;
	pTBIdst->sz         = pTBIsrc->sz;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_tagBas_gs_setHasTag(Tst_tagBasics *pTB, const Tst_bool hasTag)
{
	ST_ASSERTN_VOID(pTB == NULL || pTB->pObInternal == NULL)

	((Tst_tagBasics__intn*)pTB->pObInternal)->hasTag = hasTag;
}

void
st_tagBas_gs_setHasFields(Tst_tagBasics *pTB, const Tst_bool areFldsSet)
{
	ST_ASSERTN_VOID(pTB == NULL || pTB->pObInternal == NULL)

	((Tst_tagBasics__intn*)pTB->pObInternal)->areFldsSet = areFldsSet;
	if (areFldsSet)
		((Tst_tagBasics__intn*)pTB->pObInternal)->hasTag = ST_B_TRUE;
}

void
st_tagBas_gs_setOffset(Tst_tagBasics *pTB, const Tst_foffs tagOffs)
{
	ST_ASSERTN_VOID(pTB == NULL || pTB->pObInternal == NULL)

	((Tst_tagBasics__intn*)pTB->pObInternal)->offs = tagOffs;
}

void
st_tagBas_gs_setSize(Tst_tagBasics *pTB, const Tst_uint32 tagSz)
{
	ST_ASSERTN_VOID(pTB == NULL || pTB->pObInternal == NULL)

	((Tst_tagBasics__intn*)pTB->pObInternal)->sz = tagSz;
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_tagBas_gs_getHasTag(const Tst_tagBasics *pTB)
{
	if (pTB == NULL || pTB->pObInternal == NULL)
		return ST_B_FALSE;

	return ((Tst_tagBasics__intn*)pTB->pObInternal)->hasTag;
}

Tst_bool
st_tagBas_gs_getHasFields(const Tst_tagBasics *pTB)
{
	if (pTB == NULL || pTB->pObInternal == NULL)
		return ST_B_FALSE;

	return ((Tst_tagBasics__intn*)pTB->pObInternal)->areFldsSet;
}

Tst_foffs
st_tagBas_gs_getOffset(const Tst_tagBasics *pTB)
{
	if (pTB == NULL || pTB->pObInternal == NULL)
		return -1;

	return ((Tst_tagBasics__intn*)pTB->pObInternal)->offs;
}

Tst_uint32
st_tagBas_gs_getSize(const Tst_tagBasics *pTB)
{
	if (pTB == NULL || pTB->pObInternal == NULL)
		return 0;

	return ((Tst_tagBasics__intn*)pTB->pObInternal)->sz;
}

/******************************************************************************/
