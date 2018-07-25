# $FreeBSD$
# FreeBSD Makefile 

PROG = torture
CFLAGS += -Wno-pointer-to-int-cast -Wall -maltivec -mcpu=power8
LDFLAGS += -lpthread
MAN =
SRCS = torture.c signal.c threads.c worker.c workload.c qs.c

CC = gcc6

.include <bsd.prog.mk>
