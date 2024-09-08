/******************************************************************************/
/* mf_show_av.c                   [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for displaying Audio/Video info                                  */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 06.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/string_mte.h>
#include <santag/common/sys_fnc.h>
#include <santag/audvid/audvid.h>
#include <santag/audvid/aud_mpeg1.h>
#include <santag/audvid/cont_ogg.h>
#include <santag/audvid/cont_ogg_vorbcomm.h>
/** */
#define SRC_MF_SHOW_AV_C
#include "mf_show_av-pp.h"
/*** */
#include "mf_outp-pp.h"
#undef SRC_MF_SHOW_AV_C

/*
// System-Includes
*/
#include <string.h>      /* strchr(), memcpy() */
#include <stdlib.h>      /* calloc(), free(), ... */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Print MPEG-1 stream info
 *
 * pCmdln  MUST be set
 * pMO     MUST be set
 */
Tst_err
ast_mf_sw_showMPEG1stream(const Tast_cln_a *pCmdln, const Tst_bool printVBRhd,
		const Tst_mpeg1_obj *pMO)
{
	Tst_uint32 valUI;
	Tst_av_playtime playt;

	ST_ASSERTN_IARG(pCmdln == NULL || pMO == NULL)

	ST_AV_STC_RSETPT(playt)

	ast_mf_op_prFInf("AUD mpg MPEG-1 Audio v");

	switch (st_mpeg1_gs_getStr_audioVers(pMO)) {
		case ST_MPEG1_AUDVERS_25: ast_mf_op_prFInf("2.5"); break;
		case ST_MPEG1_AUDVERS_2 : ast_mf_op_prFInf("2.0"); break;
		case ST_MPEG1_AUDVERS_1 : ast_mf_op_prFInf("1.0"); break;
		default: ast_mf_op_prFInf("x.x");
	}
	ast_mf_op_prFInf(" layer ");
	switch (st_mpeg1_gs_getStr_layer(pMO)) {
		case ST_MPEG1_LAYER_3: ast_mf_op_prFInf("III"); break;
		case ST_MPEG1_LAYER_2: ast_mf_op_prFInf("II"); break;
		case ST_MPEG1_LAYER_1: ast_mf_op_prFInf("I"); break;
		default: ast_mf_op_prFInf("x");
	}
	/** Bitrate */
	valUI = st_mpeg1_gs_getStr_bitrate(pMO);
	if (valUI > 0) {
		ast_mf_op_prFInf(", %u kbit/s %s", valUI,
				(st_mpeg1_gs_getStr_isCBR(pMO) ? "CBR" : "VBR"));
		/*if (! st_mpeg1_gs_getStr_isCBR(pMO)) {
			if (printVBRhd &&
					(pMO->mhdVbr.vbrType == ST_MPEG1_VBRTYP_XING ||
						pMO->mhdVbr.vbrType == ST_MPEG1_VBRTYP_VBRI)) {
				ast_mf_op_prFInf("(%s",
						(pMO->mhdVbr.vbrType == ST_MPEG1_VBRTYP_XING ?
							"Xi" : "Fh"));
				if (pMO->mhdVbr.qual >= 0)
					ast_mf_op_prFInf(", q%d", pMO->mhdVbr.qual);
				ast_mf_op_prFInf(", mxbr%d, mobr%d)",
						pMO->bitrateMax, pMO->bitrateMost);
			} else
				ast_mf_op_prFInf("(%s)", "na");
		}*/
	} else if (st_mpeg1_gs_getStr_isFreeFmt(pMO)) {
		ast_mf_op_prFInf(", 0 kbit/s FF");
	} else {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_AUD))
			ast_mf_op_prFInf(", - - -");
		else
			ast_mf_op_prFInf(", 0 kbit/s n/a");
	}
	/** Sampling rate frequency index */
	valUI = st_mpeg1_gs_getStr_samplerate(pMO);
	ast_mf_op_prFInf(", %d %s, ",
			(valUI > 0 ? valUI : 0),
			(valUI > 0 ? "Hz" : "n/a"));
	/** Channel Mode */
	switch (st_mpeg1_gs_getStr_channMode(pMO)) {
	case ST_MPEG1_CMODE_STEREO:
		ast_mf_op_prFInf("stereo"); break;
	case ST_MPEG1_CMODE_JSTEREO:
		ast_mf_op_prFInf("j-stereo"); break;
	case ST_MPEG1_CMODE_DUAL:
		ast_mf_op_prFInf("dual-ch"); break;
	case ST_MPEG1_CMODE_SINGLE:
		ast_mf_op_prFInf("single-ch"); break;
	default:
		ast_mf_op_prFInf("unknown channMode");
	}
	/** Protection */
	/*ast_mf_op_prFInf(", CRC %c",
			(st_mpeg1_gs_getStr_hasCRC(pMO) ? 'y' : 'n'));*/
	/** */
	ast_mf_op_prFInf("\n");

	/* playtime */
	ast_mf_op_prFInf("AUD mpg ");
	st_mpeg1_gs_getStr_playtime(pMO, &playt);
	AST_MF__sw_printPlaytime(&playt, ST_B_FALSE);
	ast_mf_op_prFInf("\n");

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Print OGG streams' info
 *
 * pAObj  MUST be set
 */
