
E4_OUTPUT_DIR ?= build/all

LDSOFLAGS = -shared -fPIC -Wl,-soname,libe4.so.$(ABI_VERSION) $(E4_LDFLAGS)
INCLUDES = -Iinclude/ -I$(E4_OUTPUT_DIR)/include/

LDDYLIBFLAGS =-v -undefined suppress -flat_namespace -dynamiclib -current_version $(VERSION) -compatibility_version $(ABI_VERSION) 

# OBJ paths match their src folder equivalents
OBJDIR  = tmp/all/build
TESTOBJDIR = tmp/all/test

BUILDDIR = $(E4_OUTPUT_DIR)
LIBDIR  = $(BUILDDIR)/lib
OUTINCDIR = $(BUILDDIR)/include
DISTDIR	= dist/all/
TESTDIR = $(BUILDDIR)/test

LIB	= $(LIBDIR)/$(LIBNAME).a
LIBSO	= $(LIBDIR)/$(LIBNAME).so.$(VERSION)
LIBSO_NAME = $(LIBNAME).so.$(VERSION)
LIBSO_ABI = $(LIBDIR)/$(LIBNAME).so.$(ABI_VERSION)
LIBSO_CUR = $(LIBDIR)/$(LIBNAME).so

LIBDYLIB_NAME = $(LIBNAME).dylib
LIBDYLIB      = $(LIBDIR)/$(LIBDYLIB_NAME)

CONFIG_DEFINES += -DE4_MODE_ALL

ifeq ("$(STORE)", "mem")
$(error E4 configuration type ALL must be used with NONE or FILE storage types)
endif
