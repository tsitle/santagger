/******************************************************************************/
/* sys_fnc.h                    [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* A few system functions                                                     */
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

#ifndef HEAD_SYS_FNC_H
#define HEAD_SYS_FNC_H

/*
// Own-Includes
*/
#include "../lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Functions
*/
/**
 * Get string length (assuming string is ISO-encoded)
 *
 * @param pStrISO String in ISO-encoding (can be NULL)
 * @return Length of string
 */
ST_EXPORT Tst_uint32
st_sysStrlen(ST_OPTARG(const Tst_str *pStrISO));

#define st_sysStrlen2(mac_pStrISO)    st_sysStrlen((Tst_str*)(mac_pStrISO))
#define st_sysStrEmpty(mac_pStrISO)   ((mac_pStrISO) == NULL || (mac_pStrISO)[0] == 0x00)
#define st_sysStrEmpty2(mac_pStrISO)  st_sysStrEmpty(mac_pStrISO)

// --------------------------------

/**
 * Copy string
 *
 * @param pSrc Source string in ISO-encoding (can be NULL)
 * @param ppDst Destination string (memory for *ppDst will be re-allocated)
 * @return Error code
 */
ST_EXPORT Tst_err
st_sysStrcpy(ST_OPTARG(const Tst_str *pSrc), Tst_str **ppDst);

#define st_sysStrcpy2(mac_pSrc, mac_ppDst)  \
				st_sysStrcpy((const Tst_str*)mac_pSrc, (Tst_str**)mac_ppDst)

// --------------------------------

/**
 * Append one string to another
 *
 * @param pSrc Source string in ISO-encoding (can be NULL)
 * @param ppDst Destination string (memory for ppDst will be re-allocated)
 * @return Error code
 */
ST_EXPORT Tst_err
st_sysStrapp(ST_OPTARG(const Tst_str *pSrc), Tst_str **ppDst);

// --------------------------------

/**
 * Convert string to uppercase
 *
 * @param pStr String to convert
 */
ST_EXPORT void
st_sysStrToUpper(ST_OPTARG(Tst_str *pStr));

/**
 * Convert string to lowercase
 *
 * @param pStr String to convert
 */
ST_EXPORT void
st_sysStrToLower(ST_OPTARG(Tst_str *pStr));

/**
 * Convert character to uppercase
 *
 * @param ch Character to convert
 * @return Character
 */
ST_EXPORT Tst_str
st_sysCharToUpper(const Tst_str ch);

/**
 * Convert character to lowercase
 *
 * @param ch Character to convert
 * @return Character
 */
ST_EXPORT Tst_str
st_sysCharToLower(const Tst_str ch);

// --------------------------------

/**
 * Compare two strings
 *
 * @param caseSensitive Compare case-sensitive?
 * @param pStr1 First string
 * @param pStr2 Second string
 * @return True if equal
 */
ST_EXPORT Tst_bool
st_sysStrcmp(
		const Tst_bool caseSensitive,
		ST_OPTARG(const Tst_str *pStr1),
		ST_OPTARG(const Tst_str *pStr2)
	);

#define st_sysStrcmp2(mac_cS, mac_pStr1, mac_pStr2)  \
				st_sysStrcmp(mac_cS, (const Tst_str*)mac_pStr1, \
						(const Tst_str*)mac_pStr2)

/**
 * Compare two strings - but only the first n characters
 *
 * @param caseSensitive Compare case-sensitive?
 * @param maxlen Maximum length to compare
 * @param pStr1 First string
 * @param pStr2 Second string
 * @return True if equal
 */
ST_EXPORT Tst_bool
st_sysStrcmpN(
		const Tst_bool caseSensitive,
		const Tst_uint32 maxlen,
		ST_OPTARG(const Tst_str *pStr1),
		ST_OPTARG(const Tst_str *pStr2)
	);

#define st_sysStrcmpN2(mac_cS, mac_ml, mac_pStr1, mac_pStr2)  \
				st_sysStrcmpN(mac_cS, mac_ml, (const Tst_str*)mac_pStr1, \
						(const Tst_str*)mac_pStr2)

// --------------------------------

/**
 * Reverse Byte-Order for 'short' 2-byte type
 *
 * @param val Value
 * @return Reversed value
 */
ST_EXPORT Tst_uint16
st_sysReverseByteOrder_UI16(const Tst_uint16 val);

/**
 * Reverse Byte-Order for 'int' 4-byte type
 *
 * @param val Value
 * @return Reversed value
 */
ST_EXPORT Tst_uint32
st_sysReverseByteOrder_UI32(const Tst_uint32 val);

/**
 * Reverse Byte-Order for 8-byte buffer type
 *
 * @param pVal Buffer
 * @param valSz Size of buffer (must be 8)
 * @return Reversed buffer
 */
ST_EXPORT Tst_buf*
st_sysReverseByteOrder_LL(Tst_buf *pVal, const Tst_uint32 valSz);

// --------------------------------

/**
 * Get system time
 *
 * @return System time as double
 */
ST_EXPORT double
st_sysGetTime(void);

/**
 * Put current process to sleep
 *
 * @param millisecs Milliseconds to sleep for
 */
ST_EXPORT void
st_sysSleepMS(const Tst_uint32 millisecs);

// --------------------------------

/**
 * Initialize Pseudo-Random-Number generator.
 * The PRNG is initialized with the current time, but you can use an additional seed
 *
 * @param optionalSeed Optional seed value
 */
ST_EXPORT void
st_sysInitRand(const Tst_uint32 optionalSeed);

/**
 * Get Pseudo-Random-Number as integer
 *
 * @param min Minimum output number
 * @param max Maximum output number
 * @return Pseudo-Random-Number between min and max ( min <= x <= max )
 */
ST_EXPORT Tst_uint32
st_sysGetRand(const Tst_uint32 min, const Tst_uint32 max);

/**
 * Get Pseudo-Random-Number as double
 *
 * @param min Minimum output number
 * @param max Maximum output number
 * @return Pseudo-Random-Number between min and max ( min <= x <= max )
 */
ST_EXPORT double
st_sysGetRandDbl(const double min, const double max);

// --------------------------------

/**
 * Search for a buffer inside another buffer
 *
 * @param pBuf Buffer to search in
 * @param bSz Size of pBuf
 * @param pSearch Buffer to search for
 * @param sSz Size of pSearch
 * @param pOffs Offset from which to start searching (can be NULL)
 * @return True if found
 */
ST_EXPORT Tst_bool
st_sysBufSearch(
		const Tst_buf *pBuf,
		const Tst_uint32 bSz,
		const Tst_buf *pSearch,
		const Tst_uint32 sSz,
		ST_OPTARG(Tst_uint32 *pOffs)
	);


/*
// Macros
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
	ST_EXPORT extern const Tst_uint16 st_sysfnc_g_bigEndTest;
	/** tests whether system is Big-Endian */
	#define ST_SYSFNC_ISBIGEND  ((*(char*)&st_sysfnc_g_bigEndTest) != 0x21)
#endif  /* CONFIG_ST_ALL_DEBUG_ADD */


ST_END_C_DECLS

#endif  /* HEAD_SYS_FNC_H */

/******************************************************************************/
