/******************************************************************************/
/* cln_hndhelp.c                  [Sandy Tagger]                         tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Functions for handling help pages                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 07.04.2011 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/dynlist.h>
#include <santag/common/sys_fnc.h>
#include <santag/utils/utils_fmt.h>
#include <santag/tag/tag_id3_helper.h>
#include <santag/tag/tag_id3v2.h>
/** */
#define SRC_CLN_HNDHELP_C
#include "cln_hndhelp-pp.h"
/*** */
#include "cln_hndhelp_fnc-prot.h"
#include "cln_fnc-prot.h"
#include "cln_stcs-prot.h"
#undef SRC_CLN_HNDHELP_C

/*
// System-Includes
*/
#include <string.h>      /* strchr() */
#include <stdio.h>       /* snprintf() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* help for --genre */
void
ast_cln_hhlp_show_c_genre(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP  = AST_CLN_CMD_GEN_LO;
	const char cOPTCAPSH = AST_CLN_CMD_GEN_SO;
	char const *pC;
	char       msg[256];

	pCmdln->cbMsg("Help for --%s <%s|%s>:\n\n",
			cOPTCAP, AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_STR);

	ast_cln_hhlp_show_c_genreValues(pCmdln, ST_B_FALSE);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  -%c \"\"", cOPTCAPSH);
	pCmdln->cbMsg("    same as: -%c 255", cOPTCAPSH);
	pC = st_id3_getGenreCap(9);
	if (strchr(pC, ' ') != NULL)
		pCmdln->cbMsg("  -%c \"%s\"", cOPTCAPSH, pC);
	else
		pCmdln->cbMsg("  -%c %s", cOPTCAPSH, pC);
	pCmdln->cbMsg("    same as: -%c %d", cOPTCAPSH, 9);
	pC = st_id3_getGenreCap(138);
	if (strchr(pC, ' ') != NULL)
		pCmdln->cbMsg("  --%s \"%s\"", cOPTCAP, pC);
	else
		pCmdln->cbMsg("  --%s %s", cOPTCAP, pC);
	pCmdln->cbMsg("    same as: --%s %d", cOPTCAP, 138);
	sprintf(msg, "Deep Throat");
	pCmdln->cbMsg("  -%c \"%s\"", cOPTCAPSH, msg);
	pCmdln->cbMsg("    same as: --%s \"%s\"", cOPTCAP, msg);
	pCmdln->cbMsg("  ID3v2 special genre:");
	pCmdln->cbMsg("    -%c remix", cOPTCAPSH);
}

void
ast_cln_hhlp_show_c_genreValues(Tast_cln_a *pCmdln, const Tst_bool full)
{
	const char *cOPTCAP = AST_CLN_CMD_GEN_LO;
	Tst_uint32 x;

	if (full)
		pCmdln->cbMsg("Help for --%s <%s|%s>: Possible Values\n\n",
				cOPTCAP, AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_STR);

	pCmdln->cbMsg("Possible values are:");
	pCmdln->cbMsg("  -Num-   -Str-");
	if (full)
		for (x = 0; x <= ST_ID3_GENRE_DEFMAXNR; x++)
			pCmdln->cbMsg("   %3u     %s", x, st_id3_getGenreCap((Tst_int32)x));
	else {
		x = 0;
		pCmdln->cbMsg("   %3u     %s", x, st_id3_getGenreCap((Tst_int32)x));
		pCmdln->cbMsg("  (through)");
		x = ST_ID3_GENRE_DEFMAXNR;
		pCmdln->cbMsg("   %3u     %s", x, st_id3_getGenreCap((Tst_int32)x));
		pCmdln->cbMsg(" (to see the entire list please use '--%s %s')\n",
				cOPTCAP, AST_CLN_ARG_HLPVALS);
	}
	ast_cln_hhlpfnc_c_genre_part(pCmdln);
}

/*----------------------------------------------------------------------------*/

