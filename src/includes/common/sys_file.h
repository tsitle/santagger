/******************************************************************************/
/* sys_file.h                   [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)                */
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

#ifndef HEAD_SYS_FILE_H
#define HEAD_SYS_FILE_H

/*
// Own-Includes
*/
#include "src/includes/common/sys_file-defs.h"


ST_BEGIN_C_DECLS

/*
// Functions -- sys_file.c
*/
/** general functions */
/*** */
ST_EXPORT Tst_bool
st_sysDoesFileExist(const Tst_str *pFilename);

ST_EXPORT Tst_bool
st_sysDoesDirExist(const Tst_str *pPathname);

/*** */
/*** */
ST_EXPORT Tst_bool
st_sysUnlinkFile(const Tst_str *pFilename);

ST_EXPORT Tst_bool
st_sysRenameFile(const Tst_str *pSrc, const Tst_str *pDst);

ST_EXPORT Tst_bool
st_sysCreateDir(const Tst_str *pDir, const Tst_sys_setFilePerm perms);

/*** */
ST_EXPORT Tst_bool
st_sysFileBasename(const Tst_str *pFilen, Tst_str **ppFBn);

ST_EXPORT Tst_bool
st_sysFileExt(const Tst_str *pFilen, Tst_str **ppFExt);

ST_EXPORT Tst_bool
st_sysDirname(const Tst_str *pPath, Tst_str **ppDirn);

ST_EXPORT Tst_bool
st_sysConcatDirAndFilen(const Tst_str *pDirn,
                        const Tst_str *pFilen,
                        Tst_str **ppFullPath);

/*** */
ST_EXPORT Tst_bool
st_sysGetTmpFilename(Tst_str *pTmpFn,
                     const Tst_uint32 tmpFnMaxSz,
                     const Tst_bool inCurrentDir);

ST_EXPORT Tst_bool
st_sysGetTmpFilenameInDir(const Tst_str *pDirn, Tst_str **ppTmpFn);

/*** */
ST_EXPORT Tst_fsize
st_sysGetFileSz_byFn(const Tst_str *pFilename,
                     ST_OPTARG(Tst_bool *pErrTooBig));

/** for reading/writing to files */
/*** */
ST_EXPORT Tst_err
st_sysFStc_setFilen(Tst_sys_fstc *pFStc, const Tst_str *pFilename);

ST_EXPORT const Tst_str*
st_sysFStc_getFilen(const Tst_sys_fstc *pFStc);

ST_EXPORT Tst_bool
st_sysFStc_isOpen(const Tst_sys_fstc *pFStc);

ST_EXPORT Tst_bool
st_sysFStc_isWritable(const Tst_sys_fstc *pFStc);

ST_EXPORT Tst_bool
st_sysFStc_isVirtual(const Tst_sys_fstc *pFStc);

/*** */
/**** open existing file */
ST_EXPORT Tst_err
st_sysFStc_openExisting(Tst_sys_fstc *pFStc,
                        const Tst_bool allowSymAndHardLinks,
                        const Tst_bool allowWriting);
/**** create and open new file */
ST_EXPORT Tst_err
st_sysFStc_openNew(Tst_sys_fstc *pFStc);

ST_EXPORT Tst_err
st_sysFStc_openNewVirtual(Tst_sys_fstc *pFStc);
/**** change read/write permissions for handle */
ST_EXPORT Tst_err
st_sysFStc_changeMode(Tst_sys_fstc *pFStc, const Tst_bool allowWriting);
/**** close file */
ST_EXPORT Tst_err
st_sysFStc_close(Tst_sys_fstc *pFStc);

/*** */
ST_EXPORT Tst_fsize
st_sysFStc_getFileSz(Tst_sys_fstc *pFStc, ST_OPTARG(Tst_bool *pErrTooBig));

ST_EXPORT Tst_bool
st_sysFStc_isEOF(Tst_sys_fstc *pFStc);

ST_EXPORT Tst_foffs
st_sysFStc_getFPos(Tst_sys_fstc *pFStc);

ST_EXPORT Tst_bool
st_sysFStc_setFPos(Tst_sys_fstc *pFStc,
                   const Tst_foffs offs,
                   const Tst_sys_setFilePos fromWhere);

ST_EXPORT Tst_fsize
st_sysFStc_skipBytes(Tst_sys_fstc *pFStc,
                     const Tst_fsize skipFS);

ST_EXPORT void
st_sysFStc_flush(Tst_sys_fstc *pFStc);

/*** */
ST_EXPORT Tst_err
st_sysFStc_copyFromAndToFile(const Tst_bool pretendWriting,
                             Tst_sys_fstc *pFStcIn,
                             Tst_sys_fstc *pFStcOut,
                             const Tst_fsize maxCopyFS,
                             ST_OPTARG(Tst_fsize *pCopiedFS));
ST_EXPORT Tst_err
st_sysFStc_writeBuf(Tst_sys_fstc *pFStcOut,
                    const void *pBuf,
                    const Tst_uint32 bufSz);
ST_EXPORT Tst_err
st_sysFStc_readBuf(Tst_sys_fstc *pFStcIn,
                   const Tst_uint32 maxCopy,
                   void *pBufOut,
                   Tst_uint32 *pCopied);


/*
// Functions -- sys_file.c
*/
/** Structs */
ST_EXPORT Tst_err
st_sys_stc_initFStc(Tst_sys_fstc *pFStc);

ST_EXPORT void
st_sys_stc_rsetFStc(Tst_sys_fstc *pFStc, const Tst_bool resetFilen);

ST_EXPORT void
st_sys_stc_freeFStc(Tst_sys_fstc *pFStc);


ST_END_C_DECLS

#endif  /* HEAD_SYS_FILE_H */

/******************************************************************************/
