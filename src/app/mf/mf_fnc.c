/******************************************************************************/
/* mf_fnc.c                       [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Misc. functions for the 'main functions'                                   */
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

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
/** */
#define SRC_MF_FNC_C
#include "mf_fnc-pp.h"
#undef SRC_MF_FNC_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdarg.h>      /* va_list, ... */
#include <stdlib.h>      /* calloc(), free(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tast_cln_t_tagListIx
ast_mf_fnc_getSrcTLIXfromMTTP(const Tst_tagMeta_type mttp)
{
	switch (mttp) {
	case ST_MTAG_TTP_IV1:
		return AST_CLN_T_TAGLIST_IV1;
	case ST_MTAG_TTP_IV2:
		return AST_CLN_T_TAGLIST_IV2;
	case ST_MTAG_TTP_AV2:
		return AST_CLN_T_TAGLIST_AV2;
	case ST_MTAG_TTP_VOR:
		return AST_CLN_T_TAGLIST_VOR;
	case ST_MTAG_TTP_MRG:
		return AST_CLN_T_TAGLIST_MRG;
	default:  /* ? */
		ST_ASSERTN_NUM(AST_CLN_T_TAGLIST_MRG, 0 != 1)  /* cause exit() here */
		return AST_CLN_T_TAGLIST_MRG;
	}
}

Tst_tagMeta_type
ast_mf_fnc_mapTLIXtoMTTP(const Tast_cln_t_tagListIx tlix)
{
	switch (tlix) {
	case AST_CLN_T_TAGLIST_IV1:
		return ST_MTAG_TTP_IV1;
	case AST_CLN_T_TAGLIST_IV2:
		return ST_MTAG_TTP_IV2;
	case AST_CLN_T_TAGLIST_AV2:
		return ST_MTAG_TTP_AV2;
	case AST_CLN_T_TAGLIST_VOR:
		return ST_MTAG_TTP_VOR;
	case AST_CLN_T_TAGLIST_MRG:
		return ST_MTAG_TTP_MRG;
	default:  /* ? */
		ST_ASSERTN_NUM(ST_MTAG_TTP_NONE, 0 != 1)  /* cause exit() here */
		return ST_MTAG_TTP_NONE;
	}
}

