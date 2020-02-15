#flipjpeg - Flip JPEG Image

    flipjpeg.c is an ANSI C program which uses the Independent JPEG
    Group's (IJG) free JPEG software, which you can find at
    ftp://ftp.uu.net/graphics/jpeg.  flipjpeg.c uses standard input
    and output, and has only been tested under UNIX.  It is probably
    possible to compile flipjpeg.c on another platform with a few
    modifications, such as changing the input and output files.  See
    the IJG software documentation for more information.

    In order to compile flipjpeg.c under UNIX, unpack the IJG
    software and place flipjpeg.c in the same directory.  After
    running 'configure' to create the Makefile for the IJG software,
    add the following lines to Makefile:

```Makefile
    FJOBJECTS= flipjpeg.o rdswitch.o cdjpeg.o

    flipjpeg: $(FJOBJECTS) libjpeg.a
        $(LN) $(LDFLAGS) -o flipjpeg $(FJOBJECTS) libjpeg.a $(LDLIBS)
```
    To build:
```
      make flipjpeg
```