/* help for --fadd/fset */
void
ast_cln_hhlp_show_c_faddset(Tast_cln_a *pCmdln, const char *pOptCap,
		const Tst_bool addOrSet)
{
	char const *pFmtFldAddSet = (addOrSet ?
				AST_CLN_SVT_FMT_FLDADD : AST_CLN_SVT_FMT_FLDSET);
	char msg[128];
	Tast_cln_t_tagList_b(allwTags);

	ast_cln_stc_rsetT_taglist(allwTags, ST_B_TRUE);
	allwTags[AST_CLN_T_TAGLIST_IV1] = ST_B_FALSE;
	allwTags[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;


	pCmdln->cbMsg("Help for --%s <%s>:\n\n", pOptCap, pFmtFldAddSet);
	ast_cln_hhlpfnc_fmt_flddscaddset(pCmdln,
			(addOrSet ?
				AST_CLN_HHLP_FMT_FADD_FLDNAME | AST_CLN_HHLP_FMT_FADD_STR :
				AST_CLN_HHLP_FMT_FSET_FLDNAME | AST_CLN_HHLP_FMT_FSET_STR),
			/*isTagOptional:*/ST_B_TRUE, NULL, NULL);
	ast_cln_hhlpfnc_fmt_tag(pCmdln);
	pCmdln->cbMsg("\n");

	snprintf(msg, sizeof(msg),
			"Possible values for <%s> in <%s> are",
			AST_CLN_SVT_FMT_TAG, pFmtFldAddSet);
	AST_CLN__hhlp_t_taglist_posvals(pCmdln, msg,
			allwTags,
			/*autoAllw:*/ST_B_FALSE, /*nonAllw:*/ST_B_FALSE,
			/*allAllw:*/ST_B_FALSE);

	pCmdln->cbMsg("To see the list of possible field names");
	pCmdln->cbMsg("please use '--%s %sIV2' for ID3v2 tag fields",
			pOptCap, AST_CLN_ARG_HLPVALS);
	pCmdln->cbMsg("        or '--%s %sAV2' for APEv2 tag fields",
			pOptCap, AST_CLN_ARG_HLPVALS);
	pCmdln->cbMsg("        or '--%s %sVOR' for Vorbis tag fields",
			pOptCap, AST_CLN_ARG_HLPVALS);
	pCmdln->cbMsg("\n");

	if (addOrSet)
		pCmdln->cbMsg("With this command you can add tag fields.");
	else
		pCmdln->cbMsg("With this command you can add, modify or "
				"reset (aka remove) tag fields.");
	pCmdln->cbMsg("The <%s> only defines how the field's name is interpreted,",
			AST_CLN_SVT_FMT_TAG);
	pCmdln->cbMsg("but it does not define in which tag the field will be used.");
	pCmdln->cbMsg("For example you may use a ID3v2 <%s> but the field",
			AST_CLN_SVT_FMT_FLDNAME);
	pCmdln->cbMsg("may be used in a APEv2 or Vorbis tag.");
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Note that");
	pCmdln->cbMsg("- the default for <%s> in <%s> is ID3v2",
			AST_CLN_SVT_FMT_TAG, pFmtFldAddSet);
	pCmdln->cbMsg("  and if you omit <%s> then ID3v2 will be assumed",
			AST_CLN_SVT_FMT_TAG);
	if (addOrSet) {
		pCmdln->cbMsg("- some tag fields are only allowed once per tag");
	} else {
		pCmdln->cbMsg("- if you don't specify the field's number");
		pCmdln->cbMsg("  * and one or more fields with the specified field name");
		pCmdln->cbMsg("    already exist in a tag then");
		pCmdln->cbMsg("    all appropriate tag fields will be modified/resetted");
		pCmdln->cbMsg("  * and a tag doesn't already contain the specified field");
		pCmdln->cbMsg("    then a new field will be added to the tag");
		pCmdln->cbMsg("- if you don't specify a value for a field then");
		pCmdln->cbMsg("  all appropriate tag fields will be resetted");
	}
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s %s:TBPM=200", pOptCap, AST_CLN_ARG_TIV2SH);
	pCmdln->cbMsg("    same as: --%s %s:TBPM=200", pOptCap, AST_CLN_ARG_TIV2);
	pCmdln->cbMsg("         or: --%s TBPM=200", pOptCap);
	pCmdln->cbMsg("  --%s %s:MY_TEMPO_IS=200", pOptCap, AST_CLN_ARG_TVORSH);
	pCmdln->cbMsg("    same as: --%s %s:MY_TEMPO_IS=200",
			pOptCap, AST_CLN_ARG_TVOR);
	pCmdln->cbMsg("  --%s \"%s:IS IT RYTHMIC=yes, but not so much\"",
			pOptCap, AST_CLN_ARG_TAV2SH);
	if (! addOrSet) {
		pCmdln->cbMsg("  --%s %s:PRIV=", pOptCap, AST_CLN_ARG_TIV2SH);
		pCmdln->cbMsg("    same as: --%s PRIV=", pOptCap);
		pCmdln->cbMsg("         or: --%s PRIV=\"\"", pOptCap);
		pCmdln->cbMsg("  --%s COMM.2=", pOptCap);
		pCmdln->cbMsg("  --%s \"%s:LOCATION.1=At the Beach\"",
				pOptCap, AST_CLN_ARG_TVORSH);
		pCmdln->cbMsg("  --%s %s:LOCATION.2=Indoor",
				pOptCap, AST_CLN_ARG_TVORSH);
	}
}

void
ast_cln_hhlp_show_c_faddsetValues(Tast_cln_a *pCmdln, const char *pOptCap,
		const Tst_bool addOrSet, const Tast_cln_t_tagListIx whichHlp)
{
	char const *pFmtFldAddSet = (addOrSet ?
				AST_CLN_SVT_FMT_FLDADD : AST_CLN_SVT_FMT_FLDSET);
	char msg[128];

	snprintf(msg, sizeof(msg),
			"Help for --%s <%s>: Possible Values", pOptCap, pFmtFldAddSet);

	switch (whichHlp) {
	case AST_CLN_T_TAGLIST_IV2:
		pCmdln->cbMsg("%s (ID3v2)\n\n", msg);
		ast_cln_hhlpfnc_fmt_flddscaddset(pCmdln,
				(addOrSet ? AST_CLN_HHLP_FMT_FADD_FLDNAME :
					AST_CLN_HHLP_FMT_FSET_FLDNAME),
				/*isTagOptional:*/ST_B_TRUE,
				AST_CLN_ARG_TIV2, AST_CLN_ARG_TIV2SH);

		AST_CLN__hhlp_c_faddset_posvalsIV2(pCmdln, pOptCap,
				/*fldOrDsc:*/ST_B_TRUE, addOrSet);

		pCmdln->cbMsg("Examples:");
		if (addOrSet) {
			pCmdln->cbMsg("  --%s \"TXX=Summer breeze made my ice melt\"",
					pOptCap);
			pCmdln->cbMsg("    same as: --%s \"TXXX=It's colder outside at night\"",
					pOptCap);
			pCmdln->cbMsg("  --%s \"TAM=My custom AM text field\"", pOptCap);
			pCmdln->cbMsg("    same as: --%s \"%s:TAM=My custom AM text field\"",
					pOptCap, AST_CLN_ARG_TIV2SH);
			pCmdln->cbMsg("  --%s \"TONG=My custom ONG text field\"", pOptCap);
			pCmdln->cbMsg("  --%s \"W123=http://www.123.org\"", pOptCap);
		} else {
			pCmdln->cbMsg("  --%s AENC=", pOptCap);
			pCmdln->cbMsg("  --%s CNT=13", pOptCap);
			pCmdln->cbMsg("    same as: --%s PCNT=13", pOptCap);
			pCmdln->cbMsg("         or: --%s %s:PCNT=13",
					pOptCap, AST_CLN_ARG_TIV2SH);
			pCmdln->cbMsg("  --%s \"TAR=A custom text field\"", pOptCap);
			pCmdln->cbMsg("  --%s \"WID3=http://id3.org\"", pOptCap);
		}
		break;
	case AST_CLN_T_TAGLIST_AV2:
		pCmdln->cbMsg("%s (APEv2)\n\n", msg);
		ast_cln_hhlpfnc_fmt_flddscaddset(pCmdln,
				(addOrSet ? AST_CLN_HHLP_FMT_FADD_STR :
					AST_CLN_HHLP_FMT_FSET_STR),
				/*isTagOptional:*/ST_B_FALSE,
				AST_CLN_ARG_TAV2, AST_CLN_ARG_TAV2SH);

		AST_CLN__hhlp_c_faddset_posvalsAV2_or_VOR(pCmdln, pOptCap,
				addOrSet, /*av2OrVor:*/ST_B_TRUE);
		break;
	case AST_CLN_T_TAGLIST_VOR:
		pCmdln->cbMsg("%s (Vorbis)\n\n", msg);
		ast_cln_hhlpfnc_fmt_flddscaddset(pCmdln,
				(addOrSet ? AST_CLN_HHLP_FMT_FADD_STR :
					AST_CLN_HHLP_FMT_FSET_STR),
				/*isTagOptional:*/ST_B_FALSE,
				AST_CLN_ARG_TVOR, AST_CLN_ARG_TVORSH);

		AST_CLN__hhlp_c_faddset_posvalsAV2_or_VOR(pCmdln, pOptCap,
				addOrSet, /*av2OrVor:*/ST_B_FALSE);
		break;
	default:
		break;
	}
}

/*----------------------------------------------------------------------------*/

/* help for --dset */
void
ast_cln_hhlp_show_c_dset(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP = AST_CLN_CMD_DSET_LO;

	pCmdln->cbMsg("Help for --%s <%s>:\n\n", cOPTCAP, AST_CLN_SVT_FMT_DSCSET);
	ast_cln_hhlpfnc_fmt_flddscaddset(pCmdln, AST_CLN_HHLP_FMT_DSET_FLDNAME,
			/*isTagOptional:*/ST_B_TRUE, NULL, NULL);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("To see the list of possible values for <%s>",
			AST_CLN_SVT_FMT_FLDNAME);
	pCmdln->cbMsg("please use '--%s %s'",
			cOPTCAP, AST_CLN_ARG_HLPVALS);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("With this command you can set, modify and reset");
	pCmdln->cbMsg("the description of certain ID3v2 tag fields.");
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s COMM=Biography", cOPTCAP);
	pCmdln->cbMsg("  --%s \"TXX.2=Back in the '80s\"", cOPTCAP);
	pCmdln->cbMsg("  --%s WXX=", cOPTCAP);
}

void
ast_cln_hhlp_show_c_dsetValues(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP = AST_CLN_CMD_DSET_LO;

	pCmdln->cbMsg("Help for --%s <%s>: Possible Values (ID3v2)\n\n",
			cOPTCAP, AST_CLN_SVT_FMT_DSCSET);

	ast_cln_hhlpfnc_fmt_flddscaddset(pCmdln, AST_CLN_HHLP_FMT_DSET_FLDNAME,
			/*isTagOptional:*/ST_B_TRUE, NULL, NULL);

	AST_CLN__hhlp_c_faddset_posvalsIV2(pCmdln, cOPTCAP,
			/*fldOrDsc:*/ST_B_FALSE, /*addOrSet:*/ST_B_FALSE);

	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s \"USLT=Original Lyrics\"", cOPTCAP);
	pCmdln->cbMsg("  --%s \"WXXX.1=The band's MySpace page\"", cOPTCAP);
	pCmdln->cbMsg("  --%s TXXX.2=", cOPTCAP);
}

