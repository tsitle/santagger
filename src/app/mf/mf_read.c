/******************************************************************************/
/* mf_read.c                      [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for reading Audio/Video/Tag files                                */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.10.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/sys_math.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/audvid/aud_mpeg1.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/audvid/cont_wav.h"
#include "src/includes/audvid/cont_ogg_vorbcomm.h"
#include "src/includes/tag/tag_basics.h"
#include "src/includes/tag/tag_id3v1.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_vorbc.h"
#include "src/includes/tag/tag_meta.h"
/** */
#define SRC_MF_READ_C
#include "mf_read-pp.h"
/*** */
#include "mf_read_decmpg1-pp.h"
#include "mf_read_decvrb-pp.h"
#include "mf_fnc-pp.h"
#include "mf_outp-pp.h"
#undef SRC_MF_READ_C

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc(), random() */
#include <string.h>      /* memset() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Reads current from file
 *
 * pMF         MUST be set
 * pCmdln      MUST be set
 *
 * returns: ERRCODE
 */
Tst_err
ast_mf_rd_readFile(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	#define LOC_PRDEB_FFMT_MTP_  \
			ast_mf_op_d_deb(cFNCN, \
					"this file format shouldn't contain this data type");
	#define LOC_PRDEB_SKPD_  { \
				if (skpd > 0) { \
					ast_mf_op_d_deb(cFNCN, \
							"  (had to skip "ST_TFSIZE_PRF_LU" bytes)", \
							(Tst_fsize)skpd); \
				} \
			}
	#define LOC_PRMULTSTR_  { \
				snprintf(debMsg, sizeof(debMsg), \
						"already has audio/video stream, aborting"); \
				if (pCmdln->cmds.anlz || \
						ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) { \
					if (pCmdln->cmds.anlz) { \
						ast_mf_op_cb_anaMsg("STR: %s", debMsg); \
					} else { \
						ast_mf_op_d_deb(cFNCN, "stream %s", debMsg); \
					} \
				} else if (! pCmdln->opts.quiet) { \
					ast_mf_op_prMsg("(Stream %s)", debMsg); \
				} \
			}

	const char *cFNCN = __func__;
	Tst_err    res    = ST_ERR_SUCC;
	Tst_bool   cont   = ST_B_TRUE;  /* continue with loop ? */
	Tst_bool   isEOF  = ST_B_FALSE;
	Tst_bool   doSkip = ST_B_FALSE;
	Tst_bool   doQuick;
	Tst_fsize  skpd   = 0;     /* amount of skipped bytes */
	Tst_foffs  lastPos;
	char       debMsg[512];
	Tst_buf    tbuf[20];
	Tast_mf__rd_jumpState jState;
	Tst_utilsFmt_mimeTp   mimeTp = ST_UTILSFMT_MTP_NONE;

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL)

	if (pCmdln->cmds.anlz)
		ast_mf_op_cb_anaMsg("STR: reading input stream...");
	else if (pCmdln->opts.showStat)
		ast_mf_op_prMsg("*Reading input stream...");

	AST_MF__rd_stc_rsetJState(jState);
	doQuick = pCmdln->cmds.rd && pCmdln->opts.quickScan;

	while (cont && ! st_streamrd_isEOF(&pMF->strrd)) {
		lastPos = st_streamrd_getCurPos(&pMF->strrd);
		/* */
		res = AST_MF__rd_getNextMimeType(cFNCN, pCmdln, pMF,
				tbuf, sizeof(tbuf), &isEOF, &mimeTp);
		if (res != ST_ERR_SUCC || isEOF)
			break;
		/* */
		switch (mimeTp) {
		case ST_UTILSFMT_MTP_AUDFLAC:   /* Flac audio */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_deb(cFNCN,
						"found data type: Flac Audio");
				if (pMF->ffmt != AST_MF_FFMT_FLAC)
					LOC_PRDEB_FFMT_MTP_
				LOC_PRDEB_SKPD_
			}
			/* */
			if (AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_AUD) ||
					AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_VID)) {
				LOC_PRMULTSTR_
				cont = ST_B_FALSE;
			} else {
				res = AST_MF__rd_readAV(pCmdln, pMF, ST_UTILSFMT_MTP_AUDFLAC);
				if (res != ST_ERR_SUCC) {
					cont = ST_B_FALSE;
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"reading A/V failed");
				}
				/* */
				if (res == ST_ERR_SUCC && doQuick) {
					/* jump to next tag */
					res  = AST_MF__rd_jumpTo(pCmdln, pMF, &jState);
					cont = (res == ST_ERR_SUCC);
				}
			}
			skpd = 0;
			break;
		case ST_UTILSFMT_MTP_AUDMPEG1:   /* MPEG-1 audio */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_deb(cFNCN,
						"found data type: MPEG-I Audio");
				if (pMF->ffmt != AST_MF_FFMT_MPG1)
					LOC_PRDEB_FFMT_MTP_
				LOC_PRDEB_SKPD_
			}
			/* */
			if (AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_AUD) ||
					AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_VID)) {
				LOC_PRMULTSTR_
				cont = ST_B_FALSE;
			} else {
				res = AST_MF__rd_readAV(pCmdln, pMF, ST_UTILSFMT_MTP_AUDMPEG1);
				if (res != ST_ERR_SUCC) {
					cont = ST_B_FALSE;
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"reading A/V failed");
				}
				/* */
				if (res == ST_ERR_SUCC && doQuick) {
					/* jump to next tag */
					res  = AST_MF__rd_jumpTo(pCmdln, pMF, &jState);
					cont = (res == ST_ERR_SUCC);
				}
			}
			skpd = 0;
			break;
		case ST_UTILSFMT_MTP_AUDVIDOGG:  /* OGG audio/video */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_deb(cFNCN,
						"found data type: OGG Audio/Video");
				if (pMF->ffmt != AST_MF_FFMT_OGG)
					LOC_PRDEB_FFMT_MTP_
				LOC_PRDEB_SKPD_
			}
			/* */
			if (AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_AUD) ||
					AST_MF_HASCONTTYPE(pMF, AST_MF_CTP_VID)) {
				LOC_PRMULTSTR_
				cont = ST_B_FALSE;
			} else {
				res = AST_MF__rd_readAV(pCmdln, pMF, ST_UTILSFMT_MTP_AUDVIDOGG);
				if (res != ST_ERR_SUCC) {
					cont = ST_B_FALSE;
					ast_mf_op_d_mfErr(pMF, cFNCN,
							"reading A/V failed");
				}
				/* */
				if (res == ST_ERR_SUCC && doQuick) {
					/* jump to next tag */
					res  = AST_MF__rd_jumpTo(pCmdln, pMF, &jState);
					cont = (res == ST_ERR_SUCC);
				}
			}
			skpd = 0;
			break;
		case ST_UTILSFMT_MTP_APPTAGIV1:   /* ID3v1 Tag */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_deb(cFNCN,
						"found data type: ID3v1 Tag");
				if (pMF->ffmt != AST_MF_FFMT_TAGF &&
						pMF->ffmt != AST_MF_FFMT_MPG1)
					LOC_PRDEB_FFMT_MTP_
				LOC_PRDEB_SKPD_
			}
			res = AST_MF__rd_readTag(pCmdln, pMF, ST_MTAG_TTP_IV1);
			if (res != ST_ERR_SUCC) {
				cont = ST_B_FALSE;
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"reading tag failed");
			}
			skpd = 0;
			/* */
			if (res == ST_ERR_SUCC && doQuick &&
					jState.jumped && pMF->ffmt != AST_MF_FFMT_TAGF) {
				/* jump to next tag before ID3v1 */
				res  = AST_MF__rd_jumpTo(pCmdln, pMF, &jState);
				cont = (res == ST_ERR_SUCC);
			}
			break;
		case ST_UTILSFMT_MTP_APPTAGIV2:  /* ID3v2 Tag */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_deb(cFNCN,
						"found data type: ID3v2 Tag");
				if (pMF->ffmt != AST_MF_FFMT_TAGF &&
						pMF->ffmt != AST_MF_FFMT_MPG1)
					LOC_PRDEB_FFMT_MTP_
				LOC_PRDEB_SKPD_
			}
			res = AST_MF__rd_readTag(pCmdln, pMF, ST_MTAG_TTP_IV2);
			if (res != ST_ERR_SUCC) {
				cont = ST_B_FALSE;
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"reading tag failed");
			}
			skpd = 0;
			break;
		case ST_UTILSFMT_MTP_APPTAGAV2:  /* APEv2 Tag */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_deb(cFNCN,
						"found data type: APEv2 Tag");
				if (pMF->ffmt != AST_MF_FFMT_TAGF &&
						pMF->ffmt != AST_MF_FFMT_MPG1)
					LOC_PRDEB_FFMT_MTP_
				LOC_PRDEB_SKPD_
			}
			res = AST_MF__rd_readTag(pCmdln, pMF, ST_MTAG_TTP_AV2);
			if (res != ST_ERR_SUCC) {
				cont = ST_B_FALSE;
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"reading tag failed");
			}
			skpd = 0;
			/* */
			if (res == ST_ERR_SUCC && doQuick &&
					jState.jumped && jState.iv1TagSz > 0 &&
					pMF->ffmt != AST_MF_FFMT_TAGF)
				cont = ST_B_FALSE;
			break;
		case ST_UTILSFMT_MTP_APPTAGVOR:  /* Pseudo Vorbis Tag */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_deb(cFNCN,
						"found data type: Pseudo Vorbis Tag");
				if (pMF->ffmt != AST_MF_FFMT_TAGF)
					LOC_PRDEB_FFMT_MTP_
				LOC_PRDEB_SKPD_
			}
			res = AST_MF__rd_readTag(pCmdln, pMF, ST_MTAG_TTP_VOR);
			if (res != ST_ERR_SUCC) {
				cont = ST_B_FALSE;
				ast_mf_op_d_mfErr(pMF, cFNCN,
						"reading tag failed");
			}
			skpd = 0;
			break;
		default:
			/* data type is unknown or can't be handled yet */
			if (skpd == 0 &&
					(pCmdln->cmds.anlz ||
						ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))) {
				if (pCmdln->cmds.anlz)
					ast_mf_op_cb_anaMsg("STR: ! "
							"found unknown data type in input stream "
							"(o "ST_TFOFFS_PRF_0X")",
							st_streamrd_getCurPos(&pMF->strrd));
				else
					ast_mf_op_d_deb(cFNCN,
							"found data type: unknown, skipping data...");
			}
			doSkip = ST_B_TRUE;
		}

		if (! cont)
			continue;
		/* */
		if (! doSkip && lastPos == st_streamrd_getCurPos(&pMF->strrd)) {
			/* we haven't moved ahead, so we skip a byte to avoid
			 *   infinite loop  */
			if (pCmdln->cmds.anlz ||
						ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				snprintf(debMsg, sizeof(debMsg),
						"need to skip one byte in input stream");
				if (pCmdln->cmds.anlz)
					ast_mf_op_cb_anaMsg("STR: %s", debMsg);
				else
					ast_mf_op_d_deb(cFNCN, debMsg);
			}
			doSkip = ST_B_TRUE;
		}
		if (doSkip) {
			res = st_streamrd_rdSkipBytes(&pMF->strrd, 1, ST_B_TRUE);
			if (res != ST_ERR_SUCC)
				cont = ST_B_FALSE;
			else
				++skpd;
			doSkip = ST_B_FALSE;
		}
	}

	if (res == ST_ERR_SUCC && pCmdln->cmds.anlz)
		ast_mf_op_cb_anaMsg("STR: end of input stream");

	return res;
	#undef LOC_PRDEB_FFMT_MTP_
	#undef LOC_PRDEB_SKPD_
	#undef LOC_PRMULTSTR_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Read buffer from stream reader and get mime type
 */
