CC=gcc
CFLAGS= -std=gnu99 -Wall -g -o

OBJ= notjustcats


notjustcats: notjustcats.c
	$(CC) $(CFLAGS) notjustcats $^


clean:
	rm -rf  $(BINS) *.o