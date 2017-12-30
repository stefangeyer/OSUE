# auth by Stefan Geyer
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

SOURCEDIR = src
BUILDDIR = build
DOCDIR = docs

SERVER_SOURCEDIR = $(SOURCEDIR)/$(SERVER_EXECUTABLE)
CLIENT_SOURCEDIR = $(SOURCEDIR)/$(CLIENT_EXECUTABLE)
SERVER_BUILDDIR = $(BUILDDIR)/$(SERVER_EXECUTABLE)
CLIENT_BUILDDIR = $(BUILDDIR)/$(CLIENT_EXECUTABLE)

HEADERS = $(wildcard $(SERVER_SOURCEDIR)/*.h) $(wildcard $(CLIENT_SOURCEDIR)/*.h)
# Cannot mix implicit and static pattern rules (static used for OBJECTS) --> Cannot use $(SOURCEDIR)/%.c
SERVER_SOURCES = $(wildcard $(SERVER_SOURCEDIR)/*.c)
CLIENT_SOURCES = $(wildcard $(CLIENT_SOURCEDIR)/*.c)
# $(patsubst pattern,replacement,text)
SERVER_OBJECTS = $(patsubst $(SERVER_SOURCEDIR)/%.c,$(SERVER_BUILDDIR)/%.o,$(SERVER_SOURCES))
CLIENT_OBJECTS = $(patsubst $(CLIENT_SOURCEDIR)/%.c,$(CLIENT_BUILDDIR)/%.o,$(CLIENT_SOURCES))

SERVER_EXECUTABLE = server
CLIENT_EXECUTABLE = client

.PHONY: all clean docs delivery
all: $(BUILDDIR) $(BUILDDIR)/$(CLIENT_EXECUTABLE) $(BUILDDIR)/$(SERVER_EXECUTABLE)

docs:
	doxygen Doxyfile

# PHONY so we can override the tgz when needed
delivery:
	tar -cvzf $@.tgz Makefile Doxyfile $(SOURCES) $(HEADERS)

$(BUILDDIR)/$(SERVER_EXECUTABLE): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/$(CLIENT_EXECUTABLE): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_OBJECTS): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SERVER_OBJECTS): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR):
	mkdir -p $@ $@/$(SERVER_EXECUTABLE) $@/$(CLIENT_EXECUTABLE)

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(DOCDIR)
	rm -rf delivery.tgz