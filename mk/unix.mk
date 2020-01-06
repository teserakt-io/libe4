
include mk/first.mk
include mk/conf.mk

CONF ?= symkey
STORE ?= file
CSTD ?= -std=c89

include mk/$(CONF)/conf.mk
include mk/$(CONF)/objects.mk
include mk/$(CONF)/tests.mk
include mk/$(CONF)/rules.mk
include mk/rules.mk
