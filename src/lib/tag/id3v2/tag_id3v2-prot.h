/******************************************************************************/
/* tag_id3v2-prot.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for ID3v2 Tags                                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 10.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_ID3V2_PROT_H
#define HEAD_TAG_ID3V2_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj-defs.h"
#include "src/includes/common/dynlist-defs.h"
#include "src/includes/common/string_mte-defs.h"
#include "src/includes/common/streamrd-defs.h"
#include "src/includes/common/streamwr-defs.h"
#include "src/includes/utils/utils_fmt-defs.h"
#include "src/includes/tag/tag_basics-defs.h"
#include "src/includes/tag/tag_id3v2-defs.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected), Part I
*/
#if defined(SRC_TAG_ID3V2_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C) || defined(SRC_TAG_ID3V2_FLDADD_C) || \
			defined(SRC_TAG_ID3V2_FNC_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZRD2_C) || \
			defined(SRC_TAG_ID3V2_ZWR_C) || defined(SRC_TAG_ID3V2_ZTIDY_C) || \
			defined(SRC_TAG_ID3V2_ZSYNC_C)
	/** */
#	define ST_ID3V2_TAG_HEADFOOT_SZ    10      /* size of ID3v2 Header/Footer */
#	define ST_ID3V2_TAG_HEAD           "ID3"   /* ID3v2.(2,3,4) Tag Header */
#	define ST_ID3V2_TAG_FOOT           "3DI"   /* ID3v2.(2,3,4) Tag Footer */
#	define ST_ID3V2_TAG_HEADFOOT_SLEN  3
	/** maximum values */
	/*** max. value for conversion of Integer to Synchsafe-Integer */
#	define ST_ID3V2_MAX_SAFEINT      0x0fffffff  /* ^=256MB --> max tag size  */
	/*** max. value for frame size in ID3v2.2 tags */
#	define ST_ID3V2_MAX_TFLD_SZ_V2   0x00ffffff  /* ^=16MB */
	/*** */
#	define ST_ID3V2_MAX_TFLDID_CNT      99  /* max. fields per ID per tag */
#	define ST_ID3V2_MAX_TFLD_CNT      9999  /* max. fields per tag */
#	define ST_ID3V2_MAX_MSTR_CNT       999  /* max. strings per multi-string */
#	define ST_ID3V2_MAX_STRLEN_M        16  /* max. ISO length of strings in MB */
#	define ST_ID3V2_MAX_AF_FSZ_V2_M     15  /* v2.2:   max. fsz for att. files in MB */
#	define ST_ID3V2_MAX_AF_FSZ_V34_M   128  /* v2.3/4:    ------- " -------          */
	/*** min/max sizes for compressing frames */
#	define ST_ID3V2_CMPR_MIN  1024
#	define ST_ID3V2_CMPR_MAX  (1024 * 256)  /* don't make this too big */
	                                        /*   --> consumes much RAM */
	/** */
