/******************************************************************************/
/* cln_pargs.c                    [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for parsing program options                                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 19.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#include "src/includes/common/sys_fnc.h"
#include "src/includes/common/string_mte.h"
#include "src/includes/tag/tag_comfnc.h"
#include "src/includes/tag/tag_id3v2.h"
/** */
#define SRC_CLN_PARGS_C
#include "cln_pargs-pp.h"
/*** */
#include "cln_af-prot.h"
#include "cln_hndarg-pp.h"
#include "cln_fnc-prot.h"
#include "cln_setdefs-prot.h"
#undef SRC_CLN_PARGS_C

/*
// System-Includes
*/
#ifdef _MSC_VER
	#error MSVC is not supported
#endif
#include <stdlib.h>      /* calloc() */
#include <string.h>      /* strcat() */
#include <ctype.h>       /* isprint() */
#include <unistd.h>      /* getopt(), char(optarg), int(optind,opterr,optopt), ... */
#if (HAVE_LIBVORBIS == 1)
	#include <vorbis/codec.h>
#endif
#if (HAVE_LIBMPG123 == 1)
	#include <mpg123.h>
#endif
#if (HAVE_LIBMAD == 1)
	#include <mad.h>
#endif
#if (HAVE_LIBZ == 1)
	#include <zlib.h>
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Parse commands, options and other params
 *
 * pAppFn   MUST be != NULL
 * argv     MUST be != NULL
 * pCmdln   MUST be set
 * pParbeg  MUST be != NULL
 *
 * returns: ERRCODE
 *          pCmdln and pParbeg will be set
 */
