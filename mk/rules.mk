


$(OBJDIR)/%.$O: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY:setup

setup: 
	mkdir -p $(OBJDIR); \
	mkdir -p $(TESTOBJDIR); \
	mkdir -p $(OBJDIR)/crypto; \
	mkdir -p $(OBJDIR)/crypto/curve25519; \
	mkdir -p $(OBJDIR)/crypto/ed25519; \
	mkdir -p $(OBJDIR)/crypto/ctaes; \
	mkdir -p $(TESTDIR); \
	mkdir -p $(LIBDIR); \
	mkdir -p $(DISTDIR); \

lib: setup $(E4LIBS)

clean:
	find . -name "*.o" -exec rm -vf {} \;
	rm -rf $(DISTDIR) 
	rm -rf $(OBJDIR) 
	rm -rf $(LIB)
	rm -rf $(LIBDIR)


test: clean setup lib $(E4TESTS)

format:
	clang-format -i src/*.c src/crypto/*.c include/e4/*.h include/e4/crypto/*.h include/e4/internal/*.h


