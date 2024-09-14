/******************************************************************************/
/* cln_hndhelp_fnc.c              [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for handling help pages                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_CLN_HNDHELP_FNC_C
#include "cln_hndhelp_fnc-prot.h"
/*** */
#include "cln_fnc-prot.h"
#undef SRC_CLN_HNDHELP_FNC_C

/*
// System-Includes
*/
#include <stdlib.h>      /* free() */
#include <string.h>      /* strchr() */
#include <stdio.h>       /* snprintf() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_CLN__hhlpfnc_initElemIV2(Tast_cln_pvalIV2 *pElem);
static Tst_int32
AST_CLN__hhlpfnc_cbCmpElemsIV2(
		const void *pElem1,
		const void *pElem2,
		void *pOpts
	);
static void
AST_CLN__hhlpfnc_initElemAV2_or_VOR(Tast_cln_pvalAV2_or_VOR *pElem);
static Tst_int32
AST_CLN__hhlpfnc_cbCmpElemsAV2_or_VOR(
		const void *pElem1,
		const void *pElem2,
		void *pOpts
	);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
ast_cln_hhlpfnc_cbRsetElemIV2(void *pElem)
{
	ST_DELPOINT(((Tast_cln_pvalIV2*)pElem)->pID02)
	ST_DELPOINT(((Tast_cln_pvalIV2*)pElem)->pID34)
	ST_DELPOINT(((Tast_cln_pvalIV2*)pElem)->pCap)
	ST_DELPOINT(((Tast_cln_pvalIV2*)pElem)->pFTPstr)
	((Tast_cln_pvalIV2*)pElem)->ftp   = ST_ID3V2_FTP_NONE;
	((Tast_cln_pvalIV2*)pElem)->verFl = 0;
}

void
ast_cln_hhlpfnc_cbRsetElemAV2_or_VOR(void *pElem)
{
	ST_DELPOINT(((Tast_cln_pvalAV2_or_VOR*)pElem)->pID)
	ST_DELPOINT(((Tast_cln_pvalAV2_or_VOR*)pElem)->pCap)
	ST_DELPOINT(((Tast_cln_pvalAV2_or_VOR*)pElem)->pFTPstr)
	((Tast_cln_pvalAV2_or_VOR*)pElem)->ftpAV2 = ST_APEV2_FTP_NONE;
	((Tast_cln_pvalAV2_or_VOR*)pElem)->ftpVOR = ST_VORBC_FTP_NONE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* for <tag>, <tagpair>, <taglist> */
void
ast_cln_hhlpfnc_fmt_tag(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s|%s>\"",
			AST_CLN_SVT_FMT_TAG, AST_CLN_SVT_FMT_TAGLONG,
			AST_CLN_SVT_FMT_TAGSHORT);
}

void
ast_cln_hhlpfnc_fmt_tagpair(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s>-<%s>\"",
			AST_CLN_SVT_FMT_TAGPAIR, AST_CLN_SVT_FMT_TAGLIST,
			AST_CLN_SVT_FMT_TAG);
}

void
ast_cln_hhlpfnc_fmt_taglist(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s>[,<%s>[...]]\"",
			AST_CLN_SVT_FMT_TAGLIST, AST_CLN_SVT_FMT_TAG, AST_CLN_SVT_FMT_TAG);
}

/* for <vlev>, <vlevlist> */
void
ast_cln_hhlpfnc_fmt_vlev(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s|%s|%s>\"",
			AST_CLN_SVT_FMT_VLEV, AST_CLN_SVT_FMT_NUM,
			AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_STRSHORT);
}

void
ast_cln_hhlpfnc_fmt_vlevlist(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s>[,<%s>[...]]\"",
			AST_CLN_SVT_FMT_VLEVLIST, AST_CLN_SVT_FMT_VLEV,
			AST_CLN_SVT_FMT_VLEV);
}

/* for <bs>, <bslist> */
void
ast_cln_hhlpfnc_fmt_bs(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s>[.<%s>]\"",
			AST_CLN_SVT_FMT_BS, AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_NUM);
	pCmdln->cbMsg("  the first <%s> being the number of the main bitstream",
			AST_CLN_SVT_FMT_NUM);
	pCmdln->cbMsg("  the second <%s> being the number of the sub-bitstream",
			AST_CLN_SVT_FMT_NUM);
}

