CC=cc
CFLAGS=-I.

.PHONY: all test check clean

all: pwpjpg sfwjpg  

pwpjpg: pwpjpg.c
	$(CC) -o pwpjpg pwpjpg.c

sfwjpg: sfwjpg.c
	$(CC) -o sfwjpg sfwjpg.c

test:
	./sfwjpg example.sfw

check:
	sh tests/run_tests.sh

clean:
	rm -f pwpjpg sfwjpg example.jpg
