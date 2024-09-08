/******************************************************************************/
/* mf_fnc-pp.h                    [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Private/Protected functions for the 'main functions'                       */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_MF_FNC_PP_H
#define HEAD_MF_FNC_PP_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "mf-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_MAIN_C) || defined(SRC_MF_FNC_C) || \
			defined(SRC_MF_CNVMRG_C) || \
			defined(SRC_MF_READ_C) || defined(SRC_MF_READ_DECMPG1_C) || \
			defined(SRC_MF_READ_DECVRB_C) || \
			defined(SRC_MF_EXTR_C) || \
			defined(SRC_MF_EDIT_C) || \
			defined(SRC_MF_SHOW_C) || defined(SRC_MF_SHOW_AV_C) || \
			defined(SRC_MF_SHOW_TIV1_C) || defined(SRC_MF_SHOW_TIV2_C)
	/** */
	Tast_cln_t_tagListIx
	ast_mf_fnc_getSrcTLIXfromMTTP(const Tst_tagMeta_type mttp);

	Tst_tagMeta_type
	ast_mf_fnc_mapTLIXtoMTTP(const Tast_cln_t_tagListIx tlix);

	Tst_bool
	ast_mf_fnc_isMTTPinList(const Tst_tagMeta_type list[],
	                        const Tst_uint32 elems,
	                        const Tst_tagMeta_type mttp);

	/** */
	Tst_bool
	ast_mf_fnc_isBsInList(const Tast_cln_t_ebs *pBsArr,
	                      const Tst_uint32 bsIx,
	                      const Tst_uint32 bsSIx);

	/** */
	Tst_err
	ast_mf_fnc_createOutFn(const Tst_str *pFnIn,
	                       ST_OPTARG(const Tst_str *pFnExt1),
	                       ST_OPTARG(const Tst_str *pFnExt2),
	                       const Tst_uint32 bsIx,
	                       const Tst_uint32 bsSIx,
	                       ST_OPTARG(const Tst_str *pOutpdir),
	                       const Tst_bool owExFiles,
	                       Tst_str **ppFnOut);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_MF_FNC_PP_H */

/******************************************************************************/
