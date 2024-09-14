/******************************************************************************/
/* dynlist_stcs-prot.h          [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Protected functions for Dynamic List structures                            */
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

#ifndef HEAD_DYNLIST_STCS_PROT_H
#define HEAD_DYNLIST_STCS_PROT_H

/*
// Own-Includes
*/
/** */
#include "dynlist-prot.h"


ST_BEGIN_C_DECLS

/*
// Functions (protected)
*/
#if defined(SRC_DYNLIST_C) || defined(SRC_DYNLIST_STCS_C)
	/** */
	ST_INLINE_H void
	st_dynlist_stc_freeElem(Tst_dynlist_elem *pElem,
	                        Tst_dynlist_cbFreeElem cbFreeElement);
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_DYNLIST_STCS_PROT_H */

/******************************************************************************/
