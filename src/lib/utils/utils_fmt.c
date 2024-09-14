/******************************************************************************/
/* utils_fmt.c                  [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Some functions for determining file formats etc.                           */
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
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/utils/utils_fmt.h"
/** */
#define SRC_UTILS_FMT_C
#include "utils_fmt-priv.h"
#undef SRC_UTILS_FMT_C

/*
// System-Includes
*/
#include <stdlib.h>      /* free() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static const Tst_utilsFmt_mime_arrElem*
ST_UTILSFMT__getMime_arrE(const Tst_str *pFExt, const Tst_str *pMime,
                          const Tst_utilsFmt_mimeTp mtp);

static Tst_utilsFmt_mimeTp
ST_UTILSFMT__getMime_enum_byStream_x(Tst_streamrd *pSObj,
                                     const Tst_bool pic, const Tst_bool audvid,
                                     const Tst_bool tag);

static const Tst_utilsFmt_picFmt_arrElem*
ST_UTILSFMT__getPicFmt_arrE(const Tst_str *pFExt, const Tst_str *pMime,
                            const Tst_utilsFmt_picFmt pfm,
                            const Tst_utilsFmt_mimeTp mtp);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byFExt(const Tst_str *pFExt)
{
	Tst_utilsFmt_picFmt_arrElem const *pElem;

	ST_ASSERTN_NUM(ST_UTILSFMT_PF_NONE, pFExt == NULL)

	pElem = ST_UTILSFMT__getPicFmt_arrE(pFExt, NULL,
			ST_UTILSFMT_PF_NONE, ST_UTILSFMT_MTP_NONE);
	if (pElem == NULL)
		return ST_UTILSFMT_PF_NONE;
	return pElem->pfmE;
}

Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byFn(const Tst_str *pFilen)
{
	Tst_str *pFE = NULL;
	Tst_utilsFmt_picFmt pfm;

	ST_ASSERTN_NUM(ST_UTILSFMT_PF_NONE, pFilen == NULL)

	if (! st_sysFileExt(pFilen, &pFE)) {
		ST_DELPOINT(pFE)
		return ST_UTILSFMT_PF_NONE;
	}
	pfm = st_utilsFmt_getPicFmt_enum_byFExt(pFE);
	ST_DELPOINT(pFE)
	return pfm;
}

Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byMime(const Tst_str *pMime)
{
	Tst_utilsFmt_picFmt_arrElem const *pElem;

	ST_ASSERTN_NUM(ST_UTILSFMT_PF_NONE, pMime == NULL)

	pElem = ST_UTILSFMT__getPicFmt_arrE(NULL, pMime,
			ST_UTILSFMT_PF_NONE, ST_UTILSFMT_MTP_NONE);
	if (pElem == NULL)
		return ST_UTILSFMT_PF_NONE;
	return pElem->pfmE;
}

Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byBuffer(const Tst_buf *pBuf,
		const Tst_uint32 sz)
{
	Tst_utilsFmt_picFmt_arrElem const *pElem;
	Tst_utilsFmt_mimeTp mtp;

	ST_ASSERTN_NUM(ST_UTILSFMT_PF_NONE, pBuf == NULL)

	mtp = st_utilsFmt_getMime_enum_byBuffer_pic(pBuf, sz);
	if (mtp == ST_UTILSFMT_MTP_NONE)
		return ST_UTILSFMT_PF_NONE;
	pElem = ST_UTILSFMT__getPicFmt_arrE(NULL, NULL,
			ST_UTILSFMT_PF_NONE, mtp);
	if (pElem == NULL)
		return ST_UTILSFMT_PF_NONE;
	return pElem->pfmE;
}

