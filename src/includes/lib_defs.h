/******************************************************************************/
/* lib_defs.h               [Sandy Tagger | Common Lib]                  tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Global definitions                                                         */
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

#ifndef HEAD_LIB_DEFS_H
#define HEAD_LIB_DEFS_H

/*
// Own-Includes
*/
#include "lib_version.h"
#if !defined(ST_LIBSANTAG_VERS_STRING)
	#error No library version defined
#endif

/*
// System-Includes
*/
#include <stdio.h>      /* FILE, printf(), stdin, ... */
#include <stdint.h>     /* uint8_t, uint64_t, ... */
#include <inttypes.h>   /* PRIu64, PRId64, PRIx64 */
#include <sys/types.h>  /* off_t */
#if (CONFIG_ST_ALL_DEBUG == 1)
	#include <assert.h>
#endif


/* directive for inlining functions */
#ifndef ST_INLINE_S
	#define ST_INLINE_S  /* empty */
#endif
#define ST_INLINE_H  /* empty */

/* ST_BEGIN/END_C_DECLS is used so that C++ compilers
 *   don't mangle their names  */
#undef ST_BEGIN_C_DECLS
#undef ST_END_C_DECLS
#ifdef __cplusplus
	#define ST_BEGIN_C_DECLS  extern "C" {
	#define ST_END_C_DECLS    }
#else
	#define ST_BEGIN_C_DECLS  /* empty */
	#define ST_END_C_DECLS    /* empty */
#endif

/* ST_PARAMS is a macro used to wrap function prototypes, so that
 * compilers that don't understand ANSI C prototypes still work,
 * and ANSI C compilers can issue warnings about type mismatches */
#undef ST_PARAMS
#if defined (__STDC__) || defined (_AIX) || \
			(defined (__mips) && defined (_SYSTYPE_SVR4)) || \
			defined(_WIN32) || defined (__CYGWIN__) || \
			defined(__cplusplus)
	#define ST_PARAMS(protos)  protos
#else
	#define ST_PARAMS(protos)  ()
#endif

/**
 * ST_OPTARG is a macro used to wrap optional function parameters
 * just to make it clear that those parameters are optional
 */
#define ST_OPTARG(protos)  /*optional:*/protos

/* ST_EXPORT is used to declare functions etc. in the library
 * as accessible from the outside */
#if defined(__GNUC__)
	#define ST_EXPORT __attribute__ ((visibility ("default")))
#elif defined(_WIN32) || defined (__CYGWIN__)
	#define ST_EXPORT __declspec(dllexport)
#else
	#define ST_EXPORT  /* empty */
#endif

/*#if !defined(__LITTLE_ENDIAN__) &&
			( defined(WORDS_BIGENDIAN) || defined(__BIG_ENDIAN__) )*/


ST_BEGIN_C_DECLS

/*
// Constants, Part I
*/
/** Library name and version */
#define ST_LIBSANTAG_NAME  "libsantag"

/** Error Codes */
typedef enum {
	ST_ERR_SUCC =  0,  /* success */
	ST_ERR_QUIT =  1,  /* everything's fine, but quit now */
	ST_ERR_ABRT =  2,  /* abort -error message has already been printed- */
	ST_ERR_FAIL =  3,  /* (file) error */
	ST_ERR_OMEM =  4,  /* out of mem */
	ST_ERR_IARG =  5,  /* invalid argument */
	ST_ERR_FEOF =  6,  /* end of file */
	ST_ERR_FCRD =  7,  /* can't read from file */
	ST_ERR_FCWR =  8,  /* can't write to file */
	ST_ERR_FCOF =  9,  /* can't open file */
	ST_ERR_FCCF = 10,  /* can't create file */
	/* */
	ST_ERR_IDAT = 20,  /* invalid data found */
	ST_ERR_CNTH = 21,  /* can't handle data in tag or tag field */
	ST_ERR_PIFM = 22,  /* invalid picture format */
	ST_ERR_PITP = 23,  /* invalid picture type   */
	ST_ERR_TFNF = 24,  /* field not found */
	/* */
	ST_ERR_MIHD = 40,  /* MPEG streams: invalid MPEG header */
	ST_ERR_MICO = 41,  /* MPEG streams: invalid brate-ver-lay combo */
	ST_ERR_MIBI = 42,  /* MPEG streams: invalid BitrateIndex */
	ST_ERR_MISF = 43,  /* MPEG streams: invalid sampl. frequ. */
	/* */
	ST_ERR_OCFF = 60,  /* OGG streams: can't find next frame */
	ST_ERR_OCRH = 61,  /* OGG streams: can't read frameheader */
	ST_ERR_OCRD = 62,  /* OGG streams: can't read framedata */
	ST_ERR_OSTP = 63,  /* OGG streams: stop reading/parsing */
	/* */
	ST_ERR_DLLR = 80   /* Dynamic list: limit reached */
} Tst_err;

