/******************************************************************************/
/* tag_meta_fnc.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for Meta Tags                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_meta.h"
/** */
#define SRC_TAG_META_FNC_C
#include "tag_meta_fnc-prot.h"
#undef SRC_TAG_META_FNC_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_uint32
st_tagMeta_getNewUID(Tst_tagMeta_mtArr_intn *pMAI)
{
	const Tst_uint32 cMAXRUNS = 100;
	Tst_uint32 rnd  = 0,
	           x,
	           runs = 0;
	Tst_tagMeta_mt *pMT;

	do {
		for (x = 0; x < 4; x++)
			rnd = rnd | (st_sysGetRand(0, 255) << (x * 8));
		/* check if UID already exists */
		if (st_dynlist_ite_gotoFirst(&pMAI->list))
			do {
				pMT = (Tst_tagMeta_mt*)st_dynlist_ite_getCurrent(&pMAI->list);
				if (pMT == NULL)
					break;
				if (! st_tagMeta_gs_getTag_isSet(pMT))
					continue;
				if (((Tst_tagMeta_mt_intn*)pMT->pObInternal)->uid == rnd) {
					rnd = 0;
					break;
				}
			} while (st_dynlist_ite_gotoNext(&pMAI->list));
	} while (rnd == 0 && ++runs < cMAXRUNS);
	if (rnd == 0)
		rnd = st_dynlist_getElementCount(&pMAI->list) + 1;
	return rnd;
}

/*----------------------------------------------------------------------------*/

Tst_tagMeta_mt*
st_tagMeta_getTag_byUidOrBsIx(const Tst_tagMeta_mtArr *pMA,
		Tst_uint32 *pIndex, const Tst_uint32 *pUID,
		const Tst_uint32 *pBSIx, const Tst_uint32 *pBSSIx)
{
	Tst_tagMeta_mt         *pMT;
	Tst_tagMeta_mt_intn    *pMTI;
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_NULL(pMA == NULL || pMA->pObInternal == NULL)

	pMAI = (Tst_tagMeta_mtArr_intn*)pMA->pObInternal;

	if ((pUID != NULL && *pUID == 0) ||
			! st_dynlist_ite_gotoFirst(&pMAI->list))
		return NULL;
	do {
		pMT = (Tst_tagMeta_mt*)st_dynlist_ite_getCurrent(&pMAI->list);
		if (pMT == NULL)
			return NULL;
		if (! st_tagMeta_gs_getTag_isSet(pMT))
			continue;
		*pIndex = st_dynlist_ite_getCurrentIndex(&pMAI->list);
		pMTI    = (Tst_tagMeta_mt_intn*)pMT->pObInternal;

		if (pUID != NULL) {
			if (pMTI->uid == *pUID)
				return pMT;
		} else {
			if (pMTI->bsIx == *pBSIx &&
					(pMTI->bsSIx == *pBSSIx ||
						(pMTI->bsSIx == 0 && *pBSSIx == 1) ||
						(pMTI->bsSIx == 1 && *pBSSIx == 0)))
				return pMT;
		}
	} while (st_dynlist_ite_gotoNext(&pMAI->list));
	return NULL;
}

/*----------------------------------------------------------------------------*/

/*
 * Get description array from tag type
 *   e.g. ST_MTAG_TTP_VOR --> {"Vorbis", "vor"}
 */
const char**
st_tagMeta_fnc_getTagDesc(const Tst_tagMeta_type mttp)
{
	switch (mttp) {
	case ST_MTAG_TTP_IV1:
		return ST_MTAG_TAGDSC_IV1;
	case ST_MTAG_TTP_IV2:
		return ST_MTAG_TAGDSC_IV2;
	case ST_MTAG_TTP_AV2:
		return ST_MTAG_TAGDSC_AV2;
	case ST_MTAG_TTP_VOR:
		return ST_MTAG_TAGDSC_VOR;
	case ST_MTAG_TTP_MRG:
		return ST_MTAG_TAGDSC_MRG;
	default:
		return ST_MTAG_TAGDSC_NA;
	}
}

/******************************************************************************/
