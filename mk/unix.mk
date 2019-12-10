

CONF ?= symkey
include mk/$(CONF)/conf.mk
include mk/$(CONF)/objects.mk
include mk/$(CONF)/tests.mk
include mk/rules.mk
