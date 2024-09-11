/******************************************************************************/
/* tag_vorbc-prot.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for Vorbis Tags                                      */
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

#ifndef HEAD_TAG_VORBC_PROT_H
#define HEAD_TAG_VORBC_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj-defs.h"
#include "src/includes/common/dynlist-defs.h"
#include "src/includes/common/string_mte-defs.h"
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/streamwr-defs.h"
#include "src/includes/tag/tag_basics-defs.h"
#include "src/includes/tag/tag_vorbc-defs.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected), Part I
*/
#if defined(SRC_TAG_VORBC_C) || defined(SRC_TAG_VORBC_FLDADD_C) || \
			defined(SRC_TAG_VORBC_ZRD_C) || defined(SRC_TAG_VORBC_ZWR_C) || \
			defined(SRC_TAG_VORBC_FNC_C)
	/** */
#	define ST_VORBC_TAG_PHEAD_SZ_MIN  20  /* min size of Pseudo Vorbis Tag Header */
#	define ST_VORBC_TAG_PHEAD         "VORBCOMM"  /* Pseudo Vorbis Tag identifier */
#	define ST_VORBC_TAG_PHEAD_SLEN    8
	/** maximum values */
#	define ST_VORBC_MAX_TAGSZ       0x00ffffff  /* max. tag size ^=16MB */
#	define ST_VORBC_MAX_TFLD_CNT    9999        /* max. fields per tag */
#	define ST_VORBC_MAX_TFLDID_CNT  99          /* max. fields per ID per tag */
#	define ST_VORBC_MAX_AF_FSZ_M    128         /* max. filesize for pictures in MB */
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_TAG_VORBC_C) || defined(SRC_TAG_VORBC_STCS_C) || \
			defined(SRC_TAG_VORBC_GS_C) || defined(SRC_TAG_VORBC_OPTS_C) || \
			defined(SRC_TAG_VORBC_FLDADD_C) || defined(SRC_TAG_VORBC_FLDGET_C) || \
			defined(SRC_TAG_VORBC_FNC_C) || \
			defined(SRC_TAG_VORBC_ZTIDY_C) || \
			defined(SRC_TAG_VORBC_ZRD_C) || defined(SRC_TAG_VORBC_ZWR_C) || \
			defined(SRC_TAG_VORBC_ZFDEB_C)
	/** Vorbis tag: complete field props, incl. ID String, Caption, ... */
	typedef struct {
		/* constant strings */
		char const     *pCIDstr;  /* ID string */
		char const     *pCCap;    /* Caption */
		/* */
		Tst_vorbc_frID id;        /* numerical ID */
		Tst_vorbc_frTp typ;       /* field type */
		/* */
		Tst_bool       retFromIte;  /* return this field from iterator ? */
		/* variable strings */
		char           *pVIDstr;  /* ID string */
	} Tst_vorbc_fldProp_intn;

	/** Vorbis tag: data of one field */
	/*** picture data */
	typedef struct {
		Tst_vorbc_picTp ptp;      /* picture type */
		Tst_uint32      imgWid;   /* image width, height, color depth */
		Tst_uint32      imgHei;
		Tst_uint32      imgDep;
		Tst_uint32      imgCUs;   /* number of used colors */
		Tst_str         *pMime;   /* MIME-Type */
		Tst_mtes_string desc;     /* description */
	} Tst_vorbc_fldAttrPict;
	/*** */
	typedef struct {
		/* data */
		Tst_mtes_strArr        dataSArr;     /* string array */
		Tst_binobj             dataPicBDat;  /* picture's binary data */
		/* attributes */
		Tst_vorbc_fldAttrPict  attrPic;   /* picture's attributes */
		/* */
		Tst_vorbc_fldProp_intn fldPrI;    /* field properties */
		Tst_uint32             fnr;       /* field nr. */
		Tst_bool               isOK;      /* is field OK (not empty) ? */
		Tst_uint32             uniqueIx;  /* unique field index */
		/* */
		Tst_bool wasRendered;  /* has field been rendered ? */
	} Tst_vorbc_fldData_intn;

	/** Vorbis tag: data of all fields */
	typedef struct {
		Tst_dynlist list;  /* array with all fields' data */
		/* */
		Tst_uint32 skipCnt;       /* amount of skipped fields */
		Tst_uint32 uniqueIxLast;  /* last used unique field index */
	} Tst_vorbc_flds;

	/** Vorbis: Options */
	typedef struct {
		Tst_basOpts basOpts;       /* basic options */
		Tst_bool    rdOnlyBasics;  /* read only tag basics ? */
	} Tst_vorbc_opts;

	/** Vorbis Tag */
	typedef struct {
		Tst_streamrd    *pStrrd;      /* current stream reader */
		Tst_streamwr    *pStrwr;      /* current stream writer */
		Tst_tagBasics   tbas;         /* basic tag info */
		Tst_str         *pFilen;      /* filename */
		/* */
		Tst_binobj      rawTagBDat;   /* raw tag data,
		                               *   will be set by st_vorbc_readTag_fromXXX() */
		/* */
		Tst_mtes_string vendor;       /* vendor string */
		Tst_mtes_string tagger;       /* tagging application string */
		/* */
		Tst_vorbc_flds  flds;         /* fields */
		/* */
		Tst_bool        wasEmbedded;  /* was this a embedded tag ? */
		/* */
		Tst_vorbc_opts  opts;         /* options */
	} Tst_vorbc_tag_intn;
#endif  /* protected */


/*
// Constants (protected), Part II
*/
#if defined(SRC_TAG_VORBC_C) || \
			defined(SRC_TAG_VORBC_FLDADD_C) || defined(SRC_TAG_VORBC_FLDGET_C) || \
			defined(SRC_TAG_VORBC_FNC_C) || defined(SRC_TAG_VORBC_GS_C) || \
			defined(SRC_TAG_VORBC_ZRD_C) || defined(SRC_TAG_VORBC_ZWR_C)
	/** Vorbis Tag Field Properties
	 **   defined in tag_vorbc-priv_flds.h      */
	extern const Tst_vorbc_fldProp_intn ST_VORBC_FLDPROPS[];
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_VORBC_PROT_H */

/******************************************************************************/
