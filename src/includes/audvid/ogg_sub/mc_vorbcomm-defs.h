/******************************************************************************/
/* mc_vorbcomm-defs.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for Vorbis Comments -- included by cont_ogg-defs.h             */
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

#ifndef LIBSANTAGGER_MC_VORBCOMM_DEFS_H
#define LIBSANTAGGER_MC_VORBCOMM_DEFS_H


ST_BEGIN_C_DECLS

/*
// Types
*/
/** OGG Vorbis comment header */
typedef struct {
	void *pObInternal;  /* pointer to Tst_contOgg_vorbCmtRaw_intn */
} Tst_contOgg_vorbCommentRaw;


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_MC_VORBCOMM_DEFS_H */

/******************************************************************************/
