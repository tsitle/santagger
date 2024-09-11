/******************************************************************************/
/* tag_id3v2_ztidy-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for tidying up ID3v2 Tags                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.03.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_ZTIDY_PROT_H
#define HEAD_TAG_ID3V2_ZTIDY_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_id3v2-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V2_C) || defined(SRC_TAG_ID3V2_ZTIDY_C)
	void
	st_id3v2_tidy_getFldMap(const Tst_id3v2_frID srcID1,
	                        Tst_id3v2_frID *pSrcID2,
	                        Tst_id3v2_frID *pSrcID3,
	                        const Tst_byte tagVersionMajor,
	                        Tst_bool *pToMap,
	                        Tst_bool *pDiscard,
	                        Tst_id3v2_frID *pDestID1,
	                        Tst_id3v2_frID *pDestID2,
	                        Tst_id3v2_frID *pDestID3);
	Tst_err
	st_id3v2_tidy_mapField(Tst_id3v2_tag *pTag,
	                       Tst_id3v2_fldData *pFld1,
	                       Tst_id3v2_fldData *pFld2,
	                       Tst_id3v2_fldData *pFld3,
	                       const Tst_id3v2_frID destID1,
	                       const Tst_id3v2_frID destID2,
	                       const Tst_id3v2_frID destID3,
	                       Tst_bool *pWasMapped);
	/** */
	Tst_err
	st_id3v2_tidy_mergFields(const char *pFnc,
	                         Tst_id3v2_tag *pTag,
	                         Tst_id3v2_fldData *pFld);
	Tst_err
	st_id3v2_tidy_mergMultStr(const char *pFnc,
	                          const Tst_id3v2_opts *pOpts,
	                          Tst_id3v2_fldData *pFld);
	/** */
	Tst_err
	st_id3v2_tidy_remDups(const char *pFnc,
	                      const Tst_bool discardFields,
	                      Tst_id3v2_tag *pTag,
	                      Tst_id3v2_fldData *pFld,
	                      Tst_bool *pMadeOneUnique);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_ZTIDY_PROT_H */

/******************************************************************************/
