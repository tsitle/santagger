/******************************************************************************/
/* tag_apev2_stcs.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for APEv2 Tag structures                                         */
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
#include "src/includes/common/binobj.h"
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_STCS_C
#include "tag_apev2_stcs-prot.h"
/*** */
#include "tag_apev2-prot.h"
#undef SRC_TAG_APEV2_STCS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void    ST_TAV2__stc_rsetOpts(Tst_apev2_opts *pOpts);
static Tst_err ST_TAV2__stc_initFlds(Tst_apev2_flds *pFlds);
static void    ST_TAV2__stc_rsetFlds(Tst_apev2_flds *pFlds);
static void    ST_TAV2__stc_freeFlds(Tst_apev2_flds *pFlds);
static void    ST_TAV2__stc_cbRsetFld(void *pElem);
static void    ST_TAV2__stc_cbFreeFld(void *pElem);
static Tst_err ST_TAV2__copyFldProps_str(ST_OPTARG(const char *pStrS1),
                                         ST_OPTARG(const char *pStrS2),
                                         char **ppStrD, const Tst_uint32 maxlen);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_apev2_tag */
Tst_err
st_apev2_stc_initTag(Tst_apev2_tag *pTag, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pFilen == NULL)

	ST_CALLOC(pTagI, Tst_apev2_tag_intn*,
			1, sizeof(Tst_apev2_tag_intn))
	if (pTagI == NULL)
		return ST_ERR_OMEM;
	pTag->pObInternal = pTagI;

	pTagI->pFilen = NULL;
	res           = st_sysStrcpy(pFilen, &pTagI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	res = ST_TAV2__stc_initFlds(&pTagI->flds);
	if (res == ST_ERR_SUCC)
		res = st_tagBas_stc_initTB(&pTagI->tbas);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&pTagI->tagger);
	if (res != ST_ERR_SUCC)
		return res;

	st_apev2_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_apev2_stc_rsetTag(Tst_apev2_tag *pTag)
{
	st_apev2_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_apev2_stc_rsetTag_prot(Tst_apev2_tag *pTag, const Tst_bool resetOpts,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetStrwr, Tst_streamwr *pStrwr,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

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
	ST_TAV2__stc_rsetFlds(&pTagI->flds);
	st_mtes_stc_rsetStr(&pTagI->tagger, ST_B_TRUE, ST_B_TRUE);
	if (resetOpts)
		ST_TAV2__stc_rsetOpts(&pTagI->opts);
}

void
st_apev2_stc_freeTag(Tst_apev2_tag *pTag)
{
	Tst_apev2_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL)

	if (pTag->pObInternal != NULL) {
		pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;
		/* */
		st_apev2_stc_rsetTag_prot(pTag, ST_B_TRUE,
				ST_B_TRUE, NULL,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		st_tagBas_stc_freeTB(&pTagI->tbas);
		st_mtes_stc_freeStr(&pTagI->tagger);
		ST_TAV2__stc_freeFlds(&pTagI->flds);
		ST_DELPOINT(pTagI->pFilen)
		/* */
		ST_DELPOINT(pTag->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_apev2_fldData */
Tst_err
st_apev2_stc_initFDat(Tst_apev2_fldData *pFld)
{
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_IARG(pFld == NULL)

	ST_CALLOC(pFldI, Tst_apev2_fldData_intn*,
			1, sizeof(Tst_apev2_fldData_intn))
	if (pFldI == NULL)
		return ST_ERR_OMEM;
	pFld->pObInternal = pFldI;

	st_binobj_stc_initBO(&pFldI->dataBinDat);
	st_mtes_stc_initSA(&pFldI->dataSArr);
	st_apev2_stc_initFPr_i(&pFldI->fldPrI);

	st_apev2_stc_rsetFDat(pFld);
	return ST_ERR_SUCC;
}

void
st_apev2_stc_rsetFDat(Tst_apev2_fldData *pFld)
{
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_VOID(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

	st_apev2_stc_rsetFDat_onlyBufs(pFld);
	/* */
	st_apev2_stc_rsetFPr_i(&pFldI->fldPrI);
	pFldI->isOK     = ST_B_FALSE;
	pFldI->fnr      = 0;
	pFldI->uniqueIx = 0;
	/* */
	pFldI->wasRendered = ST_B_FALSE;
}

void
st_apev2_stc_rsetFDat_onlyBufs(Tst_apev2_fldData *pFld)
{
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_VOID(pFld == NULL || pFld->pObInternal == NULL)

	pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;

	st_binobj_stc_rsetBO(&pFldI->dataBinDat);
	st_mtes_stc_rsetSA(&pFldI->dataSArr);
}

void
st_apev2_stc_freeFDat(Tst_apev2_fldData *pFld)
{
	Tst_apev2_fldData_intn *pFldI;

	ST_ASSERTN_VOID(pFld == NULL)

	if (pFld->pObInternal != NULL) {
		st_apev2_stc_rsetFDat(pFld);

		pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;
		/** */
		st_binobj_stc_freeBO(&pFldI->dataBinDat);
		st_mtes_stc_freeSA(&pFldI->dataSArr);
		st_apev2_stc_freeFPr_i(&pFldI->fldPrI);
		/** */
		ST_DELPOINT(pFld->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_apev2_fldProp */
void
st_apev2_stc_rsetFPr(Tst_apev2_fldProp *pFPr)
{
	ST_ASSERTN_VOID(pFPr == NULL)

	pFPr->pObInternal = NULL;
}

/* Tst_apev2_fldProp_intn */
void
st_apev2_stc_initFPr_i(Tst_apev2_fldProp_intn *pFPrI)
{
	ST_ASSERTN_VOID(pFPrI == NULL)

	pFPrI->pVIDstr = NULL;
	st_apev2_stc_rsetFPr_i(pFPrI);
}

void
st_apev2_stc_rsetFPr_i(Tst_apev2_fldProp_intn *pFPrI)
{
	ST_ASSERTN_VOID(pFPrI == NULL)

	pFPrI->pCIDstr = NULL;
	pFPrI->pCCap   = NULL;
	ST_DELPOINT(pFPrI->pVIDstr)
	pFPrI->id         = ST_APEV2_FID_NONE;
	pFPrI->typ        = ST_APEV2_FTP_NONE;
	pFPrI->retFromIte = ST_B_FALSE;
}

Tst_err
st_apev2_stc_copyFPr_i(ST_OPTARG(const Tst_str *pSrcFKey),
		const Tst_apev2_fldProp_intn *pSrcI,
		Tst_apev2_fldProp_intn *pDstI)
{
	Tst_err res;

	st_apev2_stc_rsetFPr_i(pDstI);
	/* */
	pDstI->pCIDstr = pSrcI->pCIDstr;
	pDstI->pCCap   = pSrcI->pCCap;
	/* */
	res = ST_TAV2__copyFldProps_str(
			(pSrcFKey == NULL ? pSrcI->pVIDstr : (const char*)pSrcFKey),
			pSrcI->pCIDstr,
			&pDstI->pVIDstr, 1024);
	if (res != ST_ERR_SUCC)
		return res;
	st_sysStrToUpper((Tst_str*)pDstI->pVIDstr);
	/* */
	if (pSrcFKey != NULL) {
		Tst_uint32 slen;

		st_sysStrToUpper((Tst_str*)pDstI->pVIDstr);
		slen = st_sysStrlen2(pDstI->pVIDstr);
		if (slen == 0 ||
				! st_mtes_testIfIsASCII((const Tst_str*)pDstI->pVIDstr, slen + 1))
			return ST_ERR_IDAT;  /* invalid data */
	}
	/* */
	pDstI->id         = pSrcI->id;
	pDstI->typ        = pSrcI->typ;
	pDstI->retFromIte = pSrcI->retFromIte;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* Tst_apev2_fldd_dattp */
void
st_apev2_stc_rsetDatTp(Tst_apev2_fldd_dattp *pDatTp)
{
	ST_ASSERTN_VOID(pDatTp == NULL)

	pDatTp->hasText = ST_B_FALSE;
	pDatTp->hasBDat = ST_B_FALSE;
}

/* Tst_apev2_fldd_strtp */
void
st_apev2_stc_rsetStrTp(Tst_apev2_fldd_strtp *pStrTp)
{
	ST_ASSERTN_VOID(pStrTp == NULL)

	pStrTp->hasNumStr    = ST_B_FALSE;
	pStrTp->hasYearStr   = ST_B_FALSE;
	pStrTp->hasTStampStr = ST_B_FALSE;
	pStrTp->hasGenreStr  = ST_B_FALSE;
	pStrTp->hasSimpleStr = ST_B_FALSE;
	pStrTp->hasUrlStr    = ST_B_FALSE;
	pStrTp->hasFullStr   = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_apev2_opts */
static void
ST_TAV2__stc_rsetOpts(Tst_apev2_opts *pOpts)
{
	ST_ASSERTN_VOID(pOpts == NULL)

	ST_RSETSTC_BASOPTS(pOpts->basOpts)
	pOpts->rdOnlyBasics = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/* Tst_apev2_flds */
static Tst_err
ST_TAV2__stc_initFlds(Tst_apev2_flds *pFlds)
{
	Tst_err res;

	ST_ASSERTN_IARG(pFlds == NULL)

	res = st_dynlist_stc_initDL(&pFlds->list, sizeof(Tst_apev2_fldData),
			ST_TAV2__stc_cbRsetFld, ST_TAV2__stc_cbFreeFld);
	if (res != ST_ERR_SUCC)
		return res;
	ST_TAV2__stc_rsetFlds(pFlds);
	return ST_ERR_SUCC;
}

static void
ST_TAV2__stc_rsetFlds(Tst_apev2_flds *pFlds)
{
	ST_ASSERTN_VOID(pFlds == NULL)

	st_dynlist_stc_rsetDL(&pFlds->list);
	pFlds->uniqueIxLast = 1;
	pFlds->skipCnt      = 0;
}

static void
ST_TAV2__stc_freeFlds(Tst_apev2_flds *pFlds)
{
	ST_ASSERTN_VOID(pFlds == NULL)

	ST_TAV2__stc_rsetFlds(pFlds);
	/* */
	st_dynlist_stc_freeDL(&pFlds->list);
}

/*----------------------------------------------------------------------------*/

static void
ST_TAV2__stc_cbRsetFld(void *pElem)
{
	st_apev2_stc_rsetFDat((Tst_apev2_fldData*)pElem);
}

static void
ST_TAV2__stc_cbFreeFld(void *pElem)
{
	st_apev2_stc_freeFDat((Tst_apev2_fldData*)pElem);
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TAV2__copyFldProps_str(ST_OPTARG(const char *pStrS1),
		ST_OPTARG(const char *pStrS2),
		char **ppStrD, const Tst_uint32 maxlen)
{
	Tst_uint32 len;
	char const *pSrc;

	if (pStrS1 != NULL)
		pSrc = pStrS1;
	else
		pSrc = pStrS2;
	len = st_sysStrlen2(pSrc);
	if (len > maxlen)
		len = maxlen;
	ST_REALLOC(*ppStrD, char*, len + 1, 1)
	if (*ppStrD == NULL)
		return ST_ERR_OMEM;
	if (len > 0)
		strncpy(*ppStrD, pSrc, len);
	(*ppStrD)[len] = 0x00;
	return ST_ERR_SUCC;
}

/******************************************************************************/
