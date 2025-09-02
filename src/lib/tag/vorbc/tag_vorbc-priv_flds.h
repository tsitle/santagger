/******************************************************************************/
/* tag_vorbc-priv_flds.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private definitions for Vorbis Tags                                        */
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

#ifndef LIBSANTAGGER_TAG_VORBC_PRIV_FLDS_H
#define LIBSANTAGGER_TAG_VORBC_PRIV_FLDS_H


ST_BEGIN_C_DECLS

/*
// Constants (private)
*/
#ifdef SRC_TAG_VORBC_C
	/** Vorbis Tag Field Properties */
	const Tst_vorbc_fldProp_intn ST_VORBC_FLDPROPS[] = {
		{"TITLE",          "Track title",
			ST_VORBC_FID_TITL, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"VERSION",        "Version (e.g. remix info)",
			ST_VORBC_FID_VERS, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"ALBUM",          "Album title",
			ST_VORBC_FID_ALBU, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"TRACKNUMBER",    "Track number",
			ST_VORBC_FID_TRKN, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"TRACKTOTAL",     "Total tracks",                                /* unofficial */
			ST_VORBC_FID_TRKT, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"DISCNUMBER",     "Disc number",                                 /* unofficial */
			ST_VORBC_FID_DSCN, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"DISCTOTAL",      "Total discs",                                 /* unofficial */
			ST_VORBC_FID_DSCT, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"ARTIST",         "Artist",
			ST_VORBC_FID_ARTI, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"ALBUMARTIST",    "Artist",                                      /* unofficial */
			ST_VORBC_FID_ARTI, ST_VORBC_FTP_STR, ST_B_FALSE, NULL},
		{"PERFORMER",      "Performer",
			ST_VORBC_FID_PERF, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"COPYRIGHT",      "Copyright holder",
			ST_VORBC_FID_COPY, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"LICENSE",        "License",
			ST_VORBC_FID_LICS, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"ORGANIZATION",   "Organization",
			ST_VORBC_FID_ORGA, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"DESCRIPTION",    "Description",
			ST_VORBC_FID_DESC, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"GENRE",          "Genre",
			ST_VORBC_FID_GENR, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"DATE",           "Recording date",
			ST_VORBC_FID_RECD, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"LOCATION",       "Recording location",
			ST_VORBC_FID_RECL, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"CONTACT",        "Contact address",
			ST_VORBC_FID_CONT, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"ISRC",           "International Standard Recording Code",
			ST_VORBC_FID_ISRC, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"TEMPO",          "Beats per minute",                            /* unofficial */
			ST_VORBC_FID_TMPO, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"BPM",            "Beats per minute",                            /* unofficial */
			ST_VORBC_FID_BPMI, ST_VORBC_FTP_STR, ST_B_FALSE, NULL},
		{"LYRICS",         "Lyrics",                                      /* unofficial */
			ST_VORBC_FID_LYRI, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"METADATA_BLOCK_PICTURE", "Picture",                             /* unofficial */
			ST_VORBC_FID_MBPI, ST_VORBC_FTP_PICT, ST_B_TRUE, NULL},
		{"REPLAYGAIN_TRACK_GAIN", "ReplayGain track volume adjustment",   /* unofficial */
			ST_VORBC_FID_RGTG, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"REPLAYGAIN_TRACK_PEAK", "ReplayGain track peak level",          /* unofficial */
			ST_VORBC_FID_RGTP, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"REPLAYGAIN_ALBUM_GAIN", "ReplayGain album volume adjustment",   /* unofficial */
			ST_VORBC_FID_RGAG, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"REPLAYGAIN_ALBUM_PEAK", "ReplayGain album peak level",          /* unofficial */
			ST_VORBC_FID_RGAP, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"ENCODER",        "Encoder software",                            /* unofficial */
			ST_VORBC_FID_ENCO, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"INVOLVEDPEOPLE", "Involved people",                             /* unofficial */
			ST_VORBC_FID_IPLS, ST_VORBC_FTP_STR, ST_B_TRUE, NULL},
		{"TAGGER",         "Tagging application",                         /* unofficial */
			ST_VORBC_FID_TAGG, ST_VORBC_FTP_STR, ST_B_FALSE, NULL},
		{"CUSTOM_TXT_FLD", "Custom text",
			ST_VORBC_FID_CTXT, ST_VORBC_FTP_STR, ST_B_FALSE, NULL},
		{NULL, NULL, ST_VORBC_FID_NONE, ST_VORBC_FTP_NONE, ST_B_FALSE, NULL} };
#endif  /* private */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_VORBC_PRIV_FLDS_H */

/******************************************************************************/
