/******************************************************************************/
/* cont_ogg_zfdeb-prot.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for OGG debugging                                      */
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

#ifndef LIBSANTAGGER_CONT_OGG_ZFDEB_PROT_H
#define LIBSANTAGGER_CONT_OGG_ZFDEB_PROT_H

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
#	define ST_CONTOGG_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#if (ST_CONTOGG_DEB_ == 1)
	#include "src/includes/common/sys_math-defs.h"
#endif
/** */
#include "cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_C) || \
			defined(SRC_CONT_OGG_ZRD_C) || defined(SRC_CONT_OGG_ZRD_COD_C) || \
			defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_ZWR_COD_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_MC_VORBCOMM_ZRD_C) || \
			defined(SRC_MC_SKEL_ZRD_C) || \
			defined(SRC_AC_VORB_ZRD_C) || \
			defined(SRC_AC_FLAC_ZRD_C) || defined(SRC_AC_FLAC_ZRDH_C) || \
			defined(SRC_AC_FLAC_ZRDA_C) || \
			defined(SRC_AC_SPX_CLT_ZRD_C) || \
			defined(SRC_VC_DRC_THEO_ZRD_C)
	/** debug/error */
	void st_contOgg_d_deb(const Tst_contOgg_opts *pOpts,
	                      const char *pFnc,
	                      const char *pFmt, ...);
	void st_contOgg_d_debBS1(const Tst_contOgg_opts *pOpts,
	                         const Tst_contOgg_substr_intn *pBSI,
	                         const char *pFnc,
	                         const char *pFmt, ...);
	void st_contOgg_d_debBS2(const Tst_contOgg_opts *pOpts,
	                         const Tst_contOgg_substr_intn *pBSI,
	                         const Tst_contOgg_fhd *pFHd,
	                         const char *pFnc,
	                         const char *pFmt, ...);
	void st_contOgg_d_debBS3(const Tst_contOgg_opts *pOpts,
	                         const Tst_contOgg_substr_intn *pBSI,
	                         const char *pFnc,
	                         const char *pFmt, ...);
	void st_contOgg_d_debBS3d(const Tst_contOgg_opts *pOpts,
	                          const Tst_contOgg_substr_intn *pBSI,
	                          const char *pFnc,
	                          const char *pFmt, ...);
	void st_contOgg_d_debBS3d_flacSF(const Tst_contOgg_opts *pOpts,
	                                 const Tst_contOgg_flacSubFr *pSFr,
	                                 const char *pFnc,
	                                 const char *pFmt, ...);
	void st_contOgg_d_debBS4(const Tst_contOgg_opts *pOpts,
	                         const Tst_contOgg_substr_intn *pBSI,
	                         const char *pFnc,
	                         const char *pFmt, ...);
	void st_contOgg_d_err(const Tst_contOgg_opts *pOpts,
	                      const char *pFnc,
	                      const Tst_str *pFn,
	                      const char *pFmt, ...);
	void st_contOgg_d_errBS2(const Tst_contOgg_opts *pOpts,
	                         const Tst_contOgg_substr_intn *pBSI,
	                         const Tst_contOgg_fhd *pFHd,
	                         const char *pFnc,
	                         const Tst_str *pFn,
	                         const char *pFmt, ...);

	/* analizing */
	void st_contOgg_d_debOrAna(const Tst_contOgg_opts *pOpts,
	                           const Tst_bool isSrcOggOrFlac,
	                           const char *pFnc,
	                           const char *pFmt, ...);
	void st_contOgg_d_debOrAna_warn(const Tst_contOgg_opts *pOpts,
	                                const Tst_bool isSrcOggOrFlac,
	                                const char *pFnc,
	                                const char *pFmt, ...);
	void st_contOgg_d_debOrAnaBS1(const Tst_contOgg_opts *pOpts,
	                              const Tst_contOgg_substr_intn *pBSI,
	                              const char *pFnc,
	                              const char *pFmt, ...);
	void st_contOgg_d_debOrAnaBS1warn(const Tst_contOgg_opts *pOpts,
	                                  const Tst_contOgg_substr_intn *pBSI,
	                                  const char *pFnc,
	                                  const char *pFmt, ...);
	void st_contOgg_d_debOrAnaBS2(const Tst_contOgg_opts *pOpts,
	                              const Tst_contOgg_substr_intn *pBSI,
	                              const Tst_contOgg_fhd *pFHd,
	                              const char *pFnc,
	                              const char *pFmt, ...);
	void st_contOgg_d_debOrAnaBS2warn(const Tst_contOgg_opts *pOpts,
	                                  const Tst_contOgg_substr_intn *pBSI,
	                                  const Tst_contOgg_fhd *pFHd,
	                                  const char *pFnc,
	                                  const char *pFmt, ...);
	void st_contOgg_d_debOrAnaBS3(const Tst_contOgg_opts *pOpts,
	                              const Tst_contOgg_substr_intn *pBSI,
	                              const char *pFnc,
	                              const char *pFmt, ...);
	void st_contOgg_d_debOrAnaBS3warn(const Tst_contOgg_opts *pOpts,
	                                  const Tst_contOgg_substr_intn *pBSI,
	                                  const char *pFnc,
	                                  const char *pFmt, ...);

	/** */
#	if (ST_CONTOGG_DEB_ == 1)
	void st_contOgg_prf(const char *pFmt, ...);
	void st_contOgg_prf64(const char *pMsg, const Tst_uint64 *pUI64);
	void st_contOgg_prE(const char *pFmt, ...);
	void st_contOgg_prBitsInByte(const Tst_byte byt);
	void st_contOgg_prBuf(const Tst_buf *pBuf, const Tst_uint32 sz);
#	endif
#endif  /* protected */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_CONT_OGG_ZFDEB_PROT_H */

/******************************************************************************/
