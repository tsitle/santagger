/******************************************************************************/
/* tag_id3v2_zrd.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for reading ID3v2 Tags                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
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
#include "src/includes/common/streamrd.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/tag/tag_id3_helper.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_TAG_ID3V2_ZRD_C
#include "tag_id3v2_zrd-pp.h"
/*** */
#include "tag_id3v2_zrd2-pp.h"
#include "tag_id3v2-prot.h"
#include "tag_id3v2_fnc-prot.h"
#include "tag_id3v2_zchk-pp.h"
#include "tag_id3v2_stcs-prot.h"
#include "tag_id3v2_zsync-prot.h"
#include "tag_id3v2_zfdeb-prot.h"
#include "../tag_basics-prot.h"
#include "../tag_zfdeb-prot.h"
#undef SRC_TAG_ID3V2_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read ID3v2 Tag Header/Footer
 *
 * pTag        MUST be set
 * pIsOK       MUST be != NULL
 * pEhdNfraSz  MUST be != NULL
 * pTotRd      MUST be != NULL
 *
 * returns: ERRCODE
 */
Tst_err
st_id3v2_rd_readHdOrFoot(Tst_id3v2_tag *pTag,
		const Tst_bool isFooterOrHeader,
		Tst_bool *pIsOK, Tst_uint32 *pEhdNfraSz, Tst_uint32 *pTotRd)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_buf    tbuf[ST_ID3V2_TAG_HEADFOOT_SZ];
	Tst_byte   flagByte,
	           tverMaj,
	           tverMin;
	Tst_uint32 totTagSz,
	           rdFromStr = 0;
	Tst_foffs  offs;
	Tst_bool   isFoot,
	           isHead;
	Tst_id3v2_tag_intn *pTagI;

	/* all numeric values are read as Big-Endian */

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pIsOK == NULL || pEhdNfraSz == NULL || pTotRd == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	*pIsOK      = ST_B_FALSE;
	*pEhdNfraSz = 0;
	*pTotRd     = 0;

	offs = st_streamrd_getCurPos(pTagI->pStrrd);
	/* read ID3v2 Header/Footer */
	res = st_streamrd_rdAheadBuffer(pTagI->pStrrd,
			ST_ID3V2_TAG_HEADFOOT_SZ, tbuf, &rdFromStr);
	if (res != ST_ERR_SUCC || rdFromStr != ST_ID3V2_TAG_HEADFOOT_SZ) {
		if (res == ST_ERR_FEOF) {
			res = ST_ERR_SUCC;
		} else if (res != ST_ERR_SUCC)
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"can't read from stream");
		return res;
	}

	/* "ID3" / "3DI" */
	isFoot = (memcmp(tbuf,
				ST_ID3V2_TAG_FOOT, ST_ID3V2_TAG_HEADFOOT_SLEN) == 0);
	isHead = (! isFoot &&
			memcmp(tbuf,
					ST_ID3V2_TAG_HEAD, ST_ID3V2_TAG_HEADFOOT_SLEN) == 0);
	if ((isFoot && ! isFooterOrHeader) || (isHead && isFooterOrHeader)) {
		/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"expected %s but found %s",
					(isFooterOrHeader ? "footer" : "header"),
					(isFooterOrHeader ? "header" : "footer"));**/
		return ST_ERR_SUCC;
	}
	if ((! isFooterOrHeader && ! isHead) || (isFooterOrHeader && ! isFoot))
		return ST_ERR_SUCC;
	st_streamrd_rdSkipBytes(pTagI->pStrrd,
			ST_ID3V2_TAG_HEADFOOT_SZ, ST_B_TRUE);

	*pTotRd = ST_ID3V2_TAG_HEADFOOT_SZ;

	/* parse header */
	/** VERSION */
	tverMaj = (Tst_byte)tbuf[3];
	tverMin = (Tst_byte)tbuf[4];
	if (tverMaj == 0xff || tverMin == 0xff) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"invalid ID3v2 version in %s",
					(isFooterOrHeader ? "footer" : "header"));
		if (! pTagI->tgErrs.thIsVersInv) {
			pTagI->tgErrs.thIsVersInv = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		return ST_ERR_SUCC;
	}
	if (tverMaj < 2 || tverMaj > 4) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"unknown ID3v2 major tag-version %u in %s",
					tverMaj,
					(isFooterOrHeader ? "footer" : "header"));
		if (! pTagI->tgErrs.thIsVersInv) {
			pTagI->tgErrs.thIsVersInv = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		return ST_ERR_SUCC;
	}
	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"found ID3v2.%x.%x %s",
				tverMaj, tverMin,
				(isFooterOrHeader ? "footer" : "header"));

	/** FLAGS */
	flagByte = (Tst_byte)tbuf[5];
	if ((tverMaj == 2 && tverMin == 0 &&
				(flagByte & 0x3f) != 0x00) ||
			(tverMaj == 2 && tverMin > 0 &&
				(flagByte & 0x1f) != 0x00) ||
			(tverMaj == 3 &&
				(flagByte & 0x1f) != 0x00) ||
			(tverMaj == 4 &&
				(flagByte & 0x0f) != 0x00)) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"invalid ID3v2 flags in %s: 0x%02x",
					(isFooterOrHeader ? "footer" : "header"),
					(Tst_byte)flagByte);
		if (! pTagI->tgErrs.thIsFlagByteInv) {
			pTagI->tgErrs.thIsFlagByteInv = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		return ST_ERR_SUCC;
	}
	/**st_id3v2_prf("    readHeader: flags: 0x%02x\n", flagByte);**/
	if (! isFooterOrHeader) {
		switch (tverMaj) {
		case 2:
			/*** Unsynchronization */
			pTagI->flagsTHd.isUnsy  = (flagByte & 0x80) >> 7;
			/*** Compression */
			pTagI->flagsTHd.isCompr = (flagByte & 0x40) >> 6;
			/*** Has Compressed Metaframe (?) */
			if (tverMin > 0 && (flagByte & 0x20) >> 5)
				pTagI->flagsTHd.hasCDM = ST_B_TRUE;
			break;
		default:  /* 3 + 4 */
			/*** Unsynchronization */
			pTagI->flagsTHd.isUnsy   = (flagByte & 0x80) >> 7;
			/*** Extended header */
			pTagI->flagsTHd.hasExtHd = (flagByte & 0x40) >> 6;
			/*** Experimental indicator */
			pTagI->flagsTHd.isExp    = (flagByte & 0x20) >> 5;
			/*** Has Footer */
			if (tverMaj == 4)
				pTagI->flagsTHd.hasFoot = (flagByte & 0x10) >> 4;
		}

		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			if (pTagI->flagsTHd.isUnsy)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag is unsynchronized");
			if (pTagI->flagsTHd.isCompr)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag is compressed");
			if (pTagI->flagsTHd.hasExtHd)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag has extended header");
			if (pTagI->flagsTHd.isExp)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag is experimental");
			if (pTagI->flagsTHd.hasFoot)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag has footer");
			if (pTagI->flagsTHd.hasCDM)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag has Compressed Metaframe");
		}
	}

	/** TAG SIZE (without ID and header) */
	/*** 4 bytes, most-significant bit always 0 and skipped */
	if (((Tst_byte)tbuf[6] & 0x80) == 0x80 ||
			((Tst_byte)tbuf[7] & 0x80) == 0x80 ||
			((Tst_byte)tbuf[8] & 0x80) == 0x80 ||
			((Tst_byte)tbuf[9] & 0x80) == 0x80) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"invalid ID3v2 tag size in %s",
					(isFooterOrHeader ? "footer" : "header"));
		if (! pTagI->tgErrs.thIsSizeInv) {
			pTagI->tgErrs.thIsSizeInv = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		return ST_ERR_SUCC;
	}
	if (! isFooterOrHeader) {
		/*** convert SyncSafe Integer */
		memcpy(pEhdNfraSz, &tbuf[6], 4);  /* ENDIAN: BE-->BE */
#		if (WORDS_BIGENDIAN != 1)
		*pEhdNfraSz = st_sysReverseByteOrder_UI32(*pEhdNfraSz);  /* ENDIAN: BE-->LE */
#		endif
		*pEhdNfraSz = st_id3v2_sync_syncsafe2uint(*pEhdNfraSz);
		/*** */
		totTagSz = *pTotRd + *pEhdNfraSz;
		if (pTagI->flagsTHd.hasFoot)
			totTagSz += ST_ID3V2_TAG_HEADFOOT_SZ;

		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			if (totTagSz < 1024)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag size=%u bytes", totTagSz);
			else if (totTagSz < 1024 * 1024)
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag size=%u bytes (%.2f KB)", totTagSz,
						(float)totTagSz / 1024.0);
			else
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"ID3v2 tag size=%u bytes (%u MB)", totTagSz,
						(Tst_uint32)((float)totTagSz / (1024.0 * 1024.0)));
		}
	}

	/* */
	if (! isFooterOrHeader) {
		pTagI->verMaj = tverMaj;
		pTagI->verMin = tverMin;

		/* */
		st_tagBas_gs_setHasTag(&pTagI->tbas, ST_B_TRUE);
		st_tagBas_gs_setSize(&pTagI->tbas, totTagSz);
		st_tagBas_gs_setOffset(&pTagI->tbas, offs);
	}

	*pIsOK = ST_B_TRUE;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Reads ID3v2 Extended Header and Tag frames
 *
 * pTag         MUST be set
 * pTag->tbas   MUST be set
 *
 * returns: ERRCODE
 *          pTag->fields will be set
 */
