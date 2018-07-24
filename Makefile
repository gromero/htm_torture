GCC = gcc
LDFLAGS = -lpthread
CC = $(GCC) -Wno-pointer-to-int-cast -Wall -maltivec -mcpu=power8
DEBUG = -O0 -g
SRC = src

CFLAGS += $(DEBUG)

torture: $(SRC)/worker.o $(SRC)/torture.o $(SRC)/workload.o $(SRC)/threads.o $(SRC)/signal.o $(SRC)/qs.o
	$(CC) $(DEBUG) -o torture $(SRC)/worker.o $(SRC)/torture.o $(SRC)/workload.o $(SRC)/threads.o $(SRC)/signal.o $(SRC)/qs.o $(LDFLAGS)

all: torture

clean:
	rm -f $(SRC)/*.o
	rm -f a.out
	rm -f z
	rm -fr torture
	rm -fr cscope*

cscope:
	find . -name "*.c" -o -name "*.h" > cscope.files
	cscope -q -R -b -i cscope.files
