/******************************************************************************/
/* tag_meta_stcs-prot.h         [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for Meta Tag structures                                */
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

#ifndef HEAD_TAG_META_STCS_PROT_H
#define HEAD_TAG_META_STCS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "tag_meta-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected), Part I
*/
#if defined(SRC_TAG_META_STCS_C)
	/** */
	void
	st_tagMeta_stc_rsetTag_prot(Tst_tagMeta_mt *pTag,
	                            const Tst_bool resetOpts,
	                            const Tst_bool resetStrrd,
	                            Tst_streamrd *pStrrd,
	                            const Tst_bool resetStrwr,
	                            Tst_streamwr *pStrwr,
	                            const Tst_bool resetFilen,
	                            const Tst_str *pFilen);
	/** */
	void
	st_tagMeta_stc_rsetTagArr_prot(Tst_tagMeta_mtArr *pMA,
	                               const Tst_bool resetStrrd,
	                               Tst_streamrd *pStrrd,
	                               const Tst_bool resetStrwr,
	                               Tst_streamwr *pStrwr,
	                               const Tst_bool resetFilen,
	                               const Tst_str *pFilen);
#endif  /* protected */


/*
// Functions (protected), Part II
*/
#if defined(SRC_TAG_META_C) || defined(SRC_TAG_META_GS_C) || \
			defined(SRC_TAG_META_STCS_C) || defined(SRC_TAG_META_FNC_C)
	/** */
	Tst_err st_tagMeta_stc_initTag(Tst_tagMeta_mt *pTag, const Tst_str *pFilen);
	void    st_tagMeta_stc_rsetTag(Tst_tagMeta_mt *pTag);
	void    st_tagMeta_stc_freeTag(Tst_tagMeta_mt *pTag);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_META_STCS_PROT_H */

/******************************************************************************/
