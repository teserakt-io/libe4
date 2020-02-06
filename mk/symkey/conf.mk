
E4_OUTPUT_DIR ?= build/symkey

LDSOFLAGS = -shared -fPIC -Wl,-soname,libe4s.so.1
INCLUDES = -Iinclude/ -I$(E4_OUTPUT_DIR)/include/

# OBJ paths match their src folder equivalents
OBJDIR  = tmp/symkey/build
TESTOBJDIR = tmp/symkey/test

BUILDDIR = $(E4_OUTPUT_DIR)
LIBDIR  = $(BUILDDIR)/lib
OUTINCDIR = $(BUILDDIR)/include
DISTDIR	= dist/symkey/
TESTDIR = $(BUILDDIR)/test

LIB	= $(LIBDIR)/$(LIBNAME).a
LIBSO	= $(LIBDIR)/$(LIBNAME)s.so.$(VERSION)

CONFIG_DEFINES += -DE4_MODE_SYMKEY
