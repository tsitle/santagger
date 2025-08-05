/******************************************************************************/
/* tag_vorbc_gs-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected Getter/Setter functions for Vorbis Tags                          */
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

#ifndef HEAD_TAG_VORBC_GS_PROT_H
#define HEAD_TAG_VORBC_GS_PROT_H

/*
// Own-Includes
*/
/** */
#include "tag_vorbc-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_VORBC_GS_C) || defined(SRC_TAG_CONV_C)
	/** */
	Tst_err
	st_vorbc_gs_setTag_taggerInfo(Tst_vorbc_tag *pTag,
	                              const Tst_mtes_string *pTI);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_TAG_VORBC_GS_C) || defined(SRC_TAG_FLDMAP_C)
	/** */
	Tst_err
	st_vorbc_gs_setField_props(Tst_vorbc_fldData *pFld,
	                           Tst_vorbc_frID frID,
	                           ST_OPTARG(const Tst_str *pFrIDstr));
	/** */
	Tst_err
	st_vorbc_gs_setFieldData_stringArr(Tst_vorbc_fldData *pFld,
	                                   Tst_mtes_strArr *pStrArr);
	Tst_err
	st_vorbc_gs_setFieldData_picBin(Tst_vorbc_fldData *pFld,
	                                Tst_binobj *pBinDat);
	/** */
	Tst_err
	st_vorbc_gs_setFieldAttr_desc(Tst_vorbc_fldData *pFld,
	                              Tst_mtes_string *pStr);
	Tst_err
	st_vorbc_gs_setFieldAttr_picTp(Tst_vorbc_fldData *pFld,
	                               const Tst_vorbc_picTp picTp);
	Tst_err
	st_vorbc_gs_setFieldAttr_picFmt(Tst_vorbc_fldData *pFld,
	                                const Tst_utilsFmt_picFmt picFmt);
	/** */
	Tst_vorbc_fldProp*
	st_vorbc_ite_nextTagFldProp_all(ST_OPTARG(Tst_vorbc_fldProp *pFldPrLast),
	                                Tst_vorbc_fldProp *pFldPrNext);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_GS_PROT_H */

/******************************************************************************/
