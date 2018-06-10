# 3coloring by Stefan Geyer
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)
LDFLAGS = -lrt -lpthread

SOURCEDIR = src
BUILDDIR = build
DOCDIR = docs

SUPERVISOR_SOURCEDIR = $(SOURCEDIR)/$(SUPERVISOR)
GENERATOR_SOURCEDIR = $(SOURCEDIR)/$(GENERATOR)
COMMON_SOURCEDIR = $(SOURCEDIR)/$(COMMON)
SUPERVISOR_BUILDDIR = $(BUILDDIR)/$(SUPERVISOR)-o
GENERATOR_BUILDDIR = $(BUILDDIR)/$(GENERATOR)-o
COMMON_BUILDDIR = $(BUILDDIR)/$(COMMON)-o

# Cannot mix implicit and static pattern rules (static used for OBJECTS) --> Cannot use $(SOURCEDIR)/%.c
SUPERVISOR_SOURCES = $(wildcard $(SUPERVISOR_SOURCEDIR)/*.c)
GENERATOR_SOURCES = $(wildcard $(GENERATOR_SOURCEDIR)/*.c)
COMMON_SOURCES = $(wildcard $(COMMON_SOURCEDIR)/*.c)
# $(patsubst pattern,replacement,text)
SUPERVISOR_OBJECTS = $(patsubst $(SUPERVISOR_SOURCEDIR)/%.c,$(SUPERVISOR_BUILDDIR)/%.o,$(SUPERVISOR_SOURCES))
GENERATOR_OBJECTS = $(patsubst $(GENERATOR_SOURCEDIR)/%.c,$(GENERATOR_BUILDDIR)/%.o,$(GENERATOR_SOURCES))
COMMON_OBJECTS = $(patsubst $(COMMON_SOURCEDIR)/%.c,$(COMMON_BUILDDIR)/%.o,$(COMMON_SOURCES))

COMMON = common
SUPERVISOR = supervisor
GENERATOR = generator

.PHONY: all clean docs delivery
all: $(BUILDDIR) $(BUILDDIR)/$(SUPERVISOR) $(BUILDDIR)/$(GENERATOR)

docs:
	doxygen Doxyfile

# PHONY so we can override the tgz when needed
delivery:
	tar -cvzf $@.tgz Makefile Doxyfile $(SOURCEDIR)

$(BUILDDIR)/$(SUPERVISOR): $(COMMON_OBJECTS) $(SUPERVISOR_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/$(GENERATOR): $(COMMON_OBJECTS) $(GENERATOR_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SUPERVISOR_BUILDDIR)/%.o: $(SUPERVISOR_SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(GENERATOR_BUILDDIR)/%.o: $(GENERATOR_SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(COMMON_BUILDDIR)/%.o: $(COMMON_SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR):
	mkdir -p $@ $(SUPERVISOR_BUILDDIR) $(GENERATOR_BUILDDIR) $(COMMON_BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(DOCDIR)
	rm -rf delivery.tgz