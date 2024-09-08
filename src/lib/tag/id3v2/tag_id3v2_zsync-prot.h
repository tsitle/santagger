/******************************************************************************/
/* tag_id3v2_zsync-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected sub functions for Unsynchronization & Co.                        */
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

#ifndef HEAD_TAG_ID3V2_ZSYNC_PROT_H
#define HEAD_TAG_ID3V2_ZSYNC_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/lib_defs.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_TAG_ID3V2_ZSYNC_C) || \
			defined(SRC_TAG_ID3V2_ZRD_C) || defined(SRC_TAG_ID3V2_ZWR_C)
	/** */
	Tst_uint32
	st_id3v2_sync_syncsafe2uint(Tst_uint32 cvInt);

	Tst_uint32
	st_id3v2_sync_uint2syncsafe(Tst_uint32 cvInt);

	/** */
	Tst_err
	st_id3v2_sync_reverseUnsync_stream2bo(ST_OPTARG(Tst_bool *pFalseSyncsFnd),
	                                      Tst_streamrd *pStrrd,
	                                      const Tst_uint32 dataLen,
	                                      Tst_binobj *pDataOut,
	                                      Tst_uint32 *pDelta);
	Tst_err
	st_id3v2_sync_doUnsync_bo2bo(Tst_binobj *pDataIn,
	                             Tst_binobj *pDataOut,
	                             Tst_uint32 *pDelta);

	/** */
	Tst_bool
	st_id3v2_sync_hasFalseSync(const Tst_bool fastOrThourough,
	                           Tst_buf *pBuf,
	                           const Tst_uint32 bufSz,
	                           Tst_uint32 *pFSBcnt);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_TAG_ID3V2_ZSYNC_PROT_H */

/******************************************************************************/
