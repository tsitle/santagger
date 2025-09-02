/******************************************************************************/
/* tag_id3v1-pp_flds.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected/Private definitions for ID3v1 (Extended) Tags                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_ID3V1_PP_FLDS_H
#define LIBSANTAGGER_TAG_ID3V1_PP_FLDS_H


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_TAG_ID3V1_C) || defined(SRC_TAG_ID3V1_GS_C) || \
			defined(SRC_TAG_ID3V1_OPTS_C) || defined(SRC_TAG_ID3V1_STCS_C) || \
			defined(SRC_TAG_ID3V1_ZRD_C) || defined(SRC_TAG_ID3V1_ZWR_C) || \
			defined(SRC_TAG_ID3V1_FNC_C) || defined(SRC_TAG_ID3V1_ZFDEB_C) || \
			defined(SRC_TAG_FLDMAP_C)
	/** field names */
	/*** ID3v1 */
#	define ST_ID3V1_TAG_FIDSTR_ARTI  "ARTI"
#	define ST_ID3V1_TAG_FIDSTR_ALBU  "ALBU"
#	define ST_ID3V1_TAG_FIDSTR_TITL  "TITL"
#	define ST_ID3V1_TAG_FIDSTR_TRKN  "TRKN"
#	define ST_ID3V1_TAG_FIDSTR_YEAR  "YEAR"
#	define ST_ID3V1_TAG_FIDSTR_COMM  "COMM"
#	define ST_ID3V1_TAG_FIDSTR_GENR  "GENR"
	/*** ID3v1 Extended */
#	define ST_ID3V1_TAG_EXT_FIDSTR_EARTI  "EART"
#	define ST_ID3V1_TAG_EXT_FIDSTR_EALBU  "EALB"
#	define ST_ID3V1_TAG_EXT_FIDSTR_ETITL  "ETIT"
#	define ST_ID3V1_TAG_EXT_FIDSTR_EGENR  "EGEN"
#	define ST_ID3V1_TAG_EXT_FIDSTR_SPEED  "ESPD"
#	define ST_ID3V1_TAG_EXT_FIDSTR_TIMES  "ESTT"
#	define ST_ID3V1_TAG_EXT_FIDSTR_TIMEE  "EENT"
#endif  /* protected */


/*
// Constants (private)
*/
#ifdef SRC_TAG_ID3V1_C
	/** ID3v1 Tag Field Properties */
	const Tst_id3v1_fldProp_intn ST_ID3V1_FLDPROPS[] = {
		{ST_ID3V1_TAG_FIDSTR_ARTI, "Artist",
			ST_ID3V1_FID_ARTI, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_STR, ST_B_TRUE},
		{ST_ID3V1_TAG_FIDSTR_ALBU, "Album title",
			ST_ID3V1_FID_ALBU, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_STR, ST_B_TRUE},
		{ST_ID3V1_TAG_FIDSTR_TITL, "Track Title",
			ST_ID3V1_FID_TITL, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_STR, ST_B_TRUE},
		{ST_ID3V1_TAG_FIDSTR_COMM, "Comment",
			ST_ID3V1_FID_COMM, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_STR, ST_B_TRUE},
		{ST_ID3V1_TAG_FIDSTR_TRKN, "Track number",
			ST_ID3V1_FID_TRKN, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_INT, ST_B_TRUE},
		{ST_ID3V1_TAG_FIDSTR_YEAR, "Year",
			ST_ID3V1_FID_YEAR, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_INT, ST_B_TRUE},
		{ST_ID3V1_TAG_FIDSTR_GENR, "Genre",
			ST_ID3V1_FID_GENR, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_INT, ST_B_TRUE},
		{ST_ID3V1_TAG_EXT_FIDSTR_EARTI, "(extended) Artist",
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_EARTI, ST_ID3V1_FTP_STR, ST_B_FALSE},
		{ST_ID3V1_TAG_EXT_FIDSTR_EALBU, "(extended) Album",
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_EALBU, ST_ID3V1_FTP_STR, ST_B_FALSE},
		{ST_ID3V1_TAG_EXT_FIDSTR_ETITL, "(extended) Title",
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_ETITL, ST_ID3V1_FTP_STR, ST_B_FALSE},
		{ST_ID3V1_TAG_EXT_FIDSTR_EGENR, "(extended) Genre",
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_EGENR, ST_ID3V1_FTP_STR, ST_B_FALSE},
		{ST_ID3V1_TAG_EXT_FIDSTR_SPEED, "Speed",
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_SPEED, ST_ID3V1_FTP_INT, ST_B_FALSE},
		{ST_ID3V1_TAG_EXT_FIDSTR_TIMES, "Starttime",
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_TIMES, ST_ID3V1_FTP_STR, ST_B_FALSE},
		{ST_ID3V1_TAG_EXT_FIDSTR_TIMEE, "Endtime",
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_TIMEE, ST_ID3V1_FTP_STR, ST_B_FALSE},
		{NULL, NULL,
			ST_ID3V1_FID_NONE, ST_ID3V1E_FID_NONE, ST_ID3V1_FTP_NONE, ST_B_FALSE} };
#endif  /* private */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_ID3V1_PP_FLDS_H */

/******************************************************************************/
