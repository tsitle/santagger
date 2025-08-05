/******************************************************************************/
/* mf_extr.c                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for extracting data from Tags etc.                               */
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
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_meta.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_MF_EXTR_C
#include "mf_extr-pp.h"
/*** */
#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#undef SRC_MF_EXTR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc(), random() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Extracts data from file
 *   The following can be extracted:
 *   - raw tags
 *   - pictures from ID3v2 tags (or tags that have been converted to IV2)
 *   - GEOBs     ------ " ----------------------
 *   - binary data ------ " ----------------------
 *
 * pMF      MUST be set
 * pCmdln   MUST be set
 */
Tst_err
ast_mf_extr_extrData(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	Tst_err    res = ST_ERR_SUCC;
	Tst_bool   fnd;
	Tst_uint32 x;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			! st_sysFStc_isOpen(&pMF->fstc))

	if (pCmdln->opts.showStat)
		ast_mf_op_prMsg("*Extracting data...");

	/* Tag extraction */
	fnd = ST_B_FALSE;
	for (x = 0; x < sizeof(pCmdln->opts.lstExtrTag) / sizeof(Tst_bool); x++)
		if (pCmdln->opts.lstExtrTag[x]) {
			fnd = ST_B_TRUE;
			break;
		}
	if (fnd) {
		res = AST_MF__extr_extrTag(pCmdln, pMF);
		if (res != ST_ERR_SUCC)
			return res;
	}

	/* Pic/GEOB/BinData extraction */
	fnd = (pCmdln->cmds.extrPic && pCmdln->opts.pLstExtrPic != NULL) ||
			(pCmdln->cmds.extrGeo && pCmdln->opts.pLstExtrGeo != NULL) ||
			(pCmdln->cmds.extrBinD);
	if (fnd)
		res = AST_MF__extr_extrOther(pCmdln, pMF);

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Extracts Tags from file / buffer
 *   doesn't work with converted Tags
 */
