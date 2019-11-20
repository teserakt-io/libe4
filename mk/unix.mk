

CONF ?= default
include mk/conf_$(CONF).mk
include mk/objects.mk
include mk/rules.mk
