/******************************************************************************/
/* tag_meta-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for Meta Tags                                                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_META_DEFS_H
#define HEAD_TAG_META_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/tag/tag_id3v1-defs.h"
#include "src/includes/tag/tag_id3v2-defs.h"
#include "src/includes/tag/tag_apev2-defs.h"
#include "src/includes/tag/tag_vorbc-defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** tag descriptions */
extern const char *ST_MTAG_TAGDSC_IV1[2];
extern const char *ST_MTAG_TAGDSC_IV2[2];
extern const char *ST_MTAG_TAGDSC_AV2[2];
extern const char *ST_MTAG_TAGDSC_VOR[2];
extern const char *ST_MTAG_TAGDSC_MRG[2];
extern const char *ST_MTAG_TAGDSC_NA[2];

/** tag types */
typedef enum {
	ST_MTAG_TTP_IV1,  /* ID3v1 */
	ST_MTAG_TTP_IV2,  /* ID3V2 */
	ST_MTAG_TTP_AV2,  /* APEv2 */
	ST_MTAG_TTP_VOR,  /* Vorbis Comment */
	/* */
	ST_MTAG_TTP_MRG,  /* merged tags (to ID3v2) */
	/* */
	ST_MTAG_TTP_NONE  /* ! this must be the last element */
} Tst_tagMeta_type;


/*
// Types
*/
/** Meta Tag */
typedef struct {
	void *pObInternal;  /* pointer to Tst_tagMeta_mt_intn */
} Tst_tagMeta_mt;

/** Meta Tag array */
typedef struct {
	void *pObInternal;  /* pointer to Tst_tagMeta_mtArr_intn */
} Tst_tagMeta_mtArr;

/** Callbacks for setting options */
typedef void
        (*Tst_tagMeta_cbSetOpts_iv1)(void *pSetOptsData, Tst_id3v1_tag *pTag);
typedef void
        (*Tst_tagMeta_cbSetOpts_iv2)(void *pSetOptsData, Tst_id3v2_tag *pTag);
typedef void
        (*Tst_tagMeta_cbSetOpts_av2)(void *pSetOptsData, Tst_apev2_tag *pTag);
typedef void
        (*Tst_tagMeta_cbSetOpts_vor)(void *pSetOptsData, Tst_vorbc_tag *pTag);


ST_END_C_DECLS

#endif  /* HEAD_TAG_META_DEFS_H */

/******************************************************************************/
