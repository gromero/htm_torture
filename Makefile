GCC = gcc
LDFLAGS = -lpthread
CC = gcc
DEBUG = -O0 -g
CFLAGS += $(DEBUG) -Wno-pointer-to-int-cast -Wall -lpthread -maltivec -mcpu=power8
SRC = src

OBJS = worker.o torture.o workload.o threads.o signal.o qs.o

%.o: $(SRC)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

torture: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJS)
	rm -fr torture
	rm -fr cscope*

cscope:
	find . -name "*.c" -o -name "*.h" > cscope.files
	cscope -q -R -b -i cscope.files
