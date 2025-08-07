/******************************************************************************/
/* sys_file.c                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* System functions related to files                                          */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 20.05.2005 (dd.mm.yyyy)                                     */
/******************************************************************************/

/*
// Own-Includes
*/
#if (CONFIG_ST_ALL_DEBUG_ADD == 1)
	#define ST_SYSFILE_DEB_  0  /* enable additional debugging stuff ? */
#endif
/** */
#include "src/includes/common/sys_file.h"
#include "src/includes/common/sys_fnc.h"

/*
// System-Includes
*/
#include <stdlib.h>      /* calloc() */
#include <sys/stat.h>    /* stat(), mkdir(), ... */
//#include <sys/types.h>   /* mode_t, ... */
#include <fcntl.h>       /* open(), O_WRONLY, ... */
#include <unistd.h>      /* unlink(),readlink(),rename() */
#include <string.h>      /* memcpy() */
#include <errno.h>       /* errno */
#include <stdio.h>       /* fseek(), fseeko(), ftell(), ftello(), ... */
#if (ST_SYSFILE_DEB_ == 1)
	#include <stdarg.h>      /* va_list, ... */
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* file descriptor etc. */
typedef struct {
	Tst_str  *pFilen;  /* filename */
	FILE     *pFP;     /* pointer to file stream */
	int      fd;       /* file descriptor */
	Tst_bool modeWr;   /* is file writable ? */
	Tst_bool isVirt;   /* is virtual (empty non-existing file) ? */
} Tst_sys__fstc_intn;

/*----------------------------------------------------------------------------*/

static Tst_bool
ST_SYSFILE__concatDirAndFilen(const Tst_str *pDirn, const Tst_str *pFilen,
                              Tst_str *pFullPath, Tst_uint32 fpSz);
static Tst_bool
ST_SYSFILE__getTmpFilename(Tst_bool inCurDir, Tst_bool inSpecDir,
                           ST_OPTARG(const Tst_str *pInDir_Dirn),
                           Tst_str *pTmpFn, Tst_uint32 tmpFnMaxSz);
static Tst_err
ST_SYSFILE__fileOpen(Tst_sys_fstc *pFStc, Tst_bool exOrNew,
                     Tst_bool allowSymAndHardLinks,
                     Tst_bool allowWriting,
                     Tst_bool createVirtual);
static Tst_bool
ST_SYSFILE__fOd_exists(const Tst_str *pPath, Tst_sys_fstc *pFStc,
                       Tst_bool fOrD, Tst_uint32 recursLev,
                       Tst_fsize *pSize, Tst_bool *pErrTooBig);

/** */
#if (ST_SYSFILE_DEB_ == 1)
static void ST_SYSFILE__prf(char *pFmt, ...);
static void ST_SYSFILE__prE(char *pFmt, ...);
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Tst_sys_fstc */
Tst_err
st_sys_stc_initFStc(Tst_sys_fstc *pFStc)
{
	Tst_sys__fstc_intn *pFStcI = NULL;

	ST_ASSERTN_IARG(pFStc == NULL)

	ST_CALLOC(pFStcI, Tst_sys__fstc_intn*,
			1, sizeof(Tst_sys__fstc_intn))
	if (pFStcI == NULL) {
		pFStc->pObInternal = NULL;
		return ST_ERR_OMEM;
	}
	pFStc->pObInternal = pFStcI;
	/* */
	pFStcI->pFilen = NULL;
	pFStcI->pFP    = NULL;
	pFStcI->fd     = -1;
	pFStcI->isVirt = ST_B_FALSE;
	st_sys_stc_rsetFStc(pFStc, ST_B_FALSE);
	return ST_ERR_SUCC;
}

void
st_sys_stc_rsetFStc(Tst_sys_fstc *pFStc, const Tst_bool resetFilen)
{
	Tst_sys__fstc_intn *pFStcI;

	ST_ASSERTN_VOID(pFStc == NULL || pFStc->pObInternal == NULL)

	pFStcI = (Tst_sys__fstc_intn*)pFStc->pObInternal;
	if (resetFilen && pFStcI->pFilen != NULL) {
		ST_DELPOINT(pFStcI->pFilen)
	}
	if (pFStcI->pFP != NULL && ! pFStcI->isVirt) {
		fclose(pFStcI->pFP);  /* also closes pFStcI->fd */
	} else if (pFStcI->fd >= 0 && ! pFStcI->isVirt) {
		close(pFStcI->fd);
	}
	pFStcI->pFP    = NULL;
	pFStcI->fd     = -1;
	pFStcI->modeWr = ST_B_FALSE;
	pFStcI->isVirt = ST_B_FALSE;
}