/** verbosity levels */
enum {
	ST_VL_GEN    = 0x0001,  /* print general info */
	ST_VL_OPTS   = 0x0002,  /* print cmdline options */
	ST_VL_TVREAD = 0x0004,  /* print read tag field values */
	ST_VL_TVSET  = 0x0008,  /* print set tag field vals */
	ST_VL_TVWRIT = 0x0010,  /* print written tag field vals */
	ST_VL_TAG    = 0x0020,  /* print tag info */
	ST_VL_AUD    = 0x0040,  /* print audio info */
	ST_VL_AUDDET = 0x0080   /* print detailed audio info */
};

/** charset, some chars in ISO-8859-1 encoding */
enum {
	ST_CSET_ISO_LF     = 0x0a,  /* '\n' */
	ST_CSET_ISO_CR     = 0x0d,  /* '\r' */
	ST_CSET_ISO_TAB    = 0x09,  /* '\t' */
	ST_CSET_ISO_SPA    = 0x20,  /* ' '  */
	ST_CSET_ISO_SLA    = 0x2f,  /* '/'  */
	ST_CSET_ISO_PARTH1 = 0x28,  /* '('  */
	ST_CSET_ISO_PARTH2 = 0x29,  /* ')'  */
	ST_CSET_ISO_PRNSMA = 0x20,  /* smallest printable char: ' ' */
	ST_CSET_ISO_PRNBIG = 0x7e,  /* biggest      --- " --- : '~' */
	ST_CSET_ISO_ACAP   = 0x41,  /* 'A'  */
	ST_CSET_ISO_ZCAP   = 0x5a,  /* 'Z'  */
	ST_CSET_ISO_ASMA   = 0x61,  /* 'a'  */
	ST_CSET_ISO_ZSMA   = 0x7a,  /* 'z'  */
	ST_CSET_ISO_DIG0   = 0x30,  /* '0'  */
	ST_CSET_ISO_DIG9   = 0x39,  /* '9'  */
	ST_CSET_ISO_BRA1B  = 0x5b,  /* '['  */
	ST_CSET_ISO_BRA1E  = 0x5d,  /* ']'  */
	ST_CSET_ISO_BRA2B  = 0x3c,  /* '<'  */
	ST_CSET_ISO_BRA2E  = 0x3e,  /* '>'  */
	ST_CSET_ISO_BRA3B  = 0x7b,  /* '{'  */
	ST_CSET_ISO_BRA3E  = 0x7d,  /* '}'  */
	/* german special chars: umlauts and sharp-s */
	ST_CSET_ISO_UMAEC  = 0xc4,  /* &Auml; */
	ST_CSET_ISO_UMOEC  = 0xd6,  /* &Ouml; */
	ST_CSET_ISO_UMUEC  = 0xdc,  /* &Uuml; */
	ST_CSET_ISO_UMAES  = 0xe4,  /* &auml; */
	ST_CSET_ISO_UMOES  = 0xf6,  /* &ouml; */
	ST_CSET_ISO_UMUES  = 0xfc,  /* &uuml; */
	ST_CSET_ISO_UMSZ   = 0xdf,  /* &szlig; */
	/* scandinavian special chars: umlauts */
	ST_CSET_ISO_UMAE2C = 0xc6,  /* Ae */
	ST_CSET_ISO_UMOE2C = 0xd8,  /* O-stroke */
	ST_CSET_ISO_UMAAC  = 0xc5,  /* A-ring */
	ST_CSET_ISO_UMAE2S = 0xe6,  /* ae */
	ST_CSET_ISO_UMOE2S = 0xf8,  /* o-stroke */
	ST_CSET_ISO_UMAAS  = 0xe5,  /* a-ring */
	/* spanish special chars */
	ST_CSET_ISO_NTILDC = 0xd1,  /* N-tilde */
	ST_CSET_ISO_OACUC  = 0xd3,  /* O-acute */
	ST_CSET_ISO_UACUC  = 0xda,  /* U-acute */
	ST_CSET_ISO_AACUC  = 0xc1,  /* A-acute */
	ST_CSET_ISO_EACUC  = 0xc9,  /* E-acute */
	ST_CSET_ISO_IACUC  = 0xcd,  /* I-acute */
	ST_CSET_ISO_YACUC  = 0xdd,  /* Y-acute */
	ST_CSET_ISO_NTILDS = 0xf1,  /* n-tilde */
	ST_CSET_ISO_OACUS  = 0xf3,  /* o-acute */
	ST_CSET_ISO_UACUS  = 0xfa,  /* u-acute */
	ST_CSET_ISO_AACUS  = 0xe1,  /* a-acute */
	ST_CSET_ISO_EACUS  = 0xe9,  /* e-acute */
	ST_CSET_ISO_IACUS  = 0xed,  /* i-acute */
	ST_CSET_ISO_YACUS  = 0xfd,  /* y-acute */
	ST_CSET_ISO_IEXCM  = 0xa1,  /* inverted exclamation mark */
	ST_CSET_ISO_IQUEM  = 0xbf,  /* inverted question mark */
	/* other special chars */
	ST_CSET_ISO_COPY   = 0xa9,  /* copyright */
	ST_CSET_ISO_REG    = 0xae   /* registered */
};


