/******************************************************************************/
/* cont_wav-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitons for WAV containers                                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.07.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CONT_WAV_DEFS_H
#define HEAD_CONT_WAV_DEFS_H

/*
// Own-Includes
*/
#include <santag/lib_defs.h>


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_CONTWAV_AUD_NAME     "WAVE"
#define ST_CONTWAV_AUD_NAME_SH  "wav"

/** */
typedef enum {
	ST_CONTWAV_WAVEFMT_PCM        = 0x0001,  /* PCM */
	ST_CONTWAV_WAVEFMT_IEEE_FLOAT = 0x0003,  /* IEEE float */
	ST_CONTWAV_WAVEFMT_ALAW       = 0x0006,  /* 8-bit ITU-T G.711 A-law */
	ST_CONTWAV_WAVEFMT_MULAW      = 0x0007,  /* 8-bit ITU-T G.711 mu-law */
	ST_CONTWAV_WAVEFMT_EXTENSIBLE = 0xfffe,  /* extensible wave-format */
	ST_CONTWAV_WAVEFMT_NONE       = 0xffff
} Tst_contWav_fmttag;


/*
// Types
*/
/** */
typedef struct {
	void *pObInternal;  /* pointer to Tst_contWav_handle_intn */
} Tst_contWav_handle;

/** */
typedef struct {
	void *pObInternal;  /* pointer to Tst_contWav_obj_intn */
} Tst_contWav_obj;

/** */
typedef void (*Tst_contWav_cbNewOutputStream)(const Tst_uint32 bsIx,
                                              const Tst_uint32 bsSIx,
                                              const Tst_str *pFilen,
                                              const Tst_bool pretending);


ST_END_C_DECLS

#endif  /* HEAD_CONT_WAV_DEFS_H */

/******************************************************************************/
