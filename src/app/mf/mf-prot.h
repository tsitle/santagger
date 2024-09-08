/******************************************************************************/
/* mf-prot.h                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for the 'main functions'                             */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MF_PROT_H
#define HEAD_MF_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/sys_file-defs.h>
#include <santag/common/streamrd-defs.h>
#include <santag/audvid/aud_mpeg1-defs.h>
#include <santag/audvid/cont_ogg-defs.h>
#include <santag/tag/tag_meta-defs.h>
/** */
#include "../app_defs.h"
#include "../cln/cln-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_MAIN_C) || \
			defined(SRC_MF_CNVMRG_C) || defined(SRC_MF_FNC_C) || \
			defined(SRC_MF_EDIT_C) || defined(SRC_MF_EXTR_C) || \
			defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C) || \
			defined(SRC_MF_SHOW_C) || defined(SRC_MF_SHOW_AV_C) || \
			defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_TIV2_C) || \
			defined(SRC_MF_OUTP_C) || \
			defined(SRC_MF_STCS_C)
	/** file formats */
	typedef enum {
		AST_MF_FFMT_TAGF,  /* tag file */
		AST_MF_FFMT_FLAC,  /* flac file (with tag) */
		AST_MF_FFMT_MPG1,  /* mpeg-1 file (with tags) */
		AST_MF_FFMT_OGG,   /* ogg file (with tag) */
		AST_MF_FFMT_NONE
	} Tast_mf_ffmt;

	/** content-types */
	enum {
		AST_MF_CTP_NONE  = 0,
		AST_MF_CTP_AUD   = 1,  /* audio */
		AST_MF_CTP_VID   = 2,  /* video */
		AST_MF_CTP_TAG   = 4,  /* tag */
		AST_MF_CTP_OTHER = 8   /* skeleton */
	};
#endif  /* protected */


/*
// Types (protected), Part I
*/
#if defined(SRC_MAIN_C) || \
			defined(SRC_MF_CNVMRG_C) || defined(SRC_MF_FNC_C) || \
			defined(SRC_MF_EDIT_C) || defined(SRC_MF_EXTR_C) || \
			defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C) || \
			defined(SRC_MF_SHOW_C) || defined(SRC_MF_SHOW_AV_C) || \
			defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_TIV2_C) || \
			defined(SRC_MF_OUTP_C) || \
			defined(SRC_MF_STCS_C)
	/** */
	typedef struct {
		Tst_cb_ana_msg cbAnaMsg;   /* for analize-mode */
		Tast_cln_a     *pCmdln;
	} Tast_mf_setOptsData;

	/** Audio/Tag file info */
	typedef struct {
		Tst_sys_fstc        fstc;       /* current filedescr. etc */
		Tst_streamrd        strrd;      /* current stream reader */
		const Tst_str       *pFilen;    /* pointer to current filename */
		/* audio */
		Tst_mpeg1_obj       audMpg1;    /* MPEG-1 audio info */
		Tst_contOgg_obj     avOgg;      /* OGG audio/video info */
		/* tags */
		Tst_tagMeta_mtArr   tagArr;     /* all tags */
		/* */
		Tast_mf_ffmt        ffmt;       /* file format */
		Tst_int32           contTypes;  /* content-types (or'd values) */
		/* */
		Tast_mf_setOptsData setOptsData;
		/* */
		Tst_cb_ana_msg cbAnaMsg;   /* for analize-mode */
	} Tast_mf_finfo;
#endif  /* protected */


/*
// Types (protected), Part I
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_CNVMRG_C) || \
			defined(SRC_MF_CNVMRG_C) || defined(SRC_MF_EDIT_C) || \
			defined(SRC_MF_STCS_C)
	/** for merging/editing tags */
	typedef struct {
		Tst_uint32 *pArr;
		Tst_uint32 cnt;
		Tst_uint32 sz;
	} Tast_mf_editInfo_uidArr;
	typedef struct {
		/* obsolete UIDs */
		Tast_mf_editInfo_uidArr obsUIDs;
		/* already written/skipped UIDs */
		Tast_mf_editInfo_uidArr usdUIDs;
		/* UIDs of main tags */
		Tst_uint32 uidMainIV1;
		Tst_uint32 uidMainIV2;
		Tst_uint32 uidMainAV2;
		Tst_uint32 uidMainVOR;
		/* */
		Tst_bool modEmbVorTag;  /* has a embedded Vorbis Tag been modified ? */
		/* */
		Tst_bool hasAudio;
		Tst_bool wroteAudio;
		Tst_bool wroteIV1;
		Tst_bool wroteIV2;
		Tst_bool wroteAV2;
		Tst_bool wroteVOR;
		/* */
		Tst_bool allwNonEmbTags;  /* allow non-embedded tags ? */
		/* allowed tags in current file type */
		Tast_cln_t_tagList_b(allwLst);
		/* tags to edit/write */
		Tast_cln_t_tagList_b(tagEdLst);
	} Tast_mf_editInfo;
#endif  /* protected */


/*
// Macros
*/
/** */
#define AST_MF_HASCONTTYPE(mac_pMF, mac_chkCTP)  \
			(((mac_pMF)->contTypes & (Tst_int32)mac_chkCTP) == (Tst_int32)mac_chkCTP)


ST_END_C_DECLS

#endif  /* HEAD_MF_PROT_H */

/******************************************************************************/