void
st_sys_stc_freeFStc(Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_VOID(pFStc == NULL)

	if (pFStc->pObInternal != NULL) {
		st_sys_stc_rsetFStc(pFStc, ST_B_TRUE);
		/* */
		ST_DELPOINT(pFStc->pObInternal)
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Does file exist ?
 *
 * pFilename  MUST be != NULL and ISO- or UTF8-encoded
 *
 * returns: true if exists
 */
Tst_bool
st_sysDoesFileExist(const Tst_str *pFilename)
{
	Tst_fsize size = 0;

	return ST_SYSFILE__fOd_exists(pFilename, NULL, ST_B_TRUE, 0, &size, NULL);
}

/*
 * Does directory exist ?
 *
 * pPathname  MUST be != NULL and ISO- or UTF8-encoded
 *
 * returns: true if exists
 */
Tst_bool
st_sysDoesDirExist(const Tst_str *pPathname)
{
	Tst_fsize size = 0;

	return ST_SYSFILE__fOd_exists(pPathname, NULL, ST_B_FALSE, 0, &size, NULL);
}

/*----------------------------------------------------------------------------*/

/*
 * Remove file
 *
 * returns: true on success
 */
Tst_bool
st_sysUnlinkFile(const Tst_str *pFilename)
{
	return (! st_sysDoesFileExist(pFilename) ||
			unlink((const char*)pFilename) == 0);
}

/**
 * Rename a file
 *
 * @returns true on success
 */
Tst_bool
st_sysRenameFile(const Tst_str *pSrc, const Tst_str *pDst)
{
	if (! st_sysDoesFileExist(pSrc)) {
		return ST_B_FALSE;
	}
	//
	if (st_sysDoesFileExist(pDst)) {
		Tst_bool isOnOtherDevice = ST_B_FALSE;
		struct stat lstat_info,
		            fstat_info;

		// check if source and destination files reside on the same device
		#if ! (defined(_WIN32) || defined (__CYGWIN__))
			if (lstat((const char*)pDst, &lstat_info) != 0) {
				return ST_B_FALSE;  // can't retrieve file stats
			}
		#endif

		const int tmpFd = open(
				(const char*)pSrc,
				O_RDONLY
				#if defined(_WIN32) || defined (__CYGWIN__)
					| O_BINARY
				#endif
			);
		if (tmpFd == -1) {
			return ST_B_FALSE;  // can't open file
		}
		if (fstat(tmpFd, &fstat_info) != 0) {
			close(tmpFd);
			return ST_B_FALSE;  // can't retrieve file stats
		}
		#if defined(_WIN32) || defined (__CYGWIN__)
			// there are no symlinks/hardlinks under MS Windows, so we just fake [lstat_info]
			fstat(tmpFd, &lstat_info);
		#endif

		if (lstat_info.st_dev != fstat_info.st_dev) {  // st_dev: device inode resides on
			isOnOtherDevice = ST_B_TRUE;
		}
		close(tmpFd);

		if (isOnOtherDevice) {
			return ST_B_FALSE;  // can't rename file to another device
		}
	}
	//
	if (st_sysDoesFileExist(pDst) && ! st_sysUnlinkFile(pDst)) {
		return ST_B_FALSE;
	}
	//
	return (rename((const char*)pSrc, (const char*)pDst) == 0);
}

/*
 * Create a directory
 *
 * returns: true on success
 */
Tst_bool
st_sysCreateDir(const Tst_str *pDir, const Tst_sys_setFilePerm perms)
{
	#if defined(_WIN32) || defined (__CYGWIN__)
		return (mkdir((const char*)pDir) == 0);
	#else
		#define LOC_MAP_(mac_myPerm, mac_sysPerm)  { \
				if ((perms & (ST_SYS_SETPERM_##mac_myPerm)) != 0) ndMode |= (S_I##mac_sysPerm); }
		mode_t ndMode = 0;

		if (perms != ST_SYS_SETPERM_NONE) {
			LOC_MAP_(OTHX, XOTH)
			LOC_MAP_(OTHW, WOTH)
			LOC_MAP_(OTHR, ROTH)
			LOC_MAP_(GRPX, XGRP)
			LOC_MAP_(GRPW, WGRP)
			LOC_MAP_(GRPR, RGRP)
			LOC_MAP_(USRX, XUSR)
			LOC_MAP_(USRW, WUSR)
			LOC_MAP_(USRR, RUSR)
			LOC_MAP_(SVTX, SVTX)
			LOC_MAP_(SGID, SGID)
			LOC_MAP_(SUID, SUID)
		}
		return (mkdir((const char*)pDir, ndMode) == 0);
		#undef LOC_MAP_
	#endif
}

/*----------------------------------------------------------------------------*/

/*
 * Get file's basename
 *   (e.g. "./watchtheclock.txt" --> "watchtheclock.txt"
 *         "/thorn/asunder/" --> ""
 *         "asunder" --> "asunder"
 *         "/usr/bin/santagger" --> "santagger" )
 *
 * pFilen  MUST be != NULL
 * ppFBn   MUST be != NULL
 *
 * returns: false on error
 *          *ppFBn will contain basename
 */
Tst_bool
st_sysFileBasename(const Tst_str *pFilen, Tst_str **ppFBn)
{
	char const *pS;
	Tst_uint32 pos = 0,
	           len;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFilen == NULL || ppFBn == NULL)

	len = st_sysStrlen(pFilen);
	if (len > 0 && (pS = strrchr((const char*)pFilen, '/')) != NULL) {
		pos  = (Tst_uint32)(pS - (const char*)pFilen) + 1;
		len -= (len < pos ? len : pos);
	}
	ST_REALLOC(*ppFBn, Tst_str*, len + 1, 1)
	if (*ppFBn == NULL) {
		return ST_B_FALSE;
	}
	if (len > 0) {
		memcpy(*ppFBn, &pFilen[pos], len);
	}
	(*ppFBn)[len] = 0;
	return ST_B_TRUE;
}

/*
 * Get filename extension
 *   (e.g. "./watchtheclock.txt" --> "txt"
 *         "/thorn/asunder/" --> ""
 *         "/usr/bin/santagger" --> "" )
 *
 * pFilen  MUST be != NULL
 * ppFExt  MUST be != NULL
 *
 * returns: false on error
 *          *ppFExt will contain extension
 */
Tst_bool
st_sysFileExt(const Tst_str *pFilen, Tst_str **ppFExt)
{
	char const *pD;
	Tst_str    *pB = NULL;
	Tst_uint32 pos = 0,
	           len;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFilen == NULL || ppFExt == NULL)

	if (! st_sysFileBasename(pFilen, &pB)) {
		ST_DELPOINT((*ppFExt))
		ST_DELPOINT(pB)
		return ST_B_FALSE;
	}
	len = st_sysStrlen(pB);
	if (len > 0 && (pD = strrchr((const char*)pB, '.')) != NULL) {
		pos = (Tst_uint32)(pD - (const char*)pB) + 1;
		if (len < pos) {
			len = 0;
		} else {
			len -= pos;
		}
	}
	ST_REALLOC(*ppFExt, Tst_str*, len + 1, 1)
	if (*ppFExt == NULL) {
		ST_DELPOINT(pB)
		return ST_B_FALSE;
	}
	if (len > 0) {
		memcpy(*ppFExt, &pB[pos], len);
	}
	(*ppFExt)[len] = 0;
	ST_DELPOINT(pB)
	return ST_B_TRUE;
}

/*
 * Get dirname
 *   (e.g. "./watchtheclock.txt" --> "."
 *         "/thorn/asunder/" --> "/thorn/asunder"
 *         "asunder" --> "."
 *         "/thorn" --> "/"
 *         "/usr/bin/santagger" --> "/usr/bin" )
 *
 * pPath    MUST be != NULL
 * ppDirn   MUST be != NULL
 *
 * returns: false on error
 *          *ppDirn will contain directory's name
 */
Tst_bool
st_sysDirname(const Tst_str *pPath, Tst_str **ppDirn)
{
	char const *pS;
	Tst_uint32 len;

	ST_ASSERTN_BOOL(ST_B_FALSE, pPath == NULL || ppDirn == NULL)

	len = st_sysStrlen(pPath);
	if (len > 0 && (pS = strrchr((const char*)pPath, '/')) != NULL) {
		Tst_uint32 pos;

		pos = (Tst_uint32)(pS - (const char*)pPath);
		if (pos > 0) {
			--pos;
		}
		len = pos + 1;
	} else {
		len = 0;
	}
	ST_REALLOC(*ppDirn, Tst_str*, len + 2, 1)
	if (*ppDirn == NULL) {
		return ST_B_FALSE;
	}
	if (len > 0) {
		memcpy(*ppDirn, pPath, len);
	} else {
		(*ppDirn)[len++] = '.';
	}
	(*ppDirn)[len] = 0;
	return ST_B_TRUE;
}

/*
 * Get full file path
 *   (e.g. "."               + "watchtheclock.txt" --> "./watchtheclock.txt"
 *         "./"              + "watchtheclock.txt" --> "./watchtheclock.txt"
 *         "/thorn/asunder/" + ""                  --> "/thorn/asunder/"
 *         ""                + "asunder"           --> "./asunder"
 *         "/thorn"          + ""                  --> "/thorn/"
 *         "/usr/bin"        + "santagger"         --> "/usr/bin/santagger" )
 *         "/usr"            + "bin/santagger"     --> "/usr/santagger" )
 *
 * pDirn        MUST be != NULL
 * pFilen       MUST be != NULL
 * ppFullPath   MUST be != NULL
 *
 * returns: false on error
 *          *ppFullPath will contain the full path
 */
Tst_bool
st_sysConcatDirAndFilen(const Tst_str *pDirn, const Tst_str *pFilen,
		Tst_str **ppFullPath)
{
	Tst_uint32 fpSz;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDirn == NULL || pFilen == NULL ||
				ppFullPath == NULL)

	fpSz = st_sysStrlen(pDirn) + 2 + st_sysStrlen(pFilen) + 1;
	ST_REALLOC(*ppFullPath, Tst_str*, fpSz, 1)
	if (*ppFullPath == NULL) {
		return ST_B_FALSE;
	}
	return ST_SYSFILE__concatDirAndFilen(pDirn, pFilen, *ppFullPath, fpSz);
}

/*----------------------------------------------------------------------------*/

/*
 * Create a temporary filename
 *
 * pTmpFn       MUST be != NULL and at least tmpFnMaxSz bytes big
 * tmpFnMaxSz   MUST be >= 32
 *
 * returns: true on success
 *          pTmpFn will contain temporary filename
 */
Tst_bool
st_sysGetTmpFilename(Tst_str *pTmpFn, const Tst_uint32 tmpFnMaxSz,
		const Tst_bool inCurrentDir)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pTmpFn == NULL || tmpFnMaxSz < 32)

	return ST_SYSFILE__getTmpFilename(inCurrentDir, /*inSpecDir:*/ST_B_FALSE,
				/*pInDir_Dirn:*/NULL, pTmpFn, tmpFnMaxSz);
}

