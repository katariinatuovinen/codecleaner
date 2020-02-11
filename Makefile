CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -g -D_POSIX_C_SOURCE
LIB=ar rcs

all: main cleaner

main: main.c libcleaner.a cleanerlib.h log.h
	$(CC) $(CFLAGS) -o main main.c log.c -L. -lcleaner

cleaner: cleaner.c libcleaner.a cleanerlib.h log.h
	$(CC) $(CFLAGS) -o cleaner cleaner.c log.c -L. -lcleaner

cleanerlib.o: cleanerlib.c
		$(CC) $(CFLAGS) -c cleanerlib.c

libcleaner.a: cleanerlib.o
		$(LIB) libcleaner.a cleanerlib.o && rm -f cleanerlib.o

clean:
	rm -f main cleaner *.o *.a *.log *.clean