/*
// Types, Part I, the basic datatypes for everything
*/
/** */
typedef uint8_t      Tst_str;      /* (8bits) for strings */
typedef uint8_t      Tst_byte;     /* (8bits) */
typedef uint8_t      Tst_uint8;    /* (8bits) */
typedef int8_t       Tst_int8;     /* (8bits) */
typedef uint16_t     Tst_uint16;   /* (16bits) */
typedef int16_t      Tst_int16;    /* (16bits) */
typedef uint32_t     Tst_uint32;   /* (32bits) */
typedef int32_t      Tst_int32;    /* (32bits) */
#if (CONFIG_ST_ALL_HAVE64BIT == 1)
typedef uint64_t     Tst_uint64_native;  /* (64bits) */
typedef int64_t      Tst_int64_native;   /* (64bits) */
#endif
/** Tst_uint64 + Tst_int64 are in sys_math-defs.h */
#define Tst_buf      Tst_byte      /* (8bits) for buffers */
/** Tst_foffs + Tst_fsize */
/*** (32/64bits) for offsets in files, etc. */
#if (HAVE_FSEEKO == 1)
	#if (CONFIG_ST_ALL_NATIVE64BIT == 1)
		#define Tst_foffs  long int
	#else
		#define Tst_foffs  long long int
	#endif
#else
	#define Tst_foffs  long int
#endif
/*** (64bits) for filesizes */
#if (HAVE_FSEEKO == 1)
	#if (CONFIG_ST_ALL_NATIVE64BIT == 1)
		#define Tst_fsize  unsigned long int
	#else
		#define Tst_fsize  unsigned long long int
	#endif
#else
	#define Tst_fsize  unsigned long int
#endif
/** Tst_bool */
#ifdef __cplusplus
	typedef bool Tst_bool;
	#define ST_B_FALSE  false
	#define ST_B_TRUE   true
#else
	typedef enum {
		ST_B_FALSE = 0,  /* as I always say: "Zero is nothing,    */
		ST_B_TRUE  = 1   /*                   One is everything"  */
	} Tst_bool;
