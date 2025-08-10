/******************************************************************************/
/* app_defs.h                     [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
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
#include "app_version.h"
#if !defined(APP_ST_VERS_STRING)
	#error No application version defined
#endif

/*
// Constants (public)
*/
/** */
#define APP_ST_NAME    "Sandy Tagger (santagger)"
#define APP_ST_AUTHOR  "Thomas Sandkuehler <technisandk@gmail.com>"
#define APP_ST_COPYR   ("2005-2025 by " APP_ST_AUTHOR)

#endif  /* HEAD_APP_DEFS_H */

/******************************************************************************/
