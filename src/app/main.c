/******************************************************************************/
/* main.c                         [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* Program version: v1.0, begun 10.09.2024 (dd.mm.yyyy)                       */
/*                  v0.5, begun 01.12.2010                                    */
/*                  v0.4, begun 04.04.2010                                    */
/*                  v0.3, begun 28.12.2009                                    */
/*                  v0.1, begun 19.05.2005                                    */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Program start                                                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/* This program is distributed in the hope that it will be useful,            */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of             */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              */
/* GNU General Public License for more details.                               */
/*                                                                            */
/* You should have received a copy of the GNU General Public License          */
/* along with this program; if not, write to the Free Software                */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston,                      */
/* MA  02111-1307  USA                                                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#if (LIBSANTAGGER_CFG_EXTRA_DEBUG == 1)
#	define LOC_SHOWLOCALE_  0  /* show locale ? */
#endif
/** */
#include "src/includes/common/binobj.h"
#include "src/includes/common/streamrd.h"
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"
#include "src/includes/utils/utils_fmt.h"
#include "src/includes/audvid/aud_mpeg1.h"
#include "src/includes/audvid/cont_ogg.h"
#include "src/includes/tag/tag_apev2.h"
#include "src/includes/tag/tag_id3v1.h"
#include "src/includes/tag/tag_id3v2.h"
#include "src/includes/tag/tag_vorbc.h"
#include "src/includes/tag/tag_meta.h"
/** */
#define SRC_MAIN_C
#include "mf/mf_cnvmrg-pp.h"
#include "mf/mf_edit-pp.h"
#include "mf/mf_read-pp.h"
#include "mf/mf_extr-pp.h"
#include "mf/mf_show-pp.h"
//#include "mf/mf_fnc-pp.h"
#include "mf/mf_outp-pp.h"
#include "mf/mf_stcs-pp.h"
/*** */
#include "cln/cln_pargs-pp.h"
#include "cln/cln_fnc-prot.h"
#include "cln/cln_stcs-prot.h"
#undef SRC_MAIN_C

/*
// System-Includes
*/
#ifdef _MSC_VER
	#error MSVC is not supported
#endif
#include <stdlib.h>      /* exit(), calloc(), getenv() */
//#include <string.h>      /* memset() */
#include <locale.h>      /* setlocale() */
#include <unistd.h>      /* isatty() */
#if ! (defined(_WIN32) || defined (__CYGWIN__))
	#include <sys/ioctl.h>   /* TIOCGWINSZ */
#endif
#if (LOC_SHOWLOCALE_ == 1)
#	include <langinfo.h>    /* nl_langinfo() */
#endif
#if (LIBSANTAGGER_HAVE_LIBZ == 1)
#	include <zlib.h>        /* uncompress(), compress(), compressBound(), Z_OK */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_MAIN__getTermCap(const Tst_str *pAppFn);
/** */
static void
AST_MAIN__cbSetOpts_tagIV1(void *pSetOptsData, Tst_id3v1_tag *pTag);

static void
AST_MAIN__cbSetOpts_tagIV2(void *pSetOptsData, Tst_id3v2_tag *pTag);

static void
AST_MAIN__cbSetOpts_tagAV2(void *pSetOptsData, Tst_apev2_tag *pTag);

static void
AST_MAIN__cbSetOpts_tagVOR(void *pSetOptsData, Tst_vorbc_tag *pTag);

static void
AST_MAIN__setOptsInSubObjs(Tast_mf_finfo *pMF, Tast_cln_a *pCmdln);
/** */
static Tst_bool
AST_MAIN__get_n_chk_fsz(Tast_mf_finfo *pMF, const Tst_str *pAppFn,
                        const Tast_cln_a *pCmdln, Tst_fsize *pFsz);
static Tst_bool
AST_MAIN__getFFmt(Tast_mf_finfo *pMF, const Tst_str *pAppFn,
                  Tast_cln_a *pCmdln);
/** */
static Tst_err
AST_MAIN__updInput(Tast_mf_finfo *pMF);
/** */
#if (LIBSANTAGGER_HAVE_LIBZ == 1)
	static Tst_err
	AST_MAIN__cbZLibDecompress(Tst_binobj *pBinDatIn,
	                           Tst_uint32 uncomprSzShould,
	                           Tst_binobj *pBinDatOut);
	static Tst_err
	AST_MAIN__cbZLibCompress(Tst_binobj *pBinDatIn,
	                         Tst_binobj *pBinDatOut);
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Mother and Father of all to come
 *
 * returns: 0=success
 *          1=error
 */
