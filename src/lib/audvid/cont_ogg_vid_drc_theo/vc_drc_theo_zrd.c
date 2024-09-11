/******************************************************************************/
/* vc_drc_theo_zrd.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for reading Ogg-Dirac and Ogg-Theora                             */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 16.06.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/audvid/cont_ogg.h"
/** */
#define SRC_VC_DRC_THEO_ZRD_C
#include "vc_drc_theo_zrd-prot.h"
/*** */
#include "../cont_ogg/cont_ogg_zfdeb-prot.h"
#include "../cont_ogg_met_vorbcomm/mc_vorbcomm_zrd-prot.h"
#include "../av_zfdeb-prot.h"
#undef SRC_VC_DRC_THEO_ZRD_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* this is based on code from liboggz-1.1.1
 * and on code from schroedinger-1.0.9, a Dirac-Encoder */
typedef struct {
	Tst_uint32 numerator;
	Tst_uint32 denominator;
} Tst_contOgg__dirac_numDen;

typedef struct {
	Tst_uint32 versMajor;
	Tst_uint32 versMinor;
	Tst_uint32 profile;
	Tst_uint32 level;
	/* */
	Tst_uint32 index;
	Tst_uint32 width;
	Tst_uint32 height;
	Tst_uint32 chroma_format;
	/* */
	Tst_uint32 isInterlacedUI;
	Tst_uint32 isTopFieldFirstUI;
	/* */
	Tst_uint32 frameRate_numerator;
	Tst_uint32 frameRate_denominator;
	Tst_uint32 pixelAspectRatio_numerator;
	Tst_uint32 pixelAspectRatio_denominator;
	/* */
	Tst_uint32 cleanWidth;
	Tst_uint32 cleanHeight;
	Tst_uint32 leftOffset;
	Tst_uint32 topOffset;
	/* */
	Tst_uint32 lumaOffset;
	Tst_uint32 lumaExcursion;
	Tst_uint32 chromaOffset;
	Tst_uint32 chromaExcursion;
	/* */
	Tst_uint32 colourPrimaries;
	Tst_uint32 colourMatrix;
	Tst_uint32 transferFunction;
	/* */
	Tst_uint32 interlacedCoding;
	/* */
	Tst_uint32 unused0;
	Tst_uint32 unused1;
	Tst_uint32 unused2;
} Tst_contOgg__dirac_identSub;

enum {
	ST_CONTOGG_DIRAC_CHROMA_444 = 0,
	ST_CONTOGG_DIRAC_CHROMA_422,
	ST_CONTOGG_DIRAC_CHROMA_420
};

