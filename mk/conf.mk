
include mk/version.mk

CC       ?= clang
AR       ?= ar
LD       ?= clang
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -fPIC -std=$(CSTD) $(E4_CFLAGS)
LDFLAGS	 = -L. $(E4_LDFLAGS)

CONFIG_DEFINES=

O = o

TESTCFLAGS = -Wall -Werror -g -std=c11 $(E4_CFLAGS)
TESTLDFLAGS= $(E4_LDFLAGS)

GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

INCDIR  = include
SRCDIR  = src
DOCDIR  = doc

LIBNAME = libe4

ifeq ("$(STORE)", "none")
CONFIG_DEFINES += -DE4_STORE_NONE
endif
ifeq ("$(STORE)", "mem")
CONFIG_DEFINES += -DE4_STORE_MEM
endif
ifeq ("$(STORE)", "file")
CONFIG_DEFINES += -DE4_STORE_FILE
endif


.PHONY postbuild_config_echo:
	$(info Build Successful)
	@echo ""
	@echo "================================================================================="
	@echo ""
	@echo "LibE4 (C) Teserakt AG 2018-2020 has been built."
	@echo "The compilation options are as follows"
	@echo ""
	@echo "CONF=$(CONF)"
	@echo "CSTD=$(CSTD)"
	@echo "CC=$(CC)"
	@echo "LD=$(LD)"
	@echo "STORE=$(STORE)"
	@echo "E4_OUTPUT_DIR=$(E4_OUTPUT_DIR)"
ifeq ("$(CONF)", "all")
	@echo ""
	@echo "Shared Library: $(LIBSO)"
endif
	@echo ""
	@echo "================================================================================="
