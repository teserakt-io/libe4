
pub_lib: setup $(OBJS)
	mkdir -p $(LIBDIR); \
        cp -rfv $(INCDIR) $(OUTINCDIR); \
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

E4LIBS += pub_lib
