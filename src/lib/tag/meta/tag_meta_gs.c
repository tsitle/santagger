/******************************************************************************/
/* tag_meta_gs.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Getter/Setter functions for Meta Tags                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 14.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/dynlist.h>
#include <santag/common/streamrd.h>
#include <santag/common/sys_fnc.h>
#include <santag/tag/tag_basics.h>
#include <santag/tag/tag_apev2.h>
#include <santag/tag/tag_id3v2.h>
#include <santag/tag/tag_id3v1.h>
#include <santag/tag/tag_vorbc.h>
#include <santag/tag/tag_meta.h>
/** */
#define SRC_TAG_META_GS_C
#include "tag_meta-prot.h"
#include "tag_meta_fnc-prot.h"
#undef SRC_TAG_META_GS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** */
static Tst_err
ST_TMETA__gs_setArr_someForAllTags(Tst_tagMeta_mtArr *pMA,
                                   const Tst_bool setStrrd,
                                   const Tst_bool setStrwr,
                                   const Tst_bool setFilen,
                                   ST_OPTARG(Tst_streamrd *pStrrd),
                                   ST_OPTARG(Tst_streamwr *pStrwr),
                                   const Tst_str *pFilen);
/** */
static Tst_err
ST_TMETA__gs_setTag_some(Tst_tagMeta_mt *pMT,
                         const Tst_bool setStrrd,
                         const Tst_bool setStrwr,
                         const Tst_bool setFilen,
                         ST_OPTARG(Tst_streamrd *pStrrd),
                         ST_OPTARG(Tst_streamwr *pStrwr),
                         const Tst_str *pFilen);
/** */
static void*
ST_TMETA__getRealTag(const Tst_tagMeta_mt *pMT,
                     const Tst_tagMeta_type tagTp);
/** */
static Tst_tagMeta_mt*
ST_TMETA__getNextTag(const Tst_tagMeta_mtArr *pMA,
                     ST_OPTARG(const Tst_tagMeta_mt *pLast),
                     ST_OPTARG(const Tst_tagMeta_type *pTagTp),
                     ST_OPTARG(const Tst_tagMeta_type *pTagTpOrg));

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_tagMeta_gs_setArr_strrdForAllTags(Tst_tagMeta_mtArr *pMA,
		ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_TMETA__gs_setArr_someForAllTags(pMA,
			ST_B_TRUE, ST_B_FALSE, ST_B_FALSE,
			pStrrd, NULL, NULL);
}

void
st_tagMeta_gs_setArr_strwrForAllTags(Tst_tagMeta_mtArr *pMA,
		ST_OPTARG(Tst_streamwr *pStrwr))
{
	ST_TMETA__gs_setArr_someForAllTags(pMA,
			ST_B_FALSE, ST_B_TRUE, ST_B_FALSE,
			NULL, pStrwr, NULL);
}

Tst_err
st_tagMeta_gs_setArr_filenForAllTags(Tst_tagMeta_mtArr *pMA,
		const Tst_str *pFilen)
{
	return ST_TMETA__gs_setArr_someForAllTags(pMA,
			ST_B_FALSE, ST_B_FALSE, ST_B_TRUE,
			NULL, NULL, pFilen);
}

/*----------------------------------------------------------------------------*/

void
st_tagMeta_gs_setArr_cbForSettingOptions(Tst_tagMeta_mtArr *pMA,
		ST_OPTARG(Tst_tagMeta_cbSetOpts_iv1 cbSetOptsIV1),
		ST_OPTARG(void *pSetOptsDataIV1),
		ST_OPTARG(Tst_tagMeta_cbSetOpts_iv2 cbSetOptsIV2),
		ST_OPTARG(void *pSetOptsDataIV2),
		ST_OPTARG(Tst_tagMeta_cbSetOpts_av2 cbSetOptsAV2),
		ST_OPTARG(void *pSetOptsDataAV2),
		ST_OPTARG(Tst_tagMeta_cbSetOpts_vor cbSetOptsVOR),
		ST_OPTARG(void *pSetOptsDataVOR))
{
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_VOID(pMA == NULL || pMA->pObInternal == NULL)

	pMAI = (Tst_tagMeta_mtArr_intn*)pMA->pObInternal;

	pMAI->cbSetOptsIV1    = cbSetOptsIV1;
	pMAI->pSetOptsDataIV1 = pSetOptsDataIV1;
	pMAI->cbSetOptsIV2    = cbSetOptsIV2;
	pMAI->pSetOptsDataIV2 = pSetOptsDataIV2;
	pMAI->cbSetOptsAV2    = cbSetOptsAV2;
	pMAI->pSetOptsDataAV2 = pSetOptsDataAV2;
	pMAI->cbSetOptsVOR    = cbSetOptsVOR;
	pMAI->pSetOptsDataVOR = pSetOptsDataVOR;
}

