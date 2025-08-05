/******************************************************************************/
/* cont_wav_zfdeb-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for WAV debugging                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 09.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CONT_WAV_ZFDEB_PROT_H
#define HEAD_CONT_WAV_ZFDEB_PROT_H

/*
// Own-Includes
*/
/** */
#include "cont_wav-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_WAV_ZFDEB_C) || defined(SRC_CONT_WAV_WR_C)
	/** debug/error */
	void st_contWav_d_deb(const Tst_contWav_opts *pOpts,
	                      const char *pFnc,
	                      const char *pFmt, ...);
	void st_contWav_d_err(const Tst_contWav_opts *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const char *pFmt, ...);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_CONT_WAV_ZFDEB_PROT_H */

/******************************************************************************/