Tst_err
ast_mf_sw_showOGGstreams(const Tast_cln_a *pCmdln, const Tst_contOgg_obj *pOgg)
{
#	define LOC_PR_OGG_(tpStr, stpStr)  { \
				ast_mf_op_prFInf("%s %s ", tpStr, stpStr); \
				ast_mf_sw_printOGGstrNr(pOgg, grpIx, grpSIx, \
						/*prForAudioOrTag:*/ST_B_TRUE, \
						&is1grp, &delML); }
#	define LOC_PR_OGGCOD_(tpStr, stpStr, codStr)  { \
				ast_mf_op_prFInf("%s %s ", tpStr, stpStr); \
				ast_mf_sw_printOGGstrNr(pOgg, grpIx, grpSIx, \
						/*prForAudioOrTag:*/ST_B_TRUE, &is1grp, &delML); \
				ast_mf_op_prFInf("%s", codStr); }
	Tst_uint32 delML,
	           grpIx     = 0,
	           grpSIx    = 0;
	Tst_bool   oneSet,
	           is1grp    = ST_B_FALSE,
	           isSrcOoF;
	Tst_str    *pTmpS    = NULL,
	           *pVorVend = NULL;
	Tst_av_playtime                  playt;
	Tst_contOgg_substream            *pBS = NULL;
	Tst_contOgg_vorbCommentRaw const *pVRC;
	Tst_contOgg_vorbisInfo           infVor;
	Tst_contOgg_speexInfo            infSpx;
	Tst_contOgg_celtInfo             infClt;
	Tst_contOgg_flacInfo             infFlc;
	Tst_contOgg_diracInfo            infDrc;
	Tst_contOgg_theoraInfo           infThe;

	ST_ASSERTN_IARG(pCmdln == NULL || pOgg == NULL)

	ST_AV_STC_RSETPT(playt)
	st_contOgg_stc_rsetVorbisInfo(&infVor);
	st_contOgg_stc_rsetSpeexInfo(&infSpx);
	st_contOgg_stc_rsetCeltInfo(&infClt);
	st_contOgg_stc_rsetFlacInfo(&infFlc);
	st_contOgg_stc_rsetDiracInfo(&infDrc);
	st_contOgg_stc_rsetTheoraInfo(&infThe);

	while ((pBS = st_contOgg_ite_getNextStream_any(pOgg, pBS)) != NULL) {
		delML  = 0;
		oneSet = ST_B_FALSE;
		st_contOgg_gs_getStr_groupIndex(pBS, &grpIx, &grpSIx);
		/* codec */
		switch (st_contOgg_gs_getStr_type(pBS)) {
		case ST_CONTOGG_BSTP_VORB:  /* VORBIS - Audio */
			LOC_PR_OGGCOD_("AUD", "ogg", "Vorbis")
			ast_mf_op_prFInf(", ");
			if (st_contOgg_gs_getStr_vorbisInfo(pBS, &infVor)) {
				/* bitrates */
				if (infVor.bitrateMinKBS > 0 && infVor.bitrateMaxKBS > 0) {
					ast_mf_op_prFInf("avg %u",
							(infVor.bitrateMinKBS + infVor.bitrateMaxKBS) / 2);
					oneSet = ST_B_TRUE;
				} else {
					if (infVor.bitrateMinKBS > 0) {
						ast_mf_op_prFInf("min %u", infVor.bitrateMinKBS);
						oneSet = ST_B_TRUE;
					}
					if (st_contOgg_gs_getStr_avgBitrateInKBS(pBS) > 0) {
						ast_mf_op_prFInf("%savg %u",
								(oneSet ? "," : ""),
								st_contOgg_gs_getStr_avgBitrateInKBS(pBS));
						oneSet = ST_B_TRUE;
					}
					if (infVor.bitrateNomKBS > 0) {
						ast_mf_op_prFInf("%snom %u",
								(oneSet ? "," : ""), infVor.bitrateNomKBS);
						oneSet = ST_B_TRUE;
					}
					if (infVor.bitrateMaxKBS > 0) {
						ast_mf_op_prFInf("%smax %u",
								(oneSet ? "," : ""), infVor.bitrateMaxKBS);
						oneSet = ST_B_TRUE;
					}
				}
				if (! oneSet)
					ast_mf_op_prFInf("n/a");
				ast_mf_op_prFInf(" kbit/s, ");
				/* sample rate */
				ast_mf_op_prFInf("%u %s, ",
						infVor.sampleRate,
						(infVor.sampleRate > 0 ? "Hz" : "n/a"));
				/* channels */
				ast_mf_op_prFInf("%u channel%s", infVor.channels,
						(infVor.channels == 1 ? "" : "s"));
				/* vendor */
				pVRC = st_contOgg_vorbComm_gs_getRawComment(pBS);
				if (pVRC != NULL) {
					st_contOgg_vorbComm_gs_getRC_vendor(pVRC, &pVorVend);
					if (st_sysStrlen(pVorVend) > 0) {
						LOC_PR_OGG_("\nAUD", "ogg")
						delML += 8 + 10;
						AST_MF__sw_shortPCh(pVorVend, &pTmpS, delML);
						ast_mf_op_prFInf(" vendor '%s'", pTmpS);
						ST_DELPOINT(pTmpS)
					}
					ST_DELPOINT(pVorVend)
				}
				/* playtime */
				LOC_PR_OGG_("\nAUD", "ogg")
				st_contOgg_gs_getStr_playtime(pBS, &playt);
				AST_MF__sw_printPlaytime(&playt, ST_B_FALSE);
			} else
				ast_mf_op_prFInf("n/a");
			break;
		case ST_CONTOGG_BSTP_FLAC:  /* FLAC - Audio */
			isSrcOoF = st_contOgg_gs_getIsSrcOggOrFlac(pOgg);
			LOC_PR_OGGCOD_("AUD", (isSrcOoF ? "ogg" : "flc"), "Flac")
			ast_mf_op_prFInf(", ");
			if (st_contOgg_gs_getStr_flacInfo(pBS, &infFlc)) {
				/* bitrates */
				if (st_contOgg_gs_getStr_avgBitrateInKBS(pBS) > 0) {
					ast_mf_op_prFInf("avg %u",
							st_contOgg_gs_getStr_avgBitrateInKBS(pBS));
					oneSet = ST_B_TRUE;
				}
				if (! oneSet)
					ast_mf_op_prFInf("n/a");
				ast_mf_op_prFInf(" kbit/s, ");
				/* sample rate */
				ast_mf_op_prFInf("%u %s, ",
						infFlc.sampleRate,
						(infFlc.sampleRate > 0 ? "Hz" : "n/a"));
				/* channels */
				ast_mf_op_prFInf("%u channel%s, ", infFlc.channels,
						(infFlc.channels == 1 ? "" : "s"));
				/* bits per sample */
				ast_mf_op_prFInf("%u bits/sample", infFlc.bitsPerSample);
				/* version */
				if (isSrcOoF) {
					LOC_PR_OGG_("\nAUD", "ogg")
					ast_mf_op_prFInf(" vers %u.%u",
							infFlc.oggFlacVersMaj, infFlc.oggFlacVersMin);
				}
				/* vendor */
				pVRC = st_contOgg_vorbComm_gs_getRawComment(pBS);
				if (pVRC != NULL) {
					st_contOgg_vorbComm_gs_getRC_vendor(pVRC, &pVorVend);
					if (st_sysStrlen(pVorVend) > 0) {
						LOC_PR_OGG_("\nAUD", (isSrcOoF ? "ogg" : "flc"))
						delML += 8 + 10;
						AST_MF__sw_shortPCh(pVorVend, &pTmpS, delML);
						ast_mf_op_prFInf(" vendor '%s'", pTmpS);
						ST_DELPOINT(pTmpS)
					}
					ST_DELPOINT(pVorVend)
				}
				/* playtime */
				LOC_PR_OGG_("\nAUD", (isSrcOoF ? "ogg" : "flc"))
				st_contOgg_gs_getStr_playtime(pBS, &playt);
				AST_MF__sw_printPlaytime(&playt, ST_B_FALSE);
			} else
				ast_mf_op_prFInf("n/a");
			break;
		case ST_CONTOGG_BSTP_SPEE:  /* SPEEX - Audio */
			LOC_PR_OGGCOD_("AUD", "ogg", "Speex")
			ast_mf_op_prFInf(", ");
			if (st_contOgg_gs_getStr_speexInfo(pBS, &infSpx)) {
				/* bitrate */
				if (st_contOgg_gs_getStr_avgBitrateInKBS(pBS) > 0) {
					ast_mf_op_prFInf("avg %u",
							st_contOgg_gs_getStr_avgBitrateInKBS(pBS));
					oneSet = ST_B_TRUE;
				}
				if (infSpx.bitrateKBS > 0 && infSpx.isVBR) {
					ast_mf_op_prFInf("%sVBR max %u",
							(oneSet ? "," : ""), infSpx.bitrateKBS);
					oneSet = ST_B_TRUE;
				}
				if (! oneSet)
					ast_mf_op_prFInf("n/a");
				ast_mf_op_prFInf(" kbit/s, ");
				/* sample rate */
				ast_mf_op_prFInf("%u %s, ",
						infSpx.sampleRate,
						(infSpx.sampleRate > 0 ? "Hz" : "n/a"));
				/* channels */
				ast_mf_op_prFInf("%s",
						(infSpx.channels == 1 ? "mono" :
							infSpx.channels == 2 ? "stereo" : "n/a"));
				/* versions */
				LOC_PR_OGG_("\nAUD", "ogg")
				ast_mf_op_prFInf(" vers '%s', versID %u%s, bitstream versID %u%s",
						infSpx.versStr,
						(infSpx.speexVers << 1) >> 1,
						((Tst_int32)infSpx.speexVers < 0 ? " (exp)" :  ""),
						(infSpx.bitstrVers << 1) >> 1,
						((Tst_int32)infSpx.bitstrVers < 0 ? " (exp)" :  ""));
				/* vendor */
				pVRC = st_contOgg_vorbComm_gs_getRawComment(pBS);
				if (pVRC != NULL) {
					st_contOgg_vorbComm_gs_getRC_vendor(pVRC, &pVorVend);
					if (st_sysStrlen(pVorVend) > 0) {
						LOC_PR_OGG_("\nAUD", "ogg")
						delML += 8 + 10;
						AST_MF__sw_shortPCh(pVorVend, &pTmpS, delML);
						ast_mf_op_prFInf(" vendor '%s'", pTmpS);
						ST_DELPOINT(pTmpS)
					}
					ST_DELPOINT(pVorVend)
				}
				/* playtime */
				LOC_PR_OGG_("\nAUD", "ogg")
				st_contOgg_gs_getStr_playtime(pBS, &playt);
				AST_MF__sw_printPlaytime(&playt, ST_B_FALSE);
			} else
				ast_mf_op_prFInf("n/a");
			break;
		case ST_CONTOGG_BSTP_CELT:  /* CELT - Audio */
			LOC_PR_OGGCOD_("AUD", "ogg", "CELT")
			ast_mf_op_prFInf(", ");
			if (st_contOgg_gs_getStr_celtInfo(pBS, &infClt)) {
				/* bitrate */
				if (st_contOgg_gs_getStr_avgBitrateInKBS(pBS) > 0) {
					ast_mf_op_prFInf("avg %u",
							st_contOgg_gs_getStr_avgBitrateInKBS(pBS));
					oneSet = ST_B_TRUE;
				}
				if (! oneSet)
					ast_mf_op_prFInf("n/a");
				ast_mf_op_prFInf(" kbit/s, ");
				/* sample rate */
				ast_mf_op_prFInf("%u %s, ",
						infClt.sampleRate,
						(infClt.sampleRate > 0 ? "Hz" : "n/a"));
				/* channels */
				ast_mf_op_prFInf("%s",
						(infClt.channels == 1 ? "mono" :
							infClt.channels == 2 ? "stereo" : "n/a"));
				/* versions */
				LOC_PR_OGG_("\nAUD", "ogg")
				ast_mf_op_prFInf(" vers '%s', bitstream versID %u%s",
						infClt.versStr,
						(infClt.bitstrVers << 1) >> 1,
						((Tst_int32)infClt.bitstrVers < 0 ? " (exp)" :  ""));
				/* vendor */
				pVRC = st_contOgg_vorbComm_gs_getRawComment(pBS);
				if (pVRC != NULL) {
					st_contOgg_vorbComm_gs_getRC_vendor(pVRC, &pVorVend);
					if (st_sysStrlen(pVorVend) > 0) {
						LOC_PR_OGG_("\nAUD", "ogg")
						delML += 8 + 10;
						AST_MF__sw_shortPCh(pVorVend, &pTmpS, delML);
						ast_mf_op_prFInf(" vendor '%s'", pTmpS);
						ST_DELPOINT(pTmpS)
					}
					ST_DELPOINT(pVorVend)
				}
				/* playtime */
				LOC_PR_OGG_("\nAUD", "ogg")
				st_contOgg_gs_getStr_playtime(pBS, &playt);
				AST_MF__sw_printPlaytime(&playt, ST_B_FALSE);
			} else
				ast_mf_op_prFInf("n/a");
			break;
		case ST_CONTOGG_BSTP_PCM:   /* PCM - Audio */
			LOC_PR_OGGCOD_("AUD", "ogg", "PCM"); break;
		case ST_CONTOGG_BSTP_MIDI:  /* MIDI - Audio */
			LOC_PR_OGGCOD_("AUD", "ogg", "MIDI"); break;
		case ST_CONTOGG_BSTP_THEO:  /* THEORA - Video */
			LOC_PR_OGGCOD_("VID", "ogg", "Theora")
			ast_mf_op_prFInf(", ");
			if (st_contOgg_gs_getStr_theoraInfo(pBS, &infThe)) {
				/* bitrate */
				if (infThe.bitrateKBS > 0)
					ast_mf_op_prFInf("%u", infThe.bitrateKBS);
				else
					ast_mf_op_prFInf("n/a");
				ast_mf_op_prFInf(" kbit/s, ");
				/* quality */
				if (infThe.quality != 0)
					ast_mf_op_prFInf("%u qual, ", infThe.quality);
				/* */
				ast_mf_op_prFInf("%ux%u dim, %.3f fps",
						infThe.width, infThe.height, infThe.fps);
				/* */
				LOC_PR_OGG_("\nVID", "ogg")
				ast_mf_op_prFInf(" vers %u.%u.%u",
						infThe.versMaj, infThe.versMin, infThe.versRev);
				/* vendor */
				pVRC = st_contOgg_vorbComm_gs_getRawComment(pBS);
				if (pVRC != NULL) {
					st_contOgg_vorbComm_gs_getRC_vendor(pVRC, &pVorVend);
					if (st_sysStrlen(pVorVend) > 0) {
						LOC_PR_OGG_("\nVID", "ogg")
						delML += 8 + 10;
						AST_MF__sw_shortPCh(pVorVend, &pTmpS, delML);
						ast_mf_op_prFInf(" vendor '%s'", pTmpS);
						ST_DELPOINT(pTmpS)
					}
					ST_DELPOINT(pVorVend)
				}
				/* playtime */
				LOC_PR_OGG_("\nVID", "ogg")
				st_contOgg_gs_getStr_playtime(pBS, &playt);
				AST_MF__sw_printPlaytime(&playt, ST_B_TRUE);
			} else
				ast_mf_op_prFInf("n/a");
			break;
		case ST_CONTOGG_BSTP_DIRA:  /* Dirac - Video */
			LOC_PR_OGGCOD_("VID", "ogg", "Dirac")
			ast_mf_op_prFInf(", ");
			if (st_contOgg_gs_getStr_diracInfo(pBS, &infDrc)) {
				/* bitrate */
				/*if (bs.compBitrateAvgKBS > 0)
					ast_mf_op_prFInf("avg %u", bs.compBitrateAvgKBS);
				else
					ast_mf_op_prFInf("n/a");
				ast_mf_op_prFInf(" kbit/s, ");*/
				/* */
				ast_mf_op_prFInf("%ux%u dim, %.3f fps",
						infDrc.width, infDrc.height, infDrc.fps);
				/* */
				LOC_PR_OGG_("\nVID", "ogg")
				ast_mf_op_prFInf(" vers %u.%u",
						infDrc.versMaj, infDrc.versMin);
			} else
				ast_mf_op_prFInf("n/a");
			break;
		case ST_CONTOGG_BSTP_UVS:   /* UVS - Video */
			LOC_PR_OGGCOD_("VID", "ogg", "UVS"); break;
		case ST_CONTOGG_BSTP_YUV:   /* YUV4MPEG2 - Video */
			LOC_PR_OGGCOD_("VID", "ogg", "YUV4MPEG2"); break;
		case ST_CONTOGG_BSTP_CMML:  /* CMML - Text */
			LOC_PR_OGGCOD_("TXT", "ogg", "CMML"); break;
		case ST_CONTOGG_BSTP_KATE:  /* Kate - Text */
			LOC_PR_OGGCOD_("TXT", "ogg", "Kate"); break;
		case ST_CONTOGG_BSTP_PNG:   /* PNG - Picture */
			LOC_PR_OGGCOD_("PIC", "ogg", "PNG"); break;
		case ST_CONTOGG_BSTP_JNG:   /* JNG - Picture */
			LOC_PR_OGGCOD_("PIC", "ogg", "JNG"); break;
		case ST_CONTOGG_BSTP_MNG:   /* MNG - Animation */
			LOC_PR_OGGCOD_("ANI", "ogg", "MNG"); break;
		case ST_CONTOGG_BSTP_SKEL:  /* Skeleton */
			LOC_PR_OGGCOD_("MET", "ogg", "Skeleton"); break;
		default:
			LOC_PR_OGGCOD_("AUD", "ogg", "unknown codec")
		}
		ast_mf_op_prFInf("\n");
	}

	return ST_ERR_SUCC;