Tst_err
ast_cln_pa_parseArgs(const Tst_str *pAppFn,
		const Tst_uint32 argc, const Tst_str **argv,
		Tast_cln_a *pCmdln, Tst_uint32 *pParbeg)
{
	const char *cFNCN = __func__;
	Tst_err    res    = ST_ERR_SUCC;
	//Tst_int32  doi    = 0,                    /* digit_optind */
	//Tst_int32  tooi   = optind ? optind : 1,  /* this_option_optind */
	Tst_int32  oix    = 0;              /* option_index */
	Tst_int32  c;                       /* will hold option char */
	Tst_uint32 cntarr = 0;              /* amount of array elems */
	/* optind comes from getopt.h */

	ST_ASSERTN_IARG(pAppFn == NULL || argv == NULL || pCmdln == NULL ||
			pParbeg == NULL ||
			pCmdln->cbDbg == NULL || pCmdln->cbErr == NULL ||
			pCmdln->cbMsg == NULL)

	pCmdln->pAppFn = pAppFn;

	/* pre-check args */
	if (argc < 2 || argc > AST_CLN_MAX_ARGS + 1) {
		if (argc > AST_CLN_MAX_ARGS + 1) {
			pCmdln->cbErr(pAppFn, cFNCN,
					"max. %d parameters permitted", AST_CLN_MAX_ARGS);
		} else {
			AST_CLN__pa_printUsage(pAppFn, pCmdln->cbMsg);
		}
		return ST_ERR_ABRT;
	}
	res = ast_cln_fnc_checkForAmbig(pAppFn, cFNCN, argc, argv, pCmdln);
	if (res != ST_ERR_SUCC) {
		return res;
	}
	/* */
	*pParbeg = 0;
	/* */
	pCmdln->opts.disp.asISOorU8 = pCmdln->opts.isInpISOorU8;

	/* count array elements */
	ast_cln_fnc_countLongopts((const Tast_cln_longopts*)&AST_CLN_LONGOPTS,
			&cntarr);
	/* init long option cache array */
	ST_CALLOC(pCmdln->pPAlngoptcArr, Tast_cln_longoptCache*,
			cntarr, sizeof(Tast_cln_longoptCache))
	if (pCmdln->pPAlngoptcArr == NULL) {
		return ST_ERR_OMEM;
	}
	ast_cln_fnc_setLongoptsCache((const Tast_cln_longopts*)&AST_CLN_LONGOPTS,
			cntarr, pCmdln->pPAlngoptcArr);

	/* parse parameters */
	/*opterr = 0;*/  /* don't call exit(1) on invalid params */
	while (res == ST_ERR_SUCC) {
		//tooi = (optind != 0 ? optind : 1);
		oix  = 0;
		c    = getopt_long((int)argc, (char**)argv, AST_CLN_SHORTOPTS,
					AST_CLN_LONGOPTS, &oix);
		if (c == -1) {
			break;
		}
		/** */
		res = AST_CLN__pa_hndArg(c, oix, cntarr, pCmdln);
	}

	if (res == ST_ERR_SUCC &&
			pCmdln->optsTagFlds.attFileArr.cnt > 0) {
		/* print attach-file array */
		if (ST_ISVERB(pCmdln->opts.basOpts.verb, ST_VL_OPTS)) {
			ast_cln_af_prAFarr(pCmdln);
		}
		/* check files in attach-file array */
		res = ast_cln_af_checkAFarr(pCmdln);
	}

	if (res == ST_ERR_SUCC) {
		/* set some options to their defaults */
		ast_cln_sd_setDefs_tag_swOrWr(pCmdln);
	}

	if (res == ST_ERR_SUCC) {
		/* have program params been passed ? */
		if (optind < 0) {
			res = ST_ERR_FAIL;
		} else if ((Tst_uint32)optind < argc) {
			/**ast_cln_prf("    parseArgs: non-option ARGV-elements: ");
			while ((Tst_uint32)optind < argc) {
				ast_cln_prf("\"%s\"", argv[optind++]);
				if (argc >= 1 && (Tst_uint32)(optind - 1) < (argc - 1))
					ast_cln_prf(", ");
			}
			ast_cln_prf("\n");**/
			*pParbeg = (Tst_uint32)optind;
		}
	}
	/* */
	return res;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* print general program usage */
static Tst_err
AST_CLN__pa_printUsage(const Tst_str *pAppFn, const Tast_cln_cbMsg cbMsg)
{
	cbMsg("Usage: %s [commands...] [options...] [file_names...]\n", pAppFn);
	cbMsg("(default command is --%s)\n\n", AST_CLN_CMD_RD_LO);

	cbMsg("Examples: %s song1.mp3 song2.ogg song3.flac song4.mp1\n", pAppFn);
	cbMsg("          %s -%c \"Song 1\" song1.mp3\n", pAppFn, AST_CLN_CMD_SNG_SO);
	cbMsg("          %s --%s \"Linux Gang\" song*.*\n", pAppFn, AST_CLN_CMD_BND_LO);
	cbMsg("          (for more use %s --%s)\n\n", pAppFn, AST_CLN_CMD_EXA_LO);

	cbMsg("For a list of all commands and options please use");
	cbMsg("  %s --%s", pAppFn, AST_CLN_CMD_HLP_LO);

	return ST_ERR_ABRT;
}

/*----------------------------------------------------------------------------*/

/* handle special command --help */
static Tst_err
AST_CLN__pa_sc_help(const Tst_str *pAppFn, const Tast_cln_cbMsg cbMsg,
		const Tst_bool isInpISOorU8)
{
	#define LOC_PR_SL_(op1, op2, va, de) \
				AST_CLN__pa_sc_help_prOne(cbMsg, op1, op2, NULL, va, de, ST_B_FALSE)
	#define LOC_PR_S_(op1, va, de) \
				AST_CLN__pa_sc_help_prOne(cbMsg, op1, NULL, NULL, va, de, ST_B_FALSE)
	#define LOC_PR_L_(op2, va, de) \
				AST_CLN__pa_sc_help_prOne(cbMsg, 0x00, op2, NULL, va, de, ST_B_FALSE)
	#define LOC_PR_L_AO_(op2, addOp, va, de) \
				AST_CLN__pa_sc_help_prOne(cbMsg, 0x00, op2, addOp, va, de, ST_B_FALSE)
	#define LOC_PR_DE_(de) \
				AST_CLN__pa_sc_help_prOne(cbMsg, 0x00, NULL, NULL, NULL, de, ST_B_FALSE)
	#define LOC_PR_DIV_ \
				AST_CLN__pa_sc_help_prOne(cbMsg, 0x00, NULL, NULL, NULL, NULL, ST_B_TRUE)
	char scona[128];
	char sargs[128];
	char sdesc[128];

	cbMsg("Usage: %s [<commands>...] [<options>...] <file_names>...\n\n", pAppFn);

	/* commands */
	/** readonly-commands */
	cbMsg("To read Tags and Audio/Video streams:");
	LOC_PR_SL_(AST_CLN_CMD_RD_SO, AST_CLN_CMD_RD_LO, NULL,
			"Displays information about the file");
		LOC_PR_DE_("(default command)");
	cbMsg("\n");

	cbMsg("To analize Tags and Audio/Video streams:");
	LOC_PR_L_(AST_CLN_CMD_ANA_LO, NULL,
			"Displays quite comprehensive info about");
		LOC_PR_DE_("the condition of the file");
	cbMsg("\n");

	cbMsg("To decode Audio streams:");
	LOC_PR_L_(AST_CLN_CMD_DECAUD_LO, "wav|aiff",
			"Decode to WAVE or AIFF");
		LOC_PR_DE_("Works with the following input:");
		LOC_PR_DE_(" - Ogg-Flac and pure Flac");
		#if (HAVE_LIBVORBIS == 1)
			LOC_PR_DE_(" - Ogg-Vorbis");
		#endif
		#if (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1)
			LOC_PR_DE_(" - MPEG-1 (Layer I-III, e.g. MP3)");
		#endif
		LOC_PR_DE_("See --bps option");
	cbMsg("\n");

	/** tag-edit-commands */
	cbMsg("To edit or add Tags:");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_STR);
	LOC_PR_SL_(AST_CLN_CMD_BND_SO, AST_CLN_CMD_BND_LO, sargs,
			"Bandname/Artist  (\"\"==none)");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_STR);
	LOC_PR_SL_(AST_CLN_CMD_ALB_SO, AST_CLN_CMD_ALB_LO, sargs,
			"Albumname  (\"\"==none)");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_STR);
	LOC_PR_SL_(AST_CLN_CMD_SNG_SO, AST_CLN_CMD_SNG_LO, sargs,
			"Songname/Title  (\"\"==none)");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_POS);
	LOC_PR_SL_(AST_CLN_CMD_TRK_SO, AST_CLN_CMD_TRK_LO, sargs,
			"Track-Nr.  (0==none)");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_NUM);
	LOC_PR_L_(AST_CLN_CMD_TRK_TOT_LO, sargs,
			"Tracks total  (0==none)");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_POS);
	LOC_PR_SL_(AST_CLN_CMD_DSC_SO, AST_CLN_CMD_DSC_LO, sargs,
			"Disc-Nr.  (0==none)");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_NUM);
	LOC_PR_L_(AST_CLN_CMD_DSC_TOT_LO, sargs,
			"Discs total  (0==none)");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_NUM);
	LOC_PR_SL_(AST_CLN_CMD_YEA_SO, AST_CLN_CMD_YEA_LO, sargs,
			"Year  (0==none)");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_STR);
	LOC_PR_SL_(AST_CLN_CMD_COM_SO, AST_CLN_CMD_COM_LO, sargs,
			"Comment  (\"\"==none)");
	snprintf(sargs, sizeof(sargs), "help|<%s|%s>",
			AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_STR);
	LOC_PR_SL_(AST_CLN_CMD_GEN_SO, AST_CLN_CMD_GEN_LO, sargs,
			"Genre  (255 or \"\"==none)");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_FILEN);
	LOC_PR_L_AO_(AST_CLN_CMD_ADD_PIC_LO, "--pic-*", sargs,
			"Attach picture to ID3v2/Vorbis Tag");
		LOC_PR_DE_("You can use this command more than once");
		LOC_PR_DE_("See --pic-* options");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_NUM);
	LOC_PR_L_AO_(AST_CLN_CMD_MOD_PIC_LO, "--pic-*", sargs,
			"Modify picture in ID3v2/Vorbis Tag");
		snprintf(sdesc, sizeof(sdesc),
				"The special value 0 for <%s> means", AST_CLN_SVT_FMT_NUM);
		LOC_PR_DE_(sdesc);
		LOC_PR_DE_("modify all pictures.");
		LOC_PR_DE_("You can use this command more than once");
		LOC_PR_DE_("See --pic-* options");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_NUMLIST);
	LOC_PR_L_(AST_CLN_CMD_REM_PIC_LO, sargs,
			"Remove picture(s) from ID3v2/Vorbis Tag");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_FILEN);
	LOC_PR_L_AO_(AST_CLN_CMD_ADD_GEO_LO, "--geo-*", sargs,
			"Attach any type of file to ID3v2/Ape Tag");
		LOC_PR_DE_("You can use this command more than once");
		LOC_PR_DE_("See --geo-* options");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_NUM);
	LOC_PR_L_AO_(AST_CLN_CMD_MOD_GEO_LO, "--geo-*", sargs,
			"Modify attached file in ID3v2/Ape Tag");
		snprintf(sdesc, sizeof(sdesc),
				"The special value 0 for <%s> means", AST_CLN_SVT_FMT_NUM);
		LOC_PR_DE_(sdesc);
		LOC_PR_DE_("modify all attached files.");
		LOC_PR_DE_("You can use this command more than once");
		LOC_PR_DE_("See --geo-* options");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_NUMLIST);
	LOC_PR_L_(AST_CLN_CMD_REM_GEO_LO, sargs,
			"Remove attached file from ID3v2/Ape Tag");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_FLDADD);
	LOC_PR_L_(AST_CLN_CMD_FADD_LO, sargs,
			"Add a tag field");
		LOC_PR_DE_("You can use this command more than once");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_FLDSET);
	LOC_PR_L_(AST_CLN_CMD_FSET_LO, sargs,
			"Set/Remove a tag field");
		LOC_PR_DE_("You can use this command more than once");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_DSCSET);
	LOC_PR_L_(AST_CLN_CMD_DSET_LO, sargs,
			"Set a ID3v2 tag field's description");
		LOC_PR_DE_("You can use this command more than once");
	cbMsg("\n");

	cbMsg("To rewrite files:");
	LOC_PR_L_(AST_CLN_CMD_REWR_LO, NULL,
			"Rewrites all tags and audio/video streams");
		LOC_PR_DE_("found in the file and corrects some");
		LOC_PR_DE_("common problems while doing so");
	cbMsg("\n");

	/** tag-rem-commands */
	cbMsg("To remove Tags:");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_TAGLIST);
	LOC_PR_L_(AST_CLN_CMD_REM_TAG_LO, sargs,
			"Removes all given tags from the file");
	cbMsg("\n");

	/** tag-extr-commands */
	cbMsg("To extract data:  (you may combine several --extr-* commands)");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_TAGLIST);
	LOC_PR_L_(AST_CLN_CMD_EXTR_TAG_LO, sargs,
			"Extract Tag(s)");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_NUMLIST);
	LOC_PR_L_(AST_CLN_CMD_EXTR_PIC_LO, sargs,
			"Extract picture(s) from Tags");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_NUMLIST);
	LOC_PR_L_(AST_CLN_CMD_EXTR_GEO_LO, sargs,
			"Extract GEO(s) from Tags");
	LOC_PR_L_(AST_CLN_CMD_EXTR_BD_LO, NULL,
			"Extract all binary data from Tags");
		LOC_PR_DE_("fields (except for Pic/GEO)");
	cbMsg("\n");

	/** tag-conv-commands */
	cbMsg("To convert tags:");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_TAGPAIR);
	LOC_PR_L_(AST_CLN_CMD_CONV_LO, sargs,
			"Converts all given tag types into");
		LOC_PR_DE_("the specified tag type");
	cbMsg("\n");

	/** special commands */
	cbMsg("Special commands:");
	LOC_PR_L_(AST_CLN_CMD_INF_LO, NULL,
			"Show general program info");
	LOC_PR_L_(AST_CLN_CMD_VER_LO, NULL,
			"Show program version");
	LOC_PR_L_(AST_CLN_CMD_LIC_LO, NULL,
			"Show program's license");
	LOC_PR_L_(AST_CLN_CMD_EXA_LO, NULL,
			"Show program usage examples");
	LOC_PR_SL_(AST_CLN_CMD_HLP_SO, AST_CLN_CMD_HLP_LO, NULL,
			"Show this help");
	cbMsg("\n");

	/* options */
	/** */
	cbMsg("Options:");
	LOC_PR_S_(AST_CLN_OPT_QUIET_SO, NULL,
			"Don't output anything (but errors)");
	LOC_PR_S_(AST_CLN_OPT_SWSTAT_SO, NULL,
			"Show some status info");
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_VLEVLIST);
	LOC_PR_L_(AST_CLN_OPT_VERBLEV_LO, sargs,
			"Verbosity level");
		LOC_PR_DE_("You can use this option more than once");
	LOC_PR_DIV_;
	LOC_PR_SL_(AST_CLN_OPT_PRT_SO, AST_CLN_OPT_PRT_LO, NULL,
			"Don't modify input files");
	LOC_PR_DIV_;
	LOC_PR_L_(AST_CLN_OPT_QSCAN_LO, NULL,
			"Perform only a quick scan");
	/** */
	#if ! (defined(_WIN32) || defined (__CYGWIN__))
		LOC_PR_DIV_;
		LOC_PR_L_(AST_CLN_OPT_ALLWLNKS_LO, NULL,
				"Allow sym-/hardlinks as input files");
		LOC_PR_L_(AST_CLN_OPT_DISALLWLNKS_LO, NULL,
				"Disallow sym-/hardlinks as input files  [default]");
	#endif
	/** */
	LOC_PR_DIV_;
	LOC_PR_L_(AST_CLN_OPT_DISP_ALL_LO, NULL,
			"Display all tag fields");
	LOC_PR_L_(AST_CLN_OPT_DISP_FS_LO, NULL,
			"Display strings in tag fields in full length,");
		LOC_PR_DE_("don't cut them off and don't replace");
		LOC_PR_DE_("chars like \\n with \"[\\n]\".");
		LOC_PR_DE_("Also displays binary data");
	LOC_PR_L_(AST_CLN_OPT_DISP_DET_LO, NULL,
			"Display details of tag fields");
	LOC_PR_L_(AST_CLN_OPT_DISP_ISO_LO, NULL,
			"Display strings in ISO-8859-1 encoding");
	if (isInpISOorU8)
		LOC_PR_DE_("(current setting)");
	LOC_PR_L_(AST_CLN_OPT_DISP_U8_LO, NULL,
			"Display strings in Unicode UTF-8 encoding");
	if (! isInpISOorU8)
		LOC_PR_DE_("(current setting)");
	/** */
	LOC_PR_DIV_;
	snprintf(sdesc, sizeof(sdesc), "Tags to show  [default:%s]",
			AST_CLN_DEF_TAG_SW_STR);
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_TAGLIST);
	LOC_PR_L_(AST_CLN_OPT_TAG_SW_LO, sargs, sdesc);
	snprintf(sdesc, sizeof(sdesc), "Tags to edit/write  [default:%s]",
			AST_CLN_DEF_TAG_WR_STR);
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_TAGLIST);
	LOC_PR_L_(AST_CLN_OPT_TAG_ED_LO, sargs, sdesc);
	/** */
	LOC_PR_DIV_;
	snprintf(sdesc, sizeof(sdesc),
			"Select bitstream(s) to edit  [default:%s]",
			AST_CLN_DEF_EBS_STR);
	snprintf(sargs, sizeof(sargs), "help|<%s>", AST_CLN_SVT_FMT_BSLIST);
	LOC_PR_L_(AST_CLN_OPT_EBS_LO, sargs, sdesc);
	/** */
	LOC_PR_DIV_;
	cbMsg("  For MPEG-1 audio streams:");
	snprintf(scona, sizeof(scona), "%s/2/3", AST_CLN_OPT_FRC_MP1_LO);
	LOC_PR_L_(scona, NULL,
			"Force MPEG-1 Layer I, II or III");
	snprintf(scona, sizeof(scona), "%s/2/25", AST_CLN_OPT_FRC_AV1_LO);
	LOC_PR_L_(scona, NULL,
			"Force MPEG-1 Audio Version 1, 2 or 2.5");
	/** */
	LOC_PR_DIV_;
	cbMsg("  For decoding audio streams:");
	LOC_PR_L_(AST_CLN_OPT_BPS_LO, "0|16|24|32",
			"Bits per sample to write decoded samples with");
		snprintf(sdesc, sizeof(sdesc),
				"[default:%d]  (0 means let decoder decide)",
				AST_CLN_DEF_BPS_VAL);
		LOC_PR_DE_(sdesc);
		LOC_PR_DE_("(when decoding Flac this option is ignored)");
	/** */
	LOC_PR_DIV_;
	cbMsg("  For ID3v2 Tags:");
	snprintf(sargs, sizeof(sargs), "help|<%s>",
			AST_CLN_SVT_FMT_TIV2OPTLIST);
	LOC_PR_L_(AST_CLN_OPT_TIV2_LO, sargs, "Set some ID3v2 specific options");
		snprintf(sdesc, sizeof(sdesc), "[default:%s]", AST_CLN_DEF_TIV2_STR);
		LOC_PR_DE_(sdesc);
		LOC_PR_DE_("You can use this option more than once");
	/** */
	LOC_PR_DIV_;
	cbMsg("  These options should be used after the corresponding");
	cbMsg("    --add-pic/geo or --mod-pic/geo:");
	snprintf(sdesc, sizeof(sdesc), "Type of picture to attach  [default:%s]",
			st_id3v2_fnc_getPicTp_capShort_byEnum(AST_CLN_DEF_PIC_TP_VAL));
	snprintf(sargs, sizeof(sargs), "help|<%s|%s>",
			AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_PICTP);
	LOC_PR_L_(AST_CLN_OPT_PIC_TP_LO, sargs, sdesc);
	snprintf(sdesc, sizeof(sdesc),
			"Format of picture to attach  [default:%s]",
			AST_CLN_DEF_PIC_FM_STR);
	snprintf(sargs, sizeof(sargs), "help|<%s|%s>",
			AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_PICFM);
	LOC_PR_L_(AST_CLN_OPT_PIC_FM_LO, sargs, sdesc);
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_STR);
	LOC_PR_L_(AST_CLN_OPT_PIC_DE_LO, sargs,
			"Description of picture to attach");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_MIME);
	LOC_PR_L_(AST_CLN_OPT_GEO_MI_LO, sargs,
			"MIME-Type of file to attach");
	snprintf(sargs, sizeof(sargs), "<%s>", AST_CLN_SVT_FMT_STR);
	LOC_PR_L_(AST_CLN_OPT_GEO_DE_LO, sargs,
			"Description of file to attach");
	LOC_PR_DIV_;
	cbMsg("  These options should be used after the corresponding --mod-pic/geo:");
	LOC_PR_L_(AST_CLN_OPT_PIC_FN_LO, "<filen>",
			"File to exchange picture with");
	LOC_PR_L_(AST_CLN_OPT_GEO_FN_LO, "<filen>",
			"File to exchange attached file with");
	/** */
	LOC_PR_DIV_;
	LOC_PR_L_(AST_CLN_OPT_OUTDIR_LO, "<dirn>",
			"Directory to write new/modified files to");
	LOC_PR_L_(AST_CLN_OPT_OWEXF_LO, NULL,
			"Overwrite existing files");
		LOC_PR_DE_("when decoding or extracting data  [default:no]");

	cbMsg("\n");

	return ST_ERR_QUIT;
	#undef LOC_PR_SL_
	#undef LOC_PR_S_
	#undef LOC_PR_L_
	#undef LOC_PR_L_AO_
	#undef LOC_PR_DE_
	#undef LOC_PR_DIV_
}

