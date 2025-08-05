/******************************************************************************/
/* aud_mpeg1-prot.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for MPEG-1 audio                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AUD_MPEG1_PROT_H
#define HEAD_AUD_MPEG1_PROT_H

/*
// Own-Includes
*/
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/sys_math-defs.h"
#include "src/includes/audvid/audvid-defs.h"
#include "src/includes/audvid/aud_mpeg1-defs.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_STCS_C) || \
			defined(SRC_AUD_MPEG1_GS_C) || defined(SRC_AUD_MPEG1_OPTS_C) || \
			defined(SRC_AUD_MPEG1_ZFDEB_C) || \
			defined(SRC_AUD_MPEG1_ZRD_C) || defined(SRC_AUD_MPEG1_ZRD2_C)
	/** */
#	define ST_MPEG1_FR_HD_SZ      4  /* MPEG Frameheader size in bytes */
#	define ST_MPEG1_FR_SZ_MIN     8  /* min MPEG frame size in bytes */
#	define ST_MPEG1_FR_SZ_MAX  8068  /* max MPEG frame size in bytes */
#	define ST_MPEG1_FSZ_MIN    ST_MPEG1_FR_SZ_MIN  /* min MPEG filesize */

	/** */
#	define ST_MPEG1_TAGVBR_XING      "Xing"  /* VBR Header Tag for Xing */
#	define ST_MPEG1_TAGVBR_VBRI      "VBRI"  /* VBR Header Tag for VBRI */
#	define ST_MPEG1_TAGVBR_INFO      "Info"  /* CBR Header Tag */
#	define ST_MPEG1_TAGVBR_XING_SLEN  4
#	define ST_MPEG1_TAGVBR_VBRI_SLEN  4
#	define ST_MPEG1_TAGVBR_INFO_SLEN  4

	/* amount of bitrates in bitr-array */