#	undef LOC_PR_OGG_
#	undef LOC_PR_OGGCOD_
}

/*----------------------------------------------------------------------------*/

/*
 * Print OGG stream number
 */
void
ast_mf_sw_printOGGstrNr(const Tst_contOgg_obj *pOgg,
		const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		const Tst_bool prForAudioOrTag,
		Tst_bool *pIs1Grp, Tst_uint32 *pDelML)
{
	char       msg[128];
	char const *pTheEnd;
	Tst_contOgg_substream *pBS = NULL;

	*pDelML = 0;
	if (st_contOgg_gs_getStreamCount(pOgg) < 2)
		return;
	if (! *pIs1Grp) {
		while ((pBS = st_contOgg_ite_getNextStream_any(pOgg, pBS)) != NULL)
			if (st_contOgg_gs_getStr_isGrouped(pBS)) {
				*pIs1Grp = ST_B_TRUE;
				break;
			}
	}
	pTheEnd = (prForAudioOrTag ? ": " : "");
	if (bsSIx > 0)
		sprintf((char*)&msg, "bs#%u.%u%s", bsIx, bsSIx, pTheEnd);
	else if (*pIs1Grp) {
		if (prForAudioOrTag)
			sprintf((char*)&msg, "bs#%u  %s", bsIx, pTheEnd);
		else
			sprintf((char*)&msg, "bs#%u%s", bsIx, pTheEnd);
	} else
		sprintf((char*)&msg, "bs#%u%s", bsIx, pTheEnd);
	ast_mf_op_prFInf((char*)&msg);
	*pDelML = st_sysStrlen2((char*)&msg);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Shorten ISO-encoded string
 */
static Tst_err
AST_MF__sw_shortPCh(const Tst_str *pPChar, Tst_str **ppShort,
		const Tst_uint32 delML)
{
	const char *cDOTDOT = " [..]";
	Tst_err    res    = ST_ERR_SUCC;
	Tst_uint32 slen   = st_sysStrlen(pPChar),
	           ddLen  = st_sysStrlen2(cDOTDOT),
	           slenT,
	           maxlen = ast_g_mf_op_termLnLen;
	Tst_bool   shortd = ST_B_FALSE;

	if (delML > 0) {
		if (maxlen > delML)
			maxlen -= delML;
		else
			maxlen = 0;
	}
	if (maxlen < ddLen)
		maxlen = 3 + ddLen;

	slenT = slen;
	if (slenT > maxlen) {
		slenT  = maxlen;
		shortd = ST_B_TRUE;
	}
	ST_REALLOC(*ppShort, Tst_str*, slenT + 1, 1)
	if (*ppShort == NULL)
		return ST_ERR_OMEM;
	memcpy(*ppShort, pPChar, slenT - (shortd ? ddLen : 0));
	if (shortd)
		memcpy(&(*ppShort)[slenT - ddLen], cDOTDOT, ddLen);
	(*ppShort)[slenT] = 0x00;

	if (strchr((const char*)*ppShort, '%') != NULL) {
		Tst_mtes_string tmpMTES;

		st_mtes_stc_initStr(&tmpMTES);
		res = st_mtes_copyFromCharp(*ppShort, slenT + 1,
				ST_MTES_TE_UTF8, &tmpMTES);
		if (res == ST_ERR_SUCC)
			res = st_mtes_strReplace(&tmpMTES,
					(const Tst_str*)"%", (const Tst_str*)"%%", NULL);
		if (res == ST_ERR_SUCC)
			res = st_mtes_copyToCharp_utf8(&tmpMTES, ppShort);
		st_mtes_stc_freeStr(&tmpMTES);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Print playtime
 */
static void
AST_MF__sw_printPlaytime(const Tst_av_playtime *pPTime,
		const Tst_bool isCirca)
{
	ast_mf_op_prFInf(" time ");
	if (pPTime->ptHours != 0 || pPTime->ptMins != 0 || pPTime->ptSecs != 0) {
		if (pPTime->ptHours > 0)
			ast_mf_op_prFInf("%u:", pPTime->ptHours);
		ast_mf_op_prFInf("%02u:%02u ", pPTime->ptMins, pPTime->ptSecs);
		if (pPTime->ptHours > 0)
			ast_mf_op_prFInf("hh:");
		ast_mf_op_prFInf("mm:ss");
		if (isCirca)
			ast_mf_op_prFInf(" (ca.)");
	} else
		ast_mf_op_prFInf("unknown");
}

/******************************************************************************/
