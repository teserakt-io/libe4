
CC       ?= clang
AR       ?= ar
LD       ?= clang
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -fPIC -std=c89 $(E4_CFLAGS)
LDFLAGS	 = -L. $(E4_CFLAGS)
LDSOFLAGS = -shared -fPIC -Wl,-soname,libe4p.so.1
INCLUDES = -Iinclude/ -Ibuild/pubkey/include/

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR  = include
OBJDIR  = tmp/pubkey/build
TESTOBJDIR = tmp/pubkey/test
SRCDIR  = src
DOCDIR  = doc
BUILDDIR = build/pubkey
LIBDIR  = build/pubkey/lib
OUTINCDIR = build/pubkey/include
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
LIBSO	= $(LIBDIR)/$(LIBNAME)p.so.$(VERSION)
DISTDIR	= dist/pubkey/
TESTDIR = build/pubkey/test

O = o

# test specific parts:
TESTCFLAGS = -Wall -Werror -g -std=c11 -Wno-unused-variable $(E4_CFLAGS)
TESTLDFLAGS = $(LDFLAGS)
