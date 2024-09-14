/******************************************************************************/
/* mf_read_decmpg1-pp.h           [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions etc. for decoding MPEG-1 audio                 */
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

#ifndef HEAD_MF_READ_DECMPG1_PP_H
#define HEAD_MF_READ_DECMPG1_PP_H

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
#if (HAVE_LIBMPG123 == 1)
#	include <mpg123.h>
#	if (MPG123_API_VERSION <= 25)
		/* defining the following is necessary because of
		 *   a bug in libmpg123, at least in version 1.12.1  */
#		define MPG123_NO_CONFIGURE
#	endif
#elif (HAVE_LIBMAD == 1)
#	include <mad.h>
#endif


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECMPG1_C)
#	if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
	/** */
	Tst_err
	ast_mf_rddec_initExtLibMpg(void);

	void
	ast_mf_rddec_freeExtLibMpg(void);
	/** */
	Tst_err
	ast_mf_rddec_cbDecMpg_hndNew(void **ppHnd, const Tst_bool printUsedBPS);

	void
	ast_mf_rddec_cbDecMpg_hndDel(void *pHnd);

	Tst_err
	ast_mf_rddec_cbDecMpg_setOutputFmt(void *pHnd,
	                                   const Tst_uint32 sampleRate,
	                                   const Tst_uint32 channels,
	                                   const Tst_uint32 desiredBitsPerSample,
	                                   Tst_uint32 *pUsedBitsPerSample);

	Tst_err
	ast_mf_rddec_cbDecMpg_openFeed(void *pHnd);

	Tst_err
	ast_mf_rddec_cbDecMpg_feedInp(void *pHnd,
	                              Tst_buf *pInpBuf,
	                              const Tst_uint32 inpBufSz);

	Tst_err
	ast_mf_rddec_cbDecMpg_decode(void *pHnd,
	                             const Tst_bool isLastCall,
	                             Tst_bool *pNeedMore,
	                             Tst_av_cbPCM_samplesClient
	                               cbPCMclient,
	                             void *pPCMclientHandle);

	Tst_err
	ast_mf_rddec_cbDecMpg_closeFeed(void *pHnd);
#	endif  /* libmpg123 or libmad */
#endif  /* protected */


/*
// Types (private)
*/
#ifdef SRC_MF_READ_DECMPG1_C
#	if (HAVE_LIBMPG123 != 1) && (HAVE_LIBMAD == 1)
	/** for input data */
	typedef struct {
		Tst_uint32 inpBufUsed;
		Tst_uint32 inpBufSz;
		Tst_buf    *pInpBuf;
		Tst_uint32 procInp;     /* processed bytes from input since last feed */
		Tst_fsize  procInpTot;  /* total processed input bytes */
		Tst_bool   wasDecoded;  /* were frames decoded since last feed ? */
	} Tast_mf__rddec_ibuf;
#	endif  /* !libmpg123 && libmad */

#	if (HAVE_LIBMPG123 == 1)
	/** for libmpg123 */
	/*** */
	typedef struct {
		Tst_bool             printUsedBPS;
		/* */
		mpg123_handle        *pM123hnd;
		/* */
		enum mpg123_enc_enum encoding;
		/* output buffer */
		Tast_mf_rddec_dbuf   dbuf;
	} Tast_mf__rddec_mpg123;

#	elif (HAVE_LIBMAD == 1)
	/** for libmad */
	/*** */
	typedef struct {
		Tst_fsize   clipped_samples;
		mad_fixed_t peak_clipping;
		mad_fixed_t peak_sample;
	} Tast_mf__rddec_madAudioStats;
	/*** */
	typedef struct {
		Tst_bool printUsedBPS;
		/* */
		struct mad_stream            mStream;
		struct mad_frame             mFrame;
		struct mad_synth             mSynth;
		Tast_mf__rddec_madAudioStats aStats;
		/* */
		Tst_buf             guardBuf[MAD_BUFFER_GUARD];
		/* input buffer */
		Tast_mf__rddec_ibuf ibuf;
		/* output buffer */
		Tast_mf_rddec_dbuf  dbuf;
	} Tast_mf__rddec_mad;
#	endif  /* libmpg123 */
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_MF_READ_DECMPG1_C
	/** */
#	if (HAVE_LIBMPG123 == 1)
	/*** */
	static Tst_err
	AST_MF__rddec_cbDecMpg_decodeOnly_mpg123(void *pHnd,
	                                         const Tst_bool isLastCall,
	                                         Tst_bool *pNeedMore,
	                                         const Tst_bool passSamplesToClient);
	/*** */
	static Tst_err
	AST_MF__rddec_getMpg123NewFmt(const char *pFnc, Tast_mf__rddec_mpg123 *pM123);
	/*** */
	static Tst_err
	AST_MF__rddec_convMpg123_rawSamples_inp16out16(Tast_mf__rddec_mpg123 *pM123);

	static Tst_err
	AST_MF__rddec_convMpg123_rawSamples_inp16shift(Tast_mf__rddec_mpg123 *pM123);

	static Tst_err
	AST_MF__rddec_convMpg123_rawSamples_inp32out32(Tast_mf__rddec_mpg123 *pM123);

	static Tst_err
	AST_MF__rddec_convMpg123_rawSamples_inp32shift(Tast_mf__rddec_mpg123 *pM123);
#	elif (HAVE_LIBMAD == 1)
	/*** */
	static Tst_err
	AST_MF__rddec_cbDecMpg_decodeOnly_mad(void *pHnd,
	                                      const Tst_bool isLastCall,
	                                      Tst_bool *pNeedMore,
	                                      const Tst_bool passSamplesToClient);
	/*** */
	static Tst_err
	AST_MF__rddec_removeProcInp(Tast_mf__rddec_ibuf *pIBuf);
	/*** */
	static Tst_err
	AST_MF__rddec_convMad_rawSamples_inp24noshift(Tast_mf__rddec_mad *pMad);

	static Tst_err
	AST_MF__rddec_convMad_rawSamples_inp24shift(Tast_mf__rddec_mad *pMad);
#	endif  /* libmpg123 */

	/** */
#	if (HAVE_LIBMPG123 == 1)
	static Tst_err
	AST_MF__rddec_stc_initM123(Tast_mf__rddec_mpg123 *pM123);

	static void
	AST_MF__rddec_stc_freeM123(Tast_mf__rddec_mpg123 *pM123);
#	elif (HAVE_LIBMAD == 1)
	static Tst_err
	AST_MF__rddec_stc_initMad(Tast_mf__rddec_mad *pMad);

	static void
	AST_MF__rddec_stc_freeMad(Tast_mf__rddec_mad *pMad);
#	endif  /* libmpg123 */

	/** */
#	if (HAVE_LIBMPG123 != 1) && (HAVE_LIBMAD == 1)
	static void
	AST_MF__rddec_stc_initIB(Tast_mf__rddec_ibuf *pIBuf);

	static void
	AST_MF__rddec_stc_rsetIB(Tast_mf__rddec_ibuf *pIBuf);
#	define AST_MF__rddec_stc_freeIB(pIBuf)  AST_MF__rddec_stc_rsetIB(pIBuf)
#	endif  /* !libmpg123 && libmad */

	/** */
#	if (HAVE_LIBMPG123 == 1)
	static Tst_err
	AST_MF__rddec_stc_reszDecBufTmp(Tast_mf_rddec_dbuf *pDBuf,
	                                const Tst_uint32 newSz);
#	endif  /* libmpg123 */
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_MF_READ_DECMPG1_PP_H */

/******************************************************************************/
