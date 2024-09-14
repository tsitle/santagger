/******************************************************************************/
/* cont_wav_zfnc.h              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Sub functions for WAV containers                                           */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 09.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/dynlist.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_file.h"
/** */
#define SRC_CONT_WAV_ZFNC_C
#include "cont_wav_zfnc-prot.h"
/*** */
#include "cont_wav_stcs-prot.h"
#undef SRC_CONT_WAV_ZFNC_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* memcpy() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
st_contwav_fnc_addNewElem(Tst_contWav_obj_intn *pWObjI,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		Tst_contWav_handle **ppElem)
{
	Tst_err res;
	Tst_contWav_handle_intn *pElemI;

	ST_ASSERTN_IARG(pWObjI == NULL || bsIx == 0 || bsSIx == 0 || ppElem == NULL)

	/* check whether an element with bsIx and bsSIx already exists */
	*ppElem = st_contwav_fnc_getElem(pWObjI, bsIx, bsSIx);
	if (*ppElem != NULL)
		return ST_ERR_FAIL;
	/* create and add new element */
	ST_CALLOC(*ppElem, Tst_contWav_handle*, 1, sizeof(Tst_contWav_handle))
	if (*ppElem == NULL)
		return ST_ERR_OMEM;
	res = st_contWav_stc_initElem(*ppElem, (const Tst_str*)"-none-");
	if (res != ST_ERR_SUCC) {
		ST_DELPOINT(*ppElem)
		return res;
	}
	res = st_dynlist_addElement(&pWObjI->list, *ppElem);
	if (res != ST_ERR_SUCC) {
		st_contWav_stc_freeElem(*ppElem);
		ST_DELPOINT(*ppElem)
		return res;
	}
	/* */
	pElemI = (Tst_contWav_handle_intn*)(*ppElem)->pObInternal;
	pElemI->bsIx     = bsIx;
	pElemI->bsSIx    = bsSIx;
	pElemI->uniqueIx = pWObjI->uniqueIxLast++;
	pElemI->pOpts    = &pWObjI->opts;
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

Tst_contWav_handle*
st_contwav_fnc_getElem(Tst_contWav_obj_intn *pWObjI,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx)
{
	Tst_contWav_handle *pE;

	ST_ASSERTN_NULL(pWObjI == NULL || bsIx == 0 || bsSIx == 0)

	if (bsIx == pWObjI->cacheBsIx && bsSIx == pWObjI->cacheBsSIx &&
			pWObjI->pCacheElem != NULL)
		return pWObjI->pCacheElem;

	if (! st_dynlist_ite_gotoFirst(&pWObjI->list))
		return NULL;
	do {
		pE = (Tst_contWav_handle*)st_dynlist_ite_getCurrent(&pWObjI->list);
		ST_ASSERTN_NULL(pE == NULL)

		if (((Tst_contWav_handle_intn*)pE->pObInternal)->bsIx == bsIx &&
				((Tst_contWav_handle_intn*)pE->pObInternal)->bsSIx == bsSIx) {
			pWObjI->cacheBsIx  = bsIx;
			pWObjI->cacheBsSIx = bsSIx;
			pWObjI->pCacheElem = pE;
			return pE;
		}
	} while (st_dynlist_ite_gotoNext(&pWObjI->list));
	return NULL;
}

/*----------------------------------------------------------------------------*/

Tst_err
st_contwav_fnc_createOutpFn(const Tst_str *pOrgFilen,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx, const Tst_bool appendBsIx,
		const Tst_bool wrRIFForAIFF,
		ST_OPTARG(const Tst_str *pOutpdir), const Tst_bool owExFiles,
		Tst_str **ppNewFilen)
{
	const Tst_str *cFNEXT_WAV  = (const Tst_str*)"wav",
	              *cFNEXT_AIFF = (const Tst_str*)"aiff";
	Tst_str       *pOrgFBn  = NULL,
	              *pOrgDirn = NULL;
	Tst_str const *pFExt    = (wrRIFForAIFF ? cFNEXT_WAV : cFNEXT_AIFF),
	              *pIntOutpdir;
	Tst_uint32    nr        = 1000,
	              olen;
	Tst_bool      resB,
	              fexists,
	              vacFnd    = ST_B_FALSE;

	ST_ASSERTN_IARG(pOrgFilen == NULL || ppNewFilen == NULL)

	/* */
	if (! st_sysFileBasename(pOrgFilen, &pOrgFBn))
		return ST_ERR_FAIL;
	if (st_sysStrEmpty(pOutpdir)) {
		if (! st_sysDirname(pOrgFilen, &pOrgDirn)) {
			ST_DELPOINT(pOrgFBn)
			return ST_ERR_FAIL;
		}
		pIntOutpdir = pOrgDirn;
	} else
		pIntOutpdir = pOutpdir;
	/* cut off original fileextension incl. '.' */
	olen = st_sysStrlen(pOrgFBn);
	if (olen > 0) {
		Tst_str *pCh;

		pCh  = (Tst_str*)strrchr((char*)pOrgFBn, (int)'.');
		if (pCh != NULL)
			olen = (Tst_uint32)(pCh - pOrgFBn);
	}
	pOrgFBn[olen] = 0x00;
	/* copy everything but current fileextension to ppNewFilen
	 *   "/some/dir/filename.wav" --> "/some/dir/filename"  */
	resB = st_sysConcatDirAndFilen(pIntOutpdir, pOrgFBn, ppNewFilen);
	/* */
	ST_DELPOINT(pOrgFBn)
	ST_DELPOINT(pOrgDirn)
	if (! resB)
		return ST_ERR_FAIL;
	/* */
	olen = st_sysStrlen(*ppNewFilen);
	ST_REALLOC(*ppNewFilen, Tst_str*, olen + 32, 1)
	if (*ppNewFilen == NULL)
		return ST_ERR_OMEM;
	memset(&(*ppNewFilen)[olen], 0, 32);

	do {
		/* append (number and) fileexts to ppNewFilen
		 *   "filename" --> "filename-1_1.wav"  or  "filename-1_1-001.wav"*/
		if (nr == 1000) {
			if (appendBsIx)
				snprintf((char*)&(*ppNewFilen)[olen], 32, "-%u_%02u.%s",
						bsIx, bsSIx, pFExt);
			else
				snprintf((char*)&(*ppNewFilen)[olen], 32, ".%s",
						pFExt);
		} else {
			if (appendBsIx)
				snprintf((char*)&(*ppNewFilen)[olen], 32, "-%u_%02u-%03u.%s",
						bsIx, bsSIx, nr, pFExt);
			else
				snprintf((char*)&(*ppNewFilen)[olen], 32, "-%03u.%s",
						nr, pFExt);
		}
		/* */
		fexists = (! owExFiles) && st_sysDoesFileExist(*ppNewFilen);
		if (fexists && nr == 999)
			nr = 0;  /* --> stop while() and output error */
		else if (fexists && nr < 999) {
			++nr;  /* --> use nr+1 for filename */
			vacFnd = ST_B_TRUE;
		} else if (! fexists && (nr == 1 || nr == 1000))
			vacFnd = ST_B_TRUE;
		else
			--nr;
	} while (! vacFnd && nr > 0);

	if (! vacFnd) {
		ST_DELPOINT(*ppNewFilen)
		return ST_ERR_FAIL;
	}
	if (nr < 1000) {
		if (appendBsIx)
			snprintf((char*)&(*ppNewFilen)[olen], 32, "-%u_%02u-%03u.%s",
					bsIx, bsSIx, nr, pFExt);
		else
			snprintf((char*)&(*ppNewFilen)[olen], 32, "-%03u.%s",
					nr, pFExt);
	}
	return ST_ERR_SUCC;
}

/******************************************************************************/