static void
AST_CLN__pa_sc_help_prOne(const Tast_cln_cbMsg cbMsg,
		const char opt1, const char *pOpt2,
		const char *pAddOpt,
		const char *pVals, const char *pDesc, const Tst_bool onlyDiv)
{
	const Tst_uint32 cOPTWID = 28;
	char       msg[1024];
	char       tmp[512];
	Tst_uint32 x;

	if (onlyDiv) {
		/* divider */
		for (x = 0; x < cOPTWID; x++) {
			tmp[x] = '*';
		}
		tmp[cOPTWID] = 0x00;
	} else if (opt1 != 0x00 && pOpt2) {
		/* short and long option */
		sprintf(tmp, "-%c, --%s%s%s", opt1, pOpt2,
				(pVals ? "=" : ""), (pVals ? pVals : ""));
	} else if (opt1 != 0x00) {
		/* short option */
		sprintf(tmp, "-%c %s", opt1, (pVals ? pVals : ""));
	} else if (pOpt2) {
		/* long option */
		if (pAddOpt) {
			sprintf(tmp, "--%s %s [%s]", pOpt2, (pVals ? pVals : ""), pAddOpt);
		} else {
			sprintf(tmp, "--%s %s", pOpt2, (pVals ? pVals : ""));
		}
	} else {
		tmp[0] = ' ';
		tmp[1] = 0x00;
	}
	sprintf(msg, "  %-*s", cOPTWID, tmp);
	if (pDesc) {
		sprintf(tmp, " %s", pDesc);
		strcat(msg, tmp);
	}
	cbMsg(msg);
}

