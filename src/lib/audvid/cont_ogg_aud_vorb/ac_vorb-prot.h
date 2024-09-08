/******************************************************************************/
/* ac_vorb-prot.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for Ogg-Vorbis                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AC_VORB_PROT_H
#define HEAD_AC_VORB_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/lib_defs.h"


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
			defined(SRC_AC_SPX_CLT_ZRD_C) || \
			defined(SRC_AC_VORB_STCS_C) || defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** OGG Vorbis Identification header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_uint32 vers;           /* vorbis version */
		Tst_uint32 sampleRate;     /* samplerate in Hz */
		Tst_uint32 bitrateMinKBS;  /* min. bitrate in kbit/s */
		Tst_uint32 bitrateNomKBS;  /* nominal bitrate in kbit/s */
		Tst_uint32 bitrateMaxKBS;  /* max. bitrate in kbit/s */
		Tst_byte   channels;       /* channels (1-8 per standard) */
		Tst_byte   blockSz0;
		Tst_byte   blockSz1;
	} Tst_contOgg_vorbIdent;

	/** OGG Vorbis setup header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
	} Tst_contOgg_vorbSetup;

	/** OGG Vorbis decoding stuff */
	typedef struct {
		Tst_bool     decToPCM;      /* decode to PCM ? */
		void         *pDecHnd;      /* decoder's handle */
		/* */
		Tst_streamrd decStrrd;      /* for feeding data to the decoder */
		Tst_buf      *pDBuf;
		Tst_uint32   dbufUsd;
		Tst_uint32   dbufSz;
		/* */
		Tst_uint32   sampleR;       /* samplerate in Hz */
		Tst_uint32   channels;      /* amount of channels */
		Tst_uint32   bps;           /* bits per sample */
		/* */
		void         *pPCMstrHnd;   /* PCM client's handle for output stream */
		Tst_bool     isOutpFmtSet;  /* has PCM output format been set ? */
	} Tst_contOgg_vorbDecObj;

	/** OGG Vorbis info */
	typedef struct {
		Tst_contOgg_vorbIdent      ident;
		Tst_contOgg_vorbCommentRaw comm;
		Tst_contOgg_vorbSetup      setup;
		/* */
		Tst_uint32 aPackCnt;  /* amount of audio packets */
		/* */
		Tst_contOgg_vorbDecObj decObj;
	} Tst_contOgg_vorbis;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AC_VORB_PROT_H */

/******************************************************************************/
