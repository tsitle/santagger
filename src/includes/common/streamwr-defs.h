/******************************************************************************/
/* streamwr-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for the Stream Writer                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_STREAMWR_DEFS_H
#define HEAD_STREAMWR_DEFS_H

/*
// Own-Includes
*/
#include <santag/lib_defs.h>


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** output endianess */
typedef enum {
	ST_STREAMWR_OEND_LE,  /* little-endian
	                       * -- ! works only for writing n bits where
	                       *      n must be dividable by 8 without remainder */
	ST_STREAMWR_OEND_BE,  /* big-endian */
	ST_STREAMWR_OEND_IGN  /* ignore */
} Tst_streamwr_endian;


/*
// Types
*/
/** */
typedef struct {
	void *pObInternal;  /* pointer to Tst_streamwr_intn */
} Tst_streamwr;


ST_END_C_DECLS

#endif  /* HEAD_STREAMWR_DEFS_H */

/******************************************************************************/