#	define ST_ID3V2_MIMEDEF  "application/octet-stream"
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_TAG_ID3V2_C) || \
			defined(SRC_TAG_ID3V2_GS_C) || defined(SRC_TAG_ID3V2_STCS_C) || \
			defined(SRC_TAG_ID3V2_OPTS_C) || \
			defined(SRC_TAG_ID3V2_FLDADD_C) || defined(SRC_TAG_ID3V2_FLDGET_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C) || defined(SRC_TAG_ID3V2_FNC_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZRD2_C) || \
			defined(SRC_TAG_ID3V2_ZWR_C) || defined(SRC_TAG_ID3V2_ZTIDY_C) || \
			defined(SRC_TAG_ID3V2_ZFDEB_C)
	/** ID3v2 Tag: Header Flags */
	typedef struct {
		Tst_bool hasExtHd;   /* has Extended Header ? */
		Tst_bool isExp;      /* is Experimental Tag ? */
		Tst_bool isUnsy;     /* is Unsyched ? */
		Tst_bool hasFoot;    /* has Footer ? */
		Tst_bool isCompr;    /* is Compressed ? */
		Tst_bool hasCDM;     /* ID3v2.2.1: has CDM frame ? */
	} Tst_id3v2_thdFlags;

	/** ID3v2 Tag: Frame Header Flags */
	typedef struct {
		Tst_bool doV34tap;    /* do Tag alter preservation ? */
		Tst_bool doV34fap;    /* do File alter preservation ? */
		Tst_bool isV34ro;     /* is Read only ? */
		Tst_bool isV34compr;  /* is Compressed ? */
		Tst_bool isV34enc;    /* is Encrypted ? */
		Tst_bool isV34gi;     /* is Grouping identity ? */
		Tst_bool isV4unsy;    /* is Unsynchronized ? */
		Tst_bool hasV4dli;    /* has Data length indicator ? */
	} Tst_id3v2_frhdFlags;

	/** ID3v2 tag: for output of frames */
	typedef struct {
		Tst_id3v2_fldData **ppFldArr;  /* array of pointers to frames */
		Tst_uint32        cnt;         /* amount of used pointers */
		Tst_uint32        sz;          /* size of ppFldArr */
	} Tst_id3v2_fieldsOutp;

	/** for ID3v2 Picture types */
	typedef struct {
		const Tst_byte        bin;      /* binary value */
		const char            *pCap;    /* caption (e.g. "32x32 file icon") */
		const char            *pShort;  /* short caption (e.g. "icon-32") */
		const Tst_id3v2_picTp ptpE;     /* enum value */
	} Tst_id3v2_picTp_arrElem;

	/** ID3v2 tag: frame properties, incl. ID String, Caption, ... */
	typedef struct {
		/* constant strings */
		char const     *pCID02str;   /* ID string for ID3v2.2 */
		char const     *pCID34str;   /* ID string for ID3v2.(3,4) */
		char const     *pCCap;       /* Caption */
		/* */
		Tst_byte       verFl;        /* version flag -->frame valid for versions x,y,z
		                              *   1:v2.2, 2:v2.3, 4:v2.4
		                              *   3:v2.2/3, 6:v2.3/4, 7:v2.2/3/4  */
		Tst_id3v2_frID id;           /* numerical ID */
		Tst_id3v2_frTp typ;          /* frame type */
		/* */
		Tst_bool       retFromIte;   /* return this field from iterator ? */
		/* */
		char const     *pIDstr;      /* pointer to currently used ID string */
		/* variable strings */
		char           vid02str[5];  /* ID string for ID3v2.2 */
		char           vid34str[5];  /* ID string for ID3v2.(3,4) */
	} Tst_id3v2_fldProp_intn;

	/** ID3v2 tag frames: order to read/write data in */
