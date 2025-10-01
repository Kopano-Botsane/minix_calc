# Makefile for calc server

SRCS= main.c

CPPFLAGS += -D_SYSTEM

.include <minix.service.mk>
