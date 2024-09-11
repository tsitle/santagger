/******************************************************************************/
/* tag_id3v1_stcs.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for ID3v1 (Extended) Tag structures                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.05.2005 (dd.mm.yyyy)                                     */
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
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_id3v1.h"
/** */
#define SRC_TAG_ID3V1_STCS_C
#include "tag_id3v1_stcs-prot.h"
/*** */
#include "tag_id3v1-prot.h"
#include "tag_id3v1_fnc-prot.h"
#undef SRC_TAG_ID3V1_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_TIV1__stc_rsetOpts(Tst_id3v1_opts *pOpts);
/** */
static void
ST_TIV1__stc_rsetFDat_spec(Tst_id3v1_fldData *pDat,
                           const Tst_id3v1_frID id,
                           const Tst_id3v1ext_frID idExt);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_id3v1_tag */
Tst_err
st_id3v1_stc_initTag(Tst_id3v1_tag *pTag, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_id3v1_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pFilen == NULL)

	ST_CALLOC(pTagI, Tst_id3v1_tag_intn*,
			1, sizeof(Tst_id3v1_tag_intn))
	if (pTagI == NULL)
		return ST_ERR_OMEM;
	pTag->pObInternal = pTagI;

	pTagI->pFilen = NULL;
	res           = st_sysStrcpy(pFilen, &pTagI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	res = st_id3v1_stc_initFDat(&pTagI->fdArt);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdAlb);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdTit);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdCom);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdTrk);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdYea);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdGen);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdExtArt);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdExtAlb);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdExtTit);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdExtGen);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdExtTiS);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdExtTiE);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFDat(&pTagI->fdExtSpd);
	/* */
	if (res == ST_ERR_SUCC)
		res = st_tagBas_stc_initTB(&pTagI->tbas);
	if (res != ST_ERR_SUCC)
		return res;
	st_id3v1_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_id3v1_stc_rsetTag(Tst_id3v1_tag *pTag)
{
	st_id3v1_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_id3v1_stc_rsetTag_prot(Tst_id3v1_tag *pTag, const Tst_bool resetOpts,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetStrwr, Tst_streamwr *pStrwr,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_id3v1_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v1_tag_intn*)pTag->pObInternal;

	if (resetStrrd)
		pTagI->pStrrd = pStrrd;
	if (resetStrwr)
		pTagI->pStrwr = pStrwr;
	if (resetFilen) {
		ST_ASSERTN_VOID(pFilen == NULL)

		if (st_sysStrcpy(pFilen, &pTagI->pFilen) != ST_ERR_SUCC) {
			pTagI->pFilen = NULL;  /* this is no good */

			ST_ASSERTN_VOID(pTagI->pFilen == NULL)  /* cause exit() here */
		}
	}
	st_tagBas_stc_rsetTB(&pTagI->tbas);
	/* */
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdArt,
			ST_ID3V1_FID_ARTI, ST_ID3V1E_FID_NONE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdAlb,
			ST_ID3V1_FID_ALBU, ST_ID3V1E_FID_NONE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdTit,
			ST_ID3V1_FID_TITL, ST_ID3V1E_FID_NONE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdCom,
			ST_ID3V1_FID_COMM, ST_ID3V1E_FID_NONE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdTrk,
			ST_ID3V1_FID_TRKN, ST_ID3V1E_FID_NONE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdYea,
			ST_ID3V1_FID_YEAR, ST_ID3V1E_FID_NONE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdGen,
			ST_ID3V1_FID_GENR, ST_ID3V1E_FID_NONE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdExtArt,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_EARTI);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdExtAlb,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_EALBU);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdExtTit,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_ETITL);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdExtGen,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_EGENR);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdExtTiS,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_TIMES);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdExtTiE,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_TIMEE);
	ST_TIV1__stc_rsetFDat_spec(&pTagI->fdExtSpd,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_SPEED);
	pTagI->hasExtTag = ST_B_FALSE;
	/* */
	pTagI->wereCorrd = ST_B_FALSE;

	if (resetOpts)
		ST_TIV1__stc_rsetOpts(&pTagI->opts);
}

