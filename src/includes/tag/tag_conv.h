/******************************************************************************/
/* tag_conv.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for converting Tags                                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.02.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_CONV_H
#define HEAD_TAG_CONV_H

/*
// Own-Includes
*/
#include <santag/tag/tag_conv-defs.h>


ST_BEGIN_C_DECLS

/*
// Functions
*/
/** src: x, dst: y */
ST_EXPORT Tst_err
st_tagConv_XtoY(const Tst_byte iv2TargVers,
                const Tst_tagConv_tagTp srcTTP,
                void *pTagSrc,
                const Tst_tagConv_tagTp cvTTP,
                void *pTagCv);


ST_END_C_DECLS

#endif  /* HEAD_TAG_CONV_H */

/******************************************************************************/