void
ast_cln_hhlpfnc_fmt_bslist(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s>[,<%s>[...]]\"",
			AST_CLN_SVT_FMT_BSLIST, AST_CLN_SVT_FMT_BS, AST_CLN_SVT_FMT_BS);
}

/* for <numrange>, <numlist> */
void
ast_cln_hhlpfnc_fmt_numrange(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s[-<%s>]>\"",
			AST_CLN_SVT_FMT_NUMRANGE, AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_NUM);
}

void
ast_cln_hhlpfnc_fmt_numlist(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s|%s|%s>[,<%s|%s|%s>[...]]\"",
			AST_CLN_SVT_FMT_NUMLIST,
			AST_CLN_SVT_FMT_NUMRANGE, AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_STRSHORT,
			AST_CLN_SVT_FMT_NUMRANGE, AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_STRSHORT);
}

/* for <pos> */
void
ast_cln_hhlpfnc_fmt_pos(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s>[/[<%s>]]\" or \"/<%s>\"",
			AST_CLN_SVT_FMT_POS,
			AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_NUM);
}

/* for <fldname> */
void
ast_cln_hhlpfnc_fmt_fldname(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s|%s>\"",
			AST_CLN_SVT_FMT_FLDNAME,
			AST_CLN_SVT_FMT_FNSHORT, AST_CLN_SVT_FMT_FNLONG);
}

/* for <fldadd>/<fldset>/<dscset> */
void
ast_cln_hhlpfnc_fmt_flddscaddset(Tast_cln_a *pCmdln, const int fmt,
		const Tst_bool isTagOptional,
		ST_OPTARG(const char *pTagLong), ST_OPTARG(const char *pTagShort))
{
	char msgTagPre[64];
	char msgTag[64 * 2];
	char msgFmtFLDNAME[256];
	char msgFmtSTR[256];

	ST_ASSERTN_VOID(pCmdln == NULL)

	if (pTagLong == NULL || pTagShort == NULL)
		snprintf(msgTagPre, sizeof(msgTagPre), "<%s>", AST_CLN_SVT_FMT_TAG);
	else
		snprintf(msgTagPre, sizeof(msgTagPre), "(%s|%s)", pTagLong, pTagShort);
	snprintf(msgTag, sizeof(msgTag), "%s%s:%s",
			(isTagOptional ? "[" : ""), msgTagPre, (isTagOptional ? "]" : ""));

	switch (fmt) {
	case AST_CLN_HHLP_FMT_FADD_FLDNAME:
	case AST_CLN_HHLP_FMT_FADD_STR:
	case AST_CLN_HHLP_FMT_FADD_FLDNAME|AST_CLN_HHLP_FMT_FADD_STR:
		snprintf(msgFmtFLDNAME, sizeof(msgFmtFLDNAME),
				"%s<%s>=<%s|%s>",
				msgTag, AST_CLN_SVT_FMT_FLDNAME,
				AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_NUM);
		snprintf(msgFmtSTR, sizeof(msgFmtSTR),
				"%s<%s>=<%s|%s>",
				msgTag, AST_CLN_SVT_FMT_STR,
				AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_NUM);
		break;
	case AST_CLN_HHLP_FMT_FSET_FLDNAME:
	case AST_CLN_HHLP_FMT_FSET_STR:
	case AST_CLN_HHLP_FMT_FSET_FLDNAME|AST_CLN_HHLP_FMT_FSET_STR:
		snprintf(msgFmtFLDNAME, sizeof(msgFmtFLDNAME),
				"%s<%s>[.<%s>]=[<%s|%s>]",
				msgTag, AST_CLN_SVT_FMT_FLDNAME,
				AST_CLN_SVT_FMT_NUM,
				AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_NUM);
		snprintf(msgFmtSTR, sizeof(msgFmtSTR),
				"%s<%s>[.<%s>]=[<%s|%s>]",
				msgTag, AST_CLN_SVT_FMT_STR,
				AST_CLN_SVT_FMT_NUM,
				AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_NUM);
		break;
	case AST_CLN_HHLP_FMT_DSET_FLDNAME:
	case AST_CLN_HHLP_FMT_DSET_STR:
	case AST_CLN_HHLP_FMT_DSET_FLDNAME|AST_CLN_HHLP_FMT_DSET_STR:
		snprintf(msgFmtFLDNAME, sizeof(msgFmtFLDNAME),
				"<%s>[.<%s>]=[<%s>]",
				AST_CLN_SVT_FMT_FLDNAME, AST_CLN_SVT_FMT_NUM,
				AST_CLN_SVT_FMT_STR);
		snprintf(msgFmtSTR, sizeof(msgFmtSTR),
				"<%s>[.<%s>]=[<%s>]",
				AST_CLN_SVT_FMT_STR, AST_CLN_SVT_FMT_NUM,
				AST_CLN_SVT_FMT_STR);
		break;
	default:
		break;
	}

	switch (fmt) {
	case AST_CLN_HHLP_FMT_FADD_FLDNAME:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_FLDADD, msgFmtFLDNAME);
		break;
	case AST_CLN_HHLP_FMT_FADD_STR:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_FLDADD, msgFmtSTR);
		break;
	case AST_CLN_HHLP_FMT_FADD_FLDNAME|AST_CLN_HHLP_FMT_FADD_STR:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_FLDADD, msgFmtFLDNAME);
		pCmdln->cbMsg("                   or \"%s\"", msgFmtSTR);
		break;
	case AST_CLN_HHLP_FMT_FSET_FLDNAME:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_FLDSET, msgFmtFLDNAME);
		break;
	case AST_CLN_HHLP_FMT_FSET_STR:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_FLDSET, msgFmtSTR);
		break;
	case AST_CLN_HHLP_FMT_FSET_FLDNAME|AST_CLN_HHLP_FMT_FSET_STR:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_FLDSET, msgFmtFLDNAME);
		pCmdln->cbMsg("                   or \"%s\"", msgFmtSTR);
		break;
	case AST_CLN_HHLP_FMT_DSET_FLDNAME:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_DSCSET, msgFmtFLDNAME);
		break;
	case AST_CLN_HHLP_FMT_DSET_STR:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_DSCSET, msgFmtSTR);
		break;
	case AST_CLN_HHLP_FMT_DSET_FLDNAME|AST_CLN_HHLP_FMT_DSET_STR:
		pCmdln->cbMsg("Format of <%s> is \"%s\"",
				AST_CLN_SVT_FMT_DSCSET, msgFmtFLDNAME);
		pCmdln->cbMsg("                   or \"%s\"", msgFmtSTR);
		break;
	default:
		break;
	}
}

