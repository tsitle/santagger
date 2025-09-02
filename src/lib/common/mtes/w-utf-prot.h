/******************************************************************************/
/* w-utf-prot.h                 [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for UTF string handling                                */
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

#ifndef LIBSANTAGGER_W_UTF_PROT_H
#define LIBSANTAGGER_W_UTF_PROT_H

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_W_UTF_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*******************************************************************************
 * Types (protected)
 ******************************************************************************/

#if defined(SRC_W_UTF_C) || defined(SRC_STRING_MTE_C)
	/** */
	enum st_utf_utf16_byteorder {  /* from libid3tag */
		ST_UTF16_BYTEORDER_ANY,
		ST_UTF16_BYTEORDER_BE,
		ST_UTF16_BYTEORDER_LE
	};
	typedef enum st_utf_utf16_byteorder Tst_utf16_bytOrd;  /* by Tas */
#endif  /* protected */


/*******************************************************************************
 * Functions (protected)
 ******************************************************************************/

#if defined(SRC_W_UTF_C) || defined(SRC_STRING_MTE_C)
	/*
	 * Check whether string is NUL-terminated, by Tas
	 */
	Tst_bool
	st_utf_isStrNullTerm_iso_u8(const Tst_str *pStr, const Tst_uint32 sz);

	Tst_bool
	st_utf_isStrNullTerm_u16(const Tst_str *pStr, const Tst_uint32 sz);

	/*
	 * Get length of string in chars, NOT the buffer size, by Tas
	 *
	 * returns: length
	 */
	Tst_uint32
	st_utf_strlen_utf8(const Tst_str *pUTF8,
	                   const Tst_uint32 utfSz,
	                   const Tst_bool checkTerm);

	/*
	 * Encode ISO-String to UTF8-String, by Tas
	 *
	 * returns: 0 on error or if >0 size of *ppStrUTF8 buffer (incl. trailing zeros)
	 */
	Tst_uint32
	st_utf_convStr_iso_to_utf8(const Tst_str *pISO,
	                           const Tst_uint32 isoSz,
	                           const Tst_bool checkTerm,
	                           Tst_str **ppStrUTF8);

	/*
	 * Decode UTF8-String to ISO-String, by Tas
	 *
	 * returns: 0 on error or if >0 size of *ppStrISO buffer (incl. trailing zero)
	 */
	Tst_uint32
	st_utf_convStr_utf8_to_iso(const Tst_str *pUTF8,
	                           const Tst_uint32 utfSz,
	                           const Tst_bool checkTerm,
	                           Tst_str **ppStrISO);

	/*
	 * Reencode UTF8-String as UTF16-String, by Tas
	 *	If you want to reencode as UTF-16BE String, byteordOut==ST_UTF16_BYTEORDER_BE
	 *  If you want to reencode as UTF-16(LE) String, byteordOut==ST_UTF16_BYTEORDER_LE
	 *
	 * returns: 0 on error or if >0 size of *ppStrUTF16 buffer (incl. trailing zero)
	 */
	Tst_uint32
	st_utf_convStr_utf8_to_utf16(const Tst_str *pUTF8,
	                             const Tst_uint32 utfSz,
	                             const Tst_bool checkTerm,
	                             Tst_str **ppStrUTF16,
	                             const Tst_utf16_bytOrd byteordOut);

	/*
	 * Encode ISO-String to UTF16-String, by Tas
	 *	If you want to encode a UTF-16BE String, byteordOut==ST_UTF16_BYTEORDER_BE
	 *  If you want to encode a UTF-16(LE) String, byteordOut==ST_UTF16_BYTEORDER_LE
	 *
	 * returns: 0 on error or if >0 size of *ppStrUTF16 buffer (incl. trailing zeros)
	 */
	Tst_uint32
	st_utf_convStr_iso_to_utf16(const Tst_str *pISO,
	                            const Tst_uint32 isoSz,
	                            const Tst_bool checkTerm,
	                            Tst_str **ppStrUTF16,
	                            const Tst_utf16_bytOrd byteordOut);

	/*
	 * Decode UTF16-String to ISO-String, by Tas
	 *	If you want to decode a UTF-16BE String, byteordIn==ST_UTF16_BYTEORDER_BE
	 *  If you want to decode a UTF-16(LE) String, byteordIn==ST_UTF16_BYTEORDER_LE
	 *
	 * returns: 0 on error or if >0 size of *ppStrISO buffer (incl. trailing zero)
	 */
	Tst_uint32
	st_utf_convStr_utf16_to_iso(const Tst_str *pUTF16,
	                            const Tst_uint32 utfSz,
	                            const Tst_bool checkTerm,
	                            Tst_str **ppStrISO,
	                            const Tst_utf16_bytOrd byteordIn);

	/*
	 * Reencode UTF16-String as UTF8-String, by Tas
	 *	If you want to reencode a UTF-16BE String, byteordIn==ST_UTF16_BYTEORDER_BE
	 *  If you want to reencode a UTF-16(LE) String, byteordIn==ST_UTF16_BYTEORDER_LE
	 *
	 * returns: 0 on error or if >0 size of *ppStrUTF8 buffer (incl. trailing zero)
	 */
	Tst_uint32
	st_utf_convStr_utf16_to_utf8(const Tst_str *pUTF16,
	                             const Tst_uint32 utfSz,
	                             const Tst_bool checkTerm,
	                             Tst_str **ppStrUTF8,
	                             const Tst_utf16_bytOrd byteordIn);

	/*
	 * Reencode UTF16-String changing only the byteorder, by Tas
	 *  For byteorder arguments see above
	 *
	 * returns: 0 on error or if >0 size of *ppStrUTF16out buffer (incl. trailing zero)
	 */
	Tst_uint32
	st_utf_convStr_utf16_to_utf16(const Tst_str *pUTF16in,
	                              const Tst_uint32 utfSz,
	                              const Tst_bool checkTerm,
	                              Tst_str **ppStrUTF16out,
	                              const Tst_utf16_bytOrd byteordIn,
	                              const Tst_utf16_bytOrd byteordOut);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_W_UTF_PROT_H */

/******************************************************************************/
