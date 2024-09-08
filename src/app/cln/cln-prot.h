/******************************************************************************/
/* cln-prot.h                     [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for program options parsing                          */
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

#ifndef HEAD_CLN_PROT_H
#define HEAD_CLN_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/dynlist-defs.h>
#include <santag/utils/utils_fmt-defs.h>
#include <santag/audvid/aud_mpeg1-defs.h>
#include <santag/tag/tag_id3v2-defs.h>
/** */
#include "../app_defs.h"

/*
// System-Includes
*/
#if defined(SRC_CLN_FNC_C) || defined(SRC_CLN_PARGS_C) || \
			defined(SRC_CLN_HNDARG_C)
#	ifndef _GNU_SOURCE
#		define DO_UNDEF_GNU_SRC  1
#		define _GNU_SOURCE
#	else
#		define DO_UNDEF_GNU_SRC  0
#	endif
#	include <getopt.h>      /* getopt_long(), struct option, ...  */
#	if (DO_UNDEF_GNU_SRC == 1)
#		undef _GNU_SOURCE
#	endif
#	undef DO_UNDEF_GNU_SRC
#endif


ST_BEGIN_C_DECLS

/*
// Constants (protected), Part I
*/
#if defined(SRC_CLN_PARGS_C) || defined(SRC_CLN_FNC_C) || \
			defined(SRC_CLN_SETDEFS_C) || defined(SRC_CLN_PSVT_C) || \
			defined(SRC_CLN_HNDARG_C) || defined(SRC_CLN_HNDHELP_C) || \
			defined(SRC_CLN_HNDHELP_FNC_C) || defined(SRC_CLN_STCS_C) || \
			defined(SRC_CLN_AF_C) || \
			defined(SRC_MF_CNVMRG_C) || defined(SRC_MF_FNC_C) || \
			defined(SRC_MF_EDIT_C) || defined(SRC_MF_EXTR_C) || \
			defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C) || \
			defined(SRC_MF_SHOW_C) || defined(SRC_MF_SHOW_AV_C) || \
			defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_TIV2_C) || \
			defined(SRC_MF_OUTP_C) || \
			defined(SRC_MF_STCS_C) || \
			defined(SRC_MAIN_C)
	/** amount of elements in Tast_cln_t_tagListIx */
#	define AST_CLN_T_TAGLIST_CNT  5

	/** indices of tag types in <taglist> i.e. Tast_cln_t_tagList_b */
	typedef enum {
		AST_CLN_T_TAGLIST_IV1 = 0,  /* ID3v1 */
		AST_CLN_T_TAGLIST_IV2 = 1,  /* ID3v2 */
		AST_CLN_T_TAGLIST_AV2 = 2,  /* APEv2 */
		AST_CLN_T_TAGLIST_VOR = 3,  /* Vorbis */
		AST_CLN_T_TAGLIST_MRG = 4   /* merged */
	} Tast_cln_t_tagListIx;
#endif  /* protected */


/*
// Types (protected), Part I
*/
#if defined(SRC_CLN_PARGS_C) || defined(SRC_CLN_PSVT_C) || \
			defined(SRC_CLN_HNDARG_C) || defined(SRC_CLN_HNDHELP_C) || \
			defined(SRC_CLN_HNDHELP_FNC_C) || defined(SRC_CLN_FNC_C) || \
			defined(SRC_CLN_STCS_C) || defined(SRC_CLN_AF_C) || \
			defined(SRC_CLN_SETDEFS_C)
	/** */
	typedef struct option Tast_cln_longopts;
	/** */
	typedef struct {
		Tst_int32  lev;
		char const *pCap;
		char const *pCapSh;
		char const *pDesc;
	} Tast_cln_t_verblevList_capDesc;
	/** */
	typedef struct {
		Tst_int32  tlix;
		char const *pCap;
		char const *pCapSh;
		char const *pDesc;
	} Tast_cln_t_tagList_capDesc;
	/** */
	typedef struct {
		char const *pCap;
		char const *pDesc;
	} Tast_cln_t_tiv2optList_capDesc;
