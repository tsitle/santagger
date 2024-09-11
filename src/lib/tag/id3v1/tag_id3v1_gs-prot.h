/******************************************************************************/
/* tag_id3v1_gs-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected getter/setter functions for ID3v1 (Extended) Tags                */
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

#ifndef HEAD_TAG_ID3V1_GS_PROT_H
#define HEAD_TAG_ID3V1_GS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/string_mte-defs.h"
#include "src/includes/tag/tag_id3v1-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V1_GS_C) || defined(SRC_TAG_FLDMAP_C)
	/** */
	Tst_err
	st_id3v1_gs_setField_props(Tst_id3v1_fldData *pFld,
	                           const Tst_id3v1_frID id);
	Tst_err
	st_id3v1_gs_setFieldData_string(Tst_id3v1_fldData *pFld,
	                                Tst_mtes_string *pStr);
	Tst_err
	st_id3v1_gs_setFieldData_ival(Tst_id3v1_fldData *pFld,
	                              const Tst_int32 ival);
	/** */
	Tst_id3v1_fldProp*
	st_id3v1_ite_nextTagFldProp_all(ST_OPTARG(Tst_id3v1_fldProp *pFldPrLast),
	                                Tst_id3v1_fldProp *pFldPrNext);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V1_GS_PROT_H */

/******************************************************************************/
