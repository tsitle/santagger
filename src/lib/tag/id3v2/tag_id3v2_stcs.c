/******************************************************************************/
/* tag_id3v2_stcs.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Sub functions for ID3v2 Tag structures                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj.h>
#include <santag/common/dynlist.h>
#include <santag/common/string_mte.h>
#include <santag/common/sys_fnc.h>
#include <santag/tag/tag_basics.h>
#include <santag/tag/tag_id3v2.h>
/** */
#define SRC_TAG_ID3V2_STCS_C
#include "tag_id3v2_stcs-prot.h"
/*** */
#include "tag_id3v2_fnc-prot.h"
#undef SRC_TAG_ID3V2_STCS_C

/*
// System-Includes
*/
#include <string.h>      /* memset() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** */
static Tst_err
ST_TIV2__stc_initFDatArr(Tst_id3v2_fields *pFlds);

static void
ST_TIV2__stc_rsetFDatArr(Tst_id3v2_fields *pFlds);

static void
ST_TIV2__stc_freeFDatArr(Tst_id3v2_fields *pFlds);
/** */
static void
ST_TIV2__stc_rsetOpts(Tst_id3v2_opts *pOpts);
/** */
static Tst_err
ST_TIV2__stc_copyFPr_strC(const char *pStrS1, const char *pStrS2,
                          char *pStrD, const Tst_uint32 maxlen);
/** */
static void
ST_TIV2__stc_makeUniqueCustIDstr(Tst_str *pIDstr);
/** */
static void
ST_TIV2__stc_cbRsetFld(void *pElem);

static void
ST_TIV2__stc_cbFreeFld(void *pElem);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_id3v2_tag */
Tst_err
st_id3v2_stc_initTag(Tst_id3v2_tag *pTag, const Tst_str *pFilen)
{
	Tst_err res;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pFilen == NULL)

	ST_CALLOC(pTagI, Tst_id3v2_tag_intn*,
			1, sizeof(Tst_id3v2_tag_intn))
	if (pTagI == NULL)
		return ST_ERR_OMEM;
	pTag->pObInternal = pTagI;

	pTagI->pFilen = NULL;
	res           = st_sysStrcpy(pFilen, &pTagI->pFilen);
	if (res != ST_ERR_SUCC)
		return res;

	res = ST_TIV2__stc_initFDatArr(&pTagI->flds);
	if (res == ST_ERR_SUCC)
		res = st_tagBas_stc_initTB(&pTagI->tbas);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&pTagI->tagger);
	if (res != ST_ERR_SUCC)
		return res;

	st_id3v2_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_FALSE, NULL);
	return ST_ERR_SUCC;
}

void
st_id3v2_stc_rsetTag(Tst_id3v2_tag *pTag)
{
	st_id3v2_stc_rsetTag_prot(pTag, ST_B_TRUE,
			ST_B_TRUE, NULL,
			ST_B_TRUE, NULL,
			ST_B_TRUE, (const Tst_str*)"-none-");
}

void
st_id3v2_stc_rsetTag_prot(Tst_id3v2_tag *pTag,
		const Tst_bool resetOpts,
		const Tst_bool resetStrrd, Tst_streamrd *pStrrd,
		const Tst_bool resetStrwr, Tst_streamwr *pStrwr,
		const Tst_bool resetFilen, const Tst_str *pFilen)
{
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	/* */
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
	/* */
	st_tagBas_stc_rsetTB(&pTagI->tbas);
	/* */
	st_id3v2_stc_rsetTHdF(&pTagI->flagsTHd);
	ST_TIV2__stc_rsetFDatArr(&pTagI->flds);
	st_id3v2_stc_rsetTagErrs(&pTagI->tgErrs);
	/* */
	pTagI->verMaj  = 0;
	pTagI->verMin  = 0;
	pTagI->ehdSz   = 0;
	pTagI->paddLen = 0;
	/* */
	if (resetOpts)
		ST_TIV2__stc_rsetOpts(&pTagI->opts);
	/* */
	pTagI->tgErrCnt = 0;
	/* */
	st_mtes_stc_rsetStr(&pTagI->tagger, ST_B_TRUE, ST_B_TRUE);
}

