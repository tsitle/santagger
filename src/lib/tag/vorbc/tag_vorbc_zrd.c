/******************************************************************************/
/* tag_vorbc_zrd.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for reading Vorbis Tags                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/tag/tag_vorbc.h"
/** */
#define SRC_TAG_VORBC_ZRD_C
#include "tag_vorbc_zrd-pp.h"
/*** */
#include "tag_vorbc_fnc-prot.h"
#include "tag_vorbc_stcs-prot.h"
#include "tag_vorbc_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_zfdeb-prot.h"
#include "../tag_comfnc-prot.h"
#include "../../utils/w-base64-prot.h"
#undef SRC_TAG_VORBC_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read Vorbis Header
 *   and write it to rawTagBDat
 *
 * returns: ERRCODE
 */
Tst_err
st_vorbc_rd_readHeader(Tst_streamrd *pSObjP,
		Tst_vorbc_tag_intn *pTagI,
		Tst_bool *pIsOK, Tst_uint32 *pNumFlds, Tst_uint32 *pHeadSz)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_uint32 totsize   = 0,
	           vlen,
	           rdFromStr = 0,
	           tmpUI;
	Tst_str    *pTmpVend = NULL;
	Tst_buf    phBuf[ST_VORBC_TAG_PHEAD_SLEN + 4];
	Tst_foffs  offs;

	ST_ASSERTN_IARG(pSObjP == NULL || pTagI == NULL ||
			pIsOK == NULL || pNumFlds == NULL || pHeadSz == NULL)

	*pIsOK    = ST_B_FALSE;
	*pNumFlds = 0;
	*pHeadSz  = 0;

	offs = st_streamrd_getCurPos(pSObjP);
	/* read pseudo header: "VORBCOMM" + TOTAL SIZE */
	tmpUI = sizeof(phBuf);
	res   = st_streamrd_rdAheadBuffer(pSObjP, tmpUI, phBuf, &rdFromStr);
	if (res != ST_ERR_SUCC || rdFromStr != tmpUI) {
		if (res == ST_ERR_FEOF)
			res = ST_ERR_SUCC;
		else if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}

	if (memcmp(phBuf, ST_VORBC_TAG_PHEAD, ST_VORBC_TAG_PHEAD_SLEN) != 0)
		return ST_ERR_SUCC;

	st_streamrd_rdSkipBytes(pSObjP, tmpUI, ST_B_TRUE);

	/* read TOTAL SIZE */
	memcpy(&totsize, &phBuf[ST_VORBC_TAG_PHEAD_SLEN], 4);
#	if (WORDS_BIGENDIAN == 1)
	totsize = st_sysReverseByteOrder_UI32(totsize);  /* ENDIAN: LE-->BE */
