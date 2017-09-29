CFLAGS=-O0 -g -pthread

%.o:	%.c
	gcc $(CFLAGS) -c $< -o $@

all:	tm-vsx-unavail.o tm-vsx-unavail-tiny.o tm-unavailable.o
	gcc $(CFLAGS) tm-vsx-unavail.o -o tm-vsx-unavail
	gcc $(CFLAGS) tm-vsx-unavail-tiny.o -o tm-vsx-unavail-tiny
	gcc $(CFLAGS) tm-unavailable.o -o tm-unavailable

clean:
	rm -fr *.o
	rm -fr core
	rm -fr tm-vsx-unavail
	rm -fr tm-vsx-unavail-tiny
	rm -fr tm-unavailable
