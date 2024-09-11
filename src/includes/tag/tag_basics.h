/******************************************************************************/
/* tag_basics.h                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Tag basics                                                   */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_BASICS_H
#define HEAD_TAG_BASICS_H

/*
// Own-Includes
*/
#include "src/includes/tag/tag_basics-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions
*/
/** Structs */
ST_EXPORT Tst_err
st_tagBas_stc_initTB(Tst_tagBasics *pTB);

ST_EXPORT void
st_tagBas_stc_rsetTB(Tst_tagBasics *pTB);

ST_EXPORT void
st_tagBas_stc_freeTB(Tst_tagBasics *pTB);

ST_EXPORT Tst_err
st_tagBas_stc_copyTB(const Tst_tagBasics *pTBsrc,
                     Tst_tagBasics *pTBdst);

/** Getter */
ST_EXPORT Tst_bool
st_tagBas_gs_getHasTag(const Tst_tagBasics *pTB);

ST_EXPORT Tst_bool
st_tagBas_gs_getHasFields(const Tst_tagBasics *pTB);

ST_EXPORT Tst_foffs
st_tagBas_gs_getOffset(const Tst_tagBasics *pTB);

ST_EXPORT Tst_uint32
st_tagBas_gs_getSize(const Tst_tagBasics *pTB);


ST_END_C_DECLS

#endif  /* HEAD_TAG_BASICS_H */

/******************************************************************************/
