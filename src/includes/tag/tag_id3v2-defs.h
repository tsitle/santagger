/******************************************************************************/
/* tag_id3v2-defs.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for ID3v2 Tags                                                 */
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

#ifndef HEAD_TAG_ID3V2_DEFS_H
#define HEAD_TAG_ID3V2_DEFS_H

/*
// Own-Includes
*/
#include "../common/binobj-defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** */
#define ST_ID3V2_TAG_NAME     "ID3v2"
#define ST_ID3V2_TAG_NAME_SH  "iv2"

/** ID3v2 picture types */
typedef enum {
	ST_ID3V2_PT_OTHER   = 0x00, /* Other */
	ST_ID3V2_PT_3232    = 0x01, /* 32x32 pixels 'file icon' (PNG only) */
	ST_ID3V2_PT_OFICON  = 0x02, /* Other file icon */
	ST_ID3V2_PT_COVFRO  = 0x03, /* Cover (front) */
	ST_ID3V2_PT_COVBAC  = 0x04, /* Cover (back) */
	ST_ID3V2_PT_LEAFL   = 0x05, /* Leaflet page */
	ST_ID3V2_PT_MEDIA   = 0x06, /* Media (e.g. lable side of CD) */
	ST_ID3V2_PT_LEADART = 0x07, /* Lead artist/lead performer/soloist */
	ST_ID3V2_PT_ART     = 0x08, /* Artist/performer */
	ST_ID3V2_PT_COND    = 0x09, /* Conductor */
	ST_ID3V2_PT_BAND    = 0x0a, /* Band/Orchestra */
	ST_ID3V2_PT_COMP    = 0x0b, /* Composer */
	ST_ID3V2_PT_LYRIC   = 0x0c, /* Lyricist/text writer */
	ST_ID3V2_PT_RECLOC  = 0x0d, /* Recording Location */
	ST_ID3V2_PT_DURREC  = 0x0e, /* During recording */
	ST_ID3V2_PT_DURPER  = 0x0f, /* During performance */
	ST_ID3V2_PT_SCRCAP  = 0x10, /* Movie/video screen capture */
	ST_ID3V2_PT_FLASH   = 0x11, /* A bright coloured fish */
	ST_ID3V2_PT_ILLUS   = 0x12, /* Illustration */
	ST_ID3V2_PT_BLOGO   = 0x13, /* Band/artist logotype */
	ST_ID3V2_PT_PLOGO   = 0x14, /* Publisher/Studio logotype */
	ST_ID3V2_PT_NONE    = 0x15  /* none ! this must be the last elem ! */
} Tst_id3v2_picTp;

/** ID3v2 synced lyrics timestamp format */
typedef enum {
	ST_ID3V2_SLTTSF_MPEG  = 0x00,  /* MPEG frames based */
	ST_ID3V2_SLTTSF_MILLI = 0x01,  /* Millisecs based */
	ST_ID3V2_SLTTSF_NONE  = 0x02   /* none ! this must be the last elem ! */
} Tst_id3v2_sltTSF;

/** ID3v2 synced lyrics content type */
typedef enum {
	ST_ID3V2_SLTCTP_OTHER  = 0x00,  /* Other */
	ST_ID3V2_SLTCTP_LYR    = 0x01,  /* Lyrics */
	ST_ID3V2_SLTCTP_TRANS  = 0x02,  /* Text transcription */
	ST_ID3V2_SLTCTP_MOVE   = 0x03,  /* Movement/Part name */
	ST_ID3V2_SLTCTP_EVENT  = 0x04,  /* Events */
	ST_ID3V2_SLTCTP_CHORD  = 0x05,  /* Chord */
	ST_ID3V2_SLTCTP_TRIV   = 0x06,  /* Trivia/'Pop up' */
	ST_ID3V2_SLTCTP_URLWP  = 0x07,  /* URLs to webpages */
	ST_ID3V2_SLTCTP_URLIMG = 0x08,  /* URLs to images */
	ST_ID3V2_SLTCTP_NONE   = 0x09   /* none ! this must be the last elem ! */
} Tst_id3v2_sltCTp;

