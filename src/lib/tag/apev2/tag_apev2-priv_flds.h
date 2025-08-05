/******************************************************************************/
/* tag_apev2-priv_flds.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private definitions for APEv2 Tags                                         */
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

#ifndef HEAD_TAG_APEV2_PRIV_FLDS_H
#define HEAD_TAG_APEV2_PRIV_FLDS_H


ST_BEGIN_C_DECLS

/*
// Constants (private)
*/
#ifdef SRC_TAG_APEV2_C
	/** APEv2 Tag Field Properties */
	const Tst_apev2_fldProp_intn ST_APEV2_FLDPROPS[] = {
		{"TITLE",            "Track title",
			ST_APEV2_FID_TITL, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"SUBTITLE",         "Subtitle",
			ST_APEV2_FID_SUBT, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"ARTIST",           "Artist",
			ST_APEV2_FID_ARTI, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"ALBUM",            "Album title",
			ST_APEV2_FID_ALBU, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"DEBUT ALBUM",      "Debut album",
			ST_APEV2_FID_DEBA, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"PUBLISHER",        "Publisher",
			ST_APEV2_FID_PUBL, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"CONDUCTOR",        "Conductor",
			ST_APEV2_FID_COND, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"TRACK",            "Track number",
			ST_APEV2_FID_TRKN, ST_APEV2_FTP_INT, ST_B_TRUE, NULL},
		{"TRACKTOTAL",       "Tracks total",                              /* unofficial */
			ST_APEV2_FID_TRKT, ST_APEV2_FTP_INT, ST_B_TRUE, NULL},
		{"DISC",             "Disc number",                               /* unofficial */
			ST_APEV2_FID_DSCN, ST_APEV2_FTP_INT, ST_B_TRUE, NULL},
		{"DISCTOTAL",        "Discs total",                               /* unofficial */
			ST_APEV2_FID_DSCT, ST_APEV2_FTP_INT, ST_B_TRUE, NULL},
		{"COMPOSER",         "Composer",
			ST_APEV2_FID_COMP, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"COMMENT",          "Comment",
			ST_APEV2_FID_COMM, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"COPYRIGHT",        "Copyright holder",
			ST_APEV2_FID_COPY, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"PUBLICATIONRIGHT", "Publication right holder",
			ST_APEV2_FID_PUBR, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"FILE",             "File location",
			ST_APEV2_FID_FILE, ST_APEV2_FTP_URL, ST_B_TRUE, NULL},
		{"EAN/UPC",          "EAN-13/UPC-A bar code identifier",
			ST_APEV2_FID_EANU, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"ISBN",             "ISBN number",
			ST_APEV2_FID_ISBN, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"CATALOG",          "Catalog number",
			ST_APEV2_FID_CATA, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"LC",               "Label Code",
			ST_APEV2_FID_LABC, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"YEAR",             "Year",
			ST_APEV2_FID_YEAR, ST_APEV2_FTP_YEAR, ST_B_TRUE, NULL},
		{"RECORD DATE",      "Recording date",
			ST_APEV2_FID_RECD, ST_APEV2_FTP_TSTA, ST_B_TRUE, NULL},
		{"RECORD LOCATION",  "Recording location",
			ST_APEV2_FID_RECL, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"GENRE",            "Genre",
			ST_APEV2_FID_GENR, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"MEDIA",            "Media",
			ST_APEV2_FID_MEDI, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"INDEX",            "Indexes for quick access",
			ST_APEV2_FID_INDX, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"RELATED",          "Location of related information",
			ST_APEV2_FID_RELA, ST_APEV2_FTP_URL, ST_B_TRUE, NULL},
		{"ISRC",             "International Standard Recording Code",
			ST_APEV2_FID_ISRC, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"ABSTRACT",         "Abstract",
			ST_APEV2_FID_ABST, ST_APEV2_FTP_URL, ST_B_FALSE, NULL},
		{"LANGUAGE",         "Language(s)",
			ST_APEV2_FID_LANG, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"BIBLIOGRAPHY",     "Bibliography/Dicography",
			ST_APEV2_FID_BIBL, ST_APEV2_FTP_URL, ST_B_TRUE, NULL},
		{"INTROPLAY",        "Characteristic part of piece for intro",
			ST_APEV2_FID_INTR, ST_APEV2_FTP_STR, ST_B_FALSE, NULL},
		{"REPLAYGAIN_TRACK_GAIN", "ReplayGain track volume adjustment",   /* unofficial */
			ST_APEV2_FID_RGTG, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"REPLAYGAIN_TRACK_PEAK", "ReplayGain track peak level",          /* unofficial */
			ST_APEV2_FID_RGTP, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"REPLAYGAIN_ALBUM_GAIN", "ReplayGain album volume adjustment",   /* unofficial */
			ST_APEV2_FID_RGAG, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"REPLAYGAIN_ALBUM_PEAK", "ReplayGain album peak level",          /* unofficial */
			ST_APEV2_FID_RGAP, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"INVOLVEDPEOPLE",   "Involved people",                           /* unofficial */
			ST_APEV2_FID_IPLS, ST_APEV2_FTP_STR, ST_B_TRUE, NULL},
		{"TAGGER",           "Tagging application",                       /* unofficial */
			ST_APEV2_FID_TAGG, ST_APEV2_FTP_STR, ST_B_FALSE, NULL},
		{"CUSTOM_BIN_FLD",   "Custom binary data",
			ST_APEV2_FID_CBIN, ST_APEV2_FTP_BDAT, ST_B_FALSE, NULL},
		{"CUSTOM_TXT_FLD",   "Custom text",
			ST_APEV2_FID_CTXT, ST_APEV2_FTP_STR, ST_B_FALSE, NULL},
		{NULL, NULL, ST_APEV2_FID_NONE, ST_APEV2_FTP_NONE, ST_B_FALSE, NULL} };
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_APEV2_PRIV_FLDS_H */

/******************************************************************************/
