/******************************************************************************/
/* cont_ogg_opts.c              [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for OGG / Flac options                                           */
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
#include "src/includes/audvid/cont_ogg.h"
/** */
#define SRC_CONT_OGG_OPTS_C
#include "cont_ogg-prot.h"
#undef SRC_CONT_OGG_OPTS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_contOgg_opts_setBasicOpts(Tst_contOgg_obj *pAObj,
		ST_OPTARG(const Tst_basOpts *pBOpts))
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	if (pBOpts == NULL)
		ST_RSETSTC_BASOPTS(
				((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.basOpts)
	else
		ST_COPYSTC_BASOPTS(*pBOpts,
				((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.basOpts);
}

/* analizing mode ? */
void
st_contOgg_opts_setAnlzMode(Tst_contOgg_obj *pAObj,
		const Tst_bool anlz, ST_OPTARG(const Tst_cb_ana_msg cbAnlz))
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.cbAnaMsg = cbAnlz;
	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.anlz     =
			(anlz && cbAnlz != NULL);
}

void
st_contOgg_opts_setQuickScan(Tst_contOgg_obj *pAObj, const Tst_bool doQuickScan)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.quickScan = doQuickScan;
}

/* check CRC of OGG-frames ? */
void
st_contOgg_opts_setCheckCRCofOGGframes(Tst_contOgg_obj *pAObj,
		const Tst_bool check)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.checkCRCofr =
			check;
}

/* read comments from streams ? */
void
st_contOgg_opts_setReadComments(Tst_contOgg_obj *pAObj, const Tst_bool rdCmts)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.rdComments = rdCmts;
}

/* force rewriting of vorbis comments in OGG/Flac streams ? */
void
st_contOgg_opts_setForceRewriteComments(Tst_contOgg_obj *pAObj,
		const Tst_bool forceRewrCmts)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.forceRewrCmts = forceRewrCmts;
}

void
st_contOgg_opts_setPCMoutputBPS(Tst_contOgg_obj *pAObj, const Tst_uint32 bps)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.decodeBPS = bps;
}

void
st_contOgg_opts_setLetDecoderPrintUsedBPS(Tst_contOgg_obj *pAObj,
		const Tst_bool printBPS)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.printDecUsedBPS = printBPS;
}

/*----------------------------------------------------------------------------*/

/* for handling raw PCM samples */
void
st_contOgg_opts_setCB_pcmClient(Tst_contOgg_obj *pAObj,
		ST_OPTARG(const Tst_av_cbPCM_prepareWriting cbPCMprepareWriting),
		ST_OPTARG(const Tst_av_cbPCM_setOutputFmt cbPCMsetOutputFmt),
		ST_OPTARG(const Tst_av_cbPCM_samplesClient cbPCMsamplesClient),
		ST_OPTARG(const Tst_av_cbPCM_finishWriting cbPCMfinishWriting),
		ST_OPTARG(void *pClientObj))
{
	Tst_contOgg_obj_intn *pAObjI;

	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;

	pAObjI->opts.cbPCMprepareWriting = cbPCMprepareWriting;
	pAObjI->opts.cbPCMsetOutputFmt   = cbPCMsetOutputFmt;
	pAObjI->opts.cbPCMsamplesClient  = cbPCMsamplesClient;
	pAObjI->opts.cbPCMfinishWriting  = cbPCMfinishWriting;
	pAObjI->opts.pPCMclientObj       = pClientObj;
	/* */
	pAObjI->opts.isPCMclientSet = (cbPCMprepareWriting != NULL &&
			cbPCMsetOutputFmt != NULL && cbPCMsamplesClient != NULL &&
			cbPCMfinishWriting != NULL && pClientObj != NULL);
}

/*----------------------------------------------------------------------------*/

/* read all frames ? */
/*void
st_contOgg_opts_setFlac_readAllFrames(Tst_contOgg_obj *pAObj,
		const Tst_bool rdAllFr)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.flacOpts.readAll =
			rdAllFr;
}*/

/* fully decode frames (requires readAll) ? */
void
st_contOgg_opts_setFlac_decodeFully(Tst_contOgg_obj *pAObj,
		const Tst_bool decFull)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.flacOpts.decodeFull =
			decFull;
}

/* check CRC of frame header ? */
void
st_contOgg_opts_setFlac_checkCRCofFrHd(Tst_contOgg_obj *pAObj,
		const Tst_bool check)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.flacOpts.checkCRCfh =
			check;
}