#	endif
	totsize += ST_VORBC_TAG_PHEAD_SLEN + 4;
	/**st_vorbc_prf("rdHead(): totsz %u\n", totsize);**/

	/* read VENDOR LENGTH */
	res = st_streamrd_rdUInt32(pSObjP,
			ST_STREAMRD_IEND_LE, 32, &vlen);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_FEOF)
			res = ST_ERR_SUCC;
		else if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}
	if ((Tst_fsize)vlen > st_streamrd_getAmountRemainingBytes(pSObjP)) {
		/* corrupt data */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
					"vendor length too big: %u > %u",
					vlen,
					(Tst_uint32)st_streamrd_getAmountRemainingBytes(pSObjP));
		return ST_ERR_SUCC;
	}
	/**st_vorbc_prf("rdHead(): vendlen %u\n", vlen);**/

	/* read VENDOR */
	/** */
	ST_CALLOC(pTmpVend, Tst_str*, vlen + 1, 1)
	if (pTmpVend == NULL)
		return ST_ERR_OMEM;
	/** */
	res = st_streamrd_rdBuffer_noCRC(pSObjP,
			vlen, pTmpVend, &rdFromStr);
	if (res != ST_ERR_SUCC || rdFromStr != vlen) {
		if (res == ST_ERR_FEOF)
			res = ST_ERR_SUCC;
		else if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}
	pTmpVend[vlen] = 0x00;
	res            = st_mtes_copyFromCharp_utf8(pTmpVend, &pTagI->vendor);
	/**st_vorbc_prf("rdHead(): vend '%s'\n", pTmpVend);**/
	if (res == ST_ERR_SUCC) {
		Tst_mtes_strRestr restr;

		/* strip vendor string */
		/** */
		restr.hasR    = ST_B_TRUE;  /* we have restrictions */
		restr.ruleAlw = ST_B_TRUE;  /* mode: allow */
		st_mtes_stc_rsetRestr(&restr, ST_B_TRUE);
		/** disallow '\n' and '\t' */
		restr.disNewln = ST_B_TRUE;
		restr.disTab   = ST_B_TRUE;
		/** */
		res = st_mtes_stripStr(&pTagI->vendor, &restr, NULL, NULL);
		if (res == ST_ERR_SUCC)
			res = st_mtes_copyToCharp_utf8(&pTagI->vendor, &pTmpVend);
	}
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pTmpVend)
		return res;
	}
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdebStr(&pTagI->opts, 1, cFNCN, NULL,
				"ven", &pTagI->vendor, "");

	/* read ELEMENT COUNT */
	res = st_streamrd_rdUInt32(pSObjP,
			ST_STREAMRD_IEND_LE, 32, pNumFlds);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pTmpVend)
		if (res == ST_ERR_FEOF)
			res = ST_ERR_SUCC;
		else if (res != ST_ERR_SUCC)
			st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}
	/**st_vorbc_prf("rdHead(): tfcnt %u\n", *pNumFlds);**/

	/* header size: ["VORBCOMM" + TOTAL SZ] + VEND LEN + VEND + ELEM CNT */
	*pHeadSz = ST_VORBC_TAG_PHEAD_SLEN + 4 + 4 + vlen + 4;

	/* write header to rawTagBDat */
	/** pseudo header: "VORBCOMM" + TOTAL SIZE */
	res = st_binobj_setData(&pTagI->rawTagBDat, phBuf, sizeof(phBuf));
	/** VENDOR LENGTH */
	if (res == ST_ERR_SUCC) {
#		if (WORDS_BIGENDIAN == 1)
		tmpUI = st_sysReverseByteOrder_UI32(vlen);  /* ENDIAN: BE-->LE */
#		else
		tmpUI = vlen;
#		endif
		res = st_binobj_appendData(&pTagI->rawTagBDat, (Tst_buf*)&tmpUI, 4);
	}
	/** VENDOR */
	if (res == ST_ERR_SUCC)
		res = st_binobj_appendData(&pTagI->rawTagBDat, pTmpVend, vlen);
	ST_DELPOINT(pTmpVend)
	/** ELEMENT COUNT */
	if (res == ST_ERR_SUCC) {
#		if (WORDS_BIGENDIAN == 1)
		tmpUI = st_sysReverseByteOrder_UI32(*pNumFlds);  /* ENDIAN: BE-->LE */
#		else
		tmpUI = *pNumFlds;
#		endif
		res = st_binobj_appendData(&pTagI->rawTagBDat, (Tst_buf*)&tmpUI, 4);
	}

	if (res != ST_ERR_SUCC)
		return res;

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
				"Vorbis pseudo header found");

	st_tagBas_gs_setHasTag(&pTagI->tbas, ST_B_TRUE);
	st_tagBas_gs_setOffset(&pTagI->tbas, offs);
	st_tagBas_gs_setSize(&pTagI->tbas, totsize);

	*pIsOK = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read Vorbis Tag fields
 *   and write them to rawTagBDat
 *
 * returns: ERRCODE
 */
