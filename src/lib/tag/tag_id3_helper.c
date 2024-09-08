/******************************************************************************/
/* tag_id3.c                    [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Common functions for ID3 Tags                                              */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 17.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/sys_fnc.h>
#include <santag/tag/tag_id3_helper.h>
/** */
#define SRC_TAG_ID3_HELPER_C
#include "tag_id3_helper-prot.h"
#undef SRC_TAG_ID3_HELPER_C

/*
// System-Includes
*/
#include <string.h>      /* memcpy(), ... */
#include <strings.h>     /* index() */
#include <stdlib.h>      /* calloc() */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* ID3 genres */
const Tst_id3_genre ST_ID3_GENRES[] = {
		/* ID3v1 names (0-79) */
		{  0, "Blues",                  ST_B_TRUE }, {  1, "Classic Rock",           ST_B_TRUE },
		{  2, "Country",                ST_B_TRUE }, {  3, "Dance",                  ST_B_TRUE },
		{  4, "Disco",                  ST_B_TRUE }, {  5, "Funk",                   ST_B_TRUE },
		{  6, "Grunge",                 ST_B_TRUE }, {  7, "Hip-Hop",                ST_B_TRUE },
		{  8, "Jazz",                   ST_B_TRUE }, {  9, "Metal",                  ST_B_TRUE },
		{ 10, "New Age",                ST_B_TRUE }, { 11, "Oldies",                 ST_B_TRUE },
		{ 12, "Other",                  ST_B_TRUE }, { 13, "Pop",                    ST_B_TRUE },
		{ 14, "R&B",                    ST_B_TRUE }, { 15, "Rap",                    ST_B_TRUE },
		{ 16, "Reggae",                 ST_B_TRUE }, { 17, "Rock",                   ST_B_TRUE },
		{ 18, "Techno",                 ST_B_TRUE }, { 19, "Industrial",             ST_B_TRUE },
		{ 20, "Alternative",            ST_B_TRUE }, { 21, "Ska",                    ST_B_TRUE },
		{ 22, "Death Metal",            ST_B_TRUE }, { 23, "Pranks",                 ST_B_TRUE },
		{ 24, "Soundtrack",             ST_B_TRUE }, { 25, "Euro-Techno",            ST_B_TRUE },
		{ 26, "Ambient",                ST_B_TRUE }, { 27, "Trip-Hop",               ST_B_TRUE },
		{ 28, "Vocal",                  ST_B_TRUE }, { 29, "Jazz+Funk",              ST_B_TRUE },
		{ 30, "Fusion",                 ST_B_TRUE }, { 31, "Trance",                 ST_B_TRUE },
		{ 32, "Classical",              ST_B_TRUE }, { 33, "Instrumental",           ST_B_TRUE },
		{ 34, "Acid",                   ST_B_TRUE }, { 35, "House",                  ST_B_TRUE },
		{ 36, "Game",                   ST_B_TRUE }, { 37, "Sound Clip",             ST_B_TRUE },
		{ 38, "Gospel",                 ST_B_TRUE }, { 39, "Noise",                  ST_B_TRUE },
		{ 40, "Alternative Rock",       ST_B_TRUE }, { 41, "Bass",                   ST_B_TRUE },
		{ 42, "Soul",                   ST_B_TRUE }, { 43, "Punk",                   ST_B_TRUE },
		{ 44, "Space",                  ST_B_TRUE }, { 45, "Meditative",             ST_B_TRUE },
		{ 46, "Instrumental Pop",       ST_B_TRUE }, { 47, "Instrumental Rock",      ST_B_TRUE },
		{ 48, "Ethnic",                 ST_B_TRUE }, { 49, "Gothic",                 ST_B_TRUE },
		{ 50, "Darkwave",               ST_B_TRUE }, { 51, "Techno-Industrial",      ST_B_TRUE },
		{ 52, "Electronic",             ST_B_TRUE }, { 53, "Pop-Folk",               ST_B_TRUE },
		{ 54, "Eurodance",              ST_B_TRUE }, { 55, "Dream",                  ST_B_TRUE },
		{ 56, "Southern Rock",          ST_B_TRUE }, { 57, "Comedy",                 ST_B_TRUE },
		{ 58, "Cult",                   ST_B_TRUE }, { 59, "Gangsta",                ST_B_TRUE },
		{ 60, "Top 40",                 ST_B_TRUE }, { 61, "Christian Rap",          ST_B_TRUE },
		{ 62, "Pop/Funk",               ST_B_TRUE }, { 63, "Jungle",                 ST_B_TRUE },
		{ 64, "Native American",        ST_B_TRUE }, { 65, "Cabaret",                ST_B_TRUE },
		{ 66, "New Wave",               ST_B_TRUE }, { 67, "Psychedelic",            ST_B_TRUE },
		{ 68, "Rave",                   ST_B_TRUE }, { 69, "Showtunes",              ST_B_TRUE },
		{ 70, "Trailer",                ST_B_TRUE }, { 71, "Lo-Fi",                  ST_B_TRUE },
		{ 72, "Tribal",                 ST_B_TRUE }, { 73, "Acid Punk",              ST_B_TRUE },
		{ 74, "Acid Jazz",              ST_B_TRUE }, { 75, "Polka",                  ST_B_TRUE },
		{ 76, "Retro",                  ST_B_TRUE }, { 77, "Musical",                ST_B_TRUE },
		{ 78, "Rock & Roll",            ST_B_TRUE }, { 79, "Hard Rock",              ST_B_TRUE },
		/* Winamp extensions (80-147) */
		{ 80, "Folk",                   ST_B_FALSE}, { 81, "Folk/Rock",              ST_B_FALSE},
		{ 82, "National Folk",          ST_B_FALSE}, { 83, "Swing",                  ST_B_FALSE},
		{ 84, "Fast-Fusion",            ST_B_FALSE}, { 85, "Bebob",                  ST_B_FALSE},
		{ 86, "Latin",                  ST_B_FALSE}, { 87, "Revival",                ST_B_FALSE},
		{ 88, "Celtic",                 ST_B_FALSE}, { 89, "Bluegrass",              ST_B_FALSE},
		{ 90, "Avantgarde",             ST_B_FALSE}, { 91, "Gothic Rock",            ST_B_FALSE},
		{ 92, "Progressive Rock",       ST_B_FALSE}, { 93, "Psychedelic Rock",       ST_B_FALSE},
		{ 94, "Symphonic Rock",         ST_B_FALSE}, { 95, "Slow Rock",              ST_B_FALSE},
		{ 96, "Big Band",               ST_B_FALSE}, { 97, "Chorus",                 ST_B_FALSE},
		{ 98, "Easy Listening",         ST_B_FALSE}, { 99, "Acoustic",               ST_B_FALSE},
		{100, "Humour",                 ST_B_FALSE}, {101, "Speech",                 ST_B_FALSE},
		{102, "Chanson",                ST_B_FALSE}, {103, "Opera",                  ST_B_FALSE},
		{104, "Chamber Music",          ST_B_FALSE}, {105, "Sonata",                 ST_B_FALSE},
		{106, "Symphony",               ST_B_FALSE}, {107, "Booty Bass",             ST_B_FALSE},
		{108, "Primus",                 ST_B_FALSE}, {109, "Porn Groove",            ST_B_FALSE},
		{110, "Satire",                 ST_B_FALSE}, {111, "Slow Jam",               ST_B_FALSE},
		{112, "Club",                   ST_B_FALSE}, {113, "Tango",                  ST_B_FALSE},
		{114, "Samba",                  ST_B_FALSE}, {115, "Folklore",               ST_B_FALSE},
		{116, "Ballad",                 ST_B_FALSE}, {117, "Power Ballad",           ST_B_FALSE},
		{118, "Rhythmic Soul",          ST_B_FALSE}, {119, "Freestyle",              ST_B_FALSE},
		{120, "Duet",                   ST_B_FALSE}, {121, "Punk Rock",              ST_B_FALSE},
		{122, "Drum Solo",              ST_B_FALSE}, {123, "A Cappella",             ST_B_FALSE},
		{124, "Euro-House",             ST_B_FALSE}, {125, "Dance Hall",             ST_B_FALSE},
		{126, "Goa",                    ST_B_FALSE}, {127, "Drum & Bass",            ST_B_FALSE},
		{128, "Club-House",             ST_B_FALSE}, {129, "Hardcore",               ST_B_FALSE},
		{130, "Terror",                 ST_B_FALSE}, {131, "Indie",                  ST_B_FALSE},
		{132, "BritPop",                ST_B_FALSE}, {133, "Negerpunk",              ST_B_FALSE},
		{134, "Polsk Punk",             ST_B_FALSE}, {135, "Beat",                   ST_B_FALSE},
		{136, "Christian Gangsta Rap",  ST_B_FALSE}, {137, "Heavy Metal",            ST_B_FALSE},
		{138, "Black Metal",            ST_B_FALSE}, {139, "Crossover",              ST_B_FALSE},
		{140, "Contemporary Christian", ST_B_FALSE}, {141, "Christian Rock",         ST_B_FALSE},
		{142, "Merengue",               ST_B_FALSE}, {143, "Salsa",                  ST_B_FALSE},
		{144, "Thrash Metal",           ST_B_FALSE}, {145, "Anime",                  ST_B_FALSE},
		{146, "JPop",                   ST_B_FALSE}, {147, "Synthpop",               ST_B_FALSE},
		/* */
		{  0, NULL, ST_B_FALSE} };
