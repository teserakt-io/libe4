# Makefile
# 2018-05-01  Markku-Juhani O. Saarinen <mjos@iki.fi>
# 2018-12-01  Antony Vennard <antony@teserakt.io>

CC		    = gcc
AR          = ar
ARFLAGS     = rcs
CFLAGS		= -Wall -Werror -Ofast -DE4_STORE_FILE
LDFLAGS		= -L.
INCLUDES	= -Iinclude 


# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR = include
OBJDIR  = build
SRCDIR  = src
LIBDIR  = lib
LIBNAME = libe4
LIB		= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist

OBJS    = $(OBJDIR)/e4client.o			     \
          $(OBJDIR)/e4c_store_file.o         \
		  $(OBJDIR)/crypto/aes_siv.o	     \
		  $(OBJDIR)/crypto/aes256enc_ref.o   \
		  $(OBJDIR)/crypto/sha3.o 			 \
		  $(OBJDIR)/crypto/keccakf1600.o



default: setup $(LIB)

setup:
	mkdir -p $(OBJDIR); \
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