int
main(const int argc, const char *argv[])
{
	#define LOC_PRNLN_  { \
					if (x < (Tst_uint32)(argc - 1) && \
							! cmdln.opts.quiet) \
						ast_mf_op_prMsg("\n"); \
					}
	int        resA        = 0;  // success
	Tst_err    resF        = ST_ERR_SUCC;
	Tst_str    *pAppFn     = NULL;
	Tst_uint32 fcnt        = 0,
	           x,
	           parbeg      = 0;
	Tst_bool   wasFCreated = ST_B_FALSE,
	           wasDCreated = ST_B_FALSE;
	Tst_fsize  fsz         = 0;
	char const *pLocaleStr;
	Tast_cln_a    cmdln;
	Tast_mf_finfo mf;

	/* check endianess */
	#if (LIBSANTAGGER_CFG_EXTRA_DEBUG == 1)
		if (ST_SYSFNC_ISBIGEND) {
			#if (LIBSANTAGGER_CFG_WORDS_BIGENDIAN != 1)
				ast_mf_op_d_mainErrApp((Tst_str*)argv[0],
						"App was compiled for Little-Endian but system is Big-Endian\n");
				exit(1);  // error
			#endif
		} else {
			#if (LIBSANTAGGER_CFG_WORDS_BIGENDIAN == 1)
				ast_mf_op_d_mainErrApp((Tst_str*)argv[0],
						"App was compiled for Big-Endian but system is Little-Endian\n");
				exit(1);  // error
			#endif
		}
		/* TODO */
		/**ast_mf_op_d_mainDeb("sizeof(Tst_fsize): %u, "
				"sizeof(Tst_foffs): %u, sizeof(off_t): %u\n",
				(Tst_uint32)sizeof(Tst_fsize), (Tst_uint32)sizeof(Tst_foffs),
				(Tst_uint32)sizeof(off_t));
		ast_mf_op_d_mainDeb("sizeof(long): %u, "
				"sizeof(long long): %u\n",
				(Tst_uint32)sizeof(long), (Tst_uint32)sizeof(long long));**/
	#endif

	ast_cln_stc_initCln(&cmdln);

	/* is stdout/err a TTY ? */
	ast_g_mf_op_colorUsable[0] = isatty(STDOUT_FILENO);
	ast_g_mf_op_colorUsable[1] = isatty(STDERR_FILENO);
	/* get line len */
	AST_MAIN__getTermCap((Tst_str*)argv[0]);

	/* get app's filename */
	if (! st_sysFileBasename((Tst_str*)argv[0], &pAppFn)) {
		ast_mf_op_d_mainErrApp((Tst_str*)argv[0],
				"getting file's basename failed");
		ST_DELPOINT(pAppFn)
		exit(1);  // error
	}

	/* set/get locale */
	if (setlocale(LC_CTYPE, "") == NULL
				#if ! (defined(_WIN32) || defined (__CYGWIN__))
					&& setlocale(LC_CTYPE, "C.UTF-8") == NULL
				#endif
			) {
		ast_mf_op_d_mainErrApp(pAppFn, "setting locale failed");
		ST_DELPOINT(pAppFn)
		exit(1);  // error
	}
	if ((pLocaleStr = setlocale(LC_CTYPE, NULL)) == NULL) {
		ast_mf_op_d_mainErrApp(pAppFn, "getting locale failed");
		ST_DELPOINT(pAppFn)
		exit(1);  // error
	}
	/* is in-/output UTF8-encoded ? */
	cmdln.opts.isInpISOorU8 = ! ast_mf_op_isLocaleUTF8(pLocaleStr);
	#if (LOC_SHOWLOCALE_ == 1 && AST_MF_OUTP_DEB_ == 1)
		ast_mf_op_prE("locale='%s', cs='%s', isISOorU8=%d\n",
				pLocaleStr, nl_langinfo(CODESET), cmdln.opts.isInpISOorU8);
	#endif

	/* set callbacks */
	cmdln.cbDbg = ast_mf_op_cb_clnDbg;
	cmdln.cbErr = ast_mf_op_cb_clnErr;
	cmdln.cbMsg = ast_mf_op_cb_clnMsg;

	/* parse commands, options and further arguments */
	resF = ast_cln_pa_parseArgs(pAppFn, (Tst_uint32)argc,
			(const Tst_str**)argv, &cmdln, &parbeg);
	if (resF != ST_ERR_SUCC) {
		ast_cln_stc_freeCln(&cmdln);
		if (resF == ST_ERR_QUIT) {
			ST_DELPOINT(pAppFn)
			exit(0);  // success
		}
		if (resF != ST_ERR_ABRT) {
			ast_mf_op_d_mainErrApp(pAppFn,
					"parsing args failed, res=%d", resF);
		}
		ST_DELPOINT(pAppFn)
		exit(1);  // error
	}

	/* check commands */
	if (cmdln.cmds.roMainCmds == 0 && cmdln.cmds.roTgExtCmds == 0 &&
			cmdln.cmds.rwTgRwrCmds == 0 && cmdln.cmds.rwTgEdtCmds == 0 &&
			cmdln.cmds.rwTgCnvCmds == 0 && cmdln.cmds.rwTgRemCmds == 0) {
		cmdln.cmds.rd = ST_B_TRUE;
		++cmdln.cmds.roMainCmds;
	}
	if (parbeg < 1 || parbeg >= (Tst_uint32)argc) {
		ast_mf_op_d_mainErrApp(pAppFn, "need a filename\n\n");
		ast_cln_stc_freeCln(&cmdln);
		ST_DELPOINT(pAppFn)
		exit(1);  // error
	}

	/* check options */
	if (cmdln.opts.isInpISOorU8 && ! cmdln.opts.disp.asISOorU8) {
		ast_mf_op_d_mainErrApp(pAppFn,
				"Notice: Locale is not UTF-8, but --disp-u8 was used");
	}

	/* create output directory */
	if (! st_sysStrEmpty(cmdln.opts.pOutpdir) &&
			! st_sysDoesDirExist(cmdln.opts.pOutpdir)) {
		if (! st_sysCreateDir(cmdln.opts.pOutpdir, ST_SYS_FILEPERM_DEF)) {
			ast_mf_op_d_mainErrApp(pAppFn,
					"couldn't create output dir \"%s\"", cmdln.opts.pOutpdir);
			ast_cln_stc_freeCln(&cmdln);
			ST_DELPOINT(pAppFn)
			exit(1);  // error
		}
		if (cmdln.opts.basOpts.verb != 0) {
			ast_mf_op_d_mainDeb("created output dir \"%s\"", cmdln.opts.pOutpdir);
		}
		wasDCreated = ST_B_TRUE;
	}

	/* init main object */
	resF = ast_mf_stc_initMF(&mf);
	if (resF != ST_ERR_SUCC) {
		ast_mf_op_d_mainErrApp(pAppFn,
				"initialization of main object failed, res=%d", resF);
		ast_cln_stc_freeCln(&cmdln);
		ST_DELPOINT(pAppFn)
		exit(1);  // error
	}

	/* take care of all files given as argument */
	resF = ST_ERR_SUCC;
	for (x = parbeg; x < (Tst_uint32)argc; x++) {
		/* close file */
		if (st_sysFStc_isOpen(&mf.fstc)) {
			st_sysFStc_close(&mf.fstc);
		}

		/* reset main object */
		/** */
		ast_mf_stc_rsetMF(&mf);
		mf.pFilen = (Tst_str*)argv[x];
		st_sysFStc_setFilen(&mf.fstc, mf.pFilen);
		/** */
		ast_mf_stc_setCBsDeb(&cmdln.opts.basOpts.cbsDbg);
		mf.setOptsData.cbAnaMsg = mf.cbAnaMsg;
		mf.setOptsData.pCmdln   = &cmdln;
		st_tagMeta_gs_setArr_cbForSettingOptions(&mf.tagArr,
				AST_MAIN__cbSetOpts_tagIV1, &mf.setOptsData,
				AST_MAIN__cbSetOpts_tagIV2, &mf.setOptsData,
				AST_MAIN__cbSetOpts_tagAV2, &mf.setOptsData,
				AST_MAIN__cbSetOpts_tagVOR, &mf.setOptsData);
		AST_MAIN__setOptsInSubObjs(&mf, &cmdln);

		/* check filesize */
		if (! AST_MAIN__get_n_chk_fsz(&mf, pAppFn, &cmdln, &fsz)) {
			continue;
		}
		/* */
		if (! cmdln.opts.quiet) {
			char cmdType[3];

			if (cmdln.cmds.roTgExtCmds > 0) {
				cmdType[0] = 'E';
				cmdType[1] = 'X';
				cmdType[2] = 'T';
			} else if (cmdln.cmds.rwTgRwrCmds > 0) {
				cmdType[0] = 'R';
				cmdType[1] = 'W';
				cmdType[2] = 'R';
			} else if (cmdln.cmds.rwTgEdtCmds > 0) {
				cmdType[0] = 'E';
				cmdType[1] = 'D';
				cmdType[2] = 'T';
			} else if (cmdln.cmds.rwTgRemCmds > 0) {
				cmdType[0] = 'R';
				cmdType[1] = 'E';
				cmdType[2] = 'M';
			} else if (cmdln.cmds.rwTgCnvCmds > 0) {
				cmdType[0] = 'C';
				cmdType[1] = 'N';
				cmdType[2] = 'V';
			} else if (cmdln.cmds.anlz) {
				cmdType[0] = 'A';
				cmdType[1] = 'N';
				cmdType[2] = 'Z';
			} else if (cmdln.cmds.decAud) {
				cmdType[0] = 'D';
				cmdType[1] = 'E';
				cmdType[2] = 'C';
			} else if (cmdln.opts.quickScan) {
				cmdType[0] = 'Q';
				cmdType[1] = 'S';
				cmdType[2] = 'C';
			} else {  // read
				cmdType[0] = 'R';
				cmdType[1] = 'E';
				cmdType[2] = 'A';
			}
			ast_mf_op_prMsg("File [%c%c%c]: %s", cmdType[0], cmdType[1], cmdType[2], mf.pFilen);
		}
		/* mime / fileformat */
		if (! AST_MAIN__getFFmt(&mf, pAppFn, &cmdln)) {
			continue;
		}

		/* ***********
		 * READ-ONLY  none of the following commands change the input file
		 * ***********/

		/* read file and output info */
		if (fsz > 0) {
			if (cmdln.opts.basOpts.verb != 0) {
				ast_mf_op_d_mainDeb("read \"%s\"...", mf.pFilen);
			}
			/* open file for reading */
			resF = st_sysFStc_openExisting(&mf.fstc,
					cmdln.opts.basOpts.allowLnkInpF, ST_B_FALSE);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"can't open file, res=%d", resF);
				break;
			}
			/* attach mf.fstc with mf.strrd
			 *   --> this affects the current file position etc.
			 *       while the streamreader is attached to mf.fstc
			 *         mf.fstc shouldn't be accessed directly  */
			resF = AST_MAIN__updInput(&mf);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"updating input failed, res=%d", resF);
				break;
			}

			/* read current file */
			resF = ast_mf_rd_readFile(&cmdln, &mf);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"reading file failed, res=%d", resF);
				break;
			}

			if (mf.contTypes == (Tst_int32)AST_MF_CTP_NONE) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"isn't a Audio/Tag file, ignoring it...\n\n");
				ast_mf_op_prMsg("\n");
				continue;
			}

			/* issue message if audio decoding was requested but nothing
			 *   was actually decoded  */
			if (cmdln.cmds.decAud) {
				Tst_bool fndAudioToDec;

				fndAudioToDec = AST_MF_HASCONTTYPE(&mf, AST_MF_CTP_AUD);
				if (fndAudioToDec) {
					fndAudioToDec = (mf.ffmt == AST_MF_FFMT_FLAC ||
								mf.ffmt == AST_MF_FFMT_MPG1 ||
								mf.ffmt == AST_MF_FFMT_OGG);
				}
				if (fndAudioToDec && mf.ffmt == AST_MF_FFMT_OGG) {
					Tst_int32 fndOggBSTPs;

					fndOggBSTPs   = st_contOgg_gs_getBitstreamTypes(&mf.avOgg);
					fndAudioToDec =
							((fndOggBSTPs & ST_CONTOGG_BSTP_FLAC) != 0) ||
							((fndOggBSTPs & ST_CONTOGG_BSTP_VORB) != 0);
				}
				if (! fndAudioToDec && ! cmdln.opts.quiet) {
					ast_mf_op_prMsg("*  No audio found to decode");
				}
			}

			/* convert & merge tags */
			if ((cmdln.cmds.rd || cmdln.cmds.roTgExtCmds > 0) &&
					AST_MF_HASCONTTYPE(&mf, AST_MF_CTP_TAG) &&
					(cmdln.cmds.roTgExtCmds > 0 ||
						ast_cln_fnc_isAnySetInTaglist(cmdln.opts.lstTagSw))) {
				resF = ast_mf_cm_convAndMerge(&cmdln, &mf);
				if (resF != ST_ERR_SUCC) {
					ast_mf_op_d_mainErrFile(mf.pFilen,
							"converting and merging failed, res=%d", resF);
					break;
				}
			}

			/* output file info */
			if (cmdln.cmds.rd && mf.contTypes != (Tst_int32)AST_MF_CTP_NONE) {
				if (cmdln.opts.basOpts.verb != 0) {
					ast_mf_op_d_mainDeb("show \"%s\"...", mf.pFilen);
				}
				resF = ast_mf_sw_showFileInfo(&cmdln, &mf);
				if (resF != ST_ERR_SUCC) {
					ast_mf_op_d_mainErrFile(mf.pFilen,
							"showing file info failed, res=%d", resF);
					break;
				}
			}
		}  /* end (fsz > 0) */

		++fcnt;  /* increase read files counter */

		/* extract data from file */
		if (cmdln.cmds.roTgExtCmds > 0 && fsz > 0) {
			if (cmdln.opts.basOpts.verb != 0) {
				ast_mf_op_d_mainDeb("extract from \"%s\"...", mf.pFilen);
			}
			resF = ast_mf_extr_extrData(&cmdln, &mf);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"extracting data failed, res=%d", resF);
				break;
			}
		}

		/* if any readonly-command or tag-extr-command is set or
		 *   none of the read/write-commands is set skip the rest */
		if (cmdln.cmds.roMainCmds > 0 || cmdln.cmds.roTgExtCmds > 0 ||
				(cmdln.cmds.rwTgRwrCmds == 0 &&
					cmdln.cmds.rwTgEdtCmds == 0 &&
					cmdln.cmds.rwTgRemCmds == 0 &&
					cmdln.cmds.rwTgCnvCmds == 0)) {
			LOC_PRNLN_
			continue;
		}

		/* ***********
		 * READ-WRITE  the following commands may change the input file
		 * ***********/

		/* edit audio/tag file */
		if (cmdln.cmds.rwTgRwrCmds > 0 && fsz == 0) {
			ast_mf_op_d_mainErrFile(mf.pFilen,
					"file empty --> nothing to rewrite, ignoring");
			ast_mf_op_prMsg("\n");
			continue;
		}
		if (cmdln.cmds.rewrFile &&
				! st_tagMeta_gs_getArr_isAnyTagPresent(&mf.tagArr)) {
			ast_mf_op_d_mainErrFile(mf.pFilen,
					"no tags found --> nothing to rewrite, ignoring");
			ast_mf_op_prMsg("\n");
			continue;
		}

		if (cmdln.opts.basOpts.verb != 0) {
			ast_mf_op_d_mainDeb("edit \"%s\"...", mf.pFilen);
		}

		wasFCreated = ST_B_FALSE;
		if (st_sysFStc_isOpen(&mf.fstc)) {
			/* re-open file for reading+writing */
			resF = st_sysFStc_changeMode(&mf.fstc, ST_B_TRUE);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"can't re-open file for writing, res=%d", resF);
			}
		} else if (st_sysDoesFileExist(mf.pFilen)) {
			/* open file for reading+writing */
			resF = st_sysFStc_openExisting(&mf.fstc,
					cmdln.opts.basOpts.allowLnkInpF, ST_B_TRUE);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"can't open file for writing, res=%d", resF);
			}
		} else if (! cmdln.opts.basOpts.pretWr) {
			/* create&open file for writing */
			resF = st_sysFStc_openNew(&mf.fstc);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"can't create file, res=%d", resF);
			}
			wasFCreated = ST_B_TRUE;
		} else {
			/* create&open 'virtual' file for writing */
			resF = st_sysFStc_openNewVirtual(&mf.fstc);
			if (resF != ST_ERR_SUCC) {
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"can't create virtual file, res=%d", resF);
			}
		}
		if (resF != ST_ERR_SUCC) {
			break;
		}
		/* attach mf.fstc with mf.strrd
		 *   --> this affects the current file position etc.
		 *       while the streamreader is attached to mf.fstc
		 *         mf.fstc shouldn't be accessed directly  */
		resF = AST_MAIN__updInput(&mf);
		if (resF != ST_ERR_SUCC) {
			ast_mf_op_d_mainErrFile(mf.pFilen,
					"updating input failed, res=%d", resF);
			break;
		}
		/* */
		resF = ast_mf_ed_editFile(&cmdln, &mf);
		if (resF != ST_ERR_SUCC) {
			if (resF != ST_ERR_ABRT)
				ast_mf_op_d_mainErrFile(mf.pFilen,
						"editing file failed, res=%d", resF);
			break;
		}
		/* close file */
		st_sysFStc_close(&mf.fstc);
		/* remove empty file */
		if (wasFCreated && st_sysGetFileSz_byFn(mf.pFilen, NULL) == 0) {
			st_sysUnlinkFile(mf.pFilen);
		}
		wasFCreated = ST_B_FALSE;
		/* */
		LOC_PRNLN_
	}

	/* remove empty file */
	if (wasFCreated && st_sysFStc_isOpen(&mf.fstc)) {
		st_sysFStc_close(&mf.fstc);
		if (st_sysGetFileSz_byFn(mf.pFilen, NULL) == 0) {
			st_sysUnlinkFile(mf.pFilen);
		}
	}
	/* close file */
	if (st_sysFStc_isOpen(&mf.fstc)) {
		st_sysFStc_close(&mf.fstc);
	}

	/* if output directory is empty, remove it */
	if (wasDCreated) {
		/* TODO */
	}

	if (resF == ST_ERR_SUCC) {
		if (fcnt == 0) {
			ast_mf_op_d_mainErrApp(pAppFn,
					"no files to %s",
					(cmdln.cmds.roMainCmds > 0 || cmdln.cmds.roTgExtCmds > 0 ?
						"read" : "edit"));
			resF = ST_ERR_FAIL;
		} else if (cmdln.opts.basOpts.pretWr && ! cmdln.opts.quiet) {
			ast_mf_op_prMsg("%s: only pretended writing (%u files)",
					pAppFn, fcnt);
		}
	}
	/* */
	if (resF == ST_ERR_SUCC) {
		if (cmdln.opts.showStat) {
			ast_mf_op_prMsg("*Done.");
		}
		resA = 0;  // success
	} else {
		resA = 1;  // error
	}
	/* */
	ast_mf_stc_freeMF(&mf);
	ast_cln_stc_freeCln(&cmdln);
	ST_DELPOINT(pAppFn)

	return resA;
	#undef LOC_PRNLN_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static void
