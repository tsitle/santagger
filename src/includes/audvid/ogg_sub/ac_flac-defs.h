/******************************************************************************/
/* ac_flac-defs.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for (Ogg-)Flac -- included by cont_ogg-defs.h                  */
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

#ifndef HEAD_AC_FLAC_DEFS_H
#define HEAD_AC_FLAC_DEFS_H


ST_BEGIN_C_DECLS

/*
// Types
*/
/** (OGG) Flac info */
typedef struct {
	Tst_byte   oggFlacVersMaj;  /* major version */
	Tst_byte   oggFlacVersMin;  /* minor version */
	/* */
	Tst_uint32 sampleRate;     /* samplerate */
	Tst_byte   channels;       /* number of channels */
	Tst_byte   bitsPerSample;  /* bits per sample */
	/* */
	Tst_bool   md5OfDecodedAudioChecked;
	Tst_bool   md5OfDecodedAudioOK;
} Tst_contOgg_flacInfo;


ST_END_C_DECLS

#endif  /* HEAD_AC_FLAC_DEFS_H */

/******************************************************************************/
