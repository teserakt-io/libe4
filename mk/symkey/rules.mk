
SRCS=$(E4_COMMON_SRCS) $(E4_SYM_SRCS)
OBJS=$(E4_COMMON_OBJS) $(E4_SYM_OBJS)

sym_lib: setup sym_header $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR)/* $(OUTINCDIR)/; \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

sym_so: setup sym_header $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR)/* $(OUTINCDIR)/; \
	$(CC) $(LDSOFLAGS) $(OBJS) -o $(LIBSO)

sym_header: $(BUILDDIR)/include/e4config/e4_config.h

$(BUILDDIR)/include/e4config/e4_config.h:
	echo '#define E4_MODE_SYMKEY 1' > $@
ifeq ("$(STORE)", "none")
	echo "#define E4_STORE_NONE 1" >> $@
endif
ifeq ("$(STORE)", "mem")
	echo "#define E4_STORE_MEM 1" >> $@
endif
ifeq ("$(STORE)", "file")
	echo "#define E4_STORE_FILE 1" >> $@
endif

E4LIBS += sym_lib
