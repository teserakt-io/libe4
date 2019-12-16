
include mk/first.mk

CONF ?= symkey
include mk/$(CONF)/conf.mk
include mk/$(CONF)/objects.mk
include mk/$(CONF)/tests.mk
include mk/$(CONF)/rules.mk
include mk/rules.mk