static Tst_err
AST_MF__rd_getNextMimeType(const char *pFnc,
		Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tst_buf *pTmpBuf, const Tst_uint32 bufSz,
		Tst_bool *pEOF, Tst_utilsFmt_mimeTp *pMimeTp)
{
	Tst_err    res;
	Tst_uint32 rd = 0;

	*pEOF      = ST_B_FALSE;
	pTmpBuf[0] = 0;

	res = st_streamrd_rdAheadBuffer(&pMF->strrd, bufSz, pTmpBuf, &rd);
	if (res != ST_ERR_SUCC) {
		if (res == ST_ERR_FEOF) {
			res = ST_ERR_SUCC;
			if (rd == 0)
				*pEOF = ST_B_TRUE;
		} else
			ast_mf_op_d_mfErr(pMF, pFnc,
					"can't read from stream");
	} else {
		*pMimeTp = st_utilsFmt_getMime_enum_byBuffer_tag(pTmpBuf, rd);
		if (*pMimeTp == ST_UTILSFMT_MTP_NONE)
			*pMimeTp = st_utilsFmt_getMime_enum_byBuffer_audvid(pTmpBuf, rd);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Jump to next tag
 */
static Tst_err
AST_MF__rd_jumpTo(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		Tast_mf__rd_jumpState *pJState)
{
	#define LOC_UPDSTRRD_GETMTP_(mac_fpos, mac_seekTp, mac_getMime)  { \
					st_sysFStc_setFPos(&pMF->fstc, (Tst_foffs)(mac_fpos), mac_seekTp); \
					/* */ \
					res = st_streamrd_setInput_file(&pMF->strrd, &pMF->fstc); \
					if (res != ST_ERR_SUCC) { \
						ast_mf_op_d_mfErr(pMF, cFNCN, \
								"setting input file for stream failed"); \
						return res; \
					} \
					/* */ \
					if (mac_getMime) { \
						res = AST_MF__rd_getNextMimeType(cFNCN, pCmdln, pMF, \
									tbuf, sizeof(tbuf), &isEOF, &mimeTp); \
						if (res != ST_ERR_SUCC || isEOF) { \
							if (isEOF) { \
								res = ST_ERR_FEOF; \
							} \
							return res; \
						} \
					} \
				}
	const char *cFNCN = __func__;
	Tst_err   res   = ST_ERR_SUCC;
	Tst_bool  isEOF = ST_B_FALSE;
	Tst_buf   tbuf[20];
	Tst_utilsFmt_mimeTp mimeTp = ST_UTILSFMT_MTP_NONE;

	tbuf[0] = 0;

	if (! pJState->jumped)
		pJState->fsz = st_streamrd_getInputSize(&pMF->strrd);

	/* check for ID3v1 Extended */
	if (! pJState->jumped && pJState->fsz >= 128 + 227) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, "checking for ID3v1 Extended tag");
		/* set filepos, update stream reader and get mime type */
		LOC_UPDSTRRD_GETMTP_(-1 * (128 + 227), ST_SYS_SETFPOS_END, ST_B_TRUE)
		/* */
		if (mimeTp != ST_UTILSFMT_MTP_APPTAGIV1) {
			/*if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "ID3v1 Extended tag not found");*/
			mimeTp = ST_UTILSFMT_MTP_NONE;
		} else {
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "jumping to ID3v1 Extended tag");
			pJState->iv1TagSz = 128 + 227;
		}
	}
	/* check for ID3v1 */
	if (mimeTp == ST_UTILSFMT_MTP_NONE && ! pJState->jumped && pJState->fsz >= 128) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, "checking for ID3v1 tag");
		/* set filepos, update stream reader and get mime type */
		LOC_UPDSTRRD_GETMTP_(-128, ST_SYS_SETFPOS_END, ST_B_TRUE)
		/* */
		if (mimeTp != ST_UTILSFMT_MTP_APPTAGIV1) {
			/*if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "ID3v1 tag not found");*/
			mimeTp = ST_UTILSFMT_MTP_NONE;
		} else {
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "jumping to ID3v1 tag");
			pJState->iv1TagSz = 128;
		}
	}
	/* check for APEv2 */
	if (mimeTp == ST_UTILSFMT_MTP_NONE && pJState->fsz >= 32 * 2) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, "checking for APEv2 tag footer");
		/* set filepos, update stream reader and get mime type */
		LOC_UPDSTRRD_GETMTP_(-1 * (Tst_foffs)(pJState->iv1TagSz + 32),
					ST_SYS_SETFPOS_END, ST_B_TRUE)
		/* */
		if (mimeTp != ST_UTILSFMT_MTP_APPTAGAV2) {
			/*if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "APEv2 tag footer not found");*/
			mimeTp = ST_UTILSFMT_MTP_NONE;
		} else {
			Tst_tagMeta_mt *pMT;
			Tst_apev2_tag  *pAV2;
			Tst_foffs      av2HdOffs = -1;

			/*if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
				ast_mf_op_d_deb(cFNCN, "APEv2 tag footer found");*/
			/* add a temporary tag to the array */
			pMT = st_tagMeta_addNewToArr(&pMF->tagArr, ST_MTAG_TTP_AV2, 1, 0);
			if (pMT == NULL)
				return ST_ERR_FAIL;
			pAV2 = st_tagMeta_gs_getRealTag_av2(pMT);
			/* get relative offset of header */
			res = st_apev2_readTagFooterFromStream(pAV2, &av2HdOffs);
			/* remove temporary tag from array */
			st_tagMeta_removeTagFromArr_byUID(&pMF->tagArr,
						st_tagMeta_gs_getTag_uid(pMT));
			if (res == ST_ERR_IDAT) {
				if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
					ast_mf_op_d_deb(cFNCN, "APEv2 tag footer couldn't be read");
				mimeTp = ST_UTILSFMT_MTP_NONE;
				res    = ST_ERR_SUCC;
			} else if (res != ST_ERR_SUCC) {
				return res;
			} else {
				av2HdOffs -= 32;
				if (st_streamrd_getCurPos(&pMF->strrd) + av2HdOffs < 0) {
					if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
						ast_mf_op_d_deb(cFNCN, "APEv2 tag header offset invalid");
					mimeTp = ST_UTILSFMT_MTP_NONE;
				}
			}
			/* jump to header */
			if (mimeTp != ST_UTILSFMT_MTP_NONE) {
				if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
					ast_mf_op_d_deb(cFNCN, "jumping to APEv2 tag");
				/* set filepos and update stream reader */
				LOC_UPDSTRRD_GETMTP_(-1 * (Tst_foffs)pJState->iv1TagSz + av2HdOffs,
						ST_SYS_SETFPOS_END, ST_B_FALSE)
			}
		}
	}
	if (mimeTp == ST_UTILSFMT_MTP_NONE) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, "jumping to end of file");
		/* set filepos and update stream reader */
		LOC_UPDSTRRD_GETMTP_(0, ST_SYS_SETFPOS_END, ST_B_FALSE)
	}

	/* */
	pJState->jumped = ST_B_TRUE;

	return res;
	#undef LOC_UPDSTRRD_GETMTP_
}

