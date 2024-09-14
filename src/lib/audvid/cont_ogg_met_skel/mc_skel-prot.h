/******************************************************************************/
/* mc_skel-prot.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for Ogg-Skeleton                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MC_SKEL_PROT_H
#define HEAD_MC_SKEL_PROT_H

/*
// Own-Includes
*/
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
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || defined(SRC_MC_SKEL_ZSTCS_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** OGG Skeleton Identification header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_uint16 versMaj;      /* Version major */
		Tst_uint16 versMin;      /* Version minor */
		Tst_uint64 presTimeNum;  /* Presentationtime numerator */
		Tst_uint64 presTimeDen;  /* Presentationtime denominator */
		Tst_uint64 baseTimeNum;  /* Basetime numerator */
		Tst_uint64 baseTimeDen;  /* Basetime denominator */
		Tst_str    utcTime[21];  /* UTC */
	} Tst_contOgg_skelIdent;

	/** OGG Skeleton bone */
	typedef struct {
		Tst_uint32 offsMsgHdFld;  /* Offset to message header fields */
		Tst_uint32 serial;        /* Serial number */
		Tst_uint32 hdPckCnt;      /* Number of header packets */
		Tst_uint64 granNum;       /* Granulerate numerator */
		Tst_uint64 granDen;       /* Granulerate denominator */
		Tst_uint64 baseGran;      /* Basegranule */
		Tst_uint32 preroll;       /* Preroll */
		Tst_byte   granShift;     /* Granuleshift */
		Tst_str    *pMsgHdFlds;   /* Message header fields */
	} Tst_contOgg_skelBone;

	/** OGG Skeleton bones array */
	typedef struct {
		Tst_contOgg_skelBone *pArr;
		Tst_uint32           sz;
		Tst_uint32           cnt;
	} Tst_contOgg_skelBoneArr;

	/** OGG Skeleton info */
	typedef struct {
		Tst_contOgg_skelIdent   ident;
		Tst_contOgg_skelBoneArr bones;
	} Tst_contOgg_skeleton;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_MC_SKEL_PROT_H */

/******************************************************************************/