Tst_err
st_id3v2_rd_readEHdAndFlds(Tst_id3v2_tag *pTag, const Tst_uint32 allrRead)
{
#	define LOC_DET_FREESRD_FREEBO_  { \
				if (pStrrdTag == &strrdBDT) st_binobj_detachStreamrd(&binDatTag); \
				st_streamrd_stc_freeSObj(&strrdBDT); \
				st_binobj_stc_freeBO(&binDatTag); \
				}
	const char *cFNCN = __FUNCTION__;
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 remsz,
	           delta   = 0;
	Tst_bool   /*revUnsy = ST_B_FALSE,*/
	           fsFnd   = ST_B_FALSE;
	/*Tst_fsize  amRdBy;*/
	Tst_binobj         binDatTag;
	Tst_streamrd       strrdBDT,
	                   *pStrrdTag;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	ST_ASSERTN_NUM(ST_ERR_FAIL, st_tagBas_gs_getSize(&pTagI->tbas) < allrRead)
	remsz  = st_tagBas_gs_getSize(&pTagI->tbas) - allrRead;
	//amRdBy = st_streamrd_getAmountBytesRead(pTagI->pStrrd);

	/* */
	st_binobj_stc_initBO(&binDatTag);
	st_streamrd_stc_initSObj(&strrdBDT);
	/* */
	if (pTagI->flagsTHd.hasFoot)
		remsz -= (remsz >= ST_ID3V2_TAG_HEADFOOT_SZ ?
				ST_ID3V2_TAG_HEADFOOT_SZ : remsz);

	/* Reverse unsynchronization */
	if (pTagI->verMaj <= 3 && pTagI->flagsTHd.isUnsy) {
		/* in ID3v2.4 unsych is done on frame level, not on the entire tag */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"reversing unsync...");
		delta = 0;
		res   = st_id3v2_sync_reverseUnsync_stream2bo(&fsFnd, pTagI->pStrrd,
				remsz, &binDatTag, &delta);
		if (res != ST_ERR_SUCC) {
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"reversing unsync failed");
			st_streamrd_stc_freeSObj(&strrdBDT);
			st_binobj_stc_freeBO(&binDatTag);
			return res;
		}
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"reversed unsync of %u bytes", delta);
		if (fsFnd && ! pTagI->tgErrs.tbHasFalseSyn) {
			pTagI->tgErrs.tbHasFalseSyn = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		//revUnsy = ST_B_TRUE;
		remsz = st_binobj_getDataSize(&binDatTag);
		/* attach stream reader to the binary object
		 *   that the remaining tag now resides in  */
		res = st_binobj_attachStreamrd(&binDatTag, &strrdBDT, 0);
		if (res != ST_ERR_SUCC) {
			st_streamrd_stc_freeSObj(&strrdBDT);
			st_binobj_stc_freeBO(&binDatTag);
			return res;
		}
		pStrrdTag = &strrdBDT;
	} else {
		/* we read the remaining tag from the normal stream */
		pStrrdTag = pTagI->pStrrd;
	}

	/* read extended header */
	if (pTagI->flagsTHd.hasExtHd) {
		res = ST_TIV2__rd_extHd(pTagI, pStrrdTag, &remsz);
		if (res != ST_ERR_SUCC) {
			st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
					"reading extended header failed");
			LOC_DET_FREESRD_FREEBO_
			return res;
		}
		if (remsz == 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"tag ends after extended header");
			LOC_DET_FREESRD_FREEBO_
			return ST_ERR_SUCC;
		}
	}

	/* parse all frames */
	res = ST_TIV2__rd_allFlds(pTag, pStrrdTag, &remsz);
	if (res != ST_ERR_SUCC) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"reading fields failed");
		LOC_DET_FREESRD_FREEBO_
		return res;
	}

	/* skip remaining bytes */
	if (remsz > 0 && ! st_streamrd_isEOF(pStrrdTag))
		st_streamrd_rdSkipBytes(pStrrdTag, remsz, ST_B_TRUE);

	LOC_DET_FREESRD_FREEBO_
	return ST_ERR_SUCC;