/*----------------------------------------------------------------------------*/

/* help for --conv */
void
ast_cln_hhlp_show_c_conv(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP = AST_CLN_CMD_CONV_LO;
	char msg[128];
	Tast_cln_t_tagList_b(allwTags);

	ast_cln_stc_rsetT_taglist(allwTags, ST_B_TRUE);
	allwTags[AST_CLN_T_TAGLIST_MRG] = ST_B_FALSE;

	pCmdln->cbMsg("Help for --%s <%s>:\n\n", cOPTCAP, AST_CLN_SVT_FMT_TAGPAIR);
	ast_cln_hhlpfnc_fmt_tagpair(pCmdln);
	ast_cln_hhlpfnc_fmt_taglist(pCmdln);
	ast_cln_hhlpfnc_fmt_tag(pCmdln);
	pCmdln->cbMsg("\n");

	snprintf(msg, sizeof(msg), "Possible values for <%s> on the left side are",
			AST_CLN_SVT_FMT_TAG);
	AST_CLN__hhlp_t_taglist_posvals(pCmdln, msg,
			allwTags,
			/*autoAllw:*/ST_B_FALSE, /*nonAllw:*/ST_B_FALSE,
			/*allAllw:*/ST_B_TRUE);
	snprintf(msg, sizeof(msg), "Possible values for <%s> on the right side are",
			AST_CLN_SVT_FMT_TAG);
	/*allwTags[AST_CLN_T_TAGLIST_IV1] = ST_B_FALSE;*/
	AST_CLN__hhlp_t_taglist_posvals(pCmdln, msg,
			allwTags,
			/*autoAllw:*/ST_B_FALSE, /*nonAllw:*/ST_B_FALSE,
			/*allAllw:*/ST_B_FALSE);

	pCmdln->cbMsg("Note that");
	pCmdln->cbMsg("- if a certain tag type is not allowed for the type of");
	pCmdln->cbMsg("  the current file then no conversion will be performed.");
	pCmdln->cbMsg("  E.g. in FLAC files nothing than Vorbis tags are allowed");
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s %s-%s  (convert all ID3v1 to ID3v2)",
			cOPTCAP, AST_CLN_ARG_TIV1, AST_CLN_ARG_TIV2);
	pCmdln->cbMsg("    same as: --%s %s-%s",
			cOPTCAP, AST_CLN_ARG_TIV1SH, AST_CLN_ARG_TIV2SH);
	pCmdln->cbMsg("  --%s %s,%s-%s  (convert all ID3v1 and ID3v2 to APEv2)",
			cOPTCAP, AST_CLN_ARG_TIV1, AST_CLN_ARG_TIV2, AST_CLN_ARG_TAV2);
	pCmdln->cbMsg("    same as: --%s %s,%s-%s", cOPTCAP,
			AST_CLN_ARG_TIV1SH, AST_CLN_ARG_TIV2SH, AST_CLN_ARG_TAV2SH);
	pCmdln->cbMsg("  --%s %s-%s --%s %s (convert all to ID3v2.3)",
			cOPTCAP, AST_CLN_ARG_ALL, AST_CLN_ARG_TIV2,
			AST_CLN_OPT_TIV2_LO, AST_CLN_ARG_TIV2O_V3);
	pCmdln->cbMsg("    same as: --%s %s-%s --%s %s",
			cOPTCAP, AST_CLN_ARG_ALLSH, AST_CLN_ARG_TIV2SH,
			AST_CLN_OPT_TIV2_LO, AST_CLN_ARG_TIV2O_V3);
	pCmdln->cbMsg("         or: --%s %s-%s --%s %s",
			cOPTCAP, AST_CLN_ARG_ALL, AST_CLN_ARG_TIV2SH,
			AST_CLN_OPT_TIV2_LO, AST_CLN_ARG_TIV2O_V3);
	pCmdln->cbMsg("         or: --%s %s-%s --%s %s",
			cOPTCAP, AST_CLN_ARG_ALLSH, AST_CLN_ARG_TIV2,
			AST_CLN_OPT_TIV2_LO, AST_CLN_ARG_TIV2O_V3);
	pCmdln->cbMsg("  --%s %s-%s --%s %s (convert all ID3v2 to ID3v2.2)",
			cOPTCAP, AST_CLN_ARG_TIV2SH, AST_CLN_ARG_TIV2SH,
			AST_CLN_OPT_TIV2_LO, AST_CLN_ARG_TIV2O_V2);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* help for --lev */
