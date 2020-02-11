
E4TESTOBJS_SK= $(TESTOBJDIR)/symkey_file.$O 

E4TESTOBJS += $(E4TESTOBJS_SK)

$(TESTOBJDIR)/symkey_file.$O: test/symkey/symkey_file.c
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

$(TESTDIR)/symkey_file: $(TESTOBJDIR)/symkey_file.$O
	$(CC) $(TESTLDFLAGS) $< $(LIB) -o $@

SYMKEY_TESTS = \
    $(TESTDIR)/symkey_file 

E4TESTS = $(COMMON_TESTS) $(SYMKEY_TESTS)

testexec_sk:
	./$(TESTDIR)/symkey_file

E4TESTEXEC = testexec_common testexec_sk

