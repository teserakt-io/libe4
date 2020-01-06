
VERSION=1.0.0

CC       ?= clang
AR       ?= ar
LD       ?= clang
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -fPIC -std=$(CSTD) $(E4_CFLAGS)
LDFLAGS	 = -L. $(E4_LDFLAGS)

O = o

TESTCFLAGS = -Wall -Werror -g -std=c11 $(E4_CFLAGS)
TESTLDFLAGS= $(E4_LDFLAGS)

GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

INCDIR  = include
SRCDIR  = src
DOCDIR  = doc

LIBNAME = libe4


