
CC       ?= clang
AR       ?= ar
ARFLAGS  = rcs
CFLAGS	 = -Wall -fPIC -Werror -std=c89 $(E4_CFLAGS) 
LDFLAGS	 = -L. $(E4_CFLAGS)
LDSOFLAGS = -shared -fPIC -Wl,-soname,libe4s.so.1
INCLUDES = -Iinclude/ -Ibuild/symkey/include/

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR  = include
OBJDIR  = tmp/symkey/build
TESTOBJDIR = tmp/symkey/test
SRCDIR  = src
DOCDIR  = doc
BUILDDIR = build/symkey
LIBDIR  = build/symkey/lib
OUTINCDIR = build/symkey/include
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
LIBSO	= $(LIBDIR)/$(LIBNAME).so.$(VERSION)
DISTDIR	= dist/symkey/
TESTDIR = build/symkey/test

O = o

# test specific parts:
TESTCFLAGS = -Wall -Werror -g -std=c11 $(E4_CFLAGS)
TESTLDFLAGS =
