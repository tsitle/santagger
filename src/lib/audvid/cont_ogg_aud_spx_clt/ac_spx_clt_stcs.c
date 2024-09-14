/******************************************************************************/
/* ac_spx_clt_stcs.c            [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for Ogg-Speex and Ogg-Celt structures                            */
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
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_AC_SPX_CLT_STCS_C
#include "ac_spx_clt_stcs-prot.h"
#undef SRC_AC_SPX_CLT_STCS_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy() */
#include <stdlib.h>      /* free() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_speexInfo */
void
st_contOgg_stc_rsetSpeexInfo(Tst_contOgg_speexInfo *pSI)
{
	ST_ASSERTN_VOID(pSI == NULL)

	memset(pSI->versStr, 0, sizeof(pSI->versStr));
	pSI->speexVers  = 0;
	pSI->bitstrVers = 0;
	pSI->sampleRate = 0;
	pSI->mode       = ST_CONTOGG_SPEEXMODE_NONE;
	pSI->channels   = 0;
	pSI->bitrateKBS = 0;
	pSI->isVBR      = 0;
}

/* Tst_contOgg_celtInfo */
void
st_contOgg_stc_rsetCeltInfo(Tst_contOgg_celtInfo *pCI)
{
	ST_ASSERTN_VOID(pCI == NULL)

	memset(pCI->versStr, 0, sizeof(pCI->versStr));
	pCI->bitstrVers = 0;
	pCI->sampleRate = 0;
	pCI->channels   = 0;
	pCI->bitrateKBS = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_speex */
Tst_err
st_contOgg_stc_initSpee(Tst_contOgg_speex *pAud)
{
	Tst_err res;

	ST_ASSERTN_IARG(pAud == NULL)

	st_contOgg_stc_initSpeeIdent(&pAud->ident);
	res = st_contOgg_stc_initVorbCommRaw(&pAud->comm);
	if (res != ST_ERR_SUCC)
		return res;
	st_contOgg_stc_rsetSpee(pAud);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetSpee(Tst_contOgg_speex *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetSpeeIdent(&pAud->ident);
	st_contOgg_stc_rsetVorbCommRaw(&pAud->comm);
	pAud->aPackCnt = 0;
}

void
st_contOgg_stc_freeSpee(Tst_contOgg_speex *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetSpee(pAud);
	/* */
	st_contOgg_stc_freeSpeeIdent(&pAud->ident);
	st_contOgg_stc_freeVorbCommRaw(&pAud->comm);
}

/* Tst_contOgg_speeIdent */
void
st_contOgg_stc_initSpeeIdent(Tst_contOgg_speeIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->pVersStr = NULL;
	st_contOgg_stc_rsetSpeeIdent(pID);
}

void
st_contOgg_stc_rsetSpeeIdent(Tst_contOgg_speeIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->isSet  = ST_B_FALSE;
	pID->packNr = 0;
	/* */
	ST_DELPOINT(pID->pVersStr)
	pID->speexVers       = 0;
	pID->bitstrVers      = 0;
	pID->headerSize      = 0;
	pID->sampleRate      = 0;
	pID->mode            = ST_CONTOGG_SPEEXMODE_NONE;
	pID->channels        = 0;
	pID->bitrateKBS      = 0;
	pID->frameSize       = 0;
	pID->isVBR           = 0;
	pID->framesPerPacket = 0;
	pID->extraHeaders    = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_celt */
Tst_err
st_contOgg_stc_initCelt(Tst_contOgg_celt *pAud)
{
	Tst_err res;

	ST_ASSERTN_IARG(pAud == NULL)

	res = st_contOgg_stc_initVorbCommRaw(&pAud->comm);
	if (res != ST_ERR_SUCC)
		return res;
	st_contOgg_stc_rsetCelt(pAud);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetCelt(Tst_contOgg_celt *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetCeltIdent(&pAud->ident);
	st_contOgg_stc_rsetVorbCommRaw(&pAud->comm);
	pAud->aPackCnt = 0;
}

void
st_contOgg_stc_freeCelt(Tst_contOgg_celt *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetCelt(pAud);
	/* */
	st_contOgg_stc_freeCeltIdent(&pAud->ident);
	st_contOgg_stc_freeVorbCommRaw(&pAud->comm);
}

/* Tst_contOgg_celtIdent */
void
st_contOgg_stc_initCeltIdent(Tst_contOgg_celtIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->pVersStr = NULL;
	st_contOgg_stc_rsetCeltIdent(pID);
}

void
st_contOgg_stc_rsetCeltIdent(Tst_contOgg_celtIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->isSet  = ST_B_FALSE;
	pID->packNr = 0;
	/* */
	ST_DELPOINT(pID->pVersStr)
	pID->bitstrVers     = 0;
	pID->headerSize     = 0;
	pID->sampleRate     = 0;
	pID->channels       = 0;
	pID->frameSize      = 0;
	pID->overlap        = 0;
	pID->bytesPerPacket = 0;
	pID->extraHeaders   = 0;
	/* */
	pID->bitrateKBS = 0;
}

/******************************************************************************/
