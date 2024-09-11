/******************************************************************************/
/* tag_conv.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for converting Tags                                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.02.2010 (dd.mm.yyyy)                                     */
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
#include "src/includes/utils/utils_fmt.h"
//#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_conv.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_fldmap.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_id3_helper-defs.h"
#include "src/includes/tag/tag_id3v1.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_TAG_CONV_C
#include "tag_conv-priv.h"
/*** */
//#include "tag_fldmap_zid-prot.h"
#include "id3v1/tag_id3v1_stcs-prot.h"
#include "id3v2/tag_id3v2_gs-prot.h"
#include "id3v2/tag_id3v2_stcs-prot.h"
#include "apev2/tag_apev2_gs-prot.h"
#include "apev2/tag_apev2_stcs-prot.h"
#include "vorbc/tag_vorbc_gs-prot.h"
#include "vorbc/tag_vorbc_stcs-prot.h"
#undef SRC_TAG_CONV_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* free() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define LOC_RESET_IV1_(mac_pTag)  \
			st_id3v1_stc_rsetTag_prot(mac_pTag, ST_B_FALSE,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL);
#define LOC_RESET_IV2_(mac_pTag)  \
			st_id3v2_stc_rsetTag_prot(mac_pTag, ST_B_FALSE,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL);
#define LOC_RESET_AV2_(mac_pTag)  \
			st_apev2_stc_rsetTag_prot(mac_pTag, ST_B_FALSE,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL);
#define LOC_RESET_VOR_(mac_pTag)  \
			st_vorbc_stc_rsetTag_prot(mac_pTag, ST_B_FALSE,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL,  \
					ST_B_FALSE, NULL);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Convert any tag type into any other
 *
 * returns: ERRCODE
 *          pTagCv  will be set
 */
Tst_err
st_tagConv_XtoY(const Tst_byte iv2TargVers,
		const Tst_tagConv_tagTp srcTTP, void *pTagSrc,
		const Tst_tagConv_tagTp cvTTP, void *pTagCv)
{
	Tst_err res;

	ST_ASSERTN_IARG(pTagSrc == NULL || pTagCv == NULL || srcTTP == cvTTP)

	switch (srcTTP) {
	case ST_TCNV_TTP_IV1:
		res = ST_TCNV__civ1_toOther(cvTTP, iv2TargVers,
				(Tst_id3v1_tag*)pTagSrc, pTagCv);
		break;
	case ST_TCNV_TTP_IV2:
		res = ST_TCNV__civ2_toOther(cvTTP,
				(Tst_id3v2_tag*)pTagSrc, pTagCv);
		break;
	case ST_TCNV_TTP_AV2:
		res = ST_TCNV__cav2_toOther(cvTTP, iv2TargVers,
				(Tst_apev2_tag*)pTagSrc, pTagCv);
		break;
	case ST_TCNV_TTP_VOR:
		res = ST_TCNV__cvor_toOther(cvTTP, iv2TargVers,
				(Tst_vorbc_tag*)pTagSrc, pTagCv);
		break;
	default:
		res = ST_ERR_IARG;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * From ID3v1 to ID3v2/APEv2/Vorbis/...
 */
static Tst_err
ST_TCNV__civ1_toOther(const Tst_tagConv_tagTp dstTTP, const Tst_byte iv2TargVers,
		const Tst_id3v1_tag *pTag, void *pTagCv)
{
	Tst_err   res;
	Tst_int32 ival;
	Tst_mtes_string       desc,
	                      sval;
	Tst_tfldMap_genTagFld *pGTF = NULL;
	Tst_dynlist           tfldList;

	ST_ASSERTN_IARG(pTag == NULL || pTagCv == NULL ||
			st_id3v1_gs_getTag_filen(pTag) == NULL)

	/* init vars */
	/** */
	res = st_mtes_stc_initStr(&desc);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&sval);
	if (res != ST_ERR_SUCC)
		return res;
	/** */
	res = st_dynlist_stc_initDL(&tfldList, sizeof(Tst_tfldMap_genTagFld),
			ST_TCNV__cbResetGTF, ST_TCNV__cbFreeGTF);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&sval);
		st_mtes_stc_freeStr(&desc);
		return res;
	}
	/** */
	res = st_mtes_setStrTo2(ST_MTES_TE_ISO, "from ID3v1", &desc);

	/* add fields to tfldList */
	/** Artist */
	if (res == ST_ERR_SUCC) {
		res = st_id3v1_gs_getField_artist(pTag, &sval);
		if (res == ST_ERR_SUCC && ! st_mtes_isStrEmptyCh(&sval)) {
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_ID3V1_TAG_NAME_SH, ST_ID3V1_FID_ARTI, &desc);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataStr(pGTF, &sval);
		}
	}
	/** Album */
	if (res == ST_ERR_SUCC) {
		res = st_id3v1_gs_getField_album(pTag, &sval);
		if (res == ST_ERR_SUCC && ! st_mtes_isStrEmptyCh(&sval)) {
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_ID3V1_TAG_NAME_SH, ST_ID3V1_FID_ALBU, &desc);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataStr(pGTF, &sval);
		}
	}
	/** Title */
	if (res == ST_ERR_SUCC) {
		res = st_id3v1_gs_getField_title(pTag, &sval);
		if (res == ST_ERR_SUCC && ! st_mtes_isStrEmptyCh(&sval)) {
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_ID3V1_TAG_NAME_SH, ST_ID3V1_FID_TITL, &desc);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataStr(pGTF, &sval);
		}
	}
	/** Comment */
	if (res == ST_ERR_SUCC) {
		res = st_id3v1_gs_getField_comment(pTag, &sval);
		if (res == ST_ERR_SUCC && ! st_mtes_isStrEmptyCh(&sval)) {
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_ID3V1_TAG_NAME_SH, ST_ID3V1_FID_COMM, &desc);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataStr(pGTF, &sval);
		}
	}
	/** Tracknr. */
	if (res == ST_ERR_SUCC) {
		ival = st_id3v1_gs_getField_tracknr(pTag);
		if (ival != ST_ID3_TFLD_TRACK_NONE_IV1) {
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_ID3V1_TAG_NAME_SH, ST_ID3V1_FID_TRKN, &desc);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataStr_int(pGTF, ival);
		}
	}
	/** Year */
	if (res == ST_ERR_SUCC) {
		ival = st_id3v1_gs_getField_year(pTag);
		if (ival != ST_ID3_TFLD_YEAR_NONE_IV1) {
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_ID3V1_TAG_NAME_SH, ST_ID3V1_FID_YEAR, &desc);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataStr_int(pGTF, ival);
		}
	}
	/** Genre */
	if (res == ST_ERR_SUCC) {
		ival = st_id3v1_gs_getField_genre(pTag);
		if (ival != ST_ID3_TFLD_GENRE_NONE_IV1) {
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_ID3V1_TAG_NAME_SH, ST_ID3V1_FID_GENR, &desc);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataStr_int(pGTF, ival);
		}
	}

	/* add all fields to the dest tag */
	if (res == ST_ERR_SUCC)
		res = ST_TCNV__addFieldsToDest(dstTTP, iv2TargVers,
				NULL, &tfldList, pTagCv);

	st_mtes_stc_freeStr(&desc);
	st_mtes_stc_freeStr(&sval);
	st_dynlist_stc_freeDL(&tfldList);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * From ID3v2 to ID3v1/APEv2/Vorbis/...
 */
