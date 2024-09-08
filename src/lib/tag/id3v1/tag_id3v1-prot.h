/******************************************************************************/
/* tag_id3v1-prot.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for ID3v1 (Extended) Tags                            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V1_PROT_H
#define HEAD_TAG_ID3V1_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/string_mte-defs.h>
#include <santag/common/streamrd-defs.h>
#include <santag/common/streamwr-defs.h>
#include <santag/tag/tag_basics-defs.h>
#include <santag/tag/tag_id3v1-defs.h>


ST_BEGIN_C_DECLS

/*
// Constants (protected), Part I
*/
#if defined(SRC_TAG_ID3V1_C) || defined(SRC_TAG_ID3V1_GS_C) || \
			defined(SRC_TAG_ID3V1_OPTS_C) || defined(SRC_TAG_ID3V1_STCS_C) || \
			defined(SRC_TAG_ID3V1_ZRD_C) || defined(SRC_TAG_ID3V1_ZWR_C) || \
			defined(SRC_TAG_ID3V1_FNC_C) || defined(SRC_TAG_ID3V1_ZFDEB_C) || \
			defined(SRC_TAG_ID3V1_FLDADD_C) || \
			defined(SRC_TAG_CONV_C) || defined(SRC_TAG_META_STCS_C)
	/** */
#	define ST_ID3V1_TAG_SZ      128   /* size of ID3v1.(0,1) Tag */
#	define ST_ID3V1_TAG_EXT_SZ  227   /* size of Extended ID3v1.(0,1) Tag */

	/** */
#	define ST_ID3V1_TAG_HEAD          "TAG"   /* ID3v1.(0,1) Tag */
#	define ST_ID3V1_TAG_HEAD_EXT      "TAG+"  /* ID3v1.(0,1) Extended Tag */
#	define ST_ID3V1_TAG_HEAD_SLEN      3
#	define ST_ID3V1_TAG_HEAD_EXT_SLEN  4

	/** data lengths */
	/*** */
#	define ST_ID3V1_TAG_LEN_ARTI  30
#	define ST_ID3V1_TAG_LEN_ALBU  30
#	define ST_ID3V1_TAG_LEN_TITL  30
#	define ST_ID3V1_TAG_LEN_TRKN   1
#	define ST_ID3V1_TAG_LEN_YEAR   4
#	define ST_ID3V1_TAG_LEN_COMM  30  /* 28 chars for COMM + 0x00 + TRKN */
#	define ST_ID3V1_TAG_LEN_GENR   1
	/*** */