/*----------------------------------------------------------------------------*/

/*
 * Read and parse Tag
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MF__rd_readTag(const Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const Tst_tagMeta_type tmTagType)
{
	const char *cFNCN = __func__;
	Tst_err    res = ST_ERR_SUCC;
	Tst_uint32 ecnt;
	Tst_uint32 ecntFlds;
	Tst_uint32 ecntTag;
	Tst_uint32 skfCnt;
	void       *pRTag;
	char const *pTagD;
	char       debMsg[128];
	Tst_tagMeta_mt       *pMT;
	Tst_tagBasics const  *pTBas;
	//Tast_cln_t_tagListIx tlix;

	pTagD = st_tagMeta_fnc_getTagDesc(tmTagType)[0];
	pTBas = NULL;
	//tlix  = ast_mf_fnc_getSrcTLIXfromMTTP(tmTagType);

	/* add a new tag to the array */
	pMT = st_tagMeta_addNewToArr(&pMF->tagArr, tmTagType, 1, 0);
	if (pMT == NULL) {
		return ST_ERR_FAIL;
	}

	/* */
	snprintf(debMsg, sizeof(debMsg),
			"reading %s tag...", pTagD);
	if (pCmdln->cmds.anlz || ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
		char debMsg2[sizeof(debMsg) * 2];

		snprintf(debMsg2, sizeof(debMsg2),
				"%s (UID 0x%08x)",
				debMsg, st_tagMeta_gs_getTag_uid(pMT));
		if (pCmdln->cmds.anlz) {
			ast_mf_op_cb_anaMsg("TAG: %s", debMsg2);
		} else {
			ast_mf_op_d_deb(cFNCN, debMsg2);
		}
	} else if (pCmdln->opts.showStat) {
		ast_mf_op_prMsg("*  %s", debMsg);
	}

	/* */
	pRTag = st_tagMeta_gs_getRealTag_void(pMT);
	switch (tmTagType) {
	case ST_MTAG_TTP_IV1:
		res = st_id3v1_readTagFromStream((Tst_id3v1_tag*)pRTag);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"reading ID3v1 tag failed");
			return res;
		}
		pTBas = st_id3v1_gs_getTag_tbas((Tst_id3v1_tag*)pRTag);
		break;
	case ST_MTAG_TTP_IV2:
		res = st_id3v2_readTagFromStream((Tst_id3v2_tag*)pRTag);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"reading ID3v2 tag failed");
			return ST_ERR_FAIL;
		}
		pTBas = st_id3v2_gs_getTag_tbas((Tst_id3v2_tag*)pRTag);
		break;
	case ST_MTAG_TTP_AV2:
		res = st_apev2_readTagFromStream((Tst_apev2_tag*)pRTag);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"reading APEv2 tag failed");
			return res;
		}
		pTBas = st_apev2_gs_getTag_tbas((Tst_apev2_tag*)pRTag);
		break;
	case ST_MTAG_TTP_VOR:
		res = st_vorbc_readTagFromStream((Tst_vorbc_tag*)pRTag);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"reading Vorbis tag failed");
			return res;
		}
		st_vorbc_gs_setTag_wasEmbedded((Tst_vorbc_tag*)pRTag, ST_B_FALSE);
		pTBas = st_vorbc_gs_getTag_tbas((Tst_vorbc_tag*)pRTag);
		break;
	default:
		return ST_ERR_IARG;
	}

	/* */
	if (pCmdln->cmds.anlz) {
		ast_mf_op_cb_anaMsg("TAG: end of %s tag", pTagD);
		if (st_tagBas_gs_getHasTag(pTBas)) {
			ast_mf_op_cb_anaMsg("TAG: %s present  "
					"[o="ST_TFOFFS_PRF_LD", s=%u]", pTagD,
					(Tst_foffs)st_tagBas_gs_getOffset(pTBas),
					st_tagBas_gs_getSize(pTBas));
		}
	}

	/* */
	switch (tmTagType) {
	case ST_MTAG_TTP_IV1:
		if (pCmdln->cmds.anlz &&
				st_id3v1_gs_getTag_hasErrors((Tst_id3v1_tag*)pRTag)) {
			ast_mf_op_cb_anaMsg("TAG: ! %s was erroneous", pTagD);
		} else if (pCmdln->opts.showStat &&
				st_id3v1_gs_getTag_hasErrors((Tst_id3v1_tag*)pRTag)) {
			ast_mf_op_prMsg("(Read %s Tag contained malformed data)", pTagD);
		}
		pRTag = NULL;
		break;
	case ST_MTAG_TTP_IV2:
		ecntTag  = st_id3v2_gs_getTag_tagErrorCount((Tst_id3v2_tag*)pRTag);
		ecntFlds = st_id3v2_gs_getTag_fldsErrorCount((Tst_id3v2_tag*)pRTag);
		if (pCmdln->cmds.anlz && st_tagBas_gs_getHasTag(pTBas)) {
			if (ecntTag > 0 || ecntFlds > 0) {
				AST_MF__rd_printID3v2_errs(0, (Tst_id3v2_tag*)pRTag);
			}
			skfCnt = st_id3v2_gs_getTag_amountSkippedFields((Tst_id3v2_tag*)pRTag);
			if (skfCnt > 0) {
				ast_mf_op_cb_anaMsg("     (skipped %u field%s in %s Tag)",
						skfCnt, (skfCnt == 1 ? "" : "s"), pTagD);
			}
		} else if (ecntTag > 0 || ecntFlds > 0) {
			ecnt = ecntTag + ecntFlds;
			if (pCmdln->opts.showStat) {
				ast_mf_op_prMsg("(Read %s Tag produced %u warning%s)",
						 pTagD, ecnt, (ecnt == 1 ? "" : "s"));
			}
		}
		pRTag = NULL;
		break;
	case ST_MTAG_TTP_AV2:
		skfCnt = st_apev2_gs_getTag_amountSkippedFields((Tst_apev2_tag*)pRTag);
		if (pCmdln->cmds.anlz && skfCnt > 0) {
			ast_mf_op_cb_anaMsg("TAG: (skipped %u field%s in %s Tag)",
					skfCnt, (skfCnt == 1 ? "" : "s"),
					pTagD);
		} else if (skfCnt > 0 && pCmdln->opts.showStat) {
			ast_mf_op_prMsg("(Read %s Tag produced %u warning%s)",
					 pTagD, skfCnt, (skfCnt == 1 ? "" : "s"));
		}
		pRTag = NULL;
		break;
	case ST_MTAG_TTP_VOR:
		skfCnt = st_vorbc_gs_getTag_amountSkippedFields((Tst_vorbc_tag*)pRTag);
		if (pCmdln->cmds.anlz && skfCnt > 0) {
			ast_mf_op_cb_anaMsg("TAG: (skipped %u field%s in %s Tag)",
					skfCnt, (skfCnt == 1 ? "" : "s"),
					pTagD);
		} else if (skfCnt > 0 && pCmdln->opts.showStat) {
			ast_mf_op_prMsg("(Read %s Tag produced %u warning%s)",
					 pTagD, skfCnt, (skfCnt == 1 ? "" : "s"));
		}
		pRTag = NULL;
	default:
		break;
	}

	/* */
	if (st_tagBas_gs_getHasTag(pTBas)) {
		pMF->contTypes |= (Tst_int32)AST_MF_CTP_TAG;
		if (pMF->ffmt == AST_MF_FFMT_NONE) {
			pMF->ffmt = AST_MF_FFMT_TAGF;
		}
	} else {
		st_tagMeta_removeTagFromArr_byUID(&pMF->tagArr,
				st_tagMeta_gs_getTag_uid(pMT));
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read (and parse) audio data
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MF__rd_readAV(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const Tst_utilsFmt_mimeTp ufmtStrType)
{
	const char *cFNCN = __func__;
	Tst_err   res;
	Tst_int32 oct;
	char      debMsg[128];

	ST_ASSERTN_IARG(pCmdln == NULL || pMF == NULL ||
			pMF->pFilen == NULL || st_sysStrlen(pMF->pFilen) < 1)

	/* read stream(s) */
	switch (ufmtStrType) {
	case ST_UTILSFMT_MTP_AUDVIDOGG:
	case ST_UTILSFMT_MTP_AUDFLAC:
		/* read OGG/FLAC audio/video stream(s) */
		snprintf(debMsg, sizeof(debMsg),
				"reading OGG or Flac stream(s)...");
		if (pCmdln->cmds.anlz)
			ast_mf_op_cb_anaMsg("AUD: %s", debMsg);
		else if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, debMsg);
		else if (pCmdln->opts.showStat)
			ast_mf_op_prMsg("*  %s", debMsg);
		res = AST_MF__rd_readAV_oggOrFlac(pCmdln, pMF, ufmtStrType);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (st_contOgg_gs_getHasOggStream(&pMF->avOgg) ||
				st_contOgg_gs_getHasFlacStream(&pMF->avOgg)) {
			/* add vorbis tags from streams to tag array */
			res = AST_MF__rd_readAV_addVorbc(pCmdln, pMF);
			if (res != ST_ERR_SUCC)
				return res;

			/* content-types */
			oct = st_contOgg_gs_getContTypes(&pMF->avOgg);
			if ((oct & (Tst_int32)ST_CONTOGG_BSCTP_AUD) !=
						(Tst_int32)ST_CONTOGG_BSCTP_NONE ||
					st_contOgg_gs_getHasFlacStream(&pMF->avOgg))
				pMF->contTypes |= (Tst_int32)AST_MF_CTP_AUD;
			else if ((oct & (Tst_int32)ST_CONTOGG_BSCTP_VID) !=
						(Tst_int32)ST_CONTOGG_BSCTP_NONE)
				pMF->contTypes |= (Tst_int32)AST_MF_CTP_VID;
			else if (oct != (Tst_int32)ST_CONTOGG_BSCTP_NONE)
				pMF->contTypes |= (Tst_int32)AST_MF_CTP_OTHER;
		}
		break;
	case ST_UTILSFMT_MTP_AUDMPEG1:
		/* read MPEG-1 audio stream */
		snprintf(debMsg, sizeof(debMsg),
				"reading MPEG-1 stream...");
		if (pCmdln->cmds.anlz)
			ast_mf_op_cb_anaMsg("AUD: %s", debMsg);
		else if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN, debMsg);
		else if (pCmdln->opts.showStat)
			ast_mf_op_prMsg("*  %s", debMsg);
		res = AST_MF__rd_readAV_mpeg1(pCmdln, pMF);
		if (res != ST_ERR_SUCC)
			return res;
		/* */
		if (st_mpeg1_gs_getStr_hasMPEGstream(&pMF->audMpg1)) {
			pMF->contTypes |= (Tst_int32)AST_MF_CTP_AUD;
			pMF->ffmt       = AST_MF_FFMT_MPG1;
		}
		break;
	default:
		return ST_ERR_IARG;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Read MPEG-1 stream
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MF__rd_readAV_mpeg1(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	const char *cFNCN = __func__;
	Tst_err    res;
	Tst_str    ch;
	Tst_uint32 ecnt;
	char       debMsg[512];
	Tst_bool   showErrs = ST_B_FALSE;
	#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
		Tst_contWav_obj *pWavObj = NULL;
	#endif  /* libmpg123 or libmad */

	/* init external lib */
	#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
		if (pCmdln->cmds.anlz || pCmdln->cmds.decAud) {
			res = ast_mf_rddec_initExtLibMpg();
			if (res != ST_ERR_SUCC)
				return res;
		}
	#endif  /* libmpg123 or libmad */

	/* set up options */
	/** pcm client */
	if (pCmdln->cmds.decAud) {
		#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
			res = AST_MF__rd_initPCMclient(pCmdln, pMF, cFNCN, &pWavObj);
			if (res != ST_ERR_SUCC) {
				ast_mf_rddec_freeExtLibMpg();
				return res;
			}
			st_mpeg1_opts_setCB_pcmClient(&pMF->audMpg1,
					st_contWav_wr_prepareWritingCB,
					st_contWav_wr_setOutputFmtCB,
					st_contWav_wr_writeRawSamplesCB,
					st_contWav_wr_finishWritingCB,
					pWavObj);
		#else
			if (! pCmdln->opts.quiet)
				ast_mf_op_prMsg("(Can't decode MPEG-1, no libmpg123/libmad support)");
		#endif  /* libmpg123 or libmad */
	} else
		st_mpeg1_opts_setCB_pcmClient(&pMF->audMpg1,
				NULL, NULL, NULL, NULL, NULL);
	/** decoder */
	#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
		if (pCmdln->cmds.anlz || pCmdln->cmds.decAud) {
			st_mpeg1_opts_setCB_decoding(&pMF->audMpg1,
					ast_mf_rddec_cbDecMpg_hndNew,
					ast_mf_rddec_cbDecMpg_hndDel,
					ast_mf_rddec_cbDecMpg_setOutputFmt,
					ast_mf_rddec_cbDecMpg_openFeed,
					ast_mf_rddec_cbDecMpg_feedInp,
					ast_mf_rddec_cbDecMpg_decode,
					ast_mf_rddec_cbDecMpg_closeFeed);
		} else {
			st_mpeg1_opts_setCB_decoding(&pMF->audMpg1,
					NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		}
	#endif  /* libmpg123 or libmad */
	/** */
	if (pCmdln->optsAvMisc.mpeg1_forceMLay == ST_MPEG1_LAYER_NONE) {
		/* force layer based upon fileextension,
		 *   this is necessary for various MPEG files...  */
		ecnt = st_sysStrlen(pMF->pFilen);
		if (ecnt > 0)
			ch = pMF->pFilen[ecnt - 1];
		else
			ch = 0;
		st_mpeg1_opts_setForceLayer(&pMF->audMpg1,
				(ch == '1' ? ST_MPEG1_LAYER_1 :
					(ch == '2' ? ST_MPEG1_LAYER_2 :
						(ch == '3' ? ST_MPEG1_LAYER_3 : ST_MPEG1_LAYER_NONE))));
	}

	/* read stream */
	res = st_mpeg1_readStream(&pMF->audMpg1);
	if (res != ST_ERR_SUCC) {
		ast_mf_op_d_mfErr(pMF, cFNCN,
				"reading MPEG-1 stream failed");
	} else
		showErrs = ST_B_TRUE;

	/* end pcm client and decoding */
	#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
		if (pWavObj != NULL)
			AST_MF__rd_endPCMclient(pCmdln, pMF, cFNCN, &pWavObj);

		/* free external lib */
		if (pCmdln->cmds.anlz || pCmdln->cmds.decAud)
			ast_mf_rddec_freeExtLibMpg();
	#endif  /* libmpg123 or libmad */

	/* */
	ecnt = st_mpeg1_gs_getStr_amountWarnings(&pMF->audMpg1);
	if (showErrs && ecnt > 0) {
		snprintf(debMsg, sizeof(debMsg),
				"(Read MPEG-1 stream produced %u warning%s)",
					 ecnt, (ecnt == 1 ? "" : "s"));
		if (pCmdln->cmds.anlz)
			ast_mf_op_cb_anaMsg("AUD: %s", debMsg);
		else if (pCmdln->opts.showStat)
			ast_mf_op_prMsg(debMsg);
	}

	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * Read OGG or Flac stream(s)
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MF__rd_readAV_oggOrFlac(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const Tst_utilsFmt_mimeTp ufmtStrType)
{
	const char *cFNCN = __func__;
	Tst_err    res      = ST_ERR_SUCC;
	Tst_bool   showErrs = ST_B_FALSE;
	Tst_uint32 ecnt;
	char       debMsg[512];
	Tst_contWav_obj *pWavObj = NULL;

	/* initialize decoding & set up options */
	/** pcm client */
	if (pCmdln->cmds.decAud) {
		res = AST_MF__rd_initPCMclient(pCmdln, pMF, cFNCN, &pWavObj);
		if (res != ST_ERR_SUCC) {
			return res;
		}
		st_contOgg_opts_setCB_pcmClient(&pMF->avOgg,
				st_contWav_wr_prepareWritingCB,
				st_contWav_wr_setOutputFmtCB,
				st_contWav_wr_writeRawSamplesCB,
				st_contWav_wr_finishWritingCB,
				pWavObj);
	} else {
		st_contOgg_opts_setCB_pcmClient(&pMF->avOgg,
				NULL, NULL, NULL, NULL, NULL);
	}
	/** decoder */
	if (ufmtStrType == ST_UTILSFMT_MTP_AUDVIDOGG) {
		if (pCmdln->cmds.anlz || pCmdln->cmds.decAud) {
			#if (HAVE_LIBVORBIS == 1)
				st_contOgg_opts_setCB_vorbDecoding(&pMF->avOgg,
						ast_mf_rddec_cbDecVorb_hndNew,
						ast_mf_rddec_cbDecVorb_hndDel,
						ast_mf_rddec_cbDecVorb_setOutputFmt,
						ast_mf_rddec_cbDecVorb_openFeed,
						ast_mf_rddec_cbDecVorb_feedHeader,
						ast_mf_rddec_cbDecVorb_feedAudioAndDecode,
						ast_mf_rddec_cbDecVorb_closeFeed);
			#else
				if (! pCmdln->opts.quiet) {
					ast_mf_op_prMsg("(Can't decode Ogg-Vorbis, no libvorbis support)");
				}
			#endif  /* libvorbis */
		} else {
			#if (HAVE_LIBVORBIS == 1)
				st_contOgg_opts_setCB_vorbDecoding(&pMF->avOgg,
						NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			#endif  /* libvorbis */
		}
	}
	/** */
	if (ufmtStrType == ST_UTILSFMT_MTP_AUDFLAC) {
		st_contOgg_opts_setFlac_checkCRCofFrHd(&pMF->avOgg,
				pCmdln->cmds.anlz);
		st_contOgg_opts_setFlac_checkCRCofEntireFrames(&pMF->avOgg,
				pCmdln->cmds.anlz);
	} else if (ufmtStrType == ST_UTILSFMT_MTP_AUDVIDOGG) {
		st_contOgg_opts_setFlac_checkCRCofFrHd(&pMF->avOgg, ST_B_FALSE);
		st_contOgg_opts_setFlac_checkCRCofEntireFrames(&pMF->avOgg, ST_B_FALSE);
	}

	/* read stream(s) */
	if (ufmtStrType == ST_UTILSFMT_MTP_AUDFLAC) {
		res = st_contOgg_readFlacStream(&pMF->avOgg);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"reading Flac stream failed");
		} else {
			showErrs = ST_B_TRUE;
			if (st_contOgg_gs_getHasFlacStream(&pMF->avOgg)) {
				pMF->ffmt = AST_MF_FFMT_FLAC;
			}
		}
	} else if (ufmtStrType == ST_UTILSFMT_MTP_AUDVIDOGG) {
		res = st_contOgg_readStreams(&pMF->avOgg);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"reading Ogg stream failed");
		} else {
			showErrs = ST_B_TRUE;
			if (st_contOgg_gs_getHasOggStream(&pMF->avOgg)) {
				pMF->ffmt = AST_MF_FFMT_OGG;
			}
		}
	}

	/* end pcm client and decoding */
	if (pWavObj != NULL) {
		AST_MF__rd_endPCMclient(pCmdln, pMF, cFNCN, &pWavObj);
	}
	/* */
	ecnt = st_contOgg_gs_getErrorCount(&pMF->avOgg);
	if (showErrs && ecnt > 0) {
		snprintf(debMsg, sizeof(debMsg),
				"(Read %s stream produced %u warning%s)",
				(ufmtStrType == ST_UTILSFMT_MTP_AUDFLAC ? "Flac" : "OGG"),
				ecnt, (ecnt == 1 ? "" : "s"));
		if (pCmdln->cmds.anlz) {
			ast_mf_op_cb_anaMsg("AUD: %s", debMsg);
		} else if (pCmdln->opts.showStat) {
			ast_mf_op_prMsg(debMsg);
		}
	}

	return res;
}