void
st_id3v2_stc_freeTag(Tst_id3v2_tag *pTag)
{
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_VOID(pTag == NULL)

	if (pTag->pObInternal != NULL) {
		st_id3v2_stc_rsetTag_prot(pTag, ST_B_TRUE,
				ST_B_TRUE, NULL,
				ST_B_TRUE, NULL,
				ST_B_FALSE, NULL);
		/* */
		pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
		st_tagBas_stc_freeTB(&pTagI->tbas);
		st_mtes_stc_freeStr(&pTagI->tagger);
		ST_TIV2__stc_freeFDatArr(&pTagI->flds);
		ST_DELPOINT(pTagI->pFilen)
		/* */
		ST_DELPOINT(pTag->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_fldData */
Tst_err
st_id3v2_stc_initFDat(Tst_id3v2_fldData *pDat)
{
	Tst_err res;
	Tst_id3v2_fldData_intn *pDatI;

	ST_ASSERTN_IARG(pDat == NULL)

	ST_CALLOC(pDatI, Tst_id3v2_fldData_intn*,
			1, sizeof(Tst_id3v2_fldData_intn))
	if (pDatI == NULL)
		return ST_ERR_OMEM;
	pDat->pObInternal = pDatI;

	res = st_mtes_stc_initStr(&pDatI->attrGeoFn);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&pDatI->attrDesc);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initSA(&pDatI->dataSArr);
	if (res == ST_ERR_SUCC)
		res = st_id3v2_stc_initFPr_i(&pDatI->fldPrI);
	if (res != ST_ERR_SUCC)
		return res;
	st_binobj_stc_initBO(&pDatI->dataBinDat);
	pDatI->pAttrGeoMime = NULL;

	st_id3v2_stc_rsetFDat(pDat);
	return ST_ERR_SUCC;
}

void
st_id3v2_stc_rsetFDat(Tst_id3v2_fldData *pDat)
{
	Tst_id3v2_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL || pDat->pObInternal == NULL)

	pDatI = (Tst_id3v2_fldData_intn*)pDat->pObInternal;

	/* data/attributes */
	st_id3v2_stc_rsetFDat_onlyBufs(pDat);
	/* data */
	pDatI->dataUVal = 0;
	pDatI->dataBVal = 0;
	/* attributes */
	pDatI->attrSltTSF = ST_ID3V2_SLTTSF_NONE;
	pDatI->attrSltCTP = ST_ID3V2_SLTCTP_NONE;
	pDatI->attrPicTp  = ST_ID3V2_PT_NONE;
	pDatI->attrPicFmt = ST_UTILSFMT_PF_NONE;
	pDatI->attrDLI    = 0;
	memset(&pDatI->attrLang, 0, sizeof(pDatI->attrLang));

	/* */
	st_id3v2_stc_rsetFldErrs(&pDatI->errs);
	st_id3v2_stc_rsetFPr_i(&pDatI->fldPrI);
	pDatI->wasDispld      = ST_B_FALSE;
	pDatI->wasRendered    = ST_B_FALSE;
	pDatI->wasAddedToPrio = ST_B_FALSE;
	pDatI->isOK           = ST_B_FALSE;
	pDatI->fnr            = 0;
	pDatI->uniqueIx       = 0;
	/* */
	pDatI->frErrCnt = 0;
}

void
st_id3v2_stc_rsetFDat_onlyBufs(Tst_id3v2_fldData *pDat)
{
	Tst_id3v2_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL || pDat->pObInternal == NULL)

	pDatI = (Tst_id3v2_fldData_intn*)pDat->pObInternal;

	/* data */
	st_mtes_stc_rsetSA(&pDatI->dataSArr);
	st_binobj_stc_rsetData(&pDatI->dataBinDat);
	/* attributes */
	ST_DELPOINT(pDatI->pAttrGeoMime)
	st_mtes_stc_rsetStr(&pDatI->attrGeoFn, ST_B_TRUE, ST_B_TRUE);
	st_mtes_stc_rsetStr(&pDatI->attrDesc, ST_B_TRUE, ST_B_TRUE);
}

