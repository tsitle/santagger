/******************************************************************************/
/* aud_mpeg1_opts.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for MPEG-1 options                                               */
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
/** */
#include "src/includes/audvid/aud_mpeg1.h"
/** */
#define SRC_AUD_MPEG1_OPTS_C
#include "aud_mpeg1-prot.h"
#undef SRC_AUD_MPEG1_OPTS_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
st_mpeg1_opts_setBasicOpts(Tst_mpeg1_obj *pMO,
		ST_OPTARG(const Tst_basOpts *pBOpts))
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	if (pBOpts == NULL)
		ST_RSETSTC_BASOPTS(
				((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.basOpts)
	else
		ST_COPYSTC_BASOPTS(*pBOpts,
				((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.basOpts);
}

void
st_mpeg1_opts_setAnlzMode(Tst_mpeg1_obj *pMO,
		const Tst_bool anlz, ST_OPTARG(const Tst_cb_ana_msg cbAnlz))
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.cbAnaMsg = cbAnlz;
	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.anlz     =
			(anlz && cbAnlz != NULL);
}

void
st_mpeg1_opts_setQuickScan(Tst_mpeg1_obj *pMO, const Tst_bool doQuickScan)
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.quickScan = doQuickScan;
}

/*----------------------------------------------------------------------------*/

void
st_mpeg1_opts_setCB_pcmClient(Tst_mpeg1_obj *pMO,
		ST_OPTARG(const Tst_av_cbPCM_prepareWriting cbPCMprepareWriting),
		ST_OPTARG(const Tst_av_cbPCM_setOutputFmt cbPCMsetOutputFmt),
		ST_OPTARG(const Tst_av_cbPCM_samplesClient cbPCMsamplesClient),
		ST_OPTARG(const Tst_av_cbPCM_finishWriting cbPCMfinishWriting),
		ST_OPTARG(void *pClientObj))
{
	Tst_mpeg1_obj_intn *pMOI;

	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	pMOI = (Tst_mpeg1_obj_intn*)pMO->pObInternal;

	pMOI->opts.cbPCMprepareWriting = cbPCMprepareWriting;
	pMOI->opts.cbPCMsetOutputFmt   = cbPCMsetOutputFmt;
	pMOI->opts.cbPCMsamplesClient  = cbPCMsamplesClient;
	pMOI->opts.cbPCMfinishWriting  = cbPCMfinishWriting;
	pMOI->opts.pPCMclientObj       = pClientObj;

	pMOI->opts.isPCMclientSet = (pMOI->opts.cbPCMprepareWriting != NULL &&
			pMOI->opts.cbPCMsetOutputFmt != NULL &&
			pMOI->opts.cbPCMsamplesClient != NULL &&
			pMOI->opts.cbPCMfinishWriting != NULL &&
			pMOI->opts.pPCMclientObj != NULL);
}

void
st_mpeg1_opts_setCB_decoding(Tst_mpeg1_obj *pMO,
		ST_OPTARG(const Tst_av_cbDecAud_hndNew cbHndNew),
		ST_OPTARG(const Tst_av_cbDecAud_hndDel cbHndDel),
		ST_OPTARG(const Tst_av_cbDecAud_setOutputFmt cbSetOutpFmt),
		ST_OPTARG(const Tst_av_cbDecAud_openFeed cbOpenFeed),
		ST_OPTARG(const Tst_av_cbDecAud_feedMpgAudio cbFeedInp),
		ST_OPTARG(const Tst_av_cbDecAud_decodeMpg cbDecode),
		ST_OPTARG(const Tst_av_cbDecAud_closeFeed cbCloseFeed))
{
	Tst_mpeg1_obj_intn *pMOI;

	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	pMOI = (Tst_mpeg1_obj_intn*)pMO->pObInternal;

	pMOI->opts.cbDecHndNew     = cbHndNew;
	pMOI->opts.cbDecHndDel     = cbHndDel;
	pMOI->opts.cbDecSetOutpFmt = cbSetOutpFmt;
	pMOI->opts.cbDecOpenFeed   = cbOpenFeed;
	pMOI->opts.cbDecFeedInp    = cbFeedInp;
	pMOI->opts.cbDecDecode     = cbDecode;
	pMOI->opts.cbDecCloseFeed  = cbCloseFeed;

	pMOI->opts.isDecoderSet = (pMOI->opts.cbDecHndNew != NULL &&
			pMOI->opts.cbDecHndDel != NULL &&
			pMOI->opts.cbDecSetOutpFmt != NULL &&
			pMOI->opts.cbDecOpenFeed != NULL &&
			pMOI->opts.cbDecFeedInp != NULL &&
			pMOI->opts.cbDecDecode != NULL &&
			pMOI->opts.cbDecCloseFeed != NULL);
}

/*----------------------------------------------------------------------------*/

void
st_mpeg1_opts_setDecodeFully(Tst_mpeg1_obj *pMO, const Tst_bool decFull)
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.decodeFull = decFull;
}

void
st_mpeg1_opts_setPCMoutputBPS(Tst_mpeg1_obj *pMO, const Tst_uint32 bps)
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.decodeBPS = bps;
}

void
st_mpeg1_opts_setLetDecoderPrintUsedBPS(Tst_mpeg1_obj *pMO,
		const Tst_bool printBPS)
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.printDecUsedBPS = printBPS;
}

/*----------------------------------------------------------------------------*/

void
st_mpeg1_opts_setForceLayer(Tst_mpeg1_obj *pMO, const Tst_mpeg1_layer lay)
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.forceMLay = lay;
}

void
st_mpeg1_opts_setForceAudVers(Tst_mpeg1_obj *pMO, const Tst_mpeg1_audioVers av)
{
	ST_ASSERTN_VOID(pMO == NULL || pMO->pObInternal == NULL)

	((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.forceMAv = av;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_bool
st_mpeg1_opts_getDecodeFully(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.decodeFull;
}

/*----------------------------------------------------------------------------*/

Tst_mpeg1_layer
st_mpeg1_opts_getForceLayer(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(ST_MPEG1_LAYER_NONE,
			pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.forceMLay;
}

Tst_mpeg1_audioVers
st_mpeg1_opts_getForceAudVers(const Tst_mpeg1_obj *pMO)
{
	ST_ASSERTN_NUM(ST_MPEG1_AUDVERS_NONE,
			pMO == NULL || pMO->pObInternal == NULL)

	return ((Tst_mpeg1_obj_intn*)pMO->pObInternal)->opts.forceMAv;
}

/******************************************************************************/
