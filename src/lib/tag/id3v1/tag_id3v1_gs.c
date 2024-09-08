/******************************************************************************/
/* tag_id3v1_gs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Getter/Setter functions for ID3v1 (Extended) Tags                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_id3v1.h"
/** */
#define SRC_TAG_ID3V1_GS_C
#include "tag_id3v1_gs-prot.h"
/*** */
#include "tag_id3v1-pp_flds.h"
#include "tag_id3v1_stcs-prot.h"
#include "tag_id3v1_fnc-prot.h"
#include "tag_id3v1_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V1_GS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__gs_mergTF_s(const char *pFnc,
                     Tst_id3v1_tag_intn *pTagI,
                     const Tst_id3v1_frID dstID,
                     const Tst_mtes_string *pStr);
static Tst_err
ST_TIV1__gs_mergTF_i(const char *pFnc,
                     Tst_id3v1_tag_intn *pTagI,
                     const Tst_id3v1_frID dstID,
                     Tst_int32 ival);
/** */
static Tst_err
ST_TIV1__gs_getTF_s(Tst_id3v1_tag_intn *pTagI,
                    const Tst_id3v1_frID srcID,
                    Tst_mtes_string *pStr);
static Tst_int32
ST_TIV1__gs_getTF_i(Tst_id3v1_tag_intn *pTagI,
                    const Tst_id3v1_frID srcID);
/** */
static Tst_id3v1_fldProp*
ST_TIV1__ite_nextTagFldProp(ST_OPTARG(Tst_id3v1_fldProp *pFldPrLast),
                            const Tst_bool withRestrictions,
                            Tst_id3v1_fldProp *pFldPrNext);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_id3v1_gs_setTag_strrd(Tst_id3v1_tag *pTag, ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v1_tag_intn*)pTag->pObInternal)->pStrrd = pStrrd;
}

void
st_id3v1_gs_setTag_strwr(Tst_id3v1_tag *pTag, ST_OPTARG(Tst_streamwr *pStrwr))
{
	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	((Tst_id3v1_tag_intn*)pTag->pObInternal)->pStrwr = pStrwr;
}

Tst_err
st_id3v1_gs_setTag_filen(Tst_id3v1_tag *pTag, const Tst_str *pFilen)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pFilen == NULL)

	return st_sysStrcpy(pFilen,
			&((Tst_id3v1_tag_intn*)pTag->pObInternal)->pFilen);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_gs_setField_artist(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pStr == NULL)

	return ST_TIV1__gs_mergTF_s(LOC_FNCN_,
			(Tst_id3v1_tag_intn*)pTag->pObInternal, ST_ID3V1_FID_ARTI, pStr);
#	undef LOC_FNCN_
}

Tst_err
st_id3v1_gs_setField_album(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pStr == NULL)

	return ST_TIV1__gs_mergTF_s(LOC_FNCN_,
			(Tst_id3v1_tag_intn*)pTag->pObInternal, ST_ID3V1_FID_ALBU, pStr);
#	undef LOC_FNCN_
}

Tst_err
st_id3v1_gs_setField_title(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pStr == NULL)

	return ST_TIV1__gs_mergTF_s(LOC_FNCN_,
			(Tst_id3v1_tag_intn*)pTag->pObInternal, ST_ID3V1_FID_TITL, pStr);
#	undef LOC_FNCN_
}

Tst_err
st_id3v1_gs_setField_comment(Tst_id3v1_tag *pTag, const Tst_mtes_string *pStr)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL || pStr == NULL)

	return ST_TIV1__gs_mergTF_s(LOC_FNCN_,
			(Tst_id3v1_tag_intn*)pTag->pObInternal, ST_ID3V1_FID_COMM, pStr);
#	undef LOC_FNCN_
}

Tst_err
st_id3v1_gs_setField_tracknr(Tst_id3v1_tag *pTag, Tst_int32 tnr)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	return ST_TIV1__gs_mergTF_i(LOC_FNCN_,
			(Tst_id3v1_tag_intn*)pTag->pObInternal, ST_ID3V1_FID_TRKN, tnr);
#	undef LOC_FNCN_
}

Tst_err
st_id3v1_gs_setField_genre(Tst_id3v1_tag *pTag, Tst_int32 gen)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	return ST_TIV1__gs_mergTF_i(LOC_FNCN_,
			(Tst_id3v1_tag_intn*)pTag->pObInternal, ST_ID3V1_FID_GENR, gen);