/* for <tiv2optlist> */
void
ast_cln_hhlpfnc_fmt_tiv2optlist(Tast_cln_a *pCmdln)
{
	pCmdln->cbMsg("Format of <%s> is \"<%s>[,<%s>[...]]\"",
			AST_CLN_SVT_FMT_TIV2OPTLIST,
			AST_CLN_SVT_FMT_TIV2OPT, AST_CLN_SVT_FMT_TIV2OPT);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* for --fadd/fset hvIV2 */
const char*
ast_cln_hhlpfnc_c_faddset_posvalsIV2_ftp(const Tst_id3v2_frTp ftp)
{
	switch (ftp) {
	case ST_ID3V2_FTP_COM: return "com";
	case ST_ID3V2_FTP_SLT: return "slt";
	case ST_ID3V2_FTP_PIC: return "pic";
	case ST_ID3V2_FTP_TXT: return "txt";
	case ST_ID3V2_FTP_URL: return "url";
	case ST_ID3V2_FTP_UFI: return "ufi";
	case ST_ID3V2_FTP_IPL: return "ipl";
	case ST_ID3V2_FTP_MCI: return "mci";
	case ST_ID3V2_FTP_CDM: return "cdm";
	case ST_ID3V2_FTP_WXX: return "wxx";
	case ST_ID3V2_FTP_GEO: return "geo";
	case ST_ID3V2_FTP_CNT: return "cnt";
	case ST_ID3V2_FTP_POP: return "pop";
	case ST_ID3V2_FTP_CPY: return "cpy";
	default:
		return "   ";
	}
}

Tst_bool
ast_cln_hhlpfnc_c_faddset_posvalsIV2_genlist(const Tst_bool fldOrDsc,
		const Tst_bool addOrSet, Tst_dynlist *pPVlist)
{
	#define LOC_CPYSTR_(mac_pSrcDstName, mac_len)  { \
				slen = st_sysStrlen2(mac_pSrcDstName); \
				ST_CALLOC(pNewElem->mac_pSrcDstName, char*, slen + 4 + 1, 1) \
				if (pNewElem->mac_pSrcDstName == NULL) { \
					return ST_B_FALSE; \
				} \
				if (mac_pSrcDstName == NULL) { \
					if (mac_len == 3 || mac_len == 4) { \
						strcpy(pNewElem->mac_pSrcDstName, \
								(mac_len == 3 ? "   " : "    ")); \
					} else { \
						pNewElem->mac_pSrcDstName[0] = 0; \
					} \
				} else { \
					strcpy(pNewElem->mac_pSrcDstName, mac_pSrcDstName); \
				} \
			}
	char const *pID02;
	char const *pID34;
	char const *pCap;
	char const *pFTPstr;
	Tst_uint32 verFl;
	Tst_uint32 slen;
	Tst_id3v2_fldProp fldPrCur;
	Tst_id3v2_fldProp *pFldPrLast = NULL;
	Tst_id3v2_frTp    ftp;
	Tst_id3v2_frID    fid;
	Tast_cln_pvalIV2  *pNewElem = NULL;

	st_id3v2_stc_rsetFPr(&fldPrCur);
	while (st_id3v2_ite_nextTagFldProp(pFldPrLast, &fldPrCur) != NULL) {
		pFldPrLast = &fldPrCur;
		/* */
		fid     = st_id3v2_gs_getFieldProp_id(&fldPrCur);
		ftp     = st_id3v2_gs_getFieldProp_typ(&fldPrCur);
		pFTPstr = ast_cln_hhlpfnc_c_faddset_posvalsIV2_ftp(ftp);
		pID02   = (char const*)st_id3v2_gs_getFieldProp_idStr_v02(&fldPrCur);
		pID34   = (char const*)st_id3v2_gs_getFieldProp_idStr_v34(&fldPrCur);
		pCap    = (char const*)st_id3v2_gs_getFieldProp_caption(&fldPrCur);
		verFl   = st_id3v2_gs_getFieldProp_verFl(&fldPrCur);
		if ((pID02 == NULL && pID34 == NULL) || pCap == NULL ||
				(pID34 != NULL && pID34[3] == ' '))
			continue;
		/* */
		if (fid == ST_ID3V2_FID_234_TRCK || fid == ST_ID3V2_FID_234_TPOS)
			continue;
		if (fldOrDsc) {
			if (ftp == ST_ID3V2_FTP_GEO || ftp == ST_ID3V2_FTP_PIC)
				continue;
			if (addOrSet &&
					(ftp == ST_ID3V2_FTP_CDM || ftp == ST_ID3V2_FTP_CPY ||
						ftp == ST_ID3V2_FTP_SLT))
				continue;
		} else {
			if (addOrSet)
				continue;
			if (! (ftp == ST_ID3V2_FTP_COM || ftp == ST_ID3V2_FTP_WXX ||
						ftp == ST_ID3V2_FTP_UFI || ftp == ST_ID3V2_FTP_SLT ||
						ftp == ST_ID3V2_FTP_GEO || ftp == ST_ID3V2_FTP_PIC))
				continue;
		}
		/* */
		ST_CALLOC(pNewElem, Tast_cln_pvalIV2*, 1, sizeof(Tast_cln_pvalIV2))
		if (pNewElem == NULL)
			return ST_B_FALSE;
		AST_CLN__hhlpfnc_initElemIV2(pNewElem);
		/* */
		/** */
		LOC_CPYSTR_(pID02, 3)
		LOC_CPYSTR_(pID34, 4)
		LOC_CPYSTR_(pCap, 0)
		LOC_CPYSTR_(pFTPstr, 0)
		pNewElem->ftp   = ftp;
		pNewElem->verFl = verFl;
		/* */
		if (st_dynlist_addElement(pPVlist, pNewElem) != ST_ERR_SUCC)
			return ST_B_FALSE;
	}

	return st_dynlist_sortElements(pPVlist, ST_DYNLIST_SA_DEF,
			AST_CLN__hhlpfnc_cbCmpElemsIV2, NULL, NULL);
	#undef LOC_CPYSTR_
}

