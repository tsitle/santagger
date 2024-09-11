/******************************************************************************/
/* stream-prot.h                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected definitions for the Stream Reader / Writer                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_STREAM_PROT_H
#define HEAD_STREAM_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_file-defs.h"


ST_BEGIN_C_DECLS

/*
// Constants (protected)
*/
#if defined(SRC_STREAMRD_C) || defined(SRC_STREAMWR_C) || \
			defined(SRC_STREAM_CRCS_C)
	/** leftover bits mask */
	static const Tst_byte ST_STREAM_LOBITS_MASK_8[9] = {
			0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
	/*static const Tst_uint32 ST_STREAM_LOBITS_MASK_32[33] = {
			0x00000000,
			0x00000001, 0x00000003, 0x00000007, 0x0000000f,
			0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
			0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
			0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
			0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
			0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
			0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
			0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff };*/

	/** CRC-8, poly = x^8 + x^2 + x^1 + x^0, init = 0 */
	extern const Tst_byte ST_STREAM_CRC8_TABLE[256];

	/** CRC-16, poly = x^16 + x^15 + x^2 + x^0, init = 0 */
	extern const Tst_uint16 ST_STREAM_CRC16_TABLE[256];

	/** CRC-32, polynomial=0x04c11db7 */
	extern const Tst_uint32 ST_STREAM_CRC32_TABLE[256];
#endif  /* protected */


/*
// Types (protected)
*/
#if defined(SRC_STREAMRD_C) || defined(SRC_STREAMWR_C) || \
			defined(SRC_STREAM_STCS_C) || defined(SRC_STREAM_CRCS_C)
	/** */
	typedef struct {
		Tst_buf       *pIntnBuf;
		Tst_uint32    intnBufSz;
		/* */
		Tst_buf       *pBBeg;        /* beginning of the buffer */
		Tst_buf       *pBCur;        /* current position in buffer */
		Tst_buf const *pBEnd;        /* end of buffer (^= pBuf + sz) */
		Tst_uint32    sz;            /* size of buffer that pBBeg points to */
		Tst_bool      wasAllocd;     /* were pIntnBufNew/Cur buffers alloc'd ? */
		Tst_byte      leftoverBits;  /* unused bits of *pBCur (0-8) */
		/* */
		Tst_buf       *pBCurRestore; /* for read-ahead functions */
	} Tst_stream_buf;

	/** */
	typedef struct {
		Tst_fsize    fsz;     /* size of file */
		Tst_foffs    cpos;    /* current position in file */
		Tst_sys_fstc *pFStc;  /* input file descriptor etc. */
	} Tst_stream_file;

	/** */
	typedef struct {
		Tst_bool   compCRC8;   /* compute CRC-8 ? */
		Tst_bool   pausCRC8;   /* computation of CRC-8 paused ? */
		Tst_bool   compCRC16;  /* compute CRC-16 ? */
		Tst_bool   pausCRC16;  /* computation of CRC-16 paused ? */
		Tst_bool   compCRC32;  /* compute CRC-32 ? */
		Tst_bool   pausCRC32;  /* computation of CRC-32 paused ? */
		/* */
		Tst_byte   crc8;       /* CRC-8 */
		Tst_uint16 crc16;      /* CRC-16 */
		Tst_uint32 crc32;      /* CRC-32 */
	} Tst_stream_crcs;

	/** for read-ahead functions */
	typedef struct {
		Tst_err    res;
		Tst_byte   lobits;
		Tst_uint32 tbir;
		Tst_fsize  tbyr;
		Tst_bool   compCRC[3];
	} Tst_streamrd_aheadVars;

	/** Stream Reader */
	typedef struct {
		Tst_stream_buf  sbuf;  /* data buffer */
		Tst_stream_file sfil;  /* file for I/O */
		/* */
		Tst_stream_crcs crcs;  /* checksums */
		/* */
		Tst_fsize  totBytesRd;     /* total amount of read bytes */
		Tst_uint32 totBitsRd_rem;  /* total amount of read bits - remainder */
		/* */
		Tst_streamrd_aheadVars aheadVars;
	} Tst_streamrd_intn;

	/** Stream Writer */
	typedef struct {
		Tst_stream_buf  sbuf;  /* data buffer */
		Tst_stream_file sfil;  /* file for I/O */
		/* */
		Tst_stream_crcs crcs;  /* checksums */
		/* */
		Tst_fsize  totBytesWr;     /* total amount of written bytes */
		Tst_uint32 totBitsWr_rem;  /* total amount of written bits - remainder */
	} Tst_streamwr_intn;
#endif  /* protected */


/*
// Macros (protected)
*/
#if defined(SRC_STREAMRD_C) || defined(SRC_STREAMWR_C) || \
			defined(SRC_STREAM_CRCS_C)
	/** check if buffer is set */
#	define ST_STREAM_CHECK(mac_pSObj)  \
				((mac_pSObj) != NULL && (mac_pSObj)->pObInternal != NULL && \
					((Tst_streamrd_intn*)mac_pSObj->pObInternal)->sbuf.pBCur != NULL && \
					((Tst_streamrd_intn*)mac_pSObj->pObInternal)->sbuf.pBEnd != NULL)

	/** return remaining bytes in buffer */
#	define ST_STREAM_LOBYTES(mac_pSOI)  \
				((mac_pSOI)->sbuf.pBCur >= (mac_pSOI)->sbuf.pBEnd ? 0 : \
					(Tst_uint32)((mac_pSOI)->sbuf.pBEnd - (mac_pSOI)->sbuf.pBCur))

	/** return size of buffer */
#	define ST_STREAM_BUFSIZE(mac_pSOI)  \
				(Tst_uint32)((mac_pSOI)->sbuf.pBEnd - (mac_pSOI)->sbuf.pBBeg)

	/** update CRC-8 */
#	define ST_STREAM_UPDATE_CRC8(mac_crcs, mac_byte)  \
				(mac_crcs).crc8 = \
					ST_STREAM_CRC8_TABLE[(mac_crcs).crc8 ^ (mac_byte)];

	/** update CRC-16 */
#	define ST_STREAM_UPDATE_CRC16(mac_crcs, mac_byte)  \
				(mac_crcs).crc16 = \
					(((mac_crcs).crc16 << 8) ^ \
						ST_STREAM_CRC16_TABLE[((mac_crcs).crc16 >> 8) ^ \
							(mac_byte)]) & 0xffff;

	/** update CRC-32 */
#	define ST_STREAM_UPDATE_CRC32(mac_crcs, mac_byte)  \
				(mac_crcs).crc32 = \
					((mac_crcs).crc32 << 8) ^ \
						ST_STREAM_CRC32_TABLE[((mac_crcs).crc32 >> 24) ^ \
							(mac_byte)];

	/** update CRCs */
#	define ST_STREAM_UPDCRC(mac_pSOI)  \
				if ((mac_pSOI)->sbuf.pBCur < (mac_pSOI)->sbuf.pBEnd) { \
					if ((mac_pSOI)->crcs.compCRC8) \
						ST_STREAM_UPDATE_CRC8(pSOI->crcs, *(mac_pSOI)->sbuf.pBCur) \
					if ((mac_pSOI)->crcs.compCRC16) \
						ST_STREAM_UPDATE_CRC16(pSOI->crcs, *(mac_pSOI)->sbuf.pBCur) \
					if ((mac_pSOI)->crcs.compCRC32) \
						ST_STREAM_UPDATE_CRC32(pSOI->crcs, *(mac_pSOI)->sbuf.pBCur) \
				}

	/** ENDIAN: swap byte order */ \
#	define ST_STREAM_SWAP_BO_UI16(mac_pInOutUI16)  { \
				Tst_uint16 mac_tmpSBOU16; \
				\
				((Tst_byte*)&mac_tmpSBOU16)[0] = ((Tst_byte*)(mac_pInOutUI16))[1]; \
				((Tst_byte*)&mac_tmpSBOU16)[1] = ((Tst_byte*)(mac_pInOutUI16))[0]; \
				*(mac_pInOutUI16) = mac_tmpSBOU16; \
				}