AST_MAIN__getTermCap(const Tst_str *pAppFn)
{
	char const        *p = getenv("COLUMNS");
	unsigned long int tmp_ulong;
	#ifdef TIOCGWINSZ
		struct winsize ws;
	#endif

	if (p && *p) {
		if (sscanf(p, "%lu", &tmp_ulong) == 1 &&  // NOLINT(*-err34-c)
				tmp_ulong >= 65 && tmp_ulong <= 2048) {
			ast_g_mf_op_termLnLen = tmp_ulong;
		} else {
			ast_mf_op_d_mainErrApp(pAppFn,
					"ignoring invalid width in env variable COLUMNS: '%s'",
					p);
		}
	}
	#ifdef TIOCGWINSZ
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1 &&
				ws.ws_col >= 65 && ws.ws_col <= 2048) {
			ast_g_mf_op_termLnLen = ws.ws_col;
		}
	#endif
}

/*----------------------------------------------------------------------------*/

/*
 * Callback for setting options for ID3v1 Tags
 */
static void
AST_MAIN__cbSetOpts_tagIV1(void *pSetOptsData, Tst_id3v1_tag *pTag)
{
	Tast_mf_setOptsData *pSOD;

	ST_ASSERTN_VOID(pSetOptsData == NULL || pTag == NULL)

	pSOD = (Tast_mf_setOptsData*)pSetOptsData;

	st_id3v1_opts_setBasicOpts(pTag, &pSOD->pCmdln->opts.basOpts);
}

