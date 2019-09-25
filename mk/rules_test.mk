

build/%.$O: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

default: setup $(LIB)

setup:
	mkdir -p $(OBJDIR); \
	mkdir -p $(OBJDIR)/test/; \
	mkdir -p $(OBJDIR)/crypto; \
	mkdir -p $(LIBDIR); \
	mkdir -p $(DISTDIR); \

$(LIB): setup $(OBJS)
	mkdir -p lib; \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

clean:
	find . -name "*.o" -exec rm -vf {} \;
	rm -rf $(DISTDIR) 
	rm -rf $(OBJDIR) 
	rm -rf $(LIB)
	rm -rf $(LIBDIR)


format:
	clang-format -i src/*.c src/crypto/*.c include/e4/*.h include/e4/crypto/*.h include/e4/internal/*.h

# Generic test rule.
$(TESTDIR)/%: test/%.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LIB) 

# Configuration mode tests
build/test/e4file: test/e4.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -DE4_STORE_FILE=1 -o $@ $< $(LIB)

