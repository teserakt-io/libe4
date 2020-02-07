


$(OBJDIR)/%.$O: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY:setup

setup: 
	mkdir -p $(OBJDIR); \
	rm -rf $(OUTINCDIR); \
	mkdir -p $(OUTINCDIR); \
	mkdir -p $(OUTINCDIR)/e4config; \
	mkdir -p $(TESTOBJDIR); \
	mkdir -p $(OBJDIR)/crypto; \
	mkdir -p $(OBJDIR)/crypto/curve25519; \
	mkdir -p $(OBJDIR)/crypto/ed25519; \
	mkdir -p $(OBJDIR)/crypto/ctaes; \
	mkdir -p $(TESTDIR); \
	mkdir -p $(LIBDIR);

lib: setup $(E4LIBS)

clean:
	find . -name "*.o" -exec rm -vf {} \;
	rm -rf $(DISTDIR) 
	rm -rf $(OBJDIR) 
	rm -rf $(LIB)
	rm -rf $(LIBDIR)
	rm -rf build

testbuild: clean setup lib $(E4TESTS) postbuild_config_echo

test: testbuild $(E4TESTEXEC) postbuild_config_echo

format:
	clang-format -i src/*.c src/crypto/*.c include/e4/*.h include/e4/crypto/*.h include/e4/internal/*.h

tidy: setup $(BUILDDIR)/include/e4config/e4_config.h
	clang-tidy -checks="-*,clang-analyzer-core.*" $(SRCS) -- $(CFLAGS) $(INCLUDES) $(CONFIG_DEFINES)


.PHONY install: default
	@echo "Installing LibE4 to your system."
	@echo ""
ifneq ("$(CONF)", "all")
	$(error Cannot install except when building all)
endif
ifneq ("$(PREFIX)", "")
	cp -rfv $(BUILDDIR)/include $(PREFIX)/include
	cp -rfv $(BUILDDIR)/lib $(PREFIX)/lib
else
	$(error Please specify a PREFIX variable)
endif
	@echo "Installed to $(PREFIX)."