/*
 * Callback for setting options for ID3v2 Tags
 */
static void
AST_MAIN__cbSetOpts_tagIV2(void *pSetOptsData, Tst_id3v2_tag *pTag)
{
	Tast_mf_setOptsData *pSOD;

	ST_ASSERTN_VOID(pSetOptsData == NULL || pTag == NULL)

	pSOD = (Tast_mf_setOptsData*)pSetOptsData;

	st_id3v2_opts_setBasicOpts(pTag, &pSOD->pCmdln->opts.basOpts);

	/* unsync'ing currently breaks compatibility with iTunes 10.1 */
	st_id3v2_opts_setDoUnsyncing(pTag, pSOD->pCmdln->optsTagIV2.allwUnsynch);
	#if (LIBSANTAGGER_HAVE_LIBZ == 1)
		/* compression currently breaks compatibility with iTunes 10.1 */
		st_id3v2_opts_setCompressBigFrames(pTag, pSOD->pCmdln->optsTagIV2.allwCompr);
		st_id3v2_opts_setCB_cbZLib(pTag, AST_MAIN__cbZLibDecompress,
				AST_MAIN__cbZLibCompress);
	#endif
}

/*
 * Callback for setting options for APEv2 Tags
 */
static void
AST_MAIN__cbSetOpts_tagAV2(void *pSetOptsData, Tst_apev2_tag *pTag)
{
	Tast_mf_setOptsData *pSOD;

	ST_ASSERTN_VOID(pSetOptsData == NULL || pTag == NULL)

	pSOD = (Tast_mf_setOptsData*)pSetOptsData;

	st_apev2_opts_setBasicOpts(pTag, &pSOD->pCmdln->opts.basOpts);
}