Tst_utilsFmt_picFmt
st_utilsFmt_getPicFmt_enum_byBObj(Tst_binobj *pBinDat)
{
	Tst_uint32 bytesRd = 0;
	Tst_buf    buf[20];
	Tst_streamrd strrd;

	ST_ASSERTN_NUM(ST_UTILSFMT_PF_NONE, pBinDat == NULL)

	st_streamrd_stc_initSObj(&strrd);
	if (st_binobj_attachStreamrd(pBinDat, &strrd, 0) != ST_ERR_SUCC)
		return ST_UTILSFMT_PF_NONE;

	/* copy some bytes from stream */
	if (st_streamrd_rdAheadBuffer(&strrd,
				sizeof(buf), buf, &bytesRd) != ST_ERR_SUCC ||
			bytesRd != sizeof(buf)) {
		st_binobj_detachStreamrd(pBinDat);
		st_streamrd_stc_freeSObj(&strrd);
		return ST_UTILSFMT_PF_NONE;
	}

	st_binobj_detachStreamrd(pBinDat);
	st_streamrd_stc_freeSObj(&strrd);

	return st_utilsFmt_getPicFmt_enum_byBuffer(buf, bytesRd);
}

const Tst_str*
st_utilsFmt_getPicFmt_fext_byEnum(const Tst_utilsFmt_picFmt pfmE)
{
	Tst_utilsFmt_picFmt_arrElem const *pElem;

	pElem = ST_UTILSFMT__getPicFmt_arrE(NULL, NULL,
			pfmE, ST_UTILSFMT_MTP_NONE);
	if (pElem == NULL)
		return NULL;
	return (Tst_str*)pElem->pFExt;
}

const Tst_str*
st_utilsFmt_getPicFmt_mime_byEnum(const Tst_utilsFmt_picFmt pfmE)
{
	Tst_utilsFmt_picFmt_arrElem const *pElem;

	pElem = ST_UTILSFMT__getPicFmt_arrE(NULL, NULL,
			pfmE, ST_UTILSFMT_MTP_NONE);
	if (pElem == NULL)
		return NULL;
	return st_utilsFmt_getMime_defMime_byEnum(pElem->mimTp);
}

