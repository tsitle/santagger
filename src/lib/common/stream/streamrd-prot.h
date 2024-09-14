/******************************************************************************/
/* streamrd-prot.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for the Stream Reader                                  */
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

#ifndef HEAD_STREAMRD_PROT_H
#define HEAD_STREAMRD_PROT_H

/*
// Own-Includes
*/
/** */
#include "stream-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_STREAMRD_C) || defined(SRC_AC_FLAC_ZRDA_C)
	/** For Flac */
	/*** variable length values */
	Tst_err
	st_streamrd_rdFlacRiceSamples(Tst_streamrd *pSObj,
	                              Tst_uint32 samplesTot,
	                              const Tst_byte ricePar,
	                              Tst_int32 *pResids);

	Tst_err
	st_streamrd_rdFlacUtf8Encoded_uint32(Tst_streamrd *pSObj,
	                                     Tst_uint32 *pVal);

	Tst_err
	st_streamrd_rdFlacUtf8Encoded_uint64(Tst_streamrd *pSObj,
	                                     Tst_uint64 *pVal);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_STREAMRD_PROT_H */

/******************************************************************************/
