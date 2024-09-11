/******************************************************************************/
/* tag_id3v1_zrd-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for reading ID3v1 (Extended) Tags                      */
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

#ifndef HEAD_TAG_ID3V1_ZRD_PROT_H
#define HEAD_TAG_ID3V1_ZRD_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_id3v1-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V1_C) || defined(SRC_TAG_ID3V1_ZRD_C)
	/** */
	Tst_err st_id3v1_rd_readV1(const Tst_bool rdOnlyBasics,
	                           Tst_id3v1_tag_intn *pTagI,
	                           Tst_buf *pBuf);
	Tst_err st_id3v1_rd_readV1Ext(const Tst_bool rdOnlyBasics,
	                              Tst_id3v1_tag_intn *pTagI,
	                              Tst_buf *pBuf);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V1_ZRD_PROT_H */

/******************************************************************************/
