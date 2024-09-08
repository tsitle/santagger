/******************************************************************************/
/* tag_fldmap_zid-priv.h        [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Private definitions for mapping Tag Field IDs                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 12.02.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_TAG_FLDMAP_ZID_PRIV_H
#define HEAD_TAG_FLDMAP_ZID_PRIV_H


ST_BEGIN_C_DECLS

/*
// Types (private)
*/
#ifdef SRC_TAG_FLDMAP_ZID_C
	/** A <--> B */
	typedef struct {
		int      idA;
		int      idB;
		Tst_bool valAtoB;  /* is this mapping valid for A-->B ? */
		Tst_bool valBtoA;  /* is this mapping valid for B-->A ? */
	} Tst_tfldMap__mapID_a_b;

	/** ID3v2 <--> ID3v1 */
#	define Tst_tfldMap__mapID_iv2_iv1  Tst_tfldMap__mapID_a_b

	/** APEv2 <--> ID3v1 */
#	define Tst_tfldMap__mapID_av2_iv1  Tst_tfldMap__mapID_a_b

	/** APEv2 <--> ID3v2 */
#	define Tst_tfldMap__mapID_av2_iv2  Tst_tfldMap__mapID_a_b

	/** APEv2 <--> Vorbis */
#	define Tst_tfldMap__mapID_av2_vor  Tst_tfldMap__mapID_a_b

	/** Vorbis <--> ID3v1 */
#	define Tst_tfldMap__mapID_vor_iv1  Tst_tfldMap__mapID_a_b

	/** Vorbis <--> ID3v2 */
#	define Tst_tfldMap__mapID_vor_iv2  Tst_tfldMap__mapID_a_b

#endif  /* private */


