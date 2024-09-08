/******************************************************************************/
/* cont_ogg_vorbcomm.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for Vorbis comments                                              */
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

#ifndef HEAD_CONT_OGG_VORBCOMM_H
#define HEAD_CONT_OGG_VORBCOMM_H

/*
// Own-Includes
*/
#include <santag/common/binobj-defs.h>
#include <santag/audvid/cont_ogg-defs.h>


ST_BEGIN_C_DECLS

/*
// Functions -- mc_vorbcomm_gs.c
*/
/** Setter */
/*** */
ST_EXPORT Tst_err
st_contOgg_vorbComm_gs_setRawCommentData(Tst_contOgg_substream *pBS,
                                         Tst_binobj *pBinDat);

/** Getter */
/*** */
ST_EXPORT const Tst_contOgg_vorbCommentRaw*
st_contOgg_vorbComm_gs_getRawComment(const Tst_contOgg_substream *pBS);

/*** */
ST_EXPORT Tst_bool
st_contOgg_vorbComm_gs_getRC_isSet(const Tst_contOgg_vorbCommentRaw *pVCR);

/*** */
ST_EXPORT Tst_uint32
st_contOgg_vorbComm_gs_getRC_elementCount(const Tst_contOgg_vorbCommentRaw *pVCR);

/*** */
ST_EXPORT Tst_err
st_contOgg_vorbComm_gs_getRC_vendor(const Tst_contOgg_vorbCommentRaw *pVCR,
                                    Tst_str **ppVendor);

/*** */
ST_EXPORT Tst_err
st_contOgg_vorbComm_gs_getRC_rawData(const Tst_contOgg_vorbCommentRaw *pVCR,
                                     Tst_binobj *pBinDat);
ST_EXPORT Tst_binobj*
st_contOgg_vorbComm_gs_getRC_rawDataPointer(const Tst_contOgg_vorbCommentRaw *pVCR);


/*
// Functions -- mc_vorbcomm_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_contOgg_stc_initVorbCommRaw(Tst_contOgg_vorbCommentRaw *pVCR);

ST_EXPORT void
st_contOgg_stc_rsetVorbCommRaw(Tst_contOgg_vorbCommentRaw *pVCR);

ST_EXPORT void
st_contOgg_stc_freeVorbCommRaw(Tst_contOgg_vorbCommentRaw *pVCR);


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_VORBCOMM_H */

/******************************************************************************/