Tst_bool
st_sysGetTmpFilenameInDir(const Tst_str *pDirn, Tst_str **ppTmpFn)
{
	Tst_uint32 tmpFnMaxSz;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDirn == NULL || ppTmpFn == NULL)

	tmpFnMaxSz = st_sysStrlen(pDirn) + 8 + 256;
	ST_REALLOC(*ppTmpFn, Tst_str*, tmpFnMaxSz, 1)
	if (*ppTmpFn == NULL) {
		return ST_B_FALSE;
	}
	return ST_SYSFILE__getTmpFilename(/*inCurDir:*/ST_B_FALSE, /*inSpecDir:*/ST_B_TRUE,
				pDirn, *ppTmpFn, tmpFnMaxSz);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Tst_err
st_sysFStc_setFilen(Tst_sys_fstc *pFStc, const Tst_str *pFilename)
{
	Tst_sys__fstc_intn *pFStcI;

	ST_ASSERTN_IARG(pFStc == NULL || pFStc->pObInternal == NULL)

	pFStcI = (Tst_sys__fstc_intn*)pFStc->pObInternal;

	/* copy pFilename to pFStc */
	ST_REALLOC(pFStcI->pFilen, Tst_str*, st_sysStrlen(pFilename) + 1, 1)
	if (pFStcI->pFilen == NULL) {
		return ST_ERR_OMEM;
	}
	strcpy((char*)pFStcI->pFilen, (const char*)pFilename);

	return ST_ERR_SUCC;
}

