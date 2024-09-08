/******************************************************************************/
/* sys_file-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Definitions for system functions related to files                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_SYS_FILE_DEFS_H
#define HEAD_SYS_FILE_DEFS_H

/*
// Own-Includes
*/
#include <santag/lib_defs.h>


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** for seeking in files
 **   use offset relative to... */
typedef enum {
	ST_SYS_SETFPOS_BEG = 0,  /* ... beginning of file */
	ST_SYS_SETFPOS_CUR = 1,  /* ... current position */
	ST_SYS_SETFPOS_END = 2   /* ... end of file */
} Tst_sys_setFilePos;


/*
// Types
*/
/** file handle */
typedef struct {
	void *pObInternal;  /* pointer to Tst_sys_fstc_intn */
} Tst_sys_fstc;


ST_END_C_DECLS

#endif  /* HEAD_SYS_FILE_DEFS_H */

/******************************************************************************/