static Tst_err
AST_MF__extr_extrTag(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	const Tst_tagMeta_type cMTTPS[] = {ST_MTAG_TTP_IV1, ST_MTAG_TTP_AV2,
	                                   ST_MTAG_TTP_IV2, ST_MTAG_TTP_VOR};
	const Tst_uint32       cMTTPS_N = sizeof(cMTTPS) / sizeof(Tst_tagMeta_type);
	Tst_err    res   = ST_ERR_SUCC;
	Tst_bool   fnd;
	Tst_uint32 bsIx  = 0,
	           bsSIx = 0,
	           x,
	           tagNr,
	           tagCnt;
	char const **ppTagD;
	char       fext1[16];
	Tst_tagMeta_mt        *pMT;
	Tst_tagBasics const   *pTBasOrg;
	Tst_vorbc_tag         *pVor;
	Tast_cln_t_tagListIx  tlix;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL)

	for (x = 0; x < cMTTPS_N; x++) {
		ppTagD = st_tagMeta_fnc_getTagDesc(cMTTPS[x]);
		fnd    = ST_B_FALSE;
		tlix   = ast_mf_fnc_getSrcTLIXfromMTTP(cMTTPS[x]);
		if (! pCmdln->opts.lstExtrTag[tlix])
			continue;
		/* count tags of current type */
		pMT    = NULL;
		tagNr  = 0;
		tagCnt = 0;
		while ((pMT = st_tagMeta_ite_nextTag_byTypeOrg(
					&pMF->tagArr, cMTTPS[x], pMT)) != NULL) {
			pTBasOrg = st_tagMeta_gs_getTag_tbasOrg(pMT);
			if (pTBasOrg == NULL || ! st_tagBas_gs_getHasTag(pTBasOrg) ||
					st_tagMeta_gs_getTag_type(pMT) != cMTTPS[x])
				continue;
			++tagCnt;
		}
		pMT = NULL;
		/* */
		while ((pMT = st_tagMeta_ite_nextTag_byTypeOrg(
					&pMF->tagArr, cMTTPS[x], pMT)) != NULL) {
			pTBasOrg = st_tagMeta_gs_getTag_tbasOrg(pMT);
			if (pTBasOrg == NULL || ! st_tagBas_gs_getHasTag(pTBasOrg) ||
					st_tagMeta_gs_getTag_type(pMT) != cMTTPS[x])
				continue;
			++tagNr;
			if (tagCnt > 1)
				sprintf(fext1, "%s_%02u", ppTagD[1], tagNr);
			else
				sprintf(fext1, "%s", ppTagD[1]);

			if (cMTTPS[x] != ST_MTAG_TTP_VOR) {
				res = AST_MF__extr_extrTag_sub(pCmdln,
						&pMF->fstc, ppTagD[0], /*pFnExt:*/fext1,
						st_tagBas_gs_getOffset(pTBasOrg),
						st_tagBas_gs_getSize(pTBasOrg),
						NULL, 0, 0);
			} else {
				pVor = st_tagMeta_gs_getRealTag_vor(pMT);
				ST_ASSERTN_NUM(ST_ERR_FAIL, pVor == NULL)
				/* */
				st_tagMeta_gs_getTag_bsIx(pMT, &bsIx, &bsSIx);
				/* */
				res = AST_MF__extr_extrTag_sub(pCmdln,
						&pMF->fstc, ppTagD[0], /*pFnExt:*/fext1,
						0, st_vorbc_gs_getRawTagSize(pVor),
						pVor, bsIx, bsSIx);
			}
			if (res != ST_ERR_SUCC)
				return res;
			fnd = ST_B_TRUE;
		}
		/* */
		if (! fnd && ! pCmdln->opts.quiet)
			ast_mf_op_prMsg("*  extract Tag: %s Tag not present", ppTagD[0]);
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Extracts Pictures/GEOBs/BinData from Tags
 *   works only for (converted) ID3v2 Tags
 */
static Tst_err
AST_MF__extr_extrOther(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	Tst_err    res    = ST_ERR_SUCC;
	Tst_bool   fndOne = ST_B_FALSE;
	char const *pTagD;
	Tst_uint32 tagNr  = 0,
	           tagCnt = 0;
	Tst_tagMeta_mt      *pMT = NULL;
	Tst_tagBasics const *pTBasOrg;
	Tst_id3v2_tag       *pIv2;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL)

	/* count (converted) IV2 tags */
	while ((pMT = st_tagMeta_ite_nextTag_byType(
				&pMF->tagArr, ST_MTAG_TTP_IV2, pMT)) != NULL) {
		pTBasOrg = st_tagMeta_gs_getTag_tbasOrg(pMT);
		if (pTBasOrg == NULL || ! st_tagBas_gs_getHasTag(pTBasOrg))
			continue;
		++tagCnt;
	}
	pMT = NULL;
	/* */
	while ((pMT = st_tagMeta_ite_nextTag_byType(
				&pMF->tagArr, ST_MTAG_TTP_IV2, pMT)) != NULL) {
		pTBasOrg = st_tagMeta_gs_getTag_tbasOrg(pMT);
		if (pTBasOrg == NULL || ! st_tagBas_gs_getHasTag(pTBasOrg))
			continue;
		pTagD = st_tagMeta_fnc_getTagDesc(st_tagMeta_gs_getTag_typeOrg(pMT))[0];
		++tagNr;
		/* */
		pIv2   = st_tagMeta_gs_getRealTag_iv2(pMT);
		fndOne = ST_B_TRUE;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pIv2 == NULL)

		/* Picture extraction */
		if (pCmdln->cmds.extrPic && pCmdln->opts.pLstExtrPic != NULL) {
			res = AST_MF__extr_extrOther_sub(pCmdln,
					st_sysFStc_getFilen(&pMF->fstc), pTagD, "Pic",
					pCmdln->opts.extrPicAll, pCmdln->opts.pLstExtrPic,
					ST_ID3V2_FID_NONE, ST_ID3V2_FTP_PIC,
					pIv2, (tagCnt > 1 ? tagNr : 0));
			if (res != ST_ERR_SUCC)
				break;
		}

		/* GEOB extraction */
		if (pCmdln->cmds.extrGeo && pCmdln->opts.pLstExtrGeo != NULL) {
			res = AST_MF__extr_extrOther_sub(pCmdln,
					st_sysFStc_getFilen(&pMF->fstc), pTagD, "GEO",
					pCmdln->opts.extrGeoAll, pCmdln->opts.pLstExtrGeo,
					ST_ID3V2_FID_NONE, ST_ID3V2_FTP_GEO,
					pIv2, (tagCnt > 1 ? tagNr : 0));
			if (res != ST_ERR_SUCC)
				break;
		}

		/* binary data extraction */
		if (pCmdln->cmds.extrBinD) {
			res = AST_MF__extr_extrOther_sub(pCmdln,
					st_sysFStc_getFilen(&pMF->fstc), pTagD, "BinData",
					ST_B_TRUE, NULL,
					ST_ID3V2_FID_NONE, ST_ID3V2_FTP_NONE,
					pIv2, (tagCnt > 1 ? tagNr : 0));
			if (res != ST_ERR_SUCC)
				break;
		}
	} /* end while */

	if (res == ST_ERR_SUCC && ! fndOne && ! pCmdln->opts.quiet)
		ast_mf_op_prMsg("*  extract Pic/GEO/BinData: nothing found to extract");

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Extracts one Tag from file / buffer
 *   doesn't work with converted Tags
 */
