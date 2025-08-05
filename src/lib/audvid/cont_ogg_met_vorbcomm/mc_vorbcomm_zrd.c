/******************************************************************************/
/* mc_vorbcomm_zrd.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading Vorbis comments                                      */
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
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_MC_VORBCOMM_ZRD_C
#include "mc_vorbcomm_zrd-prot.h"
/*** */
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_MC_VORBCOMM_ZRD_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef struct {
	Tst_uint32 vlen;       /* vendor length */
	Tst_uint32 eCnt;       /* element count */
	Tst_uint32 *pELenArr;  /* elements' length array */
	Tst_binobj *pEBOarr;   /* elements' data array */
	Tst_uint32 *pMaxSz;    /* max. amount of bytes to read */
} Tst_contOgg__vorbCommTmp;

static Tst_err
ST_CONTOGG__vorbComm_writeHead(Tst_binobj *pBOout, Tst_uint32 totSize);

static Tst_err
ST_CONTOGG__vorbComm_parseElems(Tst_contOgg_substr_intn *pBSI,
                                Tst_contOgg_vorbCmtRaw_intn *pVCRI,
                                Tst_streamrd *pSObj,
                                Tst_contOgg__vorbCommTmp *pVCtmp);

static Tst_err
ST_CONTOGG__vorbComm_setCommBO(const Tst_contOgg_opts *pOpts,
                               Tst_contOgg_vorbCmtRaw_intn *pVCRI,
                               const Tst_contOgg__vorbCommTmp *pVCtmp);

#if (CONFIG_ST_ALL_DEBUG == 1)
static Tst_bool
ST_CONTOGG__vorbComm_doesDataEndWithNULterm(Tst_binobj *pData);
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read Vorbis Comment
 *   This is used for Vorbis, Flac, Celt, ... streams
 *
 *   The comment will be read from pSObj and written to pVCR.
 *   If the comment must end with a 'framing bit' then
 *   hasFramingBit should be ST_B_TRUE
 *   (this is the case for Vorbis and Celt).
 *   If the comment is limited to a certain amount of bytes
 *   then hasMaxSz must be ST_B_TRUE and maxSz set accordingly
 */
