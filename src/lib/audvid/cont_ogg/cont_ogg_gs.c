/******************************************************************************/
/* cont_ogg_gs.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Getter/Setter functions for OGG containers / Flac files                    */
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
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/cont_ogg.h"
/** */
#define SRC_CONT_OGG_GS_C
#include "cont_ogg-prot.h"
#undef SRC_CONT_OGG_GS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_contOgg_gs_setStrrd(Tst_contOgg_obj *pAObj, ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->pStrrd = pStrrd;
}

void
st_contOgg_gs_setStrwr(Tst_contOgg_obj *pAObj, ST_OPTARG(Tst_streamwr *pStrwr))
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->pStrwr = pStrwr;
}

Tst_err
st_contOgg_gs_setFilen(Tst_contOgg_obj *pAObj, const Tst_str *pFilen)
{
	ST_ASSERTN_IARG(pAObj == NULL || pAObj->pObInternal == NULL ||
			pFilen == NULL)

	return st_sysStrcpy(pFilen,
			&((Tst_contOgg_obj_intn*)pAObj->pObInternal)->pFilen);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const Tst_str*
st_contOgg_gs_getFilen(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_NULL(pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->pFilen;
}

/*----------------------------------------------------------------------------*/

/*
 * Is source file a Ogg or Flac stream ?
 */
Tst_bool
st_contOgg_gs_getIsSrcOggOrFlac(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->isSrcOggOrFlac;
}

/*
 * Has current file already been read ?
 */
Tst_bool
st_contOgg_gs_getWasFileAlreadyRead(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->alrRead;
}

/*
 * Does file contain any Ogg stream ?
 */
Tst_bool
st_contOgg_gs_getHasOggStream(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->hasOGGstream;
}

/*
 * Does file contain any Flac stream ?
 */
Tst_bool
st_contOgg_gs_getHasFlacStream(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->hasFLACaudio;
}

/*
 * Get content types of Flac/Ogg streams in file
 */
Tst_int32
st_contOgg_gs_getContTypes(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_NUM((Tst_int32)ST_CONTOGG_BSCTP_NONE,
			pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->oggContTypes;
}

/*
 * Get bitstream types of Flac/Ogg streams in file
 */
Tst_int32
st_contOgg_gs_getBitstreamTypes(const Tst_contOgg_obj *pAObj)
{
	Tst_int32 btps = (Tst_int32)ST_CONTOGG_BSTP_NONE;
	Tst_contOgg_substream *pLast = NULL;

	ST_ASSERTN_NUM((Tst_int32)ST_CONTOGG_BSTP_NONE,
			pAObj == NULL || pAObj->pObInternal == NULL)

	while ((pLast = st_contOgg_ite_getNextStream_any(pAObj, pLast)) != NULL)
		btps |= (Tst_int32)st_contOgg_gs_getStr_type(pLast);

	return btps;
}

/*
 * Get amount of streams in file
 */
Tst_uint32
st_contOgg_gs_getStreamCount(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_NUM(0, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->bsArr.cnt;
}

/*
 * Get amount of warnings that occured when reading the file
 */
Tst_uint32
st_contOgg_gs_getErrorCount(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_NUM(0, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->errorCnt;
}

/*
 * Get offset where Ogg/Flac stream begins
 */
Tst_foffs
st_contOgg_gs_getStartOffset(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_NUM(0, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->startOffs;
}

/*
 * Get total size of Ogg/Flac stream
 */
Tst_fsize
st_contOgg_gs_getSize(const Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_NUM(0, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)pAObj->pObInternal)->totSize;
}

/*
 * Get sub stream by Group Index and Group Sub Index
 */
Tst_contOgg_substream*
st_contOgg_gs_getStream_byGrpIx(const Tst_contOgg_obj *pAObj,
		const Tst_uint32 grpIx, const Tst_uint32 grpSIx)
{
	Tst_uint32 x;
	Tst_contOgg_substream      *pCur;
	Tst_contOgg_substr_intn    *pCurI;
	Tst_contOgg_obj_intn const *pAObjI;

	ST_ASSERTN_NULL(pAObj == NULL || pAObj->pObInternal == NULL)

	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;

	for (x = 0; x < pAObjI->bsArr.cnt; x++) {
		pCur  = &pAObjI->bsArr.pArr[x];
		pCurI = (Tst_contOgg_substr_intn*)pCur->pObInternal;
		ST_ASSERTN_NULL(pCurI == NULL)

		if (pCurI->grpIx == grpIx &&
				(! pCurI->isGrouped || pCurI->grpSIx == grpSIx ||
					(pCurI->grpSIx == 0 && grpSIx == 1) ||
					(pCurI->grpSIx == 1 && grpSIx == 0)))
			return pCur;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get sub stream type
 *
 * returns: type on success or ST_CONTOGG_BSTP_NONE on failure
 */
Tst_contOgg_bsType
st_contOgg_gs_getStr_type(const Tst_contOgg_substream *pBS)
{
	ST_ASSERTN_NUM(ST_CONTOGG_BSTP_NONE, pBS == NULL || pBS->pObInternal == NULL)

	return ((Tst_contOgg_substr_intn*)pBS->pObInternal)->bsType;
}

/*
 * Get sub stream Group-Index and Group-Sub-Index
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_groupIndex(const Tst_contOgg_substream *pBS,
		Tst_uint32 *pGrpIx, Tst_uint32 *pGrpSIx)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pGrpIx == NULL || pGrpSIx == NULL)

	*pGrpIx  = ((Tst_contOgg_substr_intn*)pBS->pObInternal)->grpIx;
	*pGrpSIx = ((Tst_contOgg_substr_intn*)pBS->pObInternal)->grpSIx;
	return ST_B_TRUE;
}

/*
 * Is sub stream grouped with other streams ?
 */
Tst_bool
st_contOgg_gs_getStr_isGrouped(const Tst_contOgg_substream *pBS)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL)

	return ((Tst_contOgg_substr_intn*)pBS->pObInternal)->isGrouped;
}

/*----------------------------------------------------------------------------*/

/*
 * Get playtime of sub stream
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_playtime(const Tst_contOgg_substream *pBS,
		Tst_av_playtime *pPlaytime)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pPlaytime == NULL)

	memcpy(pPlaytime,
			&((Tst_contOgg_substr_intn*)pBS->pObInternal)->playtime,
			sizeof(Tst_av_playtime));
	return ST_B_TRUE;
}

/*
 * Get computed average bitrate of sub stream in KBit/s
 */
Tst_uint32
st_contOgg_gs_getStr_avgBitrateInKBS(const Tst_contOgg_substream *pBS)
{
	ST_ASSERTN_NUM(0, pBS == NULL || pBS->pObInternal == NULL)

	return ((Tst_contOgg_substr_intn*)pBS->pObInternal)->compBitrateAvgKBS;
}

/*----------------------------------------------------------------------------*/

/*
 * Get Vorbis info from sub stream
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_vorbisInfo(const Tst_contOgg_substream *pBS,
		Tst_contOgg_vorbisInfo *pVorbInf)
{
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pVorbInf == NULL)

	st_contOgg_stc_rsetVorbisInfo(pVorbInf);
	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	if (pBSI->pVorb == NULL || ! pBSI->pVorb->ident.isSet)
		return ST_B_FALSE;
	/* */
	pVorbInf->vers          = pBSI->pVorb->ident.vers;
	pVorbInf->sampleRate    = pBSI->pVorb->ident.sampleRate;
	pVorbInf->bitrateMinKBS = pBSI->pVorb->ident.bitrateMinKBS;
	pVorbInf->bitrateNomKBS = pBSI->pVorb->ident.bitrateNomKBS;
	pVorbInf->bitrateMaxKBS = pBSI->pVorb->ident.bitrateMaxKBS;
	pVorbInf->channels      = pBSI->pVorb->ident.channels;
	return ST_B_TRUE;
}

/*
 * Get Speex info from sub stream
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_speexInfo(const Tst_contOgg_substream *pBS,
		Tst_contOgg_speexInfo *pSpeexInf)
{
	Tst_uint32 len;
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pSpeexInf == NULL)

	st_contOgg_stc_rsetSpeexInfo(pSpeexInf);
	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	if (pBSI->pSpee == NULL || ! pBSI->pSpee->ident.isSet)
		return ST_B_FALSE;
	/* */
	len = st_sysStrlen(pBSI->pSpee->ident.pVersStr);
	if (len >= sizeof(pSpeexInf->versStr))
		len = sizeof(pSpeexInf->versStr) - 1;
	++len;
	memcpy(pSpeexInf->versStr, pBSI->pSpee->ident.pVersStr, len);
	pSpeexInf->speexVers  = pBSI->pSpee->ident.speexVers;
	pSpeexInf->bitstrVers = pBSI->pSpee->ident.bitstrVers;
	pSpeexInf->sampleRate = pBSI->pSpee->ident.sampleRate;
	pSpeexInf->mode       = pBSI->pSpee->ident.mode;
	pSpeexInf->channels   = pBSI->pSpee->ident.channels;
	pSpeexInf->bitrateKBS = pBSI->pSpee->ident.bitrateKBS;
	pSpeexInf->isVBR      = pBSI->pSpee->ident.isVBR;
	return ST_B_TRUE;
}

/*
 * Get Celt info from sub stream
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_celtInfo(const Tst_contOgg_substream *pBS,
		Tst_contOgg_celtInfo *pCeltInf)
{
	Tst_uint32 len;
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pCeltInf == NULL)

	st_contOgg_stc_rsetCeltInfo(pCeltInf);
	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	if (pBSI->pCelt == NULL || ! pBSI->pCelt->ident.isSet)
		return ST_B_FALSE;
	/* */
	len = st_sysStrlen(pBSI->pCelt->ident.pVersStr);
	if (len >= sizeof(pCeltInf->versStr))
		len = sizeof(pCeltInf->versStr) - 1;
	++len;
	memcpy(pCeltInf->versStr, pBSI->pCelt->ident.pVersStr, len);
	pCeltInf->bitstrVers = pBSI->pCelt->ident.bitstrVers;
	pCeltInf->sampleRate = pBSI->pCelt->ident.sampleRate;
	pCeltInf->channels   = pBSI->pCelt->ident.channels;
	pCeltInf->bitrateKBS = pBSI->pCelt->ident.bitrateKBS;
	return ST_B_TRUE;
}

/*
 * Get Flac info from sub stream
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_flacInfo(const Tst_contOgg_substream *pBS,
		Tst_contOgg_flacInfo *pFlacInf)
{
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pFlacInf == NULL)

	st_contOgg_stc_rsetFlacInfo(pFlacInf);
	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	if (pBSI->pFlac == NULL || ! pBSI->pFlac->ident.isSet)
		return ST_B_FALSE;
	/* */
	pFlacInf->oggFlacVersMaj           = pBSI->pFlac->ident.verMaj;
	pFlacInf->oggFlacVersMin           = pBSI->pFlac->ident.verMin;
	pFlacInf->sampleRate               = pBSI->pFlac->ident.sampleR;
	pFlacInf->channels                 = pBSI->pFlac->ident.chann;
	pFlacInf->bitsPerSample            = pBSI->pFlac->ident.bitsPerS;
	pFlacInf->md5OfDecodedAudioChecked = pBSI->pFlac->ident.md5OfDecAudioChkd;
	pFlacInf->md5OfDecodedAudioOK      = pBSI->pFlac->ident.md5OfDecAudioOK;
	return ST_B_TRUE;
}

/*
 * Get Dirac info from sub stream
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_diracInfo(const Tst_contOgg_substream *pBS,
		Tst_contOgg_diracInfo *pDiracInf)
{
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pDiracInf == NULL)

	st_contOgg_stc_rsetDiracInfo(pDiracInf);
	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	if (pBSI->pDira == NULL || ! pBSI->pDira->ident.isSet)
		return ST_B_FALSE;
	/* */
	pDiracInf->versMaj = pBSI->pDira->ident.versMaj;
	pDiracInf->versMin = pBSI->pDira->ident.versMin;
	pDiracInf->width   = pBSI->pDira->ident.width;
	pDiracInf->height  = pBSI->pDira->ident.height;
	pDiracInf->fps     = pBSI->pDira->ident.fps;
	return ST_B_TRUE;
}