void
st_id3v2_stc_freeFDat(Tst_id3v2_fldData *pDat)
{
	Tst_id3v2_fldData_intn *pDatI;

	ST_ASSERTN_VOID(pDat == NULL)

	if (pDat->pObInternal != NULL) {
		st_id3v2_stc_rsetFDat(pDat);

		pDatI = (Tst_id3v2_fldData_intn*)pDat->pObInternal;
		/* */
		st_id3v2_stc_freeFPr_i(&pDatI->fldPrI);
		/* */
		st_binobj_stc_freeBO(&pDatI->dataBinDat);
		st_mtes_stc_freeStr(&pDatI->attrGeoFn);
		st_mtes_stc_freeStr(&pDatI->attrDesc);
		st_mtes_stc_freeSA(&pDatI->dataSArr);
		ST_DELPOINT(pDat->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_fldProp */
void
st_id3v2_stc_rsetFPr(Tst_id3v2_fldProp *pFldPr)
{
	ST_ASSERTN_VOID(pFldPr == NULL)

	pFldPr->pObInternal = NULL;
}

/* Tst_id3v2_fldProp_intn */
Tst_err
st_id3v2_stc_initFPr_i(Tst_id3v2_fldProp_intn *pFldPrI)
{
	ST_ASSERTN_IARG(pFldPrI == NULL)

	st_id3v2_stc_rsetFPr_i(pFldPrI);
	return ST_ERR_SUCC;
}

void
st_id3v2_stc_rsetFPr_i(Tst_id3v2_fldProp_intn *pFldPrI)
{
	ST_ASSERTN_VOID(pFldPrI == NULL)

	pFldPrI->pCID02str   = NULL;
	pFldPrI->pCID34str   = NULL;
	pFldPrI->pCCap       = NULL;
	pFldPrI->vid02str[0] = 0x00;
	pFldPrI->vid34str[0] = 0x00;
	pFldPrI->pIDstr      = NULL;
	pFldPrI->id          = ST_ID3V2_FID_NONE;
	pFldPrI->typ         = ST_ID3V2_FTP_NONE;
	pFldPrI->verFl       = 0;
	pFldPrI->retFromIte  = ST_B_FALSE;
}

void
st_id3v2_stc_freeFPr_i(Tst_id3v2_fldProp_intn *pFldPrI)
{
	ST_ASSERTN_VOID(pFldPrI == NULL)

	st_id3v2_stc_rsetFPr_i(pFldPrI);
}

Tst_err
st_id3v2_stc_copyFPr_i(const Tst_uint32 srcTagVers,
		ST_OPTARG(const Tst_str *pSrcFrIDstr),
		const Tst_id3v2_fldProp_intn *pSrcI,
		Tst_id3v2_fldProp_intn *pDstI)
{
	Tst_err    res;
	char const *pVFrIDstr;
	char       tmpFrIDstr[5];
	Tst_uint32 x,
	           slen;

	ST_ASSERTN_IARG(pSrcI == NULL || pDstI == NULL ||
			pSrcI == pDstI)

	st_id3v2_stc_rsetFPr_i(pDstI);

	if (srcTagVers < 2 || srcTagVers > 4)
		return ST_ERR_IARG;
	if (pSrcFrIDstr == NULL &&
			(pSrcI->id == ST_ID3V2_FID_234_CTXT ||
				pSrcI->id == ST_ID3V2_FID_234_CURL))
		return ST_ERR_IARG;

	for (x = 0; x < sizeof(tmpFrIDstr); x++)
		tmpFrIDstr[x] = 0;
	/* */
	pDstI->pCID02str = pSrcI->pCID02str;
	pDstI->pCID34str = pSrcI->pCID34str;
	pDstI->pCCap     = pSrcI->pCCap;
	/* */
	/** */
	slen = st_sysStrlen(pSrcFrIDstr);
	/** ID3v2.2 ID-String */
	if ((slen == 3 || slen == 4) &&
			(pSrcI->id == ST_ID3V2_FID_234_CTXT ||
				pSrcI->id == ST_ID3V2_FID_234_CURL)) {
		/* adjust ID string */
		for (x = 0; x < 3; x++)
			tmpFrIDstr[x] = pSrcFrIDstr[x];
		pVFrIDstr = tmpFrIDstr;
		st_sysStrToUpper((Tst_str*)pVFrIDstr);
		ST_TIV2__stc_makeUniqueCustIDstr((Tst_str*)pVFrIDstr);
	} else
		pVFrIDstr = NULL;
	res = ST_TIV2__stc_copyFPr_strC(pVFrIDstr, pSrcI->pCID02str,
			pDstI->vid02str, 3);
	if (res == ST_ERR_SUCC)
	if (res != ST_ERR_SUCC)
		return res;
	st_sysStrToUpper((Tst_str*)pDstI->vid02str);
	/** ID3v2.3/4 ID-String */
	if ((slen == 3 || slen == 4) &&
			(pSrcI->id == ST_ID3V2_FID_234_CTXT ||
				pSrcI->id == ST_ID3V2_FID_234_CURL)) {
		/* adjust ID string */
		tmpFrIDstr[3] = 0;
		for (x = 0; x < slen; x++)
			tmpFrIDstr[x] = pSrcFrIDstr[x];
		if (slen == 3)
			tmpFrIDstr[3] = ' ';
		pVFrIDstr = tmpFrIDstr;
		st_sysStrToUpper((Tst_str*)pVFrIDstr);
		ST_TIV2__stc_makeUniqueCustIDstr((Tst_str*)pVFrIDstr);
	} else
		pVFrIDstr = NULL;
	res = ST_TIV2__stc_copyFPr_strC(pVFrIDstr, pSrcI->pCID34str,
			pDstI->vid34str, 4);
	if (res != ST_ERR_SUCC)
		return res;
	st_sysStrToUpper((Tst_str*)pDstI->vid34str);

	/* */
	if (srcTagVers == 2) {
		if (pDstI->vid02str[0] != 0x00 && pDstI->vid02str[0] != ' ')
			pDstI->pIDstr = pDstI->vid02str;
		else
			pDstI->pIDstr = pDstI->pCID02str;
	} else {
		if (pDstI->vid34str[0] != 0x00 && pDstI->vid34str[0] != ' ')
			pDstI->pIDstr = pDstI->vid34str;
		else
			pDstI->pIDstr = pDstI->pCID34str;
	}
	/* */
	pDstI->id         = pSrcI->id;
	pDstI->typ        = pSrcI->typ;
	pDstI->verFl      = pSrcI->verFl;
	pDstI->retFromIte = pSrcI->retFromIte;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_fieldsOutp */
Tst_err
st_id3v2_stc_initFDatOutpArr(Tst_id3v2_fieldsOutp *pFldsO,
		const Tst_uint32 newSz)
{
	Tst_uint32 x;

	ST_ASSERTN_IARG(pFldsO == NULL)

	pFldsO->cnt = 0;
	pFldsO->sz  = newSz;
	ST_CALLOC(pFldsO->ppFldArr, Tst_id3v2_fldData**,
			newSz + 1, sizeof(Tst_id3v2_fldData*))
	if (pFldsO->ppFldArr == NULL)
		return ST_ERR_OMEM;
	for (x = pFldsO->sz; x < newSz; x++)
		pFldsO->ppFldArr[x] = NULL;
	return ST_ERR_SUCC;
}

void
st_id3v2_stc_freeFDatOutpArr(Tst_id3v2_fieldsOutp *pFldsO)
{
	ST_ASSERTN_VOID(pFldsO == NULL)

	ST_DELPOINT(pFldsO->ppFldArr)
	pFldsO->cnt = 0;
	pFldsO->sz  = 0;
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_fldd_rword array */
void
st_id3v2_stc_rsetDatOrdArr(Tst_id3v2_fldd_rwordPArr pArr,
		const Tst_uint32 sz)
{
	Tst_uint32 x;

	ST_ASSERTN_VOID(pArr == NULL)

	for (x = 0; x < sz; x++)
		pArr[x] = ST_ID3V2_TFLD_RWO_TP_NONE;
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_fldd_dattp */
void
st_id3v2_stc_rsetDatTp(Tst_id3v2_fldd_dattp *pDatTp)
{
	ST_ASSERTN_VOID(pDatTp == NULL)

	pDatTp->hasBDat = ST_B_FALSE;
	pDatTp->hasScd  = ST_B_FALSE;
	pDatTp->hasTEnc = ST_B_FALSE;
	pDatTp->hasText = ST_B_FALSE;
	pDatTp->hasCTxt = ST_B_FALSE;
	pDatTp->hasLang = ST_B_FALSE;
	pDatTp->hasFn   = ST_B_FALSE;
	pDatTp->hasMime = ST_B_FALSE;
	pDatTp->hasUVal = ST_B_FALSE;
	pDatTp->hasBVal = ST_B_FALSE;
	/* */
	pDatTp->hasPict = ST_B_FALSE;
	pDatTp->hasSylt = ST_B_FALSE;
	pDatTp->hasGeob = ST_B_FALSE;
	pDatTp->hasCdmf = ST_B_FALSE;
}

void
st_id3v2_stc_copyDatTp(const Tst_id3v2_fldd_dattp *pSrc,
		Tst_id3v2_fldd_dattp *pDst)
{
	ST_ASSERTN_VOID(pSrc == NULL || pDst == NULL)

	memcpy(pDst, pSrc, sizeof(Tst_id3v2_fldd_dattp));
}

/* Tst_id3v2_fldd_strtp */
void
st_id3v2_stc_rsetStrTp(Tst_id3v2_fldd_strtp *pStrTp)
{
	ST_ASSERTN_VOID(pStrTp == NULL)

	pStrTp->hasNumStr    = ST_B_FALSE;
	pStrTp->hasYearStr   = ST_B_FALSE;
	pStrTp->hasDateStr   = ST_B_FALSE;
	pStrTp->hasTimeStr   = ST_B_FALSE;
	pStrTp->hasTStampStr = ST_B_FALSE;
	pStrTp->hasPosStr    = ST_B_FALSE;
	pStrTp->hasGenreStr  = ST_B_FALSE;
	pStrTp->hasSimpleStr = ST_B_FALSE;
	pStrTp->hasUrlStr    = ST_B_FALSE;
	pStrTp->hasFullStr   = ST_B_FALSE;
	pStrTp->hasMappedStr = ST_B_FALSE;
}

void
st_id3v2_stc_copyStrTp(const Tst_id3v2_fldd_strtp *pSrc,
		Tst_id3v2_fldd_strtp *pDst)
{
	ST_ASSERTN_VOID(pSrc == NULL || pDst == NULL)

	memcpy(pDst, pSrc, sizeof(Tst_id3v2_fldd_strtp));
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_thdFlags */
void
st_id3v2_stc_rsetTHdF(Tst_id3v2_thdFlags *pFl)
{
	ST_ASSERTN_VOID(pFl == NULL)

	pFl->hasExtHd = ST_B_FALSE;
	pFl->isExp    = ST_B_FALSE;
	pFl->isUnsy   = ST_B_FALSE;
	pFl->hasFoot  = ST_B_FALSE;
	pFl->isCompr  = ST_B_FALSE;
	pFl->hasCDM   = ST_B_FALSE;
}

/* Tst_id3v2_frhdFlags */
void
st_id3v2_stc_rsetFHdF(Tst_id3v2_frhdFlags *pFl)
{
	ST_ASSERTN_VOID(pFl == NULL)

	pFl->doV34tap   = ST_B_FALSE;
	pFl->doV34fap   = ST_B_FALSE;
	pFl->isV34ro    = ST_B_FALSE;
	pFl->isV34compr = ST_B_FALSE;
	pFl->isV34enc   = ST_B_FALSE;
	pFl->isV34gi    = ST_B_FALSE;
	pFl->isV4unsy   = ST_B_FALSE;
	pFl->hasV4dli   = ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_tagErrs */
void
st_id3v2_stc_rsetTagErrs(Tst_id3v2_tagErrs *pErr)
{
	ST_ASSERTN_VOID(pErr == NULL)

	memset(pErr, 0, sizeof(Tst_id3v2_tagErrs));
}

/* Tst_id3v2_fldErrs */
void
st_id3v2_stc_rsetFldErrs(Tst_id3v2_fldErrs *pErr)
{
	ST_ASSERTN_VOID(pErr == NULL)

	memset(pErr, 0, sizeof(Tst_id3v2_fldErrs));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_id3v2_opts */
static void
ST_TIV2__stc_rsetOpts(Tst_id3v2_opts *pOpts)
{
	ST_ASSERTN_VOID(pOpts == NULL)

	ST_RSETSTC_BASOPTS(pOpts->basOpts)
	pOpts->rdOnlyBasics      = ST_B_FALSE;
	pOpts->unsync            = ST_B_FALSE;
	pOpts->compress          = ST_B_FALSE;
	pOpts->alwLinkedInpFiles = ST_B_FALSE;
	/* */
	pOpts->cbZLibDecmpr = NULL;
	pOpts->cbZLibCmpr   = NULL;
}

/*----------------------------------------------------------------------------*/

/* Tst_id3v2_fields */
static Tst_err
ST_TIV2__stc_initFDatArr(Tst_id3v2_fields *pFlds)
{
	Tst_err res;

	ST_ASSERTN_IARG(pFlds == NULL)

	res = st_dynlist_stc_initDL(&pFlds->list, sizeof(Tst_id3v2_fldData),
			ST_TIV2__stc_cbRsetFld, ST_TIV2__stc_cbFreeFld);
	if (res != ST_ERR_SUCC)
		return res;
	ST_TIV2__stc_rsetFDatArr(pFlds);
	return ST_ERR_SUCC;
}

static void
ST_TIV2__stc_rsetFDatArr(Tst_id3v2_fields *pFlds)
{
	ST_ASSERTN_VOID(pFlds == NULL)

	st_dynlist_stc_rsetDL(&pFlds->list);
	pFlds->skipCnt      = 0;
	pFlds->errCnt       = 0;
	pFlds->uniqueIxLast = 1;
}

static void
ST_TIV2__stc_freeFDatArr(Tst_id3v2_fields *pFlds)
{
	ST_ASSERTN_VOID(pFlds == NULL)

	ST_TIV2__stc_rsetFDatArr(pFlds);
	st_dynlist_stc_freeDL(&pFlds->list);
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TIV2__stc_copyFPr_strC(const char *pStrS1, const char *pStrS2,
		char *pStrD, const Tst_uint32 maxlen)
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
	if (len > 0)
		memcpy(pStrD, pSrc, len);
	pStrD[len] = 0x00;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * This function uses a fairly simple method to generate a
 *   unique custom ID-String, but it does it job
 */
static void
ST_TIV2__stc_makeUniqueCustIDstr(Tst_str *pIDstr)
{
	Tst_uint32 slen    = st_sysStrlen(pIDstr),
	           lcnt    = 0,
	           startV;
	Tst_bool   isInArr,
	           secRun  = ST_B_FALSE,
	           startWS = ST_B_FALSE;
	Tst_str    *pLCh,
	           lchOrg;

	ST_ASSERTN_VOID(slen != 3 && slen != 4)

	pLCh   = &pIDstr[slen - 1];
	lchOrg = *pLCh;
	if (*pLCh >= '0' && *pLCh <= '9')
		startV = 0;
	else if (*pLCh >= 'A' && *pLCh <= 'Z')
		startV = 1;
	else {
		startV  = 2;
		startWS = ST_B_TRUE;
	}

	do {
		isInArr = (! startWS &&
					st_id3v2_fnc_isFldPropInArr_byIDstr(ST_ID3V2_FLDPROPS,
								pIDstr, (slen == 3 ? 2 : 3), NULL));
		if (! isInArr)
			isInArr = (! startWS &&
						st_id3v2_fnc_isFldPropInArr_byIDstr(ST_ID3V2_FLDPROPS_IGN,
									pIDstr, (slen == 3 ? 2 : 3), NULL));
		if (! isInArr && ! startWS)
			break;
		startWS = ST_B_FALSE;
		switch (startV) {
		case 1:  /* we started with '[A-Z]' */
			if (*pLCh >= 'A' && *pLCh < 'Z')
				*pLCh = (Tst_str)((int)*pLCh + 1);
			else if (*pLCh == 'Z')
				*pLCh = '0';
			else if (*pLCh >= '0' && *pLCh < '9')
				*pLCh = (Tst_str)((int)*pLCh + 1);
			else if (*pLCh == '9') {
				if (secRun)
					break;
				*pLCh  = lchOrg;
				secRun = ST_B_TRUE;
				--pLCh;
			}
			break;
		case 0:   /* we started with '[0-9]' */
		default:  /* we started with ' ' */
			if (*pLCh == ' ')
				*pLCh = '0';
			else if (*pLCh >= '0' && *pLCh < '9')
				*pLCh = (Tst_str)((int)*pLCh + 1);
			else if (*pLCh == '9')
				*pLCh = 'A';
			else if (*pLCh >= 'A' && *pLCh < 'Z')
				*pLCh = (Tst_str)((int)*pLCh + 1);
			else if (*pLCh == 'Z') {
				if (secRun)
					break;
				*pLCh  = lchOrg;
				secRun = ST_B_TRUE;
				--pLCh;
			}
		}
	} while (lcnt < 500);

	/* fallback */
	if (isInArr) {
		pIDstr[slen - 1] = 'Z';
		pIDstr[slen - 2] = 'Z';
	}
}

/*----------------------------------------------------------------------------*/

static void ST_TIV2__stc_cbRsetFld(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_id3v2_stc_rsetFDat((Tst_id3v2_fldData*)pElem);
}

static void ST_TIV2__stc_cbFreeFld(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_id3v2_stc_freeFDat((Tst_id3v2_fldData*)pElem);
}

/******************************************************************************/