Tst_err
st_contOgg_vorbComm_parse(Tst_contOgg_substr_intn *pBSI, Tst_streamrd *pSObj,
		Tst_contOgg_vorbCommentRaw *pVCR, const Tst_bool hasFramingBit,
		const Tst_bool hasMaxSz, Tst_uint32 maxSz,
		const Tst_int32 srcCmtBegPos)
{
#	define LOC_FREE_VCT_  { ST_DELPOINT(vcTmp.pELenArr) \
				if (vcTmp.pEBOarr != NULL) { \
					for (x = 0; x < pVCRI->elemCnt; x++) \
						st_binobj_stc_freeBO(&vcTmp.pEBOarr[x]); \
					ST_DELPOINT(vcTmp.pEBOarr) \
				} }
	const char       *cFNCN     = __func__;
	const Tst_uint32 cVENMAXLEN = 255;
	Tst_err    res;
	Tst_uint32 vlenT,
	           x,
	           bToRd,
	           rdBytes = 0;
	Tst_bool   shortV  = ST_B_FALSE;
	Tst_byte   frBit   = 0;
	Tst_contOgg__vorbCommTmp    vcTmp;
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_IARG(pVCR == NULL || pVCR->pObInternal == NULL ||
			pBSI == NULL || pSObj == NULL)

	pVCRI = (Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal;

	if (pVCRI->isSet) {
		if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
			st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
					"already read vorbComment, skipping");
		return ST_ERR_IDAT;
	}
	if (st_streamrd_getAmountRemainingBitsInCurByte(pSObj) != 8)
		return ST_ERR_IDAT;

	pVCRI->srcCmtBegPos = srcCmtBegPos;

	/* init some stuff */
	vcTmp.vlen     = 0;
	vcTmp.eCnt     = 0;
	vcTmp.pELenArr = NULL;
	vcTmp.pEBOarr  = NULL;
	vcTmp.pMaxSz   = (hasMaxSz ? &maxSz : NULL);

	/* parse comment */
	/** VENDOR LENGTH */
	if (hasMaxSz && maxSz < 4) {
		if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
			st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
					"invalid vorbComment header size, skipping");
		return ST_ERR_IDAT;
	}
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &vcTmp.vlen);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (hasMaxSz)
		maxSz -= 4;
	/** VENDOR */
	if (vcTmp.vlen > 0) {
		if (hasMaxSz && maxSz < vcTmp.vlen) {
			if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
				st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
						"invalid vorbComment header size, skipping");
			return ST_ERR_IDAT;
		}
		/** */
		vlenT = vcTmp.vlen;
		if (vlenT > cVENMAXLEN) {
			vlenT  = cVENMAXLEN;
			shortV = ST_B_TRUE;
			if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
				st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
						"shortening vendor len %u --> %u",
						vcTmp.vlen, vlenT);
		}
		ST_REALLOC(pVCRI->pVendor, Tst_str*, vlenT + 1, 1)
		if (pVCRI->pVendor == NULL)
			return ST_ERR_OMEM;
		bToRd = vlenT - (shortV ? 5 : 0);
		res   = st_streamrd_rdBuffer_noCRC(pSObj, bToRd,
				pVCRI->pVendor, &rdBytes);
		if (res != ST_ERR_SUCC || rdBytes != bToRd) {
			if (res == ST_ERR_SUCC)
				res = ST_ERR_IDAT;
			return res;
		}
		if (shortV) {
			memcpy(&pVCRI->pVendor[vlenT - 5], " [..]", 5);
			st_streamrd_rdSkipBytes(pSObj, vcTmp.vlen - rdBytes, ST_B_TRUE);
		}
		pVCRI->pVendor[vlenT] = 0x00;
		/**if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
			st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
					"vendor len %u", vlenT);**/
		if (hasMaxSz)
			maxSz -= vcTmp.vlen;
		vcTmp.vlen = vlenT;
	}
	/** ELEMENT COUNT */
	if (hasMaxSz && maxSz < 4) {
		if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
			st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
					"invalid vorbComment header size, skipping");
		return ST_ERR_IDAT;
	}
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_LE, 32, &pVCRI->elemCnt);  /* ENDIAN: LE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (hasMaxSz)
		maxSz -= 4;
	/**if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
		st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
			"elems %u", pVCRI->elemCnt);**/
	/** ELEMENTS */
	if (pVCRI->elemCnt > 0) {
		res = ST_CONTOGG__vorbComm_parseElems(pBSI, pVCRI, pSObj, &vcTmp);
		if (res != ST_ERR_SUCC) {
			LOC_FREE_VCT_
			return res;
		}
		/**if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
			st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
					"%u comments", vcTmp.eCnt);**/
	}
	/** FRAMING BIT */
	if (hasFramingBit) {
		if (hasMaxSz && maxSz < 1) {
			if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
				st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
						"invalid vorbComment header size, skipping");
			LOC_FREE_VCT_
			return ST_ERR_IDAT;
		}
		res = st_streamrd_rdByte_noCRC(pSObj, 8, &frBit);
		if (res != ST_ERR_SUCC) {
			LOC_FREE_VCT_
			return res;
		}
		if ((frBit & 0x01) == 0) {
			if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
				st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
						"framing bit invalid, skipping");
			LOC_FREE_VCT_
			return ST_ERR_IDAT;
		}
	}

	/* copy raw comments to pVCRI->pRawCmtBO,
	 *   a pseudo header will be inserted at the beginning of the comment */
	if (pBSI->pOpts->rdComments && (vcTmp.vlen > 0 || vcTmp.eCnt > 0)) {
		res = ST_CONTOGG__vorbComm_setCommBO(pBSI->pOpts, pVCRI, &vcTmp);
		if (res != ST_ERR_SUCC) {
			LOC_FREE_VCT_
			return res;
		}
	}
	LOC_FREE_VCT_

	/* */
	pVCRI->isSet        = ST_B_TRUE;
	pVCRI->packNr       = pBSI->packCur.nr;
	pVCRI->srcCmtEndPos = (Tst_int32)st_streamrd_getCurPos(pSObj);

	return ST_ERR_SUCC;