/*
 * Read and parse all Tags that may appear in OGG/FLAC files
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MF__rd_readAV_addVorbc(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF)
{
	const char *cFNCN = __func__;
	Tst_err     res;
	Tst_uint32  grpIx  = 0;
	Tst_uint32  grpSIx = 0;
	Tst_uint32  skfCnt = 0;
	char const  *pTagD;
	Tst_binobj                       *pBinDatRT;
	Tst_tagMeta_mt                   *pMT;
	Tst_vorbc_tag                    *pVor;
	Tst_contOgg_substream            *pBS = NULL;
	Tst_contOgg_vorbCommentRaw const *pVRC;
	//Tst_contOgg_bsType               bsType;

	pTagD = st_tagMeta_fnc_getTagDesc(ST_MTAG_TTP_VOR)[0];

	while ((pBS = st_contOgg_ite_getNextStream_any(&pMF->avOgg, pBS)) != NULL) {
		st_contOgg_gs_getStr_groupIndex(pBS, &grpIx, &grpSIx);
		//bsType = st_contOgg_gs_getStr_type(pBS);
		/* get pointer to raw comment from stream */
		pVRC = st_contOgg_vorbComm_gs_getRawComment(pBS);
		if (pVRC == NULL)
			continue;
		/* add new meta tag of type vorbis */
		pMT = st_tagMeta_addNewToArr(&pMF->tagArr, ST_MTAG_TTP_VOR, grpIx, grpSIx);
		if (pMT == NULL)
			return ST_ERR_FAIL;
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN))
			ast_mf_op_d_deb(cFNCN,
					"read %s Tag from raw-buf of bs #%u.%u... (UID 0x%08x)",
					pTagD, grpIx, grpSIx,
					st_tagMeta_gs_getTag_uid(pMT));
		pVor = st_tagMeta_gs_getRealTag_vor(pMT);
		/* */
		pBinDatRT = st_contOgg_vorbComm_gs_getRC_rawDataPointer(pVRC);
		if (pBinDatRT == NULL)
			return ST_ERR_FAIL;
		res = st_vorbc_readTagFromBinObj(pVor, pBinDatRT);
		if (res != ST_ERR_SUCC) {
			ast_mf_op_d_mfErr(pMF, cFNCN,
					"reading Vorbis tag from BinObj failed");
			return res;
		}
		if (st_tagBas_gs_getHasTag(st_vorbc_gs_getTag_tbas(pVor))) {
			pMF->contTypes |= (Tst_int32)AST_MF_CTP_TAG;
			/* */
			st_vorbc_gs_setTag_wasEmbedded(pVor, ST_B_TRUE);
			/* */
			skfCnt = st_vorbc_gs_getTag_amountSkippedFields(pVor);
			if (pCmdln->cmds.anlz && skfCnt > 0) {
				ast_mf_op_cb_anaMsg("TAG: (skipped %u field%s in %s Tag)",
						skfCnt, (skfCnt == 1 ? "" : "s"),
						pTagD);
			} else if (skfCnt > 0 && pCmdln->opts.showStat) {
				ast_mf_op_prMsg("(Read %s Tag produced %u warning%s)",
						 pTagD, skfCnt, (skfCnt == 1 ? "" : "s"));
			}
		}
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Initialize WAV encoding
 */