Tst_err
st_vorbc_rd_readFlds(Tst_streamrd *pSObjP, Tst_vorbc_tag *pTag,
		const Tst_uint32 headSz, Tst_uint32 numFlds)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err       res    = ST_ERR_SUCC;
	Tst_uint32    rdf    = 1,
	              fldSz  = 0,
	              rdTot  = 0,
	              remTSz = 0,
	              keySz  = 0;
	Tst_str       *pKey  = NULL;
	Tst_bool      appd,
	              skippedField = ST_B_FALSE;
	Tst_streamrd  strrdTag;
	Tst_vorbc_fldData      *pFld;
	Tst_vorbc_fldData_intn *pFldI;
	Tst_vorbc_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pSObjP == NULL ||
			pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_vorbc_tag_intn*)pTag->pObInternal;

	if (st_tagBas_gs_getSize(&pTagI->tbas) < headSz) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
					"invalid tag size: %u < %u",
					st_tagBas_gs_getSize(&pTagI->tbas), headSz);
		return ST_ERR_SUCC;
	}
	remTSz = st_tagBas_gs_getSize(&pTagI->tbas) - headSz;

	/* append all fields from pSObjP to rawTagBDat */
	rdTot = st_binobj_getDataSize(&pTagI->rawTagBDat);
	res   = st_binobj_appendData_streamrd(&pTagI->rawTagBDat, pSObjP,
			remTSz, ST_B_FALSE);
	if (res != ST_ERR_SUCC) {
		st_vorbc_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"can't read from stream");
		return res;
	}
	rdTot = st_binobj_getDataSize(&pTagI->rawTagBDat) - rdTot;
	/* update/correct tag size */
	st_vorbc_rd_updateTagSize(pTagI, cFNCN, ST_B_FALSE, headSz + rdTot);
	remTSz = rdTot;

	if (pTagI->opts.rdOnlyBasics)
		return ST_ERR_SUCC;

	/* attach stream reader, beginning after the pseudo header */
	st_streamrd_stc_initSObj(&strrdTag);
	res = st_binobj_attachStreamrd(&pTagI->rawTagBDat,
			&strrdTag, (Tst_foffs)headSz);
	if (res != ST_ERR_SUCC) {
		st_streamrd_stc_freeSObj(&strrdTag);
		return res;
	}

	if (numFlds > ST_VORBC_MAX_TFLD_CNT) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
					"too many frames for one tag, reducing from %u to %u",
					numFlds, ST_VORBC_MAX_TFLD_CNT);
		numFlds = ST_VORBC_MAX_TFLD_CNT;
	}

	/* read all fields */
	while (rdf <= numFlds && remTSz > 0 && ! st_streamrd_isEOF(&strrdTag)) {
		/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
					"read fld #%02u", rdf);**/
		/* read field header */
		res = ST_TVORBC__rd_readTagFldHd(&pTagI->opts, rdf,
				&strrdTag, &fldSz, &pKey, &keySz, &remTSz);
		if (res != ST_ERR_SUCC && res != ST_ERR_IDAT) {
			if (res == ST_ERR_FEOF)
				res = ST_ERR_IDAT;
			else
				break;
		}
		if (res == ST_ERR_IDAT) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
						"'%s' #%02u: invalid data, skipping field",
						pKey, rdf);
			++pTagI->flds.skipCnt;
			st_streamrd_rdSkipBytes(&strrdTag, fldSz, ST_B_TRUE);
			++rdf;
			res = ST_ERR_SUCC;
			continue;
		}
		if (fldSz == 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
						"'%s' #%02u: empty field, skipping", pKey, rdf);
			++pTagI->flds.skipCnt;
			++rdf;
			continue;
		}

		/* add new field to list */
		res = st_vorbc_fnc_addElemToFlds(pTag,
				ST_VORBC_FID_NONE, pKey, &skippedField, &pFld);
		if (res == ST_ERR_SUCC)
			pFldI = (Tst_vorbc_fldData_intn*)pFld->pObInternal;
		if (res != ST_ERR_SUCC || skippedField) {
			if (res == ST_ERR_SUCC && skippedField &&
					ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
				st_vorbc_d_fdeb(&pTagI->opts, 3, cFNCN, pFldI,
						"too many frames for this ID, skipping (%u > %u)",
						pFldI->fnr, ST_VORBC_MAX_TFLDID_CNT);
			}
			break;
		}

		/* append to existing field ? */
		appd = ST_B_FALSE;
		if (pFldI->fldPrI.typ != ST_VORBC_FTP_PICT)
			appd = ST_TVORBC__rd_shallAppend(&pTagI->flds, &pFld, &pFldI);

		/* read field's data */
		if (appd && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
					"'%s' #%02u: appending data of new field",
					pFldI->fldPrI.pVIDstr, pFldI->fnr);
		/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
				pFldI->fldPrI.id != ST_VORBC_FID_TAGG)
			st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
					"'%s' #%02u: read data (sz %u)",
					pFldI->fldPrI.pVIDstr, pFldI->fnr, fldSz);**/
		if (pFldI->fldPrI.typ == ST_VORBC_FTP_PICT)
			res = ST_TVORBC__rd_copyPict(pTagI, cFNCN,
					&strrdTag, fldSz, pFldI);
		else
			res = ST_TVORBC__rd_copyStrU8(&pTagI->opts, cFNCN,
					&strrdTag, fldSz, pFldI);
		remTSz -= (remTSz >= fldSz ? fldSz : remTSz);
		if (res == ST_ERR_IDAT) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
						"'%s' #%02u: invalid data, skipping field",
						pFldI->fldPrI.pVIDstr, pFldI->fnr);
			++pTagI->flds.skipCnt;
			++rdf;
			res = ST_ERR_SUCC;
			continue;
		}
		if (res != ST_ERR_SUCC)
			break;
		if (pFldI->fldPrI.typ == ST_VORBC_FTP_PICT &&
				st_binobj_getDataSize(&pFldI->dataPicBDat) == 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_vorbc_d_deb(&pTagI->opts, 0, cFNCN,
						"'%s' #%02u: no binary data for picture, skipping field",
						pFldI->fldPrI.pVIDstr, pFldI->fnr);
			++pTagI->flds.skipCnt;
			++rdf;
			continue;
		}
		/* */
		pFldI->isOK = ST_B_TRUE;
		/* */
		if (! appd)
			++rdf;
	}

	if (res == ST_ERR_SUCC) {
		/* Vorbis Tag shall never be 'empty', since the Vendor string
		 *   is always set and most OGG streams always require
		 *   a Vorbis Tag to be present  */
		st_tagBas_gs_setHasFields(&pTagI->tbas, ST_B_TRUE);
	}

	st_binobj_detachStreamrd(&pTagI->rawTagBDat);
	st_streamrd_stc_freeSObj(&strrdTag);
	ST_DELPOINT(pKey)
	return res;
}

