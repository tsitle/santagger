/******************************************************************************/
/* ac_flac_zrdh.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading (Ogg-)Flac header packets                            */
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
#include "src/includes/common/sys_math.h"
#include "src/includes/utils/utils_fmt.h"
/** */
#define SRC_AC_FLAC_ZRDH_C
#include "ac_flac_zrdh-pp.h"
/*** */
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../cont_ogg_met_vorbcomm/mc_vorbcomm_zrd-prot.h"
#include "../av_zfdeb-prot.h"
/*** */
#include "../../utils/w-base64-prot.h"
#undef SRC_AC_FLAC_ZRDH_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcmp() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata
 */
Tst_err
st_contOgg_flacrdh_parseH_MD(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh)
{
	const char *cFNCN = __func__;
	Tst_err    res    = ST_ERR_SUCC;
	Tst_bool   isLast = ST_B_FALSE;
	Tst_uint32 mdSz   = 0;
	Tst_contOgg_flacBlockType bt = ST_CONTOGG_FLAC_BT_NONE;

	/* read blockheader */
	res = st_contOgg_flacrdh_parseBH(pDPh->pSObj, &bt, &isLast, &mdSz);
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseH_MD(): "
			"bt %d, isLst %d, mdSz %u (rem %u)\n",
			bt, isLast, mdSz, pPDend - *ppPDat);**/

	/* read metadata */
	switch (bt) {
	case ST_CONTOGG_FLAC_BT_STRINF:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
					"Flac ident header found");
		pDPh->pBSI->pFlac->commIsLastHdPck = ST_B_FALSE;
		/* */
		res = ST_CONTOGG__flacrdh_parseStrInf(pOpts, pDPh, mdSz);
		break;
	case ST_CONTOGG_FLAC_BT_PADD:
		/* padding is simply mdSz times 0x00 bytes */
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
					"Flac padding found: %u bytes", mdSz);
		pDPh->pBSI->pFlac->commIsLastHdPck = ST_B_FALSE;
		/* */
		st_streamrd_rdSkipBytes(pDPh->pSObj, mdSz, ST_B_TRUE);
		break;
	case ST_CONTOGG_FLAC_BT_APP:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
					"Flac app header found");
		pDPh->pBSI->pFlac->commIsLastHdPck = ST_B_FALSE;
		/* */
		res = ST_CONTOGG__flacrdh_parseApp(pOpts, pDPh, mdSz);
		break;
	case ST_CONTOGG_FLAC_BT_SEEK:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
					"Flac seek header found");
		pDPh->pBSI->pFlac->commIsLastHdPck = ST_B_FALSE;
		/* */
		res = ST_CONTOGG__flacrdh_parseSeek(pOpts, pDPh, mdSz);
		break;
	case ST_CONTOGG_FLAC_BT_COMM:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
					"Flac comment header found");
		pDPh->pBSI->pFlac->commIsLastHdPck = ST_B_TRUE;
		/* */
		res = ST_CONTOGG__flacrdh_parseComm(pOpts, pDPh, mdSz);
		break;
	case ST_CONTOGG_FLAC_BT_CUE:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
					"Flac cue header found");
		pDPh->pBSI->pFlac->commIsLastHdPck = ST_B_FALSE;
		/* */
		res = ST_CONTOGG__flacrdh_parseCue(pOpts, pDPh, mdSz);
		break;
	case ST_CONTOGG_FLAC_BT_PICT:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
					"Flac picture header found");
		pDPh->pBSI->pFlac->commIsLastHdPck = ST_B_FALSE;
		/* */
		res = ST_CONTOGG__flacrdh_parsePict(pOpts, pDPh, mdSz);
		break;
	default:
		res = ST_ERR_IDAT;
	}
	if (res == ST_ERR_IDAT)
		++pDPh->pBSI->errorCnt;
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata - Blockheader
 */
