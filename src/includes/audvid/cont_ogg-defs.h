/******************************************************************************/
/* cont_ogg-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for OGG containers / Flac files                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CONT_OGG_DEFS_H
#define HEAD_CONT_OGG_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/lib_defs.h"
#include "src/includes/audvid/ogg_sub/mc_vorbcomm-defs.h"
#include "src/includes/audvid/ogg_sub/ac_flac-defs.h"
#include "src/includes/audvid/ogg_sub/ac_spx_clt-defs.h"
#include "src/includes/audvid/ogg_sub/ac_vorb-defs.h"
#include "src/includes/audvid/ogg_sub/vc_drc_theo-defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_CONTOGG_AUDVID_NAME     "OGG"
#define ST_CONTOGG_AUDVID_NAME_SH  "ogg"

/** bitstream type */
typedef enum {
	ST_CONTOGG_BSTP_NONE = 0,
	ST_CONTOGG_BSTP_VORB = 1,      /* Vorbis - Audio */
	ST_CONTOGG_BSTP_THEO = 2,      /* Theora - Video */
	ST_CONTOGG_BSTP_SPEE = 4,      /* Speex - Audio */
	ST_CONTOGG_BSTP_FLAC = 8,      /* Flac - Audio */
	ST_CONTOGG_BSTP_PCM  = 16,     /* PCM - Audio */
	ST_CONTOGG_BSTP_MIDI = 32,     /* MIDI - Audio */
	ST_CONTOGG_BSTP_CELT = 64,     /* CELT - Audio */
	ST_CONTOGG_BSTP_DIRA = 128,    /* Dirac - Video */
	ST_CONTOGG_BSTP_UVS  = 256,    /* UVS - Video */
	ST_CONTOGG_BSTP_YUV  = 512,    /* YUV4MPEG2 - Video */
	ST_CONTOGG_BSTP_CMML = 1024,   /* CMML - Text */
	ST_CONTOGG_BSTP_KATE = 2048,   /* Kate - Text */
	ST_CONTOGG_BSTP_PNG  = 4096,   /* PNG - Picture */
	ST_CONTOGG_BSTP_JNG  = 8192,   /* JNG - Picture */
	ST_CONTOGG_BSTP_MNG  = 16384,  /* MNG - Animation */
	ST_CONTOGG_BSTP_SKEL = 32768   /* Skeleton */
} Tst_contOgg_bsType;

/** bitstream content-types */
typedef enum {
	ST_CONTOGG_BSCTP_NONE = 0,
	ST_CONTOGG_BSCTP_AUD  = 1,   /* audio */
	ST_CONTOGG_BSCTP_VID  = 2,   /* video */
	ST_CONTOGG_BSCTP_TXT  = 4,   /* text */
	ST_CONTOGG_BSCTP_PIC  = 8,   /* picture */
	ST_CONTOGG_BSCTP_ANI  = 16,  /* animation */
	ST_CONTOGG_BSCTP_SKE  = 32   /* skeleton */
} Tst_contOgg_bsContType;


/*
// Types
*/
/** OGG sub stream */
typedef struct {
	void *pObInternal;  /* pointer to Tst_contOgg_substr_intn */
} Tst_contOgg_substream;

/** OGG file info (one Ogg file may contain multiple streams) */
typedef struct {
	void *pObInternal;  /* pointer to Tst_contOgg_obj_intn */
} Tst_contOgg_obj;


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_DEFS_H */

/******************************************************************************/
