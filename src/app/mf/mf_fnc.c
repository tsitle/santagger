/******************************************************************************/
/* mf_fnc.c                       [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
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
		ST_OPTARG(const Tst_str *pFnExt1), ST_OPTARG(const Tst_str *pFnExt2),
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		ST_OPTARG(const Tst_str *pOutpdir), const Tst_bool owExFiles,
		Tst_str **ppFnOut)
{
	Tst_str       *pOrgFBn  = NULL,
	              *pOrgDirn = NULL,
	              *pNewFExt = NULL;
	Tst_str const *pIntOutpdir;
	Tst_uint32    nr        = 1000,
	              olen,
	              maxAddLen;
	Tst_bool      resB,
	              fexists,
	              vacFnd    = ST_B_FALSE;

	ST_ASSERTN_IARG(pFnIn == NULL || ppFnOut == NULL)

	/* */
	if (! st_sysFileBasename(pFnIn, &pOrgFBn))
		return ST_ERR_FAIL;
	if (st_sysStrEmpty(pOutpdir)) {
		if (! st_sysDirname(pFnIn, &pOrgDirn)) {
			ST_DELPOINT(pOrgFBn)
			return ST_ERR_FAIL;
		}
		pIntOutpdir = pOrgDirn;
	} else
		pIntOutpdir = pOutpdir;
	/* cut off original fileextension incl. '.' */
	olen = st_sysStrlen(pOrgFBn);
	if (olen > 0) {
		Tst_str *pCh;

		pCh  = (Tst_str*)strrchr((char*)pOrgFBn, (int)'.');
		if (pCh != NULL)
			olen = (Tst_uint32)(pCh - pOrgFBn);
	}
	pOrgFBn[olen] = 0x00;
	/* copy everything but current fileextension to ppFnOut
	 *   "/some/dir/filename.wav" --> "/some/dir/filename"  */
	resB = st_sysConcatDirAndFilen(pIntOutpdir, pOrgFBn, ppFnOut);
	/* */
	ST_DELPOINT(pOrgFBn)
	ST_DELPOINT(pOrgDirn)
	if (! resB)
		return ST_ERR_FAIL;
	/* */
	if (pFnExt1 != NULL && pFnExt2 != NULL)
		maxAddLen = st_sysStrlen(pFnExt1) + 1 + st_sysStrlen(pFnExt2);
	else if (pFnExt1 != NULL)
		maxAddLen = st_sysStrlen(pFnExt1);
	else
		maxAddLen = 0;
	if (maxAddLen > 0) {
		ST_CALLOC(pNewFExt, Tst_str*, maxAddLen + 1 + 1, 1)
		if (pNewFExt == NULL)
			return ST_ERR_OMEM;
		if (pFnExt1 != NULL && pFnExt2 != NULL)
			snprintf((char*)pNewFExt, maxAddLen + 1,
				"%s.%s", pFnExt1, pFnExt2);
		else
			snprintf((char*)pNewFExt, maxAddLen + 1,
				"%s", pFnExt1);
	}
	/* */
	maxAddLen += 32;
	olen       = st_sysStrlen(*ppFnOut);
	ST_REALLOC(*ppFnOut, Tst_str*, olen + maxAddLen, 1)
	if (*ppFnOut == NULL) {
		ST_DELPOINT(pNewFExt)
		return ST_ERR_OMEM;
	}
	memset(&(*ppFnOut)[olen], 0, maxAddLen);

	do {
		/* append (number and) fileexts to ppFnOut
		 *   "filename" --> "filename.ape.tag"  or  "filename-001.ape.tag"*/
		if (nr == 1000) {
			if (pNewFExt != NULL) {
				if (bsIx == 0)
					snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
							".%s", pNewFExt);
				else
					snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
							"-%u_%02u.%s", bsIx, bsSIx, pNewFExt);
			} else {
				if (bsIx != 0)
					snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
							"-%u_%02u", bsIx, bsSIx);
			}
		} else {
			if (pNewFExt != NULL) {
				if (bsIx == 0)
					snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
							"-%03u.%s", nr, pNewFExt);
				else
					snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
							"-%u_%02u-%03u.%s", bsIx, bsSIx, nr, pNewFExt);
			} else {
				if (bsIx == 0)
					snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
							"-%03u", nr);
				else
					snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
							"-%u_%02u-%03u", bsIx, bsSIx, nr);
			}
		}
		/* */
		fexists = (! owExFiles) && st_sysDoesFileExist(*ppFnOut);
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
		ST_DELPOINT(pNewFExt)
		return ST_ERR_FAIL;
	}
	if (nr < 1000) {
		if (pNewFExt != NULL) {
			if (bsIx == 0)
				snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
						"-%03u.%s", nr, pNewFExt);
			else
				snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
						"-%u_%02u-%03u.%s", bsIx, bsSIx, nr, pNewFExt);
		} else {
			if (bsIx == 0)
				snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
						"-%03u", nr);
			else
				snprintf((char*)&(*ppFnOut)[olen], maxAddLen,
						"-%u_%02u-%03u", bsIx, bsSIx, nr);
		}
	}

	ST_DELPOINT(pNewFExt)
	return ST_ERR_SUCC;
}

/******************************************************************************/
