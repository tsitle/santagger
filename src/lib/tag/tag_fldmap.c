/******************************************************************************/
/* tag_fldmap.h                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for mapping Tag Fields                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.04.2011 (dd.mm.yyyy)                                     */
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
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_id3v1.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_vorbc.h"
#include "src/includes/tag/tag_fldmap.h"
/** */
#define SRC_TAG_FLDMAP_C
#include "tag_fldmap-prot.h"
/*** */
#include "tag_fldmap_zid-prot.h"
#include "apev2/tag_apev2_gs-prot.h"
#include "id3v1/tag_id3v1_gs-prot.h"
//#include "id3v1/tag_id3v1-pp_flds.h"
#include "id3v2/tag_id3v2_gs-prot.h"
#include "vorbc/tag_vorbc_gs-prot.h"
#undef SRC_TAG_FLDMAP_C

/*
// System-Includes
*/
#include <string.h>      /* memset() */
#include <stdlib.h>      /* free() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TFMAP__mapToAV2_getID(const Tst_tfldMap_ttp srcTTP,
                         Tst_int32 fid, const Tst_str *pFIDstr,
                         Tst_apev2_frID *pDstID);
static Tst_err
ST_TFMAP__mapToIV1_getID(const Tst_tfldMap_ttp srcTTP,
                         Tst_int32 fid, const Tst_str *pFIDstr,
                         Tst_id3v1_frID *pDstID);
static Tst_err
ST_TFMAP__mapToIV2_getID(const Tst_tfldMap_ttp srcTTP,
                         Tst_int32 fid, const Tst_str *pFIDstr,
                         Tst_id3v2_frID *pDstID);
static Tst_err
ST_TFMAP__mapToVOR_getID(const Tst_tfldMap_ttp srcTTP,
                         Tst_int32 fid, const Tst_str *pFIDstr,
                         Tst_vorbc_frID *pDstID);
/** */
static Tst_int32
ST_TFMAP__getIDfromStr(const Tst_tfldMap_ttp srcTTP, const Tst_str *pFIDstr);
/** */
static Tst_err
ST_TFMAP__addIntStr_asInt_toIV1(Tst_tfldMap_genTagFld *pGTF,
                                const Tst_bool isPosStr,
                                Tst_id3v1_fldData *pOut,
                                Tst_bool *pIsOK);
static Tst_err
ST_TFMAP__addGenreStr_asInt_toIV1(Tst_tfldMap_genTagFld *pGTF,
                                  Tst_id3v1_fldData *pOut,
                                  Tst_bool *pIsOK);

#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define LOC_PRF_  printf
#else
	/* if LOC_PRF_ is used somewhere we'll get an compiler error */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Map a generic tag field to a ID3v1 tag field
 */