#	undef LOC_FREE_VCT_
}

/*----------------------------------------------------------------------------*/

/*
 * Add element (tag field) to Vorbis comment (tag)
 */
Tst_err
st_contOgg_vorbComm_addKey(Tst_contOgg_substr_intn *pBSI,
		Tst_contOgg_vorbCommentRaw *pVCR,
		const char *pKey, Tst_binobj *pData)
{
#	define LOC_FREEBON_  { \
				st_binobj_stc_freeBO(pBOnew); \
				ST_DELPOINT(pBOnew) }
#	define LOC_DET_FREEBON_FREESOLD_  { \
				st_binobj_detachStreamrd(pVCRI->pRawCmtBO); \
				st_streamrd_stc_freeSObj(&strrdOld); \
				LOC_FREEBON_ }
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_buf    *pVend    = NULL;
	Tst_uint32 keyLen    = st_sysStrlen2(pKey),
	           vendLen   = 0,
	           alrCpd    = 0,
	           tmpUI,
	           oldTSz,
	           newDSz,
	           newTSz,
	           rdFromStr = 0;
	Tst_binobj                  *pBOnew = NULL;
	Tst_streamrd                strrdOld;
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_IARG(pVCR == NULL || pVCR->pObInternal == NULL ||
			pKey == NULL || pData == NULL ||
			ST_CONTOGG__vorbComm_doesDataEndWithNULterm(pData))

	pVCRI = (Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal;

	/* old and new comment size excludes size of pseudo header */
	oldTSz  = st_binobj_getDataSize(pVCRI->pRawCmtBO);
	oldTSz -= (oldTSz >= 12 ? 12 : oldTSz);
	newDSz  = st_binobj_getDataSize(pData);
	newTSz  = oldTSz + 4 + keyLen + 1 + newDSz;

	ST_CALLOC(pBOnew, Tst_binobj*, 1, sizeof(Tst_binobj))
	if (pBOnew == NULL)
		return ST_ERR_OMEM;
	st_binobj_stc_initBO(pBOnew);

	st_streamrd_stc_initSObj(&strrdOld);
	if (oldTSz > 0) {
		res = st_binobj_attachStreamrd(pVCRI->pRawCmtBO, &strrdOld, 0);
		if (res != ST_ERR_SUCC) {
			st_streamrd_stc_freeSObj(&strrdOld);
			LOC_FREEBON_
			return res;
		}
	}

	/* Header */
	/** write pseudo header: "VORBCOMM" + TOTAL SIZE, 12 bytes */
	ST_CONTOGG__vorbComm_writeHead(pBOnew, newTSz);
	if (oldTSz > 0)
		st_streamrd_rdSkipBytes(&strrdOld, 8 + 4, ST_B_FALSE);
	/** read/write VENDOR LENGTH, 4 bytes */
	if (oldTSz > 0) {
		res = st_streamrd_rdUInt32(&strrdOld,
				ST_STREAMRD_IEND_LE, 32, &vendLen);  /* ENDIAN: LE-->HOST */
		if (res != ST_ERR_SUCC) {
			LOC_DET_FREEBON_FREESOLD_
			return res;
		}
		alrCpd = 4;
		if (vendLen + alrCpd > oldTSz) {
			/* corrupt data */
			if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
				st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
						"vendor length too big: %u > %u",
						vendLen, oldTSz - alrCpd);
			LOC_DET_FREEBON_FREESOLD_
			return ST_ERR_IDAT;
		}
	}
