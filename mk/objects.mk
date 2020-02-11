
E4_COMMON_SRCS = $(SRCDIR)/e4util.c         \
                 $(SRCDIR)/crypto/aes_siv.c        \
                 $(SRCDIR)/crypto/aes256enc_ref.c  \
                 $(SRCDIR)/crypto/sha3.c           \
                 $(SRCDIR)/crypto/keccakf1600.c    \
                 $(SRCDIR)/crypto/test_aes_siv.c   \
                 $(SRCDIR)/crypto/test_sha3.c      \
                 $(SRCDIR)/strlcpy.c

E4_COMMON_OBJS = $(OBJDIR)/e4util.$O         \
                 $(OBJDIR)/crypto/aes_siv.$O        \
                 $(OBJDIR)/crypto/aes256enc_ref.$O  \
                 $(OBJDIR)/crypto/sha3.$O           \
                 $(OBJDIR)/crypto/keccakf1600.$O    \
                 $(OBJDIR)/crypto/test_aes_siv.$O   \
                 $(OBJDIR)/crypto/test_sha3.$O      \
                 $(OBJDIR)/strlcpy.$O
