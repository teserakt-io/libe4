
CC	 = gcc
AR       = ar
ARFLAGS  = rcs
CFLAGS	 = -Wall -Werror -Ofast -DE4_STORE_FILE
LDFLAGS	 = -L.
INCLUDES = -Iinclude/
DOC      = doxygen

# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")
