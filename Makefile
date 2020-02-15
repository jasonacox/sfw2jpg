CC=cc
CFLAGS=-I.

all: pwpjpg sfwjpg  

pwpjpg: pwpjpg.c
	$(CC) -o pwpjpg pwpjpg.c

sfwjpg: sfwjpg.c
	$(CC) -o sfwjpg sfwjpg.c

test:
	./sfwjpg example.sfw

clean:
	rm -f pwpjpg sfwjpg example.jpg
