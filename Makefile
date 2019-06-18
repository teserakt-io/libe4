# Makefile
# 2018-05-01  Markku-Juhani O. Saarinen <mjos@iki.fi>
# 2018-12-01  Antony Vennard <antony@teserakt.io>

CC	 = gcc
AR       = ar
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -Ofast -DE4_STORE_FILE
LDFLAGS	 = -L.
INCLUDES = -Iinclude/
DOC      = doxygen

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR = include
OBJDIR  = build
SRCDIR  = src
DOCDIR  = doc
LIBDIR  = lib
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist

OBJS    = $(OBJDIR)/e4client.o		     \
          $(OBJDIR)/e4c_store_file.o         \
          $(OBJDIR)/e4util.o                 \
          $(OBJDIR)/crypto/aes_siv.o	     \
	  $(OBJDIR)/crypto/aes256enc_ref.o   \
	  $(OBJDIR)/crypto/sha3.o 	     \
	  $(OBJDIR)/crypto/keccakf1600.o

TESTS   = build/test/testutil build/test/testaessiv build/test/testsha3 build/test/teste4file 

default: setup $(LIB)

setup:
	mkdir -p $(OBJDIR); \
	mkdir -p $(OBJDIR)/test/; \
	mkdir -p $(OBJDIR)/crypto; \
	mkdir -p $(LIBDIR); \
	mkdir -p $(DISTDIR); \

$(LIB): $(OBJS)
	mkdir -p lib; \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(DISTDIR) $(OBJDIR) $(LIBDIR) $(OBJS) $(LIB) *~ *.e4p
	find . -name "*.o" -exec rm -f {} \;

dist: $(LIB)
	@echo 'Making $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2'
	tar cfvj $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2 $(LIBDIR)/* $(INCDIR)/*

test: clean setup $(LIB) $(TESTS)
	@echo "=== TESTS ==="
	@echo "Executing test: testaessiv"; ./build/test/testaessiv
	@echo "Executing test: testsha3"; ./build/test/testsha3
	@echo "Executing test: testutil"; ./build/test/testutil
	@echo "Executing test: teste4file"; ./build/test/teste4file

build/test/testaessiv: test/testaessiv.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< lib/libe4.a

build/test/testsha3: test/testsha3.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< lib/libe4.a

build/test/testutil: test/util.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< lib/libe4.a

build/test/teste4file: test/e4.c
	$(CC) $(CFLAGS) $(INCLUDES) -DE4_STORE_FILE=1 -o $@ $< lib/libe4.a

.PHONY: doc
