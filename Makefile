# Makefile
# 2018-05-01  Markku-Juhani O. Saarinen <mjos@iki.fi>

LIB		= lib/libe4.a
OBJS    = src/e4client.o			\
		  src/crypto/aes_siv.o		\
		  src/crypto/aes256enc_ref.o		\
		  src/crypto/sha3.o			\
		  src/crypto/keccakf1600.o

DIST		= libe4

CC		    = gcc
AR          = ar
ARFLAGS     = rcs
CFLAGS		= -Wall -Werror -Ofast
LDFLAGS		= -L.
INCLUDES	= -Iinclude -Isrc/crypto -Ipaho.mqtt.c/src

default: $(LIB)

$(LIB): $(OBJS)
		mkdir lib; \
		$(AR) $(ARFLAGS) $(LIB) $(OBJS)

%.o: %.c
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
		rm -rf $(DIST)-*.t?z $(OBJS) $(LIB) *~ *.e4p
		find . -name "*.o" -exec rm -f {} \;

dist:		clean
		cd ..; \
		tar cfvz $(DIST)/$(DIST)-`date "+%Y%m%d%H%M"`.tgz \
			$(DIST)/* $(DIST)/.gitignore
