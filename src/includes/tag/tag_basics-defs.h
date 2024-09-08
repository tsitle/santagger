/******************************************************************************/
/* tag_basics-defs.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for Tags                                                       */
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

#ifndef HEAD_TAG_BASICS_DEFS_H
#define HEAD_TAG_BASICS_DEFS_H

/*
// Own-Includes
*/
#include <santag/lib_defs.h>


ST_BEGIN_C_DECLS

/*
// Types
*/
/** basic Tag info */
typedef struct {
	void *pObInternal;  /* pointer to Tst_tagBasics__intn */
} Tst_tagBasics;


ST_END_C_DECLS

#endif  /* HEAD_TAG_BASICS_DEFS_H */

/******************************************************************************/