/*----------------------------------------------------------------------------*/

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byFExt(const Tst_str *pFExt)
{
	Tst_utilsFmt_mime_arrElem const *pElemM;

	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pFExt == NULL)

	pElemM = ST_UTILSFMT__getMime_arrE(pFExt, NULL, ST_UTILSFMT_MTP_NONE);
	if (pElemM == NULL)
		return ST_UTILSFMT_MTP_NONE;
	return pElemM->mimTp;
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byFn(const Tst_str *pFilen)
{
	Tst_str *pFE = NULL;
	Tst_utilsFmt_mimeTp mtp;

	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pFilen == NULL)

	if (! st_sysFileExt(pFilen, &pFE)) {
		ST_DELPOINT(pFE)
		return ST_UTILSFMT_MTP_NONE;
	}
	mtp = st_utilsFmt_getMime_enum_byFExt(pFE);
	ST_DELPOINT(pFE)
	return mtp;
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byMime(const Tst_str *pMime)
{
	Tst_utilsFmt_mime_arrElem const *pElemM;

	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pMime == NULL)

	pElemM = ST_UTILSFMT__getMime_arrE(NULL, pMime, ST_UTILSFMT_MTP_NONE);
	if (pElemM == NULL)
		return ST_UTILSFMT_MTP_NONE;
	return pElemM->mimTp;
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream(Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pSObj == NULL)

	return ST_UTILSFMT__getMime_enum_byStream_x(pSObj,
			ST_B_FALSE, ST_B_FALSE, ST_B_FALSE);
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream_pic(Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pSObj == NULL)

	return ST_UTILSFMT__getMime_enum_byStream_x(pSObj,
			ST_B_TRUE, ST_B_FALSE, ST_B_FALSE);
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream_audvid(Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pSObj == NULL)

	return ST_UTILSFMT__getMime_enum_byStream_x(pSObj,
			ST_B_FALSE, ST_B_TRUE, ST_B_FALSE);
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byStream_tag(Tst_streamrd *pSObj)
{
	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pSObj == NULL)

	return ST_UTILSFMT__getMime_enum_byStream_x(pSObj,
			ST_B_FALSE, ST_B_FALSE, ST_B_TRUE);
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer(const Tst_buf *pBuf,
		const Tst_uint32 sz)
{
	Tst_utilsFmt_mimeTp mtp;

	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pBuf == NULL)

	mtp = st_utilsFmt_getMime_enum_byBuffer_pic(pBuf, sz);
	if (mtp == ST_UTILSFMT_MTP_NONE)
		mtp = st_utilsFmt_getMime_enum_byBuffer_audvid(pBuf, sz);
	if (mtp == ST_UTILSFMT_MTP_NONE)
		mtp = st_utilsFmt_getMime_enum_byBuffer_tag(pBuf, sz);
	return mtp;
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer_pic(const Tst_buf *pBuf,
		const Tst_uint32 sz)
{
	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pBuf == NULL)

	if (sz == 0)
		return ST_UTILSFMT_MTP_NONE;

	if (sz >= 10 && pBuf[0] == 0xff && pBuf[1] == 0xd8 &&
			(memcmp(&pBuf[6], "JFIF", 4) == 0 ||
				memcmp(&pBuf[6], "Exif", 4) == 0))
		return ST_UTILSFMT_MTP_PICJPG;

	else if (sz >= 4 && memcmp(pBuf, "GIF8", 4) == 0)
		return ST_UTILSFMT_MTP_PICGIF;

	else if (sz >= 4 && pBuf[0] == 0x00 && pBuf[1] == 0x00 &&
			pBuf[2] == 0x01 && pBuf[3] == 0x00)
		return ST_UTILSFMT_MTP_PICICO;

	else if (sz >= 8 && pBuf[0] == 0x89 &&
			memcmp(&pBuf[1], "PNG", 3) == 0 &&
			pBuf[4] == 0x0d && pBuf[5] == 0x0a &&
			pBuf[6] == 0x1a && pBuf[7] == 0x0a)
		return ST_UTILSFMT_MTP_PICPNG;

	else if (sz >= 4 && memcmp(pBuf, "8BPS", 4) == 0)
		return ST_UTILSFMT_MTP_PICPSD;

	else if (sz >= 4 && memcmp(pBuf, "MM", 2) == 0 &&
			pBuf[2] == 0x00 && pBuf[3] == 0x2a)
		return ST_UTILSFMT_MTP_PICTIF;  /* big-endian */

	else if (sz >= 4 && memcmp(pBuf, "II", 2) == 0 &&
			pBuf[2] == 0x2a && pBuf[3] == 0x00)
		return ST_UTILSFMT_MTP_PICTIF;  /* little-endian */

	else if (sz >= 14 && memcmp(pBuf, "BM", 2) == 0 &&
			pBuf[14] == 0x0c)
		return ST_UTILSFMT_MTP_PICBMP;  /* os/2 1.x */

	else if (sz >= 14 && memcmp(pBuf, "BM", 2) == 0 &&
			pBuf[14] == 64)
		return ST_UTILSFMT_MTP_PICBMP;  /* os/2 2.x */

	else if (sz >= 14 && memcmp(pBuf, "BM", 2) == 0 &&
			pBuf[14] == 40)
		return ST_UTILSFMT_MTP_PICBMP;  /* win 3.x */

	else if (sz >= 14 && memcmp(pBuf, "BM", 2) == 0 &&
			pBuf[14] == 128)
		return ST_UTILSFMT_MTP_PICBMP;  /* win nt/2k */

	return ST_UTILSFMT_MTP_NONE;
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer_audvid(const Tst_buf *pBuf,
		const Tst_uint32 sz)
{
	Tst_uint32 cmpUI = 0;

	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pBuf == NULL)

	if (sz == 0)
		return ST_UTILSFMT_MTP_NONE;

	if (sz >= 4 &&
			memcmp(pBuf, "OggS", 4) == 0)
		return ST_UTILSFMT_MTP_AUDVIDOGG;

	else if (sz >= 4 &&
			memcmp(pBuf, "fLaC", 4) == 0)
		return ST_UTILSFMT_MTP_AUDFLAC;

	else if (sz >= 12 &&
			memcmp(pBuf, "RIFF", 4) == 0 &&
			memcmp(&pBuf[8], "WAVE", 4) == 0)
		return ST_UTILSFMT_MTP_AUDWAV;

	else if (sz >= 12 &&
			memcmp(pBuf, "FORM", 4) == 0 &&
			memcmp(&pBuf[8], "AIFF", 4) == 0)
		return ST_UTILSFMT_MTP_AUDAIFF;

	else if (sz >= 4) {
		memcpy(&cmpUI, pBuf, 4);  /* ENDIAN: BE-->BE */
#		if (WORDS_BIGENDIAN != 1)
		cmpUI = st_sysReverseByteOrder_UI32(cmpUI);  /* ENDIAN: BE-->LE */
#		endif
		if ((cmpUI & 0xffe00000) == 0xffe00000 &&      /* (frame sync) */
				(cmpUI & 0x00180000) != 0x00080000 &&  /* (audio version) */
				(cmpUI & 0x00060000) != 0x00000000 &&  /* (layer) */
				(cmpUI & 0x0000f000) != 0x0000f000     /* (bitrate index) */) {
			return ST_UTILSFMT_MTP_AUDMPEG1;
		}
	}

	return ST_UTILSFMT_MTP_NONE;
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBuffer_tag(const Tst_buf *pBuf,
		const Tst_uint32 sz)
{
	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pBuf == NULL)

	if (sz == 0)
		return ST_UTILSFMT_MTP_NONE;

	if (sz >= 3 &&
			memcmp(pBuf, "ID3", 3) == 0)
		return ST_UTILSFMT_MTP_APPTAGIV2;  /* ID3v2 header */

	else if (sz >= 3 &&
			memcmp(pBuf, "3DI", 3) == 0)
		return ST_UTILSFMT_MTP_APPTAGIV2;  /* ID3v2 footer */

	else if (sz >= 8 &&
			memcmp(pBuf, "APETAGEX", 8) == 0)
		return ST_UTILSFMT_MTP_APPTAGAV2;  /* APEv2 */

	else if (sz >= 3 &&
			memcmp(pBuf, "TAG", 3) == 0)
		return ST_UTILSFMT_MTP_APPTAGIV1;  /* ID3v1 */

	else if (sz >= 8 &&
			memcmp(pBuf, "VORBCOMM", 8) == 0)
		return ST_UTILSFMT_MTP_APPTAGVOR;  /* Pseudo Vorbis Tag */

	return ST_UTILSFMT_MTP_NONE;
}

