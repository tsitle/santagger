/******************************************************************************/
/* tag_basics-prot.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected functions for Tag basics                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_BASICS_PROT_H
#define HEAD_TAG_BASICS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/tag/tag_basics.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_BASICS_C) || \
			defined(SRC_TAG_ID3V1_C) || \
				defined(SRC_TAG_ID3V1_GS_C) || \
				defined(SRC_TAG_ID3V1_FNC_C) || \
				defined(SRC_TAG_ID3V1_ZRD_C) || \
				defined(SRC_TAG_ID3V1_ZWR_C) || \
			defined(SRC_TAG_ID3V2_C) || \
				defined(SRC_TAG_ID3V2_FLDADD_C) || \
				defined(SRC_TAG_ID3V2_ZRD_C) || \
				defined(SRC_TAG_ID3V2_ZWR_C) || \
			defined(SRC_TAG_APEV2_C) || \
				defined(SRC_TAG_APEV2_FLDADD_C) || \
				defined(SRC_TAG_APEV2_ZRD_C) || \
				defined(SRC_TAG_APEV2_ZWR_C) || \
			defined(SRC_TAG_VORBC_C) || \
				defined(SRC_TAG_VORBC_FLDADD_C) || \
				defined(SRC_TAG_VORBC_ZRD_C) || \
				defined(SRC_TAG_VORBC_ZWR_C) || \
			defined(SRC_TAG_COMFNC_C)
	/** Getter/Setter */
	void st_tagBas_gs_setHasTag(Tst_tagBasics *pTB, const Tst_bool hasTag);
	void st_tagBas_gs_setHasFields(Tst_tagBasics *pTB, const Tst_bool areFldsSet);
	void st_tagBas_gs_setOffset(Tst_tagBasics *pTB, const Tst_foffs tagOffs);
	void st_tagBas_gs_setSize(Tst_tagBasics *pTB, const Tst_uint32 tagSz);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_BASICS_PROT_H */

/******************************************************************************/