/*
 * Callback for setting options for Vorbis Tags
 */
static void
AST_MAIN__cbSetOpts_tagVOR(void *pSetOptsData, Tst_vorbc_tag *pTag)
{
	Tast_mf_setOptsData *pSOD;

	ST_ASSERTN_VOID(pSetOptsData == NULL || pTag == NULL)

	pSOD = (Tast_mf_setOptsData*)pSetOptsData;

	st_vorbc_opts_setBasicOpts(pTag, &pSOD->pCmdln->opts.basOpts);
}

/*
 * Set options in sub-objects
 */
static void
AST_MAIN__setOptsInSubObjs(Tast_mf_finfo *pMF, Tast_cln_a *pCmdln)
{
	/* Audio: Mpeg */
	st_mpeg1_opts_setBasicOpts(&pMF->audMpg1, &pCmdln->opts.basOpts);
	st_mpeg1_opts_setAnlzMode(&pMF->audMpg1,
			pCmdln->cmds.anlz, pMF->cbAnaMsg);
	st_mpeg1_opts_setForceLayer(&pMF->audMpg1,
			pCmdln->optsAvMisc.mpeg1_forceMLay);
	st_mpeg1_opts_setForceAudVers(&pMF->audMpg1,
			pCmdln->optsAvMisc.mpeg1_forceMAv);
	st_mpeg1_opts_setDecodeFully(&pMF->audMpg1,
			pCmdln->cmds.anlz || pCmdln->cmds.decAud);
	st_mpeg1_opts_setPCMoutputBPS(&pMF->audMpg1, pCmdln->optsAvMisc.decAudBps);
	st_mpeg1_opts_setLetDecoderPrintUsedBPS(&pMF->audMpg1, ! pCmdln->opts.quiet);
	st_mpeg1_opts_setQuickScan(&pMF->audMpg1,
			pCmdln->cmds.rd && pCmdln->opts.quickScan);

	/* Audio: Ogg */
	/** common */
	st_contOgg_opts_setBasicOpts(&pMF->avOgg, &pCmdln->opts.basOpts);
	st_contOgg_opts_setAnlzMode(&pMF->avOgg,
			pCmdln->cmds.anlz, pMF->cbAnaMsg);
	st_contOgg_opts_setCheckCRCofOGGframes(&pMF->avOgg, pCmdln->cmds.anlz);
	st_contOgg_opts_setReadComments(&pMF->avOgg, ST_B_TRUE);
	st_contOgg_opts_setForceRewriteComments(&pMF->avOgg, pCmdln->cmds.rewrFile);
	st_contOgg_opts_setPCMoutputBPS(&pMF->avOgg, pCmdln->optsAvMisc.decAudBps);
	st_contOgg_opts_setLetDecoderPrintUsedBPS(&pMF->avOgg, ! pCmdln->opts.quiet);
	st_contOgg_opts_setQuickScan(&pMF->avOgg,
			pCmdln->cmds.rd && pCmdln->opts.quickScan);
	/** (Ogg-) Flac */
	st_contOgg_opts_setFlac_decodeFully(&pMF->avOgg,
			pCmdln->cmds.anlz || pCmdln->cmds.decAud);
	st_contOgg_opts_setFlac_checkMD5ofDecodedAudio(&pMF->avOgg,
			pCmdln->cmds.anlz);
	st_contOgg_opts_setFlac_checkCRCofFrHd(&pMF->avOgg,
			pCmdln->cmds.anlz);
	st_contOgg_opts_setFlac_checkCRCofEntireFrames(&pMF->avOgg,
			pCmdln->cmds.anlz);
	/** (Ogg-) Vorbis */
	st_contOgg_opts_setVorb_decodeFully(&pMF->avOgg,
			pCmdln->cmds.anlz || pCmdln->cmds.decAud);
}

