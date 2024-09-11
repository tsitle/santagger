/******************************************************************************/
/* tag_id3v2_gs-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected getter/setter functions for ID3v2 Tags                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 14.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_GS_PROT_H
#define HEAD_TAG_ID3V2_GS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj-defs.h"
#include "src/includes/common/string_mte-defs.h"
#include "src/includes/tag/tag_id3v2-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_CONV_C)
	/** */
	Tst_err
	st_id3v2_gs_setTag_taggerInfo(Tst_id3v2_tag *pTag,
	                              const Tst_mtes_string *pTI);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_ID3V2_GS_C) || \
			defined(SRC_TAG_ID3V2_FNC_C) || defined(SRC_TAG_FLDMAP_C)
	/** */
	Tst_err
	st_id3v2_gs_setField_props(Tst_id3v2_fldData *pFld,
	                           Tst_id3v2_frID frID,
	                           ST_OPTARG(const Tst_str *pFrIDstr));
	Tst_err
	st_id3v2_gs_setFieldData_stringArr(Tst_id3v2_fldData *pFld,
	                                   Tst_mtes_strArr *pStrArr);
	Tst_err
	st_id3v2_gs_setFieldData_bin(Tst_id3v2_fldData *pFld,
	                             Tst_binobj *pBinDat);
	Tst_err
	st_id3v2_gs_setFieldData_uval(Tst_id3v2_fldData *pFld,
	                             const Tst_uint32 uval);
	Tst_err
	st_id3v2_gs_setFieldData_bval(Tst_id3v2_fldData *pFld,
	                             const Tst_byte bval);
	Tst_err
	st_id3v2_gs_setFieldAttr_desc(Tst_id3v2_fldData *pFld,
	                              Tst_mtes_string *pStr);
	Tst_err
	st_id3v2_gs_setFieldAttr_lang(Tst_id3v2_fldData *pFld,
	                              const Tst_str *pLangISO);
	Tst_err
	st_id3v2_gs_setFieldAttr_geobFn(Tst_id3v2_fldData *pFld,
	                                Tst_mtes_string *pFilen);
	Tst_err
	st_id3v2_gs_setFieldAttr_geobMime(Tst_id3v2_fldData *pFld,
	                                  const Tst_str *pMimeISO);
	Tst_err
	st_id3v2_gs_setFieldAttr_picTp(Tst_id3v2_fldData *pFld,
	                               const Tst_id3v2_picTp picTp);
	Tst_err
	st_id3v2_gs_setFieldAttr_picFmt(Tst_id3v2_fldData *pFld,
	                                const Tst_utilsFmt_picFmt picFmt);
	Tst_err
	st_id3v2_gs_setFieldAttr_sltTSF(Tst_id3v2_fldData *pFld,
                                    const Tst_id3v2_sltTSF tsf);
	Tst_err
	st_id3v2_gs_setFieldAttr_sltCTP(Tst_id3v2_fldData *pFld,
                                    const Tst_id3v2_sltCTp ctp);
	/** */
	Tst_id3v2_fldData*
	st_id3v2_ite_nextFld_wIx(const Tst_id3v2_tag *pTag,
	                         ST_OPTARG(const Tst_id3v2_fldData *pFldLast),
	                         ST_OPTARG(Tst_uint32 *pIndex));
	Tst_id3v2_fldData*
	st_id3v2_ite_nextValidFld_wIx(const Tst_id3v2_tag *pTag,
	                              ST_OPTARG(const Tst_id3v2_fldData *pFldLast),
	                              ST_OPTARG(Tst_uint32 *pIndex));
	/** */
	Tst_id3v2_fldProp*
	st_id3v2_ite_nextTagFldProp_all(ST_OPTARG(Tst_id3v2_fldProp *pFldPrLast),
	                                Tst_id3v2_fldProp *pFldPrNext);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_GS_PROT_H */

/******************************************************************************/
