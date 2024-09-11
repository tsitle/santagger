/******************************************************************************/
/* mf_read_dbuf-prot.h            [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions etc. for audio decoding buffers                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 04.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MF_READ_DBUF_PROT_H
#define HEAD_MF_READ_DBUF_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Types (protected)
*/
#if defined(SRC_MF_READ_DBUF_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C)
#	if (HAVE_LIBMPG123 == 1) || \
				(HAVE_LIBMAD == 1) || \
				(HAVE_LIBVORBIS == 1)
	/** for decoded data */
	typedef struct {
		Tst_uint32 sampleR;        /* samplerate in Hz */
		Tst_uint32 channels;       /* amount of channels */
		Tst_uint32 bps;            /* bits per sample */
		Tst_bool   isOutpFmtSet;   /* has output fmt been set ? */
		/* */
		Tst_uint32 sampleCnt;      /* amount of samples per channel */
		Tst_uint32 sampleCntRes;   /* amount of s. p. ch. reserved for ppBufOutI32 */
		Tst_int32  **ppBufOutI32;  /* output buffer suitable for cont_wav */
		/* */
		Tst_buf    *pBufTmp;       /* buffer for temporary output */
		Tst_uint32 bufTmpSz;       /* size of pBufTmp */
	} Tast_mf_rddec_dbuf;
#	endif  /* libmpg123 or libmad or libvorbis */
#endif  /* protected */


/*
// Functions (protected)
*/
#if defined(SRC_MF_READ_DBUF_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C)
#	if (HAVE_LIBMPG123 == 1) || \
				(HAVE_LIBMAD == 1) || \
				(HAVE_LIBVORBIS == 1)
	void    ast_mf_rddec_stc_initDB(Tast_mf_rddec_dbuf *pDBuf);
	void    ast_mf_rddec_stc_rsetDB(Tast_mf_rddec_dbuf *pDBuf);
#	define  ast_mf_rddec_stc_freeDB(pDBuf)  ast_mf_rddec_stc_rsetDB(pDBuf)
	Tst_err ast_mf_rddec_stc_reszDecBufOutI32(Tast_mf_rddec_dbuf *pDBuf,
	                                          const Tst_uint32 newSampleCnt);
#	endif  /* libmpg123 or libmad or libvorbis */
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_MF_READ_DBUF_PROT_H */

/******************************************************************************/