static Tst_err
AST_MF__rd_initPCMclient(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const char *pFnc, Tst_contWav_obj **ppWavObj)
{
	Tst_err res;

	ST_REALLOC(*ppWavObj, Tst_contWav_obj*, 1, sizeof(Tst_contWav_obj))
	if (*ppWavObj == NULL)
		return ST_ERR_OMEM;

	res = st_contWav_stc_initWObj(*ppWavObj);
	if (res != ST_ERR_SUCC) {
		ast_mf_op_d_mfErr(pMF, pFnc,
				"initialization of WAVE/AIFF container failed");
		st_contWav_stc_freeWObj(*ppWavObj);
		ST_DELPOINT(*ppWavObj)
		return res;
	}

	st_contWav_opts_setBasicOpts(*ppWavObj, &pCmdln->opts.basOpts);
	if (pCmdln->opts.showStat)
		st_contWav_opts_setCB_newOutpStr(*ppWavObj,
				AST_MF__rd_cbPCMnewOutpStr);
	if (! st_sysStrEmpty(pCmdln->opts.pOutpdir))
		st_contWav_opts_setOutputDir(*ppWavObj, pCmdln->opts.pOutpdir);
	st_contWav_opts_setOverwriteExistingOutputFiles(*ppWavObj, pCmdln->opts.owExFiles);
	st_contWav_opts_setOutputFmt(*ppWavObj,
			(pCmdln->optsAvMisc.decAudFmt == ST_UTILSFMT_MTP_AUDWAV));

	return ST_ERR_SUCC;
}