Tst_utilsFmt_mimeTp
st_utilsFmt_getMime_enum_byBObj(Tst_binobj *pBinDat)
{
	Tst_uint32 bytesRd = 0;
	Tst_buf    buf[20];
	Tst_streamrd strrd;

	ST_ASSERTN_NUM(ST_UTILSFMT_MTP_NONE, pBinDat == NULL)

	st_streamrd_stc_initSObj(&strrd);
	if (st_binobj_attachStreamrd(pBinDat, &strrd, 0) != ST_ERR_SUCC)
		return ST_UTILSFMT_MTP_NONE;

	/* copy some bytes from stream */
	if (st_streamrd_rdAheadBuffer(&strrd,
				sizeof(buf), buf, &bytesRd) != ST_ERR_SUCC ||
			bytesRd != sizeof(buf)) {
		st_binobj_detachStreamrd(pBinDat);
		st_streamrd_stc_freeSObj(&strrd);
		return ST_UTILSFMT_MTP_NONE;
	}

	st_binobj_detachStreamrd(pBinDat);
	st_streamrd_stc_freeSObj(&strrd);

	return st_utilsFmt_getMime_enum_byBuffer(buf, bytesRd);
}

const Tst_str*
st_utilsFmt_getMime_defMime_byFExt(const Tst_str *pFExt)
{
	Tst_utilsFmt_mime_arrElem const *pElemM;

	ST_ASSERTN_NULL(pFExt == NULL)

	pElemM = ST_UTILSFMT__getMime_arrE(pFExt, NULL, ST_UTILSFMT_MTP_NONE);
	if (pElemM == NULL)
		return NULL;
	return (Tst_str*)pElemM->pMime1;
}