void
ast_cln_hhlp_show_o_verblev(Tast_cln_a *pCmdln)
{
#	define LOC_GETCAP_(lev)  \
				ast_cln_hhlpfnc_o_verblev_getCap(ST_VL_##lev, ST_B_FALSE)
#	define LOC_GETSH_(lev)  \
				ast_cln_hhlpfnc_o_verblev_getCap(ST_VL_##lev, ST_B_TRUE)
	const char *cOPTCAP = AST_CLN_OPT_VERBLEV_LO;
	char       msg[128];
	Tast_cln_t_verblevList_capDesc const *pCapDesc = AST_CLN_VERBLIST_CAPDESC;

	pCmdln->cbMsg("Help for --%s <%s>:\n\n", cOPTCAP, AST_CLN_SVT_FMT_VLEVLIST);
	ast_cln_hhlpfnc_fmt_vlevlist(pCmdln);
	ast_cln_hhlpfnc_fmt_vlev(pCmdln);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Possible values are:");
	pCmdln->cbMsg("  -Num-   -Str-   -StrShort-   -Desc-");
	while (pCapDesc->lev >= 0 && pCapDesc->pCap != NULL) {
		sprintf(msg, "%3d", pCapDesc->lev);
		pCmdln->cbMsg("   %s     %-4s    %s            %s",
				msg, pCapDesc->pCap,
				(pCapDesc->pCapSh ? pCapDesc->pCapSh : " "),
				pCapDesc->pDesc);
		++pCapDesc;
	}
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Note that");
	pCmdln->cbMsg("- you may combine two or more values, "
			"e.g. %d + %d: --%s %d",
			ST_VL_TVREAD, ST_VL_TVSET, cOPTCAP, ST_VL_TVREAD | ST_VL_TVSET);
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s %s,%s,%s", cOPTCAP,
			LOC_GETCAP_(TVREAD), LOC_GETCAP_(TVSET), LOC_GETCAP_(TVWRIT));
	pCmdln->cbMsg("    same as: --%s %s --%s %s --%s %s",
			cOPTCAP, LOC_GETCAP_(TVREAD),
			cOPTCAP, LOC_GETCAP_(TVSET),
			cOPTCAP, LOC_GETCAP_(TVWRIT));
	pCmdln->cbMsg("         or: --%s %d,%d,%d", cOPTCAP,
			ST_VL_TVREAD, ST_VL_TVSET, ST_VL_TVWRIT);
	pCmdln->cbMsg("         or: --%s %d", cOPTCAP,
			ST_VL_TVREAD | ST_VL_TVSET | ST_VL_TVWRIT);
	pCmdln->cbMsg("  --%s %s --%s %s", cOPTCAP, LOC_GETCAP_(GEN),
			cOPTCAP, LOC_GETCAP_(OPTS));
	pCmdln->cbMsg("    same as: --%s %d", cOPTCAP, ST_VL_GEN | ST_VL_OPTS);
	pCmdln->cbMsg("  --%s %s,%d", cOPTCAP, LOC_GETCAP_(TAG), ST_VL_AUD);
	pCmdln->cbMsg("    same as: --%s %d", cOPTCAP, ST_VL_TAG | ST_VL_AUD);
	pCmdln->cbMsg("  --%s %s", cOPTCAP, AST_CLN_ARG_ALL);
	pCmdln->cbMsg("  --%s %s", cOPTCAP, AST_CLN_ARG_NONESH);
#	undef LOC_GETCAP_
#	undef LOC_GETSH_
}

/*----------------------------------------------------------------------------*/

/* help for --pic-tp */
void
ast_cln_hhlp_show_o_picTp(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP = AST_CLN_OPT_PIC_TP_LO;
	Tst_str const *pCapSh;
	Tst_id3v2_picTp ptp = (Tst_id3v2_picTp)0;

	pCmdln->cbMsg("Help for --%s <%s|%s>:\n\n",
			cOPTCAP, AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_PICTP);

	pCmdln->cbMsg("Possible values are:");
	pCmdln->cbMsg("  -Num-    -PicTp-           -Desc-");
	while ((pCapSh = st_id3v2_fnc_getPicTp_capShort_byEnum(ptp)) != NULL) {
		pCmdln->cbMsg("   %3d      %-11s       %s", ptp,
				pCapSh, st_id3v2_fnc_getPicTp_cap_byEnum(ptp));
		ptp = (Tst_id3v2_picTp)((int)ptp + 1);
	}
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Examples:");
	ptp = ST_ID3V2_PT_3232;
	pCmdln->cbMsg("  --%s %d", cOPTCAP, ptp);
	pCmdln->cbMsg("    same as: --%s %s", cOPTCAP,
			st_id3v2_fnc_getPicTp_capShort_byEnum(ptp));
	ptp = ST_ID3V2_PT_COVFRO;
	pCmdln->cbMsg("  --%s %d", cOPTCAP, ptp);
	pCmdln->cbMsg("    same as: --%s %s", cOPTCAP,
			st_id3v2_fnc_getPicTp_capShort_byEnum(ptp));
}

/*----------------------------------------------------------------------------*/

/* help for --pic-fm */
void
ast_cln_hhlp_show_o_picFm(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP = AST_CLN_OPT_PIC_FM_LO;
	Tst_str const *pFE;
	Tst_utilsFmt_picFmt pfm = (Tst_utilsFmt_picFmt)0;

	pCmdln->cbMsg("Help for --%s <%s|%s>:\n\n",
			cOPTCAP, AST_CLN_SVT_FMT_NUM, AST_CLN_SVT_FMT_PICFM);

	pCmdln->cbMsg("Possible values are:");
	pCmdln->cbMsg("  -Num-    -PicFm-");
	while ((pFE = st_utilsFmt_getPicFmt_fext_byEnum(pfm)) != NULL) {
		pCmdln->cbMsg("   %3d      %-3s", pfm, pFE);
		pfm = (Tst_utilsFmt_picFmt)((int)pfm + 1);
	}
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Normally there should be no need to use this option");
	pCmdln->cbMsg("since this application determines the format by itself.");
	pCmdln->cbMsg("Nevertheless you may use this option to enforce an image format.");
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Examples:");
	pfm = ST_UTILSFMT_PF_TIF;
	pCmdln->cbMsg("  --%s %d", cOPTCAP, pfm);
	pCmdln->cbMsg("    same as: --%s %s", cOPTCAP,
			st_utilsFmt_getPicFmt_fext_byEnum(pfm));
	pfm = ST_UTILSFMT_PF_JPG;
	pCmdln->cbMsg("  --%s %d", cOPTCAP, pfm);
	pCmdln->cbMsg("    same as: --%s %s", cOPTCAP,
			st_utilsFmt_getPicFmt_fext_byEnum(pfm));
}

/*----------------------------------------------------------------------------*/

/* help for --ebs */
void
ast_cln_hhlp_show_o_ebs(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP = AST_CLN_OPT_EBS_LO;

	pCmdln->cbMsg("Help for --%s <%s>:\n\n", cOPTCAP, AST_CLN_SVT_FMT_BSLIST);
	ast_cln_hhlpfnc_fmt_bslist(pCmdln);
	ast_cln_hhlpfnc_fmt_bs(pCmdln);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("OGG files can contain multiple bitstreams and");
	pCmdln->cbMsg("if you don't want to edit all of them at once,");
	pCmdln->cbMsg("you may specify one or more bitstreams that");
	pCmdln->cbMsg("shall be edited.");
	pCmdln->cbMsg("Furthermore a bitstream can contain multiple");
	pCmdln->cbMsg("sub-bitstreams.");
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("When using the command -%c the numbers of all",
			AST_CLN_CMD_RD_SO);
	pCmdln->cbMsg("bitstreams will be displayed, if the file");
	pCmdln->cbMsg("contains more than one bitstream");
	pCmdln->cbMsg("(video files usually contain several bitstreams).");
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Note that");
	pCmdln->cbMsg("- bitstream 'x' is the same as 'x.0' and 'x.1'");
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s %s", cOPTCAP, AST_CLN_ARG_ALL);
	pCmdln->cbMsg("  --%s 1", cOPTCAP);
	pCmdln->cbMsg("    same as: --%s 1.0", cOPTCAP);
	pCmdln->cbMsg("         or: --%s 1.1", cOPTCAP);
	pCmdln->cbMsg("  --%s 1,3", cOPTCAP);
	pCmdln->cbMsg("    same as: --%s 1.0,3.0", cOPTCAP);
	pCmdln->cbMsg("         or: --%s 1.1,3.1", cOPTCAP);
	pCmdln->cbMsg("  --%s 2.2", cOPTCAP);
	pCmdln->cbMsg("  --%s 4,4.2,5", cOPTCAP);
	pCmdln->cbMsg("    same as: --%s 4.1,4.2,5.1", cOPTCAP);
	pCmdln->cbMsg("    same as: --%s 4.0,4.2,5.0", cOPTCAP);
}

/*----------------------------------------------------------------------------*/

/* help for --tiv2 */
void
ast_cln_hhlp_show_o_tiv2(Tast_cln_a *pCmdln)
{
	const char *cOPTCAP = AST_CLN_OPT_TIV2_LO;
	Tast_cln_t_tiv2optList_capDesc const *pCapDesc = AST_CLN_TIV2OPTLIST_CAPDESC;

	pCmdln->cbMsg("Help for --%s <%s>:\n\n", cOPTCAP, AST_CLN_SVT_FMT_TIV2OPTLIST);
	ast_cln_hhlpfnc_fmt_tiv2optlist(pCmdln);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Possible values for <%s> in <%s> are",
			AST_CLN_SVT_FMT_TIV2OPT, AST_CLN_SVT_FMT_TIV2OPTLIST);
	pCmdln->cbMsg("  -TIV2Opt-    -Desc-");
	while (pCapDesc->pCap != NULL && pCapDesc->pDesc != NULL) {
		pCmdln->cbMsg("   %-6s       %s", pCapDesc->pCap, pCapDesc->pDesc);
		++pCapDesc;
	}
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("With this option you can control how ID3v2 tags are written.");
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Note that");
	pCmdln->cbMsg("- some major programs (like that made by a big company with");
	pCmdln->cbMsg("  a fruit in their logo) don't support compressed");
	pCmdln->cbMsg("  or unsynchronized tag fields.");
	pCmdln->cbMsg("  (The reason might be that reading such tag fields can consume");
	pCmdln->cbMsg("   an almost arbitrary amount of memory when not using "
			"temporary files)");
	pCmdln->cbMsg("  It is therefore recommended to not use these features");
	pCmdln->cbMsg("- some major programs (like the one mentioned before)");
	pCmdln->cbMsg("  are not capable of entirely reading ID3v2.4 tags correctly.");
	pCmdln->cbMsg("  It is therefore recommended to use ID3v2.3 tags");
	pCmdln->cbMsg("- the tag version is only used when writing new tags");
	pCmdln->cbMsg("  or converting tags (with --%s) and the destination tag is ID3v2,",
			AST_CLN_CMD_CONV_LO);
	pCmdln->cbMsg("  but not when rewriting existing tags");
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s %s", cOPTCAP, AST_CLN_ARG_TIV2O_CMPR);
	pCmdln->cbMsg("  --%s %s", cOPTCAP, AST_CLN_ARG_TIV2O_V4);
	pCmdln->cbMsg("  --%s %s,%s,%s", cOPTCAP, AST_CLN_ARG_TIV2O_NOCMPR,
			AST_CLN_ARG_TIV2O_NOUNSY, AST_CLN_ARG_TIV2O_V2);
	pCmdln->cbMsg("    same as:  --%s %s --%s %s --%s %s",
			cOPTCAP, AST_CLN_ARG_TIV2O_NOCMPR,
			cOPTCAP, AST_CLN_ARG_TIV2O_NOUNSY,
			cOPTCAP, AST_CLN_ARG_TIV2O_V2);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* help for <taglist> */
void
ast_cln_hhlp_show_t_taglist(Tast_cln_a *pCmdln, const char *pOptCap,
		const Tast_cln_t_tagList_pb(pAllwTags),
		const Tst_bool autoAllw, const Tst_bool nonAllw)
{
	char msg[128];

	pCmdln->cbMsg("Help for --%s <%s>:\n\n", pOptCap, AST_CLN_SVT_FMT_TAGLIST);
	ast_cln_hhlpfnc_fmt_taglist(pCmdln);
	ast_cln_hhlpfnc_fmt_tag(pCmdln);
	pCmdln->cbMsg("\n");

	snprintf(msg, sizeof(msg), "Possible values for <%s> in <%s> are",
			AST_CLN_SVT_FMT_TAG, AST_CLN_SVT_FMT_TAGLIST);
	AST_CLN__hhlp_t_taglist_posvals(pCmdln, msg,
			pAllwTags,
			autoAllw, nonAllw, /*allAllw:*/ST_B_TRUE);

	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s %s,%s", pOptCap,
			AST_CLN_ARG_TIV1, AST_CLN_ARG_TIV2);
	pCmdln->cbMsg("    same as: --%s %s,%s", pOptCap,
			AST_CLN_ARG_TIV1SH, AST_CLN_ARG_TIV2SH);
	pCmdln->cbMsg("  --%s %s", pOptCap, AST_CLN_ARG_ALL);
	if (autoAllw)
		pCmdln->cbMsg("  --%s %s", pOptCap, AST_CLN_ARG_AUTOSH);
	if (nonAllw)
		pCmdln->cbMsg("  --%s %s", pOptCap, AST_CLN_ARG_NONESH);
}

