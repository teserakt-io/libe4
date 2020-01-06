
E4_OUTPUT_DIR ?= build/pubkey

LDSOFLAGS = -shared -fPIC -Wl,-soname,libe4p.so.1
INCLUDES = -Iinclude/ -I$(E4_OUTPUT_DIR)/include/

# OBJ paths match their src folder equivalents
OBJDIR  = tmp/pubkey/build
TESTOBJDIR = tmp/pubkey/test

BUILDDIR = $(E4_OUTPUT_DIR)
LIBDIR  = $(BUILDDIR)/lib
OUTINCDIR = $(BUILDDIR)/include
DISTDIR	= dist/pubkey/
TESTDIR = $(BUILDDIR)/test

LIB	= $(LIBDIR)/$(LIBNAME).a
LIBSO	= $(LIBDIR)/$(LIBNAME)p.so.$(VERSION)

