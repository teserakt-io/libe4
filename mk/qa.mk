
E4_CFLAGS	 = -fsanitize=address,memory,undefined -fno-omit-frame-pointer
E4_LDFLAGS	 = -fsanitize=address,memory,undefined -fno-omit-frame-pointer

include mk/symkey/conf.mk
include mk/symkey/objects.mk
include mk/symkey/rules.mk
include mk/symkey/tests.mk

include mk/pubkey/conf.mk
include mk/pubkey/objects.mk
include mk/pubkey/rules.mk
include mk/pubkey/tests.mk

include mk/rules.mk

