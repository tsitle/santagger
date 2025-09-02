/******************************************************************************/
/* tag_id3v2-priv_flds.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private definitions for ID3v2 Tags                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 10.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_TAG_ID3V2_PRIV_FLDS_H
#define LIBSANTAGGER_TAG_ID3V2_PRIV_FLDS_H


ST_BEGIN_C_DECLS

/*
// Constants (private)
*/
#if defined(SRC_TAG_ID3V2_C)
	/** ID3v2 Tag Frame Properties */
	const Tst_id3v2_fldProp_intn ST_ID3V2_FLDPROPS[] = {
		{"CRA", "AENC", "Audio encryption",                 7, ST_ID3V2_FID_234_AENC, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"PIC", "APIC", "Picture",                          7, ST_ID3V2_FID_234_APIC, ST_ID3V2_FTP_PIC, ST_B_TRUE, NULL, "", ""},
		{"   ", "ASPI", "Audio seek point index",           4, ST_ID3V2_FID___4_ASPI, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"CDM", "CDM ", "Compressed Metaframe",             1, ST_ID3V2_FID_2___CDMF, ST_ID3V2_FTP_CDM, ST_B_TRUE, NULL, "", ""},
		{/*!*/"CM1", "CM1 ", "Converted Comments from ID3v1", /*!*/1, ST_ID3V2_FID_2___COM1, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},
		{"COM", "COMM", "Comment",                          7, ST_ID3V2_FID_234_COMM, ST_ID3V2_FTP_COM, ST_B_TRUE, NULL, "", ""},
		{"   ", "COMR", "Commercial frame",                 6, ST_ID3V2_FID__34_COMR, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{/*!*/"CRM", "CRM ", "Encrypted meta frame",   /*!*/1, ST_ID3V2_FID_2___CRM1, ST_ID3V2_FTP_CPY, ST_B_FALSE, NULL, "", ""},
		/*________________________________________________*/
		{"   ", "ENCR", "Encryption method registration",   6, ST_ID3V2_FID__34_ENCR, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"   ", "EQU2", "Equalisation",                     4, ST_ID3V2_FID___4_EQU2, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"EQU", "EQUA", "Equalization",                     3, ST_ID3V2_FID_23__EQUA, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"ETC", "ETCO", "Event timing codes",               7, ST_ID3V2_FID_234_ETCO, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"GEO", "GEOB", "General encapsulated object",      7, ST_ID3V2_FID_234_GEOB, ST_ID3V2_FTP_GEO, ST_B_TRUE, NULL, "", ""},
		{"   ", "GRID", "Group identifier registration",    6, ST_ID3V2_FID__34_GRID, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"IPL", "IPLS", "Involved people list",             3, ST_ID3V2_FID_23__IPLS, ST_ID3V2_FTP_IPL, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"LNK", "LINK", "Linked information",               7, ST_ID3V2_FID_234_LINK, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"MCI", "MCDI", "Music CD Identifier",              7, ST_ID3V2_FID_234_MCDI, ST_ID3V2_FTP_MCI, ST_B_TRUE, NULL, "", ""},
		{"MLL", "MLLT", "MPEG location lookup table",       7, ST_ID3V2_FID_234_MLLT, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"   ", "OWNE", "Ownership frame",                  6, ST_ID3V2_FID__34_OWNE, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"CNT", "PCNT", "Play counter",                     7, ST_ID3V2_FID_234_PCNT, ST_ID3V2_FTP_CNT, ST_B_TRUE, NULL, "", ""},
		{"PCS", "PCST", "Podcast Stuff",                    7, ST_ID3V2_FID_234_PCST, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"POP", "POPM", "Popularimeter",                    7, ST_ID3V2_FID_234_POPM, ST_ID3V2_FTP_POP, ST_B_TRUE, NULL, "", ""},
		{"   ", "POSS", "Position synchronisation frame",   6, ST_ID3V2_FID__34_POSS, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"   ", "PRIV", "Private frame",                    6, ST_ID3V2_FID__34_PRIV, ST_ID3V2_FTP_UFI, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"BUF", "RBUF", "Recommended buffer size",          7, ST_ID3V2_FID_234_RBUF, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"   ", "RVA2", "Relative volume adjustment",       4, ST_ID3V2_FID___4_RVA2, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"RVA", "RVAD", "Relative volume adjustment",       3, ST_ID3V2_FID_23__RVAD, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"REV", "RVRB", "Reverb",                           7, ST_ID3V2_FID_234_RVRB, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"   ", "SEEK", "Seek frame",                       4, ST_ID3V2_FID___4_SEEK, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"   ", "SIGN", "Signature frame",                  4, ST_ID3V2_FID___4_SIGN, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"SLT", "SYLT", "Synchronized lyric/text",          7, ST_ID3V2_FID_234_SYLT, ST_ID3V2_FTP_SLT, ST_B_TRUE, NULL, "", ""},
		{"STC", "SYTC", "Synced tempo codes",               7, ST_ID3V2_FID_234_SYTC, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		/*________________________________________________*/
		{"TAL", "TALB", "Album title",                      7, ST_ID3V2_FID_234_TALB, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TBP", "TBPM", "Beats Per Minute",                 7, ST_ID3V2_FID_234_TBPM, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TCT", "TCAT", "iTunes Podcast Category",          7, ST_ID3V2_FID_234_TCAT, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},/*iTunes*/
		{"TCP", "TCMP", "iTunes Compilation",               7, ST_ID3V2_FID_234_TCMP, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},/*iTunes*/
		{"TCM", "TCOM", "Composer",                         7, ST_ID3V2_FID_234_TCOM, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TCO", "TCON", "Genre",                            7, ST_ID3V2_FID_234_TCON, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TCR", "TCOP", "Copyright message",                7, ST_ID3V2_FID_234_TCOP, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TDA", "TDAT", "Date",                             3, ST_ID3V2_FID_23__TDAT, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TDEN", "Encoding time",                    4, ST_ID3V2_FID___4_TDEN, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TDS", "TDES", "iTunes Podcast Description",       7, ST_ID3V2_FID_234_TDES, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},/*iTunes*/
		{"TDY", "TDLY", "Playlist delay",                   7, ST_ID3V2_FID_234_TDLY, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TDOR", "Original release time",            4, ST_ID3V2_FID___4_TDOR, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TDRC", "Recording time",                   4, ST_ID3V2_FID___4_TDRC, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TDRL", "Release time",                     4, ST_ID3V2_FID___4_TDRL, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TDTG", "Tagging time",                     4, ST_ID3V2_FID___4_TDTG, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TEN", "TENC", "Encoded by",                       7, ST_ID3V2_FID_234_TENC, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TXT", "TEXT", "Lyricist/Text writer",             7, ST_ID3V2_FID_234_TEXT, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TFT", "TFLT", "File type",                        7, ST_ID3V2_FID_234_TFLT, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TID", "TGID", "iTunes Podcast ID",                7, ST_ID3V2_FID_234_TGID, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},/*iTunes*/
		/*________________________________________________*/
		{"TIM", "TIME", "Time",                             3, ST_ID3V2_FID_23__TIME, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TIPL", "Involved people list",             4, ST_ID3V2_FID___4_TIPL, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TKE", "TKEY", "Initial key",                      7, ST_ID3V2_FID_234_TKEY, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TLA", "TLAN", "Language(s)",                      7, ST_ID3V2_FID_234_TLAN, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TLE", "TLEN", "Length in ms",                     7, ST_ID3V2_FID_234_TLEN, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"   ", "TMCL", "Musician credits list",            4, ST_ID3V2_FID___4_TMCL, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TMT", "TMED", "Media type",                       7, ST_ID3V2_FID_234_TMED, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TMOO", "Mood",                             4, ST_ID3V2_FID___4_TMOO, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TOA", "TOPE", "Original artist(s)",               7, ST_ID3V2_FID_234_TOPE, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TOT", "TOAL", "Original album title",             7, ST_ID3V2_FID_234_TOAL, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TOF", "TOFN", "Original filename",                7, ST_ID3V2_FID_234_TOFN, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TOL", "TOLY", "Original lyricist(s)",             7, ST_ID3V2_FID_234_TOLY, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TOR", "TORY", "Original release year",            3, ST_ID3V2_FID_23__TORY, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TOWN", "File owner/licensee",              6, ST_ID3V2_FID__34_TOWN, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TP1", "TPE1", "Artist",                           7, ST_ID3V2_FID_234_TPE1, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TP2", "TPE2", "Album Performer",                  7, ST_ID3V2_FID_234_TPE2, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TP3", "TPE3", "Conductor/Performer refinement",   7, ST_ID3V2_FID_234_TPE3, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TP4", "TPE4", "Interpreted or modified by",       7, ST_ID3V2_FID_234_TPE4, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TPA", "TPOS", "Part of a set",                    7, ST_ID3V2_FID_234_TPOS, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TPB", "TPUB", "Publisher",                        7, ST_ID3V2_FID_234_TPUB, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TPRO", "Produced notice",                  4, ST_ID3V2_FID___4_TPRO, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TRD", "TRDA", "Recording dates",                  3, ST_ID3V2_FID_23__TRDA, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TRK", "TRCK", "Track number",                     7, ST_ID3V2_FID_234_TRCK, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TRSN", "Internet Radio Station name",      6, ST_ID3V2_FID__34_TRSN, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TRSO", "Internet Radio Station owner",     6, ST_ID3V2_FID__34_TRSO, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TSC", "TSOC", "Composer sort order",              7, ST_ID3V2_FID_234_TSOC, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},/*iTunes*/
		{"TS2", "TSO2", "Album Performer sort order",       7, ST_ID3V2_FID_234_TSO2, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},/*iTunes*/
		{"TSA", "TSOA", "Album sort order",                 7, ST_ID3V2_FID_234_TSOA, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{/*!*/"   ", "TSA ", "Album sort order",       /*!*/7, ST_ID3V2_FID_234_TSOA, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},/*iTunes*/
		{"TST", "TSOT", "Title sort order",                 7, ST_ID3V2_FID_234_TSOT, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{/*!*/"   ", "TST ", "Title sort order",       /*!*/7, ST_ID3V2_FID_234_TSOT, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},/*iTunes*/
		{"TSP", "TSOP", "Performer sort order",             7, ST_ID3V2_FID_234_TSOP, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{/*!*/"   ", "TSP ", "Performer sort order",   /*!*/7, ST_ID3V2_FID_234_TSOP, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},/*iTunes*/
		{"TRC", "TSRC", "International Standard Recording Code", 7, ST_ID3V2_FID_234_TSRC, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TSS", "TSSE", "SW/HW and settings used for encoding",  7, ST_ID3V2_FID_234_TSSE, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"   ", "TSST", "Set subtitle",                     4, ST_ID3V2_FID___4_TSST, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TT1", "TIT1", "Content group desc",               7, ST_ID3V2_FID_234_TIT1, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TT2", "TIT2", "Track title",                      7, ST_ID3V2_FID_234_TIT2, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		{"TT3", "TIT3", "Subtitle/Desc refinement",         7, ST_ID3V2_FID_234_TIT3, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"TXX", "TXXX", "User defined text info frame",     7, ST_ID3V2_FID_234_TXXX, ST_ID3V2_FTP_WXX, ST_B_TRUE, NULL, "", ""},
		{"TYE", "TYER", "Year",                             3, ST_ID3V2_FID_23__TYER, ST_ID3V2_FTP_TXT, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		/*________________________________________________*/
		{"UFI", "UFID", "Unique file identifier",           7, ST_ID3V2_FID_234_UFID, ST_ID3V2_FTP_UFI, ST_B_TRUE, NULL, "", ""},
		{"   ", "USER", "Terms of use",                     6, ST_ID3V2_FID__34_USER, ST_ID3V2_FTP_CPY, ST_B_TRUE, NULL, "", ""},
		{"ULT", "USLT", "Unsychronized lyric/text transcription", 7, ST_ID3V2_FID_234_USLT, ST_ID3V2_FTP_COM, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"WAF", "WOAF", "Official audio file webpage",      7, ST_ID3V2_FID_234_WOAF, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{"WAR", "WOAR", "Official artist webpage",          7, ST_ID3V2_FID_234_WOAR, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{"WAS", "WOAS", "Official audio source webpage",    7, ST_ID3V2_FID_234_WOAS, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{"WCM", "WCOM", "Commercial information",           7, ST_ID3V2_FID_234_WCOM, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{"WCP", "WCOP", "Copyright/Legal information",      7, ST_ID3V2_FID_234_WCOP, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{/*!*/"WFE", "WFED", "iTunes Podcast URL",     /*!*/7, ST_ID3V2_FID_234_WFED, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},/*iTunes*/
		{"   ", "WPAY", "Payment",                          6, ST_ID3V2_FID__34_WPAY, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{"WPB", "WPUB", "Publishers official webpage",      7, ST_ID3V2_FID_234_WPUB, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{"   ", "WORS", "Official Internet Radio Station WP", 6, ST_ID3V2_FID__34_WORS, ST_ID3V2_FTP_URL, ST_B_TRUE, NULL, "", ""},
		{"WXX", "WXXX", "User defined URL link frame",      7, ST_ID3V2_FID_234_WXXX, ST_ID3V2_FTP_WXX, ST_B_TRUE, NULL, "", ""},
		/*________________________________________________*/
		{"-T-", "-T--", "Custom text frame",                7, ST_ID3V2_FID_234_CTXT, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},
		{"-U-", "-U--", "Custom URL frame",                 7, ST_ID3V2_FID_234_CURL, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},
		/*________________________________________________*/
		{NULL, NULL, NULL, 0, ST_ID3V2_FID_NONE, ST_ID3V2_FTP_NONE, ST_B_FALSE, NULL, "", ""} };

	/** ID3v2 Tag Frame Properties - to be ignored */
	const Tst_id3v2_fldProp_intn ST_ID3V2_FLDPROPS_IGN[] = {
		{"TSI", "TSIZ", "Size", 3, ST_ID3V2_FID_23__TSIZ, ST_ID3V2_FTP_TXT, ST_B_FALSE, NULL, "", ""},
		{NULL, NULL, NULL, 0, ST_ID3V2_FID_NONE, ST_ID3V2_FTP_NONE, ST_B_FALSE, NULL, "", ""} };
#endif  /* private */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_TAG_ID3V2_PRIV_FLDS_H */

/******************************************************************************/
