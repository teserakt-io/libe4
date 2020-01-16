
E4_SYM_OBJS = \
              $(OBJDIR)/e4symclient.$O 

ifeq ("$(STORE)", "file")
E4_SYM_OBJS := $(E4_SYM_OBJS) $(OBJDIR)/e4c_store_file.$O 
endif

ifeq ("$(STORE)", "mem")
E4_SYM_OBJS := $(E4_SYM_OBJS) $(OBJDIR)/e4c_store_mem.$O
endif
