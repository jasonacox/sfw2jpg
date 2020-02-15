# SFW to JPG

## Description
This project provides tools to convert Seattle FilmWorks (SFW) image formats to JPEG.

Seattle FilmWorks was a mail order film processing company that offered the option of digitizing images while processing the film.  The files were delivered back to their clients in a proprietary file formats: SFW and PWP.  SFW files were stored on disks and sent along with the processed photos and the PWP format was used to provide a download option over the Internet.

Everett Lipman created two programs, `sfwjpg` and `pwpjpg` to convert from the Seattle FilmWorks's private file formats to JFIF (JPEG File InterchangeFormat).  He also created `flipjpeg` which can be used to flip the JPEG images.  Everett opensourced these under the GNU General Public License.  

## Build Instructions
sfwjpg.c and pwpjpg.c are standalone ANSI C programs which should
compile without any modifications under UNIX.  Compilation under
Windows with Microsoft Visual C++ requires that the strcasecmp()
function, which occurs in both programs, be replaced with
strcmp().

* Successful builds on: MacOS, Linux and RPi

### Build using Makefile
```bash
make
cc -o pwpjpg pwpjpg.c
cc -o sfwjpg sfwjpg.c
```

## Usage

#### sfwjpg - Convert images in SFW file to JPG
`sfwjpg` will take a single .sfw file and convert it into a
.jpg (JFIF) file.  The image in the newly created .jpg file
will then need to be flipped vertically.  sfwjpg takes one or
two arguments, the name of the .sfw file to be converted, and
an optional output file:

```
sfwjpg mypic.sfw
```

This will create the file mypic.jpg in the current directory.
mypic.sfw will not be altered or removed.  Under Microsoft
Windows, sfwjpg.exe must be invoked from the run window
command line.

```
sfwjpg mypic.sfw foo.jpg
```

This will convert mypic.sfw and leave the result in foo.jpg.

```
sfwjpg mypic.sfw -
```

This will convert mypic.sfw and write the result to the standard
output.

#### pwpjpg - Extract images from PWP File
`pwpjpg` will take a .pwp file and extract the images in the
file.  A .pwp file typically contains the scanned images from
an entire roll of film.  The names of the images are contained
within the .pwp file.  pwpjpg takes a single argument, the
name of the .pwp file from which to extract the images:

```
pwpjpg mypics.pwp
```

This will extract all of the images contained in mypics.pwp
and write them in the current directory as .jpg (JFIF) files.
The file names will correspond to the names of the images
stored in the .pwp file.  mypics.pwp will not be altered or
removed.  Under Microsoft Windows, pwpjpg.exe must be invoked
from the run window command line.


### pwpext - Extracts and flips images in a PWP File
`pwpext` is a UNIX (Bourne) shell script which extracts and
flips the images from a .pwp file.  It takes a single
argument, the name of the .pwp file:

```
pwpext mypics.pwp
```

This will extract, flip, and store as .jpg files all of the
images contained in mypics.pwp.  mypics.pwp will not be
altered or removed.

** NOTE this requires building `flipjpeg`

### flipjpeg - Flip JPEG Images
`flipjpeg` can be used to JPEG images. It uses the Independent JPEG
Group's (IJG) free JPEG software, which you can find at
https://www.ijg.org/.  `flipjpeg` uses standard input
and output.  In order to compile flipjpeg.c, unpack the IJG
software and place flipjpeg.c in the same directory.  After
running 'configure' to create the Makefile for the IJG software,
add the following lines to Makefile:

```Makefile
FJOBJECTS= flipjpeg.o rdswitch.o cdjpeg.o

flipjpeg: $(FJOBJECTS) libjpeg.a
    $(LN) $(LDFLAGS) -o flipjpeg $(FJOBJECTS) libjpeg.a $(LDLIBS)
```

## Credit and Revision History
* Original code and Copyright (c) 1997-1999 by Everett Lipman
* Uploaded to Github with updated documentation and Makefiles - Jason Cox @jasonacox https://github.com/jasonacox/sfw2jpg

## References
* Seattle FilmWorks - https://en.wikipedia.org/wiki/Seattle_FilmWorks
* SFW93A format and conversion details
 - https://jonesrh.info/sfw/sfw_sfw93a_details.html 