static Tst_err
ST_TCNV__civ2_toOther(const Tst_tagConv_tagTp dstTTP,
		Tst_id3v2_tag *pTag, void *pTagCv)
{
	Tst_err res;
	Tst_binobj            binDat;
	Tst_mtes_string       descOrg,
	                      descFrom,
	                      geobFn,
	                      taggerInfo;
	Tst_mtes_strArr       sArrVal;
	Tst_dynlist           tfldList;
	Tst_tfldMap_genTagFld *pGTF   = NULL;
	Tst_id3v2_fldData     *pFldIt = NULL;
	Tst_id3v2_fldProp     fldPr;
	Tst_id3v2_frID        fidIV2;
	Tst_id3v2_frTp        ftpIV2;

	ST_ASSERTN_IARG(pTag == NULL || pTagCv == NULL ||
			st_id3v2_gs_getTag_filen(pTag) == NULL)

	/* init vars */
	/** */
	res = st_mtes_stc_initStr(&descFrom);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&descOrg);
	if (res == ST_ERR_SUCC)
		res = st_binobj_stc_initBO(&binDat);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initSA(&sArrVal);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&geobFn);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&taggerInfo);
	if (res != ST_ERR_SUCC)
		return res;
	/** */
	res = st_dynlist_stc_initDL(&tfldList, sizeof(Tst_tfldMap_genTagFld),
			ST_TCNV__cbResetGTF, ST_TCNV__cbFreeGTF);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&descFrom);
		st_mtes_stc_freeStr(&descOrg);
		st_binobj_stc_freeBO(&binDat);
		st_mtes_stc_freeSA(&sArrVal);
		st_mtes_stc_freeStr(&geobFn);
		st_mtes_stc_freeStr(&taggerInfo);
		return res;
	}
	st_id3v2_stc_rsetFPr(&fldPr);
	/** */
	res = st_mtes_setStrTo2(ST_MTES_TE_ISO, "from ID3v2", &descFrom);
	/** */
	if (res == ST_ERR_SUCC)
		res = st_id3v2_gs_getTag_taggerInfo(pTag, &taggerInfo);

	/* add fields to tfldList */
	while (res == ST_ERR_SUCC &&
			(pFldIt = st_id3v2_ite_nextValidFld(pTag, pFldIt)) != NULL) {
		res = st_id3v2_gs_getField_props(pFldIt, &fldPr);
		if (res != ST_ERR_SUCC)
			break;
		fidIV2 = st_id3v2_gs_getFieldProp_id(&fldPr);
		ftpIV2 = st_id3v2_gs_getFieldProp_typ(&fldPr);
		/* */
		switch (ftpIV2) {
		case ST_ID3V2_FTP_PIC:      /* [A]PIC */
			if (dstTTP == ST_TCNV_TTP_IV1 || dstTTP == ST_TCNV_TTP_AV2 ||
					st_id3v2_gs_getFieldData_binSz(pFldIt) == 0)
				continue;
			{
				Tst_id3v2_picTp     ptp;
				Tst_utilsFmt_picFmt pfmt;

				ptp  = st_id3v2_gs_getFieldAttr_picType(pFldIt);
				pfmt = st_id3v2_gs_getFieldAttr_picFmt(pFldIt);

				res = st_id3v2_gs_getFieldData_bin(pFldIt, &binDat);
				if (res == ST_ERR_SUCC)
					res = st_id3v2_gs_getFieldAttr_desc(pFldIt, &descOrg);
				if (res != ST_ERR_SUCC)
					continue;  /* --> end of while() */
				/* add new generic field to list */
				res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
						ST_ID3V2_TAG_NAME_SH, fidIV2, &descOrg);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setDataBinary(pGTF, &binDat);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setAttrPICTtype(pGTF, ptp);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setAttrPICTformat(pGTF, pfmt);
			}
			break;
		case ST_ID3V2_FTP_GEO:      /* GEO[B] */
			if (dstTTP == ST_TCNV_TTP_IV1 || dstTTP == ST_TCNV_TTP_VOR ||
					st_id3v2_gs_getFieldData_binSz(pFldIt) == 0)
				continue;
			{
				Tst_str const *pGeobMime = NULL;

				res = st_id3v2_gs_getFieldData_bin(pFldIt, &binDat);
				if (res == ST_ERR_SUCC)
					res = st_id3v2_gs_getFieldAttr_desc(pFldIt, &descOrg);
				if (res == ST_ERR_SUCC)
					res = st_id3v2_gs_getFieldAttr_geobFilen(pFldIt, &geobFn);
				if (res != ST_ERR_SUCC)
					continue;  /* --> end of while() */
				pGeobMime = st_id3v2_gs_getFieldAttr_geobMime(pFldIt);
				/* add new generic field to list */
				res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
						ST_ID3V2_TAG_NAME_SH, fidIV2, &descOrg);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setDataBinary(pGTF, &binDat);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setAttrGEOBfilen(pGTF, &geobFn);
				if (res == ST_ERR_SUCC && pGeobMime != NULL)
					res = st_tagFldMap_gs_setAttrGEOBmime(pGTF, pGeobMime);
			}
			break;
		case ST_ID3V2_FTP_COM:      /* COM[M],U[S]LT */
		case ST_ID3V2_FTP_TXT:      /* CM1,T00[0]-TZZ[Z] */
		case ST_ID3V2_FTP_URL:      /* W00[0]-WZZ[Z] */
		case ST_ID3V2_FTP_WXX:      /* TXX[X],WXX[X] */
		case ST_ID3V2_FTP_SLT:      /* S[Y]LT */
		case ST_ID3V2_FTP_IPL:      /* IPL[S] */
			if (st_id3v2_gs_getFieldData_stringArr_elemCount(pFldIt) == 0)
				continue;
			{
				Tst_str const *pLang = NULL;

				/* for SYLT we use the converted lyrics that reside
				 *   within the string array, not the binary data  */

				res = st_id3v2_gs_getFieldData_stringArr(pFldIt, &sArrVal);
				if (res == ST_ERR_SUCC)
					res = st_id3v2_gs_getFieldAttr_desc(pFldIt, &descOrg);
				if (res != ST_ERR_SUCC)
					continue;  /* --> end of while() */
				/* */
				switch (ftpIV2) {
				case ST_ID3V2_FTP_COM:      /* COM[M],U[S]LT */
				case ST_ID3V2_FTP_SLT:      /* S[Y]LT */
					pLang = st_id3v2_gs_getFieldAttr_lang(pFldIt);
					break;
				default:
					break;
				}

				/* merge IPLS multi-string into one single string */
				if (ftpIV2 == ST_ID3V2_FTP_IPL &&
						st_mtes_strArrGetElemCount(&sArrVal) > 1) {
					res = st_tagCFnc_iplsMerge(&sArrVal);
					if (res != ST_ERR_SUCC)
						continue;  /* --> end of while() */
				}

				/* add new generic field to list */
				res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
						ST_ID3V2_TAG_NAME_SH, fidIV2, &descOrg);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setDataStr_sarr(pGTF, &sArrVal);
				if (res == ST_ERR_SUCC && pLang != NULL)
					res = st_tagFldMap_gs_setAttrLang(pGTF, pLang);
			}
			break;
		default:
			break;
		}
	}

	/* add all fields to the dest tag */
	if (res == ST_ERR_SUCC)
		res = ST_TCNV__addFieldsToDest(dstTTP, /*iv2TargVers:*/0,
					&taggerInfo, &tfldList, pTagCv);

	st_mtes_stc_freeStr(&descFrom);
	st_mtes_stc_freeStr(&descOrg);
	st_binobj_stc_freeBO(&binDat);
	st_mtes_stc_freeSA(&sArrVal);
	st_dynlist_stc_freeDL(&tfldList);
	st_mtes_stc_freeStr(&geobFn);
	st_mtes_stc_freeStr(&taggerInfo);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * From APEv2 to ID3v1/ID3v2/Vorbis/...
 */
