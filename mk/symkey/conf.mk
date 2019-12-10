
CC       ?= clang
AR       ?= ar
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -g -DE4_STORE_FILE -std=c89 $(E4_CFLAGS)
LDFLAGS	 = -L. $(E4_CFLAGS)
INCLUDES = -Iinclude/

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR  = include
OBJDIR  = tmp/symkey
SRCDIR  = src
DOCDIR  = doc
LIBDIR  = build/symkey/lib
OUTINCDIR = build/symkey/include
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist/symkey/
TESTDIR = build/symkey/test

O = o