#endif


/*
// Types, Part II
*/
/** callbacks */
/*** callback types for debug/error messages */
typedef void (*Tst_cb_deb_dbg)(
		const int depth,
		const char *pFnc,
		const char *pMsg
	);
typedef void (*Tst_cb_deb_err)(
		const int depth,
		const char* pFnc,
		const Tst_str *pFn,
		const char *pMsg
	);
/*** */
typedef struct {
	Tst_cb_deb_dbg cbDbgFnc;  /* for debug messages w/ function name */
	Tst_cb_deb_err cbErrFnc;  /* for error messages w/ function name */
} Tst_callbacks_dbg;
/*** callback type for analize-mode */
typedef void (*Tst_cb_ana_msg)(const char *pFmt, ...);

/** basic options */
typedef struct {
	Tst_int32 verb;          /* verbosity level */
	Tst_bool  pretWr;        /* pretend writing ? */
	Tst_bool  allowLnkInpF;  /* allow sym-/hardlinks as input files ? */
	/* */
	Tst_callbacks_dbg cbsDbg;  /* callbacks for debug/error messages */
} Tst_basOpts;


/*
// Constants, Part II
*/
/** max. size for debugging messages */
#define ST_DEBMSG_MAXSZ  16384

/** Suffixes for integer constants */
#if (CONFIG_ST_ALL_HAVE64BIT == 1)
	#define ST_UI64_CONST_SUFX(mac_libdefs_ui64)  mac_libdefs_ui64 ## UL
	#define ST_SI64_CONST_SUFX(mac_libdefs_si64)  mac_libdefs_si64 ## L
#elif defined(__cplusplus)
	#define ST_UI64_CONST_SUFX(mac_libdefs_ui64)  mac_libdefs_ui64 ## ULL
	#define ST_SI64_CONST_SUFX(mac_libdefs_si64)  mac_libdefs_si64 ## LL
#else
	#define ST_UI64_CONST_SUFX(mac_libdefs_ui64)  mac_libdefs_ui64 ## ULL
	#define ST_SI64_CONST_SUFX(mac_libdefs_si64)  mac_libdefs_si64 ## LL
#endif

/** min./max. values */
/*** Tst_int16 */
#define ST_TINT16_MIN  (-ST_TINT16_MAX - 1)
#define ST_TINT16_MAX  32767
/*** Tst_uint16 */
#define ST_TUINT16_MIN  0
#define ST_TUINT16_MAX  65535
/*** Tst_int32 */
#define ST_TINT32_MIN  (-ST_TINT32_MAX - 1)
#define ST_TINT32_MAX  2147483647
/*** Tst_uint32 */
#define ST_TUINT32_MIN  0
#define ST_TUINT32_MAX  4294967295
/*** Tst_foffs */
#if (HAVE_FSEEKO == 1)
	#define ST_TFOFFS_MAX  ((Tst_foffs)ST_SI64_CONST_SUFX(9223372036854775807))
#else
	#define ST_TFOFFS_MAX  ((Tst_foffs)ST_TINT32_MAX)
#endif
#define ST_TFOFFS_MIN  ((Tst_foffs)(-ST_TFOFFS_MAX - 1))
/*** Tst_fsize */
#if (HAVE_FSEEKO == 1)
	#define ST_TFSIZE_MAX  ((Tst_fsize)ST_UI64_CONST_SUFX(18446744073709551615))
#else
	#define ST_TFSIZE_MAX  ((Tst_fsize)ST_TUINT32_MAX)
#endif
#define ST_TFSIZE_MIN  0

/** for printf & co. */
/*** 64bit integer */
#ifndef PRIu64
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		#define PRIu64  "llu"
	#elif defined(__cplusplus)
		#define PRIu64  "llu"
	#else
		#define PRIu64  "llu"
	#endif
#endif
#ifndef PRId64
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		#define PRId64  "lld"
	#elif defined(__cplusplus)
		#define PRId64  "lld"
	#else
		#define PRId64  "lld"
	#endif
