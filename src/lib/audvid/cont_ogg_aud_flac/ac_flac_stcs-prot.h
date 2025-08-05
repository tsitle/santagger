/******************************************************************************/
/* ac_flac_stcs-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for Flac structures                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AC_FLAC_STCS_PROT_H
#define HEAD_AC_FLAC_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "ac_flac-prot.h"
#include "../cont_ogg/cont_ogg-common-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_STCS_C) || \
			defined(SRC_CONT_OGG_C) || defined(SRC_CONT_OGG_STCS_C) || \
			defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_AC_FLAC_ZRDA_C)
	/** */
	void st_contOgg_stc_rsetFlacOpts(Tst_contOgg_flacOpts *pFO);

	/** */
	Tst_err st_contOgg_stc_initFlac(Tst_contOgg_flac *pAud);
	void    st_contOgg_stc_rsetFlac(Tst_contOgg_flac *pAud);
	void    st_contOgg_stc_freeFlac(Tst_contOgg_flac *pAud);

	/** */
	Tst_err st_contOgg_stc_initFlacIdent(Tst_contOgg_flacIdent *pID);
	void    st_contOgg_stc_rsetFlacIdent(Tst_contOgg_flacIdent *pID);
	void    st_contOgg_stc_freeFlacIdent(Tst_contOgg_flacIdent *pID);

	/** */
	void st_contOgg_stc_initFlacAFra(Tst_contOgg_flacAudFrame *pFra);
	void st_contOgg_stc_rsetFlacAFra(Tst_contOgg_flacAudFrame *pFra,
	                                 const Tst_bool freeBufs);
#	define st_contOgg_stc_freeFlacAFra(pFra)  \
				st_contOgg_stc_rsetFlacAFra(pFra, ST_B_TRUE)

	/** */
#	define st_contOgg_stc_initFlacFH(pFH)  st_contOgg_stc_rsetFlacFH(pFH)
	void   st_contOgg_stc_rsetFlacFH(Tst_contOgg_flacFH *pFH);
#	define st_contOgg_stc_freeFlacFH(pFH)  st_contOgg_stc_rsetFlacFH(pFH)

	/** */
	void    st_contOgg_stc_initFlacSubFr(Tst_contOgg_flacSubFr *pSFr);
	void    st_contOgg_stc_rsetFlacSubFr(Tst_contOgg_flacSubFr *pSFr,
	                                     const Tst_bool freeBuf);
#	define  st_contOgg_stc_freeFlacSubFr(pSFr)  \
				st_contOgg_stc_rsetFlacSubFr(pSFr, ST_B_TRUE)
	Tst_err st_contOgg_stc_reszFlacSubFrResid(Tst_contOgg_flacSubFr *pSFr,
	                                          const Tst_uint32 samplesPerChann);

	/** */
	void    st_contOgg_stc_initFlacRSamps(Tst_contOgg_flacRawSamples *pSam);
	void    st_contOgg_stc_rsetFlacRSamps(Tst_contOgg_flacRawSamples *pSam);
#	define  st_contOgg_stc_freeFlacRSamps(pSam)  \
				st_contOgg_stc_rsetFlacRSamps(pSam)
	Tst_err st_contOgg_stc_reszFlacRSamps(Tst_contOgg_flacRawSamples *pSam,
	                                      const Tst_uint32 channels,
	                                      const Tst_uint32 samplesPerChann);

	/** */
	void
	st_contOgg_stc_initFlacTempSF_verbArr(Tst_contOgg_flacTempSF_verbArr *pSFV);

	void
	st_contOgg_stc_rsetFlacTempSF_verbArr(Tst_contOgg_flacTempSF_verbArr *pSFV);

#	define st_contOgg_stc_freeFlacTempSF_verbArr(pSFV)  \
				st_contOgg_stc_rsetFlacTempSF_verbArr(pSFV)

	Tst_err
	st_contOgg_stc_reszFlacTempSF_verbArr(Tst_contOgg_flacTempSF_verbArr *pSFV,
	                                      const Tst_uint32 blkSz);
	/** */
	Tst_err st_contOgg_stc_initFDO(Tst_contOgg_flacDecObj *pDec);
	void    st_contOgg_stc_rsetFDO(Tst_contOgg_flacDecObj *pDec);
	void    st_contOgg_stc_freeFDO(Tst_contOgg_flacDecObj *pDec);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AC_FLAC_STCS_PROT_H */

/******************************************************************************/
