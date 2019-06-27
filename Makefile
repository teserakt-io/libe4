# Makefile
# 2018-05-01  Markku-Juhani O. Saarinen <mjos@iki.fi>
# 2018-12-01  Antony Vennard <antony@teserakt.io>

CC	 = gcc
AR       = ar
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -g -DE4_STORE_FILE
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
	  $(OBJDIR)/crypto/keccakf1600.o     \
	  $(OBJDIR)/crypto/test_aes_siv.o    \
	  $(OBJDIR)/crypto/test_sha3.o       \
	  $(OBJDIR)/strlcpy.o

TESTS   = build/test/util \
          build/test/crypto \
	  build/test/e4file 

default: setup $(LIB)

setup:
	mkdir -p $(OBJDIR); \
	mkdir -p $(OBJDIR)/test/; \
	mkdir -p $(OBJDIR)/crypto; \
	mkdir -p $(LIBDIR); \
	mkdir -p $(DISTDIR); \

$(LIB): setup $(OBJS)
	mkdir -p lib; \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	find . -name "*.o" -exec rm -vf {} \;
	rm -rf $(DISTDIR) 
	rm -rf $(OBJDIR) 
	rm -rf $(LIB)
	rm -rf $(LIBDIR)


dist: $(LIB)
	@echo 'Making $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2'
	tar cfvj $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2 $(LIBDIR)/* $(INCDIR)/*

test: clean setup $(LIB) $(TESTS)

# Generic test rule.
build/test/%: test/%.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LIB) 

build/test/e4file: test/e4.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -DE4_STORE_FILE=1 -o $@ $< $(LIB)

.PHONY: doc
