LDFLAGS = -lpthread
CC = gcc
DEBUG = -O0 -g

%.o: %.c $(DEPS)
	$(CC) $(DEBUG) -c -o $@ $< $(CFLAGS)

x: worker.o x.o workload.o threads.o
	$(CC) $(DEBUG) worker.o x.o workload.o threads.o $(LDFLAGS) 

asm:
	gcc -S -fverbose-asm worker.c x.c workload.c threads.c

z: z.c
	$(CC) $(DEBUG) z.c -o z

all: x

clean:
	rm *.o
	rm a.out
	rm z