/*----------------------------------------------------------------------------*/

/* help for <numlist> */
void
ast_cln_hhlp_show_t_numlist(Tast_cln_a *pCmdln, const char *pOptCap)
{
	pCmdln->cbMsg("Help for --%s <%s>:\n\n", pOptCap, AST_CLN_SVT_FMT_NUMLIST);
	ast_cln_hhlpfnc_fmt_numlist(pCmdln);
	ast_cln_hhlpfnc_fmt_numrange(pCmdln);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Possible values are:");
	pCmdln->cbMsg("  -Num-    -Str-   -StrShort-   -Desc-");
	pCmdln->cbMsg("     1                           index of tag field");
	pCmdln->cbMsg("  (through)");
	pCmdln->cbMsg("   999                           index of tag field");
	pCmdln->cbMsg("            %-4s    %s            %s",
			AST_CLN_ARG_ALL, AST_CLN_ARG_ALLSH, "all matching fields");
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s 1", pOptCap);
	pCmdln->cbMsg("  --%s 1,2", pOptCap);
	pCmdln->cbMsg("  --%s 1,3,4,5,6,8,9,10", pOptCap);
	pCmdln->cbMsg("    same as: --%s 1,3-6,8-10", pOptCap);
	pCmdln->cbMsg("  --%s %s", pOptCap, AST_CLN_ARG_ALL);
}

