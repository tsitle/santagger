/******************************************************************************/
/* ac_vorb_stcs.c               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for OGG-Vorbis structures                                        */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/streamrd.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
/** */
#define SRC_AC_VORB_STCS_C
#include "ac_vorb_stcs-prot.h"
#undef SRC_AC_VORB_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* free(),... */
#include <string.h>      /* memset(),... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_vorbisInfo */
void
st_contOgg_stc_rsetVorbisInfo(Tst_contOgg_vorbisInfo *pVI)
{
	ST_ASSERTN_VOID(pVI == NULL)

	pVI->vers          = 0;
	pVI->sampleRate    = 0;
	pVI->bitrateMinKBS = 0;
	pVI->bitrateNomKBS = 0;
	pVI->bitrateMaxKBS = 0;
	pVI->channels      = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_vorbis */
Tst_err
st_contOgg_stc_initVorb(Tst_contOgg_vorbis *pAud)
{
	Tst_err res;

	ST_ASSERTN_IARG(pAud == NULL)

	res = st_contOgg_stc_initVorbCommRaw(&pAud->comm);
	if (res != ST_ERR_SUCC)
		return res;
	st_contOgg_stc_initVorbIdent(&pAud->ident);
	st_contOgg_stc_initVorbSetup(&pAud->setup);
	res = st_contOgg_stc_initVorbDec(&pAud->decObj);
	if (res != ST_ERR_SUCC)
		return res;
	/* */
	st_contOgg_stc_rsetVorb(pAud);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetVorb(Tst_contOgg_vorbis *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetVorbIdent(&pAud->ident);
	st_contOgg_stc_rsetVorbCommRaw(&pAud->comm);
	st_contOgg_stc_rsetVorbSetup(&pAud->setup);
	st_contOgg_stc_rsetVorbDec(&pAud->decObj);
	pAud->aPackCnt = 0;
}

void
st_contOgg_stc_freeVorb(Tst_contOgg_vorbis *pAud)
{
	ST_ASSERTN_VOID(pAud == NULL)

	st_contOgg_stc_rsetVorb(pAud);
	/* */
	st_contOgg_stc_freeVorbIdent(&pAud->ident);
	st_contOgg_stc_freeVorbCommRaw(&pAud->comm);
	st_contOgg_stc_freeVorbSetup(&pAud->setup);
	st_contOgg_stc_freeVorbDec(&pAud->decObj);
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_vorbIdent */
void
st_contOgg_stc_rsetVorbIdent(Tst_contOgg_vorbIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->isSet  = ST_B_FALSE;
	pID->packNr = 0;
	/* */
	pID->vers          = 0;
	pID->sampleRate    = 0;
	pID->bitrateMinKBS = 0;
	pID->bitrateNomKBS = 0;
	pID->bitrateMaxKBS = 0;
	pID->channels      = 0;
	pID->blockSz0      = 0;
	pID->blockSz1      = 0;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_vorbSetup */
void
st_contOgg_stc_rsetVorbSetup(Tst_contOgg_vorbSetup *pSet)
{
	ST_ASSERTN_VOID(pSet == NULL)

	pSet->isSet  = ST_B_FALSE;
	pSet->packNr = 0;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_vorbOpts */
void
st_contOgg_stc_rsetVorbOpts(Tst_contOgg_vorbOpts *pVO)
{
	ST_ASSERTN_VOID(pVO == NULL)

	pVO->decodeFull   = ST_B_FALSE;
	pVO->isDecoderSet = ST_B_FALSE;
	/* */
	pVO->cbDecHndNew        = NULL;
	pVO->cbDecHndDel        = NULL;
	pVO->cbDecSetOutpFmt    = NULL;
	pVO->cbDecOpenFeed      = NULL;
	pVO->cbDecFeedHd        = NULL;
	pVO->cbDecFeedAudAndDec = NULL;
	pVO->cbDecCloseFeed     = NULL;
}

/*----------------------------------------------------------------------------*/

/* Tst_contOgg_vorbDecObj */
Tst_err
st_contOgg_stc_initVorbDec(Tst_contOgg_vorbDecObj *pDec)
{
	ST_ASSERTN_IARG(pDec == NULL)

	st_streamrd_stc_initSObj(&pDec->decStrrd);
	pDec->pDBuf  = NULL;
	pDec->dbufSz = 0;
	/* */
	st_contOgg_stc_rsetVorbDec(pDec);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetVorbDec(Tst_contOgg_vorbDecObj *pDec)
{
	ST_ASSERTN_VOID(pDec == NULL)

	pDec->pDecHnd = NULL;
	/* */
	st_streamrd_stc_rsetSObj(&pDec->decStrrd);
	pDec->dbufUsd = 0;
	/* */
	pDec->sampleR  = 0;
	pDec->channels = 0;
	pDec->bps      = 0;
	/* */
	pDec->pPCMstrHnd   = NULL;
	pDec->isOutpFmtSet = ST_B_FALSE;
}

void
st_contOgg_stc_freeVorbDec(Tst_contOgg_vorbDecObj *pDec)
{
	ST_ASSERTN_VOID(pDec == NULL)

	st_contOgg_stc_rsetVorbDec(pDec);
	/* */
	st_streamrd_stc_freeSObj(&pDec->decStrrd);
	ST_DELPOINT(pDec->pDBuf)
	pDec->dbufSz = 0;
}

/******************************************************************************/