#	undef LOC_DET_FREESRD_FREEBO_
}

/*----------------------------------------------------------------------------*/

void
st_id3v2_rd_updateTagSize(Tst_id3v2_tag_intn *pTagI,
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
			st_id3v2_d_deb(&pTagI->opts, 0, pFnc,
					"correcting tag size %u --> %u",
					curSz, newSz);
		st_tagBas_gs_setSize(&pTagI->tbas, newSz);
		if (! pTagI->tgErrs.thWasSzCorrd) {
			pTagI->tgErrs.thWasSzCorrd = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Partially parse ID3v2.(3,4) Extended Header
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TIV2__rd_extHd(Tst_id3v2_tag_intn *pTagI,
		Tst_streamrd *pStrrdTag, Tst_uint32 *pRemSz)
{
	const char *cFNCN = __FUNCTION__;
	Tst_uint32 used = 0;

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"reading extended header...");

	/* SIZE (w/o SIZE itself) */
	/** */
	if (st_streamrd_isEOF(pStrrdTag) || *pRemSz < 4)
		return ST_ERR_FAIL;
	st_streamrd_rdUInt32(pStrrdTag,
			ST_STREAMRD_IEND_BE, 32, &pTagI->ehdSz);  /* ENDIAN: BE-->HOST */
	*pRemSz -= 4;
	/** */
	if (pTagI->verMaj == 4)
		pTagI->ehdSz = st_id3v2_sync_syncsafe2uint(pTagI->ehdSz);
	/**st_id3v2_prf("  extHd: ehdSz (4+)%u\n", pTagI->ehdSz);**/

	/* FLAGS, 2 bytes */
	if (pTagI->verMaj == 3) {
		if (st_streamrd_isEOF(pStrrdTag) || *pRemSz < 2)
			return ST_ERR_FAIL;
		st_streamrd_rdSkipBytes(pStrrdTag, 2, ST_B_TRUE);
		*pRemSz -= 2;
		used    += 2;
	}
	/* NUMBER OF FLAG BYTES, 1 byte */
	if (pTagI->verMaj == 4) {
		if (st_streamrd_isEOF(pStrrdTag) || *pRemSz < 1)
			return ST_ERR_FAIL;
		st_streamrd_rdSkipBytes(pStrrdTag, 1, ST_B_TRUE);
		*pRemSz -= 1;
		used    += 1;
	}
	/* PADDING, 4 bytes */
	if (pTagI->verMaj == 3) {
		if (st_streamrd_isEOF(pStrrdTag) || *pRemSz < 4)
			return ST_ERR_FAIL;
		st_streamrd_rdSkipBytes(pStrrdTag, 4, ST_B_TRUE);
		*pRemSz -= 4;
		used    += 4;
	}
	/* EXTENDED FLAGS, x bytes */
	if (pTagI->verMaj == 4) {
		/*if (st_streamrd_isEOF(pStrrdTag) || *pRemSz < x)
			return ST_ERR_FAIL;
		st_streamrd_rdSkipBytes(pStrrdTag, x, ST_B_TRUE);
		*pRemSz -= x;
		used    += x;*/
	}

	/* skip the rest */
	if (st_streamrd_isEOF(pStrrdTag) || *pRemSz + used < pTagI->ehdSz ||
			used > pTagI->ehdSz)
		return ST_ERR_FAIL;
	st_streamrd_rdSkipBytes(pStrrdTag, pTagI->ehdSz - used, ST_B_TRUE);
	*pRemSz -= (pTagI->ehdSz - used);

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse all frames
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TIV2__rd_allFlds(Tst_id3v2_tag *pTag,
		Tst_streamrd *pStrrdTag, Tst_uint32 *pRemSz)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res       = ST_ERR_SUCC;
	Tst_str    frhId[4 + 1],
	           uch       = 0x00;
	Tst_uint32 frhSz,
	           frhIdSz,
	           frhFl,
	           fcnt,
	           rdFromStr = 0;
	//Tst_fsize  amRdBy;
	Tst_bool   stop      = ST_B_FALSE,
	           skipFld;
	Tst_uint16 tmpWrd;
	Tst_id3v2_fldData      *pFld;
	Tst_id3v2_fldData_intn *pFldI;
	Tst_id3v2_tag_intn     *pTagI;

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	frhIdSz = (pTagI->verMaj == 2 ? 3 : 4);
	//amRdBy = st_streamrd_getAmountBytesRead(pStrrdTag);

	while (*pRemSz > 0) {
		if (*pRemSz < frhIdSz)
			break;
		skipFld = ST_B_FALSE;
		/* read frame ID */
		frhId[0] = 0;
		st_streamrd_rdBuffer_noCRC(pStrrdTag, frhIdSz, frhId, &rdFromStr);
		if (rdFromStr != frhIdSz) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"tag field can't be read [1]");
			break;
		}
		*pRemSz       -= rdFromStr;
		frhId[frhIdSz] = 0;
		/* check frame ID */
		if (frhId[0] == 0 ||
				(frhIdSz == 3 &&
					st_sysStrcmpN2(ST_B_TRUE, 3, "   ", frhId)) ||
				(frhIdSz == 4 &&
					st_sysStrcmpN2(ST_B_TRUE, 4, "    ", frhId))) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"padding byte found in tag field");
			pTagI->paddLen = *pRemSz + rdFromStr;
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"paddLen=%u", pTagI->paddLen);
			st_streamrd_rdSkipBytes(pStrrdTag, *pRemSz, ST_B_TRUE);
			*pRemSz = 0;
			break;
		}
		st_sysStrToUpper((Tst_str*)frhId);
		if (st_id3v2_fnc_hasIDstrInvalidChars((Tst_str*)frhId,
					/*mod:*/ST_B_TRUE, &uch)) {
			if (! pTagI->tgErrs.tbHasInvFID) {
				pTagI->tgErrs.tbHasInvFID = ST_B_TRUE;
				++pTagI->tgErrCnt;
			}
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"invalid frame ID found in tag field: '%s' (inv ch=0x%02x), "
						"skipping",
						frhId, uch);
			/*stop = ST_B_TRUE;*/
			skipFld = ST_B_TRUE;  /* field will be skipped later on */
		}
		if (stop) {
			break;
		}
		/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"tag ID '%s' found", frhId);**/

		if (*pRemSz < frhIdSz)
			break;
		/* read frame size */
		res = st_streamrd_rdUInt32(pStrrdTag,
				ST_STREAMRD_IEND_BE, frhIdSz * 8, &frhSz);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC) {
			if (res == ST_ERR_FEOF) {
				res = ST_ERR_SUCC;
				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
					st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
							"'%s': tag field can't be read [2]", frhId);
			}
			break;
		}
		*pRemSz -= frhIdSz;

		if (pTagI->verMaj >= 4)
			frhSz = st_id3v2_sync_syncsafe2uint(frhSz);
		/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"'%s': frhSz %u", frhId, frhSz);**/
		if (frhSz == 0) {
			if (! pTagI->tgErrs.tbHasInvFrSz) {
				pTagI->tgErrs.tbHasInvFrSz = ST_B_TRUE;
				++pTagI->tgErrCnt;
			}
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"'%s': frhSz==0 found in tag field", frhId);
			/*break;*/
		}
		if (frhSz + (pTagI->verMaj > 2 ? 2 : 0) > *pRemSz) {
			Tst_uint32 tmpDeltFS = (pTagI->verMaj > 2 ? 2 : 0);

			if (! pTagI->tgErrs.tbHasInvFrSz) {
				pTagI->tgErrs.tbHasInvFrSz = ST_B_TRUE;
				++pTagI->tgErrCnt;
			}
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"'%s': too big frame size found in tag field: %u > %u",
						frhId,
						frhSz + tmpDeltFS, *pRemSz);
			if (*pRemSz <= tmpDeltFS)
				break;
			frhSz = *pRemSz - tmpDeltFS;
		}

		/* read frame flags */
		frhFl = 0;
		if (pTagI->verMaj > 2) {
			if (*pRemSz < 2)
				break;
			res = st_streamrd_rdUInt16(pStrrdTag,
					ST_STREAMRD_IEND_BE, 16, &tmpWrd);  /* ENDIAN: BE-->HOST */
			if (res != ST_ERR_SUCC) {
				if (res == ST_ERR_FEOF) {
					res = ST_ERR_SUCC;
					if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
						st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
								"'%s': tag field can't be read [3]", frhId);
				}
				break;
			}
			*pRemSz -= 2;
			frhFl    = tmpWrd;
		}

		/* read tag field */
		if (frhSz > 0) {
			Tst_fsize tmpAmRdBy = st_streamrd_getAmountBytesRead(pStrrdTag);

			if (! skipFld) {
				res = ST_TIV2__rd_fld(pTag, pStrrdTag,
						frhSz, frhId, frhFl, &skipFld);
				if (res != ST_ERR_SUCC) {
					st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
							"reading field failed", frhId);
					break;
				}
			}
			/* skip remainig bytes of field (if there are any) */
			tmpAmRdBy = st_streamrd_getAmountBytesRead(pStrrdTag) - tmpAmRdBy;
			if (tmpAmRdBy > frhSz) {
				res = ST_ERR_FAIL;
				break;
			}
			if (tmpAmRdBy < frhSz)
				st_streamrd_rdSkipBytes(pStrrdTag,
						(Tst_uint32)((Tst_fsize)frhSz - tmpAmRdBy), ST_B_TRUE);
			*pRemSz -= frhSz;
		}

		/* if compressed metaframe was found, do a recursive fnccall */
		if (pTagI->opts.cbZLibDecmpr != NULL &&
				(fcnt = st_id3v2_gs_getTag_amountFlds(pTag)) > 0) {
			pFld = (Tst_id3v2_fldData*)st_dynlist_getElement(&pTagI->flds.list, fcnt);
			ST_ASSERTN_NUM(ST_ERR_FAIL, pFld == NULL || pFld->pObInternal == NULL)
			pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;

			if (pFldI->isOK && pFldI->fldPrI.typ == ST_ID3V2_FTP_CDM) {
				Tst_uint32 recRemSz = st_binobj_getDataSize(&pFldI->dataBinDat);
				Tst_streamrd recStrrdTag;

				pFldI->isOK = ST_B_FALSE;  /* deactivate compressed frame */

				if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
					st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
							"'%s': doing recursion...", frhId);
				st_streamrd_stc_initSObj(&recStrrdTag);
				res = st_binobj_attachStreamrd(&pFldI->dataBinDat, &recStrrdTag, 0);
				if (res == ST_ERR_SUCC) {
					res = ST_TIV2__rd_allFlds(pTag, &recStrrdTag, &recRemSz);
					if (res != ST_ERR_SUCC)
						st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
								"'%s': recursive fnccall failed", frhId);
					st_binobj_detachStreamrd(&pFldI->dataBinDat);
				}
				st_streamrd_stc_freeSObj(&recStrrdTag);
				if (res != ST_ERR_SUCC)
					break;
			}
		}
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Reads a ID3v2 frame
 *
 * pTag         MUST be set
 * pStrrdTag    MUST be set
 * pIDstr       MUST be set
 * pSkipField   MUST be != NULL
 *
 * returns: ERRCODE
 *          pTag->fields will be set
 */
