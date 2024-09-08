/******************************************************************************/
/* ac_spx_clt-defs.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for Ogg-Speex and Ogg-Celt -- included by cont_ogg-defs.h      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AC_SPX_CLT_DEFS_H
#define HEAD_AC_SPX_CLT_DEFS_H


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** speex mode */
typedef enum {
	ST_CONTOGG_SPEEXMODE_NAR = 0,  /* narrow band */
	ST_CONTOGG_SPEEXMODE_WID = 1,  /* wide band */
	ST_CONTOGG_SPEEXMODE_UW  = 2,  /* ultrawide band */
	ST_CONTOGG_SPEEXMODE_NONE
} Tst_contOgg_speexMode;


/*
// Types
*/
/** OGG Speex info */
typedef struct {
	Tst_str    versStr[256];  /* version string for Speex */
	Tst_uint32 speexVers;     /* version for Speex */
	Tst_uint32 bitstrVers;    /* bitstream version */
	/* */
	Tst_uint32            sampleRate;  /* sampling rate used */
	Tst_contOgg_speexMode mode;        /* mode used */
	Tst_uint32            channels;    /* number of channels encoded */
	/* */
	Tst_uint32 bitrateKBS;  /* bitrate in kbit/s */
	Tst_bool   isVBR;       /* 1 for a VBR encoding, 0 otherwise */
} Tst_contOgg_speexInfo;

/** OGG Celt info */
typedef struct {
	Tst_str    versStr[256];  /* version string */
	Tst_uint32 bitstrVers;    /* bitstream version */
	/* */
	Tst_uint32 sampleRate;    /* sampling rate of the original audio */
	Tst_uint32 channels;      /* number of channels */
	/* */
	Tst_uint32 bitrateKBS;    /* bitrate in kbit/s */
} Tst_contOgg_celtInfo;


ST_END_C_DECLS

#endif  /* HEAD_AC_SPX_CLT_DEFS_H */

/******************************************************************************/
