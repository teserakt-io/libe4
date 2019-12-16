
#E4TESTOBJS = $(TESTOBJDIR)/util.$O \
#             $(TESTOBJDIR)/crypto.$O \
#	     $(TESTOBJDIR)/symkey_file.$O 

$(TESTOBJDIR)/util.$O: test/util.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/crypto.$O: test/crypto.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/symkey_file.$O: test/symkey/symkey_file.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTDIR)/util: $(TESTOBJDIR)/util.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/crypto: $(TESTOBJDIR)/crypto.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/symkey_file: $(TESTOBJDIR)/symkey_file.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

SYMKEY_TESTS = \
    $(TESTDIR)/util               \
    $(TESTDIR)/crypto             \
    $(TESTDIR)/symkey_file 

E4TESTS += $(SYMKEY_TESTS)
