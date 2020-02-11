
include mk/symkey/tests.mk
include mk/pubkey/tests.mk

tests_all: $(COMMON_TESTS) $(SYMKEY_TESTS) $(PUBKEY_TESTS)

E4TESTS = tests_all

testexec_all: testexec_common testexec_sk testexec_pk

E4TESTEXEC = testexec_all
