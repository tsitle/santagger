/******************************************************************************/
/* vc_drc_theo_zrd-prot.h       [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for Ogg-Dirac and Ogg-Theora codec handling            */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.06.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_VC_DRC_THEO_ZRD_PROT_H
#define HEAD_VC_DRC_THEO_ZRD_PROT_H

/*
// Own-Includes
*/
/** */
#include "src/includes/common/streamrd-defs.h"
/** */
#include "../cont_ogg/cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_ZRD_COD_C) || defined(SRC_VC_DRC_THEO_ZRD_C)
	/** */
	Tst_err
	st_contOgg_drcrd_parseHPack(const Tst_contOgg_opts *pOpts,
	                            Tst_contOgg_substr_intn *pBSI,
	                            const Tst_byte packTp,
	                            Tst_streamrd *pSObj);
	/** */
	Tst_err
	st_contOgg_theord_parseHPack(const Tst_contOgg_opts *pOpts,
	                             Tst_contOgg_substr_intn *pBSI,
	                             Tst_byte packTp,
	                             Tst_streamrd *pSObj,
	                             Tst_bool *pStopThen);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_VC_DRC_THEO_ZRD_PROT_H */

/******************************************************************************/
