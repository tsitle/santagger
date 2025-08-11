# <a name="top"></a>Sandy Tagger User Manual

* * *

Index of Help Pages:

- [--help](#hp0)
- [--examples](#hp1)
- [--track-nr help](#hp2)
- [--disc-nr help](#hp3)
- [--genre help](#hp4)
- [--genre hv](#hp5)
- [--rem-pic help](#hp6)
- [--rem-geo help](#hp7)
- [--fadd help](#hp8)
- [--fadd hvIV2](#hp9)
- [--fadd hvAV2](#hp10)
- [--fadd hvVOR](#hp11)
- [--fset help](#hp12)
- [--fset hvIV2](#hp13)
- [--fset hvAV2](#hp14)
- [--fset hvVOR](#hp15)
- [--dset help](#hp16)
- [--dset hv](#hp17)
- [--rem-tag help](#hp18)
- [--extr-tag help](#hp19)
- [--extr-pic help](#hp20)
- [--extr-geo help](#hp21)
- [--conv help](#hp22)
- [--lev help](#hp23)
- [--ebs help](#hp24)
- [--tag-sw help](#hp25)
- [--tag-ed help](#hp26)
- [--tiv2 help](#hp27)
- [--pic-tp help](#hp28)
- [--pic-fm help](#hp29)

* * *

## <a name="hp0"></a>\[--help\]

[Back To Top](#top)

```
Usage: santagger [<commands>...] [<options>...] <file_names>...

To read Tags and Audio/Video streams:
  -r, --read                   Displays information about the file
                               (default command)

To analize Tags and Audio/Video streams:
  --ana                        Displays quite comprehensive info about
                               the condition of the file

To decode Audio streams:
  --dec wav|aiff               Decode to WAVE or AIFF
                               Works with the following input:
                                - Ogg-Flac and pure Flac
                                - Ogg-Vorbis
                                - MPEG-1 (Layer I-III, e.g. MP3)
                               See --bps option

To edit or add Tags:
  -b, --band=<str>             Bandname/Artist  (""==none)
  -a, --album=<str>            Albumname  (""==none)
  -s, --song=<str>             Songname/Title  (""==none)
  -t, --track-nr=help|<pos>    Track-Nr.  (0==none)
  --track-tot <num>            Tracks total  (0==none)
  -d, --disc-nr=help|<pos>     Disc-Nr.  (0==none)
  --disc-tot <num>             Discs total  (0==none)
  -y, --year=<num>             Year  (0==none)
  -c, --comment=<str>          Comment  (""==none)
  -g, --genre=help|<num|str>   Genre  (255 or ""==none)
  --add-pic <filen> [--pic-*]  Attach picture to ID3v2/Vorbis Tag
                               You can use this command more than once
                               See --pic-* options
  --mod-pic <num> [--pic-*]    Modify picture in ID3v2/Vorbis Tag
                               The special value 0 for <num> means
                               modify all pictures.
                               You can use this command more than once
                               See --pic-* options
  --rem-pic help|<numlist>     Remove picture(s) from ID3v2/Vorbis Tag
  --add-geo <filen> [--geo-*]  Attach any type of file to ID3v2/Ape Tag
                               You can use this command more than once
                               See --geo-* options
  --mod-geo <num> [--geo-*]    Modify attached file in ID3v2/Ape Tag
                               The special value 0 for <num> means
                               modify all attached files.
                               You can use this command more than once
                               See --geo-* options
  --rem-geo help|<numlist>     Remove attached file from ID3v2/Ape Tag
  --fadd help|<fldadd>         Add a tag field
                               You can use this command more than once
  --fset help|<fldset>         Set/Remove a tag field
                               You can use this command more than once
  --dset help|<dscset>         Set a ID3v2 tag field's description
                               You can use this command more than once

To rewrite files:
  --rewr                       Rewrites all tags and audio/video streams
                               found in the file and corrects some
                               common problems while doing so

To remove Tags:
  --rem-tag help|<taglist>     Removes all given tags from the file

To extract data:  (you may combine several --extr-* commands)
  --extr-tag help|<taglist>    Extract Tag(s)
  --extr-pic help|<numlist>    Extract picture(s) from Tags
  --extr-geo help|<numlist>    Extract GEO(s) from Tags
  --extr-bd                    Extract all binary data from Tags
                               fields (except for Pic/GEO)

To convert tags:
  --conv help|<tagpair>        Converts all given tag types into
                               the specified tag type

Special commands:
  --info                       Show general program info
  --version                    Show program version
  --license                    Show program's license
  --examples                   Show program usage examples
  -h, --help                   Show this help

Options:
  -q                           Don't output anything (but errors)
  -v                           Show some status info
  --lev help|<vlevlist>        Verbosity level
                               You can use this option more than once
  ****************************
  -p, --pretend                Don't modify input files
  ****************************
  --qscan                      Perform only a quick scan
  ****************************
  --allow-links                Allow sym-/hardlinks as input files
  --disallow-links             Disallow sym-/hardlinks as input files  [default]
  ****************************
  --disp-all                   Display all tag fields
  --disp-fs                    Display strings in tag fields in full length,
                               don't cut them off and don't replace
                               chars like \n with "[\n]".
                               Also displays binary data
  --disp-det                   Display details of tag fields
  --disp-iso                   Display strings in ISO-8859-1 encoding
                               (current setting)
  --disp-u8                    Display strings in Unicode UTF-8 encoding
  ****************************
  --tag-sw help|<taglist>      Tags to show  [default:merg]
  --tag-ed help|<taglist>      Tags to edit/write  [default:auto]
  ****************************
  --ebs help|<bslist>          Select bitstream(s) to edit  [default:all]
  ****************************
  For MPEG-1 audio streams:
  --force-mp1/2/3              Force MPEG-1 Layer I, II or III
  --force-av1/2/25             Force MPEG-1 Audio Version 1, 2 or 2.5
  ****************************
  For decoding audio streams:
  --bps 0|16|24|32             Bits per sample to write decoded samples with
                               [default:0]  (0 means let decoder decide)
                               (when decoding Flac this option is ignored)
  ****************************
  For ID3v2 Tags:
  --tiv2 help|<tiv2optlist>    Set some ID3v2 specific options
                               [default:v3,nocmpr,nounsy]
                               You can use this option more than once
  ****************************
  These options should be used after the corresponding
    --add-pic/geo or --mod-pic/geo:
  --pic-tp help|<num|pictp>    Type of picture to attach  [default:cov-front]
  --pic-fm help|<num|picfm>    Format of picture to attach  [default:auto]
  --pic-de <str>               Description of picture to attach
  --geo-mi <mime>              MIME-Type of file to attach
  --geo-de <str>               Description of file to attach
  ****************************
  These options should be used after the corresponding --mod-pic/geo:
  --pic-fn <filen>             File to exchange picture with
  --geo-fn <filen>             File to exchange attached file with
  ****************************
  --od <dirn>                  Directory to write new/modified files to
  --ow                         Overwrite existing files
                               when decoding or extracting data  [default:no]

```

* * *

## <a name="hp1"></a>\[--examples\]

[Back To Top](#top)

```
Usage examples:
  santagger *.mp3
    same as: santagger -r *.mp3
  santagger -r -v *.mp3
    same as: santagger -rv *.mp3
  santagger --disp-all --disp-fs --disp-det *.mp3
    same as: santagger --disp-a --disp-f --disp-d *.mp3
  santagger --genre help
    same as: santagger -g h
  santagger -b "My favorite band" -g 9 *.mp3
  santagger -b "Flux" -g "Prog-Conservative Prayers" *.mp3
  santagger -s "Brad Pitt Dancing" --track-nr 1 --track-tot 10 song01.mp3
  santagger -s "Angelina In Sight" --disc-tot 2 song02.mp3
  santagger --track-nr=3/10 --disc-nr=1/2 song03.mp3
    same as: santagger -t 3/10 -d 1/2 song03.mp3
    same as: santagger --track-nr 3 --track-tot 10 \
                       --disc-nr 1 --disc-tot 2 song03.mp3
  santagger --rem-tag id3v1 --pretend *.mp3
  santagger --rem-tag iv1,av2 *.mp3
  santagger -p --lev 3 --album Wonders *.mp3
  santagger -y 2005 --comment "the best" song01.mp3 song02.mp3
  santagger --band="My favorite band" --ebs 2 *.ogg
  santagger --ana song01.mp3 song02.mp3
  santagger --ana --lev tag --lev aud *.mp3
  santagger --lev tag,aud --extr-pic 1,3,7 --extr-tag all *.ogg *.flac
  santagger --extr-pic a --extr-bd *.ogg *.mp3
  santagger --force-mp3 --force-av1 *.mp3
  santagger --add-pic cov.jpg --pic-tp cov-back --pic-de "Japan Ed" *.mp3
  santagger --add-geo bio.xml --geo-de Biography --geo-mi "application/xml" \
            --add-geo lyr.pdf --geo-de Lyrics *.mp3
  santagger --dec wav --ow --od wavfiles/ *.flac
  santagger --dec aiff --bps 24 *.ogg
  santagger --conv id3v1,apev2-id3v2 --tiv2 v3 *.mp3
  santagger --fset TBPM=150 song_with_150bpm.ogg
```

* * *

## <a name="hp2"></a>\[--track-nr help\]

[Back To Top](#top)

```
Help for --track-nr <pos>:

Format of <pos> is "<num>[/[<num>]]" or "/<num>"

This command can be used to combine two commands:
  --track-nr and --track-tot
The special value '0' resets all appropriate tag fields.

Examples:
  --track-nr 0
    same as: --track-nr 0/
  --track-nr 0/0
    same as: --track-nr 0 --track-tot 0
  --track-nr 1
    same as: --track-nr 1/
  --track-nr 2/3
    same as: --track-nr 2 --track-tot 3
  --track-nr 0/4
    same as: --track-nr 0 --track-tot 4
  --track-nr /5
    same as: --track-tot 5
```

* * *

## <a name="hp3"></a>\[--disc-nr help\]

[Back To Top](#top)

```
Help for --disc-nr <pos>:

Format of <pos> is "<num>[/[<num>]]" or "/<num>"

This command can be used to combine two commands:
  --disc-nr and --disc-tot
The special value '0' resets all appropriate tag fields.

Examples:
  --disc-nr 0
    same as: --disc-nr 0/
  --disc-nr 0/0
    same as: --disc-nr 0 --disc-tot 0
  --disc-nr 1
    same as: --disc-nr 1/
  --disc-nr 2/3
    same as: --disc-nr 2 --disc-tot 3
  --disc-nr 0/4
    same as: --disc-nr 0 --disc-tot 4
  --disc-nr /5
    same as: --disc-tot 5
```

* * *

## <a name="hp4"></a>\[--genre help\]

[Back To Top](#top)

```
Help for --genre <num|str>:

Possible values are:
  -Num-   -Str-
     0     Blues
  (through)
   147     Synthpop
 (to see the entire list please use '--genre hv')
 For ID3v1 you should only use one of the above genres
 For ID3v2 you can also use one of these special genres:
   Cover
   Remix
 And for all tags except ID3v1 you may also specify your own genres
  (e.g. "Deep Throat")

 The special values 255 and "" reset all appropriate tag fields.

Examples:
  -g ""
    same as: -g 255
  -g Metal
    same as: -g 9
  --genre "Black Metal"
    same as: --genre 138
  -g "Deep Throat"
    same as: --genre "Deep Throat"
  ID3v2 special genre:
    -g remix
```

* * *

## <a name="hp5"></a>\[--genre hv\]

[Back To Top](#top)

```
Help for --genre <num|str>: Possible Values

Possible values are:
  -Num-   -Str-
     0     Blues
     1     Classic Rock
     2     Country
     3     Dance
     4     Disco
     5     Funk
     6     Grunge
     7     Hip-Hop
     8     Jazz
     9     Metal
    10     New Age
    11     Oldies
    12     Other
    13     Pop
    14     R&B
    15     Rap
    16     Reggae
    17     Rock
    18     Techno
    19     Industrial
    20     Alternative
    21     Ska
    22     Death Metal
    23     Pranks
    24     Soundtrack
    25     Euro-Techno
    26     Ambient
    27     Trip-Hop
    28     Vocal
    29     Jazz+Funk
    30     Fusion
    31     Trance
    32     Classical
    33     Instrumental
    34     Acid
    35     House
    36     Game
    37     Sound Clip
    38     Gospel
    39     Noise
    40     Alternative Rock
    41     Bass
    42     Soul
    43     Punk
    44     Space
    45     Meditative
    46     Instrumental Pop
    47     Instrumental Rock
    48     Ethnic
    49     Gothic
    50     Darkwave
    51     Techno-Industrial
    52     Electronic
    53     Pop-Folk
    54     Eurodance
    55     Dream
    56     Southern Rock
    57     Comedy
    58     Cult
    59     Gangsta
    60     Top 40
    61     Christian Rap
    62     Pop/Funk
    63     Jungle
    64     Native American
    65     Cabaret
    66     New Wave
    67     Psychedelic
    68     Rave
    69     Showtunes
    70     Trailer
    71     Lo-Fi
    72     Tribal
    73     Acid Punk
    74     Acid Jazz
    75     Polka
    76     Retro
    77     Musical
    78     Rock & Roll
    79     Hard Rock
    80     Folk
    81     Folk/Rock
    82     National Folk
    83     Swing
    84     Fast-Fusion
    85     Bebob
    86     Latin
    87     Revival
    88     Celtic
    89     Bluegrass
    90     Avantgarde
    91     Gothic Rock
    92     Progressive Rock
    93     Psychedelic Rock
    94     Symphonic Rock
    95     Slow Rock
    96     Big Band
    97     Chorus
    98     Easy Listening
    99     Acoustic
   100     Humour
   101     Speech
   102     Chanson
   103     Opera
   104     Chamber Music
   105     Sonata
   106     Symphony
   107     Booty Bass
   108     Primus
   109     Porn Groove
   110     Satire
   111     Slow Jam
   112     Club
   113     Tango
   114     Samba
   115     Folklore
   116     Ballad
   117     Power Ballad
   118     Rhythmic Soul
   119     Freestyle
   120     Duet
   121     Punk Rock
   122     Drum Solo
   123     A Cappella
   124     Euro-House
   125     Dance Hall
   126     Goa
   127     Drum & Bass
   128     Club-House
   129     Hardcore
   130     Terror
   131     Indie
   132     BritPop
   133     Negerpunk
   134     Polsk Punk
   135     Beat
   136     Christian Gangsta Rap
   137     Heavy Metal
   138     Black Metal
   139     Crossover
   140     Contemporary Christian
   141     Christian Rock
   142     Merengue
   143     Salsa
   144     Thrash Metal
   145     Anime
   146     JPop
   147     Synthpop
 For ID3v1 you should only use one of the above genres
 For ID3v2 you can also use one of these special genres:
   Cover
   Remix
 And for all tags except ID3v1 you may also specify your own genres
  (e.g. "Deep Throat")

 The special values 255 and "" reset all appropriate tag fields.
```

* * *

## <a name="hp6"></a>\[--rem-pic help\]

[Back To Top](#top)

```
Help for --rem-pic <numlist>:

Format of <numlist> is "<numrange|str|strshort>[,<numrange|str|strshort>[...]]"
Format of <numrange> is "<num[-<num>]>"

Possible values are:
  -Num-    -Str-   -StrShort-   -Desc-
     1                           index of tag field
  (through)
   999                           index of tag field
            all     a            all matching fields

Examples:
  --rem-pic 1
  --rem-pic 1,2
  --rem-pic 1,3,4,5,6,8,9,10
    same as: --rem-pic 1,3-6,8-10
  --rem-pic all
```

* * *

## <a name="hp7"></a>\[--rem-geo help\]

[Back To Top](#top)

```
Help for --rem-geo <numlist>:

Format of <numlist> is "<numrange|str|strshort>[,<numrange|str|strshort>[...]]"
Format of <numrange> is "<num[-<num>]>"

Possible values are:
  -Num-    -Str-   -StrShort-   -Desc-
     1                           index of tag field
  (through)
   999                           index of tag field
            all     a            all matching fields

Examples:
  --rem-geo 1
  --rem-geo 1,2
  --rem-geo 1,3,4,5,6,8,9,10
    same as: --rem-geo 1,3-6,8-10
  --rem-geo all
```

* * *

## <a name="hp8"></a>\[--fadd help\]

[Back To Top](#top)

```
Help for --fadd <fldadd>:

Format of <fldadd> is "[<tag>:]<fldname>=<str|num>"
                   or "[<tag>:]<str>=<str|num>"
Format of <tag> is "<taglong|tagshort>"

Possible values for <tag> in <fldadd> are:
  -TagLong-  -TagShort-  -Desc-
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag

To see the list of possible field names
please use '--fadd hvIV2' for ID3v2 tag fields
        or '--fadd hvAV2' for APEv2 tag fields
        or '--fadd hvVOR' for Vorbis tag fields

With this command you can add tag fields.
The <tag> only defines how the field's name is interpreted,
but it does not define in which tag the field will be used.
For example you may use a ID3v2 <fldname> but the field
may be used in a APEv2 or Vorbis tag.

Note that
- the default for <tag> in <fldadd> is ID3v2
  and if you omit <tag> then ID3v2 will be assumed
- some tag fields are only allowed once per tag

Examples:
  --fadd iv2:TBPM=200
    same as: --fadd id3v2:TBPM=200
         or: --fadd TBPM=200
         or: --fadd vor:TEMPO=200
  --fadd vor:MY_WEIGHT_IS=173
    same as: --fadd vorbc:MY_WEIGHT_IS=173
  --fadd "av2:IS IT RYTHMIC=yes, but not so much"
  --fadd IPLS="Producer:Florian; Guitar:Mike; Vocals:Pete"
```

* * *

## <a name="hp9"></a>\[--fadd hvIV2\]

[Back To Top](#top)

```
Help for --fadd <fldadd>: Possible Values (ID3v2)

Format of <fldadd> is "[(id3v2|iv2):]<fldname>=<str|num>"
Format of <fldname> is "<fnshort|fnlong>"

Possible values for ID3v2 <fldname> are:
  -FnShort- -FnLong- -FldType- -VerFl- -Desc-
   COM       COMM     com       234     Comment
   IPL       IPLS     ipl       23      Involved people list
   MCI       MCDI     mci       234     Music CD Identifier
   CNT       PCNT     cnt       234     Play counter
   POP       POPM     pop       234     Popularimeter
             PRIV     ufi        34     Private frame
   TAL       TALB     txt       234     Album title
   TBP       TBPM     txt       234     Beats Per Minute
   TCT       TCAT     txt       234     iTunes Podcast Category
   TCP       TCMP     txt       234     iTunes Compilation
   TCM       TCOM     txt       234     Composer
   TCO       TCON     txt       234     Genre
   TCR       TCOP     txt       234     Copyright message
   TDA       TDAT     txt       23      Date
             TDEN     txt         4     Encoding time
   TDS       TDES     txt       234     iTunes Podcast Description
   TDY       TDLY     txt       234     Playlist delay
             TDOR     txt         4     Original release time
             TDRC     txt         4     Recording time
             TDRL     txt         4     Release time
             TDTG     txt         4     Tagging time
   TEN       TENC     txt       234     Encoded by
   TXT       TEXT     txt       234     Lyricist/Text writer
   TFT       TFLT     txt       234     File type
   TID       TGID     txt       234     iTunes Podcast ID
   TIM       TIME     txt       23      Time
             TIPL     txt         4     Involved people list
   TKE       TKEY     txt       234     Initial key
   TLA       TLAN     txt       234     Language(s)
   TLE       TLEN     txt       234     Length in ms
             TMCL     txt         4     Musician credits list
   TMT       TMED     txt       234     Media type
             TMOO     txt         4     Mood
   TOA       TOPE     txt       234     Original artist(s)
   TOT       TOAL     txt       234     Original album title
   TOF       TOFN     txt       234     Original filename
   TOL       TOLY     txt       234     Original lyricist(s)
   TOR       TORY     txt       23      Original release year
             TOWN     txt        34     File owner/licensee
   TP1       TPE1     txt       234     Artist
   TP2       TPE2     txt       234     Album Performer
   TP3       TPE3     txt       234     Conductor/Performer refinement
   TP4       TPE4     txt       234     Interpreted or modified by
   TPB       TPUB     txt       234     Publisher
             TPRO     txt         4     Produced notice
   TRD       TRDA     txt       23      Recording dates
             TRSN     txt        34     Internet Radio Station name
             TRSO     txt        34     Internet Radio Station owner
   TSC       TSOC     txt       234     Composer sort order
   TS2       TSO2     txt       234     Album Performer sort order
   TSA       TSOA     txt       234     Album sort order
   TST       TSOT     txt       234     Title sort order
   TSP       TSOP     txt       234     Performer sort order
   TRC       TSRC     txt       234     International Standard Recording Code
   TSS       TSSE     txt       234     SW/HW and settings used for encoding
             TSST     txt         4     Set subtitle
   TT1       TIT1     txt       234     Content group desc
   TT2       TIT2     txt       234     Track title
   TT3       TIT3     txt       234     Subtitle/Desc refinement
   TXX       TXXX     wxx       234     User defined text info frame
   TYE       TYER     txt       23      Year
   UFI       UFID     ufi       234     Unique file identifier
   ULT       USLT     com       234     Unsychronized lyric/text transcription
   WAF       WOAF     url       234     Official audio file webpage
   WAR       WOAR     url       234     Official artist webpage
   WAS       WOAS     url       234     Official audio source webpage
   WCM       WCOM     url       234     Commercial information
   WCP       WCOP     url       234     Copyright/Legal information
             WPAY     url        34     Payment
   WPB       WPUB     url       234     Publishers official webpage
             WORS     url        34     Official Internet Radio Station WP
   WXX       WXXX     wxx       234     User defined URL link frame

 Additionally to these fixed field names you may also
 use your own custom field names in these ranges:
   T00       T000     txt       234     Custom text info frame
  (through)
   TZZ       TZZZ     txt       234     Custom text info frame
   W00       W000     url       234     Custom URL info frame
  (through)
   WZZ       WZZZ     url       234     Custom URL info frame

Note that
- most fields do only exist in ID3v2 tags and not in any other tag type.
  If you want to add/modify fields in other tag types you should either
  only use fields of <fldtype> txt and url,
  or simply use another <tag> as input format in <fldadd>
- some programs (e.g. one made by a big company with a fruit in their logo)
  sometimes use field names that end with a whitespace in
  ID3v2.3 or ID3v2.4 tags. Such field names are invalid
  and will be replaced by valid ones if possible
  or ignored if necessary
- the version flag <verfl> shows for which ID3v2 versions
  a certain field is valid:
  2 means for ID3v2.2,  3 means for ID3v2.3 and  4 means for ID3v2.4
- if you for example add a field that is only valid for ID3v2.4
  to a ID3v2.3 tag then it will either get mapped to
  a comment or simply be ignored

Examples:
  --fadd "TXX=Summer breeze made my ice melt"
    same as: --fadd "TXXX=It's colder outside at night"
  --fadd "TAM=My custom AM text field"
    same as: --fadd "iv2:TAM=My custom AM text field"
  --fadd "TONG=My custom ONG text field"
  --fadd "W123=http://www.123.org"
```

* * *

## <a name="hp10"></a>\[--fadd hvAV2\]

[Back To Top](#top)

```
Help for --fadd <fldadd>: Possible Values (APEv2)

Format of <fldadd> is "(apev2|av2):<str>=<str|num>"

Possible values for APEv2 field names are:
  -Str-                    -FldType-  -Desc-
   TITLE                    str        Track title
   SUBTITLE                 str        Subtitle
   ARTIST                   str        Artist
   ALBUM                    str        Album title
   DEBUT ALBUM              str        Debut album
   PUBLISHER                str        Publisher
   CONDUCTOR                str        Conductor
   TRACK                    int        Track number
   TRACKTOTAL               int        Tracks total
   DISC                     int        Disc number
   DISCTOTAL                int        Discs total
   COMPOSER                 str        Composer
   COMMENT                  str        Comment
   COPYRIGHT                str        Copyright holder
   PUBLICATIONRIGHT         str        Publication right holder
   FILE                     url        File location
   EAN/UPC                  str        EAN-13/UPC-A bar code identifier
   ISBN                     str        ISBN number
   CATALOG                  str        Catalog number
   LC                       str        Label Code
   YEAR                     yea        Year
   RECORD DATE              tst        Recording date
   RECORD LOCATION          str        Recording location
   GENRE                    str        Genre
   MEDIA                    str        Media
   INDEX                    str        Indexes for quick access
   RELATED                  url        Location of related information
   ISRC                     str        International Standard Recording Code
   LANGUAGE                 str        Language(s)
   BIBLIOGRAPHY             url        Bibliography/Dicography
   REPLAYGAIN_TRACK_GAIN    str        ReplayGain track volume adjustment
   REPLAYGAIN_TRACK_PEAK    str        ReplayGain track peak level
   REPLAYGAIN_ALBUM_GAIN    str        ReplayGain album volume adjustment
   REPLAYGAIN_ALBUM_PEAK    str        ReplayGain album peak level
   INVOLVEDPEOPLE           str        Involved people

 Additionally to these fixed field names you may also
 use your own custom field names

Note that
- a custom field name may contain almost any char and be of arbitrary length
- to use ':' or '.' in field names you need escape those chars
  with a leading backslash ('\', e.g. "HOME\:TOWN\.69",
  in Linux Bash Shell that might have to be "HOME\\:TOWN\\.69")
- field names are case-insensitive (i.e. 'A' == 'a')

Examples:
  --fadd av2:LANGUAGE=Norwegian
  --fadd av2:BIBLIOGRAPHY=www.hugeband.nor
  --fadd "av2:RECORD DATE=2011-04-30"
    same as: --fadd "av2:record date=2011-04-30"
```

* * *

## <a name="hp11"></a>\[--fadd hvVOR\]

[Back To Top](#top)

```
Help for --fadd <fldadd>: Possible Values (Vorbis)

Format of <fldadd> is "(vorbc|vor):<str>=<str|num>"

Possible values for Vorbis field names are:
  -Str-                    -FldType-  -Desc-
   TITLE                    str        Track title
   VERSION                  str        Version (e.g. remix info)
   ALBUM                    str        Album title
   TRACKNUMBER              str        Track number
   TRACKTOTAL               str        Total tracks
   DISCNUMBER               str        Disc number
   DISCTOTAL                str        Total discs
   ARTIST                   str        Artist
   PERFORMER                str        Performer
   COPYRIGHT                str        Copyright holder
   LICENSE                  str        License
   ORGANIZATION             str        Organization
   DESCRIPTION              str        Description
   GENRE                    str        Genre
   DATE                     str        Recording date
   LOCATION                 str        Recording location
   CONTACT                  str        Contact address
   ISRC                     str        International Standard Recording Code
   TEMPO                    str        Beats per minute
   LYRICS                   str        Lyrics
   REPLAYGAIN_TRACK_GAIN    str        ReplayGain track volume adjustment
   REPLAYGAIN_TRACK_PEAK    str        ReplayGain track peak level
   REPLAYGAIN_ALBUM_GAIN    str        ReplayGain album volume adjustment
   REPLAYGAIN_ALBUM_PEAK    str        ReplayGain album peak level
   ENCODER                  str        Encoder software
   INVOLVEDPEOPLE           str        Involved people

 Additionally to these fixed field names you may also
 use your own custom field names

Note that
- a custom field name may contain almost any char and be of arbitrary length
- to use ':' or '.' in field names you need escape those chars
  with a leading backslash ('\', e.g. "HOME\:TOWN\.69",
  in Linux Bash Shell that might have to be "HOME\\:TOWN\\.69")
- field names are case-insensitive (i.e. 'A' == 'a')

Examples:
  --fadd vor:TEMPO=75
  --fadd vor:GENRE=Rock
    same as: --fadd vor:genre=Rock
```

* * *

## <a name="hp12"></a>\[--fset help\]

[Back To Top](#top)

```
Help for --fset <fldset>:

Format of <fldset> is "[<tag>:]<fldname>[.<num>]=[<str|num>]"
                   or "[<tag>:]<str>[.<num>]=[<str|num>]"
Format of <tag> is "<taglong|tagshort>"

Possible values for <tag> in <fldset> are:
  -TagLong-  -TagShort-  -Desc-
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag

To see the list of possible field names
please use '--fset hvIV2' for ID3v2 tag fields
        or '--fset hvAV2' for APEv2 tag fields
        or '--fset hvVOR' for Vorbis tag fields

With this command you can add, modify or reset (aka remove) tag fields.
The <tag> only defines how the field's name is interpreted,
but it does not define in which tag the field will be used.
For example you may use a ID3v2 <fldname> but the field
may be used in a APEv2 or Vorbis tag.

Note that
- the default for <tag> in <fldset> is ID3v2
  and if you omit <tag> then ID3v2 will be assumed
- if you don't specify the field's number
  * and one or more fields with the specified field name
    already exist in a tag then
    all appropriate tag fields will be modified/resetted
  * and a tag doesn't already contain the specified field
    then a new field will be added to the tag
- if you don't specify a value for a field then
  all appropriate tag fields will be resetted

Examples:
  --fset iv2:TBPM=200
    same as: --fset id3v2:TBPM=200
         or: --fset TBPM=200
         or: --fset vor:TEMPO=200
  --fset vor:MY_WEIGHT_IS=173
    same as: --fset vorbc:MY_WEIGHT_IS=173
  --fset "av2:IS IT RYTHMIC=yes, but not so much"
  --fset IPLS="Producer:Florian; Guitar:Mike; Vocals:Pete"
  --fset iv2:PRIV=
    same as: --fset PRIV=
         or: --fset PRIV=""
  --fset TXXX.2=
  --fset TXXX.1="At the Beach"
```

* * *

## <a name="hp13"></a>\[--fset hvIV2\]

[Back To Top](#top)

```
Help for --fset <fldset>: Possible Values (ID3v2)

Format of <fldset> is "[(id3v2|iv2):]<fldname>[.<num>]=[<str|num>]"
Format of <fldname> is "<fnshort|fnlong>"

Possible values for ID3v2 <fldname> are:
  -FnShort- -FnLong- -FldType- -VerFl- -Desc-
   CRA       AENC     cpy       234     Audio encryption
             ASPI     cpy         4     Audio seek point index
   COM       COMM     com       234     Comment
             COMR     cpy        34     Commercial frame
             ENCR     cpy        34     Encryption method registration
             EQU2     cpy         4     Equalisation
   EQU       EQUA     cpy       23      Equalization
   ETC       ETCO     cpy       234     Event timing codes
             GRID     cpy        34     Group identifier registration
             TIPL     txt         4     Involved people list
   IPL       IPLS     ipl       23      Involved people list
   LNK       LINK     cpy       234     Linked information
   MCI       MCDI     mci       234     Music CD Identifier
   MLL       MLLT     cpy       234     MPEG location lookup table
             OWNE     cpy        34     Ownership frame
   CNT       PCNT     cnt       234     Play counter
   PCS       PCST     cpy       234     Podcast Stuff
   POP       POPM     pop       234     Popularimeter
             POSS     cpy        34     Position synchronisation frame
             PRIV     ufi        34     Private frame
   BUF       RBUF     cpy       234     Recommended buffer size
   RVA       RVAD     cpy       23      Relative volume adjustment
             RVA2     cpy         4     Relative volume adjustment
   REV       RVRB     cpy       234     Reverb
             SEEK     cpy         4     Seek frame
             SIGN     cpy         4     Signature frame
   SLT       SYLT     slt       234     Synchronized lyric/text
   STC       SYTC     cpy       234     Synced tempo codes
   TAL       TALB     txt       234     Album title
   TBP       TBPM     txt       234     Beats Per Minute
   TCT       TCAT     txt       234     iTunes Podcast Category
   TCP       TCMP     txt       234     iTunes Compilation
   TCM       TCOM     txt       234     Composer
   TCO       TCON     txt       234     Genre
   TCR       TCOP     txt       234     Copyright message
   TDA       TDAT     txt       23      Date
             TDEN     txt         4     Encoding time
   TDS       TDES     txt       234     iTunes Podcast Description
   TDY       TDLY     txt       234     Playlist delay
             TDOR     txt         4     Original release time
             TDRC     txt         4     Recording time
             TDRL     txt         4     Release time
             TDTG     txt         4     Tagging time
   TEN       TENC     txt       234     Encoded by
   TXT       TEXT     txt       234     Lyricist/Text writer
   TFT       TFLT     txt       234     File type
   TID       TGID     txt       234     iTunes Podcast ID
   TIM       TIME     txt       23      Time
   TKE       TKEY     txt       234     Initial key
   TLA       TLAN     txt       234     Language(s)
   TLE       TLEN     txt       234     Length in ms
             TMCL     txt         4     Musician credits list
   TMT       TMED     txt       234     Media type
             TMOO     txt         4     Mood
   TOA       TOPE     txt       234     Original artist(s)
   TOT       TOAL     txt       234     Original album title
   TOF       TOFN     txt       234     Original filename
   TOL       TOLY     txt       234     Original lyricist(s)
   TOR       TORY     txt       23      Original release year
             TOWN     txt        34     File owner/licensee
   TP1       TPE1     txt       234     Artist
   TP2       TPE2     txt       234     Album Performer
   TP3       TPE3     txt       234     Conductor/Performer refinement
   TP4       TPE4     txt       234     Interpreted or modified by
   TPB       TPUB     txt       234     Publisher
             TPRO     txt         4     Produced notice
   TRD       TRDA     txt       23      Recording dates
             TRSN     txt        34     Internet Radio Station name
             TRSO     txt        34     Internet Radio Station owner
   TSC       TSOC     txt       234     Composer sort order
   TS2       TSO2     txt       234     Album Performer sort order
   TSA       TSOA     txt       234     Album sort order
   TST       TSOT     txt       234     Title sort order
   TSP       TSOP     txt       234     Performer sort order
   TRC       TSRC     txt       234     International Standard Recording Code
   TSS       TSSE     txt       234     SW/HW and settings used for encoding
             TSST     txt         4     Set subtitle
   TT1       TIT1     txt       234     Content group desc
   TT2       TIT2     txt       234     Track title
   TT3       TIT3     txt       234     Subtitle/Desc refinement
   TXX       TXXX     wxx       234     User defined text info frame
   TYE       TYER     txt       23      Year
   UFI       UFID     ufi       234     Unique file identifier
             USER     cpy        34     Terms of use
   ULT       USLT     com       234     Unsychronized lyric/text transcription
   WAF       WOAF     url       234     Official audio file webpage
   WAR       WOAR     url       234     Official artist webpage
   WAS       WOAS     url       234     Official audio source webpage
   WCM       WCOM     url       234     Commercial information
   WCP       WCOP     url       234     Copyright/Legal information
             WPAY     url        34     Payment
   WPB       WPUB     url       234     Publishers official webpage
             WORS     url        34     Official Internet Radio Station WP
   WXX       WXXX     wxx       234     User defined URL link frame

 Additionally to these fixed field names you may also
 use your own custom field names in these ranges:
   T00       T000     txt       234     Custom text info frame
  (through)
   TZZ       TZZZ     txt       234     Custom text info frame
   W00       W000     url       234     Custom URL info frame
  (through)
   WZZ       WZZZ     url       234     Custom URL info frame

Note that
- most fields do only exist in ID3v2 tags and not in any other tag type.
  If you want to add/modify fields in other tag types you should either
  only use fields of <fldtype> txt and url,
  or simply use another <tag> as input format in <fldset>
- fields of <fldtype> cdm, cpy, mci, pop, ufi and slt can only be resetted
  but not added or modified
- some programs (e.g. one made by a big company with a fruit in their logo)
  sometimes use field names that end with a whitespace in
  ID3v2.3 or ID3v2.4 tags. Such field names are invalid
  and will be replaced by valid ones if possible
  or ignored if necessary
- the version flag <verfl> shows for which ID3v2 versions
  a certain field is valid:
  2 means for ID3v2.2,  3 means for ID3v2.3 and  4 means for ID3v2.4
- if you for example add a field that is only valid for ID3v2.4
  to a ID3v2.3 tag then it will either get mapped to
  a comment or simply be ignored

Examples:
  --fset AENC=
  --fset CNT=13
    same as: --fset PCNT=13
         or: --fset iv2:PCNT=13
  --fset "TAR=A custom text field"
  --fset "WID3=http://id3.org"
```

* * *

## <a name="hp14"></a>\[--fset hvAV2\]

[Back To Top](#top)

```
Help for --fset <fldset>: Possible Values (APEv2)

Format of <fldset> is "(apev2|av2):<str>[.<num>]=[<str|num>]"

Possible values for APEv2 field names are:
  -Str-                    -FldType-  -Desc-
   TITLE                    str        Track title
   SUBTITLE                 str        Subtitle
   ARTIST                   str        Artist
   ALBUM                    str        Album title
   DEBUT ALBUM              str        Debut album
   PUBLISHER                str        Publisher
   CONDUCTOR                str        Conductor
   TRACK                    int        Track number
   TRACKTOTAL               int        Tracks total
   DISC                     int        Disc number
   DISCTOTAL                int        Discs total
   COMPOSER                 str        Composer
   COMMENT                  str        Comment
   COPYRIGHT                str        Copyright holder
   PUBLICATIONRIGHT         str        Publication right holder
   FILE                     url        File location
   EAN/UPC                  str        EAN-13/UPC-A bar code identifier
   ISBN                     str        ISBN number
   CATALOG                  str        Catalog number
   LC                       str        Label Code
   YEAR                     yea        Year
   RECORD DATE              tst        Recording date
   RECORD LOCATION          str        Recording location
   GENRE                    str        Genre
   MEDIA                    str        Media
   INDEX                    str        Indexes for quick access
   RELATED                  url        Location of related information
   ISRC                     str        International Standard Recording Code
   LANGUAGE                 str        Language(s)
   BIBLIOGRAPHY             url        Bibliography/Dicography
   REPLAYGAIN_TRACK_GAIN    str        ReplayGain track volume adjustment
   REPLAYGAIN_TRACK_PEAK    str        ReplayGain track peak level
   REPLAYGAIN_ALBUM_GAIN    str        ReplayGain album volume adjustment
   REPLAYGAIN_ALBUM_PEAK    str        ReplayGain album peak level
   INVOLVEDPEOPLE           str        Involved people

 Additionally to these fixed field names you may also
 use your own custom field names

Note that
- a custom field name may contain almost any char and be of arbitrary length
- to use ':' or '.' in field names you need escape those chars
  with a leading backslash ('\', e.g. "HOME\:TOWN\.69",
  in Linux Bash Shell that might have to be "HOME\\:TOWN\\.69")
- field names are case-insensitive (i.e. 'A' == 'a')

Examples:
  --fset av2:YEAR=2011
    same as:--fset av2:year=2011
  --fset "av2:SUBTITLE=Greatest Hits"
  --fset av2:COMMENT.2=
  --fset "av2:DEBUT ALBUM=Men on Mars"
```

* * *

## <a name="hp15"></a>\[--fset hvVOR\]

[Back To Top](#top)

```
Help for --fset <fldset>: Possible Values (Vorbis)

Format of <fldset> is "(vorbc|vor):<str>[.<num>]=[<str|num>]"

Possible values for Vorbis field names are:
  -Str-                    -FldType-  -Desc-
   TITLE                    str        Track title
   VERSION                  str        Version (e.g. remix info)
   ALBUM                    str        Album title
   TRACKNUMBER              str        Track number
   TRACKTOTAL               str        Total tracks
   DISCNUMBER               str        Disc number
   DISCTOTAL                str        Total discs
   ARTIST                   str        Artist
   PERFORMER                str        Performer
   COPYRIGHT                str        Copyright holder
   LICENSE                  str        License
   ORGANIZATION             str        Organization
   DESCRIPTION              str        Description
   GENRE                    str        Genre
   DATE                     str        Recording date
   LOCATION                 str        Recording location
   CONTACT                  str        Contact address
   ISRC                     str        International Standard Recording Code
   TEMPO                    str        Beats per minute
   LYRICS                   str        Lyrics
   REPLAYGAIN_TRACK_GAIN    str        ReplayGain track volume adjustment
   REPLAYGAIN_TRACK_PEAK    str        ReplayGain track peak level
   REPLAYGAIN_ALBUM_GAIN    str        ReplayGain album volume adjustment
   REPLAYGAIN_ALBUM_PEAK    str        ReplayGain album peak level
   ENCODER                  str        Encoder software
   INVOLVEDPEOPLE           str        Involved people

 Additionally to these fixed field names you may also
 use your own custom field names

Note that
- a custom field name may contain almost any char and be of arbitrary length
- to use ':' or '.' in field names you need escape those chars
  with a leading backslash ('\', e.g. "HOME\:TOWN\.69",
  in Linux Bash Shell that might have to be "HOME\\:TOWN\\.69")
- field names are case-insensitive (i.e. 'A' == 'a')

Examples:
  --fset vor:DATE=2009-12-31
    same as:--fset vor:date=2009-12-31
```

* * *

## <a name="hp16"></a>\[--dset help\]

[Back To Top](#top)

```
Help for --dset <dscset>:

Format of <dscset> is "<fldname>[.<num>]=[<str>]"

To see the list of possible values for <fldname>
please use '--dset hv'

With this command you can set, modify and reset
the description of certain ID3v2 tag fields.

Examples:
  --dset COMM=Biography
  --dset "TXX.2=Back in the '80s"
  --dset WXX=
```

* * *

## <a name="hp17"></a>\[--dset hv\]

[Back To Top](#top)

```
Help for --dset <dscset>: Possible Values (ID3v2)

Format of <dscset> is "<fldname>[.<num>]=[<str>]"
Format of <fldname> is "<fnshort|fnlong>"

Possible values for ID3v2 <fldname> are:
  -FnShort- -FnLong- -FldType- -VerFl- -Desc-
   PIC       APIC     pic       234     Picture
   COM       COMM     com       234     Comment
   GEO       GEOB     geo       234     General encapsulated object
             PRIV     ufi        34     Private frame
   SLT       SYLT     slt       234     Synchronized lyric/text
   TXX       TXXX     wxx       234     User defined text info frame
   UFI       UFID     ufi       234     Unique file identifier
   ULT       USLT     com       234     Unsychronized lyric/text transcription
   WXX       WXXX     wxx       234     User defined URL link frame

Note that
- the version flag <verfl> shows for which ID3v2 versions
  a certain field is valid:
  2 means for ID3v2.2,  3 means for ID3v2.3 and  4 means for ID3v2.4

Examples:
  --dset "USLT=Original Lyrics"
  --dset "WXXX.1=The band's MySpace page"
  --dset TXXX.2=
```

* * *

## <a name="hp18"></a>\[--rem-tag help\]

[Back To Top](#top)

```
Help for --rem-tag <taglist>:

Format of <taglist> is "<tag>[,<tag>[...]]"
Format of <tag> is "<taglong|tagshort>"

Possible values for <tag> in <taglist> are:
  -TagLong-  -TagShort-  -Desc-
   id3v1      iv1         ID3v1 Tag
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag
   all        a           all Tags

Examples:
  --rem-tag id3v1,id3v2
    same as: --rem-tag iv1,iv2
  --rem-tag all
```

* * *

## <a name="hp19"></a>\[--extr-tag help\]

[Back To Top](#top)

```
Help for --extr-tag <taglist>:

Format of <taglist> is "<tag>[,<tag>[...]]"
Format of <tag> is "<taglong|tagshort>"

Possible values for <tag> in <taglist> are:
  -TagLong-  -TagShort-  -Desc-
   id3v1      iv1         ID3v1 Tag
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag
   all        a           all Tags

Examples:
  --extr-tag id3v1,id3v2
    same as: --extr-tag iv1,iv2
  --extr-tag all
```

* * *

## <a name="hp20"></a>\[--extr-pic help\]

[Back To Top](#top)

```
Help for --extr-pic <numlist>:

Format of <numlist> is "<numrange|str|strshort>[,<numrange|str|strshort>[...]]"
Format of <numrange> is "<num[-<num>]>"

Possible values are:
  -Num-    -Str-   -StrShort-   -Desc-
     1                           index of tag field
  (through)
   999                           index of tag field
            all     a            all matching fields

Examples:
  --extr-pic 1
  --extr-pic 1,2
  --extr-pic 1,3,4,5,6,8,9,10
    same as: --extr-pic 1,3-6,8-10
  --extr-pic all
```

* * *

## <a name="hp21"></a>\[--extr-geo help\]

[Back To Top](#top)

```
Help for --extr-geo <numlist>:

Format of <numlist> is "<numrange|str|strshort>[,<numrange|str|strshort>[...]]"
Format of <numrange> is "<num[-<num>]>"

Possible values are:
  -Num-    -Str-   -StrShort-   -Desc-
     1                           index of tag field
  (through)
   999                           index of tag field
            all     a            all matching fields

Examples:
  --extr-geo 1
  --extr-geo 1,2
  --extr-geo 1,3,4,5,6,8,9,10
    same as: --extr-geo 1,3-6,8-10
  --extr-geo all
```

* * *

## <a name="hp22"></a>\[--conv help\]

[Back To Top](#top)

```
Help for --conv <tagpair>:

Format of <tagpair> is "<taglist>-<tag>"
Format of <taglist> is "<tag>[,<tag>[...]]"
Format of <tag> is "<taglong|tagshort>"

Possible values for <tag> on the left side are:
  -TagLong-  -TagShort-  -Desc-
   id3v1      iv1         ID3v1 Tag
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag
   all        a           all Tags

Possible values for <tag> on the right side are:
  -TagLong-  -TagShort-  -Desc-
   id3v1      iv1         ID3v1 Tag
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag

Note that
- if a certain tag type is not allowed for the type of
  the current file then no conversion will be performed.
  E.g. in FLAC files nothing than Vorbis tags are allowed

Examples:
  --conv id3v1-id3v2  (convert all ID3v1 to ID3v2)
    same as: --conv iv1-iv2
  --conv id3v1,id3v2-apev2  (convert all ID3v1 and ID3v2 to APEv2)
    same as: --conv iv1,iv2-av2
  --conv all-id3v2 --tiv2 v3 (convert all to ID3v2.3)
    same as: --conv a-iv2 --tiv2 v3
         or: --conv all-iv2 --tiv2 v3
         or: --conv a-id3v2 --tiv2 v3
  --conv iv2-iv2 --tiv2 v2 (convert all ID3v2 to ID3v2.2)
```

* * *

## <a name="hp23"></a>\[--lev help\]

[Back To Top](#top)

```
Help for --lev <vlevlist>:

Format of <vlevlist> is "<vlev>[,<vlev>[...]]"
Format of <vlev> is "<num|str|strshort>"

Possible values are:
  -Num-   -Str-   -StrShort-   -Desc-
     1     gen                  general info
     2     opt                  cmdline options
     4     trd                  read tag field values
     8     tst                  set tag field vals
    16     twr                  written tag field vals
    32     tag                  tag info (overwrites trd/tst/twr)
    64     aud                  audio info
   128     dau                  detailed audio info
   255     all     a            enable all levels
     0     none    n            disable all levels

Note that
- you may combine two or more values, e.g. 4 + 8: --lev 12

Examples:
  --lev trd,tst,twr
    same as: --lev trd --lev tst --lev twr
         or: --lev 4,8,16
         or: --lev 28
  --lev gen --lev opt
    same as: --lev 3
  --lev tag,64
    same as: --lev 96
  --lev all
  --lev n
```

* * *

## <a name="hp24"></a>\[--ebs help\]

[Back To Top](#top)

```
Help for --ebs <bslist>:

Format of <bslist> is "<bs>[,<bs>[...]]"
Format of <bs> is "<num>[.<num>]"
  the first <num> being the number of the main bitstream
  the second <num> being the number of the sub-bitstream

OGG files can contain multiple bitstreams and
if you don't want to edit all of them at once,
you may specify one or more bitstreams that
shall be edited.
Furthermore a bitstream can contain multiple
sub-bitstreams.

When using the command -r the numbers of all
bitstreams will be displayed, if the file
contains more than one bitstream
(video files usually contain several bitstreams).

Note that
- bitstream 'x' is the same as 'x.0' and 'x.1'

Examples:
  --ebs all
  --ebs 1
    same as: --ebs 1.0
         or: --ebs 1.1
  --ebs 1,3
    same as: --ebs 1.0,3.0
         or: --ebs 1.1,3.1
  --ebs 2.2
  --ebs 4,4.2,5
    same as: --ebs 4.1,4.2,5.1
    same as: --ebs 4.0,4.2,5.0
```

* * *

## <a name="hp25"></a>\[--tag-sw help\]

[Back To Top](#top)

```
Help for --tag-sw <taglist>:

Format of <taglist> is "<tag>[,<tag>[...]]"
Format of <tag> is "<taglong|tagshort>"

Possible values for <tag> in <taglist> are:
  -TagLong-  -TagShort-  -Desc-
   id3v1      iv1         ID3v1 Tag
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag
   merg       mrg         Merged Tags
   all        a           all Tags
   none       n           no Tag(s)

Examples:
  --tag-sw id3v1,id3v2
    same as: --tag-sw iv1,iv2
  --tag-sw all
  --tag-sw n
```

* * *

## <a name="hp26"></a>\[--tag-ed help\]

[Back To Top](#top)

```
Help for --tag-ed <taglist>:

Format of <taglist> is "<tag>[,<tag>[...]]"
Format of <tag> is "<taglong|tagshort>"

Possible values for <tag> in <taglist> are:
  -TagLong-  -TagShort-  -Desc-
   id3v1      iv1         ID3v1 Tag
   id3v2      iv2         ID3v2 Tag
   apev2      av2         APEv2 Tag
   vorbc      vor         Vorbis Tag
   all        a           all Tags
   auto       au          auto select
   none       n           no Tag(s)

Examples:
  --tag-ed id3v1,id3v2
    same as: --tag-ed iv1,iv2
  --tag-ed all
  --tag-ed au
  --tag-ed n
```

* * *

## <a name="hp27"></a>\[--tiv2 help\]

[Back To Top](#top)

```
Help for --tiv2 <tiv2optlist>:

Format of <tiv2optlist> is "<tiv2opt>[,<tiv2opt>[...]]"

Possible values for <tiv2opt> in <tiv2optlist> are
  -TIV2Opt-    -Desc-
   cmpr         allow compression
   nocmpr       disallow compression (recommended)
   unsy         allow unsynchronization
   nounsy       disallow unsynchronization (recommended)
   v2           save as version v2.2
   v3           save as version v2.3 (recommended)
   v4           save as version v2.4

With this option you can control how ID3v2 tags are written.

Note that
- some major programs (like that made by a big company with
  a fruit in their logo) don't support compressed
  or unsynchronized tag fields.
  (The reason might be that reading such tag fields can consume
   an almost arbitrary amount of memory when not using temporary files)
  It is therefore recommended to not use these features
- some major programs (like the one mentioned before)
  are not capable of entirely reading ID3v2.4 tags correctly.
  It is therefore recommended to use ID3v2.3 tags
- the tag version is only used when writing new tags
  or converting tags (with --conv) and the destination tag is ID3v2,
  but not when rewriting existing tags

Examples:
  --tiv2 cmpr
  --tiv2 v4
  --tiv2 nocmpr,nounsy,v2
    same as:  --tiv2 nocmpr --tiv2 nounsy --tiv2 v2
```

* * *

## <a name="hp28"></a>\[--pic-tp help\]

[Back To Top](#top)

```
Help for --pic-tp <num|pictp>:

Possible values are:
  -Num-    -PicTp-           -Desc-
     0      other             Other
     1      icon-32           32x32 pixels 'file icon'
     2      icon-other        Other file icon
     3      cov-front         Cover (front)
     4      cov-back          Cover (back)
     5      leaflet           Leaflet page
     6      media             Media (e.g. lable side of CD)
     7      artist-lead       Lead artist
     8      artist            Artist/performer
     9      conduct           Conductor
    10      band              Band/Orchestra
    11      comp              Composer
    12      lyricist          Lyricist
    13      rec-loc           Recording Location
    14      dur-rec           During recording
    15      dur-perf          During performance
    16      screen            Movie/video screen capture
    17      flash             A bright coloured fish
    18      illus             Illustration
    19      logo-band         Band/artist logotype
    20      logo-studio       Publisher/Studio logotype

Examples:
  --pic-tp 1
    same as: --pic-tp icon-32
  --pic-tp 3
    same as: --pic-tp cov-front
```

* * *

## <a name="hp29"></a>\[--pic-fm help\]

[Back To Top](#top)

```
Help for --pic-fm <num|picfm>:

Possible values are:
  -Num-    -PicFm-
     0      jpg
     1      png
     2      tif
     3      gif
     4      psd
     5      ico
     6      bmp

Normally there should be no need to use this option
since this application determines the format by itself.
Nevertheless you may use this option to enforce an image format.

Examples:
  --pic-fm 2
    same as: --pic-fm tif
  --pic-fm 0
    same as: --pic-fm jpg
```

* * *

*Generated on 2025-08-11 14:26*