const Tst_byte ST_ID3_GENRE_DEFMAXNR = 147;  /* "Synthpop" */

/* ID3v2-only genres */
const Tst_id3_genre_spec ST_ID3_GENRES_SPEC[] = {
		{ST_ID3_GEN_SPEC_COV, "CR", "Cover"},
		{ST_ID3_GEN_SPEC_RMX, "RX", "Remix"},
		{ST_ID3_GEN_SPEC_NONE, NULL, NULL} };

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Does Genre exist ? If so return its index in the genre-array
 *
 * pIx may be NULL
 *
 * returns: true if found
 *          pIx will be set to the array-index of the genre-entry
 */
Tst_bool
st_id3_genreExists_int(const Tst_byte gennr, ST_OPTARG(Tst_int32 *pIx))
{
	Tst_int32 iix   = 0,
	          *pIIX = pIx;
	Tst_id3_genre *pGens = (Tst_id3_genre*)&ST_ID3_GENRES;

	if (pIx != NULL)
		*pIx = 0;
	else
		pIIX = &iix;
	while (pGens->pCap != NULL) {
		if (pGens->nr == gennr)
			return ST_B_TRUE;
		pGens = (Tst_id3_genre*)&ST_ID3_GENRES[++(*pIIX)];
	}
	if (pIx != NULL)
		*pIx = -1;
	return ST_B_FALSE;
}