/** ID3v2 frame types (regarding their structure) */
typedef enum {
	ST_ID3V2_FTP_COM,      /* COM[M],U[S]LT */
	ST_ID3V2_FTP_SLT,      /* S[Y]LT */
	ST_ID3V2_FTP_PIC,      /* [A]PIC */
	ST_ID3V2_FTP_TXT,      /* CM1,T00[0]-TZZ[Z] */
	ST_ID3V2_FTP_URL,      /* W00[0]-WZZ[Z] */
	ST_ID3V2_FTP_UFI,      /* UFI[D],PRIV */
	ST_ID3V2_FTP_IPL,      /* IPL[S] */
	ST_ID3V2_FTP_MCI,      /* MC[D]I */
	ST_ID3V2_FTP_CDM,      /* CDM */
	ST_ID3V2_FTP_WXX,      /* TXX[X],WXX[X] */
	ST_ID3V2_FTP_GEO,      /* GEO[B] */
	ST_ID3V2_FTP_CNT,      /* [P]CNT */
	ST_ID3V2_FTP_POP,      /* POP[M] */
	ST_ID3V2_FTP_CPY,      /* copy data */
	ST_ID3V2_FTP_NONE      /* none ! this must be the last elem ! */
} Tst_id3v2_frTp;

/** ID3v2 frame data read/write order - type of data */
typedef enum {
	ST_ID3V2_TFLD_RWO_TP_TENC,  /* text encoding */
	ST_ID3V2_TFLD_RWO_TP_LANG,  /* language */
	ST_ID3V2_TFLD_RWO_TP_SCD,   /* short content desc, ... */
	ST_ID3V2_TFLD_RWO_TP_TEXT,  /* text (single/multi string) */
	ST_ID3V2_TFLD_RWO_TP_BDAT,  /* binary data */
	ST_ID3V2_TFLD_RWO_TP_UVAL,  /* unsigned int value */
	ST_ID3V2_TFLD_RWO_TP_BVAL,  /* unsigned byte value */
	/* */
	ST_ID3V2_TFLD_RWO_TP_PICT,  /* [A]PIC imgfmt/mime + pic type */
	ST_ID3V2_TFLD_RWO_TP_GEOB,  /* GEO[B] mime + filen */
	ST_ID3V2_TFLD_RWO_TP_CDMF,  /* CDM comprMeth + uncomprSz */
	ST_ID3V2_TFLD_RWO_TP_SYLT,  /* S[Y]LT timestampfmt + conttp */
	/* */
	ST_ID3V2_TFLD_RWO_TP_NONE   /* none ! this must be the last elem ! */
} Tst_id3v2_fldd_rword_typ;

