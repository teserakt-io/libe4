
$(OBJDIR)/%.$O: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

default: setup $(LIB)

setup:
	mkdir -p $(OBJDIR); \
	mkdir -p $(OBJDIR)/test/; \
	mkdir -p $(OBJDIR)/crypto; \
	mkdir -p $(OBJDIR)/crypto/curve25519; \
	mkdir -p $(OBJDIR)/crypto/ed25519; \
	mkdir -p $(LIBDIR); \
	mkdir -p $(DISTDIR); \

$(LIB): setup $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR) $(OUTINCDIR); \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

clean:
	find . -name "*.o" -exec rm -vf {} \;
	rm -rf $(DISTDIR) 
	rm -rf $(OBJDIR) 
	rm -rf $(LIB)
	rm -rf $(LIBDIR)


release: $(LIB)
	@echo 'Making $(DISTDIR)/$(LIBNAME)-$(VERSION).tar.bz2'
	tar cfvj $(DISTDIR)/$(LIBNAME)-$(VERSION).tar.bz2 $(LIBDIR)/* $(INCDIR)/*

dist: $(LIB)
	@echo 'Making $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2'
	tar cfvj $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2 $(LIBDIR)/* $(INCDIR)/*

test: clean setup $(LIB) $(TESTS)

format:
	clang-format -i src/*.c src/crypto/*.c include/e4/*.h include/e4/crypto/*.h include/e4/internal/*.h

# Generic test rule.
$(TESTDIR)/%: test/%.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LIB) 

# Configuration mode tests
build/test/e4symmetric: test/e4.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -DE4_STORE_FILE=1 -o $@ $< $(LIB)

