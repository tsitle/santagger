/******************************************************************************/
/* binobj_stcs.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for Binary Object structures                                     */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/sys_file.h"
/** */
#define SRC_BINOBJ_STCS_C
#include "binobj-prot.h"
#undef SRC_BINOBJ_STCS_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static const Tst_uint32 ST_BINOBJ_DEF_ADD_BUFSZ = 64 * 1024;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_binobj */
Tst_err
st_binobj_stc_initBO(Tst_binobj *pBO)
{
	Tst_binobj_intn *pBOI = NULL;

	ST_ASSERTN_IARG(pBO == NULL)

	ST_CALLOC(pBOI, Tst_binobj_intn*, 1, sizeof(Tst_binobj_intn))
	if (pBOI == NULL) {
		pBO->pObInternal = NULL;
		return ST_ERR_OMEM;
	}
	pBO->pObInternal = pBOI;

	/* */
	pBOI->pDataBuf    = NULL;
	pBOI->pDataTmpFn  = NULL;
	pBOI->pDataFStc   = NULL;
	pBOI->dataBufSz   = 0;
	pBOI->isBufOrFile = ST_B_TRUE;

	st_binobj_stc_rsetBO(pBO);

	return ST_ERR_SUCC;
}

void
st_binobj_stc_rsetBO(Tst_binobj *pBO)
{
	Tst_binobj_intn *pBOI = NULL;

	ST_ASSERTN_VOID(pBO == NULL || pBO->pObInternal == NULL)

	pBOI = (Tst_binobj_intn*)pBO->pObInternal;

	pBOI->threshold = ST_BINOBJ_DEF_THRESHOLD;
	pBOI->reallcSz  = ST_BINOBJ_DEF_ADD_BUFSZ;
	pBOI->pSObjR    = NULL;
	st_binobj_stc_rsetData(pBO);
}

void
st_binobj_stc_freeBO(Tst_binobj *pBO)
{
	ST_ASSERTN_VOID(pBO == NULL)

	if (pBO->pObInternal != NULL) {
		st_binobj_stc_rsetBO(pBO);
		ST_DELPOINT(((Tst_binobj_intn*)pBO->pObInternal)->pDataBuf)
		/* */
		ST_DELPOINT(pBO->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/

void
st_binobj_stc_rsetData(Tst_binobj *pBO)
{
	Tst_binobj_intn *pBOI = NULL;

	ST_ASSERTN_VOID(pBO == NULL || pBO->pObInternal == NULL)

	pBOI = (Tst_binobj_intn*)pBO->pObInternal;

	if (pBOI->pDataFStc != NULL) {
		st_sysFStc_close(pBOI->pDataFStc);
		st_sys_stc_freeFStc(pBOI->pDataFStc);
		ST_DELPOINT(pBOI->pDataFStc)
	}
	if (pBOI->pDataTmpFn != NULL) {
		st_sysUnlinkFile(pBOI->pDataTmpFn);
		ST_DELPOINT(pBOI->pDataTmpFn)
	}
	pBOI->dataSz      = 0;
	pBOI->isBufOrFile = ST_B_TRUE;
}

/******************************************************************************/
