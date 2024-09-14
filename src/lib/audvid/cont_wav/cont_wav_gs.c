/******************************************************************************/
/* cont_wav_gs.c                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Getter/Setter functions for WAV containers                                 */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/audvid/cont_wav.h"
/** */
#define SRC_CONT_WAV_GS_C
#include "cont_wav-prot.h"
/*** */
#include "cont_wav_zfnc-prot.h"
#undef SRC_CONT_WAV_GS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const Tst_str*
st_contWav_gs_getHandle_filen(const Tst_contWav_handle *pHandle)
{
	ST_ASSERTN_NULL(pHandle == NULL || pHandle->pObInternal == NULL)

	return ((Tst_contWav_handle_intn*)pHandle->pObInternal)->pFilen;
}

Tst_bool
st_contWav_gs_getHandle_index(const Tst_contWav_handle *pHandle,
		Tst_uint32 *pBsIx, Tst_uint32 *pBsSIx)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pHandle == NULL || pHandle->pObInternal == NULL ||
			pBsIx == NULL || pBsSIx == NULL)

	*pBsIx  = ((Tst_contWav_handle_intn*)pHandle->pObInternal)->bsIx;
	*pBsSIx = ((Tst_contWav_handle_intn*)pHandle->pObInternal)->bsSIx;
	return ST_B_TRUE;
}

Tst_bool
st_contWav_gs_getHandle_isOpen(const Tst_contWav_handle *pHandle)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pHandle == NULL || pHandle->pObInternal == NULL)

	return st_sysFStc_isOpen(
			&((Tst_contWav_handle_intn*)pHandle->pObInternal)->fstcOut);
}

Tst_bool
st_contWav_gs_getHandle_writtenSamples(const Tst_contWav_handle *pHandle,
		Tst_uint64 *pSampleCnt)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pHandle == NULL || pHandle->pObInternal == NULL ||
			pSampleCnt == NULL)

	st_sysUInt64_setUI64(pSampleCnt,
			&((Tst_contWav_handle_intn*)pHandle->pObInternal)->hdInfo.totSampleCnt);
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_contWav_handle*
st_contWav_ite_getNextHandle(Tst_contWav_obj *pWObj,
		ST_OPTARG(const Tst_contWav_handle *pLast))
{
	Tst_uint32 luix;
	Tst_contWav_handle   *pIteE;
	Tst_contWav_obj_intn *pWObjI;

	ST_ASSERTN_NULL(pWObj == NULL || pWObj->pObInternal == NULL)

	pWObjI = (Tst_contWav_obj_intn*)pWObj->pObInternal;

	luix = (pLast == NULL || pLast->pObInternal == NULL ? 0 :
			((Tst_contWav_handle_intn*)pLast->pObInternal)->uniqueIx);

	if (! st_dynlist_ite_gotoFirst(&pWObjI->list))
		return NULL;
	do {
		pIteE = (Tst_contWav_handle*)st_dynlist_ite_getCurrent(&pWObjI->list);
		ST_ASSERTN_NULL(pIteE == NULL || pIteE->pObInternal == NULL)

		if (((Tst_contWav_handle_intn*)pIteE->pObInternal)->uniqueIx > luix)
			return pIteE;
	} while (st_dynlist_ite_gotoNext(&pWObjI->list));

	return NULL;
}

/******************************************************************************/