/*----------------------------------------------------------------------------*/

/* for --fadd/fset hvAV2/hvVOR */
const char*
ast_cln_hhlpfnc_c_faddset_posvalsAV2_ftp(const Tst_apev2_frTp ftp)
{
	switch (ftp) {
	case ST_APEV2_FTP_STR:  return "str";
	case ST_APEV2_FTP_INT:  return "int";
	case ST_APEV2_FTP_FLT:  return "fpn";
	case ST_APEV2_FTP_YEAR: return "yea";
	case ST_APEV2_FTP_TSTA: return "tst";
	case ST_APEV2_FTP_LYR:  return "lyr";
	case ST_APEV2_FTP_URL:  return "url";
	case ST_APEV2_FTP_BDAT: return "bin";
	default:
		return "   ";
	}
}

const char*
ast_cln_hhlpfnc_c_faddset_posvalsVOR_ftp(const Tst_vorbc_frTp ftp)
{
	switch (ftp) {
	case ST_VORBC_FTP_STR:  return "str";
	case ST_VORBC_FTP_PICT: return "pic";
	default:
		return "   ";
	}
}

Tst_bool
ast_cln_hhlpfnc_c_faddset_posvalsAV2_or_VOR_genlist(Tst_dynlist *pPVlist,
		const Tst_bool av2OrVor, Tst_uint32 *pFldnMaxlen)
{
	#define LOC_CPYSTR_(mac_pSrcDstName, mac_updFML)  { \
				slen = st_sysStrlen2(mac_pSrcDstName); \
				if (mac_updFML && slen > *pFldnMaxlen) { \
					*pFldnMaxlen = slen; \
				} \
				ST_CALLOC(pNewElem->mac_pSrcDstName, char*, slen + 1, 1) \
				if (pNewElem->mac_pSrcDstName == NULL) { \
					return ST_B_FALSE; \
				} \
				if (mac_pSrcDstName == NULL) { \
					pNewElem->mac_pSrcDstName[0] = 0; \
				} else { \
					strcpy(pNewElem->mac_pSrcDstName, mac_pSrcDstName); \
				} \
			}
	char const *pID;
	char const *pCap;
	char const *pFTPstr;
	Tst_uint32 slen;
	Tst_apev2_fldProp       fldPrCurAV2;
	Tst_apev2_fldProp       *pFldPrLastAV2 = NULL;
	Tst_vorbc_fldProp       fldPrCurVOR;
	Tst_vorbc_fldProp       *pFldPrLastVOR = NULL;
	Tst_apev2_frTp          ftpAV2         = ST_APEV2_FTP_NONE;
	Tst_vorbc_frTp          ftpVOR         = ST_VORBC_FTP_NONE;
	Tast_cln_pvalAV2_or_VOR *pNewElem      = NULL;

	*pFldnMaxlen = 0;

	st_apev2_stc_rsetFPr(&fldPrCurAV2);
	st_vorbc_stc_rsetFPr(&fldPrCurVOR);
	while ((av2OrVor &&
				st_apev2_ite_nextTagFldProp(pFldPrLastAV2, &fldPrCurAV2) != NULL) ||
			(! av2OrVor &&
				st_vorbc_ite_nextTagFldProp(pFldPrLastVOR, &fldPrCurVOR) != NULL)) {
		pFldPrLastAV2 = &fldPrCurAV2;
		pFldPrLastVOR = &fldPrCurVOR;
		/* */
		if (av2OrVor)
			pID = (char const*)st_apev2_gs_getFieldProp_idStr(&fldPrCurAV2);
		else
			pID = (char const*)st_vorbc_gs_getFieldProp_idStr(&fldPrCurVOR);
		if (pID == NULL)
			continue;
		if (av2OrVor) {
			ftpAV2 = st_apev2_gs_getFieldProp_typ(&fldPrCurAV2);
			if (ftpAV2 == ST_APEV2_FTP_BDAT)
				continue;
			pFTPstr = ast_cln_hhlpfnc_c_faddset_posvalsAV2_ftp(ftpAV2);
			pCap    = (char const*)st_apev2_gs_getFieldProp_caption(&fldPrCurAV2);
		} else {
			ftpVOR = st_vorbc_gs_getFieldProp_typ(&fldPrCurVOR);
			if (ftpVOR == ST_VORBC_FTP_PICT)
				continue;
			pFTPstr = ast_cln_hhlpfnc_c_faddset_posvalsVOR_ftp(ftpVOR);
			pCap    = (char const*)st_vorbc_gs_getFieldProp_caption(&fldPrCurVOR);
		}
		/* */
		ST_CALLOC(pNewElem, Tast_cln_pvalAV2_or_VOR*,
				1, sizeof(Tast_cln_pvalAV2_or_VOR))
		if (pNewElem == NULL)
			return ST_B_FALSE;
		AST_CLN__hhlpfnc_initElemAV2_or_VOR(pNewElem);
		/* */
		/** */
		LOC_CPYSTR_(pID, ST_B_TRUE)
		LOC_CPYSTR_(pCap, ST_B_FALSE)
		LOC_CPYSTR_(pFTPstr, ST_B_FALSE)
		if (av2OrVor)
			pNewElem->ftpAV2 = ftpAV2;
		else
			pNewElem->ftpVOR = ftpVOR;
		/* */
		if (st_dynlist_addElement(pPVlist, pNewElem) != ST_ERR_SUCC)
			return ST_B_FALSE;
	}

	return st_dynlist_sortElements(pPVlist, ST_DYNLIST_SA_DEF,
			AST_CLN__hhlpfnc_cbCmpElemsAV2_or_VOR, NULL, NULL);
	#undef LOC_CPYSTR_
}

