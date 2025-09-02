/******************************************************************************/
/* vc_drc_theo-prot.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for Ogg-Dirac and Ogg-Theora                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.06.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_VC_DRC_THEO_PROT_H
#define LIBSANTAGGER_VC_DRC_THEO_PROT_H

/*
// Own-Includes
*/
/** */
#include "../cont_ogg_met_vorbcomm/mc_vorbcomm-prot.h"


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
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** OGG Dirac Identification header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_uint32 versMaj;  /* major version */
		Tst_uint32 versMin;  /* minor version */
		Tst_uint32 width;    /* video dimension, width */
		Tst_uint32 height;   /* video dimension, height */
		double     fps;      /* frames per sec */
	} Tst_contOgg_diraIdent;

	/** OGG Dirac info */
	typedef struct {
		Tst_contOgg_diraIdent ident;
		/* */
		Tst_uint32 vPackCnt;  /* amount of Dirac video packets */
	} Tst_contOgg_dirac;

	/** OGG Theora Identification header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_byte   versMaj;     /* major version */
		Tst_byte   versMin;     /* minor version */
		Tst_byte   versRev;     /* revision version */
		Tst_uint32 width;       /* video dimension, width */
		Tst_uint32 height;      /* video dimension, height */
		double     fps;         /* frames per sec */
		Tst_uint32 bitrateKBS;  /* target bitrate in kbit/s */
		Tst_uint32 quality;     /* quality */
		Tst_uint32 kfgshift;    /* used to partition the granule position */
	} Tst_contOgg_theoIdent;

	/** OGG Theora info */
	typedef struct {
		Tst_contOgg_theoIdent      ident;
		Tst_contOgg_vorbCommentRaw comm;
		Tst_bool                   isSetupSet;  /* setup header found ? */
		/* */
		Tst_uint32 vPackCnt;  /* amount of Theora video packets */
		/* */
		Tst_uint64 maxFrIx;   /* max. frame index */
	} Tst_contOgg_theora;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_VC_DRC_THEO_PROT_H */

/******************************************************************************/
