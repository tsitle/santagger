/******************************************************************************/
/* app_defs.h                     [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Application wide definitions                                               */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_APP_DEFS_H
#define HEAD_APP_DEFS_H

/*
// Own-Includes
*/
#if HAVE_APP_VERSION_H
#	include <app_version.h>
#	if !(defined(APP_ST_VERS_MAJ) && defined(APP_ST_VERS_MIN) && \
				defined(APP_ST_VERS_STRING) && \
				defined(APP_ST_VERS_STRING_ADD) && \
				defined(APP_ST_VERS_STRING_COMPLETE))
#		error No application version defined
#	endif
#endif
#if (APP_ST_VERS_MAJ == 0 && APP_ST_VERS_MIN == 0)
	/* to detect dummy values */
#	error No application version defined
#endif


ST_BEGIN_C_DECLS

/*
// Constants (public)
*/
/** */
#define APP_ST_NAME    "Sandy Tagger (santagger)"
/*#define APP_ST_VERS_MAJ              X
  #define APP_ST_VERS_MIN              Y
  #define APP_ST_VERS_STRING           "X.Y"
  #define APP_ST_VERS_STRING_ADD       "dummyZ"
  #define APP_ST_VERS_STRING_COMPLETE  "X.Y-dummyZ" */
#define APP_ST_AUTHOR  "Thomas Sandkuehler <santagger@slur97.org>"
#define APP_ST_COPYR   "2005-2011 by " APP_ST_AUTHOR


ST_END_C_DECLS

#endif  /* HEAD_APP_DEFS_H */

/******************************************************************************/
