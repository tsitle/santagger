/******************************************************************************/
/* cont_wav_stcs-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for WAV container structs                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.07.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_CONT_WAV_STCS_PROT_H
#define LIBSANTAGGER_CONT_WAV_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "cont_wav-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_WAV_STCS_C) || defined(SRC_CONT_WAV_WR_C) || \
			defined(SRC_CONT_WAV_ZFNC_C)
	Tst_err st_contWav_stc_initElem(Tst_contWav_handle *pElem,
	                             const Tst_str *pFilen);
	void    st_contWav_stc_rsetElem(Tst_contWav_handle *pElem);
	void    st_contWav_stc_freeElem(Tst_contWav_handle *pElem);
	/** */
	void   st_contWav_stc_initOpts(Tst_contWav_opts *pWOpts);
	void   st_contWav_stc_rsetOpts(Tst_contWav_opts *pWOpts);
#	define st_contWav_stc_freeOpts  st_contWav_stc_rsetOpts
	/** */
	void    st_contWav_stc_initDBuf(Tst_contWav_destBuffer *pDBuf);
	void    st_contWav_stc_rsetDBuf(Tst_contWav_destBuffer *pDBuf);
#	define  st_contWav_stc_freeDBuf  st_contWav_stc_rsetDBuf
	Tst_err st_contWav_stc_reszDBuf_dstBuf(Tst_contWav_destBuffer *pDBuf,
	                                       const Tst_uint32 newSz);
	Tst_err st_contWav_stc_reszDBuf_srcBufSh(Tst_contWav_destBuffer *pDBuf,
	                                         const Tst_uint32 newSz);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_CONT_WAV_STCS_PROT_H */

/******************************************************************************/