#	define ST_MPEG1_AMOUNT_BITRS  24
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_AUD_MPEG1_C) || defined(SRC_AUD_MPEG1_STCS_C) || \
			defined(SRC_AUD_MPEG1_GS_C) || defined(SRC_AUD_MPEG1_OPTS_C) || \
			defined(SRC_AUD_MPEG1_ZFDEB_C) || \
			defined(SRC_AUD_MPEG1_ZRD_C) || defined(SRC_AUD_MPEG1_ZRD2_C)
	/** MPEG Frameheader */
	typedef struct {
		Tst_mpeg1_audioVers audioVers;   /* audio version */
		Tst_mpeg1_layer     layer;       /* mpeg layer */
		Tst_uint32          bitrate;     /* bitrate in kbit/s */
		Tst_uint32          samplerate;  /* samplerate in Hz */
		Tst_uint32          paddLen;     /* padding len in byte */
		Tst_mpeg1_channMode channMode;   /* channel mode */
		Tst_bool            hasCRC;      /* does frame have a CRC ? */
		/* */
		Tst_bool   isOffsOK;    /* is frame header at correct position ? */
		Tst_bool   hasVBRhd;    /* does frame contain VBR/CBR header ? */
		Tst_uint32 fraNr;       /* this frame's number */
		Tst_uint32 frameSz;     /* frame size (incl. frame head. sz) */
		/* */
		Tst_buf    rawFHd[ST_MPEG1_FR_HD_SZ];  /* raw frame header */
	} Tst_mpeg1_fhd;

	/** MPEG VBR info */
	typedef struct {
		Tst_mpeg1_vbrTyp vbrHdType;  /* type VBR header, Xing/VBRI */
		Tst_foffs        offs;       /* offset of VBR header */
		Tst_uint32       aFraCnt;    /* amount of audio frames (excl. CBR/VBR head.) */
		Tst_uint32       fsz;        /* filesize (incl. CBR/VBR head.) */
		Tst_uint32       qual;       /* quality, 0-100 */
		/* */
		Tst_bool isOffsOK;   /* is VBR header at correct pos. ? */
		Tst_bool isCBR;      /* is MPEG with CBR or VBR ? */
	} Tst_mpeg1_vbr;

	/** for st_mpeg1_rd_findAndReadNextFrHd() */
	typedef struct {
		Tst_mpeg1_fhd *pMFHcomp;  /* frame header to compare new one with */
		Tst_mpeg1_fhd mfhNew;     /* new frame header info */
		/* */
		Tst_mpeg1_vbrTyp vbrTp;          /* type of VBR header */
		Tst_bool         isFreeFmtAllw;  /* is FreeFormat allowed ? */
		/* */
		Tst_bool  couldFindNxFra; /* could next frame be found ? */
		Tst_bool  isEOFreached;   /* has EOF been reached ? */
		Tst_bool  isFreeFmt;      /* is Free-Format ? */
		Tst_foffs frOffs;         /* new frame's offset */
		Tst_foffs frOffsDelta;    /* difference betw.  frame's offset
		                           *   and where it should've been  */
	} Tst_mpeg1_farnfh;

	/** for MPEG frames */
	typedef struct {
		Tst_foffs  offsFrFirst;       /* offset of first frame */
		Tst_foffs  offsFrFirstDelta;  /* diff. offset */
		Tst_foffs  offsFrLast;        /* offset of last frame */
		Tst_foffs  offsFrLastDelta;   /* diff. offset */
		Tst_uint32 cnt;               /* amount of valid frames */
		/* errors */
		Tst_uint32 errAtWrongOffsCnt;  /* amount of frames at the wrong offset */
		Tst_bool   errIsCBRwithVBR;    /* does CBR stream have differing bitr. ? */
		Tst_bool   errIsVBRwoHead;     /* doesn't VBR stream have a VBR header ? */
		Tst_bool   errIsVBRhdAtWrongOffs;  /* is CBR/VBR header at wrong offset ? */
		Tst_bool   errWasFraCntCorrd;  /* was frame count in CBR/VBR head. corrected ? */
		Tst_bool   errWasBytCntCorrd;  /* was 'filesize' in CBR/VBR head. corrected ? */
		Tst_bool   errIsStrIncompl;    /* is stream incomplete ? */
		/* */
		Tst_fsize  totStrSz;  /* total size stream in bytes */
	} Tst_mpeg1_frames;

	/** MPEG: Options */
	typedef struct {
		Tst_basOpts basOpts;    /* basic options */
		Tst_bool    anlz;       /* analize frames ? */
		Tst_bool    quickScan;  /* do only a quick scan ? */
		/* */
		Tst_mpeg1_layer     forceMLay;   /* force MPEG Layer */
		Tst_mpeg1_audioVers forceMAv;    /* force MPEG Audio vers. */
		/* */
		Tst_bool   decodeFull;       /* fully decode frames ? */
		Tst_bool   isDecoderSet;     /* are all decoding CBs set ? */
		Tst_bool   isPCMclientSet;   /* is pcm client set ? */
		Tst_uint32 decodeBPS;        /* bitsPerSample for decoding */
		Tst_bool   printDecUsedBPS;  /* allow decoder to print used BPS ? */
		/* callbacks */
		/** */
		Tst_cb_ana_msg               cbAnaMsg;  /* for analize-mode */
		/** for decoding samples */
		Tst_av_cbDecAud_hndNew       cbDecHndNew;
		Tst_av_cbDecAud_hndDel       cbDecHndDel;
		Tst_av_cbDecAud_setOutputFmt cbDecSetOutpFmt;
		Tst_av_cbDecAud_openFeed     cbDecOpenFeed;
		Tst_av_cbDecAud_feedMpgAudio cbDecFeedInp;
		Tst_av_cbDecAud_decodeMpg    cbDecDecode;
		Tst_av_cbDecAud_closeFeed    cbDecCloseFeed;
		/** for handling raw pcm samples */
		Tst_av_cbPCM_prepareWriting  cbPCMprepareWriting;
		Tst_av_cbPCM_setOutputFmt    cbPCMsetOutputFmt;
		Tst_av_cbPCM_samplesClient   cbPCMsamplesClient;
		Tst_av_cbPCM_finishWriting   cbPCMfinishWriting;
		void                         *pPCMclientObj;  /* client's main object */
	} Tst_mpeg1_opts;

	/** MPEG stream info */
	typedef struct {
		Tst_streamrd *pStrrd;  /* current stream reader */
		Tst_str      *pFilen;  /* filename */
		/* */
		Tst_mpeg1_fhd   mhd;          /* overall MPEG stream info */
		Tst_mpeg1_vbr   mhdVbr;       /* CBR/VBR info */
		Tst_bool        isFreeFmt;    /* is Free-Format ? */
		Tst_uint32      bitrateMax;   /* max. bitrate in kbit/s */
		Tst_uint32      bitrateMost;  /* most used bitrate */
		Tst_av_playtime playt;        /* playtime */
		/* */
		Tst_mpeg1_frames frames;
		/* */
		Tst_bool hasMPEGstream;  /* has MPEG stream ? */
		/* */
		Tst_mpeg1_opts opts;  /* options */
	} Tst_mpeg1_obj_intn;

	/** MPEG Decoder data */
	typedef struct {
		void       *pDecHnd;    /* decoder handle */
		/* */
		Tst_uint32 bufInSz;     /* input buffer size */
		Tst_uint32 bufInUsed;   /* used bytes of input buffer */
		Tst_buf    *pBufIn;     /* input buffer */
		/* */
		Tst_uint32 feedFrames;  /* amount of feed frames since last decoding */
		Tst_bool   needMore;    /* do we have to feed more before decoding ? */
		/* */
		Tst_uint32 sampleR;     /* samplerate in Hz */
		Tst_uint32 channels;    /* amount of channels */
		Tst_uint32 bps;         /* bits per sample */
		/* */
		void       *pPCMstrHnd;   /* client's handle for PCM outp. stream */
		Tst_bool   isOutpFmtSet;  /* has PCM output format been set ? */
	} Tst_mpeg1_decoder;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AUD_MPEG1_PROT_H */

/******************************************************************************/