/* check CRC of entire frames ? */
void
st_contOgg_opts_setFlac_checkCRCofEntireFrames(Tst_contOgg_obj *pAObj,
		const Tst_bool check)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.flacOpts.checkCRCfr =
			check;
}

/* check MD5 of decoded audio ? */
void
st_contOgg_opts_setFlac_checkMD5ofDecodedAudio(Tst_contOgg_obj *pAObj,
		const Tst_bool check)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.flacOpts.checkMD5da =
			check;
}

/*----------------------------------------------------------------------------*/

void
st_contOgg_opts_setCB_vorbDecoding(Tst_contOgg_obj *pAObj,
		ST_OPTARG(const Tst_av_cbDecAud_hndNew cbHndNew),
		ST_OPTARG(const Tst_av_cbDecAud_hndDel cbHndDel),
		ST_OPTARG(const Tst_av_cbDecAud_setOutputFmt cbSetOutpFmt),
		ST_OPTARG(const Tst_av_cbDecAud_openFeed cbOpenFeed),
		ST_OPTARG(const Tst_av_cbDecAud_feedVrbHeader cbFeedHd),
		ST_OPTARG(const Tst_av_cbDecAud_feedVrbAudioAndDecode cbFeedAudAndDec),
		ST_OPTARG(const Tst_av_cbDecAud_closeFeed cbCloseFeed))
{
	Tst_contOgg_obj_intn *pAObjI;

	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	pAObjI = (Tst_contOgg_obj_intn*)pAObj->pObInternal;

	pAObjI->opts.vorbOpts.cbDecHndNew        = cbHndNew;
	pAObjI->opts.vorbOpts.cbDecHndDel        = cbHndDel;
	pAObjI->opts.vorbOpts.cbDecSetOutpFmt    = cbSetOutpFmt;
	pAObjI->opts.vorbOpts.cbDecOpenFeed      = cbOpenFeed;
	pAObjI->opts.vorbOpts.cbDecFeedHd        = cbFeedHd;
	pAObjI->opts.vorbOpts.cbDecFeedAudAndDec = cbFeedAudAndDec;
	pAObjI->opts.vorbOpts.cbDecCloseFeed     = cbCloseFeed;

	pAObjI->opts.vorbOpts.isDecoderSet =
			(pAObjI->opts.vorbOpts.cbDecHndNew != NULL &&
				pAObjI->opts.vorbOpts.cbDecHndDel != NULL &&
				pAObjI->opts.vorbOpts.cbDecSetOutpFmt != NULL &&
				pAObjI->opts.vorbOpts.cbDecOpenFeed != NULL &&
				pAObjI->opts.vorbOpts.cbDecFeedHd != NULL &&
				pAObjI->opts.vorbOpts.cbDecFeedAudAndDec != NULL &&
				pAObjI->opts.vorbOpts.cbDecCloseFeed != NULL);
}

void
st_contOgg_opts_setVorb_decodeFully(Tst_contOgg_obj *pAObj,
		const Tst_bool decFull)
{
	ST_ASSERTN_VOID(pAObj == NULL || pAObj->pObInternal == NULL)

	((Tst_contOgg_obj_intn*)pAObj->pObInternal)->opts.vorbOpts.decodeFull =
			decFull;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool
st_contOgg_opts_getAnlzMode(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.anlz;
}

Tst_bool
st_contOgg_opts_getCheckCRCofOGGframes(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.checkCRCofr;
}

Tst_bool
st_contOgg_opts_getReadComments(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.rdComments;
}

Tst_bool
st_contOgg_opts_getForceRewriteComments(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.forceRewrCmts;
}

/*----------------------------------------------------------------------------*/

/*Tst_bool
st_contOgg_opts_getFlac_readAllFrames(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.flacOpts.readAll;
}*/

Tst_bool
st_contOgg_opts_getFlac_decodeFully(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.flacOpts.decodeFull;
}

Tst_bool
st_contOgg_opts_getFlac_checkCRCofFrHd(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.flacOpts.checkCRCfh;
}

Tst_bool
st_contOgg_opts_getFlac_checkCRCofEntireFrames(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.flacOpts.checkCRCfr;
}

Tst_bool
st_contOgg_opts_getFlac_checkMD5ofDecodedAudio(Tst_contOgg_obj *pAObj)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pAObj == NULL || pAObj->pObInternal == NULL)

	return ((Tst_contOgg_obj_intn*)(pAObj->pObInternal))->opts.flacOpts.checkMD5da;
}

/******************************************************************************/
