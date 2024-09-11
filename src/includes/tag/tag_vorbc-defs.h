/******************************************************************************/
/* tag_vorbc-defs.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for Vorbis Tag                                                 */
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

#ifndef HEAD_TAG_VORBC_DEFS_H
#define HEAD_TAG_VORBC_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_VORBC_TAG_NAME     "Vorbis"
#define ST_VORBC_TAG_NAME_SH  "vor"

/** Vorbis picture types (same as ID3v2) */
typedef enum {
	ST_VORBC_PT_OTHER   = 0x00, /* Other */
	ST_VORBC_PT_3232    = 0x01, /* 32x32 pixels 'file icon' (PNG only) */
	ST_VORBC_PT_OFICON  = 0x02, /* Other file icon */
	ST_VORBC_PT_COVFRO  = 0x03, /* Cover (front) */
	ST_VORBC_PT_COVBAC  = 0x04, /* Cover (back) */
	ST_VORBC_PT_LEAFL   = 0x05, /* Leaflet page */
	ST_VORBC_PT_MEDIA   = 0x06, /* Media (e.g. lable side of CD) */
	ST_VORBC_PT_LEADART = 0x07, /* Lead artist/lead performer/soloist */
	ST_VORBC_PT_ART     = 0x08, /* Artist/performer */
	ST_VORBC_PT_COND    = 0x09, /* Conductor */
	ST_VORBC_PT_BAND    = 0x0a, /* Band/Orchestra */
	ST_VORBC_PT_COMP    = 0x0b, /* Composer */
	ST_VORBC_PT_LYRIC   = 0x0c, /* Lyricist/text writer */
	ST_VORBC_PT_RECLOC  = 0x0d, /* Recording Location */
	ST_VORBC_PT_DURREC  = 0x0e, /* During recording */
	ST_VORBC_PT_DURPER  = 0x0f, /* During performance */
	ST_VORBC_PT_SCRCAP  = 0x10, /* Movie/video screen capture */
	ST_VORBC_PT_FLASH   = 0x11, /* A bright coloured fish */
	ST_VORBC_PT_ILLUS   = 0x12, /* Illustration */
	ST_VORBC_PT_BLOGO   = 0x13, /* Band/artist logotype */
	ST_VORBC_PT_PLOGO   = 0x14, /* Publisher/Studio logotype */
	ST_VORBC_PT_NONE    = 0x15  /* none ! this must be the last elem ! */
} Tst_vorbc_picTp;

/** Field Types */
typedef enum {
	ST_VORBC_FTP_STR,   /* string */
	ST_VORBC_FTP_PICT,  /* picture */
	ST_VORBC_FTP_NONE   /* none ! this must be the last elem ! */
} Tst_vorbc_frTp;

/** Field IDs */
typedef enum {
	ST_VORBC_FID_TITL,  /* Title */
	ST_VORBC_FID_VERS,  /* Version (e.g. of remix) */
	ST_VORBC_FID_ALBU,  /* Album */
	ST_VORBC_FID_TRKN,  /* Track */
	ST_VORBC_FID_TRKT,  /* Tracks total (unofficial) */
	ST_VORBC_FID_DSCN,  /* Disc (unofficial) */
	ST_VORBC_FID_DSCT,  /* Discs total (unofficial) */
	ST_VORBC_FID_ARTI,  /* Artist (unofficial) */
	ST_VORBC_FID_PERF,  /* Performer */
	ST_VORBC_FID_COPY,  /* Copyright */
	ST_VORBC_FID_LICS,  /* License */
	ST_VORBC_FID_ORGA,  /* Organization */
	ST_VORBC_FID_DESC,  /* Description */
	ST_VORBC_FID_GENR,  /* Genre */
	ST_VORBC_FID_RECD,  /* Recording Date */
	ST_VORBC_FID_RECL,  /* Recording Location */
	ST_VORBC_FID_CONT,  /* Contact */
	ST_VORBC_FID_ISRC,  /* ISRC */
	ST_VORBC_FID_BPMI,  /* BeatsPerMinute (unofficial) */
	ST_VORBC_FID_TMPO,  /* Tempo/BPM (unofficial) */
	ST_VORBC_FID_LYRI,  /* Lyrics (unofficial) */
	ST_VORBC_FID_MBPI,  /* Picture (unofficial) */
	ST_VORBC_FID_RGTG,  /* ReplayGain Track Gain (unofficial) */
	ST_VORBC_FID_RGTP,  /* ReplayGain Track Peak (unofficial) */
	ST_VORBC_FID_RGAG,  /* ReplayGain Album Gain (unofficial) */
	ST_VORBC_FID_RGAP,  /* ReplayGain Album Peak (unofficial) */
	ST_VORBC_FID_ENCO,  /* Encoder (unofficial) */
	ST_VORBC_FID_IPLS,  /* Involved People (unofficial) */
	ST_VORBC_FID_TAGG,  /* Tagging application used (unofficial) */
	ST_VORBC_FID_CTXT,  /* Custom text field */
	ST_VORBC_FID_NONE   /* none ! this must be the last elem ! */
} Tst_vorbc_frID;


/*
// Types
*/
/** Vorbis tag frames: what kind of data does/can field contain ? */
typedef struct {
	Tst_bool hasBDat;  /* binary data ? */
	Tst_bool hasPict;  /* picture ?
	                    *   imgfmt, ptp (+desc +bindata)  */
	Tst_bool hasText;  /* text ? */
} Tst_vorbc_fldd_dattp;

/** Vorbis tag frames: what kind of string(s) does strArr contain ? */
typedef struct {
	Tst_bool hasNumStr;     /* numerical string (e.g. "90182") ? */
	Tst_bool hasTStampStr;  /* numerical time stamp string ?
	                         *   (e.g. "2002-01-17T21:22:01")  */
	/* */
	Tst_bool hasGenreStr;   /* genre string (e.g. "(9)Metal") ? */
	Tst_bool hasSimpleStr;  /* simple string without \n ? */
	Tst_bool hasFullStr;    /* full text string with \n ? */
} Tst_vorbc_fldd_strtp;

/** Vorbis tag: complete field props, incl. ID String, Caption, ... */
typedef struct {
	void *pObInternal;  /* pointer to Tst_vorbc_fldProp_intn */
} Tst_vorbc_fldProp;

/** Vorbis tag: data of one field */
typedef struct {
	void *pObInternal;  /* pointer to Tst_vorbc_fldData_intn */
} Tst_vorbc_fldData;

/** Vorbis Tag */
typedef struct {
	void *pObInternal;  /* pointer to Tst_vorbc_tag_intn */
} Tst_vorbc_tag;


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_DEFS_H */

/******************************************************************************/