#	if (WORDS_BIGENDIAN == 1)
	tmpUI = st_sysReverseByteOrder_UI32(vendLen);  /* ENDIAN: BE-->LE */
#	else
	tmpUI = vendLen;
#	endif
	res = st_binobj_appendData(pBOnew, (Tst_buf*)&tmpUI, 4);
	if (res != ST_ERR_SUCC) {
		LOC_DET_FREEBON_FREESOLD_
		return res;
	}
	/** read/write VENDOR, vendLen-bytes */
	if (vendLen > 0) {
		/* */
		ST_CALLOC(pVend, Tst_buf*, vendLen, 1)
		if (pVend == NULL) {
			LOC_DET_FREEBON_FREESOLD_
			return ST_ERR_OMEM;
		}
		res = st_streamrd_rdBuffer_noCRC(&strrdOld, vendLen, pVend, &rdFromStr);
		if (res != ST_ERR_SUCC || rdFromStr != vendLen) {
			LOC_DET_FREEBON_FREESOLD_
			ST_DELPOINT(pVend)
			return res;
		}
		alrCpd += vendLen;
		/* */
		res = st_binobj_appendData(pBOnew, pVend, vendLen);
		if (res != ST_ERR_SUCC) {
			LOC_DET_FREEBON_FREESOLD_
			ST_DELPOINT(pVend)
			return res;
		}
		ST_DELPOINT(pVend)
	}
	/** write ELEMENT COUNT, 4 bytes */
	if (oldTSz > 0) {
		st_streamrd_rdSkipBytes(&strrdOld, 4, ST_B_FALSE);
		alrCpd += 4;
	}
	tmpUI = pVCRI->elemCnt + 1;
#	if (WORDS_BIGENDIAN == 1)
	tmpUI = st_sysReverseByteOrder_UI32(tmpUI);  /* ENDIAN: BE-->LE */
#	endif
	res = st_binobj_appendData(pBOnew, (Tst_buf*)&tmpUI, 4);
	if (res != ST_ERR_SUCC) {
		LOC_DET_FREEBON_FREESOLD_
		return res;
	}

	/* copy old elements */
	if (oldTSz > alrCpd) {
		Tst_buf    cpBuf[2048];
		Tst_uint32 toCp = sizeof(cpBuf);

		while (! st_streamrd_isEOF(&strrdOld) && alrCpd < oldTSz) {
			if (alrCpd + toCp > oldTSz)
				toCp = oldTSz - alrCpd;
			res = st_streamrd_rdBuffer_noCRC(&strrdOld, toCp, cpBuf, &rdFromStr);
			if (res != ST_ERR_SUCC) {
				LOC_DET_FREEBON_FREESOLD_
				return res;
			}
			res = st_binobj_appendData(pBOnew, cpBuf, rdFromStr);
			if (res != ST_ERR_SUCC) {
				LOC_DET_FREEBON_FREESOLD_
				return res;
			}
			alrCpd += rdFromStr;
			if (toCp != sizeof(cpBuf))
				break;
		}
	}
	if (oldTSz > 0)
		st_binobj_detachStreamrd(pVCRI->pRawCmtBO);
	st_streamrd_stc_freeSObj(&strrdOld);

	/* sanity check */
	if (oldTSz > 0 && st_binobj_getDataSize(pBOnew) != oldTSz + 12) {
		LOC_FREEBON_
		return ST_ERR_FAIL;
	}

	/* add new element */
	/** */
	if (ST_AVFDEB_ISVERBAUDDET_BD(pBSI->pOpts->basOpts))
		st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
				"adding key '%s' with len %u",
				pKey, newDSz);
	/**printf("adding key '%s' with len %u\n",
				pKey, newDSz);**/
	/** ELEMENT LENGTH */
	tmpUI = keyLen + 1 + newDSz;
#	if (WORDS_BIGENDIAN == 1)
	tmpUI = st_sysReverseByteOrder_UI32(tmpUI);  /* ENDIAN: BE-->LE */