static Tst_err
ST_TIV2__rd_fld(Tst_id3v2_tag *pTag,
		Tst_streamrd *pStrrdTag, const Tst_uint32 frSize,
		const Tst_str *pIDstr, const Tst_uint32 flags,
		Tst_bool *pSkipField)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res     = ST_ERR_SUCC;
	Tst_bool   validID = ST_B_TRUE,
	           skipSil;
	Tst_id3v2_tag_intn *pTagI;

	ST_ASSERTN_IARG(pTag == NULL || pTag->pObInternal == NULL ||
			pStrrdTag == NULL || pIDstr == NULL || pSkipField == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;
	ST_ASSERTN_IARG(pTagI->pFilen == NULL)

	*pSkipField = ST_B_FALSE;

	/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"parse tag field '%s'...", pIDstr);**/

	/* shall frame be ignored ? */
	skipSil = st_id3v2_fnc_isFldPropInArr_byIDstr(ST_ID3V2_FLDPROPS_IGN,
			pIDstr, pTagI->verMaj, NULL);
	if (! skipSil) {
		/* is frame ID valid ? */
		validID = st_id3v2_fnc_isFldPropInArr_byIDstr(ST_ID3V2_FLDPROPS,
				pIDstr, pTagI->verMaj, NULL);
		if (! validID) {
			Tst_uint32 slen = st_sysStrlen(pIDstr);

			if ((pTagI->verMaj == 2 && slen == 3) ||
						(pTagI->verMaj > 2 && slen == 4))
				validID = (pIDstr[0] == 'T' || pIDstr[0] == 't' ||
							pIDstr[0] == 'W' || pIDstr[0] == 'w');
		}
	}
	if (! validID || skipSil) {
		*pSkipField = ST_B_TRUE;
		++pTagI->flds.skipCnt;
		if (! validID) {
			if (! pTagI->tgErrs.tbHasUnknFIDs) {
				pTagI->tgErrs.tbHasUnknFIDs = ST_B_TRUE;
				++pTagI->tgErrCnt;
			}
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
						"unknown tag field: '%s'", pIDstr);
		} else if (skipSil && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"silently ignoring tag field: '%s'", pIDstr);
		return ST_ERR_SUCC;
	}

	/* check total amount of tag fields */
	if (st_id3v2_gs_getTag_amountFlds(pTag) >= ST_ID3V2_MAX_TFLD_CNT) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"'%s': too many frames for one tag, skipping (max %u)",
					pIDstr, ST_ID3V2_MAX_TFLD_CNT);
		*pSkipField = ST_B_TRUE;
		++pTagI->flds.skipCnt;
		/* update error counter */
		if (! pTagI->tgErrs.tbHasTooManyTFs) {
			pTagI->tgErrs.tbHasTooManyTFs = ST_B_TRUE;
			++pTagI->tgErrCnt;
		}
		return ST_ERR_SUCC;
	}

	/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
		st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
				"read tag field: '%s': sz %u, fl %04x",
				pIDstr, frSize, flags);**/

	/* actually read the field */
	res = ST_TIV2__rd_fld_sub(pTag, pStrrdTag, frSize, pIDstr,
			flags, pSkipField);
	if (res != ST_ERR_SUCC) {
		st_id3v2_d_err(&pTagI->opts, cFNCN, pTagI->pFilen,
				"'%s': sub fnc for reading field failed", pIDstr);
	} else if (*pSkipField) {
		/* error counter has already been updated */
		++pTagI->flds.skipCnt;
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_deb(&pTagI->opts, 0, cFNCN,
					"skipping tag field: '%s'", pIDstr);
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Really reads ID3v2 Tag frame
 *
 * pTag         MUST be set
 * pStrrdTag    MUST be set
 * pIDstr       MUST be set
 * pSkipField   MUST be != NULL
 *
 * returns: ERRCODE
 *          pTag->fields will be set
 *          pSkipField   will be set
 */
static Tst_err
ST_TIV2__rd_fld_sub(Tst_id3v2_tag *pTag,
		Tst_streamrd *pStrrdTag, Tst_uint32 frSize,
		const Tst_str *pFrIDstr,
		const Tst_uint32 flags,
		Tst_bool *pSkipField)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res = ST_ERR_SUCC;
	Tst_streamrd           strrdFldBO,
	                       *pStrrdFld = pStrrdTag;
	Tst_binobj             binDatFld;
	Tst_id3v2_frhdFlags    flagsFHd;
	Tst_id3v2_fldData      *pFld  = NULL;
	Tst_id3v2_fldData_intn *pFldI = NULL;
	Tst_id3v2_tag_intn     *pTagI;

	st_id3v2_stc_rsetFHdF(&flagsFHd);

	ST_ASSERTN_IARG(pStrrdTag == NULL ||
			pFrIDstr == NULL || pSkipField == NULL)

	pTagI = (Tst_id3v2_tag_intn*)pTag->pObInternal;

	*pSkipField = ST_B_FALSE;

	/* add new field to tag */
	res = st_id3v2_fnc_addElemToFlds(pTag, ST_ID3V2_FID_NONE, pFrIDstr,
			pSkipField, &pFld);
	if (res == ST_ERR_SUCC)
		pFldI = (Tst_id3v2_fldData_intn*)pFld->pObInternal;
	if (res != ST_ERR_SUCC || *pSkipField) {
		if (res == ST_ERR_SUCC && *pSkipField &&
				pFldI->fnr > ST_ID3V2_MAX_TFLDID_CNT &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			st_id3v2_d_fdeb2(&pTagI->opts, 0, cFNCN, &pFldI->fldPrI,
					"too many frames for this ID, skipping (%u > %u)",
					pFldI->fnr, ST_ID3V2_MAX_TFLDID_CNT);
		} else if (res == ST_ERR_SUCC && *pSkipField &&
				ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			st_id3v2_d_fdeb2(&pTagI->opts, 0, cFNCN, &pFldI->fldPrI,
					"frame is invalid for this tag version");
		}
		return res;
	}

	/* is field valid for this ID3v2 version ? */
	if ((pTagI->verMaj == 2 && (pFldI->fldPrI.verFl & 1) == 0) ||
			(pTagI->verMaj == 3 && (pFldI->fldPrI.verFl & 2) == 0) ||
			(pTagI->verMaj == 4 && (pFldI->fldPrI.verFl & 4) == 0)) {
		/* update error flags/counter */
		if (! pFldI->errs.frIsntValVers) {
			pFldI->errs.frIsntValVers = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
		/* */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"frame invalid for this ID3v2 version");
	}

	st_binobj_stc_initBO(&binDatFld);
	st_streamrd_stc_initSObj(&strrdFldBO);

	/* only ID3v2.(3,4) */
	if (pTagI->verMaj > 2) {
		/* parse frame flags */
		ST_TIV2__rd_fld_flagsParse(pTagI, pFldI, flags, &flagsFHd);
		/* handle flags */
		res = ST_TIV2__rd_fld_flagsHandle(pTagI, pFldI, &flagsFHd,
				&pStrrdFld, &frSize, &strrdFldBO, &binDatFld);
		if (res == ST_ERR_CNTH) {
			/* 'cant handle data', error flags already updated */
			*pSkipField = ST_B_TRUE;
			/* */
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"can't handle data");
			st_binobj_stc_freeBO(&binDatFld);
			st_streamrd_stc_freeSObj(&strrdFldBO);
			return ST_ERR_SUCC;
		} else if (res == ST_ERR_IDAT) {
			/* 'has invalid data' */
			*pSkipField = ST_B_TRUE;
			/* update error counter */
			if (! pFldI->errs.frHasInvFlData) {
				pFldI->errs.frHasInvFlData = ST_B_TRUE;
				++pFldI->frErrCnt;
			}
			/* */
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"invalid data in flags");
			st_binobj_stc_freeBO(&binDatFld);
			st_streamrd_stc_freeSObj(&strrdFldBO);
			return ST_ERR_SUCC;
		} else if (res != ST_ERR_SUCC) {
			st_id3v2_d_ferr3(&pTagI->opts, cFNCN, pTagI->pFilen, pFldI,
					"handling field flags failed");
			st_binobj_stc_freeBO(&binDatFld);
			st_streamrd_stc_freeSObj(&strrdFldBO);
			return res;
		}
	}

	/* actually read values in frame */
	res = st_id3v2_rd2_readFldData(pTag, pFldI, pStrrdFld, frSize);
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_IDAT) {
			/* 'invalid data' */
			*pSkipField = ST_B_TRUE;
			/* update error flags */
			if (! pFldI->errs.frHasInvData) {
				pFldI->errs.frHasInvData = ST_B_TRUE;
				++pFldI->frErrCnt;
			}
			/* */
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"invalid data");
			res = ST_ERR_SUCC;
		} else {
			/* other error */
			st_id3v2_d_ferr3(&pTagI->opts, cFNCN,
					pTagI->pFilen, pFldI,
					"reading field data failed");
		}
	} else {
		/* now do some extra error correction */
		res = st_id3v2_chk_checkFld(&pTagI->opts,
				(pTagI->opts.basOpts.verb != 0), pFld);
		if (res == ST_ERR_SUCC && ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
			if (pFldI->isOK) {
				/**st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"accepted");**/
			} else
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"empty");
		} else if (res != ST_ERR_SUCC)
			st_id3v2_d_ferr3(&pTagI->opts, cFNCN,
					pTagI->pFilen, pFldI,
					"checking field failed");
	}

	st_binobj_stc_freeBO(&binDatFld);
	st_streamrd_stc_freeSObj(&strrdFldBO);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Frame Header flags
 *
 * returns: ERRCODE
 */