/** ID3v2 frame IDs */
typedef enum {
	ST_ID3V2_FID_234_AENC,
	ST_ID3V2_FID_234_APIC,  /* # Cover */
	ST_ID3V2_FID___4_ASPI,
	/* */
	ST_ID3V2_FID_2___CDMF,  /* - compressed metaframe, ID3v2.2.1 */
	ST_ID3V2_FID_2___COM1,  /* # Comment conv. from ID3v1, iTunes 9 */
	ST_ID3V2_FID__34_COMR,
	ST_ID3V2_FID_234_COMM,  /* # Comment */
	ST_ID3V2_FID_2___CRM1,
	/* */
	ST_ID3V2_FID__34_ENCR,
	ST_ID3V2_FID___4_EQU2,
	ST_ID3V2_FID_23__EQUA,
	ST_ID3V2_FID_234_ETCO,
	/* */
	ST_ID3V2_FID_234_GEOB,
	ST_ID3V2_FID__34_GRID,
	/* */
	ST_ID3V2_FID_23__IPLS,
	/* */
	ST_ID3V2_FID_234_LINK,
	/* */
	ST_ID3V2_FID_234_MCDI,
	ST_ID3V2_FID_234_MLLT,
	/* */
	ST_ID3V2_FID__34_OWNE,
	/* */
	ST_ID3V2_FID_234_PCNT,
	ST_ID3V2_FID_234_PCST,
	ST_ID3V2_FID_234_POPM,
	ST_ID3V2_FID__34_POSS,
	ST_ID3V2_FID__34_PRIV,
	/* */
	ST_ID3V2_FID_234_RBUF,
	ST_ID3V2_FID___4_RVA2,
	ST_ID3V2_FID_23__RVAD,
	ST_ID3V2_FID_234_RVRB,
	/* */
	ST_ID3V2_FID___4_SEEK,
	ST_ID3V2_FID___4_SIGN,
	ST_ID3V2_FID_234_SYLT,
	ST_ID3V2_FID_234_SYTC,
	/* */
	ST_ID3V2_FID_234_TALB,  /* # Album */
	/** */
	ST_ID3V2_FID_234_TBPM,
	/** */
	ST_ID3V2_FID_234_TCAT,
	ST_ID3V2_FID_234_TCMP,
	ST_ID3V2_FID_234_TCOM,
	ST_ID3V2_FID_234_TCON,  /* # Genre '(9)Metal', '(9)', 'Metal' */
	ST_ID3V2_FID_234_TCOP,
	/** */
	ST_ID3V2_FID_23__TDAT,
	ST_ID3V2_FID___4_TDEN,
	ST_ID3V2_FID_234_TDES,
	ST_ID3V2_FID_234_TDLY,
	ST_ID3V2_FID___4_TDOR,
	ST_ID3V2_FID___4_TDRC,  /* # Year */
	ST_ID3V2_FID___4_TDRL,  /* # Year */
	ST_ID3V2_FID___4_TDTG,
	/** */
	ST_ID3V2_FID_234_TENC,
	ST_ID3V2_FID_234_TEXT,
	/** */
	ST_ID3V2_FID_234_TFLT,
	/** */
	ST_ID3V2_FID_234_TGID,
	/** */
	ST_ID3V2_FID_234_TIT1,
	ST_ID3V2_FID_234_TIT2,  /* # Song */
	ST_ID3V2_FID_234_TIT3,
	ST_ID3V2_FID_23__TIME,
	ST_ID3V2_FID___4_TIPL,
	/** */
	ST_ID3V2_FID_234_TKEY,
	/** */
	ST_ID3V2_FID_234_TLAN,
	ST_ID3V2_FID_234_TLEN,
	/** */
	ST_ID3V2_FID___4_TMCL,
	ST_ID3V2_FID_234_TMED,
	ST_ID3V2_FID___4_TMOO,
	/** */
	ST_ID3V2_FID_234_TOAL,
	ST_ID3V2_FID_23__TORY,
	ST_ID3V2_FID_234_TOLY,
	ST_ID3V2_FID_234_TOPE,
	ST_ID3V2_FID_234_TOFN,
	ST_ID3V2_FID__34_TOWN,
	/** */
	ST_ID3V2_FID_234_TPE1,  /* # Band */
	ST_ID3V2_FID_234_TPE2,
	ST_ID3V2_FID_234_TPE3,
	ST_ID3V2_FID_234_TPE4,
	ST_ID3V2_FID_234_TPOS,  /* # Discnr., '1', '1/7' */
	ST_ID3V2_FID___4_TPRO,
	ST_ID3V2_FID_234_TPUB,
	/** */
	ST_ID3V2_FID_234_TRCK,  /* # Tracknr., '1', '1/7' */
	ST_ID3V2_FID_23__TRDA,
	ST_ID3V2_FID__34_TRSN,
	ST_ID3V2_FID__34_TRSO,
	/** */
	ST_ID3V2_FID_23__TSIZ,  /* this one gets ignored when reading tags */
	/** */
	ST_ID3V2_FID_234_TSOC,
	ST_ID3V2_FID_234_TSO2,
	ST_ID3V2_FID_234_TSSE,
	ST_ID3V2_FID_234_TSOP,
	ST_ID3V2_FID_234_TSOA,
	ST_ID3V2_FID_234_TSRC,
	ST_ID3V2_FID___4_TSST,
	ST_ID3V2_FID_234_TSOT,
	/** */
	ST_ID3V2_FID_234_TXXX,
	/** */
	ST_ID3V2_FID_23__TYER,  /* # Year */
	/* */
	ST_ID3V2_FID_234_UFID,
	ST_ID3V2_FID__34_USER,
	ST_ID3V2_FID_234_USLT,
	/* */
	ST_ID3V2_FID_234_WCOM,
	ST_ID3V2_FID_234_WCOP,
	ST_ID3V2_FID_234_WFED,
	ST_ID3V2_FID_234_WOAR,
	ST_ID3V2_FID_234_WOAF,
	ST_ID3V2_FID_234_WOAS,
	ST_ID3V2_FID__34_WORS,
	ST_ID3V2_FID__34_WPAY,
	ST_ID3V2_FID_234_WPUB,
	ST_ID3V2_FID_234_WXXX,
	/* */
	ST_ID3V2_FID_234_CTXT,  /* Custom text field */
	ST_ID3V2_FID_234_CURL,  /* Custom URL field */
	/* */
	ST_ID3V2_FID_NONE       /* - none ! this must be the last elem ! */
} Tst_id3v2_frID;