const Tst_str*
st_sysFStc_getFilen(const Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_NULL(pFStc == NULL || pFStc->pObInternal == NULL)

	return ((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFilen;
}

Tst_bool
st_sysFStc_isOpen(const Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFStc == NULL || pFStc->pObInternal == NULL)

	return (((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP != NULL);
}

Tst_bool
st_sysFStc_isWritable(const Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFStc == NULL || pFStc->pObInternal == NULL)

	return (((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP != NULL &&
			((Tst_sys__fstc_intn*)pFStc->pObInternal)->modeWr);
}

Tst_bool
st_sysFStc_isVirtual(const Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_BOOL(ST_B_FALSE, pFStc == NULL || pFStc->pObInternal == NULL)

	return (((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP != NULL &&
			((Tst_sys__fstc_intn*)pFStc->pObInternal)->isVirt);
}

/*----------------------------------------------------------------------------*/

/*
 * Safely open existing file
 */
Tst_err
st_sysFStc_openExisting(Tst_sys_fstc *pFStc,
		const Tst_bool allowSymAndHardLinks, const Tst_bool allowWriting)
{
	return ST_SYSFILE__fileOpen(pFStc, /*existingOrNew:*/ST_B_TRUE,
			allowSymAndHardLinks, allowWriting,
			/*virtual:*/ST_B_FALSE);
}

/*
 * Safely create and open new file for reading/writing
 */
Tst_err
st_sysFStc_openNew(Tst_sys_fstc *pFStc)
{
	return ST_SYSFILE__fileOpen(pFStc, /*existingOrNew:*/ST_B_FALSE,
			/*allwLinks:*/ST_B_FALSE, /*allwWr:*/ST_B_TRUE,
			/*virtual:*/ST_B_FALSE);
}

/*
 * Create and open new 'virtual' file for reading/writing
 */
Tst_err
st_sysFStc_openNewVirtual(Tst_sys_fstc *pFStc)
{
	return ST_SYSFILE__fileOpen(pFStc, /*existingOrNew:*/ST_B_FALSE,
			/*allwLinks:*/ST_B_FALSE, /*allwWr:*/ST_B_TRUE,
			/*virtual:*/ST_B_TRUE);
}

/*
 * Safely change access mode of already opened file
 */
Tst_err
st_sysFStc_changeMode(Tst_sys_fstc *pFStc, const Tst_bool allowWriting)
{
	Tst_err res = ST_ERR_SUCC;
	struct stat        fstat_info1,
	                   fstat_info2;
	Tst_sys__fstc_intn *pFStcI;

	ST_ASSERTN_IARG(pFStc == NULL ||
			pFStc->pObInternal == NULL)
	pFStcI = (Tst_sys__fstc_intn*)pFStc->pObInternal;

	if (pFStcI->fd < 0 || st_sysStrEmpty(pFStcI->pFilen)) {
		return ST_ERR_FAIL;
	}
	if (pFStcI->modeWr == allowWriting) {
		return ST_ERR_SUCC;
	}

	if (! pFStcI->isVirt) {
		if (fstat(pFStcI->fd, &fstat_info1) != 0) {
			res = ST_ERR_FCOF;  /* can't open file */
		} else {
			fclose(pFStcI->pFP);  /* also closes pFStcI->fd */
			pFStcI->pFP = NULL;
			/* */
			pFStcI->fd = open(
					(const char*)pFStcI->pFilen,
					(allowWriting ? O_RDWR : O_RDONLY)
					#if defined(_WIN32) || defined (__CYGWIN__)
						| O_BINARY
					#endif
				);
			if (pFStcI->fd == -1) {
				res = ST_ERR_FCOF;  /* can't open file */
			}

			if (res == ST_ERR_SUCC &&
					((fstat(pFStcI->fd, &fstat_info2) != 0) ||
						! (fstat_info1.st_mode == fstat_info2.st_mode &&
							fstat_info1.st_ino == fstat_info2.st_ino &&
							fstat_info1.st_dev == fstat_info2.st_dev))) {
				res = ST_ERR_FCOF;  /* can't open file */
			}
		}

		/* associate pFP with fd */
		if (res == ST_ERR_SUCC) {
			pFStcI->pFP = fdopen(pFStcI->fd, allowWriting ? "rb+" : "rb");
			if (pFStcI->pFP == NULL) {
				res = ST_ERR_FCCF;  /* can't create file */
			}
		}
	}

	if (res == ST_ERR_SUCC) {
		pFStcI->modeWr = allowWriting;
	} else {
		if (pFStcI->fd >= 0) {
			close(pFStcI->fd);
			pFStcI->fd  = -1;
			pFStcI->pFP = NULL;
		}
		st_sys_stc_rsetFStc(pFStc, ST_B_FALSE);
	}
	return res;
}

/*
 * Close file that has been opened before
 */
Tst_err
st_sysFStc_close(Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_IARG(pFStc == NULL || pFStc->pObInternal == NULL)

	if (((Tst_sys__fstc_intn*)pFStc->pObInternal)->fd >= 0) {
		if (! ((Tst_sys__fstc_intn*)pFStc->pObInternal)->isVirt) {
			/* also closes pFStcI->fd */
			fclose(((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP);
		}
		((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP = NULL;
		((Tst_sys__fstc_intn*)pFStc->pObInternal)->fd  = -1;
	}
	st_sys_stc_rsetFStc(pFStc, ST_B_FALSE);
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Get file size
 *
 * pFilename   MUST be != NULL and ISO- or UTF8-encoded
 * pErrTooBig  may be NULL
 *
 * returns: filesize or 0 on error
 *          if pErrTooBig is != NULL and the filesize exceeds the limits
 *            of off_t then pErrTooBig will be to ST_B_TRUE
 */
Tst_fsize
st_sysGetFileSz_byFn(const Tst_str *pFilename, ST_OPTARG(Tst_bool *pErrTooBig))
{
	Tst_fsize size = 0;

	if (st_sysStrEmpty(pFilename) ||
			! ST_SYSFILE__fOd_exists(pFilename, NULL,
					ST_B_TRUE, 0, &size, pErrTooBig)) {
		return 0;
	}
	return size;
}

/*
 * Get file size of already opened file
 *
 * pFStc       MUST be != NULL
 * pErrTooBig  may be NULL
 *
 * returns: filesize or 0 on error
 */
Tst_fsize
st_sysFStc_getFileSz(Tst_sys_fstc *pFStc, ST_OPTARG(Tst_bool *pErrTooBig))
{
	Tst_fsize size = 0;

	ST_ASSERTN_NUM(0, pFStc == NULL || pFStc->pObInternal == NULL)

	if (! ST_SYSFILE__fOd_exists(NULL, pFStc,
			ST_B_TRUE, 0, &size, pErrTooBig)) {
		return 0;
	}
	return size;
}

/*----------------------------------------------------------------------------*/

/*
 * Has EOF been reached ?
 */
Tst_bool
st_sysFStc_isEOF(Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_BOOL(ST_B_TRUE, pFStc == NULL || pFStc->pObInternal == NULL)

	return (((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP == NULL ||
			((Tst_sys__fstc_intn*)pFStc->pObInternal)->isVirt ||
			(feof(((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP) != 0));
}

/*
 * Get current position in file
 *
 * pFStc  MUST be set
 *
 * returns: filepos or -1 on error
 */
Tst_foffs
st_sysFStc_getFPos(Tst_sys_fstc *pFStc)
{
	Tst_foffs pos = 0;

	ST_ASSERTN_NUM(-1, pFStc == NULL || pFStc->pObInternal == NULL)

	if (! ((Tst_sys__fstc_intn*)pFStc->pObInternal)->isVirt) {
		if (((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP == NULL ||
					#if (HAVE_FSEEKO == 1)
						(pos = ftello(((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP)) < 0
					#else
						(pos = ftell(((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP)) < 0
					#endif
				) {
			return (-1);
		}
	}
	return pos;
}

/*
 * Set current position in file
 *
 * pFStc   MUST be set
 *
 * returns: true on success
 */
Tst_bool
st_sysFStc_setFPos(Tst_sys_fstc *pFStc,
		const Tst_foffs offs, const Tst_sys_setFilePos fromWhere)
{
	Tst_bool resB;
	Tst_sys__fstc_intn *pFStcI;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFStc == NULL || pFStc->pObInternal == NULL)

	pFStcI = (Tst_sys__fstc_intn*)pFStc->pObInternal;
	ST_ASSERTN_BOOL(ST_B_FALSE, pFStcI->pFP == NULL)

	if (pFStcI->isVirt) {
		resB = ST_B_TRUE;
	} else {
		int whence;

		switch (fromWhere) {
			case ST_SYS_SETFPOS_BEG: whence = SEEK_SET; break;
			case ST_SYS_SETFPOS_CUR: whence = SEEK_CUR; break;
			case ST_SYS_SETFPOS_END: whence = SEEK_END; break;
			default:
				return ST_B_FALSE;
		}
		#if (HAVE_FSEEKO == 1)
			resB = (fseeko(pFStcI->pFP, offs, whence) == 0);
		#else
			resB = (fseek(pFStcI->pFP, offs, whence) == 0);
		#endif
		if (resB && feof(pFStcI->pFP) != 0) {
			clearerr(pFStcI->pFP);
		}
	}
	return resB;
}

/*
 * Skip some bytes in file
 *
 * pFStc   MUST be set
 *
 * returns: amount of skipped bytes or on error 0
 */
Tst_fsize
st_sysFStc_skipBytes(Tst_sys_fstc *pFStc, const Tst_fsize skipFS)
{
	Tst_foffs cpos,
	          cposN;
	FILE      *pFP;

	ST_ASSERTN_NUM(0, pFStc == NULL || pFStc->pObInternal == NULL)
	pFP = ((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP;
	ST_ASSERTN_NUM(0, pFP == NULL)
	if (skipFS == 0) {
		return 0;
	}

	if (((Tst_sys__fstc_intn*)pFStc->pObInternal)->isVirt) {
		return 0;
	}
	cpos = st_sysFStc_getFPos(pFStc);
	if (cpos < 0) {
		return 0;
	}

	/*fseeko(pFP, (Tst_foffs)skipFS, SEEK_CUR);*/
	{
		Tst_fsize toRd,
				  rdTot = 0,
				  rd;
		Tst_buf   buf[4096];

		toRd = (Tst_fsize)sizeof(toRd);
		while (ST_B_TRUE) {
			if (rdTot + toRd > skipFS) {
				toRd = skipFS - rdTot;
				if (toRd == 0) {
					break;
				}
			}
			/* */
			rd = (Tst_fsize)fread(buf, 1, (size_t)toRd, pFP);
			if (rd != toRd) {
				break;
			}
			rdTot += rd;
		}
	}

	cposN = st_sysFStc_getFPos(pFStc);
	if (cposN < cpos) {
		return 0;
	}
	return (Tst_fsize)(cposN - cpos);
}

/*----------------------------------------------------------------------------*/

/*
 * Flush writable file
 */
void
st_sysFStc_flush(Tst_sys_fstc *pFStc)
{
	ST_ASSERTN_VOID(pFStc == NULL || pFStc->pObInternal == NULL)

	if (! ((Tst_sys__fstc_intn*)pFStc->pObInternal)->modeWr ||
			((Tst_sys__fstc_intn*)pFStc->pObInternal)->isVirt) {
		return;
	}

	ST_ASSERTN_VOID(((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP == NULL)

	fflush(((Tst_sys__fstc_intn*)pFStc->pObInternal)->pFP);
}

/*----------------------------------------------------------------------------*/

/*
 * Copy up to maxCopy bytes from pFStcIn to pFStcOut
 *
 * pFStcIn     MUST be set
 * pFStcOut    MUST be set if ! pretendWriting
 * pCopiedFS   may be == NULL
 *
 * returns: ERRCODE
 */
Tst_err
st_sysFStc_copyFromAndToFile(const Tst_bool pretendWriting,
		Tst_sys_fstc *pFStcIn, Tst_sys_fstc *pFStcOut,
		const Tst_fsize maxCopyFS,
		ST_OPTARG(Tst_fsize *pCopiedFS))
{
	Tst_fsize toRd,
	          rd,
	          rdTot = 0;
	Tst_buf   buf[4096 * 4];
	FILE      *pFPin,
	          *pFPout = NULL;

	ST_ASSERTN_IARG(pFStcIn == NULL || pFStcIn->pObInternal == NULL ||
			(! pretendWriting &&
				(pFStcOut == NULL || pFStcOut->pObInternal == NULL)))
	pFPin = ((Tst_sys__fstc_intn*)pFStcIn->pObInternal)->pFP;
	ST_ASSERTN_IARG(pFPin == NULL)
	if (! pretendWriting) {
		pFPout = ((Tst_sys__fstc_intn*)pFStcOut->pObInternal)->pFP;
		ST_ASSERTN_IARG(pFPout == NULL)
		if (! ((Tst_sys__fstc_intn*)pFStcOut->pObInternal)->modeWr) {
			return ST_ERR_FAIL;
		}
	}

	if (pCopiedFS != NULL) {
		*pCopiedFS = 0;
	}
	if (maxCopyFS == 0 ||
			((Tst_sys__fstc_intn*)pFStcIn->pObInternal)->isVirt ||
			(pFStcOut != NULL && pFStcOut->pObInternal != NULL && ((Tst_sys__fstc_intn*)pFStcOut->pObInternal)->isVirt)) {
		return ST_ERR_SUCC;
	}

	toRd = (Tst_fsize)sizeof(buf);
	memset(buf, 0, sizeof(buf));
	while (feof(pFPin) == 0) {
		if (rdTot + toRd > maxCopyFS) {
			toRd = maxCopyFS - rdTot;
			if (toRd == 0) {
				break;
			}
		}
		/* */
		rd = (Tst_fsize)fread(buf, 1, (size_t)toRd, pFPin);
		if (rd != toRd && ferror(pFPin) != 0 && feof(pFPin) == 0) {
			clearerr(pFPin);
			return ST_ERR_FCRD;
		}
		if (rd > 0) {
			rdTot += rd;
			/* */
			if (! pretendWriting &&
					(Tst_fsize)fwrite(buf, 1, (size_t)rd, pFPout) != rd) {
				clearerr(pFPout);
				return ST_ERR_FCWR;
			}
			if (pCopiedFS != NULL) {
				*pCopiedFS += rd;
			}
		}
		if (rd != toRd) {
			break;
		}
	}
	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/

/*
 * Write buffer to pFStcOut
 *
 * pFStcOut  MUST be set if ! pretendWriting
 * pBuf      MUST be != NULL
 *
 * returns: ERRCODE
 */
Tst_err
st_sysFStc_writeBuf(Tst_sys_fstc *pFStcOut,
		const void *pBuf, const Tst_uint32 bufSz)
{
	Tst_uint32 wr;
	FILE       *pFPout;

	ST_ASSERTN_IARG(pFStcOut == NULL || pFStcOut->pObInternal == NULL ||
			! ((Tst_sys__fstc_intn*)pFStcOut->pObInternal)->modeWr)
	pFPout = ((Tst_sys__fstc_intn*)pFStcOut->pObInternal)->pFP;
	ST_ASSERTN_IARG(pFPout == NULL)

	if (bufSz == 0 ||
			((Tst_sys__fstc_intn*)pFStcOut->pObInternal)->isVirt) {
		return ST_ERR_SUCC;
	}

	wr = (Tst_uint32)fwrite(pBuf, 1, (size_t)bufSz, pFPout);
	if (wr != bufSz) {
		return ST_ERR_FCWR;
	}
	return ST_ERR_SUCC;
}

/*
 * Read buffer from file
 *
 * pFStcIn   MUST be set
 * pBufOut   MUST be != NULL and at least maxCopy-Bytes big
 * pCopied   MUST be != NULL
 *
 * returns: ERRCODE
 */
Tst_err
st_sysFStc_readBuf(Tst_sys_fstc *pFStcIn,
		const Tst_uint32 maxCopy, void *pBufOut,
		Tst_uint32 *pCopied)
{
	Tst_uint32 rd;
	FILE       *pFPin;

	ST_ASSERTN_IARG(pFStcIn == NULL || pFStcIn->pObInternal == NULL ||
			pBufOut == NULL)
	pFPin = ((Tst_sys__fstc_intn*)pFStcIn->pObInternal)->pFP;
	ST_ASSERTN_IARG(pFPin == NULL)

	*pCopied = 0;
	if (maxCopy == 0 ||
			((Tst_sys__fstc_intn*)pFStcIn->pObInternal)->isVirt) {
		return ST_ERR_SUCC;
	}

	rd = (Tst_uint32)fread(pBufOut, 1, (size_t)maxCopy, pFPin);
	if (rd != maxCopy && ferror(pFPin) != 0 && feof(pFPin) == 0) {
		clearerr(pFPin);
		return ST_ERR_FCRD;
	}
	*pCopied = rd;

	return ST_ERR_SUCC;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static Tst_bool
ST_SYSFILE__concatDirAndFilen(const Tst_str *pDirn, const Tst_str *pFilen,
		Tst_str *pFullPath, const Tst_uint32 fpSz)
{
	Tst_uint32    lenD,
	              lenF;
	Tst_str       *pFBn = NULL;
	Tst_str const *pIntFilen;

	ST_ASSERTN_BOOL(ST_B_FALSE, pDirn == NULL || pFilen == NULL ||
				pFullPath == NULL)

	lenD = st_sysStrlen(pDirn);
	lenF = st_sysStrlen(pFilen);
	if (lenF > 0 && strrchr((const char*)pFilen, '/') != NULL) {
		if (! st_sysFileBasename(pFilen, &pFBn)) {
			return ST_B_FALSE;
		}
		pIntFilen = pFBn;
		lenF      = st_sysStrlen(pFBn);
	} else {
		pIntFilen = pFilen;
	}

	if (lenD > 0) {
		if (lenD > fpSz) {
			return ST_B_FALSE;
		}
		memcpy(pFullPath, pDirn, lenD);
		if (pDirn[lenD - 1] != '/') {
			if (lenD + 1 > fpSz) {
				return ST_B_FALSE;
			}
			pFullPath[lenD++] = '/';
		}
	} else {
		if (2 > fpSz) {
			return ST_B_FALSE;
		}
		pFullPath[lenD++] = '.';
		pFullPath[lenD++] = '/';
	}
	if (lenF > 0) {
		if (lenD + lenF > fpSz) {
			return ST_B_FALSE;
		}
		memcpy(&pFullPath[lenD], pIntFilen, lenF);
	}
	if (lenD + lenF + 1 > fpSz) {
		return ST_B_FALSE;
	}
	pFullPath[lenD + lenF] = 0x00;

	ST_DELPOINT(pFBn)
	return ST_B_TRUE;
}

/*----------------------------------------------------------------------------*/

static Tst_bool
ST_SYSFILE__getTmpFilename(const Tst_bool inCurDir, const Tst_bool inSpecDir,
		ST_OPTARG(const Tst_str *pInDir_Dirn),
		Tst_str *pTmpFn, const Tst_uint32 tmpFnMaxSz)
{
	Tst_bool ex;


	ST_ASSERTN_BOOL(ST_B_FALSE, pTmpFn == NULL || tmpFnMaxSz > 1024 * 4 ||
				(inSpecDir && pInDir_Dirn == NULL))

	if (inCurDir || inSpecDir) {
		const Tst_uint32 cMAXTRIES = 1000;
		Tst_uint32    cnt = 0,
		              tmpVal[3],
		              lastVal[3];
		Tst_str       locTmpFn[128];
		Tst_str const *pODir;

		if (inCurDir) {
			pODir = (const Tst_str*)".";
		} else {
			pODir = pInDir_Dirn;
		}
		do {
			tmpVal[0] = st_sysGetRand(0, 999);
			tmpVal[1] = st_sysGetRand(0, 999);
			if (tmpVal[0] == tmpVal[1]) {
				tmpVal[1] = st_sysGetRand(0, 999);
				if (tmpVal[0] == tmpVal[1]) {
					tmpVal[1] = (tmpVal[1] + 1) % 999;
				}
			}
			tmpVal[2] = st_sysGetRand(0, 999);
			if (tmpVal[0] == tmpVal[2] || tmpVal[1] == tmpVal[2]) {
				tmpVal[2] = st_sysGetRand(0, 999);
				if (tmpVal[0] == tmpVal[2] || tmpVal[1] == tmpVal[2]) {
					tmpVal[2] = (tmpVal[2] + 2) % 999;
				}
			}
			if (cnt > 0 &&
					(tmpVal[0] == lastVal[0] || tmpVal[1] == lastVal[1] ||
						tmpVal[2] == lastVal[2])) {
				/* just in case the PRNG doesn't work correctly */
				tmpVal[0] = ((tmpVal[0] + 1) * (cnt + 1)) % 999;
				tmpVal[1] = (tmpVal[0] * 2) % 999;
				tmpVal[2] = (tmpVal[1] * 3) % 999;
			}
			lastVal[0] = tmpVal[0];
			lastVal[1] = tmpVal[1];
			lastVal[2] = tmpVal[2];
			/* */
			snprintf((char*)locTmpFn, sizeof(locTmpFn),
					"tmpfile.%03u.%03u.%03u.tmp",
					tmpVal[0], tmpVal[1], tmpVal[2]);
			ex = ! ST_SYSFILE__concatDirAndFilen(pODir, locTmpFn,
						pTmpFn, tmpFnMaxSz);
			if (ex) {
				break;
			}
			/* */
			++cnt;
			ex = st_sysDoesFileExist(pTmpFn) || st_sysDoesDirExist(pTmpFn);
		} while (ex && cnt < cMAXTRIES);
	} else {
		snprintf((char*)pTmpFn, tmpFnMaxSz, "tmpfile.XXXXXXXX");
		const int tmpHnd = mkstemp((char*)pTmpFn);
		ex = (tmpHnd < 1);
		if (tmpHnd > 0) {
			close(tmpHnd);
			// delete the file to mimic the behavior of the other 'if' branch
			st_sysUnlinkFile(pTmpFn);
		}
	}
	if (ex) {
		pTmpFn[0] = 0x00;
	}
	return (! ex);
}

/*----------------------------------------------------------------------------*/

static Tst_err
ST_SYSFILE__fileOpen(Tst_sys_fstc *pFStc, const Tst_bool exOrNew,
		const Tst_bool allowSymAndHardLinks, const Tst_bool allowWriting,
		const Tst_bool createVirtual)
{
	Tst_err res = ST_ERR_SUCC;
	#if ! (defined(_WIN32) || defined (__CYGWIN__))
		struct stat lstat_info;
	#endif
	struct stat        fstat_info;
	Tst_sys__fstc_intn *pFStcI;

	ST_ASSERTN_IARG(pFStc == NULL || pFStc->pObInternal == NULL)

	st_sys_stc_rsetFStc(pFStc, ST_B_FALSE);
	pFStcI = (Tst_sys__fstc_intn*)pFStc->pObInternal;
	if (st_sysStrEmpty(pFStcI->pFilen)) {
		return ST_ERR_FAIL;
	}

	if (exOrNew && ! createVirtual) {
		/* open existing file */

		#if ! (defined(_WIN32) || defined (__CYGWIN__))
			if (lstat((const char*)pFStcI->pFilen, &lstat_info) != 0 ||
					(! (S_ISREG(lstat_info.st_mode) || (S_ISLNK(lstat_info.st_mode) && allowSymAndHardLinks)))) {
				res = ST_ERR_FCOF;  /* can't open file */
			}
		#endif

		if (res == ST_ERR_SUCC) {
			pFStcI->fd = open(
					(const char*)pFStcI->pFilen,
					(allowWriting ? O_RDWR : O_RDONLY)
					#if defined(_WIN32) || defined (__CYGWIN__)
						| O_BINARY
					#endif
				);
			if (pFStcI->fd == -1) {
				res = ST_ERR_FCOF;  /* can't open file */
			}
		}

		if (res == ST_ERR_SUCC && fstat(pFStcI->fd, &fstat_info) != 0) {
			res = ST_ERR_FCOF;  /* can't open file */
		}
		#if ! (defined(_WIN32) || defined (__CYGWIN__))
			if (res == ST_ERR_SUCC &&
					lstat_info.st_nlink > 1 && ! allowSymAndHardLinks) {
				/* file has multiple hard links */
				res = ST_ERR_FCOF;  /* can't open file */
			}
		#endif
	} else if (! createVirtual) {
		/* create new file */

		if (st_sysDoesFileExist(pFStcI->pFilen)) {
			res = ST_ERR_FCCF;  /* can't create file */
		} else {
			pFStcI->fd = open(
					(const char*)pFStcI->pFilen,
					(allowWriting ? O_RDWR : O_RDONLY) | O_CREAT | O_EXCL
					#if defined(_WIN32) || defined (__CYGWIN__)
						| O_BINARY
					#endif
					, S_IRUSR | S_IWUSR
				);
			if (pFStcI->fd == -1) {
				res = ST_ERR_FCCF;  /* can't create file */
			}
		}
	} else {
		pFStcI->isVirt = ST_B_TRUE;
		pFStcI->fd     = 1;  /* virtual filedescriptor */
	}

	/* associate pFP with fd */
	if (res == ST_ERR_SUCC && ! createVirtual) {
		pFStcI->pFP = fdopen(pFStcI->fd, allowWriting ? "rb+" : "rb");
		if (pFStcI->pFP == NULL) {
			res = ST_ERR_FCCF;  /* can't create file */
		}
	} else if (res == ST_ERR_SUCC) {
		pFStcI->pFP = (FILE*)0x1;  /* virtual file pointer */
	}

	if (res == ST_ERR_SUCC) {
		pFStcI->modeWr = allowWriting;
	} else {
		if (pFStcI->pFP != NULL) {
			if (! createVirtual) {
				fclose(pFStcI->pFP);  /* also closes pFStcI->fd */
			}
			pFStcI->pFP = NULL;
			pFStcI->fd  = -1;
		} else if (pFStcI->fd >= 0) {
			if (! createVirtual) {
				close(pFStcI->fd);
			}
			pFStcI->fd = -1;
		}
		st_sys_stc_rsetFStc(pFStc, ST_B_FALSE);
	}
	return res;
}

/*----------------------------------------------------------------------------*/

/*
 * returns: true if file/directory exists
 */
static Tst_bool
ST_SYSFILE__fOd_exists(const Tst_str *pPath,
		Tst_sys_fstc *pFStc,
		const Tst_bool fOrD, const Tst_uint32 recursLev,
		Tst_fsize *pSize, Tst_bool *pErrTooBig)
{
	#if (HAVE_READLINK == 1)
		Tst_bool  resB;
		Tst_str   *pStr = NULL;
		Tst_int32 resI;
	#endif
	struct stat statBuf;

	ST_ASSERTN_BOOL(ST_B_FALSE, pFStc != NULL && pFStc->pObInternal == NULL)

	*pSize = 0;
	if (pErrTooBig != NULL) {
		*pErrTooBig = ST_B_FALSE;
	}
	if (recursLev > 100 || (st_sysStrEmpty(pPath) && pFStc == NULL)) {
		return ST_B_FALSE;
	}
	if (pFStc != NULL &&
			((Tst_sys__fstc_intn*)pFStc->pObInternal)->isVirt) {
		return ST_B_FALSE;
	}

	/* check whether file/dir exists */
	if (pFStc == NULL) {
		char tmpPathStr[1024 * 4];
		const size_t tmpPathLen = (sizeof(tmpPathStr) > strlen((const char *)pPath) ? strlen((const char *)pPath) : sizeof(tmpPathStr) - 1);
		memcpy(tmpPathStr, pPath, tmpPathLen);  // pPath is guaranteed to be != NULL here
		tmpPathStr[tmpPathLen] = 0x00;
		if (stat(tmpPathStr, &statBuf) != 0) {
			if (errno == EOVERFLOW) {
				/* file too big for off_t */
				if (pErrTooBig != NULL) {
					*pErrTooBig = ST_B_TRUE;
				}
				return ST_B_TRUE;  /* file exists, it's just too big */
			}
			return ST_B_FALSE;
		}
	} else if (fstat(((Tst_sys__fstc_intn*)pFStc->pObInternal)->fd, &statBuf) != 0) {
		if (errno == EOVERFLOW) {
			/* file too big for off_t */
			if (pErrTooBig != NULL) {
				*pErrTooBig = ST_B_TRUE;
			}
			return ST_B_TRUE;  /* file exists, it's just too big */
		}
		return ST_B_FALSE;
	}

	/* if it's a symlink, follow it */
	#if (HAVE_READLINK == 1)
		if (S_ISLNK(statBuf.st_mode) != 0) {
			if (statBuf.st_size < 1) {
				return ST_B_FALSE;
			}
			/** read destination of symlink */
			ST_CALLOC(pStr, Tst_str*, statBuf.st_size + 4, 1)
			if (pStr == NULL) {
				return ST_B_FALSE;
			}
			if (pPath == NULL) {
				resI = -1;
			} else {
				resI = (Tst_int32)readlink((const char*)pPath, (char*)pStr, statBuf.st_size + 2);
			}
			if (resI < 0) {
				ST_DELPOINT(pStr)
				return ST_B_FALSE;
			}
			/** check if symlink's dest exists */
			pStr[resI] = 0;
			resB       = ST_SYSFILE__fOd_exists(pStr, NULL, fOrD, recursLev + 1,
					pSize, pErrTooBig);
			/** */
			ST_DELPOINT(pStr)
			return resB;
		}
	#endif
	/* */
	*pSize = (Tst_fsize)statBuf.st_size /*+
			(statBuf.st_size < 0) * ((Tst_fsize)ST_TFOFFS_MAX - ST_TFOFFS_MIN + 1)*/;
	if (fOrD) {
		return (S_ISREG(statBuf.st_mode) != 0);  /* is regular file ? */
	}
	return (S_ISDIR(statBuf.st_mode) != 0);  /* is directory ? */
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if (ST_SYSFILE_DEB_ == 1)
static void ST_SYSFILE__prf(char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vprintf(pFmt, args);
	va_end(args);
}

static void ST_SYSFILE__prE(char *pFmt, ...)
{
	va_list args;

	va_start(args, pFmt);
	vfprintf(stderr, pFmt, args);
	va_end(args);
}
#endif

/******************************************************************************/