Tst_err
st_contOgg_flacrdh_parseBH(Tst_streamrd *pSObj,
		Tst_contOgg_flacBlockType *pBlockType,
		Tst_bool *pIsLast, Tst_uint32 *pMDsz)
{
	Tst_err  res;
	Tst_byte bt = 0;

	res = st_streamrd_rdByte(pSObj, 8, &bt);
	if (res != ST_ERR_SUCC)
		return res;
	*pIsLast    = ((bt & 0x80) == 0x80);
	*pBlockType = (Tst_contOgg_flacBlockType)(bt & 0x7f);
	if (*pBlockType >= ST_CONTOGG_FLAC_BT_NONE)
		return ST_ERR_IDAT;
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 24, pMDsz);  /* ENDIAN: BE-->HOST */
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata - StreamInfo
 */
static Tst_err
ST_CONTOGG__flacrdh_parseStrInf(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh, const Tst_uint32 mdSz)
{
#	define LOC_FNCN_  __func__
	Tst_err    res;
	Tst_uint16 shVal   = 0;
	Tst_uint32 rdBytes = 0;
	Tst_contOgg_flacIdent *pFID = &pDPh->pBSI->pFlac->ident;

	if (mdSz != 34) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, LOC_FNCN_,
					"invalid header size");
		return ST_ERR_IDAT;
	}

	/* */
	/** MIN BLOCKSIZE in samples, 16 bits */
	res = st_streamrd_rdUInt16(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 16, &shVal);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	pFID->blkSzMin = shVal;
	/** MAX BLOCKSIZE in samples, 16 bits */
	res = st_streamrd_rdUInt16(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 16, &shVal);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	pFID->blkSzMax = shVal;
	if (pFID->blkSzMin < 1 || pFID->blkSzMax < 1)
		return ST_ERR_IDAT;
	/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseStrInf(): min/max blksz %u/%u\n",
			pFID->blkSzMin, pFID->blkSzMax);**/

	/** MIN FRAME SIZE, 24 bits (16+8) */
	res = st_streamrd_rdUInt32(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 24, &pFID->frSzMin);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/** MAX FRAME SIZE, 24 bits (16+8) */
	res = st_streamrd_rdUInt32(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 24, &pFID->frSzMax);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseStrInf(): min/max frsz %u/%u\n",
			pFID->frSzMin, pFID->frSzMax);**/

	/** SAMPLERATE, 20 bits (16+4) */
	res = st_streamrd_rdUInt32(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 20, &pFID->sampleR);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseStrInf(): samplerate %u\n",
			pFID->sampleR);**/
	if (pFID->sampleR < 1) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, LOC_FNCN_,
					"invalid samplerate found");
		return ST_ERR_IDAT;
	}

	/** NUMBER OF CHANNELS, 3 bits */
	res = st_streamrd_rdByte(pDPh->pSObj, 3, &pFID->chann);
	if (res != ST_ERR_SUCC)
		return res;
	++pFID->chann;
	/** BITS PER SAMPLE, 5 bits (1+4) */
	res = st_streamrd_rdByte(pDPh->pSObj, 5, &pFID->bitsPerS);
	if (res != ST_ERR_SUCC)
		return res;
	++pFID->bitsPerS;
	/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseStrInf(): chann %u, bitsPs %u\n",
			pFID->chann, pFID->bitsPerS);**/
	if (pFID->chann < 1 || pFID->chann > 8) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, LOC_FNCN_,
					"invalid amount of channels found: %u", pFID->chann);
		return ST_ERR_IDAT;
	}
	if (pFID->bitsPerS < 4 || pFID->bitsPerS > 32) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pDPh->pBSI, LOC_FNCN_,
					"invalid amount of bitsPerSample found: %u",
					pFID->bitsPerS);
		return ST_ERR_IDAT;
	}

	/** TOTAL SAMPLES IN STREAM, 36 bits (4+32) */
	res = st_streamrd_rdUInt64(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 36, &pFID->samplesTot_fromHd);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;

	/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseStrInf(): samples 0x%02x%08x\n",
				pFID->samplesTot_fromHd.hiU, pFID->samplesTot_fromHd.loU);**/

	/** MD5, 16*8 bits */
	if (st_streamrd_getAmountRemainingBitsInCurByte(pDPh->pSObj) != 8)
		return ST_ERR_IDAT;
	res = st_streamrd_rdBuffer(pDPh->pSObj, 16, pFID->md5_a, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != 16) {
		if (res == ST_ERR_SUCC)
			res = ST_ERR_IDAT;
		return res;
	}
	/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseStrInf(): md5 0x");
	st_contOgg_prBuf((Tst_buf*)&pFID->md5_a,
			sizeof(pFID->md5_a)); st_contOgg_prf("\n");**/

	/* if MD5 sum is all zeros, don't compare sums */
	for (shVal = 0; shVal < sizeof(pFID->md5_a); shVal++)
		if (pFID->md5_a[shVal] != 0) {
			pFID->isMD5set = ST_B_TRUE;
			break;
		}

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pDPh->pBSI, LOC_FNCN_,
				"\n\t\t\tblkSz in samples min %u / max %u,\n\t\t\t"
				"frameSz min %u / max %u,\n\t\t\t"
				"sampleR %u, channels %u",
				pFID->blkSzMin, pFID->blkSzMax,
				pFID->frSzMin, pFID->frSzMax,
				pFID->sampleR, pFID->chann);
	return ST_ERR_SUCC;
