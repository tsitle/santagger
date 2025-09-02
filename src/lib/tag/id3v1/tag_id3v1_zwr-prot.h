/******************************************************************************/
/* tag_id3v1_zwr-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for writing ID3v1 (Extended) Tags                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_ID3V1_ZWR_PROT_H
#define LIBSANTAGGER_TAG_ID3V1_ZWR_PROT_H

/*
// Own-Includes
*/
/** */
#include "tag_id3v1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V1_C) || defined(SRC_TAG_ID3V1_ZWR_C)
	/** */
	Tst_err
	st_id3v1_wr_rendTagToBO(const Tst_id3v1_tag_intn *pTagI,
	                        Tst_binobj *pTagBO);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_ID3V1_ZWR_PROT_H */

/******************************************************************************/
