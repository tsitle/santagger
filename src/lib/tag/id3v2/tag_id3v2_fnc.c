/******************************************************************************/
/* tag_id3v2_fnc.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for ID3v2 Tag handling                                       */
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
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/dynlist.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_FNC_C
#include "tag_id3v2_fnc-priv.h"
/*** */
#include "tag_id3v2_gs-prot.h"
#include "tag_id3v2_stcs-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_FNC_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get number of first valid field with the specified ID
 *
 * returns: field nr. or (ST_ID3V2_MAX_TFLD_CNT+1) if no
 *          valid field was found
 */
#if 0
Tst_uint32
st_id3v2_fnc_getFirstValidFld_nr_byID(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID id, ST_OPTARG(const Tst_str *pIDstr))
{
	Tst_uint32 nr = 0;

	ST_ASSERTN_NUM(ST_ID3V2_MAX_TFLD_CNT + 1,
			pTag == NULL || pTag->pObInternal == NULL)

	ST_TIV2__fnc_getFirstValidFld_byID(pTag, id, pIDstr, NULL, &nr);
	return nr;
}
#endif

/*
 * Get pointer to first valid field with the specified ID
 *
 * returns: pointer to field or NULL if no valid field was found
 */
Tst_id3v2_fldData*
st_id3v2_fnc_getFirstValidFld_pt_byID(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID id, ST_OPTARG(const Tst_str *pIDstr))
{
	Tst_id3v2_fldData *pDat = NULL;

	ST_ASSERTN_NULL(pTag == NULL || pTag->pObInternal == NULL)

	ST_TIV2__fnc_getFirstValidFld_byID(pTag, id, pIDstr, &pDat, NULL);
	return pDat;
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_id3v2_fnc_hasIDstrInvalidChars(ST_OPTARG(Tst_str *pFIDstr),
		const Tst_bool modInvChars,
		ST_OPTARG(Tst_str *pInvalidCharFound))
{
	Tst_bool   resB = ST_B_FALSE;
	Tst_uint32 slen = st_sysStrlen(pFIDstr),
	           x;
	Tst_byte   uch;

	if (pInvalidCharFound != NULL)
		*pInvalidCharFound = 0x00;
	if (pFIDstr == NULL || (slen != 3 && slen != 4))
		return ST_B_TRUE;

	for (x = 0; x < slen; x++) {
		/* only A-Z and 0-9 (and for iTunes the last
		 *   char may also be ' ') allowed  */
		uch = (Tst_byte)pFIDstr[x];
		if (! ((uch >= ST_CSET_ISO_ACAP && uch <= ST_CSET_ISO_ZCAP) ||
					(uch >= ST_CSET_ISO_ASMA && uch <= ST_CSET_ISO_ZSMA) ||
					(uch >= ST_CSET_ISO_DIG0 && uch <= ST_CSET_ISO_DIG9) ||
					(x == 3 && uch == ST_CSET_ISO_SPA))) {
			if (pInvalidCharFound != NULL && ! resB)
				*pInvalidCharFound = (Tst_str)uch;
			if (modInvChars)
				pFIDstr[x] = '?';
			resB = ST_B_TRUE;
		}
	}
	return resB;
}

Tst_id3v2_frID
st_id3v2_fnc_getID_byIDstr(const Tst_str *pFIDstr)
{
	ST_ASSERTN_NUM(ST_ID3V2_FID_NONE, pFIDstr == NULL)

	if (st_id3v2_fnc_hasIDstrInvalidChars((Tst_str*)pFIDstr,
				/*mod:*/ST_B_FALSE, NULL))
		return ST_ID3V2_FID_NONE;

	return st_id3v2_fnc_adjustFrID(ST_ID3V2_FID_NONE, pFIDstr, NULL);
}

Tst_err
st_id3v2_fnc_getIDstr_byEnum(Tst_id3v2_frID frID, const Tst_uint32 tagVerMaj,
		Tst_str **ppFIDstr)
{
	Tst_id3v2_fldProp_intn *pFldPrI = NULL;

	ST_ASSERTN_IARG(ppFIDstr == NULL || (tagVerMaj < 2 || tagVerMaj > 4))

	/* adjust frID */
	frID = st_id3v2_fnc_adjustFrID(frID, NULL, &pFldPrI);
	if (frID == ST_ID3V2_FID_NONE) {
		Tst_err res;

		res = st_sysStrcpy2("", ppFIDstr);
		if (res == ST_ERR_SUCC)
			res = ST_ERR_IARG;
		return res;
	}

	return st_sysStrcpy2((tagVerMaj == 2 ? pFldPrI->pCID02str : pFldPrI->pCID34str),
				ppFIDstr);
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_fnc_getFieldDataTypes(const Tst_id3v2_frID frID,
		Tst_id3v2_fldd_dattp *pDatTp)
{
	Tst_id3v2_fldd_strtp tpStr;

	ST_ASSERTN_IARG(pDatTp == NULL)

	st_id3v2_stc_rsetStrTp(&tpStr);

	return st_id3v2_fnc_setDataNstrTp(frID, pDatTp, &tpStr, NULL, 0);
}

Tst_err
st_id3v2_fnc_getFieldStringTypes(const Tst_id3v2_frID frID,
		Tst_id3v2_fldd_strtp *pStrTp)
{
	Tst_id3v2_fldd_dattp tpDat;

	ST_ASSERTN_IARG(pStrTp == NULL)

	st_id3v2_stc_rsetDatTp(&tpDat);

	return st_id3v2_fnc_setDataNstrTp(frID, &tpDat, pStrTp, NULL, 0);
}

/*----------------------------------------------------------------------------*/

Tst_id3v2_picTp
st_id3v2_fnc_getPicTp_enum_byBin(const Tst_byte bin)
{
	Tst_id3v2_picTp_arrElem const *pElem;

	pElem = ST_TIV2__fnc_getPicTp_arrE(bin, NULL, NULL, ST_ID3V2_PT_NONE);
	if (pElem == NULL)
		return ST_ID3V2_PT_NONE;
	return pElem->ptpE;
}

Tst_id3v2_picTp
st_id3v2_fnc_getPicTp_enum_byCapShort(const Tst_str *pCapSh)
{
	Tst_id3v2_picTp_arrElem const *pElem;

	ST_ASSERTN_NUM(ST_ID3V2_PT_NONE, pCapSh == NULL)

	pElem = ST_TIV2__fnc_getPicTp_arrE(0xff, NULL, pCapSh, ST_ID3V2_PT_NONE);
	if (pElem == NULL)
		return ST_ID3V2_PT_NONE;
	return pElem->ptpE;
}

Tst_byte
st_id3v2_fnc_getPicTp_bin_byEnum(const Tst_id3v2_picTp ptpE)
{
	Tst_id3v2_picTp_arrElem const *pElem;

	pElem = ST_TIV2__fnc_getPicTp_arrE(0xff, NULL, NULL, ptpE);
	if (pElem == NULL)
		pElem = ST_TIV2__fnc_getPicTp_arrE(0xff, NULL, NULL, ST_ID3V2_PT_OTHER);
	return pElem->bin;
}

const Tst_str*
st_id3v2_fnc_getPicTp_cap_byEnum(const Tst_id3v2_picTp ptpE)
{
	Tst_id3v2_picTp_arrElem const *pElem;

	pElem = ST_TIV2__fnc_getPicTp_arrE(0xff, NULL, NULL, ptpE);
	if (pElem == NULL)
		return NULL;
	return (Tst_str*)pElem->pCap;
}

const Tst_str*
st_id3v2_fnc_getPicTp_capShort_byEnum(const Tst_id3v2_picTp ptpE)
{
	Tst_id3v2_picTp_arrElem const *pElem;

	pElem = ST_TIV2__fnc_getPicTp_arrE(0xff, NULL, NULL, ptpE);
	if (pElem == NULL)
		return NULL;
	return (Tst_str*)pElem->pShort;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get basename of file
 *   (e.g. "/root/blackbible.html" --> "blackbible.html")
 */
Tst_err
st_id3v2_fnc_getFileBasename(const Tst_mtes_string *pFilen,
		ST_OPTARG(Tst_mtes_string *pBasenU8), ST_OPTARG(Tst_str **ppBasenU8PCh))
{
	Tst_err res;
	Tst_str *pTmpFnU8PCh    = NULL,
	        *pTmpBasenU8PCh = NULL,
	        **ppBnPCh       = &pTmpBasenU8PCh;

	ST_ASSERTN_IARG(pFilen == NULL || (pBasenU8 == NULL && ppBasenU8PCh == NULL))

	if (ppBasenU8PCh != NULL)
		ppBnPCh = ppBasenU8PCh;
	res = st_mtes_copyToCharp_utf8(pFilen, &pTmpFnU8PCh);
	if (res == ST_ERR_SUCC) {
		if (! st_sysFileBasename(pTmpFnU8PCh, ppBnPCh))
			res = ST_ERR_FAIL;
		else if (pBasenU8 != NULL)
			res = st_mtes_setStrTo(ST_MTES_TE_UTF8, *ppBnPCh, pBasenU8);
	}
	ST_DELPOINT(pTmpFnU8PCh)
	if (ppBasenU8PCh == NULL)
		ST_DELPOINT(pTmpBasenU8PCh)
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Get extension of filename
 *   (e.g. "/root/blackbible.html" --> "html")
 */
Tst_err
st_id3v2_fnc_getFilenameExt(const Tst_mtes_string *pFilen,
		Tst_str **ppFExtU8PCh)
{
	Tst_err res;
	Tst_str *pTmpFnU8PCh = NULL;

	ST_ASSERTN_IARG(pFilen == NULL || ppFExtU8PCh == NULL)

	res = st_mtes_copyToCharp_utf8(pFilen, &pTmpFnU8PCh);
	if (res == ST_ERR_SUCC && ! st_sysFileExt(pTmpFnU8PCh, ppFExtU8PCh))
		res = ST_ERR_FAIL;
	ST_DELPOINT(pTmpFnU8PCh)
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_id3v2_frTp
st_id3v2_fnc_getFrameType_byID(const Tst_id3v2_frID frID)
{
	Tst_str const *pFrIDstr = NULL;
	Tst_id3v2_fldProp_intn *pFldPrI = NULL;

	if (frID == ST_ID3V2_FID_234_CTXT)
		pFrIDstr = (Tst_str const*)"T000";
	else if (frID == ST_ID3V2_FID_234_CURL)
		pFrIDstr = (Tst_str const*)"W000";

	if (st_id3v2_fnc_adjustFrID(frID, pFrIDstr, &pFldPrI) == ST_ID3V2_FID_NONE)
		return ST_ID3V2_FTP_NONE;

	return pFldPrI->typ;
}

/*----------------------------------------------------------------------------*/

/*
 * Set data and string type
 *   and, if pRdWrOrderArr != NULL, set order to read/write data in
 *
 * pTpDat and pTpStr  MUST be != NULL
 * pRdWrOrderArr      may be NULL
 *
 * returns: ERRCODE
 *          pTpDat and pTpStr  will be set
 *          pRdWrOrderArr      will be set if != NULL
 */
Tst_err
st_id3v2_fnc_setDataNstrTp(const Tst_id3v2_frID frID,
		Tst_id3v2_fldd_dattp *pTpDat, Tst_id3v2_fldd_strtp *pTpStr,
		ST_OPTARG(Tst_id3v2_fldd_rwordPArr pRdWrOrderArr),
		const Tst_uint32 rwoArrSz)
{
	Tst_uint32       ordSzDummy = 0;
	Tst_uint32 const *pSz       = &ordSzDummy;
	Tst_id3v2_fldd_rword_typ ordDummy[6];
	Tst_id3v2_fldd_rwordPArr pOrd = (Tst_id3v2_fldd_rwordPArr)&ordDummy;
	Tst_id3v2_frTp           frTp = ST_ID3V2_FTP_NONE;

	ST_ASSERTN_IARG(pTpDat == NULL || pTpStr == NULL)

	/* get frame type */
	frTp = st_id3v2_fnc_getFrameType_byID(frID);
	if (frTp == ST_ID3V2_FTP_NONE)
		return ST_ERR_IARG;

	/* init some stuff */
	if (pRdWrOrderArr != NULL) {
		pOrd = pRdWrOrderArr;
		pSz  = &rwoArrSz;
	} else
		ordSzDummy = sizeof(ordDummy) / sizeof(Tst_id3v2_fldd_rword_typ);
	st_id3v2_stc_rsetDatOrdArr(pOrd, *pSz);
	st_id3v2_stc_rsetDatTp(pTpDat);
	st_id3v2_stc_rsetStrTp(pTpStr);

	/* ______________________________________________________________ */
	/* COMM,USLT:     te lang                      scd  text          */
	/* TXXX,WXXX:     te                           scd  text          */
	/* APIC:          te [PIC:imgfor|APIC:mime] pt desc      BINdata  */
	/* CM1,T000-TZZZ: te                                string(s)     */
	/* W000-WZZZ:                                       url           */
	/* UFID,PRIV:                                  own       BINdata  */
	/* IPLS:          te                                strpairs      */
	/* MCDI:                                                 BINdata  */
	/* GEO[B]:        te mime filen                scd       BINdata  */
	/* ______________________________________________________________ */
	/* CDM:     comprMeth uncomprSz comprBINdata                      */
	/* [P]CNT:  4bytes [byte ...]                                     */
	/* POP[M]:  email rating 4bytes [byte ...]                        */
	/* S[Y]LT:  te lang timestampfmt conttp contdesc BINdata          */
	/* ______________________________________________________________ */

	switch (frTp) {
	case ST_ID3V2_FTP_COM:      /* COM[M],U[S]LT */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TENC;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_LANG;
		pOrd[2] = ST_ID3V2_TFLD_RWO_TP_SCD;
		pOrd[3] = ST_ID3V2_TFLD_RWO_TP_TEXT;
		/* */
		pTpStr->hasFullStr = ST_B_TRUE;
		break;
	case ST_ID3V2_FTP_SLT:      /* S[Y]LT */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TENC;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_LANG;
		pOrd[2] = ST_ID3V2_TFLD_RWO_TP_SYLT;
		pOrd[3] = ST_ID3V2_TFLD_RWO_TP_SCD;
		pOrd[4] = ST_ID3V2_TFLD_RWO_TP_BDAT;
		/* */
		pTpStr->hasFullStr = ST_B_TRUE;  /* converted text w/ '\n' */
		break;
	case ST_ID3V2_FTP_PIC:      /* [A]PIC */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TENC;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_PICT;
		pOrd[2] = ST_ID3V2_TFLD_RWO_TP_SCD;
		pOrd[3] = ST_ID3V2_TFLD_RWO_TP_BDAT;
		/* */
		break;
	case ST_ID3V2_FTP_TXT:      /* CM1,T00[0]-TZZ[Z] */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TENC;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_TEXT;
		/* */
		switch (frID) {
		case ST_ID3V2_FID_234_CURL:
			pTpStr->hasUrlStr = ST_B_TRUE; break;
		case ST_ID3V2_FID_234_TCON:
			pTpStr->hasGenreStr = ST_B_TRUE; break;
		case ST_ID3V2_FID_23__TDAT:
			pTpStr->hasDateStr = ST_B_TRUE; break;
		case ST_ID3V2_FID___4_TDOR:
		case ST_ID3V2_FID___4_TDRL:
		case ST_ID3V2_FID___4_TDRC:
		case ST_ID3V2_FID___4_TDEN:
		case ST_ID3V2_FID___4_TDTG:
			pTpStr->hasTStampStr = ST_B_TRUE; break;
		case ST_ID3V2_FID_23__TIME:
			pTpStr->hasTimeStr = ST_B_TRUE; break;
		case ST_ID3V2_FID_234_TBPM:
		case ST_ID3V2_FID_234_TCMP:
		case ST_ID3V2_FID_234_TDLY:
		case ST_ID3V2_FID_234_TLEN:
			pTpStr->hasNumStr = ST_B_TRUE; break;
		case ST_ID3V2_FID___4_TIPL:
		case ST_ID3V2_FID___4_TMCL:
			pTpStr->hasMappedStr = ST_B_TRUE; break;
		case ST_ID3V2_FID_234_TRCK:
		case ST_ID3V2_FID_234_TPOS:
			pTpStr->hasPosStr = ST_B_TRUE; break;
		case ST_ID3V2_FID_23__TORY:
		case ST_ID3V2_FID_23__TYER:
			pTpStr->hasYearStr = ST_B_TRUE; break;
		case ST_ID3V2_FID_234_WFED:
			pTpStr->hasUrlStr = ST_B_TRUE; break;
		default:
			pTpStr->hasSimpleStr = ST_B_TRUE;
		}
		break;
	case ST_ID3V2_FTP_URL:      /* W00[0]-WZZ[Z] */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TEXT;
		/* */
		pTpStr->hasUrlStr = ST_B_TRUE;
		break;
	case ST_ID3V2_FTP_UFI:      /* UFI[D],PRIV */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_SCD;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_BDAT;
		/* */
		break;
	case ST_ID3V2_FTP_IPL:      /* IPL[S] */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TENC;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_TEXT;
		/* */
		pTpStr->hasMappedStr = ST_B_TRUE;
		break;
	case ST_ID3V2_FTP_MCI:      /* MC[D]I */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_BDAT;
		/* */
		break;
	case ST_ID3V2_FTP_CDM:      /* CDM */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_CDMF;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_BDAT;
		/* */
		break;
	case ST_ID3V2_FTP_WXX:      /* TXX[X],WXX[X] */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TENC;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_SCD;
		pOrd[2] = ST_ID3V2_TFLD_RWO_TP_TEXT;
		/* */
		if (frID == ST_ID3V2_FID_234_TXXX)
			pTpStr->hasSimpleStr = ST_B_TRUE;
		else
			pTpStr->hasUrlStr = ST_B_TRUE;
		break;
	case ST_ID3V2_FTP_GEO:      /* GEO[B] */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_TENC;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_GEOB;
		pOrd[2] = ST_ID3V2_TFLD_RWO_TP_SCD;
		pOrd[3] = ST_ID3V2_TFLD_RWO_TP_BDAT;
		/* */
		break;
	case ST_ID3V2_FTP_CNT:      /* [P]CNT */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_UVAL;
		/* */
		break;
	case ST_ID3V2_FTP_POP:      /* POP[M] */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_SCD;
		pOrd[1] = ST_ID3V2_TFLD_RWO_TP_BVAL;
		pOrd[2] = ST_ID3V2_TFLD_RWO_TP_UVAL;
		/* */
		break;
	case ST_ID3V2_FTP_CPY:      /* copy data */
		pOrd[0] = ST_ID3V2_TFLD_RWO_TP_BDAT;
		break;
	default:  /* ST_ID3V2_FTP_NONE */
		return ST_ERR_IARG;
	}

	/* */
	pTpDat->hasCTxt = (frTp == ST_ID3V2_FTP_SLT ||
				frTp == ST_ID3V2_FTP_UFI ||
				frTp == ST_ID3V2_FTP_MCI);
	pTpDat->hasSylt = (frTp == ST_ID3V2_FTP_SLT);
	pTpDat->hasPict = (frTp == ST_ID3V2_FTP_PIC);
	pTpDat->hasCdmf = (frTp == ST_ID3V2_FTP_CDM);
	pTpDat->hasGeob = (frTp == ST_ID3V2_FTP_GEO);
	pTpDat->hasFn   = pTpDat->hasGeob;
	pTpDat->hasMime = pTpDat->hasGeob;
	pTpDat->hasTEnc = ST_TIV2__fnc_isInRWOarr(ST_ID3V2_TFLD_RWO_TP_TENC, pOrd, *pSz);
	pTpDat->hasLang = ST_TIV2__fnc_isInRWOarr(ST_ID3V2_TFLD_RWO_TP_LANG, pOrd, *pSz);
	pTpDat->hasScd  = ST_TIV2__fnc_isInRWOarr(ST_ID3V2_TFLD_RWO_TP_SCD, pOrd, *pSz);
	pTpDat->hasText = ST_TIV2__fnc_isInRWOarr(ST_ID3V2_TFLD_RWO_TP_TEXT, pOrd, *pSz);
	pTpDat->hasBDat = ST_TIV2__fnc_isInRWOarr(ST_ID3V2_TFLD_RWO_TP_BDAT, pOrd, *pSz);
	pTpDat->hasUVal = ST_TIV2__fnc_isInRWOarr(ST_ID3V2_TFLD_RWO_TP_UVAL, pOrd, *pSz);
	pTpDat->hasBVal = ST_TIV2__fnc_isInRWOarr(ST_ID3V2_TFLD_RWO_TP_BVAL, pOrd, *pSz);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Is frame ID in array ?
 *
 * pFldPrIArr  MUST be set
 * pIDstr      MUST be set
 *
 * returns: true if found
 */
Tst_bool
st_id3v2_fnc_isFldPropInArr_byIDstr(const Tst_id3v2_fldProp_intn *pFldPrIArr,
		const Tst_str *pIDstr, const Tst_byte verMaj,
		ST_OPTARG(Tst_uint32 *pIx))
{
	Tst_uint32 tmpIx,
	           *pIIx = &tmpIx;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFldPrIArr == NULL || pIDstr == NULL)

	/* the ID string is unique */

	if (pIx != NULL)
		pIIx = pIx;
	*pIIx = 0;
	if (st_sysStrEmpty(pIDstr))
		return ST_B_FALSE;
	while ((verMaj == 2 && pFldPrIArr[*pIIx].pCID02str != NULL) ||
			(verMaj > 2 && pFldPrIArr[*pIIx].pCID34str != NULL)) {
		if ((verMaj == 2 &&
					st_sysStrcmp2(ST_B_FALSE, pFldPrIArr[*pIIx].pCID02str, pIDstr)) ||
				(verMaj > 2 &&
					st_sysStrcmp2(ST_B_FALSE, pFldPrIArr[*pIIx].pCID34str, pIDstr)))
			return (pFldPrIArr[*pIIx].id != ST_ID3V2_FID_NONE);
		++(*pIIx);
	}
	return ST_B_FALSE;
}

Tst_bool
st_id3v2_fnc_isFldPropInArr_byEnum(const Tst_id3v2_fldProp_intn *pFldPrIArr,
		const Tst_id3v2_frID id, ST_OPTARG(Tst_uint32 *pIx))
{
	Tst_uint32 tmpIx,
	           *pIIx = &tmpIx;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFldPrIArr == NULL)

	/* one ID can be assigned to multiple ID strings
	 *   we return the index of the first occurence of the ID */

	if (pIx != NULL)
		pIIx = pIx;
	*pIIx = 0;
	if (id == ST_ID3V2_FID_NONE)
		return ST_B_FALSE;
	while (pFldPrIArr[*pIIx].id != ST_ID3V2_FID_NONE) {
		if (pFldPrIArr[*pIIx].id == id)
			return ST_B_TRUE;
		++(*pIIx);
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Adjust the frame ID
 *   If frID is ST_ID3V2_FID_234_CTXT or ST_ID3V2_FID_234_CURL
 *   then an ID-String must be given in pFrIDstr
 *
 * returns: the 'adjusted' frame ID
 *          or ST_ID3V2_FID_NONE if frID is a custum one (see above) and
 *             no ID-String was given
 */
Tst_id3v2_frID
st_id3v2_fnc_adjustFrID(Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		ST_OPTARG(Tst_id3v2_fldProp_intn **ppFldPrI))
{
	Tst_uint32 ix         = 0,
	           slen       = st_sysStrlen(pFrIDstr);
	Tst_bool   isInArr[2] = {ST_B_FALSE, ST_B_FALSE};

	if (ppFldPrI != NULL)
		*ppFldPrI = NULL;

	/* search by ID-String */
	if (! (slen == 3 || slen == 4) &&
			(frID == ST_ID3V2_FID_234_CTXT || frID == ST_ID3V2_FID_234_CURL))
		frID = ST_ID3V2_FID_NONE;
	if (slen == 3 || slen == 4) {
		isInArr[0] = st_id3v2_fnc_isFldPropInArr_byIDstr(ST_ID3V2_FLDPROPS,
					pFrIDstr, (slen == 3 ? 2 : 3), &ix);
		if (isInArr[0])
			frID = ST_ID3V2_FLDPROPS[ix].id;
		else {
			isInArr[1] = st_id3v2_fnc_isFldPropInArr_byIDstr(ST_ID3V2_FLDPROPS_IGN,
						pFrIDstr, (slen == 3 ? 2 : 3), &ix);
			if (isInArr[1])
				frID = ST_ID3V2_FLDPROPS_IGN[ix].id;
			else {
				Tst_bool isCust = ST_B_FALSE;

				if (pFrIDstr[0] == 't' || pFrIDstr[0] == 'T') {
					frID   = ST_ID3V2_FID_234_CTXT;
					isCust = ST_B_TRUE;
				} else if (pFrIDstr[0] == 'w' || pFrIDstr[0] == 'W') {
					frID   = ST_ID3V2_FID_234_CURL;
					isCust = ST_B_TRUE;
				}
				if (isCust)
					isInArr[0] = st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS,
								frID, &ix);
			}
		}
	}
	/* search by numeric ID */
	if (! (isInArr[0] || isInArr[1]) && frID != ST_ID3V2_FID_NONE) {
		isInArr[0] = st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS,
					frID, &ix);
		if (! isInArr[0]) {
			isInArr[1] = st_id3v2_fnc_isFldPropInArr_byEnum(ST_ID3V2_FLDPROPS_IGN,
						frID, &ix);
			if (! isInArr[1])
				frID = ST_ID3V2_FID_NONE;
		}
	}

	if (ppFldPrI != NULL) {
		if (isInArr[0])
			*ppFldPrI = (Tst_id3v2_fldProp_intn*)&ST_ID3V2_FLDPROPS[ix];
		else if (isInArr[1])
			*ppFldPrI = (Tst_id3v2_fldProp_intn*)&ST_ID3V2_FLDPROPS_IGN[ix];
	}
	return frID;
}

/*----------------------------------------------------------------------------*/

/*
 * Match two IDs
 */
Tst_bool
st_id3v2_fnc_matchFrIDs(Tst_id3v2_frID frID1, ST_OPTARG(const char *pFrID1str),
		Tst_id3v2_frID frID2, ST_OPTARG(const char *pFrID2str))
{
	frID1 = st_id3v2_fnc_adjustFrID(frID1, (const Tst_str*)pFrID1str, NULL);
	frID2 = st_id3v2_fnc_adjustFrID(frID2, (const Tst_str*)pFrID2str, NULL);

	return (frID1 == frID2 &&
				((frID1 != ST_ID3V2_FID_234_CTXT && frID1 != ST_ID3V2_FID_234_CURL) ||
					(frID1 != ST_ID3V2_FID_NONE &&
						pFrID1str != NULL && pFrID2str != NULL &&
						st_sysStrcmp2(ST_B_FALSE, pFrID1str, pFrID2str))));
}

/*
 * Check whether it's the Tagger Info field ID
 */
Tst_bool
st_id3v2_fnc_isTaggInfFr(Tst_id3v2_frID frID, ST_OPTARG(const char *pFrIDstr),
		const Tst_uint32 tagVerMaj)
{
	return (frID == ST_ID3V2_FID_234_CTXT && pFrIDstr != NULL &&
			st_sysStrcmp2(ST_B_FALSE, pFrIDstr,
						(tagVerMaj == 2 ? "TGA" : "TGGA")));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Creates and adds a new element to the field list
 */
Tst_err
st_id3v2_fnc_addElemToFlds(Tst_id3v2_tag *pTag,
		Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr),
		Tst_bool *pSkipped, Tst_id3v2_fldData **ppNewFld)
{
	Tst_err       res;
	Tst_str const *pIDstr = NULL;
	Tst_id3v2_fldProp_intn *pFldPrIsrc = NULL;
	Tst_id3v2_fldData_intn *pNFldI     = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pSkipped == NULL || ppNewFld == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	*pSkipped = ST_B_FALSE;
	*ppNewFld = NULL;

	/* adjust frID */
	frID = st_id3v2_fnc_adjustFrID(frID, pFrIDstr, &pFldPrIsrc);
	if (frID == ST_ID3V2_FID_NONE)
		return ST_ERR_IARG;

	/* add new field and init field data */
	/** */
	ST_CALLOC(*ppNewFld, Tst_id3v2_fldData*, 1, sizeof(Tst_id3v2_fldData))
	if (*ppNewFld == NULL)
		return ST_ERR_OMEM;
	res = st_id3v2_stc_initFDat(*ppNewFld);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(*ppNewFld)
		return res;
	}
	/** */
	pNFldI           = (Tst_id3v2_fldData_intn*)(*ppNewFld)->pObInternal;
	pNFldI->uniqueIx = pTagI->flds.uniqueIxLast++;
	/** */
	res = st_dynlist_addElement(&pTagI->flds.list, *ppNewFld);
	if (res != ST_ERR_SUCC) {
		st_id3v2_stc_freeFDat(*ppNewFld);
		ST_DELPOINT(*ppNewFld)
		return res;
	}
	/** copy field properties */
	if (frID == ST_ID3V2_FID_234_CTXT || frID == ST_ID3V2_FID_234_CURL)
		pIDstr = pFrIDstr;
	res = st_id3v2_stc_copyFPr_i(pTagI->verMaj,
			pIDstr, pFldPrIsrc, &pNFldI->fldPrI);
	if (res != ST_ERR_SUCC)
		return res;

	/* set field number, minimum number is 1 */
	pNFldI->fnr = ST_TIV2__fnc_getNewFldNr(pTag,
				pNFldI->fldPrI.id, (const Tst_str*)pNFldI->fldPrI.pIDstr);
	if (pNFldI->fnr == 0) {
		*pSkipped = ST_B_TRUE;
	} else if (pNFldI->fnr > ST_ID3V2_MAX_TFLDID_CNT) {
		*pSkipped = ST_B_TRUE;
		/* update error counter */
		if (! pNFldI->errs.frTooManyTFI) {
			pNFldI->errs.frTooManyTFI = ST_B_TRUE;
			++pNFldI->frErrCnt;
		}
	}

	/* check if field is valid for tag version */
	if (! *pSkipped &&
			(
				(pTagI->verMaj == 2 &&
					(pNFldI->fldPrI.vid02str[0] == 0x00 ||
						pNFldI->fldPrI.vid02str[0] == ' ' ||
						pNFldI->fldPrI.vid02str[0] == '-')) ||
				(pTagI->verMaj > 2 &&
					(pNFldI->fldPrI.vid34str[0] == 0x00 ||
						pNFldI->fldPrI.vid34str[0] == ' ' ||
						pNFldI->fldPrI.vid34str[0] == '-'))
			)) {
		*pSkipped = ST_B_TRUE;
		/* update error counter */
		if (! pNFldI->errs.frIsntValVers) {
			pNFldI->errs.frIsntValVers = ST_B_TRUE;
			++pNFldI->frErrCnt;
		}
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_fnc_sortFields_byPrio(Tst_id3v2_tag *pTag,
		const Tst_bool sortPicTypes)
{
	Tst_err    res;
	Tst_uint32 x,
	           cix;
	Tst_id3v2_fieldsOutp   ptArr;
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	x = st_id3v2_gs_getTag_amountFlds(pTag);
	if (x < 2)
		return ST_ERR_SUCC;

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	/* init ptArr */
	res = st_id3v2_stc_initFDatOutpArr(&ptArr, x);
	if (res != ST_ERR_SUCC)
		return res;

	/* fill ptArr with fields in order of their priority */
	res = ST_TIV2__fnc_sortFields_byPrio_sub1(pTag, sortPicTypes, &ptArr);
	if (res != ST_ERR_SUCC) {
		st_id3v2_stc_freeFDatOutpArr(&ptArr);
		return res;
	}

	/* show me all fields in order ... */
	for (x = 1; x <= ptArr.cnt; x++) {
		pItFld = ptArr.ppFldArr[x - 1];
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFld == NULL)
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		cix = ST_TIV2__fnc_getListIndex_byUIX(pTagI, pItFldI->uniqueIx);
		if (cix == x)
			continue;
		if (cix == 0 ||
				! st_dynlist_moveElement(&pTagI->flds.list, cix, x)) {
			res = ST_ERR_FAIL;
			break;
		}
	}
	st_id3v2_stc_freeFDatOutpArr(&ptArr);

	/* update unique indices */
	cix = 1;
	if (! st_dynlist_ite_gotoFirst(&pTagI->flds.list))
		return ST_ERR_FAIL;
	do {
		pItFld = (Tst_id3v2_fldData*)st_dynlist_ite_getCurrent(&pTagI->flds.list);
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFld == NULL)
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		pItFldI->uniqueIx = cix++;
	} while (st_dynlist_ite_gotoNext(&pTagI->flds.list));
	pTagI->flds.uniqueIxLast = cix;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

void
st_id3v2_fnc_updateFldErrorCnt(Tst_id3v2_tag *pTag)
{
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	pTagI->flds.errCnt = 0;
	while ((pItFld = st_id3v2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_VOID(pItFldI == NULL)

		pTagI->flds.errCnt += pItFldI->frErrCnt;
	}
}

/*----------------------------------------------------------------------------*/

Tst_err
st_id3v2_fnc_moveFld(Tst_id3v2_tag *pTag, const Tst_uint32 curIx,
		const Tst_uint32 newIx)
{
	Tst_uint32 uix = 1;
	Tst_dynlist        *pDL;
	Tst_id3v2_fldData  *pItFld;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			curIx == 0 || newIx == 0)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	/**st_id3v2_prf(">>>>st_id3v2_fnc_moveFld: %u-->%u\n",
			curIx, newIx);**/

	pDL = &pTagI->flds.list;
	/* move element */
	if (! st_dynlist_moveElement(pDL, curIx, newIx))
		return ST_ERR_FAIL;
	/* update uniqueIx */
	/** */
	if (newIx > 1) {
		if (! st_dynlist_ite_goto(pDL, newIx - 1))
			return ST_ERR_FAIL;
		pItFld = (Tst_id3v2_fldData*)st_dynlist_ite_getCurrent(pDL);
		ST_ASSERTN_NUM(ST_ERR_FAIL,
				pItFld == NULL || pItFld->pObInternal == NULL)

		uix = ((Tst_id3v2_fldData_intn*)pItFld->pObInternal)->uniqueIx + 1;
		if (! st_dynlist_ite_gotoNext(pDL))
			return ST_ERR_FAIL;
	} else if (! st_dynlist_ite_gotoFirst(pDL))
		return ST_ERR_FAIL;
	/** */
	do {
		pItFld = (Tst_id3v2_fldData*)st_dynlist_ite_getCurrent(pDL);
		ST_ASSERTN_NUM(ST_ERR_FAIL,
				pItFld == NULL || pItFld->pObInternal == NULL)

		((Tst_id3v2_fldData_intn*)pItFld->pObInternal)->uniqueIx = uix++;
	} while (st_dynlist_ite_gotoNext(pDL));
	pTagI->flds.uniqueIxLast = uix;

	/* DEBUG *
	st_dynlist_ite_gotoFirst(pDL);
	do {
		pItFld = (Tst_id3v2_fldData*)st_dynlist_ite_getCurrent(pDL);

		st_id3v2_prf(">>> uix %u\n",
				((Tst_id3v2_fldData_intn*)pItFld->pObInternal)->uniqueIx);
	} while (st_dynlist_ite_gotoNext(pDL));
	st_id3v2_prf(">>> lastuix %u\n",
				pTagI->flds.uniqueIxLast);
	* DEBUG */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_TIV2__fnc_getFirstValidFld_byID(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID id, ST_OPTARG(const Tst_str *pIDstr),
		ST_OPTARG(Tst_id3v2_fldData **ppDat), ST_OPTARG(Tst_uint32 *pNr))
{
	Tst_uint32 inr,
	           *pINr = &inr;
	Tst_id3v2_fldData      *pItFld = NULL;
	Tst_id3v2_fldData_intn *pItFldI;

	ST_ASSERTN_VOID(pTag == NULL || pTag->pObInternal == NULL ||
			(ppDat == NULL && pNr == NULL))

	if (ppDat != NULL)
		*ppDat = NULL;
	if (pNr != NULL)
		pINr = pNr;
	*pINr = ST_ID3V2_MAX_TFLD_CNT + 1;
	if (id == ST_ID3V2_FID_NONE && pIDstr == NULL)
		return;

	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_VOID(pItFldI == NULL)

		if (pItFldI->fnr < *pINr &&
				st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
							id, (const char*)pIDstr)) {
			*pINr = pItFldI->fnr;
			if (ppDat != NULL)
				*ppDat = pItFld;
		}
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Get field number (amount of fields with the same ID) for a new field
 *
 * pFlds   MUST be set
 *
 * returns: number of field
 *          or 0 on failure
 */
static Tst_uint32
ST_TIV2__fnc_getNewFldNr(Tst_id3v2_tag *pTag,
		const Tst_id3v2_frID frID, ST_OPTARG(const Tst_str *pFrIDstr))
{
	Tst_uint32 nr = 0;
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;

	ST_ASSERTN_NUM(0, pTag == NULL || pTag->pObInternal == NULL)

	while ((pItFld = st_id3v2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(0, pItFldI == NULL)

		if (st_id3v2_fnc_matchFrIDs(pItFldI->fldPrI.id, pItFldI->fldPrI.pIDstr,
					frID, (const char*)pFrIDstr))
			++nr;
	}
	return nr;
}

/*----------------------------------------------------------------------------*/

static Tst_bool
ST_TIV2__fnc_isInRWOarr(const Tst_id3v2_fldd_rword_typ rwotp,
		const Tst_id3v2_fldd_rwordPArr pRdWrOrderArr, const Tst_uint32 rwoArrSz)
{
	Tst_uint32 x;

	for (x = 0; x < rwoArrSz; x++)
		if (pRdWrOrderArr[x] == rwotp)
			return ST_B_TRUE;
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

static const Tst_id3v2_picTp_arrElem*
ST_TIV2__fnc_getPicTp_arrE(const Tst_byte bin,
		const Tst_str *pCap, const Tst_str *pCapSh, const Tst_id3v2_picTp ptpE)
{
	Tst_id3v2_picTp_arrElem const *pElem;
	Tst_uint32 slenC = 0,
	           slenS = 0,
	           x     = 0;

	if (pCap)
		slenC = st_sysStrlen2(pCap);
	if (pCapSh)
		slenS = st_sysStrlen2(pCapSh);
	if (bin == 0xff && slenC < 1 && slenS < 1 && ptpE == ST_ID3V2_PT_NONE)
		return NULL;
	/* */
	pElem = (Tst_id3v2_picTp_arrElem*)&ST_ID3V2_PICTYPES[x];
	while (pElem->ptpE != ST_ID3V2_PT_NONE && pElem->bin < 0xff) {
		if (slenC > 0) {
			if (st_sysStrlen2(pElem->pCap) == slenC &&
					st_sysStrcmpN2(ST_B_FALSE, slenC, pElem->pCap, pCap))
				return pElem;
		} else if (slenS > 0) {
			if (st_sysStrlen2(pElem->pShort) == slenS &&
					st_sysStrcmpN2(ST_B_FALSE, slenS, pElem->pShort, pCapSh))
				return pElem;
		} else if (bin != 0xff) {
			if (pElem->bin == bin)
				return pElem;
		} else {
			if (pElem->ptpE == ptpE)
				return pElem;
		}
		pElem = (Tst_id3v2_picTp_arrElem*)&ST_ID3V2_PICTYPES[++x];
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/

static Tst_uint32
ST_TIV2__fnc_getListIndex_byUIX(Tst_id3v2_tag_intn *pTagI,
		const Tst_uint32 uniqueIx)
{
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;

	/* this function shall not use st_id3v2_ite_nextFld & Co. */

	if (! st_dynlist_ite_gotoFirst(&pTagI->flds.list))
		return 0;
	do {
		pItFld = (Tst_id3v2_fldData*)st_dynlist_ite_getCurrent(&pTagI->flds.list);
		ST_ASSERTN_NUM(0, pItFld == NULL)
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(0, pItFldI == NULL)

		if (pItFldI->uniqueIx == uniqueIx)
			return st_dynlist_ite_getCurrentIndex(&pTagI->flds.list);
	} while (st_dynlist_ite_gotoNext(&pTagI->flds.list));
	return 0;
}

static Tst_err
ST_TIV2__fnc_sortFields_byPrio_sub1(Tst_id3v2_tag *pTag,
		const Tst_bool sortPicTypes, Tst_id3v2_fieldsOutp *pPtArr)
{
#	define LOC_CHECKRES_  { if (res != ST_ERR_SUCC) return res; }
	const Tst_uint32 cMAX_FTP    = sizeof(ST_ID3V2_PRIO_FTP) / sizeof(Tst_id3v2_frTp),
	                 cMAX_TXTFID = sizeof(ST_ID3V2_PRIO_TXTFID) / sizeof(Tst_id3v2_frID),
	                 cMAX_COMFID = sizeof(ST_ID3V2_PRIO_COMFID) / sizeof(Tst_id3v2_frID),
	                 cMAX_WXXFID = sizeof(ST_ID3V2_PRIO_WXXFID) / sizeof(Tst_id3v2_frID),
	                 cMAX_PICTP  = sizeof(ST_ID3V2_PRIO_PICTP) / sizeof(Tst_id3v2_picTp);
	Tst_err    res;
	Tst_uint32 prioX = 0,
	           prioY = 0;
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;

	/* reset wasAddedToPrio */
	while ((pItFld = st_id3v2_ite_nextFld(pTag, pItFld)) != NULL) {
		pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

		pItFldI->wasAddedToPrio = ST_B_FALSE;
	}

	/* search frames in order of priority */
	for (prioX = 0; prioX < cMAX_FTP; prioX++) {
		if (ST_ID3V2_PRIO_FTP[prioX] == ST_ID3V2_FTP_TXT) {
			/* search those Text-Frames first those
			 *   ID is ST_ID3V2_PRIO_TXTFID[x] */
			for (prioY = 0; prioY < cMAX_TXTFID; prioY++) {
				res = ST_TIV2__fnc_sortFields_byPrio_sub2(pTag, pPtArr,
						ST_ID3V2_FTP_TXT, ST_ID3V2_PRIO_TXTFID[prioY],
						ST_ID3V2_PT_NONE);
				LOC_CHECKRES_
			}
		} else if (ST_ID3V2_PRIO_FTP[prioX] == ST_ID3V2_FTP_COM) {
			/* search those COMM/USLT-Frames first those
			 *   ID is ST_ID3V2_PRIO_COMFID[x] */
			for (prioY = 0; prioY < cMAX_COMFID; prioY++) {
				res = ST_TIV2__fnc_sortFields_byPrio_sub2(pTag, pPtArr,
						ST_ID3V2_FTP_COM, ST_ID3V2_PRIO_COMFID[prioY],
						ST_ID3V2_PT_NONE);
				LOC_CHECKRES_
			}
		} else if (ST_ID3V2_PRIO_FTP[prioX] == ST_ID3V2_FTP_WXX) {
			/* search those T/WXXX-Frames first those
			 *   ID is ST_ID3V2_PRIO_WXXFID[x] */
			for (prioY = 0; prioY < cMAX_WXXFID; prioY++) {
				res = ST_TIV2__fnc_sortFields_byPrio_sub2(pTag, pPtArr,
						ST_ID3V2_FTP_WXX, ST_ID3V2_PRIO_WXXFID[prioY],
						ST_ID3V2_PT_NONE);
				LOC_CHECKRES_
			}
		} else if (ST_ID3V2_PRIO_FTP[prioX] == ST_ID3V2_FTP_PIC &&
				sortPicTypes) {
			/* search those APIC-Frames first those
			 *   picType is ST_ID3V2_PRIO_PICTP[x] */
			for (prioY = 0; prioY < cMAX_PICTP; prioY++) {
				res = ST_TIV2__fnc_sortFields_byPrio_sub2(pTag, pPtArr,
						ST_ID3V2_FTP_PIC, ST_ID3V2_FID_NONE,
						ST_ID3V2_PRIO_PICTP[prioY]);
				LOC_CHECKRES_
			}
		}
		/* search remaining frames w/ priority */
		res = ST_TIV2__fnc_sortFields_byPrio_sub2(pTag, pPtArr,
				ST_ID3V2_PRIO_FTP[prioX], ST_ID3V2_FID_NONE, ST_ID3V2_PT_NONE);
		LOC_CHECKRES_
	}
	/* search frames w/o priority */
	res = ST_TIV2__fnc_sortFields_byPrio_sub2(pTag, pPtArr,
			ST_ID3V2_FTP_NONE, ST_ID3V2_FID_NONE, ST_ID3V2_PT_NONE);
	LOC_CHECKRES_

	return ST_ERR_SUCC;
#	undef LOC_CHECKRES_
}

static Tst_err
ST_TIV2__fnc_sortFields_byPrio_sub2(Tst_id3v2_tag *pTag,
		Tst_id3v2_fieldsOutp *pPtArr,
		const Tst_id3v2_frTp prioFTP, const Tst_id3v2_frID prioFID,
		const Tst_id3v2_picTp prioPtp)
{
	Tst_uint32 fnr,
	           fcnt     = st_id3v2_gs_getTag_amountFlds(pTag),
	           fnrMax   = ST_ID3V2_MAX_TFLDID_CNT + 1,
	           ftpIxMin = 0,
	           ftpIxMax = fcnt,
	           ix       = 0;
	Tst_id3v2_fldData      *pItFld  = NULL;
	Tst_id3v2_fldData_intn *pItFldI = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	/**st_id3v2_prf("\nST_TIV2__fnc_sortFields_byPrio_sub2(): starting\n");**/
	if (prioFTP != ST_ID3V2_FTP_NONE || prioFID != ST_ID3V2_FID_NONE) {
		if (prioFID != ST_ID3V2_FID_NONE)
			fnrMax = 0;
		if (prioFTP != ST_ID3V2_FTP_NONE) {
			ftpIxMin = ST_ID3V2_MAX_TFLD_CNT + 1;
			ftpIxMax = 0;
		}
		/* */
		while ((pItFld = st_id3v2_ite_nextValidFld_wIx(pTag, pItFld, &ix)) != NULL) {
			pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
			ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

			if (prioFTP != ST_ID3V2_FTP_NONE &&
					pItFldI->fldPrI.typ == prioFTP) {
				if (ftpIxMin > ST_ID3V2_MAX_TFLD_CNT)
					ftpIxMin = ix;
				ftpIxMax = ix;
			}
			if (prioFID != ST_ID3V2_FID_NONE &&
					pItFldI->fldPrI.id == prioFID &&
					pItFldI->fnr > fnrMax)
				fnrMax = pItFldI->fnr;
		}
		/* */
		if (prioFID != ST_ID3V2_FID_NONE) {
			if (fnrMax < 1)
				return ST_ERR_SUCC;
			++fnrMax;
		}
		if (prioFTP != ST_ID3V2_FTP_NONE) {
			if (ftpIxMin > ST_ID3V2_MAX_TFLD_CNT || ftpIxMax < 1)
				return ST_ERR_SUCC;
			++ftpIxMax;
		}
	}
	if (fnrMax < ST_ID3V2_MAX_TFLD_CNT + 1) {
		if (fnrMax > fcnt + 1)
			fnrMax = fcnt + 1;
	} else
		fnrMax = 0;
	if (ftpIxMax > fcnt && ftpIxMin < ST_ID3V2_MAX_TFLD_CNT + 1)
		ftpIxMax = fcnt;
	if (ftpIxMin == 0)
		ftpIxMin = 1;
	/* */
	for (fnr = 1; fnr < fnrMax; fnr++) {
		for (ix = ftpIxMin; ix <= ftpIxMax; ix++) {
			/**st_id3v2_prf("ST_TIV2__fnc_sortFields_byPrio_sub2(): "
					"ix %04u  cnt %u  (fnr %u - %u, ftp %u - %u)\n",
					ix, fcnt, 1, fnrMax - 1, ftpIxMin, ftpIxMax);**/
			pItFld = (Tst_id3v2_fldData*)st_dynlist_getElement(&pTagI->flds.list, ix);
			ST_ASSERTN_NUM(ST_ERR_FAIL, pItFld == NULL)
			pItFldI = (Tst_id3v2_fldData_intn*)pItFld->pObInternal;
			ST_ASSERTN_NUM(ST_ERR_FAIL, pItFldI == NULL)

			if (! pItFldI->isOK ||
					pItFldI->wasAddedToPrio ||
					(prioFTP != ST_ID3V2_FTP_NONE && pItFldI->fldPrI.typ != prioFTP) ||
					(prioFID != ST_ID3V2_FID_NONE && pItFldI->fldPrI.id != prioFID) ||
					(prioFTP == ST_ID3V2_FTP_PIC && prioPtp != ST_ID3V2_PT_NONE &&
						pItFldI->attrPicTp != prioPtp) ||
					pItFldI->fnr != fnr)
				continue;
			if (pPtArr->cnt >= pPtArr->sz)
				return ST_ERR_FAIL;
			pPtArr->ppFldArr[pPtArr->cnt++] = pItFld;
			pItFldI->wasAddedToPrio         = ST_B_TRUE;
		}
	}
	return ST_ERR_SUCC;
}

/******************************************************************************/
