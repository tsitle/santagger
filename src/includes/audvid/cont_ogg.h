/******************************************************************************/
/* cont_ogg.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for OGG containers / Flac files                                  */
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

#ifndef HEAD_CONT_OGG_H
#define HEAD_CONT_OGG_H

/*
// Own-Includes
*/
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/streamwr-defs.h"
#include "src/includes/audvid/cont_ogg-defs.h"
#include "src/includes/audvid/audvid-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- cont_ogg.c
*/
/** read: ogg streams */
ST_EXPORT Tst_err
st_contOgg_readStreams(Tst_contOgg_obj *pAObj);

/** read: pure flac streams */
ST_EXPORT Tst_err
st_contOgg_readFlacStream(Tst_contOgg_obj *pAObj);

/** write: ogg streams */
ST_EXPORT Tst_err
st_contOgg_writeStreams(Tst_contOgg_obj *pAObj);

/** write: pure flac streams */
ST_EXPORT Tst_err
st_contOgg_writeFlacStream(Tst_contOgg_obj *pAObj);


/*
// Functions -- cont_ogg_gs.c
*/
/** Setter */
/*** */
ST_EXPORT void
st_contOgg_gs_setStrrd(Tst_contOgg_obj *pAObj, ST_OPTARG(Tst_streamrd *pStrrd));

ST_EXPORT void
st_contOgg_gs_setStrwr(Tst_contOgg_obj *pAObj, ST_OPTARG(Tst_streamwr *pStrwr));

ST_EXPORT Tst_err
st_contOgg_gs_setFilen(Tst_contOgg_obj *pAObj, const Tst_str *pFilen);

