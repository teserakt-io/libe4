
E4TESTOBJS = $(TESTOBJDIR)/util.$O \
             $(TESTOBJDIR)/crypto.$O \
	     $(TESTOBJDIR)/pubkey_file.$O 

$(TESTOBJDIR)/util.$O: test/util.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/crypto.$O: test/crypto.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/pubkey_file.$O: test/pubkey/pubkey_filestore_test.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/pubkey_e4cmd.$O: test/pubkey/pubkey_e4cmd_test.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/pubkey_crypto_test.$O: test/pubkey/pubkey_crypto_test.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/ed25519_test.$O: test/pubkey/ed25519_test.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTDIR)/util: $(TESTOBJDIR)/util.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/crypto: $(TESTOBJDIR)/crypto.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/pubkey_file: $(TESTOBJDIR)/pubkey_file.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/pubkey_e4cmd: $(TESTOBJDIR)/pubkey_e4cmd.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/pubkey_crypto_test: $(TESTOBJDIR)/pubkey_crypto_test.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/ed25519_test: $(TESTOBJDIR)/ed25519_test.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

PUBKEY_TESTS = \
    $(TESTDIR)/util               \
    $(TESTDIR)/crypto             \
    $(TESTDIR)/pubkey_file        \
    $(TESTDIR)/pubkey_e4cmd       \
    $(TESTDIR)/pubkey_crypto_test \
    $(TESTDIR)/ed25519_test

E4TESTS += $(PUBKEY_TESTS)

testexec_pk:
	./$(TESTDIR)/util
	./$(TESTDIR)/crypto
	./$(TESTDIR)/pubkey_file
	./$(TESTDIR)/pubkey_e4cmd
	./$(TESTDIR)/pubkey_crypto_test
	./$(TESTDIR)/ed25519_test

E4TESTEXEC += testexec_pk