/*----------------------------------------------------------------------------*/

/* for --genre */
void
ast_cln_hhlpfnc_c_genre_part(Tast_cln_a *pCmdln)
{
	Tst_uint32 x = 0;

	pCmdln->cbMsg(" For ID3v1 you should only use one of the above genres\n");
	pCmdln->cbMsg(" For ID3v2 you can also use one of these special genres:");
	while (ST_ID3_GENRES_SPEC[x].pIDstr != NULL &&
			ST_ID3_GENRES_SPEC[x].pCap != NULL)
		pCmdln->cbMsg("   %s", ST_ID3_GENRES_SPEC[x++].pCap);
	pCmdln->cbMsg(" And for all tags except ID3v1 you may "
			"also specify your own genres");
	pCmdln->cbMsg("  (e.g. \"Deep Throat\")");
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg(" The special values 255 and \"\" "
			"reset all appropriate tag fields.");
}

/*----------------------------------------------------------------------------*/

/* for --lev */
const char*
ast_cln_hhlpfnc_o_verblev_getCap(const Tst_int32 lev,
		const Tst_bool getSh)
{
	Tst_uint32 x = 0;

	while (AST_CLN_VERBLIST_CAPDESC[x].lev >= 0 &&
			AST_CLN_VERBLIST_CAPDESC[x].pCap != NULL)
		if (AST_CLN_VERBLIST_CAPDESC[x].lev == lev) {
			if (getSh)
				return AST_CLN_VERBLIST_CAPDESC[x].pCapSh;
			else
				return AST_CLN_VERBLIST_CAPDESC[x].pCap;
		} else
			++x;
	return NULL;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_CLN__hhlpfnc_initElemIV2(Tast_cln_pvalIV2 *pElem)
{
	pElem->pID02   = NULL;
	pElem->pID34   = NULL;
	pElem->pCap    = NULL;
	pElem->pFTPstr = NULL;
	pElem->ftp     = ST_ID3V2_FTP_NONE;
	pElem->verFl   = 0;
}

static Tst_int32
AST_CLN__hhlpfnc_cbCmpElemsIV2(const void *pElem1, const void *pElem2, void *pOpts)
{
	return st_sysStrcmp2(ST_B_FALSE,
			((const Tast_cln_pvalIV2*)pElem1)->pCap,
			((const Tast_cln_pvalIV2*)pElem2)->pCap);
}

static void
AST_CLN__hhlpfnc_initElemAV2_or_VOR(Tast_cln_pvalAV2_or_VOR *pElem)
{
	pElem->pID     = NULL;
	pElem->pCap    = NULL;
	pElem->pFTPstr = NULL;
	pElem->ftpAV2  = ST_APEV2_FTP_NONE;
	pElem->ftpVOR  = ST_VORBC_FTP_NONE;
}

static Tst_int32
AST_CLN__hhlpfnc_cbCmpElemsAV2_or_VOR(const void *pElem1, const void *pElem2,
		void *pOpts)
{
	return st_sysStrcmp2(ST_B_FALSE,
			((const Tast_cln_pvalAV2_or_VOR*)pElem1)->pCap,
			((const Tast_cln_pvalAV2_or_VOR*)pElem2)->pCap);
}

/******************************************************************************/
