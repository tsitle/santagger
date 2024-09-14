/******************************************************************************/
/* cont_ogg_zrd.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading OGG containers / Flac files                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.04.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/audvid/audvid.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_CONT_OGG_ZRD_C
#include "cont_ogg_zrd-pp.h"
/*** */
#include "cont_ogg_zrd_cod-pp.h"
#include "cont_ogg_stcs-prot.h"
#include "cont_ogg_zfdeb-prot.h"
#include "../cont_ogg_aud_vorb/ac_vorb_zrd-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_OGG_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Print out list of used checksums
 */
void
st_contOgg_rd_showUsedChecksums(const Tst_contOgg_opts *pOpts,
		const Tst_bool isSrcOggOrFlac)
{
	const char *cFNCN = __func__;
	char debMsg[512];

	if (isSrcOggOrFlac) {
		debMsg[0] = 0;
		snprintf(debMsg, 512,
				"checksums OGG: fra=%c", (pOpts->checkCRCofr ? 'y' : 'n'));
		st_contOgg_d_debOrAna(pOpts, isSrcOggOrFlac, cFNCN,
				debMsg);
	}
	/* */
	debMsg[0] = 0;
	snprintf(debMsg, 512,
			"checksums FLAC: fraHd=%c, fra=%c, decAudio=%c",
			(pOpts->flacOpts.checkCRCfh ? 'y' : 'n'),
			(pOpts->flacOpts.checkCRCfr ? 'y' : 'n'),
			(pOpts->flacOpts.checkMD5da ? 'y' : 'n'));
	st_contOgg_d_debOrAna(pOpts, isSrcOggOrFlac, cFNCN,
			debMsg);
}

/*----------------------------------------------------------------------------*/

/*
 * Read OGG packet
 */