/*
 * Finish WAV encoding
 */
static Tst_err
AST_MF__rd_endPCMclient(Tast_cln_a *pCmdln, Tast_mf_finfo *pMF,
		const char *pFnc, Tst_contWav_obj **ppWavObj)
{
	Tst_err       res;
	Tst_fsize     fsz      = 0;
	Tst_bool      isTooBig = ST_B_FALSE;
	Tst_str const *pFilen  = NULL;
	Tst_uint32    bsIx     = 0;
	Tst_uint32    bsSIx    = 0;
	Tst_uint64    wrSamps;
	Tst_contWav_handle *pElem = NULL;

	ST_SYSMATH_STC_RSETUI64(wrSamps)

	while ((pElem = st_contWav_ite_getNextHandle(*ppWavObj, pElem)) != NULL) {
		if (! st_contWav_gs_getHandle_index(pElem, &bsIx, &bsSIx)) {
			ast_mf_op_d_mfErr(pMF, pFnc,
					"getting next WAVE/AIFF handle index failed");
			continue;
		}
		/* finish writing output file */
		if (st_contWav_gs_getHandle_isOpen(pElem)) {
			res = st_contWav_wr_finishWritingCB(pElem);
			if (res != ST_ERR_SUCC) {
				ast_mf_op_d_mfErr(pMF, pFnc,
						"finish writing WAVE/AIFF failed for bs#%u.%02u",
						bsIx, bsSIx);
				continue;
			}
		}
		/* */
		pFilen = st_contWav_gs_getHandle_filen(pElem);
		ST_ASSERTN_NUM(ST_ERR_FAIL, pFilen == NULL)

		/* delete output file if empty */
		fsz = st_sysGetFileSz_byFn(pFilen, &isTooBig);
		st_contWav_gs_getHandle_writtenSamples(pElem, &wrSamps);
		if (((! isTooBig && fsz == 0) ||
					st_sysUInt64_cmpLoHi(&wrSamps, 0, 0) == 0) &&
				st_sysDoesFileExist(pFilen)) {
			if (pCmdln->opts.showStat)
				ast_mf_op_prMsg("(Output file for bs#%u.%02u is empty, "
						"deleting it...)", bsIx, bsSIx);
			if (! st_sysUnlinkFile(pFilen))
				ast_mf_op_d_mfErr(pMF, pFnc,
						"couldn't delete file '%s'", pFilen);
		}
	}

	st_contWav_stc_freeWObj(*ppWavObj);
	ST_DELPOINT(*ppWavObj)

	return ST_ERR_SUCC;
}

