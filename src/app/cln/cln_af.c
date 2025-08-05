/******************************************************************************/
/* cln_af.c                       [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for attaching files                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_CLN_AF_C
#include "cln_af-prot.h"
/*** */
#include "cln_stcs-prot.h"
#undef SRC_CLN_AF_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), qsort() */
#include <string.h>      /* memcpy(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* add file to attachFile array */
Tst_err
ast_cln_af_addAF(Tast_cln_a *pCmdln, const Tst_bool isPicOrGeo,
		const Tst_bool addOrMod,
		ST_OPTARG(const Tst_str *pFn), const Tst_uint32 modFNr)
{
	Tst_err res;
	Tast_cln_attFileArr *pAFarr;
	Tast_cln_attFile    *pNew;

	/* resize array, if necessary */
	pAFarr = &pCmdln->optsTagFlds.attFileArr;
	if (pAFarr->cnt >= pAFarr->sz) {
		res = ast_cln_stc_reszAFarr(pAFarr, pAFarr->sz + 2);
		if (res != ST_ERR_SUCC)
			return res;
	}
	ST_ASSERTN_NUM(ST_ERR_FAIL, pAFarr->pArr == NULL)

	pNew = (Tast_cln_attFile*)&pAFarr->pArr[pAFarr->cnt];
	/* */
	if (addOrMod) {
		res = st_sysStrcpy(pFn, &pNew->pFn);
		if (res != ST_ERR_SUCC)
			return res;
	} else
		pNew->modFNr = modFNr;
	pNew->isPicOrGeo = isPicOrGeo;
	pNew->addOrMod   = addOrMod;
	/* */
	pNew->isOK = ST_B_TRUE;
	++pAFarr->cnt;
	/* */
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/* add attributes to file in attachFile array */
Tst_err
ast_cln_af_addAttrToAF(Tast_cln_a *pCmdln, const Tst_bool isPicOrGeo,
		ST_OPTARG(const Tst_str *pDesc),
		const Tst_id3v2_picTp picTpE, const Tst_utilsFmt_picFmt picFmtE,
		ST_OPTARG(const Tst_str *pMime),
		ST_OPTARG(const Tst_str *pModFilen))
{
	const char *cFNCN = __func__;
	Tst_err res;
	Tast_cln_attFileArr *pAFarr;
	Tast_cln_attFile    *pAF = NULL;

	pAFarr = &pCmdln->optsTagFlds.attFileArr;
	if (pAFarr->cnt > 0)
		pAF = &pAFarr->pArr[pAFarr->cnt - 1];

	if (pAF == NULL || isPicOrGeo != pAF->isPicOrGeo) {
		if (isPicOrGeo)
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"attribute has no corresponding --%s/%s, aborting",
					AST_CLN_CMD_ADD_PIC_LO, AST_CLN_CMD_MOD_PIC_LO);
		else
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"attribute has no corresponding --%s/%s, aborting",
					AST_CLN_CMD_ADD_GEO_LO, AST_CLN_CMD_MOD_GEO_LO);
		return ST_ERR_ABRT;
	}

	/* desc */
	if (pDesc != NULL) {
		res = st_sysStrcpy(pDesc, &pAF->pDesc);
		if (res != ST_ERR_SUCC)
			return res;
	}
	if (isPicOrGeo) {
		/* picType */
		if (picTpE != ST_ID3V2_PT_NONE) {
			pAF->picTp    = picTpE;
			pAF->isPTPset = ST_B_TRUE;
		}
		/* picFormat */
		if (picFmtE != ST_UTILSFMT_PF_NONE) {
			pAF->picFm    = picFmtE;
			pAF->isPFMset = ST_B_TRUE;
		}
	} else {
		/* mime */
		if (pMime != NULL) {
			res = st_sysStrcpy(pMime, &pAF->pMime);
			if (res != ST_ERR_SUCC)
				return res;
		}
	}
	/* filename */
	if (! pAF->addOrMod && pModFilen != NULL) {
		res = st_sysStrcpy(pModFilen, &pAF->pFn);
		if (res != ST_ERR_SUCC)
			return res;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * This is used for debugging level ST_VL_OPTS
 */
void
ast_cln_af_prAFarr(const Tast_cln_a *pCmdln)
{
	Tst_uint32 x;
	Tast_cln_attFileArr const *pAFarr;
	Tast_cln_attFile    const *pAF;

	if (! ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_OPTS))
		return;
	pAFarr = &pCmdln->optsTagFlds.attFileArr;
	for (x = 0; x < pAFarr->cnt; x++) {
		pAF = (Tast_cln_attFile*)&pAFarr->pArr[x];
		if (! pAF->isOK)
			continue;
		pCmdln->cbDbg(NULL, "\t< AF-%s: addOrMod=%c",
				(pAF->isPicOrGeo ? "pic" : "geo"),
				(pAF->addOrMod ? 'a' : 'm'));
		if (! pAF->addOrMod)
			pCmdln->cbDbg(NULL, "\t<         modFNr=%02u", pAF->modFNr);
		pCmdln->cbDbg(NULL, "\t<         fn='%s'",
				(pAF->pFn != NULL ? (char*)pAF->pFn : ""));
		if (pAF->pDesc != NULL)
			pCmdln->cbDbg(NULL, "\t<         de='%s'", pAF->pDesc);
		if (pAF->isPicOrGeo) {
			if (pAF->picTp != ST_ID3V2_PT_NONE)
				pCmdln->cbDbg(NULL, "\t<         pt=%d (%s)", pAF->picTp,
						st_id3v2_fnc_getPicTp_capShort_byEnum(pAF->picTp));
			if (pAF->picFm != ST_UTILSFMT_PF_NONE)
				pCmdln->cbDbg(NULL, "\t<         fm=%d (%s: '%s')", pAF->picFm,
						st_utilsFmt_getPicFmt_fext_byEnum(pAF->picFm),
						st_utilsFmt_getPicFmt_mime_byEnum(pAF->picFm));
		} else if (pAF->pMime != NULL)
			pCmdln->cbDbg(NULL, "\t<         mi='%s'", pAF->pMime);
	}
}

