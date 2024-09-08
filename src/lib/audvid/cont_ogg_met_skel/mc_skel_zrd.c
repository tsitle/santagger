/******************************************************************************/
/* mc_skel_zrd.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for reading Ogg-Skeleton                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/sys_fnc.h>
#include <santag/common/sys_math.h>
#include <santag/common/streamrd.h>
#include <santag/audvid/cont_ogg.h>
/** */
#define SRC_MC_SKEL_ZRD_C
#include "mc_skel_zrd-prot.h"
/*** */
#include "mc_skel_zstcs-prot.h"
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_MC_SKEL_ZRD_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Skeleton header packet
 */
Tst_err
st_contOgg_skelrd_parseHPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj,
		Tst_contOgg_cbStripStr cbStripStr)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_uint32 rdBytes = 0;
	Tst_contOgg_skelIdent *pSID = &pBSI->pSkel->ident;

	if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
				"Skeleton header found");

	if (st_streamrd_getAmountRemainingBitsInCurByte(pSObj) != 8)
		return ST_ERR_IDAT;

	/* Version major */
	res = st_streamrd_rdUInt16(pSObj,
			ST_STREAMRD_IEND_LE, 16, &pSID->versMaj);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (pSID->versMaj != 3)  {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"can't handle Skeleton major version %u", pSID->versMaj);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}
	/* Version minor */
	res = st_streamrd_rdUInt16(pSObj,
			ST_STREAMRD_IEND_LE, 16, &pSID->versMin);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("st_contOgg_skelrd_parseHPack(): "
			"vers %u.%u\n", pSID->versMaj, pSID->versMin);**/
	if (pSID->versMin != 0)  {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"can't handle Skeleton minor version %u", pSID->versMin);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}
	/* Presentationtime numerator */
	res = st_streamrd_rdUInt64(pSObj,
			ST_STREAMRD_IEND_LE, 64, &pSID->presTimeNum);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/* Presentationtime denumerator */
	res = st_streamrd_rdUInt64(pSObj,
			ST_STREAMRD_IEND_LE, 64, &pSID->presTimeDen);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/* Basetime numerator */
	res = st_streamrd_rdUInt64(pSObj,
			ST_STREAMRD_IEND_LE, 64, &pSID->baseTimeNum);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/* Basetime denumerator */
	res = st_streamrd_rdUInt64(pSObj,
			ST_STREAMRD_IEND_LE, 64, &pSID->baseTimeDen);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/* UTC */
	res = st_streamrd_rdBuffer_noCRC(pSObj, 20, pSID->utcTime, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != 20) {
		if (res == ST_ERR_SUCC)
			res = ST_ERR_IDAT;
		return res;
	}
	pSID->utcTime[20] = 0;
	cbStripStr(pSID->utcTime, 20);
	/**st_contOgg_prf("st_contOgg_skelrd_parseHPack(): "
			"UTC '%s'\n", pSID->utcTime);**/
	/* */
	pBSI->bsType = ST_CONTOGG_BSTP_SKEL;
	pSID->isSet  = ST_B_TRUE;
	pSID->packNr = pBSI->packCur.nr;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Skeleton bone packet
 */
Tst_err
st_contOgg_skelrd_parseBone(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_fsize  rem;
	Tst_uint32 sz  = 0,
	           usd = 0;
	Tst_byte   x   = 0;
	Tst_contOgg_skelBone *pSBone;

	if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
				"Skeleton bone found");

	if (st_streamrd_getAmountRemainingBitsInCurByte(pSObj) != 8)
		return ST_ERR_IDAT;

	/* add new bone to array */
	if (pBSI->pSkel->bones.cnt + 1 > pBSI->pSkel->bones.sz) {
		res = st_contOgg_stc_reszSkelBoneArr(&pBSI->pSkel->bones,
				pBSI->pSkel->bones.sz + 5);
		if (res != ST_ERR_SUCC)
			return res;
	}
	pSBone = &pBSI->pSkel->bones.pArr[pBSI->pSkel->bones.cnt++];

	/* parse bone */
	/** Offset to message header fields */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSBone->offsMsgHdFld);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("st_contOgg_skelrd_parseBone(): "
			"offsMHF %u\n", pSBone->offsMsgHdFld);**/
	/** Serial number */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSBone->serial);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("st_contOgg_skelrd_parseBone(): "
			"snr 0x%08x\n", pSBone->serial);**/
	/** Number of header packets */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSBone->hdPckCnt);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("st_contOgg_skelrd_parseBone(): "
			"hdPCnt %u\n", pSBone->hdPckCnt);**/
	/** Granulerate numerator */
	res = st_streamrd_rdUInt64(pSObj,
			ST_STREAMRD_IEND_LE, 64, &pSBone->granNum);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/** Granulerate denumerator */
	res = st_streamrd_rdUInt64(pSObj,
			ST_STREAMRD_IEND_LE, 64, &pSBone->granDen);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/** Basegranule */
	res = st_streamrd_rdUInt64(pSObj,
			ST_STREAMRD_IEND_LE, 64, &pSBone->baseGran);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/** Preroll */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pSBone->preroll);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/** Granuleshift */
	res = st_streamrd_rdByte_noCRC(pSObj, 8, &pSBone->granShift);
	if (res != ST_ERR_SUCC)
		return res;
	/** padding */
	st_streamrd_rdSkipBytes(pSObj, 3, ST_B_TRUE);
	/** Message header fields */
	rem = st_streamrd_getAmountRemainingBytes(pSObj);
	if (rem > 0) {
		while (rem > 0) {
			if (rem >= 8 &&
					st_streamrd_rdMemcmpBytes(pSObj, 8, "fisbone\0"))
				break;
			/** */
			if (usd + 1 >= sz) {
				sz += 10;
				ST_REALLOC(pSBone->pMsgHdFlds, Tst_str*, sz, 1)
				if (pSBone->pMsgHdFlds == NULL)
					return ST_ERR_OMEM;
			}
			/** */
			res = st_streamrd_rdByte_noCRC(pSObj, 8, &x);
			if (res != ST_ERR_SUCC)
				return res;
			pSBone->pMsgHdFlds[usd++] = x;
			--rem;
		}
		if (pSBone->pMsgHdFlds != NULL)
			pSBone->pMsgHdFlds[usd] = 0;
		/**st_contOgg_prf("st_contOgg_skelrd_parseBone(): "
				"msgHd sz %u '%s'\n", usd, (char*)pSBone->pMsgHdFlds);**/
	}

	return ST_ERR_SUCC;
}

/******************************************************************************/