#	endif
	res = st_binobj_appendData(pBOnew, (Tst_buf*)&tmpUI, 4);
	if (res != ST_ERR_SUCC) {
		LOC_FREEBON_
		return res;
	}
	/** ELEMENT KEY */
	res = st_binobj_appendData(pBOnew, (Tst_buf*)pKey, keyLen);
	if (res != ST_ERR_SUCC) {
		LOC_FREEBON_
		return res;
	}
	res = st_binobj_appendData(pBOnew, (Tst_buf*)"=", 1);
	if (res != ST_ERR_SUCC) {
		LOC_FREEBON_
		return res;
	}
	/** ELEMENT DATA */
	res = st_binobj_appendData_binobj(pBOnew, pData);
	if (res != ST_ERR_SUCC) {
		LOC_FREEBON_
		return res;
	}

	/* replace old comment with new one */
	st_binobj_stc_freeBO(pVCRI->pRawCmtBO);
	ST_DELPOINT(pVCRI->pRawCmtBO)
	pVCRI->pRawCmtBO = pBOnew;

	/* update element count */
	++pVCRI->elemCnt;

	return ST_ERR_SUCC;
#	undef LOC_FREEBON_
#	undef LOC_DET_FREEBON_FREESOLD_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Vorbis comment basics (vendor + element count)
 */
