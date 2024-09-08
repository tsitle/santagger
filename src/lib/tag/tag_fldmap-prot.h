/******************************************************************************/
/* tag_fldmap-prot.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for converting Tag Fields                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_FLDMAP_PROT_H
#define HEAD_TAG_FLDMAP_PROT_H

ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_TAG_FLDMAP_C) || defined(SRC_TAG_FLDMAP_GS_C) || \
			defined(SRC_TAG_FLDMAP_STCS_C)
	/** */
	typedef enum {
		ST_TFLDMAP_TTP_IV1,
		ST_TFLDMAP_TTP_IV2,
		ST_TFLDMAP_TTP_AV2,
		ST_TFLDMAP_TTP_VOR,
		ST_TFLDMAP_TTP_NONE
	} Tst_tfldMap_ttp;
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_TAG_FLDMAP_C) || defined(SRC_TAG_FLDMAP_GS_C) || \
			defined(SRC_TAG_FLDMAP_STCS_C)
	/** */
	typedef struct {
		/* general */
		Tst_bool            modeAddOrSet;
		Tst_tfldMap_ttp     ttp;
		/* field ID */
		Tst_int32           fldID;
		Tst_str             *pFldIDstr;
		/* field nr. */
		Tst_int32           fldNr;
		/* data */
		Tst_bool            isDataStrSet;
		Tst_bool            isDataBinSet;
		Tst_bool            isDataUValSet;
		Tst_bool            isDataBValSet;
		Tst_mtes_strArr     dataStrArr;
		Tst_binobj          dataBin;
		Tst_uint32          dataUVal;
		Tst_byte            dataBVal;
		/* attributes */
		Tst_bool            isAttrDescSet;
		Tst_bool            isAttrLangSet;
		Tst_bool            isAttrGeoFnSet;
		Tst_bool            isAttrGeoMimeSet;
		Tst_bool            isAttrPicTpSet;
		Tst_bool            isAttrPicFmtSet;
		Tst_bool            isAttrSltTsfSet;
		Tst_bool            isAttrSltCtpSet;
		Tst_mtes_string     attrDesc;
		Tst_str             attrLangISO[4];
		Tst_mtes_string     attrGeoFilen;
		Tst_str             *pAttrGeoMimeISO;
		Tst_id3v2_picTp     attrPicTp;
		Tst_utilsFmt_picFmt attrPicFmt;
		Tst_id3v2_sltTSF    attrSltTSF;
		Tst_id3v2_sltCTp    attrSltCTP;
	} Tst_tfldMap_genTagFld_intn;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_FLDMAP_PROT_H */

/******************************************************************************/
