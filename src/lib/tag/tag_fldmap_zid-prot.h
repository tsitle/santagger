/******************************************************************************/
/* tag_fldmap_zid-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected functions for mapping Tag Field IDs                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_FLDMAP_ZID_PROT_H
#define HEAD_TAG_FLDMAP_ZID_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/tag/tag_apev2-defs.h>
#include <santag/tag/tag_id3v1-defs.h>
#include <santag/tag/tag_id3v2-defs.h>
#include <santag/tag/tag_vorbc-defs.h>


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_FLDMAP_ZID_C) || defined(SRC_TAG_FLDMAP_C) || \
			defined(SRC_TAG_CONV_C)
	/* --> ID3v1 */
	Tst_id3v1_frID
	st_tfldMap_mapID_IV2_to_IV1(const Tst_id3v2_frID srcIV2);

	Tst_id3v1_frID
	st_tfldMap_mapID_AV2_to_IV1(const Tst_apev2_frID srcAV2);

	Tst_id3v1_frID
	st_tfldMap_mapID_VOR_to_IV1(const Tst_vorbc_frID srcVOR);

	/* --> ID3v2 */
	Tst_id3v2_frID
	st_tfldMap_mapID_IV1_to_IV2(const Tst_id3v1_frID srcIV1);

	Tst_id3v2_frID
	st_tfldMap_mapID_AV2_to_IV2(const Tst_apev2_frID srcAV2);

	Tst_id3v2_frID
	st_tfldMap_mapID_VOR_to_IV2(const Tst_vorbc_frID srcVOR);

	/* --> APEv2 */
	Tst_apev2_frID
	st_tfldMap_mapID_IV1_to_AV2(const Tst_id3v1_frID srcIV1);

	Tst_apev2_frID
	st_tfldMap_mapID_IV2_to_AV2(const Tst_id3v2_frID srcIV2);

	Tst_apev2_frID
	st_tfldMap_mapID_VOR_to_AV2(const Tst_vorbc_frID srcVOR);

	/* --> Vorbis */
	Tst_vorbc_frID
	st_tfldMap_mapID_IV1_to_VOR(const Tst_id3v1_frID srcIV1);

	Tst_vorbc_frID
	st_tfldMap_mapID_IV2_to_VOR(const Tst_id3v2_frID srcIV2);

	Tst_vorbc_frID
	st_tfldMap_mapID_AV2_to_VOR(const Tst_apev2_frID srcAV2);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_FLDMAP_ZID_PROT_H */

/******************************************************************************/
