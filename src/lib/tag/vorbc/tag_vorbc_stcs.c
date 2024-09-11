/******************************************************************************/
/* tag_vorbc_stcs.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Vorbis Tag structures                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_vorbc.h"
#include "src/includes/tag/tag_basics.h"
/** */
#define SRC_TAG_VORBC_STCS_C
#include "tag_vorbc_stcs-prot.h"
#undef SRC_TAG_VORBC_STCS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** */
static Tst_err
ST_TVORBC__stc_initFlds(Tst_vorbc_flds *pFlds);
static void
ST_TVORBC__stc_rsetFlds(Tst_vorbc_flds *pFlds);
static void
ST_TVORBC__stc_freeFlds(Tst_vorbc_flds *pFlds);
/** */
static Tst_err
ST_TVORBC__stc_initFAttrPic(Tst_vorbc_fldAttrPict *pAttrP);
static void
ST_TVORBC__stc_rsetFAttrPic(Tst_vorbc_fldAttrPict *pAttrP);
static void
ST_TVORBC__stc_freeFAttrPic(Tst_vorbc_fldAttrPict *pAttrP);
/** */
static void
ST_TVORBC__stc_rsetOpts(Tst_vorbc_opts *pOpts);
/** */
static void
ST_TVORBC__stc_cbRsetFld(void *pElem);
static void
ST_TVORBC__stc_cbFreeFld(void *pElem);
/** */
static Tst_err
ST_TVORBC__copyFldProps_str(ST_OPTARG(const char *pStrS1),
                            ST_OPTARG(const char *pStrS2),
                            char **ppStrD, const Tst_uint32 maxlen);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_vorbc_tag */