/*----------------------------------------------------------------------------*/

/* handle special command --info */
static Tst_err
AST_CLN__pa_sc_info(const Tast_cln_cbMsg cbMsg, const Tst_str* pAppFn)
{
	Tst_str  *pSup = NULL;

	cbMsg("\nApp      : %s %s", APP_ST_NAME, APP_ST_VERS_STRING);
	cbMsg("Compiled : %s %s (%s-Endian)", __DATE__, __TIME__,
			#if (WORDS_BIGENDIAN != 1)
				"Little"
			#else
				"Big"
			#endif
		);
	st_sysStrapp((Tst_str*)"", &pSup);
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		st_sysStrapp((Tst_str*)"64bit ", &pSup);
	#endif
	#if (HAVE_LIBVORBIS == 1)
		st_sysStrapp((Tst_str*)"libvorbis ", &pSup);
	#endif
	#if (HAVE_LIBMPG123 == 1)
		st_sysStrapp((Tst_str*)"libmpg123 ", &pSup);
	#endif
	#if (HAVE_LIBMAD == 1)
		st_sysStrapp((Tst_str*)"libmad ", &pSup);
	#endif
	#if (HAVE_LIBZ == 1)
		st_sysStrapp((Tst_str*)"zlib ", &pSup);
	#endif
	cbMsg("Supports : %s", pSup);
	ST_DELPOINT(pSup);
	cbMsg("Author   : %s", APP_ST_AUTHOR);
	cbMsg("Copyright: %s", APP_ST_COPYR);
	cbMsg("License  : General Public License v3");
	cbMsg("           (for details try '%s --license')\n\n", pAppFn);

	return ST_ERR_QUIT;
}

