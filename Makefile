# Makefile for calc server

.include <bsd.own.mk>

PROG=	calc
SRCS=	calc.c table.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

.include <minix.service.mk>