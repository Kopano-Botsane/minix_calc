# Makefile for the calc server

.include <bsd.own.mk>

PROG=	calc
SRCS=	main.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

.include <bsd.prog.mk>
