/******************************************************************************/
/* streamwr-priv.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private functions/definitions for the Stream Writer                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 18.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_STREAMWR_PRIV_H
#define HEAD_STREAMWR_PRIV_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_STREAMWR_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "stream-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (private)
*/
#if defined(SRC_STREAMWR_C)
	/** */
	static const Tst_uint32 ST_STREAMWR_DEFBUFSZ = 4 * 1024;
#endif  /* private */


/*
// Functions (private)
*/
#if defined(SRC_STREAMWR_C)
	/** */
	static Tst_err
	ST_STREAMWR__flushOutpFile(Tst_streamwr_intn *pSOI);
	/** */
	static Tst_err
	ST_STREAMWR__wrBuffer_pr(Tst_streamwr *pSObj,
	                         const Tst_uint32 bufSz,
	                         const Tst_buf *pBuf,
	                         const Tst_bool withCRCs);

	/** */
#	if (ST_STREAMWR_DEB_ == 1)
	static void ST_STREAMWR__prf(const char *pFmt, ...);
	static void ST_STREAMWR__prE(const char *pFmt, ...);
	static void ST_STREAMWR__prBitsInByte(const Tst_byte byt);
#	endif
#endif  /* private */


/*
// Macros (private)
*/
#if defined(SRC_STREAMWR_C)
	/** increase buffer position */
#	define ST_STREAMWR__NOCRC_INCPOS_RET(mac_res, mac_pSOI)  { \
				if ((mac_pSOI)->sbuf.pBCur + 1 >= (mac_pSOI)->sbuf.pBEnd) { \
					if ((mac_pSOI)->sfil.pFStc == NULL) { \
						(mac_res) = st_stream_stc_reszIntnBuf(&(mac_pSOI)->sbuf, \
								(mac_pSOI)->sbuf.intnBufSz + ST_STREAMWR_DEFBUFSZ, 0); \
						if ((mac_res) != ST_ERR_SUCC) \
							return (mac_res); \
						++(mac_pSOI)->sbuf.pBCur; \
						(mac_pSOI)->sbuf.pBEnd = (mac_pSOI)->sbuf.pBBeg + \
								(mac_pSOI)->sbuf.intnBufSz; \
					} else { \
						++(mac_pSOI)->sbuf.pBCur; \
						(mac_res) = ST_STREAMWR__flushOutpFile(mac_pSOI); \
						if ((mac_res) != ST_ERR_SUCC) \
							return (mac_res); \
					} \
				} else { \
					++(mac_pSOI)->sbuf.pBCur; \
					/* !!! */ /*(mac_pSOI)->sbuf.leftoverBits = 8;*/ \
				} \
				}
#	define ST_STREAMWR__UPDCRC_INCPOS_RET(mac_res, mac_pSOI)  { \
				ST_STREAM_UPDCRC(mac_pSOI) \
				ST_STREAMWR__NOCRC_INCPOS_RET(mac_res, mac_pSOI) \
				}

	/** increase written bits */
#	define ST_STREAMWR__INCREASE_WRITBITS(mac_pSOI)  { \
				(mac_pSOI)->totBytesWr += \
						(((mac_pSOI)->totBitsWr_rem - \
							((mac_pSOI)->totBitsWr_rem % 8)) / 8); \
				(mac_pSOI)->totBitsWr_rem = (mac_pSOI)->totBitsWr_rem % 8; \
				}
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_STREAMWR_PRIV_H */

/******************************************************************************/
