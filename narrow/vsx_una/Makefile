CFLAGS=-O0 -g -pthread
all:	tm-vsx-unavail.o tm-vsx-unavail-tiny.o
	gcc $(CFLAGS) tm-vsx-unavail.o -o tm-vsx-unavail
	gcc $(CFLAGS) tm-vsx-unavail-tiny.o -o tm-vsx-unavail-tiny

clean:
	rm -fr *.o
	rm -fr core
	rm -fr tm-vsx-unavail
	rm -fr tm-vsx-unavail-tiny
