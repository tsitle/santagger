/******************************************************************************/
/* cont_ogg.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for OGG containers / Flac files                                  */
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
#include "src/includes/common/streamrd.h"
#include "src/includes/common/streamwr.h"
#include "src/includes/audvid/cont_ogg.h"
/** */
#define SRC_CONT_OGG_C
#include "cont_ogg-prot.h"
/*** */
#include "cont_ogg_zrd-pp.h"
#include "cont_ogg_zrd_cod-pp.h"
#include "cont_ogg_zwr-pp.h"
#include "cont_ogg_zwr_cod-pp.h"
#include "cont_ogg_stcs-prot.h"
#include "cont_ogg_zfdeb-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_OGG_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_CONTOGG__addFlacBS(Tst_contOgg_obj_intn *pAObjI,
                      Tst_contOgg_fhd *pDummyFHd,
                      Tst_contOgg_substream **ppNewBS,
                      Tst_contOgg_substr_intn **ppNewBSI);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read OGG streams
 *
 * pAObj   MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_contOgg_readStreams(Tst_contOgg_obj *pAObj)
{
	const char *cFNCN = __func__;
	Tst_err  res      = ST_ERR_SUCC;
	Tst_bool stopThen = ST_B_FALSE;
	Tst_contOgg_fhd         *pFHd = NULL;
	Tst_contOgg_substr_intn *pBSI = NULL;
	Tst_contOgg_obj_intn    *pAObjI;

	ST_ASSERTN_IARG(pAObj == NULL || pAObj->pObInternal == NULL)
	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;
	ST_ASSERTN_IARG(pAObjI->pFilen == NULL || pAObjI->pStrrd == NULL)

	st_contOgg_stc_rsetAObj_prot(pAObj, ST_B_FALSE,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL);
	pAObjI->alrRead         = ST_B_TRUE;
	pAObjI->opts.modeRdOrWr = ST_B_TRUE;

	if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
		st_contOgg_d_deb(&pAObjI->opts, cFNCN, "starting...");
	if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
		st_contOgg_rd_showUsedChecksums(&pAObjI->opts, pAObjI->isSrcOggOrFlac);

	/* */
	pAObjI->startOffs = st_streamrd_getCurPos(pAObjI->pStrrd);

	while (res == ST_ERR_SUCC && ! stopThen &&
			! st_streamrd_isEOF(pAObjI->pStrrd)) {
		/* read one packet (1 or more frames) to pBSI->packCur from any stream
		 *   --> pBSI will point to current stream
		 *       pFHd will point to pending frame for next call
		 *         of st_contOgg_rd_rdPacket() */
		/**st_contOgg_prf("\nst_contOgg_readStreams(): read new pack  "
				"(EOF=%c, rb="ST_TFSIZE_PRF_LU")\n",
				(st_streamrd_isEOF(pAObjI->pStrrd) ? 'y' : 'n'),
				(Tst_fsize)st_streamrd_getAmountRemainingBytes(pAObjI->pStrrd));**/
		res = st_contOgg_rd_rdPacket(pAObjI, &pBSI, &pFHd);
		switch (res) {
		case ST_ERR_SUCC:
			break;
		case ST_ERR_IDAT:  /* invalid data found */
			/* skip packet */
			res = ST_ERR_SUCC;
			if (pBSI == NULL)
				stopThen = ST_B_TRUE;
			continue;
		case ST_ERR_FEOF:  /* end of file */
			stopThen = ST_B_TRUE;
			if (pBSI == NULL ||
					st_binobj_getDataSize(&pBSI->packCur.bdata) == 0)
				continue;
			break;
		/*case ST_ERR_OCFF:*/  /* can't find next frame */
		default:
			if (res == ST_ERR_OCFF)
				res = ST_ERR_SUCC;
			stopThen = ST_B_TRUE;
			continue;
		}

		if (pBSI == NULL) {
			st_contOgg_d_err(&pAObjI->opts, cFNCN, pAObjI->pFilen,
					"no sub stream set to work with");
			return ST_ERR_FAIL;
		}
		if (pBSI->pFHdBeg == NULL) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS4(&pAObjI->opts, pBSI, cFNCN,
						"packet had no beginning, skipping packet");
			continue;  /* skip packet */
		}
		if (st_binobj_getDataSize(&pBSI->packCur.bdata) == 0) {
			if (ST_AVFDEB_ISVERBAUDDET_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS3(&pAObjI->opts, pBSI, cFNCN,
						"empty packet, skipping");
			/*++pBSI->errorCnt;*/
			continue;
		}

		/* parse packet in pBSI->packCur */
		/**st_contOgg_prf("st_contOgg_readStreams(): "
				"parse pack (c %u)\n", pBSI->packCnt);**/
		res = st_contOgg_rdcod_parsePackOgg(&pAObjI->opts, pBSI);
		if (res == ST_ERR_IDAT || res == ST_ERR_FEOF) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_deb(&pAObjI->opts, cFNCN,
						"parsing packet interrupted");
			++pBSI->errorCnt;
			/* skip packet */
			res = ST_ERR_SUCC;
		}

		/* if we're doing a quick scan stop after the first 5 non-header-packets */
		if (pAObjI->opts.quickScan && pBSI->codecNonHeadPckCnt >= 5) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_deb(&pAObjI->opts, cFNCN,
						"stopping here since we're only doing a quick scan");
			break;
		}
	}
	if (res == ST_ERR_OSTP || res == ST_ERR_FEOF)
		res = ST_ERR_SUCC;

	/* finish PCM writer and decoder */
	st_contOgg_rd_finish_pcmClient_andDecoder(pAObjI);

	/* do aftermath... */
	if (res == ST_ERR_SUCC)
		st_contOgg_rd_afterReading(cFNCN, pAObjI);

	/* free some mem */
	st_contOgg_stc_rsetBsArrPackData(&pAObjI->bsArr);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read Flac stream
 *
 * pAObj   MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
