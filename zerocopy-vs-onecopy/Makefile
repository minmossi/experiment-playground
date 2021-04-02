CC=gcc
#CFLAGS=-flto -Ofast -march=native -fno-signed-zeros -fno-trapping-math -funroll-loops -fopenmp -D_GLIBCXX_PARALLEL -pg -Wall
#CFLAGS=-pg -Wall
CFLAGS=-Wall

all: zerocopy onecopy

zerocopy: zerocopy.c
	$(CC) -o zerocopy $(CFLAGS) zerocopy.c

onecopy: onecopy.c
	$(CC) -o onecopy $(CFLAGS) onecopy.c

clean:
	rm -f zerocopy onecopy
