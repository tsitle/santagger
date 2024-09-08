/******************************************************************************/
/* stream_crcs-prot.h           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected functions for CRC computation                                    */
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

#ifndef HEAD_STREAM_CRCS_PROT_H
#define HEAD_STREAM_CRCS_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "stream-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_STREAMRD_C) || defined(SRC_STREAMWR_C) || \
			defined(SRC_STREAM_CRCS_C)
	/** */
	void
	st_stream_startCRC8(Tst_stream_crcs *pCRCs);

	void
	st_stream_pauseOrContinueCRC8(Tst_stream_crcs *pCRCs);

	void
	st_stream_updateCRC8(Tst_stream_crcs *pCRCs,
	                     const Tst_buf *pBuf,
	                     const Tst_uint32 sz);

	Tst_byte
	st_stream_getCRC8(const Tst_stream_crcs *pCRCs);

	void
	st_stream_stopCRC8(Tst_stream_crcs *pCRCs);
	/** */
	void
	st_stream_startCRC16(Tst_stream_crcs *pCRCs);

	void
	st_stream_pauseOrContinueCRC16(Tst_stream_crcs *pCRCs);

	void
	st_stream_updateCRC16(Tst_stream_crcs *pCRCs,
	                      const Tst_buf *pBuf,
	                      const Tst_uint32 sz);

	Tst_uint16
	st_stream_getCRC16(const Tst_stream_crcs *pCRCs);

	void
	st_stream_stopCRC16(Tst_stream_crcs *pCRCs);
	/** */
	void
	st_stream_startCRC32(Tst_stream_crcs *pCRCs);

	void
	st_stream_pauseOrContinueCRC32(Tst_stream_crcs *pCRCs);

	void
	st_stream_updateCRC32(Tst_stream_crcs *pCRCs,
	                      const Tst_buf *pBuf,
	                      const Tst_uint32 sz);

	Tst_uint32
	st_stream_getCRC32(const Tst_stream_crcs *pCRCs);

	void
	st_stream_stopCRC32(Tst_stream_crcs *pCRCs);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_STREAM_CRCS_PROT_H */

/******************************************************************************/