#endif  /* protected */


/*
// Constants (protected), Part II
*/
#if defined(SRC_CLN_PARGS_C) || defined(SRC_CLN_PSVT_C) || \
			defined(SRC_CLN_HNDARG_C) || defined(SRC_CLN_HNDHELP_C) || \
			defined(SRC_CLN_HNDHELP_FNC_C) || defined(SRC_CLN_FNC_C) || \
			defined(SRC_CLN_STCS_C) || defined(SRC_CLN_AF_C) || \
			defined(SRC_CLN_SETDEFS_C)
	/** */
#	define AST_CLN_MAX_ARGS     512  /* max. amount of command line args */
#	define AST_CLN_MAX_ARGLEN  8192  /* max. length of one argument */

	/** */
#	define AST_CLN__CONCAT2(mac_s1, mac_s2)  mac_s1##_##mac_s2
#	define AST_CLN__CONCAT(mac_s1, mac_s2)   AST_CLN__CONCAT2(mac_s1, mac_s2)
#	define AST_CLN__QUOTE2(mac_s1)           #mac_s1
#	define AST_CLN__QUOTE(mac_s1)            AST_CLN__QUOTE2(mac_s1)

	/** commands */
	/*** special commands */
#	define AST_CLN_CMD_HLP_SO       'h'
#	define AST_CLN_CMD_HLP_LO       "help"
#	define AST_CLN_CMD_INF_LO       "info"
#	define AST_CLN_CMD_VER_LO       "version"
#	define AST_CLN_CMD_LIC_LO       "license"
#	define AST_CLN_CMD_EXA_LO       "examples"
	/*** readonly-commands */
#	define AST_CLN_CMD_RD_SO        'r'
#	define AST_CLN_CMD_RD_LO        "read"
#	define AST_CLN_CMD_ANA_LO       "ana"
#	define AST_CLN_CMD_DECAUD_LO    "dec"
	/*** tag-extr-commands */
#	define AST_CLN_CMD_EXTR_PIC_LO  "extr-pic"
#	define AST_CLN_CMD_EXTR_GEO_LO  "extr-geo"
#	define AST_CLN_CMD_EXTR_BD_LO   "extr-bd"
#	define AST_CLN_CMD_EXTR_TAG_LO  "extr-tag"
	/*** tag-edit-commands */
#	define AST_CLN_CMD_BND_SO       'b'
#	define AST_CLN_CMD_BND_LO       "band"
#	define AST_CLN_CMD_ALB_SO       'a'
#	define AST_CLN_CMD_ALB_LO       "album"
#	define AST_CLN_CMD_SNG_SO       's'
#	define AST_CLN_CMD_SNG_LO       "song"
#	define AST_CLN_CMD_TRK_SO       't'
#	define AST_CLN_CMD_TRK_LO       "track-nr"
#	define AST_CLN_CMD_TRK_TOT_LO   "track-tot"
#	define AST_CLN_CMD_DSC_SO       'd'
#	define AST_CLN_CMD_DSC_LO       "disc-nr"
#	define AST_CLN_CMD_DSC_TOT_LO   "disc-tot"
#	define AST_CLN_CMD_YEA_SO       'y'
#	define AST_CLN_CMD_YEA_LO       "year"
#	define AST_CLN_CMD_COM_SO       'c'
#	define AST_CLN_CMD_COM_LO       "comment"
#	define AST_CLN_CMD_GEN_SO       'g'
#	define AST_CLN_CMD_GEN_LO       "genre"
#	define AST_CLN_CMD_ADD_PIC_LO   "add-pic"
#	define AST_CLN_CMD_ADD_GEO_LO   "add-geo"
#	define AST_CLN_CMD_MOD_PIC_LO   "mod-pic"
#	define AST_CLN_CMD_MOD_GEO_LO   "mod-geo"
#	define AST_CLN_CMD_REM_PIC_LO   "rem-pic"
#	define AST_CLN_CMD_REM_GEO_LO   "rem-geo"
#	define AST_CLN_CMD_FADD_LO      "fadd"
#	define AST_CLN_CMD_FSET_LO      "fset"
#	define AST_CLN_CMD_DSET_LO      "dset"
#	define AST_CLN_CMD_REWR_LO      "rewr"
	/*** tag-conv-commands */
