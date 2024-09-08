/******************************************************************************/
/* mc_vorbcomm_gs.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Getter/Setter functions for Vorbis Comments                                */
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
#include <santag/common/binobj.h>
#include <santag/common/sys_fnc.h>
#include <santag/audvid/cont_ogg_vorbcomm.h>
/** */
#define SRC_MC_VORBCOMM_GS_C
#include "mc_vorbcomm-prot.h"
/*** */
#include "mc_vorbcomm_zrd-prot.h"
#include "../cont_ogg/cont_ogg-prot.h"
#undef SRC_MC_VORBCOMM_GS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Replace raw comment
 */
Tst_err
st_contOgg_vorbComm_gs_setRawCommentData(Tst_contOgg_substream *pBS,
		Tst_binobj *pBinDat)
{
	Tst_err res;
	Tst_contOgg_substr_intn    *pBSI;
	Tst_contOgg_vorbCommentRaw *pVCR = NULL;

	ST_ASSERTN_IARG(pBS == NULL || pBS->pObInternal == NULL ||
			pBinDat == NULL)

	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;

	if (pBSI->bsType == ST_CONTOGG_BSTP_FLAC && pBSI->pFlac != NULL)
		pVCR = &pBSI->pFlac->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_VORB && pBSI->pVorb != NULL)
		pVCR = &pBSI->pVorb->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_SPEE && pBSI->pSpee != NULL)
		pVCR = &pBSI->pSpee->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_CELT && pBSI->pCelt != NULL)
		pVCR = &pBSI->pCelt->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_THEO && pBSI->pTheo != NULL)
		pVCR = &pBSI->pTheo->comm;
	else
		return ST_ERR_FAIL;

	res = st_binobj_copy(pBinDat,
			((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->pRawCmtBO);

	if (res == ST_ERR_SUCC)
		res = st_contOgg_vorbComm_parseBasics(pBSI, pVCR);

	if (res == ST_ERR_SUCC)
		((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->wasModified = ST_B_TRUE;

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Get pointer to raw Vorbis Comment from stream
 *
 * returns: pointer to comment on success or NULL
 */
const Tst_contOgg_vorbCommentRaw*
st_contOgg_vorbComm_gs_getRawComment(const Tst_contOgg_substream *pBS)
{
	Tst_contOgg_substr_intn const *pBSI;
	Tst_contOgg_vorbCommentRaw    *pVCR = NULL;

	ST_ASSERTN_NULL(pBS == NULL || pBS->pObInternal == NULL)

	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;

	if (pBSI->bsType == ST_CONTOGG_BSTP_FLAC && pBSI->pFlac != NULL)
		pVCR = &pBSI->pFlac->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_VORB && pBSI->pVorb != NULL)
		pVCR = &pBSI->pVorb->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_SPEE && pBSI->pSpee != NULL)
		pVCR = &pBSI->pSpee->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_CELT && pBSI->pCelt != NULL)
		pVCR = &pBSI->pCelt->comm;
	else if (pBSI->bsType == ST_CONTOGG_BSTP_THEO && pBSI->pTheo != NULL)
		pVCR = &pBSI->pTheo->comm;
	/* */
	if (pVCR == NULL ||
			! ((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->isSet)
		return NULL;
	return pVCR;
}

/*
 * Is comment set (valid) ?
 */
Tst_bool
st_contOgg_vorbComm_gs_getRC_isSet(const Tst_contOgg_vorbCommentRaw *pVCR)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pVCR == NULL || pVCR->pObInternal == NULL)

	return ((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->isSet;
}

/*
 * Get amount of elements in comment
 */
Tst_uint32
st_contOgg_vorbComm_gs_getRC_elementCount(const Tst_contOgg_vorbCommentRaw *pVCR)
{
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_NUM(0, pVCR == NULL || pVCR->pObInternal == NULL)

	pVCRI = (Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal;
	return (! pVCRI->isSet ? 0 : pVCRI->elemCnt);
}

/*
 * Get vendor from comment
 */
Tst_err
st_contOgg_vorbComm_gs_getRC_vendor(const Tst_contOgg_vorbCommentRaw *pVCR,
		Tst_str **ppVendor)
{
	Tst_uint32 len;
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_IARG(pVCR == NULL || pVCR->pObInternal == NULL ||
			ppVendor == NULL)

	pVCRI = (Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal;
	/* */
	len = st_sysStrlen(pVCRI->pVendor);
	ST_REALLOC(*ppVendor, Tst_buf*, len + 1, 1)
	if (*ppVendor == NULL)
		return ST_ERR_OMEM;
	if (len > 0)
		memcpy(*ppVendor, pVCRI->pVendor, len);
	(*ppVendor)[len] = 0;
	return ST_ERR_SUCC;
}

/*
 * Get raw comment
 */
Tst_err
st_contOgg_vorbComm_gs_getRC_rawData(const Tst_contOgg_vorbCommentRaw *pVCR,
		Tst_binobj *pBinDat)
{
	ST_ASSERTN_IARG(pVCR == NULL || pVCR->pObInternal == NULL ||
			pBinDat == NULL)

	return st_binobj_copy(
			((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->pRawCmtBO,
			pBinDat);
}

/*
 * Get pointer to raw comment
 */
Tst_binobj*
st_contOgg_vorbComm_gs_getRC_rawDataPointer(const Tst_contOgg_vorbCommentRaw *pVCR)
{
	ST_ASSERTN_NULL(pVCR == NULL || pVCR->pObInternal == NULL)

	return ((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->pRawCmtBO;
}

/*
 * Was raw comment modified ?
 */
Tst_bool
st_contOgg_vorbComm_gs_getRC_wasModified(const Tst_contOgg_vorbCommentRaw *pVCR)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pVCR == NULL || pVCR->pObInternal == NULL)

	return ((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->wasModified;
}

/*
 * Get OGG packet no. where raw comment came from
 */
Tst_uint32
st_contOgg_vorbComm_gs_getRC_srcPackNr(const Tst_contOgg_vorbCommentRaw *pVCR)
{
	ST_ASSERTN_NUM(0, pVCR == NULL || pVCR->pObInternal == NULL)

	return ((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->packNr;
}

/*
 * Get offset where raw comment began in the OGG packet
 */
Tst_int32
st_contOgg_vorbComm_gs_getRC_srcCmtBegPos(const Tst_contOgg_vorbCommentRaw *pVCR)
{
	ST_ASSERTN_NUM(-1, pVCR == NULL || pVCR->pObInternal == NULL)

	return ((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->srcCmtBegPos;
}

/*
 * Get offset where raw comment ended in the OGG packet
 */
Tst_int32
st_contOgg_vorbComm_gs_getRC_srcCmtEndPos(const Tst_contOgg_vorbCommentRaw *pVCR)
{
	ST_ASSERTN_NUM(-1, pVCR == NULL || pVCR->pObInternal == NULL)

	return ((Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal)->srcCmtEndPos;
}

/******************************************************************************/
