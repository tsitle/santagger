/******************************************************************************/
/* cont_wav_zfnc-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for WAV containers                                 */
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

#ifndef LIBSANTAGGER_CONT_WAV_ZFNC_PROT_H
#define LIBSANTAGGER_CONT_WAV_ZFNC_PROT_H

/*
// Own-Includes
*/
/** */
#include "cont_wav-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_WAV_ZFNC_C) || defined(SRC_CONT_WAV_GS_C) || \
			defined(SRC_CONT_WAV_WR_C)
	/** */
	Tst_err
	st_contwav_fnc_addNewElem(Tst_contWav_obj_intn *pWObjI,
	                          const Tst_uint32 bsIx,
	                          const Tst_uint32 bsSIx,
	                          Tst_contWav_handle **ppElem);
	Tst_contWav_handle*
	st_contwav_fnc_getElem(Tst_contWav_obj_intn *pWObjI,
	                       const Tst_uint32 bsIx,
	                       const Tst_uint32 bsSIx);
	/** */
	Tst_err
	st_contwav_fnc_createOutpFn(const Tst_str *pOrgFilen,
	                            const Tst_uint32 bsIx,
	                            const Tst_uint32 bsSIx,
	                            const Tst_bool appendBsIx,
	                            const Tst_bool wrRIFForAIFF,
	                            ST_OPTARG(const Tst_str *pOutpdir),
	                            const Tst_bool owExFiles,
	                            Tst_str **ppNewFilen);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_CONT_WAV_ZFNC_PROT_H */

/******************************************************************************/