/*----------------------------------------------------------------------------*/

void
st_vorbc_rd_updateTagSize(Tst_vorbc_tag_intn *pTagI,
		const char *pFnc,
		const Tst_bool useOffsets,
		Tst_uint32 newSz)
{
	Tst_foffs  offsCur,
	           offsBeg;
	Tst_uint32 curSz;

	curSz = st_tagBas_gs_getSize(&pTagI->tbas);
	if (useOffsets) {
		offsCur = st_streamrd_getCurPos(pTagI->pStrrd);
		offsBeg = st_tagBas_gs_getOffset(&pTagI->tbas);
		if (offsCur > offsBeg)
			newSz = (Tst_uint32)(offsCur - offsBeg);
		else
			newSz = 0;
	}
	if (newSz != curSz) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_vorbc_d_deb(&pTagI->opts, 0, pFnc,
					"correcting tag size %u --> %u",
					curSz, newSz);
		st_tagBas_gs_setSize(&pTagI->tbas, newSz);
		/*if (! pTagI->tgErrs.thWasSzCorrd) {
			pTagI->tgErrs.thWasSzCorrd = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}*/
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read Vorbis Tag field header
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TVORBC__rd_readTagFldHd(const Tst_vorbc_opts *pOpts,
		const Tst_uint32 fnr,
		Tst_streamrd *pStrrdTag,
		Tst_uint32 *pFldSz,
		Tst_str **ppFldKey, Tst_uint32 *pFldKeySz,
		Tst_uint32 *pRemSz)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_uint32 len = 0,
	           x;
	Tst_bool   fnd = ST_B_FALSE;
	Tst_byte   byt = 0;
	Tst_str    uch = 0x00;

	*pFldSz = 0;
	if (*ppFldKey != NULL)
		(*ppFldKey)[0] = 0;

	if (*pRemSz < 5)
		return ST_ERR_SUCC;  /* we're done */

	/* SIZE (4 bytes) */
	st_streamrd_rdUInt32(pStrrdTag,
			ST_STREAMRD_IEND_LE, 32, pFldSz);  /* ENDIAN: LE-->HOST */
	*pRemSz -= (*pRemSz >= 4 ? 4 : *pRemSz);

	/* ITEM KEY (x bytes) */
	/** */
	x = 0;
	do {
		if (*pRemSz == 0) {
			*pFldSz = 0;
			return ST_ERR_SUCC;  /* we're done */
		}
		/* */
		if (x + 2 >= *pFldKeySz) {
			*pFldKeySz += 64;
			ST_REALLOC(*ppFldKey, Tst_str*, *pFldKeySz, 1)
			if (*ppFldKey == NULL)
				return ST_ERR_OMEM;
		}
		/* */
		res = st_streamrd_rdByte_noCRC(pStrrdTag, 8, &byt);
		--(*pRemSz);
		if (res == ST_ERR_FEOF) {
			*pFldSz = 0;
			return ST_ERR_SUCC;  /* we're done */
		}
		(*ppFldKey)[x] = (Tst_str)byt;
		if (byt == (Tst_byte)'=') {
			len = x;
			fnd = ST_B_TRUE;
		}
		++x;
	} while (! fnd && ! st_streamrd_isEOF(pStrrdTag));
	if (! fnd || len == 0) {
		*pFldSz = 0;
		return ST_ERR_SUCC;  /* we're done */
	}
	(*ppFldKey)[len] = 0x00;
	st_sysStrToUpper(*ppFldKey);
	/** */
	if (*pFldSz < len + 1)
		*pFldSz = 0;
	else
		*pFldSz -= (len + 1);
	/** check ITEM KEY */
	if (st_vorbc_fnc_hasIDstrInvalidChars(*ppFldKey, &uch)) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_vorbc_d_deb(pOpts, 0, cFNCN,
					"#%02u: invalid key in field header (inv ch=0x%02x)",
					fnr, uch);
		(*ppFldKey)[0] = 0;
		return ST_ERR_IDAT;  /* invalid data */
	}

	/* check size */
	if (*pFldSz > *pRemSz) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_vorbc_d_deb(pOpts, 0, cFNCN,
					"#%02u: field data size, %u --> %u",
					fnr, *pFldSz, *pRemSz);
		*pFldSz = *pRemSz;
	}

	return ST_ERR_SUCC;
}