/*----------------------------------------------------------------------------*/

/*
 * Set stream reader for tag
 */
void
st_tagMeta_gs_setTag_strrd(Tst_tagMeta_mt *pMT, ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_ASSERTN_VOID(pMT == NULL || pMT->pObInternal == NULL)

	ST_TMETA__gs_setTag_some(pMT,
			ST_B_TRUE, ST_B_FALSE, ST_B_FALSE,
			pStrrd, NULL, NULL);
}

/*
 * Set stream writer for tag
 */
void
st_tagMeta_gs_setTag_strwr(Tst_tagMeta_mt *pMT, ST_OPTARG(Tst_streamwr *pStrwr))
{
	ST_ASSERTN_VOID(pMT == NULL || pMT->pObInternal == NULL)

	ST_TMETA__gs_setTag_some(pMT,
			ST_B_FALSE, ST_B_TRUE, ST_B_FALSE,
			NULL, pStrwr, NULL);
}

/*
 * Set filename for tag
 */
Tst_err
st_tagMeta_gs_setTag_filen(Tst_tagMeta_mt *pMT, const Tst_str *pFilen)
{
	ST_ASSERTN_IARG(pMT == NULL || pMT->pObInternal == NULL ||
			pFilen == NULL)

	return ST_TMETA__gs_setTag_some(pMT,
			ST_B_FALSE, ST_B_FALSE, ST_B_TRUE,
			NULL, NULL, pFilen);
}

/*
 * Copy the st_tagBasics from the original tag
 */
