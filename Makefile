LDFLAGS = -lpthread
CC = gcc -Wno-pointer-to-int-cast
DEBUG = -O0 -g
DEPS = torture.h

%.o: %.c $(DEPS)
	$(CC) $(DEBUG) -c -o $@ $< $(CFLAGS)

torture: worker.o torture.o workload.o threads.o
	$(CC) $(DEBUG) -o torture worker.o torture.o workload.o threads.o $(LDFLAGS)

asm:
	gcc -S -fverbose-asm worker.c torture.c workload.c threads.c

z: z.c
	$(CC) $(DEBUG) z.c -o z

fp_una:	fp_una.c
	gcc -S -O0 fp_una.c
	sed -i 's/main/_start/g' fp_una.s
	as -mpwr8 fp_una.s -o fp_una.o
	ld -static fp_una.o -o fp_una

altivec_una:	altivec_una.c
	gcc -S -O0 altivec_una.c
	sed -i 's/main/_start/g' altivec_una.s
	as -mpwr8 altivec_una.s -o altivec_una.o
	ld -static altivec_una.o -o altivec_una

vsx_una:	vsx_una.c
	gcc -S -O0 vsx_una.c
	sed -i 's/main/_start/g' vsx_una.s
	as -mpwr8 vsx_una.s -o vsx_una.o
	ld -static vsx_una.o -o vsx_una

vsx_una_:	vsx_una_.c
	gcc -S -O0 vsx_una_.c
	sed -i 's/main/_start/g' vsx_una_.s
	as -mpwr8 vsx_una_.s -o vsx_una_.o
	ld -static vsx_una_.o -o vsx_una_

all: torture

clean:
	rm -f *.o
	rm -f *.s
	rm -f a.out
	rm -f z
	rm -fr torture
	rm fp_una
	rm altivec_una