static Tst_bool
ST_TVORBC__rd_shallAppend(Tst_vorbc_flds *pFlds,
		Tst_vorbc_fldData **ppFld, Tst_vorbc_fldData_intn **ppFldI)
{
	Tst_uint32 x,
	           fcnt;
	Tst_vorbc_fldData      *pTmpFld;
	Tst_vorbc_fldData_intn *pTmpFldI;

	fcnt = st_dynlist_getElementCount(&pFlds->list);
	if (fcnt < 2)
		return ST_B_FALSE;
	for (x = 1; x <= fcnt - 1; x++) {
		pTmpFld = (Tst_vorbc_fldData*)st_dynlist_getElement(&pFlds->list, x);
		if (pTmpFld == NULL)
			return ST_B_FALSE;
		pTmpFldI = (Tst_vorbc_fldData_intn*)pTmpFld->pObInternal;
		/* */
		if (pTmpFldI->isOK &&
				st_sysStrcmp2(ST_B_FALSE, pTmpFldI->fldPrI.pVIDstr,
						(*ppFldI)->fldPrI.pVIDstr)) {
			*ppFld  = pTmpFld;
			*ppFldI = pTmpFldI;
			return ST_B_TRUE;
		}
	}
	return ST_B_FALSE;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy one not NUL-terminated string
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TVORBC__rd_copyStrU8(const Tst_vorbc_opts *pOpts, const char *pFnc,
		Tst_streamrd *pStrrdTag, const Tst_uint32 len,
		Tst_vorbc_fldData_intn *pFldI)
{
	const Tst_uint32 cWID = 1;
	Tst_err    res;
	Tst_str    *pCh   = NULL;
	Tst_uint32 pchLen = 0,
	           copied = 0;

	/* copy one string from pStrrdTag */
	res = st_tagCFnc_readStr(cWID, ST_MTES_TE_UTF8, pStrrdTag,
			len, &copied, &pCh, &pchLen, NULL);

	/* */
	if (res == ST_ERR_SUCC)
		res = st_mtes_strArrAddCharp_utf8(&pFldI->dataSArr, pCh);
	ST_DELPOINT(pCh)

	/* */
	if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts) &&
			st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
			pFldI->fldPrI.id != ST_VORBC_FID_TAGG)
		st_vorbc_d_fdebStr(pOpts, 1, pFnc, pFldI,
				"txt",
				st_mtes_strArrGetElem(&pFldI->dataSArr,
						st_mtes_strArrGetElemCount(&pFldI->dataSArr)), "");
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Decode picture block
 */