static void
ST_TIV2__rd_fld_flagsParse(Tst_id3v2_tag_intn *pTagI,
		Tst_id3v2_fldData_intn *pFldI, const Tst_uint32 flags,
		Tst_id3v2_frhdFlags *pFlagsFHd)
{
	const char *cFNCN = __FUNCTION__;

	if (pTagI->verMaj == 3) {
		/* %abc00000 %ijk00000 */
		if ((flags & 0x1f1f) != 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"invalid frHd flags 0x%04x", flags & 0x1f1f);
			if (! pFldI->errs.frHasInvFlags) {
				pFldI->errs.frHasInvFlags = ST_B_TRUE;
				++pFldI->frErrCnt;
			}
		}
		/** a - Tag alter preservation */
		pFlagsFHd->doV34tap   = (flags & 0x8000) >> 15;
		/** b - File alter preservation */
		pFlagsFHd->doV34fap   = (flags & 0x4000) >> 14;
		/** c - Read only */
		pFlagsFHd->isV34ro    = (flags & 0x2000) >> 13;
		/** i - Compression */
		pFlagsFHd->isV34compr = (flags & 0x0080) >> 7;
		/** j - Encryption */
		pFlagsFHd->isV34enc   = (flags & 0x0040) >> 6;
		/** k - Grouping identity */
		pFlagsFHd->isV34gi    = (flags & 0x0020) >> 5;
	} else {
		/* %0abc0000 %0h00kmnp */
		if ((flags & 0x8fb0) != 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"invalid frHd flags 0x%04x", flags & 0x8fb0);
			if (! pFldI->errs.frHasInvFlags) {
				pFldI->errs.frHasInvFlags = ST_B_TRUE;
				++pFldI->frErrCnt;
			}
		}
		/** a - Tag alter preservation */
		pFlagsFHd->doV34tap   = (flags & 0x4000) >> 14;
		/** b - File alter preservation */
		pFlagsFHd->doV34fap   = (flags & 0x2000) >> 13;
		/** c - Read only */
		pFlagsFHd->isV34ro    = (flags & 0x1000) >> 12;
		/** h - Grouping identity */
		pFlagsFHd->isV34gi    = (flags & 0x0040) >> 6;
		/** k - Compression */
		pFlagsFHd->isV34compr = (flags & 0x0008) >> 3;
		/** m - Encryption */
		pFlagsFHd->isV34enc   = (flags & 0x0004) >> 2;
		/** n - Unsynchronisation */
		pFlagsFHd->isV4unsy   = (flags & 0x0002) >> 1;
		/** p - Data length indicator */
		pFlagsFHd->hasV4dli   = (flags & 0x0001) >> 0;
	}
}

