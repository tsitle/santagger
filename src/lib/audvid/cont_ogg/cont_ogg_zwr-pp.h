/******************************************************************************/
/* cont_ogg_zwr-pp.h            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private/Protected functions for writing OGG containers / Flac files        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_CONT_OGG_ZWR_PP_H
#define HEAD_CONT_OGG_ZWR_PP_H

/*
// Own-Includes
*/
/** */
#include "cont_ogg-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_CONT_OGG_ZWR_C) || defined(SRC_CONT_OGG_C)
	/** */
	void
	st_contOgg_wr_markBSforVCmtUpd(Tst_contOgg_obj *pAObj);
	/** */
	void
	st_contOgg_wr_showStats(Tst_contOgg_obj *pAObj);
	/** */
	Tst_err
	st_contOgg_wr_replaceVCmt(Tst_contOgg_obj_intn *pAObjI,
	                          Tst_contOgg_substream *pBS);
	/** */
	Tst_err
	st_contOgg_wr_addEOS(Tst_contOgg_obj *pAObj);
	/** */
	Tst_err
	st_contOgg_wr_wrPacket(Tst_contOgg_obj_intn *pAObjI,
	                       Tst_contOgg_substr_intn *pBSI,
	                       const Tst_contOgg_fhd *pFHd);
	/** */
	Tst_err
	st_contOgg_wr_wrFrame(Tst_contOgg_obj_intn *pAObjI,
	                      Tst_contOgg_substr_intn *pBSI,
	                      Tst_contOgg_fhd *pFHd,
	                      const Tst_uint32 frDatStart,
	                      Tst_uint32 frDatSz);
#endif  /* protected */


/*
// Types (private)
*/
#ifdef SRC_CONT_OGG_ZWR_C
	/** */
	typedef struct {
		Tst_uint32 sz;
		Tst_uint32 segsUsd;
		Tst_uint32 segsSz;
		Tst_byte   *pSegTab;
		Tst_binobj bindat;
	} Tst_contOgg__wr_partialPack;
#endif  /* private */


/*
// Functions (private)
*/
#ifdef SRC_CONT_OGG_ZWR_C
	/** */
	static void
	ST_CONTOGG__wr_stc_initPP(Tst_contOgg__wr_partialPack *pPP);
	static void
	ST_CONTOGG__wr_stc_freePP(Tst_contOgg__wr_partialPack *pPP);
	/** */
	static Tst_err
	ST_CONTOGG__wr_copyToPP(const char *pFnc,
	                        Tst_contOgg_obj_intn *pAObjI,
	                        Tst_contOgg_substr_intn *pBSI,
	                        const Tst_uint32 startPos,
	                        Tst_contOgg__wr_partialPack *pPP);
	static Tst_err
	ST_CONTOGG__wr_appendPP(const char *pFnc,
	                        Tst_contOgg_obj_intn *pAObjI,
	                        Tst_contOgg_substr_intn *pBSI,
	                        Tst_contOgg__wr_partialPack *pPP);
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_CONT_OGG_ZWR_PP_H */

/******************************************************************************/