Tst_err
st_tagMeta_gs_setTag_tbasOrg(Tst_tagMeta_mt *pMT,
		const Tst_tagBasics *pTBasOrg)
{
	Tst_err res;

	ST_ASSERTN_IARG(pMT == NULL || pMT->pObInternal == NULL ||
			pTBasOrg == NULL)

	res = st_tagBas_stc_copyTB(pTBasOrg,
			&((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tbasOrg);
	if (res == ST_ERR_SUCC)
		((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tbasOrgSet = ST_B_TRUE;
	return res;
}

/*
 * Set the original tag type
 */
void
st_tagMeta_gs_setTag_typeOrg(Tst_tagMeta_mt *pMT,
		const Tst_tagMeta_type mtType)
{
	ST_ASSERTN_VOID(pMT == NULL || pMT->pObInternal == NULL)

	((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tpOrg = mtType;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Is any tag in pMA present ?
 *   --> if any of pTag->tbas.hasTag is set
 *
 * pMA   MUST be set
 */
Tst_bool
st_tagMeta_gs_getArr_isAnyTagPresent(const Tst_tagMeta_mtArr *pMA)
{
	return (st_tagMeta_gs_getArr_amountPresentTags(pMA) > 0);
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_tagMeta_gs_getArr_amountTags(const Tst_tagMeta_mtArr *pMA)
{
	ST_ASSERTN_NUM(0, pMA == NULL || pMA->pObInternal == NULL)

	return st_dynlist_getElementCount(
			&((Tst_tagMeta_mtArr_intn*)pMA->pObInternal)->list);
}

Tst_uint32
st_tagMeta_gs_getArr_amountPresentTags(const Tst_tagMeta_mtArr *pMA)
{
	Tst_uint32 num = 0;
	Tst_tagBasics const    *pTBas;
	Tst_tagMeta_mt         *pMT;
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_NUM(0, pMA == NULL || pMA->pObInternal == NULL)

	pMAI = (Tst_tagMeta_mtArr_intn*)pMA->pObInternal;

	if (! st_dynlist_ite_gotoFirst(&pMAI->list))
		return 0;
	do {
		pMT = (Tst_tagMeta_mt*)st_dynlist_ite_getCurrent(&pMAI->list);
		if (pMT == NULL)
			return 0;

		pTBas = st_tagMeta_gs_getTag_tbas(pMT);
		if (pTBas != NULL && st_tagBas_gs_getHasTag(pTBas))
			++num;
	} while (st_dynlist_ite_gotoNext(&pMAI->list));
	return num;
}

/*----------------------------------------------------------------------------*/

/*
 * Get tag with UID
 *
 * pMA   MUST be set
 */
Tst_tagMeta_mt*
st_tagMeta_gs_getArr_tag_byUID(const Tst_tagMeta_mtArr *pMA,
		const Tst_uint32 uid)
{
	Tst_uint32 ix = 0;

	if (uid == 0)
		return NULL;
	return st_tagMeta_getTag_byUidOrBsIx(pMA, &ix, &uid, NULL, NULL);
}

/*
 * Get tag with bsIx and bsSIx
 *
 * pMA   MUST be set
 */
Tst_tagMeta_mt*
st_tagMeta_gs_getArr_tag_byBSIx(const Tst_tagMeta_mtArr *pMA,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx)
{
	Tst_uint32 ix = 0;

	return st_tagMeta_getTag_byUidOrBsIx(pMA, &ix, NULL, &bsIx, &bsSIx);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Is Tag set (is the real tag pointer != NULL and the UID != 0) ?
 */
Tst_bool
st_tagMeta_gs_getTag_isSet(const Tst_tagMeta_mt *pMT)
{
	Tst_tagMeta_mt_intn *pMTI;

	if (pMT == NULL || pMT->pObInternal == NULL)
		return ST_B_FALSE;

	pMTI = (Tst_tagMeta_mt_intn*)pMT->pObInternal;

	switch (pMTI->tp) {
	case ST_MTAG_TTP_IV1:
		return (pMTI->pIV1 != NULL && pMTI->uid != 0);
	case ST_MTAG_TTP_AV2:
		return (pMTI->pAV2 != NULL && pMTI->uid != 0);
	case ST_MTAG_TTP_VOR:
		return (pMTI->pVOR != NULL && pMTI->uid != 0);
	case ST_MTAG_TTP_MRG:
	case ST_MTAG_TTP_IV2:
		return (pMTI->pIV2 != NULL && pMTI->uid != 0);
	default:
		return ST_B_FALSE;
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Get the st_tagBasics of the 'real' tag inside the meta tag
 *
 * pMT   MUST be set
 */
const Tst_tagBasics*
st_tagMeta_gs_getTag_tbas(const Tst_tagMeta_mt *pMT)
{
	Tst_tagMeta_mt_intn *pMTI;

	ST_ASSERTN_NULL(pMT == NULL || pMT->pObInternal == NULL)

	pMTI = (Tst_tagMeta_mt_intn*)pMT->pObInternal;

	if (! st_tagMeta_gs_getTag_isSet(pMT))
		return NULL;
	switch (pMTI->tp) {
	case ST_MTAG_TTP_IV1:
		return st_id3v1_gs_getTag_tbas(pMTI->pIV1);
	case ST_MTAG_TTP_AV2:
		return st_apev2_gs_getTag_tbas(pMTI->pAV2);
	case ST_MTAG_TTP_VOR:
		return st_vorbc_gs_getTag_tbas(pMTI->pVOR);
	case ST_MTAG_TTP_MRG:
	case ST_MTAG_TTP_IV2:
		return st_id3v2_gs_getTag_tbas(pMTI->pIV2);
	default:
		break;
	}
	return NULL;
}

/*
 * Get the st_tagBasics of the original tag
 *   if these haven't been set we'll return the
 *   st_tagBasics of the current tag
 *
 * pMT   MUST be set
 */
const Tst_tagBasics*
st_tagMeta_gs_getTag_tbasOrg(const Tst_tagMeta_mt *pMT)
{
	ST_ASSERTN_NULL(pMT == NULL || pMT->pObInternal == NULL)

	if (! ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tbasOrgSet)
		return st_tagMeta_gs_getTag_tbas(pMT);
	return &((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tbasOrg;
}

const Tst_str*
st_tagMeta_gs_getTag_filen(const Tst_tagMeta_mt *pMT)
{
	Tst_tagMeta_mt_intn *pMTI;

	ST_ASSERTN_NULL(pMT == NULL || pMT->pObInternal == NULL)

	pMTI = (Tst_tagMeta_mt_intn*)pMT->pObInternal;

	/* get from real tag */
	if (pMTI->pIV1 != NULL) {
		return st_id3v1_gs_getTag_filen(pMTI->pIV1);
	} else if (pMTI->pIV2 != NULL) {
		return st_id3v2_gs_getTag_filen(pMTI->pIV2);
	} else if (pMTI->pAV2 != NULL) {
		return st_apev2_gs_getTag_filen(pMTI->pAV2);
	} else if (pMTI->pVOR != NULL) {
		return st_vorbc_gs_getTag_filen(pMTI->pVOR);
	}

	return ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->pFilen;
}

/*----------------------------------------------------------------------------*/

Tst_uint32
st_tagMeta_gs_getTag_uid(const Tst_tagMeta_mt *pMT)
{
	ST_ASSERTN_NUM(0, pMT == NULL || pMT->pObInternal == NULL)

	return ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->uid;
}

/*
 * Get the current tag type
 */
Tst_tagMeta_type
st_tagMeta_gs_getTag_type(const Tst_tagMeta_mt *pMT)
{
	ST_ASSERTN_NUM(ST_MTAG_TTP_NONE, pMT == NULL || pMT->pObInternal == NULL)

	return ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tp;
}

/*
 * Get the original tag type
 */
Tst_tagMeta_type
st_tagMeta_gs_getTag_typeOrg(const Tst_tagMeta_mt *pMT)
{
	ST_ASSERTN_NUM(ST_MTAG_TTP_NONE, pMT == NULL || pMT->pObInternal == NULL)

	if (((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tpOrg == ST_MTAG_TTP_NONE)
		return ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tp;
	return ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->tpOrg;
}

Tst_bool
st_tagMeta_gs_getTag_bsIx(const Tst_tagMeta_mt *pMT,
		Tst_uint32 *pBsIx, Tst_uint32 *pBsSIx)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pMT == NULL || pMT->pObInternal == NULL ||
			pBsIx == NULL || pBsSIx == NULL)

	*pBsIx  = ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->bsIx;
	*pBsSIx = ((Tst_tagMeta_mt_intn*)pMT->pObInternal)->bsSIx;
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

/*
 * Get the 'real' tag inside the meta tag
 *
 * pMT   MUST be set
 */
void*
st_tagMeta_gs_getRealTag_void(const Tst_tagMeta_mt *pMT)
{
	return ST_TMETA__getRealTag(pMT, st_tagMeta_gs_getTag_type(pMT));
}

Tst_id3v1_tag*
st_tagMeta_gs_getRealTag_iv1(const Tst_tagMeta_mt *pMT)
{
	return (Tst_id3v1_tag*)ST_TMETA__getRealTag(pMT, ST_MTAG_TTP_IV1);
}

Tst_id3v2_tag*
st_tagMeta_gs_getRealTag_iv2(const Tst_tagMeta_mt *pMT)
{
	return (Tst_id3v2_tag*)ST_TMETA__getRealTag(pMT, ST_MTAG_TTP_IV2);
}

Tst_apev2_tag*
st_tagMeta_gs_getRealTag_av2(const Tst_tagMeta_mt *pMT)
{
	return (Tst_apev2_tag*)ST_TMETA__getRealTag(pMT, ST_MTAG_TTP_AV2);
}

Tst_vorbc_tag*
st_tagMeta_gs_getRealTag_vor(const Tst_tagMeta_mt *pMT)
{
	return (Tst_vorbc_tag*)ST_TMETA__getRealTag(pMT, ST_MTAG_TTP_VOR);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Find next tag in array
 *   If pLast == NULL the first tag will be returned
 *   If pLast != NULL the next tag after pLast will be returned
 *
 * pMA   MUST be set
 *
 * returns: pointer to next tag
 *          NULL if none found
 */
Tst_tagMeta_mt*
st_tagMeta_ite_nextTag_any(const Tst_tagMeta_mtArr *pMA,
		ST_OPTARG(const Tst_tagMeta_mt *pLast))
{
	return ST_TMETA__getNextTag(pMA, pLast, NULL, NULL);
}

Tst_tagMeta_mt*
st_tagMeta_ite_nextTag_byType(const Tst_tagMeta_mtArr *pMA,
		Tst_tagMeta_type mtType, ST_OPTARG(const Tst_tagMeta_mt *pLast))
{
	return ST_TMETA__getNextTag(pMA, pLast, &mtType, NULL);
}

Tst_tagMeta_mt*
st_tagMeta_ite_nextTag_byTypeOrg(const Tst_tagMeta_mtArr *pMA,
		Tst_tagMeta_type mtTypeOrg, ST_OPTARG(const Tst_tagMeta_mt *pLast))
{
	return ST_TMETA__getNextTag(pMA, pLast, NULL, &mtTypeOrg);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Set fstc/streamrd/streamwr/filename for array and all existing tags
 */
static Tst_err
ST_TMETA__gs_setArr_someForAllTags(Tst_tagMeta_mtArr *pMA,
		const Tst_bool setStrrd, const Tst_bool setStrwr,
		const Tst_bool setFilen,
		ST_OPTARG(Tst_streamrd *pStrrd), ST_OPTARG(Tst_streamwr *pStrwr),
		const Tst_str *pFilen)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_tagMeta_mt         *pMT;
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_IARG(pMA == NULL || pMA->pObInternal == NULL)

	pMAI = (Tst_tagMeta_mtArr_intn*)pMA->pObInternal;

	if (setStrrd)
		pMAI->pStrrd = pStrrd;
	if (setStrwr)
		pMAI->pStrwr = pStrwr;
	if (setFilen) {
		ST_ASSERTN_IARG(pFilen == NULL)

		res = st_sysStrcpy(pFilen, &pMAI->pFilen);
		if (res != ST_ERR_SUCC)
			return res;
	}

	if (! st_dynlist_ite_gotoFirst(&pMAI->list))
		return ST_ERR_SUCC;
	do {
		pMT = (Tst_tagMeta_mt*)st_dynlist_ite_getCurrent(&pMAI->list);
		ST_ASSERTN_NUM(ST_ERR_FAIL, pMT == NULL)
		if (! st_tagMeta_gs_getTag_isSet(pMT))
			continue;

		res = ST_TMETA__gs_setTag_some(pMT,
				setStrrd, setStrwr, setFilen,
				pStrrd, pStrwr, pFilen);
		if (res != ST_ERR_SUCC)
			break;
	} while (st_dynlist_ite_gotoNext(&pMAI->list));
	return res;
}

/*
 * Set fstc/streamrd/streamwr/filename for one tag
 */
static Tst_err
ST_TMETA__gs_setTag_some(Tst_tagMeta_mt *pMT,
		const Tst_bool setStrrd, const Tst_bool setStrwr, const Tst_bool setFilen,
		ST_OPTARG(Tst_streamrd *pStrrd), ST_OPTARG(Tst_streamwr *pStrwr),
		const Tst_str *pFilen)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_tagMeta_mt_intn *pMTI;

	ST_ASSERTN_IARG(pMT == NULL || pMT->pObInternal == NULL)

	pMTI = (Tst_tagMeta_mt_intn*)pMT->pObInternal;

	if (setFilen) {
		ST_ASSERTN_IARG(pFilen == NULL)

		res = st_sysStrcpy(pFilen, &pMTI->pFilen);
		if (res != ST_ERR_SUCC)
			return res;
	}

	if (pMTI->pIV1 != NULL) {
		if (setStrrd)
			st_id3v1_gs_setTag_strrd(pMTI->pIV1, pStrrd);
		if (setStrwr)
			st_id3v1_gs_setTag_strwr(pMTI->pIV1, pStrwr);
		if (setFilen)
			res = st_id3v1_gs_setTag_filen(pMTI->pIV1, pFilen);
	} else if (pMTI->pIV2 != NULL) {
		if (setStrrd)
			st_id3v2_gs_setTag_strrd(pMTI->pIV2, pStrrd);
		if (setStrwr)
			st_id3v2_gs_setTag_strwr(pMTI->pIV2, pStrwr);
		if (setFilen)
			res = st_id3v2_gs_setTag_filen(pMTI->pIV2, pFilen);
	} else if (pMTI->pAV2 != NULL) {
		if (setStrrd)
			st_apev2_gs_setTag_strrd(pMTI->pAV2, pStrrd);
		if (setStrwr)
			st_apev2_gs_setTag_strwr(pMTI->pAV2, pStrwr);
		if (setFilen)
			res = st_apev2_gs_setTag_filen(pMTI->pAV2, pFilen);
	} else if (pMTI->pVOR != NULL) {
		if (setStrrd)
			st_vorbc_gs_setTag_strrd(pMTI->pVOR, pStrrd);
		if (setStrwr)
			st_vorbc_gs_setTag_strwr(pMTI->pVOR, pStrwr);
		if (setFilen)
			res = st_vorbc_gs_setTag_filen(pMTI->pVOR, pFilen);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

static void*
ST_TMETA__getRealTag(const Tst_tagMeta_mt *pMT,
		const Tst_tagMeta_type tagTp)
{
	Tst_tagMeta_mt_intn *pMTI;

	ST_ASSERTN_NULL(pMT == NULL || pMT->pObInternal == NULL)

	pMTI = (Tst_tagMeta_mt_intn*)pMT->pObInternal;

	switch (tagTp) {
	case ST_MTAG_TTP_IV1:
		return pMTI->pIV1;
	case ST_MTAG_TTP_IV2:
	case ST_MTAG_TTP_MRG:
		return pMTI->pIV2;
	case ST_MTAG_TTP_AV2:
		return pMTI->pAV2;
	case ST_MTAG_TTP_VOR:
		return pMTI->pVOR;
	default:
		return NULL;
	}
}

/*----------------------------------------------------------------------------*/

static Tst_tagMeta_mt*
ST_TMETA__getNextTag(const Tst_tagMeta_mtArr *pMA,
		ST_OPTARG(const Tst_tagMeta_mt *pLast),
		ST_OPTARG(const Tst_tagMeta_type *pTagTp),
		ST_OPTARG(const Tst_tagMeta_type *pTagTpOrg))
{
	Tst_tagMeta_mt         *pMTcur;
	Tst_tagMeta_mt_intn    *pMTIcur,
	                       *pMTIlast;
	Tst_tagMeta_mtArr_intn *pMAI;

	ST_ASSERTN_NULL(pMA == NULL || pMA->pObInternal == NULL)

	pMAI     = (Tst_tagMeta_mtArr_intn*)pMA->pObInternal;
	pMTIlast = (pLast == NULL ? NULL :
			(Tst_tagMeta_mt_intn*)pLast->pObInternal);

	if (! st_dynlist_ite_gotoFirst(&pMAI->list))
		return NULL;
	do {
		pMTcur = (Tst_tagMeta_mt*)st_dynlist_ite_getCurrent(&pMAI->list);
		if (pMTcur == NULL)
			return NULL;
		if (! st_tagMeta_gs_getTag_isSet(pMTcur))
			continue;
		pMTIcur = (Tst_tagMeta_mt_intn*)pMTcur->pObInternal;
		/**printf("tmeta: uid 0x%08x\n", pMTIcur->uid);**/

		if (pMTIlast == NULL || pMTIcur->uniqueIx > pMTIlast->uniqueIx) {
			if (pTagTp == NULL && pTagTpOrg == NULL)
				return pMTcur;
			else if (pTagTp != NULL && pMTIcur->tp == *pTagTp)
				return pMTcur;
			else if (pTagTpOrg != NULL &&
					(pMTIcur->tpOrg == *pTagTpOrg ||
						(pMTIcur->tpOrg == ST_MTAG_TTP_NONE &&
							pMTIcur->tp == *pTagTpOrg)))
				return pMTcur;
		}
	} while (st_dynlist_ite_gotoNext(&pMAI->list));
	return NULL;
}

/******************************************************************************/
