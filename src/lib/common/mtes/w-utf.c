/******************************************************************************/
/* w-utf.c                      [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for UTF string handling                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 09.01.2010 (dd.mm.yyyy)                                     */
/*                                                                            */
/* This file contains wrapping functions for the UTF functions                */
/*   that libid3tag uses and that were written by Underbit Technologies.      */
/* Those functions etc. that I wrote are marked with "by Tas".                */
/******************************************************************************/
/* Original header from libid3tag-0.15.1b follows:                            */
/*
 * libid3tag - ID3 tag manipulation library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#define SRC_W_UTF_C
#include "w-utf-prot.h"
#undef SRC_W_UTF_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef unsigned long  Tst_utf__ucs4_t;
typedef signed char    Tst_utf__utf8_t;
typedef unsigned short Tst_utf__utf16_t;
typedef unsigned char  Tst_utf__latin1_t;
typedef unsigned char  Tst_utf__byte_t;
typedef unsigned long  Tst_utf__length_t;

#if (ST_W_UTF_DEB_ == 1)
/* Debugging, by Tas */
static void
ST_UTF__convStr_debugIso(const char* pName, const Tst_str *pISO,
                         const unsigned isoSz);
static void
ST_UTF__convStr_debugUcs(const char* pName, const Tst_utf__ucs4_t *pUcs4);
static void
ST_UTF__convStr_debugUtf8(const char* pName, const Tst_str *pU8,
                          const unsigned u8Sz);
static void
ST_UTF__convStr_debugUtf16(const char* pName, const Tst_str *pU16,
                           const unsigned u16Sz);
#endif

/* ---------------------------------------------------------------------- */

/* UCS4 */
/** like strlen(), returns len of ISO-String repres. of UCS4-String */
static Tst_utf__length_t
ST_UTF__ucs4_length(Tst_utf__ucs4_t const *ucs4);
/** */
static Tst_utf__length_t
ST_UTF__ucs4_utf8size(Tst_utf__ucs4_t const *ucs4);
static Tst_utf__length_t
ST_UTF__ucs4_utf16size(Tst_utf__ucs4_t const *ucs4);

/* UTF8 */
/** encode UCS4 to UTF8-String */
static Tst_utf__length_t
ST_UTF__utf8_serialize(Tst_utf__byte_t **ptr,
                       Tst_utf__ucs4_t const *ucs4, int terminate);
/** decode UTF-8 to UCS4-String */
static Tst_utf__ucs4_t*
ST_UTF__utf8_deserialize(Tst_utf__byte_t const **ptr,
                         Tst_utf__length_t length);
/** like strlen(), returns len of UTF8-String represent. of UCS4-String */
static Tst_utf__length_t
ST_UTF__utf8_length(Tst_utf__utf8_t const *utf8,
                    const Tst_utf__length_t u8sz);
/** */
static Tst_utf__length_t
ST_UTF__utf8_encodechar(Tst_utf__utf8_t *utf8, Tst_utf__ucs4_t ucs4);
static Tst_utf__length_t
ST_UTF__utf8_put(Tst_utf__byte_t **ptr, Tst_utf__utf8_t utf8);
/** */
static Tst_utf__utf8_t
ST_UTF__utf8_get(Tst_utf__byte_t const **ptr);
static void
ST_UTF__utf8_decode(Tst_utf__utf8_t const *utf8, Tst_utf__ucs4_t *ucs4,
                    const Tst_utf__length_t u8sz);
static Tst_utf__length_t
ST_UTF__utf8_decodechar(Tst_utf__utf8_t const *utf8,
                        Tst_utf__ucs4_t *ucs4, const Tst_utf__length_t u8sz);

/* UTF16 */
/** encode UCS4 to UTF16-String */
static Tst_utf__length_t
ST_UTF__utf16_serialize(Tst_utf__byte_t **ptr, Tst_utf__ucs4_t const *ucs4,
                        enum st_utf_utf16_byteorder byteorder, int terminate);
/** decode UTF16 to UCS4-String */
static int
ST_UTF__utf16_deserialize(Tst_utf__byte_t const **ptr,
                          Tst_utf__length_t length,
                          enum st_utf_utf16_byteorder byteorder,
                          Tst_utf__ucs4_t **ppUcs4);
/** like strlen(), returns len of UTF16-String represent. of UCS4-String */
static Tst_utf__length_t
ST_UTF__utf16_length(Tst_utf__utf16_t const *utf16);
/** */
static Tst_utf__length_t
ST_UTF__utf16_put(Tst_utf__byte_t **ptr, Tst_utf__utf16_t utf16,
                  enum st_utf_utf16_byteorder byteorder);
static Tst_utf__length_t
ST_UTF__utf16_encodechar(Tst_utf__utf16_t *utf16, Tst_utf__ucs4_t ucs4);
/** */
static Tst_utf__utf16_t
ST_UTF__utf16_get(Tst_utf__byte_t const **ptr, enum st_utf_utf16_byteorder byteorder);
static Tst_utf__length_t
ST_UTF__utf16_decodechar(Tst_utf__utf16_t const *utf16, Tst_utf__ucs4_t *ucs4);
static void
ST_UTF__utf16_decode(Tst_utf__utf16_t const *utf16, Tst_utf__ucs4_t *ucs4);

/* LATIN */
static void
ST_UTF__latin1_decode(Tst_utf__latin1_t const *latin1, Tst_utf__ucs4_t *ucs4);
static Tst_utf__length_t
ST_UTF__latin1_decodechar(Tst_utf__latin1_t const *latin1, Tst_utf__ucs4_t *ucs4);
static void
ST_UTF__latin1_encode(Tst_utf__latin1_t *latin1, Tst_utf__ucs4_t const *ucs4);
static Tst_utf__length_t
ST_UTF__latin1_encodechar(Tst_utf__latin1_t *latin1, Tst_utf__ucs4_t ucs4);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* check if string is NUL-terminated, by Tas */
Tst_bool
st_utf_isStrNullTerm_iso_u8(const Tst_str *pStr, const Tst_uint32 sz)
{
	Tst_str const *pS;

	if (pStr == NULL || sz == 0)
		return ST_B_FALSE;

	pS = pStr + (sz - 1);
	while (pS >= pStr)
		if (*pS-- == 0x00)
			return ST_B_TRUE;
	return ST_B_FALSE;
}

