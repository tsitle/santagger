/******************************************************************************/
/* utils_fmt-priv.h             [Sandy Tagger | Lib]                     tw=4 */
/*                                                                            */
/* (C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)                */
/*                                                                            */
/* Private definitions for determining file formats etc.                      */
/******************************************************************************/
/*                                                                            */
/* This program is free software; you can redistribute it and/or              */
/* modify it under the terms of the GNU General Public License as             */
/* published by the Free Software Foundation; either version 3 of             */
/* the License, or (at your option) any later version.                        */
/*                                                                            */
/******************************************************************************/
/* Creation date: 08.01.2010 (dd.mm.yyyy)                                     */
/******************************************************************************/

#ifndef LIBSANTAGGER_UTILS_FMT_PRIV_H
#define LIBSANTAGGER_UTILS_FMT_PRIV_H


ST_BEGIN_C_DECLS

/*
// Types (private)
*/
#ifdef SRC_UTILS_FMT_C
	/** for Picture formats */
	typedef struct {
		const char                *pFExt;  /* default file-extension (e.g. "jpg") */
		const Tst_utilsFmt_mimeTp mimTp;   /* mime type */
		const Tst_utilsFmt_picFmt pfmE;    /* enum value */
	} Tst_utilsFmt_picFmt_arrElem;

	/** for MIME types */
	typedef struct {
		const char     *pFExt1;  /* default file-extension (e.g. "jpg") */
		const char     *pFExt2;  /* altern. file-extension (e.g. "jpe")  */
		const char     *pFExt3;  /* altern. file-extension (e.g. "jpeg")  */
		const char     *pMime1;  /* default mime-type (e.g. "img/jpg") */
		const char     *pMime2;  /* altern. mime-type (e.g. "img/x-jpe") */
		const char     *pMime3;  /* altern. mime-type (e.g. "img/jpeg") */
		const Tst_bool isCompr;  /* is a file with this MIME-type compressable ? */
		const Tst_utilsFmt_mimeTp mimTp;   /* enum value */
	} Tst_utilsFmt_mime_arrElem;
#endif  /* private */