Tst_err
st_contOgg_rd_rdPacket(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substr_intn **ppBSI,
		Tst_contOgg_fhd **ppFHd)
{
#	define LOC_FNCN_  __func__
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 frDatSz;

	/**if (*ppFHd == NULL)
		st_contOgg_prf("\n%s(): new pack beg\n", LOC_FNCN_);
	else
		st_contOgg_prf("\n%s(): resuming\n", LOC_FNCN_);**/

	do {
		if (*ppFHd == NULL) {
			/* create new frame header element */
			if (pAObjI->frHdArr.used + 5 >
					st_dynlist_getElementCount(&pAObjI->frHdArr.list)) {
				res = st_contOgg_stc_reszFHdArr(&pAObjI->frHdArr,
						pAObjI->frHdArr.used + 10);
				if (res != ST_ERR_SUCC)
					break;
			}
			*ppFHd = st_contOgg_stc_getFHdArrElem_byIX(&pAObjI->frHdArr,
					pAObjI->frHdArr.used + 1);
			ST_ASSERTN_NUM(ST_ERR_FAIL, *ppFHd == NULL)

			/* read frame header */
			res = st_contOgg_rd_rdFrameHd(pAObjI, *ppFHd);
			if (res != ST_ERR_SUCC) {
				if (res != ST_ERR_FEOF)
					*ppBSI = NULL;
				break;
			}

			/* add frame header to appropriate stream --> sets *ppBSI */
			res = ST_CONTOGG__rd_addFHdToStream(pAObjI, *ppFHd, ppBSI);
			if (res != ST_ERR_SUCC)
				break;

			if ((*ppFHd)->frOffsDelta > 0) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
					st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI, *ppFHd,
							LOC_FNCN_,
							"frame header at wrong offset: "
							"+"ST_TFOFFS_PRF_LD" bytes",
							(Tst_foffs)(*ppFHd)->frOffsDelta);
				++(*ppBSI)->errorCnt;
			}

			/* check frame number */
			if ((*ppFHd)->frSequNr != (*ppFHd)->realNr) {
				if ((*ppFHd)->frSequNr < (*ppFHd)->realNr) {
					if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
						st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI,
								*ppFHd, LOC_FNCN_,
								"invalid frame number found: %u < %u",
								(*ppFHd)->frSequNr, (*ppFHd)->realNr);
					++(*ppBSI)->errorCnt;
				} else if ((*ppBSI)->misFrames == 0) {
					if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
						st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI,
								*ppFHd, LOC_FNCN_,
								"missing frame(s) found: fr #%u != #%u",
								(*ppFHd)->frSequNr, (*ppBSI)->lastFrNr + 1);
					if ((*ppFHd)->frSequNr > (*ppBSI)->lastFrNr + 1)
						(*ppBSI)->misFrames += ((*ppFHd)->frSequNr -
								((*ppBSI)->lastFrNr + 1));
					++(*ppBSI)->errorCnt;
				}
			}
			if ((*ppFHd)->frSequNr > (*ppBSI)->lastFrNr)
				(*ppBSI)->lastFrNr = (*ppFHd)->frSequNr;

			/* check frame size */
			if ((*ppFHd)->frSz > ST_CONTOGG_FR_SZ_MAX) {
				if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts)) {
					st_contOgg_d_debBS2(&pAObjI->opts, *ppBSI, *ppFHd,
							LOC_FNCN_,
							"invalid frame size found [1], sz %u > %u",
							(*ppFHd)->frSz, ST_CONTOGG_FR_SZ_MAX);
					st_contOgg_d_debBS2(&pAObjI->opts, *ppBSI, *ppFHd,
							LOC_FNCN_,
							"will stop here");
				}
				++(*ppBSI)->errorCnt;
				res = ST_ERR_OSTP;
				break;
			}
		} else {
			/* we're resuming */
			(*ppBSI)->tmpIs1st = ST_B_TRUE;
			/**st_contOgg_prf("%s(): "
					"stream #%u.%u frame #%u: resume: new pack\n",
					LOC_FNCN_,
					(*ppBSI)->grpIx, (*ppBSI)->grpSIx, (*ppFHd)->realNr);**/
			if ((*ppFHd)->wasUsed) {
				st_contOgg_d_errBS2(&pAObjI->opts,
						*ppBSI, *ppFHd, LOC_FNCN_, pAObjI->pFilen,
						"frame has already been used");
				++(*ppBSI)->errorCnt;
				res = ST_ERR_FAIL;
				break;
			}
		}

		frDatSz = ((*ppFHd)->frSz >= (*ppFHd)->hdSz ?
				(*ppFHd)->frSz - (*ppFHd)->hdSz : 0);

		/* is frame a fresh one ? */
		if (! (*ppFHd)->hdTypeCont) {
			if (! (*ppBSI)->tmpIs1st) {
				/* stop here and resume later */
				/**st_contOgg_prf("%s(): "
						"stream #%u.%u frame #%u: resume later\n",
						LOC_FNCN_,
						(*ppBSI)->grpIx, (*ppBSI)->grpSIx, (*ppFHd)->realNr);**/
				break;
			}
			/* */
			st_contOgg_stc_rsetPack_dataOnly(&(*ppBSI)->packCur);
			(*ppBSI)->packCur.nr = ++(*ppBSI)->packCnt;

			/**st_contOgg_prf("%s(): "
					"stream #%u.%u frame #%u: is fhdBeg (frSz %u)\n",
					LOC_FNCN_, (*ppBSI)->grpIx, (*ppBSI)->grpSIx,
					(*ppFHd)->realNr, (*ppFHd)->frSz);**/
			(*ppBSI)->fhdBegRNr = (*ppFHd)->realNr;

			(*ppBSI)->tmpIs1st = ST_B_FALSE;

			/* 'deactive' old frame headers */
			res = st_contOgg_stc_deactFHdArrPrevElems(&pAObjI->frHdArr,
					(*ppBSI)->serial, (*ppBSI)->fhdBegRNr);
			if (res != ST_ERR_SUCC)
				break;
		}

		/* save max. granule pos */
		if (/*! (*ppFHd)->hdTypeEOS &&*/
				st_sysUInt64_cmpLoHi(&(*ppFHd)->granulePos,
					(Tst_uint32)-1, (Tst_uint32)-1) < 0 &&
				st_sysUInt64_cmpUI64(&(*ppFHd)->granulePos,
							&(*ppBSI)->maxGranulePos) > 0) {
			st_sysUInt64_setUI64(&(*ppBSI)->maxGranulePos, &(*ppFHd)->granulePos);
			/**st_contOgg_prf64("%s(): MXGP:",
					LOC_FNCN_, &(*ppFHd)->granulePos);**/
		}

		/* append frame data to packCur */
		res = st_contOgg_rd_rdFrameDat(pAObjI, *ppBSI, *ppFHd,
				frDatSz, /*appendOrAuto:*/ST_B_FALSE);
		if (res != ST_ERR_SUCC)
			break;
		(*ppFHd)->wasUsed = ST_B_TRUE;

		/* is frame BOS or EOS ? */
		if ((*ppFHd)->hdTypeBOS) {
			if (pAObjI->bsArr.openBScnt < 1)
				++pAObjI->bsArr.grpCnt;
			else
				(*ppBSI)->isGrouped = ST_B_TRUE;
			(*ppBSI)->grpIx  = pAObjI->bsArr.grpCnt;
			(*ppBSI)->grpSIx = ++pAObjI->bsArr.openBScnt;
			if ((*ppBSI)->isGrouped)
				ST_CONTOGG__rd_setLastBStoGrouped(&pAObjI->bsArr, (*ppBSI)->grpIx);
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS2(&pAObjI->opts, *ppBSI, *ppFHd,
						LOC_FNCN_,
						"new sub stream found, serial 0x%08x", (*ppBSI)->serial);
		} else if ((*ppFHd)->hdTypeEOS) {
			if (pAObjI->bsArr.openBScnt > 0)
				--pAObjI->bsArr.openBScnt;
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS2(&pAObjI->opts, *ppBSI, *ppFHd,
						LOC_FNCN_,
						"sub stream ended, serial 0x%08x", (*ppBSI)->serial);
		}

		/* increase used frame headers counter */
		++pAObjI->frHdArr.used;
		*ppFHd = NULL;
	} while (res == ST_ERR_SUCC);

	if (res != ST_ERR_SUCC && res != ST_ERR_FEOF)
		*ppFHd = NULL;
	else if (*ppBSI != NULL) {
		/* set pointer to first frame of current packet
		 *   might be NULL  */
		(*ppBSI)->pFHdBeg = st_contOgg_stc_getFHdArrElem_bySNandFNR(&pAObjI->frHdArr,
				(*ppBSI)->serial, (*ppBSI)->fhdBegRNr);

		(*ppBSI)->fhdBegRNr = 0;  /* not needed anymore */
		/**if ((*ppBSI)->pFHdBeg != NULL) {
			st_contOgg_prf("%s(): "
					"stream #%u.%u: using frame #%u as fhdBeg\n",
					LOC_FNCN_,
					(*ppBSI)->grpIx, (*ppBSI)->grpSIx, (*ppBSI)->pFHdBeg->realNr);
		} else
			st_contOgg_prf("%s(): "
					"stream #%u.%u: no fhdBeg\n", LOC_FNCN_,
					(*ppBSI)->grpIx, (*ppBSI)->grpSIx);**/
	}

	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Read one OGG frame header
 */
