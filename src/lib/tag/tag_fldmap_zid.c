/******************************************************************************/
/* tag_fldmap_zid.c             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Functions for mapping Tag Field IDs                                        */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 13.12.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
/** */
#define SRC_TAG_FLDMAP_ZID_C
#include "tag_fldmap_zid-prot.h"
#include "tag_fldmap_zid-priv.h"
#undef SRC_TAG_FLDMAP_ZID_C

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** A<-->B */
static void
ST_TFMAP__mapID_a_b(const int srcA, const int srcB,
                    int *pDstB, int *pDstA,
                    const Tst_tfldMap__mapID_a_b *pFldMap,
                    const int noneA, const int noneB);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * --> ID3v1
 */
Tst_id3v1_frID
st_tfldMap_mapID_IV2_to_IV1(const Tst_id3v2_frID srcIV2)
{
	int dstB = (int)ST_ID3V1_FID_NONE;

	ST_TFMAP__mapID_a_b((const int)srcIV2, /*srcB:*/dstB,
			&dstB, /*dstA:*/NULL,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_IV2_IV1,
			(const int)ST_ID3V2_FID_NONE, /*noneB:*/dstB);
	return (Tst_id3v1_frID)dstB;
}

Tst_id3v1_frID
st_tfldMap_mapID_AV2_to_IV1(const Tst_apev2_frID srcAV2)
{
	int dstB = (int)ST_ID3V1_FID_NONE;

	ST_TFMAP__mapID_a_b((const int)srcAV2, /*srcB:*/dstB,
			&dstB, /*dstA:*/NULL,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_AV2_IV1,
			(const int)ST_APEV2_FID_NONE, /*noneB:*/dstB);
	return (Tst_id3v1_frID)dstB;
}

Tst_id3v1_frID
st_tfldMap_mapID_VOR_to_IV1(const Tst_vorbc_frID srcVOR)
{
	int dstB = (int)ST_ID3V1_FID_NONE;

	ST_TFMAP__mapID_a_b((const int)srcVOR, /*srcB:*/dstB,
			&dstB, /*dstA:*/NULL,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_VOR_IV1,
			(const int)ST_VORBC_FID_NONE, /*noneB:*/dstB);
	return (Tst_id3v1_frID)dstB;
}

/*
 * --> ID3v2
 */
Tst_id3v2_frID
st_tfldMap_mapID_IV1_to_IV2(const Tst_id3v1_frID srcIV1)
{
	int dstA = (int)ST_ID3V2_FID_NONE;

	ST_TFMAP__mapID_a_b(/*srcA:*/dstA, (const int)srcIV1,
			/*dstB:*/NULL, &dstA,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_IV2_IV1,
			/*noneA:*/dstA, (const int)ST_ID3V1_FID_NONE);
	return (Tst_id3v2_frID)dstA;
}

Tst_id3v2_frID
st_tfldMap_mapID_AV2_to_IV2(const Tst_apev2_frID srcAV2)
{
	int dstB = (int)ST_ID3V2_FID_NONE;

	ST_TFMAP__mapID_a_b((const int)srcAV2, /*srcB:*/dstB,
			&dstB, /*dstA:*/NULL,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_AV2_IV2,
			(const int)ST_APEV2_FID_NONE, /*noneB:*/dstB);
	return (Tst_id3v2_frID)dstB;
}

Tst_id3v2_frID
st_tfldMap_mapID_VOR_to_IV2(const Tst_vorbc_frID srcVOR)
{
	int dstB = (int)ST_ID3V2_FID_NONE;

	ST_TFMAP__mapID_a_b((const int)srcVOR, /*srcB:*/dstB,
			&dstB, /*dstA:*/NULL,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_VOR_IV2,
			(const int)ST_VORBC_FID_NONE, /*noneB:*/dstB);
	return (Tst_id3v2_frID)dstB;
}

/*
 * --> APEv2
 */