/*
// Constants (private)
*/
#ifdef SRC_TAG_FLDMAP_ZID_C
	/** ID3v2 <--> ID3v1 */
	const Tst_tfldMap__mapID_iv2_iv1 ST_TFLDMAP_MAPID_IV2_IV1[] = {
			{(int)ST_ID3V2_FID_234_TIT2, (int)ST_ID3V1_FID_TITL,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_ID3V2_FID_234_TPE1, (int)ST_ID3V1_FID_ARTI,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_ID3V2_FID_234_TALB, (int)ST_ID3V1_FID_ALBU,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_ID3V2_FID_234_TRCK, (int)ST_ID3V1_FID_TRKN,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_ID3V2_FID_234_TCON, (int)ST_ID3V1_FID_GENR,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_ID3V2_FID_234_COMM, (int)ST_ID3V1_FID_COMM,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_ID3V2_FID___4_TDRC, (int)ST_ID3V1_FID_YEAR,  /* IV2-->IV1 */
				ST_B_TRUE, ST_B_FALSE},
			{(int)ST_ID3V2_FID___4_TDRL, (int)ST_ID3V1_FID_YEAR,  /* IV2-->IV1 */
				ST_B_TRUE, ST_B_FALSE},
			{(int)ST_ID3V2_FID_23__TYER, (int)ST_ID3V1_FID_YEAR,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_ID3V2_FID_NONE, (int)ST_ID3V1_FID_NONE,
				ST_B_FALSE, ST_B_FALSE} };

	/** APEv2 <--> ID3v1 */
	const Tst_tfldMap__mapID_av2_iv1 ST_TFLDMAP_MAPID_AV2_IV1[] = {
			{(int)ST_APEV2_FID_TITL, (int)ST_ID3V1_FID_TITL,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ARTI, (int)ST_ID3V1_FID_ARTI,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ALBU, (int)ST_ID3V1_FID_ALBU,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_TRKN, (int)ST_ID3V1_FID_TRKN,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_GENR, (int)ST_ID3V1_FID_GENR,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_COMM, (int)ST_ID3V1_FID_COMM,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_YEAR, (int)ST_ID3V1_FID_YEAR,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_RECD, (int)ST_ID3V1_FID_YEAR,  /* AV2-->IV1 */
				ST_B_TRUE, ST_B_FALSE},
			{(int)ST_APEV2_FID_NONE, (int)ST_ID3V1_FID_NONE,
				ST_B_FALSE, ST_B_FALSE} };

	/** APEv2 <--> ID3v2 */
	const Tst_tfldMap__mapID_av2_iv2 ST_TFLDMAP_MAPID_AV2_IV2[] = {
			{(int)ST_APEV2_FID_TITL, (int)ST_ID3V2_FID_234_TIT2,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_SUBT, (int)ST_ID3V2_FID_234_TIT3,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ARTI, (int)ST_ID3V2_FID_234_TPE1,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ALBU, (int)ST_ID3V2_FID_234_TALB,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_PUBL, (int)ST_ID3V2_FID_234_TPUB,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_COND, (int)ST_ID3V2_FID_234_TPE3,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_TRKN, (int)ST_ID3V2_FID_234_TRCK,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_TRKT, (int)ST_ID3V2_FID_234_TRCK,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_DSCN, (int)ST_ID3V2_FID_234_TPOS,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_DSCT, (int)ST_ID3V2_FID_234_TPOS,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_COMP, (int)ST_ID3V2_FID_234_TCOM,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_COMM, (int)ST_ID3V2_FID_234_COMM,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_COPY, (int)ST_ID3V2_FID_234_TCOP,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_YEAR, (int)ST_ID3V2_FID___4_TDRL,
				ST_B_FALSE, ST_B_TRUE},
			{(int)ST_APEV2_FID_RECD, (int)ST_ID3V2_FID___4_TDRC,
				ST_B_FALSE, ST_B_TRUE},
			{(int)ST_APEV2_FID_YEAR, (int)ST_ID3V2_FID_23__TYER,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_GENR, (int)ST_ID3V2_FID_234_TCON,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ISRC, (int)ST_ID3V2_FID_234_TSRC,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_LANG, (int)ST_ID3V2_FID_234_TLAN,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_IPLS, (int)ST_ID3V2_FID_23__IPLS,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_CBIN, (int)ST_ID3V2_FID_234_GEOB,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_NONE, (int)ST_ID3V2_FID_NONE,
				ST_B_FALSE, ST_B_FALSE} };

	/** APEv2 <--> Vorbis */
	const Tst_tfldMap__mapID_av2_vor ST_TFLDMAP_MAPID_AV2_VOR[] = {
			{(int)ST_APEV2_FID_TITL, (int)ST_VORBC_FID_TITL,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ARTI, (int)ST_VORBC_FID_ARTI,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ALBU, (int)ST_VORBC_FID_ALBU,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_TRKN, (int)ST_VORBC_FID_TRKN,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_TRKT, (int)ST_VORBC_FID_TRKT,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_DSCN, (int)ST_VORBC_FID_DSCN,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_DSCT, (int)ST_VORBC_FID_DSCT,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_COMM, (int)ST_VORBC_FID_DESC,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_COPY, (int)ST_VORBC_FID_COPY,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_YEAR, (int)ST_VORBC_FID_RECD,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_RECD, (int)ST_VORBC_FID_RECD,  /* AV2-->VOR */
				ST_B_TRUE, ST_B_FALSE},
			{(int)ST_APEV2_FID_RECL, (int)ST_VORBC_FID_RECL,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_GENR, (int)ST_VORBC_FID_GENR,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_ISRC, (int)ST_VORBC_FID_ISRC,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_IPLS, (int)ST_VORBC_FID_IPLS,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_APEV2_FID_NONE, (int)ST_VORBC_FID_NONE,
				ST_B_FALSE, ST_B_FALSE} };

	/** Vorbis <--> ID3v1 */
	const Tst_tfldMap__mapID_vor_iv1 ST_TFLDMAP_MAPID_VOR_IV1[] = {
			{(int)ST_VORBC_FID_TITL, (int)ST_ID3V1_FID_TITL,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_ARTI, (int)ST_ID3V1_FID_ARTI,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_ALBU, (int)ST_ID3V1_FID_ALBU,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_TRKN, (int)ST_ID3V1_FID_TRKN,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_GENR, (int)ST_ID3V1_FID_GENR,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_DESC, (int)ST_ID3V1_FID_COMM,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_RECD, (int)ST_ID3V1_FID_YEAR,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_NONE, (int)ST_ID3V1_FID_NONE,
				ST_B_FALSE, ST_B_FALSE} };

	/** Vorbis <--> ID3v2 */
	const Tst_tfldMap__mapID_vor_iv2 ST_TFLDMAP_MAPID_VOR_IV2[] = {
			{(int)ST_VORBC_FID_TITL, (int)ST_ID3V2_FID_234_TIT2,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_ARTI, (int)ST_ID3V2_FID_234_TPE1,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_ALBU, (int)ST_ID3V2_FID_234_TALB,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_PERF, (int)ST_ID3V2_FID_234_TPE3,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_TRKN, (int)ST_ID3V2_FID_234_TRCK,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_TRKT, (int)ST_ID3V2_FID_234_TRCK,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_DSCN, (int)ST_ID3V2_FID_234_TPOS,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_DSCT, (int)ST_ID3V2_FID_234_TPOS,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_COPY, (int)ST_ID3V2_FID_234_TCOP,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_GENR, (int)ST_ID3V2_FID_234_TCON,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_DESC, (int)ST_ID3V2_FID_234_COMM,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_RECD, (int)ST_ID3V2_FID___4_TDRC,  /* IV2-->VOR */
				ST_B_FALSE, ST_B_TRUE},
			{(int)ST_VORBC_FID_RECD, (int)ST_ID3V2_FID___4_TDRL,  /* IV2-->VOR */
				ST_B_FALSE, ST_B_TRUE},
			{(int)ST_VORBC_FID_RECD, (int)ST_ID3V2_FID_23__TYER,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_ISRC, (int)ST_ID3V2_FID_234_TSRC,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_ORGA, (int)ST_ID3V2_FID_234_TPUB,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_LICS, (int)ST_ID3V2_FID_234_TCOP,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_BPMI, (int)ST_ID3V2_FID_234_TBPM,
				ST_B_TRUE, ST_B_FALSE},
			{(int)ST_VORBC_FID_TMPO, (int)ST_ID3V2_FID_234_TBPM,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_LYRI, (int)ST_ID3V2_FID_234_USLT,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_LYRI, (int)ST_ID3V2_FID_234_SYLT,  /* IV2-->VOR */
				ST_B_FALSE, ST_B_TRUE},
			{(int)ST_VORBC_FID_MBPI, (int)ST_ID3V2_FID_234_APIC,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_IPLS, (int)ST_ID3V2_FID_23__IPLS,
				ST_B_TRUE, ST_B_TRUE},
			{(int)ST_VORBC_FID_NONE, (int)ST_ID3V2_FID_NONE,
				ST_B_FALSE, ST_B_FALSE} };
#endif  /* private */


ST_END_C_DECLS

#endif  /* HEAD_TAG_FLDMAP_ZID_PRIV_H */

/******************************************************************************/
