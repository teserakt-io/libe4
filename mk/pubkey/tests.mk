
E4TESTOBJS = $(TESTOBJDIR)/util.$O \
             $(TESTOBJDIR)/crypto.$O \
	     $(TESTOBJDIR)/pubkey_file.$O 

$(TESTOBJDIR)/util.$O: test/util.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/crypto.$O: test/crypto.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/pubkey_file.$O: test/pubkey/pubkey_filestore_test.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/pubkey_crypto_test.$O: test/pubkey/pubkey_crypto_test.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTDIR)/util: $(TESTOBJDIR)/util.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/crypto: $(TESTOBJDIR)/crypto.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/pubkey_file: $(TESTOBJDIR)/pubkey_file.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/pubkey_crypto_test: $(TESTOBJDIR)/pubkey_crypto_test.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

PUBKEY_TESTS = \
    $(TESTDIR)/util               \
    $(TESTDIR)/crypto             \
    $(TESTDIR)/pubkey_file        \
    $(TESTDIR)/pubkey_crypto_test

E4TESTS += $(PUBKEY_TESTS)
