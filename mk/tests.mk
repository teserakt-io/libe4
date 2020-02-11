
E4TESTOBJS_CMN = $(TESTOBJDIR)/util.$O \
                 $(TESTOBJDIR)/crypto.$O \
	         $(TESTOBJDIR)/siv.$O 

E4TESTOBJS += $(E4TESTOBJS_CMN)

$(TESTOBJDIR)/siv.$O: test/siv.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/util.$O: test/util.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTOBJDIR)/crypto.$O: test/crypto.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTDIR)/util: $(TESTOBJDIR)/util.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/crypto: $(TESTOBJDIR)/crypto.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

$(TESTDIR)/siv_kat: $(TESTOBJDIR)/siv.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

COMMON_TESTS = \
    $(TESTDIR)/siv_kat  \
    $(TESTDIR)/util               \
    $(TESTDIR)/crypto

testexec_common:
	./$(TESTDIR)/siv_kat
	./$(TESTDIR)/util
	./$(TESTDIR)/crypto

