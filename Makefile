CFLAGS=-I. -Os

all: rmd128 rmd256

clobber: clean
	rm -f rmd128 rmd256

clean:
	rm -f rmd128.o rmd256.o

rmd128: test/main128.c rmd128.o
	$(CC) $(CFLAGS) -o rmd128 test/main128.c rmd128.o

rmd128.o: rmd128.c rmd128.h
	$(CC) $(CFLAGS) -c rmd128.c

rmd256: test/main256.c rmd256.o
	$(CC) $(CFLAGS) -o rmd256 test/main256.c rmd256.o

rmd256.o: rmd256.c rmd256.h
	$(CC) $(CFLAGS) -c rmd256.c
