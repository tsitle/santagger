/******************************************************************************/
/* aud_mpeg1_gs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Getter/Setter functions for MPEG-1                                         */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 22.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/aud_mpeg1.h"
/** */
#define SRC_AUD_MPEG1_GS_C
#include "aud_mpeg1-prot.h"
#undef SRC_AUD_MPEG1_GS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_mpeg1_gs_setStr_strrd(Tst_mpeg1_obj *pMO, ST_OPTARG(Tst_streamrd *pStrrd))
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->pStrrd = pStrrd;
}

Tst_err
st_mpeg1_gs_setStr_filen(Tst_mpeg1_obj *pMO, const Tst_str *pFilen)
{
	ST_ASSERTN_IARG(pMO == NULL || pMO->pObInternal == NULL || pFilen == NULL)

	return st_sysStrcpy(pFilen,
			&((Tst_mpeg1_obj_intn*)pMO->pObInternal)->pFilen);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const Tst_str*
st_mpeg1_gs_getStr_filen(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NULL(pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->pFilen;
}

/*----------------------------------------------------------------------------*/

Tst_bool
st_mpeg1_gs_getStr_hasMPEGstream(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->hasMPEGstream;
}

Tst_foffs
st_mpeg1_gs_getStr_startOffset(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(-1, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->frames.offsFrFirst +
			((Tst_mpeg1_obj_intn*)pMO->pObInternal)->frames.offsFrFirstDelta;
}

Tst_fsize
st_mpeg1_gs_getStr_size(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(0, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->frames.totStrSz;
}

Tst_uint32
st_mpeg1_gs_getStr_amountWarnings(const Tst_mpeg1_obj *pMO)
{
	Tst_mpeg1_obj_intn *pMOI;

	ST_ASSERTN_NUM(0, pMO == NULL || pMO->pObInternal == NULL)

	pMOI = (Tst_mpeg1_obj_intn*)pMO->pObInternal;

	return pMOI->frames.errAtWrongOffsCnt +
			(pMOI->frames.errIsCBRwithVBR ? 1 : 0) +
			(pMOI->frames.errIsVBRwoHead ? 1 : 0) +
			(pMOI->frames.errIsVBRhdAtWrongOffs ? 1 : 0) +
			(pMOI->frames.errWasFraCntCorrd ? 1 : 0) +
			(pMOI->frames.errWasBytCntCorrd ? 1 : 0) +
			(pMOI->frames.errIsStrIncompl ? 1 : 0);
}

Tst_mpeg1_layer
st_mpeg1_gs_getStr_layer(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(ST_MPEG1_LAYER_NONE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->mhd.layer;
}

Tst_mpeg1_audioVers
st_mpeg1_gs_getStr_audioVers(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(ST_MPEG1_AUDVERS_NONE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->mhd.audioVers;
}

Tst_uint32
st_mpeg1_gs_getStr_bitrate(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(0, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->mhd.bitrate;
}

Tst_bool
st_mpeg1_gs_getStr_isCBR(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->mhdVbr.isCBR;
}

Tst_bool
st_mpeg1_gs_getStr_isFreeFmt(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->isFreeFmt;
}

Tst_uint32
st_mpeg1_gs_getStr_samplerate(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(0, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->mhd.samplerate;
}

Tst_mpeg1_channMode
st_mpeg1_gs_getStr_channMode(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(ST_MPEG1_CMODE_NONE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->mhd.channMode;
}

Tst_bool
st_mpeg1_gs_getStr_hasCRC(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->mhd.hasCRC;
}

Tst_bool
st_mpeg1_gs_getStr_playtime(const Tst_mpeg1_obj *pMO,
		Tst_av_playtime *pPlaytime)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pMO == NULL || pMO->pObInternal == NULL ||
			pPlaytime == NULL)

	memcpy(pPlaytime,
			&((Tst_mpeg1_obj_intn*)pMO->pObInternal)->playt,
			sizeof(Tst_av_playtime));
	return ST_B_TRUE;
}

/******************************************************************************/