#	undef LOC_FNCN_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata - Comment
 */
static Tst_err
ST_CONTOGG__flacrdh_parseComm(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh, const Tst_uint32 mdSz)
{
	if (st_streamrd_getAmountRemainingBitsInCurByte(pDPh->pSObj) != 8)
		return ST_ERR_IDAT;

	return st_contOgg_vorbComm_parse(pDPh->pBSI, pDPh->pSObj,
			&pDPh->pBSI->pFlac->comm,
			/*hasFramingBit:*/ST_B_FALSE, /*hasMaxSz:*/ST_B_TRUE,
			/*maxSz:*/mdSz,
			/*srcCmtBegPos:*/(Tst_int32)st_streamrd_getCurPos(pDPh->pSObj) - 4);
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata - Picture
 */
static Tst_err
ST_CONTOGG__flacrdh_parsePict(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh, const Tst_uint32 mdSz)
{
	Tst_err    res;
	Tst_binobj boIn;
	Tst_binobj boOut;

	if (st_streamrd_getAmountRemainingBitsInCurByte(pDPh->pSObj) != 8)
		return ST_ERR_IDAT;

	if (pOpts->rdComments) {
		st_binobj_stc_initBO(&boIn);

		res = st_binobj_setData_streamrd(&boIn, pDPh->pSObj, mdSz, ST_B_TRUE);
		if (res != ST_ERR_SUCC) {
			st_binobj_stc_freeBO(&boIn);
			return res;
		}

		st_binobj_stc_initBO(&boOut);

		res = st_base64_encode_binobj(&boIn, /*withNULterm:*/ST_B_FALSE, &boOut);
		st_binobj_stc_freeBO(&boIn);

		if (res == ST_ERR_SUCC)
			res = st_contOgg_vorbComm_addKey(pDPh->pBSI,
					&pDPh->pBSI->pFlac->comm,
					"METADATA_BLOCK_PICTURE", &boOut);

		st_binobj_stc_freeBO(&boOut);
	} else {
		/* skip picture data */
		st_streamrd_rdSkipBytes(pDPh->pSObj, mdSz, ST_B_TRUE);
		res = ST_ERR_SUCC;
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata - Application
 */
static Tst_err
ST_CONTOGG__flacrdh_parseApp(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh, Tst_uint32 mdSz)
{
	#define LOC_FNCN_  __func__
	#if (WORDS_BIGENDIAN != 1)
		#define LOC_APPUI32_CVEND_(mac_ui32)  \
					mac_ui32 = st_sysReverseByteOrder_UI32(mac_ui32);  /* ENDIAN: LE-->BE */
	#else
		#define LOC_APPUI32_CVEND_(mac_ui32)  /* empty */
	#endif
	#define LOC_APPUI32_(mac_ui32Val)  { \
				ui32 = (Tst_uint32)(mac_ui32Val); \
				LOC_APPUI32_CVEND_(ui32) \
				res = st_binobj_appendData(&binDatRaw, \
						(const Tst_buf*)&ui32, 4); \
			}
	Tst_err    res;
	Tst_str    aid[5]  = {0, 0, 0, 0, 0};
	Tst_str    *pRdFn  = NULL;
	Tst_uint32 rdBytes = 0;
	Tst_bool   isPic;
	Tst_utilsFmt_mimeTp picMimTp = ST_UTILSFMT_MTP_NONE;

	if (mdSz < 4)
		return ST_ERR_IDAT;

	/** APP ID, 32 bits */
	res = st_streamrd_rdBuffer(pDPh->pSObj, 4, aid, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != 4) {
		if (res == ST_ERR_SUCC)
			res = ST_ERR_IDAT;
		return res;
	}
	mdSz -= 4;

	if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pDPh->pBSI, LOC_FNCN_,
				"appID '%s', %u bytes of data", aid, mdSz);

	isPic = (memcmp(aid, "imag", 4) == 0);
	if (isPic) {
		Tst_str    *pRdMimeS = NULL;
		Tst_byte   byt       = 0;
		Tst_uint32 tmpSLen;

		/* skip unknown data */
		st_streamrd_rdSkipBytes(pDPh->pSObj, 4, ST_B_TRUE);
		mdSz -= 4;
		/* read MIME string */
		res = st_streamrd_rdString(pDPh->pSObj, 1024, &pRdMimeS);
		if (res == ST_ERR_SUCC) {
			tmpSLen = st_sysStrlen(pRdMimeS);
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS1(pOpts, pDPh->pBSI, LOC_FNCN_,
						"read MIME '%s' (len %u)", pRdMimeS, tmpSLen);**/
			mdSz -= (mdSz >= tmpSLen + 1 ? tmpSLen + 1 : mdSz);
		}
		ST_DELPOINT(pRdMimeS)
		/* skip zeros */
		while (res == ST_ERR_SUCC && mdSz > 0) {
			res = st_streamrd_rdAheadByte(pDPh->pSObj, 8, &byt);
			if (res != ST_ERR_SUCC || byt != 0)
				break;
			st_streamrd_rdSkipBytes(pDPh->pSObj, 1, ST_B_TRUE);
			--mdSz;
		}
		/* read filename/description */
		if (res == ST_ERR_SUCC && mdSz > 0)
			res = st_streamrd_rdString(pDPh->pSObj, 1024, &pRdFn);
		if (res == ST_ERR_SUCC) {
			tmpSLen = st_sysStrlen(pRdFn);
			/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS1(pOpts, pDPh->pBSI, LOC_FNCN_,
						"read filen/desc '%s' (len %u)", pRdFn, tmpSLen);**/
			mdSz -= (mdSz >= tmpSLen + 1 ? tmpSLen + 1 : mdSz);
		}
		/* skip zeros */
		while (res == ST_ERR_SUCC && mdSz > 0) {
			res = st_streamrd_rdAheadByte(pDPh->pSObj, 8, &byt);
			if (res != ST_ERR_SUCC || byt != 0)
				break;
			st_streamrd_rdSkipBytes(pDPh->pSObj, 1, ST_B_TRUE);
			--mdSz;
		}
		/* read data size */
		if (res == ST_ERR_SUCC) {
			if (mdSz < 5)
				isPic = ST_B_FALSE;
			else {
				res = st_streamrd_rdUInt32(pDPh->pSObj,
						ST_STREAMRD_IEND_LE, 32, &tmpSLen);
				if (res == ST_ERR_SUCC) {
					mdSz -= 4;
					/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
						st_contOgg_d_debBS1(pOpts, pDPh->pBSI, LOC_FNCN_,
								"read dataSz %u (leftOver %u)",
								tmpSLen, mdSz);**/
				}
			}
		}
	}
	if (res == ST_ERR_SUCC && isPic) {
		picMimTp = st_utilsFmt_getMime_enum_byStream_pic(pDPh->pSObj);
		isPic    = (picMimTp != ST_UTILSFMT_MTP_NONE);
		if (! isPic && ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS1(pOpts, pDPh->pBSI, LOC_FNCN_,
					"unknown picture format found, skipping data");
	}
	if (res == ST_ERR_SUCC && isPic) {
		Tst_uint32    ui32,
		              dlen;
		Tst_str const *pMimeS = NULL;
		Tst_binobj binDatRaw;

		st_binobj_stc_initBO(&binDatRaw);

		/* write dummy picture info to binDatRaw */
		/** PICTURE TYPE */
		LOC_APPUI32_(0)  /* Picture Type: Other */
		/** MIME LENGTH */
		if (res == ST_ERR_SUCC) {
			Tst_uint32 mlen;

			pMimeS = st_utilsFmt_getMime_defMime_byEnum(picMimTp);
			mlen   = st_sysStrlen(pMimeS);
			LOC_APPUI32_(mlen)
			/** MIME, ASCII-encoded */
			if (res == ST_ERR_SUCC && mlen > 0)
				res = st_binobj_appendData(&binDatRaw,
						(const Tst_buf*)pMimeS, mlen);
		}
		/** DESC LENGTH */
		if (res == ST_ERR_SUCC) {
			dlen = st_sysStrlen(pRdFn);
			LOC_APPUI32_(dlen)
			if (res == ST_ERR_SUCC && dlen > 0) {
				/** DESCRIPTION, UTF8-encoded */
				res = st_binobj_appendData(&binDatRaw,
						(const Tst_buf*)pRdFn, dlen);
			}
		}
		/** IMAGE WIDTH, HEIGHT, COLOR DEPTH */
		if (res == ST_ERR_SUCC) {
			LOC_APPUI32_(0)
			LOC_APPUI32_(0)
			LOC_APPUI32_(0)
		}
		/** NUMBER OF COLORS USED */
		if (res == ST_ERR_SUCC)
			LOC_APPUI32_(0)
		/* append data from stream to binDatRaw */
		/** BIN LENGTH */
		if (res == ST_ERR_SUCC)
			LOC_APPUI32_(mdSz)
		/** BINARY DATA */
		if (res == ST_ERR_SUCC && mdSz)
			res = st_binobj_appendData_streamrd(&binDatRaw, pDPh->pSObj,
					mdSz, ST_B_TRUE);
		/* convert picture stuff to Base64 and add it to Vorbis Comment */
		if (res == ST_ERR_SUCC) {
			Tst_binobj binDatB64;

			if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
				st_contOgg_d_debBS1(pOpts, pDPh->pBSI, LOC_FNCN_,
						"adding picture to Vorbis Comment");
			st_binobj_stc_initBO(&binDatB64);

			res = st_base64_encode_binobj(&binDatRaw,
					/*withNULterm:*/ST_B_FALSE, &binDatB64);

			if (res == ST_ERR_SUCC)
				res = st_contOgg_vorbComm_addKey(pDPh->pBSI,
						&pDPh->pBSI->pFlac->comm,
						"METADATA_BLOCK_PICTURE", &binDatB64);

			st_binobj_stc_freeBO(&binDatB64);
		}
		/** */
		st_binobj_stc_freeBO(&binDatRaw);
	} else if (res == ST_ERR_SUCC)
		st_streamrd_rdSkipBytes(pDPh->pSObj, mdSz, ST_B_TRUE);

	ST_DELPOINT(pRdFn)
	return res;
	#undef LOC_FNCN_
	#undef LOC_APPUI32_CVEND_
	#undef LOC_APPUI32_
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata - Seektable
 */
static Tst_err
ST_CONTOGG__flacrdh_parseSeek(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh, Tst_uint32 mdSz)
{
	Tst_err    res;
	Tst_uint64 ui64;
	Tst_uint16 sh = 0;
	Tst_bool   skip;

	ST_SYSMATH_STC_RSETUI64(ui64)
	while (mdSz >= 8 + 8 + 2) {
		/** 1st SAMPLE NUMBER */
		res = st_streamrd_rdUInt64(pDPh->pSObj,
				ST_STREAMRD_IEND_BE, 64, &ui64);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC)
			return res;
		mdSz -= 8;

		skip = (st_sysUInt64_cmpLoHi(&ui64, (Tst_uint32)-1, (Tst_uint32)-1) == 0);
		/**st_contOgg_prf64("\nOGG__parseFlacH_MD_seek(): 1stSampNr", &ui64);**/

		/** OFFSET */
		if (skip)
			res = st_streamrd_rdSkipBytes(pDPh->pSObj, 8, ST_B_TRUE);
		else {
			res = st_streamrd_rdUInt64(pDPh->pSObj,
					ST_STREAMRD_IEND_BE, 64, &ui64);  /* ENDIAN: BE-->HOST */
			/**st_contOgg_prf64("ST_CONTOGG__flacrdh_parseSeek(): offset", &ui64);**/
		}
		if (res != ST_ERR_SUCC)
			return res;
		mdSz -= 8;

		/** SAMPLES IN TARGET FRAME */
		if (skip)
			res = st_streamrd_rdSkipBytes(pDPh->pSObj, 2, ST_B_TRUE);
		else {
			res = st_streamrd_rdUInt16(pDPh->pSObj,
					ST_STREAMRD_IEND_BE, 16, &sh);  /* ENDIAN: BE-->HOST */
			/**st_contOgg_prf("ST_CONTOGG__flacrdh_parseSeek(): sampCnt %u\n", sh);**/
		}
		if (res != ST_ERR_SUCC)
			return res;
		mdSz -= 2;
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Flac Metadata - Cuesheet
 */
static Tst_err
ST_CONTOGG__flacrdh_parseCue(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh, const Tst_uint32 mdSz)
{
	#define LOC_DEBMSGSZ_TMP_  256
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_str    medCatNr[128 + 1];
	Tst_bool   isCDDA;
	Tst_byte   trkCnt   = 0;
	Tst_byte   x        = 0;
	Tst_uint64 ui64;
	Tst_uint32 rdBytes  = 0;
	Tst_str    decUI[50];
	Tst_str    debMsgTmp[LOC_DEBMSGSZ_TMP_];
	Tst_str    *pDebMsgOut = NULL;

	if (st_streamrd_getAmountRemainingBitsInCurByte(pDPh->pSObj) != 8) {
		return ST_ERR_IDAT;
	}

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		ST_CALLOC(pDebMsgOut, Tst_str*, LOC_DEBMSGSZ_TMP_, 1);
		if (pDebMsgOut == NULL) {
			return ST_ERR_OMEM;
		}
		pDebMsgOut[0] = 0x00;
	}

	/** MEDIA CATALOG NUMBER, 128*8 bits */
	res = st_streamrd_rdBuffer(pDPh->pSObj, 128, medCatNr, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != 128) {
		if (res == ST_ERR_SUCC) {
			res = ST_ERR_IDAT;
		}
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	medCatNr[sizeof(medCatNr) - 1] = 0;
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "CUE media catalog number='%s'",
				medCatNr);
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	/** NUMBER OF LEAD-IN SAMPLES, 64 bits */
	ST_SYSMATH_STC_RSETUI64(ui64)
	res = st_streamrd_rdUInt64(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 64, &ui64);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		st_sysUInt64_toDecStr(&ui64, decUI, sizeof(decUI));
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\tCUE lead-in samples=%s",
				decUI);
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	/** does CUESHEET correspond to a Compact Disc ?, 1 bits */
	res = st_streamrd_rdByte(pDPh->pSObj, 1, &x);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	isCDDA = (x == 1);
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\tCUE is CompactDisc=%s",
				isCDDA ? "yes" : "no");
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	/** RESERVED, 7+258*8 bits */
	st_streamrd_rdSkipBits(pDPh->pSObj, 7 + 258 * 8, ST_B_TRUE);

	/** TRACKCOUNT, 8 bits */
	res = st_streamrd_rdByte(pDPh->pSObj, 8, &trkCnt);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\tCUE track count=%u",
				trkCnt);
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
		//
		st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
				"\n\t\t%s", pDebMsgOut);
	}

	/** CUESHEET_TRACKs */
	for (x = 0; x < trkCnt; x++) {
		res = ST_CONTOGG__flacrdh_parseCue_track(pOpts, pDPh, x, isCDDA);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	ST_DELPOINT(pDebMsgOut);

	return ST_ERR_SUCC;
	#undef LOC_DEBMSGSZ_TMP_
}

