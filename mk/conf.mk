
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
OBJDIR  = build
SRCDIR  = src
DOCDIR  = doc
LIBDIR  = lib
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist

O = o