/*----------------------------------------------------------------------------*/

/* help for <pos> */
void
ast_cln_hhlp_show_t_pos(Tast_cln_a *pCmdln, const char *pOptCap,
		const char *pOptCapTot)
{
	pCmdln->cbMsg("Help for --%s <%s>:\n\n", pOptCap, AST_CLN_SVT_FMT_POS);
	ast_cln_hhlpfnc_fmt_pos(pCmdln);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("This command can be used to combine two commands:");
	pCmdln->cbMsg("  --%s and --%s\n", pOptCap, pOptCapTot);
	pCmdln->cbMsg("The special value '0' resets all appropriate tag fields.");
	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Examples:");
	pCmdln->cbMsg("  --%s 0", pOptCap);
	pCmdln->cbMsg("    same as: --%s 0/", pOptCap);
	pCmdln->cbMsg("  --%s 0/0", pOptCap);
	pCmdln->cbMsg("    same as: --%s 0 --%s 0", pOptCap, pOptCapTot);
	pCmdln->cbMsg("  --%s 1", pOptCap);
	pCmdln->cbMsg("    same as: --%s 1/", pOptCap);
	pCmdln->cbMsg("  --%s 2/3", pOptCap);
	pCmdln->cbMsg("    same as: --%s 2 --%s 3", pOptCap, pOptCapTot);
	pCmdln->cbMsg("  --%s 0/4", pOptCap);
	pCmdln->cbMsg("    same as: --%s 0 --%s 4", pOptCap, pOptCapTot);
	pCmdln->cbMsg("  --%s /5", pOptCap);
	pCmdln->cbMsg("    same as: --%s 5", pOptCapTot);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* for --fadd/set hvIV2 */
static void
AST_CLN__hhlp_c_faddset_posvalsIV2(Tast_cln_a *pCmdln, const char *pOptCap,
		const Tst_bool fldOrDsc, const Tst_bool addOrSet)
{
#	define LOC_VERFL_(mac_verFl)  (((mac_verFl) & 1) == 1 ? '2' : ' '), \
				(((mac_verFl) & 2) == 2 ? '3' : ' '), \
				(((mac_verFl) & 4) == 4 ? '4' : ' ')
#	define LOC_GETFTPSTR_(mac_ftp)  \
				ast_cln_hhlpfnc_c_faddset_posvalsIV2_ftp(mac_ftp)
	const char cID02empty[]  = "   ";
	const char cID34empty[]  = "    ";
	char const *pFmtFldAddSet = (addOrSet ?
				AST_CLN_SVT_FMT_FLDADD : AST_CLN_SVT_FMT_FLDSET);
	Tast_cln_pvalIV2 *pPVelem = NULL;
	Tst_dynlist      pvlist;

	if (st_dynlist_stc_initDL(&pvlist, sizeof(Tast_cln_pvalIV2),
				ast_cln_hhlpfnc_cbRsetElemIV2,
				ast_cln_hhlpfnc_cbRsetElemIV2) != ST_ERR_SUCC)
		return;
	if (! ast_cln_hhlpfnc_c_faddset_posvalsIV2_genlist(fldOrDsc,
				addOrSet, &pvlist))
		return;

	ast_cln_hhlpfnc_fmt_fldname(pCmdln);
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Possible values for ID3v2 <%s> are:",
			AST_CLN_SVT_FMT_FLDNAME);
	pCmdln->cbMsg("  -FnShort- -FnLong- -FldType- -VerFl- -Desc-");
	if (st_dynlist_ite_gotoFirst(&pvlist))
		do {
			pPVelem = (Tast_cln_pvalIV2*)st_dynlist_ite_getCurrent(&pvlist);
			/* */
			if (! st_sysStrcmpN2(ST_B_TRUE, 3, pPVelem->pID02, cID02empty) &&
					! st_sysStrcmpN2(ST_B_TRUE, 4,
								pPVelem->pID34, cID34empty))  /* v2 & v3/4 */
				pCmdln->cbMsg("   %3s       %4s     %3s       %c%c%c     %s",
						pPVelem->pID02, pPVelem->pID34, pPVelem->pFTPstr,
						LOC_VERFL_(pPVelem->verFl), pPVelem->pCap);
			else if (! st_sysStrcmpN2(ST_B_TRUE, 3,
						pPVelem->pID02, cID02empty))  /* v2 */
				pCmdln->cbMsg("   %3s       %4s     %3s       %c%c%c     %s",
						pPVelem->pID02, " ", pPVelem->pFTPstr,
						LOC_VERFL_(pPVelem->verFl), pPVelem->pCap);
			else if (! st_sysStrcmpN2(ST_B_TRUE, 4,
						pPVelem->pID34, cID34empty))  /* v3/4 */
				pCmdln->cbMsg("   %3s       %4s     %3s       %c%c%c     %s",
						" ", pPVelem->pID34, pPVelem->pFTPstr,
						LOC_VERFL_(pPVelem->verFl), pPVelem->pCap);
		} while (st_dynlist_ite_gotoNext(&pvlist));
	pCmdln->cbMsg("\n");

	if (fldOrDsc) {
		pCmdln->cbMsg(" Additionally to these fixed field names you may also");
		pCmdln->cbMsg(" use your own custom field names in these ranges:\n");
		pCmdln->cbMsg("   %3s       %4s     %3s       %c%c%c     %s",
				"T00", "T000", LOC_GETFTPSTR_(ST_ID3V2_FTP_TXT),
				LOC_VERFL_(1 | 2 | 4),
				"Custom text info frame");
		pCmdln->cbMsg("  (through)");
		pCmdln->cbMsg("   %3s       %4s     %3s       %c%c%c     %s",
				"TZZ", "TZZZ", LOC_GETFTPSTR_(ST_ID3V2_FTP_TXT),
				LOC_VERFL_(1 | 2 | 4),
				"Custom text info frame");
		pCmdln->cbMsg("   %3s       %4s     %3s       %c%c%c     %s",
				"W00", "W000", LOC_GETFTPSTR_(ST_ID3V2_FTP_URL),
				LOC_VERFL_(1 | 2 | 4),
				"Custom URL info frame");
		pCmdln->cbMsg("  (through)");
		pCmdln->cbMsg("   %3s       %4s     %3s       %c%c%c     %s",
				"WZZ", "WZZZ", LOC_GETFTPSTR_(ST_ID3V2_FTP_URL),
				LOC_VERFL_(1 | 2 | 4),
				"Custom URL info frame");
		pCmdln->cbMsg("\n");
	}

	pCmdln->cbMsg("Note that");
	if (fldOrDsc) {
		pCmdln->cbMsg("- most fields do only exist in ID3v2 tags and not in "
				"any other tag type.");
		pCmdln->cbMsg("  If you want to add/modify fields in other tag types "
				"you should either");
		pCmdln->cbMsg("  only use fields of <%s> %s and %s,",
				AST_CLN_SVT_FMT_FLDTYPE,
				LOC_GETFTPSTR_(ST_ID3V2_FTP_TXT),
				LOC_GETFTPSTR_(ST_ID3V2_FTP_URL));
		pCmdln->cbMsg("  or simply use another <%s> as input format in <%s>",
				AST_CLN_SVT_FMT_TAG, pFmtFldAddSet);
		if (! addOrSet) {
			pCmdln->cbMsg("- fields of <%s> %s, %s, %s, %s, %s and %s can only "
					"be resetted",
					AST_CLN_SVT_FMT_FLDTYPE,
					LOC_GETFTPSTR_(ST_ID3V2_FTP_CDM),
					LOC_GETFTPSTR_(ST_ID3V2_FTP_CPY),
					LOC_GETFTPSTR_(ST_ID3V2_FTP_MCI),
					LOC_GETFTPSTR_(ST_ID3V2_FTP_POP),
					LOC_GETFTPSTR_(ST_ID3V2_FTP_UFI),
					LOC_GETFTPSTR_(ST_ID3V2_FTP_SLT));
			pCmdln->cbMsg("  but not added or modified");
		}
		pCmdln->cbMsg("- some programs (e.g. one made by a big company with a "
				"fruit in their logo)");
		pCmdln->cbMsg("  sometimes use field names that end with a whitespace in");
		pCmdln->cbMsg("  ID3v2.3 or ID3v2.4 tags. Such field names are invalid");
		pCmdln->cbMsg("  and will be replaced by valid ones if possible");
		pCmdln->cbMsg("  or ignored if necessary");
	}
	pCmdln->cbMsg("- the version flag <%s> shows for which ID3v2 versions",
			AST_CLN_SVT_FMT_VERFL);
	pCmdln->cbMsg("  a certain field is valid:");
	pCmdln->cbMsg("  2 means for ID3v2.2,  3 means for ID3v2.3 and  "
			"4 means for ID3v2.4");
	if (fldOrDsc) {
		pCmdln->cbMsg("- if you for example add a field that is only valid "
				"for ID3v2.4");
		pCmdln->cbMsg("  to a ID3v2.3 tag then it will either get mapped to");
		pCmdln->cbMsg("  a comment or simply be ignored");
	}
	pCmdln->cbMsg("\n");

	st_dynlist_stc_freeDL(&pvlist);
#	undef LOC_VERFL_
#	undef LOC_GETFTPSTR_
}

