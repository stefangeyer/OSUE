# battleship by Stefan Geyer
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

SOURCEDIR = src
BUILDDIR = build
DOCDIR = docs

# no need to specify headers, as they are in the same directory as the source files
CLIENT_SOURCES = client.c
CLIENT_OBJECTS = client.o
SERVER_SOURCES = server.c
SERVER_OBJECTS = server.o

CLIENT_EXECUTABLE = client
SERVER_EXECUTABLE = server

.PHONY: all clean docs
all: $(BUILDDIR) $(BUILDDIR)/$(CLIENT_EXECUTABLE) $(BUILDDIR)/$(SERVER_EXECUTABLE)

docs:
	doxygen Doxyfile

$(BUILDDIR)/$(CLIENT_EXECUTABLE): $(BUILDDIR)/$(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/$(SERVER_EXECUTABLE): $(BUILDDIR)/$(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR):
	mkdir $@

$(BUILDDIR)/$(CLIENT_OBJECTS): $(SOURCEDIR)/$(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/$(SERVER_OBJECTS): $(SOURCEDIR)/$(SERVER_SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(DOCDIR)