/******************************************************************************/
/* mf_read_decvrb-pp.h            [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions etc. for decoding Ogg-Vorbis audio             */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MF_READ_DECVRB_PP_H
#define HEAD_MF_READ_DECVRB_PP_H

/*
// Own-Includes
*/
/** */
#include "src/includes/audvid/audvid-defs.h"
/** */
#include "mf-prot.h"
#include "mf_read_dbuf-prot.h"

/*
// System-Includes
*/
#if (HAVE_LIBVORBIS == 1)
#	if (CONFIG_ST_ALL_HAVE64BIT != 1)
#		error libvorbis support requires int64 support
#	else
#		include <vorbis/codec.h>
#		include <ogg/ogg.h>
#	endif
#endif


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECVRB_C)
#	if (HAVE_LIBVORBIS == 1)
	/** */
	Tst_err
	ast_mf_rddec_cbDecVorb_hndNew(void **ppHnd, const Tst_bool printUsedBPS);

	void
	ast_mf_rddec_cbDecVorb_hndDel(void *pHnd);

	Tst_err
	ast_mf_rddec_cbDecVorb_setOutputFmt(void *pHnd,
	                                    const Tst_uint32 sampleRate,
	                                    const Tst_uint32 channels,
	                                    const Tst_uint32 desiredBitsPerSample,
	                                    Tst_uint32 *pUsedBitsPerSample);

	Tst_err
	ast_mf_rddec_cbDecVorb_openFeed(void *pHnd);

	Tst_err
	ast_mf_rddec_cbDecVorb_feedHeader(void *pHnd,
	                                  Tst_buf *pInpBuf,
	                                  const Tst_uint32 inpBufSz,
	                                  const Tst_bool hdTypeBOS,
	                                  const Tst_uint64 granulePos);

	Tst_err
	ast_mf_rddec_cbDecVorb_feedAudioAndDecode(void *pHnd,
	                                          Tst_buf *pInpBuf,
	                                          const Tst_uint32 inpBufSz,
	                                          const Tst_uint64 granulePos,
	                                          Tst_av_cbPCM_samplesClient
	                                            cbPCMclient,
	                                          void *pPCMclientHandle);

	Tst_err
	ast_mf_rddec_cbDecVorb_closeFeed(void *pHnd);
#	endif  /* libvorbis */
#endif  /* protected */


/*
// Types (private)
*/
#ifdef SRC_MF_READ_DECVRB_C
#	if (HAVE_LIBVORBIS == 1)
	/** for libvorbis */
	typedef struct {
		Tst_bool           printUsedBPS;
		/* */
		vorbis_info        vrbInfo;
		vorbis_comment     vrbCmt;
		vorbis_dsp_state   *pVrbDSP;
		vorbis_block       *pVrbBlock;
		ogg_packet         oggPckt;
		/* */
		float              **ppPcmIn;  /* output samples */
		/* output buffer */
		Tast_mf_rddec_dbuf dbuf;
	} Tast_mf__rddec_vorb;
#	endif  /* libvorbis */
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_MF_READ_DECVRB_C
#	if (HAVE_LIBVORBIS == 1)
	/** */
	static Tst_err
	AST_MF__rddec_convVorb_rawSamples(Tast_mf__rddec_vorb *pVrb);
	/** */
	static void
	AST_MF__rddec_printLibErr(const char *pFnc, const int libErr);
	/** */
	static Tst_err
	AST_MF__rddec_stc_initVrb(Tast_mf__rddec_vorb *pVrb);

	static void
	AST_MF__rddec_stc_freeVrb(Tast_mf__rddec_vorb *pVrb);
#	endif  /* libvorbis */
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_READ_DECVRB_PP_H */

/******************************************************************************/
