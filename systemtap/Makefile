CC=gcc
CFLAGS=-O0 -g

%.o:	%.c
	$(CC) $(CFLAGS) $< -c -o  $@

all:	print_vsx.o
	$(CC) $(CFLAGS) print_vsx.o -o print_vsx
clean:
	rm -fr *.o
	rm -fr print_vsx
