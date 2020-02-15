CC=cc
CFLAGS=-I.

all: pwpjpg sfwjpg flipjpeg 

pwpjpg: pwpjpg.c
	$(CC) -o pwpjpg pwpjpg.c

sfwjpg: sfwjpg.c
	$(CC) -o sfwjpg sfwjpg.c

clean:
	rm -f pwpjpg sfwjpg 