static Tst_err
ST_TCNV__cav2_toOther(const Tst_tagConv_tagTp dstTTP, const Tst_byte iv2TargVers,
		Tst_apev2_tag *pTag, void *pTagCv)
{
	Tst_err    res;
	Tst_uint32 saecnt;
	Tst_binobj            binDat;
	Tst_mtes_string       descFrom,
	                      taggerInfo;
	Tst_mtes_strArr       sArrVal;
	Tst_dynlist           tfldList;
	Tst_tfldMap_genTagFld *pGTF   = NULL;
	Tst_apev2_fldData     *pFldIt = NULL;
	Tst_apev2_fldProp     fldPr;
	Tst_apev2_frID        fidAV2;

	ST_ASSERTN_IARG(pTag == NULL || pTagCv == NULL ||
			st_apev2_gs_getTag_filen(pTag) == NULL)

	/* init vars */
	/** */
	res = st_mtes_stc_initStr(&descFrom);
	if (res == ST_ERR_SUCC)
		res = st_binobj_stc_initBO(&binDat);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initSA(&sArrVal);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&taggerInfo);
	if (res != ST_ERR_SUCC)
		return res;
	/** */
	res = st_dynlist_stc_initDL(&tfldList, sizeof(Tst_tfldMap_genTagFld),
			ST_TCNV__cbResetGTF, ST_TCNV__cbFreeGTF);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&descFrom);
		st_binobj_stc_freeBO(&binDat);
		st_mtes_stc_freeSA(&sArrVal);
		st_mtes_stc_freeStr(&taggerInfo);
		return res;
	}
	st_apev2_stc_rsetFPr(&fldPr);
	/** */
	res = st_mtes_setStrTo2(ST_MTES_TE_ISO, "from APEv2", &descFrom);
	/** */
	if (res == ST_ERR_SUCC)
		res = st_apev2_gs_getTag_taggerInfo(pTag, &taggerInfo);

	/* add fields to tfldList */
	while (res == ST_ERR_SUCC &&
			(pFldIt = st_apev2_ite_nextValidFld(pTag, pFldIt)) != NULL) {
		res = st_apev2_gs_getField_props(pFldIt, &fldPr);
		if (res != ST_ERR_SUCC)
			break;
		fidAV2 = st_apev2_gs_getFieldProp_id(&fldPr);
		/* */
		switch (st_apev2_gs_getFieldProp_typ(&fldPr)) {
		case ST_APEV2_FTP_BDAT:
			if (dstTTP == ST_TCNV_TTP_IV1 || dstTTP == ST_TCNV_TTP_VOR ||
					st_apev2_gs_getFieldData_geoBinSz(pFldIt) == 0)
				continue;
			res = st_apev2_gs_getFieldData_geoBin(pFldIt, &binDat);
			if (res != ST_ERR_SUCC)
				continue;  /* --> end of while() */
			/* add new generic field to list */
			res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
					ST_APEV2_TAG_NAME_SH, fidAV2, &descFrom);
			if (res == ST_ERR_SUCC)
				res = st_tagFldMap_gs_setDataBinary(pGTF, &binDat);
			if (res == ST_ERR_SUCC) {
				Tst_utilsFmt_mimeTp mimE;

				mimE = st_utilsFmt_getMime_enum_byBObj(&binDat);
				if (mimE != ST_UTILSFMT_MTP_NONE) {
					res = st_tagFldMap_gs_setAttrGEOBmime(pGTF,
								st_utilsFmt_getMime_defMime_byEnum(mimE));
				}
			}
			break;
		case ST_APEV2_FTP_STR:
		case ST_APEV2_FTP_INT:
		case ST_APEV2_FTP_FLT:
		case ST_APEV2_FTP_YEAR:
		case ST_APEV2_FTP_TSTA:
		case ST_APEV2_FTP_LYR:
		case ST_APEV2_FTP_URL:
			saecnt = st_apev2_gs_getFieldData_stringArr_elemCount(pFldIt);
			if (saecnt == 0)
				continue;
			res = st_apev2_gs_getFieldData_stringArr(pFldIt, &sArrVal);
			if (res != ST_ERR_SUCC)
				continue;  /* --> end of while() */
			/* handle some special cases */
			switch (fidAV2) {
			case ST_APEV2_FID_TRKN:
			case ST_APEV2_FID_TRKT:
			case ST_APEV2_FID_DSCN:
			case ST_APEV2_FID_DSCT:
				/* add new generic field to list */
				res = ST_TCNV__addGTFtoList_setPosStr(&tfldList, &pGTF,
						&descFrom, &sArrVal,
						/*isTrackOrDisc:*/(fidAV2 == ST_APEV2_FID_TRKN ||
							fidAV2 == ST_APEV2_FID_TRKT),
						/*isNrOrTot:*/(fidAV2 == ST_APEV2_FID_TRKN ||
							fidAV2 == ST_APEV2_FID_DSCN));
				break;
			case ST_APEV2_FID_IPLS:
				if (saecnt == 1)
					res = st_tagCFnc_iplsSplit(&sArrVal);
			default:
				/* add new generic field to list */
				if (res == ST_ERR_SUCC)
					res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
							ST_APEV2_TAG_NAME_SH, fidAV2, &descFrom);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setDataStr_sarr(pGTF, &sArrVal);
			}
			break;
		default:
			break;
		}
	}

	/* add all fields to the dest tag */
	if (res == ST_ERR_SUCC)
		res = ST_TCNV__addFieldsToDest(dstTTP, iv2TargVers,
					&taggerInfo, &tfldList, pTagCv);

	st_mtes_stc_freeStr(&descFrom);
	st_binobj_stc_freeBO(&binDat);
	st_mtes_stc_freeSA(&sArrVal);
	st_dynlist_stc_freeDL(&tfldList);
	st_mtes_stc_freeStr(&taggerInfo);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * From Vorbis to ID3v1/ID3v2/APEv2/...
 */
