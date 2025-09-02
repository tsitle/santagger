/******************************************************************************/
/* aud_mpeg1-defs.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for MPEG-1 audio (Layer I-III)                                 */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 30.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_AUD_MPEG1_DEFS_H
#define LIBSANTAGGER_AUD_MPEG1_DEFS_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_MPEG1_AUD_NAME     "MPEG-1"
#define ST_MPEG1_AUD_NAME_SH  "mpg1"

/** Channel modes */
typedef enum {
	ST_MPEG1_CMODE_STEREO  = 1,  /* stero */
	ST_MPEG1_CMODE_JSTEREO = 2,  /* joint stereo */
	ST_MPEG1_CMODE_DUAL    = 3,  /* dual channel, 2x mono */
	ST_MPEG1_CMODE_SINGLE  = 4,  /* single channel, 1x mono */
	ST_MPEG1_CMODE_NONE    = 0   /* none/unknown */
} Tst_mpeg1_channMode;

/** VBR types */
typedef enum {
	ST_MPEG1_VBRTYP_XING = 1,  /* Xing */
	ST_MPEG1_VBRTYP_VBRI = 2,  /* Fraunhofer VBRI */
	ST_MPEG1_VBRTYP_INFO = 3,  /* CBR Info Header */
	ST_MPEG1_VBRTYP_NONE = 0   /* none/unknown */
} Tst_mpeg1_vbrTyp;

/** MPEG Layer */
typedef enum {
	ST_MPEG1_LAYER_1    = 1,
	ST_MPEG1_LAYER_2    = 2,
	ST_MPEG1_LAYER_3    = 3,
	ST_MPEG1_LAYER_NONE = 0
} Tst_mpeg1_layer;

/** MPEG Audio Version */
typedef enum {
	ST_MPEG1_AUDVERS_1    = 1,
	ST_MPEG1_AUDVERS_2    = 2,
	ST_MPEG1_AUDVERS_25   = 25,
	ST_MPEG1_AUDVERS_NONE = 0
} Tst_mpeg1_audioVers;


/*
// Types
*/
/** MPEG stream info */
typedef struct {
	void *pObInternal;  /* pointer to Tst_mpeg1_obj_intn */
} Tst_mpeg1_obj;


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_AUD_MPEG1_DEFS_H */

/******************************************************************************/