#	undef LOC_FNCN_
}

Tst_err
st_id3v1_gs_setField_year(Tst_id3v1_tag *pTag, Tst_int32 year)
{
#	define LOC_FNCN_  __FUNCTION__

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	return ST_TIV1__gs_mergTF_i(LOC_FNCN_,
			(Tst_id3v1_tag_intn*)pTag->pObInternal, ST_ID3V1_FID_YEAR, year);
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_gs_setField_props(Tst_id3v1_fldData *pFld, const Tst_id3v1_frID id)
{
	Tst_err    res;
	Tst_bool   isInArr;
	Tst_uint32 ixArr = 0;
	Tst_id3v1_fldData_intn       *pFldI;
	Tst_id3v1_fldProp_intn const *pFldPrIArr,
	                             *pFldPrISrc;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v1_fldData_intn*)pFld->pObInternal;
	/* */
	st_id3v1_stc_rsetFPr_i(&pFldI->fldPrI);
	pFldI->isOK = ST_B_FALSE;

	if ((int)id < 0 || id >= ST_ID3V1_FID_NONE)
		return ST_ERR_IARG;

	/* try to find field properties entry */
	isInArr = st_id3v1_fnc_isFldPropInArr_byEnum(ST_ID3V1_FLDPROPS, id,
			ST_ID3V1E_FID_NONE, &ixArr);
	if (! isInArr)
		return ST_ERR_IARG;
	pFldPrIArr = (Tst_id3v1_fldProp_intn const*)ST_ID3V1_FLDPROPS;
	pFldPrISrc = &pFldPrIArr[ixArr];

	/* now we copy the original field props */
	res = st_id3v1_stc_copyFPr_i(pFldPrISrc, &pFldI->fldPrI);
	if (res != ST_ERR_SUCC)
		st_id3v1_stc_rsetFPr_i(&pFldI->fldPrI);
	return res;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_gs_setFieldData_string(Tst_id3v1_fldData *pFld,
		Tst_mtes_string *pStr)
{
	Tst_err res;
	Tst_id3v1_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pStr == NULL)

	pFldI = (Tst_id3v1_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->isOK = ST_B_FALSE;
	/* */
	res = st_mtes_strCpy(pStr, &pFldI->dataStr);
	if (res != ST_ERR_SUCC)
		return res;
	/* strip string */
	res = st_id3v1_fnc_stripStr(&pFldI->dataStr, NULL, NULL, NULL);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	pFldI->isOK = ! st_mtes_isStrEmptyCh(&pFldI->dataStr);
	return ST_ERR_SUCC;
}

Tst_err
st_id3v1_gs_setFieldData_ival(Tst_id3v1_fldData *pFld, const Tst_int32 ival)
{
	Tst_int32 noneVal;
	Tst_id3v1_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_id3v1_fldData_intn*)pFld->pObInternal;

	/* */
	pFldI->dataIVal = ival;
	/* */
	switch (pFldI->fldPrI.id) {
	case ST_ID3V1_FID_TRKN:
		noneVal = ST_ID3_TFLD_TRACK_NONE_IV1;
		if (pFldI->dataIVal < ST_ID3_TFLD_TRACK_MIN_IV1 ||
				pFldI->dataIVal > ST_ID3_TFLD_TRACK_MAX_IV1)
			pFldI->dataIVal = noneVal;
		break;
	case ST_ID3V1_FID_GENR:
		noneVal = ST_ID3_TFLD_GENRE_NONE_IV1;
		if (pFldI->dataIVal < ST_ID3_TFLD_GENRE_MIN_IV1 ||
				pFldI->dataIVal > ST_ID3_TFLD_GENRE_MAX_IV1)
			pFldI->dataIVal = noneVal;
		break;
	case ST_ID3V1_FID_YEAR:
		noneVal = ST_ID3_TFLD_YEAR_NONE_IV1;
		if (pFldI->dataIVal < ST_ID3_TFLD_YEAR_MIN ||
				pFldI->dataIVal > ST_ID3_TFLD_YEAR_MAX)
			pFldI->dataIVal = noneVal;
		break;
	default:
		noneVal = pFldI->dataIVal;
	}
	pFldI->isOK = (pFldI->dataIVal != noneVal);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const Tst_tagBasics*
st_id3v1_gs_getTag_tbas(const Tst_id3v1_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return &((Tst_id3v1_tag_intn*)pTag->pObInternal)->tbas;
}

const Tst_str*
st_id3v1_gs_getTag_filen(const Tst_id3v1_tag *pTag)
{
	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v1_tag_intn*)pTag->pObInternal)->pFilen;
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_id3v1_gs_getTag_hasErrors(const Tst_id3v1_tag *pTag)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pTag == NULL || pTag->pObInternal == NULL)

	return ((Tst_id3v1_tag_intn*)pTag->pObInternal)->wereCorrd;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v1_gs_getField_artist(const Tst_id3v1_tag *pTag, Tst_mtes_string *pArtist)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pArtist == NULL)

	return ST_TIV1__gs_getTF_s((Tst_id3v1_tag_intn*)pTag->pObInternal,
			ST_ID3V1_FID_ARTI, pArtist);

}

