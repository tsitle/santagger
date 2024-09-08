/******************************************************************************/
/* ac_vorb-defs.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for Ogg-Vorbis -- included by cont_ogg-defs.h                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AC_VORB_DEFS_H
#define HEAD_AC_VORB_DEFS_H


ST_BEGIN_C_DECLS

/*
// Types
*/
/** OGG Vorbis Info */
typedef struct {
	Tst_uint32 vers;           /* vorbis version */
	Tst_uint32 sampleRate;     /* samplerate in Hz */
	Tst_uint32 bitrateMinKBS;  /* min. bitrate in kbit/s */
	Tst_uint32 bitrateNomKBS;  /* nominal bitrate in kbit/s */
	Tst_uint32 bitrateMaxKBS;  /* max. bitrate in kbit/s */
	Tst_byte   channels;       /* channels (1-8 per standard) */
} Tst_contOgg_vorbisInfo;


ST_END_C_DECLS

#endif  /* HEAD_AC_VORB_DEFS_H */

/******************************************************************************/
