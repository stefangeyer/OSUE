# auth by Stefan Geyer
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

SOURCEDIR = src
BUILDDIR = build
DOCDIR = docs

SERVER_SOURCEDIR = $(SOURCEDIR)/$(SERVER)
CLIENT_SOURCEDIR = $(SOURCEDIR)/$(CLIENT)
COMMON_SOURCEDIR = $(SOURCEDIR)/$(COMMON)
SERVER_BUILDDIR = $(BUILDDIR)/$(SERVER)
CLIENT_BUILDDIR = $(BUILDDIR)/$(CLIENT)
COMMON_BUILDDIR = $(BUILDDIR)/$(COMMON)

# Cannot mix implicit and static pattern rules (static used for OBJECTS) --> Cannot use $(SOURCEDIR)/%.c
SERVER_SOURCES = $(wildcard $(SERVER_SOURCEDIR)/*.c)
CLIENT_SOURCES = $(wildcard $(CLIENT_SOURCEDIR)/*.c)
COMMON_SOURCES = $(wildcard $(COMMON_SOURCEDIR)/*.c)
# $(patsubst pattern,replacement,text)
SERVER_OBJECTS = $(patsubst $(SERVER_SOURCEDIR)/%.c,$(SERVER_BUILDDIR)/%.o,$(SERVER_SOURCES))
CLIENT_OBJECTS = $(patsubst $(CLIENT_SOURCEDIR)/%.c,$(CLIENT_BUILDDIR)/%.o,$(CLIENT_SOURCES))
COMMON_OBJECTS = $(patsubst $(COMMON_SOURCEDIR)/%.c,$(COMMON_BUILDDIR)/%.o,$(COMMON_SOURCES))

COMMON = common
SERVER = server
CLIENT = client

.PHONY: all clean docs delivery
all: $(BUILDDIR) $(BUILDDIR)/$(CLIENT) $(BUILDDIR)/$(SERVER)

docs:
	doxygen Doxyfile

# PHONY so we can override the tgz when needed
delivery:
	tar -cvzf $@.tgz Makefile Doxyfile $(SOURCEDIR)

$(BUILDDIR)/$(SERVER): $(SERVER_OBJECTS) $(COMMON_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/$(CLIENT): $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_OBJECTS): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SERVER_OBJECTS): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(COMMON_OBJECTS): $(COMMON_SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR):
	mkdir -p $@ $@/$(SERVER) $@/$(CLIENT) $@/$(COMMON)

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(DOCDIR)
	rm -rf delivery.tgz