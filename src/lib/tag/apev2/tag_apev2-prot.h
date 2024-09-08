/******************************************************************************/
/* tag_apev2-prot.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for APEv2 Tags                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_APEV2_PROT_H
#define HEAD_TAG_APEV2_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/binobj-defs.h>
#include <santag/common/dynlist-defs.h>
#include <santag/common/string_mte-defs.h>
#include <santag/common/streamrd-defs.h>
#include <santag/common/streamwr-defs.h>
#include <santag/tag/tag_basics-defs.h>
#include <santag/tag/tag_apev2-defs.h>
/** */
#include "tag_apev2-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants - Part I  (protected)
*/
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_FNC_C) || \
			defined(SRC_TAG_APEV2_FLDADD_C) || \
			defined(SRC_TAG_APEV2_ZRD_C) || defined(SRC_TAG_APEV2_ZWR_C)
	/** */
#	define ST_APEV2_TAG_HEADFOOT_SZ    32          /* size of APEv2 Tag Header/Footer */
#	define ST_APEV2_TAG_HEADFOOT       "APETAGEX"  /* APEv2 Tag */
#	define ST_APEV2_TAG_HEADFOOT_SLEN  8
	/** tag/field header flags */
#	define ST_APEV2_TAG_FLAG_HASHEADER    (1 << 31)
#	define ST_APEV2_TAG_FLAG_HASNOFOOTER  (1 << 30)
#	define ST_APEV2_TAG_FLAG_ISHEADER     (1 << 29)
	/** maximum values */
#	define ST_APEV2_MAX_TAGSZ       ((1024 * 1024 * 16) + ST_APEV2_TAG_HEADFOOT_SZ)
#	define ST_APEV2_MAX_MSTR_CNT    999   /* max. strings per multi-string */
#	define ST_APEV2_MAX_TFLD_CNT    9999  /* max. fields per tag */
#	define ST_APEV2_MAX_TFLDID_CNT  99    /* max. fields per ID per tag */
#	define ST_APEV2_MAX_AF_FSZ_M    128   /* max. filesize for pictures in MB */
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_GS_C) || \
			defined(SRC_TAG_APEV2_OPTS_C) || defined(SRC_TAG_APEV2_STCS_C) || \
			defined(SRC_TAG_APEV2_FLDADD_C) || defined(SRC_TAG_APEV2_FLDGET_C) || \
			defined(SRC_TAG_APEV2_FNC_C) || \
			defined(SRC_TAG_APEV2_ZTIDY_C) || \
			defined(SRC_TAG_APEV2_ZRD_C) || defined(SRC_TAG_APEV2_ZWR_C) || \
			defined(SRC_TAG_APEV2_ZFDEB_C)
	/** APEv2 tag: complete field props, incl. ID String, Caption, ... */
	typedef struct {
		/* constant strings */
		char const     *pCIDstr;  /* ID string */
		char const     *pCCap;    /* Caption */
		/* */
		Tst_apev2_frID id;        /* numerical ID */
		Tst_apev2_frTp typ;       /* field type */
		/* */
		Tst_bool       retFromIte;  /* return this field from iterator ? */
		/* variable strings */
		char           *pVIDstr;  /* ID string */
	} Tst_apev2_fldProp_intn;

	/** APEv2 tag: data of one field */
	typedef struct {
		Tst_mtes_strArr dataSArr;    /* string array */
		Tst_binobj      dataBinDat;  /* binary data */
		/* */
		Tst_apev2_fldProp_intn fldPrI;    /* field properties */
		Tst_uint32             fnr;       /* field nr. */
		Tst_bool               isOK;      /* is field OK (not empty) ? */
		Tst_uint32             uniqueIx;  /* unique field index */
		/* */
		Tst_bool wasRendered;  /* has field been rendered ? */
	} Tst_apev2_fldData_intn;

	/** APEv2 tag: data of all fields */
	typedef struct {
		Tst_dynlist list;  /* fields */
		/* */
		Tst_uint32 uniqueIxLast;  /* last used unique field index */
		/* */
		Tst_uint32 skipCnt;   /* amount of skipped fields */
	} Tst_apev2_flds;

	/** APEv2 tag: Options */
	typedef struct {
		Tst_basOpts basOpts;       /* basic options */
		Tst_bool    rdOnlyBasics;  /* read only tag basics ? */
	} Tst_apev2_opts;

	/** APEv2 Tag */
	typedef struct {
		Tst_streamrd    *pStrrd;  /* current stream reader */
		Tst_streamwr    *pStrwr;  /* current stream writer */
		Tst_tagBasics   tbas;     /* basic tag info */
		Tst_str         *pFilen;  /* filename */
		/* */
		Tst_mtes_string tagger;   /* tagging application string */
		/* */
		Tst_apev2_opts  opts;     /* options */
		/* */
		Tst_apev2_flds  flds;     /* fields */
	} Tst_apev2_tag_intn;
#endif  /* protected */


/*
// Constants - Part II  (protected)
*/
#if defined(SRC_TAG_APEV2_C) || defined(SRC_TAG_APEV2_FNC_C) || \
			defined(SRC_TAG_APEV2_FLDADD_C) || defined(SRC_TAG_APEV2_FLDGET_C) || \
			defined(SRC_TAG_APEV2_GS_C) || \
			defined(SRC_TAG_APEV2_ZRD_C) || defined(SRC_TAG_APEV2_ZWR_C)
	/** APEv2 Tag Field Properties
	 **   defined in tag_apev2-priv_flds.h      */
	extern const Tst_apev2_fldProp_intn ST_APEV2_FLDPROPS[];
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_APEV2_PROT_H */

/******************************************************************************/
