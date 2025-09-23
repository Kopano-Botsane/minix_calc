.include <bsd.prog.mk>
.include <minix.server.mk>

PROG= calc
SRCS= main.c

.PATH: ${.CURDIR} ${.CURDIR}/..
