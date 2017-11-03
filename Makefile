LDFLAGS = -lpthread
CC = gcc -Wno-pointer-to-int-cast -Wall
DEBUG = -O0 -g
PREFIX ?= usr/local
SRC = src
DESTDIR =
BINDIR = $(PREFIX)/bin
INSTALL ?= install
CFLAGS += $(DEBUG)
DOCUMENTATION = Documentation

torture: $(SRC)/worker.o $(SRC)/torture.o $(SRC)/workload.o $(SRC)/threads.o $(SRC)/signal.o
	$(CC) $(DEBUG) -o torture $(SRC)/worker.o $(SRC)/torture.o $(SRC)/workload.o $(SRC)/threads.o $(SRC)/signal.o $(LDFLAGS)

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

install:
	mkdir -p $(DESTDIR)/$(BINDIR)
	mkdir -p $(DESTDIR)/$(PREFIX)/share/man/man1
	$(INSTALL) -d $(DESTDIR)/$(BINDIR)
	$(INSTALL) -m 755 torture $(DESTDIR)/$(BINDIR)
	$(INSTALL) -m 644 $(DOCUMENTATION)/torture.1 $(DESTDIR)/$(PREFIX)/share/man/man1
	gzip -9 $(DESTDIR)/$(PREFIX)/share/man/man1/torture.1
