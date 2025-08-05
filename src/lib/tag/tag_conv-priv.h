/******************************************************************************/
/* tag_conv-priv.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private functions for converting Tags                                      */
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

#ifndef HEAD_TAG_CONV_PRIV_H
#define HEAD_TAG_CONV_PRIV_H

ST_BEGIN_C_DECLS

/*
// Functions (private)
*/
#ifdef SRC_TAG_CONV_C
	/** IV1-->x */
	static Tst_err
	ST_TCNV__civ1_toOther(const Tst_tagConv_tagTp dstTTP,
	                      const Tst_byte iv2TargVers,
	                      const Tst_id3v1_tag *pTag,
	                      void *pTagCv);
	/** IV2-->x */
	static Tst_err
	ST_TCNV__civ2_toOther(const Tst_tagConv_tagTp dstTTP,
	                      Tst_id3v2_tag *pTag,
	                      void *pTagCv);
	/** AV2-->x */
	static Tst_err
	ST_TCNV__cav2_toOther(const Tst_tagConv_tagTp dstTTP,
	                      const Tst_byte iv2TargVers,
	                      Tst_apev2_tag *pTag,
	                      void *pTagCv);
	/** VOR-->x */
	static Tst_err
	ST_TCNV__cvor_toOther(const Tst_tagConv_tagTp dstTTP,
	                      const Tst_byte iv2TargVers,
	                      Tst_vorbc_tag *pTag,
	                      void *pTagCv);

	/** */
	static Tst_err
	ST_TCNV__addGTFtoList(Tst_dynlist *pList,
	                      Tst_tfldMap_genTagFld **ppGTF,
	                      const char *pTagNameSh,
	                      const Tst_int32 fldID,
	                      ST_OPTARG(Tst_mtes_string *pDesc));
	static Tst_err
	ST_TCNV__addGTFtoList_setPosStr(Tst_dynlist *pList,
	                                Tst_tfldMap_genTagFld **ppGTF,
	                                ST_OPTARG(Tst_mtes_string *pDesc),
	                                Tst_mtes_strArr *pSArr,
	                                const Tst_bool isTrackOrDisc,
	                                const Tst_bool isNrOrTot);
	/** */
	static Tst_err
	ST_TCNV__addFieldsToDest(const Tst_tagConv_tagTp dstTTP,
	                         const Tst_byte iv2TargVers,
	                         ST_OPTARG(const Tst_mtes_string *pTaggerInfo),
	                         Tst_dynlist *pTFldList,
	                         void *pTagCv);
	/** */
	static void
	ST_TCNV__cbResetGTF(void *pElem);
	static void
	ST_TCNV__cbFreeGTF(void *pElem);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_CONV_PRIV_H */

/******************************************************************************/