#	define AST_CLN_CMD_CONV_LO      "conv"
	/*** tag-rem-commands */
#	define AST_CLN_CMD_REM_TAG_LO   "rem-tag"

	/** options */
	/*** */
#	define AST_CLN_OPT_PRT_SO       'p'
#	define AST_CLN_OPT_PRT_LO       "pretend"
#	define AST_CLN_OPT_SWSTAT_SO    'v'
#	define AST_CLN_OPT_VERBLEV_LO   "lev"
#	define AST_CLN_OPT_QUIET_SO     'q'
	/*** */
#	define AST_CLN_OPT_QSCAN_LO     "qscan"
	/*** */
#	define AST_CLN_OPT_ALLWLNKS_LO     "allow-links"
#	define AST_CLN_OPT_DISALLWLNKS_LO  "disallow-links"
	/*** */
#	define AST_CLN_OPT_DISP_ALL_LO  "disp-all"
#	define AST_CLN_OPT_DISP_DET_LO  "disp-det"
#	define AST_CLN_OPT_DISP_FS_LO   "disp-fs"
#	define AST_CLN_OPT_DISP_ISO_LO  "disp-iso"
#	define AST_CLN_OPT_DISP_U8_LO   "disp-u8"
	/*** */
#	define AST_CLN_OPT_EBS_LO       "ebs"
	/*** */
#	define AST_CLN_OPT_TAG_SW_LO    "tag-sw"
#	define AST_CLN_OPT_TAG_ED_LO    "tag-ed"
	/*** */
#	define AST_CLN_OPT_FRC_MP1_LO   "force-mp1"
#	define AST_CLN_OPT_FRC_MP2_LO   "force-mp2"
#	define AST_CLN_OPT_FRC_MP3_LO   "force-mp3"
#	define AST_CLN_OPT_FRC_AV1_LO   "force-av1"
#	define AST_CLN_OPT_FRC_AV2_LO   "force-av2"
#	define AST_CLN_OPT_FRC_AV25_LO  "force-av25"
	/*** */
#	define AST_CLN_OPT_BPS_LO       "bps"
	/*** */
#	define AST_CLN_OPT_TIV2_LO      "tiv2"
	/*** */
#	define AST_CLN_OPT_PIC_TP_LO    "pic-tp"
#	define AST_CLN_OPT_PIC_FM_LO    "pic-fm"
#	define AST_CLN_OPT_PIC_DE_LO    "pic-de"
#	define AST_CLN_OPT_PIC_FN_LO    "pic-fn"
#	define AST_CLN_OPT_GEO_DE_LO    "geo-de"
#	define AST_CLN_OPT_GEO_MI_LO    "geo-mi"
#	define AST_CLN_OPT_GEO_FN_LO    "geo-fn"
	/*** */
#	define AST_CLN_OPT_OUTDIR_LO    "outdir"

	/** arguments */
	/*** general arguments */
#	define AST_CLN_ARG_HLP           "help"
#	define AST_CLN_ARG_HLPSH         "h"
#	define AST_CLN_ARG_HLPVALS       "hv"
#	define AST_CLN_ARG_AUTO          "auto"
#	define AST_CLN_ARG_AUTOSH        "au"
#	define AST_CLN_ARG_ALL           "all"
#	define AST_CLN_ARG_ALLSH         "a"
#	define AST_CLN_ARG_NONE          "none"
#	define AST_CLN_ARG_NONESH        "n"
	/*** tag types */
