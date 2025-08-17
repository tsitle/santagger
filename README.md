# Sandy Tagger - a command line tool for tagging audio and video files

Overview of *Sandy Tagger*'s features:

- read metadata from audio and video files
- read, edit, create, delete and convert tags
- convert audio files to the WAV/AIFF format
- analyze audio and video files (i.e., search for erroneous tags etc.)
- extract raw tags, binary blobs and images to binary files

## Trivia

This project began in 2005, but it was never published.  
But now, 20 years later, I decided to finally publish it.  
I have been using this application all these years for my music collection, and I hope it will be useful to others, too.

## Supported Codecs and Tags

| Operations   | Audio Codec               |
|--------------|---------------------------|
| RD / DC      | MPEG-1 Layer I / II / III |
| CR           | RIFF / AIFF Wave files    |
| RD / ED / DC | Flac                      |
| RD / ED / DC | Ogg Flac                  |
| RD / ED / DC | Ogg Vorbis                |
| RD / ED      | Ogg Speex                 |
| RD / ED      | Ogg Celt                  |

| Operations | Video Codec |
|------------|-------------|
| RD / ED    | Ogg Theora  |
| RD / ED    | Ogg Dirac   |

| Operations        | Tag            |
|-------------------|----------------|
| RD / ED / CR / CV | ID3v1.0/1      |
| RD / CV           | ID3v1 Extended |
| RD / ED / CR / CV | ID3v2.2/3/4    |
| RD / ED / CR / CV | Ape v2         |
| RD / ED / CR / CV | Vorbis Comment |

(Legend: RD=read, ED=edit tags, CR=create, DC=decode to WAV/AIFF, CV=convert)

## Building the Application

For instructions on how to build the application, see [README-build.md](README-build.md).  
If you just want to download the latest release, you can skip this section.

## Downloading the Application

The latest release archives can be downloaded from [Releases](https://github.com/tsitle/santagger/releases)

## Installing the Application

Once you have downloaded an archive file, you'll need to extract it.  
For example, if you downloaded the archive file `santagger-1.0-linux-x64.tgz`, you would do the following:  
`$ tar -xzf santagger-1.0-linux-x64.tgz`

Since the application is a single executable, you can just copy it to a directory in your PATH.  
For example, you could copy it to `/usr/local/bin` or `/usr/bin`.

On MS Windows you can do the same, but you might have to add the directory to your PATH environment variable.  
For example, you could copy it to `C:\Program Files\SandyTagger\`.

## User Manual

See the generated [User Manual](README-usermanual.md)

## Usage Examples

Apart from the examples below, you can find more examples by running

```
$ santagger --examples 
```

### Read a file

```
$ santagger avatarium-the_january_sea.mp3 
```

This will print the following information:

```
File [REA]: avatarium-the_january_sea.mp3
AUD mpg MPEG-1 Audio v1.0 layer III, 320 kbit/s CBR, 44100 Hz, j-stereo
AUD mpg  time 04:12 mm:ss
TAG mrg (Merged iv2: 7 valid fields)
TAG mrg Artist:  "Avatarium"
TAG mrg Album:   "The Girl With The Raven Mask"
TAG mrg Title:   "The January Sea"
TAG mrg Track:   2/9
TAG mrg Year:    2015
TAG mrg Genre:   "Metal"
TAG mrg Picture: 1 picture
```

### Edit the main Tag Fields

```
$ santagger --tag-ed iv2 \
        -b "Avatarium" \
        -a "The Girl With The Raven Mask" \
        -s "The January Sea" \
        -c "Great album" \
        -t 2 --track-tot 9 \
        -g 'Metal' \
        -y 2015 \
        --add-pic cover.jpg \
        "avatarium-the_january_sea.mp3"
```

### Adding / Replacing / Removing Pictures

If the file does not have an album cover, you can add one by using the `--add-pic` option.  
But if you want to replace the existing album cover, you can use the `--mod-pic` option:

```
$ santagger --mod-pic 1 --pic-fn cover.jpg "avatarium-the_january_sea.mp3"
```

You can actually use different types of pictures - and not just 'Album Cover'.  
For a complete list of supported picture types, you can run `$ santagger --pic-tp help`.  
And you can add multiple pictures to a file by using `--add-pic`.  
You can even add a description for each picture with `--pic-de`.

Adding two pictures with all possible attributes could look like this:

```
$ santagger \
        --add-pic --pic-fn cover-front-jap.jpg --pic-tp cov-front --pic-de "Front Cover Japanese Edition" \
        --add-pic --pic-fn cover-back-jap.jpg --pic-tp cov-back --pic-de "Back Cover Japanese Edition" \
        "avatarium-the_january_sea.mp3"
```

To see the list of pictures in a file, you could run `$ santagger --disp-all --disp-det avatarium-the_january_sea.mp3`:

```
File [REA]: avatarium-the_january_sea.mp3
AUD mpg MPEG-1 Audio v1.0 layer III, 320 kbit/s CBR, 44100 Hz, j-stereo
AUD mpg  time 07:56 mm:ss
TAG mrg (Merged iv2: 8 valid fields)
TAG mrg Artist: 
   Text "Avatarium"
TAG mrg Album: 
   Text "The Girl With The Raven Mask"
TAG mrg Title: 
   Text "The January Sea"
TAG mrg Track: 
   PosS 2/9
TAG mrg Year: 
   NumS 2015
TAG mrg Genre: 
   Text "Metal"
TAG mrg Picture: 
   01: Desc "Front Cover Japanese Edition"
   01: Pict type='Cover (front)', format=jpg
   01: Data not displayed [1681759 bytes, 1.60 MB]
   02: Desc "Back Cover Japanese Edition"
   02: Pict type='Cover (back)', format=jpg
   02: Data not displayed [982604 bytes, 959.57 KB]
```

If you wanted to remove the second picture, you could do so by using the `--rem-pic` option:

```
$ santagger --rem-pic 2 "avatarium-the_january_sea.mp3"
```

### Removing all Tags from a file

```
$ santagger --rem-tag all some_file.ogg
```

Note that `--rem-tag` removes all tag fields of the specified tag format.  
For instance, MP3 files can use the old tag format `ID3v1` and the modern tag format `ID3v2`,
while Ogg Vorbis files use `Vorbis Tag` and Flac files use `APEv2`.

If you had an MP3 file with both ID3 tag formats and wanted to remove only the legacy ID3v1 tag fields,
you could do so by using the following command: `santagger --rem-tag id3v1 some_file.mp3`.

### Decoding an Ogg Vorbis / Ogg Flac / Flac / MP3 file to WAV

```
$ santagger --dec wav --od wavefiles some_file.ogg
```

You can also specify the Bits Per Sample for the WAV file:

```
$ santagger --dec wav --od wavefiles --bps 24 some_file.ogg
```

Note: you don't have to specify the output directory.
