/******************************************************************************/
/* cont_ogg-common-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected common definitions for OGG containers etc.                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 29.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CONT_OGG_COMMON_PROT_H
#define HEAD_CONT_OGG_COMMON_PROT_H


ST_BEGIN_C_DECLS

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
			defined(SRC_AC_FLAC_ZRDA_C) || \
			defined(SRC_AC_FLAC_STCS_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || \
			defined(SRC_AC_VORB_STCS_C) || defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** Flac: Options */
	typedef struct {
		Tst_bool readAll;     /* read all frames ? */
		Tst_bool decodeFull;  /* fully decode frames (requires readAll) ? */
		Tst_bool checkCRCfh;  /* check CRC of frame headers ? */
		Tst_bool checkCRCfr;  /* check CRC of entire frames ? */
		Tst_bool checkMD5da;  /* check MD5 of decoded audio ? */
	} Tst_contOgg_flacOpts;

	/** Vorbis: Options */
	typedef struct {
		Tst_bool decodeFull;    /* fully decode frames ? */
		Tst_bool isDecoderSet;  /* are all decoding CBs set ? */
		/** for decoding samples */
		Tst_av_cbDecAud_hndNew                cbDecHndNew;
		Tst_av_cbDecAud_hndDel                cbDecHndDel;
		Tst_av_cbDecAud_setOutputFmt          cbDecSetOutpFmt;
		Tst_av_cbDecAud_openFeed              cbDecOpenFeed;
		Tst_av_cbDecAud_feedVrbHeader         cbDecFeedHd;
		Tst_av_cbDecAud_feedVrbAudioAndDecode cbDecFeedAudAndDec;
		Tst_av_cbDecAud_closeFeed             cbDecCloseFeed;
	} Tst_contOgg_vorbOpts;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_COMMON_PROT_H */

/******************************************************************************/