/*
 * Handle frame flags, i.e. decompress frame
 *   If the frame gets decompressed or re-unsync'ed the new data will
 *   be written to pBinDatFld and *ppStrrdFld will then point to
 *   pStrrdFldBO. pStrrdFldBO will be attached to pBinDatFld
 *
 * returns: ERRCODE
 */
static Tst_err
ST_TIV2__rd_fld_flagsHandle(Tst_id3v2_tag_intn *pTagI,
		Tst_id3v2_fldData_intn *pFldI, Tst_id3v2_frhdFlags *pFlagsFHd,
		Tst_streamrd **ppStrrdFld, Tst_uint32 *pFrSize,
		Tst_streamrd *pStrrdFldBO, Tst_binobj *pBinDatFld)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_uint32 delta = 0;
	Tst_bool   fsFnd = ST_B_FALSE;
	Tst_binobj boDecInp;

	/* addional information to the frame header may follow
	 *   the frame header, in order of the appearance of
	 *   the flags   */

	pFldI->attrDLI = 0;

	if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts)) {
		if (pFlagsFHd->doV34tap)
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"has flag TagAlterPres");
		if (pFlagsFHd->doV34fap)
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"has flag FileAlterPres");
		if (pFlagsFHd->isV34ro)
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"has flag ReadOnly");
	}

	/* V4: Grouping identity */
	if (pTagI->verMaj == 4 && pFlagsFHd->isV34gi) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"skipping GRPID");
		if (*pFrSize < 1)
			return ST_ERR_IDAT;  /* we're missing something */
		st_streamrd_rdSkipBytes(*ppStrrdFld, 1, ST_B_TRUE);
		*pFrSize -= (*pFrSize >= 1 ? 1 : 0);
		if (*pFrSize == 0)
			return ST_ERR_IDAT;  /* frame can't end here */
	}
	/* V3/4: compression */
	if (pFlagsFHd->isV34compr) {
		/* */
		if (pTagI->verMaj == 4 && ! pFlagsFHd->hasV4dli) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"missing DLI flag");
			return ST_ERR_IDAT;
		}
		if (pTagI->opts.cbZLibDecmpr == NULL) {
			/* we can't handle compression */
			if (! pFldI->errs.frHasCompr) {
				pFldI->errs.frHasCompr = ST_B_TRUE;
				++pFldI->frErrCnt;
			}
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"no zlib callback set for decompression");
			return ST_ERR_CNTH;
		} else if (pTagI->verMaj == 3) {
			/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"reading uncompr sz");**/
			if (*pFrSize < 4)
				return ST_ERR_IDAT;  /* we're missing something */
			/* read uncomressed size of frame */
			res = st_streamrd_rdUInt32(*ppStrrdFld,
					ST_STREAMRD_IEND_BE, 32, &pFldI->attrDLI);  /* ENDIAN: BE-->HOST */
			if (res != ST_ERR_SUCC)
				return res;
			/* */
			*pFrSize -= (*pFrSize >= 4 ? 4 : *pFrSize);
			if (*pFrSize == 0)
				return ST_ERR_IDAT;  /* frame can't end here */
		}
	}
	/* V3/4: encryption */
	if (pFlagsFHd->isV34enc) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"sorry, can't handle encryption yet");
		if (*pFrSize < 1)
			return ST_ERR_IDAT;  /* we're missing something */
		st_streamrd_rdSkipBytes(*ppStrrdFld, 1, ST_B_TRUE);
		*pFrSize -= (*pFrSize >= 1 ? 1 : 0);
		if (*pFrSize == 0)
			return ST_ERR_IDAT;  /* frame can't end here */
		/* we can't handle encryption yet */
		if (! pFldI->errs.frHasEncr) {
			pFldI->errs.frHasEncr = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
		return ST_ERR_CNTH;
	}
	/* V3: Grouping identity */
	if (pTagI->verMaj == 3 && pFlagsFHd->isV34gi) {
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"skipping GRPID");
		if (*pFrSize < 1)
			return ST_ERR_IDAT;  /* we're missing something */
		st_streamrd_rdSkipBytes(*ppStrrdFld, 1, ST_B_TRUE);
		*pFrSize -= (*pFrSize >= 1 ? 1 : 0);
		if (*pFrSize == 0)
			return ST_ERR_IDAT;  /* frame can't end here */
	}

	if (pTagI->verMaj == 3 && ! pFlagsFHd->isV34compr)
		return ST_ERR_SUCC;

	/* V4: Data length indicator */
	if (pTagI->verMaj == 4 && pFlagsFHd->hasV4dli) {
		/**if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"reading DLI");**/
		if (*pFrSize < 4)
			return ST_ERR_IDAT;  /* we're missing something */
		/* convert SyncSafe Integer */
		/** */
		res = st_streamrd_rdUInt32(*ppStrrdFld,
				ST_STREAMRD_IEND_BE, 32, &pFldI->attrDLI);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			return res;
		pFldI->attrDLI = st_id3v2_sync_syncsafe2uint(pFldI->attrDLI);
		/* */
		*pFrSize -= (*pFrSize >= 4 ? 4 : *pFrSize);
		if (*pFrSize == 0)
			return ST_ERR_IDAT;  /* frame can't end here */
	}

	/* V4: Unsynchronisation */
	if (pTagI->verMaj == 4 && pFlagsFHd->isV4unsy) {
		if (*pFrSize == 0)
			return ST_ERR_IDAT;  /* we're missing something */
		/* reverse unsync */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"reversing unsync...");
		res = st_id3v2_sync_reverseUnsync_stream2bo(&fsFnd,
				*ppStrrdFld, *pFrSize, pBinDatFld, &delta);
		if (res != ST_ERR_SUCC) {
			st_id3v2_d_ferr3(&pTagI->opts, cFNCN,
					pTagI->pFilen, pFldI,
					"reversing unsync failed");
			return res;
		}

		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"reversed unsync of %u bytes", delta);
		if (fsFnd && ! pFldI->errs.frHasFalseSyn) {
			pFldI->errs.frHasFalseSyn = ST_B_TRUE;
			++pFldI->frErrCnt;
		}
		/* */
		res = st_binobj_attachStreamrd(pBinDatFld, pStrrdFldBO, 0);
		if (res != ST_ERR_SUCC)
			return res;
		*pFrSize    = st_binobj_getDataSize(pBinDatFld);
		*ppStrrdFld = pStrrdFldBO;
	}

	/* V3/4: decompress buffer */
	if (pTagI->opts.cbZLibDecmpr != NULL && pFlagsFHd->isV34compr) {
		if (pFldI->attrDLI == 0) {
			if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
				st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
						"DLI==0 !");
			return ST_ERR_IDAT;
		}
		/* copy frame to boDecInp */
		st_binobj_stc_initBO(&boDecInp);
		res = st_binobj_setData_streamrd(&boDecInp, *ppStrrdFld,
				*pFrSize, ST_B_FALSE);
		if (res != ST_ERR_SUCC) {
			st_binobj_stc_freeBO(&boDecInp);
			return res;
		}
		/* decompress from boDecInp to pBinDatFld */
		if (ST_TFDEB_ISVERBTAG_BD(pTagI->opts.basOpts))
			st_id3v2_d_fdeb3(&pTagI->opts, 0, cFNCN, pFldI,
					"decompressing frame...");
		res = pTagI->opts.cbZLibDecmpr(&boDecInp,
				pFldI->attrDLI, pBinDatFld);
		st_binobj_stc_freeBO(&boDecInp);
		if (res != ST_ERR_SUCC) {
			/* couldn't decompress */
			pFldI->errs.frFraWasntDcmpr = ST_B_TRUE;
			return ST_ERR_IDAT;
		}
		/* */
		res = st_binobj_attachStreamrd(pBinDatFld, pStrrdFldBO, 0);
		if (res != ST_ERR_SUCC)
			return res;
		*pFrSize    = st_binobj_getDataSize(pBinDatFld);
		*ppStrrdFld = pStrrdFldBO;
	}

	return ST_ERR_SUCC;
}

/******************************************************************************/
