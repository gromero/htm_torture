LDFLAGS = -lpthread
CC = gcc
DEBUG = -g

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

x: worker.o x.o workload.o threads.o
	$(CC) $(DEBUG) worker.o x.o workload.o threads.o $(LDFLAGS) 

clean:
	rm *.o
	rm a.out