/** Getter */
/*** */
ST_EXPORT const Tst_str*
st_contOgg_gs_getFilen(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_gs_getIsSrcOggOrFlac(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_gs_getWasFileAlreadyRead(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_gs_getHasOggStream(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_gs_getHasFlacStream(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_int32
st_contOgg_gs_getContTypes(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_int32
st_contOgg_gs_getBitstreamTypes(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_uint32
st_contOgg_gs_getStreamCount(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_uint32
st_contOgg_gs_getErrorCount(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_foffs
st_contOgg_gs_getStartOffset(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_fsize
st_contOgg_gs_getSize(const Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_contOgg_substream*
st_contOgg_gs_getStream_byGrpIx(const Tst_contOgg_obj *pAObj,
                                const Tst_uint32 grpIx,
                                const Tst_uint32 grpSIx);

/*** */
ST_EXPORT Tst_contOgg_bsType
st_contOgg_gs_getStr_type(const Tst_contOgg_substream *pBS);

ST_EXPORT Tst_bool
st_contOgg_gs_getStr_groupIndex(const Tst_contOgg_substream *pBS,
                                Tst_uint32 *pGrpIx,
                                Tst_uint32 *pGrpSIx);

ST_EXPORT Tst_bool
st_contOgg_gs_getStr_isGrouped(const Tst_contOgg_substream *pBS);

/*** */
ST_EXPORT Tst_bool
st_contOgg_gs_getStr_playtime(const Tst_contOgg_substream *pBS,
                              Tst_av_playtime *pPlaytime);

/*** */
ST_EXPORT Tst_uint32
st_contOgg_gs_getStr_avgBitrateInKBS(const Tst_contOgg_substream *pBS);

/*** */
ST_EXPORT Tst_bool
st_contOgg_gs_getStr_vorbisInfo(const Tst_contOgg_substream *pBS,
                                Tst_contOgg_vorbisInfo *pVorbInf);

ST_EXPORT Tst_bool
st_contOgg_gs_getStr_speexInfo(const Tst_contOgg_substream *pBS,
                               Tst_contOgg_speexInfo *pSpeexInf);

ST_EXPORT Tst_bool
st_contOgg_gs_getStr_celtInfo(const Tst_contOgg_substream *pBS,
                              Tst_contOgg_celtInfo *pCeltInf);

ST_EXPORT Tst_bool
st_contOgg_gs_getStr_flacInfo(const Tst_contOgg_substream *pBS,
                              Tst_contOgg_flacInfo *pFlacInf);

ST_EXPORT Tst_bool
st_contOgg_gs_getStr_diracInfo(const Tst_contOgg_substream *pBS,
                               Tst_contOgg_diracInfo *pDiracInf);

ST_EXPORT Tst_bool
st_contOgg_gs_getStr_theoraInfo(const Tst_contOgg_substream *pBS,
                                Tst_contOgg_theoraInfo *pTheoraInf);

/** Iterator */
ST_EXPORT Tst_contOgg_substream*
st_contOgg_ite_getNextStream_any(const Tst_contOgg_obj *pAObj,
                                 ST_OPTARG(const Tst_contOgg_substream *pLast));


/*
// Functions -- cont_ogg_opts.c
*/
/** Options: Setter */
/*** */
ST_EXPORT void
st_contOgg_opts_setBasicOpts(Tst_contOgg_obj *pAObj,
                             ST_OPTARG(const Tst_basOpts *pBOpts));

ST_EXPORT void
st_contOgg_opts_setAnlzMode(Tst_contOgg_obj *pAObj,
                            const Tst_bool anlz,
                            ST_OPTARG(const Tst_cb_ana_msg cbAnlz));

ST_EXPORT void
st_contOgg_opts_setQuickScan(Tst_contOgg_obj *pAObj,
                             const Tst_bool doQuickScan);

ST_EXPORT void
st_contOgg_opts_setCheckCRCofOGGframes(Tst_contOgg_obj *pAObj,
                                       const Tst_bool check);

ST_EXPORT void
st_contOgg_opts_setReadComments(Tst_contOgg_obj *pAObj, const Tst_bool rdCmts);

ST_EXPORT void
st_contOgg_opts_setForceRewriteComments(Tst_contOgg_obj *pAObj,
                                        const Tst_bool forceRewrCmts);

ST_EXPORT void
st_contOgg_opts_setPCMoutputBPS(Tst_contOgg_obj *pAObj, const Tst_uint32 bps);

ST_EXPORT void
st_contOgg_opts_setLetDecoderPrintUsedBPS(Tst_contOgg_obj *pAObj,
                                          const Tst_bool printBPS);

/*** */
ST_EXPORT void
st_contOgg_opts_setCB_pcmClient(Tst_contOgg_obj *pAObj,
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
st_contOgg_opts_setFlac_decodeFully(Tst_contOgg_obj *pAObj,
                                    const Tst_bool decFull);

ST_EXPORT void
st_contOgg_opts_setFlac_checkCRCofFrHd(Tst_contOgg_obj *pAObj,
                                       const Tst_bool check);

ST_EXPORT void
st_contOgg_opts_setFlac_checkCRCofEntireFrames(Tst_contOgg_obj *pAObj,
                                               const Tst_bool check);

ST_EXPORT void
st_contOgg_opts_setFlac_checkMD5ofDecodedAudio(Tst_contOgg_obj *pAObj,
                                               const Tst_bool check);

/*** */
ST_EXPORT void
st_contOgg_opts_setCB_vorbDecoding(Tst_contOgg_obj *pAObj,
                                   ST_OPTARG(const Tst_av_cbDecAud_hndNew
                                     cbHndNew),
                                   ST_OPTARG(const Tst_av_cbDecAud_hndDel
                                     cbHndDel),
                                   ST_OPTARG(const Tst_av_cbDecAud_setOutputFmt
                                     cbSetOutpFmt),
                                   ST_OPTARG(const Tst_av_cbDecAud_openFeed
                                     cbOpenFeed),
                                   ST_OPTARG(const Tst_av_cbDecAud_feedVrbHeader
                                     cbFeedHd),
                                   ST_OPTARG(const Tst_av_cbDecAud_feedVrbAudioAndDecode
                                     cbFeedAudAndDec),
                                   ST_OPTARG(const Tst_av_cbDecAud_closeFeed
                                     cbCloseFeed));

ST_EXPORT void
st_contOgg_opts_setVorb_decodeFully(Tst_contOgg_obj *pAObj,
                                    const Tst_bool decFull);

/** Options: Getter */
/*** */
ST_EXPORT Tst_bool
st_contOgg_opts_getAnlzMode(Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_opts_getCheckCRCofOGGframes(Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_opts_getReadComments(Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_opts_getForceRewriteComments(Tst_contOgg_obj *pAObj);

/*** */
ST_EXPORT Tst_bool
st_contOgg_opts_getFlac_decodeFully(Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_opts_getFlac_checkCRCofFrHd(Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_opts_getFlac_checkCRCofEntireFrames(Tst_contOgg_obj *pAObj);

ST_EXPORT Tst_bool
st_contOgg_opts_getFlac_checkMD5ofDecodedAudio(Tst_contOgg_obj *pAObj);


/*
// Functions -- cont_ogg_stcs.c
//              ac_flac_stcs.c
//              ac_spx_clt_stcs.c
//              ac_vorb_stcs.c
//              vc_drc_theo_stcs.c
*/
/** Structs */
/*** Main */
ST_EXPORT Tst_err
st_contOgg_stc_initAObj(Tst_contOgg_obj *pAObj,
                        const Tst_str *pFilen);

ST_EXPORT void
st_contOgg_stc_rsetAObj(Tst_contOgg_obj *pAObj);

ST_EXPORT void
st_contOgg_stc_freeAObj(Tst_contOgg_obj *pAObj);

/*** Flac */
ST_EXPORT void
st_contOgg_stc_rsetFlacInfo(Tst_contOgg_flacInfo *pFI);

/*** Speex / Celt */
ST_EXPORT void
st_contOgg_stc_rsetSpeexInfo(Tst_contOgg_speexInfo *pSI);

ST_EXPORT void
st_contOgg_stc_rsetCeltInfo(Tst_contOgg_celtInfo *pCI);

/*** Vorbis */
ST_EXPORT void
st_contOgg_stc_rsetVorbisInfo(Tst_contOgg_vorbisInfo *pVI);

/*** Dirac / Theora */
ST_EXPORT void
st_contOgg_stc_rsetDiracInfo(Tst_contOgg_diracInfo *pDI);

ST_EXPORT void
st_contOgg_stc_rsetTheoraInfo(Tst_contOgg_theoraInfo *pTI);


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_H */

/******************************************************************************/