/*----------------------------------------------------------------------------*/

Tst_err
ast_cln_af_checkAFarr(Tast_cln_a *pCmdln)
{
	const char       *cFNCN     = __func__;
	const Tst_uint32 cMAXFSZ_MB = 128,
	                 cMAXFSZ_BY = cMAXFSZ_MB * 1024 * 1024;
	Tst_err    res         = ST_ERR_SUCC;
	Tst_bool   errIsTooBig = ST_B_FALSE;
	Tst_uint32 x;
	Tst_fsize  fsz;
	Tast_cln_attFileArr *pAFarr;
	Tast_cln_attFile    *pAF;

	pAFarr = &pCmdln->optsTagFlds.attFileArr;
	for (x = 0; x < pAFarr->cnt; x++) {
		pAF = (Tast_cln_attFile*)&pAFarr->pArr[x];
		if (! pAF->isOK)
			continue;
		/* */
		if (pAF->addOrMod &&
				pAF->isPicOrGeo && pAF->picTp == ST_ID3V2_PT_NONE) {
			pAF->picTp = AST_CLN_DEF_PIC_TP_VAL;
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_OPTS))
				pCmdln->cbDbg(cFNCN,
						"setting pic-tp for pic #%02u to '%s'", x + 1,
						st_id3v2_fnc_getPicTp_capShort_byEnum(pAF->picTp));
		}
		/* */
		if (! pAF->addOrMod) {
			Tst_bool oneSet = ST_B_FALSE;

			if (pAF->pFn != NULL)
				oneSet = ST_B_TRUE;
			else if (pAF->pDesc != NULL)
				oneSet = ST_B_TRUE;
			else if (pAF->isPicOrGeo && (pAF->isPTPset || pAF->isPFMset))
				oneSet = ST_B_TRUE;
			else if (! pAF->isPicOrGeo && pAF->pMime != NULL)
				oneSet = ST_B_TRUE;
			if (! oneSet) {
				pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
						"entry for '--%s %u' has no attributes set, will abort",
						(pAF->isPicOrGeo ? AST_CLN_CMD_MOD_PIC_LO : AST_CLN_CMD_MOD_GEO_LO),
						pAF->modFNr);
				pAF->isOK = ST_B_FALSE;
				res       = ST_ERR_ABRT;
				break;
			}
		}
		/* */
		if (! pAF->addOrMod && pAF->pFn == NULL)
			continue;
		if (pAF->pFn == NULL || ! st_sysDoesFileExist(pAF->pFn)) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"file \"%s\" doesn't exist, will abort",
					(pAF->pFn != NULL ? (char*)pAF->pFn : ""));
			pAF->isOK = ST_B_FALSE;
			res       = ST_ERR_ABRT;
			break;
		}
		fsz = st_sysGetFileSz_byFn(pAF->pFn, &errIsTooBig);
		if (errIsTooBig) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"file \"%s\" is too big", pAF->pFn);
			pAF->isOK = ST_B_FALSE;
			res       = ST_ERR_FAIL;
			fsz       = 0;
		} else if (fsz < 1) {
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"file \"%s\" is empty", pAF->pFn);
			pAF->isOK = ST_B_FALSE;
			res       = ST_ERR_FAIL;
		}
		if (fsz <= (Tst_fsize)cMAXFSZ_BY)
			continue;
		pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
				"file \"%s\" is too big, "
				"size="ST_TFSIZE_PRF_LU" bytes (%.2f MB), max=%u bytes (%u MB), "
				"aborting", pAF->pFn,
				(Tst_fsize)fsz, (double)fsz / 1024.0 / 1024.0,
				cMAXFSZ_BY, cMAXFSZ_MB);
		pAF->isOK = ST_B_FALSE;
		res       = ST_ERR_ABRT;
		break;
	}
	return res;
}

/******************************************************************************/
