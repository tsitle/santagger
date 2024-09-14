/******************************************************************************/
/* av_zfdeb-prot.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected sub functions for debugging Audio/Video stuff                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 02.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_AV_ZFDEB_PROT_H
#define HEAD_AV_ZFDEB_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/lib_defs.h"

/*
// System-Includes
*/
#include <stdarg.h>      /* va_list, ... */


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_AV_ZFDEB_C) || \
			defined(SRC_AUD_MPEG1_ZFDEB_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
			defined(SRC_CONT_WAV_ZFDEB_C)
	/** */
	void
	st_audFDeb_dbg(const Tst_bool stdOrErr,
	               const Tst_basOpts *pBasOpts,
	               const char *pFnc,
	               const Tst_str *pFn,
	               const char *pFmt,
	               va_list args);
#endif  /* protected */


/*
// Macros (protected)
*/
#if defined(SRC_AV_ZFDEB_C) || \
			defined(SRC_AUD_MPEG1_ZFDEB_C) || \
				defined(SRC_AUD_MPEG1_C) || \
				defined(SRC_AUD_MPEG1_ZRD_C) || \
				defined(SRC_AUD_MPEG1_ZRD2_C) || \
			defined(SRC_CONT_OGG_ZFDEB_C) || \
				defined(SRC_CONT_OGG_C) || \
				defined(SRC_CONT_OGG_ZRD_C) || \
				defined(SRC_CONT_OGG_ZRD_COD_C) || \
				defined(SRC_CONT_OGG_ZWR_C) || \
				defined(SRC_CONT_OGG_ZWR_COD_C) || \
				defined(SRC_AC_FLAC_ZRD_C) || \
				defined(SRC_AC_FLAC_ZRDA_C) || \
				defined(SRC_AC_FLAC_ZRDH_C) || \
				defined(SRC_AC_SPX_CLT_ZRD_C) || \
				defined(SRC_AC_VORB_ZRD_C) || \
				defined(SRC_MC_SKEL_ZRD_C) || \
				defined(SRC_MC_VORBCOMM_ZRD_C) || \
				defined(SRC_VC_DRC_THEO_ZRD_C) || \
			defined(SRC_CONT_WAV_ZFDEB_C) || \
				defined(SRC_CONT_WAV_WR_C)
	/** */
#	define ST_AVFDEB_HASCBS_DEB_BD(mac_bopts)  \
				((mac_bopts).cbsDbg.cbDbgFnc != NULL)
#	define ST_AVFDEB_HASCBS_ERR_BD(mac_bopts)  \
				((mac_bopts).cbsDbg.cbErrFnc != NULL)
	/** */
#	define ST_AVFDEB_HASCB_ANAL_P(mac_pOpts)  \
				(mac_pOpts != NULL && ST_AVFDEB_HASCB_ANAL_D(*mac_pOpts))
#	define ST_AVFDEB_HASCB_ANAL_D(mac_opts)  ((mac_opts).cbAnaMsg != NULL)

	/** */
#	define ST_AVFDEB_ISVERBAUD_BP(mac_pBOpts)  \
				(mac_pBOpts != NULL && ST_AVFDEB_ISVERBAUD_BD(*mac_pBOpts))
#	define ST_AVFDEB_ISVERBAUD_BD(mac_bopts)  \
				((mac_bopts).verb != 0 && \
					(ST_ISVERB((mac_bopts).verb, ST_VL_AUD) || \
						ST_ISVERB((mac_bopts).verb, ST_VL_AUDDET)))

	/** */
#	define ST_AVFDEB_ISVERBAUDDET_BP(mac_pBOpts)  \
				(mac_pBOpts != NULL && ST_AVFDEB_ISVERBAUDDET_BD(*mac_pBOpts))
#	define ST_AVFDEB_ISVERBAUDDET_BD(mac_bopts)  \
				((mac_bopts).verb != 0 && ST_ISVERB((mac_bopts).verb, ST_VL_AUDDET))

	/** */
#	define ST_AVFDEB_ISVERBAUD_OR_ANA_P(mac_pOpts)  \
				(mac_pOpts != NULL && ST_AVFDEB_ISVERBAUD_OR_ANA_D(*mac_pOpts))
#	define ST_AVFDEB_ISVERBAUD_OR_ANA_D(mac_opts)  \
				(ST_AVFDEB_ISVERBAUD_BD((mac_opts).basOpts) || \
					((mac_opts).anlz && ST_AVFDEB_HASCB_ANAL_D(mac_opts)))

	/** */
#	define ST_AVFDEB_ISVERBAUDDET_OR_ANA_P(mac_pOpts)  \
				(mac_pOpts != NULL && ST_AVFDEB_ISVERBAUDDET_OR_ANA_D(*mac_pOpts))
#	define ST_AVFDEB_ISVERBAUDDET_OR_ANA_D(mac_opts)  \
				(ST_AVFDEB_ISVERBAUDDET_BD((mac_opts).basOpts) || \
					((mac_opts).anlz && ST_AVFDEB_HASCB_ANAL_D(mac_opts)))
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_AV_ZFDEB_PROT_H */

/******************************************************************************/
