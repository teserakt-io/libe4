
OBJS = $(OBJDIR)/e4symclient.$O           \
       $(OBJDIR)/e4c_store_file.$O        \
       $(OBJDIR)/e4util.$O                \
       $(OBJDIR)/crypto/aes_siv.$O        \
       $(OBJDIR)/crypto/aes256enc_ref.$O  \
       $(OBJDIR)/crypto/sha3.$O           \
       $(OBJDIR)/crypto/keccakf1600.$O    \
       $(OBJDIR)/crypto/test_aes_siv.$O   \
       $(OBJDIR)/crypto/test_sha3.$O      \
       $(OBJDIR)/strlcpy.$O


OBJS += $(OBJDIR)/crypto/curve25519/curve25519-donna.$O \
        $(OBJDIR)/crypto/ed25519/add_scalar.$O \
	$(OBJDIR)/crypto/ed25519/ed25519_test.$O \
	$(OBJDIR)/crypto/ed25519/fe.$O \
	$(OBJDIR)/crypto/ed25519/ge.$O \
	$(OBJDIR)/crypto/ed25519/key_exchange.$O \
	$(OBJDIR)/crypto/ed25519/keypair.$O \
	$(OBJDIR)/crypto/ed25519/sc.$O \
	$(OBJDIR)/crypto/ed25519/seed.$O \
	$(OBJDIR)/crypto/ed25519/sign.$O \
	$(OBJDIR)/crypto/ed25519/verify.$O

TESTS = \
    $(TESTDIR)/util          \
    $(TESTDIR)/crypto        \
    $(TESTDIR)/e4file 
