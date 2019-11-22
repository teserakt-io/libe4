
CC	 ?= clang
AR       ?= ar
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -g -DE4_STORE_FILE -std=c89
LDFLAGS	 = -L.
INCLUDES = -Iinclude/

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR = include
OBJDIR  = tmp/default
SRCDIR  = src
DOCDIR  = doc
LIBDIR  = build/lib
OUTINCDIR = build/include
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist

O = o
