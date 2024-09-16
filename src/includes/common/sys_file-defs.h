/******************************************************************************/
/* sys_file-defs.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
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
#include "../lib_defs.h"


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

/** for setting file/directory permissions */
/*** these can be combined by OR'ing them (e.g. mode=(OTHX | OTHR | GRPX | GRPR)) */
typedef enum {
	ST_SYS_SETPERM_OTHX = 1,     /* execute/search by others */
	ST_SYS_SETPERM_OTHW = 2,     /* write by others */
	ST_SYS_SETPERM_OTHR = 4,     /* read by others */
	ST_SYS_SETPERM_GRPX = 8,     /* execute/search by group */
	ST_SYS_SETPERM_GRPW = 16,    /* write by group */
	ST_SYS_SETPERM_GRPR = 32,    /* read by group */
	ST_SYS_SETPERM_USRX = 64,    /* execute/search by owner */
	ST_SYS_SETPERM_USRW = 128,   /* write by owner */
	ST_SYS_SETPERM_USRR = 256,   /* read by owner */
	ST_SYS_SETPERM_SVTX = 512,   /* sticky bit */
	ST_SYS_SETPERM_SGID = 1024,  /* set-group-ID */
	ST_SYS_SETPERM_SUID = 2048,  /* set-user-ID */
	ST_SYS_SETPERM_NONE = 0
} Tst_sys_setFilePerm;
/*** default file/directory permissions (OTHX+OTHR+GRPX+GRPR+USRX+USRW+USRR) */
#define ST_SYS_FILEPERM_DEF  (ST_SYS_SETPERM_OTHX|ST_SYS_SETPERM_OTHR|ST_SYS_SETPERM_GRPX|ST_SYS_SETPERM_GRPR|ST_SYS_SETPERM_USRX|ST_SYS_SETPERM_USRW|ST_SYS_SETPERM_USRR)


/*
// Types
*/
/** file handle */
typedef struct {
	void *pObInternal;  /* pointer to Tst_sys__fstc_intn */
} Tst_sys_fstc;


ST_END_C_DECLS

#endif  /* HEAD_SYS_FILE_DEFS_H */

/******************************************************************************/
