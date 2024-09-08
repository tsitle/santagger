/******************************************************************************/
/* vc_drc_theo_stcs.c           [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for OGG-Dirac / OGG-Theora structures                            */
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
#include <santag/common/sys_math.h>
#include <santag/audvid/cont_ogg.h>
#include <santag/audvid/cont_ogg_vorbcomm.h>
/** */
#define SRC_VC_DRC_THEO_STCS_C
#include "vc_drc_theo_stcs-prot.h"
#undef SRC_VC_DRC_THEO_STCS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_diracInfo */
void
st_contOgg_stc_rsetDiracInfo(Tst_contOgg_diracInfo *pDI)
{
	ST_ASSERTN_VOID(pDI == NULL)

	pDI->versMaj = 0;
	pDI->versMin = 0;
	pDI->width   = 0;
	pDI->height  = 0;
	pDI->fps     = 0.0;
}

/* Tst_contOgg_theoraInfo */
void
st_contOgg_stc_rsetTheoraInfo(Tst_contOgg_theoraInfo *pTI)
{
	ST_ASSERTN_VOID(pTI == NULL)

	pTI->versMaj    = 0;
	pTI->versMin    = 0;
	pTI->versRev    = 0;
	pTI->width      = 0;
	pTI->height     = 0;
	pTI->fps        = 0.0;
	pTI->bitrateKBS = 0;
	pTI->quality    = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_theora */
Tst_err
st_contOgg_stc_initTheo(Tst_contOgg_theora *pVid)
{
	Tst_err res;

	ST_ASSERTN_IARG(pVid == NULL)

	res = st_contOgg_stc_initVorbCommRaw(&pVid->comm);
	if (res != ST_ERR_SUCC)
		return res;
	st_contOgg_stc_rsetTheo(pVid);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetTheo(Tst_contOgg_theora *pVid)
{
	ST_ASSERTN_VOID(pVid == NULL)

	st_contOgg_stc_rsetTheoIdent(&pVid->ident);
	st_contOgg_stc_rsetVorbCommRaw(&pVid->comm);
	pVid->isSetupSet = ST_B_FALSE;
	pVid->vPackCnt   = 0;
	ST_SYSMATH_STC_RSETUI64(pVid->maxFrIx)
}

void
st_contOgg_stc_freeTheo(Tst_contOgg_theora *pVid)
{
	ST_ASSERTN_VOID(pVid == NULL)

	st_contOgg_stc_rsetTheo(pVid);
	/* */
	st_contOgg_stc_freeTheoIdent(&pVid->ident);
	st_contOgg_stc_freeVorbCommRaw(&pVid->comm);
}

/* Tst_contOgg_theoIdent */
void
st_contOgg_stc_rsetTheoIdent(Tst_contOgg_theoIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->isSet  = ST_B_FALSE;
	pID->packNr = 0;
	/* */
	pID->versMaj    = 0;
	pID->versMin    = 0;
	pID->versRev    = 0;
	pID->width      = 0;
	pID->height     = 0;
	pID->fps        = 0.0;
	pID->bitrateKBS = 0;
	pID->quality    = 0;
	pID->kfgshift   = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_contOgg_dirac */
Tst_err
st_contOgg_stc_initDira(Tst_contOgg_dirac *pVid)
{
	ST_ASSERTN_IARG(pVid == NULL)

	st_contOgg_stc_rsetDira(pVid);
	return ST_ERR_SUCC;
}

void
st_contOgg_stc_rsetDira(Tst_contOgg_dirac *pVid)
{
	ST_ASSERTN_VOID(pVid == NULL)

	st_contOgg_stc_rsetDiraIdent(&pVid->ident);
	pVid->vPackCnt = 0;
}

/* Tst_contOgg_diraIdent */
void
st_contOgg_stc_rsetDiraIdent(Tst_contOgg_diraIdent *pID)
{
	ST_ASSERTN_VOID(pID == NULL)

	pID->isSet  = ST_B_FALSE;
	pID->packNr = 0;
	/* */
	pID->versMaj = 0;
	pID->versMin = 0;
	pID->width   = 0;
	pID->height  = 0;
	pID->fps     = 0.0;
}

/******************************************************************************/
