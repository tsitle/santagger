/******************************************************************************/
/* tag_apev2_zrd.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading APEv2 Tags                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.01.2010 (dd.mm.yyyy)                                     */
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
#include "src/includes/common/streamrd.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_apev2.h"
/** */
#define SRC_TAG_APEV2_ZRD_C
#include "tag_apev2_zrd-pp.h"
/*** */
#include "tag_apev2_fnc-prot.h"
#include "tag_apev2_stcs-prot.h"
#include "tag_apev2_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_comfnc-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_APEV2_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read APE Header or Footer
 *
 * returns: ERRCODE
 */
Tst_err
st_apev2_rd_readHdOrFoot(Tst_apev2_tag_intn *pTagI,
		const Tst_bool isFooterOrHeader,
		Tst_bool *pIsOK, Tst_uint32 *pTagSz, Tst_uint32 *pNumFlds)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_buf    buf[ST_APEV2_TAG_HEADFOOT_SZ];
	Tst_uint32 flags     = 0,
	           rdFromStr = 0;
	Tst_foffs  offs;

	ST_ASSERTN_IARG(pTagI == NULL || pIsOK == NULL ||
				pTagSz == NULL || pNumFlds == NULL)

	*pIsOK    = ST_B_FALSE;
	*pTagSz   = 0;
	*pNumFlds = 0;

	offs = st_streamrd_getCurPos(pTagI->pStrrd);
	/* */
	res = st_streamrd_rdAheadBuffer(pTagI->pStrrd, ST_APEV2_TAG_HEADFOOT_SZ,
			buf, &rdFromStr);
	if (res != ST_ERR_SUCC || rdFromStr != ST_APEV2_TAG_HEADFOOT_SZ) {
		if (res == ST_ERR_FEOF)
			res = ST_ERR_SUCC;
		else if (res != ST_ERR_SUCC)
			st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}

	/* "APETAGEX" */
	if (memcmp(buf, ST_APEV2_TAG_HEADFOOT, ST_APEV2_TAG_HEADFOOT_SLEN) != 0)
		return ST_ERR_SUCC;
	st_streamrd_rdSkipBytes(pTagI->pStrrd,
			ST_APEV2_TAG_HEADFOOT_SZ, ST_B_TRUE);

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
				"APEv2 %s found",
				(isFooterOrHeader ? "footer" : "header"));

	/* parse header/footer */
	res = ST_TAV2__rd_readHdOrFoot_parse(&pTagI->opts, pTagI->pFilen,
			buf, isFooterOrHeader, pTagSz, &flags, pNumFlds);
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_IDAT)
			res = ST_ERR_SUCC;
		return res;
	}

	if (! isFooterOrHeader) {
		st_tagBas_gs_setHasTag(&pTagI->tbas, ST_B_TRUE);
		st_tagBas_gs_setOffset(&pTagI->tbas, offs);
		st_tagBas_gs_setSize(&pTagI->tbas, *pTagSz + ST_APEV2_TAG_HEADFOOT_SZ);
	}

	*pIsOK = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read APE Tag fields
 *
 * returns: ERRCODE
 */