Tst_bool
st_utf_isStrNullTerm_u16(const Tst_str *pStr, const Tst_uint32 sz)
{
	Tst_str const *pS;

	if (pStr == NULL || sz < 2)
		return ST_B_FALSE;

	pS = pStr + (sz - 1);
	while (pS > pStr)
		if (*pS-- == 0x00 && *pS-- == 0x00)
			return ST_B_TRUE;
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Get length of string in chars, NOT the buffer size, by Tas
 *
 * pUTF8  MUST be set
 *
 * returns: length
 */
Tst_uint32
st_utf_strlen_utf8(const Tst_str *pUTF8, const Tst_uint32 utfSz,
		const Tst_bool checkTerm)
{
	if (checkTerm && ! st_utf_isStrNullTerm_iso_u8(pUTF8, utfSz))
		return 0;
	return (Tst_uint32)ST_UTF__utf8_length((Tst_utf__utf8_t const *)pUTF8, utfSz);
}

/*----------------------------------------------------------------------------*/

/*
 * Encode ISO-String to UTF8-String, by Tas
 *
 * pISO       MUST be set
 * ppStrUTF8  MUST be != NULL
 *
 * returns: 0 on error or if >0 size of *ppStrUTF8 buffer (incl. trailing zeros)
 */
Tst_uint32
st_utf_convStr_iso_to_utf8(const Tst_str *pISO, const Tst_uint32 isoSz,
		const Tst_bool checkTerm, Tst_str **ppStrUTF8)
{
	Tst_utf__length_t res         = 0;
	Tst_utf__byte_t   *pB         = NULL;
	Tst_utf__ucs4_t   *pU         = NULL;
	Tst_utf__length_t outlenAsUT8 = 0;

	if (pISO == NULL || ppStrUTF8 == NULL || pISO == *ppStrUTF8)
		return 0;
	ST_DELPOINT(*ppStrUTF8)
	if (isoSz == 0 || (checkTerm && ! st_utf_isStrNullTerm_iso_u8(pISO, isoSz)))
		return 0;

	/* uchar --> ucs4 */
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugIso("ITU8", pISO, isoSz);
#	endif
	ST_CALLOC(pU, Tst_utf__ucs4_t*, isoSz + 2, sizeof(Tst_utf__ucs4_t))
	if (pU == NULL)
		return 0;
	ST_UTF__latin1_decode(pISO, pU);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUcs("ITU8", pU);
#	endif

	/* ucs4 -> utf8 */
	outlenAsUT8 = ST_UTF__ucs4_utf8size(pU);
	ST_CALLOC(*ppStrUTF8, Tst_str*, (outlenAsUT8 + 2) * sizeof(Tst_utf__utf8_t), 1)
	if (*ppStrUTF8 == NULL) {
		ST_DELPOINT(pU)
		return 0;
	}
	pB  = *ppStrUTF8;  /* don't change this or &pB in the next line ! */
	res = ST_UTF__utf8_serialize(&pB, pU, 1 /* true */);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf8("ITU8", *ppStrUTF8, (Tst_uint32)res);
#	endif

	ST_DELPOINT(pU)
	if  (res < 1)
		ST_DELPOINT(*ppStrUTF8)
	return (Tst_uint32)res;
}

/*
 * Decode UTF8-String to ISO-String, by Tas
 *
 * pUTF8     MUST be set
 * ppStrISO  MUST be != NULL
 *
 * returns: 0 on error or if >0 size of *ppStrISO buffer (incl. trailing zero)
 */
Tst_uint32
st_utf_convStr_utf8_to_iso(const Tst_str *pUTF8, const Tst_uint32 utfSz,
		const Tst_bool checkTerm, Tst_str **ppStrISO)
{
	Tst_utf__length_t     newsz = 0;
	Tst_utf__ucs4_t       *pU   = NULL;
	Tst_utf__byte_t const *pB   = NULL;

	if (pUTF8 == NULL || ppStrISO == NULL || pUTF8 == *ppStrISO)
		return 0;
	ST_DELPOINT(*ppStrISO)
	if (utfSz < 1 || (checkTerm && ! st_utf_isStrNullTerm_iso_u8(pUTF8, utfSz)))
		return 0;

	/* utf8 --> ucs4 */
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf8("U8TI", pUTF8, utfSz);
#	endif
	pB = pUTF8;  /* don't change this or &pB in the next line ! */
	pU = ST_UTF__utf8_deserialize(&pB, utfSz);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUcs("U8TI", pU);
#	endif

	/* ucs4 --> uchar */
	newsz = ST_UTF__ucs4_length(pU) + 1;
	ST_CALLOC(*ppStrISO, Tst_str*, newsz + 2, 1)
	if (*ppStrISO == NULL) {
		ST_DELPOINT(pU)
		return 0;
	}
	ST_UTF__latin1_encode(*ppStrISO, pU);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugIso("U8TI", *ppStrISO, newsz);
#	endif
	ST_DELPOINT(pU)
	return (Tst_uint32)newsz;
}

/*
 * Reencode UTF8-String as UTF16-String, by Tas
 *	If you want to reencode as UTF-16BE String, byteordOut==ST_UTF16_BYTEORDER_BE
 *  If you want to reencode as UTF-16(LE) String, byteordOut==ST_UTF16_BYTEORDER_LE
 *
 * pUTF8       MUST be set
 * ppStrUTF16  MUST be != NULL
 *
 * returns: 0 on error or if >0 size of *ppStrISO buffer (incl. trailing zero)
 */
Tst_uint32
st_utf_convStr_utf8_to_utf16(const Tst_str *pUTF8, const Tst_uint32 utfSz,
		const Tst_bool checkTerm, Tst_str **ppStrUTF16, const Tst_utf16_bytOrd byteordOut)
{
	Tst_utf__length_t     res          = 0;
	Tst_utf__ucs4_t       *pU          = NULL;
	Tst_utf__byte_t const *pB1         = NULL;
	Tst_utf__byte_t       *pB2         = NULL;
	Tst_utf__length_t     outlenAsUT16 = 0;

	if (pUTF8 == NULL || ppStrUTF16 == NULL || pUTF8 == *ppStrUTF16 ||
			byteordOut == ST_UTF16_BYTEORDER_ANY)
		return 0;
	ST_DELPOINT(*ppStrUTF16)
	if (utfSz < 1 || (checkTerm && ! st_utf_isStrNullTerm_iso_u8(pUTF8, utfSz)))
		return 0;

	/* utf8 --> ucs4 */
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf8("U8TU16", pUTF8, utfSz);
#	endif
	pB1 = pUTF8;  /* don't change this or &pB in the next line ! */
	pU  = ST_UTF__utf8_deserialize(&pB1, utfSz);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUcs("U8TU16", pU);
#	endif

	/* ucs4 -> utf16 */
	outlenAsUT16 = ST_UTF__ucs4_utf16size(pU);
	ST_CALLOC(*ppStrUTF16, Tst_str*, (outlenAsUT16 + 2) * sizeof(Tst_utf__utf16_t), 1)
	if (*ppStrUTF16 == NULL) {
		ST_DELPOINT(pU)
		return 0;
	}
	pB2 = *ppStrUTF16;  /* don't change this or &pB in the next line ! */
	res = ST_UTF__utf16_serialize(&pB2, pU, byteordOut, 1 /* true */);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf16("U8TU16", *ppStrUTF16, res);
#	endif

	ST_DELPOINT(pU)
	if (res < 1)
		ST_DELPOINT(*ppStrUTF16)
	return (Tst_uint32)res;
}

/*
 * Encode ISO-String to UTF16-String, by Tas
 *	If you want to encode a UTF-16BE String, byteordOut==ST_UTF16_BYTEORDER_BE
 *  If you want to encode a UTF-16(LE) String, byteordOut==ST_UTF16_BYTEORDER_LE
 *
 * pISO        MUST be set
 * ppStrUTF16  MUST be != NULL
 *
 * returns: 0 on error or if >0 size of *ppStrUTF16 buffer (incl. trailing zeros)
 */
Tst_uint32
st_utf_convStr_iso_to_utf16(const Tst_str *pISO, const Tst_uint32 isoSz,
		const Tst_bool checkTerm, Tst_str **ppStrUTF16, const Tst_utf16_bytOrd byteordOut)
{
	Tst_utf__length_t res          = 0;
	Tst_utf__byte_t   *pB          = NULL;
	Tst_utf__ucs4_t   *pU          = NULL;
	Tst_utf__length_t outlenAsUT16 = 0;

	if (pISO == NULL || ppStrUTF16 == NULL || pISO == *ppStrUTF16 ||
			byteordOut == ST_UTF16_BYTEORDER_ANY)
		return 0;
	ST_DELPOINT(*ppStrUTF16)
	if (isoSz < 1 || (checkTerm && ! st_utf_isStrNullTerm_iso_u8(pISO, isoSz)))
		return 0;

	/* uchar --> ucs4 */
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugIso("ITU16", pISO, isoSz);
#	endif
	ST_CALLOC(pU, Tst_utf__ucs4_t*, isoSz + 2, sizeof(Tst_utf__ucs4_t))
	if (pU == NULL)
		return 0;
	ST_UTF__latin1_decode(pISO, pU);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUcs("ITU16", pU);
#	endif

	/* ucs4 -> utf16 */
	outlenAsUT16 = ST_UTF__ucs4_utf16size(pU);
	ST_CALLOC(*ppStrUTF16, Tst_str*, (outlenAsUT16 + 2) * sizeof(Tst_utf__utf16_t), 1)
	if (*ppStrUTF16 == NULL) {
		ST_DELPOINT(pU)
		return 0;
	}
	pB  = *ppStrUTF16;  /* don't change this or &pB in the next line ! */
	res = ST_UTF__utf16_serialize(&pB, pU, byteordOut, 1 /* true */);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf16("ITU16", *ppStrUTF16, (Tst_uint32)res);
#	endif
	/* or like this:
	 *  Tst_utf__byte_t *pUTF16orgaddr;
	 *  pUTF16orgaddr = *ppStrUTF16;
	 *  res           = ST_UTF__utf16_serialize(ppStrUTF16, pU, byteordOut, 1);
	 *  *ppStrUTF16   = pUTF16orgaddr;  // ==^ (*ppStrUTF16) -= res;
	 */

	ST_DELPOINT(pU)
	if (res < 1)
		ST_DELPOINT(*ppStrUTF16)
	return (Tst_uint32)res;
}

/*
 * Decode UTF16-String to ISO-String, by Tas
 *	If you want to decode a UTF-16BE String, byteordIn==ST_UTF16_BYTEORDER_BE
 *  If you want to decode a UTF-16(LE) String, byteordIn==ST_UTF16_BYTEORDER_LE
 *
 * pUTF16    MUST be set
 * ppStrISO  MUST be != NULL
 *
 * returns: 0 on error or if >0 size of *ppStrISO buffer (incl. trailing zero)
 */
Tst_uint32
st_utf_convStr_utf16_to_iso(const Tst_str *pUTF16, const Tst_uint32 utfSz,
		const Tst_bool checkTerm, Tst_str **ppStrISO, const Tst_utf16_bytOrd byteordIn)
{
	Tst_utf__length_t     newsz = 0;
	Tst_utf__ucs4_t       *pU   = NULL;
	Tst_utf__byte_t const *pB   = NULL;

	if (pUTF16 == NULL || ppStrISO == NULL || pUTF16 == *ppStrISO ||
			byteordIn == ST_UTF16_BYTEORDER_ANY)
		return 0;
	ST_DELPOINT(*ppStrISO)
	if (utfSz < 1 || (checkTerm && ! st_utf_isStrNullTerm_u16(pUTF16, utfSz)))
		return 0;

	/* utf16 --> ucs4 */
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf16("U16TI", pUTF16, utfSz);
#	endif
	pB = pUTF16;  /* don't change this or &pB in the next line ! */
	if (! ST_UTF__utf16_deserialize(&pB, utfSz, byteordIn, &pU))
		return 0;
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUcs("U16TI", pU);
#	endif

	/* ucs4 --> uchar */
	newsz     = ST_UTF__ucs4_length(pU) + 1;
	ST_CALLOC(*ppStrISO, Tst_str*, newsz + 2, 1)
	if (*ppStrISO == NULL) {
		ST_DELPOINT(pU)
		return 0;
	}
	ST_UTF__latin1_encode(*ppStrISO, pU);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugIso("U16TI", *ppStrISO, (Tst_uint32)newsz);
#	endif
	ST_DELPOINT(pU)
	return (Tst_uint32)newsz;
}

/*
 * Reencode UTF16-String to UTF8-String, by Tas
 *	If you want to reencode a UTF-16BE String, byteordIn==ST_UTF16_BYTEORDER_BE
 *  If you want to reencode a UTF-16(LE) String, byteordIn==ST_UTF16_BYTEORDER_LE
 *
 * pUTF16     MUST be set
 * ppStrUTF8  MUST be != NULL
 *
 * returns: 0 on error or if >0 size of *ppStrUTF8 buffer (incl. trailing zero)
 */
Tst_uint32
st_utf_convStr_utf16_to_utf8(const Tst_str *pUTF16, const Tst_uint32 utfSz,
		const Tst_bool checkTerm, Tst_str **ppStrUTF8, const Tst_utf16_bytOrd byteordIn)
{
	Tst_utf__length_t     res         = 0;
	Tst_utf__ucs4_t       *pU         = NULL;
	Tst_utf__byte_t const *pB1        = NULL;
	Tst_utf__byte_t       *pB2        = NULL;
	Tst_utf__length_t     outlenAsUT8 = 0;

	if (pUTF16 == NULL || ppStrUTF8 == NULL || pUTF16 == *ppStrUTF8 ||
			byteordIn == ST_UTF16_BYTEORDER_ANY)
		return 0;
	ST_DELPOINT(*ppStrUTF8)
	if (utfSz < 1 || (checkTerm && ! st_utf_isStrNullTerm_u16(pUTF16, utfSz)))
		return 0;

	/* utf16 --> ucs4 */
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf16("U16TU8", pUTF16, utfSz);
#	endif
	pB1 = pUTF16;  /* don't change this or &pB in the next line ! */
	if (! ST_UTF__utf16_deserialize(&pB1, utfSz, byteordIn, &pU))
		return 0;
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUcs("U16TU8", pU);
#	endif

	/* ucs4 -> utf8 */
	outlenAsUT8 = ST_UTF__ucs4_utf8size(pU);
	ST_CALLOC(*ppStrUTF8, Tst_str*, (outlenAsUT8 + 2) * sizeof(Tst_utf__utf8_t), 1)
	if (*ppStrUTF8 == NULL)
		return 0;
	pB2 = *ppStrUTF8;  /* don't change this or &pB in the next line ! */
	res = ST_UTF__utf8_serialize(&pB2, pU, 1 /* true */);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf8("U16TU8", *ppStrUTF8, (Tst_uint32)res);
#	endif
	ST_DELPOINT(pU)
	if  (res < 1)
		ST_DELPOINT(*ppStrUTF8)
	return (Tst_uint32)res;
}

/*
 * Reencode UTF16-String changing only the byteorder, by Tas
 *  For byteorder arguments see above
 *
 * pUTF16in       MUST be set
 * ppStrUTF16out  MUST be != NULL
 *
 * returns: 0 on error or if >0 size of *ppStrUTF16out buffer (incl. trailing zero)
 */
Tst_uint32
st_utf_convStr_utf16_to_utf16(const Tst_str *pUTF16in, const Tst_uint32 utfSz,
		const Tst_bool checkTerm, Tst_str **ppStrUTF16out,
		const Tst_utf16_bytOrd byteordIn, const Tst_utf16_bytOrd byteordOut)
{
	Tst_utf__length_t     res          = 0;
	Tst_utf__ucs4_t       *pU          = NULL;
	Tst_utf__byte_t const *pB1         = NULL;
	Tst_utf__byte_t       *pB2         = NULL;
	Tst_utf__length_t     outlenAsUT16 = 0;

	if (pUTF16in == NULL || ppStrUTF16out == NULL || pUTF16in == *ppStrUTF16out ||
			byteordIn == ST_UTF16_BYTEORDER_ANY || byteordOut == ST_UTF16_BYTEORDER_ANY)
		return 0;
	ST_DELPOINT(*ppStrUTF16out)
	if (utfSz < 1 || (checkTerm && ! st_utf_isStrNullTerm_u16(pUTF16in, utfSz)))
		return 0;

#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf16("U16TU16", pUTF16in, utfSz);
#	endif

	if (byteordIn == byteordOut) {
		/* just copy */
		outlenAsUT16 = ST_UTF__ucs4_utf16size(pU);
		ST_CALLOC(*ppStrUTF16out, Tst_str*,
				(outlenAsUT16 + 2 * 2) * sizeof(Tst_utf__utf16_t), 1)
		if (*ppStrUTF16out == NULL)
			return 0;
		memcpy(*ppStrUTF16out, pUTF16in, utfSz);
#		if (ST_W_UTF_DEB_ == 1)
		ST_UTF__convStr_debugUtf16("U16TU16", *ppStrUTF16out, utfSz);
#		endif
		return (Tst_uint32)utfSz;
	}

	/* utf16 --> ucs4 */
	pB1 = pUTF16in;  /* don't change this or &pB in the next line ! */
	if (! ST_UTF__utf16_deserialize(&pB1, utfSz, byteordIn, &pU)) {
		ST_DELPOINT(*ppStrUTF16out)
		return 0;
	}
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUcs("U16TU16", pU);
#	endif

	outlenAsUT16 = ST_UTF__ucs4_utf16size(pU);
	ST_CALLOC(*ppStrUTF16out, Tst_str*,
			(outlenAsUT16 + 2 * 2) * sizeof(Tst_utf__utf16_t), 1)
	if (*ppStrUTF16out == NULL)
		return 0;

	/* ucs4 -> utf16 */
	pB2 = *ppStrUTF16out;  /* don't change this or &pB in the next line ! */
	res = ST_UTF__utf16_serialize(&pB2, pU, byteordOut, 1 /* true */);
#	if (ST_W_UTF_DEB_ == 1)
	ST_UTF__convStr_debugUtf16("U16TU16", *ppStrUTF16out, (Tst_uint32)res);
#	endif

	ST_DELPOINT(pU)
	if (res < 1)
		ST_DELPOINT(*ppStrUTF16out)
	return (Tst_uint32)res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_W_UTF_DEB_ == 1)
/*
 * Debug, by Tas
 */
static void
ST_UTF__convStr_debugIso(const char *pName, const Tst_str *pISO,
		const Tst_uint32 isoSz)
{
	Tst_uint32 x;

	/**printf("\t\t%7s iso: '", pName);
	for (x = 0; x < isoSz; x++)
		if (pISO[x] >= ST_CSET_ISO_PRNSMA && pISO[x] <= ST_CSET_ISO_PRNBIG)
			printf("%c", (char)pISO[x]);
		else
			printf("[0x%02x]", (Tst_uint32)pISO[x]);**/
	printf("\t\t%7s iso: 0x'", pName);
	for (x = 0; x < isoSz; x++)
		printf("%02x ", (Tst_uint32)pISO[x]);
	printf("', sz %u\n", (Tst_uint32)isoSz);
}

static void
ST_UTF__convStr_debugUcs(const char *pName, const Tst_utf__ucs4_t *pUcs4)
{
	Tst_utf__ucs4_t const *pU = pUcs4;
	Tst_uint32            len = 0;

	printf("\t\t%7s ucs: 0x'", pName);
	do {
		printf("%08x ", (Tst_uint32)*pU);
		len++;
	} while (*pU++);
	printf("', sz %u\n", (Tst_uint32)len);
}

static void
ST_UTF__convStr_debugUtf8(const char *pName, const Tst_str *pU8,
		const Tst_uint32 u8Sz)
{
	Tst_uint32 x;

	printf("\t\t%7s u8 : '", pName);
	for (x = 0; x < u8Sz; x++)
		if (pU8[x] >= ST_CSET_ISO_PRNSMA && pU8[x] <= ST_CSET_ISO_PRNBIG)
			printf("%c", (char)pU8[x]);
		else
			printf("[0x%02x]", (Tst_uint32)pU8[x]);
	printf("', sz %u\n", (Tst_uint32)u8Sz);
}

static void
ST_UTF__convStr_debugUtf16(const char *pName, const Tst_str *pU16,
		const Tst_uint32 u16Sz)
{
	Tst_uint32 x;

	printf("\t\t%7s u16: 0x'", pName);
	for (x = 0; x < u16Sz; x++)
		printf("%02x ", (Tst_uint32)pU16[x]);
	printf("', sz %u\n", (Tst_uint32)u16Sz);
}
#endif


/*----------------------------------------------------------------------------*/
/*  Here comes the pain :-) --------------------------------------------------*/
/*     The entire following code was taken from libid3tag --------------------*/
/*----------------------------------------------------------------------------*/

#define ST_UCS4_REPLACEMENTCHAR  0x000000b7L  /* middle dot */

/*
 * NAME:	ucs4->length()
 * DESCRIPTION:	return the number of ucs4 chars represented by a ucs4 string
 */
static Tst_utf__length_t
ST_UTF__ucs4_length(Tst_utf__ucs4_t const *ucs4)
{
  Tst_utf__ucs4_t const *ptr = ucs4;

  while (*ptr)
    ++ptr;

  return ptr - ucs4;
}

/*
 * NAME:	ucs4->utf8size()
 * DESCRIPTION:	return the encoding size of a utf8-encoded ucs4 string
 */
static Tst_utf__length_t
ST_UTF__ucs4_utf8size(Tst_utf__ucs4_t const *ucs4)
{
  Tst_utf__length_t size = 0;

  while (*ucs4) {
    if (*ucs4 <= 0x0000007fL)
      size += 1;
    else if (*ucs4 <= 0x000007ffL)
      size += 2;
    else if (*ucs4 <= 0x0000ffffL)
      size += 3;
    else if (*ucs4 <= 0x001fffffL)
      size += 4;
    else if (*ucs4 <= 0x03ffffffL)
      size += 5;
    else if (*ucs4 <= 0x7fffffffL)
      size += 6;
    else
      size += 2;  /* based on U+00B7 replacement char */

    ++ucs4;
  }

  return size + 1;
}

/*
 * NAME:	ucs4->utf16size()
 * DESCRIPTION:	return the encoding size of a utf16-encoded ucs4 string
 */
static Tst_utf__length_t
ST_UTF__ucs4_utf16size(Tst_utf__ucs4_t const *ucs4)
{
  Tst_utf__length_t size = 0;

  while (*ucs4) {
    ++size;
    if (*ucs4 >= 0x00010000L &&
	*ucs4 <= 0x0010ffffL)
      ++size;

    ++ucs4;
  }

  return size + 1;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * NAME:	utf8->serialize()
 * DESCRIPTION:	serialize a ucs4 string using utf8 encoding
 */
static Tst_utf__length_t
ST_UTF__utf8_serialize(Tst_utf__byte_t **ptr,
		Tst_utf__ucs4_t const *ucs4, int terminate)
{
  Tst_utf__length_t size = 0;
  Tst_utf__utf8_t   utf8[6],
                    *out;

  while (*ucs4) {
    switch (ST_UTF__utf8_encodechar(out = utf8, *ucs4++)) {
    case 6: size += ST_UTF__utf8_put(ptr, *out++);
    case 5: size += ST_UTF__utf8_put(ptr, *out++);
    case 4: size += ST_UTF__utf8_put(ptr, *out++);
    case 3: size += ST_UTF__utf8_put(ptr, *out++);
    case 2: size += ST_UTF__utf8_put(ptr, *out++);
    case 1: size += ST_UTF__utf8_put(ptr, *out++);
    case 0: break;
    }
  }

  if (terminate)
    size += ST_UTF__utf8_put(ptr, 0);

  return size;
}

/*
 * NAME:	utf8->deserialize()
 * DESCRIPTION:	deserialize a ucs4 string using utf8 encoding
 */
static Tst_utf__ucs4_t*
ST_UTF__utf8_deserialize(Tst_utf__byte_t const **ptr,
		Tst_utf__length_t length)
{
  Tst_utf__byte_t const *end;
  Tst_utf__utf8_t *utf8ptr, *utf8;
  Tst_utf__ucs4_t *ucs4;

  end = *ptr + length;

  ST_CALLOC(utf8, Tst_utf__utf8_t*, length + 1, sizeof(Tst_utf__utf8_t))
  if (utf8 == 0)
    return 0;

  utf8ptr = utf8;
  while (end - *ptr > 0 && (*utf8ptr = ST_UTF__utf8_get(ptr)))
    ++utf8ptr;

  *utf8ptr = 0;

  ST_CALLOC(ucs4, Tst_utf__ucs4_t*,
  		ST_UTF__utf8_length(utf8, length) + 1, sizeof(Tst_utf__ucs4_t))
  if (ucs4)
    ST_UTF__utf8_decode(utf8, ucs4, length);

  ST_DELPOINT(utf8)

  return ucs4;
}

/*----------------------------------------------------------------------------*/

/*
 * NAME:	utf8->get()
 * DESCRIPTION:	deserialize a single utf8 character
 */
static Tst_utf__utf8_t
ST_UTF__utf8_get(Tst_utf__byte_t const **ptr)
{
  return *(*ptr)++;
}

/*
 * NAME:	utf8->length()
 * DESCRIPTION:	return the number of ucs4 chars represented by a utf8 string
 *
 * modified by Tas to avoid buffer overflows
 */
static Tst_utf__length_t
ST_UTF__utf8_length(Tst_utf__utf8_t const *utf8,
		const Tst_utf__length_t u8sz)
{
#	define LOC_REMSZ_  (u8sz - (utf8 - utf8Beg))
	Tst_utf__length_t length = 0;
	Tst_utf__utf8_t const *utf8Beg = utf8;

	while (*utf8) {
		if ((utf8[0] & 0x80) == 0x00)
			++length;
		else if ((utf8[0] & 0xe0) == 0xc0 && LOC_REMSZ_ >= 2 &&
				(utf8[1] & 0xc0) == 0x80) {
			if (((utf8[0] & 0x1fL) << 6) >= 0x00000080L) {
				++length;
				utf8 += 1;
			}
		}
		else if ((utf8[0] & 0xf0) == 0xe0 && LOC_REMSZ_ >= 3 &&
				(utf8[1] & 0xc0) == 0x80 &&
				(utf8[2] & 0xc0) == 0x80) {
			if ((((utf8[0] & 0x0fL) << 12) |
					((utf8[1] & 0x3fL) <<  6)) >= 0x00000800L) {
				++length;
				utf8 += 2;
			}
		}
		else if ((utf8[0] & 0xf8) == 0xf0 && LOC_REMSZ_ >= 4 &&
				(utf8[1] & 0xc0) == 0x80 &&
				(utf8[2] & 0xc0) == 0x80 &&
				(utf8[3] & 0xc0) == 0x80) {
			if ((((utf8[0] & 0x07L) << 18) |
					((utf8[1] & 0x3fL) << 12)) >= 0x00010000L) {
				++length;
				utf8 += 3;
			}
		}
		else if ((utf8[0] & 0xfc) == 0xf8 && LOC_REMSZ_ >= 5 &&
				(utf8[1] & 0xc0) == 0x80 &&
				(utf8[2] & 0xc0) == 0x80 &&
				(utf8[3] & 0xc0) == 0x80 &&
				(utf8[4] & 0xc0) == 0x80) {
			if ((((utf8[0] & 0x03L) << 24) |
					((utf8[0] & 0x3fL) << 18)) >= 0x00200000L) {
				++length;
				utf8 += 4;
			}
		}
		else if ((utf8[0] & 0xfe) == 0xfc && LOC_REMSZ_ >= 6 &&
				(utf8[1] & 0xc0) == 0x80 &&
				(utf8[2] & 0xc0) == 0x80 &&
				(utf8[3] & 0xc0) == 0x80 &&
				(utf8[4] & 0xc0) == 0x80 &&
				(utf8[5] & 0xc0) == 0x80) {
			if ((((utf8[0] & 0x01L) << 30) |
					((utf8[0] & 0x3fL) << 24)) >= 0x04000000L) {
				++length;
				utf8 += 5;
			}
		}
		++utf8;
	}
	return length;
#	undef LOC_REMSZ_
}

/*
 * NAME:	utf8->decode()
 * DESCRIPTION:	decode a complete utf8 string into a ucs4 string
 */
static void
ST_UTF__utf8_decode(Tst_utf__utf8_t const *utf8, Tst_utf__ucs4_t *ucs4,
		const Tst_utf__length_t u8sz)
{
  do
    utf8 += ST_UTF__utf8_decodechar(utf8, ucs4, u8sz);
  while (*ucs4++);
}

/*
 * NAME:	utf8->decodechar()
 * DESCRIPTION:	decode a series of utf8 chars into a single ucs4 char
 *
 * modified by Tas to avoid buffer overflows
 */
static Tst_utf__length_t
ST_UTF__utf8_decodechar(Tst_utf__utf8_t const *utf8,
		Tst_utf__ucs4_t *ucs4, const Tst_utf__length_t u8sz)
{
#	define LOC_REMSZ_  (u8sz - (utf8 - start))
	Tst_utf__utf8_t const *start = utf8;

	while (LOC_REMSZ_ > 0) {
		if ((utf8[0] & 0x80) == 0x00) {
			*ucs4 = utf8[0];
			return utf8 - start + 1;
		}
		else if ((utf8[0] & 0xe0) == 0xc0 && LOC_REMSZ_ >= 2 &&
				(utf8[1] & 0xc0) == 0x80) {
			*ucs4 = ((utf8[0] & 0x1fL) << 6) |
					((utf8[1] & 0x3fL) << 0);
			if (*ucs4 >= 0x00000080L)
				return utf8 - start + 2;
		}
		else if ((utf8[0] & 0xf0) == 0xe0 && LOC_REMSZ_ >= 3 &&
			(utf8[1] & 0xc0) == 0x80 &&
			(utf8[2] & 0xc0) == 0x80) {
			*ucs4 = ((utf8[0] & 0x0fL) << 12) |
					((utf8[1] & 0x3fL) <<  6) |
					((utf8[2] & 0x3fL) <<  0);
			if (*ucs4 >= 0x00000800L)
				return utf8 - start + 3;
		}
		else if ((utf8[0] & 0xf8) == 0xf0 && LOC_REMSZ_ >= 4 &&
				(utf8[1] & 0xc0) == 0x80 &&
				(utf8[2] & 0xc0) == 0x80 &&
				(utf8[3] & 0xc0) == 0x80) {
			*ucs4 = ((utf8[0] & 0x07L) << 18) |
					((utf8[1] & 0x3fL) << 12) |
					((utf8[2] & 0x3fL) <<  6) |
					((utf8[3] & 0x3fL) <<  0);
			if (*ucs4 >= 0x00010000L)
				return utf8 - start + 4;
		}
		else if ((utf8[0] & 0xfc) == 0xf8 && LOC_REMSZ_ >= 5 &&
				(utf8[1] & 0xc0) == 0x80 &&
				(utf8[2] & 0xc0) == 0x80 &&
				(utf8[3] & 0xc0) == 0x80 &&
				(utf8[4] & 0xc0) == 0x80) {
			*ucs4 = ((utf8[0] & 0x03L) << 24) |
					((utf8[1] & 0x3fL) << 18) |
					((utf8[2] & 0x3fL) << 12) |
					((utf8[3] & 0x3fL) <<  6) |
					((utf8[4] & 0x3fL) <<  0);
			if (*ucs4 >= 0x00200000L)
				return utf8 - start + 5;
		}
		else if ((utf8[0] & 0xfe) == 0xfc && LOC_REMSZ_ >= 6 &&
				(utf8[1] & 0xc0) == 0x80 &&
				(utf8[2] & 0xc0) == 0x80 &&
				(utf8[3] & 0xc0) == 0x80 &&
				(utf8[4] & 0xc0) == 0x80 &&
				(utf8[5] & 0xc0) == 0x80) {
			*ucs4 = ((utf8[0] & 0x01L) << 30) |
					((utf8[1] & 0x3fL) << 24) |
					((utf8[2] & 0x3fL) << 18) |
					((utf8[3] & 0x3fL) << 12) |
					((utf8[4] & 0x3fL) <<  6) |
					((utf8[5] & 0x3fL) <<  0);
			if (*ucs4 >= 0x04000000L)
				return utf8 - start + 6;
		}
		++utf8;
	}
	return 1;  /* shouldn't happen */
#	undef LOC_REMSZ_
}

/*----------------------------------------------------------------------------*/

/*
 * NAME:	utf8->encodechar()
 * DESCRIPTION:	encode a single ucs4 char into a series of up to 6 utf8 chars
 */
static Tst_utf__length_t
ST_UTF__utf8_encodechar(Tst_utf__utf8_t *utf8, Tst_utf__ucs4_t ucs4)
{
  if (ucs4 <= 0x0000007fL) {
    utf8[0] = ucs4;

    return 1;
  }
  else if (ucs4 <= 0x000007ffL) {
    utf8[0] = 0xc0 | ((ucs4 >>  6) & 0x1f);
    utf8[1] = 0x80 | ((ucs4 >>  0) & 0x3f);

    return 2;
  }
  else if (ucs4 <= 0x0000ffffL) {
    utf8[0] = 0xe0 | ((ucs4 >> 12) & 0x0f);
    utf8[1] = 0x80 | ((ucs4 >>  6) & 0x3f);
    utf8[2] = 0x80 | ((ucs4 >>  0) & 0x3f);

    return 3;
  }
  else if (ucs4 <= 0x001fffffL) {
    utf8[0] = 0xf0 | ((ucs4 >> 18) & 0x07);
    utf8[1] = 0x80 | ((ucs4 >> 12) & 0x3f);
    utf8[2] = 0x80 | ((ucs4 >>  6) & 0x3f);
    utf8[3] = 0x80 | ((ucs4 >>  0) & 0x3f);

    return 4;
  }
  else if (ucs4 <= 0x03ffffffL) {
    utf8[0] = 0xf8 | ((ucs4 >> 24) & 0x03);
    utf8[1] = 0x80 | ((ucs4 >> 18) & 0x3f);
    utf8[2] = 0x80 | ((ucs4 >> 12) & 0x3f);
    utf8[3] = 0x80 | ((ucs4 >>  6) & 0x3f);
    utf8[4] = 0x80 | ((ucs4 >>  0) & 0x3f);

    return 5;
  }
  else if (ucs4 <= 0x7fffffffL) {
    utf8[0] = 0xfc | ((ucs4 >> 30) & 0x01);
    utf8[1] = 0x80 | ((ucs4 >> 24) & 0x3f);
    utf8[2] = 0x80 | ((ucs4 >> 18) & 0x3f);
    utf8[3] = 0x80 | ((ucs4 >> 12) & 0x3f);
    utf8[4] = 0x80 | ((ucs4 >>  6) & 0x3f);
    utf8[5] = 0x80 | ((ucs4 >>  0) & 0x3f);

    return 6;
  }

  /* default */

  return ST_UTF__utf8_encodechar(utf8, ST_UCS4_REPLACEMENTCHAR);
}

/*
 * NAME:	utf8->put()
 * DESCRIPTION:	serialize a single utf8 character
 */
static Tst_utf__length_t
ST_UTF__utf8_put(Tst_utf__byte_t **ptr, Tst_utf__utf8_t utf8)
{
  if (ptr)
    *(*ptr)++ = utf8;

  return 1;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * NAME:	utf16->serialize()
 * DESCRIPTION:	serialize a ucs4 string using utf16 encoding
 */
static Tst_utf__length_t
ST_UTF__utf16_serialize(Tst_utf__byte_t **ptr,
		Tst_utf__ucs4_t const *ucs4,
		enum st_utf_utf16_byteorder byteorder, int terminate)
{
  Tst_utf__length_t size = 0;
  Tst_utf__utf16_t utf16[2], *out;

  if (byteorder == ST_UTF16_BYTEORDER_ANY)
    size += ST_UTF__utf16_put(ptr, 0xfeff, byteorder);

  while (*ucs4) {
    switch (ST_UTF__utf16_encodechar(out = utf16, *ucs4++)) {
    case 2: size += ST_UTF__utf16_put(ptr, *out++, byteorder);
    case 1: size += ST_UTF__utf16_put(ptr, *out++, byteorder);
    case 0: break;
    }
  }

  if (terminate)
    size += ST_UTF__utf16_put(ptr, 0, byteorder);

  return size;
}

/*
 * NAME:	utf16->deserialize()
 * DESCRIPTION:	deserialize a ucs4 string using utf16 encoding
 */
static int
ST_UTF__utf16_deserialize(Tst_utf__byte_t const **ptr,
		Tst_utf__length_t length,
		enum st_utf_utf16_byteorder byteorder, Tst_utf__ucs4_t **ppUcs4)
{
  Tst_utf__byte_t const *end;
  Tst_utf__utf16_t *utf16ptr, *utf16;

  end = *ptr + (length & ~1);

  ST_CALLOC(utf16, Tst_utf__utf16_t*, length / 2 + 1, sizeof(Tst_utf__utf16_t))
  if (utf16 == 0)
    return 0;

  if (byteorder == ST_UTF16_BYTEORDER_ANY && end - *ptr > 0) {
    switch (((*ptr)[0] << 8) | ((*ptr)[1] << 0)) {
    case 0xfeff:
      byteorder = ST_UTF16_BYTEORDER_BE;
      *ptr += 2;
      break;

    case 0xfffe:
      byteorder = ST_UTF16_BYTEORDER_LE;
      *ptr += 2;
      break;
    }
  }

  utf16ptr = utf16;
  while (end - *ptr > 0 && (*utf16ptr = ST_UTF__utf16_get(ptr, byteorder))) {
    ++utf16ptr;
  }

  *utf16ptr = 0;

  ST_CALLOC(*ppUcs4, Tst_utf__ucs4_t*,
  		ST_UTF__utf16_length(utf16) + 2, sizeof(Tst_utf__ucs4_t))
  if (*ppUcs4)
    ST_UTF__utf16_decode(utf16, *ppUcs4);
  else {
	  ST_DELPOINT(utf16)
	  return 0;
  }

  ST_DELPOINT(utf16)

  return 1;
}

/*----------------------------------------------------------------------------*/

/*
 * NAME:	utf16->get()
 * DESCRIPTION:	deserialize a single utf16 character
 */
static Tst_utf__utf16_t
ST_UTF__utf16_get(Tst_utf__byte_t const **ptr,
			  enum st_utf_utf16_byteorder byteorder)
{
  Tst_utf__utf16_t utf16;

  switch (byteorder) {
  case ST_UTF16_BYTEORDER_LE:
    utf16 =
      ((*ptr)[0] << 0) |
      ((*ptr)[1] << 8);
    break;
  default:
  /*case ST_UTF16_BYTEORDER_BE:*/
    utf16 =
      ((*ptr)[0] << 8) |
      ((*ptr)[1] << 0);
    /*break;*/
  }

  *ptr += 2;

  return utf16;
}

/*
 * NAME:	utf16->length()
 * DESCRIPTION:	return the number of ucs4 chars represented by a utf16 string
 */
static Tst_utf__length_t
ST_UTF__utf16_length(Tst_utf__utf16_t const *utf16)
{
  Tst_utf__length_t length = 0;

  while (*utf16) {
    if (utf16[0] < 0xd800 || utf16[0] > 0xdfff)
      ++length;
    else if (utf16[0] >= 0xd800 && utf16[0] <= 0xdbff &&
	     utf16[1] >= 0xdc00 && utf16[1] <= 0xdfff) {
      ++length;
      ++utf16;
    }

    ++utf16;
  }

  return length;
}

/*
 * NAME:	utf16->decode()
 * DESCRIPTION:	decode a complete utf16 string into a ucs4 string
 */
static void
ST_UTF__utf16_decode(Tst_utf__utf16_t const *utf16, Tst_utf__ucs4_t *ucs4)
{
  do {
    utf16 += ST_UTF__utf16_decodechar(utf16, ucs4);
  } while (*ucs4++);
}

/*
 * NAME:	utf16->decodechar()
 * DESCRIPTION:	decode a series of utf16 chars into a single ucs4 char
 */
static Tst_utf__length_t
ST_UTF__utf16_decodechar(Tst_utf__utf16_t const *utf16,
		Tst_utf__ucs4_t *ucs4)
{
  Tst_utf__utf16_t const *start = utf16;

  while (1) {
    if (utf16[0] < 0xd800 || utf16[0] > 0xdfff) {
      *ucs4 = utf16[0];
      return utf16 - start + 1;
    }
    else if (utf16[0] >= 0xd800 && utf16[0] <= 0xdbff &&
	     utf16[1] >= 0xdc00 && utf16[1] <= 0xdfff) {
      *ucs4 = (((utf16[0] & 0x03ffL) << 10) |
	       ((utf16[1] & 0x03ffL) <<  0)) + 0x00010000L;
      return utf16 - start + 2;
    }

    ++utf16;
  }
}

/*----------------------------------------------------------------------------*/

/*
 * NAME:	utf16->put()
 * DESCRIPTION:	serialize a single utf16 character
 */
static Tst_utf__length_t
ST_UTF__utf16_put(Tst_utf__byte_t **ptr, Tst_utf__utf16_t utf16,
			   enum st_utf_utf16_byteorder byteorder)
{
  if (ptr) {
    switch (byteorder) {
    case ST_UTF16_BYTEORDER_LE:
      (*ptr)[0] = (utf16 >> 0) & 0xff;
      (*ptr)[1] = (utf16 >> 8) & 0xff;
      break;
    default:
    /*case ST_UTF16_BYTEORDER_BE:*/
      (*ptr)[0] = (utf16 >> 8) & 0xff;
      (*ptr)[1] = (utf16 >> 0) & 0xff;
      /*break;*/
    }

    *ptr += 2;
  }

  return 2;
}

/*
 * NAME:	utf16->encodechar()
 * DESCRIPTION:	encode a single ucs4 char into a series of up to 2 utf16 chars
 */
static Tst_utf__length_t
ST_UTF__utf16_encodechar(Tst_utf__utf16_t *utf16,
		Tst_utf__ucs4_t ucs4)
{
  if (ucs4 < 0x00010000L) {
    utf16[0] = ucs4;

    return 1;
  }
  else if (ucs4 < 0x00110000L) {
    ucs4 -= 0x00010000L;

    utf16[0] = ((ucs4 >> 10) & 0x3ff) | 0xd800;
    utf16[1] = ((ucs4 >>  0) & 0x3ff) | 0xdc00;

    return 2;
  }

  /* default */

  return ST_UTF__utf16_encodechar(utf16, ST_UCS4_REPLACEMENTCHAR);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * NAME:	latin1->decode()
 * DESCRIPTION:	decode a complete latin1 string into a ucs4 string
 */
static void
ST_UTF__latin1_decode(Tst_utf__latin1_t const *latin1, Tst_utf__ucs4_t *ucs4)
{
  do
    latin1 += ST_UTF__latin1_decodechar(latin1, ucs4);
  while (*ucs4++);
}

/*
 * NAME:	latin1->decodechar()
 * DESCRIPTION:	decode a (single) latin1 char into a single ucs4 char
 */
static Tst_utf__length_t
ST_UTF__latin1_decodechar(Tst_utf__latin1_t const *latin1,
		Tst_utf__ucs4_t *ucs4)
{
  *ucs4 = *latin1;

  return 1;
}

/*
 * NAME:	latin1->encode()
 * DESCRIPTION:	encode a complete ucs4 string into a latin1 string
 */
static void
ST_UTF__latin1_encode(Tst_utf__latin1_t *latin1, Tst_utf__ucs4_t const *ucs4)
{
  do
    latin1 += ST_UTF__latin1_encodechar(latin1, *ucs4);
  while (*ucs4++);
}

/*
 * NAME:	latin1->encodechar()
 * DESCRIPTION:	encode a single ucs4 char into a (single) latin1 char
 */
static Tst_utf__length_t
ST_UTF__latin1_encodechar(Tst_utf__latin1_t *latin1,
		Tst_utf__ucs4_t ucs4)
{
  *latin1 = ucs4;
  if (ucs4 > 0x000000ffL)
    *latin1 = ST_UCS4_REPLACEMENTCHAR;

  return 1;
}

/******************************************************************************/
