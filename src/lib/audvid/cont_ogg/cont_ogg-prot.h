/******************************************************************************/
/* cont_ogg-prot.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for OGG containers / Flac files                      */
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

#ifndef LIBSANTAGGER_CONT_OGG_PROT_H
#define LIBSANTAGGER_CONT_OGG_PROT_H

/*
// Own-Includes, Part I
*/
/** */
#include "src/includes/common/binobj-defs.h"
#include "src/includes/common/dynlist-defs.h"
#include "src/includes/common/sys_math-defs.h"
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/streamwr-defs.h"
#include "src/includes/audvid/audvid-defs.h"
#include "src/includes/audvid/cont_ogg-defs.h"
/** */
#include "cont_ogg-common-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_CONT_OGG_C) || defined(SRC_CONT_OGG_ZRD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || \
			defined(SRC_CONT_OGG_STCS_C)
	/** all (min) sizes are in bytes */
#	define ST_CONTOGG_FR_HD_SZ      27  /* OGG Frameheader size */
#	define ST_CONTOGG_FR_SZ_MAX  65307  /* max OGG frame size */
#	define ST_CONTOGG_FSZ_MIN    ST_CONTOGG_FR_HD_SZ_MIN  /* min OGG filesize */

	/** threshold for binary object for Ogg-Packets */
#	define ST_CONTOGG_PACKSZ_THRESHOLD  (2 * 1024 * 1024)  /* 2 MB */
	/** default additional realloc-size for binary object for Ogg-Packets */
#	define ST_CONTOGG_PACKSZ_REALLC     (ST_CONTOGG_FR_SZ_MAX * 5)
#endif  /* protected */


