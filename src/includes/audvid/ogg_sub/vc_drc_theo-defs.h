/******************************************************************************/
/* vc_drc_theo-defs.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for Ogg-Dirac and Ogg-Theora -- included by cont_ogg-defs.h    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.06.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_VC_DRC_THEO_DEFS_H
#define HEAD_VC_DRC_THEO_DEFS_H


ST_BEGIN_C_DECLS

/*
// Types
*/
/** OGG Dirac info */
typedef struct {
	Tst_uint32 versMaj;  /* major version */
	Tst_uint32 versMin;  /* minor version */
	Tst_uint32 width;    /* video dimension, width */
	Tst_uint32 height;   /* video dimension, height */
	double     fps;      /* frames per sec */
} Tst_contOgg_diracInfo;

/** OGG Theora info */
typedef struct {
	Tst_byte   versMaj;     /* major version */
	Tst_byte   versMin;     /* minor version */
	Tst_byte   versRev;     /* revision version */
	Tst_uint32 width;       /* video dimension, width */
	Tst_uint32 height;      /* video dimension, height */
	double     fps;         /* frames per sec */
	Tst_uint32 bitrateKBS;  /* target bitrate in kbit/s */
	Tst_uint32 quality;     /* quality */
} Tst_contOgg_theoraInfo;


ST_END_C_DECLS

#endif  /* HEAD_VC_DRC_THEO_DEFS_H */

/******************************************************************************/