/*----------------------------------------------------------------------------*/

/*
 * Check file and get filesize
 */
static Tst_bool
AST_MAIN__get_n_chk_fsz(Tast_mf_finfo *pMF, const Tst_str *pAppFn,
		const Tast_cln_a *pCmdln, Tst_fsize *pFsz)
{
	Tst_bool errIsTooBig = ST_B_FALSE,
	         fex;

	*pFsz = 0;

	if (st_sysStrEmpty(pMF->pFilen)) {
		ast_mf_op_d_mainErrApp(pAppFn, "empty filename, ignoring it...\n\n");
		return ST_B_FALSE;
	}
	if (st_sysDoesDirExist(pMF->pFilen)) {
		/**ast_mf_op_d_mainErrFile(pMF->pFilen,
				"is a path, ignoring it...\n\n");**/
		return ST_B_FALSE;
	}
	fex = st_sysDoesFileExist(pMF->pFilen);
	if (fex) {
		*pFsz       = st_sysGetFileSz_byFn(pMF->pFilen, &errIsTooBig);
		errIsTooBig = errIsTooBig || *pFsz > ST_FILESIZE_MAX;
		if (errIsTooBig) {
			ast_mf_op_d_mainErrFile(pMF->pFilen,
					"file is too big (max %.2f MB ("ST_TFSIZE_PRF_LU" bytes)), "
					"ignoring it...\n\n",
					((double)ST_FILESIZE_MAX / (double)1024.0) / (double)1024.0,
					(Tst_fsize)ST_FILESIZE_MAX);
			return ST_B_FALSE;
		}
	}

	if (pCmdln->cmds.rwTgEdtCmds == 0) {
		if (! fex) {
			ast_mf_op_d_mainErrFile(pMF->pFilen,
					"file doesn't exist or is not readable, ignoring it...\n\n");
			return ST_B_FALSE;
		}
		if (*pFsz < 1) {
			ast_mf_op_d_mainErrFile(pMF->pFilen,
					"file is too small, ignoring it...\n\n");
			return ST_B_FALSE;
		}
	}
	return ST_B_TRUE;
}