Tst_err
st_apev2_rd_readTagFlds(Tst_apev2_tag *pTag,
		Tst_uint32 numFlds, Tst_uint32 *pRemSz)
{
	const char *cFNCN = __func__;
	Tst_err    res          = ST_ERR_SUCC;
	Tst_uint32 rdf          = 1,
	           fldSz        = 0,
	           fbufSz       = 0;
	Tst_str    *pFKey       = NULL;
	Tst_bool   binOrUTF8    = ST_B_FALSE,  /* UTF8 */
	           skippedField = ST_B_FALSE;
	Tst_buf    *pFBuf       = NULL;
	Tst_fsize  amRdBy;
	Tst_apev2_fldData      *pFld = NULL;
	Tst_apev2_fldData_intn *pFldI;
	Tst_apev2_tag_intn     *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pRemSz == NULL)

	pTagI = (Tst_apev2_tag_intn*)pTag->pObInternal;

	if (numFlds > ST_APEV2_MAX_TFLD_CNT) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"too many frames for one tag, reducing from %u to %u",
					numFlds, ST_APEV2_MAX_TFLD_CNT);
		numFlds = ST_APEV2_MAX_TFLD_CNT;
	}

	/* read all fields */
	while (rdf <= numFlds && *pRemSz > 0 &&
			! st_streamrd_isEOF(pTagI->pStrrd)) {
		/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"read fld #%02u", rdf);**/
		/* read field header */
		res = ST_TAV2__rd_readTagFldHd(&pTagI->opts, rdf, pTagI->pStrrd,
				&fldSz, &binOrUTF8, &pFKey, pRemSz);
		if (res != ST_ERR_SUCC && res != ST_ERR_IDAT) {
			if (res == ST_ERR_FEOF)
				res = ST_ERR_IDAT;
			else
				break;
		}
		if (fldSz > *pRemSz) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
						"#%02u '%s': correcting field size %u --> %u",
						rdf, pFKey, fldSz, *pRemSz);
			fldSz = *pRemSz;
		}
		if (res == ST_ERR_IDAT) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
						"#%02u '%s': skipping field --> invalid data",
						rdf, pFKey);
			/* */
			st_streamrd_rdSkipBytes(pTagI->pStrrd, fldSz, ST_B_TRUE);
			++pTagI->flds.skipCnt;
			++rdf;
			res = ST_ERR_SUCC;
			continue;
		}
		if (fldSz == 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
						"#%02u '%s': skipping field --> empty", rdf, pFKey);
			++pTagI->flds.skipCnt;
			++rdf;
			continue;
		}

		/* add new field to list */
		res = st_apev2_fnc_addElemToFlds(pTag,
				ST_APEV2_FID_NONE, pFKey,
				/*isBinOrU8:*/binOrUTF8,
				&skippedField, &pFld);
		if (res == ST_ERR_SUCC)
			pFldI = (Tst_apev2_fldData_intn*)pFld->pObInternal;
		if (res != ST_ERR_SUCC || skippedField) {
			if (res == ST_ERR_SUCC && skippedField &&
					ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
				st_apev2_d_fdeb(&pTagI->opts, 3, cFNCN, pFldI,
						"too many frames for this ID, skipping (%u > %u)",
						pFldI->fnr, ST_APEV2_MAX_TFLDID_CNT);
			}
			break;
		}
		/* */
		/** */
		/*if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
					"#%02u '%s': read data (sz %u)",
					pFldI->fnr, pFldI->fldPrI.pIDstr, fldSz);*/
		amRdBy = st_streamrd_getAmountBytesRead(pTagI->pStrrd);
		/** */
		res = ST_TAV2__rd_readFldData(pTagI, pFldI,
				fldSz, &pFBuf, &fbufSz);
		/** */
		amRdBy = st_streamrd_getAmountBytesRead(pTagI->pStrrd) - amRdBy;
		if (amRdBy > (Tst_fsize)fldSz) {
			res = ST_ERR_FAIL;
			break;
		}
		if (amRdBy < (Tst_fsize)fldSz)
			st_streamrd_rdSkipBytes(pTagI->pStrrd,
					(Tst_uint32)((Tst_fsize)fldSz - amRdBy), ST_B_TRUE);
		*pRemSz -= fldSz;
		/** */
		if (res != ST_ERR_SUCC) {
			if (res != ST_ERR_IDAT)
				st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
						"reading field data failed");
			break;
		}

		pFldI->isOK = ST_B_TRUE;
		st_tagBas_gs_setHasFields(&pTagI->tbas, ST_B_TRUE);
		/* */
		++rdf;
	}

	ST_DELPOINT(pFBuf)
	ST_DELPOINT(pFKey)
	return res;
}

