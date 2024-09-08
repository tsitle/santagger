/******************************************************************************/
/* tag_id3v1_fldadd.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for adding ID3v1 (Extended) Tag Fields                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/tag/tag_id3v1.h>
/** */
#define SRC_TAG_ID3V1_FLDADD_C
#include "tag_id3v1-prot.h"
/*** */
#include "tag_id3v1_zfdeb-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V1_FLDADD_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_fldAdd(Tst_id3v1_tag *pTag, const Tst_id3v1_fldData *pFldAdd)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res;
	Tst_id3v1_fldData_intn const *pFldI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFldAdd == NULL || pFldAdd->pObInternal == NULL)

	pFldI = (Tst_id3v1_fldData_intn const*)pFldAdd->pObInternal;

	if (! pFldI->isOK) {
		Tst_id3v1_tag_intn *pTagI = (Tst_id3v1_tag_intn*)pTag->pObInternal;

		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v1_d_fdeb(&pTagI->opts, 0, cFNCN, &pFldI->fldPrI,
					"ignoring field");
		return ST_ERR_SUCC;
	}

	switch (pFldI->fldPrI.id) {
	case ST_ID3V1_FID_TITL:
		res = st_id3v1_gs_setField_title(pTag, &pFldI->dataStr);
		break;
	case ST_ID3V1_FID_ALBU:
		res = st_id3v1_gs_setField_album(pTag, &pFldI->dataStr);
		break;
	case ST_ID3V1_FID_TRKN:
		res = st_id3v1_gs_setField_tracknr(pTag, pFldI->dataIVal);
		break;
	case ST_ID3V1_FID_ARTI:
		res = st_id3v1_gs_setField_artist(pTag, &pFldI->dataStr);
		break;
	case ST_ID3V1_FID_COMM:
		res = st_id3v1_gs_setField_comment(pTag, &pFldI->dataStr);
		break;
	case ST_ID3V1_FID_GENR:
		res = st_id3v1_gs_setField_genre(pTag, pFldI->dataIVal);
		break;
	case ST_ID3V1_FID_YEAR:
		res = st_id3v1_gs_setField_year(pTag, pFldI->dataIVal);
		break;
	default:
		res = ST_ERR_IARG;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_fldSet_dataStr(Tst_id3v1_tag *pTag,
		const Tst_id3v1_frID frID, const Tst_mtes_string *pText)
{
	Tst_err res;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	switch (frID) {
	case ST_ID3V1_FID_TITL:
		res = st_id3v1_gs_setField_title(pTag, pText);
		break;
	case ST_ID3V1_FID_ALBU:
		res = st_id3v1_gs_setField_album(pTag, pText);
		break;
	case ST_ID3V1_FID_ARTI:
		res = st_id3v1_gs_setField_artist(pTag, pText);
		break;
	case ST_ID3V1_FID_COMM:
		res = st_id3v1_gs_setField_comment(pTag, pText);
		break;
	default:
		res = ST_ERR_IARG;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_fldSet_dataInt(Tst_id3v1_tag *pTag,
		const Tst_id3v1_frID frID, const Tst_int32 ival)
{
	Tst_err res;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	switch (frID) {
	case ST_ID3V1_FID_TRKN:
		res = st_id3v1_gs_setField_tracknr(pTag, ival);
		break;
	case ST_ID3V1_FID_GENR:
		res = st_id3v1_gs_setField_genre(pTag, ival);
		break;
	case ST_ID3V1_FID_YEAR:
		res = st_id3v1_gs_setField_year(pTag, ival);
		break;
	default:
		res = ST_ERR_IARG;
	}
	return res;
}

/******************************************************************************/
