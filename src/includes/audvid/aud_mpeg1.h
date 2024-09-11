/******************************************************************************/
/* aud_mpeg1.h                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for MPEG-1 audio (Layer I-III)                                   */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AUD_MPEG1_H
#define HEAD_AUD_MPEG1_H

/*
// Own-Includes
*/
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/audvid/aud_mpeg1-defs.h"
#include "src/includes/audvid/audvid-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- aud_mpeg1.c
*/
/** */
ST_EXPORT Tst_err
st_mpeg1_readStream(Tst_mpeg1_obj *pMO);


/*
// Functions -- aud_mpeg1_gs.c
*/
/** Setter */
ST_EXPORT void
st_mpeg1_gs_setStr_strrd(Tst_mpeg1_obj *pMO, ST_OPTARG(Tst_streamrd *pStrrd));

ST_EXPORT Tst_err
st_mpeg1_gs_setStr_filen(Tst_mpeg1_obj *pMO, const Tst_str *pFilen);

/** Getter */
ST_EXPORT const Tst_str*
st_mpeg1_gs_getStr_filen(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_bool
st_mpeg1_gs_getStr_hasMPEGstream(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_foffs
st_mpeg1_gs_getStr_startOffset(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_fsize
st_mpeg1_gs_getStr_size(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_uint32
st_mpeg1_gs_getStr_amountWarnings(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_mpeg1_layer
st_mpeg1_gs_getStr_layer(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_mpeg1_audioVers
st_mpeg1_gs_getStr_audioVers(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_uint32
st_mpeg1_gs_getStr_bitrate(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_bool
st_mpeg1_gs_getStr_isCBR(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_bool
st_mpeg1_gs_getStr_isFreeFmt(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_uint32
st_mpeg1_gs_getStr_samplerate(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_mpeg1_channMode
st_mpeg1_gs_getStr_channMode(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_bool
st_mpeg1_gs_getStr_hasCRC(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_bool
st_mpeg1_gs_getStr_playtime(const Tst_mpeg1_obj *pMO,
                            Tst_av_playtime *pPlaytime);


/*
// Functions -- aud_mpeg1_opts.c
*/
/** Options: Setter */
/*** */
ST_EXPORT void
st_mpeg1_opts_setBasicOpts(Tst_mpeg1_obj *pMO,
                           ST_OPTARG(const Tst_basOpts *pBOpts));

ST_EXPORT void
st_mpeg1_opts_setAnlzMode(Tst_mpeg1_obj *pMO,
                          const Tst_bool anlz,
                          ST_OPTARG(const Tst_cb_ana_msg cbAnlz));

ST_EXPORT void
st_mpeg1_opts_setQuickScan(Tst_mpeg1_obj *pMO,
                           const Tst_bool doQuickScan);

/*** callbacks: decoding */
ST_EXPORT void
st_mpeg1_opts_setCB_decoding(Tst_mpeg1_obj *pMO,
                             ST_OPTARG(const Tst_av_cbDecAud_hndNew
                               cbHndNew),
                             ST_OPTARG(const Tst_av_cbDecAud_hndDel
                               cbHndDel),
                             ST_OPTARG(const Tst_av_cbDecAud_setOutputFmt
                               cbSetOutpFmt),
                             ST_OPTARG(const Tst_av_cbDecAud_openFeed
                               cbOpenFeed),
                             ST_OPTARG(const Tst_av_cbDecAud_feedMpgAudio
                               cbFeedInp),
                             ST_OPTARG(const Tst_av_cbDecAud_decodeMpg
                               cbDecode),
                             ST_OPTARG(const Tst_av_cbDecAud_closeFeed
                               cbCloseFeed));
ST_EXPORT void
st_mpeg1_opts_setCB_pcmClient(Tst_mpeg1_obj *pMO,
                              ST_OPTARG(const Tst_av_cbPCM_prepareWriting
                                cbPCMprepareWriting),
                              ST_OPTARG(const Tst_av_cbPCM_setOutputFmt
                                cbPCMsetOutputFmt),
                              ST_OPTARG(const Tst_av_cbPCM_samplesClient
                                cbPCMsamplesClient),
                              ST_OPTARG(const Tst_av_cbPCM_finishWriting
                                cbPCMfinishWriting),
                              ST_OPTARG(void *pClientObj));

/*** */
ST_EXPORT void
st_mpeg1_opts_setDecodeFully(Tst_mpeg1_obj *pMO,
                             const Tst_bool decFull);

ST_EXPORT void
st_mpeg1_opts_setPCMoutputBPS(Tst_mpeg1_obj *pMO,
                              const Tst_uint32 bps);

ST_EXPORT void
st_mpeg1_opts_setLetDecoderPrintUsedBPS(Tst_mpeg1_obj *pMO,
                                        const Tst_bool printBPS);

/*** */
ST_EXPORT void
st_mpeg1_opts_setForceLayer(Tst_mpeg1_obj *pMO,
                            const Tst_mpeg1_layer lay);

ST_EXPORT void
st_mpeg1_opts_setForceAudVers(Tst_mpeg1_obj *pMO,
                              const Tst_mpeg1_audioVers av);

/** Options: Getter */
/*** */
ST_EXPORT Tst_bool
st_mpeg1_opts_getDecodeFully(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_mpeg1_layer
st_mpeg1_opts_getForceLayer(const Tst_mpeg1_obj *pMO);

ST_EXPORT Tst_mpeg1_audioVers
st_mpeg1_opts_getForceAudVers(const Tst_mpeg1_obj *pMO);


/*
// Functions -- aud_mpeg1_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_mpeg1_stc_initMO(Tst_mpeg1_obj *pMO, const Tst_str *pFilen);

ST_EXPORT void
st_mpeg1_stc_rsetMO(Tst_mpeg1_obj *pMO);

ST_EXPORT void
st_mpeg1_stc_freeMO(Tst_mpeg1_obj *pMO);


ST_END_C_DECLS

#endif  /* HEAD_AUD_MPEG1_H */

/******************************************************************************/