Tst_err
st_tagFldMap_mapToIV1(Tst_tfldMap_genTagFld *pFld, Tst_id3v1_fldData *pOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_str *pDstIDstr = NULL;
	Tst_id3v1_frID             dstID  = ST_ID3V1_FID_NONE;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_id3v1_stc_rsetFDat(pOut);

	/* map the ID */
	res = st_tagFldMap_mapToIV1_onlyID(pFld, &dstID, &pDstIDstr,
			pModeAddOrSet, pFNr, pIsOK);

	/* now we actually map the field */
	if (res == ST_ERR_SUCC && dstID != ST_ID3V1_FID_NONE) {
		/* set field data/attributes */
		/** ID */
		/**LOC_PRF_(" map2IV1: dstID %3d\n", dstID);**/
		res = st_id3v1_gs_setField_props(pOut, dstID);
		/** data */
		if (res == ST_ERR_SUCC && pGTFI->isDataStrSet &&
				st_mtes_strArrGetElemCount(&pGTFI->dataStrArr) > 0) {
			/* handle some special cases */
			switch (dstID) {
			case ST_ID3V1_FID_TRKN:
			case ST_ID3V1_FID_YEAR:
				res = ST_TFMAP__addIntStr_asInt_toIV1(pFld,
						/*isPosStr:*/(dstID == ST_ID3V1_FID_TRKN),
						pOut, pIsOK);
				break;
			case ST_ID3V1_FID_GENR:
				res = ST_TFMAP__addGenreStr_asInt_toIV1(pFld,
						pOut, pIsOK);
				break;
			default:
				res = st_id3v1_gs_setFieldData_string(pOut,
						st_mtes_strArrGetElem(&pGTFI->dataStrArr, 1));
				/* */
				*pIsOK = (res == ST_ERR_SUCC);
			}
		}
	}

	ST_DELPOINT(pDstIDstr)
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Map a generic tag field to a ID3v2 tag field
 */
Tst_err
st_tagFldMap_mapToIV2(Tst_tfldMap_genTagFld *pFld, Tst_id3v2_fldData *pOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_str *pDstIDstr = NULL;
	Tst_id3v2_frID             dstID  = ST_ID3V2_FID_NONE;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_id3v2_stc_rsetFDat(pOut);

	/* map the ID */
	res = st_tagFldMap_mapToIV2_onlyID(pFld, &dstID, &pDstIDstr,
			pModeAddOrSet, pFNr, pIsOK);

	/* now we actually map the field */
	if (res == ST_ERR_SUCC && dstID != ST_ID3V2_FID_NONE) {
		/* set field data/attributes */
		/** ID and ID-String */
		/**LOC_PRF_(" map2IV2: dstID %3d '%s'\n",
				dstID, (pDstIDstr ? (const char*)pDstIDstr : ""));**/
		res = st_id3v2_gs_setField_props(pOut, dstID, pDstIDstr);
		/** data */
		if (res == ST_ERR_SUCC && pGTFI->isDataStrSet) {
			res    = st_id3v2_gs_setFieldData_stringArr(pOut, &pGTFI->dataStrArr);
			*pIsOK = (res == ST_ERR_SUCC);
		} else if (res == ST_ERR_SUCC && pGTFI->isDataBinSet) {
			res    = st_id3v2_gs_setFieldData_bin(pOut, &pGTFI->dataBin);
			*pIsOK = (res == ST_ERR_SUCC);
		} else if (res == ST_ERR_SUCC && pGTFI->isDataUValSet) {
			res    = st_id3v2_gs_setFieldData_uval(pOut, pGTFI->dataUVal);
			*pIsOK = (res == ST_ERR_SUCC);
		} else if (res == ST_ERR_SUCC && pGTFI->isDataBValSet) {
			res    = st_id3v2_gs_setFieldData_bval(pOut, pGTFI->dataBVal);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		/** attributes */
		if (res == ST_ERR_SUCC && pGTFI->isAttrDescSet) {
			res    = st_id3v2_gs_setFieldAttr_desc(pOut, &pGTFI->attrDesc);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrLangSet) {
			res    = st_id3v2_gs_setFieldAttr_lang(pOut, pGTFI->attrLangISO);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrGeoFnSet) {
			res    = st_id3v2_gs_setFieldAttr_geobFn(pOut, &pGTFI->attrGeoFilen);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrGeoMimeSet) {
			res    = st_id3v2_gs_setFieldAttr_geobMime(pOut, pGTFI->pAttrGeoMimeISO);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrPicTpSet) {
			res    = st_id3v2_gs_setFieldAttr_picTp(pOut, pGTFI->attrPicTp);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrPicFmtSet) {
			res    = st_id3v2_gs_setFieldAttr_picFmt(pOut, pGTFI->attrPicFmt);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrSltTsfSet) {
			res    = st_id3v2_gs_setFieldAttr_sltTSF(pOut, pGTFI->attrSltTSF);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrSltCtpSet) {
			res    = st_id3v2_gs_setFieldAttr_sltCTP(pOut, pGTFI->attrSltCTP);
			*pIsOK = (res == ST_ERR_SUCC);
		}
	}

	ST_DELPOINT(pDstIDstr)
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Map a generic tag field to a APEv2 tag field
 */
Tst_err
st_tagFldMap_mapToAV2(Tst_tfldMap_genTagFld *pFld, Tst_apev2_fldData *pOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_str *pDstIDstr = NULL;
	Tst_apev2_frID             dstID  = ST_APEV2_FID_NONE;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_apev2_stc_rsetFDat(pOut);

	/* map the ID */
	res = st_tagFldMap_mapToAV2_onlyID(pFld, &dstID, &pDstIDstr,
			pModeAddOrSet, pFNr, pIsOK);

	/* now we actually map the field */
	if (res == ST_ERR_SUCC && dstID != ST_APEV2_FID_NONE) {
		/* set field data/attributes */
		/** ID and ID-String */
		/**LOC_PRF_(" map2AV2: dstID %3d '%s'\n",
				dstID, (pDstIDstr ? (const char*)pDstIDstr : ""));**/
		res = st_apev2_gs_setField_props(pOut, dstID, pDstIDstr);
		/** data */
		if (res == ST_ERR_SUCC && pGTFI->isDataStrSet) {
			res    = st_apev2_gs_setFieldData_stringArr(pOut, &pGTFI->dataStrArr);
			*pIsOK = (res == ST_ERR_SUCC);
		} else if (res == ST_ERR_SUCC && pGTFI->isDataBinSet) {
			res    = st_apev2_gs_setFieldData_geoBin(pOut, &pGTFI->dataBin);
			*pIsOK = (res == ST_ERR_SUCC);
		}
	}

	ST_DELPOINT(pDstIDstr)
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Map a generic tag field to a Vorbis tag field
 */
Tst_err
st_tagFldMap_mapToVOR(Tst_tfldMap_genTagFld *pFld, Tst_vorbc_fldData *pOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_str *pDstIDstr = NULL;
	Tst_vorbc_frID             dstID  = ST_VORBC_FID_NONE;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;

	st_vorbc_stc_rsetFDat(pOut);

	/* map the ID */
	res = st_tagFldMap_mapToVOR_onlyID(pFld, &dstID, &pDstIDstr,
			pModeAddOrSet, pFNr, pIsOK);

	/* now we actually map the field */
	if (res == ST_ERR_SUCC && dstID != ST_VORBC_FID_NONE) {
		/* set field data/attributes */
		/** ID and ID-String */
		/**LOC_PRF_(" map2VOR: dstID %3d '%s'\n",
				dstID, (pDstIDstr ? (const char*)pDstIDstr : ""));**/
		res = st_vorbc_gs_setField_props(pOut, dstID, pDstIDstr);
		/** data */
		if (res == ST_ERR_SUCC && pGTFI->isDataStrSet) {
			res    = st_vorbc_gs_setFieldData_stringArr(pOut, &pGTFI->dataStrArr);
			*pIsOK = (res == ST_ERR_SUCC);
		} else if (res == ST_ERR_SUCC && pGTFI->isDataBinSet) {
			res    = st_vorbc_gs_setFieldData_picBin(pOut, &pGTFI->dataBin);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		/** attributes */
		if (res == ST_ERR_SUCC && pGTFI->isAttrDescSet) {
			res    = st_vorbc_gs_setFieldAttr_desc(pOut, &pGTFI->attrDesc);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrPicTpSet) {
			res = st_vorbc_gs_setFieldAttr_picTp(
					pOut,
					st_tagCFnc_convId3v2ToVorbc_picTp(pGTFI->attrPicTp)
				);
			*pIsOK = (res == ST_ERR_SUCC);
		}
		if (res == ST_ERR_SUCC && pGTFI->isAttrPicFmtSet) {
			res    = st_vorbc_gs_setFieldAttr_picFmt(pOut, pGTFI->attrPicFmt);
			*pIsOK = (res == ST_ERR_SUCC);
		}
	}

	ST_DELPOINT(pDstIDstr)
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Map only the ID and ID-String to ID3v1
 */
Tst_err
st_tagFldMap_mapToIV1_onlyID(Tst_tfldMap_genTagFld *pFld,
		Tst_id3v1_frID *pIDout, Tst_str **ppIDstrOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res = ST_ERR_SUCC;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pIDout == NULL || ppIDstrOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;
	if (pGTFI->ttp == ST_TFLDMAP_TTP_NONE ||
			(pGTFI->fldID < 0 && pGTFI->pFldIDstr == NULL))
		return ST_ERR_IARG;

	*pIDout = ST_ID3V1_FID_NONE;
	res     = st_sysStrcpy2("", ppIDstrOut);
	/* */
	*pModeAddOrSet = pGTFI->modeAddOrSet;
	*pFNr          = -1;
	*pIsOK         = ST_B_FALSE;

	/* try to map field ID or ID-String to an ID3v1-ID */
	if (res == ST_ERR_SUCC &&
			(pGTFI->fldID >= 0 || ! st_sysStrEmpty(pGTFI->pFldIDstr))) {
		res = ST_TFMAP__mapToIV1_getID(pGTFI->ttp,
				pGTFI->fldID, pGTFI->pFldIDstr, pIDout);
		/* if no ID3v1-ID could be found we try to map to an fallback ID */
		if (res == ST_ERR_SUCC && *pIDout == ST_ID3V1_FID_NONE) {
			if (pGTFI->isDataStrSet)
				*pIDout = ST_ID3V1_FID_COMM;
		}
	}
	/* */
	if (res == ST_ERR_SUCC && *pIDout != ST_ID3V1_FID_NONE) {
		if (! pGTFI->modeAddOrSet && pGTFI->fldNr > 0)
			*pFNr = pGTFI->fldNr;
		if (st_sysStrlen(*ppIDstrOut) == 0)
			res = st_id3v1_fnc_getIDstr_byEnum(*pIDout, ppIDstrOut);
		*pIsOK = (res == ST_ERR_SUCC);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Map only the ID and ID-String to ID3v2
 */
Tst_err
st_tagFldMap_mapToIV2_onlyID(Tst_tfldMap_genTagFld *pFld,
		Tst_id3v2_frID *pIDout, Tst_str **ppIDstrOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pIDout == NULL || ppIDstrOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;
	if (pGTFI->ttp == ST_TFLDMAP_TTP_NONE ||
			(pGTFI->fldID < 0 && pGTFI->pFldIDstr == NULL))
		return ST_ERR_IARG;

	*pIDout = ST_ID3V2_FID_NONE;
	res     = st_sysStrcpy2("", ppIDstrOut);
	/* */
	*pModeAddOrSet = pGTFI->modeAddOrSet;
	*pFNr          = -1;
	*pIsOK         = ST_B_FALSE;

	/* try to map field ID or ID-String to an ID3v2-ID */
	if (res == ST_ERR_SUCC &&
			(pGTFI->fldID >= 0 || ! st_sysStrEmpty(pGTFI->pFldIDstr))) {
		res = ST_TFMAP__mapToIV2_getID(pGTFI->ttp,
				pGTFI->fldID, pGTFI->pFldIDstr, pIDout);
		if (res == ST_ERR_SUCC &&
				(*pIDout == ST_ID3V2_FID_234_CTXT ||
					*pIDout == ST_ID3V2_FID_234_CURL) &&
				! st_sysStrEmpty(pGTFI->pFldIDstr)) {
			/* we have a custom Text/URL frame */
			res = st_sysStrcpy(pGTFI->pFldIDstr, ppIDstrOut);
		}
		if (res == ST_ERR_SUCC &&
				(*pIDout == ST_ID3V2_FID_234_CTXT ||
					*pIDout == ST_ID3V2_FID_234_CURL) &&
				(st_sysStrEmpty(pGTFI->pFldIDstr) ||
					pGTFI->pFldIDstr[0] == '-')) {
			/**LOC_PRF_(" map2IV2: custom dstID %3d but ID-Str '%s'\n",
					*pIDout, (pGTFI->pFldIDstr ?
						(const char*)pGTFI->pFldIDstr : ""));**/
			*pIDout = ST_ID3V2_FID_NONE;
		}
		/* if no ID3v2-ID could be found we try to map to an fallback ID */
		if (res == ST_ERR_SUCC && *pIDout == ST_ID3V2_FID_NONE) {
			if (pGTFI->isDataStrSet) {
				*pIDout = ST_ID3V2_FID_234_COMM;
				res     = st_sysStrcpy2("COMM", ppIDstrOut);
			} else if (pGTFI->isDataBinSet) {
				*pIDout = ST_ID3V2_FID_234_GEOB;
				res     = st_sysStrcpy2("GEOB", ppIDstrOut);
			}
		}
	}
	/* */
	if (res == ST_ERR_SUCC && *pIDout != ST_ID3V2_FID_NONE) {
		if (! pGTFI->modeAddOrSet && pGTFI->fldNr > 0)
			*pFNr = pGTFI->fldNr;
		if (st_sysStrEmpty(*ppIDstrOut))
			res = st_id3v2_fnc_getIDstr_byEnum(*pIDout, 4, ppIDstrOut);
		*pIsOK = (res == ST_ERR_SUCC);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Map only the ID and ID-String to APEv2
 */
Tst_err
st_tagFldMap_mapToAV2_onlyID(Tst_tfldMap_genTagFld *pFld,
		Tst_apev2_frID *pIDout, Tst_str **ppIDstrOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pIDout == NULL || ppIDstrOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;
	if (pGTFI->ttp == ST_TFLDMAP_TTP_NONE ||
			(pGTFI->fldID < 0 && pGTFI->pFldIDstr == NULL))
		return ST_ERR_IARG;

	*pIDout = ST_APEV2_FID_NONE;
	res     = st_sysStrcpy2("", ppIDstrOut);
	/* */
	*pModeAddOrSet = pGTFI->modeAddOrSet;
	*pFNr          = -1;
	*pIsOK         = ST_B_FALSE;

	/* try to map field ID or ID-String to an APEv2-ID */
	if (res == ST_ERR_SUCC &&
			(pGTFI->fldID >= 0 || ! st_sysStrEmpty(pGTFI->pFldIDstr))) {
		res = ST_TFMAP__mapToAV2_getID(pGTFI->ttp,
				pGTFI->fldID, pGTFI->pFldIDstr, pIDout);
		if (res == ST_ERR_SUCC &&
				(*pIDout == ST_APEV2_FID_CTXT ||
					*pIDout == ST_APEV2_FID_CBIN)) {
			/* we have a custom Text/Binary frame */
			res = st_sysStrcpy(pGTFI->pFldIDstr, ppIDstrOut);
			if (st_sysStrEmpty(pGTFI->pFldIDstr)) {
				if (*pIDout == ST_APEV2_FID_CBIN)
					res = st_sysStrcpy2("GEOB", ppIDstrOut);
			} else
				res = st_sysStrcpy(pGTFI->pFldIDstr, ppIDstrOut);
		}
		if (res == ST_ERR_SUCC &&
				(*pIDout == ST_APEV2_FID_CTXT ||
					*pIDout == ST_APEV2_FID_CBIN) &&
				(st_sysStrEmpty(pGTFI->pFldIDstr) ||
					pGTFI->pFldIDstr[0] == '-')) {
			if (*pIDout != ST_APEV2_FID_CBIN || st_sysStrEmpty(*ppIDstrOut)) {
				/**LOC_PRF_(" map2AV2: custom dstID %3d but ID-Str '%s'\n",
						*pIDout, (pGTFI->pFldIDstr ?
							(const char*)pGTFI->pFldIDstr : ""));**/
				*pIDout = ST_APEV2_FID_NONE;
			}
		}
		/* if no APEv2-ID could be found we try to map to an fallback ID */
		if (res == ST_ERR_SUCC && *pIDout == ST_APEV2_FID_NONE) {
			if (pGTFI->isDataStrSet) {
				if (st_sysStrEmpty(pGTFI->pFldIDstr)) {
					*pIDout = ST_APEV2_FID_COMM;
					res     = st_sysStrcpy2("COMMENT", ppIDstrOut);
				} else {
					*pIDout = ST_APEV2_FID_CTXT;
					res     = st_sysStrcpy(pGTFI->pFldIDstr, ppIDstrOut);
				}
			} else if (pGTFI->isDataBinSet &&
					(! st_sysStrEmpty(pGTFI->pFldIDstr) ||
						(*pIDout == ST_APEV2_FID_CBIN && ! st_sysStrEmpty(*ppIDstrOut)))) {
				*pIDout = ST_APEV2_FID_CBIN;
				if (*pIDout != ST_APEV2_FID_CBIN || ! st_sysStrEmpty(pGTFI->pFldIDstr))
					res = st_sysStrcpy(pGTFI->pFldIDstr, ppIDstrOut);
			}
		}
	}
	/* */
	if (res == ST_ERR_SUCC && *pIDout != ST_APEV2_FID_NONE) {
		if (! pGTFI->modeAddOrSet && pGTFI->fldNr > 0)
			*pFNr = pGTFI->fldNr;
		if (st_sysStrlen(*ppIDstrOut) == 0)
			res = st_apev2_fnc_getIDstr_byEnum(*pIDout, ppIDstrOut);
		*pIsOK = (res == ST_ERR_SUCC);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Map only the ID and ID-String to Vorbis
 */
Tst_err
st_tagFldMap_mapToVOR_onlyID(Tst_tfldMap_genTagFld *pFld,
		Tst_vorbc_frID *pIDout, Tst_str **ppIDstrOut,
		Tst_bool *pModeAddOrSet, Tst_int32 *pFNr, Tst_bool *pIsOK)
{
	Tst_err res;
	Tst_tfldMap_genTagFld_intn *pGTFI = NULL;

	ST_ASSERTN_IARG(pFld == NULL || pFld->pObInternal == NULL ||
			pIDout == NULL || ppIDstrOut == NULL ||
			pModeAddOrSet == NULL || pFNr == NULL || pIsOK == NULL)

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pFld->pObInternal;
	if (pGTFI->ttp == ST_TFLDMAP_TTP_NONE ||
			(pGTFI->fldID < 0 && pGTFI->pFldIDstr == NULL))
		return ST_ERR_IARG;

	*pIDout = ST_VORBC_FID_NONE;
	res     = st_sysStrcpy2("", ppIDstrOut);
	/* */
	*pModeAddOrSet = pGTFI->modeAddOrSet;
	*pFNr          = -1;
	*pIsOK         = ST_B_FALSE;

	/* try to map field ID or ID-String to an Vorbis-ID */
	if (res == ST_ERR_SUCC &&
			(pGTFI->fldID >= 0 || ! st_sysStrEmpty(pGTFI->pFldIDstr))) {
		res = ST_TFMAP__mapToVOR_getID(pGTFI->ttp,
				pGTFI->fldID, pGTFI->pFldIDstr, pIDout);
		if (res == ST_ERR_SUCC &&
				*pIDout == ST_VORBC_FID_CTXT &&
				! st_sysStrEmpty(pGTFI->pFldIDstr)) {
			/* we have a custom Text frame */
			res = st_sysStrcpy(pGTFI->pFldIDstr, ppIDstrOut);
		}
		if (res == ST_ERR_SUCC &&
				*pIDout == ST_VORBC_FID_CTXT &&
				(st_sysStrEmpty(pGTFI->pFldIDstr) ||
					pGTFI->pFldIDstr[0] == '-')) {
			/**LOC_PRF_(" map2VOR: custom dstID %3d but ID-Str '%s'\n",
					*pIDout, (pGTFI->pFldIDstr ?
						(const char*)pGTFI->pFldIDstr : ""));**/
			*pIDout = ST_VORBC_FID_NONE;
		}
		/* if no Vorbis-ID could be found we try to map to an fallback ID */
		if (res == ST_ERR_SUCC && *pIDout == ST_VORBC_FID_NONE) {
			if (pGTFI->isDataStrSet) {
				if (st_sysStrEmpty(pGTFI->pFldIDstr)) {
					*pIDout = ST_VORBC_FID_DESC;
					res     = st_sysStrcpy2("DESCRIPTION", ppIDstrOut);
				} else {
					*pIDout = ST_VORBC_FID_CTXT;
					res     = st_sysStrcpy(pGTFI->pFldIDstr, ppIDstrOut);
				}
			}
		}
	}
	/* */
	if (res == ST_ERR_SUCC && *pIDout != ST_VORBC_FID_NONE) {
		if (! pGTFI->modeAddOrSet && pGTFI->fldNr > 0)
			*pFNr = pGTFI->fldNr;
		if (st_sysStrlen(*ppIDstrOut) == 0)
			res = st_vorbc_fnc_getIDstr_byEnum(*pIDout, ppIDstrOut);
		*pIsOK = (res == ST_ERR_SUCC);
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TFMAP__mapToAV2_getID(const Tst_tfldMap_ttp srcTTP,
		Tst_int32 fid, const Tst_str *pFIDstr,
		Tst_apev2_frID *pDstID)
{
	*pDstID = ST_APEV2_FID_NONE;

	if (! st_sysStrEmpty(pFIDstr)) {
		Tst_int32 tmpID;

		tmpID = ST_TFMAP__getIDfromStr(srcTTP, pFIDstr);
		if (tmpID >= 0)
			fid = tmpID;
	}
	if (fid >= 0) {
		switch (srcTTP) {
		case ST_TFLDMAP_TTP_IV1:
			*pDstID = st_tfldMap_mapID_IV1_to_AV2((Tst_id3v1_frID)fid);
			break;
		case ST_TFLDMAP_TTP_AV2:
			*pDstID = (Tst_apev2_frID)fid;
			break;
		case ST_TFLDMAP_TTP_VOR:
			*pDstID = st_tfldMap_mapID_VOR_to_AV2((Tst_vorbc_frID)fid);
			break;
		default:  /* ST_TFLDMAP_TTP_IV2 */
			*pDstID = st_tfldMap_mapID_IV2_to_AV2((Tst_id3v2_frID)fid);
		}
	}
	return ST_ERR_SUCC;
}

static Tst_err
ST_TFMAP__mapToIV1_getID(const Tst_tfldMap_ttp srcTTP,
		Tst_int32 fid, const Tst_str *pFIDstr,
		Tst_id3v1_frID *pDstID)
{
	*pDstID = ST_ID3V1_FID_NONE;

	if (! st_sysStrEmpty(pFIDstr)) {
		Tst_int32 tmpID;

		tmpID = ST_TFMAP__getIDfromStr(srcTTP, pFIDstr);
		if (tmpID >= 0)
			fid = tmpID;
	}
	if (fid >= 0) {
		switch (srcTTP) {
		case ST_TFLDMAP_TTP_IV1:
			*pDstID = (Tst_id3v1_frID)fid;
			break;
		case ST_TFLDMAP_TTP_AV2:
			*pDstID = st_tfldMap_mapID_AV2_to_IV1((Tst_apev2_frID)fid);
			break;
		case ST_TFLDMAP_TTP_VOR:
			*pDstID = st_tfldMap_mapID_VOR_to_IV1((Tst_vorbc_frID)fid);
			break;
		default:  /* ST_TFLDMAP_TTP_IV2 */
			*pDstID = st_tfldMap_mapID_IV2_to_IV1((Tst_id3v2_frID)fid);
		}
	}
	return ST_ERR_SUCC;
}

static Tst_err
ST_TFMAP__mapToIV2_getID(const Tst_tfldMap_ttp srcTTP,
		Tst_int32 fid, const Tst_str *pFIDstr,
		Tst_id3v2_frID *pDstID)
{
	*pDstID = ST_ID3V2_FID_NONE;

	if (! st_sysStrEmpty(pFIDstr)) {
		Tst_int32 tmpID;

		tmpID = ST_TFMAP__getIDfromStr(srcTTP, pFIDstr);
		if (tmpID >= 0)
			fid = tmpID;
	}
	if (fid >= 0) {
		switch (srcTTP) {
		case ST_TFLDMAP_TTP_IV1:
			*pDstID = st_tfldMap_mapID_IV1_to_IV2((Tst_id3v1_frID)fid);
			break;
		case ST_TFLDMAP_TTP_AV2:
			*pDstID = st_tfldMap_mapID_AV2_to_IV2((Tst_apev2_frID)fid);
			break;
		case ST_TFLDMAP_TTP_VOR:
			*pDstID = st_tfldMap_mapID_VOR_to_IV2((Tst_vorbc_frID)fid);
			break;
		default:  /* ST_TFLDMAP_TTP_IV2 */
			*pDstID = (Tst_id3v2_frID)fid;
		}
	}
	return ST_ERR_SUCC;
}

static Tst_err
ST_TFMAP__mapToVOR_getID(const Tst_tfldMap_ttp srcTTP,
		Tst_int32 fid, const Tst_str *pFIDstr,
		Tst_vorbc_frID *pDstID)
{
	*pDstID = ST_VORBC_FID_NONE;

	if (! st_sysStrEmpty(pFIDstr)) {
		Tst_int32 tmpID;

		tmpID = ST_TFMAP__getIDfromStr(srcTTP, pFIDstr);
		if (tmpID >= 0)
			fid = tmpID;
	}
	if (fid >= 0) {
		switch (srcTTP) {
		case ST_TFLDMAP_TTP_IV1:
			*pDstID = st_tfldMap_mapID_IV1_to_VOR((Tst_id3v1_frID)fid);
			break;
		case ST_TFLDMAP_TTP_AV2:
			*pDstID = st_tfldMap_mapID_AV2_to_VOR((Tst_apev2_frID)fid);
			break;
		case ST_TFLDMAP_TTP_VOR:
			*pDstID = (Tst_vorbc_frID)fid;
			break;
		default:  /* ST_TFLDMAP_TTP_IV2 */
			*pDstID = st_tfldMap_mapID_IV2_to_VOR((Tst_id3v2_frID)fid);
		}
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_int32
ST_TFMAP__getIDfromStr(const Tst_tfldMap_ttp srcTTP, const Tst_str *pFIDstr)
{
	Tst_int32 resI = -1;
	Tst_apev2_frID fidSrcAV2;
	Tst_id3v1_frID fidSrcIV1;
	Tst_id3v2_frID fidSrcIV2;
	Tst_vorbc_frID fidSrcVOR;

	switch (srcTTP) {
	case ST_TFLDMAP_TTP_IV1:
		fidSrcIV1 = st_id3v1_fnc_getID_byIDstr(pFIDstr);
		if (fidSrcIV1 != ST_ID3V1_FID_NONE)
			resI = (Tst_int32)fidSrcIV1;
		break;
	case ST_TFLDMAP_TTP_AV2:
		fidSrcAV2 = st_apev2_fnc_getID_byIDstr(pFIDstr, ST_APEV2_FTP_NONE);
		if (fidSrcAV2 != ST_APEV2_FID_NONE)
			resI = (Tst_int32)fidSrcAV2;
		break;
	case ST_TFLDMAP_TTP_VOR:
		fidSrcVOR = st_vorbc_fnc_getID_byIDstr(pFIDstr);
		if (fidSrcVOR != ST_VORBC_FID_NONE)
			resI = (Tst_int32)fidSrcVOR;
		break;
	default:  /* ST_TFLDMAP_TTP_IV2 */
		fidSrcIV2 = st_id3v2_fnc_getID_byIDstr(pFIDstr);
		if (fidSrcIV2 != ST_ID3V2_FID_NONE)
			resI = (Tst_int32)fidSrcIV2;
		break;
	}
	return resI;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TFMAP__addIntStr_asInt_toIV1(Tst_tfldMap_genTagFld *pGTF,
		const Tst_bool isPosStr, Tst_id3v1_fldData *pOut, Tst_bool *pIsOK)
{
	Tst_int32                  ival;
	Tst_mtes_string            *pStr1;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pGTF->pObInternal;

	*pIsOK = ST_B_FALSE;
	if (! pGTFI->isDataStrSet) {
		return ST_ERR_SUCC;
	}
	pStr1 = st_mtes_strArrGetElem(&pGTFI->dataStrArr, 1);
	if (pStr1 == NULL) {
		pGTFI->isDataStrSet = ST_B_FALSE;
		return ST_ERR_SUCC;
	}

	if (isPosStr) {
		Tst_err    res;
		Tst_uint32 num    = 0,
		           numTot = 0;

		res = st_tagCFnc_chk_getPosStringNums(pStr1, &num, &numTot);
		if (res != ST_ERR_SUCC || num == 0) {
			return res;
		}
		ival = (Tst_int32)num;
	} else {
		ival = st_mtes_toInt32(pStr1);
		if (ival <= 0) {
			return ST_ERR_SUCC;
		}
	}

	*pIsOK = ST_B_TRUE;
	return st_id3v1_gs_setFieldData_ival(pOut, ival);
}

static Tst_err
ST_TFMAP__addGenreStr_asInt_toIV1(Tst_tfldMap_genTagFld *pGTF,
		Tst_id3v1_fldData *pOut, Tst_bool *pIsOK)
{
	Tst_err   res;
	Tst_int32 ival = -1;
	Tst_mtes_string            *pStr1;
	Tst_tfldMap_genTagFld_intn *pGTFI;

	pGTFI = (Tst_tfldMap_genTagFld_intn*)pGTF->pObInternal;

	*pIsOK = ST_B_FALSE;
	if (! pGTFI->isDataStrSet) {
		return ST_ERR_SUCC;
	}
	pStr1 = st_mtes_strArrGetElem(&pGTFI->dataStrArr, 1);
	if (pStr1 == NULL) {
		pGTFI->isDataStrSet = ST_B_FALSE;
		return ST_ERR_SUCC;
	}

	res = st_tagCFnc_chk_getGenreNum(pStr1, &ival);
	if (res == ST_ERR_SUCC && ival >= 0) {
		*pIsOK = ST_B_TRUE;
		res    = st_id3v1_gs_setFieldData_ival(pOut, ival);
	}
	return res;
}

/******************************************************************************/
