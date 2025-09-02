/******************************************************************************/
/* string_mte-defs.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Definitions for strings with multiple text-encodings                       */
/*   (Latin1 aka ISO, UTF8, UTF16-LE/BE)                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_STRING_MTE_DEFS_H
#define LIBSANTAGGER_STRING_MTE_DEFS_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Constants
*/
/** Byte-Order-Marker  */
ST_EXPORT extern const Tst_str ST_UTF16_BOM_LE[];  /* Little-Endian */
ST_EXPORT extern const Tst_str ST_UTF16_BOM_BE[];  /* Big-Endian */

/** Text Encodings */
typedef enum {
	ST_MTES_TE_ISO,      /* ISO-8859-1, 0x00 term */
	ST_MTES_TE_UTF16LE,  /* UTF-16 Little-Endian, 0x0000 term */
	ST_MTES_TE_UTF16BE,  /* UTF-16BE Big-Endian, 0x0000 term, only ID3v2.4 */
	ST_MTES_TE_UTF8,     /* UTF-8, 0x00 term, only ID3v2.4 */
	ST_MTES_TE_NONE      /* none ! this must be the last elem ! */
} Tst_mtes_textEnc;


/*
// Types
*/
/** stripStrXXX(): character set, UTF16LE/BE */
typedef struct {
	Tst_bool isLEorBE;
	/* ISO/U16 */
	Tst_byte lf[2];      /* '\n' */
	Tst_byte cr[2];      /* '\r' */
	Tst_byte tab[2];     /* '\t' */
	Tst_byte spa[2];     /* ' '  */
	Tst_byte slash[2];   /* '/'  */
	Tst_byte parth1[2];  /* '('  */
	Tst_byte parth2[2];  /* ')'  */
	Tst_byte prnSma[2];  /* smallest printable char: ' ' */
	Tst_byte prnBig[2];  /* biggest    ---  "  --- : '~' */
	Tst_byte aCap[2];    /* 'A' */
	Tst_byte zCap[2];    /* 'Z' */
	Tst_byte aSma[2];    /* 'a' */
	Tst_byte zSma[2];    /* 'z' */
	Tst_byte digZe[2];   /* '0' */
	Tst_byte digNi[2];   /* '9' */
	Tst_byte bra1B[2];   /* '[' */
	Tst_byte bra1E[2];   /* ']' */
	Tst_byte bra2B[2];   /* '<' */
	Tst_byte bra2E[2];   /* '>' */
	Tst_byte bra3B[2];   /* '{' */
	Tst_byte bra3E[2];   /* '}' */
	/* german/scandinavian special chars: umlauts */
	Tst_byte umAeC[2];   /* &Auml; (Ae) */
	Tst_byte umOeC[2];   /* &Ouml; (Oe) */
	Tst_byte umUeC[2];   /* &Uuml; (Ue) */
	Tst_byte umAeS[2];   /* &auml; (ae) */
	Tst_byte umOeS[2];   /* &ouml; (oe) */
	Tst_byte umUeS[2];   /* &uuml; (ue) */
	Tst_byte umSz[2];    /* &szlig; (sz) */
	Tst_byte umAe2C[2];  /* scandinavian Ae */
	Tst_byte umOe2C[2];  /* scandinavian O-stroke */
	Tst_byte umAaC[2];   /* scandinavian A-ring */
	Tst_byte umAe2S[2];  /* scandinavian ae */
	Tst_byte umOe2S[2];  /* scandinavian o-stroke */
	Tst_byte umAaS[2];   /* scandinavian a-ring */
	/* spanish special chars */
	Tst_byte nTildC[2];  /* N-tilde */
	Tst_byte oAcuC[2];   /* O-acute */
	Tst_byte uAcuC[2];   /* U-acute */
	Tst_byte aAcuC[2];   /* A-acute */
	Tst_byte eAcuC[2];   /* E-acute */
	Tst_byte iAcuC[2];   /* I-acute */
	Tst_byte yAcuC[2];   /* Y-acute */
	Tst_byte nTildS[2];  /* n-tilde */
	Tst_byte oAcuS[2];   /* o-acute */
	Tst_byte uAcuS[2];   /* u-acute */
	Tst_byte aAcuS[2];   /* a-acute */
	Tst_byte eAcuS[2];   /* e-acute */
	Tst_byte iAcuS[2];   /* i-acute */
	Tst_byte yAcuS[2];   /* y-acute */
	Tst_byte invExM[2];  /* inverted exclamation mark */
	Tst_byte invQuM[2];  /* inverted question mark */
	/* other special chars */
	Tst_byte copySg[2];  /* copyright sign */
	Tst_byte regSg[2];   /* registered sign */
	/* U16 only */
	Tst_byte umOe3C_u[2];  /* scandinavian Oe */
	Tst_byte umOe3S_u[2];  /* scandinavian oe */
} Tst_mtes_cset;

/** string restrictions */
typedef struct {
	Tst_bool hasR;        /* are any restrictions set ? */
	Tst_bool ruleAlw;     /* rule: allow or disallow per default ? */
	/* */
	Tst_bool disAlpha;    /* disallow alphabetical chars [Aa-Zz] ? */
	Tst_bool disDig;      /* disallow digits [0-9] ? */
	Tst_bool disSpa;      /* disallow whitespaces ? */
	Tst_bool disTab;      /* disallow tabs ['\t'] ? */
	Tst_bool disNewln;    /* disallow newline char ['\n'] ? */
	Tst_bool disSlash;    /* disallow slashes ? */
	Tst_bool disBracks;   /* disallow brackets ['<>','[]','{}'] ? */
	Tst_bool disParen;    /* disallow parenthesis ['(',')'] ? */
	Tst_bool disLngSpec;  /* disallow language specific chars ['ä','ß',...] ? */
	Tst_bool disNotPrint; /* disallow not printable chars ? */
	Tst_bool disOther;    /* disallow all other chars ? */
	/* */
	Tst_uint32 strMaxl;   /* max len of string in ISO representation,
	                       *   0 means infinite */
} Tst_mtes_strRestr;

/** A MTE-String object (MTE=Multi-Text-Encoding) */
typedef struct {
	void *pObInternal;  /* pointer to Tst_mtes__string_intn */
} Tst_mtes_string;

/** Array of MTE-String objects (MTE=Multi-Text-Encoding) */
typedef struct {
	void *pObInternal;  /* pointer to Tst_mtes__strArr_intn */
} Tst_mtes_strArr;


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_STRING_MTE_DEFS_H */

/******************************************************************************/
