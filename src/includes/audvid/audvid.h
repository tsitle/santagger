/******************************************************************************/
/* audvid.h                     [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Common functions for audio/video                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.07.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AUDVID_H
#define HEAD_AUDVID_H

/*
// Own-Includes
*/
#include "audvid-defs.h"


ST_BEGIN_C_DECLS

/*
// Macros
*/
/** Tst_av_playtime */
#define ST_AV_STC_RSETPT(mac_pt)  { \
				(mac_pt).ptHours = 0; \
				(mac_pt).ptMins  = 0; \
				(mac_pt).ptSecs  = 0; \
			}


ST_END_C_DECLS

#endif  /* HEAD_AUDVID_H */

/******************************************************************************/