st_contOgg_readFlacStream(Tst_contOgg_obj *pAObj)
{
	const char *cFNCN = __func__;
	Tst_err   res;
	Tst_bool  stopThen = ST_B_FALSE;
	Tst_foffs offsEnd;
	Tst_contOgg_fhd         dummyFHd;
	Tst_contOgg_substream   *pNewBS  = NULL;
	Tst_contOgg_substr_intn *pNewBSI = NULL;
	Tst_contOgg_obj_intn    *pAObjI;

	ST_ASSERTN_IARG(pAObj == NULL || pAObj->pObInternal == NULL)
	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;
	ST_ASSERTN_IARG(pAObjI->pFilen == NULL || pAObjI->pStrrd == NULL)

	/* init some stuff */
	/** */
	st_contOgg_stc_rsetAObj_prot(pAObj, ST_B_FALSE,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL,
			ST_B_FALSE, NULL);
	pAObjI->alrRead         = ST_B_TRUE;
	pAObjI->isSrcOggOrFlac  = ST_B_FALSE;
	pAObjI->opts.modeRdOrWr = ST_B_TRUE;
	/** */
	res = ST_CONTOGG__addFlacBS(pAObjI, &dummyFHd, &pNewBS, &pNewBSI);
	if (res != ST_ERR_SUCC)
		return res;

	if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
		st_contOgg_d_deb(&pAObjI->opts, cFNCN, "starting...");
	if (ST_AVFDEB_ISVERBAUD_OR_ANA_D(pAObjI->opts))
		st_contOgg_rd_showUsedChecksums(&pAObjI->opts, pAObjI->isSrcOggOrFlac);

	/* */
	pAObjI->startOffs = st_streamrd_getCurPos(pAObjI->pStrrd);

	/* read stream */
	while (res == ST_ERR_SUCC && ! stopThen &&
			! st_streamrd_isEOF(pAObjI->pStrrd)) {
		++pNewBSI->packCur.nr;
		/* parse data */
		/**if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
			st_contOgg_d_deb(&pAObjI->opts, cFNCN,
					"parse frame...");**/
		res = st_contOgg_rdcod_parsePackFlac(&pAObjI->opts,
				pNewBSI, pAObjI->pStrrd);
		if (res == ST_ERR_SUCC) {
			++dummyFHd.frSequNr;
			++dummyFHd.realNr;
		}
		/* */
		if (! pAObjI->opts.flacOpts.readAll && pNewBSI->pFlac != NULL &&
				pNewBSI->pFlac->aFramesCnt > 0)
			break;
		/* if we're doing a quick scan stop after the first 5 non-header-packets */
		if (pAObjI->opts.quickScan && pNewBSI->pFlac->aFramesCnt >= 5) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_deb(&pAObjI->opts, cFNCN,
						"stopping here since we're only doing a quick scan");
			break;
		}
	}
	if (res == ST_ERR_IDAT || res == ST_ERR_OSTP || res == ST_ERR_FEOF) {
		if (res == ST_ERR_IDAT) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_deb(&pAObjI->opts, cFNCN,
						"reading stream interrupted");
			++pNewBSI->errorCnt;
		}
		res = ST_ERR_SUCC;
	}

	pNewBSI->ended = ST_B_TRUE;

	/* finish PCM writer and decoder */
	st_contOgg_rd_finish_pcmClient_andDecoder(pAObjI);

	/* do aftermath... */
	if (res == ST_ERR_SUCC) {
		offsEnd = st_streamrd_getCurPos(pAObjI->pStrrd);
		if (! pAObjI->opts.quickScan &&
				pNewBSI->flacOffsFirstAudPkt > 0 &&
				offsEnd > pAObjI->startOffs &&
				offsEnd > pNewBSI->flacOffsFirstAudPkt)
			pNewBSI->codecNonHeadSize = (Tst_fsize)(offsEnd -
					pNewBSI->flacOffsFirstAudPkt);
		else
			pNewBSI->codecNonHeadSize = 0;
		/* */
		st_contOgg_rd_afterReading(cFNCN, pAObjI);
	}

	/* free some mem */
	st_contOgg_stc_rsetBsArrPackData(&pAObjI->bsArr);

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Write OGG streams
 */
