/******************************************************************************/
/* tag_id3v2_fnc-priv.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private sub functions for ID3v2 Tag handling                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_FNC_PRIV_H
#define HEAD_TAG_ID3V2_FNC_PRIV_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_id3v2_fnc-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (private)
*/
#ifdef SRC_TAG_ID3V2_FNC_C
	/** priorities for output of frames */
	const Tst_id3v2_frTp ST_ID3V2_PRIO_FTP[] = {
		ST_ID3V2_FTP_TXT, ST_ID3V2_FTP_COM, ST_ID3V2_FTP_WXX,
		ST_ID3V2_FTP_IPL, ST_ID3V2_FTP_SLT, ST_ID3V2_FTP_URL,
		ST_ID3V2_FTP_UFI, ST_ID3V2_FTP_MCI, ST_ID3V2_FTP_POP,
		ST_ID3V2_FTP_CNT, ST_ID3V2_FTP_GEO, ST_ID3V2_FTP_PIC};

	const Tst_id3v2_frID ST_ID3V2_PRIO_TXTFID[] = {
		ST_ID3V2_FID_234_TPE1, ST_ID3V2_FID_234_TALB,
		ST_ID3V2_FID_234_TIT2, ST_ID3V2_FID_234_TRCK,
		ST_ID3V2_FID_234_TPOS, ST_ID3V2_FID_23__TYER,
		ST_ID3V2_FID___4_TDRL, ST_ID3V2_FID___4_TDRC,
		ST_ID3V2_FID_234_TCON,
		ST_ID3V2_FID_234_CTXT, ST_ID3V2_FID_234_CURL};

	const Tst_id3v2_frID ST_ID3V2_PRIO_WXXFID[] = {
		ST_ID3V2_FID_234_TXXX, ST_ID3V2_FID_234_WXXX};

	const Tst_id3v2_frID ST_ID3V2_PRIO_COMFID[] = {
		ST_ID3V2_FID_234_COMM, ST_ID3V2_FID_234_USLT};

	const Tst_id3v2_picTp ST_ID3V2_PRIO_PICTP[] = {
		ST_ID3V2_PT_COVFRO, ST_ID3V2_PT_COVBAC,
		ST_ID3V2_PT_BAND,   ST_ID3V2_PT_COMP,
		ST_ID3V2_PT_ILLUS,  ST_ID3V2_PT_BLOGO};

	/** ID3v2 Picture types */
	const Tst_id3v2_picTp_arrElem ST_ID3V2_PICTYPES[] = {
		{0x00, "Other",                         "other",       ST_ID3V2_PT_OTHER},
		{0x01, "32x32 pixels 'file icon'",      "icon-32",     ST_ID3V2_PT_3232},
		{0x02, "Other file icon",               "icon-other",  ST_ID3V2_PT_OFICON},
		{0x03, "Cover (front)",                 "cov-front",   ST_ID3V2_PT_COVFRO},
		{0x04, "Cover (back)",                  "cov-back",    ST_ID3V2_PT_COVBAC},
		{0x05, "Leaflet page",                  "leaflet",     ST_ID3V2_PT_LEAFL},
		{0x06, "Media (e.g. lable side of CD)", "media",       ST_ID3V2_PT_MEDIA},
		{0x07, "Lead artist",                   "artist-lead", ST_ID3V2_PT_LEADART},
		{0x08, "Artist/performer",              "artist",      ST_ID3V2_PT_ART},
		{0x09, "Conductor",                     "conduct",     ST_ID3V2_PT_COND},
		{0x0a, "Band/Orchestra",                "band",        ST_ID3V2_PT_BAND},
		{0x0b, "Composer",                      "comp",        ST_ID3V2_PT_COMP},
		{0x0c, "Lyricist",                      "lyricist",    ST_ID3V2_PT_LYRIC},
		{0x0d, "Recording Location",            "rec-loc",     ST_ID3V2_PT_RECLOC},
		{0x0e, "During recording",              "dur-rec",     ST_ID3V2_PT_DURREC},
		{0x0f, "During performance",            "dur-perf",    ST_ID3V2_PT_DURPER},
		{0x10, "Movie/video screen capture",    "screen",      ST_ID3V2_PT_SCRCAP},
		{0x11, "A bright coloured fish",        "flash",       ST_ID3V2_PT_FLASH},
		{0x12, "Illustration",                  "illus",       ST_ID3V2_PT_ILLUS},
		{0x13, "Band/artist logotype",          "logo-band",   ST_ID3V2_PT_BLOGO},
		{0x14, "Publisher/Studio logotype",     "logo-studio", ST_ID3V2_PT_PLOGO},
		{0xff, NULL, NULL, ST_ID3V2_PT_NONE} };
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_TAG_ID3V2_FNC_C
	/** */
	static void
	ST_TIV2__fnc_getFirstValidFld_byID(Tst_id3v2_tag *pTag,
	                                   const Tst_id3v2_frID id,
	                                   ST_OPTARG(const Tst_str *pIDstr),
	                                   ST_OPTARG(Tst_id3v2_fldData **ppDat),
	                                   ST_OPTARG(Tst_uint32 *pNr));
	/** */
	static Tst_uint32
	ST_TIV2__fnc_getNewFldNr(Tst_id3v2_tag *pTag,
	                         const Tst_id3v2_frID frID,
	                         ST_OPTARG(const Tst_str *pFrIDstr));

	/** */
	static Tst_bool
	ST_TIV2__fnc_isInRWOarr(const Tst_id3v2_fldd_rword_typ rwotp,
	                        const Tst_id3v2_fldd_rwordPArr pRdWrOrderArr,
	                        const Tst_uint32 rwoArrSz);
	/** */
	static const Tst_id3v2_picTp_arrElem*
	ST_TIV2__fnc_getPicTp_arrE(const Tst_byte bin, const Tst_str *pCap,
	                           const Tst_str *pCapSh,
	                           const Tst_id3v2_picTp ptpE);
	/** */
	static Tst_uint32
	ST_TIV2__fnc_getListIndex_byUIX(Tst_id3v2_tag_intn *pTagI,
	                                const Tst_uint32 uniqueIx);
	/** */
	static Tst_err
	ST_TIV2__fnc_sortFields_byPrio_sub1(Tst_id3v2_tag *pTag,
	                                    const Tst_bool sortPicTypes,
	                                    Tst_id3v2_fieldsOutp *pPtArr);
	static Tst_err
	ST_TIV2__fnc_sortFields_byPrio_sub2(Tst_id3v2_tag *pTag,
	                                    Tst_id3v2_fieldsOutp *pPtArr,
	                                    const Tst_id3v2_frTp prioFTP,
	                                    const Tst_id3v2_frID prioFID,
	                                    const Tst_id3v2_picTp prioPtp);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_FNC_PRIV_H */

/******************************************************************************/