static Tst_err
AST_MF__extr_extrTag_sub(const Tast_cln_a *pCmdln,
		Tst_sys_fstc *pFStcIn, const char *pTDesc, const char *pFnExt,
		const Tst_foffs offs, const Tst_uint32 binSize,
		Tst_vorbc_tag *pVor, Tst_uint32 vorBsIx, Tst_uint32 vorBsSIx)
{
	const char *cFNCN = __func__;
	Tst_err       res;
	Tst_str       *pOutFn = NULL;
	Tst_str const *pInFn  = st_sysFStc_getFilen(pFStcIn);
	Tst_fsize     cp      = 0
	              /*totCp   = 0*/;
	char          debMsg[128],
	              debMsg2[128];
	Tst_sys_fstc  fstcOut;

	ST_ASSERTN_IARG(pCmdln == NULL ||
			pFStcIn == NULL || pTDesc == NULL || pFnExt == NULL)

	if (binSize == 0) {
		return ST_ERR_SUCC;
	}

	/* create out-filename */
	res = ast_mf_fnc_createOutFn(pInFn,
			(Tst_str*)pFnExt, (Tst_str*)"tag",
			(pVor != NULL && st_vorbc_gs_getTag_wasEmbedded(pVor) ?
				vorBsIx : 0),
			(pVor != NULL && st_vorbc_gs_getTag_wasEmbedded(pVor) ?
				(vorBsIx == 0 ? 0 : (vorBsSIx == 0 ? 1 : vorBsSIx)) : 0),
			pCmdln->opts.pOutpdir, pCmdln->opts.owExFiles,
			&pOutFn);
	if (res != ST_ERR_SUCC) {
		return res;
	}

	/* */
	if (pVor == NULL) {
		/* open file for writing */
		res = st_sys_stc_initFStc(&fstcOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pOutFn)
			return res;
		}
		if (! pCmdln->opts.basOpts.pretWr) {
			if (pCmdln->opts.owExFiles && st_sysDoesFileExist(pOutFn)) {
				/* remove existing file */
				if (! st_sysUnlinkFile(pOutFn)) {
					ast_mf_op_d_fileErr(pInFn, cFNCN,
							"can't delete output-file '%s'", pOutFn);
					ST_DELPOINT(pOutFn)
					st_sys_stc_freeFStc(&fstcOut);
					return ST_ERR_FAIL;
				}
			}
			st_sysFStc_setFilen(&fstcOut, pOutFn);
			res = st_sysFStc_openNew(&fstcOut);
			if (res != ST_ERR_SUCC) {
				ast_mf_op_d_fileErr(pInFn, cFNCN,
						"can't open output-file '%s'", pOutFn);
				ST_DELPOINT(pOutFn)
				st_sys_stc_freeFStc(&fstcOut);
				return res;
			}
		}

		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN,
					"extracting %s Tag from \"%s\"...%s",
					pTDesc, pInFn,
					(pCmdln->opts.basOpts.pretWr ? "  (pretending)" : ""));

		/* copy raw tag(s) to output */
		if (binSize > 0) {
			if (! st_sysFStc_setFPos(pFStcIn, offs, ST_SYS_SETFPOS_BEG)) {
				ast_mf_op_d_fileErr(pInFn, cFNCN,
					"can't seek in input-file '%s'",
					pInFn);
				res = ST_ERR_FAIL;
			} else {
				res = st_sysFStc_copyFromAndToFile(pCmdln->opts.basOpts.pretWr,
						pFStcIn, &fstcOut, (Tst_fsize)binSize, &cp);
				if (res == ST_ERR_SUCC && cp != (Tst_fsize)binSize) {
					ast_mf_op_d_fileErr(pInFn, cFNCN,
							"couldn't copy whole tag from file '%s'",
							pInFn);
					res = ST_ERR_FAIL;
				} else if (res == ST_ERR_FCRD) {
					ast_mf_op_d_fileErr(pInFn, cFNCN,
							"can't read from file '%s'",
							pInFn);
				} else if (res == ST_ERR_FCWR) {
					ast_mf_op_d_fileErr(pInFn, cFNCN,
							"can't write to file '%s'", pOutFn);
				} else if (res != ST_ERR_SUCC) {
					ast_mf_op_d_fileErr(pInFn, cFNCN,
							"copying file failed");
				} /*else {
					totCp += cp;
				}*/
			}
		}

		/* */
		if (st_sysFStc_isOpen(&fstcOut)) {
			st_sysFStc_close(&fstcOut);
		}
		st_sys_stc_freeFStc(&fstcOut);
	} else if (pVor != NULL && ! pCmdln->opts.basOpts.pretWr) {
		/* copy raw tag(s) to output */
		res = st_vorbc_gs_getRawTagIntoFile(pVor, pOutFn);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_fileErr(pInFn, cFNCN,
					"extracting raw vorbis tag failed");
		}
	}

	/* */
	if (res == ST_ERR_SUCC) {
		if (! pCmdln->opts.quiet) {
			debMsg[0] = 0;
			if (binSize >= 1024 * 1024) {
				sprintf(debMsg, ", %.2f MB",
						(double)binSize / (double)(1024 * 1024));
			} else if (binSize > 1024) {
				sprintf(debMsg, ", %.2f KB",
						(double)binSize / (double)(1024));
			}
			debMsg2[0] = 0;
			if (pCmdln->opts.basOpts.pretWr) {
				sprintf(debMsg2, "  (pretended)");
			}
			ast_mf_op_prMsg("*  extract %s Tag: wrote '%s' ("
					ST_TFSIZE_PRF_LU" bytes%s)%s",
					pTDesc, pOutFn,
					(Tst_fsize)binSize, debMsg, debMsg2);
		}
	} else if (! pCmdln->opts.basOpts.pretWr) {
		st_sysUnlinkFile(pOutFn);
	}

	ST_DELPOINT(pOutFn)
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Is index ix in list ?
 *
 * returns: true if found
 */
