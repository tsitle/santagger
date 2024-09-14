/******************************************************************************/
/* cont_ogg_zwr.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for writing OGG containers / Flac files                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/streamwr.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_CONT_OGG_ZWR_C
#include "cont_ogg_zwr-pp.h"
/*** */
#include "cont_ogg_zwr_cod-pp.h"
#include "cont_ogg_stcs-prot.h"
#include "cont_ogg_zfdeb-prot.h"
#include "../cont_ogg_met_vorbcomm/mc_vorbcomm_gs-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_CONT_OGG_ZWR_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Mark all sub streams that need a vorbis comment update
 */
void
st_contOgg_wr_markBSforVCmtUpd(Tst_contOgg_obj *pAObj)
{
	Tst_contOgg_substream            *pBS = NULL;
	Tst_contOgg_substr_intn          *pBSI;
	Tst_contOgg_vorbCommentRaw const *pVCR;

	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	while ((pBS = st_contOgg_ite_getNextStream_any(pAObj, pBS)) != NULL) {
		pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
		ST_ASSERTN_VOID(pBSI == NULL)

		switch (pBSI->bsType) {
		case ST_CONTOGG_BSTP_VORB:
		case ST_CONTOGG_BSTP_THEO:
		case ST_CONTOGG_BSTP_FLAC:
		case ST_CONTOGG_BSTP_SPEE:
		case ST_CONTOGG_BSTP_CELT:
			break;
		default:
			continue;
		}

		pVCR = st_contOgg_vorbComm_gs_getRawComment(pBS);
		if (pVCR == NULL)
			continue;
		if (! st_contOgg_vorbComm_gs_getRC_wasModified(pVCR) &&
				! ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.forceRewrCmts)
			continue;
		pBSI->wr.needVCmtUpd = ST_B_TRUE;
		pBSI->wr.vcmtPackNr  = st_contOgg_vorbComm_gs_getRC_srcPackNr(pVCR);
		pBSI->wr.vcmtSrcBP   = st_contOgg_vorbComm_gs_getRC_srcCmtBegPos(pVCR);
		if (pBSI->wr.vcmtSrcBP >= 0)
			pBSI->wr.vcmtSrcEP = st_contOgg_vorbComm_gs_getRC_srcCmtEndPos(pVCR);
		else
			pBSI->wr.vcmtSrcEP = -1;
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Show some stats
 */
void
st_contOgg_wr_showStats(Tst_contOgg_obj *pAObj)
{
	const char *cFNCN = __func__;
	Tst_contOgg_obj_intn    *pAObjI;
	Tst_contOgg_substream   *pBS = NULL;
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;
	if (! ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
		return;

	while ((pBS = st_contOgg_ite_getNextStream_any(pAObj, pBS)) != NULL) {
		pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
		ST_ASSERTN_VOID(pBSI == NULL)

		st_contOgg_d_debBS1(&pAObjI->opts, pBSI, cFNCN,
				"wrote %u frames containing %u packets",
				pBSI->wr.frameCnt, pBSI->wr.packCnt);
	}
}

/*----------------------------------------------------------------------------*/

/*
 * Replace vorbis comment in packet
 */
Tst_err
st_contOgg_wr_replaceVCmt(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substream *pBS)
{
	const char *cFNCN = __func__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 packSz;
	Tst_contOgg__wr_partialPack restPP,
	                            prePP;
	Tst_contOgg_substr_intn     *pBSI;

	ST_ASSERTN_IARG(pAObjI == NULL || pBS == NULL || pBS->pObInternal == NULL)

	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;

	/**st_binobj_copyIntoFile(&pBSI->packCur.bdata, "pack2.tmp");**/

	/* copy everything before the vorbis comment in the packet to prePP */
	if (pBSI->wr.vcmtSrcBP < 0) {
		st_contOgg_d_err(&pAObjI->opts, cFNCN, pAObjI->pFilen,
				"no vorbis comment found to replace");
		return ST_ERR_IDAT;
	}
	ST_CONTOGG__wr_stc_initPP(&prePP);
	if (pBSI->wr.vcmtSrcBP > 0) {
		prePP.sz = (Tst_uint32)pBSI->wr.vcmtSrcBP;
		res      = ST_CONTOGG__wr_copyToPP(cFNCN, pAObjI, pBSI, 0, &prePP);
	}

	/* find beginning of next codec header packet */
	packSz = st_binobj_getDataSize(&pBSI->packCur.bdata);
	if (res == ST_ERR_SUCC &&
			pBSI->wr.vcmtSrcEP >= 0 &&
			(Tst_uint32)pBSI->wr.vcmtSrcEP < packSz) {
		res = st_binobj_attachStreamrd(&pBSI->packCur.bdata,
				&pBSI->sobjRdFBuf, (Tst_uint32)pBSI->wr.vcmtSrcEP);
		if (res == ST_ERR_SUCC) {
			Tst_int32 pos;

			pos = st_contOgg_wrcod_fndNextHd(pAObjI, pBSI, &pBSI->sobjRdFBuf);
			if (pos > 0) {
				if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
					st_contOgg_d_deb(&pAObjI->opts, cFNCN,
							"skipping %d bytes of padding", pos);
				pBSI->wr.vcmtSrcEP += pos;
			}
		}
		st_binobj_detachStreamrd(&pBSI->packCur.bdata);
		if (res != ST_ERR_SUCC)
			return res;
	}

	ST_CONTOGG__wr_stc_initPP(&restPP);

	if (res == ST_ERR_SUCC) {
		if (pBSI->wr.vcmtSrcEP >= 0) {
			if (packSz < (Tst_uint32)pBSI->wr.vcmtSrcEP)
				res = ST_ERR_IDAT;
			else
				restPP.sz = packSz - (Tst_uint32)pBSI->wr.vcmtSrcEP;
		} else
			restPP.sz = packSz;
		/**st_contOgg_prf("%s(): packSz %u, restSz %u\n",
				cFNCN, packSz, restPP.sz);**/
	}

	/* copy everything after the vorbis comment in the packet to restPP */
	if (res == ST_ERR_SUCC && restPP.sz > 0)
		res = ST_CONTOGG__wr_copyToPP(cFNCN, pAObjI, pBSI,
				(pBSI->wr.vcmtSrcEP >= 0 ?
					(Tst_uint32)pBSI->wr.vcmtSrcEP : 0), &restPP);

	/* clear the old packet's data */
	st_contOgg_stc_rsetPack_dataOnly(&pBSI->packCur);

	/* append 'pre' to pBSI->packCur */
	if (res == ST_ERR_SUCC && prePP.sz > 0)
		res = ST_CONTOGG__wr_appendPP(cFNCN, pAObjI, pBSI, &prePP);

	ST_CONTOGG__wr_stc_freePP(&prePP);

	/* append new vorbis comment to pBSI->packCur */
	if (res == ST_ERR_SUCC)
		res = st_contOgg_wrcod_appendVCmt(pAObjI, pBS);

	/* append 'rest' to pBSI->packCur */
	if (res == ST_ERR_SUCC && restPP.sz > 0)
		res = ST_CONTOGG__wr_appendPP(cFNCN, pAObjI, pBSI, &restPP);

#	if 0
	if (res == ST_ERR_SUCC) {
		Tst_uint32 x,
		           ttt = 0,
		           psz;

		st_contOgg_prf("%s(): checking segTab\n", cFNCN);
		for (x = 0; x < pBSI->packCur.segsUsd; x++)
			ttt += pBSI->packCur.pSegs[x];
		psz = st_binobj_getDataSize(&pBSI->packCur.bdata);
		if (ttt != psz) {
			/**/st_contOgg_prf("%s(): segTab faulty: sz %u != %u\n",
					cFNCN, ttt, psz);/**/
			res = ST_ERR_IDAT;
		} /**/else
			st_contOgg_prf("%s(): segTab ok: sz %u\n",
					cFNCN, psz);/**/
	}
#	endif


	ST_CONTOGG__wr_stc_freePP(&restPP);
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Add End-Of-Stream packets where necessary
 */
Tst_err
st_contOgg_wr_addEOS(Tst_contOgg_obj *pAObj)
{
#	define LOC_FNCN_  __func__
	Tst_err res = ST_ERR_SUCC;
	Tst_contOgg_obj_intn    *pAObjI;
	Tst_contOgg_substream   *pBS = NULL;
	Tst_contOgg_substr_intn *pBSI;
	Tst_contOgg_fhd         fhd;

	ST_ASSERTN_IARG(pAObj == NULL || pAObj->pObInternal == NULL)

	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;

	st_contOgg_stc_rsetFHd(&fhd);
	st_sysUInt64_setLoHi(&fhd.granulePos, (Tst_uint32)-1, (Tst_uint32)-1);
	fhd.hdTypeEOS = ST_B_TRUE;
	fhd.vers      = 0;

	while ((pBS = st_contOgg_ite_getNextStream_any(pAObj, pBS)) != NULL) {
		pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
		ST_ASSERTN_NUM(ST_ERR_FAIL, pBSI == NULL)

		if (pBSI->wr.eosWritten)
			continue;

		if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
			st_contOgg_d_debBS1(&pAObjI->opts, pBSI, LOC_FNCN_,
					"adding missing EOS packet");
		st_contOgg_stc_rsetPack_dataOnly(&pBSI->packCur);
		/* */
		fhd.bsSerial = pBSI->serial;
		/* */
		res = st_contOgg_wr_wrFrame(pAObjI, pBSI, &fhd, 0, 0);
		if (res != ST_ERR_SUCC)
			break;
	}
	return res;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Write one OGG packet (consisting of one or more frames)
 */
Tst_err
st_contOgg_wr_wrPacket(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substr_intn *pBSI, const Tst_contOgg_fhd *pFHd)
{
#	define LOC_FNCN_     __func__
#	define LOC_DEFFRSZ_  (255 * 16 * 8)  /* ca. 32kb */
	Tst_err    res     = ST_ERR_SUCC;
	Tst_uint32 packSz,
	           written = 0,
	           frDatSz,
	           tmpFrDatSz,
	           usd     = 0,
	           x;
	Tst_contOgg_fhd cfhd;

	ST_ASSERTN_IARG(pAObjI == NULL || pBSI == NULL || pFHd == NULL)

	packSz = st_binobj_getDataSize(&pBSI->packCur.bdata);
	st_contOgg_stc_rsetFHd(&cfhd);
	st_contOgg_stc_copyFHd(pFHd, &cfhd);
	st_sysUInt64_setLoHi(&cfhd.granulePos, (Tst_uint32)-1, (Tst_uint32)-1);
	cfhd.hdTypeBOS  = ST_B_FALSE;
	cfhd.hdTypeEOS  = ST_B_FALSE;
	cfhd.hdTypeCont = ST_B_FALSE;

	/**st_contOgg_prf("%s(): writing pack pk#%u (%u bytes)\n",
			LOC_FNCN_, pBSI->packCur.nr, packSz);**/

	frDatSz = LOC_DEFFRSZ_;
	while (written < packSz) {
		if (written + frDatSz > packSz)
			frDatSz = packSz - written;
		/* prepare FRAME SEGMENTS/SEGMENT TABLE */
		tmpFrDatSz   = frDatSz;
		cfhd.segsUsd = 0;
		for (x = 0; x < sizeof(cfhd.segTab); x++) {
			if (usd >= pBSI->packCur.segsUsd)
				break;
			cfhd.segTab[x] = pBSI->packCur.pSegs[usd++];
			if (tmpFrDatSz < cfhd.segTab[x]) {
				frDatSz -= tmpFrDatSz;
				--usd;
				break;
			}
			tmpFrDatSz -= cfhd.segTab[x];
			/**st_contOgg_prf("%s(): segTab %3u: 0x%02x (%u)\n",
					LOC_FNCN_, x, cfhd.segTab[x], cfhd.segTab[x]);**/
			++cfhd.segsUsd;
			if (tmpFrDatSz == 0)
				break;
		}
		if (cfhd.segsUsd == 0) {
			if (frDatSz > 0) {
				res = ST_ERR_IDAT;
				break;
			}
			cfhd.segTab[0] = 0;
			cfhd.segsUsd   = 1;
			/**st_contOgg_prf("%s(): segTab %3u: 0x%02x (%u)\n",
					LOC_FNCN_, 0, cfhd.segTab[0], cfhd.segTab[0]);**/
		}
		/* */
		if (written + frDatSz >= packSz) {
			/* for the last frame we use granule pos=0 */
			st_sysUInt64_setLoHi(&cfhd.granulePos, 0, 0);
		}
		/* */
		res = st_contOgg_wr_wrFrame(pAObjI, pBSI, &cfhd,
				written, frDatSz);
		if (res != ST_ERR_SUCC)
			break;
		written += frDatSz;
		cfhd.hdTypeCont = ST_B_TRUE;
	}

	return res;
#	undef LOC_FNCN_
#	undef LOC_DEFFRSZ_
}

/*----------------------------------------------------------------------------*/

/*
 * Write one OGG frame
 */
Tst_err
st_contOgg_wr_wrFrame(Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substr_intn *pBSI, Tst_contOgg_fhd *pFHd,
		const Tst_uint32 frDatStart, Tst_uint32 frDatSz)
{
#	define LOC_FNCN_     __func__
#	define LOC_CRCOFFS_  22
#	define LOC_OGGS_     (Tst_buf*)"OggS"
	Tst_err    res;
	Tst_uint32 crc32     = 0,
	           toRd,
	           rdTot     = 0,
	           rdFromStr = 0;
	Tst_buf    buf[4096];
	Tst_streamrd *pStrrdTmp;
	Tst_streamwr *pStrwrBuf;

	ST_ASSERTN_IARG(pAObjI == NULL || pBSI == NULL || pFHd == NULL)

	/* all numeric values are written as Little-Endian */

	/**if (pBSI->packCur.nr < 4)
		st_contOgg_prf("%s(): "
				"write framehd pk#%u frO#%04u [BOS=%d,CON=%d,EOS=%d]\n",
				LOC_FNCN_, pBSI->packCur.nr, pBSI->wr.frameCnt + 1,
				pFHd->hdTypeBOS, pFHd->hdTypeCont, pFHd->hdTypeEOS);**/

	pStrrdTmp = &pBSI->sobjRdFBuf;
	pStrwrBuf = &pBSI->sobjWrFBuf;
	st_streamrd_stc_rsetSObj(pStrrdTmp);
	res = st_streamwr_setOutput_buffer(pStrwrBuf);

	/* start CRC */
	if (res == ST_ERR_SUCC)
		st_streamwr_startCRC32(pStrwrBuf);

	/* write frame header to temporary buffer */
	if (res == ST_ERR_SUCC) {
		Tst_byte byt;
		Tst_buf  zeros[4] = {0, 0, 0, 0};

		/** 'OggS' */
		res = st_streamwr_wrBuffer(pStrwrBuf, 4, LOC_OGGS_);
		/** VERSION */
		if (res == ST_ERR_SUCC)
			res = st_streamwr_wrByte(pStrwrBuf, 8, pFHd->vers);
		/** HEADER TYPE */
		if (res == ST_ERR_SUCC) {
			/**st_contOgg_prf("%s(): hdTp c:%d, b:%d, e:%d\n",
					LOC_FNCN_,
					pFHd->hdTypeCont, pFHd->hdTypeBOS, pFHd->hdTypeEOS);**/
			/* */
			byt  = (pFHd->hdTypeCont ? 0x01 : 0);
			byt |= (pFHd->hdTypeBOS  ? 0x02 : 0);
			byt |= (pFHd->hdTypeEOS  ? 0x04 : 0);
			/* */
			res = st_streamwr_wrByte(pStrwrBuf, 8, byt);
		}
		/** GRANULE POS
		 **   if granPos == 0xff..ff -->no packets finish on this frame */
		if (res == ST_ERR_SUCC)
			res = st_streamwr_wrUInt64(pStrwrBuf,
					ST_STREAMWR_OEND_LE, 64, &pFHd->granulePos);  /* ENDIAN: HOST-->LE */
		/** BITSTREAM SERIAL */
		if (res == ST_ERR_SUCC)
			res = st_streamwr_wrUInt32(pStrwrBuf,
					ST_STREAMWR_OEND_LE, 32, pFHd->bsSerial);  /* ENDIAN: HOST-->LE */
		/** FRAME SEQUENCE NUMBER */
		if (res == ST_ERR_SUCC) {
			res = st_streamwr_wrUInt32(pStrwrBuf,
					ST_STREAMWR_OEND_LE, 32, pBSI->wr.frameCnt);  /* ENDIAN: HOST-->LE */
			++pBSI->wr.frameCnt;
			pFHd->frSequNr = pBSI->wr.frameCnt;
			pFHd->realNr   = pBSI->wr.frameCnt;
		}
		/** CRC - dummy */
		if (res == ST_ERR_SUCC)
			res = st_streamwr_wrBuffer(pStrwrBuf, 4, zeros);
		/* prepare FRAME SEGMENTS/SEGMENT TABLE */
		if (res == ST_ERR_SUCC) {
			Tst_uint32 tmpDSz = 0,
			           stX;

			for (stX = 0; stX < pFHd->segsUsd; stX++)
				tmpDSz += pFHd->segTab[stX];
			if (tmpDSz != frDatSz) {
				/* segment table is corrupt */
				if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
					st_contOgg_d_debBS2(&pAObjI->opts, pBSI, pFHd,
							LOC_FNCN_,
							"correcting corrupted segment table");
				if (tmpDSz > frDatSz) {
					/* this is very probably the result of a cropped stream */
					tmpDSz        = 0;
					pFHd->segsUsd = 1;
					do {
						tmpDSz += pFHd->segTab[pFHd->segsUsd - 1];
						if (tmpDSz > frDatSz) {
							pFHd->segTab[pFHd->segsUsd - 1] -= (tmpDSz - frDatSz);
							break;
						}
						++pFHd->segsUsd;
					} while (1);
				} else if (tmpDSz < frDatSz) {
					/* problem here is that missing lacing values
					 *   cannot be recovered.
					 *   so we have to delete data here...
					 *   On the other hand, this case is almost impossible  */
					if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
						st_contOgg_d_debBS2(&pAObjI->opts, pBSI, pFHd,
								LOC_FNCN_,
								"had to crop frame data (-%u bytes)",
								frDatSz - tmpDSz);
					frDatSz = tmpDSz;
				}
			}
		}
		/** FRAME SEGMENTS */
		if (res == ST_ERR_SUCC) {
			/**st_contOgg_prf("%s(): segsUsd %u\n",
					LOC_FNCN_, pFHd->segsUsd);**/
			res = st_streamwr_wrByte(pStrwrBuf, 8, pFHd->segsUsd);
		}
		/** SEGMENT TABLE */
		if (res == ST_ERR_SUCC && pFHd->segsUsd > 0) {
			/**for (stX = 0; stX < pFHd->segsUsd; stX++)
				st_contOgg_prf("%s(): fr#%04u: segTab %3u: 0x%02x (%u)\n",
						LOC_FNCN_, pFHd->frSequNr, stX,
						pFHd->segTab[stX], pFHd->segTab[stX]);**/
			res = st_streamwr_wrBuffer(pStrwrBuf, pFHd->segsUsd, pFHd->segTab);
		}
	}

	/* write frame data to temporary buffer */
	if (res == ST_ERR_SUCC && frDatSz > 0) {
		res = st_binobj_attachStreamrd(&pBSI->packCur.bdata,
				pStrrdTmp, frDatStart);

		/** read data from pStrrdTmp and append it to strwrBuf */
		if (res == ST_ERR_SUCC) {
			toRd = sizeof(buf);
			while (rdTot < frDatSz) {
				if (rdTot + toRd > frDatSz)
					toRd = frDatSz - rdTot;
				res = st_streamrd_rdBuffer_noCRC(pStrrdTmp,
						toRd, buf, &rdFromStr);
				if (res != ST_ERR_SUCC && res != ST_ERR_FEOF)
					break;
				/* */
				res = st_streamwr_wrBuffer(pStrwrBuf, rdFromStr, buf);
				if (res != ST_ERR_SUCC)
					break;
				rdTot += rdFromStr;
				if (rdFromStr != toRd)
					break;
			}
		}
		st_binobj_detachStreamrd(&pBSI->packCur.bdata);
	}

	/* stop and get CRC */
	if (res == ST_ERR_SUCC) {
		crc32 = st_streamwr_getCRC32(pStrwrBuf);
		st_streamwr_stopCRC32(pStrwrBuf);
		/**st_contOgg_prf("%s(): "
					"stream #%u.%u: crc32 0x%08x\n",
					LOC_FNCN_, pBSI->grpIx, pBSI->grpSIx, crc32);**/
	}

	/* write frame header + data to final output */
	if (res == ST_ERR_SUCC) {
		Tst_uint32 tmpUI;

		tmpUI = st_streamwr_getOutputBufSize(pStrwrBuf);
		res   = st_streamrd_setInput_buffer(pStrrdTmp,
				st_streamwr_getOutputBuffer(pStrwrBuf), tmpUI);

		/** write beginning of header */
		toRd = LOC_CRCOFFS_;
		if (res == ST_ERR_SUCC)
			res = st_streamrd_rdBuffer_noCRC(pStrrdTmp,
					toRd, buf, &rdFromStr);
		if (res == ST_ERR_SUCC && ! pAObjI->opts.basOpts.pretWr)
			res = st_streamwr_wrBuffer(pAObjI->pStrwr, rdFromStr, buf);
		rdTot  = rdFromStr;
		tmpUI -= (tmpUI >= rdFromStr ? rdFromStr : tmpUI);
		/** write CRC32 */
		if (res == ST_ERR_SUCC && ! pAObjI->opts.basOpts.pretWr)
			res = st_streamwr_wrUInt32(pAObjI->pStrwr,
					ST_STREAMWR_OEND_LE, 32, crc32);
		st_streamrd_rdSkipBytes(pStrrdTmp, 4, ST_B_TRUE);
		rdTot += 4;
		tmpUI -= (tmpUI >= 4 ? 4 : tmpUI);
		/** read rest from pStrrdTmp and write it to pAObjI->pStrwr */
		toRd  = tmpUI;
		tmpUI = 0;
		if (res == ST_ERR_SUCC)
			res = st_contOgg_wrcod_copyData(&pAObjI->opts,
					pStrrdTmp, pAObjI->pStrwr, toRd, &tmpUI);
		rdTot += tmpUI;
		if (res == ST_ERR_SUCC)
			pAObjI->totSize += rdTot;
	}

	return res;
#	undef LOC_FNCN_
#	undef LOC_CRCOFFS_
#	undef LOC_OGGS_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg__wr_partialPack */
static void
ST_CONTOGG__wr_stc_initPP(Tst_contOgg__wr_partialPack *pPP)
{
	ST_ASSERTN_VOID(pPP == NULL)

	pPP->sz      = 0;
	pPP->segsUsd = 0;
	pPP->segsSz  = 0;
	pPP->pSegTab = NULL;
	st_binobj_stc_initBO(&pPP->bindat);
}

static void
ST_CONTOGG__wr_stc_freePP(Tst_contOgg__wr_partialPack *pPP)
{
	ST_ASSERTN_VOID(pPP == NULL)

	ST_DELPOINT(pPP->pSegTab)
	st_binobj_stc_freeBO(&pPP->bindat);
}

/*----------------------------------------------------------------------------*/

/*
 * Copy data from pBSI->packCur.bdata to pPP->bindat
 *   and copy the corresponding lacing values
 *   from pBSI->packCur.pSegs to pPP->pSegTab
 */
static Tst_err
ST_CONTOGG__wr_copyToPP(const char *pFnc,
		Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substr_intn *pBSI,
		const Tst_uint32 startPos,
		Tst_contOgg__wr_partialPack *pPP)
{
	Tst_err    res   = ST_ERR_SUCC;
	Tst_uint32 segsX = 0;
	Tst_streamrd *pStrrdTmp = &pBSI->sobjRdFBuf;

	ST_ASSERTN_IARG(pPP == NULL)

	if (pPP->sz == 0)
		return res;

	/* read data from pStrrdTmp and append it to pPP->bindat */
	res = st_binobj_attachStreamrd(&pBSI->packCur.bdata,
			pStrrdTmp, startPos);
	if (res == ST_ERR_SUCC)
		res = st_binobj_appendData_streamrd(&pPP->bindat, pStrrdTmp,
				pPP->sz, ST_B_FALSE);
	if (res == ST_ERR_SUCC &&
			st_binobj_getDataSize(&pPP->bindat) != pPP->sz) {
		if (ST_AVFDEB_ISVERBAUD_BD(pAObjI->opts.basOpts))
			st_contOgg_d_deb(&pAObjI->opts, pFnc,
					"correcting partial packet's size: %u-->%u",
					pPP->sz, st_binobj_getDataSize(&pPP->bindat));
		pPP->sz = st_binobj_getDataSize(&pPP->bindat);
	}
	st_binobj_detachStreamrd(&pBSI->packCur.bdata);

	/**st_binobj_copyIntoFile(&pPP->bindat, "pack2_r.tmp");**/

	/* copy lacing values for the 'partial packet' to pPP->pSegTab */
	if (res == ST_ERR_SUCC) {
		Tst_uint32 skipSz = startPos;

		/* skip lacing vals that belong to the old vorb. cmt.
		 *   --> segsX  */
		while (skipSz > 0 && segsX < pBSI->packCur.segsUsd) {
			if (pBSI->packCur.pSegs[segsX] <= skipSz)
				skipSz -= pBSI->packCur.pSegs[segsX++];
			else {
				/**st_contOgg_prf("%s(): segTab[%u]-=%u\n",
						pFnc, segsX, skipSz);**/
				pBSI->packCur.pSegs[segsX] -= skipSz;
				skipSz                      = 0;
			}
		}
		if (skipSz > 0) {
			/**st_contOgg_prf(pFnc"(): skipSz not reached\n");**/
			res = ST_ERR_IDAT;
		}
	}
	if (res == ST_ERR_SUCC) {
		pPP->segsSz = pBSI->packCur.segsUsd + 2;
		ST_CALLOC(pPP->pSegTab, Tst_byte*, pPP->segsSz, 1)
		if (pPP->pSegTab == NULL)
			res = ST_ERR_OMEM;
	}
	if (res == ST_ERR_SUCC) {
		Tst_uint32 tmpPPsz = 0;

		/* copy lacing vals that belong to the 'partial packet'
		 *   --> pPP->pSegTab  */
		while (segsX < pBSI->packCur.segsUsd) {
			pPP->pSegTab[pPP->segsUsd++] = pBSI->packCur.pSegs[segsX];
			tmpPPsz                     += pBSI->packCur.pSegs[segsX];
			++segsX;
		}
		if (tmpPPsz != pPP->sz) {
			/**st_contOgg_prf("%s(): tmpPPsz != szPP: %u != %u\n",
					pFnc, tmpPPsz, pPP->sz);**/
			res = ST_ERR_IDAT;
		}
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Append data from pPP->bindat to pBSI->packCur.bdata
 *   and append the corresponding lacing values
 *   from pPP->pSegTab to pBSI->packCur.pSegs
 */
static Tst_err
ST_CONTOGG__wr_appendPP(const char *pFnc,
		Tst_contOgg_obj_intn *pAObjI,
		Tst_contOgg_substr_intn *pBSI,
		Tst_contOgg__wr_partialPack *pPP)
{
	Tst_err res = ST_ERR_SUCC;

	ST_ASSERTN_IARG(pPP == NULL)

	if (pPP->sz == 0)
		return res;

	/* data */
	res = st_binobj_appendData_binobj(&pBSI->packCur.bdata,
			&pPP->bindat);
	/* lacing values */
	if (res == ST_ERR_SUCC &&
			pBSI->packCur.segsUsd + pPP->segsUsd > pBSI->packCur.segsSz) {
		pBSI->packCur.segsSz = pBSI->packCur.segsUsd + pPP->segsUsd;
		ST_REALLOC(pBSI->packCur.pSegs, Tst_byte*, pBSI->packCur.segsSz, 1)
		if (pBSI->packCur.pSegs == NULL)
			res = ST_ERR_OMEM;
	}
	if (res == ST_ERR_SUCC) {
		memcpy(&pBSI->packCur.pSegs[pBSI->packCur.segsUsd],
				pPP->pSegTab, pPP->segsUsd);
		pBSI->packCur.segsUsd += pPP->segsUsd;
	}

	return res;
}

/******************************************************************************/
