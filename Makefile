CC = gcc
DEFS = −D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o mydiff.o

.PHONY: all clean
all: mydiff

mydiff: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c mydiff.h
mydiff.o: mydiff.c mydiff.h

clean:
	rm -rf *.o mydiff