static void
AST_MF__rd_cbPCMnewOutpStr(const Tst_uint32 bsIx, const Tst_uint32 bsSIx,
		const Tst_str *pFilen, const Tst_bool pretending)
{
	ast_mf_op_prMsg("*  writing PCM output stream bs#%u.%02u%s\n --> '%s'",
			bsIx, bsSIx,
			(pretending ? "  (pretending)" : ""),
			(const char*)pFilen);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * mode: 0=analize
 */
static void
AST_MF__rd_printID3v2_errs(const int mode, Tst_id3v2_tag *pV2tag)
{
	#define LOC_PRERR1_(str)  {ast_mf_op_cb_anaMsg("       - %s", str);}
	#define LOC_PRERR2_(str)  { \
				ast_mf_op_cb_anaMsg("       - %s #%02u: %s", \
						pFIDstr, fnr, str);}
	#define LOC_PRERR3_(str)  { \
				ast_mf_op_cb_anaMsg("         %s #%02u: %s", \
						pFIDstr, fnr, str);}
	#define LOC_IF_INCCNT1_H_(val)  (terrs.th##val && ++cntT[1])
	#define LOC_IF_INCCNT1_B_(val)  (terrs.tb##val && ++cntT[1])
	#define LOC_IF_INCCNT2_F_(val) \
				(ferrs.fr##val && ++cntF[1] && ++cntF[2])
	Tst_err       res;
	Tst_uint32    cntA     = 0;             /* all */
	Tst_uint32    cntT[]   = {0, 0};        /* in header/body */
	Tst_uint32    cntF[]   = {0, 0, 0, 0};  /* fields */
	Tst_uint32    fnr      = 0;
	int           delta;
	const Tst_str *pFIDstr    = NULL;
	Tst_id3v2_fldData *pItFld = NULL;
	Tst_id3v2_fldProp fldPr;
	Tst_id3v2_fldErrs ferrs;
	Tst_id3v2_tagErrs terrs;

	st_id3v2_stc_rsetFPr(&fldPr);
	st_id3v2_stc_rsetTagErrs(&terrs);
	st_id3v2_stc_rsetFldErrs(&ferrs);

	st_id3v2_gs_getTag_tagErrors(pV2tag, &terrs);
	/* total amount of errs in head/body */
	cntT[0] = st_id3v2_gs_getTag_tagErrorCount(pV2tag);
	/* total amount of errs in all frames */
	cntF[0] = st_id3v2_gs_getTag_fldsErrorCount(pV2tag);
	cntA    = cntT[0] + cntF[0];
	ast_mf_op_cb_anaMsg("TAG: ! ID3v2 was erroneous (%u warning%s)",
			cntA, (cntA == 1 ? "" : "s"));

	/* */
	if LOC_IF_INCCNT1_H_(IsSizeInv)
		LOC_PRERR1_("tag size invalid")
	if LOC_IF_INCCNT1_H_(HasUnknFl)
		LOC_PRERR1_("unknown flag")
	if LOC_IF_INCCNT1_H_(IsEntTagCompr)
		LOC_PRERR1_("entire tag compressed")
	if LOC_IF_INCCNT1_H_(IsVersInv)
		LOC_PRERR1_("tag version invalid")
	if LOC_IF_INCCNT1_H_(IsFlagByteInv)
		LOC_PRERR1_("flag byte invalid")
	if LOC_IF_INCCNT1_H_(WasSzCorrd)
		LOC_PRERR1_("tag size was incorrect")
	/* */
	if LOC_IF_INCCNT1_B_(HasFalseSyn)
		LOC_PRERR1_("false sync byte(s) found")
	if LOC_IF_INCCNT1_B_(HasInvFID)
		LOC_PRERR1_("invalid field ID found")
	if LOC_IF_INCCNT1_B_(HasInvFrSz)
		LOC_PRERR1_("invalid field size found")
	if LOC_IF_INCCNT1_B_(HasUnknFIDs)
		LOC_PRERR1_("unknown field ID(s) found")
	if LOC_IF_INCCNT1_B_(HasTooManyTFs)
		LOC_PRERR1_("too many fields")
	/* */
	while ((pItFld = st_id3v2_ite_nextFld(pV2tag, pItFld)) != NULL) {
		res = st_id3v2_gs_getField_props(pItFld, &fldPr);
		if (res != ST_ERR_SUCC)
			return;
		fnr     = st_id3v2_gs_getField_nr(pItFld);
		pFIDstr = st_id3v2_gs_getFieldProp_idStr(&fldPr);
		st_id3v2_gs_getField_errors(pItFld, &ferrs);
		cntF[2] = 0;  /* handled errs in this frame */
		cntF[3] = st_id3v2_gs_getField_errorCount(pItFld);  /* errs in this frame */
		/* */
		if LOC_IF_INCCNT2_F_(HasFalseSyn)
			LOC_PRERR2_("false sync byte(s) found")
		if LOC_IF_INCCNT2_F_(HasCompr)
			LOC_PRERR2_("is compressed")
		if LOC_IF_INCCNT2_F_(HasEncr)
			LOC_PRERR2_("is encrypted")
		if LOC_IF_INCCNT2_F_(HasInvFlags)
			LOC_PRERR2_("had invalid flag(s)")
		if LOC_IF_INCCNT2_F_(HasInvFlData)
			LOC_PRERR2_("had invalid data in flag(s)")
		if LOC_IF_INCCNT2_F_(HasInvData)
			LOC_PRERR2_("had invalid/bogus data")
		if LOC_IF_INCCNT2_F_(IsIPLSincompl)
			LOC_PRERR2_("list mapping is incomplete")
		if LOC_IF_INCCNT2_F_(HasInvChars)
			LOC_PRERR2_("read string(s) was malformed/incomplete")
		if LOC_IF_INCCNT2_F_(HasInvComprM)
			LOC_PRERR2_("has invalid compression method")
		if LOC_IF_INCCNT2_F_(IsntValVers)
			LOC_PRERR2_("wasn't valid for this ID3v2 version")
		if LOC_IF_INCCNT2_F_(FraWasntDcmpr)
			LOC_PRERR2_("field wasn't decompressable")
		if LOC_IF_INCCNT2_F_(DatWasntDcmpr)
			LOC_PRERR2_("data wasn't decompressable")
		if LOC_IF_INCCNT2_F_(HasInvTE)
			LOC_PRERR2_("had invalid Text-Encoding")
		if LOC_IF_INCCNT2_F_(HasInvPT)
			LOC_PRERR2_("had invalid Picture-type")
		if LOC_IF_INCCNT2_F_(HasInvPF)
			LOC_PRERR2_("had invalid Picture-format")
		if LOC_IF_INCCNT2_F_(HasInvSLTtsf)
			LOC_PRERR2_("had invalid Timestamp-format")
		if LOC_IF_INCCNT2_F_(HasInvSLTctp)
			LOC_PRERR2_("had invalid Content-type")
		if LOC_IF_INCCNT2_F_(HasInvLng)
			LOC_PRERR2_("had invalid Language-ID")
		if LOC_IF_INCCNT2_F_(CorrdU16)
			LOC_PRERR2_("UTF16 encoding was incorrect")
		if LOC_IF_INCCNT2_F_(WasMergd)
			LOC_PRERR2_("has been merged with another field")
		if LOC_IF_INCCNT2_F_(IsDup)
			LOC_PRERR2_("was/is a duplicate of another field")
		if LOC_IF_INCCNT2_F_(WasMappd)
			LOC_PRERR2_("was mapped to another field")
		if LOC_IF_INCCNT2_F_(TooManyTFI)
			LOC_PRERR2_("exceeded field per ID max.")
		if LOC_IF_INCCNT2_F_(TooManyMS)
			LOC_PRERR2_("had too many strings in multi-string")
		/* */
		if (cntF[2] != cntF[3])
			LOC_PRERR2_("has/had unknown warnings")
		/* */
		if (! st_id3v2_gs_getField_isOK(pItFld) && ! ferrs.frWasMappd) {
			if (cntF[3] > 0) {
				if (ferrs.frIsntValVers)
					LOC_PRERR3_("was mapped")
				else if (! (ferrs.frIsDup || ferrs.frWasMergd))
					LOC_PRERR3_("was discarded")
			} else
				LOC_PRERR2_("(was empty/discarded)")
		}
	}
	/* */
	if (cntA != cntT[1] + cntF[1]) {
		delta = (int)cntA - (int)(cntT[1] + cntF[1]);
		ast_mf_op_cb_anaMsg("       - %+d unknown warning%s",
				delta, (delta == 1 || delta == -1 ? "" : "s"));
	}
	#undef LOC_PRERR1_
	#undef LOC_PRERR2_
	#undef LOC_PRERR3_
	#undef LOC_IF_INCCNT1_H_
	#undef LOC_IF_INCCNT1_B_
	#undef LOC_IF_INCCNT2_F_
}

/******************************************************************************/