/*
 * Get file format
 */
static Tst_bool
AST_MAIN__getFFmt(Tast_mf_finfo *pMF, const Tst_str *pAppFn,
		Tast_cln_a *pCmdln)
{
	Tst_utilsFmt_mimeTp mimTp;

	/* get/set mime-type */
	mimTp = st_utilsFmt_getMime_enum_byFn(pMF->pFilen);
	if (mimTp != ST_UTILSFMT_MTP_NONE) {
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
			ast_mf_op_d_mainDeb("mime: %s",
					st_utilsFmt_getMime_defMime_byEnum(mimTp));
		}
		switch (mimTp) {
			case ST_UTILSFMT_MTP_AUDFLAC:   /* Flac audio */
				pMF->ffmt = AST_MF_FFMT_FLAC; break;
			case ST_UTILSFMT_MTP_AUDMPEG1:  /* MPEG-1 Layer I-III audio */
				pMF->ffmt = AST_MF_FFMT_MPG1; break;
			case ST_UTILSFMT_MTP_AUDOGG:    /* OGG audio */
			case ST_UTILSFMT_MTP_VIDOGG:    /* OGG video */
			case ST_UTILSFMT_MTP_AUDVIDOGG: /* OGG audio/video */
			case ST_UTILSFMT_MTP_APPOGG:    /* OGG app */
				pMF->ffmt = AST_MF_FFMT_OGG; break;
			case ST_UTILSFMT_MTP_APPTAG:    /* Tag file */
			case ST_UTILSFMT_MTP_APPTAGIV1: /* ID3v1 Tag file */
			case ST_UTILSFMT_MTP_APPTAGIV2: /* ID3v2 Tag file */
			case ST_UTILSFMT_MTP_APPTAGAV2: /* APEv2 Tag file */
			case ST_UTILSFMT_MTP_APPTAGVOR: /* Pseudo Vorbis Tag file */
				pMF->ffmt = AST_MF_FFMT_TAGF; break;
			default:
				// do nothing
				break;
		}
	}
	switch (pMF->ffmt) {
		case AST_MF_FFMT_FLAC:   /* Flac audio */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_mainDeb("ffmt: FLAC");
			}
			break;
		case AST_MF_FFMT_MPG1:   /* MPEG-1 Layer I-III audio */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_mainDeb("ffmt: MPEG1");
			}
			break;
		case AST_MF_FFMT_OGG:    /* OGG audio/video/app */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_mainDeb("ffmt: OGG");
			}
			break;
		case AST_MF_FFMT_TAGF:   /* Tag file */
			if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_GEN)) {
				ast_mf_op_d_mainDeb("ffmt: TAG");
			}
			break;
		default:
			/**ast_mf_op_d_mainDeb("ffmt: Other");**/
			pMF->ffmt = AST_MF_FFMT_NONE;
			ast_mf_op_d_mainErrFile(pMF->pFilen,
					"unsupported file-format, ignoring file...\n\n");
			return ST_B_FALSE;
	}
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