static Tst_bool
AST_MF__extr_isIndexInLst(const Tst_uint32 ix,
		const Tst_uint32 *pIxArr)
{
	Tst_bool         res  = ST_B_FALSE;
	Tst_uint32 const *pPt = pIxArr;

	while (*pPt != 0) {
		if (*pPt == ix) {
			res = ST_B_TRUE;
			break;
		}
		++pPt;
	}
	return res;
}

/*
 * Extracts binary data from a Tag
 *   works only for (converted) ID3v2 Tags
 */
static Tst_err
AST_MF__extr_extrOther_sub(const Tast_cln_a *pCmdln,
		const Tst_str *pFnIn, const char *pTDesc, const char *pDDesc,
		const Tst_bool extrAll, ST_OPTARG(const Tst_uint32 *pIxArr),
		const Tst_id3v2_frID fldID, const Tst_id3v2_frTp fldType,
		Tst_id3v2_tag *pTag, const Tst_uint32 tagNr)
{
	const char *cFNCN = __func__;
	Tst_err       res     = ST_ERR_SUCC;
	Tst_str       *pOutFn = NULL,
	              fext1[64],
	              fext2[64],
	              *pCh;
	//Tst_str const *pItFldIDstr;
	Tst_uint32    itFldNr;
	Tst_bool      extrOne = ST_B_FALSE;
	char const    *pFExtTmp;
	Tst_id3v2_fldData    *pItFld = NULL;
	Tst_id3v2_fldProp    fldPr;
	Tst_id3v2_frTp       itFldType;
	Tst_id3v2_frID       itFldID;
	Tst_id3v2_fldd_dattp itFldDatTp;

	ST_ASSERTN_IARG(pCmdln == NULL ||
			pFnIn == NULL || pTDesc == NULL || pDDesc == NULL || pTag == NULL)

	switch (fldType) {
	case ST_ID3V2_FTP_PIC:
	case ST_ID3V2_FTP_GEO:
		break;
	case ST_ID3V2_FTP_CDM:
		return ST_ERR_IARG;
	/*case ST_ID3V2_FTP_UFI:
	case ST_ID3V2_FTP_MCI:
	case ST_ID3V2_FTP_SLT:
	case ST_ID3V2_FTP_CPY:*/
	default:
		break;
	}

	fext2[0] = 0x00;

	/* extract data from all applying fields */
	st_id3v2_stc_rsetFPr(&fldPr);
	st_id3v2_stc_rsetDatTp(&itFldDatTp);
	while ((pItFld = st_id3v2_ite_nextValidFld(pTag, pItFld)) != NULL) {
		st_id3v2_gs_getField_props(pItFld, &fldPr);
		itFldType   = st_id3v2_gs_getFieldProp_typ(&fldPr);
		itFldID     = st_id3v2_gs_getFieldProp_id(&fldPr);
		//pItFldIDstr = st_id3v2_gs_getFieldProp_idStr(&fldPr);
		itFldNr     = st_id3v2_gs_getField_nr(pItFld);

		if ((fldType != ST_ID3V2_FTP_NONE && itFldType != fldType) ||
				(fldID != ST_ID3V2_FID_NONE && itFldID != fldID) ||
				itFldType == ST_ID3V2_FTP_CDM)
			continue;
		if (! extrAll &&
				(pIxArr == NULL || ! AST_MF__extr_isIndexInLst(itFldNr, pIxArr)))
			continue;
		st_id3v2_fnc_getFieldDataTypes(itFldID, &itFldDatTp);

		if (! itFldDatTp.hasBDat || st_id3v2_gs_getFieldData_binSz(pItFld) == 0)
			continue;

		/* */
		switch (fldType) {
		case ST_ID3V2_FTP_PIC:
		case ST_ID3V2_FTP_GEO:
			fext1[0] = 0x00;
			if (fldType == ST_ID3V2_FTP_PIC) {
				if (tagNr > 0)
					sprintf((char*)fext1, "t%02u-apic_%02u", tagNr, itFldNr);
				else
					sprintf((char*)fext1, "apic_%02u", itFldNr);
				pFExtTmp = (char*)st_utilsFmt_getPicFmt_fext_byEnum(
						st_id3v2_gs_getFieldAttr_picFmt(pItFld));
				if (pFExtTmp == NULL)
					pFExtTmp = "xxx";
			} else {
				Tst_str const *pGeoMim;

				if (tagNr > 0)
					sprintf((char*)fext1, "t%02u-geob_%02u", tagNr, itFldNr);
				else
					sprintf((char*)fext1, "geob_%02u", itFldNr);
				pGeoMim = st_id3v2_gs_getFieldAttr_geobMime(pItFld);
				if (pGeoMim != NULL)
					pFExtTmp = (char const*)st_utilsFmt_getMime_defFExt_byMime(pGeoMim);
				else
					pFExtTmp = NULL;
				if (pFExtTmp == NULL)
					pFExtTmp = "bin";
			}
			/* */
			strcpy((char*)fext2, pFExtTmp);
			break;
		case ST_ID3V2_FTP_CDM:
			continue;
		/*case ST_ID3V2_FTP_UFI:
		case ST_ID3V2_FTP_MCI:
		case ST_ID3V2_FTP_SLT:
		case ST_ID3V2_FTP_CPY:*/
		default:
			if (itFldType == ST_ID3V2_FTP_PIC || itFldType == ST_ID3V2_FTP_GEO ||
					itFldType == ST_ID3V2_FTP_CDM)
				continue;
			if (tagNr > 0)
				sprintf((char*)fext1, "t%02u-%s_%02u",
						tagNr, (char*)st_id3v2_gs_getFieldProp_idStr(&fldPr), itFldNr);
			else
				sprintf((char*)fext1, "%s_%02u",
						(char*)st_id3v2_gs_getFieldProp_idStr(&fldPr), itFldNr);
			st_sysStrToLower(fext1);
			while ((pCh = (Tst_str*)strrchr((char*)fext1, ' ')) != NULL)
				*pCh = '_';
			/* */
			strcpy((char*)fext2, "bin");
		}
		/* create out-filename */
		res = ast_mf_fnc_createOutFn(pFnIn,
				fext1, fext2,
				0, 0,
				pCmdln->opts.pOutpdir, pCmdln->opts.owExFiles,
				&pOutFn);
		if (res != ST_ERR_SUCC)
			return res;

		/* write bin data to file */
		if (! pCmdln->opts.basOpts.pretWr) {
			if (pCmdln->opts.owExFiles && st_sysDoesFileExist(pOutFn)) {
				/* remove existing file */
				if (! st_sysUnlinkFile(pOutFn)) {
					ast_mf_op_d_fileErr(pFnIn, cFNCN,
							"can't delete output-file '%s'", pOutFn);
					res = ST_ERR_FAIL;
				}
			}
			if (res == ST_ERR_SUCC) {
				res = st_id3v2_gs_getFieldData_binToFile(pItFld, pOutFn);
				if (res != ST_ERR_SUCC)
					ast_mf_op_d_fileErr(pFnIn, cFNCN,
							"copying binary data to file failed");
			}
		}

		/* */
		if (res == ST_ERR_SUCC) {
			if (! pCmdln->opts.quiet) {
				Tst_uint32 binSz = st_id3v2_gs_getFieldData_binSz(pItFld);
				char       debMsg[128],
				           debMsg2[128];

				debMsg[0] = 0;
				if (binSz >= 1024 * 1024)
					sprintf(debMsg, ", %.2f MB",
							(double)binSz / (double)(1024 * 1024));
				else if (binSz > 1024)
					sprintf(debMsg, ", %.2f KB",
							(double)binSz / (double)(1024));
				debMsg2[0] = 0;
				if (pCmdln->opts.basOpts.pretWr)
					sprintf(debMsg2, "  (pretended)");
				ast_mf_op_prMsg("*  extract %s from %s Tag: "
						"wrote '%s' (%u bytes%s)%s",
						pDDesc, pTDesc, pOutFn, binSz, debMsg, debMsg2);
			}
			extrOne = ST_B_TRUE;
		}
		if (res != ST_ERR_SUCC)
			break;
	}
	ST_DELPOINT(pOutFn)
	/* */
	if (res == ST_ERR_SUCC && ! extrOne && ! pCmdln->opts.quiet)
		ast_mf_op_prMsg("*  extract %s from %s Tag: nothing extracted",
				pDDesc, pTDesc);
	/* */
	return res;
}

/******************************************************************************/