Tst_err
st_contOgg_rd_rdFrameHd(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_fhd *pFHd)
{
#	define LOC_FNCN_  __func__
#	define LOC_OGGS_  (Tst_buf*)"OggS"
	Tst_err    res;
	Tst_byte   byt      = 0;
	Tst_buf    zeros[4] = {0, 0, 0, 0};
	Tst_uint32 rdBytes  = 0,
	           stX;
	Tst_utilsFmt_mimeTp mimeTp;

	st_contOgg_stc_rsetFHd(pFHd);

	/**st_contOgg_prf("\n%s(): rd frame  [remBy %u]\n",
			LOC_FNCN_,
			(Tst_uint32)st_streamrd_getAmountRemainingBytes(pAObjI->pStrrd));**/

	/* all numeric values are read as Little-Endian */

	while (ST_B_TRUE) {
		if (st_streamrd_isEOF(pAObjI->pStrrd)) {
			/* we've reached the file end */
			return ST_ERR_FEOF;
		}
		/* check for 'OggS' */
		if (st_streamrd_rdMemcmpBytes(pAObjI->pStrrd, 4, LOC_OGGS_))
			break;
		/* check whether some tag begins here */
		mimeTp = st_utilsFmt_getMime_enum_byStream_tag(pAObjI->pStrrd);
		if (mimeTp != ST_UTILSFMT_MTP_NONE) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_deb(&pAObjI->opts, LOC_FNCN_,
						"found some tag at "ST_TFOFFS_PRF_LD", stopping here",
						(Tst_foffs)st_streamrd_getCurPos(pAObjI->pStrrd));
			return ST_ERR_FEOF;
		}
		/* */
		st_streamrd_rdSkipBytes(pAObjI->pStrrd, 1, ST_B_TRUE);
		++pFHd->frOffsDelta;
	}

	/* start CRC */
	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr)
		st_streamrd_startCRC32(pAObjI->pStrrd);

	/* parse header */
	/** save frame offset */
	pFHd->offs = st_streamrd_getCurPos(pAObjI->pStrrd);
	if (pFHd->offs < 0)
		return ST_ERR_FAIL;
	/** 'OggS' */
	st_streamrd_rdSkipBytes(pAObjI->pStrrd, 4, ST_B_TRUE);
	/* update CRC */
	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr)
		st_streamrd_updateCRC32(pAObjI->pStrrd, LOC_OGGS_, 4);

	/** VERSION */
	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr)
		res = st_streamrd_rdByte(pAObjI->pStrrd, 8, &pFHd->vers);
	else
		res = st_streamrd_rdByte_noCRC(pAObjI->pStrrd, 8, &pFHd->vers);
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("%s(): vers %u\n", LOC_FNCN_, pFHd->vers);**/

	/** HEADER TYPE */
	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr)
		res = st_streamrd_rdByte(pAObjI->pStrrd, 8, &byt);
	else
		res = st_streamrd_rdByte_noCRC(pAObjI->pStrrd, 8, &byt);
	if (res != ST_ERR_SUCC)
		return res;
	pFHd->hdTypeCont = ((byt & 0x01) == 0x01);
	pFHd->hdTypeBOS  = ((byt & 0x02) == 0x02);
	pFHd->hdTypeEOS  = ((byt & 0x04) == 0x04);
	/**st_contOgg_prf("%s(): hdTp c:%d, b:%d, e:%d\n",
			LOC_FNCN_,
			pFHd->hdTypeCont, pFHd->hdTypeBOS, pFHd->hdTypeEOS);**/

	/** GRANULE POS
	 **   if granPos == 0xff..ff -->no packets finish on this frame */
	res = st_streamrd_rdUInt64(pAObjI->pStrrd,
			ST_STREAMRD_IEND_LE, 64, &pFHd->granulePos);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf64(LOC_FNCN_"(): grPos", &pFHd->granulePos);**/

	/** BITSTREAM SERIAL */
	res = st_streamrd_rdUInt32(pAObjI->pStrrd,
			ST_STREAMRD_IEND_LE, 32, &pFHd->bsSerial);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("%s(): bsSer 0x%08x\n", LOC_FNCN_, pFHd->bsSerial);**/

	/** FRAME SEQUENCE NUMBER */
	res = st_streamrd_rdUInt32(pAObjI->pStrrd,
			ST_STREAMRD_IEND_LE, 32, &pFHd->frSequNr);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	++pFHd->frSequNr;
	/**st_contOgg_prf("%s(): frSeqNr %u\n", LOC_FNCN_, pFHd->frSequNr);**/

	/** CRC */
	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr)
		st_streamrd_pauseOrContinueCRC32(pAObjI->pStrrd);
	res = st_streamrd_rdUInt32(pAObjI->pStrrd,
			ST_STREAMRD_IEND_LE, 32, &pFHd->crc32_a);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("%s(): crc 0x%08x\n", LOC_FNCN_, pFHd->crc32_a);**/
	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr) {
		st_streamrd_pauseOrContinueCRC32(pAObjI->pStrrd);
		st_streamrd_updateCRC32(pAObjI->pStrrd, zeros, 4);
	}

	/** FRAME SEGMENTS */
	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr)
		res = st_streamrd_rdByte(pAObjI->pStrrd, 8, &pFHd->segsUsd);
	else
		res = st_streamrd_rdByte_noCRC(pAObjI->pStrrd, 8, &pFHd->segsUsd);
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("%s(): frSegs %u\n", LOC_FNCN_, pFHd->segsUsd);**/

	/** SEGMENT TABLE (--> frame size) */
	pFHd->hdSz = ST_CONTOGG_FR_HD_SZ + pFHd->segsUsd;
	pFHd->frSz = pFHd->hdSz;
	if (pFHd->segsUsd > 0) {
		if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr)
			res = st_streamrd_rdBuffer(pAObjI->pStrrd, pFHd->segsUsd,
					pFHd->segTab, &rdBytes);
		else
			res = st_streamrd_rdBuffer_noCRC(pAObjI->pStrrd, pFHd->segsUsd,
					pFHd->segTab, &rdBytes);
		if (res != ST_ERR_SUCC || rdBytes != pFHd->segsUsd) {
			if (res == ST_ERR_SUCC)
				res = ST_ERR_IDAT;
			return res;
		}
		for (stX = 0; stX < pFHd->segsUsd; stX++) {
			pFHd->frSz += pFHd->segTab[stX];
			/**if (pFHd->frSequNr < 50)
				st_contOgg_prf("%s(): fr#%04u: segTab %3u: 0x%02x (%u)\n",
					LOC_FNCN_, pFHd->frSequNr,
					stX, pFHd->segTab[stX], pFHd->segTab[stX]);**/
		}
		/**if (pFHd->frSequNr < 50)
			st_contOgg_prf("\n");**/
	}
	/**st_contOgg_prf("%s(): frame size %u\n\n", LOC_FNCN_, pFHd->frSz);**/

	return ST_ERR_SUCC;
