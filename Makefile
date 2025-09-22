# /usr/src/servers/calc/Makefile
.include <bsd.own.mk>

PROG=   calc
SRCS=   calc.c

DPADD+= ${LIBSYS}
LDADD+= -lsys

.include <minix.service.mk>