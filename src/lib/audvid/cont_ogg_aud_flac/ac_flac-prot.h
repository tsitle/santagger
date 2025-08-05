/******************************************************************************/
/* ac_flac-prot.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for (Ogg-)Flac codec handling                        */
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

#ifndef HEAD_AC_FLAC_PROT_H
#define HEAD_AC_FLAC_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/audvid/cont_ogg-defs.h"
/** */
#include "../../utils/w-md5-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_STCS_C) || \
			defined(SRC_CONT_OGG_OPTS_C) || defined(SRC_CONT_OGG_GS_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_FLAC_ZRDA_C) || defined(SRC_AC_FLAC_STCS_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || \
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** flac blockType */
	typedef enum {
		ST_CONTOGG_FLAC_BT_STRINF = 0,  /* streaminfo */
		ST_CONTOGG_FLAC_BT_PADD   = 1,  /* padding */
		ST_CONTOGG_FLAC_BT_APP    = 2,  /* application */
		ST_CONTOGG_FLAC_BT_SEEK   = 3,  /* seektable */
		ST_CONTOGG_FLAC_BT_COMM   = 4,  /* vorbis comment */
		ST_CONTOGG_FLAC_BT_CUE    = 5,  /* cuesheet */
		ST_CONTOGG_FLAC_BT_PICT   = 6,  /* picture */
		ST_CONTOGG_FLAC_BT_NONE   = 7
	} Tst_contOgg_flacBlockType;

	/** flac channel assignment */
	typedef enum {
		/* independant channels */
		ST_CONTOGG_FLAC_CA_INDEP,
		/* left/side stereo:
		 *  chann 0 is the l. chann, chann 1 is the side(diff.) chann */
		ST_CONTOGG_FLAC_CA_LS,
		/* right/side stereo:
		 *  chann 0 is the side(diff.) chann, chann 1 is the r. chann */
		ST_CONTOGG_FLAC_CA_RS,
		/* mid/side stereo:
		 *  chann 0 is the mid(average) chann, chann 1 is the side(diff.) chann */
		ST_CONTOGG_FLAC_CA_MS,
		/* */
		ST_CONTOGG_FLAC_CA_NONE
	} Tst_contOgg_flacChannAss;

	/** flac subframe type */
	typedef enum {
		ST_CONTOGG_FLAC_SFRTP_CONST,  /* SUBFRAME_CONSTANT */
		ST_CONTOGG_FLAC_SFRTP_VERB,   /* SUBFRAME_VERBATIM */
		ST_CONTOGG_FLAC_SFRTP_FIX,    /* SUBFRAME_FIXED */
		ST_CONTOGG_FLAC_SFRTP_LPC,    /* SUBFRAME_LPC */
		ST_CONTOGG_FLAC_SFRTP_NONE
	} Tst_contOgg_flacSubFrType;
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_STCS_C) || \
			defined(SRC_CONT_OGG_OPTS_C) || defined(SRC_CONT_OGG_GS_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_FLAC_ZRDA_C) || defined(SRC_AC_FLAC_STCS_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || \
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** OGG Flac Identification header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_byte   verMaj;         /* major version */
		Tst_byte   verMin;         /* minor version */
		Tst_uint32 hdPcks_fromHd;  /* amount of header packets as read from header */
		Tst_uint32 hdPcks_read;    /* amount of actually read header packets */
		/* */
		Tst_uint32      blkSzMin;   /* min blocksize */
		Tst_uint32      blkSzMax;   /* max blocksize */
		Tst_uint32      frSzMin;    /* min frame size */
		Tst_uint32      frSzMax;    /* max frame size */
		Tst_uint32      sampleR;    /* samplerate */
		Tst_byte        chann;      /* number of channels */
		Tst_byte        bitsPerS;   /* bits per sample */
		Tst_uint64      samplesTot_fromHd;  /* total samples as read from header */
		Tst_uint64      samplesTot_read;    /* amount of actually read samples  */
		/* */
		Tst_bool        isMD5set;   /* is MD5 checksum set in header ? */
		Tst_bool        md5OfDecAudioChkd;  /* has MD5 of dec. audio been checked ? */
		Tst_bool        md5OfDecAudioOK;    /* is MD5  ---- " ------ OK ? */
		Tst_buf         md5_a[16];  /* MD5 checksum from header */
		Tst_buf         md5_b[16];  /*   --- " ---  calculated from decoded audio */
		Tst_md5_context md5_bCtx;   /*   --- " ---  context */
		/* */
		Tst_foffs offsAbs1st;  /* absolute offset of 1st frameheader */
	} Tst_contOgg_flacIdent;

	/** Flac Audio Residual (raw) */
	typedef struct {
		Tst_uint32 maxBlkSz;  /* 'size' of pResids */
		Tst_uint32 cnt;       /* amount of samples in pResids */
		Tst_int32  *pResids;  /* raw residual samples */
	} Tst_contOgg_flacRawResid;

	/** Flac Audio Subframe */
	typedef struct {
		Tst_bool   wasUsed;     /* has this subframe been used ? */
		Tst_uint32 frameNr;     /* frame number this subframe belongs to */
		Tst_byte   sampleSz;    /* sample size in bits */
		Tst_uint32 wastedBits;  /* wasted bits-per-sample */
		Tst_byte   predOrder;   /* predictor order */
		Tst_byte   channel;     /* channel number */
		Tst_contOgg_flacSubFrType subFrTp;  /* subframe type */
		Tst_contOgg_flacRawResid  rawResid; /* raw residual samples */
	} Tst_contOgg_flacSubFr;

	/** Flac Audio Frameheader */
	typedef struct {
		Tst_byte   sampleSz;         /* sample size in bits */
		Tst_byte   crc8_a;           /* CRC-8 checksum from header */
		Tst_byte   crc8_b;           /*  --- " ----    calculated from read data */
		Tst_uint16 crc16_a;          /* CRC-16 checksum from header */
		Tst_uint16 crc16_b;          /*  --- " ----     calculated from read data */
		Tst_bool   isBlockStratVar;  /* is variable- or fixed -blocksize stream ? */
		Tst_uint32 sampleR;          /* sample rate in Hz */
		Tst_uint32 blkSz;            /* block size in inter-channel samples */
		Tst_byte   channels;         /* amount of channels */
		Tst_contOgg_flacChannAss channAss;  /* channel assignment */
	} Tst_contOgg_flacFH;

	/** Flac Audio Samples (raw) */
	typedef struct {
		Tst_uint32 maxChannCnt;   /* amount of channels */
		Tst_uint32 maxSamPerCha;  /* samples per channel */
		Tst_int32  **ppSamples;   /* raw samples */
	} Tst_contOgg_flacRawSamples;

	/** Flac SubFrame Verbatim temporary array */
	typedef struct {
		Tst_int32  *pI32arr;
		Tst_uint32 maxBlkSz;  /* 'size' of pI32arr */
	} Tst_contOgg_flacTempSF_verbArr;

	/** Flac Audio Frame */
	typedef struct {
		Tst_contOgg_flacFH         fh;       /* frameheader */
		Tst_contOgg_flacSubFr      sfr[8];   /* subframes for channels */
		Tst_contOgg_flacSubFr      *pCSFr;   /* pointer to current subframe */
		Tst_contOgg_flacRawSamples rawSamp;  /* raw audio samples */
		/* */
		Tst_contOgg_flacTempSF_verbArr tempSF_verbArr;
		Tst_byte                       *pTempSF_residEPars;
		Tst_uint32                     tempSF_residEParsSz;
	} Tst_contOgg_flacAudFrame;

	/** OGG Flac decoding stuff */
	typedef struct {
		void     *pPCMstrHnd;   /* PCM client's handle for output stream */
		Tst_bool isOutpFmtSet;  /* has PCM output format been set ? */
	} Tst_contOgg_flacDecObj;

	/** OGG Flac info */
	typedef struct {
		Tst_contOgg_flacIdent      ident;
		Tst_contOgg_vorbCommentRaw comm;
		Tst_bool                   commIsLastHdPck;
		/* */
		Tst_contOgg_flacAudFrame curAFra;  /* current audio frame data */
		/* */
		Tst_uint32 aPackCnt;    /* amount of Flac audio packets */
		Tst_uint32 aFramesCnt;  /* amount of Flac audio frames */
		/* */
		Tst_contOgg_flacDecObj decObj;
	} Tst_contOgg_flac;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AC_FLAC_PROT_H */

/******************************************************************************/
