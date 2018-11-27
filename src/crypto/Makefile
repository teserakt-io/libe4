# Makefile
# 2018-07-01  Markku-Juhani O. Saarinen <markku@teserakt.io>

DIST		= crypto
BIN		= xtest
OBJS		= main.o	test_sha3.o	test_aes_siv.o	\
		sha3.o		keccakf1600.o			\
		aes_siv.o	aes256enc_ref.o


CC		= gcc
CFLAGS		= -Wall -Ofast -march=native
LIBS		=
LDFLAGS		=
INCLUDES	=

$(BIN):	$(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(DIST)-*.t?z $(OBJS) $(BIN) *~
