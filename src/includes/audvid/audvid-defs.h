/******************************************************************************/
/* audvid-defs.h                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Common definitions for audio/video                                         */
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

#ifndef HEAD_AUDVID_DEFS_H
#define HEAD_AUDVID_DEFS_H

/*
// Own-Includes
*/
#include "src/includes/common/sys_math-defs.h"


ST_BEGIN_C_DECLS

/*
// Types
*/
/** Playtime */
typedef struct {
	Tst_byte ptHours;  /* hours */
	Tst_byte ptMins;   /* minutes */
	Tst_byte ptSecs;   /* seconds */
} Tst_av_playtime;

/** Callbacks for handling raw PCM samples */
typedef Tst_err
        (*Tst_av_cbPCM_prepareWriting)(void *pClientObj,
                                       const Tst_str *pInputFilen,
                                       const Tst_uint32 bsIx,
                                       const Tst_uint32 bsSIx,
                                       const Tst_bool appendBsIx,
                                       void **ppHandle);
typedef Tst_err
        (*Tst_av_cbPCM_setOutputFmt)(void *pHandle,
                                     const Tst_uint32 sampleRate,
                                     const Tst_uint32 bitsPerSample,
                                     const Tst_uint32 channMask,
                                     const Tst_uint32 channels);
typedef Tst_err
        (*Tst_av_cbPCM_samplesClient)(void *pHandle,
                                      const Tst_uint32 sampleCnt,
                                      const Tst_int32 **ppRawSamples);
typedef Tst_err
        (*Tst_av_cbPCM_finishWriting)(void *pHandle);

/** Callbacks for decoding audio */
/*** init/free handles */
typedef Tst_err
        (*Tst_av_cbDecAud_hndNew)(void **ppHnd, const Tst_bool printUsedBPS);

typedef void
        (*Tst_av_cbDecAud_hndDel)(void *pHnd);

/*** set output format */
typedef Tst_err
        (*Tst_av_cbDecAud_setOutputFmt)(void *pHnd,
                                        const Tst_uint32 sampleRate,
                                        const Tst_uint32 channels,
                                        const Tst_uint32 desiredBitsPerSample,
                                        Tst_uint32 *pUsedBitsPerSample);

/*** open/close feed, feed input and decode samples */
typedef Tst_err
        (*Tst_av_cbDecAud_openFeed)(void *pHnd);

typedef Tst_err
        (*Tst_av_cbDecAud_feedMpgAudio)(void *pHnd,
                                        Tst_buf *pInpBuf,
                                        const Tst_uint32 inpBufSz);

typedef Tst_err
        (*Tst_av_cbDecAud_feedVrbHeader)(void *pHnd,
                                         Tst_buf *pInpBuf,
                                         const Tst_uint32 inpBufSz,
                                         const Tst_bool hdTypeBOS,
                                         const Tst_uint64 granulePos);

typedef Tst_err
        (*Tst_av_cbDecAud_feedVrbAudioAndDecode)(void *pHnd,
                                                 Tst_buf *pInpBuf,
                                                 const Tst_uint32 inpBufSz,
                                                 const Tst_uint64 granulePos,
                                                 Tst_av_cbPCM_samplesClient
                                                   cbPCMclient,
                                                 void *pPCMclientHandle);

typedef Tst_err
        (*Tst_av_cbDecAud_decodeMpg)(void *pHnd,
                                     const Tst_bool isLastCall,
                                     Tst_bool *pNeedMore,
                                     Tst_av_cbPCM_samplesClient
                                       cbPCMclient,
                                     void *pPCMclientHandle);

typedef Tst_err
        (*Tst_av_cbDecAud_closeFeed)(void *pHnd);


ST_END_C_DECLS

#endif  /* HEAD_AUDVID_DEFS_H */

/******************************************************************************/