Tst_err
st_contOgg_writeStreams(Tst_contOgg_obj *pAObj)
{
	const char *cFNCN = __func__;
	Tst_err    res          = ST_ERR_SUCC;
	Tst_uint32 frDatSz,
	           bsSerialLast = 0;
	Tst_bool   rewrPack;
	Tst_contOgg_fhd         fhd;
	Tst_contOgg_substream   *pBS  = NULL;
	Tst_contOgg_substr_intn *pBSI = NULL;
	Tst_contOgg_obj_intn    *pAObjI;

	ST_ASSERTN_IARG(pAObj == NULL || pAObj->pObInternal == NULL)
	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;
	ST_ASSERTN_IARG(pAObjI->pFilen == NULL || pAObjI->pStrrd == NULL ||
			pAObjI->pStrwr == NULL)

	st_contOgg_stc_rsetFHd(&fhd);
	if (! pAObjI->alrRead)
		return ST_ERR_FAIL;
	pAObjI->opts.modeRdOrWr = ST_B_FALSE;

	/* */
	pAObjI->startOffs = st_streamrd_getCurPos(pAObjI->pStrrd);
	pAObjI->totSize   = 0;
	st_contOgg_stc_rsetBsArrPackData(&pAObjI->bsArr);

	if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
		st_contOgg_d_deb(&pAObjI->opts, cFNCN, "starting...%s",
				(pAObjI->opts.basOpts.pretWr ? "  (pretending)" : ""));

	/* mark all sub streams that need a vorbis comment update */
	st_contOgg_wr_markBSforVCmtUpd(pAObj);

	/* rewrite stream (meaning all sub streams) */
	while (res == ST_ERR_SUCC && ! st_streamrd_isEOF(pAObjI->pStrrd)) {
		/* read frame header */
		res = st_contOgg_rd_rdFrameHd(pAObjI, &fhd);
		if (res == ST_ERR_SUCC &&
				(bsSerialLast != fhd.bsSerial || fhd.bsSerial == 0)) {
			/* get current stream pointer */
			pBS = st_contOgg_rd_getBS(pAObjI, fhd.bsSerial);
			if (pBS == NULL) {
				st_contOgg_d_err(&pAObjI->opts, cFNCN, pAObjI->pFilen,
						"no sub stream found to work with");
				res = ST_ERR_FAIL;
			}
			pBSI         = (Tst_contOgg_substr_intn*)pBS->pObInternal;
			bsSerialLast = fhd.bsSerial;
		}
		if (res != ST_ERR_SUCC)
			break;
		/* */
		if (fhd.frSequNr == 1) {
			if ((! fhd.hdTypeBOS || fhd.hdTypeCont || fhd.hdTypeEOS) &&
					ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, cFNCN,
						"correcting frame flags of first frame");
			fhd.hdTypeBOS  = ST_B_TRUE;
			fhd.hdTypeCont = ST_B_FALSE;
			fhd.hdTypeEOS  = ST_B_FALSE;
		} else if (fhd.hdTypeBOS) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, cFNCN,
						"correcting frame flags of frame #%u",
						fhd.frSequNr);
			fhd.hdTypeBOS = ST_B_FALSE;
		}
		if (! fhd.hdTypeCont) {
			/* frame is beginning of a new packet */
			/** */
			if (pBSI->wr.pendingPack) {
				/* replace vorbis comment in packet */
				if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
					st_contOgg_d_debBS1(&pAObjI->opts, pBSI, cFNCN,
							"replacing Vorbis Comment in packet #%u",
							pBSI->wr.vcmtPackNr);
				res = st_contOgg_wr_replaceVCmt(pAObjI, pBS);
				/* write packet */
				if (res == ST_ERR_SUCC)
					res = st_contOgg_wr_wrPacket(pAObjI, pBSI,
							&pBSI->wr.pendPckFHd);
				if (res != ST_ERR_SUCC)
					break;
				/* */
				pBSI->wr.pendingPack = ST_B_FALSE;
			}
			/** */
			st_contOgg_stc_rsetPack_dataOnly(&pBSI->packCur);
			++pBSI->packCur.nr;
			++pBSI->wr.packCnt;
		}

		/* */
		rewrPack = pBSI->wr.needVCmtUpd &&
				(pBSI->wr.vcmtPackNr == pBSI->packCur.nr) &&
				! pBSI->wr.eosWritten;

		/* read frame data */
		frDatSz = (fhd.frSz >= fhd.hdSz ? fhd.frSz - fhd.hdSz : 0);
		res     = st_contOgg_rd_rdFrameDat(pAObjI, pBSI, &fhd,
				frDatSz, /*appendOrAuto:*/rewrPack);
		if (res != ST_ERR_SUCC)
			break;

		if (rewrPack) {
			if (! pBSI->wr.pendingPack)
				st_contOgg_stc_copyFHd(&fhd, &pBSI->wr.pendPckFHd);
			pBSI->wr.pendingPack = ST_B_TRUE;
			continue;
		}

		/* */
		if (pBSI->wr.eosWritten && ! (fhd.hdTypeEOS && fhd.hdTypeCont)) {
			if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
				st_contOgg_d_debBS1(&pAObjI->opts, pBSI, cFNCN,
						"skipping frame beyond EOS");
			continue;
		}
		if (fhd.hdTypeEOS)
			pBSI->wr.eosWritten = ST_B_TRUE;

		/* write frame */
		/** */
		frDatSz = st_binobj_getDataSize(&pBSI->packCur.bdata);
		/** */
		res = st_contOgg_wr_wrFrame(pAObjI, pBSI, &fhd, 0, frDatSz);
	}
	if (res == ST_ERR_OCFF || res == ST_ERR_OSTP || res == ST_ERR_FEOF)
		res = ST_ERR_SUCC;

	if (res == ST_ERR_SUCC) {
		/* add End-Of-Stream packet where necessary */
		res = st_contOgg_wr_addEOS(pAObj);
	}

	if (res == ST_ERR_SUCC &&
			ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
		st_contOgg_d_deb(&pAObjI->opts, cFNCN, "done");

	if (res == ST_ERR_SUCC)
		st_contOgg_wr_showStats(pAObj);

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Write Flac stream
 */
Tst_err
st_contOgg_writeFlacStream(Tst_contOgg_obj *pAObj)
{
	const char *cFNCN = __func__;
	Tst_err   res = ST_ERR_SUCC;
	Tst_fsize stillToCopy;
	Tst_contOgg_substream   *pBS  = NULL;
	Tst_contOgg_substr_intn *pBSI = NULL;
	Tst_contOgg_obj_intn    *pAObjI;

	ST_ASSERTN_IARG(pAObj == NULL || pAObj->pObInternal == NULL)
	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;
	ST_ASSERTN_IARG(pAObjI->pFilen == NULL || pAObjI->pStrrd == NULL ||
			pAObjI->pStrwr == NULL)

	/* init some stuff */
	/** */
	if (! pAObjI->alrRead || pAObjI->isSrcOggOrFlac || pAObjI->bsArr.pArr == NULL)
		return ST_ERR_FAIL;
	pAObjI->isSrcOggOrFlac  = ST_B_FALSE;
	pAObjI->opts.modeRdOrWr = ST_B_FALSE;
	/** */
	pBS  = &pAObjI->bsArr.pArr[0];
	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, pBSI == NULL)

	/* */
	pAObjI->startOffs = st_streamrd_getCurPos(pAObjI->pStrrd);
	stillToCopy       = pAObjI->totSize;
	pAObjI->totSize   = 0;
	st_contOgg_stc_rsetBsArrPackData(&pAObjI->bsArr);

	if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
		st_contOgg_d_deb(&pAObjI->opts, cFNCN, "starting...%s",
				(pAObjI->opts.basOpts.pretWr ? "  (pretending)" : ""));

	/* rewrite stream */
	/** 'fLaC' */
	if (! st_streamrd_rdMemcmpBytes(pAObjI->pStrrd, 4, "fLaC")) {
		if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
			st_contOgg_d_debBS1(&pAObjI->opts, pBSI, cFNCN,
					"invalid flac header found, aborting");
		res = ST_ERR_IDAT;
	} else {
		st_streamrd_rdSkipBytes(pAObjI->pStrrd, 4, ST_B_TRUE);
		if (! pAObjI->opts.basOpts.pretWr)
			res = st_streamwr_wrBuffer(pAObjI->pStrwr, 4, (const Tst_buf*)"fLaC");
		if (res == ST_ERR_SUCC)
			pAObjI->totSize += 4;
	}
	/** write/copy header packets */
	if (res == ST_ERR_SUCC)
		res = st_contOgg_wrcod_flacHeaders(pAObjI, pBS);
	/** copy remaining data */
	if (res == ST_ERR_SUCC && ! st_streamrd_isEOF(pAObjI->pStrrd)) {
		Tst_uint32 copied = 0,
		           toCopy = 0xffffffff;
		Tst_fsize  alrCpd;

		alrCpd       = (Tst_fsize)(st_streamrd_getCurPos(pAObjI->pStrrd) -
					pAObjI->startOffs);
		stillToCopy -= alrCpd;

		if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
			st_contOgg_d_debBS1(&pAObjI->opts, pBSI, cFNCN,
					"copying audio data... ("ST_TFSIZE_PRF_LU" bytes)",
					stillToCopy);

		/* in case we have a file larger than 4GB we need
		 *   this additional while-loop */
		while (res == ST_ERR_SUCC && ! st_streamrd_isEOF(pAObjI->pStrrd)) {
			if ((Tst_fsize)toCopy > stillToCopy)
				toCopy = (Tst_uint32)stillToCopy;
			res = st_contOgg_wrcod_copyData(&pAObjI->opts,
					pAObjI->pStrrd, pAObjI->pStrwr, toCopy, &copied);
			if (res == ST_ERR_SUCC) {
				pAObjI->totSize += copied;
				stillToCopy     -= (Tst_fsize)copied;
			}
		}
	}

	if (res == ST_ERR_SUCC &&
			ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
		st_contOgg_d_deb(&pAObjI->opts, cFNCN, "done");

	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_err
ST_CONTOGG__addFlacBS(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_fhd *pDummyFHd,
		Tst_contOgg_substream **ppNewBS, Tst_contOgg_substr_intn **ppNewBSI)
{
	Tst_err res;

	res = st_contOgg_stc_reszBsArr(pAObjI->pFilen, &pAObjI->bsArr, 1);
	if (res != ST_ERR_SUCC)
		return res;
	pAObjI->bsArr.cnt = 1;
	/* */
	*ppNewBS  = &pAObjI->bsArr.pArr[0];
	*ppNewBSI = (Tst_contOgg_substr_intn*)(*ppNewBS)->pObInternal;
	ST_ASSERTN_NUM(ST_ERR_FAIL, *ppNewBSI == NULL)
	(*ppNewBSI)->isSrcOggOrFlac = ST_B_FALSE;
	(*ppNewBSI)->nr             = 1;
	(*ppNewBSI)->grpIx          = 1;
	(*ppNewBSI)->grpSIx         = 1;
	/* */
	st_contOgg_stc_rsetFHd(pDummyFHd);
	pDummyFHd->offs     = 0;
	pDummyFHd->frSequNr = 1;
	pDummyFHd->realNr   = 1;
	/* */
	(*ppNewBSI)->started = ST_B_TRUE;
	(*ppNewBSI)->pFHdBeg = pDummyFHd;

	return ST_ERR_SUCC;
}

/******************************************************************************/
