/******************************************************************************/
/* tag_apev2-defs.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for APEv2 Tag                                                  */
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

#ifndef HEAD_TAG_APEV2_DEFS_H
#define HEAD_TAG_APEV2_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_APEV2_TAG_NAME     "APEv2"
#define ST_APEV2_TAG_NAME_SH  "av2"

/** Field Types */
typedef enum {
	ST_APEV2_FTP_STR,   /* string */
	ST_APEV2_FTP_INT,   /* string: integer in string repesentation */
	ST_APEV2_FTP_FLT,   /* string: floating point number in str. repr. */
	ST_APEV2_FTP_YEAR,  /* string: year in str. repr. */
	ST_APEV2_FTP_TSTA,  /* string: timestamp in str. repr. */
	ST_APEV2_FTP_LYR,   /* string: lyrics */
	ST_APEV2_FTP_URL,   /* string: URL */
	ST_APEV2_FTP_BDAT,  /* binary data */
	ST_APEV2_FTP_NONE   /* none ! this must be the last elem ! */
} Tst_apev2_frTp;

/** Field IDs */
typedef enum {
	ST_APEV2_FID_TITL,  /* Title */
	ST_APEV2_FID_SUBT,  /* Subtitle */
	ST_APEV2_FID_ARTI,  /* Artist */
	ST_APEV2_FID_ALBU,  /* Album */
	ST_APEV2_FID_DEBA,  /* Debut Album */
	ST_APEV2_FID_PUBL,  /* Publisher */
	ST_APEV2_FID_COND,  /* Conductor */
	ST_APEV2_FID_TRKN,  /* Track number */
	ST_APEV2_FID_TRKT,  /* Tracks total (unofficial) */
	ST_APEV2_FID_DSCN,  /* Disc number (unofficial) */
	ST_APEV2_FID_DSCT,  /* Discs total (unofficial) */
	ST_APEV2_FID_COMP,  /* Composer */
	ST_APEV2_FID_COMM,  /* Comment */
	ST_APEV2_FID_COPY,  /* Copyright */
	ST_APEV2_FID_PUBR,  /* Publicationright */
	ST_APEV2_FID_FILE,  /* File */
	ST_APEV2_FID_EANU,  /* EAN/UPC */
	ST_APEV2_FID_ISBN,  /* ISBN */
	ST_APEV2_FID_CATA,  /* Catalog */
	ST_APEV2_FID_LABC,  /* LC */
	ST_APEV2_FID_YEAR,  /* Year */
	ST_APEV2_FID_RECD,  /* Recording Date */
	ST_APEV2_FID_RECL,  /* Recording Location */
	ST_APEV2_FID_GENR,  /* Genre */
	ST_APEV2_FID_MEDI,  /* Media */
	ST_APEV2_FID_INDX,  /* Index */
	ST_APEV2_FID_RELA,  /* Related */
	ST_APEV2_FID_ISRC,  /* ISRC */
	ST_APEV2_FID_ABST,  /* Abstract */
	ST_APEV2_FID_LANG,  /* Language */
	ST_APEV2_FID_BIBL,  /* Bibliography */
	ST_APEV2_FID_INTR,  /* Introplay */
	ST_APEV2_FID_RGTG,  /* ReplayGain Track Gain (unofficial) */
	ST_APEV2_FID_RGTP,  /* ReplayGain Track Peak (unofficial) */
	ST_APEV2_FID_RGAG,  /* ReplayGain Album Gain (unofficial) */
	ST_APEV2_FID_RGAP,  /* ReplayGain Album Peak (unofficial) */
	ST_APEV2_FID_IPLS,  /* Involved People (unofficial) */
	ST_APEV2_FID_TAGG,  /* Tagging application used (unofficial) */
	ST_APEV2_FID_CBIN,  /* Custom binary field */
	ST_APEV2_FID_CTXT,  /* Custom text field */
	ST_APEV2_FID_NONE   /* none ! this must be the last elem ! */
} Tst_apev2_frID;


/*
// Types
*/
/** APEv2 tag frames: what kind of data does/can field contain ? */
typedef struct {
	Tst_bool hasBDat;  /* binary data ? */
	Tst_bool hasText;  /* text ? */
} Tst_apev2_fldd_dattp;

/** APEv2 tag frames: what kind of string(s) does strArr contain ? */
typedef struct {
	Tst_bool hasNumStr;     /* numerical string (e.g. "90182") ? */
	Tst_bool hasYearStr;    /* numerical year string, YYYY (e.g. "2001") ? */
	Tst_bool hasTStampStr;  /* numerical time stamp string ?
	                         *   (e.g. "2002-01-17T21:22:01")  */
	/* */
	Tst_bool hasGenreStr;   /* genre string (e.g. "(9)Metal") ? */
	Tst_bool hasSimpleStr;  /* simple string without \n ? */
	Tst_bool hasUrlStr;     /* URL string (email or other URL) ? */
	Tst_bool hasFullStr;    /* full text string with \n ? */
} Tst_apev2_fldd_strtp;

/** APEv2 tag: complete field props, incl. ID String, Caption, ... */
typedef struct {
	void *pObInternal;  /* pointer to Tst_apev2_fldProp_intn */
} Tst_apev2_fldProp;

/** APEv2 tag: data of one field */
typedef struct {
	void *pObInternal;  /* pointer to Tst_apev2_fldData_intn */
} Tst_apev2_fldData;

/** APEv2 Tag */
typedef struct {
	void *pObInternal;  /* pointer to Tst_apev2_tag_intn */
} Tst_apev2_tag;


ST_END_C_DECLS

#endif  /* HEAD_TAG_APEV2_DEFS_H */

/******************************************************************************/
