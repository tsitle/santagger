/******************************************************************************/
/* cont_wav.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for WAV containers                                               */
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

#ifndef HEAD_CONT_WAV_H
#define HEAD_CONT_WAV_H

/*
// Own-Includes
*/
#include "src/includes/common/sys_math-defs.h"
#include "src/includes/audvid/cont_wav-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- cont_wav_gs.c
*/
/** Getter */
ST_EXPORT const Tst_str*
st_contWav_gs_getHandle_filen(const Tst_contWav_handle *pHandle);

ST_EXPORT Tst_bool
st_contWav_gs_getHandle_index(const Tst_contWav_handle *pHandle,
                              Tst_uint32 *pBsIx,
                              Tst_uint32 *pBsSIx);
ST_EXPORT Tst_bool
st_contWav_gs_getHandle_isOpen(const Tst_contWav_handle *pHandle);

ST_EXPORT Tst_bool
st_contWav_gs_getHandle_writtenSamples(const Tst_contWav_handle *pHandle,
                                       Tst_uint64 *pSampleCnt);

/** Iterator */
ST_EXPORT Tst_contWav_handle*
st_contWav_ite_getNextHandle(Tst_contWav_obj *pWObj,
                             ST_OPTARG(const Tst_contWav_handle *pLast));


/*
// Functions -- cont_wav_opts.c
*/
/** Options: Setter */
/*** */
ST_EXPORT void
st_contWav_opts_setBasicOpts(Tst_contWav_obj *pWObj,
                             ST_OPTARG(const Tst_basOpts *pBOpts));
/*** */
ST_EXPORT void
st_contWav_opts_setCB_newOutpStr(Tst_contWav_obj *pWObj,
                                 ST_OPTARG(const Tst_contWav_cbNewOutputStream
                                   cbNewOutpStr));
/*** */
ST_EXPORT Tst_err
st_contWav_opts_setOutputDir(Tst_contWav_obj *pWObj,
                             const Tst_str *pOutpdir);

ST_EXPORT void
st_contWav_opts_setOverwriteExistingOutputFiles(Tst_contWav_obj *pWObj,
                                                const Tst_bool owExFiles);
/*** */
ST_EXPORT void
st_contWav_opts_setOutputFmt(Tst_contWav_obj *pWObj,
                             const Tst_bool fmtRIFForAIFF);

/** Options: Getter */
/*** */
ST_EXPORT void
st_contWav_opts_getOutputFmt(Tst_contWav_obj *pWObj,
                             Tst_bool *pIsRIFForAIFF);


/*
// Functions -- cont_wav_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_contWav_stc_initWObj(Tst_contWav_obj *pWObj);

ST_EXPORT void
st_contWav_stc_rsetWObj(Tst_contWav_obj *pWObj);

ST_EXPORT void
st_contWav_stc_freeWObj(Tst_contWav_obj *pWObj);


/*
// Functions -- cont_wav_wr.c
*/
/** */
ST_EXPORT Tst_err
st_contWav_wr_prepareWritingCB(void *pWObj,
                               const Tst_str *pInputFilen,
                               const Tst_uint32 bsIx,
                               const Tst_uint32 bsSIx,
                               const Tst_bool appendBsIx,
                               void **ppHandle);
ST_EXPORT Tst_err
st_contWav_wr_setOutputFmtCB(void *pHandle,
                             const Tst_uint32 sampleRate,
                             const Tst_uint32 bitsPerSample,
                             const Tst_uint32 channMask,
                             const Tst_uint32 channels);
ST_EXPORT Tst_err
st_contWav_wr_writeRawSamplesCB(void *pHandle,
                                const Tst_uint32 sampleCnt,
                                const Tst_int32 **ppRawSamples);
ST_EXPORT Tst_err
st_contWav_wr_finishWritingCB(void *pHandle);


ST_END_C_DECLS

#endif  /* HEAD_CONT_WAV_H */

/******************************************************************************/
