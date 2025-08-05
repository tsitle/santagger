/******************************************************************************/
/* cont_wav_opts.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for WAV container options                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 03.07.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/audvid/cont_wav.h"
/** */
#define SRC_CONT_WAV_OPTS_C
#include "cont_wav-prot.h"
#undef SRC_CONT_WAV_OPTS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_contWav_opts_setBasicOpts(Tst_contWav_obj *pWObj,
		ST_OPTARG(const Tst_basOpts *pBOpts))
{
	ST_ASSERTN_VOID(pWObj == NULL || pWObj->pObInternal == NULL)

	if (pBOpts == NULL)
		ST_RSETSTC_BASOPTS(
				((Tst_contWav_obj_intn*)pWObj->pObInternal)->opts.basOpts)
	else
		ST_COPYSTC_BASOPTS(*pBOpts,
				((Tst_contWav_obj_intn*)pWObj->pObInternal)->opts.basOpts);
}

/*----------------------------------------------------------------------------*/

void
st_contWav_opts_setCB_newOutpStr(Tst_contWav_obj *pWObj,
		ST_OPTARG(const Tst_contWav_cbNewOutputStream cbNewOutpStr))
{
	ST_ASSERTN_VOID(pWObj == NULL || pWObj->pObInternal == NULL)

	((Tst_contWav_obj_intn*)pWObj->pObInternal)->opts.cbNewOutpStr =
			cbNewOutpStr;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_contWav_opts_setOutputDir(Tst_contWav_obj *pWObj, const Tst_str *pOutpdir)
{
	ST_ASSERTN_IARG(pWObj == NULL || pWObj->pObInternal == NULL ||
				pOutpdir == NULL)

	return st_sysStrcpy(pOutpdir,
				&((Tst_contWav_obj_intn*)pWObj->pObInternal)->opts.pOutpdir);
}

void
st_contWav_opts_setOverwriteExistingOutputFiles(Tst_contWav_obj *pWObj,
		const Tst_bool owExFiles)
{
	ST_ASSERTN_VOID(pWObj == NULL || pWObj->pObInternal == NULL)

	((Tst_contWav_obj_intn*)pWObj->pObInternal)->opts.owExFiles = owExFiles;
}

/*----------------------------------------------------------------------------*/

void
st_contWav_opts_setOutputFmt(Tst_contWav_obj *pWObj,
		const Tst_bool fmtRIFForAIFF)
{
	ST_ASSERTN_VOID(pWObj == NULL || pWObj->pObInternal == NULL)

	((Tst_contWav_obj_intn*)pWObj->pObInternal)->opts.wrFmtRIFForAIFF =
			fmtRIFForAIFF;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_contWav_opts_getOutputFmt(Tst_contWav_obj *pWObj,
		Tst_bool *pIsRIFForAIFF)
{
	Tst_contWav_obj_intn *pWObjI;

	ST_ASSERTN_VOID(pWObj == NULL || pWObj->pObInternal == NULL ||
			pIsRIFForAIFF == NULL)

	pWObjI = (Tst_contWav_obj_intn*)pWObj->pObInternal;

	*pIsRIFForAIFF = pWObjI->opts.wrFmtRIFForAIFF;
}

/******************************************************************************/