#endif
#ifndef PRIx64
	#if (CONFIG_ST_ALL_HAVE64BIT == 1)
		#define PRIx64  "llx"
	#elif defined(__cplusplus)
		#define PRIx64  "llx"
	#else
		#define PRIx64  "llx"
	#endif
#endif
#define ST_UI64_PRF_LU   "%"PRIu64
#define ST_UI64_PRF_0X   "0x%016"PRIx64
#define ST_SI64_PRF_LD   "%"PRId64
#define ST_SI64_PRF_PLD  "%+"PRId64
#define ST_SI64_PRF_0X   ST_UI64_PRF_0X
/*** Tst_foffs */
#if (HAVE_FSEEKO == 1)
	#define ST_TFOFFS_PRF_0X   ST_SI64_PRF_0X
	#define ST_TFOFFS_PRF_LD   ST_SI64_PRF_LD
	#define ST_TFOFFS_PRF_PLD  ST_SI64_PRF_PLD
#else
	#define ST_TFOFFS_PRF_0X   "0x%08x"
	#define ST_TFOFFS_PRF_LD   "%ld"
	#define ST_TFOFFS_PRF_PLD  "%+ld"
#endif
/*** Tst_fsize */
#if (HAVE_FSEEKO == 1)
	#define ST_TFSIZE_PRF_0X  ST_UI64_PRF_0X
	#define ST_TFSIZE_PRF_LU  ST_UI64_PRF_LU
#else
	#define ST_TFSIZE_PRF_0X  "0x%08lx"
	#define ST_TFSIZE_PRF_LU  "%u"
#endif

/** max filesize */
#if (HAVE_FSEEKO == 1)
	#define ST_FILESIZE_MAX  \
				(ST_TFSIZE_MAX & ST_UI64_CONST_SUFX(0x7ffffffffff00000))
#else
	#define ST_FILESIZE_MAX  (ST_TFSIZE_MAX & 0x7ff00000U)
#endif


/*
// Macros
*/
/** alloc mem */
#define ST_CALLOC(mac_libdefs_pDest, mac_libdefs_type, mac_libdefs_cnt, mac_libdefs_size)  { \
			/**fprintf(stderr, "\n__calloc__ '%s':'%s':%d  size %u\n", \
					__FILE__, __func__, __LINE__, \
					(Tst_uint32)(mac_libdefs_cnt * mac_libdefs_size));**/ \
			(mac_libdefs_pDest) = (mac_libdefs_type)calloc((mac_libdefs_cnt), (mac_libdefs_size)); \
		}
#define ST_REALLOC(mac_libdefs_pDest, mac_libdefs_type, mac_libdefs_cnt, mac_libdefs_size)  { \
			/**fprintf(stderr, "\n__realloc__ '%s':'%s':%d  size %u\n", \
					__FILE__, __func__, __LINE__, \
					(Tst_uint32)(mac_libdefs_cnt * mac_libdefs_size));**/ \
			void *pMacTempPtrXxx = (mac_libdefs_type)realloc((mac_libdefs_pDest), \
						(mac_libdefs_cnt) * (mac_libdefs_size)); \
			if (pMacTempPtrXxx == NULL) { \
				free(mac_libdefs_pDest); \
				(mac_libdefs_pDest) = NULL; \
			} else { \
				(mac_libdefs_pDest) = pMacTempPtrXxx; \
			} \
		}

/** free pointers */
#define ST_DELPOINT(mac_libdefs_somePointer)  { \
			if (mac_libdefs_somePointer) { \
				free(mac_libdefs_somePointer); (mac_libdefs_somePointer) = NULL; \
			} \
		}

/** check whether some specific verbosity level is activated */
#define ST_ISVERB(mac_libdefs_verbLevel, mac_libdefs_verbLevelCheck)  \
				(((mac_libdefs_verbLevel) & (mac_libdefs_verbLevelCheck)) == \
						(mac_libdefs_verbLevelCheck))

