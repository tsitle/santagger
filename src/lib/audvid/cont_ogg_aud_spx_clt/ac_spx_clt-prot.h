/******************************************************************************/
/* ac_spx_clt-prot.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for Ogg-Speex and Ogg-Celt                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_AC_SPX_CLT_PROT_H
#define LIBSANTAGGER_AC_SPX_CLT_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/audvid/cont_ogg-defs.h"


ST_BEGIN_C_DECLS

/*
// Types  (protected)
*/
#if defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_STCS_C) || \
			defined(SRC_CONT_OGG_OPTS_C) || defined(SRC_CONT_OGG_GS_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_FLAC_ZRDA_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || defined(SRC_AC_SPX_CLT_STCS_C) || \
			defined(SRC_AC_VORB_ZRD_C) || defined(SRC_AC_VORB_STCS_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** OGG Speex Identification header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_str    *pVersStr;   /* version string for Speex */
		Tst_uint32 speexVers;   /* version for Speex */
		Tst_uint32 bitstrVers;  /* bitstream version */
		Tst_uint32 headerSize;  /* total size of the header */
		/* */
		Tst_uint32            sampleRate;  /* sampling rate used */
		Tst_contOgg_speexMode mode;        /* mode used */
		Tst_uint32            channels;    /* number of channels encoded */
		/* */
		Tst_uint32 bitrateKBS;  /* bitrate in kbit/s */
		Tst_uint32 frameSize;   /* size of frames */
		Tst_bool   isVBR;       /* 1 for a VBR encoding, 0 otherwise */
		Tst_uint32 framesPerPacket;  /* number of frames stored per Ogg packet */
		Tst_uint32 extraHeaders;     /* number of add. headers after the comments */
	} Tst_contOgg_speeIdent;

	/** OGG Speex info */
	typedef struct {
		Tst_contOgg_speeIdent      ident;
		Tst_contOgg_vorbCommentRaw comm;
		/* */
		Tst_uint32 aPackCnt;  /* amount of Speex audio packets */
	} Tst_contOgg_speex;

	/** OGG Celt Identification header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_str    *pVersStr;       /* version string */
		Tst_uint32 bitstrVers;      /* bitstream version */
		Tst_uint32 headerSize;      /* size of this header */
		Tst_uint32 sampleRate;      /* sampling rate of the original audio */
		Tst_uint32 channels;        /* number of channels */
		Tst_uint32 frameSize;       /* samples per frame (per channel) */
		Tst_uint32 overlap;         /* overlapping samples (per channel) */
		Tst_uint32 bytesPerPacket;  /* number of bytes per compr. packet (0=unknown) */
		Tst_uint32 extraHeaders;    /* number of add. headers that follow this header */
		/* */
		Tst_uint32 bitrateKBS;      /* bitrate in kbit/s */
	} Tst_contOgg_celtIdent;

	/** OGG Celt info */
	typedef struct {
		Tst_contOgg_celtIdent      ident;
		Tst_contOgg_vorbCommentRaw comm;
		/* */
		Tst_uint32 aPackCnt;  /* amount of Celt audio packets */
	} Tst_contOgg_celt;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_AC_SPX_CLT_PROT_H */

/******************************************************************************/