static Tst_err
AST_MAIN__updInput(Tast_mf_finfo *pMF)
{
	Tst_err res;

	/* stream reader */
	res = st_streamrd_setInput_file(&pMF->strrd, &pMF->fstc);
	if (res != ST_ERR_SUCC) {
		ast_mf_op_d_mainErrFile(pMF->pFilen,
				"setting input file for stream failed, res=%d", res);
		return res;
	}

	/* tag array */
	st_tagMeta_gs_setArr_strrdForAllTags(&pMF->tagArr, &pMF->strrd);
	res = st_tagMeta_gs_setArr_filenForAllTags(&pMF->tagArr, pMF->pFilen);
	if (res != ST_ERR_SUCC) {
		return res;
	}

	/* mpeg */
	st_mpeg1_gs_setStr_strrd(&pMF->audMpg1, &pMF->strrd);
	res = st_mpeg1_gs_setStr_filen(&pMF->audMpg1, pMF->pFilen);
	if (res != ST_ERR_SUCC) {
		return res;
	}

	/* ogg/flac */
	st_contOgg_gs_setStrrd(&pMF->avOgg, &pMF->strrd);
	res = st_contOgg_gs_setFilen(&pMF->avOgg, pMF->pFilen);
	if (res != ST_ERR_SUCC) {
		return res;
	}

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

#if (LIBSANTAGGER_HAVE_LIBZ == 1)
/*
 * Decompress a ZLIB-compressed buffer/file
 *
 * pBinDatIn        MUST be set
 * pBinDatOut       MUST be set
 * uncomprSzShould  MUST contain the correct size
 *                    of the uncompressed data
 *                    --> that size must be known before decompressing
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MAIN__cbZLibDecompress(Tst_binobj *pBinDatIn,
		const Tst_uint32 uncomprSzShould,
		Tst_binobj *pBinDatOut)
{
	Tst_err           res;
	Tst_buf           *pTmpOut = NULL,
	                  *pTmpIn  = NULL;
	Tst_uint32        inSz     = 0;
	unsigned long int out      = 0;

	ST_ASSERTN_IARG(pBinDatIn == NULL ||
			uncomprSzShould == 0 || pBinDatOut == NULL)

	st_binobj_stc_rsetData(pBinDatOut);
	/* */
	res = st_binobj_copyIntoBuffer(pBinDatIn, &pTmpIn, &inSz);
	if (res != ST_ERR_SUCC) {
		return res;
	}

	ST_CALLOC(pTmpOut, Tst_buf*, uncomprSzShould + 1, 1)
	if (pTmpOut == NULL) {
		ST_DELPOINT(pTmpIn)
		return ST_ERR_OMEM;
	}

	out = (unsigned long)uncomprSzShould;
	if (uncompress(pTmpOut, &out, pTmpIn, inSz) != Z_OK ||
			out != (unsigned long)uncomprSzShould) {
		res = ST_ERR_FAIL;
	}
	ST_DELPOINT(pTmpIn)

	if (res == ST_ERR_SUCC) {
		res = st_binobj_setData(pBinDatOut, pTmpOut, (Tst_uint32)out);
	}

	ST_DELPOINT(pTmpOut)
	return res;
}

/*
 * Compress a buffer/file with ZLIB
 *
 * pBinDatIn    MUST be set
 * pBinDatOut   MUST be set
 *
 * returns: ERRCODE
 */
static Tst_err
AST_MAIN__cbZLibCompress(Tst_binobj *pBinDatIn,
		Tst_binobj *pBinDatOut)
{
	Tst_err           res;
	Tst_buf           *pTmpOut = NULL,
	                  *pTmpIn  = NULL;
	Tst_uint32        inSz     = 0;
	unsigned long int out;

	ST_ASSERTN_IARG(pBinDatIn == NULL || pBinDatOut == NULL)

	st_binobj_stc_rsetData(pBinDatOut);
	/* */
	res = st_binobj_copyIntoBuffer(pBinDatIn, &pTmpIn, &inSz);
	if (res != ST_ERR_SUCC) {
		return res;
	}
	out = compressBound((unsigned long int)inSz);

	ST_CALLOC(pTmpOut, Tst_buf*, (Tst_uint32)out + 512, 1)
	if (pTmpOut == NULL) {
		ST_DELPOINT(pTmpIn)
		return ST_ERR_OMEM;
	}

	if (compress(pTmpOut, &out, pTmpIn, (unsigned long int)inSz) != Z_OK) {
		res = ST_ERR_FAIL;
	}
	ST_DELPOINT(pTmpIn)

	if (res == ST_ERR_SUCC) {
		res = st_binobj_setData(pBinDatOut, pTmpOut, (Tst_uint32)out);
	}

	ST_DELPOINT(pTmpOut)
	return res;
}
#endif  /* zlib */

/******************************************************************************/
