# mydiff by Stefan Geyer
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

SOURCEDIR = src
BUILDDIR = build
DOCDIR = docs

# Cannot mix implicit and static pattern rules (static used for OBJECTS) --> Cannot use $(SOURCEDIR)/%.c
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
# $(patsubst pattern,replacement,text)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

EXECUTABLE = mydiff

.PHONY: all clean docs
all: $(BUILDDIR) $(BUILDDIR)/$(EXECUTABLE)

docs:
	doxygen Doxyfile

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR):
	mkdir $@

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(DOCDIR)