static const Tst_contOgg__dirac_numDen cDIRAC_FRATE_TBL[] = {
	{1, 1}, /* this first value is never used */
	{24000, 1001}, {24, 1}, {25, 1}, {30000, 1001}, {30, 1},
	{50, 1}, {60000, 1001}, {60, 1}, {15000, 1001}, {25, 2}
};
static const Tst_contOgg__dirac_numDen cDIRAC_ASPECT_RATIOS[] = {
	{0, 0}, {1, 1}, {10, 11}, {12, 11}, {40, 33}, {16, 11}, {4, 3}
};
static const Tst_contOgg__dirac_identSub cDIRAC_VIDFMTS[] = {
	{ 0, 0, 0, 0,
	    0, /* custom */
	    640, 480, ST_CONTOGG_DIRAC_CHROMA_420,
	    ST_B_FALSE, ST_B_FALSE,
	    24000, 1001, 1, 1,
	    640, 480, 0, 0,
	    0, 255, 128, 255,
	    0, 0, 0 },
	{ 0, 0, 0, 0,
	    1, /* QSIF525 */
	    176, 120, ST_CONTOGG_DIRAC_CHROMA_420,
	    ST_B_FALSE, ST_B_FALSE,
	    15000, 1001, 10, 11,
	    176, 120, 0, 0,
	    0, 255, 128, 255,
	    1, 1, 0 },
	{ 0, 0, 0, 0,
	    2, /* QCIF */
	    176, 144, ST_CONTOGG_DIRAC_CHROMA_420,
	    ST_B_FALSE, ST_B_TRUE,
	    25, 2, 12, 11,
	    176, 144, 0, 0,
	    0, 255, 128, 255,
	    2, 1, 0 },
	{ 0, 0, 0, 0,
	    3, /* SIF525 */
	    352, 240, ST_CONTOGG_DIRAC_CHROMA_420,
	    ST_B_FALSE, ST_B_FALSE,
	    15000, 1001, 10, 11,
	    352, 240, 0, 0,
	    0, 255, 128, 255,
	    1, 1, 0 },
	{ 0, 0, 0, 0,
	    4, /* CIF */
	    352, 288, ST_CONTOGG_DIRAC_CHROMA_420,
	    ST_B_FALSE, ST_B_TRUE,
	    25, 2, 12, 11,
	    352, 288, 0, 0,
	    0, 255, 128, 255,
	    2, 1, 0 },
	{ 0, 0, 0, 0,
	    5, /* 4SIF525 */
	    704, 480, ST_CONTOGG_DIRAC_CHROMA_420,
	    ST_B_FALSE, ST_B_FALSE,
	    15000, 1001, 10, 11,
	    704, 480, 0, 0,
	    0, 255, 128, 255,
	    1, 1, 0 },
	{ 0, 0, 0, 0,
	    6, /* 4CIF */
	    704, 576, ST_CONTOGG_DIRAC_CHROMA_420,
	    ST_B_FALSE, ST_B_TRUE,
	    25, 2, 12, 11,
	    704, 576, 0, 0,
	    0, 255, 128, 255,
	    2, 1, 0 },
	{ 0, 0, 0, 0,
	    7, /* SD480I-60 */
	    720, 480, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_TRUE, ST_B_FALSE,
	    30000, 1001, 10, 11,
	    704, 480, 8, 0,
	    64, 876, 512, 896,
	    1, 1, 0 },
	{ 0, 0, 0, 0,
	    8, /* SD576I-50 */
	    720, 576, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_TRUE, ST_B_TRUE,
	    25, 1, 12, 11,
	    704, 576, 8, 0,
	    64, 876, 512, 896,
	    2, 1, 0 },
	{ 0, 0, 0, 0,
	    9, /* HD720P-60 */
	    1280, 720, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_FALSE, ST_B_TRUE,
	    60000, 1001, 1, 1,
	    1280, 720, 0, 0,
	    64, 876, 512, 896,
	    0, 0, 0 },
	{ 0, 0, 0, 0,
	    10, /* HD720P-50 */
	    1280, 720, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_FALSE, ST_B_TRUE,
	    50, 1, 1, 1,
	    1280, 720, 0, 0,
	    64, 876, 512, 896,
	    0, 0, 0 },
	{ 0, 0, 0, 0,
	    11, /* HD1080I-60 */
	    1920, 1080, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_TRUE, ST_B_TRUE,
	    30000, 1001, 1, 1,
	    1920, 1080, 0, 0,
	    64, 876, 512, 896,
	    0, 0, 0 },
	{ 0, 0, 0, 0,
	    12, /* HD1080I-50 */
	    1920, 1080, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_TRUE, ST_B_TRUE,
	    25, 1, 1, 1,
	    1920, 1080, 0, 0,
	    64, 876, 512, 896,
	    0, 0, 0 },
	{ 0, 0, 0, 0,
	    13, /* HD1080P-60 */
	    1920, 1080, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_FALSE, ST_B_TRUE,
	    60000, 1001, 1, 1,
	    1920, 1080, 0, 0,
	    64, 876, 512, 896,
	    0, 0, 0 },
	{ 0, 0, 0, 0,
	    14, /* HD1080P-50 */
	    1920, 1080, ST_CONTOGG_DIRAC_CHROMA_422,
	    ST_B_FALSE, ST_B_TRUE,
	    50, 1, 1, 1,
	    1920, 1080, 0, 0,
	    64, 876, 512, 896,
	    0, 0, 0 },
	{ 0, 0, 0, 0,
	    15, /* DC2K */
	    2048, 1080, ST_CONTOGG_DIRAC_CHROMA_444,
	    ST_B_FALSE, ST_B_TRUE,
	    24, 1, 1, 1,
	    2048, 1080, 0, 0,
	    256, 3504, 2048, 3584,
	    3, 0, 0 },
	{ 0, 0, 0, 0,
	    16, /* DC4K */
	    4096, 2160, ST_CONTOGG_DIRAC_CHROMA_444,
	    ST_B_FALSE, ST_B_TRUE,
	    24, 1, 1, 1,
	    2048, 1536, 0, 0,
	    256, 3504, 2048, 3584,
	    3, 0, 0 }
};

