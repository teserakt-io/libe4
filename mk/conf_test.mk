

CC	 = clang
AR       = ar
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -g -std=c89 -fsanitize=address,memory,undefined -fno-omit-frame-pointer
LDFLAGS	 = -L. -fsanitize=address,memory,undefined -fno-omit-frame-pointer

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR = include
OBJDIR  = out/test/obj
SRCDIR  = src
DOCDIR  = doc
LIBDIR  = out/test/lib
LIBNAME = libe4
LIB	= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist

O = o