Tst_err
st_vorbc_stc_initTag(Tst_vorbc_tag *pTag, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_vorbc_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pFilen == NULL)

	ST_CALLOC(pTagI, Tst_vorbc_tag_intn*,
			1, sizeof(Tst_vorbc_tag_intn))
	if (pTagI == NULL)
		return ST_ERR_OMEM;
	pTag->pObInternal = pTagI;

	pTagI->pFilen = NULL;
	res           = st_sysStrcpy(pFilen, &pTagI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	res = st_binobj_stc_initBO(&pTagI->rawTagBDat);
	if (res == ST_ERR_SUCC)
		res = ST_TVORBC__stc_initFlds(&pTagI->flds);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&pTagI->vendor);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&pTagI->tagger);
	if (res == ST_ERR_SUCC)
		res = st_tagBas_stc_initTB(&pTagI->tbas);
	if (res != ST_ERR_SUCC)
		return res;

	st_vorbc_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_vorbc_stc_rsetTag(Tst_vorbc_tag *pTag)
{
	st_vorbc_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_vorbc_stc_rsetTag_prot(Tst_vorbc_tag *pTag, const Tst_bool resetOpts,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetStrwr, Tst_streamwr *pStrwr,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_vorbc_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

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
	st_binobj_stc_rsetBO(&pTagI->rawTagBDat);
	/* */
	st_mtes_stc_rsetStr(&pTagI->vendor, ST_B_TRUE, ST_B_TRUE);
	st_mtes_stc_rsetStr(&pTagI->tagger, ST_B_TRUE, ST_B_TRUE);
	/* */
	ST_TVORBC__stc_rsetFlds(&pTagI->flds);
	/* */
	pTagI->wasEmbedded = ST_B_FALSE;
	/* */
	if (resetOpts)
		ST_TVORBC__stc_rsetOpts(&pTagI->opts);
}

void
st_vorbc_stc_freeTag(Tst_vorbc_tag *pTag)
{
	Tst_vorbc_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL)

	if (pTag->pObInternal != NULL) {
		st_vorbc_stc_rsetTag_prot(pTag, ST_B_TRUE,
				ST_B_TRUE, NULL,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		/* */
		pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;
		/* */
		st_mtes_stc_freeStr(&pTagI->vendor);
		st_mtes_stc_freeStr(&pTagI->tagger);
		/* */
		ST_TVORBC__stc_freeFlds(&pTagI->flds);
		/* */
		st_tagBas_stc_freeTB(&pTagI->tbas);
		ST_DELPOINT(pTagI->pFilen)
		st_binobj_stc_freeBO(&pTagI->rawTagBDat);
		/* */
		ST_DELPOINT(pTag->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_vorbc_fldData */
Tst_err
st_vorbc_stc_initFDat(Tst_vorbc_fldData *pDat)
{
	Tst_err res;
	Tst_vorbc_fldData_intn *pDatI;

	ST_ASSERTN_IARG(pDat == NULL)

	ST_CALLOC(pDatI, Tst_vorbc_fldData_intn*,
			1, sizeof(Tst_vorbc_fldData_intn))
	if (pDatI == NULL)
		return ST_ERR_OMEM;
	pDat->pObInternal = pDatI;

	res = st_mtes_stc_initSA(&pDatI->dataSArr);
	if (res == ST_ERR_SUCC)
		res = st_binobj_stc_initBO(&pDatI->dataPicBDat);
	if (res == ST_ERR_SUCC)
		res = ST_TVORBC__stc_initFAttrPic(&pDatI->attrPic);
	if (res != ST_ERR_SUCC)
		return res;

	st_vorbc_stc_initFPr_i(&pDatI->fldPrI);

	st_vorbc_stc_rsetFDat(pDat);
	return ST_ERR_SUCC;
}

void
st_vorbc_stc_rsetFDat(Tst_vorbc_fldData *pDat)
{
	Tst_vorbc_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL || pDat->pObInternal == NULL)

	pDatI = (Tst_vorbc_fldData_intn*)pDat->pObInternal;

	st_vorbc_stc_rsetFDat_onlyBufs(pDat);
	/* */
	st_vorbc_stc_rsetFPr_i(&pDatI->fldPrI);
	/* */
	pDatI->isOK     = ST_B_FALSE;
	pDatI->fnr      = 0;
	pDatI->uniqueIx = 0;
	/* */
	pDatI->wasRendered = ST_B_FALSE;
}

void
st_vorbc_stc_rsetFDat_onlyBufs(Tst_vorbc_fldData *pDat)
{
	Tst_vorbc_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL || pDat->pObInternal == NULL)

	pDatI = (Tst_vorbc_fldData_intn*)pDat->pObInternal;

	st_mtes_stc_rsetSA(&pDatI->dataSArr);
	st_binobj_stc_rsetBO(&pDatI->dataPicBDat);
	ST_TVORBC__stc_rsetFAttrPic(&pDatI->attrPic);
}

void
st_vorbc_stc_freeFDat(Tst_vorbc_fldData *pDat)
{
	Tst_vorbc_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL)

	if (pDat->pObInternal != NULL) {
		st_vorbc_stc_rsetFDat(pDat);
		/* */
		pDatI = (Tst_vorbc_fldData_intn*)pDat->pObInternal;
		st_mtes_stc_freeSA(&pDatI->dataSArr);
		st_binobj_stc_freeBO(&pDatI->dataPicBDat);
		ST_TVORBC__stc_freeFAttrPic(&pDatI->attrPic);
		st_vorbc_stc_freeFPr_i(&pDatI->fldPrI);
		/* */
		ST_DELPOINT(pDat->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_vorbc_fldProp */
void
st_vorbc_stc_rsetFPr(Tst_vorbc_fldProp *pFPr)
{
	ST_ASSERTN_VOID(pFPr == NULL)

	pFPr->pObInternal = NULL;
}

/* Tst_vorbc_fldProp_intn */
void
st_vorbc_stc_initFPr_i(Tst_vorbc_fldProp_intn *pFPrI)
{
	ST_ASSERTN_VOID(pFPrI == NULL)

	pFPrI->pVIDstr = NULL;
	st_vorbc_stc_rsetFPr_i(pFPrI);
}

void
st_vorbc_stc_rsetFPr_i(Tst_vorbc_fldProp_intn *pFPrI)
{
	ST_ASSERTN_VOID(pFPrI == NULL)

	pFPrI->pCIDstr = NULL;
	pFPrI->pCCap   = NULL;
	ST_DELPOINT(pFPrI->pVIDstr)
	pFPrI->id         = ST_VORBC_FID_NONE;
	pFPrI->typ        = ST_VORBC_FTP_NONE;
	pFPrI->retFromIte = ST_B_FALSE;
}

Tst_err
st_vorbc_stc_copyFPr_i(ST_OPTARG(const Tst_str *pSrcFKey),
		const Tst_vorbc_fldProp_intn *pSrcI,
		Tst_vorbc_fldProp_intn *pDstI)
{
	Tst_err res;

	st_vorbc_stc_rsetFPr_i(pDstI);
	/* */
	pDstI->pCIDstr = pSrcI->pCIDstr;
	pDstI->pCCap   = pSrcI->pCCap;
	/* */
	res = ST_TVORBC__copyFldProps_str(
			(pSrcFKey == NULL ? pSrcI->pVIDstr : (const char*)pSrcFKey),
			pSrcI->pCIDstr,
			&pDstI->pVIDstr, 1024);
	if (res != ST_ERR_SUCC)
		return res;
	st_sysStrToUpper((Tst_str*)pDstI->pVIDstr);
	/* */
	if (pSrcFKey != NULL) {
		Tst_uint32 slen;

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

/* Tst_vorbc_fldd_dattp */
void
st_vorbc_stc_rsetDatTp(Tst_vorbc_fldd_dattp *pDatTp)
{
	ST_ASSERTN_VOID(pDatTp == NULL)

	pDatTp->hasText = ST_B_FALSE;
	pDatTp->hasBDat = ST_B_FALSE;
	pDatTp->hasPict = ST_B_FALSE;
}

/* Tst_vorbc_fldd_strtp */
void
st_vorbc_stc_rsetStrTp(Tst_vorbc_fldd_strtp *pStrTp)
{
	ST_ASSERTN_VOID(pStrTp == NULL)

	pStrTp->hasNumStr    = ST_B_FALSE;
	pStrTp->hasTStampStr = ST_B_FALSE;
	pStrTp->hasGenreStr  = ST_B_FALSE;
	pStrTp->hasSimpleStr = ST_B_FALSE;
	pStrTp->hasFullStr   = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_vorbc_flds */
static Tst_err
ST_TVORBC__stc_initFlds(Tst_vorbc_flds *pFlds)
{
	Tst_err res;

	ST_ASSERTN_IARG(pFlds == NULL)

	res = st_dynlist_stc_initDL(&pFlds->list, sizeof(Tst_vorbc_fldData),
			ST_TVORBC__stc_cbRsetFld, ST_TVORBC__stc_cbFreeFld);
	if (res != ST_ERR_SUCC)
		return res;
	ST_TVORBC__stc_rsetFlds(pFlds);
	return ST_ERR_SUCC;
}

static void
ST_TVORBC__stc_rsetFlds(Tst_vorbc_flds *pFlds)
{
	ST_ASSERTN_VOID(pFlds == NULL)

	st_dynlist_stc_rsetDL(&pFlds->list);
	pFlds->skipCnt      = 0;
	pFlds->uniqueIxLast = 1;
}

static void
ST_TVORBC__stc_freeFlds(Tst_vorbc_flds *pFlds)
{
	ST_ASSERTN_VOID(pFlds == NULL)

	ST_TVORBC__stc_rsetFlds(pFlds);
	st_dynlist_stc_freeDL(&pFlds->list);
}

/*----------------------------------------------------------------------------*/

/* Tst_vorbc_fldAttrPict */
static Tst_err
ST_TVORBC__stc_initFAttrPic(Tst_vorbc_fldAttrPict *pAttrP)
{
	Tst_err res;

	ST_ASSERTN_IARG(pAttrP == NULL)

	pAttrP->pMime = NULL;
	/* */
	res = st_mtes_stc_initStr(&pAttrP->desc);
	if (res != ST_ERR_SUCC)
		return res;

	ST_TVORBC__stc_rsetFAttrPic(pAttrP);
	return ST_ERR_SUCC;
}

static void
ST_TVORBC__stc_rsetFAttrPic(Tst_vorbc_fldAttrPict *pAttrP)
{
	ST_ASSERTN_VOID(pAttrP == NULL)

	pAttrP->ptp    = ST_VORBC_PT_OTHER;
	pAttrP->imgWid = 0;
	pAttrP->imgHei = 0;
	pAttrP->imgDep = 0;
	pAttrP->imgCUs = 0;
	ST_DELPOINT(pAttrP->pMime)
	st_mtes_stc_rsetStr(&pAttrP->desc, ST_B_TRUE, ST_B_TRUE);
}

static void
ST_TVORBC__stc_freeFAttrPic(Tst_vorbc_fldAttrPict *pAttrP)
{
	ST_ASSERTN_VOID(pAttrP == NULL)

	ST_TVORBC__stc_rsetFAttrPic(pAttrP);
	/* */
	st_mtes_stc_freeStr(&pAttrP->desc);
}

/*----------------------------------------------------------------------------*/

/* Tst_vorbc_opts */
static void
ST_TVORBC__stc_rsetOpts(Tst_vorbc_opts *pOpts)
{
	ST_ASSERTN_VOID(pOpts == NULL)

	ST_RSETSTC_BASOPTS(pOpts->basOpts)
	pOpts->rdOnlyBasics = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

static void
ST_TVORBC__stc_cbRsetFld(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_vorbc_stc_rsetFDat((Tst_vorbc_fldData*)pElem);
}

static void
ST_TVORBC__stc_cbFreeFld(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_vorbc_stc_freeFDat((Tst_vorbc_fldData*)pElem);
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TVORBC__copyFldProps_str(ST_OPTARG(const char *pStrS1),
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