/*----------------------------------------------------------------------------*/

/* handle special command --version */
static Tst_err
AST_CLN__pa_sc_version(const Tast_cln_cbMsg cbMsg)
{
	#if (HAVE_LIBVORBIS == 1) || (HAVE_LIBMPG123 == 1) || (HAVE_LIBMAD == 1) || (HAVE_LIBZ == 1)
		const char *cNA = "n/a";
		char const *pVer;
	#endif
	Tst_mtes_string tagger;
	Tst_str         *pTmp = NULL;
	Tst_err         tmpRes;

	cbMsg("%s %s", APP_ST_NAME, APP_ST_VERS_STRING);
	//
	st_mtes_stc_initStr(&tagger);
	tmpRes = st_tagCFnc_getTaggerStr(&tagger, ST_B_TRUE);
	if (tmpRes != ST_ERR_SUCC) {
		st_mtes_stc_freeStr(&tagger);
		return tmpRes;
	}
	tmpRes = st_mtes_copyToCharp_iso(&tagger, &pTmp);
	st_mtes_stc_freeStr(&tagger);
	if (tmpRes != ST_ERR_SUCC) {
		return tmpRes;
	}
	cbMsg("[%s]", pTmp);
	//
	#if (HAVE_LIBVORBIS == 1)
		pVer = vorbis_version_string();
		if (pVer == NULL) {
			pVer = cNA;
		}
		cbMsg("[libvorbis: %s]", pVer);
	#endif
	#if (HAVE_LIBMPG123 == 1)
		cbMsg("[libmpg123: API version %d]", MPG123_API_VERSION);
	#endif
	#if (HAVE_LIBMAD == 1)
		pVer = MAD_VERSION;
		cbMsg("[libmad: %s]", pVer);
	#endif
	#if (HAVE_LIBZ == 1)
		pVer = ZLIB_VERSION;
		cbMsg("[zlib: %s]", pVer);
	#endif

	return ST_ERR_QUIT;
}