#	define ST_ID3V1_TAG_EXT_LEN_ETITL  60
#	define ST_ID3V1_TAG_EXT_LEN_EARTI  60
#	define ST_ID3V1_TAG_EXT_LEN_EALBU  60
#	define ST_ID3V1_TAG_EXT_LEN_EGENR  30
#	define ST_ID3V1_TAG_EXT_LEN_SPEED   1
#	define ST_ID3V1_TAG_EXT_LEN_TIMES   6
#	define ST_ID3V1_TAG_EXT_LEN_TIMEE   6

	/** ID3v1 Extended Field IDs */
	typedef enum {
		ST_ID3V1E_FID_ETITL = 100,  /* (extended) Title */
		ST_ID3V1E_FID_EALBU = 101,  /* (extended) Album */
		ST_ID3V1E_FID_EARTI = 102,  /* (extended) Artist */
		ST_ID3V1E_FID_EGENR = 103,  /* (extended) Genre */
		ST_ID3V1E_FID_SPEED = 104,  /* Speed */
		ST_ID3V1E_FID_TIMES = 105,  /* Starttime */
		ST_ID3V1E_FID_TIMEE = 106,  /* Endtime */
		ST_ID3V1E_FID_NONE  = 107   /* none, ! this must be the last elem ! */
	} Tst_id3v1ext_frID;
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_TAG_ID3V1_C) || defined(SRC_TAG_ID3V1_GS_C) || \
			defined(SRC_TAG_ID3V1_OPTS_C) || defined(SRC_TAG_ID3V1_STCS_C) || \
			defined(SRC_TAG_ID3V1_ZRD_C) || defined(SRC_TAG_ID3V1_ZWR_C) || \
			defined(SRC_TAG_ID3V1_FNC_C) || defined(SRC_TAG_ID3V1_ZFDEB_C) || \
			defined(SRC_TAG_ID3V1_FLDADD_C) || \
			defined(SRC_TAG_CONV_C) || defined(SRC_TAG_META_STCS_C)
	/** ID3v1: Options */
	typedef struct {
		Tst_basOpts basOpts;       /* basic options */
		Tst_bool    rdOnlyBasics;  /* read only tag basics ? */
	} Tst_id3v1_opts;

	/** ID3v1 tag: frame properties, incl. ID String, Caption, ... */
	typedef struct {
		/* constant strings */
		char const        *pCIDstr;    /* ID string */
		char const        *pCCap;      /* Caption */
		/* */
		Tst_id3v1_frID    id;          /* numerical ID */
		Tst_id3v1ext_frID idExt;       /* numerical ID */
		Tst_id3v1_frTp    typ;         /* frame type */
		/* */
		Tst_bool          retFromIte;  /* return this field from iterator ? */
	} Tst_id3v1_fldProp_intn;

	/** ID3v1 tag frames: data of one frame */
	typedef struct {
		/* data */
		Tst_int32       dataIVal;  /* int value */
		Tst_mtes_string dataStr;   /* string value */
		/* */
		Tst_id3v1_fldProp_intn fldPrI;  /* frame properties,
		                                 * copy of corresponding elem
		                                 * from ST_ID3V1_FLDPROPS */
		/* errors/warnings */
		Tst_bool wereCorrd;  /* were values corrected ? */
		/* */
		Tst_bool isOK;  /* could frame be read and is not empty ? */
	} Tst_id3v1_fldData_intn;

	/** ID3v1 (Extended) Tag */
	typedef struct {
		Tst_streamrd  *pStrrd;  /* current stream reader */
		Tst_streamwr  *pStrwr;  /* current stream writer */
		Tst_tagBasics tbas;     /* basic tag info */
		Tst_str       *pFilen;  /* filename */
		/* tag fields */
		/** for normal ID3v1 Tags: */
		Tst_id3v1_fldData fdArt;  /* Artist */
		Tst_id3v1_fldData fdAlb;  /* Album */
		Tst_id3v1_fldData fdTit;  /* Title */
		Tst_id3v1_fldData fdCom;  /* Comment */
		Tst_id3v1_fldData fdTrk;  /* Tracknr, 1-255 */
		Tst_id3v1_fldData fdYea;  /* Year, 1500-2500 */
		Tst_id3v1_fldData fdGen;  /* Genre, 0-254 */
		/** for extended ID3v1 Tags: */
		Tst_id3v1_fldData fdExtArt;   /* extended Artist */
		Tst_id3v1_fldData fdExtAlb;   /* extended Album */
		Tst_id3v1_fldData fdExtTit;   /* extended Title */
		Tst_id3v1_fldData fdExtGen;   /* extended Genre */
		Tst_id3v1_fldData fdExtTiS;   /* Starttime */
		Tst_id3v1_fldData fdExtTiE;   /* Endtime */
		Tst_id3v1_fldData fdExtSpd;   /* Speed */
		Tst_bool          hasExtTag;  /* has extended ID3v1 tag ? */
		/* errors/warnings */
		Tst_bool wereCorrd;  /* were values corrected ? */
		/* options */
		Tst_id3v1_opts opts;
	} Tst_id3v1_tag_intn;
#endif  /* protected */


/*
// Constants (protected), Part II
*/
#if defined(SRC_TAG_ID3V1_C) || \
			defined(SRC_TAG_ID3V1_FLDADD_C) || defined(SRC_TAG_ID3V1_GS_C) || \
			defined(SRC_TAG_ID3V1_STCS_C) || defined(SRC_TAG_ID3V1_FNC_C)
	/** ID3v1 Tag Frame Properties
	 **   defined in tag_id3v1-pp_flds.h      */
	extern const Tst_id3v1_fldProp_intn ST_ID3V1_FLDPROPS[];
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V1_PROT_H */

/******************************************************************************/