/** assertion */
#if (CONFIG_ST_ALL_DEBUG == 1)
	#define ST_ASSERTN_IARG(mac_libdefs_assConds)  { \
				if (mac_libdefs_assConds) { \
					assert(! (mac_libdefs_assConds)); return ST_ERR_IARG; \
				} \
			}
	#define ST_ASSERTN_FAIL(mac_libdefs_assConds)  { \
				if (mac_libdefs_assConds) { \
					assert(! (mac_libdefs_assConds)); return ST_ERR_FAIL; \
				} \
			}
	#define ST_ASSERTN_VOID(mac_libdefs_assConds)  { \
				if (mac_libdefs_assConds) { \
					assert(! (mac_libdefs_assConds)); return; \
				} \
			}
	#define ST_ASSERTN_NUM(mac_libdefs_assNum, mac_libdefs_assConds)  { \
				if (mac_libdefs_assConds) { \
					assert(! (mac_libdefs_assConds)); return (mac_libdefs_assNum); \
				} \
			}
	#define ST_ASSERTN_BOOL(mac_libdefs_assBoo, mac_libdefs_assConds)  { \
				if (mac_libdefs_assConds) { \
					assert(! (mac_libdefs_assConds)); return (mac_libdefs_assBoo); \
				} \
			}
	#define ST_ASSERTN_NULL(mac_libdefs_assConds)  { \
				if (mac_libdefs_assConds) { \
					assert(! (mac_libdefs_assConds)); return NULL; \
				} \
			}
#else
	#define ST_ASSERTN_IARG(mac_libdefs_assConds)                      /* empty */
	#define ST_ASSERTN_FAIL(mac_libdefs_assConds)                      /* empty */
	#define ST_ASSERTN_VOID(mac_libdefs_assConds)                      /* empty */
	#define ST_ASSERTN_NUM(mac_libdefs_assNum, mac_libdefs_assConds)   /* empty */
	#define ST_ASSERTN_BOOL(mac_libdefs_assBoo, mac_libdefs_assConds)  /* empty */
	#define ST_ASSERTN_NULL(mac_libdefs_assConds)                      /* empty */
#endif

/** Tst_callbacks_dbg */
#define ST_RSETSTC_CBSDBG(mac_libdefs_cbs)  { \
			(mac_libdefs_cbs).cbDbgFnc = NULL; \
			(mac_libdefs_cbs).cbErrFnc = NULL; \
		}
#define ST_COPYSTC_CBSDBG(mac_libdefs_cbsSrc, mac_libdefs_cbsDst)  { \
			(mac_libdefs_cbsDst).cbDbgFnc = (mac_libdefs_cbsSrc).cbDbgFnc; \
			(mac_libdefs_cbsDst).cbErrFnc = (mac_libdefs_cbsSrc).cbErrFnc; \
		}

/** Tst_basOpts */
#define ST_RSETSTC_BASOPTS(mac_libdefs_bopt)  { \
			(mac_libdefs_bopt).verb         = 0; \
			(mac_libdefs_bopt).pretWr       = ST_B_TRUE; \
			(mac_libdefs_bopt).allowLnkInpF = ST_B_FALSE; \
			ST_RSETSTC_CBSDBG((mac_libdefs_bopt).cbsDbg) \
		}
#define ST_COPYSTC_BASOPTS(mac_libdefs_boptSrc, mac_libdefs_boptDst)  { \
			(mac_libdefs_boptDst).verb         = (mac_libdefs_boptSrc).verb; \
			(mac_libdefs_boptDst).pretWr       = (mac_libdefs_boptSrc).pretWr; \
			(mac_libdefs_boptDst).allowLnkInpF = (mac_libdefs_boptSrc).allowLnkInpF; \
			ST_COPYSTC_CBSDBG((mac_libdefs_boptSrc).cbsDbg, (mac_libdefs_boptDst).cbsDbg) \
		}

/** */
/*define ST_GEN_DEBUG_LEVEL(mac_libdefs_verbLevel, ...)  \
			debug_log ((mac_libdefs_verbLevel), \
					__FILE__, __func__, __LINE__, __VA_ARGS__)*/

ST_END_C_DECLS

#endif  /* HEAD_LIB_DEFS_H */

/******************************************************************************/