Tst_err
st_id3v1_gs_getField_album(const Tst_id3v1_tag *pTag, Tst_mtes_string *pAlbum)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pAlbum == NULL)

	return ST_TIV1__gs_getTF_s((Tst_id3v1_tag_intn*)pTag->pObInternal,
			ST_ID3V1_FID_ALBU, pAlbum);
}

Tst_err
st_id3v1_gs_getField_title(const Tst_id3v1_tag *pTag, Tst_mtes_string *pTitle)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pTitle == NULL)

	return ST_TIV1__gs_getTF_s((Tst_id3v1_tag_intn*)pTag->pObInternal,
			ST_ID3V1_FID_TITL, pTitle);
}

Tst_err
st_id3v1_gs_getField_comment(const Tst_id3v1_tag *pTag, Tst_mtes_string *pComm)
{
	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pComm == NULL)

	return ST_TIV1__gs_getTF_s((Tst_id3v1_tag_intn*)pTag->pObInternal,
			ST_ID3V1_FID_COMM, pComm);
}

Tst_int32
st_id3v1_gs_getField_tracknr(const Tst_id3v1_tag *pTag)
{
	ST_ASSERTN_NUM(-1, pTag == NULL || pTag->pObInternal == NULL)

	return ST_TIV1__gs_getTF_i((Tst_id3v1_tag_intn*)pTag->pObInternal,
			ST_ID3V1_FID_TRKN);
}

Tst_int32
st_id3v1_gs_getField_year(const Tst_id3v1_tag *pTag)
{
	ST_ASSERTN_NUM(-1, pTag == NULL || pTag->pObInternal == NULL)

	return ST_TIV1__gs_getTF_i((Tst_id3v1_tag_intn*)pTag->pObInternal,
			ST_ID3V1_FID_YEAR);
}

Tst_int32
st_id3v1_gs_getField_genre(const Tst_id3v1_tag *pTag)
{
	ST_ASSERTN_NUM(-1, pTag == NULL || pTag->pObInternal == NULL)

	return ST_TIV1__gs_getTF_i((Tst_id3v1_tag_intn*)pTag->pObInternal,
			ST_ID3V1_FID_GENR);
}

/*----------------------------------------------------------------------------*/

Tst_id3v1_frTp
st_id3v1_gs_getFieldProp_typ(const Tst_id3v1_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_ID3V1_FTP_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_id3v1_fldProp_intn*)pFldPr->pObInternal)->typ;
}

Tst_id3v1_frID
st_id3v1_gs_getFieldProp_id(const Tst_id3v1_fldProp *pFldPr)
{
	ST_ASSERTN_NUM(ST_ID3V1_FID_NONE,
			pFldPr == NULL || pFldPr->pObInternal == NULL)

	return ((Tst_id3v1_fldProp_intn*)pFldPr->pObInternal)->id;
}

const Tst_str*
st_id3v1_gs_getFieldProp_idStr(const Tst_id3v1_fldProp *pFldPr)
{
	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	return (const Tst_str*)((Tst_id3v1_fldProp_intn*)pFldPr->pObInternal)->pCIDstr;
}