/*
 * Does Genre exist ? If so return its index in the genre-array
 *   pCap will be compared case-insensitive
 *
 * pCap  MUST be set
 * pIx   may be NULL
 *
 * returns: true if found
 *          pIx will be set to the array-index of the genre-entry
 */
Tst_bool
st_id3_genreExists_str(const Tst_str *pCap, ST_OPTARG(Tst_int32 *pIx))
{
	Tst_int32 iix   = 0,
	          *pIIX = pIx;
	Tst_id3_genre const *pGens = (Tst_id3_genre*)&ST_ID3_GENRES;

	if (pCap == NULL)
		return ST_B_FALSE;

	if (pIx != NULL)
		*pIx = 0;
	else
		pIIX = &iix;
	while (pGens->pCap != NULL) {
		if (st_sysStrcmp(ST_B_FALSE, (Tst_str*)pGens->pCap, pCap))
			return ST_B_TRUE;
		pGens = (Tst_id3_genre*)&ST_ID3_GENRES[++(*pIIX)];
	}
	if (pIx != NULL)
		*pIx = -1;
	return ST_B_FALSE;
}

/*
 * Return caption for genre
 */
const char*
st_id3_getGenreCap(const Tst_int32 ix)
{
	if (ix < 0 || ix > ST_ID3_GENRE_DEFMAXNR)
		return NULL;

	return ST_ID3_GENRES[ix].pCap;
}

