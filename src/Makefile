LDFLAGS = -lpthread
CC = gcc -Wno-pointer-to-int-cast -Wall
DEBUG = -O0 -g
SRC = src
DEPS = $(SRC)/torture.h

CFLAGS += $(DEBUG)

torture: worker.o torture.o workload.o threads.o signal.o
	$(CC) $(DEBUG) -o torture worker.o torture.o workload.o threads.o signal.o $(LDFLAGS)

all: torture

clean:
	rm -f *.o
	rm -f a.out
	rm -f z
	rm -fr torture

cscope:
	find . -name "*.c" -o -name "*.h" > cscope.files
	cscope -q -R -b -i cscope.files