static Tst_err
ST_TCNV__cvor_toOther(const Tst_tagConv_tagTp dstTTP, const Tst_byte iv2TargVers,
		Tst_vorbc_tag *pTag, void *pTagCv)
{
	Tst_err    res;
	Tst_uint32 saecnt;
	Tst_binobj            binDat;
	Tst_mtes_string       descFrom,
	                      descOrg,
	                      taggerInfo;
	Tst_mtes_strArr       sArrVal;
	Tst_dynlist           tfldList;
	Tst_tfldMap_genTagFld *pGTF   = NULL;
	Tst_vorbc_fldData     *pFldIt = NULL;
	Tst_vorbc_fldProp     fldPr;
	Tst_vorbc_frID        fidVOR;

	ST_ASSERTN_IARG(pTag == NULL || pTagCv == NULL ||
			st_vorbc_gs_getTag_filen(pTag) == NULL)

	/* init vars */
	/** */
	res = st_mtes_stc_initStr(&descFrom);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&descOrg);
	if (res == ST_ERR_SUCC)
		res = st_binobj_stc_initBO(&binDat);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initSA(&sArrVal);
	if (res == ST_ERR_SUCC)
		res = st_mtes_stc_initStr(&taggerInfo);
	if (res != ST_ERR_SUCC)
		return res;
	/** */
	res = st_dynlist_stc_initDL(&tfldList, sizeof(Tst_tfldMap_genTagFld),
			ST_TCNV__cbResetGTF, ST_TCNV__cbFreeGTF);
	if (res != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&descFrom);
		st_mtes_stc_freeStr(&descOrg);
		st_binobj_stc_freeBO(&binDat);
		st_mtes_stc_freeSA(&sArrVal);
		st_mtes_stc_freeStr(&taggerInfo);
		return res;
	}
	st_vorbc_stc_rsetFPr(&fldPr);
	/** */
	res = st_mtes_setStrTo2(ST_MTES_TE_ISO, "from Vorbis", &descFrom);
	/** */
	if (res == ST_ERR_SUCC)
		res = st_vorbc_gs_getTag_taggerInfo(pTag, &taggerInfo);

	/* add fields to tfldList */
	while (res == ST_ERR_SUCC &&
			(pFldIt = st_vorbc_ite_nextValidFld(pTag, pFldIt)) != NULL) {
		res = st_vorbc_gs_getField_props(pFldIt, &fldPr);
		if (res != ST_ERR_SUCC)
			break;
		fidVOR = st_vorbc_gs_getFieldProp_id(&fldPr);
		/* */
		switch (st_vorbc_gs_getFieldProp_typ(&fldPr)) {
		case ST_VORBC_FTP_PICT:
			if (dstTTP == ST_TCNV_TTP_IV1 || dstTTP == ST_TCNV_TTP_AV2 ||
					st_vorbc_gs_getFieldData_picBinSz(pFldIt) == 0)
				continue;
			{
				Tst_str const *pPicMime = NULL;
				Tst_vorbc_picTp     ptp;
				Tst_utilsFmt_picFmt pfmt;

				ptp = st_vorbc_gs_getFieldAttr_picType(pFldIt);

				if (res == ST_ERR_SUCC)
					res = st_vorbc_gs_getFieldData_picBin(pFldIt, &binDat);
				if (res == ST_ERR_SUCC)
					res = st_vorbc_gs_getFieldAttr_picDesc(pFldIt, &descOrg);
				if (res != ST_ERR_SUCC)
					continue;  /* --> end of while() */
				pPicMime = st_vorbc_gs_getFieldAttr_picMime(pFldIt);
				/* add new generic field to list */
				res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
						ST_VORBC_TAG_NAME_SH, fidVOR, &descOrg);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setDataBinary(pGTF, &binDat);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setAttrPICTtype(
							pGTF,
							st_tagCFnc_convVorbcToId3v2_picTp(ptp)
						);
				if (res == ST_ERR_SUCC && pPicMime != NULL) {
					pfmt = st_utilsFmt_getPicFmt_enum_byMime(pPicMime);
					res  = st_tagFldMap_gs_setAttrPICTformat(pGTF, pfmt);
				}
			}
			break;
		case ST_VORBC_FTP_STR:
			saecnt = st_vorbc_gs_getFieldData_stringArr_elemCount(pFldIt);
			if (saecnt == 0)
				continue;
			res = st_vorbc_gs_getFieldData_stringArr(pFldIt, &sArrVal);
			if (res != ST_ERR_SUCC)
				continue;  /* --> end of while() */
			/* handle some special cases */
			switch (fidVOR) {
			case ST_VORBC_FID_TRKN:
			case ST_VORBC_FID_TRKT:
			case ST_VORBC_FID_DSCN:
			case ST_VORBC_FID_DSCT:
				/* add new generic field to list */
				res = ST_TCNV__addGTFtoList_setPosStr(&tfldList, &pGTF,
						&descFrom, &sArrVal,
						/*isTrackOrDisc:*/(fidVOR == ST_VORBC_FID_TRKN ||
							fidVOR == ST_VORBC_FID_TRKT),
						/*isNrOrTot:*/(fidVOR == ST_VORBC_FID_TRKN ||
							fidVOR == ST_VORBC_FID_DSCN));
				break;
			case ST_VORBC_FID_IPLS:
				if (saecnt == 1)
					res = st_tagCFnc_iplsSplit(&sArrVal);
			default:
				/* add new generic field to list */
				if (res == ST_ERR_SUCC)
					res = ST_TCNV__addGTFtoList(&tfldList, &pGTF,
							ST_VORBC_TAG_NAME_SH, fidVOR, &descFrom);
				if (res == ST_ERR_SUCC)
					res = st_tagFldMap_gs_setDataStr_sarr(pGTF, &sArrVal);
			}
			break;
		default:
			break;
		}
	}

	/* add all fields to the dest tag */
	if (res == ST_ERR_SUCC)
		res = ST_TCNV__addFieldsToDest(dstTTP, iv2TargVers,
					&taggerInfo, &tfldList, pTagCv);

	st_mtes_stc_freeStr(&descFrom);
	st_mtes_stc_freeStr(&descOrg);
	st_binobj_stc_freeBO(&binDat);
	st_mtes_stc_freeSA(&sArrVal);
	st_dynlist_stc_freeDL(&tfldList);
	st_mtes_stc_freeStr(&taggerInfo);

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_TCNV__addGTFtoList(Tst_dynlist *pList, Tst_tfldMap_genTagFld **ppGTF,
		const char *pTagNameSh, const Tst_int32 fldID,
		ST_OPTARG(Tst_mtes_string *pDesc))
{
	Tst_err res;

	/* init new element */
	ST_CALLOC(*ppGTF, Tst_tfldMap_genTagFld*,
			1, sizeof(Tst_tfldMap_genTagFld))
	if (*ppGTF == NULL)
		return ST_ERR_OMEM;
	res = st_tagFldMap_stc_initGTF(*ppGTF);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(*ppGTF)
		return res;
	}
	res = st_dynlist_addElement(pList, *ppGTF);
	/* set mode and input tag type */
	if (res == ST_ERR_SUCC) {
		st_tagFldMap_gs_setMode(*ppGTF, /*addOrSet:*/ST_B_TRUE);
		st_tagFldMap_gs_setFldID(*ppGTF, fldID);
		res = st_tagFldMap_gs_setTagType(*ppGTF, (const Tst_str*)pTagNameSh);
	}
	/* set description */
	if (res == ST_ERR_SUCC && pDesc != NULL)
		res = st_tagFldMap_gs_setAttrDesc(*ppGTF, pDesc);
	return res;
}