const Tst_str*
st_utilsFmt_getMime_defMime_byFn(const Tst_str *pFilen)
{
	Tst_str       *pFE = NULL;
	Tst_str const *pMi;

	ST_ASSERTN_NULL(pFilen == NULL)

	if (! st_sysFileExt(pFilen, &pFE)) {
		ST_DELPOINT(pFE)
		return NULL;
	}
	pMi = st_utilsFmt_getMime_defMime_byFExt(pFE);
	ST_DELPOINT(pFE)
	return pMi;
}

const Tst_str*
st_utilsFmt_getMime_defMime_byEnum(const Tst_utilsFmt_mimeTp mtp)
{
	Tst_utilsFmt_mime_arrElem const *pElemM;

	pElemM = ST_UTILSFMT__getMime_arrE(NULL, NULL, mtp);
	if (pElemM == NULL)
		return NULL;
	return (Tst_str*)pElemM->pMime1;
}

const Tst_str*
st_utilsFmt_getMime_defFExt_byFExt(const Tst_str *pFExt)
{
	Tst_utilsFmt_mime_arrElem const *pElemM;

	ST_ASSERTN_NULL(pFExt == NULL)

	pElemM = ST_UTILSFMT__getMime_arrE(pFExt, NULL, ST_UTILSFMT_MTP_NONE);
	if (pElemM == NULL)
		return NULL;
	return (Tst_str*)pElemM->pFExt1;
}

const Tst_str*
st_utilsFmt_getMime_defFExt_byFn(const Tst_str *pFilen)
{
	Tst_str       *pFE = NULL;
	Tst_str const *pMi;

	ST_ASSERTN_NULL(pFilen == NULL)

	if (! st_sysFileExt(pFilen, &pFE)) {
		ST_DELPOINT(pFE)
		return NULL;
	}
	pMi = st_utilsFmt_getMime_defFExt_byFExt(pFE);
	ST_DELPOINT(pFE)
	return pMi;
}

const Tst_str*
st_utilsFmt_getMime_defFExt_byMime(const Tst_str *pMime)
{
	Tst_utilsFmt_mime_arrElem const *pElemM;

	ST_ASSERTN_NULL(pMime == NULL)

	pElemM = ST_UTILSFMT__getMime_arrE(NULL, pMime, ST_UTILSFMT_MTP_NONE);
	if (pElemM == NULL)
		return NULL;
	return (Tst_str*)pElemM->pFExt1;
}

