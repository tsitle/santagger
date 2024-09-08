/******************************************************************************/
/* mf_show_tiv1.c                 [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for displaying ID3v1 Tags                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/string_mte.h>
#include <santag/common/sys_fnc.h>
#include <santag/tag/tag_basics.h>
#include <santag/tag/tag_id3_helper.h>
#include <santag/tag/tag_id3v1.h>
/** */
#define SRC_MF_SHOW_TIV1_C
#include "mf_show_tiv1-pp.h"
/*** */
#include "mf_outp-pp.h"
#undef SRC_MF_SHOW_TIV1_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), free(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (AST_MF_SW_DEBUG_IV1 == 1)

/*
 * Print ID3v1 Tag
 *
 * pCmdln  MUST be set
 * pTag    MUST be set
 */
Tst_err
ast_mf_sw_showTagIV1(const Tast_cln_a *pCmdln, const Tst_id3v1_tag *pTag)
{
	const Tst_uint32 cSWID = 30;
	Tst_err         res;
	Tst_mtes_string sval;
	Tst_int32       ival;

	ST_ASSERTN_IARG(pCmdln == NULL || pTag == NULL)

	res = st_mtes_stc_initStr(&sval);
	if (res != ST_ERR_SUCC)
		return res;

	ast_mf_op_prFInf("TAG iv1 (ID3v1)\n");

	/* left: BAND */
	ast_mf_op_prFInf("Artist: ");
	res = st_id3v1_gs_getField_artist(pTag, &sval);
	if (res == ST_ERR_SUCC)
		AST_MF__sw_sTIV1_prStr_simple(&sval, ST_B_FALSE, cSWID,
				pCmdln->opts.disp.asISOorU8);

	/* right: ALBUM */
	ast_mf_op_prFInf("   Album:   ");
	if (res == ST_ERR_SUCC) {
		res = st_id3v1_gs_getField_album(pTag, &sval);
		AST_MF__sw_sTIV1_prStr_simple(&sval, ST_B_TRUE, cSWID,
				pCmdln->opts.disp.asISOorU8);
	}

	/* left: SONG */
	ast_mf_op_prFInf("Song:   ");
	if (res == ST_ERR_SUCC) {
		res = st_id3v1_gs_getField_title(pTag, &sval);
		AST_MF__sw_sTIV1_prStr_simple(&sval, ST_B_FALSE, cSWID,
				pCmdln->opts.disp.asISOorU8);
	}

	/* right: COMMENT */
	ast_mf_op_prFInf("   Comment: ");
	if (res == ST_ERR_SUCC) {
		res = st_id3v1_gs_getField_comment(pTag, &sval);
		AST_MF__sw_sTIV1_prStr_simple(&sval, ST_B_TRUE, cSWID,
				pCmdln->opts.disp.asISOorU8);
	}

	st_mtes_stc_freeStr(&sval);
	if (res != ST_ERR_SUCC)
		return res;

	/* left: TRACKNR */
	ival = st_id3v1_gs_getField_tracknr(pTag);
	if (ival > 0)
		ast_mf_op_prFInf("Track:  %-3d%*s", ival, cSWID - 3, "");
	else
		ast_mf_op_prFInf("Track:  %-*s", cSWID, "");

	/* right: YEAR */
	ival = st_id3v1_gs_getField_year(pTag);
	if (ival > 0)
		ast_mf_op_prFInf("   Year:    %4d\n", ival);
	else
		ast_mf_op_prFInf("   Year:\n");

	/* left: GENRE */
	ast_mf_op_prFInf("Genre:  ");
	ival = st_id3v1_gs_getField_genre(pTag);
	if (st_tagBas_gs_getHasTag(st_id3v1_gs_getTag_tbas(pTag)))
		AST_MF__sw_sTIV1_prGen((Tst_byte)ival);
	ast_mf_op_prFInf("\n");
	return ST_ERR_SUCC;
}

#endif  /* AST_MF_SW_DEBUG_IV1 */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (AST_MF_SW_DEBUG_IV1 == 1)

/*
 * Print string without shortening it
 */
static void
AST_MF__sw_sTIV1_prStr_simple(const Tst_mtes_string *pStr, const Tst_bool wNln,
		const Tst_uint32 defwid, const Tst_bool dispAsISOorU8)
{
	Tst_err    res;
	Tst_uint32 len;
	Tst_str    *pC,
	           *pTmpPCh = NULL;
	Tst_mtes_string  tmps;
	Tst_mtes_textEnc te = ST_MTES_TE_UTF8;

	if (st_mtes_stc_initStr(&tmps) != ST_ERR_SUCC) {
		ast_mf_op_prFInf("-ERROR-\n");
		return;
	}

	len = st_mtes_strlenCh(pStr);
	if (dispAsISOorU8)
		te = ST_MTES_TE_ISO;

	res = st_mtes_convStr_c(pStr, &tmps, te);
	if (res == ST_ERR_SUCC)
		res = st_mtes_strReplace(&tmps,
				(const Tst_str*)"%", (const Tst_str*)"%%", NULL);
	if (res == ST_ERR_SUCC)
		res = st_mtes_copyToCharp(&tmps, &pTmpPCh, NULL);
	st_mtes_stc_freeStr(&tmps);
	if (res != ST_ERR_SUCC) {
		ast_mf_op_prFInf("-ERROR-\n");
		return;
	}

	if (dispAsISOorU8 && pTmpPCh != NULL) {
		pC = pTmpPCh;
		while (*pC) {
			if (*pC < ST_CSET_ISO_PRNSMA)
				*pC = ST_CSET_ISO_SPA;
			++pC;
		}
	}
	ast_mf_op_prFInf("%s", (len < 1 ? "" : (char*)pTmpPCh));
	ST_DELPOINT(pTmpPCh)
	if (len < defwid)
		ast_mf_op_prFInf("%*s", defwid - len, " ");
	if (wNln)
		ast_mf_op_prFInf("\n");
}

/*
 * Print ID3v1 Genre
 */
static void
AST_MF__sw_sTIV1_prGen(const Tst_byte gen)
{
	int      x      = -1;
	Tst_bool isUnkn = ST_B_TRUE;

	if (gen > 254)
		return;
	st_id3_genreExists_int(gen, &x);
	if (x >= 0) {
		isUnkn = ST_B_FALSE;
		ast_mf_op_prFInf("%s", st_id3_getGenreCap(x));
		if (! st_id3_isStandardGenre(x))
			ast_mf_op_prFInf(" (not standard)");
	}
	if (isUnkn)
		ast_mf_op_prFInf("#%u (unknown)", gen);
}

#endif  /* AST_MF_SW_DEBUG_IV1 */

/******************************************************************************/