/*
 * Parse Flac Metadata - Cuesheet Track
 */
static Tst_err
ST_CONTOGG__flacrdh_parseCue_track(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_codFlac_defParsH *pDPh,
		const Tst_byte curTrkNr, const Tst_bool isCDDA)
{
	#define LOC_DEBMSGSZ_TMP_  256
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_byte   trkNr    = 0;
	Tst_byte   trkIxCnt = 0;
	Tst_byte   trkTp    = 0;
	Tst_byte   trkEmph  = 0;
	Tst_byte   ixPnt    = 0;
	Tst_byte   x        = 0;
	Tst_uint64 ui64;
	Tst_uint32 rdBytes  = 0;
	Tst_str    decUI[50];
	char       trkISRC[20];
	Tst_str    debMsgTmp[LOC_DEBMSGSZ_TMP_];
	Tst_str    *pDebMsgOut = NULL;

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		ST_CALLOC(pDebMsgOut, Tst_str*, LOC_DEBMSGSZ_TMP_, 1);
		if (pDebMsgOut == NULL) {
			return ST_ERR_OMEM;
		}
		pDebMsgOut[0] = 0x00;
	}

	/** TRACK OFFSET in samples, 8*8 bits */
	ST_SYSMATH_STC_RSETUI64(ui64)
	res = st_streamrd_rdUInt64(pDPh->pSObj,
			ST_STREAMRD_IEND_BE, 64, &ui64);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		decUI[0] = 0;
		st_sysUInt64_toDecStr(&ui64, decUI, sizeof(decUI));
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "CUE track[%u]\n\t\t\toffset=%s",
				curTrkNr, decUI);
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	/** TRACK NUMBER, 8 bits */
	res = st_streamrd_rdByte(pDPh->pSObj, 8, &trkNr);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\t\ttrack nr=%u%s",
				trkNr,
				(trkNr == 170 || trkNr == 255 ? " (LEAD_OUT)" : ""));
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	/** TRACK ISRC, 12*8 bits */
	res = st_streamrd_rdBuffer(pDPh->pSObj, 12, (Tst_buf*)trkISRC, &rdBytes);
	if (res != ST_ERR_SUCC || rdBytes != 12) {
		if (res == ST_ERR_SUCC) {
			res = ST_ERR_IDAT;
		}
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	trkISRC[12] = 0;
	if (trkNr < 170 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\t\ttrack ISRC='%s'",
				trkISRC);
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	/** TRACK TYPE and PRE-EMPHASIS FLAG, 2 bits */
	st_streamrd_rdByte(pDPh->pSObj, 1, &trkTp);
	res = st_streamrd_rdByte(pDPh->pSObj, 1, &trkEmph);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pDebMsgOut);
		return res;
	}
	if (trkNr < 170 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_,
				"\n\t\t\ttrack type=%saudio, pre-emphasis=%s",
				(trkTp == 0 ? "" : "non-"), (trkEmph == 0 ? "no" : "yes"));
		//
		res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
	}

	/** RESERVED, 6+13*8 bits */
	st_streamrd_rdSkipBits(pDPh->pSObj, 6 + 13 * 8, ST_B_TRUE);

	/** NUMBER OF TRACK INDEX POINTS, 8 bits */
	res = st_streamrd_rdByte(pDPh->pSObj, 8, &trkIxCnt);
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(pDebMsgOut);
		return res;
	}

	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		if (trkNr < 170) {
			snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\t\ttrack index points=%u",
					trkIxCnt);
			//
			res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
			if (res != ST_ERR_SUCC) {
				ST_DELPOINT(pDebMsgOut);
				return res;
			}
		}
		st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
				"\n\t\t%s", pDebMsgOut);
	}

	// start a new debug output string
	if (ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		pDebMsgOut[0] = 0x00;
	}

	/** CUESHEET_TRACK_INDEX */
	if (trkNr < 170 && trkIxCnt > 0 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\t\t");
	}

	for (x = 0; x < trkIxCnt; x++) {
		if (trkNr < 170 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
			snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "CUE track index[%u]",
					x);
			//
			res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
			if (res != ST_ERR_SUCC) {
				ST_DELPOINT(pDebMsgOut);
				return res;
			}
		}

		/** OFFSET in samples, relative to the track offset,
		 **   of the index point, 8*8 bits */
		ST_SYSMATH_STC_RSETUI64(ui64)
		res = st_streamrd_rdUInt64(pDPh->pSObj,
				ST_STREAMRD_IEND_BE, 64, &ui64);  /* ENDIAN: BE-->HOST */
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
		if (trkNr < 170 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
			st_sysUInt64_toDecStr(&ui64, decUI, sizeof(decUI));
			snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\t\t\toffset=%s",
					decUI);
			//
			res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
			if (res != ST_ERR_SUCC) {
				ST_DELPOINT(pDebMsgOut);
				return res;
			}
		}

		/** INDEX POINT NUMBER, 8 bits */
		res = st_streamrd_rdByte(pDPh->pSObj, 8, &ixPnt);
		if (res != ST_ERR_SUCC) {
			ST_DELPOINT(pDebMsgOut);
			return res;
		}
		if (trkNr < 170 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
			snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "\n\t\t\t\tnumber=%u",
					ixPnt);
			//
			res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
			if (res != ST_ERR_SUCC) {
				ST_DELPOINT(pDebMsgOut);
				return res;
			}
		}

		/** RESERVED, 3*8 bits */
		st_streamrd_rdSkipBytes(pDPh->pSObj, 3, ST_B_TRUE);

		if (trkNr < 170 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
			snprintf((char*)debMsgTmp, LOC_DEBMSGSZ_TMP_, "%s",
					x + 1 < trkIxCnt ? "\n\t\t\t" : "");
			//
			res = st_sysStrapp(debMsgTmp, &pDebMsgOut);
			if (res != ST_ERR_SUCC) {
				ST_DELPOINT(pDebMsgOut);
				return res;
			}
		}
	}

	if (trkNr < 170 && trkIxCnt > 0 && ST_AVFDEB_ISVERBAUDDET_BD(pOpts->basOpts)) {
		st_contOgg_d_debBS3(pOpts, pDPh->pBSI, cFNCN,
				(char*)pDebMsgOut);
	}

	ST_DELPOINT(pDebMsgOut);

	return ST_ERR_SUCC;
	#undef LOC_DEBMSGSZ_TMP_
}

/******************************************************************************/