const Tst_str*
st_id3v1_gs_getFieldProp_caption(const Tst_id3v1_fldProp *pFldPr)
{
	ST_ASSERTN_NULL(pFldPr == NULL || pFldPr->pObInternal == NULL)

	return (const Tst_str*)((Tst_id3v1_fldProp_intn*)pFldPr->pObInternal)->pCCap;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_id3v1_fldProp*
st_id3v1_ite_nextTagFldProp(ST_OPTARG(Tst_id3v1_fldProp *pFldPrLast),
		Tst_id3v1_fldProp *pFldPrNext)
{
	return ST_TIV1__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_TRUE, pFldPrNext);
}

Tst_id3v1_fldProp*
st_id3v1_ite_nextTagFldProp_all(ST_OPTARG(Tst_id3v1_fldProp *pFldPrLast),
		Tst_id3v1_fldProp *pFldPrNext)
{
	return ST_TIV1__ite_nextTagFldProp(pFldPrLast,
			/*withRestrictions:*/ST_B_FALSE, pFldPrNext);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__gs_mergTF_s(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
		const Tst_id3v1_frID dstID, const Tst_mtes_string *pStr)
{
	Tst_err res;
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_IARG(pTagI == NULL || pStr == NULL)

	switch (dstID) {
	case ST_ID3V1_FID_TITL:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdTit.pObInternal;
		break;
	case ST_ID3V1_FID_ALBU:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdAlb.pObInternal;
		break;
	case ST_ID3V1_FID_ARTI:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdArt.pObInternal;
		break;
	case ST_ID3V1_FID_COMM:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdCom.pObInternal;
		break;
	default:
		ST_ASSERTN_IARG(dstID != ST_ID3V1_FID_TITL &&
				dstID != ST_ID3V1_FID_ALBU && dstID != ST_ID3V1_FID_ARTI &&
				dstID != ST_ID3V1_FID_COMM)  /* cause exit() here */
		return ST_ERR_IARG;
	}
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	pFldI->isOK = ST_B_FALSE;

	res = st_mtes_strCpy(pStr, &pFldI->dataStr);
	if (res != ST_ERR_SUCC)
		return res;
	/* strip string */
	res = st_id3v1_fnc_stripStr(&pFldI->dataStr, NULL, NULL, NULL);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	pFldI->isOK = ! st_mtes_isStrEmptyCh(&pFldI->dataStr);
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v1_d_fdebStr(&pTagI->opts, 3, pFnc, &pFldI->fldPrI,
				&pFldI->dataStr, "");
	return ST_ERR_SUCC;
}

static Tst_err
ST_TIV1__gs_mergTF_i(const char *pFnc, Tst_id3v1_tag_intn *pTagI,
		const Tst_id3v1_frID dstID, Tst_int32 ival)
{
	Tst_int32 noneVal = -1;
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_IARG(pTagI == NULL)

	switch (dstID) {
	case ST_ID3V1_FID_TRKN:
		pFldI   = (Tst_id3v1_fldData_intn*)pTagI->fdTrk.pObInternal;
		noneVal = ST_ID3_TFLD_TRACK_NONE_IV1;
		if (ival < ST_ID3_TFLD_TRACK_MIN_IV1 ||
				ival > ST_ID3_TFLD_TRACK_MAX_IV1)
			ival = noneVal;
		break;
	case ST_ID3V1_FID_GENR:
		pFldI   = (Tst_id3v1_fldData_intn*)pTagI->fdGen.pObInternal;
		noneVal = ST_ID3_TFLD_GENRE_NONE_IV1;
		if (ival < ST_ID3_TFLD_GENRE_MIN_IV1 ||
				ival > ST_ID3_TFLD_GENRE_MAX_IV1)
			ival = noneVal;
		break;
	case ST_ID3V1_FID_YEAR:
		pFldI   = (Tst_id3v1_fldData_intn*)pTagI->fdYea.pObInternal;
		noneVal = ST_ID3_TFLD_YEAR_NONE_IV1;
		if (ival < ST_ID3_TFLD_YEAR_MIN || ival > ST_ID3_TFLD_YEAR_MAX)
			ival = noneVal;
		break;
	default:
		ST_ASSERTN_IARG(dstID != ST_ID3V1_FID_TRKN &&
				dstID != ST_ID3V1_FID_GENR &&
				dstID != ST_ID3V1_FID_YEAR)  /* cause exit() here */
		return ST_ERR_IARG;
	}
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	pFldI->dataIVal = ival;
	pFldI->isOK     = (ival != noneVal);
	/* */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v1_d_fdeb(&pTagI->opts, 3, pFnc, &pFldI->fldPrI,
				"%d%s", pFldI->dataIVal,
				(pFldI->dataIVal == noneVal ? " (==none)" : ""));
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV1__gs_getTF_s(Tst_id3v1_tag_intn *pTagI,
		const Tst_id3v1_frID srcID, Tst_mtes_string *pStr)
{
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_IARG(pTagI == NULL)

	switch (srcID) {
	case ST_ID3V1_FID_TITL:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdTit.pObInternal;
		break;
	case ST_ID3V1_FID_ALBU:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdAlb.pObInternal;
		break;
	case ST_ID3V1_FID_ARTI:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdArt.pObInternal;
		break;
	case ST_ID3V1_FID_COMM:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdCom.pObInternal;
		break;
	default:
		ST_ASSERTN_IARG(srcID != ST_ID3V1_FID_TITL &&
				srcID != ST_ID3V1_FID_ALBU && srcID != ST_ID3V1_FID_ARTI &&
				srcID != ST_ID3V1_FID_COMM)  /* cause exit() here */
		return ST_ERR_IARG;
	}
	ST_ASSERTN_NUM(ST_ERR_FAIL, pFldI == NULL)

	return st_mtes_strCpy(&pFldI->dataStr, pStr);
}

static Tst_int32
ST_TIV1__gs_getTF_i(Tst_id3v1_tag_intn *pTagI, const Tst_id3v1_frID srcID)
{
	Tst_id3v1_fldData_intn *pFldI = NULL;

	ST_ASSERTN_NUM(-1, pTagI == NULL)

	switch (srcID) {
	case ST_ID3V1_FID_TRKN:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdTrk.pObInternal;
		break;
	case ST_ID3V1_FID_GENR:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdGen.pObInternal;
		break;
	case ST_ID3V1_FID_YEAR:
		pFldI = (Tst_id3v1_fldData_intn*)pTagI->fdYea.pObInternal;
		break;
	default:
		ST_ASSERTN_NUM(-1, srcID != ST_ID3V1_FID_TRKN &&
				srcID != ST_ID3V1_FID_GENR &&
				srcID != ST_ID3V1_FID_YEAR)  /* cause exit() here */
		return -1;
	}
	ST_ASSERTN_NUM(-1, pFldI == NULL)

	return pFldI->dataIVal;
}

/*----------------------------------------------------------------------------*/

static Tst_id3v1_fldProp*
ST_TIV1__ite_nextTagFldProp(ST_OPTARG(Tst_id3v1_fldProp *pFldPrLast),
		const Tst_bool withRestrictions, Tst_id3v1_fldProp *pFldPrNext)
{
	Tst_uint32 z       = 0;
	Tst_bool   lastFnd = ST_B_TRUE;
	Tst_id3v1_frID         lastID    = ST_ID3V1_FID_NONE;
	Tst_id3v1ext_frID      lastIDext = ST_ID3V1E_FID_NONE;
	Tst_id3v1_fldProp_intn *pFldPrI;

	ST_ASSERTN_NULL(pFldPrNext == NULL)

	if (pFldPrLast != NULL && pFldPrLast->pObInternal != NULL) {
		pFldPrI   = (Tst_id3v1_fldProp_intn*)pFldPrLast->pObInternal;
		lastID    = pFldPrI->id;
		lastIDext = pFldPrI->idExt;
		lastFnd   = ST_B_FALSE;
	}
	pFldPrNext->pObInternal = NULL;

	while (ST_B_TRUE) {
		pFldPrI = (Tst_id3v1_fldProp_intn*)&ST_ID3V1_FLDPROPS[z++];

		if ((pFldPrI->id == ST_ID3V1_FID_NONE &&
					pFldPrI->idExt == ST_ID3V1E_FID_NONE) ||
				pFldPrI->pCIDstr == NULL || pFldPrI->pCCap == NULL)
			break;
		if (withRestrictions && ! pFldPrI->retFromIte)
			continue;
		if (! lastFnd) {
			if (pFldPrI->id == lastID || pFldPrI->idExt == lastIDext)
				lastFnd = ST_B_TRUE;
			continue;
		}
		pFldPrNext->pObInternal = pFldPrI;
		return pFldPrNext;
	}
	return NULL;
}

/******************************************************************************/