/*----------------------------------------------------------------------------*/

void
st_apev2_rd_updateTagSize(Tst_apev2_tag_intn *pTagI,
		const char *pFnc,
		const Tst_bool useOffsets,
		Tst_uint32 newSz)
{
	Tst_foffs  offsCur,
	           offsBeg;
	Tst_uint32 curSz;

	ST_ASSERTN_VOID(pTagI == NULL || pFnc == NULL)

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
			st_apev2_d_deb(&pTagI->opts, 0, pFnc,
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
 * Parse APE Header or Footer
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TAV2__rd_readHdOrFoot_parse(const Tst_apev2_opts *pOpts,
		const Tst_str *pFn,
		const Tst_buf *pBuf,
		const Tst_bool isFooterOrHeader,
		Tst_uint32 *pSize,
		Tst_uint32 *pFlags,
		Tst_uint32 *pNumFlds)
{
	const char *cFNCN = __func__;
	Tst_uint32    vers = 0;
	Tst_buf const *pBB = pBuf;

	*pSize    = 0;
	*pFlags   = 0;
	*pNumFlds = 0;

	/* ID (8 bytes) */
	pBB += ST_APEV2_TAG_HEADFOOT_SLEN;

	/* VERSION (4 bytes), currently 2.0 (2000) is the max allowed */
	memcpy(&vers, pBB, 4);  /* ENDIAN: LE-->LE */
#	if (WORDS_BIGENDIAN == 1)
	vers = st_sysReverseByteOrder_UI32(vers);  /* ENDIAN: LE-->BE */
#	endif
	if (vers > 2000 && ! isFooterOrHeader) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_apev2_d_deb(pOpts, 0, cFNCN,
					"can't handle tag version %u", vers);
		return ST_ERR_IDAT;
	}
	pBB += 4;

	/* SIZE excluding header, including fields and footer (4 bytes) */
	memcpy(pSize, pBB, 4);  /* ENDIAN: LE-->LE */
#	if (WORDS_BIGENDIAN == 1)
	*pSize = st_sysReverseByteOrder_UI32(*pSize);  /* ENDIAN: LE-->BE */
#	endif
	if (*pSize > ST_APEV2_MAX_TAGSZ) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_apev2_d_deb(pOpts, 0, cFNCN,
					"invalid size=%u in %s", *pSize,
					(isFooterOrHeader ? "footer" : "header"));
		return ST_ERR_IDAT;
	}
	/**st_apev2_prf("%s(): size=%u in %s\n", cFNCN, *pSize,
				(isFooterOrHeader ? "Footer" : "Header"));**/
	pBB += 4;

	/* FIELDS (4 bytes) */
	memcpy(pNumFlds, pBB, 4);  /* ENDIAN: LE-->LE */
#	if (WORDS_BIGENDIAN == 1)
	*pNumFlds = st_sysReverseByteOrder_UI32(*pNumFlds);  /* ENDIAN: LE-->BE */
#	endif
	if (*pNumFlds > 65536) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_apev2_d_deb(pOpts, 0, cFNCN,
					"invalid field amount=%u in %s", *pNumFlds,
					(isFooterOrHeader ? "footer" : "header"));
		return ST_ERR_IDAT;
	}
	/**if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_apev2_d_deb(pOpts, 0, cFNCN,
				"field amount=%u in %s", fields,
				(isFooterOrHeader ? "footer" : "header"));**/
	pBB += 4;

	/* FLAGS (4 bytes) */
	memcpy(pFlags, pBB, 4);  /* ENDIAN: LE-->LE */
#	if (WORDS_BIGENDIAN == 1)
	*pFlags = st_sysReverseByteOrder_UI32(*pFlags);  /* ENDIAN: LE-->BE */