static Tst_err
ST_TVORBC__rd_copyPict(Tst_vorbc_tag_intn *pTagI, const char *pFnc,
		Tst_streamrd *pStrrdTag, const Tst_uint32 len,
		Tst_vorbc_fldData_intn *pFldI)
{
#	define LOC_REMSZ_PIC_  \
				(Tst_uint32)st_streamrd_getAmountRemainingBytes(&strrdPic)
#	define LOC_DET_FREEBO_FREESTRRD_  { \
				st_binobj_detachStreamrd(&binDatOut); \
				st_binobj_stc_freeBO(&binDatOut); \
				st_streamrd_stc_freeSObj(&strrdPic); \
				}
	Tst_err       res;
	Tst_buf       nulTerm[1] = {0};
	Tst_uint32    tmp32      = 0,
	              rdFromStr  = 0;
	Tst_str       *pTmpS     = NULL;
	Tst_str const *pMim;
	Tst_utilsFmt_picFmt picFmtE;
	Tst_binobj          binDatInp,
	                    binDatOut;
	Tst_streamrd        strrdPic;

	/* this is actually the FLAC picture-field format
	 *   that is inofficially supported by Vorbis comment */

	/* all integers are read as Big-Endian */

	st_binobj_stc_initBO(&binDatInp);

	/* read as binary object
	 *   containing a NUL-terminated, Base64-encoded string  */
	res = st_binobj_setData_streamrd(&binDatInp, pStrrdTag,
			len, ST_B_FALSE);
	if (res != ST_ERR_SUCC) {
		st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
				"can't read from stream");
		return res;
	}
	res = st_binobj_appendData(&binDatInp, nulTerm, 1);
	if (res != ST_ERR_SUCC)
		return res;

	st_binobj_stc_initBO(&binDatOut);

	/* decode Base64 */
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdeb(&pTagI->opts, 0, pFnc, pFldI,
				"decoding base64-encoded data");
	res = st_base64_decode_binobj(&binDatInp, &binDatOut);
	if (res != ST_ERR_SUCC) {
		st_binobj_stc_freeBO(&binDatInp);
		st_binobj_stc_freeBO(&binDatOut);
		if (res != ST_ERR_IDAT)
			st_vorbc_d_err(&pTagI->opts, pFnc, pTagI->pFilen,
					"decoding base64 failed");
		return res;
	}
	st_binobj_stc_freeBO(&binDatInp);

	st_streamrd_stc_initSObj(&strrdPic);
	res = st_binobj_attachStreamrd(&binDatOut, &strrdPic, 0);
	if (res != ST_ERR_SUCC) {
		st_binobj_stc_freeBO(&binDatOut);
		st_streamrd_stc_freeSObj(&strrdPic);
		return res;
	}

	/* */
	/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdeb(&pTagI->opts, 1, pFnc, pFldI,
				"pic dec-b64 %u bytes", decSz);**/

	/* decode METADATA_BLOCK_PICTURE */
	/** PICTURE TYPE */
	/*** */
	if (LOC_REMSZ_PIC_ < 4) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &tmp32);  /* ENDIAN: BE-->HOST */
	pFldI->attrPic.ptp = (Tst_vorbc_picTp)tmp32;
	/*** */
	if (pFldI->attrPic.ptp >= ST_VORBC_PT_NONE)
		pFldI->attrPic.ptp = ST_VORBC_PT_OTHER;

	/** MIME LENGTH */
	if (LOC_REMSZ_PIC_ < 4) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &tmp32);  /* ENDIAN: BE-->HOST */

	/** MIME, ASCII-encoded */
	if (LOC_REMSZ_PIC_ < tmp32) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	ST_REALLOC(pFldI->attrPic.pMime, Tst_str*, tmp32 + 1, 1)
	if (pFldI->attrPic.pMime == NULL) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_OMEM;
	}
	if (tmp32 > 0) {
		res = st_streamrd_rdBuffer_noCRC(&strrdPic, tmp32,
				pFldI->attrPic.pMime, &rdFromStr);
		if (res != ST_ERR_SUCC) {
			LOC_DET_FREEBO_FREESTRRD_
			return ST_ERR_IDAT;
		}
	} else
		rdFromStr = 0;
	pFldI->attrPic.pMime[rdFromStr] = 0x00;
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdeb(&pTagI->opts, 1, pFnc, pFldI,
				"mime='%s', ptp=%u",
				pFldI->attrPic.pMime, pFldI->attrPic.ptp);

	/** DESC LENGTH */
	if (LOC_REMSZ_PIC_ < 4) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &tmp32);  /* ENDIAN: BE-->HOST */

	/** DESCRIPTION, UTF8-encoded */
	/*** */
	if (LOC_REMSZ_PIC_ < tmp32) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	ST_CALLOC(pTmpS, Tst_str*, tmp32 + 1, 1)
	if (pTmpS == NULL) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_OMEM;
	}
	res = st_streamrd_rdBuffer_noCRC(&strrdPic, tmp32, pTmpS, &rdFromStr);
	if (res != ST_ERR_SUCC) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	pTmpS[rdFromStr] = 0x00;
	/*** */
	res = st_mtes_copyFromCharp_utf8(pTmpS, &pFldI->attrPic.desc);
	ST_DELPOINT(pTmpS)
	if (res != ST_ERR_SUCC) {
		LOC_DET_FREEBO_FREESTRRD_
		return res;
	}
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdebStr(&pTagI->opts, 1, pFnc, pFldI,
				"scd", &pFldI->attrPic.desc, "");

	/** IMAGE WIDTH, HEIGHT, COLOR DEPTH */
	/*** */
	if (LOC_REMSZ_PIC_ < 4 * 3) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &pFldI->attrPic.imgWid);  /* ENDIAN: BE-->HOST */
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &pFldI->attrPic.imgHei);  /* ENDIAN: BE-->HOST */
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &pFldI->attrPic.imgDep);  /* ENDIAN: BE-->HOST */
	/*** */
	if (pFldI->attrPic.imgWid < 1 || pFldI->attrPic.imgHei < 1) {
		pFldI->attrPic.imgWid = 0;
		pFldI->attrPic.imgHei = 0;
		pFldI->attrPic.imgDep = 0;
	}
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdeb(&pTagI->opts, 1, pFnc, pFldI,
				"pic dim %ux%u",
				pFldI->attrPic.imgWid, pFldI->attrPic.imgHei);

	/** NUMBER OF COLORS USED
	 **   For indexed-color pictures (e.g. GIF),
	 **   the number of colors used, or 0 for non-indexed pictures */
	if (LOC_REMSZ_PIC_ < 4) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &pFldI->attrPic.imgCUs);  /* ENDIAN: BE-->HOST */
	/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdeb(&pTagI->opts, 1, pFnc, pFldI,
				"pic colUsed %u", pFldI->attrPic.imgCUs);**/

	/** BIN LENGTH */
	if (LOC_REMSZ_PIC_ < 4) {
		LOC_DET_FREEBO_FREESTRRD_
		return ST_ERR_IDAT;
	}
	st_streamrd_rdUInt32(&strrdPic,
			ST_STREAMRD_IEND_BE, 32, &tmp32);  /* ENDIAN: BE-->HOST */

	/** BINARY DATA */
	if (tmp32 > 0) {
		if (tmp32 > LOC_REMSZ_PIC_) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_vorbc_d_fdeb(&pTagI->opts, 0, pFnc, pFldI,
						"correcting bin sz %u --> %u bytes",
						tmp32, LOC_REMSZ_PIC_);
			tmp32 = LOC_REMSZ_PIC_;
		}
		res = st_binobj_setData_streamrd(&pFldI->dataPicBDat,
				&strrdPic, tmp32, ST_B_FALSE);
		if (res != ST_ERR_SUCC) {
			LOC_DET_FREEBO_FREESTRRD_
			return res;
		}
	}
	tmp32 = st_binobj_getDataSize(&pFldI->dataPicBDat);
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_vorbc_d_fdeb(&pTagI->opts, 1, pFnc, pFldI,
				"bin %u bytes", tmp32);

	/* check mime-type */
	if (tmp32 > 0) {
		picFmtE = st_utilsFmt_getPicFmt_enum_byBObj(&pFldI->dataPicBDat);
		pMim    = st_utilsFmt_getPicFmt_mime_byEnum(picFmtE);
		if (pMim != NULL &&
				! st_sysStrcmp(ST_B_TRUE, pMim, pFldI->attrPic.pMime)) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_vorbc_d_fdeb(&pTagI->opts, 1, pFnc, pFldI,
						"changing mime to '%s'",
						pMim);
			ST_REALLOC(pFldI->attrPic.pMime, Tst_str*, st_sysStrlen(pMim) + 1, 1)
			if (pFldI->attrPic.pMime == NULL)
				return ST_ERR_OMEM;
			strcpy((char*)pFldI->attrPic.pMime, (const char*)pMim);
		}
	}

	LOC_DET_FREEBO_FREESTRRD_
	return ST_ERR_SUCC;
#	undef LOC_REMSZ_PIC_
#	undef LOC_DET_FREEBO_FREESTRRD_
}

/******************************************************************************/