/* this is based on code from libtheora-1.1.1 */
typedef enum {
	/** The color space was not specified at the encoder.
	    It may be conveyed by an external means.*/
	ST_CONTOGG_THEORA_CS_UNSPECIFIED,
	/** A color space designed for NTSC content.*/
	ST_CONTOGG_THEORA_CS_ITU_REC_470M,
	/**A color space designed for PAL/SECAM content.*/
	ST_CONTOGG_THEORA_CS_ITU_REC_470BG,
	/**The total number of currently defined color spaces.*/
	ST_CONTOGG_THEORA_CS_NSPACES
} Tst_contOgg__theora_colorspace;

typedef enum{
	/** Chroma decimation by 2 in both the X and Y directions (4:2:0).
	    The Cb and Cr chroma planes are half the width and half the
	    height of the luma plane.*/
	ST_CONTOGG_THEORA_PF_420,
	/** Currently reserved.*/
	ST_CONTOGG_THEORA_PF_RSVD,
	/** Chroma decimation by 2 in the X direction (4:2:2).
	    The Cb and Cr chroma planes are half the width of the luma plane,
	    but full height.*/
	ST_CONTOGG_THEORA_PF_422,
	/** No chroma decimation (4:4:4).
	    The Cb and Cr chroma planes are full width and full height.*/
	ST_CONTOGG_THEORA_PF_444,
	/** The total number of currently defined pixel formats.*/
	ST_CONTOGG_THEORA_PF_NFORMATS
} Tst_contOgg__theora_pixelFmt;

/** */
static Tst_err
ST_CONTOGG__drcrd_readUInt32(Tst_streamrd *pSObj, Tst_uint32 *pUI32);

static Tst_err
ST_CONTOGG__drcrd_readBool(Tst_streamrd *pSObj, Tst_bool *pBool);

/** */
static Tst_err
ST_CONTOGG__theord_parseIdent(const Tst_contOgg_opts *pOpts,
                              Tst_contOgg_substr_intn *pBSI,
                              const Tst_byte packTp,
                              Tst_streamrd *pSObj);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse Dirac header packet
 */
