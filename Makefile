# test_sample/Makefile
PROG=   test_sample
SRCS=   test_sample.c

DPADD+= ${LIBSYS}
LDADD+= -lsys

.include <minix.prog.mk>