#	define AST_CLN_ARG_TIV1          "id3v1"
#	define AST_CLN_ARG_TIV1SH        "iv1"
#	define AST_CLN_ARG_TIV2          "id3v2"
#	define AST_CLN_ARG_TIV2SH        "iv2"
#	define AST_CLN_ARG_TAV2          "apev2"
#	define AST_CLN_ARG_TAV2SH        "av2"
#	define AST_CLN_ARG_TVOR          "vorbc"
#	define AST_CLN_ARG_TVORSH        "vor"
#	define AST_CLN_ARG_TMRG          "merg"
#	define AST_CLN_ARG_TMRGSH        "mrg"
	/*** decoding output formats */
#	define AST_CLN_ARG_DECAUD_WAV    "wav"
#	define AST_CLN_ARG_DECAUD_AIFF   "aiff"
	/*** ID3v2 options */
#	define AST_CLN_ARG_TIV2O_CMPR    "cmpr"
#	define AST_CLN_ARG_TIV2O_NOCMPR  "nocmpr"
#	define AST_CLN_ARG_TIV2O_UNSY    "unsy"
#	define AST_CLN_ARG_TIV2O_NOUNSY  "nounsy"
#	define AST_CLN_ARG_TIV2O_V2      "v2"
#	define AST_CLN_ARG_TIV2O_V3      "v3"
#	define AST_CLN_ARG_TIV2O_V4      "v4"

	/** special value type format names */
#	define AST_CLN_SVT_FMT_STR          "str"
#	define AST_CLN_SVT_FMT_STRSHORT     "strshort"
#	define AST_CLN_SVT_FMT_NUM          "num"
#	define AST_CLN_SVT_FMT_NUMRANGE     "numrange"
#	define AST_CLN_SVT_FMT_NUMLIST      "numlist"
#	define AST_CLN_SVT_FMT_MIME         "mime"
#	define AST_CLN_SVT_FMT_FILEN        "filen"
#	define AST_CLN_SVT_FMT_PICTP        "pictp"
#	define AST_CLN_SVT_FMT_PICFM        "picfm"
#	define AST_CLN_SVT_FMT_DESC         "desc"
#	define AST_CLN_SVT_FMT_VLEV         "vlev"
#	define AST_CLN_SVT_FMT_VLEVLIST     "vlevlist"
#	define AST_CLN_SVT_FMT_BS           "bs"
#	define AST_CLN_SVT_FMT_BSLIST       "bslist"
#	define AST_CLN_SVT_FMT_POS          "pos"
#	define AST_CLN_SVT_FMT_TAG          "tag"
#	define AST_CLN_SVT_FMT_TAGLONG      "taglong"
#	define AST_CLN_SVT_FMT_TAGSHORT     "tagshort"
#	define AST_CLN_SVT_FMT_TAGLIST      "taglist"
#	define AST_CLN_SVT_FMT_TAGPAIR      "tagpair"
#	define AST_CLN_SVT_FMT_FLDNAME      "fldname"
#	define AST_CLN_SVT_FMT_FNSHORT      "fnshort"
#	define AST_CLN_SVT_FMT_FNLONG       "fnlong"
#	define AST_CLN_SVT_FMT_FLDTYPE      "fldtype"
#	define AST_CLN_SVT_FMT_VERFL        "verfl"
#	define AST_CLN_SVT_FMT_FLDADD       "fldadd"
#	define AST_CLN_SVT_FMT_FLDSET       "fldset"
#	define AST_CLN_SVT_FMT_DSCSET       "dscset"
#	define AST_CLN_SVT_FMT_TIV2OPT      "tiv2opt"
#	define AST_CLN_SVT_FMT_TIV2OPTLIST  "tiv2optlist"

	/** these are implemented in cln_pargs-pp.h */
	extern const Tast_cln_t_verblevList_capDesc AST_CLN_VERBLIST_CAPDESC[];
	extern const Tast_cln_t_tagList_capDesc     AST_CLN_TAGLIST_CAPDESC[];
	extern const Tast_cln_t_tiv2optList_capDesc AST_CLN_TIV2OPTLIST_CAPDESC[];

	/** defaults, be careful when using/changing these... */
	/*** */