/*
// Constants (private)
*/
#ifdef SRC_UTILS_FMT_C
	/** ID3v2 Picture formats */
	const Tst_utilsFmt_picFmt_arrElem ST_UTILSFMT_PICFORMATS[] = {
			{"jpg", ST_UTILSFMT_MTP_PICJPG, ST_UTILSFMT_PF_JPG},
			{"png", ST_UTILSFMT_MTP_PICPNG, ST_UTILSFMT_PF_PNG},
			{"tif", ST_UTILSFMT_MTP_PICTIF, ST_UTILSFMT_PF_TIF},
			{"gif", ST_UTILSFMT_MTP_PICGIF, ST_UTILSFMT_PF_GIF},
			{"psd", ST_UTILSFMT_MTP_PICPSD, ST_UTILSFMT_PF_PSD},
			{"ico", ST_UTILSFMT_MTP_PICICO, ST_UTILSFMT_PF_ICO},
			{"bmp", ST_UTILSFMT_MTP_PICBMP, ST_UTILSFMT_PF_BMP},
			{NULL, ST_UTILSFMT_MTP_NONE, ST_UTILSFMT_PF_NONE} };

	/** some MIME types */
	const Tst_utilsFmt_mime_arrElem ST_UTILSFMT_MIMETYPES[] = {
			{"flac",  NULL,  NULL,   "audio/flac",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDFLAC},
			{"mp3",   "mp2", "mp1",  "audio/mpeg",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDMPEG1},
			/* MPEG-4: m4a=audio-only
			 *         m4p=audio encrypted (iTunes-store)
			 *         m4b=audio book / podcast
			 *         m4r=ringtone  */
			{"m4a",   "m4p",  "m4r", "audio/mpeg",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDMPEG4},
			{"m3u",   NULL,  NULL,   "audio/mpegurl",
					"audio/x-mpegurl",
					NULL, ST_B_TRUE, ST_UTILSFMT_MTP_AUDMPEGURL},
			{"oga",   NULL, NULL,    "audio/ogg",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDOGG},
			{"spx",   NULL,  NULL,   "audio/ogg",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDOGG},
			{"wma",   NULL,  NULL,   "audio/x-ms-wma",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDWMA},
			{"wav",   NULL,  NULL,   "audio/x-wav",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDWAV},
			{"aiff",  NULL,  NULL,   "audio/x-aiff",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDAIFF},
			/** */
			{"3gp",   NULL,  NULL,   "video/3gpp",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_VID3GP},
			{"mpg",   "mpeg", "mpe", "video/mpeg",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_VIDMPEG},
			{"mp4",   "m4v",  NULL,   "video/mp4",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_VIDMPEG4},
			{"qt",   "mov",  NULL,   "video/quicktime",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_VIDQT},
			{"ogv",  NULL,   NULL,   "video/ogg",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_VIDOGG},
			{"avi",   NULL,  NULL,   "video/x-msvideo",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_VIDAVI},
			{"mpv",   "mkv", NULL,   "video/x-matroska",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_VIDMKV},
			/** */
			{"ogg",   NULL,  NULL,   "audio/ogg",
					"video/ogg",  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_AUDVIDOGG},
			/** */
			{"jpg",   "jpe", "jpeg", "image/jpg",
					"image/jpeg", "image/pjpeg", ST_B_FALSE, ST_UTILSFMT_MTP_PICJPG},
			{"png",   NULL,  NULL,   "image/png",
					"image/x-png", NULL, ST_B_FALSE, ST_UTILSFMT_MTP_PICPNG},
			{"tif",   "tiff", NULL,  "image/tif",
					"image/tiff",  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_PICTIF},
			{"gif",   NULL,  NULL,   "image/gif",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_PICGIF},
			{"psd",   NULL,  NULL,   "image/x-photoshop",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_PICPSD},
			{"ico",   NULL,  NULL,   "image/x-icon",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_PICICO},
			{"bmp",   NULL,  NULL,   "image/x-ms-bmp",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_PICBMP},
			/** */
			{"bin",   "raw", "xxx",  "application/octet-stream",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPOCTET},
			{"doc",   "dot", NULL,   "application/msword",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPWORD},
			{"ogx",   NULL,  NULL,   "application/ogg",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPOGG},
			{"pdf",   NULL,  NULL,   "application/pdf",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPPDF},
			{"key",   NULL,  NULL,   "application/pgp-keys",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPPGPK},
			{"pgp",   NULL,  NULL,   "application/pgp-signature",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPPGPS},
			{"ps",    NULL,  NULL,   "application/postscript",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPPS},
			{"rtf",   NULL,  NULL,   "application/rtf",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPRTF},
			{"xhtml", "xht", NULL,   "application/xhtml+xml",
					NULL, NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPXHTML},
			{"xml",   NULL,  NULL,   "application/xml",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPXML},
			{"odt",   NULL,  NULL,   "application/vnd.oasis.opendocument.text",
					NULL, NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPODT},
			{"sdw",   NULL,  NULL,   "application/vnd.stardivision.writer",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPSDW},
			{"torrent", NULL, NULL,  "application/x-bittorrent",
					NULL, NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPTORR},
			{"tag",   NULL,  NULL,   "application/x-tagfile",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPTAG},
			{"tiv1",  NULL,  NULL,   "application/x-tagfile",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPTAGIV1},
			{"tiv2",  NULL,  NULL,   "application/x-tagfile",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPTAGIV2},
			{"tav2",  NULL,  NULL,   "application/x-tagfile",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPTAGAV2},
			{"tvor",  NULL,  NULL,   "application/x-tagfile",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPTAGVOR},
			/** */
			{"7z",    NULL,  NULL,   "application/x-7z-compressed",
					NULL, NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APP7Z},
			{"dmg",   NULL,  NULL,   "application/x-apple-diskimage",
					NULL, NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPDMG},
			{"gtar",  "tgz", NULL,   "application/x-gtar",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPGTAR},
			{"gz",    NULL,  NULL,   "application/gzip",
					"application/x-gzip-compressed", NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPGZ},
			{"rar",   NULL,  NULL,   "application/rar",
					NULL,  NULL, ST_B_FALSE, ST_UTILSFMT_MTP_APPRAR},
			{"tar",   NULL,  NULL,   "application/x-tar",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_APPTAR},
			{"zip",   NULL,  NULL,   "application/zip",
					"application/x-zip",
					"application/x-zip-compressed", ST_B_FALSE, ST_UTILSFMT_MTP_APPZIP},
			/** */
			{"html",  "htm", NULL,   "text/html",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_TXTHTML},
			{"txt",   "asc", NULL,   "text/plain",
					NULL,  NULL, ST_B_TRUE, ST_UTILSFMT_MTP_TXTPLAIN},
			{NULL, NULL, NULL, NULL, NULL, NULL, ST_B_FALSE, ST_UTILSFMT_MTP_NONE} };
#endif  /* private */


ST_END_C_DECLS

#endif  /* LIBSANTAGGER_UTILS_FMT_PRIV_H */

/******************************************************************************/