/*
 * Get Theora info from sub stream
 *
 * returns: ST_B_TRUE on success
 */
Tst_bool
st_contOgg_gs_getStr_theoraInfo(const Tst_contOgg_substream *pBS,
		Tst_contOgg_theoraInfo *pTheoraInf)
{
	Tst_contOgg_substr_intn *pBSI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pBS == NULL || pBS->pObInternal == NULL ||
			pTheoraInf == NULL)

	st_contOgg_stc_rsetTheoraInfo(pTheoraInf);
	pBSI = (Tst_contOgg_substr_intn*)pBS->pObInternal;
	if (pBSI->pTheo == NULL || ! pBSI->pTheo->ident.isSet)
		return ST_B_FALSE;
	/* */
	pTheoraInf->versMaj    = pBSI->pTheo->ident.versMaj;
	pTheoraInf->versMin    = pBSI->pTheo->ident.versMin;
	pTheoraInf->versRev    = pBSI->pTheo->ident.versRev;
	pTheoraInf->width      = pBSI->pTheo->ident.width;
	pTheoraInf->height     = pBSI->pTheo->ident.height;
	pTheoraInf->fps        = pBSI->pTheo->ident.fps;
	pTheoraInf->bitrateKBS = pBSI->pTheo->ident.bitrateKBS;
	pTheoraInf->quality    = pBSI->pTheo->ident.quality;
	/* */
	if (pBSI->compBitrateAvgKBS > 0)
		pTheoraInf->bitrateKBS = pBSI->compBitrateAvgKBS;
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Find next stream in array
 *   If pLast == NULL the first stream will be returned
 *   If pLast != NULL the next stream after pLast will be returned
 *
 * pAObj   MUST be set
 * pLast   may be NULL
 *
 * returns: pointer to next stream or NULL
 */
Tst_contOgg_substream*
st_contOgg_ite_getNextStream_any(const Tst_contOgg_obj *pAObj,
		ST_OPTARG(const Tst_contOgg_substream *pLast))
{
	Tst_uint32 x;
	Tst_contOgg_substream      *pCur;
	Tst_contOgg_obj_intn const *pAObjI;

	ST_ASSERTN_NULL(pAObj == NULL || pAObj->pObInternal == NULL)

	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;

	for (x = 0; x < pAObjI->bsArr.cnt; x++) {
		pCur = &pAObjI->bsArr.pArr[x];
		if (pLast == NULL || pCur > pLast)
			return pCur;
	}
	return NULL;
}

/******************************************************************************/
