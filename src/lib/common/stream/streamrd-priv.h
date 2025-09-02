/******************************************************************************/
/* streamrd-priv.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private functions/definitions for the Stream Reader                        */
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

#ifndef LIBSANTAGGER_STREAMRD_PRIV_H
#define LIBSANTAGGER_STREAMRD_PRIV_H

/*
// Own-Includes
*/
#if (LIBSANTAGGER_CFG_EXTRA_DEBUG == 1)
#	define ST_STREAMRD_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "streamrd-prot.h"


ST_BEGIN_C_DECLS

/*
// Constants (private)
*/
#ifdef SRC_STREAMRD_C
	/** default input buffer size for reading from files */
	static const Tst_uint32 ST_STREAMRD_DEFBUFSZ = 4 * 1024;

	/** msb table for unary values */
#	define LOC_RDUNARY_MSB_(n)  n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
	static const Tst_byte ST_STREAMRD_RDUNARY_MSB_TABLE[256] = {
			0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
			LOC_RDUNARY_MSB_(5),
			LOC_RDUNARY_MSB_(6), LOC_RDUNARY_MSB_(6),
			LOC_RDUNARY_MSB_(7), LOC_RDUNARY_MSB_(7),
			LOC_RDUNARY_MSB_(7), LOC_RDUNARY_MSB_(7),
			LOC_RDUNARY_MSB_(8), LOC_RDUNARY_MSB_(8),
			LOC_RDUNARY_MSB_(8), LOC_RDUNARY_MSB_(8),
			LOC_RDUNARY_MSB_(8), LOC_RDUNARY_MSB_(8),
			LOC_RDUNARY_MSB_(8), LOC_RDUNARY_MSB_(8) };
#	undef LOC_RDUNARY_MSB_
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_STREAMRD_C
	/** */
	static Tst_err
	ST_STREAMRD__fillBufferFromFile(Tst_streamrd_intn *pSOI,
	                                Tst_uint32 amount);
	static Tst_bool
	ST_STREAMRD__fillBuf_n_setBCur(Tst_streamrd_intn *pSOI,
	                               const Tst_uint32 inc);

	/** */
	static Tst_err
	ST_STREAMRD__rdBuffer_pr(Tst_streamrd *pSObj,
	                         const Tst_uint32 nBytes,
	                         Tst_buf *pBuf,
	                         Tst_uint32 *pReadBytes,
	                         const Tst_bool withCRCs);
	static Tst_err
	ST_STREAMRD__rdBuffer_prSub(Tst_streamrd_intn *pSOI,
	                            const Tst_uint32 nBytes,
	                            Tst_buf *pBuf,
	                            Tst_uint32 *pReadBytes,
	                            const Tst_bool withCRCs);

	/** */
	static Tst_err
	ST_STREAMRD__rdUInt32_case2(Tst_streamrd_intn *pSOI,
	                            Tst_byte nBits,
	                            Tst_uint32 *pVal);
	static Tst_err
	ST_STREAMRD__rdUInt32_case3(Tst_streamrd_intn *pSOI,
	                            Tst_byte nBits,
	                            Tst_uint32 *pVal);
	static Tst_err
	ST_STREAMRD__rdRiceSamples_rpZ(Tst_streamrd_intn *pSOI,
	                               Tst_uint32 samplesTot,
	                               Tst_int32 *pResids);
	static Tst_err
	ST_STREAMRD__rdRiceSamples_rpNZ_1_8_CRC(Tst_streamrd_intn *pSOI,
	                                        Tst_uint32 samplesTot,
	                                        const Tst_byte ricePar,
	                                        Tst_int32 *pResids);
	static Tst_err
	ST_STREAMRD__rdRiceSamples_rpNZ_1_8_NOCRC(Tst_streamrd_intn *pSOI,
	                                          Tst_uint32 samplesTot,
	                                          const Tst_byte ricePar,
	                                          Tst_int32 *pResids);
	static Tst_err
	ST_STREAMRD__rdRiceSamples_rpNZ_9_32_CRC(Tst_streamrd *pSObj,
	                                         Tst_streamrd_intn *pSOI,
	                                         Tst_uint32 samplesTot,
	                                         const Tst_byte ricePar,
	                                         Tst_int32 *pResids);
	static Tst_err
	ST_STREAMRD__rdRiceSamples_rpNZ_9_32_NOCRC(Tst_streamrd *pSObj,
	                                           Tst_streamrd_intn *pSOI,
	                                           Tst_uint32 samplesTot,
	                                           const Tst_byte ricePar,
	                                           Tst_int32 *pResids);

	/** */
#	if (ST_STREAMRD_DEB_ == 1)
	static void ST_STREAMRD__prf(const char *pFmt, ...);
	static void ST_STREAMRD__prE(const char *pFmt, ...);
	static void ST_STREAMRD__prBitsInByte(const Tst_byte byt);
#	endif
#endif  /* private */


/*
// Macros (private)
*/
#ifdef SRC_STREAMRD_C
	/** increase buffer position - DOESN'T return FEOF */
	/*** DOESN'T update CRCs */
