/******************************************************************************/
/* tag_meta-prot.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for Meta Tags                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_META_PROT_H
#define HEAD_TAG_META_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/dynlist-defs.h"
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/streamwr-defs.h"
#include "src/includes/tag/tag_basics-defs.h"
#include "src/includes/tag/tag_apev2-defs.h"
#include "src/includes/tag/tag_id3v1-defs.h"
#include "src/includes/tag/tag_id3v2-defs.h"
#include "src/includes/tag/tag_vorbc-defs.h"
#include "src/includes/tag/tag_meta-defs.h"


ST_BEGIN_C_DECLS

/*
// Types (protected)
*/
#if defined(SRC_TAG_META_C) || defined(SRC_TAG_META_GS_C) || \
			defined(SRC_TAG_META_STCS_C) || defined(SRC_TAG_META_FNC_C)
	/** Meta Tag */
	typedef struct {
		Tst_tagBasics tbasOrg;     /* basic tag info from original tag */
		Tst_bool      tbasOrgSet;  /* has tbasOrg been set ? */
		Tst_str       *pFilen;     /* filename */
		/* */
		Tst_tagMeta_type tp;      /* type of tag */
		Tst_tagMeta_type tpOrg;   /* original type of tag */
		Tst_uint32       bsIx;    /* index of stream this tag belongs to */
		Tst_uint32       bsSIx;   /* sub index of stream this tag belongs to */
		Tst_uint32       uid;     /* unique ID of this tag */
		/* */
		Tst_id3v1_tag *pIV1;   /* ID3v1 Tag */
		Tst_id3v2_tag *pIV2;   /* ID3v2 Tag */
		Tst_apev2_tag *pAV2;   /* APEv2 Tag */
		Tst_vorbc_tag *pVOR;   /* Vorbis Tag */
		/* */
		Tst_uint32    uniqueIx;  /* unique tag index */
	} Tst_tagMeta_mt_intn;

	/** Meta Tag array */
	typedef struct {
		Tst_streamrd *pStrrd;   /* current stream reader */
		Tst_streamwr *pStrwr;   /* current stream writer */
		Tst_str      *pFilen;   /* filename */
		/* */
		Tst_dynlist list;       /* list of all tags */
		/* */
		Tst_uint32  uniqueIxLast;  /* last used unique tag index */
		/* callbacks for setting options */
		Tst_tagMeta_cbSetOpts_iv1 cbSetOptsIV1;
		void                      *pSetOptsDataIV1;
		Tst_tagMeta_cbSetOpts_iv2 cbSetOptsIV2;
		void                      *pSetOptsDataIV2;
		Tst_tagMeta_cbSetOpts_av2 cbSetOptsAV2;
		void                      *pSetOptsDataAV2;
		Tst_tagMeta_cbSetOpts_vor cbSetOptsVOR;
		void                      *pSetOptsDataVOR;
	} Tst_tagMeta_mtArr_intn;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_META_PROT_H */

/******************************************************************************/
