PROG= sample
SRCS= main.c
DPADD+= ${LIBSYS} ${LIBSEF}
LDADD+= -lsys -lsef
BINDIR= /usr/sbin

.include <minix.service.mk>