Tst_err
st_contOgg_drcrd_parseHPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, const Tst_byte packTp,
		Tst_streamrd *pSObj)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_uint32 indexUI       = 0,
	           tmpUIa        = 0,
	           tmpUIb        = 0;
	Tst_bool   bol           = ST_B_FALSE,
	           isInterld     = ST_B_FALSE,
	           isTopField1st = ST_B_FALSE;
	Tst_contOgg_diraIdent             *pDID = &pBSI->pDira->ident;
	Tst_contOgg__dirac_numDen const   *pNumDen;
	Tst_contOgg__dirac_identSub const *pIdSub;
	Tst_contOgg__dirac_numDen         tempNumDen;

	if (pBSI->packCur.nr == 1 && ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
				"Dirac ident header %sfound",
				(pBSI->packCur.nr == 1 ? "" : "copy "));

	/* parameters */
	ST_CONTOGG__drcrd_readUInt32(pSObj, &pDID->versMaj);
	ST_CONTOGG__drcrd_readUInt32(pSObj, &pDID->versMin);
	ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
	res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIb);
	if (res != ST_ERR_SUCC)
		return res;
	/**st_contOgg_prf("dirac vers %u.%u, prof %u, lev %u\n",
			pDID->versMajor, pDID->versMinor, tmpUIa, tmpUIb);**/

	/* base video header */
	/** */
	res = ST_CONTOGG__drcrd_readUInt32(pSObj, &indexUI);
	if (res != ST_ERR_SUCC)
		return res;
	/** */
	if (indexUI >= sizeof(cDIRAC_VIDFMTS) / sizeof(cDIRAC_VIDFMTS[0])) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid video format found");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}
	pIdSub = &cDIRAC_VIDFMTS[indexUI];

	/* source parameters */
	/** frame dimensions */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (bol) {
		ST_CONTOGG__drcrd_readUInt32(pSObj, &pDID->width);
		res = ST_CONTOGG__drcrd_readUInt32(pSObj, &pDID->height);
		/**st_contOgg_prf("dirac dim --> %ux%u\n", pDID->width, pDID->height);**/
	} else {
		pDID->width  = pIdSub->width;
		pDID->height = pIdSub->height;
		/**st_contOgg_prf("dirac dim %ux%u\n", pDID->width, pDID->height);**/
	}
	if (res != ST_ERR_SUCC)
		return res;

	/** chroma header */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (bol) {
		/* skip chromaFormat */
		res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
	}
	if (res != ST_ERR_SUCC)
		return res;

	/** scan header */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (bol) {
		res = ST_CONTOGG__drcrd_readBool(pSObj, &isInterld);
		if (isInterld)
			res = ST_CONTOGG__drcrd_readBool(pSObj, &isTopField1st);
	}
	if (res != ST_ERR_SUCC)
		return res;

	/** frame rate */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (bol) {
		res = ST_CONTOGG__drcrd_readUInt32(pSObj, &indexUI);
		/* */
		if (indexUI == 0) {
			ST_CONTOGG__drcrd_readUInt32(pSObj, &tempNumDen.numerator);
			res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tempNumDen.denominator);
			if (tempNumDen.denominator == 0)
				pDID->fps = 0;
			else
				pDID->fps = (double)tempNumDen.numerator /
						(double)tempNumDen.denominator;
			/**st_contOgg_prf("dirac fps --> %.2f\n", pDID->fps);**/
		} else {
			if (indexUI >= sizeof(&cDIRAC_FRATE_TBL) /
						sizeof(&cDIRAC_FRATE_TBL[0])) {
				if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
							"invalid frame rate index found");
				++pBSI->errorCnt;
				return ST_ERR_IDAT;
			}
			/* */
			pNumDen   = &cDIRAC_FRATE_TBL[indexUI];
			pDID->fps = (double)pNumDen->numerator / (double)pNumDen->denominator;
			/**st_contOgg_prf("dirac fps -->--> %.2f\n", pDID->fps);**/
		}
	} else {
		pDID->fps = (double)pIdSub->frameRate_numerator /
				(double)pIdSub->frameRate_denominator;
		/**st_contOgg_prf("dirac fps %.2f\n", pDID->fps);**/
	}
	if (res != ST_ERR_SUCC)
		return res;

	/** pixel aspect ratio */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (res != ST_ERR_SUCC)
		return res;
	if (bol) {
		res = ST_CONTOGG__drcrd_readUInt32(pSObj, &indexUI);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (indexUI == 0) {
			res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tempNumDen.numerator);
			if (res == ST_ERR_SUCC)
				res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tempNumDen.denominator);
			if (res != ST_ERR_SUCC)
				return res;
			/*if (tempNumDen.denominator == 0)
				pixAspRatio = 0;
			else
				pixAspRatio = (double)tempNumDen.numerator /
						(double)tempNumDen.denominator;*/
		} else {
			if (indexUI >= sizeof(&cDIRAC_ASPECT_RATIOS) /
						sizeof(&cDIRAC_ASPECT_RATIOS[0])) {
				if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
					st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
							"invalid pixel aspect ratio index found");
				++pBSI->errorCnt;
				return ST_ERR_IDAT;
			}
			/* */
			pNumDen = &cDIRAC_ASPECT_RATIOS[indexUI];
			const double pixAspRatio = (double)pNumDen->numerator / (double)pNumDen->denominator;
			if (pixAspRatio < 0.0f) {
				if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts)) {
					st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
							"invalid pixel aspect ratio value found");
				}
				++pBSI->errorCnt;
				return ST_ERR_IDAT;
			}
		}
	} /*else {
		pixAspRatio = (double)pIdSub->pixelAspectRatio_numerator /
				(double)pIdSub->pixelAspectRatio_denominator;
	}*/

	/** clean area */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (bol) {
		/* skip cleanWidth, cleanHeight, leftOffset and topOffset */
		ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
		ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
		ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
		res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
	}
	if (res != ST_ERR_SUCC)
		return res;

	/** signal range */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (bol) {
		res = ST_CONTOGG__drcrd_readUInt32(pSObj, &indexUI);
		/* */
		if (indexUI == 0) {
			/** skip luma_offset, luma_excursion,
			 **   chroma_offset and chroma_excursion */
			ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
			ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
			ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
			res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
		} /*else {
			* get all info from tables..., skipped *
		}*/
	}
	if (res != ST_ERR_SUCC)
		return res;

	/** colour spec */
	res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
	if (bol) {
		res = ST_CONTOGG__drcrd_readUInt32(pSObj, &indexUI);
		/* */
		/* get colour spec from table..., skipped */
		/* */
		if (indexUI == 0) {
			/* colour primaries */
			ST_CONTOGG__drcrd_readBool(pSObj, &bol);
			if (bol) {
				/* skip colour_primaries */
				ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
			}
			/* colour matrix */
			ST_CONTOGG__drcrd_readBool(pSObj, &bol);
			if (bol) {
				/* skip colour_matrix */
				ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
			}
			/* transfer function */
			res = ST_CONTOGG__drcrd_readBool(pSObj, &bol);
			if (bol) {
				/* skip transfer_function */
				res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
			}
		}
	}
	if (res != ST_ERR_SUCC)
		return res;

	/** skip interlaced_coding */
	res = ST_CONTOGG__drcrd_readUInt32(pSObj, &tmpUIa);
	if (res != ST_ERR_SUCC)
		return res;

	pBSI->bsType = ST_CONTOGG_BSTP_DIRA;
	pDID->isSet  = ST_B_TRUE;
	pDID->packNr = pBSI->packCur.nr;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Parse Theora header packet
 */