#	define ST_STREAM_SWAP_BO_UI32_FROMFLAC(mac_pInOutUI32)  { \
				Tst_uint32 mac_tmpSBOU32; \
				/* from libflac -- macrodized by Tas: */ \
				mac_tmpSBOU32 = *(mac_pInOutUI32); \
				mac_tmpSBOU32 = ((mac_tmpSBOU32 << 8) & 0xff00ff00) | \
						((mac_tmpSBOU32 >> 8) & 0x00ff00ff); \
				*(mac_pInOutUI32) = (mac_tmpSBOU32 >> 16) | (mac_tmpSBOU32 << 16); \
				}
#	define ST_STREAM_SWAP_BO_UI32_SIMPLE(mac_pInOutUI32)  { \
				/* by Tas: */ \
				((Tst_byte*)&mac_tmpSBOU32)[0] = ((Tst_byte*)(mac_pInOutUI32))[3]; \
				((Tst_byte*)&mac_tmpSBOU32)[1] = ((Tst_byte*)(mac_pInOutUI32))[2]; \
				((Tst_byte*)&mac_tmpSBOU32)[2] = ((Tst_byte*)(mac_pInOutUI32))[1]; \
				((Tst_byte*)&mac_tmpSBOU32)[3] = ((Tst_byte*)(mac_pInOutUI32))[0]; \
				*(mac_pInOutUI32) = mac_tmpSBOU32; \
				}
#	define ST_STREAM_SWAP_BO_UI32(mac_pInOutUI32)  \
				ST_STREAM_SWAP_BO_UI32_FROMFLAC(mac_pInOutUI32)
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_STREAM_PROT_H */

/******************************************************************************/