/*----------------------------------------------------------------------------*/

/* for --fadd/set hvAV2/hvVOR */
static void
AST_CLN__hhlp_c_faddset_posvalsAV2_or_VOR(Tast_cln_a *pCmdln,
		const char *pOptCap, const Tst_bool addOrSet, const Tst_bool av2OrVor)
{
#	define LOC_GETFTPSTR_AV2_(mac_ftp)  \
				ast_cln_hhlpfnc_c_faddset_posvalsAV2_ftp(mac_ftp)
#	define LOC_GETFTPSTR_VOR_(mac_ftp)  \
				ast_cln_hhlpfnc_c_faddset_posvalsVOR_ftp(mac_ftp)
	Tst_uint32 fldnMaxlen = 0;
	char const *pTagArg   = (av2OrVor ? AST_CLN_ARG_TAV2SH : AST_CLN_ARG_TVORSH),
	           *pTagDesc  = (av2OrVor ? "APEv2" : "Vorbis");
	Tast_cln_pvalAV2_or_VOR *pPVelem = NULL;
	Tst_dynlist             pvlist;

	if (st_dynlist_stc_initDL(&pvlist, sizeof(Tast_cln_pvalAV2_or_VOR),
				ast_cln_hhlpfnc_cbRsetElemAV2_or_VOR,
				ast_cln_hhlpfnc_cbRsetElemAV2_or_VOR) != ST_ERR_SUCC)
		return;
	if (! ast_cln_hhlpfnc_c_faddset_posvalsAV2_or_VOR_genlist(&pvlist,
				av2OrVor, &fldnMaxlen))
		return;

	pCmdln->cbMsg("\n");
	pCmdln->cbMsg("Possible values for %s field names are:", pTagDesc);
	pCmdln->cbMsg("  -Str-%*s    -FldType-  -Desc-",
			(fldnMaxlen > 5 ? fldnMaxlen - 5 : 0), "");
	if (st_dynlist_ite_gotoFirst(&pvlist))
		do {
			pPVelem = (Tast_cln_pvalAV2_or_VOR*)st_dynlist_ite_getCurrent(&pvlist);
			/* */
			pCmdln->cbMsg("   %-*s    %3s        %s",
					fldnMaxlen, pPVelem->pID,
					pPVelem->pFTPstr, pPVelem->pCap);
		} while (st_dynlist_ite_gotoNext(&pvlist));
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg(" Additionally to these fixed field names you may also");
	pCmdln->cbMsg(" use your own custom field names");
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Note that");
	pCmdln->cbMsg("- a custom field name may contain almost any char "
			"and be of arbitrary length");
	pCmdln->cbMsg("- to use ':' or '.' in field names you need escape those chars");
	pCmdln->cbMsg("  with a leading backslash "
			"('\\', e.g. \"HOME\\:TOWN\\.69\",");
	pCmdln->cbMsg("  in Linux Bash Shell that might have to be "
			"\"HOME\\\\:TOWN\\\\.69\")");
	pCmdln->cbMsg("- field names are case-insensitive (i.e. 'A' == 'a')");
	pCmdln->cbMsg("\n");

	pCmdln->cbMsg("Examples:");
	if (av2OrVor) {
		if (addOrSet) {
			pCmdln->cbMsg("  --%s %s:LANGUAGE=Norwegian",
					pOptCap, pTagArg);
			pCmdln->cbMsg("  --%s %s:BIBLIOGRAPHY=www.hugeband.nor",
					pOptCap, pTagArg);
			pCmdln->cbMsg("  --%s \"%s:RECORD DATE=2011-04-30\"",
					pOptCap, pTagArg);
			pCmdln->cbMsg("    same as: --%s \"%s:record date=2011-04-30\"",
					pOptCap, pTagArg);
		} else {
			pCmdln->cbMsg("  --%s %s:YEAR=2011", pOptCap, pTagArg);
			pCmdln->cbMsg("    same as:--%s %s:year=2011", pOptCap, pTagArg);
			pCmdln->cbMsg("  --%s \"%s:SUBTITLE=Greatest Hits\"",
					pOptCap, pTagArg);
			pCmdln->cbMsg("  --%s %s:COMMENT.2=", pOptCap, pTagArg);
			pCmdln->cbMsg("  --%s \"%s:DEBUT ALBUM=Men on Mars\"",
					pOptCap, pTagArg);
		}
	} else {
		if (addOrSet) {
			pCmdln->cbMsg("  --%s %s:TEMPO=75",
					pOptCap, pTagArg);
			pCmdln->cbMsg("  --%s %s:GENRE=Rock",
					pOptCap, pTagArg);
			pCmdln->cbMsg("    same as: --%s %s:genre=Rock",
					pOptCap, pTagArg);
		} else {
			pCmdln->cbMsg("  --%s %s:DATE=2009-12-31", pOptCap, pTagArg);
			pCmdln->cbMsg("    same as:--%s %s:date=2009-12-31", pOptCap, pTagArg);
			pCmdln->cbMsg("  --%s %s:DESCRIPTION.2=", pOptCap, pTagArg);
		}
	}
	st_dynlist_stc_freeDL(&pvlist);
#	undef LOC_GETFTPSTR_AV2_
#	undef LOC_GETFTPSTR_VOR_
}