Tst_err
st_contOgg_theord_parseHPack(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, const Tst_byte packTp,
		Tst_streamrd *pSObj, Tst_bool *pStopThen)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err res = ST_ERR_SUCC;

	/**if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
		st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
				"packet type %u", packTp);**/
	*pStopThen = ST_B_FALSE;
	switch (packTp) {
	case 0x80:  /* theora identification info */
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"Theora ident header found");
		/* */
		res = ST_CONTOGG__theord_parseIdent(pOpts, pBSI, packTp, pSObj);
		/* */
		if (res == ST_ERR_SUCC) {
			pBSI->bsType              = ST_CONTOGG_BSTP_THEO;
			pBSI->pTheo->ident.isSet  = ST_B_TRUE;
			pBSI->pTheo->ident.packNr = pBSI->packCur.nr;
		}
		break;
	case 0x81:  /* theora comment */
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"Theora comment header found");
		res = st_contOgg_vorbComm_parse(pBSI, pSObj,
				&pBSI->pTheo->comm,
				/*hasFramingBit:*/ST_B_FALSE, /*hasMaxSz:*/ST_B_FALSE,
				/*maxSz:*/0,
				/*srcCmtBegPos:*/(Tst_int32)st_streamrd_getCurPos(pSObj) - 7);
		if (res != ST_ERR_SUCC)
			return res;
		break;
	case 0x82:  /* theora setup info */
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"Theora setup header found");
		pBSI->pTheo->isSetupSet = ST_B_TRUE;
		*pStopThen              = ST_B_TRUE;
		break;
	default:
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid theora packType 0x%02x found, "
					"skipping packet", packTp);
		++pBSI->errorCnt;
		res = ST_ERR_IDAT;
	}
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read an unsigned int in Dirac-manner
 */