#	define ST_STREAMRD__NOCRC_INCPOS_NORET(mac_pSOI)  { \
				if ((mac_pSOI)->sbuf.pBCur + 1 >= (mac_pSOI)->sbuf.pBEnd) { \
					if ((mac_pSOI)->sfil.pFStc == NULL) { \
						(mac_pSOI)->sbuf.pBCur = (Tst_buf*)(mac_pSOI)->sbuf.pBEnd; \
						(mac_pSOI)->sbuf.leftoverBits = 0; \
					} else {\
						if (ST_STREAMRD__fillBuf_n_setBCur(mac_pSOI, 1)) \
							/* !!! */ (mac_pSOI)->sbuf.leftoverBits = 8; \
					} \
				} else { \
					++(mac_pSOI)->sbuf.pBCur; \
					/* !!! */ (mac_pSOI)->sbuf.leftoverBits = 8; \
				} \
				}
	/*** updates CRCs */
#	define ST_STREAMRD__UPDCRC_INCPOS_NORET(mac_pSOI)  { \
				ST_STREAM_UPDCRC(mac_pSOI) \
				ST_STREAMRD__NOCRC_INCPOS_NORET(mac_pSOI) \
				}

	/** increase buffer position - returns FEOF */
	/*** DOESN'T update CRCs */
#	define ST_STREAMRD__NOCRC_INCPOS_RET(mac_pSOI)  { \
				if ((mac_pSOI)->sbuf.pBCur + 1 >= (mac_pSOI)->sbuf.pBEnd) { \
					if ((mac_pSOI)->sfil.pFStc == NULL) { \
						(mac_pSOI)->sbuf.pBCur = (Tst_buf*)(mac_pSOI)->sbuf.pBEnd; \
						(mac_pSOI)->sbuf.leftoverBits = 0; \
						return ST_ERR_FEOF; \
					} \
					if (! ST_STREAMRD__fillBuf_n_setBCur(mac_pSOI, 1)) \
						return ST_ERR_FEOF; \
				} else { \
					++(mac_pSOI)->sbuf.pBCur; \
					/* !!! */ /*(mac_pSOI)->sbuf.leftoverBits = 8;*/ \
				} \
				}
	/*** updates CRCs */
#	define ST_STREAMRD__UPDCRC_INCPOS_RET(mac_pSOI)  { \
				ST_STREAM_UPDCRC(mac_pSOI) \
				ST_STREAMRD__NOCRC_INCPOS_RET(mac_pSOI) \
				}

	/** increase read bits */
#	define ST_STREAMRD__INCREASE_READBITS(mac_pSOI)  { \
				(mac_pSOI)->totBytesRd += \
						(((mac_pSOI)->totBitsRd_rem - \
							((mac_pSOI)->totBitsRd_rem % 8)) / 8); \
				(mac_pSOI)->totBitsRd_rem = (mac_pSOI)->totBitsRd_rem % 8; \
				}

	/** prepare ahead-reading */
#	define ST_STREAMRD__RDAHEAD_PREPARE(mac_pSOI)  { \
				(mac_pSOI)->aheadVars.lobits = (mac_pSOI)->sbuf.leftoverBits; \
				(mac_pSOI)->aheadVars.tbyr   = (mac_pSOI)->totBytesRd; \
				(mac_pSOI)->aheadVars.tbir   = (mac_pSOI)->totBitsRd_rem; \
				/* */ \
				(mac_pSOI)->aheadVars.compCRC[0] = (mac_pSOI)->crcs.compCRC8; \
				(mac_pSOI)->aheadVars.compCRC[1] = (mac_pSOI)->crcs.compCRC16; \
				(mac_pSOI)->aheadVars.compCRC[2] = (mac_pSOI)->crcs.compCRC32; \
				(mac_pSOI)->crcs.compCRC8  = ST_B_FALSE; \
				(mac_pSOI)->crcs.compCRC16 = ST_B_FALSE; \
				(mac_pSOI)->crcs.compCRC32 = ST_B_FALSE; \
				/* */ \
				if ((mac_pSOI)->sbuf.pBBeg != \
						(mac_pSOI)->sbuf.pBEnd) \
					(mac_pSOI)->sbuf.pBCurRestore = \
							(mac_pSOI)->sbuf.pBCur; \
				}
	/** restore state after ahead-reading */
#	define ST_STREAMRD__RDAHEAD_RESTORE(mac_wasOK, mac_pSOI)  { \
				if (mac_wasOK) { \
					if ((mac_pSOI)->sbuf.pBCurRestore != NULL) { \
						(mac_pSOI)->sbuf.pBCur        = \
								(mac_pSOI)->sbuf.pBCurRestore; \
						(mac_pSOI)->sbuf.pBCurRestore = NULL; \
						/* */ \
						(mac_pSOI)->sbuf.leftoverBits = \
								(mac_pSOI)->aheadVars.lobits; \
					} else { \
						(mac_pSOI)->sbuf.pBCur        = \
								(mac_pSOI)->sbuf.pBBeg; \
						(mac_pSOI)->sbuf.leftoverBits = \
								((mac_pSOI)->sbuf.pBBeg != \
										(mac_pSOI)->sbuf.pBEnd ? 8 : 0); \
					} \
					(mac_pSOI)->totBytesRd     = (mac_pSOI)->aheadVars.tbyr; \
					(mac_pSOI)->totBitsRd_rem  = (mac_pSOI)->aheadVars.tbir; \
					(mac_pSOI)->crcs.compCRC8  = (mac_pSOI)->aheadVars.compCRC[0]; \
					(mac_pSOI)->crcs.compCRC16 = (mac_pSOI)->aheadVars.compCRC[1]; \
					(mac_pSOI)->crcs.compCRC32 = (mac_pSOI)->aheadVars.compCRC[2]; \
				} else \
					(mac_pSOI)->sbuf.pBCurRestore = NULL; \
				}
#endif  /* private */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_STREAMRD_PRIV_H */

/******************************************************************************/