/*
// Types
*/
/** ID3v2 tag: errors/warnings in tag header/footer/body */
typedef struct {
	/* errors in tag header/footer */
	Tst_bool thIsSizeInv;     /* is tag size invalid ? */
	Tst_bool thHasUnknFl;     /* has unknown flag ? */
	Tst_bool thIsEntTagCompr; /* is entire tag compressed ? */
	Tst_bool thIsVersInv;     /* is tag version invalid ? */
	Tst_bool thIsFlagByteInv; /* is flag byte invalid ? */
	Tst_bool thWasSzCorrd;    /* was tag size corrected ? */
	/* errors in tag body */
	Tst_bool tbHasInvFID;     /* has a invalid frame ID been found ? */
	Tst_bool tbHasInvFrSz;    /* has a invalid frame size been found ? */
	Tst_bool tbHasUnknFIDs;   /* has unknown frame IDs ? */
	Tst_bool tbHasTooManyTFs; /* has too many frames ? */
	Tst_bool tbHasFalseSyn;   /* has false sync bytes ? */
} Tst_id3v2_tagErrs;

/** ID3v2 tag: errors/warnings in tag frames */
typedef struct {
	/* errors in frames */
	Tst_bool frHasFalseSyn;   /* has false sync bytes ? */
	Tst_bool frHasCompr;      /* has compressed frames ? */
	Tst_bool frHasEncr;       /* has encrypted frames ? */
	Tst_bool frHasInvFlData;  /* has invalid data in flags ? */
	Tst_bool frHasInvData;    /* has frame invalid data ? */
	Tst_bool frHasInvFlags;   /* has invalid flags ? */
	Tst_bool frIsIPLSincompl; /* was 'Involved People List' mapping incomplete ? */
	Tst_bool frHasInvChars;   /* had read string invalid chars ? */
	Tst_bool frHasInvComprM;  /* has invalid compression method ? */
	Tst_bool frIsntValVers;   /* is frame not valid for ID3v2 version ? */
	Tst_bool frFraWasntDcmpr; /* was frame not decompressable ? */
	Tst_bool frDatWasntDcmpr; /* was data in frame not decompressable ? */
	Tst_bool frHasInvTE;      /* has frame invalid text-enc */
	Tst_bool frHasInvPT;      /* has frame invalid pict-type */
	Tst_bool frHasInvPF;      /* has frame invalid pict-format */
	Tst_bool frHasInvSLTtsf;  /* has inv. SYLT timestamp-format ? */
	Tst_bool frHasInvSLTctp;  /* has inv. SYLT content-type ? */
	Tst_bool frHasInvLng;     /* has frame invalid lang */
	Tst_bool frCorrdU16;      /* was UTF16 encoding corrected ? */
	Tst_bool frWasMergd;      /* was field merged with other field ? */
	Tst_bool frIsDup;         /* was field a duplicate ? */
	Tst_bool frWasMappd;      /* was field mapped to another field ? */
	Tst_bool frTooManyTFI;    /* does exceed frames per ID max. ? */
	Tst_bool frTooManyMS;     /* has too many strings in multi-str ? */
} Tst_id3v2_fldErrs;

