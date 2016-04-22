CC=gcc
CFLAGS= -std=gnu99 -Wall -g -o

OBJ= Fat12Recovery


Fat12Recovery: Fat12Recovery.c
	$(CC) $(CFLAGS) Fat12Recovery $^


clean:
	rm -rf  $(BINS) *.o temp output2 output3 output1 $(objects)
