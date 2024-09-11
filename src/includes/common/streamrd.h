/******************************************************************************/
/* streamrd.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for the Stream Reader                                            */
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

#ifndef HEAD_STREAMRD_H
#define HEAD_STREAMRD_H

/*
// Own-Includes
*/
#include "src/includes/common/sys_math-defs.h"
#include "src/includes/common/sys_file-defs.h"
/** */
#include "src/includes/common/streamrd-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- streamrd.c
*/
/** Set Input Source */
/*** */
ST_EXPORT Tst_err
st_streamrd_setInput_buffer(Tst_streamrd *pSObj,
                            const Tst_buf *pBuffer,
                            const Tst_uint32 bufSize);
/*** */
ST_EXPORT Tst_err
st_streamrd_setInput_file(Tst_streamrd *pSObj, Tst_sys_fstc *pFStc);

/** Get Status Info */
ST_EXPORT Tst_foffs
st_streamrd_getCurPos(const Tst_streamrd *pSObj);

ST_EXPORT Tst_fsize
st_streamrd_getInputSize(const Tst_streamrd *pSObj);

ST_EXPORT Tst_fsize
st_streamrd_getAmountRemainingBytes(const Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_getAmountRemainingBits(const Tst_streamrd *pSObj,
                                   Tst_fsize *pBytes,
                                   Tst_byte *pBitsMod8);

ST_EXPORT Tst_bool
st_streamrd_isEOF(const Tst_streamrd *pSObj);

ST_EXPORT Tst_byte
st_streamrd_getAmountRemainingBitsInCurByte(const Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_getAmountBitsRead(const Tst_streamrd *pSObj,
                              Tst_fsize *pBytes,
                              Tst_byte *pBitsMod8);

ST_EXPORT Tst_fsize
st_streamrd_getAmountBytesRead(const Tst_streamrd *pSObj);

/** CRC functions */
/*** */
ST_EXPORT void
st_streamrd_startCRC8(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_pauseOrContinueCRC8(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_updateCRC8(Tst_streamrd *pSObj,
                       const Tst_buf *pBuf,
                       const Tst_uint32 sz);

ST_EXPORT Tst_byte
st_streamrd_getCRC8(const Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_stopCRC8(Tst_streamrd *pSObj);
/*** */
ST_EXPORT void
st_streamrd_startCRC16(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_pauseOrContinueCRC16(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_updateCRC16(Tst_streamrd *pSObj,
                        const Tst_buf *pBuf,
                        const Tst_uint32 sz);

ST_EXPORT Tst_uint16
st_streamrd_getCRC16(const Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_stopCRC16(Tst_streamrd *pSObj);
/*** */
ST_EXPORT void
st_streamrd_startCRC32(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_pauseOrContinueCRC32(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_updateCRC32(Tst_streamrd *pSObj,
                        const Tst_buf *pBuf,
                        const Tst_uint32 sz);

ST_EXPORT Tst_uint32
st_streamrd_getCRC32(const Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_stopCRC32(Tst_streamrd *pSObj);

/** Skip some Input */
ST_EXPORT Tst_err
st_streamrd_rdSkipBits(Tst_streamrd *pSObj,
                       Tst_uint32 skip,
                       const Tst_bool countAsRead);
ST_EXPORT Tst_err
st_streamrd_rdSkipBytes(Tst_streamrd *pSObj,
                        Tst_uint32 skip,
                        const Tst_bool countAsRead);

/** Direct memcmp (like rdAheadBuffer() in combination with memcmp() ) */
ST_EXPORT Tst_bool
st_streamrd_rdMemcmpBytes(Tst_streamrd *pSObj,
                          const Tst_uint32 nBytes,
                          const void *pCmpWith);

/** Read Values */
/*** fixed length values */
ST_EXPORT Tst_err
st_streamrd_rdByte(Tst_streamrd *pSObj,
                   const Tst_byte nBits,
                   Tst_byte *pVal);
ST_EXPORT Tst_err
st_streamrd_rdByte_noCRC(Tst_streamrd *pSObj,
                         const Tst_byte nBits,
                         Tst_byte *pVal);
ST_EXPORT Tst_err
st_streamrd_rdUInt16(Tst_streamrd *pSObj,
                     const Tst_streamrd_endian inpEndian,
                     const Tst_byte nBits,
                     Tst_uint16 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdInt16(Tst_streamrd *pSObj,
                    const Tst_streamrd_endian inpEndian,
                    const Tst_byte nBits,
                    Tst_int16 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdUInt32(Tst_streamrd *pSObj,
                     const Tst_streamrd_endian inpEndian,
                     const Tst_byte nBits,
                     Tst_uint32 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdInt32(Tst_streamrd *pSObj,
                    const Tst_streamrd_endian inpEndian,
                    const Tst_byte nBits,
                    Tst_int32 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdUInt64(Tst_streamrd *pSObj,
                     const Tst_streamrd_endian inpEndian,
                     const Tst_byte nBits,
                     Tst_uint64 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdInt64(Tst_streamrd *pSObj,
                    const Tst_streamrd_endian inpEndian,
                    const Tst_byte nBits,
                    Tst_int64 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdBuffer(Tst_streamrd *pSObj,
                     const Tst_uint32 nBytes,
                     Tst_buf *pBuf,
                     Tst_uint32 *pReadBytes);
ST_EXPORT Tst_err
st_streamrd_rdBuffer_noCRC(Tst_streamrd *pSObj,
                           const Tst_uint32 nBytes,
                           Tst_buf *pBuf,
                           Tst_uint32 *pReadBytes);
/*** variable length values */
ST_EXPORT Tst_err
st_streamrd_rdUnary(Tst_streamrd *pSObj, Tst_uint32 *pVal);

ST_EXPORT Tst_err
st_streamrd_rdString(Tst_streamrd *pSObj,
                     const Tst_uint32 maxBytes,
                     Tst_str **ppStr);

/** Read Values ahead */
/*** fixed length values */
ST_EXPORT Tst_err
st_streamrd_rdAheadByte(Tst_streamrd *pSObj,
                        const Tst_byte nBits,
                        Tst_byte *pVal);
ST_EXPORT Tst_err
st_streamrd_rdAheadUInt16(Tst_streamrd *pSObj,
                          const Tst_streamrd_endian inpEndian,
                          const Tst_byte nBits,
                          Tst_uint16 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdAheadInt16(Tst_streamrd *pSObj,
                         const Tst_streamrd_endian inpEndian,
                         const Tst_byte nBits,
                         Tst_int16 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdAheadUInt32(Tst_streamrd *pSObj,
                          const Tst_streamrd_endian inpEndian,
                          const Tst_byte nBits,
                          Tst_uint32 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdAheadInt32(Tst_streamrd *pSObj,
                         const Tst_streamrd_endian inpEndian,
                         const Tst_byte nBits,
                         Tst_int32 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdAheadUInt64(Tst_streamrd *pSObj,
                          const Tst_streamrd_endian inpEndian,
                          const Tst_byte nBits,
                          Tst_uint64 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdAheadInt64(Tst_streamrd *pSObj,
                         const Tst_streamrd_endian inpEndian,
                         const Tst_byte nBits,
                         Tst_int64 *pVal);
ST_EXPORT Tst_err
st_streamrd_rdAheadBuffer(Tst_streamrd *pSObj,
                          const Tst_uint32 nBytes,
                          Tst_buf *pBuf,
                          Tst_uint32 *pReadBytes);


/*
// Functions -- stream_stcs.c
*/
/** Structs */
ST_EXPORT Tst_err
st_streamrd_stc_initSObj(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_stc_rsetSObj(Tst_streamrd *pSObj);

ST_EXPORT void
st_streamrd_stc_freeSObj(Tst_streamrd *pSObj);


ST_END_C_DECLS

#endif  /* HEAD_STREAMRD_H */

/******************************************************************************/
