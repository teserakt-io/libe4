
pub_lib: setup $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR) $(OUTINCDIR); \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

pub_so: setup $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR) $(OUTINCDIR); \
	$(CC) $(LDSOFLAGS) $(OBJS) -o $(LIBSO)

E4LIBS += pub_lib