Tst_bool
ast_mf_fnc_isMTTPinList(const Tst_tagMeta_type list[],
		const Tst_uint32 elems,
		const Tst_tagMeta_type mttp)
{
	Tst_uint32 x;

	for (x = 0; x < elems; x++)
		if (list[x] == mttp)
			return ST_B_TRUE;
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

Tst_bool
ast_mf_fnc_isBsInList(const Tast_cln_t_ebs *pBsArr,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx)
{
	Tast_cln_t_ebs const *pEl = pBsArr;

	if (pBsArr == NULL)
		return ST_B_FALSE;
	while (pEl->ix != 0) {
		if (pEl->ix == bsIx &&
				(pEl->six == bsSIx ||
					(pEl->six == 0 && bsSIx == 1) ||
					(pEl->six == 1 && bsSIx == 0))) {
			return ST_B_TRUE;
		}
		++pEl;
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Create a nice filename
 *   e.g. pFnIn="wonderland.mp3", pFnExt1="ape", pFnExt2="tag", bsIx/SIx=0
 *        --> *ppFnOut="wonderland.ape.tag" or "wonderland-001.ape.tag"
 *   or   pFnIn="wonderland.mp3", pFnExt1="tag", pFnExt2=NULL, bsIx/SIx=0
 *        --> *ppFnOut="wonderland.tag" or "wonderland-001.tag"
 *   or   pFnIn="wonderland.mp3", pFnExt1="tag", pFnExt2=NULL, bsIx=1, bsSIx=2
 *        --> *ppFnOut="wonderland-1_02.tag" or "wonderland-1_02-001.tag"
 *
 * returns: ERRCODE
 *          ppFnOut will be set
 */
Tst_err
ast_mf_fnc_createOutFn(const Tst_str *pFnIn,
		const Tst_str *pFnExt1, const Tst_str *pFnExt2,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		Tst_str **ppFnOut)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_str    *pCh;
	Tst_uint32 nr     = 1000,
	           olen,
	           addlen = 0;
	Tst_bool   fexists,
	           vacFnd = ST_B_FALSE;

	pCh = (Tst_str*)strrchr((char*)pFnIn, (int)'.');
	if (pCh == NULL)
		++addlen;
	addlen += st_sysStrlen(pFnExt1) + (pFnExt2 ? 1 + st_sysStrlen(pFnExt2) : 0);
	olen    = st_sysStrlen(pFnIn);
	ST_REALLOC(*ppFnOut, Tst_str*, olen + addlen + 20, 1)
	if (*ppFnOut == NULL)
		return ST_ERR_OMEM;
	memset(*ppFnOut, 0, olen + addlen + 20);

	/* copy everything but current fileextension to ppFnOut
	 *   "filename.wav" --> "filename"  */
	if (pCh != NULL)
		olen = (Tst_uint32)(pCh - pFnIn);
	memcpy(*ppFnOut, pFnIn, olen);

	do {
		/* append (number and) fileexts to ppFnOut
		 *   "filename" --> "filename.ape.tag"  or  "filename-001.ape.tag"*/
		if (nr == 1000) {
			if (pFnExt2 != NULL) {
				if (bsIx == 0)
					sprintf((char*)&(*ppFnOut)[olen], ".%s.%s", pFnExt1, pFnExt2);
				else
					sprintf((char*)&(*ppFnOut)[olen], "-%u_%02u.%s.%s",
							bsIx, bsSIx,
							pFnExt1, pFnExt2);
			} else {
				if (bsIx == 0)
					sprintf((char*)&(*ppFnOut)[olen], ".%s", pFnExt1);
				else
					sprintf((char*)&(*ppFnOut)[olen], "-%u_%02u.%s",
							bsIx, bsSIx,
							pFnExt1);
			}
		} else {
			if (pFnExt2 != NULL) {
				if (bsIx == 0)
					sprintf((char*)&(*ppFnOut)[olen], "-%03u.%s.%s",
							nr, pFnExt1, pFnExt2);
				else
					sprintf((char*)&(*ppFnOut)[olen], "-%u_%02u-%03u.%s.%s",
							bsIx, bsSIx,
							nr, pFnExt1, pFnExt2);
			} else {
				if (bsIx == 0)
					sprintf((char*)&(*ppFnOut)[olen], "-%03u.%s",
							nr, pFnExt1);
				else
					sprintf((char*)&(*ppFnOut)[olen], "-%u_%02u-%03u.%s",
							bsIx, bsSIx,
							nr, pFnExt1);
			}
		}
		/* */
		fexists = st_sysDoesFileExist(*ppFnOut);
		if (fexists && nr == 999)
			nr = 0;  /* --> stop while() and output error */
		else if (fexists && nr < 999) {
			++nr;  /* --> use nr+1 for filename */
			vacFnd = ST_B_TRUE;
		} else if (! fexists && (nr == 1 || nr == 1000))
			vacFnd = ST_B_TRUE;
		else
			--nr;
	} while (! vacFnd && nr > 0);

	if (! vacFnd) {
		ST_DELPOINT(*ppFnOut)
		res = ST_ERR_FAIL;
	} else if (nr < 1000) {
		if (pFnExt2 != NULL) {
			if (bsIx == 0)
				sprintf((char*)&(*ppFnOut)[olen], "-%03u.%s.%s",
						nr, pFnExt1, pFnExt2);
			else
				sprintf((char*)&(*ppFnOut)[olen], "-%u_%02u-%03u.%s.%s",
						bsIx, bsSIx,
						nr, pFnExt1, pFnExt2);
		} else {
			if (bsIx == 0)
				sprintf((char*)&(*ppFnOut)[olen], "-%03u.%s",
						nr, pFnExt1);
			else
				sprintf((char*)&(*ppFnOut)[olen], "-%u_%02u-%03u.%s",
						bsIx, bsSIx,
						nr, pFnExt1);
		}
	}
	return res;
}

/******************************************************************************/