/** ID3v2 tag frames: what kind of data does/can field contain ? */
typedef struct {
	Tst_bool hasBDat;  /* binary data ? */
	Tst_bool hasScd;   /* short cont. desc. ? */
	Tst_bool hasTEnc;  /* text encoding ? */
	Tst_bool hasText;  /* text ? */
	Tst_bool hasCTxt;  /* text converted from bin data (e.g. SYLT) ? */
	Tst_bool hasLang;  /* language identifier (e.g. 'eng') ? */
	Tst_bool hasFn;    /* filename (e.g. GEOB) ? */
	Tst_bool hasMime;  /* mime-type (e.g. GEOB) ? */
	Tst_bool hasUVal;  /* uint value (e.g. PCNT) ? */
	Tst_bool hasBVal;  /* byte value (e.g. POPM) ? */
	/* */
	Tst_bool hasPict;  /* picture ?
	                    *   imgfmt/mime, ptp (+scd +bindata)  */
	Tst_bool hasSylt;  /* synced lyrics ?
	                    *   tsfmt, ctp (+lang +scd +bindata (+fulltxt))  */
	Tst_bool hasGeob;  /* gen. encaps. object ?
	                    *   mime (+filen +scd +bindata)  */
	Tst_bool hasCdmf;  /* compressed metaframe ?
	                    *   comprMeth, uncomprSz (+bindata)  */
} Tst_id3v2_fldd_dattp;

/** ID3v2 tag frames: what kind of string(s) does strArr contain ? */
typedef struct {
	Tst_bool hasNumStr;     /* numerical string (e.g. "90182") ? */
	Tst_bool hasYearStr;    /* numerical year string, YYYY (e.g. "2001") ? */
	Tst_bool hasDateStr;    /* numerical date string, DDMM (e.g. "3101") ? */
	Tst_bool hasTimeStr;    /* numerical time string, HHMM (e.g. "2359") ? */
	Tst_bool hasTStampStr;  /* numerical time stamp string ?
	                         *   (e.g. "2002-01-17T21:22:01")  */
	Tst_bool hasPosStr;     /* position in set string (e.g. "1/10") ? */
	/* */
	Tst_bool hasGenreStr;   /* genre string (e.g. "(9)Metal") ? */
	Tst_bool hasSimpleStr;  /* simple string without \n ? */
	Tst_bool hasUrlStr;     /* URL string (email or other URL) ? */
	Tst_bool hasMappedStr;  /* mapped strings ?
	                         *   (e.g. "Producer" --> "Will Smith")  */
	Tst_bool hasFullStr;    /* full text string with \n ? */
} Tst_id3v2_fldd_strtp;

/** ID3v2 tag: frame properties, incl. ID String, Caption, ... */
typedef struct {
	void *pObInternal;  /* pointer to Tst_id3v2_fldProp_intn */
} Tst_id3v2_fldProp;

/** ID3v2 tag frames: data of one frame */
typedef struct {
	void *pObInternal;  /* pointer to Tst_id3v2_fldData_intn */
} Tst_id3v2_fldData;

/** ID3v2 Tag: Main Structure */
typedef struct {
	void *pObInternal;  /* pointer to Tst_id3v2_tag_intn */
} Tst_id3v2_tag;

/** Callbacks for zlib */
typedef Tst_err
        (*Tst_id3v2_cbZLib_decompress)(Tst_binobj *pBinDatIn,
                                       const Tst_uint32 uncomprSzShould,
                                       Tst_binobj *pBinDatOut);
typedef Tst_err
        (*Tst_id3v2_cbZLib_compress)(Tst_binobj *pBinDatIn,
                                     Tst_binobj *pBinDatOut);


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_DEFS_H */

/******************************************************************************/