#	define Tst_id3v2_fldd_rwordPArr  Tst_id3v2_fldd_rword_typ*

	/** ID3v2 tag frames: data of one frame */
	typedef struct {
		/* data */
		Tst_uint32          dataUVal;       /* unsigned int value */
		Tst_byte            dataBVal;       /* byte value */
		Tst_binobj          dataBinDat;     /* binary data */
		Tst_mtes_strArr     dataSArr;       /* string array */
		/* attributes */
		Tst_id3v2_sltTSF    attrSltTSF;     /* SYLT: time stamp format */
		Tst_id3v2_sltCTp    attrSltCTP;     /* SYLT: content type */
		Tst_str             *pAttrGeoMime;  /* mime-type of GEOB */
		Tst_mtes_string     attrGeoFn;      /* filename of GEOB */
		Tst_id3v2_picTp     attrPicTp;      /* picture type */
		Tst_utilsFmt_picFmt attrPicFmt;     /* image format as enum */
		Tst_str             attrLang[4];    /* language */
		Tst_uint32          attrDLI;        /* uncompr. data length indicator */
		Tst_mtes_string     attrDesc;       /* short description */
		/* */
		Tst_id3v2_fldProp_intn fldPrI;  /* frame properties,
		                                 * copy of corresponding elem
		                                 * from ST_ID3V2_FLDPROPS */
		Tst_uint32 fnr;  /* since one frame ID can be used more
		                  * than once, we need a number to identify
		                  * the specific frame with the ID in fldPr */
		/* */
		Tst_id3v2_fldErrs errs;  /* errors in this frame */
		/* for ast_mf_sw_showTagIV2() */
		Tst_bool wasDispld;      /* has field already been displayed ? */
		/* for ST_TIV2__rend_allFlds() */
		Tst_bool wasRendered;    /* has field been rendered ? */
		/* for st_id3v2_fnc_sortFields_byPrio() */
		Tst_bool wasAddedToPrio; /* has field been handled ? */
		/* */
		Tst_bool   isOK;      /* could frame be read and is not empty ? */
		Tst_uint32 uniqueIx;  /* unique field index */
		/* */
		Tst_uint32 frErrCnt;  /* amount of errors/warnings */
	} Tst_id3v2_fldData_intn;

	/** ID3v2 tag: data of all frames and errors */
	typedef struct {
		Tst_dynlist list;  /* all fields' data */
		/* */
		Tst_uint32  skipCnt;       /* amount of skipped frames */
		Tst_uint32  errCnt;        /* total amount of errors in frames */
		Tst_uint32  uniqueIxLast;  /* last used unique field index */
	} Tst_id3v2_fields;

	/** ID3v2: Options */
	typedef struct {
		Tst_basOpts basOpts;  /* basic options */
		/* */
		Tst_bool    rdOnlyBasics;  /* read only tag basics ? */
		Tst_byte    saveAsVers;    /* major version to save tag as, 2/3/4 */
		Tst_bool    unsync;        /* unsync tags/frames ? */
		Tst_bool    compress;      /* compress big frames ? */
		Tst_bool    alwLinkedInpFiles;  /* allow sym- and hardlinks as input-files ? */
		/* callbacks */
		Tst_id3v2_cbZLib_decompress cbZLibDecmpr;  /* for zlib decompression */
		Tst_id3v2_cbZLib_compress   cbZLibCmpr;    /* for zlib compression */
	} Tst_id3v2_opts;

	/** ID3v2 Tag: Main Structure */
	typedef struct {
		Tst_streamrd       *pStrrd;   /* current stream reader */
		Tst_streamwr       *pStrwr;   /* current stream writer */
		Tst_tagBasics      tbas;      /* basic tag info */
		Tst_str            *pFilen;   /* filename */
		/* */
		Tst_id3v2_thdFlags flagsTHd;  /* tag header flags */
		Tst_id3v2_fields   flds;      /* frames */
		Tst_id3v2_tagErrs  tgErrs;    /* errors/warnings in tag header/body */
		Tst_uint32         tgErrCnt;  /* amount of errors/warnings in tag header etc. */
		/* */
		Tst_byte           verMaj;    /* Major Version Nr., e.g. 03 */
		Tst_byte           verMin;    /* Minor Version Nr., e.g. 00 */
		Tst_uint32         ehdSz;     /* ext header size */
		Tst_uint32         paddLen;   /* size of padding after tag */
		/* */
		Tst_mtes_string    tagger;    /* tagging application string */
		/* */
		Tst_id3v2_opts     opts;      /* options */
	} Tst_id3v2_tag_intn;
#endif  /* protected */


/*
// Constants (protected), Part II
*/
#if defined(SRC_TAG_ID3V2_C) || \
			defined(SRC_TAG_ID3V2_FLDADD_C) || \
			defined(SRC_TAG_ID3V2_FNC_C) || defined(SRC_TAG_ID3V2_GS_C) || \
			defined(SRC_TAG_ID3V2_STCS_C) || \
			defined(SRC_TAG_ID3V2_ZCHK_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || \
			defined(SRC_TAG_ID3V2_ZWR_C) || defined(SRC_TAG_ID3V2_ZFDEB_C)
	/** ID3v2 Tag Frame Properties
	 **   defined in tag_id3v2-priv_flds.h      */
	extern const Tst_id3v2_fldProp_intn ST_ID3V2_FLDPROPS[];

	/** ID3v2 Tag Frame Properties - to be ignored
	 **   defined in tag_id3v2-priv_flds.h      */
	extern const Tst_id3v2_fldProp_intn ST_ID3V2_FLDPROPS_IGN[];
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_PROT_H */

/******************************************************************************/
