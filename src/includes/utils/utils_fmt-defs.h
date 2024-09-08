/******************************************************************************/
/* utils_fmt-defs.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for file formats etc.                                          */
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

#ifndef HEAD_UTILS_FMT_DEFS_H
#define HEAD_UTILS_FMT_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** Some mime types */
typedef enum {
	ST_UTILSFMT_MTP_AUDFLAC,    /* Flac audio */
	ST_UTILSFMT_MTP_AUDMPEG1,   /* MPEG-1 Layer I-III audio */
	ST_UTILSFMT_MTP_AUDMPEG4,   /* MPEG-4 audio */
	ST_UTILSFMT_MTP_AUDMPEGURL, /* MPEG URL */
	ST_UTILSFMT_MTP_AUDOGG,     /* OGG audio */
	ST_UTILSFMT_MTP_AUDWMA,     /* WMA audio */
	ST_UTILSFMT_MTP_AUDWAV,     /* RIFF Wave audio */
	ST_UTILSFMT_MTP_AUDAIFF,    /* AIFF audio */
	/* */
	ST_UTILSFMT_MTP_VID3GP,     /* 3GP video */
	ST_UTILSFMT_MTP_VIDMPEG,    /* MPEG video */
	ST_UTILSFMT_MTP_VIDMPEG4,   /* MPEG-4 video */
	ST_UTILSFMT_MTP_VIDQT,      /* Qt video */
	ST_UTILSFMT_MTP_VIDOGG,     /* OGG video */
	ST_UTILSFMT_MTP_VIDAVI,     /* AVI video */
	ST_UTILSFMT_MTP_VIDMKV,     /* MKV video */
	/* */
	ST_UTILSFMT_MTP_AUDVIDOGG,  /* OGG audio/video */
	/* */
	ST_UTILSFMT_MTP_PICJPG,
	ST_UTILSFMT_MTP_PICPNG,
	ST_UTILSFMT_MTP_PICTIF,
	ST_UTILSFMT_MTP_PICGIF,
	ST_UTILSFMT_MTP_PICPSD,
	ST_UTILSFMT_MTP_PICICO,
	ST_UTILSFMT_MTP_PICBMP,
	/* */
	/** */
	ST_UTILSFMT_MTP_APPOCTET,
	ST_UTILSFMT_MTP_APPWORD,
	ST_UTILSFMT_MTP_APPOGG,
	ST_UTILSFMT_MTP_APPPDF,
	ST_UTILSFMT_MTP_APPPGPK,
	ST_UTILSFMT_MTP_APPPGPS,
	ST_UTILSFMT_MTP_APPPS,
	ST_UTILSFMT_MTP_APPRTF,
	ST_UTILSFMT_MTP_APPXHTML,
	ST_UTILSFMT_MTP_APPXML,
	ST_UTILSFMT_MTP_APPODT,
	ST_UTILSFMT_MTP_APPSDW,
	ST_UTILSFMT_MTP_APPTORR,
	ST_UTILSFMT_MTP_APPTAG,     /* Tag file, this is my own MIME type */
	ST_UTILSFMT_MTP_APPTAGIV1,  /* ID3v1 Tag file, this is my own MIME type */
	ST_UTILSFMT_MTP_APPTAGIV2,  /* ID3v2 Tag file, this is my own MIME type */
	ST_UTILSFMT_MTP_APPTAGAV2,  /* APEv2 Tag file, this is my own MIME type */
	ST_UTILSFMT_MTP_APPTAGVOR,  /* Pseudo Vorbis Tag file, this is my own MIME type */
	/** */
	ST_UTILSFMT_MTP_APP7Z,
	ST_UTILSFMT_MTP_APPDMG,
	ST_UTILSFMT_MTP_APPGTAR,
	ST_UTILSFMT_MTP_APPGZ,
	ST_UTILSFMT_MTP_APPRAR,
	ST_UTILSFMT_MTP_APPTAR,
	ST_UTILSFMT_MTP_APPZIP,
	/* */
	ST_UTILSFMT_MTP_TXTHTML,
	ST_UTILSFMT_MTP_TXTPLAIN,
	/* */
	ST_UTILSFMT_MTP_NONE
} Tst_utilsFmt_mimeTp;

/** Some picture formats */
typedef enum {
	ST_UTILSFMT_PF_JPG,
	ST_UTILSFMT_PF_PNG,
	ST_UTILSFMT_PF_TIF,
	ST_UTILSFMT_PF_GIF,
	ST_UTILSFMT_PF_PSD,
	ST_UTILSFMT_PF_ICO,
	ST_UTILSFMT_PF_BMP,
	ST_UTILSFMT_PF_NONE  /* none, ! this must be the last elem ! */
} Tst_utilsFmt_picFmt;


ST_END_C_DECLS

#endif  /* HEAD_UTILS_FMT_DEFS_H */

/******************************************************************************/