static Tst_err
ST_CONTOGG__drcrd_readUInt32(Tst_streamrd *pSObj, Tst_uint32 *pUI32)
{
	/* this is based on code from schroedinger-1.0.9, a Dirac-Encoder */
	Tst_err    res;
	Tst_uint32 count = 0,
	           value = 0;
	Tst_byte   bit   = 0;

	while (ST_B_TRUE) {
		res = st_streamrd_rdByte_noCRC(pSObj, 1, &bit);
		if (res != ST_ERR_SUCC || bit == 1)
			break;
		++count;
		res = st_streamrd_rdByte_noCRC(pSObj, 1, &bit);
		if (res != ST_ERR_SUCC)
			break;
		value = (value << 1) | bit;
	}

	if (res != ST_ERR_SUCC)
		*pUI32 = 0;
	else
		*pUI32 = (1 << count) - 1 + value;
	return res;
}

/*
 * Read a bool in Dirac-manner
 */
static Tst_err
ST_CONTOGG__drcrd_readBool(Tst_streamrd *pSObj, Tst_bool *pBool)
{
	Tst_err  res;
	Tst_byte bit = 0;

	res = st_streamrd_rdByte_noCRC(pSObj, 1, &bit);
	if (res != ST_ERR_SUCC)
		*pBool = ST_B_FALSE;
	else
		*pBool = (bit == 1);
	return res;
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_CONTOGG__theord_parseIdent(const Tst_contOgg_opts *pOpts,
		Tst_contOgg_substr_intn *pBSI, const Tst_byte packTp,
		Tst_streamrd *pSObj)
{
	const char *cFNCN = __FUNCTION__;
	Tst_err    res;
	Tst_uint16 ui16    = 0;
	Tst_uint32 ui32    = 0,
	           picW    = 0,
	           picH    = 0,
	           //picYfin = 0,
	           fpsN    = 0,
	           fpsD    = 0,
	           aspN    = 0,
	           aspD    = 0;
	Tst_byte   ui8     = 0,
	           picX    = 0,
	           picYpre = 0;
	//Tst_contOgg__theora_colorspace colSpace;
	Tst_contOgg__theora_pixelFmt   pixelFmt;
	Tst_contOgg_theoIdent          *pTID = &pBSI->pTheo->ident;

	/* version */
	st_streamrd_rdByte_noCRC(pSObj, 8, &pBSI->pTheo->ident.versMaj);
	res = st_streamrd_rdByte_noCRC(pSObj, 8, &pBSI->pTheo->ident.versMin);
	if (res != ST_ERR_SUCC)
		return res;
	if (pBSI->pTheo->ident.versMaj > 3 ||
			(pBSI->pTheo->ident.versMaj == 3 && pBSI->pTheo->ident.versMin > 2)) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"can't handle Theora version %u.%u.x",
					pBSI->pTheo->ident.versMaj, pBSI->pTheo->ident.versMin);
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}
	res = st_streamrd_rdByte_noCRC(pSObj, 8, &pBSI->pTheo->ident.versRev);
	if (res != ST_ERR_SUCC)
		return res;

	/* frame description */
	/** dimensions */
	st_streamrd_rdUInt16(pSObj,
			ST_STREAMRD_IEND_BE, 16, &ui16);  /* ENDIAN: BE-->HOST */
	pTID->width = (Tst_uint32)ui16 << 4;
	st_streamrd_rdUInt16(pSObj,
			ST_STREAMRD_IEND_BE, 16, &ui16);  /* ENDIAN: BE-->HOST */
	pTID->height = (Tst_uint32)ui16 << 4;
	/**st_contOgg_prf("theora %ux%u\n", pTID->width, pTID->height);**/

	/** pic_width and pic_height */
	st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 24, &picW);  /* ENDIAN: BE-->HOST */
	st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 24, &picH);  /* ENDIAN: BE-->HOST */
	/**st_contOgg_prf("theora pic WxH %ux%u\n", picW, picH);**/

	/** pic_x and pic_y */
	st_streamrd_rdByte_noCRC(pSObj, 8, &picX);
	st_streamrd_rdByte_noCRC(pSObj, 8, &picYpre);
	/**st_contOgg_prf("theora pic X,Y %u,%u\n", picX, picYpre);**/

	/** fps_numerator and fps_denominator */
	st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 32, &fpsN);  /* ENDIAN: BE-->HOST */
	res = st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 32, &fpsD);  /* ENDIAN: BE-->HOST */
	if (res != ST_ERR_SUCC)
		return res;
	if (fpsD == 0)
		pTID->fps = 0;
	else
		pTID->fps = (double)fpsN / (double)fpsD;
	/**st_contOgg_prf("theora fps %.3f\n", pTID->fps);**/

	if (pTID->width == 0 || pTID->height == 0 ||
			 picW + picX > pTID->width ||
			 picH + picYpre > pTID->height ||
			 fpsN == 0 || fpsD == 0) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid data found in Theora ident header");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** correct sense of picY */
	//picYfin = pTID->height - picH - picYpre;

	/** aspect_numerator and aspect_denominator */
	st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 24, &aspN);  /* ENDIAN: BE-->HOST */
	st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 24, &aspD);  /* ENDIAN: BE-->HOST */
	/**st_contOgg_prf("theora %08x asp %u/%u = %.3f\n",
			aspN, aspN, aspD, (double)aspN / (double)aspD);**/

	/** colorspace */
	st_streamrd_rdByte_noCRC(pSObj, 8, &ui8);
	//colSpace = (Tst_contOgg__theora_colorspace)ui8;

	/** bitrate */
	st_streamrd_rdUInt32(pSObj,
			ST_STREAMRD_IEND_BE, 24, &ui32);  /* ENDIAN: BE-->HOST */
	if ((Tst_int32)ui32 < 0)
		pTID->bitrateKBS = 0;
	else
		pTID->bitrateKBS = ui32 / 1000;
	/** quality */
	st_streamrd_rdByte_noCRC(pSObj, 6, &ui8);
	pTID->quality = (Tst_uint32)ui8;
	/**st_contOgg_prf("theora bitr %u, qual %u\n",
			pTID->bitrateKBS, pTID->quality);**/

	/** keyframe_granule_shift */
	st_streamrd_rdByte_noCRC(pSObj, 5, &ui8);
	pTID->kfgshift = (Tst_uint32)ui8;

	/** pixelFmt */
	st_streamrd_rdByte_noCRC(pSObj, 2, &ui8);
	pixelFmt = (Tst_contOgg__theora_pixelFmt)ui8;
	if (pixelFmt == ST_CONTOGG_THEORA_PF_RSVD) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid pixel format found in Theora ident header");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	/** padding */
	res = st_streamrd_rdByte_noCRC(pSObj, 3, &ui8);
	if (res != ST_ERR_SUCC)
		return res;
	if (ui8 != 0) {
		if (ST_AVFDEB_ISVERBAUD_BD(pOpts->basOpts))
			st_contOgg_d_debBS3(pOpts, pBSI, cFNCN,
					"invalid padding found in Theora ident header");
		++pBSI->errorCnt;
		return ST_ERR_IDAT;
	}

	return ST_ERR_SUCC;
}

/******************************************************************************/
