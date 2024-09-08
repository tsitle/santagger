/******************************************************************************/
/* streamrd-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for the Stream Reader                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_STREAMRD_DEFS_H
#define HEAD_STREAMRD_DEFS_H

/*
// Own-Includes
*/
#include <santag/lib_defs.h>


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** input endianess */
typedef enum {
	ST_STREAMRD_IEND_LE,  /* little-endian
	                       * -- ! works only for reading n bits where
	                       *      n must be dividable by 8 without remainder */
	ST_STREAMRD_IEND_BE,  /* big-endian */
	ST_STREAMRD_IEND_IGN  /* ignore */
} Tst_streamrd_endian;


/*
// Types
*/
/** */
typedef struct {
	void *pObInternal;  /* pointer to Tst_streamrd_intn */
} Tst_streamrd;


ST_END_C_DECLS

#endif  /* HEAD_STREAMRD_DEFS_H */

/******************************************************************************/