static Tst_err
ST_TCNV__addGTFtoList_setPosStr(Tst_dynlist *pList, Tst_tfldMap_genTagFld **ppGTF,
		ST_OPTARG(Tst_mtes_string *pDesc),
		Tst_mtes_strArr *pSArr,
		const Tst_bool isTrackOrDisc, const Tst_bool isNrOrTot)
{
	Tst_err    res;
	Tst_uint32 num    = 0,
	           numTot = 0;
	Tst_mtes_string *pStr1;

	/* */
	pStr1 = st_mtes_strArrGetElem(pSArr, 1);
	if (pStr1 == NULL)
		return ST_ERR_SUCC;
	res = st_tagCFnc_chk_getPosStringNums(pStr1, &num, &numTot);
	if (res != ST_ERR_SUCC || (num <= 0 && numTot <= 0))
		return res;
	/* */
	res = ST_TCNV__addGTFtoList(pList, ppGTF,
			ST_ID3V2_TAG_NAME_SH,
			(isTrackOrDisc ? ST_ID3V2_FID_234_TRCK : ST_ID3V2_FID_234_TPOS),
			pDesc);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	if (isNrOrTot) {
		if (numTot > 0 && num == 0)
			num = numTot;
		numTot = 0;
	} else {
		if (num > 0 && numTot == 0)
			numTot = num;
		num = 0;
	}

	return st_tagFldMap_gs_setDataStr_pos(*ppGTF,
			(Tst_int32)num, (Tst_int32)numTot);
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_TCNV__addFieldsToDest(const Tst_tagConv_tagTp dstTTP,
		const Tst_byte iv2TargVers,
		ST_OPTARG(const Tst_mtes_string *pTaggerInfo),
		Tst_dynlist *pTFldList, void *pTagCv)
{
	Tst_err   res          = ST_ERR_SUCC;
	Tst_bool  modeAddOrSet = ST_B_TRUE,
	          isOK         = ST_B_FALSE;
	Tst_int32 fnr          = -1;
	Tst_tfldMap_genTagFld *pGTF;
	Tst_id3v1_fldData     *pFDataIV1 = NULL;
	Tst_id3v2_fldData     *pFDataIV2 = NULL;
	Tst_apev2_fldData     *pFDataAV2 = NULL;
	Tst_vorbc_fldData     *pFDataVOR = NULL;

	/* reset dest tag and set tagger info */
	switch (dstTTP) {
	case ST_TCNV_TTP_IV1:
		LOC_RESET_IV1_((Tst_id3v1_tag*)pTagCv)
		break;
	case ST_TCNV_TTP_IV2:
		LOC_RESET_IV2_((Tst_id3v2_tag*)pTagCv)
		if (pTaggerInfo != NULL)
			res = st_id3v2_gs_setTag_taggerInfo((Tst_id3v2_tag*)pTagCv, pTaggerInfo);
		if (res == ST_ERR_SUCC)
			res = st_id3v2_gs_setTag_majorVersion((Tst_id3v2_tag*)pTagCv, iv2TargVers);
		break;
	case ST_TCNV_TTP_AV2:
		LOC_RESET_AV2_((Tst_apev2_tag*)pTagCv)
		if (pTaggerInfo != NULL)
			res = st_apev2_gs_setTag_taggerInfo((Tst_apev2_tag*)pTagCv, pTaggerInfo);
		break;
	case ST_TCNV_TTP_VOR:
		LOC_RESET_VOR_((Tst_vorbc_tag*)pTagCv)
		if (pTaggerInfo != NULL)
			res = st_vorbc_gs_setTag_taggerInfo((Tst_vorbc_tag*)pTagCv, pTaggerInfo);
		break;
	default:
		res = ST_ERR_IARG;
	}
	if (res != ST_ERR_SUCC)
		return res;

	/* if pTFldList is empty we're already done */
	if (! st_dynlist_ite_gotoFirst(pTFldList))
		return ST_ERR_SUCC;

	/* init some vars */
	switch (dstTTP) {
	case ST_TCNV_TTP_IV1:
		ST_CALLOC(pFDataIV1, Tst_id3v1_fldData*,
				1, sizeof(Tst_id3v1_fldData))
		if (pFDataIV1 == NULL)
			return ST_ERR_OMEM;
		st_id3v1_stc_initFDat(pFDataIV1);
		break;
	case ST_TCNV_TTP_IV2:
		ST_CALLOC(pFDataIV2, Tst_id3v2_fldData*,
				1, sizeof(Tst_id3v2_fldData))
		if (pFDataIV2 == NULL)
			return ST_ERR_OMEM;
		st_id3v2_stc_initFDat(pFDataIV2);
		break;
	case ST_TCNV_TTP_AV2:
		ST_CALLOC(pFDataAV2, Tst_apev2_fldData*,
				1, sizeof(Tst_apev2_fldData))
		if (pFDataAV2 == NULL)
			return ST_ERR_OMEM;
		st_apev2_stc_initFDat(pFDataAV2);
		break;
	case ST_TCNV_TTP_VOR:
		ST_CALLOC(pFDataVOR, Tst_vorbc_fldData*,
				1, sizeof(Tst_vorbc_fldData))
		if (pFDataVOR == NULL)
			return ST_ERR_OMEM;
		st_vorbc_stc_initFDat(pFDataVOR);
		break;
	default:
		break;
	}

	/* add all fields from pTFldList to pTagCv */
	do {
		pGTF = (Tst_tfldMap_genTagFld*)st_dynlist_ite_getCurrent(pTFldList);
		/* map the field */
		switch (dstTTP) {
		case ST_TCNV_TTP_IV1:
			res = st_tagFldMap_mapToIV1(pGTF,
					pFDataIV1, &modeAddOrSet, &fnr, &isOK);
			break;
		case ST_TCNV_TTP_IV2:
			res = st_tagFldMap_mapToIV2(pGTF,
					pFDataIV2, &modeAddOrSet, &fnr, &isOK);
			break;
		case ST_TCNV_TTP_AV2:
			res = st_tagFldMap_mapToAV2(pGTF,
					pFDataAV2, &modeAddOrSet, &fnr, &isOK);
			break;
		case ST_TCNV_TTP_VOR:
			res = st_tagFldMap_mapToVOR(pGTF,
					pFDataVOR, &modeAddOrSet, &fnr, &isOK);
			break;
		default:
			break;
		}
		if (res != ST_ERR_SUCC)
			break;
		if (! isOK)
			continue;

		/* add the field to the dest tag */
		switch (dstTTP) {
		case ST_TCNV_TTP_IV1:
			res = st_id3v1_fldAdd((Tst_id3v1_tag*)pTagCv, pFDataIV1);
			break;
		case ST_TCNV_TTP_IV2:
			res = st_id3v2_fldAdd((Tst_id3v2_tag*)pTagCv, pFDataIV2);
			break;
		case ST_TCNV_TTP_AV2:
			res = st_apev2_fldAdd((Tst_apev2_tag*)pTagCv, pFDataAV2);
			break;
		case ST_TCNV_TTP_VOR:
			res = st_vorbc_fldAdd((Tst_vorbc_tag*)pTagCv, pFDataVOR);
			break;
		default:
			break;
		}
	} while (res == ST_ERR_SUCC && st_dynlist_ite_gotoNext(pTFldList));

	/* free some vars */
	switch (dstTTP) {
	case ST_TCNV_TTP_IV1:
		st_id3v1_stc_freeFDat(pFDataIV1);
		ST_DELPOINT(pFDataIV1)
		break;
	case ST_TCNV_TTP_IV2:
		st_id3v2_stc_freeFDat(pFDataIV2);
		ST_DELPOINT(pFDataIV2)
		break;
	case ST_TCNV_TTP_AV2:
		st_apev2_stc_freeFDat(pFDataAV2);
		ST_DELPOINT(pFDataAV2)
		break;
	case ST_TCNV_TTP_VOR:
		st_vorbc_stc_freeFDat(pFDataVOR);
		ST_DELPOINT(pFDataVOR)
		break;
	default:
		break;
	}

	/* tidy up tag fields (map fields, remove duplicates, ...) */
	if (res == ST_ERR_SUCC) {
		switch (dstTTP) {
		case ST_TCNV_TTP_IV1:
			res = st_id3v1_tidyUpTag((Tst_id3v1_tag*)pTagCv);
			break;
		case ST_TCNV_TTP_IV2:
			res = st_id3v2_tidyUpTag((Tst_id3v2_tag*)pTagCv, ST_B_TRUE);
			break;
		case ST_TCNV_TTP_AV2:
			res = st_apev2_tidyUpTag((Tst_apev2_tag*)pTagCv);
			break;
		case ST_TCNV_TTP_VOR:
			res = st_vorbc_tidyUpTag((Tst_vorbc_tag*)pTagCv);
			break;
		default:
			break;
		}
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_TCNV__cbResetGTF(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_tagFldMap_stc_rsetGTF((Tst_tfldMap_genTagFld*)pElem);
}

static void
ST_TCNV__cbFreeGTF(void *pElem)
{
	ST_ASSERTN_VOID(pElem == NULL)

	st_tagFldMap_stc_freeGTF((Tst_tfldMap_genTagFld*)pElem);
}

/******************************************************************************/