void
st_id3v1_stc_freeTag(Tst_id3v1_tag *pTag)
{
	Tst_id3v1_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL)

	pTagI = (Tst_id3v1_tag_intn*)pTag->pObInternal;

	if (pTagI != NULL) {
		st_id3v1_stc_rsetTag_prot(pTag, ST_B_TRUE,
				ST_B_TRUE, NULL,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		/* */
		st_id3v1_stc_freeFDat(&pTagI->fdArt);
		st_id3v1_stc_freeFDat(&pTagI->fdAlb);
		st_id3v1_stc_freeFDat(&pTagI->fdTit);
		st_id3v1_stc_freeFDat(&pTagI->fdCom);
		st_id3v1_stc_freeFDat(&pTagI->fdTrk);
		st_id3v1_stc_freeFDat(&pTagI->fdYea);
		st_id3v1_stc_freeFDat(&pTagI->fdGen);
		st_id3v1_stc_freeFDat(&pTagI->fdExtArt);
		st_id3v1_stc_freeFDat(&pTagI->fdExtAlb);
		st_id3v1_stc_freeFDat(&pTagI->fdExtTit);
		st_id3v1_stc_freeFDat(&pTagI->fdExtGen);
		st_id3v1_stc_freeFDat(&pTagI->fdExtTiS);
		st_id3v1_stc_freeFDat(&pTagI->fdExtTiE);
		st_id3v1_stc_freeFDat(&pTagI->fdExtSpd);
		/* */
		st_tagBas_stc_freeTB(&pTagI->tbas);
		ST_DELPOINT(pTagI->pFilen)
		/* */
		ST_DELPOINT(pTag->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v1_fldData */
Tst_err
st_id3v1_stc_initFDat(Tst_id3v1_fldData *pDat)
{
	Tst_err res;
	Tst_id3v1_fldData_intn *pDatI;

	ST_ASSERTN_IARG(pDat == NULL)

	ST_CALLOC(pDatI, Tst_id3v1_fldData_intn*,
			1, sizeof(Tst_id3v1_fldData_intn))
	if (pDatI == NULL)
		return ST_ERR_OMEM;
	pDat->pObInternal = pDatI;

	res = st_mtes_stc_initStr(&pDatI->dataStr);
	if (res == ST_ERR_SUCC)
		res = st_id3v1_stc_initFPr_i(&pDatI->fldPrI);
	if (res != ST_ERR_SUCC)
		return res;

	st_id3v1_stc_rsetFDat(pDat);
	return ST_ERR_SUCC;
}

void
st_id3v1_stc_rsetFDat(Tst_id3v1_fldData *pDat)
{
	Tst_id3v1_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL || pDat->pObInternal == NULL)

	pDatI = (Tst_id3v1_fldData_intn*)pDat->pObInternal;

	/* data */
	pDatI->dataIVal = -1;
	/* data/attributes */
	st_id3v1_stc_rsetFDat_onlyBufs(pDat);

	/* */
	st_id3v1_stc_rsetFPr_i(&pDatI->fldPrI);
	pDatI->wereCorrd = ST_B_FALSE;
	pDatI->isOK      = ST_B_FALSE;
}

void
st_id3v1_stc_rsetFDat_onlyBufs(Tst_id3v1_fldData *pDat)
{
	Tst_id3v1_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL || pDat->pObInternal == NULL)

	pDatI = (Tst_id3v1_fldData_intn*)pDat->pObInternal;

	/* data */
	st_mtes_stc_rsetStr(&pDatI->dataStr, ST_B_TRUE, ST_B_TRUE);
}

void
st_id3v1_stc_freeFDat(Tst_id3v1_fldData *pDat)
{
	Tst_id3v1_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL)

	if (pDat->pObInternal != NULL) {
		st_id3v1_stc_rsetFDat(pDat);

		pDatI = (Tst_id3v1_fldData_intn*)pDat->pObInternal;
		/* */
		st_id3v1_stc_freeFPr_i(&pDatI->fldPrI);
		/* */
		st_mtes_stc_freeStr(&pDatI->dataStr);
		ST_DELPOINT(pDat->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v1_fldProp */
void
st_id3v1_stc_rsetFPr(Tst_id3v1_fldProp *pFldPr)
{
	ST_ASSERTN_VOID(pFldPr == NULL)

	pFldPr->pObInternal = NULL;
}

/* Tst_id3v1_fldProp_intn */
Tst_err
st_id3v1_stc_initFPr_i(Tst_id3v1_fldProp_intn *pFldPrI)
{
	ST_ASSERTN_IARG(pFldPrI == NULL)

	st_id3v1_stc_rsetFPr_i(pFldPrI);
	return ST_ERR_SUCC;
}

void
st_id3v1_stc_rsetFPr_i(Tst_id3v1_fldProp_intn *pFldPrI)
{
	ST_ASSERTN_VOID(pFldPrI == NULL)

	pFldPrI->pCIDstr    = NULL;
	pFldPrI->pCCap      = NULL;
	pFldPrI->id         = ST_ID3V1_FID_NONE;
	pFldPrI->idExt      = ST_ID3V1E_FID_NONE;
	pFldPrI->typ        = ST_ID3V1_FTP_NONE;
	pFldPrI->retFromIte = ST_B_FALSE;
}

void
st_id3v1_stc_freeFPr_i(Tst_id3v1_fldProp_intn *pFldPrI)
{
	ST_ASSERTN_VOID(pFldPrI == NULL)

	st_id3v1_stc_rsetFPr_i(pFldPrI);
}

Tst_err
st_id3v1_stc_copyFPr_i(const Tst_id3v1_fldProp_intn *pSrcI,
		Tst_id3v1_fldProp_intn *pDstI)
{
	ST_ASSERTN_IARG(pSrcI == NULL || pDstI == NULL ||
			pSrcI == pDstI)

	st_id3v1_stc_rsetFPr_i(pDstI);
	/* */
	pDstI->pCIDstr    = pSrcI->pCIDstr;
	pDstI->pCCap      = pSrcI->pCCap;
	pDstI->id         = pSrcI->id;
	pDstI->idExt      = pSrcI->idExt;
	pDstI->typ        = pSrcI->typ;
	pDstI->retFromIte = pSrcI->retFromIte;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_id3v1_opts */
static void
ST_TIV1__stc_rsetOpts(Tst_id3v1_opts *pOpts)
{
	ST_ASSERTN_VOID(pOpts == NULL)

	ST_RSETSTC_BASOPTS(pOpts->basOpts)
	pOpts->rdOnlyBasics = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

static void
ST_TIV1__stc_rsetFDat_spec(Tst_id3v1_fldData *pDat,
		const Tst_id3v1_frID id, const Tst_id3v1ext_frID idExt)
{
	Tst_bool   isInArr;
	Tst_uint32 ixArr = 0;
	Tst_id3v1_fldData_intn *pFldI;

	st_id3v1_stc_rsetFDat(pDat);

	pFldI = (Tst_id3v1_fldData_intn*)pDat->pObInternal;

	/* try to find field properties entry */
	if (id != ST_ID3V1_FID_NONE)
		isInArr = st_id3v1_fnc_isFldPropInArr_byEnum(ST_ID3V1_FLDPROPS, id,
				ST_ID3V1E_FID_NONE, &ixArr);
	else
		isInArr = st_id3v1_fnc_isFldPropInArr_byEnum(ST_ID3V1_FLDPROPS,
				ST_ID3V1_FID_NONE, idExt, &ixArr);
	if (! isInArr) {
		ST_ASSERTN_VOID(! isInArr)
		return;
	}

	/* now we copy the original field props */
	st_id3v1_stc_copyFPr_i(&ST_ID3V1_FLDPROPS[ixArr], &pFldI->fldPrI);
}

/******************************************************************************/
