/******************************************************************************/
/* binobj-defs.h                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for Binary Objects                                             */
/*                                                                            */
/* Binary Objects can contain arbitrary data,                                 */
/*   the trick is that the data internally gets either stored                 */
/*   in a buffer or a temporary file, depending of the data's size            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_BINOBJ_DEFS_H
#define LIBSANTAGGER_BINOBJ_DEFS_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
ST_EXPORT extern const Tst_uint32 ST_BINOBJ_DEF_THRESHOLD;


/*
// Types
*/
/** */
typedef struct {
	void *pObInternal;  /* pointer to Tst_binobj_intn */
} Tst_binobj;


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_BINOBJ_DEFS_H */

/******************************************************************************/