/*----------------------------------------------------------------------------*/

/* handle special command --license */
static Tst_err
AST_CLN__pa_sc_license(const Tast_cln_cbMsg cbMsg)
{
	cbMsg("\nLicense: GPL version 3\n\n");
	cbMsg(" This program is free software; you can redistribute it and/or");
	cbMsg(" modify it under the terms of the GNU General Public License as");
	cbMsg(" published by the Free Software Foundation; either version 3 of");
	cbMsg(" the License, or (at your option) any later version.\n\n");

	cbMsg(" This program is distributed in the hope that it will be useful,");
	cbMsg(" but WITHOUT ANY WARRANTY; without even the implied warranty of");
	cbMsg(" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
	cbMsg(" GNU General Public License for more details.\n\n");

	cbMsg(" You should have received a copy of the GNU General Public License");
	cbMsg(" along with this program; if not, write to the");
	cbMsg("   Free Software Foundation Inc.,");
	cbMsg("   59 Temple Place, Suite 330, Boston, MA 02111-1307 USA");
	cbMsg(" or visit www.gnu.org\n\n");

	return ST_ERR_QUIT;
}

/*----------------------------------------------------------------------------*/

/* handle special command --examples */
static Tst_err
AST_CLN__pa_sc_examples(const Tast_cln_cbMsg cbMsg, const Tst_str* pAppFn)
{
	#define LOC_PREX_(mac_same, mac_txt)  \
				cbMsg("  %s%s %s", mac_same, pAppFn, mac_txt);
	Tst_uint32 slen  = st_sysStrlen(pAppFn);
	Tst_uint32 spadd = 2 + slen + 1;
	char       msg[512];

	cbMsg("Usage examples:");
	LOC_PREX_("", "*.mp3");
	LOC_PREX_("  same as: ", "-r *.mp3");
	LOC_PREX_("", "-r -v *.mp3");
	LOC_PREX_("  same as: ", "-rv *.mp3");
	LOC_PREX_("", "--disp-all --disp-fs --disp-det *.mp3");
	LOC_PREX_("  same as: ", "--disp-a --disp-f --disp-d *.mp3");
	LOC_PREX_("", "--genre help");
	LOC_PREX_("  same as: ", "-g h");
	LOC_PREX_("", "-b \"My favorite band\" -g 9 *.mp3");
	LOC_PREX_("", "-b \"Flux\" -g \"Prog-Conservative Prayers\" *.mp3");
	LOC_PREX_("", "-s \"Brad Pitt Dancing\" --track-nr 1 --track-tot 10 song01.mp3");
	LOC_PREX_("", "-s \"Angelina In Sight\" --disc-tot 2 song02.mp3");
	LOC_PREX_("", "--track-nr=3/10 --disc-nr=1/2 song03.mp3");
	LOC_PREX_("  same as: ", "-t 3/10 -d 1/2 song03.mp3");
	snprintf(msg, sizeof(msg),
			"--track-nr 3 --track-tot 10 \\\n"
			"%*s--disc-nr 1 --disc-tot 2 song03.mp3",
			spadd + 11, "");
	LOC_PREX_("  same as: ", msg);
	LOC_PREX_("", "--rem-tag id3v1 --pretend *.mp3");
	LOC_PREX_("", "--rem-tag iv1,av2 *.mp3");
	LOC_PREX_("", "-p --lev 3 --album Wonders *.mp3");
	LOC_PREX_("", "-y 2005 --comment \"the best\" song01.mp3 song02.mp3");
	LOC_PREX_("", "--band=\"My favorite band\" --ebs 2 *.ogg");
	LOC_PREX_("", "--ana song01.mp3 song02.mp3");
	LOC_PREX_("", "--ana --lev tag --lev aud *.mp3");
	LOC_PREX_("", "--lev tag,aud --extr-pic 1,3,7 --extr-tag all *.ogg *.flac");
	LOC_PREX_("", "--extr-pic a --extr-bd *.ogg *.mp3");
	LOC_PREX_("", "--force-mp3 --force-av1 *.mp3");
	LOC_PREX_("", "--add-pic cov.jpg --pic-tp cov-back --pic-de \"Japan Ed\" *.mp3");
	snprintf(msg, sizeof(msg),
			"--add-geo bio.xml --geo-de Biography --geo-mi \"application/xml\" \\\n"
			"%*s--add-geo lyr.pdf --geo-de Lyrics *.mp3",
			spadd, "");
	LOC_PREX_("", msg);
	LOC_PREX_("", "--dec wav --ow --ou wavfiles/ *.flac");
	LOC_PREX_("", "--dec aiff --bps 24 *.ogg");
	LOC_PREX_("", "--conv id3v1,apev2-id3v2 --tiv2 v3 *.mp3");
	LOC_PREX_("", "--fset TBPM=150 song_with_150bpm.ogg");

	return ST_ERR_QUIT;
	#undef LOC_PREX_
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Handle return value of getopt_long()
 */
static Tst_err
AST_CLN__pa_hndArg(const Tst_int32 ch, const Tst_int32 oix,
		const Tst_uint32 cntarr, Tast_cln_a *pCmdln)
{
	const char *cFNCN = __func__;
	char debMsg[128];

	switch (ch) {
		case 0:
			/* a valid long_opt that doesn't have a shortcut has been found */
			if (oix < 0 || (Tst_uint32)oix > cntarr ||
					AST_CLN_LONGOPTS[oix].name == NULL) {
				pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
						"unknown error occured");
				return ST_ERR_FAIL;
			}
			return AST_CLN__pa_hndArgLong(oix, pCmdln);
		/** */
		case AST_CLN_CMD_HLP_SO:     /* help */
			return AST_CLN__pa_sc_help(pCmdln->pAppFn, pCmdln->cbMsg,
					pCmdln->opts.disp.asISOorU8);
		/** */
		case AST_CLN_OPT_QUIET_SO:   /* be quiet */
			return ast_cln_hnd_o_quiet(pCmdln);
		case AST_CLN_OPT_SWSTAT_SO:  /* show status info */
			return ast_cln_hnd_o_swstat(pCmdln);
		case AST_CLN_OPT_PRT_SO:     /* pretend */
			return ast_cln_hnd_o_prt(pCmdln);
		/** */
		case AST_CLN_CMD_BND_SO:     /* band */
			return ast_cln_hnd_c_tfld_bnd(pCmdln);
		case AST_CLN_CMD_ALB_SO:     /* album */
			return ast_cln_hnd_c_tfld_alb(pCmdln);
		case AST_CLN_CMD_SNG_SO:     /* song */
			return ast_cln_hnd_c_tfld_sng(pCmdln);
		case AST_CLN_CMD_TRK_SO:     /* tracknr */
			return ast_cln_hnd_c_tfld_tnr(pCmdln);
		case AST_CLN_CMD_DSC_SO:     /* discnr */
			return ast_cln_hnd_c_tfld_dsc(pCmdln);
		case AST_CLN_CMD_YEA_SO:     /* year */
			return ast_cln_hnd_c_tfld_yea(pCmdln);
		case AST_CLN_CMD_COM_SO:     /* comment */
			return ast_cln_hnd_c_tfld_com(pCmdln);
		case AST_CLN_CMD_GEN_SO:     /* genre num */
			return ast_cln_hnd_c_tfld_gen(pCmdln);
		/** */
		case AST_CLN_CMD_RD_SO:      /* read */
			return ast_cln_hnd_c_rd(pCmdln);
		/** */
		case '?':
			/* getopt_long() has already output error msg */
			/* after unknown option or missing argument of an option
			 *   we stop processing -> opterr = 0 */
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"try '%s --help' for more information", pCmdln->pAppFn);
			return ST_ERR_ABRT;
		default:
			/* a valid option hasn't been handled */
			debMsg[0] = 0;
			if (isprint(ch) != 0)
				sprintf(debMsg, " '%c'", ch);
			pCmdln->cbErr(pCmdln->pAppFn, cFNCN,
					"char #0%o%s unhandled", ch, debMsg);
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Handle arguments that do have long option only  (e.g. "--rem-tag")
 */
static Tst_err
AST_CLN__pa_hndArgLong(const Tst_int32 oix, Tast_cln_a *pCmdln)
{
	#define LOC_CMP_(mac_pArg)  \
				(parFC == (mac_pArg)[0] && parSLen == st_sysStrlen2(mac_pArg) && \
						st_sysStrcmp2(ST_B_TRUE, pParName, mac_pArg))
	char const *pParName;
	char       parFC;
	Tst_uint32 parSLen;

	ST_ASSERTN_NUM(ST_ERR_FAIL, oix < 0 || pCmdln == NULL ||
			pCmdln->pPAlngoptcArr == NULL)

	pParName = pCmdln->pPAlngoptcArr[oix].pOptName;  /*AST_CLN_LONGOPTS[oix].name*/
	parFC    = pParName[0];
	if (pCmdln->pPAlngoptcArr[oix].slen < 0) {
		pCmdln->pPAlngoptcArr[oix].slen = (Tst_int32)st_sysStrlen2(pParName);
	}
	parSLen = (Tst_uint32)pCmdln->pPAlngoptcArr[oix].slen;

	/* special commands */
	if (LOC_CMP_(AST_CLN_CMD_INF_LO))
		return AST_CLN__pa_sc_info(pCmdln->cbMsg, pCmdln->pAppFn);
	if (LOC_CMP_(AST_CLN_CMD_VER_LO))
		return AST_CLN__pa_sc_version(pCmdln->cbMsg);
	if (LOC_CMP_(AST_CLN_CMD_LIC_LO))
		return AST_CLN__pa_sc_license(pCmdln->cbMsg);
	if (LOC_CMP_(AST_CLN_CMD_EXA_LO))
		return AST_CLN__pa_sc_examples(pCmdln->cbMsg, pCmdln->pAppFn);

	/* commands */
	/** */
	if (LOC_CMP_(AST_CLN_CMD_ANA_LO))
		return ast_cln_hnd_c_ana(pCmdln);
	if (LOC_CMP_(AST_CLN_CMD_DECAUD_LO))
		return ast_cln_hnd_c_decAud(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_CMD_TRK_TOT_LO))
		return ast_cln_hnd_c_tfld_tnr_tot(pCmdln);
	if (LOC_CMP_(AST_CLN_CMD_DSC_TOT_LO))
		return ast_cln_hnd_c_tfld_dsc_tot(pCmdln);
	if (LOC_CMP_(AST_CLN_CMD_ADD_PIC_LO))
		return ast_cln_hnd_c_tfld_add_picOrGeo(pCmdln, ST_B_TRUE);
	if (LOC_CMP_(AST_CLN_CMD_ADD_GEO_LO))
		return ast_cln_hnd_c_tfld_add_picOrGeo(pCmdln, ST_B_FALSE);
	if (LOC_CMP_(AST_CLN_CMD_MOD_PIC_LO))
		return ast_cln_hnd_c_tfld_mod_picOrGeo(pCmdln, ST_B_TRUE);
	if (LOC_CMP_(AST_CLN_CMD_MOD_GEO_LO))
		return ast_cln_hnd_c_tfld_mod_picOrGeo(pCmdln, ST_B_FALSE);
	if (LOC_CMP_(AST_CLN_CMD_REM_PIC_LO))
		return ast_cln_hnd_c_tfld_rem_picOrGeo(pCmdln, ST_B_TRUE);
	if (LOC_CMP_(AST_CLN_CMD_REM_GEO_LO))
		return ast_cln_hnd_c_tfld_rem_picOrGeo(pCmdln, ST_B_FALSE);
	if (LOC_CMP_(AST_CLN_CMD_FADD_LO))
		return ast_cln_hnd_c_tfld_fadd(pCmdln);
	if (LOC_CMP_(AST_CLN_CMD_FSET_LO))
		return ast_cln_hnd_c_tfld_fset(pCmdln);
	if (LOC_CMP_(AST_CLN_CMD_DSET_LO))
		return ast_cln_hnd_c_tfld_dset(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_CMD_REWR_LO))
		return ast_cln_hnd_c_rewr(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_CMD_CONV_LO))
		return ast_cln_hnd_c_conv(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_CMD_REM_TAG_LO))
		return ast_cln_hnd_c_rem_tag(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_CMD_EXTR_TAG_LO))
		return ast_cln_hnd_c_extr_tag(pCmdln);
	if (LOC_CMP_(AST_CLN_CMD_EXTR_PIC_LO))
		return ast_cln_hnd_c_extr_picOrGeo(pCmdln, ST_B_TRUE);
	if (LOC_CMP_(AST_CLN_CMD_EXTR_GEO_LO))
		return ast_cln_hnd_c_extr_picOrGeo(pCmdln, ST_B_FALSE);
	if (LOC_CMP_(AST_CLN_CMD_EXTR_BD_LO))
		return ast_cln_hnd_c_extr_bd(pCmdln);

	/* options */
	/** */
	if (LOC_CMP_(AST_CLN_OPT_VERBLEV_LO))
		return ast_cln_hnd_o_verblev(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_QSCAN_LO))
		return ast_cln_hnd_o_qscan(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_ALLWLNKS_LO))
		return ast_cln_hnd_o_allwlnks(pCmdln, ST_B_TRUE);
	if (LOC_CMP_(AST_CLN_OPT_DISALLWLNKS_LO))
		return ast_cln_hnd_o_allwlnks(pCmdln, ST_B_FALSE);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_DISP_ALL_LO))
		return ast_cln_hnd_o_disp_all(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_DISP_DET_LO))
		return ast_cln_hnd_o_disp_det(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_DISP_FS_LO))
		return ast_cln_hnd_o_disp_fstr(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_DISP_ISO_LO))
		return ast_cln_hnd_o_disp_isoOrU8(pCmdln, ST_B_TRUE);
	if (LOC_CMP_(AST_CLN_OPT_DISP_U8_LO))
		return ast_cln_hnd_o_disp_isoOrU8(pCmdln, ST_B_FALSE);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_TAG_SW_LO))
		return ast_cln_hnd_o_tag_swOrEd(pCmdln, 0);
	if (LOC_CMP_(AST_CLN_OPT_TAG_ED_LO))
		return ast_cln_hnd_o_tag_swOrEd(pCmdln, 2);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_EBS_LO))
		return ast_cln_hnd_o_ebs(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_FRC_MP1_LO))
		return ast_cln_hnd_o_force_mpX(pCmdln, 1);
	if (LOC_CMP_(AST_CLN_OPT_FRC_MP2_LO))
		return ast_cln_hnd_o_force_mpX(pCmdln, 2);
	if (LOC_CMP_(AST_CLN_OPT_FRC_MP3_LO))
		return ast_cln_hnd_o_force_mpX(pCmdln, 3);
	if (LOC_CMP_(AST_CLN_OPT_FRC_AV1_LO))
		return ast_cln_hnd_o_force_avX(pCmdln, 1);
	if (LOC_CMP_(AST_CLN_OPT_FRC_AV2_LO))
		return ast_cln_hnd_o_force_avX(pCmdln, 2);
	if (LOC_CMP_(AST_CLN_OPT_FRC_AV25_LO))
		return ast_cln_hnd_o_force_avX(pCmdln, 25);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_BPS_LO))
		return ast_cln_hnd_o_bps(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_TIV2_LO))
		return ast_cln_hnd_o_tiv2(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_PIC_TP_LO))
		return ast_cln_hnd_o_pic_tp(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_PIC_FM_LO))
		return ast_cln_hnd_o_pic_fm(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_PIC_DE_LO))
		return ast_cln_hnd_o_pic_de(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_GEO_MI_LO))
		return ast_cln_hnd_o_geo_mi(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_GEO_DE_LO))
		return ast_cln_hnd_o_geo_de(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_PIC_FN_LO))
		return ast_cln_hnd_o_pic_fn(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_GEO_FN_LO))
		return ast_cln_hnd_o_geo_fn(pCmdln);
	/** */
	if (LOC_CMP_(AST_CLN_OPT_OUTDIR_LO))
		return ast_cln_hnd_o_od(pCmdln);
	if (LOC_CMP_(AST_CLN_OPT_OWEXF_LO))
		return ast_cln_hnd_o_ow(pCmdln);

	return ST_ERR_FAIL;
	#undef LOC_CMP_
}

/******************************************************************************/
