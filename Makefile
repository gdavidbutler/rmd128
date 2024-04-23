CFLAGS=-I. -Os

all: rmd128

clobber: clean
	rm -f rmd128

clean:
	rm -f rmd128.o

rmd128: test/main.c rmd128.o
	$(CC) $(CFLAGS) -o rmd128 test/main.c rmd128.o

rmd128.o: rmd128.c rmd128.h
	$(CC) $(CFLAGS) -c rmd128.c
