/******************************************************************************/
/* mc_vorbcomm-prot.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for Vorbis Comments                                  */
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

#ifndef HEAD_MC_VORBCOMM_PROT_H
#define HEAD_MC_VORBCOMM_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj-defs.h>


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
			defined(SRC_MC_SKEL_ZRD_C) || \
			defined(SRC_MC_VORBCOMM_STCS_C) || defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_MC_VORBCOMM_GS_C) || \
			defined(SRC_VC_DRC_THEO_STCS_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** OGG Vorbis comment header */
	typedef struct {
		Tst_bool   isSet;
		Tst_uint32 packNr;  /* in which packet is this header ? */
		/* */
		Tst_int32  srcCmtBegPos;  /* pos. where original raw comment begins in packet */
		Tst_int32  srcCmtEndPos;  /* pos. where original raw comment ends in packet */
		Tst_binobj *pRawCmtBO;    /* binary object containing raw comment */
		Tst_uint32 elemCnt;       /* amount of comment elements */
		/* */
		Tst_str    *pVendor;  /* vendor string */
		/* */
		Tst_bool   wasModified;  /* has comment been modified ? */
	} Tst_contOgg_vorbCmtRaw_intn;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_MC_VORBCOMM_PROT_H */

/******************************************************************************/
