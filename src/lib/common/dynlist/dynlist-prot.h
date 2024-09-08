/******************************************************************************/
/* dynlist-prot.h               [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for Dynamic Lists                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 09.08.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_DYNLIST_PROT_H
#define HEAD_DYNLIST_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include <santag/common/dynlist-defs.h>


ST_BEGIN_C_DECLS

/*
// Types (protected)
*/
#if defined(SRC_DYNLIST_C) || defined(SRC_DYNLIST_STCS_C)
	/** */
	struct st_dynlist_elem {
		void                   *pData;  /* actual data of this element */
		struct st_dynlist_elem *pNext;  /* next element */
		struct st_dynlist_elem *pPrev;  /* previous element */
	};
	typedef struct st_dynlist_elem Tst_dynlist_elem;
	/** */
	typedef struct {
		Tst_dynlist_elem *pE;
		Tst_uint32       ix;
	} Tst_dynlist_eix;

	/** */
	typedef struct {
		Tst_dynlist_elem *pStart;  /* first element */
		Tst_dynlist_eix  lastAdded;  /* last element that has been added */
		Tst_dynlist_elem *pEnd;    /* last element */
		Tst_dynlist_eix  iter;     /* iterator */
		Tst_uint32       sz;       /* amount of alloc'd elements */
		Tst_uint32       cnt;      /* amount of used elements */
		Tst_uint32       soElem;   /* size of one element */
		/* cache */
		Tst_dynlist_eix cacheElem;  /* cached element */
		/* callbacks */
		Tst_dynlist_cbResetElem cbRsetElem;  /* for resetting elems */
		Tst_dynlist_cbFreeElem  cbFreeElem;  /* for freeing elems */
	} Tst_dynlist_intn;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_DYNLIST_PROT_H */

/******************************************************************************/