#	undef LOC_OGGS_
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Read one OGG frame's data
 */
Tst_err
st_contOgg_rd_rdFrameDat(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substr_intn *pBSI,
		Tst_contOgg_fhd *pFHd, Tst_uint32 frDatSz,
		const Tst_bool appendOrAuto)
{
#	define LOC_FNCN_  __func__
	Tst_err res;

	/**st_contOgg_prf("\nst_contOgg_rd_rdFrameDat(): "
			"filepos %u (0x%08x), frdatSz %u\n",
			(Tst_uint32)st_streamrd_getCurPos(pAObjI->pStrrd),
			(Tst_uint32)st_streamrd_getCurPos(pAObjI->pStrrd),
			frDatSz);**/

	if (! pAObjI->opts.modeRdOrWr && ! appendOrAuto) {
		/* clear packCur */
		st_contOgg_stc_rsetPack_dataOnly(&pBSI->packCur);
	}

	/* append lacing values to packCur.pSegs */
	if (pBSI->packCur.segsUsd + pFHd->segsUsd > pBSI->packCur.segsSz) {
		ST_REALLOC(pBSI->packCur.pSegs, Tst_byte*,
				pBSI->packCur.segsUsd + pFHd->segsUsd, 1)
		if (pBSI->packCur.pSegs == NULL)
			return ST_ERR_OMEM;
		pBSI->packCur.segsSz = pBSI->packCur.segsUsd + pFHd->segsUsd;
	}
	memcpy(&pBSI->packCur.pSegs[pBSI->packCur.segsUsd],
			pFHd->segTab, pFHd->segsUsd);
	pBSI->packCur.segsUsd += pFHd->segsUsd;

	/* append frame data to packCur.bdata */
	res = st_binobj_appendData_streamrd(&pBSI->packCur.bdata,
			pAObjI->pStrrd, frDatSz,
			pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr);
	if (res != ST_ERR_SUCC)
		return res;

	if (pAObjI->opts.modeRdOrWr && pAObjI->opts.checkCRCofr) {
		/* check CRC */
		pFHd->crc32_b = st_streamrd_getCRC32(pAObjI->pStrrd);
		st_streamrd_stopCRC32(pAObjI->pStrrd);
		if (memcmp(&pFHd->crc32_a, &pFHd->crc32_b,
					sizeof(pFHd->crc32_a)) != 0) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
				st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, pBSI, pFHd,
						LOC_FNCN_,
						"CRC32 of frame DOESN'T MATCH "
						"(0x%08x != 0x%08x)",
						pFHd->crc32_a, pFHd->crc32_b);
			++pBSI->errorCnt;
			return ST_ERR_IDAT;
		}
	}
	return ST_ERR_SUCC;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Get sub stream that bsSerial belongs to
 */