#	define AST_CLN_DEF_TAG_SW_STR    AST_CLN_ARG_TMRG
#	define AST_CLN_DEF_TAG_SW_VAL    AST_CLN_T_TAGLIST_MRG
#	define AST_CLN_DEF_TAG_WR_STR    AST_CLN_ARG_AUTO
#	define AST_CLN_DEF_TAG_WR_VAL    /* doesn't have one */
#	define AST_CLN_DEF_EBS_STR       AST_CLN_ARG_ALL
#	define AST_CLN_DEF_EBS_VAL       /* doesn't have one */
#	define AST_CLN_DEF_BPS_STR       /* doesn't have one */
#	define AST_CLN_DEF_BPS_VAL       0
#	define AST_CLN_DEF_TIV2_STR      "v3,nocmpr,nounsy"
#	define AST_CLN_DEF_TIV2_VAL      /* doesn't have one */
#	define AST_CLN_DEF_TIV2_VAL_SAV  3  /* save as version */
#	define AST_CLN_DEF_TIV2_VAL_AWC  ST_B_FALSE  /* allow compr. */
#	define AST_CLN_DEF_TIV2_VAL_AWU  ST_B_FALSE  /* allow unsynchr. */
#	define AST_CLN_DEF_PIC_TP_STR    /* doesn't have one */
#	define AST_CLN_DEF_PIC_TP_VAL    ST_ID3V2_PT_COVFRO
#	define AST_CLN_DEF_PIC_FM_STR    AST_CLN_ARG_AUTO
#	define AST_CLN_DEF_PIC_FM_VAL    /* doesn't have one */
#endif  /* protected */


/*
// Types (protected), Part II
*/
#if defined(SRC_CLN_PARGS_C) || defined(SRC_CLN_FNC_C) || \
			defined(SRC_CLN_AF_C) || defined(SRC_CLN_PSVT_C) || \
			defined(SRC_CLN_SETDEFS_C) || defined(SRC_CLN_STCS_C) || \
			defined(SRC_CLN_HNDARG_C) || defined(SRC_CLN_HNDHELP_C) || \
			defined(SRC_CLN_HNDHELP_FNC_C) || \
			defined(SRC_MF_CNVMRG_C) || defined(SRC_MF_FNC_C) || \
			defined(SRC_MF_EDIT_C) || defined(SRC_MF_EXTR_C) || \
			defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C) || \
			defined(SRC_MF_SHOW_C) || defined(SRC_MF_SHOW_AV_C) || \
			defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_TIV2_C) || \
			defined(SRC_MF_OUTP_C) || \
			defined(SRC_MF_STCS_C) || \
			defined(SRC_MAIN_C)
	/** for long options cache */
	typedef struct {
		char const *pOptName;
		Tst_int32  slen;
	} Tast_cln_longoptCache;

	/** for <taglist> */
