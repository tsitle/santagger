/******************************************************************************/
/* tag_apev2_gs-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected Getter/Setter functions for APEv2 Tags                           */
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

#ifndef HEAD_TAG_APEV2_GS_PROT_H
#define HEAD_TAG_APEV2_GS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_apev2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_APEV2_GS_C) || defined(SRC_TAG_CONV_C)
	/** */
	Tst_err
	st_apev2_gs_setTag_taggerInfo(Tst_apev2_tag *pTag,
	                              const Tst_mtes_string *pTI);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_TAG_APEV2_GS_C) || defined(SRC_TAG_FLDMAP_C)
	/** */
	Tst_err
	st_apev2_gs_setField_props(Tst_apev2_fldData *pFld,
	                           Tst_apev2_frID frID,
	                           ST_OPTARG(const Tst_str *pFrIDstr));
	Tst_err
	st_apev2_gs_setFieldData_stringArr(Tst_apev2_fldData *pFld,
	                                   Tst_mtes_strArr *pStrArr);
	Tst_err
	st_apev2_gs_setFieldData_geoBin(Tst_apev2_fldData *pFld,
	                                Tst_binobj *pBinDat);
	/** */
	Tst_apev2_fldProp*
	st_apev2_ite_nextTagFldProp_all(ST_OPTARG(Tst_apev2_fldProp *pFldPrLast),
	                                Tst_apev2_fldProp *pFldPrNext);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_APEV2_GS_PROT_H */

/******************************************************************************/