#	endif
	if (isFooterOrHeader) {
		if ((*pFlags & ST_APEV2_TAG_FLAG_ISHEADER) != 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
				st_apev2_d_deb(pOpts, 0, cFNCN,
						"invalid flags 0x%08x in footer", *pFlags);
			return ST_ERR_IDAT;
		}
	} else {
		if ((*pFlags & ST_APEV2_TAG_FLAG_ISHEADER) == 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
				st_apev2_d_deb(pOpts, 0, cFNCN,
						"invalid flags 0x%08x in header", *pFlags);
			return ST_ERR_IDAT;
		}
	}
	/**if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
		st_apev2_d_deb(pOpts, 0, cFNCN,
				"flags=%08x in %s", *pFlags,
				(isFooterOrHeader ? "footer" : "header"));**/
	pBB += 4;

	/* RESERVED (8 bytes) */
	pBB += 8;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read APE Tag field header
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TAV2__rd_readTagFldHd(const Tst_apev2_opts *pOpts,
		const Tst_uint32 fnr,
		Tst_streamrd *pStrrd,
		Tst_uint32 *pFldSz, Tst_bool *pBinOrUTF8, Tst_str **ppFldKey,
		Tst_uint32 *pRemSz)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 len,
	           flags = 0;
	Tst_str    uch   = 0x00;

	*pFldSz     = 0;
	*pBinOrUTF8 = ST_B_FALSE;
	ST_DELPOINT(*ppFldKey)

	if (*pRemSz < 4)
		return ST_ERR_IDAT;  /* we're missing something */
	/* SIZE (4 bytes) */
	res = st_streamrd_rdUInt32(pStrrd,
			ST_STREAMRD_IEND_LE, 32, pFldSz);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	*pRemSz -= 4;
	/**st_apev2_prf("  fhd #%02u sz %u  0x%08x\n", fnr, *pFldSz, *pFldSz);**/

	if (*pRemSz < 4)
		return ST_ERR_IDAT;  /* we're missing something */
	/* FLAGS (4 bytes) */
	res = st_streamrd_rdUInt32(pStrrd,
			ST_STREAMRD_IEND_LE, 32, &flags);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	*pRemSz -= 4;
	if ((flags & 0x06) == 0x02)
		*pBinOrUTF8 = ST_B_TRUE;
	else if ((flags & 0x06) == 0x00 || (flags & 0x06) == 0x04)
		*pBinOrUTF8 = ST_B_FALSE;
	else {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_apev2_d_deb(pOpts, 0, cFNCN,
					"#%02u: invalid flag in field header: 0x%08x",
					fnr, flags);
		return ST_ERR_IDAT;  /* invalid data */
	}
	/**st_apev2_prf("  fhd #%02u fl 0x%08x: %s\n",
			fnr, flags, (*pBinOrUTF8 ? "bin" : "utf8"));**/

	if (*pRemSz == 0)
		return ST_ERR_IDAT;  /* we're missing something */
	/* ITEM KEY - NUL-terminated (x bytes) */
	/** */
	res = st_streamrd_rdString(pStrrd, *pRemSz, ppFldKey);
	if (res != ST_ERR_SUCC)
		return res;
	st_sysStrToUpper(*ppFldKey);
	/** */
	len      = st_sysStrlen(*ppFldKey);
	*pRemSz -= (*pRemSz >= len ? len : *pRemSz);  /* the string itself */
	*pRemSz -= (*pRemSz >= 1 ? 1 : 0);  /* the NUL-term */
	/** check ITEM KEY */
	if (st_apev2_fnc_hasIDstrInvalidChars(*ppFldKey, &uch)) {
		if (ST_TFDEB_ISVERBTAG_BD(pOpts->basOpts))
			st_apev2_d_deb(pOpts, 0, cFNCN,
					"#%02u: invalid key in field header (inv ch=0x%02x)",
					fnr, uch);
		(*ppFldKey)[0] = 0;
		return ST_ERR_IDAT;  /* invalid data */
	}
	/**st_apev2_prf("  fhd #%02u str '%s'\n", fnr, *ppFldKey);
	st_apev2_prf("  fhd #%02u < pos %u\n", fnr, *pPos);**/

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read field data
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TAV2__rd_readFldData(Tst_apev2_tag_intn *pTagI,
		Tst_apev2_fldData_intn *pFldI, const Tst_uint32 fldSz,
		Tst_buf **ppFBuf, Tst_uint32 *pFBufSz)
{
	if (pFldI->fldPrI.typ != ST_APEV2_FTP_BDAT)
		return ST_TAV2__rd_copyMultStrU8(pTagI, pFldI, fldSz);

	return ST_TAV2__rd_copyBinData(pTagI, pFldI, fldSz);
}

