
CC       ?= clang
AR       ?= ar
LD       ?= clang
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -g -DE4_MODE_PUBKEY -DE4_STORE_FILE -std=c89 $(E4_CFLAGS)
LDFLAGS	 = -L. $(E4_CFLAGS)
INCLUDES = -Iinclude/

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR  = include
OBJDIR  = tmp/pubkey/build
TESTOBJDIR = tmp/pubkey/test
SRCDIR  = src
DOCDIR  = doc
LIBDIR  = build/pubkey/lib
OUTINCDIR = build/pubkey/include
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist/pubkey/
TESTDIR = build/pubkey/test

O = o

# test specific parts:
TESTCFLAGS = -Wall -Werror -g -DE4_STORE_FILE -std=c11 -Wno-unused-variable $(E4_CFLAGS)
TESTLDFLAGS = $(LDFLAGS) -L$(LIB)
