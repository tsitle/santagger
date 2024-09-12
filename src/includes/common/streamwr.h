/******************************************************************************/
/* streamwr.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for the Stream Writer                                            */
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

#ifndef HEAD_STREAMWR_H
#define HEAD_STREAMWR_H

/*
// Own-Includes
*/
#include "sys_file-defs.h"
#include "sys_math-defs.h"
#include "streamwr-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- streamwr.c
*/
/** Set Output Destination */
/*** */
ST_EXPORT Tst_err
st_streamwr_setOutput_buffer(Tst_streamwr *pSObj);
/*** */
ST_EXPORT Tst_err
st_streamwr_setOutput_file(Tst_streamwr *pSObj, Tst_sys_fstc *pFStc);

/** Get status info */
ST_EXPORT Tst_foffs
st_streamwr_getCurPos(const Tst_streamwr *pSObj);

ST_EXPORT Tst_uint32
st_streamwr_getOutputBufSize(const Tst_streamwr *pSObj);

ST_EXPORT Tst_fsize
st_streamwr_getOutputFileSize(const Tst_streamwr *pSObj);

ST_EXPORT Tst_byte
st_streamwr_getAmountRemainingBitsInCurByte(const Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_getAmountBitsWritten(const Tst_streamwr *pSObj,
                                 Tst_fsize *pBytes,
                                 Tst_byte *pBitsMod8);

ST_EXPORT Tst_fsize
st_streamwr_getAmountBytesWritten(const Tst_streamwr *pSObj);

/** Get Output Buffer */
ST_EXPORT const Tst_buf*
st_streamwr_getOutputBuffer(const Tst_streamwr *pSObj);

/** If output is a file: write all remaining data to output
 ** If output is a buffer: reset buffer  */
ST_EXPORT Tst_err
st_streamwr_flush(Tst_streamwr *pSObj);

/** CRC functions */
/*** */
ST_EXPORT void
st_streamwr_startCRC8(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_pauseOrContinueCRC8(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_updateCRC8(Tst_streamwr *pSObj,
                       const Tst_buf *pBuf,
                       const Tst_uint32 sz);

ST_EXPORT Tst_byte
st_streamwr_getCRC8(const Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_stopCRC8(Tst_streamwr *pSObj);
/*** */
ST_EXPORT void
st_streamwr_startCRC16(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_pauseOrContinueCRC16(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_updateCRC16(Tst_streamwr *pSObj,
                        const Tst_buf *pBuf,
                        const Tst_uint32 sz);

ST_EXPORT Tst_uint16
st_streamwr_getCRC16(const Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_stopCRC16(Tst_streamwr *pSObj);
/*** */
ST_EXPORT void
st_streamwr_startCRC32(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_pauseOrContinueCRC32(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_updateCRC32(Tst_streamwr *pSObj,
                        const Tst_buf *pBuf,
                        const Tst_uint32 sz);

ST_EXPORT Tst_uint32
st_streamwr_getCRC32(const Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_stopCRC32(Tst_streamwr *pSObj);

/** Write Values */
/*** fixed length values */
ST_EXPORT Tst_err
st_streamwr_wrByte(Tst_streamwr *pSObj,
                   const Tst_byte nBits,
                   Tst_byte val);
ST_EXPORT Tst_err
st_streamwr_wrByte_noCRC(Tst_streamwr *pSObj,
                         const Tst_byte nBits,
                         Tst_byte val);
ST_EXPORT Tst_err
st_streamwr_wrUInt16(Tst_streamwr *pSObj,
                     const Tst_streamwr_endian outpEndian,
                     const Tst_byte nBits,
                     const Tst_uint16 val);
ST_EXPORT Tst_err
st_streamwr_wrInt16(Tst_streamwr *pSObj,
                    const Tst_streamwr_endian outpEndian,
                    const Tst_byte nBits,
                    const Tst_int16 val);
ST_EXPORT Tst_err
st_streamwr_wrUInt32(Tst_streamwr *pSObj,
                     const Tst_streamwr_endian outpEndian,
                     Tst_byte nBits,
                     Tst_uint32 val);
ST_EXPORT Tst_err
st_streamwr_wrInt32(Tst_streamwr *pSObj,
                    const Tst_streamwr_endian outpEndian,
                    const Tst_byte nBits,
                    const Tst_int32 val);
ST_EXPORT Tst_err
st_streamwr_wrUInt64(Tst_streamwr *pSObj,
                     const Tst_streamwr_endian outpEndian,
                     Tst_byte nBits,
                     const Tst_uint64 *pVal);
ST_EXPORT Tst_err
st_streamwr_wrInt64(Tst_streamwr *pSObj,
                    const Tst_streamwr_endian outpEndian,
                    const Tst_byte nBits,
                    const Tst_int64 *pVal);
ST_EXPORT Tst_err
st_streamwr_wrBuffer(Tst_streamwr *pSObj,
                     const Tst_uint32 bufSz,
                     const Tst_buf *pBuf);
ST_EXPORT Tst_err
st_streamwr_wrBuffer_noCRC(Tst_streamwr *pSObj,
                           const Tst_uint32 bufSz,
                           const Tst_buf *pBuf);
/*** variable length values */
ST_EXPORT Tst_err
st_streamwr_wrUnary(Tst_streamwr *pSObj, Tst_uint32 val);

ST_EXPORT Tst_err
st_streamwr_wrString(Tst_streamwr *pSObj,
                     const Tst_uint32 maxBytes,
                     const Tst_str *pStr);


/*
// Functions -- stream_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_streamwr_stc_initSObj(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_stc_rsetSObj(Tst_streamwr *pSObj);

ST_EXPORT void
st_streamwr_stc_freeSObj(Tst_streamwr *pSObj);


ST_END_C_DECLS

#endif  /* HEAD_STREAMWR_H */

/******************************************************************************/