const Tst_str*
st_utilsFmt_getMime_defFExt_byEnum(const Tst_utilsFmt_mimeTp mtp)
{
	Tst_utilsFmt_mime_arrElem const *pElemM;

	pElemM = ST_UTILSFMT__getMime_arrE(NULL, NULL, mtp);
	if (pElemM == NULL)
		return NULL;
	return (Tst_str*)pElemM->pFExt1;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static const Tst_utilsFmt_mime_arrElem*
ST_UTILSFMT__getMime_arrE(const Tst_str *pFExt,
		const Tst_str *pMime, const Tst_utilsFmt_mimeTp mtp)
{
	Tst_uint32 slenFE,
	           slenMI,
	           x = 0;
	Tst_utilsFmt_mime_arrElem const *pElemM;

	slenFE = st_sysStrlen(pFExt);
	slenMI = st_sysStrlen(pMime);
	if (slenFE < 1 && slenMI < 1 && mtp == ST_UTILSFMT_MTP_NONE)
		return NULL;
	/* */
	pElemM = (Tst_utilsFmt_mime_arrElem*)&ST_UTILSFMT_MIMETYPES[x];
	while (pElemM->pMime1 != NULL && pElemM->pFExt1 != NULL) {
		if (slenFE > 0) {
			if (st_sysStrlen2(pElemM->pFExt1) == slenFE &&
					st_sysStrcmpN2(ST_B_FALSE, slenFE, pElemM->pFExt1, pFExt))
				return pElemM;
			if (st_sysStrlen2(pElemM->pFExt2) == slenFE &&
					st_sysStrcmpN2(ST_B_FALSE, slenFE, pElemM->pFExt2, pFExt))
				return pElemM;
			if (st_sysStrlen2(pElemM->pFExt3) == slenFE &&
					st_sysStrcmpN2(ST_B_FALSE, slenFE, pElemM->pFExt3, pFExt))
				return pElemM;
		} else if (slenMI > 0) {
			if (st_sysStrlen2(pElemM->pMime1) == slenMI &&
					st_sysStrcmpN2(ST_B_FALSE, slenMI, pElemM->pMime1, pMime))
				return pElemM;
			if (st_sysStrlen2(pElemM->pMime2) == slenMI &&
					st_sysStrcmpN2(ST_B_FALSE, slenMI, pElemM->pMime2, pMime))
				return pElemM;
			if (st_sysStrlen2(pElemM->pMime3) == slenMI &&
					st_sysStrcmpN2(ST_B_FALSE, slenMI, pElemM->pMime3, pMime))
				return pElemM;
		} else {
			if (mtp == pElemM->mimTp)
				return pElemM;
		}
		pElemM = (Tst_utilsFmt_mime_arrElem*)&ST_UTILSFMT_MIMETYPES[++x];
	}
	return NULL;
}

static Tst_utilsFmt_mimeTp
ST_UTILSFMT__getMime_enum_byStream_x(Tst_streamrd *pSObj,
		const Tst_bool pic, const Tst_bool audvid, const Tst_bool tag)
{
	Tst_uint32 bytesRd = 0;
	Tst_buf    buf[20];

	if (pSObj == NULL)
		return ST_UTILSFMT_MTP_NONE;

	/* copy some bytes from stream */
	if (st_streamrd_rdAheadBuffer(pSObj,
				sizeof(buf), buf, &bytesRd) != ST_ERR_SUCC ||
			bytesRd != sizeof(buf))
		return ST_UTILSFMT_MTP_NONE;
	/* */
	if (pic)
		return st_utilsFmt_getMime_enum_byBuffer_pic(buf, sizeof(buf));
	if (audvid)
		return st_utilsFmt_getMime_enum_byBuffer_audvid(buf, sizeof(buf));
	if (tag)
		return st_utilsFmt_getMime_enum_byBuffer_tag(buf, sizeof(buf));
	return st_utilsFmt_getMime_enum_byBuffer(buf, sizeof(buf));
}

static const Tst_utilsFmt_picFmt_arrElem*
ST_UTILSFMT__getPicFmt_arrE(const Tst_str *pFExt,
		const Tst_str *pMime, const Tst_utilsFmt_picFmt pfm,
		const Tst_utilsFmt_mimeTp mtp)
{
	Tst_str const *pDefFE = NULL;
	Tst_uint32    slenDFE = 0,
	              x       = 0;
	Tst_utilsFmt_picFmt_arrElem const *pElem;

	if (pFExt != NULL)
		pDefFE = st_utilsFmt_getMime_defFExt_byFExt(pFExt);
	else if (pMime != NULL)
		pDefFE = st_utilsFmt_getMime_defFExt_byMime(pMime);
	slenDFE = st_sysStrlen2(pDefFE);
	if (slenDFE < 1 && pfm == ST_UTILSFMT_PF_NONE && mtp == ST_UTILSFMT_MTP_NONE)
		return NULL;
	/* */
	pElem = (Tst_utilsFmt_picFmt_arrElem*)&ST_UTILSFMT_PICFORMATS[x];
	while (pElem->pfmE != ST_UTILSFMT_PF_NONE && pElem->pFExt != NULL) {
		if (slenDFE > 0) {
			if (st_sysStrlen2(pElem->pFExt) == slenDFE &&
					st_sysStrcmpN2(ST_B_FALSE, slenDFE, pElem->pFExt, pDefFE))
				return pElem;
		} else if (pfm != ST_UTILSFMT_PF_NONE) {
			if (pElem->pfmE == pfm)
				return pElem;
		} else {
			if (pElem->mimTp == mtp)
				return pElem;
		}
		pElem = (Tst_utilsFmt_picFmt_arrElem*)&ST_UTILSFMT_PICFORMATS[++x];
	}
	return NULL;
}

/******************************************************************************/
