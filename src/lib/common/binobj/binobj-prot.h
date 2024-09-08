/******************************************************************************/
/* binobj-prot.h                [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org)                 */
/*                                                                            */
/* Protected definitions for Binary Object                                    */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 23.11.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef HEAD_BINOBJ_PROT_H
#define HEAD_BINOBJ_PROT_H

/*
// Own-Includes
*/
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/** */
#include "src/includes/common/dynlist-defs.h"
#include "src/includes/common/sys_file-defs.h"


ST_BEGIN_C_DECLS

/*
// Types (protected)
*/
#if defined(SRC_BINOBJ_C) || defined(SRC_BINOBJ_STCS_C)
	/** */
	typedef struct {
		Tst_uint32   threshold;
		Tst_uint32   reallcSz;
		/* */
		Tst_buf      *pDataBuf;
		Tst_uint32   dataBufSz;
		Tst_str      *pDataTmpFn;
		Tst_uint32   dataSz;
		Tst_sys_fstc *pDataFStc;
		Tst_bool     isBufOrFile;
		Tst_streamrd *pSObjR;
	} Tst_binobj_intn;
#endif  /* protected */


ST_END_C_DECLS

#endif  /* HEAD_BINOBJ_PROT_H */

/******************************************************************************/
