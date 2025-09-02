/******************************************************************************/
/* cont_wav-prot.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitons for WAV containers                                    */
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

#ifndef LIBSANTAGGER_CONT_WAV_PROT_H
#define LIBSANTAGGER_CONT_WAV_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/sys_file-defs.h"
#include "src/includes/common/sys_math-defs.h"
#include "src/includes/common/dynlist-defs.h"
#include "src/includes/audvid/cont_wav-defs.h"


ST_BEGIN_C_DECLS

/*
// Types (protected)
*/
#if defined(SRC_CONT_WAV_WR_C) || defined(SRC_CONT_WAV_GS_C) || \
			defined(SRC_CONT_WAV_OPTS_C) || defined(SRC_CONT_WAV_STCS_C) || \
			defined(SRC_CONT_WAV_ZFDEB_C) || defined(SRC_CONT_WAV_ZFNC_C)
	/** */
	typedef struct {
		Tst_basOpts basOpts;  /* basic options */
		/* */
		Tst_bool    isSystemBE;       /* is system big- or little-endian ? */
		Tst_bool    wrFmtRIFForAIFF;  /* write RIFF or AIFF format ? */
		/* */
		Tst_str     *pOutpdir;  /* output dir to write new files to */
		Tst_bool    owExFiles;  /* overwrite existing output files ? */
		/* */
		Tst_contWav_cbNewOutputStream cbNewOutpStr;  /* to notify about new stream */
	} Tst_contWav_opts;

	/** */
	typedef struct {
		Tst_bool   hdWritten;         /* has header already been written ? */
		/* */
		Tst_uint64 totSampleCnt;      /* total amount of samples */
		Tst_uint32 sampleRate;        /* sample rate */
		Tst_uint32 bitsPerSample;     /* bits per sample */
		Tst_uint32 channMask;         /* channel mask for ext. wave fmt */
		Tst_uint32 channels;          /* amount of channels */
		Tst_bool   isExtensibleWFmt;  /* is extensible wave format ? */
	} Tst_contWav_headerInfo;

	/** */
	typedef struct {
		/* these are alloc'd */
		Tst_int8   *pDstBuf;
		Tst_int32  **ppSrcBufSh;
		/* */
		Tst_uint32 dstBufSz;
		Tst_uint32 srcBufShSz;
		/* these are only casts to pDstBuf */
		Tst_uint8  *pU8buf;
		Tst_int16  *pS16buf;
		Tst_uint16 *pU16buf;
		Tst_int32  *pS32buf;
		Tst_uint32 *pU32buf;
		/* amount of output bytes */
		Tst_fsize  bytesToWrite;
	} Tst_contWav_destBuffer;

	/** */
	typedef struct {
		Tst_str      *pFilen;  /* filename used */
		Tst_sys_fstc fstcOut;  /* output file handle */
		/* */
		Tst_uint32 bsIx;   /* bitstream index */
		Tst_uint32 bsSIx;  /* bitstream sub index */
		/* */
		Tst_contWav_headerInfo hdInfo;  /* header info */
		/* */
		Tst_contWav_destBuffer dbuf;
		/* */
		Tst_uint32 uniqueIx;  /* unique elem index */
		/* */
		Tst_contWav_opts *pOpts;  /* pointer to options */
	} Tst_contWav_handle_intn;

	/** */
	typedef struct {
		Tst_dynlist        list;          /* all elements */
		Tst_uint32         uniqueIxLast;  /* last used unique elem index */
		Tst_contWav_handle *pCacheElem;   /* cached element */
		Tst_uint32         cacheBsIx;     /* cached element's bitstream index */
		Tst_uint32         cacheBsSIx;    /* cached element's bitstream sub index */
		/* */
		Tst_contWav_opts opts;  /* options */
	} Tst_contWav_obj_intn;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_CONT_WAV_PROT_H */

/******************************************************************************/
