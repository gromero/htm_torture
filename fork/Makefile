CFLAGS=-g -O0

%.o:	%.c
	gcc -c $(CFLAGS) $< -o $@

s:	s.o
	gcc $(CFLAGS) -o $@ $^ 

clean:
	rm -fr *.o
	rm -fr s