/*
 * Is standard genre ?
 */
Tst_bool
st_id3_isStandardGenre(const Tst_int32 ix)
{
	if (ix < 0 || ix > ST_ID3_GENRE_DEFMAXNR)
		return ST_B_FALSE;

	return ST_ID3_GENRES[ix].isStd;
}

/*
 * Is special Genre ?
 *   pIDandorCap will be compared case-insensitive
 *
 * pIDandorCap  MUST be set
 * pIx          may be NULL
 *
 * returns: true if is spec. genre
 */
Tst_bool
st_id3_isSpecGenre(const Tst_str *pIDandorCap, ST_OPTARG(Tst_int32 *pIx))
{
	Tst_int32  iix    = 0,
	           *pIIX  = pIx;
	Tst_str    *pSub1 = NULL,
	           *pSub2 = NULL,
	           *pSpa;
	Tst_uint32 idlen,
	           sub1len,
	           sub2len;
	Tst_id3_genre_spec const *pGens = (Tst_id3_genre_spec*)&ST_ID3_GENRES_SPEC;

	if (pIDandorCap == NULL)
		return ST_B_FALSE;

	if (pIx != NULL)
		*pIx = 0;
	else
		pIIX = &iix;
	if ((pSpa = (Tst_str*)index((char*)pIDandorCap, ST_CSET_ISO_SPA)) != NULL) {
		idlen   = st_sysStrlen2(pIDandorCap);
		sub1len = (Tst_uint32)(pSpa - pIDandorCap);
		sub2len = idlen - sub1len - 1;
		ST_CALLOC(pSub1, Tst_str*, sub1len + 1, 1)
		if (pSub1 == NULL)
			return ST_B_FALSE;
		ST_CALLOC(pSub2, Tst_str*, sub2len + 1, 1)
		if (pSub2 == NULL) {
			ST_DELPOINT(pSub1)
			return ST_B_FALSE;
		}
		++pSpa;
		memcpy(pSub1, pIDandorCap, sub1len);
		pSub1[sub1len] = 0;
		memcpy(pSub2, pSpa, sub2len);
		pSub2[sub2len] = 0;
	}
	while (pGens->specID != ST_ID3_GEN_SPEC_NONE) {
		if (pSub1 != NULL && pSub2 != NULL) {
			if (st_sysStrcmp(ST_B_FALSE, (Tst_str*)pGens->pCap, pSub2) ||
				st_sysStrcmp(ST_B_FALSE, (Tst_str*)pGens->pIDstr, pSub1)) {
				ST_DELPOINT(pSub1)
				ST_DELPOINT(pSub2)
				return ST_B_TRUE;
			}
		} else if (st_sysStrcmp(ST_B_FALSE, (Tst_str*)pGens->pCap, pIDandorCap) ||
				st_sysStrcmp(ST_B_FALSE, (Tst_str*)pGens->pIDstr, pIDandorCap)) {
			ST_DELPOINT(pSub1)
			ST_DELPOINT(pSub2)
			return ST_B_TRUE;
		}
		pGens = (Tst_id3_genre_spec*)&ST_ID3_GENRES_SPEC[++(*pIIX)];
	}
	if (pIx != NULL)
		*pIx = -1;
	ST_DELPOINT(pSub1)
	ST_DELPOINT(pSub2)
	return ST_B_FALSE;
}

/******************************************************************************/