/*----------------------------------------------------------------------------*/

/* for <taglist> */
static void
AST_CLN__hhlp_t_taglist_posvals(Tast_cln_a *pCmdln,
		const char *pHeadline,
		const Tast_cln_t_tagList_pb(pAllwTags),
		const Tst_bool autoAllw, const Tst_bool nonAllw,
		const Tst_bool allAllw)
{
	Tast_cln_t_tagList_capDesc const *pCapDesc = AST_CLN_TAGLIST_CAPDESC;

	pCmdln->cbMsg("%s:", pHeadline);
	pCmdln->cbMsg("  -TagLong-  -TagShort-  -Desc-");
	while (pCapDesc->pCap != NULL) {
		if ((pCapDesc->tlix >= 0 && pAllwTags[pCapDesc->tlix]) ||
				(pCapDesc->tlix < 0 &&
					((st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_AUTO) &&
							autoAllw) ||
						(st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_NONE) &&
								nonAllw) ||
						(st_sysStrcmp2(ST_B_TRUE, pCapDesc->pCap, AST_CLN_ARG_ALL) &&
								allAllw)))) {
			pCmdln->cbMsg("   %-7s    %-5s       %s",
					pCapDesc->pCap,
					(pCapDesc->pCapSh ? pCapDesc->pCapSh : " "),
					pCapDesc->pDesc);
		}
		++pCapDesc;
	}
	pCmdln->cbMsg("\n");
}

/******************************************************************************/