/*----------------------------------------------------------------------------*/

/*
 * Copy binary data from stream
 */
static Tst_err
ST_TAV2__rd_copyBinData(Tst_apev2_tag_intn *pTagI,
		Tst_apev2_fldData_intn *pFldI, const Tst_uint32 fldSz)
{
	const char *cFNCN = __func__;
	Tst_err res;

	res = st_binobj_setData_streamrd(&pFldI->dataBinDat, pTagI->pStrrd,
			fldSz, ST_B_FALSE);
	if (res != ST_ERR_SUCC)
		st_apev2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"reading binary data from stream failed");
	else if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_apev2_d_fdeb(&pTagI->opts, 1, cFNCN, pFldI,
				"bin %u bytes",
				st_binobj_getDataSize(&pFldI->dataBinDat));
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Copy multiple NUL-Terminated strings
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TAV2__rd_copyMultStrU8(Tst_apev2_tag_intn *pTagI,
		Tst_apev2_fldData_intn *pFldI, Tst_uint32 fldSz)
{
	const char       *cFNCN = __func__;
	const Tst_uint32 cWID   = 1;
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 copied = 0,
	           pchLen = 0,
	           x;
	Tst_str    *pCh   = NULL;

	st_mtes_stc_rsetSA(&pFldI->dataSArr);

	while (fldSz > 0 && ! st_streamrd_isEOF(pTagI->pStrrd)) {
		/* copy one string from pStrrd */
		/** */
		res = st_tagCFnc_readStr(cWID, ST_MTES_TE_UTF8, pTagI->pStrrd,
				fldSz, &copied, &pCh, &pchLen, NULL);
		/** */
		fldSz -= (fldSz >= copied ? copied : fldSz);
		/** */
		if (res != ST_ERR_SUCC || copied < 1)
			break;

		/* add string to array */
		if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) >= ST_APEV2_MAX_MSTR_CNT) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_apev2_d_deb(&pTagI->opts, 0, cFNCN,
						"too many strings in multi-string (max %u)",
						ST_APEV2_MAX_MSTR_CNT);
		} else {
			res = st_mtes_strArrAddCharp_utf8(&pFldI->dataSArr, pCh);
			if (res != ST_ERR_SUCC)
				break;
		}
	}
	ST_DELPOINT(pCh)

	if (res == ST_ERR_SUCC &&
			ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts) &&
			st_mtes_strArrGetElemCount(&pFldI->dataSArr) > 0 &&
			pFldI->fldPrI.id != ST_APEV2_FID_TAGG) {
		if (st_mtes_strArrGetElemCount(&pFldI->dataSArr) == 1)
			st_apev2_d_fdebStr(&pTagI->opts, 1, cFNCN, pFldI,
					"txt", st_mtes_strArrGetElem(&pFldI->dataSArr, 1), "");
		else
			for (x = 1; x <= st_mtes_strArrGetElemCount(&pFldI->dataSArr); x++)
				st_apev2_d_fdebStr(&pTagI->opts, 1, cFNCN, pFldI,
						"mul", st_mtes_strArrGetElem(&pFldI->dataSArr, x), "");
	}
	return res;
}

/******************************************************************************/
