vsx_una_:	vsx_una_.c
	gcc -S -O0 vsx_una_.c
	sed -i 's/main/_start/g' vsx_una_.s
	as -mpwr8 vsx_una_.s -o vsx_una_.o
	ld -static vsx_una_.o -o vsx_una_

all:
	gcc -O0 -g worker.c -lpthread -o worker
clean:
	rm -fr a.out core worker