/*
// Types (protected), Part I
*/
#if defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_STCS_C) || \
			defined(SRC_CONT_OGG_OPTS_C) || defined(SRC_CONT_OGG_GS_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_FLAC_ZRDA_C) || defined(SRC_AC_FLAC_STCS_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || defined(SRC_AC_SPX_CLT_STCS_C) || \
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** OGG Frameheader */
	typedef struct {
		Tst_foffs  offs;         /* offset of this frame */
		Tst_foffs  frOffsDelta;  /* delta offset */
		Tst_uint32 hdSz;         /* total header size */
		Tst_uint32 frSz;         /* total frame size */
		/* */
		Tst_byte   vers;         /* version */
		Tst_bool   hdTypeCont;   /* header type flag:
		                          *  contains data of a packet continued from
		                          *  the previous frame */
		Tst_bool   hdTypeBOS;    /* header type flag:
		                          *  first frame of a logical bitstream (bos) */
		Tst_bool   hdTypeEOS;    /* header type flag:
		                          *  last frame of a logical bitstream (eos) */
		Tst_uint64 granulePos;   /* granule position */
		Tst_uint32 bsSerial;     /* bitstream serial */
		Tst_uint32 frSequNr;     /* frame sequence number */
		Tst_uint32 crc32_a;      /* CRC checksum read from header */
		Tst_uint32 crc32_b;      /*  --- " ---   computed from file */
		Tst_byte   segsUsd;      /* bytes used in segTab */
		Tst_byte   segTab[256];  /* lacing values */
		/* */
		Tst_uint32 realNr;       /* real sequ number (might be != frSequNr) */
		/* */
		Tst_bool   wasUsed;      /* has frame already been used for a packet ? */
	} Tst_contOgg_fhd;

	/** OGG packet - for data of contiguous frames */
	typedef struct {
		Tst_binobj bdata;    /* packet data */
		Tst_byte   *pSegs;   /* lacing values */
		Tst_uint32 segsUsd;  /* used bytes of pSegs */
		Tst_uint32 segsSz;   /* size of pSegs */
		/* */
		Tst_uint32 nr;     /* number of current packet */
	} Tst_contOgg_packet;

	/** OGG frameheader array */
	typedef struct {
		Tst_dynlist list;  /* list of 'active' frame headers */
		Tst_uint32  used;  /* amount of used slots in list */
	} Tst_contOgg_frameHdArr;

	/** OGG: Callbacks */
	/*** Callback for stripping strings */
	typedef void
	        (*Tst_contOgg_cbStripStr)(Tst_str *pStr, const Tst_uint32 len);

	/** OGG: Options */
	typedef struct {
		Tst_basOpts basOpts;    /* basic options */
		Tst_bool    anlz;       /* analizing mode ? */
		Tst_bool    quickScan;  /* do only a quick scan ? */
		/* */
		Tst_bool   checkCRCofr;      /* check CRC of OGG-frames ? */
		Tst_bool   rdComments;       /* read comments from streams ? */
		Tst_bool   forceRewrCmts;    /* force rewriting comments ? */
		Tst_bool   isPCMclientSet;   /* is pcm client set ? */
		Tst_uint32 decodeBPS;        /* bitsPerSample for decoding */
		Tst_bool   printDecUsedBPS;  /* allow decoder to print used BPS ? */
		Tst_bool   modeRdOrWr;       /* mode: read or write ? */
		/* callbacks */
		/** */
		Tst_cb_ana_msg              cbAnaMsg;  /* for analize-mode */
		/** for handling PCM samples */
		Tst_av_cbPCM_prepareWriting cbPCMprepareWriting;
		Tst_av_cbPCM_setOutputFmt   cbPCMsetOutputFmt;
		Tst_av_cbPCM_samplesClient  cbPCMsamplesClient;
		Tst_av_cbPCM_finishWriting  cbPCMfinishWriting;
		void                        *pPCMclientObj;
		/* */
		Tst_contOgg_flacOpts flacOpts;   /* options for Flac */
		Tst_contOgg_vorbOpts vorbOpts;   /* options for Vorbis */
	} Tst_contOgg_opts;
#endif  /* protected */


/*
// Own-Includes, Part II
*/
#include "../cont_ogg_met_vorbcomm/mc_vorbcomm-prot.h"
#include "../cont_ogg_aud_flac/ac_flac-prot.h"
#include "../cont_ogg_aud_spx_clt/ac_spx_clt-prot.h"
#include "../cont_ogg_aud_vorb/ac_vorb-prot.h"
#include "../cont_ogg_met_skel/mc_skel-prot.h"
#include "../cont_ogg_vid_drc_theo/vc_drc_theo-prot.h"


/*
// Types (protected), Part II
*/
#if defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_STCS_C) || \
			defined(SRC_CONT_OGG_OPTS_C) || defined(SRC_CONT_OGG_GS_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_FLAC_ZRDA_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || \
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** */
	typedef struct {
		Tst_bool        needVCmtUpd;  /* does vorbis comment need to be updated ? */
		Tst_uint32      vcmtPackNr;   /* packet no. where vorbis comment is in */
		Tst_int32       vcmtSrcBP;    /* beg. pos. of original vorbis comment */
		Tst_int32       vcmtSrcEP;    /* end. pos. of original vorbis comment */
		Tst_bool        pendingPack;  /* do we have a pending packet to write ? */
		Tst_contOgg_fhd pendPckFHd;   /* 1st frameheader of pend. packet */
		Tst_uint32      frameCnt;     /* frames written */
		Tst_uint32      packCnt;      /* packets written */
		Tst_bool        eosWritten;   /* have we written the EOS frame ? */
	} Tst_contOgg_substr_wr;

	/** OGG logical stream */
	typedef struct {
		Tst_bool   isSrcOggOrFlac;  /* is source an ogg- or flac-file ? */
		/* */
		Tst_str    *pFilen;    /* current filename */
		Tst_bool   started;    /* has stream started ? */
		Tst_bool   ended;      /* has stream ended ? */
		Tst_uint32 serial;     /* bitstream serial */
		Tst_uint32 nr;         /* bitstream number */
		Tst_bool   isGrouped;  /* is bitstream grouped with others ? */
		Tst_uint32 grpIx;      /* index of bitstream group */
		Tst_uint32 grpSIx;     /* index of bitstream in the group */
		/* */
		Tst_contOgg_fhd *pFHdBeg;   /* pointer to first frame of contiguous frames */
		Tst_uint32      fhdBegRNr;  /* frame number of first frame of --- " --- */
		/* */
		Tst_uint32         frameCnt;   /* amount of frames */
		Tst_uint32         lastFrNr;   /* last frame's number */
		Tst_uint32         misFrames;  /* amount of missing frames */
		Tst_contOgg_packet packCur;    /* current packet */
		Tst_uint32         packCnt;    /* amount of packets */
		Tst_bool           tmpIs1st;   /* for ST_CONTOGG__rdPacket() */
		/* */
		Tst_uint64      maxGranulePos;       /* maximum granule position */
		Tst_av_playtime playtime;
		Tst_uint32      compBitrateAvgKBS;   /* computed average bitrate in kbit/s */
		Tst_fsize       codecNonHeadSize;    /* size of non-header packets */
		Tst_uint32      codecNonHeadPckCnt;  /* amount of non-header packets */
		Tst_foffs       flacOffsFirstAudPkt; /* offset of first audio packet */
		/* */
		Tst_contOgg_skeleton *pSkel;  /* Skeleton info */
		Tst_contOgg_vorbis   *pVorb;  /* Vorbis info */
		Tst_contOgg_flac     *pFlac;  /* Flac info */
		Tst_contOgg_speex    *pSpee;  /* Speex info */
		Tst_contOgg_celt     *pCelt;  /* Celt info */
		Tst_contOgg_theora   *pTheo;  /* Theora info */
		Tst_contOgg_dirac    *pDira;  /* Dirac info */
		/* */
		Tst_contOgg_bsType bsType;  /* bitstream type */
		/* */
		Tst_uint32 errorCnt;  /* amount of errors/warnings encountered */
		/* */
		Tst_streamrd sobjRdPackBuf;  /* stream reader for packet buffer */
		Tst_streamwr sobjWrFBuf;     /* stream writer for frame buffer */
		Tst_streamrd sobjRdFBuf;     /* stream reader for frame buffer */
		/* */
		Tst_contOgg_substr_wr wr;  /* everything related to writing */
		/* */
		Tst_contOgg_opts *pOpts;  /* pointer to options */
	} Tst_contOgg_substr_intn;

	/** OGG stream array */
	typedef struct {
		Tst_contOgg_substream *pArr;  /* data of all streams */
		Tst_uint32            sz;     /* size of array */
		Tst_uint32            cnt;    /* amount of used slots in array */
		/* */
		Tst_uint32 openBScnt;  /* amount of still open streams */
		Tst_uint32 grpCnt;     /* amount of grouped streams */
		/* */
		Tst_contOgg_opts *pOpts;  /* pointer to options */
	} Tst_contOgg_substreamArr;

	/** OGG stream info */
	typedef struct {
		Tst_streamrd *pStrrd;     /* current stream reader */
		Tst_streamwr *pStrwr;     /* current stream writer */
		Tst_str      *pFilen;     /* filename */
		/* */
		Tst_contOgg_frameHdArr   frHdArr;  /* frameheader array */
		Tst_contOgg_substreamArr bsArr;    /* bitstream array */
		/* */
		Tst_bool alrRead;         /* have we already read this file ? */
		/* */
		Tst_bool isSrcOggOrFlac;  /* is source an ogg- or flac-file ? */
		/* */
		Tst_bool  hasOGGstream;   /* has OGG stream(s) ? */
		Tst_bool  hasFLACaudio;   /* has flac audio stream ? */
		Tst_int32 oggContTypes;   /* content-types of streams (or'd values) */
		/* */
		Tst_uint32 errorCnt;      /* amount of errors/warnings encountered */
		/* */
		Tst_foffs startOffs;      /* offset where stream begins */
		Tst_fsize totSize;        /* total size of stream */
		/* */
		Tst_contOgg_opts opts;    /* options */
	} Tst_contOgg_obj_intn;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_CONT_OGG_PROT_H */

/******************************************************************************/
