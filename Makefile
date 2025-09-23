# Makefile for Calculator Server (calc)
.include <bsd.own.mk>

PROG=	calc
SRCS=	main.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

.include <minix.service.mk>
