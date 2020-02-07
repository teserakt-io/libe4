
E4_OUTPUT_DIR ?= build/all

LDSOFLAGS = -shared -fPIC -Wl,-soname,libe4.so.$(ABI_VERSION) $(E4_LDFLAGS)
INCLUDES = -Iinclude/ -I$(E4_OUTPUT_DIR)/include/

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

CONFIG_DEFINES += -DE4_MODE_ALL

ifneq ("$(STORE)", "none")
$(error E4 configuration type ALL cannot be defined with any storage type other than NONE)
endif