Tst_apev2_frID
st_tfldMap_mapID_IV1_to_AV2(const Tst_id3v1_frID srcIV1)
{
	int dstA = (int)ST_APEV2_FID_NONE;

	ST_TFMAP__mapID_a_b(/*srcA:*/dstA, (const int)srcIV1,
			/*dstB:*/NULL, &dstA,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_AV2_IV1,
			/*noneA:*/dstA, (const int)ST_ID3V1_FID_NONE);
	return (Tst_apev2_frID)dstA;
}

Tst_apev2_frID
st_tfldMap_mapID_IV2_to_AV2(const Tst_id3v2_frID srcIV2)
{
	int dstA = (int)ST_APEV2_FID_NONE;

	ST_TFMAP__mapID_a_b(/*srcA:*/dstA, (const int)srcIV2,
			/*dstB:*/NULL, &dstA,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_AV2_IV2,
			/*noneA:*/dstA, (const int)ST_ID3V2_FID_NONE);
	return (Tst_apev2_frID)dstA;
}

Tst_apev2_frID
st_tfldMap_mapID_VOR_to_AV2(const Tst_vorbc_frID srcVOR)
{
	int dstA = (int)ST_APEV2_FID_NONE;

	ST_TFMAP__mapID_a_b(/*srcA:*/dstA, (const int)srcVOR,
			/*dstB:*/NULL, &dstA,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_AV2_VOR,
			/*noneA:*/dstA, (const int)ST_VORBC_FID_NONE);
	return (Tst_apev2_frID)dstA;
}

/*
 * --> Vorbis
 */
Tst_vorbc_frID
st_tfldMap_mapID_IV1_to_VOR(const Tst_id3v1_frID srcIV1)
{
	int dstA = (int)ST_VORBC_FID_NONE;

	ST_TFMAP__mapID_a_b(/*srcA:*/dstA, (const int)srcIV1,
			/*dstB:*/NULL, &dstA,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_VOR_IV1,
			/*noneA:*/dstA, (const int)ST_ID3V1_FID_NONE);
	return (Tst_vorbc_frID)dstA;
}

Tst_vorbc_frID
st_tfldMap_mapID_IV2_to_VOR(const Tst_id3v2_frID srcIV2)
{
	int dstA = (int)ST_VORBC_FID_NONE;

	ST_TFMAP__mapID_a_b(/*srcA:*/dstA, (const int)srcIV2,
			/*dstB:*/NULL, &dstA,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_VOR_IV2,
			/*noneA:*/dstA, (const int)ST_ID3V2_FID_NONE);
	return (Tst_vorbc_frID)dstA;
}

Tst_vorbc_frID
st_tfldMap_mapID_AV2_to_VOR(const Tst_apev2_frID srcAV2)
{
	int dstB = (int)ST_VORBC_FID_NONE;

	ST_TFMAP__mapID_a_b((const int)srcAV2, /*srcB:*/dstB,
			&dstB, /*dstA:*/NULL,
			(const Tst_tfldMap__mapID_a_b*)ST_TFLDMAP_MAPID_AV2_VOR,
			(const int)ST_APEV2_FID_NONE, /*noneB:*/dstB);
	return (Tst_vorbc_frID)dstB;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Get mapped field ID
 */
/*** A<-->B */
static void
ST_TFMAP__mapID_a_b(const int srcA, const int srcB,
		int *pDstB, int *pDstA,
		const Tst_tfldMap__mapID_a_b *pFldMap,
		const int noneA, const int noneB)
{
	Tst_uint32 g = 0;

	ST_ASSERTN_VOID(pFldMap == NULL || (pDstA == NULL && pDstB == NULL))

	if (pDstA != NULL)
		*pDstA = noneA;
	if (pDstB != NULL)
		*pDstB = noneB;
	while (pFldMap[g].idA != noneA && pFldMap[g].idB != noneB) {
		if (pDstB != NULL &&
				pFldMap[g].valAtoB &&
				pFldMap[g].idA == srcA) {  /* A-->B */
			*pDstB = pFldMap[g].idB;
			break;
		} else if (pDstA != NULL &&
				pFldMap[g].valBtoA &&
				pFldMap[g].idB == srcB) {  /* B-->A */
			*pDstA = pFldMap[g].idA;
			break;
		}
		++g;
	}
}

/******************************************************************************/
