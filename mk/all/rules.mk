
SRCS=$(E4_COMMON_SRCS) $(E4_PUB_SRCS) $(E4_SYM_SRCS)
OBJS=$(E4_COMMON_OBJS) $(E4_PUB_OBJS) $(E4_SYM_OBJS)

all_headercpy:
	cp -rfv $(INCDIR)/* $(OUTINCDIR)/; \

all_lib: setup all_header all_headercpy $(OBJS)
	mkdir -p $(LIBDIR); \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

all_so: setup all_header all_headercpy $(OBJS)
	mkdir -p $(LIBDIR); \
	$(CC) $(LDSOFLAGS) $(OBJS) -lc -o $(LIBSO)
	ln -sf $(LIBSO_NAME) $(LIBSO_ABI)
	ln -sf $(LIBSO_NAME) $(LIBSO_CUR)

.PHONY all_header: $(BUILDDIR)/include/e4config/e4_config.h

$(BUILDDIR)/include/e4config/e4_config.h:
	echo '#define E4_MODE_ALL 1' > $@
	echo "#define E4_STORE_NONE 1" >> $@

E4LIBS += all_lib 

all_dynamic_library: all_so
