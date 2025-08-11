# Changelog

## 2025 Aug v1.0

- fixed MS Windows compatibility
- improved ST_REALLOC()
- fixed ST_TIV2__sync_doUnsync()
- added Unit Tests for sys_fnc and sys_file


## 2025 Aug v1.0-alpha

- improved st_sysRenameFile()
- fixed ST_SYSFILE__getTmpFilename()
- added Unit Tests from the old code base and updated them


## 2024 Sep v0.5-alpha5

- migrated the build system to CMake
- added support for Clang
- added support for semantic versioning
- fixed compiler warnings


## 2011 Jul v0.5-alpha4

- program parameters (there are now 34 commands and 27 options)  
  - splitted usage and help pages  
    program usage is now displayed when running santagger with  
    invalid parameters and help only when using "--help"  
  - clarified parameters description and extended  
    help pages for individual commands/options
  - added commands "--conv", "--fadd", "--fset", "--dset",  
    "--mod-pic" and "--mod-geo"
  - added options "--tiv2", "--qscan", "--(dis)allow-links",  
    "--pic-fn", "--geo-fn", "--od" and "--ow"
  - removed option "--tag-rd"
  - renamed option "--tag-wr" to "--tag-ed"
  - option "-q" has now a different behaviour: it now disables  
    all output except for error messages
  - option "-v" has now a different behaviour: when used then  
    some status info will be printed while processing files
  - extended output of command "--version"
  - changed format of argument for commands  
    "--track-nr" and "--disc-nr" from \<num\> to \<pos\>
  - modified format of argument for commands  
    "--rem-pic", "--rem-geo", "--extr-pic" and "--extr-geo"  
    to also allow \<numrange\>
  - updated output of command "--examples"
- library
  - improved st_apev2_tidyUpTag() and st_vorbc_tidyUpTag()  
    to ensure that no tag fields are empty, no duplicate fields  
    exist, and the TRACK(TOTAL) and DISC(TOTAL) fields contain  
    valid data, ...
  - added tag_fldmap.c & Co., which introduces Generic Tag Fields  
    that can be mapped to any tag type's fields
  - rewrote tag_conv.c, which now uses Generic Tag Fields for  
    converting tags
  - when writing ID3v2, APEv2 or Vorbis tags a tag field will  
    now be added that contains the name and version of the  
    tagging library.  
    This information gets displayed when using '-r' in combination  
    with '--tag-sw iv2,av2,vor'
  - added full support for custom tag fields in  
    ID3v2, APEv2 and Vorbis tags
- library interface  
  - added tag_fldmap.h and tag_fldmap-defs.h for mapping tag fields
- testsuite  
  - added test_tfldmap for testing the mapping of tag fields
- improved autotools stuff  
  - additional libraries like libvorbis are no longer  
    accidently linked into libsantag  
  - added ./configure options "--disable-vorbis",  
    "--disable-mpg123", "--disable-mad" and "--disable-zlib"
- Minor Bug fixes in  
  - ST_TID3__strcmp():  
      - differing string lengths were ignored
  - ast_cln_lst_parseBsList():  (now ast_cln_psvt_bslist())  
      - corrupt memory allocation
  - main():  
      - forgot to free some mem
      - when new files were created but ended up to be empty  
        and the "-p" option was used the empty file remained  
        in the filesystem
      - extraction of pictures/binary data from non-ID3v2 tags  
        didn't work due to a minor bug
- Major Bug fixes in  
  - st_utilsFmt_getMime_enum_byBuffer_tag():  
      - ID3v1 was not correctly detected when TITLE begins with '+' or 'E'
  - ST_MPEG1__readFrame():  
      - in files with or without CBR/VBR header in some cases the  
        last frame contains a ID3v1 tag.  
        In these cases the ID3v1 tag couldn't be recognized by the library
  - AST_MF__cm_updRawVorbTag():  
      - not embedded raw vorbis tags didn't get updated correctly


## 2011 Apr v0.5-alpha3

- program parameters  
  - added command "--version" and option "--disp-det"
  - changed behaviour of option "--disp-all"
- improved autotools stuff  
  - upon uninstallation the directory 'INCLUDEDIR/santag/'  
    gets entirely removed (in the previous version only  
    the header files got removed, but the empty directories  
    remained in the filesystem)
  - all library/binary version information is now  
    maintained by configure and stored in  
    'INCLUDEDIR/santag/lib-version.h' and  
    'src/santag/app-version.h'
  - added pkg-config support for libsantag
- updated header of all source files (\*.c and \*.h)
- Bug fixes in  
  - mf_show.c:  
      printf expressions (still) weren't supressed within  
      user-specific strings before passing them to  
      vnprintf() when using "--disp-fs"


## 2011 Mar v0.5-alpha2

- program parameters  
  - new behaviour of option "-v" and added option "--lev"
  - added option "-q" to suppress status info
  - added most common use-case examples to the beginning  
    of output of 'santagger --help'
- Mapping field "TEMPO" to "TBPM" (Vorbis-->ID3v2)
- changed default ID3v2 version to ID3v2.3 because  
  iTunes doesn't recognize the "TDRL" field correctly  
  in ID3v2.4 tags
- Bug fixes in  
  - st_mtes_copyToCharp_utf8():  
      integer underflow
  - ast_mf_ed_editFile():  
      automatic selection of tags to write didn't work
  - mf_show.c:  
      printf expressions weren't supressed within  
      user-specific strings before passing them to  
      vnprintf()


## 2011 Jan

Major changes:  
- 19/Jan/2011: added support for writing Flac streams
- 17/Jan/2011: added support for writing OGG streams
- 20/Apr/2010: added support for Vorbis Tag
- 08/Jan/2010: added support for ID3v2 Tag
- 07/Jan/2010: added support for APE Tag
- 28/Dez/2009: added support for MPEG files with VBR


## 2005 May

- beginning of the project

Unfortunately, the changes made between 2005 and 2011 could not be reconstructed.
