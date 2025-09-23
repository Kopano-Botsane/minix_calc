.include <bsd.own.mk>

# Makefile for the CALC Server
PROG=	calc
SRCS=	main.c add.c multiply.c

DPADD+=	${LIBSYS} ${LIBM}
LDADD+=	-lsys -lm

.include <minix.service.mk>
