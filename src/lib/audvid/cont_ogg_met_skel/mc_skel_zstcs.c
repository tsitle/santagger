/******************************************************************************/
/* mc_skel_zstcs.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for OGG-Skeleton structures                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/sys_math.h>
/** */
#define SRC_MC_SKEL_ZSTCS_C
#include "mc_skel_zstcs-prot.h"
#undef SRC_MC_SKEL_ZSTCS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_skeleton */
void
st_contOgg_stc_initSkel(Tst_contOgg_skeleton *pSk)
{
	ST_ASSERTN_VOID(pSk == NULL)

	st_contOgg_stc_initSkelBoneArr(&pSk->bones);
	st_contOgg_stc_rsetSkel(pSk);
}

void
st_contOgg_stc_rsetSkel(Tst_contOgg_skeleton *pSk)
{
	ST_ASSERTN_VOID(pSk == NULL)

	st_contOgg_stc_rsetSkelBoneArr(&pSk->bones);
	st_contOgg_stc_rsetSkelIdent(&pSk->ident);
}

/* Tst_contOgg_skelIdent */
void
st_contOgg_stc_rsetSkelIdent(Tst_contOgg_skelIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->isSet  = ST_B_FALSE;
	pID->packNr = 0;
	/* */
	pID->versMaj = 0;
	pID->versMin = 0;
	ST_SYSMATH_STC_RSETUI64(pID->presTimeNum)
	ST_SYSMATH_STC_RSETUI64(pID->presTimeDen)
	ST_SYSMATH_STC_RSETUI64(pID->baseTimeNum)
	ST_SYSMATH_STC_RSETUI64(pID->baseTimeDen)
	memset(pID->utcTime, 0, sizeof(pID->utcTime));
}

/* Tst_contOgg_skelBone */
void
st_contOgg_stc_initSkelBone(Tst_contOgg_skelBone *pSkB)
{
	ST_ASSERTN_VOID(pSkB == NULL)

	pSkB->pMsgHdFlds = NULL;
	st_contOgg_stc_rsetSkelBone(pSkB);
}

void
st_contOgg_stc_rsetSkelBone(Tst_contOgg_skelBone *pSkB)
{
	ST_ASSERTN_VOID(pSkB == NULL)

	pSkB->offsMsgHdFld = 0;
	pSkB->serial       = 0;
	pSkB->hdPckCnt     = 0;
	ST_SYSMATH_STC_RSETUI64(pSkB->granNum)
	ST_SYSMATH_STC_RSETUI64(pSkB->granDen)
	ST_SYSMATH_STC_RSETUI64(pSkB->baseGran)
	pSkB->preroll   = 0;
	pSkB->granShift = 0;
	ST_DELPOINT(pSkB->pMsgHdFlds)
}

/* Tst_contOgg_skelBoneArr */
void
st_contOgg_stc_initSkelBoneArr(Tst_contOgg_skelBoneArr *pBA)
{
	ST_ASSERTN_VOID(pBA == NULL)

	pBA->pArr = NULL;
	pBA->sz   = 0;
	st_contOgg_stc_rsetSkelBoneArr(pBA);
}

void
st_contOgg_stc_rsetSkelBoneArr(Tst_contOgg_skelBoneArr *pBA)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pBA == NULL)

	if (pBA->pArr != NULL) {
		for (x = 0; x < pBA->sz; x++)
			st_contOgg_stc_rsetSkelBone(&pBA->pArr[x]);
		ST_DELPOINT(pBA->pArr)
	}
	pBA->sz  = 0;
	pBA->cnt = 0;
}

Tst_err
st_contOgg_stc_reszSkelBoneArr(Tst_contOgg_skelBoneArr *pBA,
		const Tst_uint32 newSz)
{
	Tst_uint32 x;

	ST_ASSERTN_IARG(pBA == NULL)

	if (newSz > pBA->sz) {
		ST_REALLOC(pBA->pArr, Tst_contOgg_skelBone*,
				newSz, sizeof(Tst_contOgg_skelBone))
		if (pBA->pArr == NULL)
			return ST_ERR_OMEM;
		for (x = pBA->sz; x < newSz; x++)
			st_contOgg_stc_initSkelBone(&pBA->pArr[x]);
		pBA->sz = newSz;
	}

	return ST_ERR_SUCC;
}

/******************************************************************************/
