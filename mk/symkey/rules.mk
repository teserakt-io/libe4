
sym_lib: $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR) $(OUTINCDIR); \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

sym_so: setup $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR) $(OUTINCDIR); \
	$(CC) $(LDSOFLAGS) $(OBJS) -o $(LIBSO)

E4LIBS += sym_lib