Tst_contOgg_substream*
st_contOgg_rd_getBS(Tst_contOgg_obj_intn *pAObjI, const Tst_uint32 bsSerial)
{
	Tst_uint32 x;
	Tst_contOgg_substream   *pTmpBS;
	Tst_contOgg_substr_intn *pTmpBSI;

	for (x = 0; x < pAObjI->bsArr.cnt; x++) {
		pTmpBS  = &pAObjI->bsArr.pArr[x];
		pTmpBSI = (Tst_contOgg_substr_intn*)pTmpBS->pObInternal;
		ST_ASSERTN_NULL(pTmpBSI == NULL)

		if (pTmpBSI->serial == bsSerial)
			return pTmpBS;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/

/*
 * Finish PCM client and decoder
 */
void
st_contOgg_rd_finish_pcmClient_andDecoder(Tst_contOgg_obj_intn *pAObjI)
{
	Tst_uint32 x;
	void       *pPCMstrHnd;
	Tst_contOgg_opts        *pOpts = &pAObjI->opts;
	Tst_contOgg_substream   *pBS;
	Tst_contOgg_substr_intn *pBSI;

	for (x = 0; x < pAObjI->bsArr.cnt; x++) {
		pBS  = &pAObjI->bsArr.pArr[x];
		pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
		/* */
		pPCMstrHnd = NULL;
		switch (pBSI->bsType) {
		case ST_CONTOGG_BSTP_VORB:
			if (pBSI->pVorb != NULL) {
				st_contOgg_vorbrd_endDecoder(pOpts, pBSI);
				/* */
				pPCMstrHnd = pBSI->pVorb->decObj.pPCMstrHnd;
			}
			break;
		case ST_CONTOGG_BSTP_FLAC:
			if (pBSI->pFlac != NULL)
				pPCMstrHnd = pBSI->pFlac->decObj.pPCMstrHnd;
			break;
		default:
			break;
		}
		if (pOpts->isPCMclientSet && pPCMstrHnd != NULL)
			pOpts->cbPCMfinishWriting(pPCMstrHnd);
	}
}

/*----------------------------------------------------------------------------*/

/*
 * - Update stream size
 * - Output debug info
 * - Compute playtime
 * - Finish MD5 computation
 * - Update error counter
 * - Set content types
 * - etc.
 */
void
st_contOgg_rd_afterReading(const char *pFnc,
		Tst_contOgg_obj_intn *pAObjI)
{
	Tst_uint32       x,
	                 pckCnt,
	                 fraCnt,
	                 fpsUI;
	double           fpsDbl;
	Tst_uint64 const *pSamples    = NULL;
	Tst_uint32 const *pSampleRate = NULL;
	Tst_foffs        offsEnd;
	Tst_contOgg_substream   *pBS;
	Tst_contOgg_substr_intn *pBSI;

	/* update total stream size */
	offsEnd         = st_streamrd_getCurPos(pAObjI->pStrrd);
	pAObjI->totSize = (Tst_fsize)(offsEnd >= pAObjI->startOffs &&
				! pAObjI->opts.quickScan ? offsEnd - pAObjI->startOffs : 0);

	/* */
	for (x = 0; x < pAObjI->bsArr.cnt; x++) {
		pBS  = &pAObjI->bsArr.pArr[x];
		pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
		/* */
		if (pAObjI->isSrcOggOrFlac &&
				ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
			ST_CONTOGG__rd_printStreamType(&pAObjI->opts, pBSI, pFnc);
		/* */
		switch (pBSI->bsType) {
		case ST_CONTOGG_BSTP_VORB:
			/* playtime */
			if (pBSI->pVorb != NULL) {
				pSamples    = &pBSI->maxGranulePos;
				pSampleRate = &pBSI->pVorb->ident.sampleRate;
			}
			break;
		case ST_CONTOGG_BSTP_FLAC:
			/* finish MD5 */
			st_contOgg_rdcod_finishFlac(&pAObjI->opts, pBSI);
			/* playtime */
			if (pBSI->pFlac != NULL) {
				if (pAObjI->opts.flacOpts.readAll)
					pSamples = &pBSI->pFlac->ident.samplesTot_read;
				else if (st_sysUInt64_cmpLoHi(&pBSI->pFlac->ident.samplesTot_fromHd,
							(Tst_uint32)-1, (Tst_uint32)-1) < 0)
					pSamples = &pBSI->pFlac->ident.samplesTot_fromHd;
				else if (pAObjI->isSrcOggOrFlac)
					pSamples = &pBSI->maxGranulePos;
				pSampleRate = &pBSI->pFlac->ident.sampleR;
			}
			break;
		case ST_CONTOGG_BSTP_SPEE:
		case ST_CONTOGG_BSTP_CELT:
			/*st_contOgg_prf("%u frameSz\n", pBSI->pSpee->ident.frameSize);
			st_contOgg_prf("%u chan\n", pBSI->pSpee->ident.channels);
			st_contOgg_prf("%u sampleRate\n", pBSI->pSpee->ident.sampleRate);
			char hex[24];
			st_sysUInt64_toHexStr(&pBSI->maxGranulePos, hex);
			st_contOgg_prf("samples %s\n", hex);*/
			/* playtime */
			pSamples    = &pBSI->maxGranulePos;
			pSampleRate = (pBSI->bsType == ST_CONTOGG_BSTP_SPEE ?
					(pBSI->pSpee ? &pBSI->pSpee->ident.sampleRate : NULL) :
					(pBSI->pCelt ? &pBSI->pCelt->ident.sampleRate : NULL));
			break;
		case ST_CONTOGG_BSTP_THEO:
			pSamples = &pBSI->pTheo->maxFrIx;
			fpsDbl   = (pBSI->bsType == ST_CONTOGG_BSTP_THEO ?
					(pBSI->pTheo ? pBSI->pTheo->ident.fps : 0.0) :
					(pBSI->pDira ? pBSI->pDira->ident.fps : 0.0));
			if ((Tst_uint32)(fpsDbl + 0.50) > (Tst_uint32)fpsDbl)
				fpsDbl += 0.5;
			fpsUI       = (Tst_uint32)fpsDbl;
			pSampleRate = &fpsUI;
			if (pBSI->pTheo->vPackCnt > 0 &&
					st_sysUInt64_cmpUI32(pSamples, fpsUI) < 0)
				st_sysUInt64_setUI32(&pBSI->pTheo->maxFrIx, fpsUI);
			break;
		default:
			pSamples    = NULL;
			pSampleRate = NULL;
			break;
		}
		/* playtime & bitrate */
		if (pSamples != NULL && pSampleRate != NULL && ! pAObjI->opts.quickScan)
			ST_CONTOGG__rd_computePlaytimeAndBitrate(&pAObjI->opts,
					pSamples, *pSampleRate,
					pBSI->codecNonHeadSize,
					&pBSI->playtime, &pBSI->compBitrateAvgKBS);
		/* */
		pAObjI->errorCnt += pBSI->errorCnt;
		/* */
		if (pAObjI->isSrcOggOrFlac) {
			pAObjI->hasOGGstream = ST_B_TRUE;
			if (pBSI->bsType == ST_CONTOGG_BSTP_VORB ||
					pBSI->bsType == ST_CONTOGG_BSTP_SPEE ||
					pBSI->bsType == ST_CONTOGG_BSTP_FLAC ||
					pBSI->bsType == ST_CONTOGG_BSTP_PCM ||
					pBSI->bsType == ST_CONTOGG_BSTP_MIDI ||
					pBSI->bsType == ST_CONTOGG_BSTP_CELT)
				pAObjI->oggContTypes |= (Tst_int32)ST_CONTOGG_BSCTP_AUD;
			else if (pBSI->bsType == ST_CONTOGG_BSTP_THEO ||
					pBSI->bsType == ST_CONTOGG_BSTP_DIRA ||
					pBSI->bsType == ST_CONTOGG_BSTP_UVS ||
					pBSI->bsType == ST_CONTOGG_BSTP_YUV)
				pAObjI->oggContTypes |= (Tst_int32)ST_CONTOGG_BSCTP_VID;
			else if (pBSI->bsType == ST_CONTOGG_BSTP_CMML ||
					pBSI->bsType == ST_CONTOGG_BSTP_KATE)
				pAObjI->oggContTypes |= (Tst_int32)ST_CONTOGG_BSCTP_TXT;
			else if (pBSI->bsType == ST_CONTOGG_BSTP_PNG ||
					pBSI->bsType == ST_CONTOGG_BSTP_JNG)
				pAObjI->oggContTypes |= (Tst_int32)ST_CONTOGG_BSCTP_PIC;
			else if (pBSI->bsType == ST_CONTOGG_BSTP_MNG)
				pAObjI->oggContTypes |= (Tst_int32)ST_CONTOGG_BSCTP_ANI;
			else if (pBSI->bsType == ST_CONTOGG_BSTP_SKEL)
				pAObjI->oggContTypes |= (Tst_int32)ST_CONTOGG_BSCTP_SKE;
		} else {
			pAObjI->hasFLACaudio = pBSI->pFlac &&
					(pBSI->pFlac->ident.isSet || pBSI->pFlac->aFramesCnt > 0);
		}
		/* */
		if (! pAObjI->opts.quickScan && pAObjI->isSrcOggOrFlac &&
				ST_AVFDEB_ISVERBAUDDET_OR_ANA_D(pAObjI->opts)) {
			st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
					"%u frames containing %u packets",
					pBSI->frameCnt, pBSI->packCnt);
			if (pBSI->misFrames > 0)
				st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
						"%u missing frames", pBSI->misFrames);
		}
		/* */
		if (! pAObjI->opts.quickScan &&
				ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts)) {
			switch (pBSI->bsType) {
			case ST_CONTOGG_BSTP_VORB:
				st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
						"vorbis audio: %u packets",
						(pBSI->pVorb ? pBSI->pVorb->aPackCnt : 0));
				break;
			case ST_CONTOGG_BSTP_FLAC:
				pckCnt = (pBSI->pFlac ? pBSI->pFlac->aPackCnt : 0);
				fraCnt = (pBSI->pFlac ? pBSI->pFlac->aFramesCnt : 0);
				if (pAObjI->isSrcOggOrFlac) {
					if (pAObjI->opts.flacOpts.readAll) {
						st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
								"flac audio: %u packets containing %u frames",
								pckCnt, fraCnt);
					} else {
						st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
								"flac audio: %u packets", pckCnt);
					}
				} else if (pAObjI->opts.flacOpts.readAll) {
					st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
							"flac audio: %u frames", fraCnt);
				}
				break;
			case ST_CONTOGG_BSTP_SPEE:
				st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
						"speex audio: %u packets",
						(pBSI->pSpee ? pBSI->pSpee->aPackCnt : 0));
				break;
			case ST_CONTOGG_BSTP_CELT:
				st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
						"celt audio: %u packets",
						(pBSI->pCelt ? pBSI->pCelt->aPackCnt : 0));
				break;
			case ST_CONTOGG_BSTP_THEO:
				st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
						"theora video: %u packets",
						(pBSI->pTheo ? pBSI->pTheo->vPackCnt : 0));
				break;
			case ST_CONTOGG_BSTP_DIRA:
				st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
						"dirac video: %u packets",
						(pBSI->pDira ? pBSI->pDira->vPackCnt : 0));
				break;
			default:
				break;
			}
		}
		/* */
		if (! pAObjI->opts.quickScan &&
				ST_AVFDEB_ISVERBAUDDET_OR_ANA_D(pAObjI->opts))
			st_contOgg_d_debOrAnaBS1(&pAObjI->opts, pBSI, pFnc,
					"non-header data: %.2f KB (%.2f MB)",
					pBSI->codecNonHeadSize / 1024.0,
					(pBSI->codecNonHeadSize / 1024.0) / 1024.0);
		/* */
		if (! pAObjI->opts.quickScan && pAObjI->isSrcOggOrFlac) {
			if (! pBSI->started) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
					st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
							"sub stream hasn't started but has ogg frames");
				++pBSI->errorCnt;
				++pAObjI->errorCnt;
			}
			if (! pBSI->ended) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
					st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
							"sub stream hasn't ended");
				++pBSI->errorCnt;
				++pAObjI->errorCnt;
			}
			if (pBSI->serial == 0 || pBSI->serial == 0xffffffff) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
					st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
							"sub stream has serial 0x%08x, not very nice",
							pBSI->serial);
				++pBSI->errorCnt;
				++pAObjI->errorCnt;
			}
		}
		/* */
		if (! pAObjI->opts.quickScan &&
				ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts)) {
			Tst_bool isCmtSet;

			switch (pBSI->bsType) {
			case ST_CONTOGG_BSTP_VORB:
				isCmtSet = st_contOgg_vorbComm_gs_getRC_isSet(&pBSI->pVorb->comm);
				if (! (pBSI->pVorb->ident.isSet && isCmtSet &&
							pBSI->pVorb->setup.isSet)) {
					if (! pBSI->pVorb->ident.isSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
							"Vorbis ident header missing");
					if (! isCmtSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Vorbis comment header missing");
					if (! pBSI->pVorb->setup.isSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Vorbis setup header missing");
					++pBSI->errorCnt;
				}
				break;
			case ST_CONTOGG_BSTP_FLAC:
				isCmtSet = st_contOgg_vorbComm_gs_getRC_isSet(&pBSI->pFlac->comm);
				if (! (pBSI->pFlac->ident.isSet && isCmtSet)) {
					if (! pBSI->pFlac->ident.isSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Flac ident header missing");
					if (! isCmtSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Flac comment header missing");
					++pBSI->errorCnt;
				}
				break;
			case ST_CONTOGG_BSTP_SPEE:
				isCmtSet = st_contOgg_vorbComm_gs_getRC_isSet(&pBSI->pSpee->comm);
				if (! (pBSI->pSpee->ident.isSet && isCmtSet)) {
					if (! pBSI->pSpee->ident.isSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Speex ident header missing");
					if (! isCmtSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Speex comment header missing");
					++pBSI->errorCnt;
				}
				break;
			case ST_CONTOGG_BSTP_CELT:
				isCmtSet = st_contOgg_vorbComm_gs_getRC_isSet(&pBSI->pCelt->comm);
				if (! (pBSI->pCelt->ident.isSet && isCmtSet)) {
					if (! pBSI->pCelt->ident.isSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Celt ident header missing");
					if (! isCmtSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Celt comment header missing");
					++pBSI->errorCnt;
				}
				break;
			case ST_CONTOGG_BSTP_THEO:
				isCmtSet = st_contOgg_vorbComm_gs_getRC_isSet(&pBSI->pTheo->comm);
				if (! (pBSI->pTheo->ident.isSet && isCmtSet &&
							pBSI->pTheo->isSetupSet)) {
					if (! pBSI->pTheo->ident.isSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Theora ident header missing");
					if (! isCmtSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Theora comment header missing");
					if (! pBSI->pTheo->isSetupSet)
						st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
								"Theora setup header missing");
					++pBSI->errorCnt;
				}
				break;
			case ST_CONTOGG_BSTP_DIRA:
				if (! pBSI->pDira->ident.isSet) {
					st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
							"Dirac ident header missing");
					++pBSI->errorCnt;
				}
				break;
			default:
				break;
			}
		}
		/* */
		if (pBSI->errorCnt > 0 && ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
			st_contOgg_d_debOrAnaBS1warn(&pAObjI->opts, pBSI, pFnc,
					"sub stream produced %u warnings", pBSI->errorCnt);
		/* */
		if (! pBSI->isGrouped)
			pBSI->grpSIx = 0;
	}
	/* */
	if (! pAObjI->opts.quickScan &&
			ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
		st_contOgg_d_debOrAna(&pAObjI->opts, pAObjI->isSrcOggOrFlac, pFnc,
				"total stream size: "ST_TFSIZE_PRF_LU" bytes (%.2f MB)",
				(Tst_fsize)pAObjI->totSize,
				(double)(pAObjI->totSize / 1024.0) / 1024.0);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
ST_CONTOGG__rd_printStreamType(const Tst_contOgg_opts *pOpts,
		const Tst_contOgg_substr_intn *pBSI, const char *pFnc)
{
	char const *pName = NULL;

	switch (pBSI->bsType) {
	case ST_CONTOGG_BSTP_VORB:  /* Vorbis - Audio */
		pName = "Vorbis"; break;
	case ST_CONTOGG_BSTP_THEO:  /* Theora - Video */
		pName = "Theora"; break;
	case ST_CONTOGG_BSTP_SPEE:  /* Speex - Audio */
		pName = "Speex"; break;
	case ST_CONTOGG_BSTP_FLAC:  /* Flac - Audio */
		pName = "Flac"; break;
	case ST_CONTOGG_BSTP_PCM:   /* PCM - Audio */
		pName = "PCM"; break;
	case ST_CONTOGG_BSTP_MIDI:  /* MIDI - Audio */
		pName = "MIDI"; break;
	case ST_CONTOGG_BSTP_CELT:  /* CELT - Audio */
		pName = "Celt"; break;
	case ST_CONTOGG_BSTP_DIRA:  /* Dirac - Video */
		pName = "Dirac"; break;
	case ST_CONTOGG_BSTP_UVS:   /* UVS - Video */
		pName = "UVS"; break;
	case ST_CONTOGG_BSTP_YUV:   /* YUV4MPEG2 - Video */
		pName = "YUV4MPEG2"; break;
	case ST_CONTOGG_BSTP_CMML:  /* CMML - Text */
		pName = "CMML"; break;
	case ST_CONTOGG_BSTP_KATE:  /* Kate - Text */
		pName = "Kate"; break;
	case ST_CONTOGG_BSTP_PNG:   /* PNG - Picture */
		pName = "PNG"; break;
	case ST_CONTOGG_BSTP_JNG:   /* JNG - Picture */
		pName = "JNG"; break;
	case ST_CONTOGG_BSTP_MNG:   /* MNG - Animation */
		pName = "MNG"; break;
	case ST_CONTOGG_BSTP_SKEL:  /* Skeleton */
		pName = "Skeleton"; break;
	default:
		pName = "unknown";
	}

	st_contOgg_d_debOrAnaBS1(pOpts, pBSI, pFnc,
			"sub stream type: %s", pName);
}

/*----------------------------------------------------------------------------*/

static void
ST_CONTOGG__rd_computePlaytimeAndBitrate(const Tst_contOgg_opts *pOpts,
		const Tst_uint64 *pSamples,
		const Tst_uint32 sampleRate, const Tst_fsize dataSize,
		Tst_av_playtime *pPTime, Tst_uint32 *pBitrateAVG)
{
	Tst_uint64 secsTotal,
	           dsz64,
	           tmpST,
	           tmp8,
	           tmpRes;

	ST_ASSERTN_VOID(pSamples == NULL || pPTime == NULL || pBitrateAVG == NULL)

	if (st_sysUInt64_cmpLoHi(pSamples, 0, 0) == 0 ||
			st_sysUInt64_cmpLoHi(pSamples, (Tst_uint32)-1, (Tst_uint32)-1) == 0 ||
			sampleRate == 0 || sampleRate == (Tst_uint32)-1) {
		ST_AV_STC_RSETPT(*pPTime)
		*pBitrateAVG = 0;
		return;
	}

	/* secsTotal = sampleCnt / sampleR */
	ST_SYSMATH_STC_RSETUI64(secsTotal)
	st_sysUInt64_setUI64(&secsTotal, pSamples);
	st_sysUInt64_divUI32(&secsTotal, sampleRate);

	st_sysUInt64_setLoHi(&dsz64,
			(Tst_uint32)(dataSize & 0xffffffff),
			(Tst_uint32)((dataSize >> 16) >> 16));

	/* get average bitrate
	 *   avgBitr = ((dataSz * 8) / secsTotal) / 1000  */
	ST_SYSMATH_STC_RSETUI64(tmp8)
	ST_SYSMATH_STC_RSETUI64(tmpRes)
	st_sysUInt64_setLoHi(&tmp8, 8, 0);
	/* --> tmpRes ^= dataSz in bits */
	st_sysUInt64_multUI64s(&tmpRes, &dsz64, &tmp8, NULL);
	st_sysUInt64_divUI64(&tmpRes, &secsTotal);
	/* --> tmpRes ^= bitrate in kbits/s */
	st_sysUInt64_divUI32(&tmpRes, 1000);
	*pBitrateAVG = st_sysUInt64_toUI32(&tmpRes, NULL);

	/* tmpST = secsTotal / (60.0 * 60.0) */
	ST_SYSMATH_STC_RSETUI64(tmpST)
	st_sysUInt64_setUI64(&tmpST, &secsTotal);
	st_sysUInt64_divUI32(&tmpST, 60 * 60);
	if (st_sysUInt64_cmpLoHi(&tmpST, 256, 0) < 0) {
		/* pPTime->ptHour = tmpST */
		pPTime->ptHours = (Tst_byte)st_sysUInt64_toUI32(&tmpST, NULL);
		/* secsTotal -= pPTime->ptHour * 60 * 60 */
		st_sysUInt64_subUI32(&secsTotal, pPTime->ptHours * 60 * 60, NULL);
		/* tmpST = secsTotal / 60.0 */
		st_sysUInt64_setUI64(&tmpST, &secsTotal);
		st_sysUInt64_divUI32(&tmpST, 60);
		/* pPTime->ptMin = tmpST */
		pPTime->ptMins = (Tst_byte)st_sysUInt64_toUI32(&tmpST, NULL);
		/* secsTotal -= pPTime->ptMin * 60 */
		st_sysUInt64_subUI32(&secsTotal, pPTime->ptMins * 60, NULL);
		/* pPTime->ptSec = secsTotal */
		pPTime->ptSecs = (Tst_byte)st_sysUInt64_toUI32(&secsTotal, NULL);
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_CONTOGG__rd_addFHdToStream(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_fhd *pFHd, Tst_contOgg_substr_intn **ppBSI)
{
#	define LOC_FNCN_  __func__
	Tst_err    res;
	Tst_uint32 x,
	           bsIX = pAObjI->bsArr.cnt + 1;  /* out of range index */

	*ppBSI = NULL;
	if (pAObjI->bsArr.cnt > 0) {
		Tst_contOgg_substr_intn *pTmpBSI;

		/* find appropriate stream */
		for (x = 0; x < pAObjI->bsArr.cnt; x++) {
			pTmpBSI = (Tst_contOgg_substr_intn*)pAObjI->bsArr.pArr[x].pObInternal;
			if (pTmpBSI->serial == pFHd->bsSerial) {
				bsIX = x;
				break;
			}
		}
	}
	if (bsIX >= pAObjI->bsArr.cnt) {
		/* create new stream */
		/**st_contOgg_prf("ST_CONTOGG__rd_addFHdToStream(): creating new stream\n");**/
		if (pAObjI->bsArr.cnt + 1 > pAObjI->bsArr.sz) {
			res = st_contOgg_stc_reszBsArr(pAObjI->pFilen,
					&pAObjI->bsArr, pAObjI->bsArr.sz + 2);
			if (res != ST_ERR_SUCC)
				return res;
		}
		bsIX = pAObjI->bsArr.cnt;
		++pAObjI->bsArr.cnt;
	} /**else
		st_contOgg_prf("ST_CONTOGG__rd_addFHdToStream(): appending to stream %u\n", bsIX);**/
	/* */
	*ppBSI       = (Tst_contOgg_substr_intn*)pAObjI->bsArr.pArr[bsIX].pObInternal;
	(*ppBSI)->nr = bsIX + 1;
	/* */
	++(*ppBSI)->frameCnt;
	pFHd->realNr = (*ppBSI)->frameCnt;
	/* */
	if (! (pFHd->hdTypeBOS || pFHd->hdTypeEOS)) {
		if ((*ppBSI)->ended) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
				st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI, pFHd,
						LOC_FNCN_,
						"sub stream has already ended but read new frame");
			++(*ppBSI)->errorCnt;
		} else if (! (*ppBSI)->started) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
				st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI, pFHd,
						LOC_FNCN_,
						"sub stream hasn't started yet but read frame");
			++(*ppBSI)->errorCnt;
		}
	}
	if (pFHd->hdTypeBOS) {
		if ((*ppBSI)->started) {
			if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
				st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI, pFHd,
						LOC_FNCN_,
						"sub stream had already started but starts again");
			++(*ppBSI)->errorCnt;
		}
		(*ppBSI)->started = ST_B_TRUE;
	}
	if (pFHd->hdTypeEOS) {
		if (! (*ppBSI)->started || (*ppBSI)->ended) {
			if (! (*ppBSI)->started) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
					st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI, pFHd,
							LOC_FNCN_,
							"sub stream ended but never started");
				++(*ppBSI)->errorCnt;
			}
			if ((*ppBSI)->ended) {
				if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
					st_contOgg_d_debOrAnaBS2warn(&pAObjI->opts, *ppBSI, pFHd,
							LOC_FNCN_,
							"sub stream already ended but ends again");
				++(*ppBSI)->errorCnt;
			}
		}
		(*ppBSI)->ended = ST_B_TRUE;
	}
	(*ppBSI)->serial = pFHd->bsSerial;
	return ST_ERR_SUCC;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

static void
ST_CONTOGG__rd_setLastBStoGrouped(Tst_contOgg_substreamArr *pSA,
		const Tst_uint32 grpIx)
{
	Tst_uint32 x;
	Tst_contOgg_substr_intn *pBSI;

	if (pSA->pArr != NULL && pSA->cnt > 1)
		for (x = pSA->cnt - 1; x > 0; x--) {
			pBSI = (Tst_contOgg_substr_intn*)pSA->pArr[x - 1].pObInternal;
			if (pBSI->grpIx == grpIx) {
				pBSI->isGrouped = ST_B_TRUE;
				break;
			}
		}
}

/******************************************************************************/
