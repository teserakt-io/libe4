
E4_PUB_OBJS = \
       $(OBJDIR)/crypto/curve25519/curve25519-donna.$O \
       $(OBJDIR)/crypto/ed25519/add_scalar.$O \
       $(OBJDIR)/crypto/ed25519/fe.$O \
       $(OBJDIR)/crypto/ed25519/ge.$O \
       $(OBJDIR)/crypto/ed25519/sc.$O \
       $(OBJDIR)/crypto/ed25519/sign.$O \
       $(OBJDIR)/crypto/ed25519/verify.$O \
       $(OBJDIR)/crypto/sha512.$O \
       $(OBJDIR)/crypto/xed25519.$O \
       $(OBJDIR)/e4pkcclient.$O

ifeq ("$(STORE)", "file")
E4_PUB_OBJS := $(E4_PUB_OBJS) $(OBJDIR)/e4c_pk_store_file.$O 
endif

ifeq ("$(STORE)", "mem")
E4_PUB_OBJS := $(E4_PUB_OBJS) $(OBJDIR)/e4c_pk_store_mem.$O
endif