#	define Tast_cln_t_tagList_b(mac_varName)   Tst_bool mac_varName[AST_CLN_T_TAGLIST_CNT]
#	define Tast_cln_t_tagList_pb(mac_varName)  Tst_bool *mac_varName

	/** for <tagpair> */
	typedef struct {
		Tast_cln_t_tagList_b(src);
		Tast_cln_t_tagListIx dst;
	} Tast_cln_t_tagPair;

	/** for <fldadd>/<fldset>/<dscset> */
	typedef struct {
		Tast_cln_t_tagListIx ttp;            /* tag type where field name comes from */
		Tst_str              *pFldname;      /* field name */
		Tst_int32            fldnr;          /* field's number */
		Tst_str              *pValueStr;     /* field's value as string */
		Tst_int32            valueInt;       /* field's value as integer */
		Tst_bool             isIntStr;       /* does pValueStr contain an integer ? */
		/* only for <fldadd>/<fldset>: */
		Tst_bool             fldDoAddOrSet;  /* shall we add or set this field ? */
	} Tast_cln_t_fldDscAddSet;

	/* for <bs> */
	typedef struct {
		Tst_uint32 ix;   /* edit bitstream x.y */
		Tst_uint32 six;
	} Tast_cln_t_ebs;

	/* for <tiv2opt> */
	typedef struct {
		Tst_uint32 saveAsVers;   /* version to save ID3v2 Tags as */
		Tst_bool   allwCompr;    /* allow compression ? */
		Tst_bool   allwUnsynch;  /* allow unsynchronization ? */
	} Tast_cln_t_tiv2opt;

	/** for attached files: pic/geo */
	typedef struct {
		Tst_str             *pFn;      /* filename */
		Tst_str             *pDesc;    /* description */
		Tst_str             *pMime;    /* MIME-Type for GEOB */
		Tst_id3v2_picTp     picTp;     /* picture type */
		Tst_bool            isPTPset;  /* is picTp set ? */
		Tst_utilsFmt_picFmt picFm;     /* picture format */
		Tst_bool            isPFMset;  /* is picFm set ? */
		Tst_uint32          modFNr;    /* framenr. of AF to modify */
		/* */
		Tst_bool isPicOrGeo;  /* above data belongs to APIC or GEOB ? */
		Tst_bool addOrMod;    /* use this data for adding or modifying an AF ? */
		Tst_bool isOK;        /* is this struct's data OK ? */
	} Tast_cln_attFile;

	typedef struct {
		Tast_cln_attFile *pArr;
		Tst_uint32       sz;   /* size of pArr */
		Tst_uint32       cnt;  /* amount of elements in pArr */
	} Tast_cln_attFileArr;

	/** for display options from cmdline */
	typedef struct {
		Tst_bool allFlds;     /* display all fields ? */
		Tst_bool fullstr;     /* display full strings ? */
		Tst_bool fldDetails;  /* display field details ? */
		Tst_bool asISOorU8;   /* display strings in ISO- or UTF8-encoding ? */
		Tst_bool tagger;      /* display tagger info ? */
	} Tast_cln_optsDisp;

	/** for options and command arguments from cmdline */
	typedef struct {
		/* options */
		/** */
		Tst_basOpts        basOpts;        /* basic options (verblev, pretWr, allowLnks) */
		/** */
		Tst_bool           quiet;          /* be quiet (zero output) ? */
		Tst_bool           showStat;       /* show some status info ? */
		/** */
		Tst_bool           isInpISOorU8;   /* is input ISO- or UTF8-encoded ? */
		/** */
		Tst_bool           quickScan;      /* do a quick scan ? */
		/** */
		Tast_cln_t_tagList_b(lstTagSw);    /* tags to show */
		Tast_cln_t_tagList_b(lstTagEd);    /* tags to edit/write */
		Tst_bool           tagEdAuto;      /* autom. select tags to edit/write ? */
		/** */
		Tast_cln_optsDisp  disp;           /* display options */
		/** */
		Tst_str            *pOutpdir;      /* output dir for decoded audio, extr. data, ... */
		/** for parseArgs(): */
		Tst_bool           paLstTagSwSet;  /* has a list been given at cmdline ? */
		Tst_bool           paTagSwNone;    /* shall no Tag be shown at all ? */
		Tst_bool           paLstTagEdSet;
		Tst_bool           paTagEdNone;
		/* arguments of commands */
		/** */
		Tast_cln_t_tagList_b(lstRemTag);   /* tags to remove */
		/** */
		Tast_cln_t_tagList_b(lstExtrTag);  /* tags to extract */
		Tst_uint32         *pLstExtrPic;   /* pictures to extr., last elem is 0 */
		Tst_bool           extrPicAll;     /* extract all pics ? */
		Tst_uint32         *pLstExtrGeo;   /* GEOBs to extr., last elem is 0 */
		Tst_bool           extrGeoAll;     /* extract all GEOBs ? */
		/** */
		Tast_cln_t_tagPair convTagp;       /* convert which tag(s) to which */
	} Tast_cln_opts;

	/** options for tag field values from cmdline */
	typedef struct {
		/* field-desc-set list, priority 1 */
		Tst_dynlist dsetListPrio1;
		/* field-val-add/set list, priority 2 */
		Tst_dynlist faddsetListPrio2;
		/* field-val-set list, priority 3 */
		Tst_dynlist fsetListPrio3;
		/* these have priority 4 */
		/** */
		Tst_int32 trk;      /* track number */
		Tst_int32 trkTot;   /* total amount of tracks */
		Tst_int32 disc;     /* disc number */
		Tst_int32 discTot;  /* total amount of discs */
		/** attached files */
		/*** add/modify */
		Tast_cln_attFileArr attFileArr;
		/*** remove */
		Tst_uint32 *pLstRemPic;  /* pictures to remove, last elem is 0 */
		Tst_bool   remPicAll;    /* remove all pics ? */
		Tst_uint32 *pLstRemGeo;  /* GEOBs to remove, last elem is 0 */
		Tst_bool   remGeoAll;    /* remove all GEOBs ? */
	} Tast_cln_optsTagFlds;

	/** options for audio/video */
	typedef struct {
		/* MPEG-1 options */
		Tst_mpeg1_layer     mpeg1_forceMLay;
		Tst_mpeg1_audioVers mpeg1_forceMAv;
		/* list of bitstreams to edit */
		Tast_cln_t_ebs      *pEdBsArr;
		Tst_bool            edBsAll;
		/* decode audio options */
		Tst_utilsFmt_mimeTp decAudFmt;  /* audio format to decode audio to */
		Tst_uint32          decAudBps;  /* bitsPerSample to write samples with */
	} Tast_cln_optsAvMisc;

	/** for commands from cmdline */
	typedef struct {
		/* amount of set cmds ... */
		Tst_uint32 roMainCmds;   /* ... read-only */
		Tst_uint32 roTgExtCmds;  /* ... extracting tags */
		Tst_uint32 rwTgRwrCmds;  /* ... rewriting tags */
		Tst_uint32 rwTgEdtCmds;  /* ... editing tags */
		Tst_uint32 rwTgRemCmds;  /* ... removing tags */
		Tst_uint32 rwTgCnvCmds;  /* ... converting tags */
		/* read-only cmds */
		Tst_bool   rd;        /* read 'n show audio/video info and tags ? */
		Tst_bool   anlz;      /* analize files ? */
		Tst_bool   decAud;    /* decode audio ? */
		/* (read-only) tag-extraction cmds */
		Tst_bool   extrTag;   /* extract tags ? */
		Tst_bool   extrPic;   /* extract pictures from tags ? */
		Tst_bool   extrGeo;   /* extract GEOBs from tags ? */
		Tst_bool   extrBinD;  /* extract all bin data (except GEO/PIC) from tags ? */
		/* (read-write) tag-rewriting cmds */
		Tst_bool   rewrFile;  /* rewrite file ? */
		/* (read-write) tag-removal cmds */
		Tst_bool   remTag;    /* remove tags ? */
		/* (read-write) tag-conversion cmds */
		Tst_bool   conv;      /* convert tags ? */
	} Tast_cln_cmds;

	/** callback types for normal/debug/error messages */
	typedef void (*Tast_cln_cbMsg)(const char* pFmt, ...);
	typedef void (*Tast_cln_cbDbg)(const char* pFnc, const char* pFmt, ...);
	typedef void (*Tast_cln_cbErr)(const Tst_str *pAppFn,
	                               const char* pFnc,
	                               const char* pFmt, ...);

	/** for everything from cmdline */
	typedef struct {
		Tst_str const *pAppFn;  /* pointer to application's name */
		/* for parseArgs(): long options cache */
		Tast_cln_longoptCache *pPAlngoptcArr;
		/* */
		Tast_cln_cmds cmds;
		Tast_cln_opts opts;
		/* */
		Tast_cln_optsTagFlds optsTagFlds;
		Tast_cln_t_tiv2opt   optsTagIV2;   /* options for ID3v2 Tags */
		Tast_cln_optsAvMisc  optsAvMisc;
		/* callbacks for normal/debug/error messages */
		Tast_cln_cbMsg cbMsg;
		Tast_cln_cbDbg cbDbg;
		Tast_cln_cbErr cbErr;
	} Tast_cln_a;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_CLN_PROT_H */

/******************************************************************************/
