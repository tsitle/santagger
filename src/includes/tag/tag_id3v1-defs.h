/******************************************************************************/
/* tag_id3v1-defs.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for ID3v1 (Extended) Tags                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V1_DEFS_H
#define HEAD_TAG_ID3V1_DEFS_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_ID3V1_TAG_NAME     "ID3v1"
#define ST_ID3V1_TAG_NAME_SH  "iv1"

/** ID3v1 frame types */
typedef enum {
	ST_ID3V1_FTP_STR,  /* Text */
	ST_ID3V1_FTP_INT,  /* Integer */
	ST_ID3V1_FTP_NONE  /* none ! this must be the last elem ! */
} Tst_id3v1_frTp;

/** ID3v1 Field IDs */
typedef enum {
	ST_ID3V1_FID_TITL,  /* Title */
	ST_ID3V1_FID_ALBU,  /* Album */
	ST_ID3V1_FID_TRKN,  /* Track */
	ST_ID3V1_FID_ARTI,  /* Artist */
	ST_ID3V1_FID_COMM,  /* Comment */
	ST_ID3V1_FID_GENR,  /* Genre */
	ST_ID3V1_FID_YEAR,  /* Year */
	ST_ID3V1_FID_NONE   /* none ! this must be the last elem ! */
} Tst_id3v1_frID;


/*
// Types
*/
/** ID3v1 tag: frame properties, incl. ID String, Caption, ... */
typedef struct {
	void *pObInternal;  /* pointer to Tst_id3v1_fldProp_intn */
} Tst_id3v1_fldProp;

/** ID3v1 (Extended) Tag: data of one 'field' */
typedef struct {
	void *pObInternal;  /* pointer to Tst_id3v1_fldData_intn */
} Tst_id3v1_fldData;

/** ID3v1 (Extended) Tag */
typedef struct {
	void *pObInternal;  /* pointer to Tst_id3v1_tag_intn */
} Tst_id3v1_tag;


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V1_DEFS_H */

/******************************************************************************/
