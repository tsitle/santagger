/******************************************************************************/
/* tag_id3-defs.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Common definitions for ID3 Tags                                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3_DEFS_H
#define HEAD_TAG_ID3_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants, Part I
*/
/** */
#define ST_ID3_TFLD_YEAR_MIN        1500             /* for ID3v1 + ID3v2 */
#define ST_ID3_TFLD_YEAR_MAX        2500             /* for ID3v1 + ID3v2 */
#define ST_ID3_TFLD_YEAR_NONE_IV1   ((Tst_int32)-1)  /* for ID3v1 */
#define ST_ID3_TFLD_TRACK_MIN_IV1   1                /* for ID3v1 */
#define ST_ID3_TFLD_TRACK_MAX_IV1   255              /* for ID3v1 */
#define ST_ID3_TFLD_TRACK_MAX_IV2   9999999          /* for ID3v2 */
#define ST_ID3_TFLD_TRACK_NONE_IV1  ((Tst_int32)-1)  /* for ID3v1 */
#define ST_ID3_TFLD_GENRE_MIN_IV1   0                /* for ID3v1 */
#define ST_ID3_TFLD_GENRE_MAX_IV1   254              /* for ID3v1 */
#define ST_ID3_TFLD_GENRE_MAX_IV2   9999999          /* for ID3v2 */
#define ST_ID3_TFLD_GENRE_NONE_IV1  255              /* for ID3v1 */

/** */
typedef enum {
	ST_ID3_GEN_SPEC_COV,
	ST_ID3_GEN_SPEC_RMX,
	ST_ID3_GEN_SPEC_NONE  /* - none ! this must be the last elem ! */
} Tst_id3_genre_spec_id;


/*
// Types
*/
/** for ID3v1 & ID3v2 genres */
typedef struct {
	Tst_byte   nr;     /* number */
	char const *pCap;  /* caption */
	Tst_bool   isStd;  /* is standard ? */
} Tst_id3_genre;

/** for special ID3v2 genres */
typedef struct {
	Tst_id3_genre_spec_id specID;
	char const            *pIDstr;
	char const            *pCap;
} Tst_id3_genre_spec;


/*
// Constants, Part II
*/
/** */
extern ST_EXPORT const Tst_byte           ST_ID3_GENRE_DEFMAXNR;
/** ID3v2-only genres */
extern ST_EXPORT const Tst_id3_genre_spec ST_ID3_GENRES_SPEC[];


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3_DEFS_H */

/******************************************************************************/
