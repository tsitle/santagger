/******************************************************************************/
/* stream_stcs-prot.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for Stream Reader / Writer structures                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_STREAM_STCS_PROT_H
#define HEAD_STREAM_STCS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "stream-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions  (protected)
*/
#if defined(SRC_STREAMRD_C) || defined(SRC_STREAMWR_C) || \
			defined(SRC_STREAM_STCS_C)
	/** */
	void    st_stream_stc_initBuf(Tst_stream_buf *pSBuf);
	void    st_stream_stc_rsetBuf(Tst_stream_buf *pSBuf,
	                              const Tst_bool freeBufs);
#	define  st_stream_stc_freeBuf(pSBuf)  \
				st_stream_stc_rsetBuf(pSBuf, ST_B_TRUE)
	Tst_err st_stream_stc_reszIntnBuf(Tst_stream_buf *pSBuf,
	                                  const Tst_uint32 newSz,
	                                  const Tst_uint32 restor);

	/** */
	void st_stream_stc_rsetFil(Tst_stream_file *pSFil);
	/** */
	void st_stream_stc_rsetCRCs(Tst_stream_crcs *pCRCs);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_STREAM_STCS_PROT_H */

/******************************************************************************/