Tst_err
st_contOgg_vorbComm_parseBasics(Tst_contOgg_substr_intn *pBSI,
		Tst_contOgg_vorbCommentRaw *pVCR)
{
	Tst_err    res       = ST_ERR_SUCC;
	Tst_uint32 vlen      = 0,
	           rdFromStr = 0;
	Tst_streamrd                strrd;
	Tst_contOgg_vorbCmtRaw_intn *pVCRI;

	ST_ASSERTN_IARG(pBSI == NULL ||
			pVCR == NULL || pVCR->pObInternal == NULL)

	pVCRI = (Tst_contOgg_vorbCmtRaw_intn*)pVCR->pObInternal;
	st_streamrd_stc_initSObj(&strrd);

	res = st_binobj_attachStreamrd(pVCRI->pRawCmtBO, &strrd, 0);

	if (res == ST_ERR_SUCC) {
		/* skip "VORBCOMM" + TOTAL SIZE */
		st_streamrd_rdSkipBytes(&strrd, 8 + 4, ST_B_FALSE);
	}
	if (res == ST_ERR_SUCC) {
		/* read VENDOR LENGTH */
		res = st_streamrd_rdUInt32(&strrd,
				ST_STREAMRD_IEND_LE, 32, &vlen);  /* ENDIAN: LE-->HOST */
		if (res == ST_ERR_SUCC) {
			ST_REALLOC(pVCRI->pVendor, Tst_str*, vlen + 1, 1)
			if (pVCRI->pVendor == NULL)
				res = ST_ERR_OMEM;
		}
	}
	if (res == ST_ERR_SUCC) {
		/* read VENDOR */
		res = st_streamrd_rdBuffer_noCRC(&strrd,
				vlen, pVCRI->pVendor, &rdFromStr);
		pVCRI->pVendor[vlen] = 0x00;
		/**st_contOgg_prf("%s(): vendor='%s'\n",
				__func__, pVCRI->pVendor);**/
	}
	if (res == ST_ERR_SUCC) {
		/* read ELEMENT COUNT */
		res = st_streamrd_rdUInt32(&strrd,
				ST_STREAMRD_IEND_LE, 32, &pVCRI->elemCnt);  /* ENDIAN: LE-->HOST */
		/**st_contOgg_prf("%s(): eCnt=%u\n",
				__func__, pVCRI->elemCnt);**/
	}

	pVCRI->isSet = (res == ST_ERR_SUCC);
	st_binobj_detachStreamrd(pVCRI->pRawCmtBO);
	st_streamrd_stc_freeSObj(&strrd);
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Vorbis comment elements
 */
static Tst_err
ST_CONTOGG__vorbComm_parseElems(Tst_contOgg_substr_intn *pBSI,
		Tst_contOgg_vorbCmtRaw_intn *pVCRI,
		Tst_streamrd *pSObj, Tst_contOgg__vorbCommTmp *pVCtmp)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_uint32 x,
	           len = 0;

	if (pVCRI->elemCnt > 0 &&
			st_streamrd_getAmountRemainingBitsInCurByte(pSObj) != 8)
		return ST_ERR_IDAT;

	/* init pVCtmp */
	if (pBSI->pOpts->rdComments) {
		ST_CALLOC(pVCtmp->pELenArr, Tst_uint32*,
				pVCRI->elemCnt, sizeof(Tst_uint32))
		if (pVCtmp->pELenArr == NULL)
			return ST_ERR_OMEM;
		ST_CALLOC(pVCtmp->pEBOarr, Tst_binobj*,
				pVCRI->elemCnt, sizeof(Tst_binobj))
		if (pVCtmp->pEBOarr == NULL) {
			ST_DELPOINT(pVCtmp->pELenArr)
			return ST_ERR_OMEM;
		}
		for (x = 0; x < pVCRI->elemCnt; x++) {
			pVCtmp->pELenArr[x] = 0;
			st_binobj_stc_initBO(&pVCtmp->pEBOarr[x]);
		}
	}

	/* read all elements */
	for (x = 0; x < pVCRI->elemCnt; x++) {
		/* ELEMENT LENGTH */
		if (pVCtmp->pMaxSz && *pVCtmp->pMaxSz < 4) {
			if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
				st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
						"invalid vorbComment header size, skipping");
			return ST_ERR_IDAT;
		}
		res = st_streamrd_rdUInt32(pSObj,
				ST_STREAMRD_IEND_LE, 32, &len);  /* ENDIAN: LE-->HOST */
		if (res != ST_ERR_SUCC)
			return res;
		if (pBSI->pOpts->rdComments)
			pVCtmp->pELenArr[pVCtmp->eCnt] = len;
		if (pVCtmp->pMaxSz)
			*pVCtmp->pMaxSz -= 4;
		/* ELEMENT KEY + ELEMENT DATA */
		if (len > 0) {
			if (pVCtmp->pMaxSz && *pVCtmp->pMaxSz < len) {
				if (ST_AVFDEB_ISVERBAUD_BD(pBSI->pOpts->basOpts))
					st_contOgg_d_debBS3(pBSI->pOpts, pBSI, cFNCN,
							"invalid vorbComment header size, skipping");
				return ST_ERR_IDAT;
			}
			if (pBSI->pOpts->rdComments) {
				/* the element's data is actually a non-NUL-term. string */
				res = st_binobj_setData_streamrd(&pVCtmp->pEBOarr[pVCtmp->eCnt],
						pSObj, len, ST_B_TRUE);
				if (res != ST_ERR_SUCC ||
						len != st_binobj_getDataSize(&pVCtmp->pEBOarr[pVCtmp->eCnt])) {
					if (res == ST_ERR_SUCC)
						res = ST_ERR_IDAT;
					return res;
				}
			} else
				st_streamrd_rdSkipBytes(pSObj, len, ST_B_TRUE);
			if (pVCtmp->pMaxSz)
				*pVCtmp->pMaxSz -= len;
		}
		++pVCtmp->eCnt;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Write Pseudo Vorbis Header to binary object
 */
static Tst_err
ST_CONTOGG__vorbComm_writeHead(Tst_binobj *pBOout, Tst_uint32 totSize)
{
	Tst_err res;

	res = st_binobj_appendData(pBOout, (Tst_buf*)"VORBCOMM", 8);
	if (res != ST_ERR_SUCC)
		return res;

#	if (WORDS_BIGENDIAN == 1)
	totSize = st_sysReverseByteOrder_UI32(totSize);  /* ENDIAN: BE-->LE */
#	endif

	return st_binobj_appendData(pBOout, (Tst_buf*)&totSize, 4);;
}

/*
 * Write Vorbis comments (tag) to binary object
 */
static Tst_err
ST_CONTOGG__vorbComm_setCommBO(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_vorbCmtRaw_intn *pVCRI,
		const Tst_contOgg__vorbCommTmp *pVCtmp)
{
	Tst_err    res;
	Tst_uint32 newTSz,
	           x,
	           tmpUI;

	newTSz = 4 + pVCtmp->vlen + 4;
	for (x = 0; x < pVCtmp->eCnt; x++)
		newTSz += 4 + pVCtmp->pELenArr[x];

	/* Header */
	/** write pseudo header: "VORBCOMM" + TOTAL SIZE, 12 bytes */
	ST_CONTOGG__vorbComm_writeHead(pVCRI->pRawCmtBO, newTSz);
	/** write VENDOR LENGTH, 4 bytes */
	tmpUI = pVCtmp->vlen;
#	if (WORDS_BIGENDIAN == 1)
	tmpUI = st_sysReverseByteOrder_UI32(tmpUI);  /* ENDIAN: BE-->LE */
#	endif
	res = st_binobj_appendData(pVCRI->pRawCmtBO, (Tst_buf*)&tmpUI, 4);
	if (res != ST_ERR_SUCC)
		return res;
	/** write VENDOR, pVCtmp->vlen-bytes */
	res = st_binobj_appendData(pVCRI->pRawCmtBO,
			(Tst_buf*)pVCRI->pVendor, pVCtmp->vlen);
	if (res != ST_ERR_SUCC)
		return res;
	/** write ELEMENT COUNT, 4 bytes */
	tmpUI = pVCtmp->eCnt;
#	if (WORDS_BIGENDIAN == 1)
	tmpUI = st_sysReverseByteOrder_UI32(tmpUI);  /* ENDIAN: BE-->LE */
#	endif
	res = st_binobj_appendData(pVCRI->pRawCmtBO, (Tst_buf*)&tmpUI, 4);
	if (res != ST_ERR_SUCC)
		return res;

	/* elements */
	for (x = 0; x < pVCtmp->eCnt; x++) {
		/* ELEMENT LENGTH */
		tmpUI = pVCtmp->pELenArr[x];
#		if (WORDS_BIGENDIAN == 1)
		tmpUI = st_sysReverseByteOrder_UI32(tmpUI);  /* ENDIAN: BE-->LE */
#		endif
		res = st_binobj_appendData(pVCRI->pRawCmtBO, (Tst_buf*)&tmpUI, 4);
		if (res != ST_ERR_SUCC)
			return res;

		/* ELEMENT KEY + ELEMENT DATA */
		res = st_binobj_appendData_binobj(pVCRI->pRawCmtBO, &pVCtmp->pEBOarr[x]);
		if (res != ST_ERR_SUCC)
			return res;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

#if (CONFIG_ST_ALL_DEBUG == 1)
static Tst_bool
ST_CONTOGG__vorbComm_doesDataEndWithNULterm(Tst_binobj *pData)
{
	Tst_bool   resB = ST_B_FALSE;
	Tst_uint32 dsz  = st_binobj_getDataSize(pData);
	Tst_byte   byt  = 0;
	Tst_streamrd strrd;

	if (dsz == 0)
		return ST_B_FALSE;
	st_streamrd_stc_initSObj(&strrd);
	if (st_binobj_attachStreamrd(pData, &strrd, dsz - 1) != ST_ERR_SUCC) {
		st_streamrd_stc_freeSObj(&strrd);
		return ST_B_FALSE;
	}

	if (st_streamrd_rdByte_noCRC(&strrd, 8, &byt) == ST_ERR_SUCC)
		resB = (byt == 0x00);

	st_binobj_detachStreamrd(pData);
	st_streamrd_stc_freeSObj(&strrd);
	return resB;
}
#endif

/******************************************************************************/
